#include "StdAfx.h"
#include "atom3d_studio.h"
#include "plugin.h"
#include "plugin_gui.h"
#include "editorform.h"
#include "proxy.h"
#include "panel_proxy.h"
#include "component_proxy.h"
#include "idmanager.h"

#define FONTLIST_TW_ID	700
#define FONTLIST_SIZE			(FONTLIST_TW_ID+0)
#define FONTLIST_MARGIN			(FONTLIST_TW_ID+1)
#define FONTLIST_CHARSET		(FONTLIST_TW_ID+2)

#define IMAGEID_WIDGETTREE		-10
#define IMAGEID_IMAGELIST		-11
#define IMAGEID_EDITORFORM		-12
#define IMAGEID_THUMB			-13

#define ID_GUI_PREVIEW			(PLUGIN_ID_START + 500)
#define ID_GUI_GENERATE_IDS		(PLUGIN_ID_START + 501)
#define ID_GUI_EXPORT_SOURCE	(PLUGIN_ID_START + 502)
#define ID_GUI_TOGGLE_GUI		(PLUGIN_ID_START + 503)
#define ID_GUI_CLONE			(PLUGIN_ID_START + 504)
#define ID_GUI_DELETE			(PLUGIN_ID_START + 505)

//#define IMAGELIST_ID_START		(ATOM_IMAGEID_APP+100)

#define ID_UI_HERICHY			(PLUGIN_ID_START+10)
#define ID_UI_IMAGELIST			(PLUGIN_ID_START+11)
#define ID_FONT_NAME			(PLUGIN_ID_START+12)
#define ID_DIALOG_OK			(PLUGIN_ID_START+13)
#define ID_DIALOG_CANCEL		(PLUGIN_ID_START+14)
#define ID_NEW_COLOR_IMAGE		(PLUGIN_ID_START+15)
#define ID_NEW_MATERIAL_IMAGE	(PLUGIN_ID_START+16)
#define ID_NEW_TEXTURE_IMAGE	(PLUGIN_ID_START+17)
#define ID_NEW_TEXTURE_IMAGE9	(PLUGIN_ID_START+18)
#define ID_IMAGE_COLOR_NORMAL	(PLUGIN_ID_START+19)
#define ID_IMAGE_COLOR_HOVER	(PLUGIN_ID_START+20)
#define ID_IMAGE_COLOR_HOLD		(PLUGIN_ID_START+21)
#define ID_IMAGE_COLOR_DISABLED	(PLUGIN_ID_START+22)
#define ID_IMAGE_FILE_NORMAL	(PLUGIN_ID_START+23)
#define ID_IMAGE_FILE_HOVER		(PLUGIN_ID_START+24)
#define ID_IMAGE_FILE_HOLD		(PLUGIN_ID_START+25)
#define ID_IMAGE_FILE_DISABLED	(PLUGIN_ID_START+26)
#define ID_MATERIAL_FILE_NORMAL	(PLUGIN_ID_START+27)
#define ID_MATERIAL_FILE_HOVER		(PLUGIN_ID_START+28)
#define ID_MATERIAL_FILE_HOLD		(PLUGIN_ID_START+29)
#define ID_MATERIAL_FILE_DISABLED	(PLUGIN_ID_START+30)
#define ID_EDIT_IMAGE			(PLUGIN_ID_START+31)
#define ID_DELETE_IMAGE			(PLUGIN_ID_START+32)


//--- wangjian added ---//
#define ID_ASYNCLOAD_PRIORITY  1200
//----------------------//

static PluginGUI *_pluginGUI = 0;

PluginGUI::PluginGUI (void)
{
	_uiHerichy = 0;
	_fontList = 0;
	_widgetProp = 0;
	_customProp = 0;
	_toolbox = 0;
	_editorForm = 0;
	_editor = 0;
	_thumb = 0;
	_creationParam = 0;
	_imageList = 0;
	_pluginImages = 0;
	_imageListMenu = 0;
	_pluginGUI = this;
}

PluginGUI::~PluginGUI (void)
{
	_pluginGUI = 0;
}

unsigned PluginGUI::getVersion (void) const
{
	return AS_VERSION;
}

const char *PluginGUI::getName (void) const
{
	return "GUI editor";
}

void PluginGUI::deleteMe (void)
{
	ATOM_DELETE(this);
}

bool PluginGUI::initPlugin (AS_Editor *editor)
{
	editor->registerFileType (this, "ui", "ATOM3D 图形界面", AS_FILETYPE_CANEDIT);
	_editor = editor;
	return true;
}

void PluginGUI::donePlugin (void)
{
}

bool PluginGUI::validateProxyName (const char *name) const
{
	return name && _proxyNameSet.find (name) == _proxyNameSet.end ();
}

bool PluginGUI::changeProxyName (ControlProxy *proxy, const char *newName)
{
	if (!newName)
	{
		return false;
	}

	if (!strcmp (proxy->getName(), newName))
	{
		return true;
	}

	if (_proxyNameSet.find (newName) != _proxyNameSet.end ())
	{
		return false;
	}

	NameSet::iterator it = _proxyNameSet.find (proxy->getName());
	ATOM_ASSERT(it != _proxyNameSet.end ());
	_proxyNameSet.erase (it);

	_proxyNameSet.insert (newName);

	proxy->setName (newName);

	return true;
}

bool PluginGUI::saveHeaderFile (ControlProxy *proxy, const char *filename) const
{
	return false;
}

void PluginGUI::addProxyName (const char *name)
{
	_proxyNameSet.insert (name);
}

void PluginGUI::removeProxyName (const char *name)
{
	NameSet::iterator it = _proxyNameSet.find (name);
	if (it != _proxyNameSet.end ())
	{
		_proxyNameSet.erase (it);
	}
}

void PluginGUI::updateImageList (void)
{
	_imageList->clearItem();

	ControlProxy *proxy = _editorForm->getActiveProxy ();
	if (proxy)
	{
		const ATOM_MAP<int,WidgetParameters::ImageInfo> &imageListInfo = proxy->getWidgetParams()->imageList();
		for (ATOM_MAP<int,WidgetParameters::ImageInfo>::const_iterator it = imageListInfo.begin(); it != imageListInfo.end(); ++it)
		{
			char buffer[32];
			sprintf (buffer, "%d", it->first);
			_imageList->addItem (buffer);
		}
	}
}

void PluginGUI::updateWidgetTree (bool rebuild)
{
	if (rebuild)
	{
		_uiHerichy->clear ();
	
		ATOM_TreeItem *item = _uiHerichy->newItem ("Root", ATOM_IMAGEID_TREECTRL_EXPANDED_IMAGE, ATOM_IMAGEID_TREECTRL_COLLAPSED_IMAGE, NULL);

		updateWidgetTreeR (item);
	}

	_uiHerichy->selectItem (NULL, false);
	ControlProxy *activeProxy = _editorForm->getActiveProxy ();
	if (activeProxy)
	{
		_uiHerichy->selectItem (activeProxy->getTreeItem(), false);
	}
}

void PluginGUI::updateWidgetTreeR (ATOM_TreeItem *parentItem)
{
	ControlProxy *proxy = (ControlProxy*)parentItem->getUserData ();
	ATOM_Widget *widget = proxy ? proxy->getWidget() : _editorForm;

	for (ATOM_Widget *c = widget->getFirstControl(); c; c = c->getNextSibling ())
	{
		ControlProxy *proxy = ControlProxy::getWidgetProxy (c);
		if (proxy)
		{
			ATOM_TreeItem *childItem = _uiHerichy->newItem (proxy->getName(), ATOM_IMAGEID_TREECTRL_EXPANDED_IMAGE, ATOM_IMAGEID_TREECTRL_COLLAPSED_IMAGE, parentItem);
			childItem->setUserData ((unsigned)proxy);
			proxy->setTreeItem (childItem);

			updateWidgetTreeR (childItem);
		}
	}

	for (ATOM_Widget *c = widget->getFirstChild(); c; c = c->getNextSibling ())
	{
		ControlProxy *proxy = ControlProxy::getWidgetProxy (c);
		if (proxy)
		{
			ATOM_TreeItem *childItem = _uiHerichy->newItem (proxy->getName(), ATOM_IMAGEID_TREECTRL_EXPANDED_IMAGE, ATOM_IMAGEID_TREECTRL_COLLAPSED_IMAGE, parentItem);
			childItem->setUserData ((unsigned)proxy);
			proxy->setTreeItem (childItem);

			updateWidgetTreeR (childItem);
		}
	}
}

void PluginGUI::updateFontList (void)
{
	static const char *Operation = "文件";
	static const char *Manage = "管理";
	
	_fontList->clear ();
	_fontList->addButton ("fontlist_load", "载入字体列表..", Operation, &PluginGUI::FontListLoad, 0);
	_fontList->addButton ("fontlist_save", "保存字体列表", Operation, &PluginGUI::FontListSave, 0);
	_fontList->addButton ("fontlist_saveas", "另存字体列表..", Operation, &PluginGUI::FontListSaveAs, 0);
	_fontList->addButton ("fontlist_clear", "清除", Manage, &PluginGUI::FontListClear, 0);
	_fontList->addButton ("fontlist_new", "新建字体..", Manage, &PluginGUI::FontListNew, 0);

	char buff[256];
	char buff2[256];
	char group[256];
	ATOMX_TBEnum ccEnum;
	ccEnum.addEnum ("拉丁文", ATOM_CC_ISO8859_1);
	ccEnum.addEnum ("中文简体", ATOM_CC_CP936);
	ccEnum.addEnum ("中文繁体", ATOM_CC_CP950);

	for (FontInfoMap::const_iterator it = _fontInfoMap.begin(); it != _fontInfoMap.end(); ++it)
	{
		const char *name = it->second.name.c_str();
		if (it->second.comment.empty())
		{
			strcpy (group, name);
		}
		else
		{
			sprintf (group, "%s - %s", name, it->second.comment.c_str());
		}

		sprintf (buff, "fontfile_%s", name);
		_fontList->addButton (buff, "字体文件..", group, &PluginGUI::FontFileCallback, (void*)name);

		sprintf (buff, "deletefont_%s", name);
		_fontList->addButton (buff, "删除", group, &PluginGUI::FontDeleteCallback, (void*)name);

		sprintf (buff2, "fontsize_%s", name);
		_fontList->addUIntVar (buff2, FONTLIST_SIZE, it->second.size, false, group, (void*)name);
		_fontList->setVarLabel (buff2, "大小");

		sprintf (buff2, "fontmargin_%s", name);
		sprintf (buff, "%d", it->second.margin);
		_fontList->addStringVar (buff2, FONTLIST_MARGIN, buff, false, group, (void*)name);
		_fontList->setVarLabel (buff2, "间距");

		sprintf (buff2, "charset_%s", name);
		_fontList->addEnum (buff2, FONTLIST_CHARSET, ATOM_CC_CP936, ccEnum, false, group, (void*)name);
		_fontList->setVarLabel (buff2, "字符集");

		_fontList->setGroupOpenState (group, false);
	}
}

bool PluginGUI::beginEdit (const char *filename)
{
	IdManager::clear ();
	_proxyNameSet.clear ();

	_pluginImages = ATOM_NEW(ATOM_GUIImageList);
	ATOM_ColorARGB widgetTreeColor(0.73f, 0.78f, 0.85f, 1.f);
	ATOM_ColorARGB editorFormColor(0.f, 0.5f, 0.75f, 1.f);
	ATOM_ColorARGB imageListColor(0.37f, 0.69f, 0.57f, 1.f);
#if 1
	imageListColor = widgetTreeColor;
#endif
	_pluginImages->newColorImage (IMAGEID_WIDGETTREE, widgetTreeColor, widgetTreeColor, widgetTreeColor, widgetTreeColor);
	_pluginImages->newColorImage (IMAGEID_IMAGELIST, imageListColor, imageListColor, imageListColor, imageListColor);
	_pluginImages->newColorImage (IMAGEID_EDITORFORM, editorFormColor, editorFormColor, editorFormColor, editorFormColor);

	_editorForm = ATOM_NEW(EditorForm, _editor->getRealtimeCtrl(), _editor->getRealtimeCtrl()->getClientRect(), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	_editorForm->resize (ATOM_Rect2Di(0, 0, -100, -100));
	_editorForm->setPlugin (this);
	_editorForm->setImageList (_pluginImages.get());
	_editorForm->setClientImageId (IMAGEID_EDITORFORM);

	_uiHerichy = ATOM_NEW(ATOM_TreeCtrl, _editor->getRealtimeCtrl(), ATOM_Rect2Di(0, 0, 300, 600), ATOM_Widget::VScroll|ATOM_Widget::HScroll|ATOM_Widget::Border, ID_UI_HERICHY, 8, 15, 16, ATOM_Widget::ShowNormal);
	_uiHerichy->setBorderMode (ATOM_Widget::Drop);
	_uiHerichy->setImageList (_pluginImages.get());
	_uiHerichy->setClientImageId (IMAGEID_WIDGETTREE);
	updateWidgetTree (true);
	
	static const char *GeneralCtl = "普通控件";
	static const char *LayoutCtl = "布局控件";

	_toolbox = ATOM_NEW(ATOMX_TweakBar, "工具箱");
	_toolbox->setBarColor (0.5f, 0.5f, 0.5f, 1.f);
	_toolbox->addButton ("panel", "面板", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_PANEL);
	_toolbox->addButton ("spacer", "空白", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_SPACER);
	_toolbox->addButton ("viewstack", "页面集合", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_VIEWSTACK);
	_toolbox->addButton ("topwindow", "顶层窗口", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_TOPWINDOW);
	_toolbox->addButton ("dialog", "对话框", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_DIALOG);
	_toolbox->addButton ("edit", "单行编辑框", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_EDIT);
	_toolbox->addButton ("multiedit", "多行编辑框", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_MULTIEDIT);
	_toolbox->addButton ("label", "标签", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_LABEL);
	_toolbox->addButton ("button", "按钮", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_BUTTON);
	_toolbox->addButton ("progressbar", "进度条", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_PROGRESSBAR);
	_toolbox->addButton ("richedit", "富文本", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_RICHEDIT);
	_toolbox->addButton ("cell", "格子控件", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_CELL);
	_toolbox->addButton ("realtimectrl", "三维渲染控件", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_REALTIMECTRL);
	_toolbox->addButton ("listbox", "列表框", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_LISTBOX);
	_toolbox->addButton ("combobox", "下拉列表框", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_COMBOBOX);
	_toolbox->addButton ("scrollbar", "滚动条", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_SCROLLBAR);
	_toolbox->addButton ("treectrl", "树形控件", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_TREECTRL);
	_toolbox->addButton ("checkbox", "多选按钮", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_CHECKBOX);
	_toolbox->addButton ("circleprogress", "圆形进度条", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_CIRCLEPROGRESS);
	_toolbox->addButton ("listview", "表格", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_LISTVIEW);
	_toolbox->addButton ("slider", "滑动条", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_SLIDER);
	_toolbox->addButton ("marquee", "走马灯", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_MARQUEE);
	_toolbox->addButton ("hyperlink", "超链接", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_HYPERLINK);
	_toolbox->addButton ("combobox", "下拉列表", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_COMBOBOX);
	_toolbox->addButton ("flashctrl", "Flash控件", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_FLASHCTRL);
	_toolbox->addButton ("scrollmap", "滚动控件", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_SCROLLMAP);
	_toolbox->addButton ("curveeditor", "曲线编辑器", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_CURVEEDITOR);
	_toolbox->addButton ("component", "自定义组件..", GeneralCtl, &PluginGUI::ToolboxCreateWidget, (void*)WT_COMPONENT);

	_fontList = ATOM_NEW(ATOMX_TweakBar, "字体列表");
	_fontList->setBarColor (0.5f, 0.5f, 0.5f, 1.f);
	updateFontList ();

	_widgetProp = ATOM_NEW(ATOMX_TweakBar, "控件属性");
	_widgetProp->setBarColor (0.5f, 0.5f, 0.5f, 1.f);
	_editorForm->setWidgetPropertyBar (_widgetProp);

	_customProp = ATOM_NEW(ATOMX_TweakBar, "自定义属性");
	_customProp->setBarColor (0.5f, 0.5f, 0.5f, 1.f);
	_editorForm->setCustomPropertyBar (_customProp);


	_imageList = ATOM_NEW(ATOM_ListBox, _editor->getRealtimeCtrl(), ATOM_Rect2Di(0, 0, 100, 100), 16, ATOM_Widget::VScroll|ATOM_Widget::Border, ID_UI_IMAGELIST, ATOM_Widget::ShowNormal);
	_imageList->setBorderMode (ATOM_Widget::Drop);
	_imageList->setImageList (_pluginImages.get());
	_imageList->setClientImageId (IMAGEID_IMAGELIST);

	_imageListMenu = ATOM_NEW(ATOM_PopupMenu, _editor->getGUIRenderer());
	_imageListMenu->appendMenuItem ("新建", 0);
	ATOM_PopupMenu *submenu = _imageListMenu->createSubMenu (0);
	submenu->appendMenuItem ("纯色..", ID_NEW_COLOR_IMAGE);
	submenu->appendMenuItem ("图像..", ID_NEW_TEXTURE_IMAGE);
	submenu->appendMenuItem ("九宫格..", ID_NEW_TEXTURE_IMAGE9);
	submenu->appendMenuItem ("自定义材质..", ID_NEW_MATERIAL_IMAGE);
	_imageListMenu->appendMenuItem ("编辑..", ID_EDIT_IMAGE);
	_imageListMenu->appendMenuItem ("删除", ID_DELETE_IMAGE);

	//-- wangjian added ---//
	_customProp->addBoolVar ("开启异步加载", ID_ASYNCLOAD_PRIORITY, true, false, "异步加载");
	//---------------------//

	calcUILayout ();

	if (filename)
	{
		ATOM_AutoFile f(filename, ATOM_VFS::read|ATOM_VFS::text);
		if (!f)
		{
			return false;
		}

		unsigned size = f->size ();
		char *p = ATOM_NEW_ARRAY(char, size);
		size = f->read (p, size);
		p[size] = '\0';
		ATOM_TiXmlDocument doc;
		doc.Parse (p);
		ATOM_DELETE_ARRAY(p);
		if (doc.Error ())
		{
			return false;
		}

		ATOM_TiXmlElement *root = doc.RootElement ();
		if (!root)
		{
			return false;
		}

		if (strcmp (root->Value(), "UI"))
		{
			return false;
		}

		const char *fontSetFile = root->Attribute ("fonts");
		if (fontSetFile && !loadFontSet (fontSetFile))
		{
			return false;
		}
		_fontSetFile = fontSetFile ? fontSetFile : "";

		//--- wangjian added ---//
		int asyncload = -1;
		root->Attribute("asyncload", &asyncload);
		_asyncload = (asyncload!=-1);
		_customProp->setB("开启异步加载",_asyncload);
		//----------------------//

		if (!load (root))
		{
			endEdit ();
			return false;
		}
	}

	setupMenu ();

	_guiShown = true;

	return true;
}

void PluginGUI::endEdit (void)
{
	if (!_guiShown)
	{
		toggleGUI ();
	}

	cleanupMenu ();

	ATOM_DELETE(_uiHerichy);
	_uiHerichy = 0;

	ATOM_DELETE(_toolbox);
	_toolbox = 0;

	ATOM_DELETE(_fontList);
	_fontList = 0;

	ATOM_DELETE(_widgetProp);
	_widgetProp = 0;

	ATOM_DELETE(_customProp);
	_customProp = 0;

	ATOM_DELETE(_imageList);
	_imageList = 0;

	ATOM_DELETE(_thumb);
	_thumb = 0;

	ATOM_DELETE(_editorForm);
	_editorForm = 0;

	_pluginImages = 0;

	ATOM_DELETE(_imageListMenu);
	_imageListMenu = 0;

	_proxyNameSet.clear ();
	_installedFonts.clear ();
	_fontInfoMap.clear ();
	_fontSetFile.clear ();
	ControlProxy::clearProxyMap ();

	_creationParam = 0;
}

void PluginGUI::frameUpdate (void)
{
}

void PluginGUI::notifyFontChange (const char *name)
{
	notifyFontChangeR (0, name);
}

void PluginGUI::notifyFontChangeR (ControlProxy *proxy, const char *name)
{
	ATOM_Widget *widget = proxy ? proxy->getWidget() : _editorForm;

	if (widget)
	{
		for (ATOM_Widget *c = widget->getFirstControl(); c; c = c->getNextSibling())
		{
			ControlProxy *childProxy = ControlProxy::getWidgetProxy (c);
			if (childProxy)
			{
				childProxy->changeFont (name);
				notifyFontChangeR (childProxy, name);
			}
		}

		for (ATOM_Widget *c = widget->getFirstChild(); c; c = c->getNextSibling())
		{
			ControlProxy *childProxy = ControlProxy::getWidgetProxy (c);
			if (childProxy)
			{
				childProxy->changeFont (name);
				notifyFontChangeR (childProxy, name);
			}
		}
	}
}

ATOM_STRING PluginGUI::showNewFontDialog (void)
{
	const ATOM_Rect2Di &rc = _editor->getRealtimeCtrl()->getClientRect ();
	const int dialogWidth = 300;
	const int dialogHeight = 120;
	const int x = (rc.size.w - dialogWidth) / 2;
	const int y = (rc.size.h - dialogHeight) / 2;

	ATOM_Dialog *dialog = ATOM_NEW(ATOM_Dialog, _editor->getRealtimeCtrl(), ATOM_Rect2Di(x, y, dialogWidth, dialogHeight));
	dialog->setLayoutType (ATOM_WidgetLayout::Vertical);
	dialog->getLayout()->setGap (6);
	dialog->getLayout()->setInnerSpaceLeft(4);
	dialog->getLayout()->setInnerSpaceTop(4);
	dialog->getLayout()->setInnerSpaceRight(4);
	dialog->getLayout()->setInnerSpaceBottom(4);
	dialog->addEndId (ID_DIALOG_OK);
	dialog->addEndId (ID_DIALOG_CANCEL);

	ATOM_Label *label = ATOM_NEW(ATOM_Label, dialog, ATOM_Rect2Di (0, 0, -100, -33), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	label->setText ("请输入字体名称:");
	label->setAlign (ATOM_Widget::AlignX_Left);

	ATOM_Edit *edit = ATOM_NEW(ATOM_Edit, dialog, ATOM_Rect2Di (0, 0, -100, -33), ATOM_Widget::Border|ATOM_Widget::Control, ID_FONT_NAME, ATOM_Widget::ShowNormal);
	edit->setBorderMode (ATOM_Widget::Drop);
	edit->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	edit->setCursorImageId (ATOM_IMAGEID_CONTROL_BKGROUND);
	edit->setFocus ();

	ATOM_Panel *buttonBar = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di (0, 0, -100, -34), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	buttonBar->enableEventTransition (true);
	buttonBar->setClientImageId (-1);
	buttonBar->setLayoutType(ATOM_WidgetLayout::Horizontal);
	buttonBar->getLayout()->setGap (4);
	buttonBar->getLayout()->setInnerSpaceLeft(0);
	buttonBar->getLayout()->setInnerSpaceTop(0);
	buttonBar->getLayout()->setInnerSpaceRight(0);
	buttonBar->getLayout()->setInnerSpaceBottom(0);

	ATOM_Panel *spacer = ATOM_NEW(ATOM_Panel, buttonBar, ATOM_Rect2Di (0, 0, -50, -100), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	spacer->enableEventTransition (true);
	spacer->setClientImageId (-1);

	ATOM_Button *btnOk = ATOM_NEW(ATOM_Button, buttonBar, ATOM_Rect2Di (0, 0, -25, -100), ATOM_Widget::Control|ATOM_Widget::Border, ID_DIALOG_OK, ATOM_Widget::ShowNormal);
	btnOk->setText ("确定");

	ATOM_Button *btnCancel = ATOM_NEW(ATOM_Button, buttonBar, ATOM_Rect2Di (0, 0, -25, -100), ATOM_Widget::Control|ATOM_Widget::Border, ID_DIALOG_CANCEL, ATOM_Widget::ShowNormal);
	btnCancel->setText ("取消");

	if (ID_DIALOG_OK == dialog->showModal ())
	{
		ATOM_STRING fontName;
		edit->getString (fontName);
		if (!fontName.empty ())
		{
			if (_fontInfoMap.find (fontName) != _fontInfoMap.end())
			{
				::MessageBoxA (ATOM_APP->getMainWindow(), "字体名字重复!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
			}
			else
			{
				return fontName;
			}
		}
	}

	return "";
}

int PluginGUI::generateImageId (void) const
{
	int start = ATOM_IMAGEID_APP;
	ControlProxy *proxy = _editorForm->getActiveProxy();
	if (proxy)
	{
		ATOM_MAP<int,WidgetParameters::ImageInfo> &imagelist = proxy->getWidgetParams()->imageList();
		for (;;)
		{
			if (imagelist.find (start) == imagelist.end())
			{
				break;
			}
			start++;
		}
		return start;
	}
	return -1;
}

void PluginGUI::handleEvent (ATOM_Event *event)
{
	int eventId = event->getEventTypeId ();

	if (eventId == ATOM_WidgetResizeEvent::eventTypeId())
	{
		calcUILayout (); 
	}
	else if (eventId == ATOM_TreeCtrlSelChangedEvent::eventTypeId())
	{
		ATOM_TreeCtrlSelChangedEvent *e = (ATOM_TreeCtrlSelChangedEvent*)event;

		if (e->id == _uiHerichy->getId())
		{
			ATOM_TreeItem *selItem = _uiHerichy->getItemByIndex (e->selectedIndex);
			ControlProxy *proxy = selItem ? (ControlProxy*)(selItem->getUserData()) : 0;
			_editorForm->setActiveProxy (proxy);
			//--- wangjian added ---//
			if( !proxy )
			{
				_customProp->addBoolVar ("开启异步加载", ID_ASYNCLOAD_PRIORITY, _asyncload, false, "异步加载");
			}
			//---------------------//
		}
	}
	else if (eventId == ATOM_ListBoxHoverItemChangedEvent::eventTypeId())
	{
		ATOM_ListBoxHoverItemChangedEvent *e = (ATOM_ListBoxHoverItemChangedEvent*)event;
		if (e->id == ID_UI_IMAGELIST)
		{
			if (e->newIndex >= 0)
			{
				const char *s = _imageList->getItemText (e->newIndex)->getString();
				int imageId = atoi (s);
				ControlProxy *proxy = _editorForm->getActiveProxy ();
				ATOM_ASSERT(proxy);
				ATOM_MAP<int,WidgetParameters::ImageInfo>::const_iterator it = proxy->getWidgetParams()->imageList().find (imageId);
				ATOM_ASSERT(it != proxy->getWidgetParams()->imageList().end ());
				const WidgetParameters::ImageInfo &info = it->second;
				ATOM_GUIImage *image = _pluginImages->getImage (IMAGEID_THUMB);
				if (!image)
				{
					image = _pluginImages->newImage (IMAGEID_THUMB);
				}
				image->setType (WST_NORMAL, info.imageType);
				image->setType (WST_HOVER, info.imageType);
				image->setType (WST_HOLD, info.imageType);
				image->setType (WST_DISABLED, info.imageType);
				switch (info.imageType)
				{
				case ATOM_GUIImage::IT_COLOR:
					image->setColor (WST_NORMAL, info.states[WST_NORMAL].color);
					image->setColor (WST_HOVER, info.states[WST_HOVER].color);
					image->setColor (WST_HOLD, info.states[WST_HOLD].color);
					image->setColor (WST_DISABLED, info.states[WST_DISABLED].color);
					break;
				case ATOM_GUIImage::IT_IMAGE:
				case ATOM_GUIImage::IT_IMAGE9:
					image->setImage (WST_NORMAL, info.states[WST_NORMAL].filename.c_str());
					image->setRegion (WST_NORMAL, info.states[WST_NORMAL].region);
					image->setColor (WST_NORMAL, 0xFFFFFFFF);
					image->setImage (WST_HOVER, info.states[WST_HOVER].filename.c_str());
					image->setRegion (WST_HOVER, info.states[WST_HOVER].region);
					image->setColor (WST_HOVER, 0xFFFFFFFF);
					image->setImage (WST_HOLD, info.states[WST_HOLD].filename.c_str());
					image->setRegion (WST_HOLD, info.states[WST_HOLD].region);
					image->setColor (WST_HOLD, 0xFFFFFFFF);
					image->setImage (WST_DISABLED, info.states[WST_DISABLED].filename.c_str());
					image->setRegion (WST_DISABLED, info.states[WST_DISABLED].region);
					image->setColor (WST_DISABLED, 0xFFFFFFFF);
					break;
				case ATOM_GUIImage::IT_MATERIAL:
					image->setImage (WST_NORMAL, info.states[WST_NORMAL].filename.c_str());
					image->setMaterial (WST_NORMAL, info.states[WST_NORMAL].material_filename.c_str());
					image->setRegion (WST_NORMAL, info.states[WST_NORMAL].region);
					image->setImage (WST_HOVER, info.states[WST_HOVER].filename.c_str());
					image->setMaterial (WST_HOVER, info.states[WST_HOVER].material_filename.c_str());
					image->setRegion (WST_HOVER, info.states[WST_HOVER].region);
					image->setImage (WST_HOLD, info.states[WST_HOLD].filename.c_str());
					image->setMaterial (WST_HOLD, info.states[WST_HOLD].material_filename.c_str());
					image->setRegion (WST_HOLD, info.states[WST_HOLD].region);
					image->setImage (WST_DISABLED, info.states[WST_DISABLED].filename.c_str());
					image->setMaterial (WST_DISABLED, info.states[WST_DISABLED].material_filename.c_str());
					image->setRegion (WST_DISABLED, info.states[WST_DISABLED].region);
					break;
				}
				showThumb (true);
			}
			else
			{
				showThumb (false);
			}
		}
	}
	else if (eventId == ATOMX_TWCommandEvent::eventTypeId())
	{
		_editorForm->handleEvent (event);
	}
	else if (eventId == ATOMX_TWValueChangedEvent::eventTypeId())
	{
		ATOMX_TWValueChangedEvent *e = (ATOMX_TWValueChangedEvent*)event;
		if (e->id == FONTLIST_SIZE)
		{
			const char *fontName = (const char *)e->userdata;
			FontInfoMap::iterator it = _fontInfoMap.find (fontName);
			ATOM_ASSERT(it != _fontInfoMap.end());
			it->second.size = e->newValue.getI();
			ATOM_GUIFont::handle newHandle = createFont (it->second.filename.c_str(), it->second.size, it->second.charset, it->second.margin);
			if (it->second.handle != ATOM_GUIFont::invalid_handle)
			{
				ATOM_GUIFont::releaseFont (it->second.handle);
			}
			it->second.handle = newHandle;
			notifyFontChange (fontName);
		}
		//--- wangjian added ---//
		else if( e->id == ID_ASYNCLOAD_PRIORITY )
		{
			_asyncload = e->newValue.getB();
		}
		//-----------------------//
		else
		{
			_editorForm->handleEvent (event);
		}
	}
	else if (eventId == ATOM_WidgetContextMenuEvent::eventTypeId ())
	{
		ATOM_WidgetContextMenuEvent *e = (ATOM_WidgetContextMenuEvent*)event;
		if (e->id == ID_UI_IMAGELIST)
		{
			if (_editorForm->getActiveProxy())
			{
				_imageList->trackPopupMenu (e->x, e->y, _imageListMenu);
			}
		}
	}
	else if (eventId == ATOM_WidgetCommandEvent::eventTypeId ())
	{
		ATOM_WidgetCommandEvent *e = (ATOM_WidgetCommandEvent*)event;
		switch (e->id)
		{
		case ID_GUI_PREVIEW:
			{
				_editorForm->setPreviewMode (!_editorForm->isPreviewMode ());
				_editor->getMenuBar ()->getMenuItem (AS_MENUITEM_EDIT).submenu->setCheckById (ID_GUI_PREVIEW, _editorForm->isPreviewMode());
				break;
			}
		case ID_GUI_GENERATE_IDS:
			{
				generateControlIDs ();
				break;
			}
		case ID_GUI_EXPORT_SOURCE:
			{
				exportSourceCode ();
				break;
			}
		case ID_GUI_TOGGLE_GUI:
			{
				toggleGUI ();
				break;
			}
		case ID_GUI_CLONE:
			{
				_editorForm->cloneActiveProxy ();
				break;
			}
		case ID_GUI_DELETE:
			{
				_editorForm->deleteActiveProxy ();
				break;
			}
		case ID_NEW_COLOR_IMAGE:
			{
				WidgetParameters::ImageInfo info;
				info.imageType = ATOM_GUIImage::IT_COLOR;
				info.states[WST_NORMAL].color = 0xFFFFFFFF;
				info.states[WST_HOVER].color = 0xFFFFFFFF;
				info.states[WST_HOLD].color = 0xFFFFFFFF;
				info.states[WST_DISABLED].color = 0xFFFFFFFF;
				int imageid = editColorImage (generateImageId(), &info);
				if (imageid >= 0)
				{
					ATOM_MAP<int,WidgetParameters::ImageInfo> &imagelist = _editorForm->getActiveProxy()->getWidgetParams()->imageList();
					ATOM_MAP<int,WidgetParameters::ImageInfo>::iterator it = imagelist.find (imageid);
					if (it != imagelist.end ())
					{
						if (::MessageBoxA (ATOM_APP->getMainWindow(), "指定标识的图像已经存在，是否替换?", "ATOM3D编辑器", MB_YESNO|MB_ICONQUESTION) == IDNO)
						{
							break;
						}
					}
					imagelist[imageid] = info;
					_editorForm->getActiveProxy()->getWidgetParams()->apply (_editorForm->getActiveProxy()->getWidget());
					updateImageList ();
				}
				break;
			}
		case ID_NEW_MATERIAL_IMAGE:
			{
				WidgetParameters::ImageInfo info;
				info.imageType = ATOM_GUIImage::IT_MATERIAL;
				info.states[WST_NORMAL].region = ATOM_Rect2Di(0, 0, 0, 0);
				info.states[WST_HOVER].region = ATOM_Rect2Di(0, 0, 0, 0);
				info.states[WST_HOLD].region = ATOM_Rect2Di(0, 0, 0, 0);
				info.states[WST_DISABLED].region = ATOM_Rect2Di(0, 0, 0, 0);
				int imageid = editMaterialImage (generateImageId(), &info);
				if (imageid >= 0)
				{
					ATOM_MAP<int,WidgetParameters::ImageInfo> &imagelist = _editorForm->getActiveProxy()->getWidgetParams()->imageList();
					ATOM_MAP<int,WidgetParameters::ImageInfo>::iterator it = imagelist.find (imageid);
					if (it != imagelist.end ())
					{
						if (::MessageBoxA (ATOM_APP->getMainWindow(), "指定标识的图像已经存在，是否替换?", "ATOM3D编辑器", MB_YESNO|MB_ICONQUESTION) == IDNO)
						{
							break;
						}
					}
					imagelist[imageid] = info;
					_editorForm->getActiveProxy()->getWidgetParams()->apply (_editorForm->getActiveProxy()->getWidget());
					updateImageList ();
				}
				break;
			}
		case ID_NEW_TEXTURE_IMAGE:
			{
				WidgetParameters::ImageInfo info;
				info.imageType = ATOM_GUIImage::IT_IMAGE;
				info.states[WST_NORMAL].region = ATOM_Rect2Di(0, 0, 0, 0);
				info.states[WST_HOVER].region = ATOM_Rect2Di(0, 0, 0, 0);
				info.states[WST_HOLD].region = ATOM_Rect2Di(0, 0, 0, 0);
				info.states[WST_DISABLED].region = ATOM_Rect2Di(0, 0, 0, 0);
				int imageid = editTextureImage (generateImageId(), &info);
				if (imageid >= 0)
				{
					ATOM_MAP<int,WidgetParameters::ImageInfo> &imagelist = _editorForm->getActiveProxy()->getWidgetParams()->imageList();
					ATOM_MAP<int,WidgetParameters::ImageInfo>::iterator it = imagelist.find (imageid);
					if (it != imagelist.end ())
					{
						if (::MessageBoxA (ATOM_APP->getMainWindow(), "指定标识的图像已经存在，是否替换?", "ATOM3D编辑器", MB_YESNO|MB_ICONQUESTION) == IDNO)
						{
							break;
						}
					}
					imagelist[imageid] = info;
					_editorForm->getActiveProxy()->getWidgetParams()->apply (_editorForm->getActiveProxy()->getWidget());
					updateImageList ();
				}
				break;
			}
		case ID_NEW_TEXTURE_IMAGE9:
			{
				WidgetParameters::ImageInfo info;
				info.imageType = ATOM_GUIImage::IT_IMAGE9;
				info.states[WST_NORMAL].region = ATOM_Rect2Di(0, 0, 0, 0);
				info.states[WST_HOVER].region = ATOM_Rect2Di(0, 0, 0, 0);
				info.states[WST_HOLD].region = ATOM_Rect2Di(0, 0, 0, 0);
				info.states[WST_DISABLED].region = ATOM_Rect2Di(0, 0, 0, 0);
				int imageid = editTextureImage (generateImageId(), &info);
				if (imageid >= 0)
				{
					ATOM_MAP<int,WidgetParameters::ImageInfo> &imagelist = _editorForm->getActiveProxy()->getWidgetParams()->imageList();
					ATOM_MAP<int,WidgetParameters::ImageInfo>::iterator it = imagelist.find (imageid);
					if (it != imagelist.end ())
					{
						if (::MessageBoxA (ATOM_APP->getMainWindow(), "指定标识的图像已经存在，是否替换?", "ATOM3D编辑器", MB_YESNO|MB_ICONQUESTION) == IDNO)
						{
							break;
						}
					}
					imagelist[imageid] = info;
					_editorForm->getActiveProxy()->getWidgetParams()->apply (_editorForm->getActiveProxy()->getWidget());
					updateImageList ();
				}
				break;
			}
		case ID_EDIT_IMAGE:
			{
				int sel = _imageList->getSelectIndex ();
				if (sel >= 0)
				{
					const char *t = _imageList->getItemText (sel)->getString();
					int imageid = atoi (t);
					ATOM_MAP<int,WidgetParameters::ImageInfo> &imagelist = _editorForm->getActiveProxy()->getWidgetParams()->imageList();
					ATOM_MAP<int,WidgetParameters::ImageInfo>::iterator it = imagelist.find (imageid);
					ATOM_ASSERT(it != imagelist.end ());
					WidgetParameters::ImageInfo info = it->second;
					int editResult = -1;
					switch (info.imageType)
					{
					case ATOM_GUIImage::IT_COLOR:
						editResult = editColorImage (imageid, &info);
						break;
					case ATOM_GUIImage::IT_MATERIAL:
						editResult = editMaterialImage (imageid, &info);
						break;
					case ATOM_GUIImage::IT_IMAGE:
					case ATOM_GUIImage::IT_IMAGE9:
						editResult = editTextureImage (imageid, &info);
						break;
					}
					if (editResult >= 0)
					{
						if (editResult != imageid)
						{
							imagelist.erase (it);
							imagelist[editResult] = info;
						}
						else
						{
							it->second = info;
						}
						_editorForm->getActiveProxy()->getWidgetParams()->apply (_editorForm->getActiveProxy()->getWidget());
						updateImageList ();
					}
				}
				break;
			}
		case ID_DELETE_IMAGE:
			int sel = _imageList->getSelectIndex ();
			if (sel >= 0)
			{
				const char *t = _imageList->getItemText (sel)->getString();
				int imageid = atoi (t);
				ATOM_MAP<int,WidgetParameters::ImageInfo> &imagelist = _editorForm->getActiveProxy()->getWidgetParams()->imageList();
				ATOM_MAP<int,WidgetParameters::ImageInfo>::iterator it = imagelist.find (imageid);
				ATOM_ASSERT(it != imagelist.end ());
				imagelist.erase (it);
				_editorForm->getActiveProxy()->getWidgetParams()->apply (_editorForm->getActiveProxy()->getWidget());
				updateImageList ();
			}
			break;
		}
	}
}

void PluginGUI::calcUILayout (void)
{
	ATOM_Rect2Di rc = _editor->getRealtimeCtrl()->getClientRect();
	_uiHerichy->resize (ATOM_Rect2Di(0, 0, 250, rc.size.h / 3));
	_uiHerichy->setScrollValue (ATOM_Point2Di(0, 0));

	ATOM_Point2Di pt(0, rc.size.h / 3);
	_editor->getRealtimeCtrl()->clientToGUI (&pt);
	_toolbox->setBarPosition (pt.x, pt.y);
	_toolbox->setBarSize (250, rc.size.h / 3);
	_toolbox->setBarResizable (false);
	_toolbox->setBarMovable (false);
	_toolbox->setBarIconifiable (false);

	pt.x = 0;
	pt.y = rc.size.h / 3 + rc.size.h / 3;
	_editor->getRealtimeCtrl()->clientToGUI (&pt);
	_fontList->setBarPosition (pt.x, pt.y);
	_fontList->setBarSize (250, rc.size.h - rc.size.h / 3 - rc.size.h / 3);
	_fontList->setBarResizable (false);
	_fontList->setBarMovable (false);
	_fontList->setBarIconifiable (false);

	pt.x = rc.size.w - 250;
	pt.y = 0;
	_editor->getRealtimeCtrl()->clientToGUI (&pt);
	_widgetProp->setBarPosition (pt.x, pt.y);
	_widgetProp->setBarSize (250, rc.size.h - 400);
	_widgetProp->setBarResizable (false);
	_widgetProp->setBarMovable (false);
	_widgetProp->setBarIconifiable (false);

	_customProp->setBarPosition (pt.x, pt.y + rc.size.h - 400);
	_customProp->setBarSize (250, 200);
	_customProp->setBarResizable (false);
	_customProp->setBarMovable (false);
	_customProp->setBarIconifiable (false);

	_imageList->resize (ATOM_Rect2Di(rc.size.w - 250, rc.size.h - 199, 250, 199));
	_imageList->setScrollValue (ATOM_Point2Di(0, 0));
}

bool PluginGUI::saveFile (const char *filename)
{
	bool saveSelectedOnly = false;
	if (_editorForm->getActiveProxy () && IDYES == ::MessageBoxA (ATOM_APP->getMainWindow(), "只保存选择的组件?", "ATOM3D编辑器", MB_YESNO|MB_ICONQUESTION))
	{
		saveSelectedOnly = true;
	}

	if (!_fontInfoMap.empty ())
	{
		if (_fontSetFile.empty ())
		{
			if (1 != _editor->getOpenFileNames ("xml", "XML文件(*.xml)|*.xml|所有文件(*.*)|*.*|", false, true, "保存字体列表文件"))
			{
				return true;
			}
			_fontSetFile = _editor->getOpenedFileName (0);
		}
	}
	if (!_fontSetFile.empty() && !saveFontSet (_fontSetFile.c_str()))
	{
		::MessageBoxA (ATOM_APP->getMainWindow(), "保存字体列表失败!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
		return false;
	}

	char buffer[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (filename, buffer);
	ATOM_TiXmlDocument doc(buffer);

	ATOM_TiXmlDeclaration eDecl("1.0", "gb2312", "");
	doc.InsertEndChild (eDecl);

	ATOM_TiXmlElement eRoot("UI");
	eRoot.SetAttribute ("version", ATOM3D_UI_VERSION);

	if (!_fontSetFile.empty ())
	{
		eRoot.SetAttribute ("fonts", _fontSetFile.c_str());
	}

	//--- wangjian added ---//
	eRoot.SetAttribute ("asyncload", _asyncload ? 5000 : -1);
	//----------------------//

	if (!(saveSelectedOnly ? saveSelected (&eRoot) : saveAll (&eRoot)))
	{
		return false;
	}

	doc.InsertEndChild (eRoot);

	return doc.SaveFile ();
}

unsigned PluginGUI::getMinWindowWidth (void) const
{
	return 600;
}

unsigned PluginGUI::getMinWindowHeight (void) const
{
	return 400;
}

bool PluginGUI::isDocumentModified (void)
{
	return false;
}

bool PluginGUI::loadFontSet (const char *filename)
{
	for (ATOM_HashMap<ATOM_STRING, FontInfo>::iterator it = _fontInfoMap.begin(); it != _fontInfoMap.end(); ++it)
	{
		ATOM_GUIFont::releaseFont (it->second.handle);
	}
	_fontInfoMap.clear ();

	ATOM_AutoFile f(filename, ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
	{
		return false;
	}
	unsigned size = f->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = f->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument doc;
	doc.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (doc.Error())
	{
		return false;
	}

	ATOM_TiXmlElement *root = doc.RootElement ();
	if (!root)
	{
		return false;
	}

	for (ATOM_TiXmlElement *font = root->FirstChildElement ("font"); font; font = font->NextSiblingElement("font"))
	{
		const char *comment = 0;

		ATOM_TiXmlNode *nextNode = font->NextSibling ();
		if (nextNode)
		{
			ATOM_TiXmlComment *c = nextNode->ToComment ();
			if (c)
			{
				comment = c->Value ();
			}
		}

		const char *name = font->Attribute ("name");
		if (!name)
		{
			continue;
		}

		const char *file = font->Attribute ("file");
		if (!file)
		{
			continue;
		}

		int size = 0;
		font->QueryIntAttribute ("size", &size);

		int charset = ATOM_CC_CP936;
		font->QueryIntAttribute ("set", &charset);

		int margin = 0;
		font->QueryIntAttribute ("margin", &margin);

		_fontInfoMap[name].handle = createFont (file, size, charset, margin);
		_fontInfoMap[name].name = name;
		_fontInfoMap[name].filename = file;
		_fontInfoMap[name].size = size;
		_fontInfoMap[name].charset = charset;
		_fontInfoMap[name].margin = margin;
		_fontInfoMap[name].comment = comment ? comment : "";
	}

	updateFontList ();

	return true;
}

ATOM_GUIFont::handle PluginGUI::createFont (const char *fontFilename, int size, int charset, int margin)
{
	char completedPath[ATOM_VFS::max_filename_length];
	ATOM_CompletePath (fontFilename, completedPath);
	strlwr (completedPath);
	bool fontValid = true;

	if (_installedFonts.find (completedPath) == _installedFonts.end ())
	{
		if (ATOM_LoadFont (completedPath, completedPath))
		{
			_installedFonts.insert (completedPath);
		}
		else
		{
			fontValid = false;
		}
	}
	ATOM_GUIFont::handle guiFont = fontValid ? ATOM_GUIFont::createFont (completedPath, size,  charset, margin) : ATOM_GUIFont::getDefaultFont(size, margin);
	return guiFont;
}

bool PluginGUI::saveFontSet (const char *filename) const
{
	char nativeFileName[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (filename, nativeFileName);
	ATOM_TiXmlDocument doc (nativeFileName);

	ATOM_TiXmlDeclaration eDecl("1.0", "gb2312", "");
	doc.InsertEndChild (eDecl);

	ATOM_TiXmlElement root("root");

	for (ATOM_HashMap<ATOM_STRING, FontInfo>::const_iterator it = _fontInfoMap.begin(); it != _fontInfoMap.end(); ++it)
	{
		ATOM_TiXmlElement font("font");
		font.SetAttribute ("name", it->second.name.c_str());
		font.SetAttribute ("file", it->second.filename.c_str());
		font.SetAttribute ("size", it->second.size);
		font.SetAttribute ("set", it->second.charset);
		font.SetAttribute ("margin", it->second.margin);
		root.InsertEndChild (font);

		if (!it->second.comment.empty())
		{
			ATOM_TiXmlComment comment(it->second.comment.c_str());
			root.InsertEndChild (comment);
		}
	}

	doc.InsertEndChild (root);
	return doc.SaveFile ();
}

PluginGUI::FontInfoMap *PluginGUI::getFontInfoMap (void)
{
	return &_fontInfoMap;
}

const PluginGUI::FontInfoMap *PluginGUI::getFontInfoMap (void) const
{
	return &_fontInfoMap;
}

void ATOMX_CALL PluginGUI::FontFileCallback (void *userData)
{
	if (1 == _pluginGUI->_editor->getOpenFileNames ("ttf", "TrueType字体(*.ttf)|*.ttf|TrueType字体(*.ttc)|*.ttc|所有文件(*.*)|*.*|", false, false))
	{
		const char *fontName = (const char*)userData;
		FontInfoMap::iterator it = _pluginGUI->_fontInfoMap.find (fontName);
		if (it != _pluginGUI->_fontInfoMap.end ())
		{
			it->second.filename = _pluginGUI->_editor->getOpenedFileName (0);
			ATOM_GUIFont::handle newHandle = _pluginGUI->createFont (it->second.filename.c_str(), it->second.size, it->second.charset, it->second.margin);
			if (it->second.handle != ATOM_GUIFont::invalid_handle)
			{
				ATOM_GUIFont::releaseFont (it->second.handle);
			}
			it->second.handle = newHandle;
			_pluginGUI->notifyFontChange (fontName);
		}
	}
}

void ATOMX_CALL PluginGUI::FontDeleteCallback (void *userData)
{
	const char *fontName = (const char*)userData;
	FontInfoMap::iterator it = _pluginGUI->_fontInfoMap.find (fontName);
	if (it != _pluginGUI->_fontInfoMap.end ())
	{
		_pluginGUI->_fontInfoMap.erase (it);
		_pluginGUI->updateFontList ();
		_pluginGUI->notifyFontChange (fontName);
	}
}

void ATOMX_CALL PluginGUI::FontListLoad (void *userData)
{
	if (1 == _pluginGUI->_editor->getOpenFileNames ("xml", "字体列表文件(*.xml)|*.xml|所有文件(*.*)|*.*|", false, false))
	{
		_pluginGUI->_fontSetFile = _pluginGUI->_editor->getOpenedFileName (0);
		_pluginGUI->loadFontSet (_pluginGUI->_fontSetFile.c_str());
		_pluginGUI->notifyFontChange (0);
	}
}

void ATOMX_CALL PluginGUI::FontListSave (void *userData)
{
	if (_pluginGUI->_fontSetFile.empty ())
	{
		FontListSaveAs (userData);
	}
	else
	{
		_pluginGUI->saveFontSet (_pluginGUI->_fontSetFile.c_str());
	}
}

void ATOMX_CALL PluginGUI::FontListSaveAs (void *userData)
{
	if (1 == _pluginGUI->_editor->getOpenFileNames ("xml", "字体列表文件(*.xml)|*.xml|所有文件(*.*)|*.*|", false, true))
	{
		_pluginGUI->_fontSetFile = _pluginGUI->_editor->getOpenedFileName (0);
		FontListSave (userData);
	}
}

void ATOMX_CALL PluginGUI::FontListClear (void *userData)
{
	_pluginGUI->_fontInfoMap.clear ();
	_pluginGUI->_installedFonts.clear ();
	_pluginGUI->updateFontList ();
	_pluginGUI->notifyFontChange (0);
}

void ATOMX_CALL PluginGUI::FontListNew (void *userData)
{
	ATOM_STRING s = _pluginGUI->showNewFontDialog ();
	if (!s.empty ())
	{
		_pluginGUI->_fontInfoMap[s].charset = ATOM_CC_CP936;
		_pluginGUI->_fontInfoMap[s].filename = "";
		_pluginGUI->_fontInfoMap[s].handle = ATOM_GUIFont::invalid_handle;
		_pluginGUI->_fontInfoMap[s].margin = 0;
		_pluginGUI->_fontInfoMap[s].name = s;
		_pluginGUI->_fontInfoMap[s].size = 12;
		_pluginGUI->updateFontList ();
		_pluginGUI->notifyFontChange (s.c_str());
	}
}

void ATOMX_CALL PluginGUI::ToolboxCreateWidget (void *userData)
{
	static char filename[ATOM_VFS::max_filename_length];
	_pluginGUI->_creationParam = 0;

	int widgetType = (int)userData;

	if (widgetType == WT_COMPONENT)
	{
		if (1 == _pluginGUI->_editor->getOpenFileNames ("ui", "UI文件(*.ui)|*.ui|", false, false, "选择组件"))
		{
			strcpy (filename, _pluginGUI->_editor->getOpenedFileName (0));
			_pluginGUI->_creationParam = filename;
		}
	}

	ATOM_Point2Di pt = _pluginGUI->_editor->getGUIRenderer()->getMousePosition ();
	_pluginGUI->_editorForm->endCreateWidget ();
	_pluginGUI->_editorForm->screenToClient (&pt);
	_pluginGUI->_editorForm->beginCreateWidget (widgetType, pt.x, pt.y);
}

ATOMX_TweakBar *PluginGUI::getWidgetPropertyBar (void) const
{
	return _widgetProp;
}

AS_Editor *PluginGUI::getEditor (void) const
{
	return _editor;
}

static inline ATOM_ColorARGB chooseColor (ATOM_ColorARGB initValue)
{
	static COLORREF custColors[16] = {
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255)
	};

	CHOOSECOLOR cc;
	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = ATOM_APP->getMainWindow();
	cc.hInstance = NULL;
	cc.rgbResult = RGB(initValue.getByteR(), initValue.getByteG(), initValue.getByteB());
	cc.lpCustColors = custColors;
	cc.Flags = CC_FULLOPEN|CC_RGBINIT;
	cc.lCustData = NULL;
	cc.lpfnHook = NULL;
	cc.lpTemplateName = NULL;
	if (::ChooseColor (&cc))
	{
		ATOM_ColorARGB color(
			float(GetRValue(cc.rgbResult))/255.f,
			float(GetGValue(cc.rgbResult))/255.f,
			float(GetBValue(cc.rgbResult))/255.f,
			1.f);

		return color;
	}
	return initValue;
}

class ColorImageEditTrigger: public ATOM_EventTrigger
{
public:
	ATOM_GUIImageList *il;

public:
	void onCommand (ATOM_WidgetCommandEvent *event)
	{
		switch (event->id)
		{
		case ID_IMAGE_COLOR_NORMAL:
		case ID_IMAGE_COLOR_HOVER:
		case ID_IMAGE_COLOR_HOLD:
		case ID_IMAGE_COLOR_DISABLED:
			{
				int n = event->id - ID_IMAGE_COLOR_NORMAL;
				ATOM_ColorARGB color = chooseColor (il->getImage(n)->getColor(WST_NORMAL));
				il->getImage(n)->setColor (WST_NORMAL, color);
				il->getImage(n)->setColor (WST_HOVER, color);
				il->getImage(n)->setColor (WST_HOLD, color);
				il->getImage(n)->setColor (WST_DISABLED, color);
				break;
			}
		}
	}

	ATOM_DECLARE_EVENT_MAP(ColorImageEditTrigger, ATOM_EventTrigger)
};

ATOM_BEGIN_EVENT_MAP(ColorImageEditTrigger, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(ColorImageEditTrigger, ATOM_WidgetCommandEvent, onCommand)
ATOM_END_EVENT_MAP

int PluginGUI::editColorImage (int id, WidgetParameters::ImageInfo *image)
{
	int ret = -1;

	const ATOM_Rect2Di &rc = _editor->getRealtimeCtrl()->getClientRect ();
	const int dialogWidth = 300;
	const int dialogHeight = 180;
	const int x = (rc.size.w - dialogWidth) / 2;
	const int y = (rc.size.h - dialogHeight) / 2;

	ATOM_AUTOPTR(ATOM_GUIImageList) il = ATOM_NEW(ATOM_GUIImageList);
	il->newColorImage (0, image->states[WST_NORMAL].color, image->states[WST_NORMAL].color, image->states[WST_NORMAL].color, image->states[WST_NORMAL].color);
	il->newColorImage (1, image->states[WST_HOVER].color, image->states[WST_HOVER].color, image->states[WST_HOVER].color, image->states[WST_HOVER].color);
	il->newColorImage (2, image->states[WST_HOLD].color, image->states[WST_HOLD].color, image->states[WST_HOLD].color, image->states[WST_HOLD].color);
	il->newColorImage (3, image->states[WST_DISABLED].color, image->states[WST_DISABLED].color, image->states[WST_DISABLED].color, image->states[WST_DISABLED].color);

	ColorImageEditTrigger trigger;
	trigger.il = il.get();

	ATOM_Dialog *dialog = ATOM_NEW(ATOM_Dialog, _editor->getRealtimeCtrl(), ATOM_Rect2Di(x, y, dialogWidth, dialogHeight));
	dialog->setEventTrigger (&trigger);
	dialog->setLayoutType (ATOM_WidgetLayout::Vertical);
	dialog->getLayout()->setGap (6);
	dialog->getLayout()->setInnerSpaceLeft(8);
	dialog->getLayout()->setInnerSpaceTop(8);
	dialog->getLayout()->setInnerSpaceRight(8);
	dialog->getLayout()->setInnerSpaceBottom(8);
	dialog->addEndId (ID_DIALOG_OK);
	dialog->addEndId (ID_DIALOG_CANCEL);

	ATOM_Panel *idPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	idPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	idPanel->enableEventTransition (true);
	idPanel->setClientImageId (-1);
	ATOM_Label *txt = ATOM_NEW(ATOM_Label, idPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	txt->setText ("Id: ");
	ATOM_Edit *edit = ATOM_NEW(ATOM_Edit, idPanel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Border|ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	edit->setBorderMode (ATOM_Widget::Drop);
	edit->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	char buffer[32];
	sprintf (buffer, "%d", id);
	edit->setString (buffer); 

	ATOM_Panel *normalPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	normalPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	normalPanel->enableEventTransition (true);
	normalPanel->setClientImageId (-1);
	ATOM_Label *normalDesc = ATOM_NEW(ATOM_Label, normalPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	normalDesc->setAlign (ATOM_Widget::AlignX_Left);
	normalDesc->setText ("正常:");
	ATOM_Label *normalColor = ATOM_NEW(ATOM_Label, normalPanel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Border|ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	normalColor->setClientImageId (0);
	normalColor->setImageList (il.get());
	ATOM_Button *normalButton = ATOM_NEW(ATOM_Button, normalPanel, ATOM_Rect2Di(0, 0, 24, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_IMAGE_COLOR_NORMAL, ATOM_Widget::ShowNormal);
	normalButton->setText ("...");

	ATOM_Panel *hoverPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	hoverPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	hoverPanel->enableEventTransition (true);
	hoverPanel->setClientImageId (-1);
	ATOM_Label *hoverDesc = ATOM_NEW(ATOM_Label, hoverPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	hoverDesc->setAlign (ATOM_Widget::AlignX_Left);
	hoverDesc->setText ("高亮:");
	ATOM_Label *hoverColor = ATOM_NEW(ATOM_Label, hoverPanel, ATOM_Rect2Di(0, 0, -80, -100), ATOM_Widget::Border|ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	hoverColor->setClientImageId (1);
	hoverColor->setImageList (il.get());
	ATOM_Button *hoverButton = ATOM_NEW(ATOM_Button, hoverPanel, ATOM_Rect2Di(0, 0, 24, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_IMAGE_COLOR_HOVER, ATOM_Widget::ShowNormal);
	hoverButton->setText ("...");

	ATOM_Panel *holdPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	holdPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	holdPanel->enableEventTransition (true);
	holdPanel->setClientImageId (-1);
	ATOM_Label *holdDesc = ATOM_NEW(ATOM_Label, holdPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	holdDesc->setAlign (ATOM_Widget::AlignX_Left);
	holdDesc->setText ("按下:");
	ATOM_Label *holdColor = ATOM_NEW(ATOM_Label, holdPanel, ATOM_Rect2Di(0, 0, -80, -100), ATOM_Widget::Border|ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	holdColor->setClientImageId (2);
	holdColor->setImageList (il.get());
	ATOM_Button *holdButton = ATOM_NEW(ATOM_Button, holdPanel, ATOM_Rect2Di(0, 0, 24, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_IMAGE_COLOR_HOLD, ATOM_Widget::ShowNormal);
	holdButton->setText ("...");

	ATOM_Panel *disabledPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	disabledPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	disabledPanel->enableEventTransition (true);
	disabledPanel->setClientImageId (-1);
	ATOM_Label *disabledDesc = ATOM_NEW(ATOM_Label, disabledPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	disabledDesc->setAlign (ATOM_Widget::AlignX_Left);
	disabledDesc->setText ("禁止:");
	ATOM_Label *disabledColor = ATOM_NEW(ATOM_Label, disabledPanel, ATOM_Rect2Di(0, 0, -80, -100), ATOM_Widget::Border|ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	disabledColor->setClientImageId (3);
	disabledColor->setImageList (il.get());
	ATOM_Button *disabledButton = ATOM_NEW(ATOM_Button, disabledPanel, ATOM_Rect2Di(0, 0, 24, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_IMAGE_COLOR_DISABLED, ATOM_Widget::ShowNormal);
	disabledButton->setText ("...");

	ATOM_Panel *buttonPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	buttonPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	buttonPanel->enableEventTransition (true);
	buttonPanel->setClientImageId (-1);
	ATOM_Panel *spacer = ATOM_NEW(ATOM_Panel, buttonPanel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	ATOM_Button *btnOk = ATOM_NEW(ATOM_Button, buttonPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_DIALOG_OK, ATOM_Widget::ShowNormal);
	btnOk->setText ("确定");
	ATOM_Button *btnCancel = ATOM_NEW(ATOM_Button, buttonPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_DIALOG_CANCEL, ATOM_Widget::ShowNormal);
	btnCancel->setText ("取消");

	if (dialog->showModal () == ID_DIALOG_OK)
	{
		ATOM_STRING str;
		edit->getString (str);
		ret = atoi (str.c_str());

		image->states[WST_NORMAL].color = il->getImage(0)->getColor(WST_NORMAL);
		image->states[WST_HOVER].color = il->getImage(1)->getColor(WST_NORMAL);
		image->states[WST_HOLD].color = il->getImage(2)->getColor(WST_NORMAL);
		image->states[WST_DISABLED].color = il->getImage(3)->getColor(WST_NORMAL);
	}

	ATOM_DELETE(dialog);

	return ret;
}

class TextureImageEditTrigger: public ATOM_EventTrigger
{
public:
	ATOM_Edit *filenames[4];
	AS_Editor *editor;

public:
	void onCommand (ATOM_WidgetCommandEvent *event)
	{
		switch (event->id)
		{
		case ID_IMAGE_FILE_NORMAL:
		case ID_IMAGE_FILE_HOVER:
		case ID_IMAGE_FILE_HOLD:
		case ID_IMAGE_FILE_DISABLED:
			{
				if (editor->getOpenImageFileNames (false, false) == 1)
				{
					const char *filename = editor->getOpenedImageFileName (0);
					filenames[event->id - ID_IMAGE_FILE_NORMAL]->setString (filename);
				}
				break;
			}
		}
	}

	ATOM_DECLARE_EVENT_MAP(TextureImageEditTrigger, ATOM_EventTrigger)
};

ATOM_BEGIN_EVENT_MAP(TextureImageEditTrigger, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(TextureImageEditTrigger, ATOM_WidgetCommandEvent, onCommand)
ATOM_END_EVENT_MAP

int PluginGUI::editTextureImage (int id, WidgetParameters::ImageInfo *image)
{
	int ret = -1;
	char buffer[32];

	const ATOM_Rect2Di &rc = _editor->getRealtimeCtrl()->getClientRect ();
	const int dialogWidth = 300;
	const int dialogHeight = 220;
	const int x = (rc.size.w - dialogWidth) / 2;
	const int y = (rc.size.h - dialogHeight) / 2;

	ATOM_Dialog *dialog = ATOM_NEW(ATOM_Dialog, _editor->getRealtimeCtrl(), ATOM_Rect2Di(x, y, dialogWidth, dialogHeight));
	dialog->setLayoutType (ATOM_WidgetLayout::Vertical);
	dialog->getLayout()->setGap (6);
	dialog->getLayout()->setInnerSpaceLeft(8);
	dialog->getLayout()->setInnerSpaceTop(8);
	dialog->getLayout()->setInnerSpaceRight(8);
	dialog->getLayout()->setInnerSpaceBottom(8);
	dialog->addEndId (ID_DIALOG_OK);
	dialog->addEndId (ID_DIALOG_CANCEL);

	ATOM_Panel *idPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	idPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	idPanel->enableEventTransition (true);
	idPanel->setClientImageId (-1);
	ATOM_Label *txt = ATOM_NEW(ATOM_Label, idPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	txt->setText ("Id: ");
	ATOM_Edit *edit = ATOM_NEW(ATOM_Edit, idPanel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Border|ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	edit->setBorderMode (ATOM_Widget::Drop);
	edit->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	sprintf (buffer, "%d", id);
	edit->setString (buffer);

	ATOM_Panel *normalPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	normalPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	normalPanel->enableEventTransition (true);
	normalPanel->setClientImageId (-1);
	ATOM_Label *normalDesc = ATOM_NEW(ATOM_Label, normalPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	normalDesc->setAlign (ATOM_Widget::AlignX_Left);
	normalDesc->setText ("正常:");
	ATOM_Edit *normalFilename = ATOM_NEW(ATOM_Edit, normalPanel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Border|ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	normalFilename->setBorderMode (ATOM_Widget::Drop);
	normalFilename->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	normalFilename->setString (image->states[WST_NORMAL].filename.c_str());
	ATOM_Button *normalButton = ATOM_NEW(ATOM_Button, normalPanel, ATOM_Rect2Di(0, 0, 24, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_IMAGE_FILE_NORMAL, ATOM_Widget::ShowNormal);
	normalButton->setText ("...");

	ATOM_Panel *hoverPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	hoverPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	hoverPanel->enableEventTransition (true);
	hoverPanel->setClientImageId (-1);
	ATOM_Label *hoverDesc = ATOM_NEW(ATOM_Label, hoverPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	hoverDesc->setAlign (ATOM_Widget::AlignX_Left);
	hoverDesc->setText ("高亮:");
	ATOM_Edit *hoverFilename = ATOM_NEW(ATOM_Edit, hoverPanel, ATOM_Rect2Di(0, 0, -80, -100), ATOM_Widget::Border|ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	hoverFilename->setBorderMode (ATOM_Widget::Drop);
	hoverFilename->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	hoverFilename->setString (image->states[WST_HOVER].filename.c_str());
	ATOM_Button *hoverButton = ATOM_NEW(ATOM_Button, hoverPanel, ATOM_Rect2Di(0, 0, 24, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_IMAGE_FILE_HOVER, ATOM_Widget::ShowNormal);
	hoverButton->setText ("...");

	ATOM_Panel *holdPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	holdPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	holdPanel->enableEventTransition (true);
	holdPanel->setClientImageId (-1);
	ATOM_Label *holdDesc = ATOM_NEW(ATOM_Label, holdPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	holdDesc->setAlign (ATOM_Widget::AlignX_Left);
	holdDesc->setText ("按下:");
	ATOM_Edit *holdFilename = ATOM_NEW(ATOM_Edit, holdPanel, ATOM_Rect2Di(0, 0, -80, -100), ATOM_Widget::Border|ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	holdFilename->setBorderMode (ATOM_Widget::Drop);
	holdFilename->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	holdFilename->setString (image->states[WST_HOLD].filename.c_str());
	ATOM_Button *holdButton = ATOM_NEW(ATOM_Button, holdPanel, ATOM_Rect2Di(0, 0, 24, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_IMAGE_FILE_HOLD, ATOM_Widget::ShowNormal);
	holdButton->setText ("...");

	ATOM_Panel *disabledPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	disabledPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	disabledPanel->enableEventTransition (true);
	disabledPanel->setClientImageId (-1);
	ATOM_Label *disabledDesc = ATOM_NEW(ATOM_Label, disabledPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	disabledDesc->setAlign (ATOM_Widget::AlignX_Left);
	disabledDesc->setText ("禁止:");
	ATOM_Edit *disabledFilename = ATOM_NEW(ATOM_Edit, disabledPanel, ATOM_Rect2Di(0, 0, -80, -100), ATOM_Widget::Border|ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	disabledFilename->setBorderMode (ATOM_Widget::Drop);
	disabledFilename->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	disabledFilename->setString (image->states[WST_DISABLED].filename.c_str());
	ATOM_Button *disabledButton = ATOM_NEW(ATOM_Button, disabledPanel, ATOM_Rect2Di(0, 0, 24, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_IMAGE_FILE_DISABLED, ATOM_Widget::ShowNormal);
	disabledButton->setText ("...");

	ATOM_Panel *rectPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	rectPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	ATOM_Label *labelX = ATOM_NEW(ATOM_Label, rectPanel, ATOM_Rect2Di(0, 0, 20, -100), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	labelX->setAlign (ATOM_Widget::AlignX_Left);
	labelX->setText ("左:");
	ATOM_Edit *editX = ATOM_NEW(ATOM_Edit, rectPanel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Control|ATOM_Widget::Border, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal); 
	editX->setBorderMode (ATOM_Widget::Drop);
	editX->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	sprintf (buffer, "%d", image->states[WST_NORMAL].region.point.x);
	editX->setString (buffer);
	ATOM_Label *labelY = ATOM_NEW(ATOM_Label, rectPanel, ATOM_Rect2Di(0, 0, 20, -100), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	labelY->setAlign (ATOM_Widget::AlignX_Left);
	labelY->setText ("上:");
	ATOM_Edit *editY = ATOM_NEW(ATOM_Edit, rectPanel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Control|ATOM_Widget::Border, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal); 
	editY->setBorderMode (ATOM_Widget::Drop);
	editY->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	sprintf (buffer, "%d", image->states[WST_NORMAL].region.point.y);
	editY->setString (buffer);
	ATOM_Label *labelW = ATOM_NEW(ATOM_Label, rectPanel, ATOM_Rect2Di(0, 0, 20, -100), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	labelW->setAlign (ATOM_Widget::AlignX_Left);
	labelW->setText ("宽:");
	ATOM_Edit *editW = ATOM_NEW(ATOM_Edit, rectPanel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Control|ATOM_Widget::Border, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal); 
	editW->setBorderMode (ATOM_Widget::Drop);
	editW->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	sprintf (buffer, "%d", image->states[WST_NORMAL].region.size.w);
	editW->setString (buffer);
	ATOM_Label *labelH = ATOM_NEW(ATOM_Label, rectPanel, ATOM_Rect2Di(0, 0, 20, -100), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	labelH->setAlign (ATOM_Widget::AlignX_Left);
	labelH->setText ("高:");
	ATOM_Edit *editH = ATOM_NEW(ATOM_Edit, rectPanel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Control|ATOM_Widget::Border, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal); 
	editH->setBorderMode (ATOM_Widget::Drop);
	editH->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	sprintf (buffer, "%d", image->states[WST_NORMAL].region.size.h);
	editH->setString (buffer);

	ATOM_Panel *buttonPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	buttonPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	buttonPanel->enableEventTransition (true);
	buttonPanel->setClientImageId (-1);
	ATOM_Panel *spacer = ATOM_NEW(ATOM_Panel, buttonPanel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	ATOM_Button *btnOk = ATOM_NEW(ATOM_Button, buttonPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_DIALOG_OK, ATOM_Widget::ShowNormal);
	btnOk->setText ("确定");
	ATOM_Button *btnCancel = ATOM_NEW(ATOM_Button, buttonPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_DIALOG_CANCEL, ATOM_Widget::ShowNormal);
	btnCancel->setText ("取消");

	TextureImageEditTrigger trigger;
	
	trigger.filenames[0] = normalFilename;
	trigger.filenames[1] = hoverFilename;
	trigger.filenames[2] = holdFilename;
	trigger.filenames[3] = disabledFilename;
	trigger.editor = _editor;
	dialog->setEventTrigger (&trigger);

	if (dialog->showModal () == ID_DIALOG_OK)
	{
		ATOM_STRING str;
		edit->getString (str);
		ret = atoi (str.c_str());

		ATOM_Rect2Di region(
			atoi (editX->getString().c_str()),
			atoi (editY->getString().c_str()),
			atoi (editW->getString().c_str()),
			atoi (editH->getString().c_str()));
		
		image->states[WST_NORMAL].filename = normalFilename->getString();
		image->states[WST_NORMAL].region = region;
		image->states[WST_HOVER].filename = hoverFilename->getString();
		image->states[WST_HOVER].region = region;
		image->states[WST_HOLD].filename = holdFilename->getString();
		image->states[WST_HOLD].region = region;
		image->states[WST_DISABLED].filename = disabledFilename->getString();
		image->states[WST_DISABLED].region = region;
	}

	ATOM_DELETE(dialog);

	return ret;
}

class MaterialEditTrigger: public ATOM_EventTrigger
{
public:
	ATOM_Edit *filenames[4];
	ATOM_Edit *matFilenames[4];
	AS_Editor *editor;

public:
	void onCommand (ATOM_WidgetCommandEvent *event)
	{
		switch (event->id)
		{
		case ID_IMAGE_FILE_NORMAL:
		case ID_IMAGE_FILE_HOVER:
		case ID_IMAGE_FILE_HOLD:
		case ID_IMAGE_FILE_DISABLED:
			{
				if (editor->getOpenImageFileNames (false, false) == 1)
				{
					const char *filename = editor->getOpenedImageFileName (0);
					filenames[event->id - ID_IMAGE_FILE_NORMAL]->setString (filename);
				}
				break;
			}
		case ID_MATERIAL_FILE_NORMAL:
		case ID_MATERIAL_FILE_HOVER:
		case ID_MATERIAL_FILE_HOLD:
		case ID_MATERIAL_FILE_DISABLED:
			{
				if (editor->getOpenFileNames ("xml", "材质定义文件(*.xml)|*.xml|", false, false) == 1)
				{
					const char *filename = editor->getOpenedFileName (0);
					matFilenames[event->id - ID_MATERIAL_FILE_NORMAL]->setString (filename);
				}
				break;
			}
		}
	}

	ATOM_DECLARE_EVENT_MAP(MaterialEditTrigger, ATOM_EventTrigger)
};

ATOM_BEGIN_EVENT_MAP(MaterialEditTrigger, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(MaterialEditTrigger, ATOM_WidgetCommandEvent, onCommand)
ATOM_END_EVENT_MAP

int PluginGUI::editMaterialImage (int id, WidgetParameters::ImageInfo *image)
{
	int ret = -1;
	char buffer[32];

	const ATOM_Rect2Di &rc = _editor->getRealtimeCtrl()->getClientRect ();
	const int dialogWidth = 300;
	const int dialogHeight = 450;
	const int x = (rc.size.w - dialogWidth) / 2;
	const int y = (rc.size.h - dialogHeight) / 2;

	ATOM_Dialog *dialog = ATOM_NEW(ATOM_Dialog, _editor->getRealtimeCtrl(), ATOM_Rect2Di(x, y, dialogWidth, dialogHeight));
	dialog->setLayoutType (ATOM_WidgetLayout::Vertical);
	dialog->getLayout()->setGap (6);
	dialog->getLayout()->setInnerSpaceLeft(8);
	dialog->getLayout()->setInnerSpaceTop(8);
	dialog->getLayout()->setInnerSpaceRight(8);
	dialog->getLayout()->setInnerSpaceBottom(8);
	dialog->addEndId (ID_DIALOG_OK);
	dialog->addEndId (ID_DIALOG_CANCEL);

	ATOM_Panel *idPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	idPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	idPanel->enableEventTransition (true);
	idPanel->setClientImageId (-1);
	ATOM_Label *txt = ATOM_NEW(ATOM_Label, idPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	txt->setText ("Id: ");
	ATOM_Edit *edit = ATOM_NEW(ATOM_Edit, idPanel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Border|ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	edit->setBorderMode (ATOM_Widget::Drop);
	edit->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	sprintf (buffer, "%d", id);
	edit->setString (buffer);

	ATOM_Panel *normalPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	normalPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	normalPanel->enableEventTransition (true);
	normalPanel->setClientImageId (-1);
	ATOM_Label *normalDesc = ATOM_NEW(ATOM_Label, normalPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	normalDesc->setAlign (ATOM_Widget::AlignX_Left);
	normalDesc->setText ("正常:");
	ATOM_Edit *normalFilename = ATOM_NEW(ATOM_Edit, normalPanel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Border|ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	normalFilename->setBorderMode (ATOM_Widget::Drop);
	normalFilename->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	normalFilename->setString (image->states[WST_NORMAL].filename.c_str());
	ATOM_Button *normalButton = ATOM_NEW(ATOM_Button, normalPanel, ATOM_Rect2Di(0, 0, 24, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_IMAGE_FILE_NORMAL, ATOM_Widget::ShowNormal);
	normalButton->setText ("...");

	ATOM_Panel *hoverPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	hoverPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	hoverPanel->enableEventTransition (true);
	hoverPanel->setClientImageId (-1);
	ATOM_Label *hoverDesc = ATOM_NEW(ATOM_Label, hoverPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	hoverDesc->setAlign (ATOM_Widget::AlignX_Left);
	hoverDesc->setText ("高亮:");
	ATOM_Edit *hoverFilename = ATOM_NEW(ATOM_Edit, hoverPanel, ATOM_Rect2Di(0, 0, -80, -100), ATOM_Widget::Border|ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	hoverFilename->setBorderMode (ATOM_Widget::Drop);
	hoverFilename->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	hoverFilename->setString (image->states[WST_HOVER].filename.c_str());
	ATOM_Button *hoverButton = ATOM_NEW(ATOM_Button, hoverPanel, ATOM_Rect2Di(0, 0, 24, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_IMAGE_FILE_HOVER, ATOM_Widget::ShowNormal);
	hoverButton->setText ("...");

	ATOM_Panel *holdPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	holdPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	holdPanel->enableEventTransition (true);
	holdPanel->setClientImageId (-1);
	ATOM_Label *holdDesc = ATOM_NEW(ATOM_Label, holdPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	holdDesc->setAlign (ATOM_Widget::AlignX_Left);
	holdDesc->setText ("按下:");
	ATOM_Edit *holdFilename = ATOM_NEW(ATOM_Edit, holdPanel, ATOM_Rect2Di(0, 0, -80, -100), ATOM_Widget::Border|ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	holdFilename->setBorderMode (ATOM_Widget::Drop);
	holdFilename->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	holdFilename->setString (image->states[WST_HOLD].filename.c_str());
	ATOM_Button *holdButton = ATOM_NEW(ATOM_Button, holdPanel, ATOM_Rect2Di(0, 0, 24, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_IMAGE_FILE_HOLD, ATOM_Widget::ShowNormal);
	holdButton->setText ("...");

	ATOM_Panel *disabledPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	disabledPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	disabledPanel->enableEventTransition (true);
	disabledPanel->setClientImageId (-1);
	ATOM_Label *disabledDesc = ATOM_NEW(ATOM_Label, disabledPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	disabledDesc->setAlign (ATOM_Widget::AlignX_Left);
	disabledDesc->setText ("禁止:");
	ATOM_Edit *disabledFilename = ATOM_NEW(ATOM_Edit, disabledPanel, ATOM_Rect2Di(0, 0, -80, -100), ATOM_Widget::Border|ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	disabledFilename->setBorderMode (ATOM_Widget::Drop);
	disabledFilename->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	disabledFilename->setString (image->states[WST_DISABLED].filename.c_str());
	ATOM_Button *disabledButton = ATOM_NEW(ATOM_Button, disabledPanel, ATOM_Rect2Di(0, 0, 24, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_IMAGE_FILE_DISABLED, ATOM_Widget::ShowNormal);
	disabledButton->setText ("...");

	ATOM_Panel *normalMatPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	normalMatPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	normalMatPanel->enableEventTransition (true);
	normalMatPanel->setClientImageId (-1);
	ATOM_Label *normalMatDesc = ATOM_NEW(ATOM_Label, normalMatPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	normalMatDesc->setAlign (ATOM_Widget::AlignX_Left);
	normalMatDesc->setText ("正常材质:");
	ATOM_Edit *normalMatFilename = ATOM_NEW(ATOM_Edit, normalMatPanel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Border|ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	normalMatFilename->setBorderMode (ATOM_Widget::Drop);
	normalMatFilename->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	normalMatFilename->setString (image->states[WST_NORMAL].material_filename.c_str());
	ATOM_Button *normalMatButton = ATOM_NEW(ATOM_Button, normalMatPanel, ATOM_Rect2Di(0, 0, 24, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_MATERIAL_FILE_NORMAL, ATOM_Widget::ShowNormal);
	normalMatButton->setText ("...");

	ATOM_Panel *hoverMatPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	hoverMatPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	hoverMatPanel->enableEventTransition (true);
	hoverMatPanel->setClientImageId (-1);
	ATOM_Label *hoverMatDesc = ATOM_NEW(ATOM_Label, hoverMatPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	hoverMatDesc->setAlign (ATOM_Widget::AlignX_Left);
	hoverMatDesc->setText ("高亮材质:");
	ATOM_Edit *hoverMatFilename = ATOM_NEW(ATOM_Edit, hoverMatPanel, ATOM_Rect2Di(0, 0, -80, -100), ATOM_Widget::Border|ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	hoverMatFilename->setBorderMode (ATOM_Widget::Drop);
	hoverMatFilename->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	hoverMatFilename->setString (image->states[WST_HOVER].material_filename.c_str());
	ATOM_Button *hoverMatButton = ATOM_NEW(ATOM_Button, hoverMatPanel, ATOM_Rect2Di(0, 0, 24, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_MATERIAL_FILE_HOVER, ATOM_Widget::ShowNormal);
	hoverMatButton->setText ("...");

	ATOM_Panel *holdMatPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	holdMatPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	holdMatPanel->enableEventTransition (true);
	holdMatPanel->setClientImageId (-1);
	ATOM_Label *holdMatDesc = ATOM_NEW(ATOM_Label, holdMatPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	holdMatDesc->setAlign (ATOM_Widget::AlignX_Left);
	holdMatDesc->setText ("按下材质:");
	ATOM_Edit *holdMatFilename = ATOM_NEW(ATOM_Edit, holdMatPanel, ATOM_Rect2Di(0, 0, -80, -100), ATOM_Widget::Border|ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	holdMatFilename->setBorderMode (ATOM_Widget::Drop);
	holdMatFilename->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	holdMatFilename->setString (image->states[WST_HOLD].material_filename.c_str());
	ATOM_Button *holdMatButton = ATOM_NEW(ATOM_Button, holdMatPanel, ATOM_Rect2Di(0, 0, 24, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_MATERIAL_FILE_HOLD, ATOM_Widget::ShowNormal);
	holdMatButton->setText ("...");

	ATOM_Panel *disabledMatPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	disabledMatPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	disabledMatPanel->enableEventTransition (true);
	disabledMatPanel->setClientImageId (-1);
	ATOM_Label *disabledMatDesc = ATOM_NEW(ATOM_Label, disabledMatPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	disabledMatDesc->setAlign (ATOM_Widget::AlignX_Left);
	disabledMatDesc->setText ("禁止材质:");
	ATOM_Edit *disabledMatFilename = ATOM_NEW(ATOM_Edit, disabledMatPanel, ATOM_Rect2Di(0, 0, -80, -100), ATOM_Widget::Border|ATOM_Widget::Control, 0, ATOM_Widget::ShowNormal);
	disabledMatFilename->setBorderMode (ATOM_Widget::Drop);
	disabledMatFilename->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	disabledMatFilename->setString (image->states[WST_DISABLED].material_filename.c_str());
	ATOM_Button *disabledMatButton = ATOM_NEW(ATOM_Button, disabledMatPanel, ATOM_Rect2Di(0, 0, 24, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_MATERIAL_FILE_DISABLED, ATOM_Widget::ShowNormal);
	disabledMatButton->setText ("...");

	ATOM_Panel *rectPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	rectPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	ATOM_Label *labelX = ATOM_NEW(ATOM_Label, rectPanel, ATOM_Rect2Di(0, 0, 20, -100), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	labelX->setAlign (ATOM_Widget::AlignX_Left);
	labelX->setText ("左:");
	ATOM_Edit *editX = ATOM_NEW(ATOM_Edit, rectPanel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Control|ATOM_Widget::Border, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal); 
	editX->setBorderMode (ATOM_Widget::Drop);
	editX->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	sprintf (buffer, "%d", image->states[WST_NORMAL].region.point.x);
	editX->setString (buffer);
	ATOM_Label *labelY = ATOM_NEW(ATOM_Label, rectPanel, ATOM_Rect2Di(0, 0, 20, -100), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	labelY->setAlign (ATOM_Widget::AlignX_Left);
	labelY->setText ("上:");
	ATOM_Edit *editY = ATOM_NEW(ATOM_Edit, rectPanel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Control|ATOM_Widget::Border, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal); 
	editY->setBorderMode (ATOM_Widget::Drop);
	editY->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	sprintf (buffer, "%d", image->states[WST_NORMAL].region.point.y);
	editY->setString (buffer);
	ATOM_Label *labelW = ATOM_NEW(ATOM_Label, rectPanel, ATOM_Rect2Di(0, 0, 20, -100), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	labelW->setAlign (ATOM_Widget::AlignX_Left);
	labelW->setText ("宽:");
	ATOM_Edit *editW = ATOM_NEW(ATOM_Edit, rectPanel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Control|ATOM_Widget::Border, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal); 
	editW->setBorderMode (ATOM_Widget::Drop);
	editW->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	sprintf (buffer, "%d", image->states[WST_NORMAL].region.size.w);
	editW->setString (buffer);
	ATOM_Label *labelH = ATOM_NEW(ATOM_Label, rectPanel, ATOM_Rect2Di(0, 0, 20, -100), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	labelH->setAlign (ATOM_Widget::AlignX_Left);
	labelH->setText ("高:");
	ATOM_Edit *editH = ATOM_NEW(ATOM_Edit, rectPanel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Control|ATOM_Widget::Border, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal); 
	editH->setBorderMode (ATOM_Widget::Drop);
	editH->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	sprintf (buffer, "%d", image->states[WST_NORMAL].region.size.h);
	editH->setString (buffer);

	ATOM_Panel *buttonPanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	buttonPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	buttonPanel->enableEventTransition (true);
	buttonPanel->setClientImageId (-1);
	ATOM_Panel *spacer = ATOM_NEW(ATOM_Panel, buttonPanel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	ATOM_Button *btnOk = ATOM_NEW(ATOM_Button, buttonPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_DIALOG_OK, ATOM_Widget::ShowNormal);
	btnOk->setText ("确定");
	ATOM_Button *btnCancel = ATOM_NEW(ATOM_Button, buttonPanel, ATOM_Rect2Di(0, 0, 40, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_DIALOG_CANCEL, ATOM_Widget::ShowNormal);
	btnCancel->setText ("取消");

	MaterialEditTrigger trigger;

	trigger.filenames[0] = normalFilename;
	trigger.filenames[1] = hoverFilename;
	trigger.filenames[2] = holdFilename;
	trigger.filenames[3] = disabledFilename;
	trigger.matFilenames[0] = normalMatFilename;
	trigger.matFilenames[1] = hoverMatFilename;
	trigger.matFilenames[2] = holdMatFilename;
	trigger.matFilenames[3] = disabledMatFilename;
	trigger.editor = _editor;
	dialog->setEventTrigger (&trigger);

	if (dialog->showModal () == ID_DIALOG_OK)
	{
		ATOM_STRING str;
		edit->getString (str);
		ret = atoi (str.c_str());

		ATOM_Rect2Di region(
			atoi (editX->getString().c_str()),
			atoi (editY->getString().c_str()),
			atoi (editW->getString().c_str()),
			atoi (editH->getString().c_str()));

		image->states[WST_NORMAL].filename = normalFilename->getString();
		image->states[WST_NORMAL].material_filename = normalMatFilename->getString();
		image->states[WST_NORMAL].region = region;
		image->states[WST_HOVER].filename = hoverFilename->getString();
		image->states[WST_HOVER].material_filename = hoverMatFilename->getString();
		image->states[WST_HOVER].region = region;
		image->states[WST_HOLD].filename = holdFilename->getString();
		image->states[WST_HOLD].material_filename = holdMatFilename->getString();
		image->states[WST_HOLD].region = region;
		image->states[WST_DISABLED].filename = disabledFilename->getString();
		image->states[WST_DISABLED].material_filename = disabledMatFilename->getString();
		image->states[WST_DISABLED].region = region;
	}

	ATOM_DELETE(dialog);

	return ret;
}

bool PluginGUI::saveSelected (ATOM_TiXmlElement *parent)
{
	ControlProxy *proxy = _editorForm->getActiveProxy ();

	ATOM_TiXmlElement e("dummy");
	if (!proxy->saveToXML (&e))
	{
		return false;
	}

	parent->InsertEndChild (e);

	return true;
}

bool PluginGUI::saveAll (ATOM_TiXmlElement *parent)
{
	if (!parent)
	{
		return false;
	}

	for (ATOM_Widget *c = _editorForm->getFirstControl(); c; c = c->getNextSibling ())
	{
		ControlProxy *proxy = ControlProxy::getWidgetProxy (c);
		if (proxy)
		{
			ATOM_TiXmlElement e("dummy");
			if (!proxy->saveToXML (&e))
			{
				return false;
			}
			parent->InsertEndChild (e);
		}
	}

	for (ATOM_Widget *c = _editorForm->getFirstChild(); c; c = c->getNextSibling ())
	{
		ControlProxy *proxy = ControlProxy::getWidgetProxy (c);
		if (proxy)
		{
			ATOM_TiXmlElement e("dummy");
			if (!proxy->saveToXML (&e))
			{
				return false;
			}
			parent->InsertEndChild (e);
		}
	}

	return true;
}

bool PluginGUI::load (ATOM_TiXmlElement *root)
{
	for (ATOM_TiXmlElement *child = root->FirstChildElement("component"); child; child = child->NextSiblingElement("component"))
	{
		ControlProxy *proxy = ControlProxy::createProxyFromXML (this, _editorForm, child);
		if (!proxy)
		{
			return false;
		}
	}
	updateWidgetTree (true);

	return true;
}

void PluginGUI::showThumb (bool show)
{
	ATOM_DELETE(_thumb);
	_thumb = 0;

	if (show)
	{
		const int thumbW = 200;
		const int thumbH = 200;
		ATOM_Rect2Di rc = _editor->getRealtimeCtrl()->getClientRect();
		rc.point.x = rc.size.w - _imageList->getWidgetRect().size.w - thumbW;
		rc.point.y = rc.size.h - _imageList->getWidgetRect().size.h;
		rc.size.w = thumbW;
		rc.size.h = thumbH;

		_thumb = ATOM_NEW(ATOM_TopWindow, _editor->getRealtimeCtrl(), rc, ATOM_Widget::Border|ATOM_Widget::TopMost|ATOM_Widget::NonLayoutable|ATOM_Widget::NonScrollable, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
		_thumb->setStyle(_thumb->getStyle() & ~ATOM_Widget::TitleBar);
		_thumb->setBorderMode (ATOM_Widget::Raise);
		_thumb->setImageList (_pluginImages.get());
		_thumb->setClientImageId (IMAGEID_THUMB);
		_thumb->bringToFront ();
	}
}

ATOM_Widget *PluginGUI::getUIRoot (void) const
{
	return _editorForm;
}

void PluginGUI::setupMenu (void)
{
	ATOM_MenuBar *menubar = _editor->getMenuBar ();

	ATOM_PopupMenu *menuEdit = menubar->getMenuItem (AS_MENUITEM_EDIT).submenu;
	menuEdit->appendMenuItem ("预览模式 (F5)", ID_GUI_PREVIEW);
	menuEdit->appendMenuItem ("自动生成控件ID", ID_GUI_GENERATE_IDS);
	menuEdit->appendMenuItem ("输出源文件", ID_GUI_EXPORT_SOURCE);

	ATOM_PopupMenu *menuView = menubar->getMenuItem (AS_MENUITEM_VIEW).submenu;
	menuView->appendMenuItem ("切换界面显示(F11)", ID_GUI_TOGGLE_GUI);

	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_F5, 0, ID_GUI_PREVIEW));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_F11, 0, ID_GUI_TOGGLE_GUI));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_DELETE, 0, ID_GUI_DELETE));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_d, KEYMOD_CTRL, ID_GUI_CLONE));
}

void PluginGUI::cleanupMenu (void)
{
	ATOM_MenuBar *menubar = _editor->getMenuBar ();
	menubar->getMenuItem(AS_MENUITEM_EDIT).submenu->removeMenuItemById (ID_GUI_PREVIEW);
	menubar->getMenuItem(AS_MENUITEM_EDIT).submenu->removeMenuItemById (ID_GUI_GENERATE_IDS);
	menubar->getMenuItem(AS_MENUITEM_EDIT).submenu->removeMenuItemById (ID_GUI_EXPORT_SOURCE);
	menubar->getMenuItem(AS_MENUITEM_VIEW).submenu->removeMenuItemById (ID_GUI_TOGGLE_GUI);

	for (unsigned i = 0; i < _accelKeys.size(); ++i)
	{
		_editor->unregisterAccelKeyCommand (_accelKeys[i]);
	}
	_accelKeys.clear ();
}

void PluginGUI::showGUI (bool show)
{
	if (!show)
	{
		_uiHerichy->show (ATOM_Widget::Hide);
		_imageList->show (ATOM_Widget::Hide);
		showThumb (false);
		_fontList->setBarVisible (false);
		_widgetProp->setBarVisible (false);
		_toolbox->setBarVisible (false);
		_editorForm->setFocus ();
	}
	else
	{
		_uiHerichy->show (ATOM_Widget::ShowNormal);
		_imageList->show (ATOM_Widget::ShowNormal);
		_fontList->setBarVisible (true);
		_widgetProp->setBarVisible (true);
		_toolbox->setBarVisible (true);
		calcUILayout ();
	}
}

void PluginGUI::toggleGUI (void)
{
	_guiShown = !_guiShown;

	showGUI (_guiShown);
}

void PluginGUI::generateControlIDs (void)
{
	ControlProxy *proxy = _editorForm->getActiveProxy ();
	_editorForm->setActiveProxy (0);
	generateControlIDs_R (0);
	_editorForm->setActiveProxy (proxy);
}

void PluginGUI::generateControlIDs_R (ControlProxy *proxy)
{
	ATOM_Widget *parent = _editorForm;

	if (proxy)
	{
		parent = proxy->getWidget();

		if (proxy->getWidgetParams()->interactive() && proxy->getWidgetParams()->getId() == -1)
		{
			proxy->getWidgetParams()->setId (IdManager::peekId (100));
			proxy->getWidgetParams()->apply (parent);
		}
	}

	for (ATOM_Widget *c = parent->getFirstChild(); c; c = c->getNextSibling ())
	{
		ControlProxy *childProxy = ControlProxy::getWidgetProxy (c);
		if (childProxy)
		{
			generateControlIDs_R (childProxy);
		}
	}

	for (ATOM_Widget *c = parent->getFirstControl(); c; c = c->getNextSibling ())
	{
		ControlProxy *childProxy = ControlProxy::getWidgetProxy (c);
		if (childProxy)
		{
			generateControlIDs_R (childProxy);
		}
	}
}

static void generateCppNames (const char *className, const char *proxyName, ATOM_STRING &IDName, ATOM_STRING &VarName)
{
	char name[256];
	strcpy (name, proxyName);
	int len = strlen(name);

	if (!isalpha(name[0]))
	{
		name[0] = '_';
	}

	for (int n = 1; n < len; ++n)
	{
		if (!isalpha(name[n]) && !isdigit(name[n]) && name[n]!='_' && name[n]!='[' && name[n]!=']')
		{
			name[n] = '_';
		}
	}

	VarName = "m_";
	VarName += name;

	for (int n = 1; n < len; ++n)
	{
		if (name[n]=='[' || name[n]==']')
		{
			name[n] = '_';
		}
	}
	strupr (name);
	IDName = "ID_";
	IDName += name;
}

static void replaceText (ATOM_STRING &srcStr, const ATOM_STRING &markBegin, const ATOM_STRING &markEnd, const ATOM_STRING &replacedWith)
{
	ATOM_STRING target;
	const char *src = srcStr.c_str();
	int beginLen = markBegin.length();
	int endLen = markEnd.length();

	for (;;)
	{
		const char *findStart = strstr(src, markBegin.c_str());
		if (!findStart)
		{
			target += src;
			break;
		}

		const char *findEnd = strstr(findStart+beginLen, markEnd.c_str());
		if (!findEnd)
		{
			target += src;
			break;
		}

		target += ATOM_STRING(src, findStart+beginLen-src);
		target += replacedWith;
		target += markEnd;

		src = findEnd + endLen;
	}

	srcStr = target;
}

bool chooseFile (ATOM_STRING &result)
{
	OPENFILENAMEA ofn;
	char fileNameBuffer[MAX_PATH];

	strcpy (fileNameBuffer, result.c_str());

	memset (&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = ATOM_APP->getMainWindow();
	ofn.lpstrFile = fileNameBuffer;
	ofn.nMaxFile = sizeof(fileNameBuffer);

	char filterBuffer[MAX_PATH];
	strcpy (filterBuffer, "C++头文件(*.h)");
	char *s2 = filterBuffer+strlen(filterBuffer)+1;
	strcpy (s2, "*.h");
	*(s2 + strlen(s2) + 1) = '\0';

	ofn.lpstrFilter = filterBuffer;
	ofn.Flags = 0;

	if (::GetOpenFileNameA (&ofn))
	{
		result = fileNameBuffer;
		return true;
	}

	return false;
}

static bool PostProcessSource (ATOM_STRING &varList, ATOM_STRING &initList)
{
	
}

bool PluginGUI::exportSourceCode (void)
{
	ATOM_MAP<ATOM_STRING, ArrayInfo> arrayMap;

	ControlProxy *theMainChild = 0;
	for (ATOM_Widget *w = _editorForm->getFirstControl(); w; w = w->getNextSibling())
	{
		ControlProxy *proxy = ControlProxy::getWidgetProxy (w);
		if (proxy)
		{
			if (!theMainChild)
				theMainChild = proxy;
			else
			{
				::MessageBoxA (ATOM_APP->getMainWindow(), "不支持多个顶级控件", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
				return false;
			}
		}
	}
	for (ATOM_Widget *w = _editorForm->getFirstChild(); w; w = w->getNextSibling())
	{
		ControlProxy *proxy = ControlProxy::getWidgetProxy (w);
		if (proxy)
		{
			if (!theMainChild)
				theMainChild = proxy;
			else
			{
				::MessageBoxA (ATOM_APP->getMainWindow(), "不支持多个顶级控件", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
				return false;
			}
		}
	}

	if (!theMainChild)
	{
		::MessageBoxA (ATOM_APP->getMainWindow(), "没有UI控件可供导出!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
		return false;
	}

	ATOM_STRING filename;
	if (chooseFile (filename))
	{
		ATOM_STRING sResult;

		{
			ATOM_AutoFile f(filename.c_str(), ATOM_VFS::read|ATOM_VFS::text);
			if (!f)
			{
				MessageBoxA (ATOM_APP->getMainWindow(), "无法打开文件", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
				return false;
			}

			unsigned size = f->size ();
			char *content = ATOM_NEW_ARRAY(char, size + 1);
			size = f->read (content, size);
			content[size] = '\0';

			if (!generateArrayInfo_R (arrayMap, theMainChild, 0))
			{
				MessageBoxA (ATOM_APP->getMainWindow(), "生成代码失败,可能是数组格式有误,请检查", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
				return false;
			}

			for (ATOM_MAP<ATOM_STRING, ArrayInfo>::const_iterator it = arrayMap.begin(); it != arrayMap.end(); ++it)
			{
				if (it->second.dimension != it->second.elements.size())
				{
					char buffer[256];
					sprintf (buffer, "数组%s *%s[%d]下标不完整", it->second.widgetType.c_str(), it->first.c_str(), it->second.dimension);
					MessageBoxA (ATOM_APP->getMainWindow(), buffer, "ATOM3D编辑器", MB_OK|MB_ICONHAND);
					return false;
				}
			}

			ATOM_STRING idlist;
			ATOM_STRING varlist="\n";
			ATOM_STRING initlist;
			if (!generateSource_R (theMainChild, 0, "\t\t", idlist, "\t", varlist, "\t\t", initlist, "parent"))
			{
				MessageBoxA (ATOM_APP->getMainWindow(), "生成代码失败,可能是数组格式有误,请检查", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
				return false;
			}

			ATOM_STRING idListStr = "\n\tenum {\n" + idlist + "\t};\n";

			ATOM_STRING varListStr = varlist;
			char buffer[200];
			for (ATOM_MAP<ATOM_STRING, ArrayInfo>::const_iterator it = arrayMap.begin(); it != arrayMap.end(); ++it)
			{
				sprintf (buffer, "%s%s *m_%s[%d];\n", "\t", it->second.widgetType.c_str(), it->first.c_str(), it->second.dimension);
				varListStr += buffer;
			}

			ATOM_STRING initFunction = "\n\tvoid initControls(ATOM_Widget *parent) {\n";
			initFunction += initlist;
			initFunction += "\t}\n";

			sResult = content;
			replaceText (sResult, "//%%BeginIDList", "//%%EndIDList", idListStr);
			replaceText (sResult, "//%%BeginVarList", "//%%EndVarList", varListStr);
			replaceText (sResult, "//%%BeginInitFunc", "//%%EndInitFunc", initFunction);

			ATOM_DELETE_ARRAY(content);
		}

		{
			ATOM_AutoFile f2(filename.c_str(), ATOM_VFS::write|ATOM_VFS::text);
			if (!f2)
			{
				MessageBoxA (ATOM_APP->getMainWindow(), "无法写入文件", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
				return false;
			}
			f2->write (sResult.c_str(), sResult.length());
		}
	}

	return true;
}

bool PluginGUI::generateArrayInfo_R (ATOM_MAP<ATOM_STRING, ArrayInfo> &arrayMap, ControlProxy *rootProxy, ControlProxy *proxy)
{
	ATOM_Widget *parent = _editorForm;

	if (proxy)
	{
		parent = proxy->getWidget();
		int id = proxy->getWidgetParams()->getId();
		if (id != -1 && proxy != rootProxy)
		{
			const char *proxyName = proxy->getName();
			const char *p = strchr (proxyName, '[');
			if (p)
			{
				const char *q = strrchr (proxyName, ']');
				if (!q || q[1] != '\0' || q <= p+1)
				{
					return false;
				}

				for (const char *ch = p+1; ch != q; ++ch)
				{
					if (!isdigit(*ch))
					{
						return false;
					}
				}

				char buffer[100];
				int len = p - proxyName;
				memcpy (buffer, proxyName, len);
				buffer[len] = '\0';

				int index = -1;
				sscanf (p, "[%d]", &index);
				if (index >= 0)
				{
					ATOM_MAP<ATOM_STRING, ArrayInfo>::iterator it = arrayMap.find (buffer);
					if (it == arrayMap.end ())
					{
						// 发现新的控件数组

						// 扩展数组大小
						arrayMap[buffer].dimension = index + 1;

						// 记录下标
						arrayMap[buffer].elements.insert(index);

						// 记录控件类型
						arrayMap[buffer].widgetType = proxy->getWidgetClassName();
					}
					else
					{
						// 已有的控件数组 

						//检查控件类型是否一致
						if (it->second.widgetType != proxy->getWidgetClassName())
						{
							//不一致
							return false;
						}

						// 检查下标
						if (it->second.elements.find(index) != it->second.elements.end())
						{
							//下标重复
							return false;
						}
						else
						{
							// 下标无误,记录此下标
							it->second.elements.insert (index);
						}

						// 扩展数组大小
						if (it->second.dimension < index + 1)
						{
							it->second.dimension = index + 1;
						}
					}
				}
				else
				{
					return false;
				}
			}
		}
	}

	for (ATOM_Widget *c = parent->getFirstChild(); c; c = c->getNextSibling ())
	{
		ControlProxy *childProxy = ControlProxy::getWidgetProxy (c);
		if (childProxy)
		{
			if (!generateArrayInfo_R (arrayMap, rootProxy, childProxy))
			{
				return false;
			}
		}
	}

	for (ATOM_Widget *c = parent->getFirstControl(); c; c = c->getNextSibling ())
	{
		ControlProxy *childProxy = ControlProxy::getWidgetProxy (c);
		if (childProxy)
		{
			if (!generateArrayInfo_R (arrayMap, rootProxy, childProxy))
			{
				return false;
			}
		}
	}

	return true;
}

bool PluginGUI::generateSource_R (ControlProxy *rootProxy, ControlProxy *proxy, const char *idindent, ATOM_STRING &idlist, const char *varindent, ATOM_STRING &varlist, const char *initindent, ATOM_STRING &initlist, const char *parentWindowName)
{
	ATOM_Widget *parent = _editorForm;
	ATOM_STRING idName;
	ATOM_STRING varName;

	if (proxy)
	{
		parent = proxy->getWidget();
		int id = proxy->getWidgetParams()->getId();
		if (id != -1)
		{
			char idStr[32];
			sprintf (idStr, "%d", id);

			if (proxy != rootProxy)
			{
				generateCppNames (proxy->getWidgetClassName(), proxy->getName(), idName, varName);

				idlist += idindent;
				idlist += idName;
				idlist += " = ";
				idlist += idStr;
				idlist += ",\n";

				if (!strchr (varName.c_str(), '['))
				{
					varlist += varindent;
					varlist += proxy->getWidgetClassName();
					varlist += " *";
					varlist += varName;
					varlist += ";\n";
				}

				initlist += initindent;
				initlist += varName;
				initlist += " = parent ? (";
				initlist += proxy->getWidgetClassName();
				initlist += "*)";
				initlist += parentWindowName;
				initlist += "->getChildByIdRecursive(";
				initlist += idName;
				initlist += ") : 0;\n";
			}
		}
	}

	for (ATOM_Widget *c = parent->getFirstChild(); c; c = c->getNextSibling ())
	{
		ControlProxy *childProxy = ControlProxy::getWidgetProxy (c);
		if (childProxy)
		{
			if (!generateSource_R (rootProxy, childProxy, idindent, idlist, varindent, varlist, initindent, initlist, parentWindowName))
			{
				return false;
			}
		}
	}

	for (ATOM_Widget *c = parent->getFirstControl(); c; c = c->getNextSibling ())
	{
		ControlProxy *childProxy = ControlProxy::getWidgetProxy (c);
		if (childProxy)
		{
			if (!generateSource_R (rootProxy, childProxy, idindent, idlist, varindent, varlist, initindent, initlist, parentWindowName))
			{
				return false;
			}
		}
	}

	return true;
}

void *PluginGUI::getCreateParameter (void) const
{
	return _creationParam;
}

void PluginGUI::handleTransformEdited (ATOM_Node *node)
{
}

void PluginGUI::handleScenePropChanged (void)
{
}

void PluginGUI::changeRenderScheme (void)
{
}

