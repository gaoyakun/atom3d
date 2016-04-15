#include "StdAfx.h"
#include <signal.h>
#include <tchar.h>
#include <dbghlp/rtc.h>

#include "atom3d_studio.h"
#include "app.h"
#include "editor.h"
#include "axisnode.h"
#include "trackballnode.h"
#include "editor_impl.h"
#include "plugin.h"
#include "rt_trigger.h"
#include "gridnode.h"
#include "lightgeom.h"
#include "resource.h"
#include "misc/helper.h"

#include "scene_plugin/plugin_scene.h"
#include "model_plugin/plugin_model.h"
#include "tree_plugin/tree.h"
#include "tree_plugin/plugin_tree.h"
#include "gui_plugin/plugin_gui.h"
#include "actor_plugin/plugin_actor.h"
#include "composition_plugin/plugin_composition.h"
#include "ribbontrail_plugin/ribbontrail_plugin.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define AS_EDITOR_REALTIMECTRL_ID 100

static ATOM_ScriptManager scriptMngr;

ATOM_BEGIN_EVENT_MAP(StudioApp, ATOM_Application)
	ATOM_EVENT_HANDLER(StudioApp, ATOM_AppIdleEvent, onIdle)
	ATOM_EVENT_HANDLER(StudioApp, ATOM_AppInitEvent, onInit)
	ATOM_EVENT_HANDLER(StudioApp, ATOM_AppExitEvent, onExit)
	ATOM_EVENT_HANDLER(StudioApp, ATOM_AppWMEvent, onWMEvent)
	ATOM_EVENT_HANDLER(StudioApp, ATOM_MouseMoveEvent, onMouseMove)
	ATOM_EVENT_HANDLER(StudioApp, ATOM_CharEvent, onChar)
	ATOM_EVENT_HANDLER(StudioApp, ATOM_KeyDownEvent, onKeyDown)
	ATOM_EVENT_HANDLER(StudioApp, ATOM_KeyUpEvent, onKeyUp)
	ATOM_EVENT_HANDLER(StudioApp, ATOM_WindowCloseEvent, onWindowClose)
	ATOM_EVENT_HANDLER(StudioApp, ATOM_WindowResizeEvent, onWindowResize)
	ATOM_EVENT_HANDLER(StudioApp, ATOM_DeviceLostEvent, onDeviceLost)
	ATOM_EVENT_HANDLER(StudioApp, ATOM_DeviceResetEvent, onDeviceReset)
	ATOM_EVENT_HANDLER(StudioApp, ATOMX_TWValueChangedEvent, onTWValueChanged)
	ATOM_EVENT_HANDLER(StudioApp, ATOMX_TWCommandEvent, onTWCommand)
	ATOM_EVENT_HANDLER(StudioApp, ATOMX_TWAddVariableEvent, onTWAddVariable)
	ATOM_UNHANDLED_EVENT_HANDLER(StudioApp, onUnhandled)
ATOM_END_EVENT_MAP

StudioApp::StudioApp (void)
{
	ATOM_ErrorSender::setFtpSite("127.0.0.1");
	_realtimeCtrl = 0;
	_editorImpl = 0;
	_realtimeTrigger = 0;
	_pauseRender = false;
	_eatKeyEvent = false;
	_eatCharEvent = false;
	_numDrawCalls = 0;
	_device = 0;
	_numPrimitivesDrawn = 0;
	_firstCreated = true;
	_scenePlugin = ATOM_NEW(PluginScene);
	_modelPlugin = ATOM_NEW(PluginModel);
#if defined(SUPPORT_BILLBOARD_TREE)
	_treePlugin = ATOM_NEW(PluginTree);
#endif
	_guiPlugin = ATOM_NEW(PluginGUI);
	_actorPlugin = ATOM_NEW(PluginActor);
	_compositionPlugin = ATOM_NEW(PluginComposition);
	_ribbontrailPlugin = ATOM_NEW(PluginRibbonTrail);
}

StudioApp::~StudioApp (void)
{
}

bool StudioApp::chooseAndSetProject (void)
{
	char buffer[MAX_PATH] = { '\0' };
	if (ChooseDir (ATOM_APP->getMainWindow(), "选择项目路径", buffer))
	{
		if (!IsProjectDirectory (buffer))
		{
			if (::MessageBoxA (ATOM_APP->getMainWindow(), "所选目录不是项目目录，要在该目录下创建项目吗？", "ATOM3D编辑器", MB_YESNO|MB_ICONQUESTION) == IDYES)
			{
				MakeProjectDirectory (buffer, false);
				if (!IsProjectDirectory (buffer))
				{
					::MessageBoxA (ATOM_APP->getMainWindow(), "创建项目失败.", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
					return false;
				}
			}
		}

		if (IsProjectDirectory (buffer))
		{
			if (!((StudioApp*)ATOM_APP)->setProjectPath (buffer))
			{
				::MessageBoxA (ATOM_APP->getMainWindow(), "切换项目失败.", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
			}
			else
			{
				return true;
			}
		}
	}

	return false;
}

bool StudioApp::setProjectPath (const char *path)
{
	if (!IsProjectDirectory (path))
	{
		return false;
	}

	if (_firstCreated)
	{
		if (!ATOM_KernelConfig::initializeWithPath (path))
		{
			char err[512];
			sprintf (err, "无法设置项目<%s>.", path);
			::MessageBoxA (getMainWindow(), err, "ATOM3D编辑器", MB_OK|MB_ICONHAND);
			return false;
		}
		else
		{
			Config::getInstance()->setLastProject (path);
		}
	}
	else
	{
		if (IDYES == ::MessageBoxA (getMainWindow(), "切换项目需要重新启动程序，是否继续？", "ATOM3D编辑器", MB_YESNO|MB_ICONQUESTION))
		{
			Config::getInstance()->setLastProject (path);

			if (_editorImpl->exit ())
			{
				Config::getInstance()->save ();

				char moduleFileName[MAX_PATH];
				char cwd[MAX_PATH];
				::GetCurrentDirectoryA (MAX_PATH, cwd);
				::GetModuleFileNameA (NULL, moduleFileName, MAX_PATH);
				::ShellExecuteA (::GetDesktopWindow(), "OPEN", moduleFileName, NULL, NULL, SW_SHOWDEFAULT);
				::ShowWindow(getMainWindow(), SW_MINIMIZE);
				postQuitEvent (0);
			}
		}
	}

	return true;
}

INT_PTR CALLBACK UpdatePromptDlgProc (HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) 
{
	switch(message) {
	case WM_INITDIALOG:
		{
			::SetFocus (::GetDlgItem (hWnd, IDOK));
			return TRUE;
		}

	case WM_COMMAND:
		{
			if ((HWND)lParam == ::GetDlgItem (hWnd, IDOK))
			{
				::EndDialog (hWnd, 0);
			}
			break;
		}

	case WM_NOTIFY:
		{
			LPNMHDR pnmh = (LPNMHDR)lParam;
			if (pnmh->hwndFrom == ::GetDlgItem(hWnd, IDC_UPDATESERVER))
			{
				if (pnmh->code == NM_CLICK)
				{
					::ShellExecuteA(::GetDesktopWindow(), "open", Config::getInstance()->getUpdateServer(), NULL, NULL, SW_SHOWNORMAL);
				}
			}
			else if (pnmh->hwndFrom == ::GetDlgItem(hWnd, IDC_CHANGES))
			{
				if (pnmh->code == NM_CLICK)
				{
					ATOM_STRING changelog = Config::getInstance()->getUpdateServer();
					if (changelog.back() != '/')
						changelog += '/';
					changelog += "changes.html";
					::ShellExecuteA(::GetDesktopWindow(), "open", changelog.c_str(), NULL, NULL, SW_SHOWNORMAL);
				}
			}
			break;
		}

	case WM_CLOSE:
		{
			EndDialog(hWnd, 0);
			return TRUE;
		}
	}

	return FALSE;
}

void StudioApp::initVFS (int argc, char **argv)
{
	for (int i = 0; i < argc; ++i)
	{
		if (!strcmp(argv[i], "--vfsroot") && i < argc-1)
		{
			const char *vfsroot = argv[i+1];
			DWORD attributes = ::GetFileAttributesA(vfsroot);
			if ( attributes == 0xFFFFFFFF)
				break;

			if (!ATOM_KernelConfig::initializeWithPath (vfsroot))
				break;

			return;
		}
	}

	bool projectSet = false;

	if (!_currentProject.empty ())
	{
		if (!setProjectPath( _currentProject.c_str()))
		{
			_currentProject = "";
		}
		else
		{
			projectSet = true;
		}

		if (projectSet && !_currentDocument.empty())
		{
			char vfilename[ATOM_VFS::max_filename_length];
			if (ATOM_GetVFSPathName (_currentDocument.c_str(), vfilename))
			{
				_editorImpl->openDocument (vfilename, NULL);
			}
			_currentDocument = "";
		}
	}
	else
	{
		ATOM_STRING lastProject = Config::getInstance()->getLastProject ();
		if (!lastProject.empty ())
		{
			setProjectPath (lastProject.c_str());
		}
		else
		{
			chooseAndSetProject ();
		}
	}
}

void StudioApp::onIdle (ATOM_AppIdleEvent *event)
{
	ATOM_STACK_TRACE(StudioApp::onIdle);

	//--- wangjian added ---//
	// 异步加载相关
	ATOM_AsyncLoader::Update();
	//----------------------//

	if (!_pauseRender)
	{
		_editorImpl->frameUpdate ();
		_guiRenderer.handleEvent (event);

		render ();
	}
	else
	{
		::Sleep (10);
	}

	//ATOM_ContentStream::processRequests ();
	ATOM_Coroutine::dispatch ();
	
}

void StudioApp::onInit (ATOM_AppInitEvent *event)
{
	//----------------------------------------------------------------------------------------------//
	// wangjian added
	ATOM_RenderSettings::enableEditorMode (true);					// 开启编辑器模式
	ATOM_RenderSettings::enableShaderBinaryCompile(false);			// 开启二进制shader文件编译生成
	//----------------------------------------------------------------------------------------------//

	ATOM_Win32DebugStream::startMonitering ();

	ATOM_LOGGER::information ("Initializing application..\n");

	callParentHandler (event);
    if (!event->success)
    {
		return;
    }

	GetDocumentForOpen (event->argc, event->argv, _currentProject, _currentDocument);

	registerPluginsImage ();
	registerPluginsRender ();
	registerPluginsEngine ();

	ATOM_RegisterType (GridNode::_classname(), &GridNode::_create, &GridNode::_destroy, &GridNode::_purge, GridNode::_get_script_interface(), false);
	//ATOM_RegisterType (ShapeNode::_classname(), &ShapeNode::_create, &ShapeNode::_destroy, &ShapeNode::_purge, ShapeNode::_get_script_interface(), false);
	ATOM_RegisterType (LightGeode::_classname(), &LightGeode::_create, &LightGeode::_destroy, &LightGeode::_purge, LightGeode::_get_script_interface(), false);

#if defined(SUPPORT_BILLBOARD_TREE)
	ATOM_RegisterType (TreeNode::_classname(), &TreeNode::_create, &TreeNode::_destroy, &TreeNode::_purge, TreeNode::_get_script_interface(), false);
#endif

	ATOMX_RegisterObjects ();

	_editorImpl = ATOM_NEW(EditorImpl);

	if (!initGraphics ())
	{
		ATOM_LOGGER::error ("Initializing graphices failed!\n");
		event->success = false;
		return;
	}

	initVFS (event->argc, event->argv);

	ATOM_RenderSettings::enableAutoIMEChange (false);

	if (!initFont ())
	{
		ATOM_LOGGER::error ("Initializing font failed!\n");
		event->success = false;
		ATOMX_TweakBar::terminate ();
		ATOM_DoneGfx ();
		return;
	}

	if (!initMainUI ())
	{
		ATOM_LOGGER::error ("Initializing GUI failed!\n");
		event->success = false;
		ATOMX_TweakBar::terminate ();
		ATOM_DoneGfx ();
		return;
	}

	_editorImpl->init (_renderWindow.get(), &_guiRenderer, _realtimeCtrl);
	_editorImpl->registerAccelKeyCommand (KEY_o, KEYMOD_CTRL, MENUID_OPEN);
	_editorImpl->registerAccelKeyCommand (KEY_s, KEYMOD_CTRL, MENUID_SAVE);
	_editorImpl->registerAccelKeyCommand (KEY_z, KEYMOD_CTRL, MENUID_UNDO);
	_editorImpl->registerAccelKeyCommand (KEY_z, KEYMOD_CTRL|KEYMOD_SHIFT, MENUID_REDO);
	_editorImpl->registerAccelKeyCommand (KEY_c, KEYMOD_CTRL, MENUID_COPY);
	_editorImpl->registerAccelKeyCommand (KEY_x, KEYMOD_CTRL, MENUID_CUT);
	_editorImpl->registerAccelKeyCommand (KEY_v, KEYMOD_CTRL, MENUID_PASTE);
	_editorImpl->registerAccelKeyCommand (KEY_w, KEYMOD_CTRL, MENUID_TOGGLE_WIREFRAME);

	_realtimeTrigger = ATOM_NEW(RealtimeEventTrigger, _editorImpl);
	_realtimeCtrl->setEventTrigger (_realtimeTrigger);

	loadPlugins ();

	_editorImpl->initAssets ();

	const ATOM_VECTOR<EditorImpl::PluginInfo> &pluginInfos = _editorImpl->getPluginList ();
	ATOM_MenuBar *menubar = _realtimeCtrl->getMenuBar ();
	const ATOM_MenuItem &fileMenu = menubar->getMenuItem (AS_MENUITEM_FILE);
	ATOM_PopupMenu *newFileMenu = fileMenu.submenu->createSubMenu (0);
	for (unsigned i = 0; i < pluginInfos.size(); ++i)
	{
		newFileMenu->appendMenuItem (pluginInfos[i].desc.c_str(), MENUID_NEW_SUBID + i);
	}

	//ATOM_VECTOR<std::pair<const char*, AS_Plugin*> > fileTypeList;
	//_editorImpl->getRegisteredFileTypeList (fileTypeList);
	//for (unsigned i = 0; i < fileTypeList.size(); ++i)
	//{
	//}

	::DragAcceptFiles (ATOM_GetRenderDevice()->getWindow()->getWindowInfo()->handle, TRUE);

	HRSRC hTopMostEnvMap = ::FindResource (NULL, MAKEINTRESOURCE(IDR_TOPMOSTENVMAP), _T("TEXTURE"));
	if (hTopMostEnvMap)
	{
		DWORD dwSize = ::SizeofResource (NULL, hTopMostEnvMap);
		if (dwSize)
		{
			HGLOBAL hGlobal = ::LoadResource (NULL, hTopMostEnvMap);
			if (hGlobal)
			{
				LPVOID data = ::LockResource (hGlobal);
				ATOM_AUTOREF(ATOM_Texture) texture = ATOM_GetRenderDevice()->allocTexture (0, ATOM_Texture::TEXTURE2D);
				if (texture->loadTexImageFromFileInMemory (data, dwSize, 0, 0, ATOM_PIXEL_FORMAT_UNKNOWN))
				{
					ATOM_RenderSettings::setTopMostEnvMap (texture.get());
				}
				::FreeResource (hGlobal);
			}
		}
	}

	//ATOM_AUTOREF(ATOM_Texture) topmostEnvMap = ATOM_CreateTextureResource ("/x.dds");
	//ATOM_RenderSettings::setTopMostEnvMap (topmostEnvMap.get());

	ATOM_Terrain::enableEditorMode (true);

	::SetWindowText (ATOM_APP->getMainWindow(), _T("ATOM3D编辑器"));

	//--- wangjian added ---//
	// 异步加载相关
	ATOM_VECTOR<ATOM_STRING> args;
	ATOM_GetCommandLine (args);
	for (unsigned i = 0; i < args.size(); ++i)
	{
		if (args[i] == "--enable-mtgc")
		{
			ATOM_AsyncLoader::enableProcedureGC(true);
		}

		if (args[i] == "--enable-mt")
		{
			ATOM_AsyncLoader::ReadGlboalPreLoadTable("/configure/preloads/preloads.xml");

			// 使用默认参数
			if( !ATOM_AsyncLoader::IsRun() )
			{
				ATOM_AsyncLoader::AsyncLoaderRunParams runParams(	1,
					2,
					4,
					2,
					4,
					4,
					3000	);
				ATOM_AsyncLoader::Run(runParams);
			}
		}

		if (args[i] == "--enable-mc")
		{
			ATOM_RenderSettings::enableModelCompressedFormat(true);
		}
	}
	//----------------------//
}

void StudioApp::onExit (ATOM_AppExitEvent *event)
{
	ATOM_LOGGER::information ("Exiting application..\n");
	_realtimeCtrl->setEventTrigger (0);

	//--- wangjian added ---//
	// 异步加载相关 退出时 销毁所有对象 停止运行
	ATOM_AsyncLoader::AbandonAllTask(true);
	//----------------------//

	ATOM_DELETE(_editorImpl);
	_editorImpl = 0;

	ATOMX_TweakBar::terminate ();
    ATOM_DoneGfx ();

	ATOM_Win32DebugStream::stopMonitering ();
	ATOM_Coroutine::done ();
}

void StudioApp::onDeviceLost (ATOM_DeviceLostEvent *event)
{
	ATOMX_TweakBar::handleDeviceLost ();
}

void StudioApp::onDeviceReset (ATOM_DeviceResetEvent *event)
{
	unsigned w = ATOM_GetRenderDevice()->getCurrentView()->getWindowWidth ();
	unsigned h = ATOM_GetRenderDevice()->getCurrentView()->getWindowHeight ();
	ATOMX_TweakBar::handleDeviceReset (w, h);

	_editorImpl->handleEvent (event);
}

void StudioApp::onChar (ATOM_CharEvent *event)
{
	if (!_eatCharEvent)
	{
		_guiRenderer.handleEvent (event);
	}
	_eatCharEvent = false;
}

void StudioApp::onWMEvent (ATOM_AppWMEvent *event)
{
	if (event->msg == WM_LBUTTONDOWN)
	{
		event->msg = event->msg;
	}
	if(event->msg == WM_SYSCOMMAND && event->wParam == SC_KEYMENU)
	{
		event->eat = true;
		event->returnValue = 0;
	}
	else if (event->msg == WMU_RENDERWINDOW_DEVICE_ATTACHED)
	{
		_device = (ATOM_RenderDevice*)event->wParam;
	}
	else if (event->msg == WM_ENTERSIZEMOVE)
	{
		_pauseRender = true;
	}
	else if (event->msg == WM_EXITSIZEMOVE)
	{
		_pauseRender = false;
	}
	else if (event->msg == WM_SIZE)
	{
		if (event->wParam == SIZE_MINIMIZED)
		{
			_pauseRender = true;
		}
		else
		{
			_pauseRender = false;
		}
	}
	else if (event->msg == WM_ERASEBKGND)
	{
		event->eat = true;
		event->returnValue = 1;
	}
	else if (event->msg == WM_PAINT)
	{
		if (!_firstCreated)
		{
			render ();
		}
	}
	else if (event->msg == WM_GETMINMAXINFO)
	{
		( ( MINMAXINFO* )event->lParam )->ptMinTrackSize.x = _editorImpl->getMinWindowWidth ();
        ( ( MINMAXINFO* )event->lParam )->ptMinTrackSize.y = _editorImpl->getMinWindowHeight ();
	}
	else if (_guiRenderer.getCapture () == 0 && !_guiRenderer.getCurrentModalDialog())
	{
		if (event->msg == WM_KEYDOWN)
		{
			volatile int n = 0;
			if (n)
			{
				n = 1;
			}
		}

		if (ATOMX_TweakBar::processWMEvent (event))
		{
			if ((event->msg == WM_CHAR) && event->eat)
			{
				_eatCharEvent = true;
				_eatKeyEvent = true;
			}
			else if ((event->msg == WM_KEYDOWN || event->msg == WM_SYSKEYDOWN) && event->eat)
			{
				_eatCharEvent = true;
				_eatKeyEvent = true;
			}
		}
	}

	_guiRenderer.handleEvent (event);
}

void StudioApp::render (void)
{
	if (ATOM_GetRenderDevice()->beginFrame ())
	{
		_guiRenderer.render();

		ATOMX_TweakBar::draw ();

		ATOM_GetRenderDevice()->endFrame ();
		ATOM_GetRenderDevice()->present ();

		_numDrawCalls = ATOM_GetRenderDevice()->getNumDrawCalls ();
		_numPrimitivesDrawn = ATOM_GetRenderDevice()->getNumPrimitivesDrawn ();
	}
}

void StudioApp::onTWCommand (ATOMX_TWCommandEvent *event)
{
	_editorImpl->handleEvent (event);
}

void StudioApp::onTWValueChanged (ATOMX_TWValueChangedEvent *event)
{
	_editorImpl->handleEvent (event);
}

void StudioApp::onTWAddVariable (ATOMX_TWAddVariableEvent *event)
{
	_editorImpl->handleEvent (event);
}

void StudioApp::onKeyDown (ATOM_KeyDownEvent *event)
{
	if (!_eatKeyEvent)
	{
		_guiRenderer.handleEvent (event);
	}
}

void StudioApp::onKeyUp (ATOM_KeyUpEvent *event)
{
	if (!_eatKeyEvent)
	{
		ATOM_Widget *widget = _guiRenderer.getFocus ();
		bool noAccel = false;
		if (widget && widget->isEnabled())
		{
			ATOM_WidgetType wt = widget->getType();
			noAccel = (wt == WT_EDIT) || (wt == WT_MULTIEDIT);
		}
		if (noAccel || !_editorImpl->checkAndFireAccelKeyCommand (event->key, event->keymod))
		{
			_guiRenderer.handleEvent (event);
		}
	}
	_eatKeyEvent = false;
}

void StudioApp::onUnhandled (ATOM_Event *event)
{
	_guiRenderer.handleEvent (event);
}

void StudioApp::onMouseMove (ATOM_MouseMoveEvent *event)
{
	_editorImpl->setMousePosition (event->x, event->y);
	_guiRenderer.handleEvent (event);
}

void StudioApp::onWindowClose (ATOM_WindowCloseEvent *event)
{
	if (!_guiRenderer.isModalLooping ())
	{
		_editorImpl->doExit ();
	}
}

void StudioApp::onWindowResize (ATOM_WindowResizeEvent *event)
{
	unsigned w = ATOM_GetRenderDevice()->getCurrentView()->getWindowWidth ();
	unsigned h = ATOM_GetRenderDevice()->getCurrentView()->getWindowHeight ();
	ATOM_GetRenderDevice()->setViewport (0, 0, 0, w, h);
	_guiRenderer.setViewport (ATOM_Rect2Di(0, 0, w, h));
}

bool StudioApp::initGraphics (void)
{
	unsigned n = AS_VERSION;
	char title[1024];
	sprintf (title, "ATOM3D Studio v%d.%d.%d", ATOM3D_VERSION_GET_MAJOR(AS_VERSION), ATOM3D_VERSION_GET_MINOR(AS_VERSION), ATOM3D_VERSION_GET_PATCH(AS_VERSION));

	ATOM_RenderWindowDesc desc;
	desc.width = 1024;
	desc.height = 768;
	desc.naked = false;
	desc.fullscreen = false;
	desc.iconid = NULL;//(HICON)::LoadIconA(NULL, MAKEINTRESOURCE(IDI_ICON));
	desc.resizable = true;
	desc.multisample = 0;
	desc.title = title;
	desc.windowid = 0;
	desc.parentWindowId = 0;

	_renderWindow = ATOM_InitializeGfx (desc, ATOM_RenderWindow::SHOWMODE_SHOWMAXIZED, true, NULL);
	if (!_renderWindow)
	{
		return false;
	}

	_device = ATOM_GetRenderDevice();
	if (!_device)
	{
		return false;
	}

	_device->setClearColor (_renderWindow.get(), 0.3f, 0.3f, 0.3f, 1.f);

	RECT rc;
	::GetClientRect (getMainWindow(), &rc);
	if (!ATOMX_TweakBar::initialize (_device, rc.right - rc.left, rc.bottom - rc.top))
	{
		::MessageBoxA (getMainWindow(), "AntTweakBar init failed", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
		return false;
	}
	ATOMX_TweakBar::setBarVisible ("TW_HELP", false);
	
	
	return true;
}

bool StudioApp::initFont (void)
{
	ATOM_STRING strDir = Config::getInstance()->getLastProject ();
	strDir+="/configure/font.xml";
	setDefaultFont(strDir.c_str());
	_font = ATOM_GUIFont::getDefaultFont( 16, 0);

	return _font != ATOM_GUIFont::invalid_handle;
}

bool StudioApp::initMainUI (void)
{
	//--- wangjian added ---//
	_guiRenderer.enableMultiThreading(false);
	//----------------------//

	_guiRenderer.getDesktop()->setClearBackground (false);

	ATOM_Rect2Di viewport;
	viewport.point.x = 0;
	viewport.point.y = 0;
	viewport.size.w = _renderWindow->getWindowWidth();
	viewport.size.h = _renderWindow->getWindowHeight();
	_guiRenderer.setViewport (viewport);

	_realtimeCtrl = ATOM_NEW(ATOM_RealtimeCtrl, _guiRenderer.getDesktop(), _guiRenderer.getDesktop()->getClientRect(), ATOM_Widget::Control|ATOM_Widget::ClipChildren|ATOM_Widget::DynamicContent, AS_EDITOR_REALTIMECTRL_ID);
	_realtimeCtrl->resize (ATOM_Rect2Di(0, 0, -100, -100));

	ATOM_MenuBar *menubar = ATOM_NEW (ATOM_MenuBar, &_guiRenderer);
	menubar->appendMenuItem ("文件", 0);
	menubar->appendMenuItem ("编辑", 0);
	menubar->appendMenuItem ("查看", 0);
	menubar->appendMenuItem ("渲染", 0);
	menubar->appendMenuItem ("工具", 0);
	menubar->appendMenuItem ("帮助", 0);

	ATOM_PopupMenu *fileMenu = menubar->createSubMenu (0);
	fileMenu->appendMenuItem ("新建", 0); 
	fileMenu->appendMenuItem ("打开(Ctrl+O)..", MENUID_OPEN);
	fileMenu->appendMenuItem ("保存(Ctrl+S)", MENUID_SAVE);
	fileMenu->appendMenuItem ("另存为..", MENUID_SAVEAS);
	fileMenu->appendMenuItem ("关闭", MENUID_CLOSE);
	fileMenu->appendMenuItem ("切换项目..", MENUID_CHOOSE_VFSROOT);
	fileMenu->appendMenuItem ("退出", MENUID_EXIT);

	ATOM_PopupMenu *editMenu = menubar->createSubMenu (1);
	editMenu->appendMenuItem ("撤销(Ctrl+Z)", MENUID_UNDO);
	editMenu->appendMenuItem ("重做(Ctrl+Shift+Z)", MENUID_REDO);
	editMenu->appendMenuItem ("复制(Ctrl+C)", MENUID_COPY);
	editMenu->appendMenuItem ("剪切(Ctrl+X)", MENUID_CUT);
	editMenu->appendMenuItem ("粘贴(Ctrl+V)", MENUID_PASTE);

	ATOM_PopupMenu *viewMenu = menubar->createSubMenu (2);
	viewMenu->appendMenuItem ("摄像机参数..", MENUID_CAMERA_PARAMS);
	viewMenu->appendMenuItem ("自定义速度..", 0);
	ATOM_PopupMenu *timeScaleMenu = viewMenu->createSubMenu (1);
	timeScaleMenu->appendMenuItem ("32倍", MENUID_TIMESCALE_32);
	timeScaleMenu->appendMenuItem ("16倍", MENUID_TIMESCALE_16);
	timeScaleMenu->appendMenuItem ("8倍", MENUID_TIMESCALE_8);
	timeScaleMenu->appendMenuItem ("4倍", MENUID_TIMESCALE_4);
	timeScaleMenu->appendMenuItem ("2倍", MENUID_TIMESCALE_2);
	timeScaleMenu->appendMenuItem ("正常", MENUID_TIMESCALE_1);
	timeScaleMenu->appendMenuItem ("1/2倍", MENUID_TIMESCALE_1_2);
	timeScaleMenu->appendMenuItem ("1/4倍", MENUID_TIMESCALE_1_4);
	timeScaleMenu->appendMenuItem ("1/8倍", MENUID_TIMESCALE_1_8);
	timeScaleMenu->appendMenuItem ("1/16倍", MENUID_TIMESCALE_1_16);
	timeScaleMenu->appendMenuItem ("1/32倍", MENUID_TIMESCALE_1_32);

	// 渲染菜单
	ATOM_PopupMenu *renderMenu = menubar->createSubMenu (3);
	
	renderMenu->appendMenuItem ("渲染模式", 0);														// 0
	ATOM_PopupMenu *schemeModeMenu = renderMenu->createSubMenu (0);
	schemeModeMenu->appendMenuItem ("延迟着色", MENUID_RENDER_SCHEME_DEFERRED);
	schemeModeMenu->appendMenuItem ("前向着色", MENUID_RENDER_SCHEME_FORWARD);

	renderMenu->appendMenuItem ("延迟着色", MENUID_DEFERRED_RENDERING);								// 1
	renderMenu->appendMenuItem ("延迟着色-法线", MENUID_DEFERRED_RENDERING_SHOWNORMALS);			// 2
	renderMenu->appendMenuItem ("延迟着色-颜色", MENUID_DEFERRED_RENDERING_SHOWCOLORS);				// 3
	renderMenu->appendMenuItem ("延迟着色-深度", MENUID_DEFERRED_RENDERING_SHOWDEPTH);				// 4
	renderMenu->appendMenuItem ("延迟着色-光照", MENUID_DEFERRED_RENDERING_SHOWLIGHTBUFFER);		// 5
	renderMenu->appendMenuItem ("延迟着色-ShadowMap", MENUID_DEFERRED_RENDERING_SHOWSHADOWMAP);		// 6
	renderMenu->appendMenuItem ("延迟着色-ShadowMask", MENUID_DEFERRED_RENDERING_SHOWSHADOWMASK);	// 7
	//--- wangjian added ---//
	// HalfLambert相关
	renderMenu->appendMenuItem ("延迟着色-HalfLambert", MENUID_DEFERRED_RENDERING_SHOWHALFLAMBERT);	// 8
	renderMenu->appendMenuItem ("后处理-Bloom", MENUID_DEFERRED_RENDERING_SHOWBLOOM);				// 9
	//----------------------//
	//renderMenu->appendMenuItem ("前向渲染", MENUID_FORWARD_RENDERING);
	//renderMenu->appendMenuItem ("固定管线", MENUID_FF_RENDERING);
	renderMenu->appendMenuItem ("线框模式(Ctrl+W)", MENUID_TOGGLE_WIREFRAME);						// 10
	renderMenu->appendMenuItem ("大气效果", MENUID_RENDER_ATMOSPHERE);								// 11
	renderMenu->appendMenuItem ("地面散射", MENUID_RENDER_AERIALPERSPECTIVE);						// 12
	renderMenu->appendMenuItem ("SSAO", MENUID_RENDER_SSAO);										// 13
	renderMenu->appendMenuItem ("SSAO Down Sample", MENUID_RENDER_SSAODOWNSAMPLE);					// 14
	renderMenu->appendMenuItem ("SSAO Blur", MENUID_RENDER_SSAOBLUR);								// 15

	renderMenu->appendMenuItem ("阴影质量", MENUID_RENDER_SHADOW);									// 16
	ATOM_PopupMenu *shadowQualityMenu = renderMenu->createSubMenu (16);						
	shadowQualityMenu->appendMenuItem ("高", MENUID_RENDER_SHADOWQUALITY_HIGH);
	shadowQualityMenu->appendMenuItem ("低", MENUID_RENDER_SHADOWQUALITY_LOW);
	shadowQualityMenu->appendMenuItem ("无", MENUID_RENDER_SHADOWQUALITY_DISABLE);
	//ATOM_RenderSettings::enableShadow(true);

	// wangjian modified
	//renderMenu->appendMenuItem ("后处理效果", MENUID_RENDER_POSTEFFECT);							// 17
	//ATOM_PopupMenu *postEffectMenu = renderMenu->createSubMenu (17);						
	//postEffectMenu->appendMenuItem ("抗锯齿", MENUID_RENDER_PE_FXAA);
	//postEffectMenu->appendMenuItem ("Bloom", MENUID_RENDER_PE_BLOOM);
	//postEffectMenu->appendMenuItem ("雨雪气候", MENUID_RENDER_PE_WEATHER);

	renderMenu->appendMenuItem ("水体效果", 0);														// 17
	ATOM_PopupMenu *waterQualityMenu = renderMenu->createSubMenu (17);
	waterQualityMenu->appendMenuItem ("低", MENUID_RENDER_WATERQUALITY_LOW);
	waterQualityMenu->appendMenuItem ("中", MENUID_RENDER_WATERQUALITY_MEDIUM);
	waterQualityMenu->appendMenuItem ("高", MENUID_RENDER_WATERQUALITY_HIGH);
	//ATOM_RenderSettings::setWaterQuality(ATOM_RenderSettings::QUALITY_HIGH);

	renderMenu->appendMenuItem ("光源开关", 0);														// 18
	ATOM_PopupMenu *lightToggleMenu = renderMenu->createSubMenu (18);
	lightToggleMenu->appendMenuItem ("太阳光", MENUID_RENDER_LIGHT_DIRECTIONAL);
	lightToggleMenu->appendMenuItem ("点光源", MENUID_RENDER_LIGHT_POINT);

	renderMenu->appendMenuItem ("地形贴图质量", 0);													// 19
	ATOM_PopupMenu *terrainDetailMenu = renderMenu->createSubMenu (19);
	terrainDetailMenu->appendMenuItem ("低", MENUID_RENDER_TERRAINDETAIL_LOW);
	terrainDetailMenu->appendMenuItem ("中", MENUID_RENDER_TERRAINDETAIL_MEDIUM);
	terrainDetailMenu->appendMenuItem ("高", MENUID_RENDER_TERRAINDETAIL_HIGH);
	//ATOM_RenderSettings::enableDetailedTerrain(true);
	//ATOM_RenderSettings::enableTerrDetailNormal(true);

	renderMenu->appendMenuItem ("ToneMap", MENUID_TOGGLE_TONEMAPPING);								// 20

	renderMenu->appendMenuItem ("次表面散射", MENUID_TOGGLE_SSS);									// 21
	renderMenu->appendMenuItem ("State Sort", MENUID_TOGGLE_STATESORTING);							// 22 wangjian added : 渲染状态排序
	renderMenu->appendMenuItem ("Render queue cache", MENUID_TOGGLE_RENDERQUEUE_CACHE);				// 23 wangjian added : 渲染队列缓存
	renderMenu->appendMenuItem ("Dump scene statistics", MENUID_TOGGLE_DUMPSCENESTATISTICS);		// 24 wangjian added : 渲染队列缓存

	ATOM_PopupMenu *toolMenu = menubar->createSubMenu (4);
	toolMenu->appendMenuItem ("DDS贴图优化...", MENUID_DDSOPT);
	toolMenu->appendMenuItem ("TTF字体查看器...", MENUID_VIEWTTF);
	toolMenu->appendMenuItem ("安装EarthSculptor插件", MENUID_INSTALL_ESPLUGIN);
	toolMenu->appendMenuItem ("编辑脚本..", MENUID_EDIT_SCRIPT);
	toolMenu->appendMenuItem ("曲线编辑器..", MENUID_CURVE_EDITOR);
	toolMenu->appendMenuItem ("查看摄像机参数..", MENUID_DUMP_CAMERA);
	toolMenu->appendMenuItem ("保存当前场景文件列表", MENUID_SAVE_SCENE_FILELIST);					// wangjian
	toolMenu->appendMenuItem ("保存所有场景文件列表", MENUID_SAVE_ALL_SCENE_FILELIST);				// wangjian
	toolMenu->appendMenuItem ("合并法线(DXT5)和光泽图...", MENUID_MERGE_NORMAL_GLOSS_MAP);			// wangjian
	toolMenu->appendMenuItem ("重新生成所有的NM2文件", MENUID_REGEN_ALL_NM2);						// wangjian
	toolMenu->appendMenuItem ("压缩所有的NM2文件", MENUID_COMPRESS_ALL_NM2);						// wangjian

	ATOM_PopupMenu *helpMenu = menubar->createSubMenu (5);
	helpMenu->appendMenuItem ("查看更新记录", MENUID_VIEW_CHANGELOG);
	helpMenu->appendMenuItem ("查看D3D设备能力..", MENUID_VIEW_D3DCAPS);
	helpMenu->appendMenuItem ("关于..", MENUID_ABOUT);

	_realtimeCtrl->setMenuBar (menubar);

	return true;
}

void StudioApp::loadPlugins (void)
{
	char ModuleName[256];
	char FileName[256];
	GetModuleFileNameA (NULL, ModuleName, 256);
	char *p = strrchr (ModuleName, '\\');
	*(p+1) = '\0';
	strcpy (FileName, ModuleName);
	strcat (FileName, "*.as");

	WIN32_FIND_DATAA wfd;
	HANDLE hFind = ::FindFirstFileA (FileName, &wfd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!strcmp(wfd.cFileName, ".") || !strcmp(wfd.cFileName, ".."))
			{
				continue;
			}

			if (wfd.dwFileAttributes != 0xFFFFFFFF)
			{
				if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
				{
					unsigned size = strlen(wfd.cFileName);
					if (size > 3 && !stricmp(wfd.cFileName + size - 3, ".as"))
					{
						strcpy (FileName, ModuleName);
						strcat (FileName, wfd.cFileName);
						loadPlugin (wfd.cFileName);
					}
				}
			}
		}
		while (::FindNextFileA (hFind, &wfd));
		::FindClose (hFind);
	}

	_editorImpl->addPlugin (_scenePlugin);
	_scenePlugin->initPlugin (_editorImpl);

	_editorImpl->addPlugin (_modelPlugin);
	_modelPlugin->initPlugin (_editorImpl);

#if defined(SUPPORT_BILLBOARD_TREE)
	_editorImpl->addPlugin (_treePlugin);
	_treePlugin->initPlugin (_editorImpl);
#endif

	_editorImpl->addPlugin (_guiPlugin);
	_guiPlugin->initPlugin (_editorImpl);

	_editorImpl->addPlugin (_actorPlugin);
	_actorPlugin->initPlugin (_editorImpl);

	_editorImpl->addPlugin (_ribbontrailPlugin);
	_ribbontrailPlugin->initPlugin (_editorImpl);

	_editorImpl->addPlugin (_compositionPlugin);
	_compositionPlugin->initPlugin (_editorImpl);
}

void StudioApp::loadPlugin (const char *filename)
{
	HMODULE hDll = ::LoadLibraryA (filename);

	if (hDll)
	{
		typedef unsigned (AS_PLUGIN_API *PFNGetNumPlugins) (void);
		typedef AS_Plugin * (AS_PLUGIN_API *PFNGetPlugin) (unsigned index);
		PFNGetNumPlugins pfnGetNumPlugins = (PFNGetNumPlugins)::GetProcAddress (hDll, "GetNumPlugins");
		PFNGetPlugin pfnGetPlugin = (PFNGetPlugin)::GetProcAddress (hDll, "GetPlugin");
		if (pfnGetNumPlugins && pfnGetPlugin)
		{
			__try
			{
				for (unsigned i = 0; i < pfnGetNumPlugins(); ++i)
				{
					AS_Plugin *plugin = pfnGetPlugin (i);
					if (plugin)
					{
						_editorImpl->addPlugin (plugin);
						plugin->initPlugin (_editorImpl);
					}
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				ATOM_LOGGER::error ("Error loading plugin <%s>\n", filename);				
			}
		}
	}
}

int StudioApp::getNumDrawCalls (void) const
{
	return _numDrawCalls;
}

int StudioApp::getNumPrimitivesDrawn (void) const
{
	return _numPrimitivesDrawn;
}

void *StudioApp::getDevice (void) const
{
	return _device;
}

void StudioApp::onReportError (ATOM_AppErrorHandler::ErrorType errorType)
{
	_editorImpl->emergencySave ();
}

void  StudioApp::setDefaultFont( const char * filename )
{
	ATOM_AutoFile f(filename, ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
		return ;
	unsigned size = f->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = f->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument doc;
	doc.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (doc.Error())
		return ;
	ATOM_TiXmlElement *root = doc.RootElement ();
	if (!root)
		return ;
	for (ATOM_TiXmlElement *font = root->FirstChildElement ("font"); font; font = font->NextSiblingElement("font"))
	{
		const char *name = font->Attribute ("name");
		if (!name)
			continue;
		const char *file = font->Attribute ("file");
		if (!file)
			continue;
		int size = 0;
		font->QueryIntAttribute ("size", &size);
		int charset = ATOM_CC_CP936;
		font->QueryIntAttribute ("set", &charset);
		int margin = 0;
		font->QueryIntAttribute ("margin", &margin);
		int default = 0;
		font->QueryIntAttribute ("default", &default);
		if(default == 0)
			return;
		ATOM_STRING fontfile = Config::getInstance()->getLastProject();
		fontfile+="\\";
		fontfile+=file;
		ATOM_LoadFont (ATOM_GUIFont::default_font_name, fontfile.c_str());
	
		ATOM_GUIFont::handle handle =  ATOM_GUIFont::createFont (ATOM_GUIFont::default_font_name, size, charset, margin);
		ATOM_Widget::fontSize = size;
		return;
	}
	
}


