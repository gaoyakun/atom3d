#include "StdAfx.h"
#include "rt_trigger.h"
#include "app.h"
#include "editor.h"
#include "axisnode.h"
#include "trackballnode.h"
#include "editor_impl.h"
#include "camera_modal.h"
#include "camera_maya.h"
#include "tool_ddsopt.h"
#include "ttfviewer.h"
#include "misc/helper.h"

ATOM_BEGIN_EVENT_MAP(RealtimeEventTrigger, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(RealtimeEventTrigger, ATOM_WidgetCommandEvent, onCommand)
	ATOM_EVENT_HANDLER(RealtimeEventTrigger, ATOM_WidgetResizeEvent, onResize)
	ATOM_EVENT_HANDLER(RealtimeEventTrigger, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(RealtimeEventTrigger, ATOM_MenuPopupEvent, onMenuPopup)
	ATOM_UNHANDLED_EVENT_HANDLER(RealtimeEventTrigger, onUnhandled)
ATOM_END_EVENT_MAP

RealtimeEventTrigger::RealtimeEventTrigger (EditorImpl *editor)
{
	_editorImpl = editor; 
}

RealtimeEventTrigger::~RealtimeEventTrigger (void)
{
}

void RealtimeEventTrigger::onCommand (ATOM_WidgetCommandEvent *event)
{
	switch (event->id)
	{
	case MENUID_OPEN:
		{
			_editorImpl->openDocument ();
			break;
		}
	case MENUID_SAVE:
		{
			_editorImpl->saveDocument ();
			break;
		}
	case MENUID_SAVEAS:
		{
			_editorImpl->saveDocumentAs ();
			break;
		}
	case MENUID_CLOSE:
		{
			_editorImpl->closeDocument ();
			break;
		}
	case MENUID_CHOOSE_VFSROOT:
		{
			((StudioApp*)ATOM_APP)->chooseAndSetProject ();
			break;
		}
	case MENUID_EXIT:
		{
			_editorImpl->exit ();
			break;
		}
	case MENUID_UNDO:
		{
			_editorImpl->undo ();
			break;
		}
	case MENUID_REDO:
		{
			_editorImpl->redo ();
			break;
		}
	case MENUID_CAMERA_PARAMS:
		{
			_editorImpl->editCamera ();
			break;
		}
	case MENUID_TIMESCALE_32:
		{
			ATOM_APP->setTimeScale (32.f);
			break;
		}
	case MENUID_TIMESCALE_16:
		{
			ATOM_APP->setTimeScale (16.f);
			break;
		}
	case MENUID_TIMESCALE_8:
		{
			ATOM_APP->setTimeScale (8.f);
			break;
		}
	case MENUID_TIMESCALE_4:
		{
			ATOM_APP->setTimeScale (4.f);
			break;
		}
	case MENUID_TIMESCALE_2:
		{
			ATOM_APP->setTimeScale (2.f);
			break;
		}
	case MENUID_TIMESCALE_1:
		{
			ATOM_APP->setTimeScale (1.f);
			break;
		}
	case MENUID_TIMESCALE_1_2:
		{
			ATOM_APP->setTimeScale (1.f/2.f);
			break;
		}
	case MENUID_TIMESCALE_1_4:
		{
			ATOM_APP->setTimeScale (1.f/4.f);
			break;
		}
	case MENUID_TIMESCALE_1_8:
		{
			ATOM_APP->setTimeScale (1.f/8.f);
			break;
		}
	case MENUID_TIMESCALE_1_16:
		{
			ATOM_APP->setTimeScale (1.f/16.f);
			break;
		}
	case MENUID_TIMESCALE_1_32:
		{
			ATOM_APP->setTimeScale (1.f/32.f);
			break;
		}
	case MENUID_RENDER_SCHEME_DEFERRED:
		{
			_editorImpl->setSchemeMode (SCHEME_DEFERRED);
			break;
		}
	case MENUID_RENDER_SCHEME_FORWARD:
		{
			_editorImpl->setSchemeMode (SCHEME_FORWARD);
			break;
		}
	case MENUID_DEFERRED_RENDERING:
		{
			_editorImpl->setRenderMode (RM_DEFERRED);
			break;
		}
	case MENUID_DEFERRED_RENDERING_SHOWNORMALS:
		{
			_editorImpl->setRenderMode (RM_DEFERRED_NORMAL);
			break;
		}
	case MENUID_DEFERRED_RENDERING_SHOWCOLORS:
		{
			_editorImpl->setRenderMode (RM_DEFERRED_COLOR);
			break;
		}
	case MENUID_DEFERRED_RENDERING_SHOWDEPTH:
		{
			_editorImpl->setRenderMode (RM_DEFERRED_DEPTH);
			break;
		}
	case MENUID_DEFERRED_RENDERING_SHOWSHADOWMAP:
		{
			_editorImpl->setRenderMode (RM_DEFERRED_SHADOWMAP);
			break;
		}
	case MENUID_DEFERRED_RENDERING_SHOWLIGHTBUFFER:
		{
			_editorImpl->setRenderMode (RM_DEFERRED_LIGHTBUFFER);
			break;
		}
	case MENUID_DEFERRED_RENDERING_SHOWSHADOWMASK:
		{
			_editorImpl->setRenderMode (RM_DEFERRED_SHADOWMASK);
			break;
		}
	//--- wangjian added ---//
	// * 用以调试GBUFFER RT中保存的HalfLambert.
	case MENUID_DEFERRED_RENDERING_SHOWHALFLAMBERT:
		{
			_editorImpl->setRenderMode (RM_DEFERRED_HALFLAMBERT);
			break;
		}
	case MENUID_DEFERRED_RENDERING_SHOWHDRLUMIN:
		{
			_editorImpl->setRenderMode (RM_HDR_LUMIN);
			break;
		}
	case MENUID_DEFERRED_RENDERING_SHOWBLOOM:
		{
			_editorImpl->setRenderMode (RM_BLOOM);
			break;
		}
	//----------------------//
	case MENUID_TOGGLE_WIREFRAME:
		{
			_editorImpl->toggleWireFrame ();
			break;
		}

	case MENUID_TOGGLE_TONEMAPPING:
		{
			ATOM_RenderSettings::enableToneMapping (!ATOM_RenderSettings::isToneMappingEnabled());
			break;
		}

	//==============================================================================================//
	// wangjian added
	case MENUID_TOGGLE_SSS:
		{
			ATOM_RenderSettings::enableSSSRendering( !ATOM_RenderSettings::isSSSRenderingEnabled() );
			break;
		}

	case MENUID_TOGGLE_STATESORTING:
		{
			ATOM_RenderSettings::enableStateSorting(!ATOM_RenderSettings::isStateSortingEnabled());
			break;
		}
	case MENUID_TOGGLE_RENDERQUEUE_CACHE:
		{
			if( ATOM_RenderSettings::getShadowDrawItemUpdateFrameCache()!=0)
				ATOM_RenderSettings::setShadowDrawItemUpdateFrameCache(0);
			else
				ATOM_RenderSettings::setShadowDrawItemUpdateFrameCache(3);

			if( ATOM_RenderSettings::getSceneDrawItemUpdateFrameCache()!=0)
				ATOM_RenderSettings::setSceneDrawItemUpdateFrameCache(0);
			else
				ATOM_RenderSettings::setSceneDrawItemUpdateFrameCache(3);

			break;
		}
	case MENUID_TOGGLE_DUMPSCENESTATISTICS:
		{
			ATOM_SDLScene::collectSceneNodeStatistics(_editorImpl->getRealtimeCtrl()->getScene());
			ATOM_SDLScene::dumpSceneNodeStatistics();

			break;
		}
	//--------------------------//

	case MENUID_RENDER_ATMOSPHERE:
		{
			ATOM_RenderSettings::enableAtmosphereEffect (!ATOM_RenderSettings::isAtmosphereEffectEnabled());
			break;
		}
	case MENUID_RENDER_AERIALPERSPECTIVE:
		{
			ATOM_RenderSettings::Quality q = ATOM_RenderSettings::getAtmosphereQuality ();
			if (q != ATOM_RenderSettings::QUALITY_HIGH)
			{
				q = ATOM_RenderSettings::QUALITY_HIGH;
			}
			else
			{
				q = ATOM_RenderSettings::QUALITY_LOW;
			}
			ATOM_RenderSettings::setAtmosphereQuality (q);
			break;
		}
	case MENUID_RENDER_SSAO:
		{
			ATOM_RenderSettings::enableSSAO(!ATOM_RenderSettings::isSSAOEnabled());
			break;
		}
	case MENUID_RENDER_SSAODOWNSAMPLE:
		{
			ATOM_RenderSettings::enableSSAODownsample(!ATOM_RenderSettings::isSSAODownsampleEnabled());
			break;
		}
	case MENUID_RENDER_SSAOBLUR:
		{
			ATOM_RenderSettings::enableSSAOBlur(!ATOM_RenderSettings::isSSAOBlurEnabled());
			break;
		}
	case MENUID_RENDER_SHADOWQUALITY_HIGH:
		{
			ATOM_RenderSettings::enableShadow (true);
			ATOM_RenderSettings::setShadowQuality (ATOM_RenderSettings::QUALITY_HIGH);
			break;
		}
	case MENUID_RENDER_SHADOWQUALITY_LOW:
		{
			ATOM_RenderSettings::enableShadow (true);
			ATOM_RenderSettings::setShadowQuality (ATOM_RenderSettings::QUALITY_LOW);
			break;
		}
	case MENUID_RENDER_SHADOWQUALITY_DISABLE:
		{
			ATOM_RenderSettings::enableShadow (false);
			break;
		}

		//--- wangjian modified ---//
		/*case MENUID_RENDER_PE_FXAA:
		{
		ATOM_RenderSettings::enableFSAA (!ATOM_RenderSettings::isFSAAEnabled());
		break;
		}
		case MENUID_RENDER_PE_BLOOM:
		{
		ATOM_RenderSettings::enableBloom(!ATOM_RenderSettings::isBloomEnabled());
		break;
		}
		case MENUID_RENDER_PE_WEATHER:
		{
		ATOM_RenderSettings::enableRain(!ATOM_RenderSettings::isRainEnabled());
		break;
		}*/
		//----------------------------------//
	case MENUID_RENDER_LIGHT_DIRECTIONAL:
		{
			ATOM_RenderSettings::enableDirectionalLightType ( !ATOM_RenderSettings::isDirectionalLightTypeEnabled() );
			break;
		}
	case MENUID_RENDER_LIGHT_POINT:
		{
			ATOM_RenderSettings::enablePointLightType ( !ATOM_RenderSettings::isPointLightTypeEnabled() );
			break;
		}
	case MENUID_RENDER_TERRAINDETAIL_LOW:							// 低
		{
			ATOM_RenderSettings::enableTerrDetailNormal(false);		// 禁用细节法线
			ATOM_RenderSettings::enableDetailedTerrain(false);		// 禁用细节纹理
			break;
		}
	case MENUID_RENDER_TERRAINDETAIL_MEDIUM:						// 中
		{
			ATOM_RenderSettings::enableDetailedTerrain(true);		// 开启细节纹理
			ATOM_RenderSettings::enableTerrDetailNormal(false);		// 禁用细节法线
			break;
		}
	case MENUID_RENDER_TERRAINDETAIL_HIGH:							// 高
		{
			ATOM_RenderSettings::enableTerrDetailNormal(true);		// 禁用细节法线
			ATOM_RenderSettings::enableDetailedTerrain (true);		// 开启细节纹理
			break;
		}
		//------------------------//

	case MENUID_RENDER_WATERQUALITY_LOW:
		{
			ATOM_RenderSettings::setWaterQuality (ATOM_RenderSettings::QUALITY_LOW);
			break;
		}
	case MENUID_RENDER_WATERQUALITY_MEDIUM:
		{
			ATOM_RenderSettings::setWaterQuality (ATOM_RenderSettings::QUALITY_MEDIUM);
			break;
		}
	case MENUID_RENDER_WATERQUALITY_HIGH:
		{
			ATOM_RenderSettings::setWaterQuality (ATOM_RenderSettings::QUALITY_HIGH);
			break;
		}
	case MENUID_RENDER_TEXTUREQUALITY_LOW:
		{
			ATOM_GetRenderDevice()->setTextureQuality (ATOM_RenderDevice::TQ_LOW);
			break;
		}
	case MENUID_RENDER_TEXTUREQUALITY_MEDIUM:
		{
			ATOM_GetRenderDevice()->setTextureQuality (ATOM_RenderDevice::TQ_MEDIUM);
			break;
		}
	case MENUID_RENDER_TEXTUREQUALITY_HIGH:
		{
			ATOM_GetRenderDevice()->setTextureQuality (ATOM_RenderDevice::TQ_HIGH);
			break;
		}
	case MENUID_RENDER_TEXTUREQUALITY_ULTRA_HIGH:
		{
			ATOM_GetRenderDevice()->setTextureQuality (ATOM_RenderDevice::TQ_ULTRA_HIGH);
			break;
		}
	case MENUID_VIEW_CHANGELOG:
		{
			char buffer[MAX_PATH];
			::GetModuleFileNameA (NULL, buffer, MAX_PATH);
			char *p = strrchr (buffer, '\\');
			if (p)
			{
				strcpy (p + 1, "ChangeLog.txt");
			}
			::ShellExecuteA (NULL, "open", buffer, NULL, NULL, SW_SHOW);
			break;
		}
	case MENUID_VIEW_D3DCAPS:
		{
			_editorImpl->showD3DCaps ();
			break;
		}
	case MENUID_DDSOPT:
		{
#if 0
			if (1 == _editorImpl->getOpenFileNames ("ui", "UI文件(*.ui)|*.ui|", false, false, "打开UI文件"))
			{
				_editorImpl->getRealtimeCtrl()->loadHierarchy (_editorImpl->getOpenedFileName(0));
			}
#else
			//ATOM_FlashCtrl *flash = ATOM_NEW(ATOM_FlashCtrl, _editorImpl->getRealtimeCtrl(), ATOM_Rect2Di(30, 30, 300, 500), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
			//flash->loadSWF ("/PostEffectList.swf");
			//flash->callFunction ("red", ATOM_FlashArgs(0x0000FF00));
			doDDSOptimization ((HWND)_editorImpl->getRenderWindow()->getWindowId());
#endif
			break;
		}
	case MENUID_VIEWTTF:
		{
			TTFViewer viewer;
			viewer.showDialog ((HWND)_editorImpl->getRenderWindow()->getWindowId(), 0);
			break;
		}
	case MENUID_INSTALL_ESPLUGIN:
		{
			ATOM_STRING esDir = findEarthSculptorPluginDir ();
			if (esDir.empty ())
			{
				::MessageBoxA (ATOM_APP->getMainWindow(), "EarthSculptor好像没安装!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
			}
			else
			{
				if (esDir.back() != '\\')
				{
					esDir += '\\';
				}
				esDir += "Plugins\\ATOM_esplugin.dll";
			}
			char modulePath[MAX_PATH];
			::GetModuleFileNameA (NULL, modulePath, MAX_PATH);
			strcpy (strrchr (modulePath, '\\'), "\\ATOM_esplugin.dll");

			bool sourceValid = true;
			if (!ATOM_PhysicVFS().doesFileExists (modulePath))
			{
				::GetCurrentDirectoryA (MAX_PATH, modulePath);
				if (modulePath[strlen(modulePath)-1] != '\\')
					strcat (modulePath, "\\");
				strcat (modulePath, "ATOM_esplugin.dll");
				if (!ATOM_PhysicVFS().doesFileExists (modulePath))
				{
					sourceValid = false;
				}
			}

			if (!sourceValid)
			{
				::MessageBoxA (ATOM_APP->getMainWindow(), "未找到EarthSculptor插件，请重新安装ATOM3D!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
			}
			else if (!::CopyFileA (modulePath, esDir.c_str(), FALSE))
			{
				::MessageBoxA (ATOM_APP->getMainWindow(), "安装失败!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
			}
			else
			{
				::MessageBoxA (ATOM_APP->getMainWindow(), "安装成功!", "ATOM3D编辑器", MB_OK);
			}
			break;
		}
	case MENUID_EDIT_SCRIPT:
		{
			/*
			ATOM_RealtimeCtrl *w = _editorImpl->getRealtimeCtrl();
			ATOM_Widget *widget = w->loadHierarchy("/ceshi3d.ui");
			ATOM_RealtimeCtrl *rt = (ATOM_RealtimeCtrl*)widget->getFirstControl ();
			ATOM_DeferredScene *scn = ATOM_NEW(ATOM_DeferredScene);
			ATOM_RenderScheme *customRenderScheme = (ATOM_CustomRenderScheme*)ATOM_RenderScheme::createRenderScheme ("custom");
			scn->setRenderScheme (customRenderScheme);
			scn->load ("/ceshi3d.3sg");
			scn->getCamera()->setViewMatrix(*scn->getCameraMatrix());
			scn->getCamera()->setPerspective(3.14/3.f, float(rt->getClientRect().size.w)/float(rt->getClientRect().size.h), 1.f, 1000.f);
			rt->setScene (scn);
			rt->show (ATOM_Widget::ShowNormal);
			*/
			break;
		}
	case MENUID_CURVE_EDITOR:
		{
			_editorImpl->showCurveEditor (true);
			break;
		}
	case MENUID_DUMP_CAMERA:
		{
			_editorImpl->showCameraParams ();
			break;
		}
	//--- wangjian added ---//
	case MENUID_SAVE_SCENE_FILELIST:
		{
			if ( 1 == _editorImpl->getOpenFileNames ("", "数据文件(*.dat)|*.dat|", false, true, "保存文件") )
			{
				//ATOM_SaveOpenedFileList(_editorImpl->getOpenedFileName (0));
				char srcFileName[ATOM_VFS::max_filename_length];
				ATOM_GetNativePathName (_editorImpl->getOpenedFileName(0), srcFileName);
				ATOM_CopyOpenedFileList(srcFileName,0,true);
			}
			break;
		}
	case MENUID_SAVE_ALL_SCENE_FILELIST:
		{
			if ( 1 == _editorImpl->getOpenFileNames ("xml", "关卡列表文件(*.xml)|*.xml|", false, false, "打开文件") )
			{
				char srcLevelListFileName[ATOM_VFS::max_filename_length];
				ATOM_GetNativePathName (_editorImpl->getOpenedFileName(0), srcLevelListFileName);
				if( ATOM_AsyncLoader::ProcessLevelOpenedFileList(srcLevelListFileName) )
				{
#if 0		
					if ( 1 == _editorImpl->getOpenFileNames ("", "数据文件(*.dat)|*.dat|", false, true, "保存文件") )
					{
						char destFileName[ATOM_VFS::max_filename_length];
						ATOM_GetNativePathName (_editorImpl->getOpenedFileName(0), destFileName);
						ATOM_CopyOpenedFileList(destFileName,0,true);
					}
#endif
					::MessageBoxA ( ATOM_APP->getMainWindow(), "所有关卡的文件列表导出完毕!", "ATOM3D编辑器", MB_OK );
				}
			}
			break;
		}
	case MENUID_MERGE_NORMAL_GLOSS_MAP:
		{
			unsigned openedFileNum = _editorImpl->getOpenImageFileNames (true, false, "选择贴图");
			if ( openedFileNum > 0 )
			{
				ATOM_SET<ATOM_STRING> normal_textures;
				ATOM_SET<ATOM_STRING> gloss_textures;
				for( int i = 0; i < openedFileNum; ++i )
				{
					char destFileName[ATOM_VFS::max_filename_length];
					ATOM_GetNativePathName (_editorImpl->getOpenedImageFileName(i), destFileName);
					if( strstr( _editorImpl->getOpenedImageFileName(i), "_s." ) )
					{
						gloss_textures.insert(_editorImpl->getOpenedImageFileName(i));
					}
					else if( strstr( _editorImpl->getOpenedImageFileName(i), "_n." ) )
					{
						normal_textures.insert(_editorImpl->getOpenedImageFileName(i));
					}
				}
				ATOM_ASSERT(normal_textures.size()==gloss_textures.size());
				if( normal_textures.size()!=gloss_textures.size() )
				{
					::MessageBoxA (ATOM_APP->getMainWindow(), "法线图数量和光泽图数量不匹配!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
					return;
				}

				ATOM_AUTOPTR(ATOM_Material) _mergeMaterial = ATOM_MaterialManager::createMaterialFromCore (	ATOM_GetRenderDevice(), 
																				"/editor/materials/merge_normalgloss.mat"	);
				ATOM_ASSERT(_mergeMaterial);
				_mergeMaterial->setActiveEffect ("default");

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				ATOM_PhysicVFS v;

				char directory[ATOM_VFS::max_filename_length];
				if (!ATOM_GetNativePathName ((*normal_textures.begin()).c_str(), directory))
				{
					return;
				}

				char *lastSep = strrchr (directory, '\\');
				*lastSep = '\0';
				if (!v.isDir(directory))
				{
					return;
				}

				ATOM_STRING destDirectory = directory;
				destDirectory += "/outputs";
				if ( !v.isDir(destDirectory.c_str()) && !v.mkDir (destDirectory.c_str()))
				{
					return;
				}

				ATOM_SET<ATOM_STRING>::iterator iter_normal = normal_textures.begin();
				ATOM_SET<ATOM_STRING>::iterator iter_gloss = gloss_textures.begin();
				for( ; 
					iter_normal != normal_textures.end(), iter_gloss != gloss_textures.end(); 
					iter_normal++, iter_gloss++ )
				{
					const ATOM_STRING & norm_file = *iter_normal;
					const ATOM_STRING & gloss_file = *iter_gloss;

					ATOM_AUTOREF(ATOM_Texture) _normal = ATOM_CreateTextureResource(norm_file.c_str(),ATOM_PIXEL_FORMAT_UNKNOWN,-1);
					ATOM_ASSERT(_normal);
					ATOM_AUTOREF(ATOM_Texture) _gloss = ATOM_CreateTextureResource(gloss_file.c_str(),ATOM_PIXEL_FORMAT_UNKNOWN,-1);
					ATOM_ASSERT(_gloss);

					ATOM_RenderDevice *device = ATOM_GetRenderDevice();

					static ATOM_AUTOREF(ATOM_Texture) _mergeMap = 
						device->allocTexture (NULL, NULL, _normal->getWidth(), _normal->getHeight(), ATOM_PIXEL_FORMAT_RGBA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOMIPMAP|ATOM_Texture::RENDERTARGET);
					static ATOM_AUTOREF(ATOM_DepthBuffer) depth = 
						device->allocDepthBuffer (_normal->getWidth(), _normal->getHeight());

					ATOM_AUTOREF(ATOM_DepthBuffer) oldDepth = device->getDepthBuffer ();
					ATOM_AUTOREF(ATOM_Texture) oldRT = device->getRenderTarget (0);
					device->setDepthBuffer (depth.get());
					ATOM_Rect2Di oldVP = device->getViewport (NULL);

					device->setRenderTarget (0, _mergeMap.get());
					device->setViewport (NULL, ATOM_Rect2Di(0, 0, _normal->getWidth(), _normal->getHeight()));
					device->beginFrame ();

					_mergeMaterial->getParameterTable()->setTexture ("normalTexture", _normal.get());
					_mergeMaterial->getParameterTable()->setTexture ("glossTexture", _gloss.get());
					
					float deltax = (float)0.5f/(float)_normal->getWidth();
					float deltay = (float)0.5f/(float)_normal->getHeight();

					// copy old value
					{
						float vertices[4 * 5] = {
							-1.f - deltax, -1.f + deltay, 0.f, 0.f, 1.f,
							1.f - deltax, -1.f + deltay, 0.f, 1.f, 1.f,
							1.f - deltax,  1.f + deltay, 0.f, 1.f, 0.f,
							-1.f - deltax,  1.f + deltay, 0.f, 0.f, 0.f
						};
						unsigned short indices[4] = {
							0, 1, 2, 3
						};

						ATOM_RenderDevice *device = ATOM_GetRenderDevice();
						unsigned numPasses = _mergeMaterial->begin (device);
						for (unsigned pass = 0; pass < numPasses; ++pass)
						{
							if (_mergeMaterial->beginPass (device, pass))
							{
								device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_FAN, 4, 4, ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2, 5 * sizeof(float), vertices, indices);
								_mergeMaterial->endPass (device, pass);
							}
						}
						_mergeMaterial->end (device);
					}

					device->endFrame ();

					device->setRenderTarget (0, oldRT.get());
					device->setDepthBuffer (oldDepth.get());
					device->setViewport (NULL, oldVP);

					ATOM_STRING::size_type seperate = norm_file.find_last_of('/');
					ATOM_STRING file = norm_file.substr(seperate+1);
					file = destDirectory + "/" + file;
					ATOM_STRING::size_type dot = file.find_last_of('.');
					file = file.substr(0,dot);
					file = file + ".dds";
					_mergeMap->saveToFileEx(file.c_str(),ATOM_PIXEL_FORMAT_DXT5);
				}
			}
			break;
		}
	case MENUID_REGEN_ALL_NM2:
		{
			char path[MAX_PATH] = { '\0' };
			if (ChooseDir (ATOM_APP->getMainWindow(), "选择路径", path))
			{
				ATOM_PhysicVFS vfs;
				char dir[ATOM_VFS::max_filename_length];
				vfs.identifyPath (path, dir);
				if (!vfs.isDir (dir))
				{
					::MessageBoxA (ATOM_APP->getMainWindow(), "所选不是目录!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
					return;
				}

				ATOM_DEQUE<ATOM_STRING> dirs;
				ATOM_STRING s = dir;
				if (s.length() == 0)
				{
					return;
				}

				if (s[s.length()-1] != '\\' && s[s.length()-1] != '/')
				{
					s += "\\*";
				}
				else
				{
					s += '*';
				}
				dirs.push_back(s);

				ATOM_VECTOR<ATOM_STRING> filelist;

				while (!dirs.empty ())
				{
					WIN32_FIND_DATAA wfd;
					HANDLE hFind = ::FindFirstFileA (dirs.front().c_str(), &wfd);
					if (hFind != INVALID_HANDLE_VALUE)
					{
						do
						{
							if (!strcmp (wfd.cFileName, ".") || !strcmp (wfd.cFileName, ".."))
							{
								continue;
							}
							if (wfd.dwFileAttributes != 0xFFFFFFFF)
							{
								ATOM_STRING src = dirs.front();
								src.erase (src.length()-1, 1);
								src += wfd.cFileName;

								if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
								{
									const char *ext = strrchr (src.c_str(), '.');
					
									if (!stricmp (ext, ".nm2"))
									{
										filelist.push_back (src);
									}

								}
								else
								{
									src += "\\*";
									dirs.push_back (src);
								}
							}
						} while (::FindNextFileA (hFind, &wfd));
						::FindClose (hFind);
					}

					dirs.pop_front();
				}

				for( int i = 0; i < filelist.size(); ++i )
				{
					ATOM_STRING & filename = filelist[i];

					//ATOM_HARDREF(ATOM_Geode) geode = ATOM_HARDREF(ATOM_Geode)();
					//geode->setNodeFileName (filename);
					//if (geode->load (ATOM_GetRenderDevice()))
					bool createNew = false;
					ATOM_AUTOREF(ATOM_SharedModel) model = ATOM_LookupOrCreateObject (ATOM_SharedModel::_classname(), filename.c_str(), &createNew);
					if( model->load(ATOM_GetRenderDevice(), filename.c_str(), -1 ) )
					{
						if ( !model->save (filename.c_str()) )
						{
							char errorinf[MAX_PATH] = {0};
							sprintf(errorinf,"重新保存出错：%s!",filename.c_str());
							::MessageBoxA (ATOM_APP->getMainWindow(), errorinf, "ATOM3D编辑器", MB_OK|MB_ICONHAND);
							return;
						}
					}
					else
					{
						char errorinf[MAX_PATH] = {0};
						sprintf(errorinf,"重新加载出错：%s!",filename.c_str());
						::MessageBoxA (ATOM_APP->getMainWindow(), errorinf, "ATOM3D编辑器", MB_OK|MB_ICONHAND);
						return;
					}
				}

				char okinfo[MAX_PATH] = {0};
				sprintf(okinfo,"重新加载完成：%s!",path);
				::MessageBoxA (ATOM_APP->getMainWindow(), okinfo, "ATOM3D编辑器", MB_OK);
			}

			break;
		}
	case MENUID_COMPRESS_ALL_NM2:
		{
			char path[MAX_PATH] = { '\0' };
			if (ChooseDir (ATOM_APP->getMainWindow(), "选择路径", path))
			{
				ATOM_PhysicVFS vfs;
				char dir[ATOM_VFS::max_filename_length];
				vfs.identifyPath (path, dir);
				if (!vfs.isDir (dir))
				{
					::MessageBoxA (ATOM_APP->getMainWindow(), "所选不是目录!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
					return;
				}

				ATOM_DEQUE<ATOM_STRING> dirs;
				ATOM_STRING s = dir;
				if (s.length() == 0)
				{
					return;
				}

				if (s[s.length()-1] != '\\' && s[s.length()-1] != '/')
				{
					s += "\\*";
				}
				else
				{
					s += '*';
				}
				dirs.push_back(s);

				ATOM_VECTOR<ATOM_STRING> filelist;

				while (!dirs.empty ())
				{
					WIN32_FIND_DATAA wfd;
					HANDLE hFind = ::FindFirstFileA (dirs.front().c_str(), &wfd);
					if (hFind != INVALID_HANDLE_VALUE)
					{
						do
						{
							if (!strcmp (wfd.cFileName, ".") || !strcmp (wfd.cFileName, ".."))
							{
								continue;
							}
							if (wfd.dwFileAttributes != 0xFFFFFFFF)
							{
								ATOM_STRING src = dirs.front();
								src.erase (src.length()-1, 1);
								src += wfd.cFileName;

								if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
								{
									const char *ext = strrchr (src.c_str(), '.');

									if (!stricmp (ext, ".nm2"))
									{
										filelist.push_back (src);
									}

								}
								else
								{
									src += "\\*";
									dirs.push_back (src);
								}
							}
						} while (::FindNextFileA (hFind, &wfd));
						::FindClose (hFind);
					}

					dirs.pop_front();
				}

				for( int i = 0; i < filelist.size(); ++i )
				{
					ATOM_STRING & filename = filelist[i];
					bool createNew = false;
					ATOM_AUTOREF(ATOM_SharedModel) model = ATOM_LookupOrCreateObject (ATOM_SharedModel::_classname(), filename.c_str(), &createNew);
					if ( !model->convertToCompressed (ATOM_GetRenderDevice(),filename.c_str()) )
					{
						char errorinf[MAX_PATH] = {0};
						sprintf(errorinf,"压缩出错：%s!",filename.c_str());
						::MessageBoxA (ATOM_APP->getMainWindow(), errorinf, "ATOM3D编辑器", MB_OK|MB_ICONHAND);
						return;
					}

				}

				char okinfo[MAX_PATH] = {0};
				sprintf(okinfo,"压缩完成：%s!",path);
				::MessageBoxA (ATOM_APP->getMainWindow(), okinfo, "ATOM3D编辑器", MB_OK);
			}

			break;
		}
	//----------------------//
	default:
		if (event->id >= MENUID_NEW_SUBID && event->id < PLUGIN_ID_START)
		{
			_editorImpl->newDocument (event->id - MENUID_NEW_SUBID);
		}
		else
		{
			_editorImpl->handleEvent (event);
		}
		break;
	}
}

void RealtimeEventTrigger::onResize (ATOM_WidgetResizeEvent *event)
{
	callHost (event);
	setAutoCallHost (false);

	_editorImpl->getCameraModal()->reset ((ATOM_RealtimeCtrl*)getHost());
	_editorImpl->handleEvent (event);
}

void RealtimeEventTrigger::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	setAutoCallHost (false);

	ATOM_RealtimeCtrl *realtimeCtrl = (ATOM_RealtimeCtrl*)getHost();
	const ATOM_Rect2Di &vp = realtimeCtrl->getViewport();
	if (vp != ATOM_Rect2Di(0,0,0,0))
	{
		event->canvas->fillRect (ATOM_Rect2Di(ATOM_Point2Di(0,0),event->canvas->getSize()), ATOM_ColorARGB(0.f, 0.f, 0.f, 1.f));
	}

	ATOM_RenderDevice* device = ATOM_GetRenderDevice ();
	if (!device)
	{
		return;
	}

	if (realtimeCtrl->beginRender (device, !realtimeCtrl->isTransparent()))
	{
		_editorImpl->render ();
		realtimeCtrl->endRender (device);
	}
}

void RealtimeEventTrigger::onMenuPopup (ATOM_MenuPopupEvent *event)
{
	_editorImpl->initMenuStates (event);
}

void RealtimeEventTrigger::onUnhandled (ATOM_Event *event)
{
	_editorImpl->handleEvent (event);
}
