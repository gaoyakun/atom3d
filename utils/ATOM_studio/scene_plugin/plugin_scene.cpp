#include "StdAfx.h"
#include "atom3d_studio.h"
#include "plugin.h"
#include "gridnode.h"
#include "plugin_scene.h"
#include "editor.h"
#include "camera_modal.h"
#include "scene_prop_editor.h"
#include "events.h"
#include "asset.h"
#include "operation.h"
#include "lightgeom.h"
#include "locked_camera_modal.h"
#include "editorrenderscheme.h"
#include "heightgen.h"
#include "blockeditor.h"
#include "dlgloadnpc.h"
#include "dlgterraintools.h"
#include "regionedit.h"
#include "patheditor.h"
//--- wangjian added ---//
#include "engine/postEffectRain.h"
#include "engine/postEffectRainLayer.h"
#include "engine/postEffectSilhouet.h"
#include "engine/postEffectbloom.h"
#include "engine/postEffectHeatHaze.h"
#include "engine/postEffectColorFilter.h"
#include "engine/postEffectDOF.h"
#include "engine/postEffectSunShaft.h"
#include "engine/postEffectSSAO.h"
//----------------------//

#define HEIGHT_INDICATOR_DESC "高度指示面片"

#define ID_CREATE_LIGHT						(PLUGIN_ID_START + 100)
#define ID_MANUAL_CREATE					(PLUGIN_ID_START + 101)

#define ID_NODEEDIT_MOVE					(PLUGIN_ID_START + 111)
#define ID_NODEEDIT_ROTATE					(PLUGIN_ID_START + 112)
#define ID_NODEEDIT_SCALE					(PLUGIN_ID_START + 113)
#define ID_NODEEDIT_DELETE					(PLUGIN_ID_START + 114)
#define ID_NODEEDIT_CLONE					(PLUGIN_ID_START + 115)
#define ID_NODEEDIT_TOGGLELIGHTNODE			(PLUGIN_ID_START + 116)
#define ID_NODEEDIT_TOGGLEGRID				(PLUGIN_ID_START + 117)
#define ID_NODEEDIT_CLEAR_XFORM				(PLUGIN_ID_START + 118)
#define ID_NODEEDIT_CLEAR_CHILDREN_XFORM	(PLUGIN_ID_START + 119)
#define ID_VIEW_TOGGLEGUI					(PLUGIN_ID_START + 120)
#define ID_VIEW_TOGGLELOCKCAMERA			(PLUGIN_ID_START + 121)
#define ID_VIEW_SNAPSHOT					(PLUGIN_ID_START + 122)
#define ID_TOGGLE_HEIGHT_GRID				(PLUGIN_ID_START + 123)
#define ID_TOGGLE_BLOCK_GRID				(PLUGIN_ID_START + 124)
#define ID_BUILD_HEIGHT_GRID				(PLUGIN_ID_START + 125)
#define ID_EDIT_BLOCKS						(PLUGIN_ID_START + 126)
#define ID_EDIT_HEIGHT						(PLUGIN_ID_START + 127)
#define ID_LOAD_HEIGHT_DATA					(PLUGIN_ID_START + 128)
#define ID_SAVE_HEIGHT_DATA					(PLUGIN_ID_START + 129)
#define ID_EXPORT_HEIGHT_MAP				(PLUGIN_ID_START + 130)
#define ID_LOAD_BLOCK_DATA					(PLUGIN_ID_START + 131)
#define ID_SAVE_BLOCK_DATA					(PLUGIN_ID_START + 132)
#define ID_EXPORT_BLOCK_MAP					(PLUGIN_ID_START + 133)
#define ID_EXPORT_SCENE_MAP					(PLUGIN_ID_START + 134)
#define ID_HEIGHTMAP_INFO					(PLUGIN_ID_START + 135)
#define ID_NEW_HEIGHT_INDICATOR				(PLUGIN_ID_START + 136)
#define ID_LOAD_NPC							(PLUGIN_ID_START + 137)
#define ID_SAVE_NPC							(PLUGIN_ID_START + 138)
#define ID_EDIT_REGIONS						(PLUGIN_ID_START + 139)
#define ID_MATERIAL_PARAM					(PLUGIN_ID_START + 140)
#define ID_NEW_TERRAIN						(PLUGIN_ID_START + 141)
#define ID_EDIT_TERRAIN						(PLUGIN_ID_START + 142)
#define ID_SAVE_TERRAIN						(PLUGIN_ID_START + 143)

#define ID_NODEEDIT_TOGGLESCENENODEALL		(PLUGIN_ID_START + 144)	// wangjian added
#define ID_NODEEDIT_NODEPICKFILTER			(PLUGIN_ID_START + 145)	// wangjian added for test forward
#define ID_NODEEDIT_NODEPICKFILTER_ALL		(PLUGIN_ID_START + 146)	// wangjian added for test forward
#define ID_NODEEDIT_NODEPICKFILTER_LIGHT	(PLUGIN_ID_START + 147)	// wangjian added for test forward

#define ID_NODEEDIT_GENCUBEMAP				(PLUGIN_ID_START + 148)	// wangjian added for
#define ID_NODEEDIT_GENCUBEMAP_GLOBAL		(PLUGIN_ID_START + 149)	// wangjian added for
#define ID_NODEEDIT_GENCUBEMAP_LOCAL		(PLUGIN_ID_START + 150)	// wangjian added for

#define ID_SCENEGRAPH_TREE					(PLUGIN_ID_START + 500)
#define ID_POSTEFFECT_LIST					(PLUGIN_ID_START + 501)

#define ID_ALIGN_CAMERA_TO_NODE				(PLUGIN_ID_START + 502)
#define ID_APPLY_CAMERA_TO_NODE				(PLUGIN_ID_START + 503)

#define ID_DEL_POSTEFFECT					(PLUGIN_ID_START + 600)
#define ID_EDIT_POSTEFFECT					(PLUGIN_ID_START + 601)
#define ID_ADD_CGPOSTEFFECT					(PLUGIN_ID_START + 602)
#define ID_ADD_FXAAPOSTEFFECT				(PLUGIN_ID_START + 603)
#define ID_PATH_EDITOR						(PLUGIN_ID_START + 605)

//////////////////////////////////////////////////////////////////////////////////////////////
#define ID_ADD_RAINPOSTEFFECT				(PLUGIN_ID_START + 606)			// wangjian added
#define ID_ADD_RAINLAYERPOSTEFFECT			(PLUGIN_ID_START + 607)			// wangjian added
#define ID_ADD_SILHOUETEPOSTEFFECT			(PLUGIN_ID_START + 608)			// wangjian added
#define ID_ADD_BLOOMPOSTEFFECT				(PLUGIN_ID_START + 609)			// wangjian added
#define ID_ADD_HEATHAZEPOSTEFFECT			(PLUGIN_ID_START + 610)			// wangjian added
#define ID_ADD_COLORFILTERPOSTEFFECT		(PLUGIN_ID_START + 611)			// wangjian added
#define ID_ADD_DOFPOSTEFFECT				(PLUGIN_ID_START + 612)			// wangjian added
#define ID_ADD_SUNSHAFTPOSTEFFECT			(PLUGIN_ID_START + 613)			// wangjian added
#define ID_ADD_SSAOPOSTEFFECT				(PLUGIN_ID_START + 614)			// wangjian added
//////////////////////////////////////////////////////////////////////////////////////////////

#define ID_PICKEDNODELIST					(PLUGIN_ID_START + 1000)


//////////////////////////////////////////////////////////////////////////////////////////////

class MyXFormNodeOp: public AS_Operation
{
	PluginScene *_plugin;
	ATOM_AUTOREF(ATOM_Node) _node;
	ATOM_Matrix4x4f _oldXform;
	ATOM_Matrix4x4f _newXform;

public:
	MyXFormNodeOp (PluginScene *plugin, ATOM_Node *node, const ATOM_Matrix4x4f &matrix)
		: _plugin(plugin)
		, _node(node)
		, _oldXform(node->getO2T())
		, _newXform(matrix)
	{
	}

	virtual void undo (void)
	{
		_node->setO2T (_oldXform);
		_plugin->refreshPathList ();
	}

	virtual void redo (void)
	{
		_node->setO2T (_newXform);
		_plugin->refreshPathList ();
	}

	virtual void deleteMe (void)
	{
		ATOM_DELETE(this);
	}
};

class MyCreateNodeOp: public AS_Operation
{
	ATOM_AUTOREF(ATOM_Node) _child;
	ATOM_AUTOREF(ATOM_Node) _parent;
	ATOM_TreeCtrl *_treeCtrl;
	PluginScene *_pluginScene;

public:
	MyCreateNodeOp (ATOM_Node *nodeChild, ATOM_Node *nodeParent, ATOM_TreeCtrl *treeCtrl, PluginScene *pluginScene)
		: _child(nodeChild)
		, _parent(nodeParent)
		, _treeCtrl(treeCtrl)
		, _pluginScene(pluginScene)
	{

	}

	virtual void undo (void)
	{
		_pluginScene->selectNode (0);
		ATOM_TreeItem *item = (ATOM_TreeItem*)_child->getGameEntity();
		_parent->removeChild (_child.get());
		if (item)
		{
			_treeCtrl->deleteItem (item);
		}
		_pluginScene->refreshPathList();
	}

	virtual void redo (void)
	{
		_parent->appendChild (_child.get());

		ATOM_TreeItem *item = (ATOM_TreeItem*)_parent->getGameEntity();
		ATOM_TreeItem *subItem = _treeCtrl->newItem (_child->getDescribe().c_str(), ATOM_IMAGEID_TREECTRL_EXPANDED_IMAGE, ATOM_IMAGEID_TREECTRL_COLLAPSED_IMAGE, item);
		_child->setGameEntity ((ATOM_GameEntity*)subItem);
		subItem->setUserData ((unsigned)_child.get());
		_pluginScene->selectNode (_child.get());
		_pluginScene->refreshPathList();
	}

	virtual void deleteMe (void)
	{
		ATOM_DELETE(this);
	}
};

class MyReparentNodeOp: public AS_Operation
{
	ATOM_AUTOREF(ATOM_Node) _childNode;
	ATOM_AUTOREF(ATOM_Node) _parentNode;
	ATOM_AUTOREF(ATOM_Node) _oldParentNode;
	ATOM_TreeCtrl *_treeCtrl;
	PluginScene *_pluginScene;

public:
	MyReparentNodeOp (ATOM_Node *childNode, ATOM_Node *parentNode, ATOM_TreeCtrl *treeCtrl, PluginScene *pluginScene)
		: _childNode(childNode)
		, _parentNode(parentNode)
		, _treeCtrl(treeCtrl)
		, _pluginScene(pluginScene)
	{

	}

	virtual void undo (void)
	{
		_parentNode = _oldParentNode;

		redo ();
	}

	virtual void redo (void)
	{
		const ATOM_Matrix4x4f &matWorld = _childNode->getWorldMatrix ();
		const ATOM_Matrix4x4f &matInvParentWorld = _parentNode->getInvWorldMatrix ();
		_childNode->setO2T (matInvParentWorld >> matWorld);

		_oldParentNode = _childNode->getParent();
		_oldParentNode->removeChild (_childNode.get());
		_parentNode->appendChild (_childNode.get());

		ATOM_TreeItem *item = (ATOM_TreeItem*)_childNode->getGameEntity();
		ATOM_TreeItem *newRootItem = (ATOM_TreeItem*)_parentNode->getGameEntity();
		if (item && newRootItem)
		{
			item->changeRoot (newRootItem);
		}
		_pluginScene->selectNode (_childNode.get());
		_pluginScene->refreshPathList ();
	}

	virtual void deleteMe (void)
	{
		ATOM_DELETE(this);
	}
};

class MyReplaceNodeOp: public AS_Operation
{
	ATOM_AUTOREF(ATOM_Node) _newNode;
	ATOM_AUTOREF(ATOM_Node) _oldNode;
	ATOM_TreeCtrl *_treeCtrl;
	PluginScene *_pluginScene;

public:
	MyReplaceNodeOp (ATOM_Node *newNode, ATOM_Node *oldNode, ATOM_TreeCtrl *treeCtrl, PluginScene *pluginScene)
		: _newNode(newNode)
		, _oldNode(oldNode)
		, _treeCtrl(treeCtrl)
		, _pluginScene(pluginScene)
	{

	}

	virtual void undo (void)
	{
		ATOM_Node *parent = _newNode->getParent();
		parent->removeChild (_newNode.get());
		parent->appendChild (_oldNode.get());

		ATOM_TreeItem *item = (ATOM_TreeItem*)_newNode->getGameEntity();
		if (item)
		{
			_treeCtrl->deleteItem (item);
		}
		item = _treeCtrl->newItem (_oldNode->getDescribe().c_str(), ATOM_IMAGEID_TREECTRL_EXPANDED_IMAGE, ATOM_IMAGEID_TREECTRL_COLLAPSED_IMAGE, (ATOM_TreeItem*)parent->getGameEntity());
		item->setUserData ((unsigned)_oldNode.get());
		_oldNode->setGameEntity ((ATOM_GameEntity*)item);
		_pluginScene->selectNode (_oldNode.get());
		_pluginScene->refreshPathList();
	}

	virtual void redo (void)
	{
		_newNode->setO2T(_oldNode->getO2T());
		ATOM_Node *parent = _oldNode->getParent();
		parent->removeChild (_oldNode.get());
		parent->appendChild (_newNode.get());

		ATOM_TreeItem *item = (ATOM_TreeItem*)_oldNode->getGameEntity();
		if (item)
		{
			_treeCtrl->deleteItem (item);
		}
		item = _treeCtrl->newItem (_newNode->getDescribe().c_str(), ATOM_IMAGEID_TREECTRL_EXPANDED_IMAGE, ATOM_IMAGEID_TREECTRL_COLLAPSED_IMAGE, (ATOM_TreeItem*)parent->getGameEntity());
		item->setUserData ((unsigned)_newNode.get());
		_newNode->setGameEntity ((ATOM_GameEntity*)item);
		_pluginScene->selectNode (_newNode.get());
		_pluginScene->refreshPathList ();
	}

	virtual void deleteMe (void)
	{
		ATOM_DELETE(this);
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////

PluginScene::PluginScene (void)
{
	_tcSceneGraph = 0;
	_tbNodeProperties = 0;
	_scene = 0;
	_selectedNode = 0;
	_editor = 0;
	_pickedNodeList = 0;
	_cloneSrc = 0;
	_editMode = EM_SELECT;
	_isCreatingAsset = false;
	_isEditingHeight = false;
	_replaceAsset = false;
	_tbTimePreview = 0;
	_assetOffsetY = 0;
	_assetOffsetY2 = 0;
	_displayLightGeode = true;
	//--- wangjian added ---//
	_hideAllSceneNode = false;
	_nodePickFilterMenu = 0;
	_genEnvMapMenu = 0;
	//----------------------//
	_pickVisitor.setPickAll (true);		// wangjian modified
	_postEffectListMenu = 0;
	_nodeMenu = 0;
	_postEffectList = 0;
	_oldCameraModal = 0;
	_dlgHeightGen = 0;
	_dlgEditBlocks = 0;
	_dlgLoadNPC = 0;
	_dlgTerrainTools = 0;
	_dlgRegionEdit = 0;
	_dlgPathEditor = 0;
	_beginCreatingRegion = false;
	_beginCreatePath = false;
	_editorRenderSchemeFactory = ATOM_NEW(EditorRenderSchemeFactory);
	ATOM_RenderScheme::registerRenderScheme ("editor", _editorRenderSchemeFactory);
	_lockedCameraModal = ATOM_NEW(LockedCamera);
	LightGeode::setDisplay(_displayLightGeode);
}

PluginScene::~PluginScene (void)
{
	clear ();
	ATOM_DELETE(_lockedCameraModal);
	ATOM_DELETE(_editorRenderSchemeFactory);
}

unsigned PluginScene::getVersion (void) const
{
	return AS_VERSION;
}

const char *PluginScene::getName (void) const
{
	return "Scene editor";
}

void PluginScene::deleteMe (void)
{
	ATOM_DELETE(this);
}

ATOM_Node *PluginScene::getSceneRoot (void) const
{
	return _sceneRoot.get();
}

void PluginScene::clear (void)
{
	if (_tcSceneGraph)
	{
		_tcSceneGraph->setEventTrigger (0);
		ATOM_DELETE(_tcSceneGraph);
		_tcSceneGraph = 0;
	}

	if (ATOM_Widget::isValidWidget (_postEffectListMenu))
	{
		ATOM_DELETE(_postEffectListMenu);
		_postEffectListMenu = 0;
	}

	if (ATOM_Widget::isValidWidget (_nodeMenu))
	{
		ATOM_DELETE(_nodeMenu);
		_nodeMenu = 0;
	}

	ATOM_DELETE(_postEffectList);
	_postEffectList = 0;

	ATOM_DELETE(_pickedNodeList);
	_pickedNodeList = 0;

	_editor->setScenePropEditorTarget (NULL);
	_editor->showScenePropEditor (false);

	ATOM_DELETE(_tbNodeProperties);
	_tbNodeProperties = 0;

	ATOM_DELETE(_tbTimePreview);
	_tbTimePreview = 0;

	//--- wangjian added ---//
	if( ATOM_AsyncLoader::IsRun() )
	{
		if(_sceneRoot.get() && _sceneRoot->getParent() )
		{
			_sceneRoot->getParent()->removeChild(_sceneRoot.get());
			_sceneRoot = NULL;
		}
		ATOM_AsyncLoader::AbandonAllTask(false);
	}
	//----------------------//

	ATOM_DELETE(_scene);
	_scene = 0;

	ATOM_DELETE(_dlgHeightGen);
	_dlgHeightGen = 0;

	ATOM_DELETE(_dlgEditBlocks);
	_dlgEditBlocks = 0;

	ATOM_DELETE(_dlgLoadNPC);
	_dlgLoadNPC = 0;

	ATOM_DELETE(_dlgTerrainTools);
	_dlgTerrainTools = 0;

	ATOM_DELETE(_dlgRegionEdit);
	_dlgRegionEdit = 0;

	ATOM_DELETE(_dlgPathEditor);
	_dlgPathEditor = 0;

	_selectedNode = 0;
	_isCreatingAsset = false;
	_replaceAsset = false;
	_assetRotateX = 0.f;
	_assetRotateY = 0.f;
	_assetRotateZ = 0.f;
	_assetOffsetY = 0.f;
	_assetOffsetY2 = 0.f;
	_nodeToBeReplaced = 0;
	_currentCreateAsset = 0;;
}

bool PluginScene::initPlugin (AS_Editor *editor)
{
	editor->registerFileType (this, "3sg", "ATOM3D 场景", AS_FILETYPE_CANEDIT);

	_editor = editor;

	return true;
}

void PluginScene::donePlugin (void)
{
	clear ();
}

void PluginScene::setupMenu (void)
{
	ATOM_MenuBar *menubar = _editor->getMenuBar ();

	ATOM_PopupMenu *menuView = menubar->getMenuItem (AS_MENUITEM_VIEW).submenu;
	menuView->appendMenuItem ("切换界面显示(F11)", ID_VIEW_TOGGLEGUI);
	menuView->appendMenuItem ("锁定视角", ID_VIEW_TOGGLELOCKCAMERA);
	menuView->appendMenuItem ("场景拍照", ID_VIEW_SNAPSHOT);

	menubar->insertMenuItem (AS_MENUITEM_CUSTOM, "摄像机", 0);
	ATOM_PopupMenu *menu = menubar->createSubMenu (AS_MENUITEM_CUSTOM);
	menu->appendMenuItem ("对齐到选中节点", ID_ALIGN_CAMERA_TO_NODE);
	menu->appendMenuItem ("应用到选中节点", ID_APPLY_CAMERA_TO_NODE);

	menubar->insertMenuItem (AS_MENUITEM_CUSTOM, "节点", 0);
	menu = menubar->createSubMenu (AS_MENUITEM_CUSTOM);
	menu->appendMenuItem ("平移(T)", ID_NODEEDIT_MOVE);
	menu->appendMenuItem ("旋转(R)", ID_NODEEDIT_ROTATE);
	menu->appendMenuItem ("缩放(S)", ID_NODEEDIT_SCALE);
	menu->appendMenuItem ("删除(DEL)", ID_NODEEDIT_DELETE);
	menu->appendMenuItem ("克隆(D)", ID_NODEEDIT_CLONE);
	menu->appendMenuItem ("切换灯光指示", ID_NODEEDIT_TOGGLELIGHTNODE);
	menu->appendMenuItem ("切换地板网格显示", ID_NODEEDIT_TOGGLEGRID);
	//--- wangjian added ---//
	menu->appendMenuItem ("切换场景节点显示(除地形)", ID_NODEEDIT_TOGGLESCENENODEALL);
	menu->appendMenuItem ("节点点选过滤器", ID_NODEEDIT_NODEPICKFILTER);
	_nodePickFilterMenu = menu->createSubMenu (8);
	_nodePickFilterMenu->appendMenuItem ("所有", ID_NODEEDIT_NODEPICKFILTER_ALL);
	_nodePickFilterMenu->appendMenuItem ("光源", ID_NODEEDIT_NODEPICKFILTER_LIGHT);
	_nodePickFilterMenu->setCheckById(ID_NODEEDIT_NODEPICKFILTER_ALL,true);
	menu->appendMenuItem ("生成环境贴图", ID_NODEEDIT_GENCUBEMAP);
	_genEnvMapMenu = menu->createSubMenu(9);
	_genEnvMapMenu->appendMenuItem ("全局贴图", ID_NODEEDIT_GENCUBEMAP_GLOBAL);
	_genEnvMapMenu->appendMenuItem ("局部贴图", ID_NODEEDIT_GENCUBEMAP_LOCAL);
	//----------------------//

	menubar->insertMenuItem (AS_MENUITEM_CUSTOM, "地形", 0);
	menu = menubar->createSubMenu (AS_MENUITEM_CUSTOM);
	menu->appendMenuItem ("创建地形...", ID_NEW_TERRAIN);
	menu->appendMenuItem ("编辑地形...", ID_EDIT_TERRAIN);
	menu->appendMenuItem ("保存地形", ID_SAVE_TERRAIN);

	menubar->insertMenuItem (AS_MENUITEM_CUSTOM, "高度/障碍", 0);
	menu = menubar->createSubMenu (AS_MENUITEM_CUSTOM);
	menu->appendMenuItem ("重建高度图...", ID_BUILD_HEIGHT_GRID);
	menu->appendMenuItem ("显示/隐藏高度(H)", ID_TOGGLE_HEIGHT_GRID);
	menu->appendMenuItem ("显示/隐藏障碍(B)", ID_TOGGLE_BLOCK_GRID);
	menu->appendMenuItem ("编辑障碍...", ID_EDIT_BLOCKS);
	menu->appendMenuItem ("编辑高度...", ID_EDIT_HEIGHT);
	menu->appendMenuItem ("载入高度...", ID_LOAD_HEIGHT_DATA);
	menu->appendMenuItem ("保存高度...", ID_SAVE_HEIGHT_DATA);
	menu->appendMenuItem ("导出高度图...", ID_EXPORT_HEIGHT_MAP);
	menu->appendMenuItem ("载入障碍...", ID_LOAD_BLOCK_DATA);
	menu->appendMenuItem ("保存障碍...", ID_SAVE_BLOCK_DATA);
	menu->appendMenuItem ("导出障碍图...", ID_EXPORT_BLOCK_MAP);
	menu->appendMenuItem ("导出场景图...", ID_EXPORT_SCENE_MAP);
	menu->appendMenuItem ("高度图信息...", ID_HEIGHTMAP_INFO);
	menu->appendMenuItem ("创建面片", ID_NEW_HEIGHT_INDICATOR);

	menubar->insertMenuItem (AS_MENUITEM_CUSTOM, "NPC", 0);
	menu = menubar->createSubMenu (AS_MENUITEM_CUSTOM);
	menu->appendMenuItem ("载入NPC配置...", ID_LOAD_NPC);
	menu->appendMenuItem ("保存NPC配置...", ID_SAVE_NPC);

	menubar->insertMenuItem (AS_MENUITEM_CUSTOM, "区域", 0);
	menu = menubar->createSubMenu (AS_MENUITEM_CUSTOM);
	menu->appendMenuItem ("编辑区域...", ID_EDIT_REGIONS);

	menubar->insertMenuItem (AS_MENUITEM_CUSTOM, "路径", 0);
	menu = menubar->createSubMenu (AS_MENUITEM_CUSTOM);
	menu->appendMenuItem ("编辑路径...", ID_PATH_EDITOR);

	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_t, 0, ID_NODEEDIT_MOVE));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_r, 0, ID_NODEEDIT_ROTATE));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_s, 0, ID_NODEEDIT_SCALE));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_DELETE, 0, ID_NODEEDIT_DELETE));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_d, 0, ID_NODEEDIT_CLONE));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_i, KEYMOD_CTRL, ID_NODEEDIT_CLEAR_XFORM));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_i, KEYMOD_CTRL|KEYMOD_ALT, ID_NODEEDIT_CLEAR_CHILDREN_XFORM));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_l, 0, ID_VIEW_TOGGLELOCKCAMERA));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_b, 0, ID_TOGGLE_BLOCK_GRID));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_h, 0, ID_TOGGLE_HEIGHT_GRID));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_F11, 0, ID_VIEW_TOGGLEGUI));
}

void PluginScene::cleanupMenu (void)
{
	ATOM_MenuBar *menubar = _editor->getMenuBar ();
	menubar->getMenuItem(AS_MENUITEM_VIEW).submenu->removeMenuItemById (ID_VIEW_TOGGLEGUI);
	menubar->getMenuItem(AS_MENUITEM_VIEW).submenu->removeMenuItemById (ID_VIEW_TOGGLELOCKCAMERA);
	menubar->getMenuItem(AS_MENUITEM_VIEW).submenu->removeMenuItemById (ID_VIEW_SNAPSHOT);
	menubar->removeMenuItem (AS_MENUITEM_CUSTOM);
	menubar->removeMenuItem (AS_MENUITEM_CUSTOM);
	menubar->removeMenuItem (AS_MENUITEM_CUSTOM);
	menubar->removeMenuItem (AS_MENUITEM_CUSTOM);
	menubar->removeMenuItem (AS_MENUITEM_CUSTOM);
	menubar->removeMenuItem (AS_MENUITEM_CUSTOM);
	menubar->removeMenuItem (AS_MENUITEM_CUSTOM);

	for (unsigned i = 0; i < _accelKeys.size(); ++i)
	{
		_editor->unregisterAccelKeyCommand (_accelKeys[i]);
	}
	_accelKeys.clear ();
}

bool PluginScene::beginEdit (const char *filename)
{
	if (filename)
	{
		checkDuplicatedGeodes (filename);
	}

	_scene = ATOM_NEW(ATOM_DeferredScene);

	ATOM_RenderScheme *renderScheme = _editor->getRenderScheme ();
	if (!renderScheme)
	{
		return false;
	}
	_scene->setRenderScheme (renderScheme);

	if (filename)
	{
		//--- wangjian added ---//
		bool bRecordFilenameInEditorMode = false;
		// 在加载场景时检查是否开启保存场景中不需要异步加载的资源文件集合
		ATOM_VECTOR<ATOM_STRING> args;
		ATOM_GetCommandLine (args);
		for (unsigned i = 0; i < args.size(); ++i)
		{
			if (args[i] == "--record-filename-editor")
			{
				bRecordFilenameInEditorMode = true;
				ATOM_ClearOpenedFileList(0);
				ATOM_KernelConfig::setRecordFileName(true);
			}
		}
		//----------------------//

		if (!_scene->load (filename))
		{
			//--- wangjian added ---//
			// 如果开启了编辑器记录文件模式 关闭它
			if( bRecordFilenameInEditorMode )
			{
				ATOM_KernelConfig::setRecordFileName(false);
			}
			//----------------------//

			ATOM_DELETE(_scene);
			_scene = 0;
			return false;
		}
		else
		{
			//--- wangjian added ---//
			// 如果开启了编辑器记录文件模式 关闭它
			if( bRecordFilenameInEditorMode )
			{
				ATOM_KernelConfig::setRecordFileName(false);
			}
			//----------------------//

			if (1 != _scene->getRootNode()->getNumChildren() || strcmp(_scene->getRootNode()->getChild(0)->getDescribe().c_str(), "World"))
			{
				_sceneRoot = ATOM_HARDREF(ATOM_Node)();
				_sceneRoot->setO2T (ATOM_Matrix4x4f::getIdentityMatrix());
				_sceneRoot->setDescribe("World");

				ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)> nodes;
				for (unsigned i = 0; i < _scene->getRootNode()->getNumChildren(); ++i)
				{
					nodes.push_back(_scene->getRootNode()->getChild(i));
				}
				_scene->getRootNode()->clearChildren();

				for (unsigned i = 0;i < nodes.size(); ++i)
				{
					_sceneRoot->appendChild (nodes[i].get());
				}
				_scene->getRootNode()->appendChild (_sceneRoot.get());
			}
			else
			{
				_sceneRoot = _scene->getRootNode()->getChild(0);
			}

			addLightGeodeToScene ();

			const ATOM_HASHMAP<ATOM_STRING, ATOM_Path3D> &pathes = _scene->getPathes();
			if (pathes.size() > 0)
			{
				for (ATOM_HASHMAP<ATOM_STRING, ATOM_Path3D>::const_iterator it = pathes.begin(); it != pathes.end(); ++it)
				{
					getDlgPathEditor()->addPath (it->first.c_str(), it->second);
				}
			}
		}
	}
	else
	{
		_sceneRoot = ATOM_HARDREF(ATOM_Node)();
		_sceneRoot->setDescribe("World");
		_scene->getRootNode()->appendChild (_sceneRoot.get());

#if 0
		ATOM_HARDREF(ATOM_LineTrail) lineTrail;
		lineTrail->loadAttribute (nullptr);
		lineTrail->load (ATOM_GetRenderDevice());
		ATOM_LineTrail::InflectionPoint lp;
		lp.position.set(0.f, 0.f, 0.f);
		lp.color.setRaw (0xFFFFFFFF);
		lp.width = 2.f;
		lp.length = 0.f;
		lp.lengthDirty = false;
		lineTrail->appendInflectionPoint(lp);
		lp.position.set(20.f, 0.f, 0.f);
		lineTrail->appendInflectionPoint(lp);
		lp.position.set(40.f, 10.f, 0.f);
		lineTrail->appendInflectionPoint(lp);
		lp.position.set(60.f, 20.f, 0.f);
		lineTrail->appendInflectionPoint(lp);
		lp.position.set(60.f, 60.f, 0.f);
		lineTrail->appendInflectionPoint(lp);
		_scene->getRootNode()->appendChild (lineTrail.get());
#endif
	}

	_grid = ATOM_HARDREF(GridNode)();
	_grid->setPickable (0);
	_grid->setSize (100, 100);
	_grid->load (ATOM_GetRenderDevice());
	_grid->setO2T (ATOM_Matrix4x4f::getScaleMatrix(ATOM_Vector3f(100.f, 100.f, 100.f)));
	_scene->getRootNode()->appendChild (_grid.get());

	AS_CameraModal *camera = _editor->getCameraModal();
	if (filename)
	{
		const ATOM_Matrix4x4f *cameraMatrix = _scene->getCameraMatrix();
		if (cameraMatrix)
		{
			ATOM_Vector3f eye, target, up;
			cameraMatrix->decomposeLookatLH (eye, target, up);
			camera->setPosition (eye);
			camera->setDirection (target - eye);
			camera->setUpVector (ATOM_Vector3f(0.f, 1.f, 0.f));
		}
		else
		{
			ATOM_BBox bbox = calcSceneBoundingbox ();
			ATOM_Vector3f eye = bbox.getCenter();
			float t = bbox.getExtents().y;
			if (t < 100.f)
			{
				t = 100.f;
			}
			eye.x += t;
			eye.z += t;

			camera->setPosition (eye);
			camera->setDirection (bbox.getCenter() - eye);
			camera->setUpVector (ATOM_Vector3f(0.f, 1.f, 0.f));
		}
	}
	else
	{
		camera->setPosition (ATOM_Vector3f(0.f, 500.f, -500.f));
		camera->setDirection (ATOM_Vector3f(0.f, -500.f, 500.f));
		camera->setUpVector (ATOM_Vector3f(0.f, 1.f, 0.f));
	}
	camera->setFovY (_scene->getFOVY());
	camera->setNearPlane (_scene->getNearPlane());
	camera->setFarPlane (_scene->getFarPlane());
	_lockedCameraModal->setDirection (_scene->getFixedCameraVector ());

	_editor->getRealtimeCtrl()->setScene (_scene);

		
	createTweakBars ();
	createSceneGraphTree (_editor->getRealtimeCtrl());
	createPostEffectList (_editor->getRealtimeCtrl());
		
	_editor->showAssetEditor (true);
		
	calcUILayout ();

	setupMenu ();

	_pickVisitor.setCamera (_scene->getCamera());
	_isGUIShown = true;
	_isCreatingAsset = false;
	_replaceAsset = false;

	ATOM_Terrain *terrain = findTerrain ();
	if (terrain && ATOM_Terrain::isEditorModeEnabled())
	{
		_dlgTerrainTools = ATOM_NEW(DlgTerrainTools, this, terrain, _editor->getRealtimeCtrl());
	}

	return true;
}

void PluginScene::endEdit (void)
{
	if (_isEditingHeight)
	{
		endEditHeight ();
	}

	if (_dlgTerrainTools && _dlgTerrainTools->isShown())
	{
		endEditTerrain ();
	}

	if (_isCreatingAsset)
	{
		endCreateAsset (true, false);
	}

	endAddRegion (_currentRegion.get(), true);
	endAddPath (_currentPath.get(), true);

	if (_pickedNodeList)
	{
		ATOM_DELETE(_pickedNodeList);
		_pickedNodeList = 0;
	}

	endEditNodeTransform ();

	cleanupMenu ();

	_editor->getRealtimeCtrl()->setScene (0);

	//--- wangjian added ---//
	//ATOM_AsyncLoader::AbandonAllTask();
	//----------------------//

	clear ();
}

class SceneSizeCalculator: public ATOM_Visitor
{
public:
	ATOM_BBox boundingbox;

public:
	SceneSizeCalculator (void): boundingbox(ATOM_Vector3f(FLT_MAX, FLT_MAX, FLT_MAX), ATOM_Vector3f(-FLT_MAX, -FLT_MAX, -FLT_MAX)) {}

public:
	virtual void visit (ATOM_Node &node)
	{
		const ATOM_BBox &bbox = node.getWorldBoundingbox ();
		boundingbox.extend (bbox.getMin ());
		boundingbox.extend (bbox.getMax ());
	}

	virtual void visit (ATOM_LightNode &node) {}

	virtual void visit (ATOM_Atmosphere &node) {}

	virtual void visit (ATOM_VisualNode &node) {}

	virtual void visit (ATOM_ParticleSystem &node)
	{
		const ATOM_BBox &bbox = node.getWorldBoundingbox ();
		boundingbox.extend (bbox.getMin ());
		boundingbox.extend (bbox.getMax ());
	}

	virtual void visit (ATOM_GuiHud &node) {}

	virtual void visit (ATOM_Water &node)
	{
		const ATOM_BBox &bbox = node.getWorldBoundingbox ();
		boundingbox.extend (bbox.getMin ());
		boundingbox.extend (bbox.getMax ());
	}

	virtual void visit (ATOM_Terrain &node)
	{
		const ATOM_BBox &bbox = node.getWorldBoundingbox ();
		boundingbox.extend (bbox.getMin ());
		boundingbox.extend (bbox.getMax ());
	}

	virtual void visit (ATOM_Geode &node)
	{
		const ATOM_BBox &bbox = node.getWorldBoundingbox ();
		boundingbox.extend (bbox.getMin ());
		boundingbox.extend (bbox.getMax ());
	}
};


ATOM_BBox PluginScene::calcSceneBoundingbox (void)
{
	if (_scene)
	{
		SceneSizeCalculator calc;
		calc.traverse(*_scene->getRootNode());
		return calc.boundingbox;
	}
	return ATOM_BBox(ATOM_Vector3f(0.f, 0.f, 0.f), ATOM_Vector3f(0.f, 0.f, 0.f));
}

void PluginScene::frameUpdate (void)
{
	if (_beginCreatingRegion)
	{
		updateRegionTransform ();
	}
	else if (_beginCreatePath)
	{
		updatePathTransform ();
	}
	else if (_isCreatingAsset)
	{
		updateAssetTransform ();
	}
	else if (_dlgTerrainTools && _dlgTerrainTools->isShown())
	{
		ATOM_Point2Di mousePos = _editor->getMousePosition ();
		ATOM_PickVisitor v;
		v.setCamera (_editor->getRealtimeCtrl()->getScene()->getCamera());
		v.setPickAll (true);
		_editor->getRealtimeCtrl()->pick (mousePos.x, mousePos.y, v);
		bool hitTerrain = false;
		float distance = 0.f;
		ATOM_Terrain *terrain = 0;

		if (v.getNumPicked())
		{
			v.sortResults ();
			for (unsigned i = 0; i < v.getNumPicked(); ++i)
			{
				terrain = dynamic_cast<ATOM_Terrain*>(v.getPickResult(i).node.get());
				if (terrain && terrain == _dlgTerrainTools->getTerrain())
				{
					float Ry = v.getRay().getDirection().y;
					float s = (Ry > 0.f) ? 1.f : -1.f;
					if (ATOM_abs(Ry) < 0.001f)
					{
						Ry = 0.001f * s;
					}
					distance = v.getPickResult(i).distance;
					hitTerrain = true;
					break;
				}
			}
		}

		if (hitTerrain)
		{
			ATOM_Vector3f posHit = v.getRay().getOrigin() + v.getRay().getDirection() * distance;
			_dlgTerrainTools->setHitTerrain (true, posHit);
			_dlgTerrainTools->updateBrushIndicator ();

			if (_dlgTerrainTools->isBrushing())
			{
				const ATOM_BBox &bbox = terrain->getWorldBoundingbox ();
				int w = terrain->getHeightmap()->getWidth();
				int h = terrain->getHeightmap()->getHeight();
				float fx = (posHit.x - bbox.getMin().x) / (bbox.getMax().x - bbox.getMin().x);
				float fy = (posHit.z - bbox.getMin().z) / (bbox.getMax().z - bbox.getMin().z);
				int x = w * fx;
				int y = h * fy;
				_dlgTerrainTools->updateBrushing (x, y, fx, fy);
			}
		}
		else
		{
			_dlgTerrainTools->setHitTerrain (false, 0.f);
		}
	}
}

void PluginScene::updateRegionTransform (void)
{
	updateNodeTransformFromCursorHF (_currentRegion.get());
}

void PluginScene::updatePathTransform (void)
{
	updateNodeTransformFromCursorHF (_currentPath.get());
}

void PluginScene::updateAssetTransform (void)
{
	updateNodeTransformFromCursor (_currentCreateAsset.get());
}

void PluginScene::updateNodeTransformFromCursor (ATOM_Node *node)
{
	static const float fastDistance = 1000.f;
	float distance = fastDistance;

	ATOM_Point2Di mousePos = _editor->getMousePosition ();
	ATOM_PickVisitor v;
	v.setCamera (_editor->getRealtimeCtrl()->getScene()->getCamera());
	v.setPickAll (true);
	_editor->getRealtimeCtrl()->pick (mousePos.x, mousePos.y, v);
	bool hitTerrain = false;

	if (v.getNumPicked())
	{
		v.sortResults ();
		ATOM_Terrain *terrain = 0;
		for (unsigned i = 0; i < v.getNumPicked(); ++i)
		{
			terrain = dynamic_cast<ATOM_Terrain*>(v.getPickResult(i).node.get());
			if (terrain && terrain != node)
			{
				float Ry = v.getRay().getDirection().y;
				float s = (Ry > 0.f) ? 1.f : -1.f;
				if (ATOM_abs(Ry) < 0.001f)
				{
					Ry = 0.001f * s;
				}
				float f = _assetOffsetY / Ry;
				distance = v.getPickResult(i).distance + f;
				if (distance < 0.f)
				{
					distance = 0.f;
				}
				hitTerrain = true;
				break;
			}
		}
	}

	if (!hitTerrain)
	{
		float Ry = v.getRay().getDirection().y;
		float s = (Ry > 0.f) ? 1.f : -1.f;
		if (ATOM_abs(Ry) < 0.001f)
		{
			Ry = 0.001f * s;
		}
		distance = (_assetOffsetY2 - _editor->getCameraModal()->getPosition().y) / v.getRay().getDirection().y;
		if (distance < 0.f)
		{
			distance = 0.f;
		}
	}

	ATOM_Vector3f cameraPos = _editor->getCameraModal()->getPosition();
	ATOM_Vector3f rayDirection = v.getRay().getDirection();
	rayDirection.normalize ();
	ATOM_Vector3f objectPos = cameraPos + rayDirection * distance;
	ATOM_Matrix4x4f matrix = node->getWorldMatrix ();
	ATOM_Matrix4x4f matrixParent = node->getParent()->getWorldMatrix();
	matrixParent.invertAffine ();

	matrix.m30 = objectPos.x;
	matrix.m31 = objectPos.y;
	matrix.m32 = objectPos.z;

	node->setO2T (matrixParent >> matrix);
}

void PluginScene::updateNodeTransformFromCursorHF (ATOM_Node *node)
{
	ATOM_Ray ray;
	ATOM_Point2Di mousePos = _editor->getMousePosition ();
	_editor->getRealtimeCtrl()->constructRay (mousePos.x, mousePos.y, ray);
	ATOM_Vector3f pos;
	float d;
	HeightGrid *grid = getDlgHeightGen()->getGrid();
	if (grid->getSize().w <= 0 || grid->getSize().h <= 0)
	{
		updateNodeTransformFromCursor (node);
	}
	else if (getDlgHeightGen()->getGrid()->getBBoxTree()->pick (ray, d))
	{
		pos = ray.getOrigin() + ray.getDirection() * d;
		ATOM_Matrix4x4f matrix = node->getWorldMatrix ();
		ATOM_Matrix4x4f matrixParent = node->getParent()->getWorldMatrix();
		matrixParent.invertAffine ();

		matrix.m30 = pos.x;
		matrix.m31 = pos.y;
		matrix.m32 = pos.z;

		node->setO2T (matrixParent >> matrix);
	}
}

void PluginScene::createTweakBars (void)
{
	_editor->setScenePropEditorTarget (_scene);
	_editor->showScenePropEditor (true);
}

static ATOM_STRING createNodeDescStr (ATOM_Node *node)
{
	const char *desc = node->getDescribe ().c_str();
	if (!desc || !desc[0])
	{
		char buffer[256];
		sprintf (buffer, "%s(未命名)", node->getClassName());
		return buffer;
	}
	else
	{
		return desc;
	}
}

static void CreateTreeItemsR (AS_Editor *editor, PluginScene *pluginScene, ATOM_TreeCtrl *treeCtrl, ATOM_TreeItem *parent, ATOM_Node *node)
{

	if (!editor->isDummyNodeType (node->getClassName()) && !node->getPrivate())
	{
		// The light node is not pickable if it is loaded from 3sg
		if (dynamic_cast<ATOM_LightNode*>(node))
		{
			node->setPickable (0);
		}

		ATOM_STRING desc = createNodeDescStr (node);
		ATOM_TreeItem *subItem = treeCtrl->newItem (desc.c_str(), ATOM_IMAGEID_TREECTRL_EXPANDED_IMAGE, ATOM_IMAGEID_TREECTRL_COLLAPSED_IMAGE, parent);
		node->setGameEntity ((ATOM_GameEntity*)subItem);
		subItem->setUserData ((unsigned long long)node);

		for (unsigned i = 0; i < node->getNumChildren(); ++i)
		{
			ATOM_Node *subNode = node->getChild (i);
			CreateTreeItemsR (editor, pluginScene, treeCtrl, subItem, subNode);
		}
	}
}

void PluginScene::createSceneGraphTree (ATOM_RealtimeCtrl *parent)
{
	ATOM_Rect2Di rc = parent->getClientRect();
	rc.size.w = 200;

	_tcSceneGraph = ATOM_NEW(ATOM_TreeCtrl, parent, rc, ATOM_Widget::VScroll|ATOM_Widget::HScroll|ATOM_Widget::Border, ID_SCENEGRAPH_TREE, 8, 15, 16, ATOM_Widget::ShowNormal);
	_tcSceneGraph->setBorderMode (ATOM_Widget::Drop);
	_tcSceneGraph->allowItemDragDrop (true);
	CreateTreeItemsR (_editor, this, _tcSceneGraph, 0, _sceneRoot.get());

	_nodeMenu = ATOM_NEW(ATOM_PopupMenu, _editor->getGUIRenderer());
	_nodeMenu->appendMenuItem ("删除", ID_NODEEDIT_DELETE);
	_nodeMenu->appendMenuItem ("克隆", ID_NODEEDIT_CLONE);
	_nodeMenu->appendMenuItem ("清除变换", ID_NODEEDIT_CLEAR_XFORM);
	_nodeMenu->appendMenuItem ("清除子节点变换", ID_NODEEDIT_CLEAR_CHILDREN_XFORM);
}

static const char *getPostEffectClassDesc (ATOM_PostEffect *eff)
{
	if (dynamic_cast <ATOM_ColorGradingEffect*>(eff))
	{
		return "色彩曲线";
	}
	else if (dynamic_cast <ATOM_FXAAEffect*>(eff))
	{
		return "FXAA";
	}
	else if (dynamic_cast <ATOM_RainEffect*>(eff))			// wangjian added 
	{
		return "Rain";
	}
	else if (dynamic_cast <ATOM_RainLayerEffect*>(eff))		// wangjian added 
	{
		return "RainLayer";
	}
	else if (dynamic_cast <ATOM_SilhouetEffect*>(eff))		// wangjian added 
	{
		return "Silhouete";
	}
	else if (dynamic_cast <ATOM_BLOOMEffect*>(eff))			// wangjian added 
	{
		return "Bloom";
	}
	else if (dynamic_cast <ATOM_HeatHazeEffect*>(eff))		// wangjian added 
	{
		return "HeatHaze";
	}
	else if (dynamic_cast <ATOM_ColorFilterEffect*>(eff))	// wangjian added 
	{
		return "ColorFilter";
	}
	else if (dynamic_cast <ATOM_DOFEffect*>(eff))			// wangjian added 
	{
		return "DepthOfField";
	}
	else if (dynamic_cast <ATOM_SunShaftEffect*>(eff))			// wangjian added 
	{
		return "SunShaft";
	}
	else if (dynamic_cast <ATOM_SSAOEffect*>(eff))			// wangjian added 
	{
		return "SSAO";
	}

	return 0;
}

void PluginScene::createPostEffectList (ATOM_RealtimeCtrl *parent)
{
	ATOM_Rect2Di rc = parent->getClientRect ();
	rc.size.w = 200;

	_postEffectList = ATOM_NEW(ATOM_ListBox, parent, rc, 15, ATOM_Widget::HScroll|ATOM_Widget::Border, ID_POSTEFFECT_LIST, ATOM_Widget::ShowNormal);
	_postEffectList->setBorderMode (ATOM_Widget::Drop);

	_postEffectListMenu = ATOM_NEW(ATOM_PopupMenu, _editor->getGUIRenderer());
	_postEffectListMenu->appendMenuItem ("添加后处理效果", 0);
	ATOM_PopupMenu *submenu = _postEffectListMenu->createSubMenu (0);
	submenu->appendMenuItem ("色彩曲线", ID_ADD_CGPOSTEFFECT);
	submenu->appendMenuItem ("FXAA", ID_ADD_FXAAPOSTEFFECT);

	submenu->appendMenuItem ("Rain", ID_ADD_RAINPOSTEFFECT);					// wangjian added
	submenu->appendMenuItem ("RainLayer", ID_ADD_RAINLAYERPOSTEFFECT);			// wangjian added
	submenu->appendMenuItem ("Silhouete", ID_ADD_SILHOUETEPOSTEFFECT);			// wangjian added
	submenu->appendMenuItem ("Bloom", ID_ADD_BLOOMPOSTEFFECT);					// wangjian added
	submenu->appendMenuItem ("HeatHaze", ID_ADD_HEATHAZEPOSTEFFECT);			// wangjian added
	submenu->appendMenuItem ("ColorFilter", ID_ADD_COLORFILTERPOSTEFFECT);			// wangjian added
	submenu->appendMenuItem ("DOF", ID_ADD_DOFPOSTEFFECT);						// wangjian added
	submenu->appendMenuItem ("SSAO", ID_ADD_SSAOPOSTEFFECT);					// wangjian added

	_postEffectListMenu->appendMenuItem ("编辑..", ID_EDIT_POSTEFFECT);
	_postEffectListMenu->appendMenuItem ("删除", ID_DEL_POSTEFFECT);

	for (unsigned i = 0; i < _scene->getPostEffectChain()->getNumPostEffects (); ++i)
	{
		ATOM_PostEffect *eff = _scene->getPostEffectChain()->getEffect (i);
		if (eff)
		{
			_postEffectList->addItem (getPostEffectClassDesc (eff), (unsigned long long)eff);
		}
	}
}

void PluginScene::toggleLightNodeDisplay (void)
{
	_displayLightGeode = !_displayLightGeode;
	LightGeode::setDisplay (_displayLightGeode);
}

void PluginScene::toggleGrid (void)
{
	if (_grid)
	{
		if (_grid->getShow())
		{
			_grid->setShow (0);
		}
		else
		{
			_grid->setShow (1);
		}
	}
}

//--- wangjian added ---//
void PluginScene::toggleSceneNode(bool bTerrain/*=false*/)
{
	_hideAllSceneNode = !_hideAllSceneNode;

	class ATOM_HideAllSceneNodeVisitor: public ATOM_Visitor
	{
		bool _hide;
		bool _bHideTerrain;

	public:
		ATOM_HideAllSceneNodeVisitor (bool hide,bool bHideTerrain):_hide(hide),_bHideTerrain(bHideTerrain)
		{
		}

	public:
		virtual void visit (ATOM_Geode &node)
		{
			if( _bHideTerrain)
				return;

			node.setShow(!_hide);
		}

		virtual void visit (ATOM_Terrain &node)
		{
			if( _bHideTerrain)
				node.setShow(!_hide);

			return;
		}

		virtual void visit (ATOM_Sky &node)
		{
			if( _bHideTerrain)
				node.setShow(!_hide);

			return;
		}
	};

	ATOM_HideAllSceneNodeVisitor v(_hideAllSceneNode,bTerrain);
	v.traverse(*_scene->getRootNode());
}
void PluginScene::setPickFilterType(ATOM_STRING filterClassName)
{
	if( !_nodePickFilterMenu )
		return;
	
	if( !filterClassName.empty() )
	{
		_nodePickFilterMenu->setCheckById(ID_NODEEDIT_NODEPICKFILTER_ALL,false);
	}
	else
	{
		_pickFilterSet.clear();
		_nodePickFilterMenu->setCheckById(ID_NODEEDIT_NODEPICKFILTER_ALL,true);
		_nodePickFilterMenu->setCheckById(ID_NODEEDIT_NODEPICKFILTER_LIGHT,false);
	}

	bool bCheck = false;
	if( filterClassName == "LightGeode")
	{
		bCheck = !_nodePickFilterMenu->getCheckById(ID_NODEEDIT_NODEPICKFILTER_LIGHT);
		_nodePickFilterMenu->setCheckById(ID_NODEEDIT_NODEPICKFILTER_LIGHT,bCheck);
	}

	if( bCheck )
	{
		_pickFilterSet.insert(filterClassName);
	}
	else
	{
		if( _pickFilterSet.find(filterClassName)!=_pickFilterSet.end() )
			_pickFilterSet.erase(filterClassName);
	}
}
bool PluginScene::checkPickFilterType(ATOM_STRING strFilterClassName)
{
	if( _pickFilterSet.empty() )
		return true;
	if(_pickFilterSet.find(strFilterClassName)!=_pickFilterSet.end())
		return true;
	return false;
}
//----------------------//

void PluginScene::toggleLockCamera (void)
{
	if (_editor->getCameraModal() == _lockedCameraModal)
	{
		_oldCameraModal->setPosition (_lockedCameraModal->getPosition() + _lockedCameraModal->getDirection() - _oldCameraModal->getDirection());
		_editor->getCameraModal()->setFovY (_lockedCameraModal->getFovY ());
		_editor->getCameraModal()->setNearPlane (_lockedCameraModal->getNearPlane ());
		_editor->getCameraModal()->setFarPlane (_lockedCameraModal->getFarPlane ());
		_editor->setCameraModal(_oldCameraModal);
	}
	else
	{
		_oldCameraModal = _editor->getCameraModal();
		_lockedCameraModal->setFovY (_oldCameraModal->getFovY ());
		_lockedCameraModal->setNearPlane (_oldCameraModal->getNearPlane ());
		_lockedCameraModal->setFarPlane (_oldCameraModal->getFarPlane ());
		_editor->setCameraModal (_lockedCameraModal);
	}

	_editor->getMenuBar()->getMenuItem (AS_MENUITEM_VIEW).submenu->setCheckById (ID_VIEW_TOGGLELOCKCAMERA, 
		!_editor->getMenuBar()->getMenuItem (AS_MENUITEM_VIEW).submenu->getCheckById (ID_VIEW_TOGGLELOCKCAMERA));
} 

void PluginScene::alignCameraToSelectedNode (void)
{
	if (_editor->getCameraModal() == _lockedCameraModal)
	{
		::MessageBoxA (ATOM_APP->getMainWindow(), "摄像机已锁定!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
	}
	else if (!_selectedNode)
	{
		::MessageBoxA (ATOM_APP->getMainWindow(), "当前未选中任何节点!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
	}
	else
	{
		ATOM_Matrix4x4f mNode = _selectedNode->getWorldMatrix ();
		ATOM_Matrix4x4f m = ATOM_Matrix4x4f::getIdentityMatrix();
		ATOM_Vector3f t, s;
		mNode.decompose (t, m, s);
		m.setRow3 (3, t);

		ATOM_Vector3f eye, to, up;
		m.decomposeLookatLH (eye, to, up);
		_editor->getCameraModal()->setDirection (to - eye);
		_editor->getCameraModal()->setPosition (eye);
		_editor->getCameraModal()->setUpVector (up);
	}
}

void PluginScene::applyCameraToSelectedNode (void)
{
	if (!_selectedNode)
	{
		::MessageBoxA (ATOM_APP->getMainWindow(), "当前未选中任何节点!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
	}
	else
	{
		ATOM_Matrix4x4f mCam = _editor->getCameraModal()->getViewMatrix ();
		ATOM_Matrix4x4f mNode = _selectedNode->getWorldMatrix ();
		ATOM_Matrix4x4f m = ATOM_Matrix4x4f::getIdentityMatrix();
		ATOM_Vector3f t, s1, s2;
		mNode.decompose (t, m, s1);
		mCam.decompose (t, m, s2);
		s2 = s1;
		m.setRow3 (3, t);
		m >>= ATOM_Matrix4x4f::getScaleMatrix(s2);

		ATOM_Matrix4x4f mParent = _selectedNode->getParent()->getInvWorldMatrix ();
		_editor->doEditOp (ATOM_NEW(MyXFormNodeOp, this, _selectedNode.get(), mParent>>m));
	}
}

void PluginScene::toggleGUI (void)
{
	_isGUIShown = !_isGUIShown;

	if (_isGUIShown)
	{
		if (_tcSceneGraph)
		{
			_tcSceneGraph->show (ATOM_Widget::ShowNormal);
		}

		if (_postEffectList)
		{
			_postEffectList->show (ATOM_Widget::ShowNormal);
		}

		_editor->showScenePropEditor (true);

		if (_tbNodeProperties)
		{
			_tbNodeProperties->setBarVisible (true);
		}

		_editor->showAssetEditor (true);

		if (_tbTimePreview)
		{
			_tbTimePreview->setBarVisible (true);
		}
	}
	else
	{
		if (_tcSceneGraph)
		{
			_tcSceneGraph->show (ATOM_Widget::Hide);
		}

		if (_postEffectList)
		{
			_postEffectList->show (ATOM_Widget::Hide);
		}

		_editor->showScenePropEditor (false);

		if (_tbNodeProperties)
		{
			_tbNodeProperties->setBarVisible (false);
		}

		_editor->showAssetEditor (false);

		if (_tbTimePreview)
		{
			_tbTimePreview->setBarVisible (false);
		}
	}
}

bool PluginScene::saveSelectedNode (const char *filename)
{
	if (_selectedNode)
	{
		return ATOM_SaveObject (_selectedNode.get(), filename);
	}
	return false;
}

void PluginScene::cloneSelectedNode (void)
{
	if (_selectedNode && !_selectedNode->getPrivate())
	{
		ATOM_AUTOREF(ATOM_Node) newNode = _selectedNode->clone ();
		if (newNode)
		{
			_currentCreateAssetFileName.clear();
			_currentCreateAssetName.clear();
			_assetRotateX = 0.f;
			_assetRotateY = 0.f;
			_assetRotateZ = 0.f;
			_assetOffsetY = 0.f;
			_assetOffsetY2 = _selectedNode->getWorldMatrix().m31;
			_cloneSrc = _selectedNode.get();

			ATOM_LightNode *lightNode = dynamic_cast<ATOM_LightNode*>(_cloneSrc);
			if (lightNode)
			{
				ATOM_HARDREF(LightGeode) lightGeode;
				lightGeode->setLightType (lightNode->getLightType());
				lightGeode->load (ATOM_GetRenderDevice ());
				lightGeode->setPickable (1);
				newNode->appendChild (lightGeode.get());
			}

			beginCreateAsset (newNode.get(), false);
		}
	}
}

static bool isChildOf (ATOM_Node *child, ATOM_Node *parent)
{
	while (child)
	{
		if (child == parent)
		{
			return true;
		}
		child = child->getParent();
	}
	return false;
}

void PluginScene::handleEvent (ATOM_Event *event)
{
	int eventId = event->getEventTypeId ();

	if (eventId == ATOM_WidgetMouseMoveEvent::eventTypeId())
	{
		ATOM_WidgetMouseMoveEvent *e = (ATOM_WidgetMouseMoveEvent*)event;
		if (_isCreatingAsset && _replaceAsset)
		{
			_editor->getRealtimeCtrl()->pick (e->x, e->y, _pickVisitor);
			ATOM_Node *newNode = 0;

			if (_pickVisitor.getNumPicked() != 0) 
			{
				_pickVisitor.sortResults ();
				ATOM_Node *pickedNode = _pickVisitor.getPickResult(0).node.get();

				if (pickedNode != _currentCreateAsset.get()
					&& pickedNode->getParent()
					&& strcmp (pickedNode->getClassName(), ATOM_Node::_classname())
					&& strcmp (pickedNode->getClassName(), ATOM_Terrain::_classname())
					&& strcmp(pickedNode->getClassName(), ATOM_Atmosphere::_classname())
					)
				{
					newNode = pickedNode;
				}
			}

			if (newNode != _nodeToBeReplaced)
			{
				if (_nodeToBeReplaced)
				{
					_nodeToBeReplaced->setShow (ATOM_Node::SHOW);
					_nodeToBeReplaced = 0;
				}

				if (newNode)
				{
					_nodeToBeReplaced = newNode;
					newNode->setShow (ATOM_Node::HIDE_PICKABLE);
				}
			}
		}
		else if (_dlgTerrainTools && _dlgTerrainTools->isShown() && _dlgTerrainTools->isEditingOrientation ())
		{
			_dlgTerrainTools->updateOrientation (e->yrel);
		}
	}
	else if (eventId == ATOM_TreeItemDragStartEvent::eventTypeId())
	{
		ATOM_TreeItemDragStartEvent *e = (ATOM_TreeItemDragStartEvent*)event;
		e->dragSource->setIndicatorImageId (0);
		e->dragSource->setIndicatorRect (ATOM_Rect2Di(-10, -5, 20, 10));
		e->dragSource->addProperty ("index", e->index);
	}
	else if (eventId == ATOM_TreeItemDragDropEvent::eventTypeId())
	{
		ATOM_TreeItemDragDropEvent *e = (ATOM_TreeItemDragDropEvent*)event;
		int sourceIndex = e->dragSource->getProperty("index").getI();
		int destIndex = e->index; 
		if (sourceIndex != destIndex)
		{
			ATOM_AUTOPTR(ATOM_Node) destNode = (ATOM_Node*)_tcSceneGraph->getItemByIndex (destIndex)->getUserData();
			ATOM_AUTOPTR(ATOM_Node) srcNode = (ATOM_Node*)_tcSceneGraph->getItemByIndex (sourceIndex)->getUserData();
			reparentNode (srcNode.get(), destNode.get());
		}
	}
	else if (eventId == ATOM_WidgetCommandEvent::eventTypeId())
	{
		ATOM_WidgetCommandEvent *e = (ATOM_WidgetCommandEvent*)event;
		if (e->id >= ID_PICKEDNODELIST)
		{
			unsigned index = e->id - ID_PICKEDNODELIST;
			unsigned numPicked = _pickVisitor.getNumPicked();
			if (index < numPicked)
			{
				ATOM_Node *nodePicked = _pickVisitor.getPickResult(index).node.get();
				if (dynamic_cast<LightGeode*>(nodePicked))
				{
					nodePicked = nodePicked->getParent();
				}
				if (nodePicked != _selectedNode)
				{
					selectNode (nodePicked);
				}
			}
		}
		else if (e->id == ID_CREATE_LIGHT)
		{
			endEditHeight ();
			endCreateAsset (true, false);
			endAddRegion (_currentRegion.get(), true);
			endAddPath (_currentPath.get(), true);
			AS_Asset *asset = _editor->createAsset ("ATOM3D Light");
			if (asset && asset->getNode ())
			{
				if (asset->getNode ())
				{
					asset->getNode()->setDescribe ("Light");
					_currentCreateAssetFileName.clear ();
					_currentCreateAssetName = "ATOM3D Light";
					_assetRotateX = 0.f;
					_assetRotateY = 0.f;
					_assetRotateZ = 0.f;
					_assetOffsetY = 0.f;
					_assetOffsetY2 = 0.f;
					beginCreateAsset (asset->getNode (), false);
				}
				asset->deleteMe ();
			}
		}
		else if (e->id == ID_MANUAL_CREATE)
		{
			endEditHeight ();
			endCreateAsset (true, false);
			endAddRegion (_currentRegion.get(), true);
			endAddPath (_currentPath.get(), true);
		}
		else if (e->id == ID_PATH_EDITOR)
		{
			/*
			generateTerrainShadowMap ();
			startTimePreview ();
			*/
			getDlgPathEditor()->show (true);
		}
		else if (e->id == ID_NODEEDIT_MOVE)
		{
			if (_selectedNode && !_isCreatingAsset && !_isEditingHeight)
			{
				_editor->beginEditNodeTranslation (_selectedNode.get());
			}
		}
		else if (e->id == ID_NODEEDIT_ROTATE)
		{
			if (_selectedNode && !_isCreatingAsset && !_isEditingHeight)
			{
				_editor->beginEditNodeRotation (_selectedNode.get());
			}
		}
		else if (e->id == ID_NODEEDIT_SCALE)
		{
			if (_selectedNode && !_isCreatingAsset && !_isEditingHeight)
			{
				_editor->beginEditNodeScaling (_selectedNode.get());
			}
		}
		else if (e->id == ID_NODEEDIT_DELETE && !_isCreatingAsset && !_isEditingHeight)
		{
			deleteSelectedNode ();
		}
		else if (e->id == ID_NODEEDIT_CLONE && !_isCreatingAsset && !_isEditingHeight)
		{
			cloneSelectedNode ();
		}
		else if (e->id == ID_NODEEDIT_TOGGLELIGHTNODE)
		{
			toggleLightNodeDisplay ();
		}
		else if (e->id == ID_NODEEDIT_TOGGLEGRID)
		{
			toggleGrid ();
		}
		//--- wangjian added ---//
		else if(e->id == ID_NODEEDIT_TOGGLESCENENODEALL )
		{
			toggleSceneNode();
		}
		else if(e->id == ID_NODEEDIT_NODEPICKFILTER_ALL )
		{
			setPickFilterType("");
		}
		else if(e->id == ID_NODEEDIT_NODEPICKFILTER_LIGHT )
		{
			setPickFilterType("LightGeode");
		}
		else if(e->id == ID_NODEEDIT_GENCUBEMAP_GLOBAL )
		{
			genEnvMap(_scene->getCamera()->getPosition(),100,128,true,true/*,"env_2.dds"*/);
		}
		else if(e->id == ID_NODEEDIT_GENCUBEMAP_LOCAL )
		{
			genEnvMap(_scene->getCamera()->getPosition(),100,256,false,true/*,"env_2.dds"*/);
		}
		//----------------------//
		else if (e->id == ID_VIEW_TOGGLEGUI)
		{
			toggleGUI ();
		}
		else if (e->id == ID_VIEW_TOGGLELOCKCAMERA)
		{
			toggleLockCamera ();
		}
		else if (e->id == ID_ALIGN_CAMERA_TO_NODE)
		{
			alignCameraToSelectedNode ();
		}
		else if (e->id == ID_APPLY_CAMERA_TO_NODE)
		{
			applyCameraToSelectedNode ();
		}
		else if (e->id == ID_EDIT_TERRAIN)
		{
			beginEditTerrain ();
		}
		else if (e->id == ID_TOGGLE_HEIGHT_GRID)
		{
			showHeightGrid (!isHeightGridShown ());
		}
		else if (e->id == ID_TOGGLE_BLOCK_GRID)
		{
			HeightGrid *grid = getDlgHeightGen()->getGrid();
			if (grid->getSize().w > 0 && grid->getSize().h > 0)
			{
				bool b = !grid->isBlockShown();
				grid->showBlock (b);
				getDlgEditBlocks()->show (b);
			}
			else
			{
				MessageBoxA (ATOM_APP->getMainWindow(), "尚未生成高度!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
			}
		}
		else if (e->id == ID_BUILD_HEIGHT_GRID)
		{
			if (_isEditingHeight)
			{
				endEditHeight ();
			}

			if (getDlgHeightGen()->showModal () != DlgHeightGen::ID_BTNCANCEL)
			{
				showHeightGrid (true);
			}
		}
		else if (e->id == ID_EDIT_BLOCKS)
		{
			HeightGrid *grid = getDlgHeightGen()->getGrid();
			if (grid->getSize().w > 0 && grid->getSize().h > 0)
			{
				if (_isEditingHeight)
				{
					endEditHeight ();
				}

				if (_isCreatingAsset)
				{
					endCreateAsset (true, false);
				}

				endAddRegion (_currentRegion.get(), true);
				endAddPath (_currentPath.get(), true);

				if (_editor->isEditingNodeTransform ())
				{
					endEditNodeTransform ();
				}

				grid->showBlock (true);
				getDlgEditBlocks ()->show (true);
			}
			else
			{
				MessageBoxA (ATOM_APP->getMainWindow(), "尚未生成高度!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
			}
		}
		else if (e->id == ID_EDIT_HEIGHT)
		{
			beginEditHeight ();
		}
		else if (e->id == ID_LOAD_HEIGHT_DATA)
		{
			endEditHeight ();

			if (1 == _editor->getOpenFileNames ("dat", "数据文件(*.dat)|*.dat|", false, false, "载入高度文件"))
			{
				if (getDlgHeightGen()->loadHeightData (_editor->getOpenedFileName (0)))
				{
					showHeightGrid(true);
				}
			}
		}
		else if (e->id == ID_SAVE_HEIGHT_DATA)
		{
			if (1 == _editor->getOpenFileNames ("dat", "数据文件(*.dat)|*.dat|", false, true, "保存文件"))
			{
				getDlgHeightGen()->saveHeightData (_editor->getOpenedFileName (0));
			}
		}
		else if (e->id == ID_EXPORT_HEIGHT_MAP)
		{
			if (1 == _editor->getOpenFileNames ("png", "PNG图片|*.png|", false, true))
			{
				getDlgHeightGen()->exportHeightMap (_editor->getOpenedFileName (0));
			}
		}
		else if (e->id == ID_LOAD_BLOCK_DATA)
		{
			if (1 == _editor->getOpenFileNames ("dat", "数据文件(*.dat)|*.dat|", false, false, "载入障碍文件"))
			{
				if (getDlgHeightGen()->loadBlockData (_editor->getOpenedFileName (0)))
				{
					getDlgHeightGen()->getGrid()->showBlock (true);
				}
			}
		}
		else if (e->id == ID_SAVE_BLOCK_DATA)
		{
			if (1 == _editor->getOpenFileNames ("dat", "数据文件(*.dat)|*.dat|", false, true, "保存文件"))
			{
				getDlgHeightGen()->saveBlockData (_editor->getOpenedFileName (0));
			}
		}
		else if (e->id == ID_EXPORT_BLOCK_MAP)
		{
			if (1 == _editor->getOpenFileNames ("png", "PNG图片|*.png|", false, true))
			{
				getDlgHeightGen()->exportBlockMap (_editor->getOpenedFileName (0));
			}
		}
		else if (e->id == ID_EXPORT_SCENE_MAP)
		{
			if (1 == _editor->getOpenFileNames ("jpg", "JPG图片|*.jpg|", false, true))
			{
				int gridShow = _grid->getShow();
				_grid->setShow (ATOM_Node::HIDE);

				getDlgHeightGen()->exportSceneMap (_editor->getOpenedFileName (0));

				_grid->setShow (gridShow);
			}
		}
		else if (e->id == ID_HEIGHTMAP_INFO)
		{
			HeightGrid *grid = getDlgHeightGen()->getGrid();
			if (grid->getSize().w > 0 && grid->getSize().h > 0)
			{
				float x = getDlgHeightGen()->getRegion().point.x * getDlgHeightGen()->getCellSize();
				float y = getDlgHeightGen()->getRegion().point.y * getDlgHeightGen()->getCellSize();
				float w = grid->getSize().w * getDlgHeightGen()->getCellSize();
				float h = grid->getSize().h * getDlgHeightGen()->getCellSize();
				showHeightmapInfo (x, y, w, h);
			}
			else
			{
				MessageBoxA (ATOM_APP->getMainWindow(), "尚未生成高度!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
			}
		}
		else if (e->id == ID_NEW_HEIGHT_INDICATOR)
		{
			ATOM_AUTOREF(ATOM_ShapeNode) shapeNode = ATOM_HARDREF(ATOM_ShapeNode)();
			shapeNode->loadAttribute (nullptr);
			shapeNode->setType (ATOM_ShapeNode::PLANE);
			shapeNode->setSize (1.f);
			shapeNode->setO2T (ATOM_Matrix4x4f::getScaleMatrix(50.f));
			shapeNode->load (ATOM_GetRenderDevice());
			shapeNode->setPrivate (1);
			shapeNode->setDescribe (HEIGHT_INDICATOR_DESC);
			
			ATOM_AUTOREF(ATOM_Decal) decal = ATOM_HARDREF(ATOM_Decal)();
			decal->loadAttribute (nullptr);
			decal->setColor (ATOM_Vector4f(0.f, 1.f, 1.f, 1.f));
			decal->enableBlend (0);
			decal->load (ATOM_GetRenderDevice());
			decal->setTexture(ATOM_GetColorTexture (0xFFFFFFFF));
			decal->setPrivate (1);
			decal->setPickable (ATOM_Node::NONPICKABLE);
			decal->setO2T (ATOM_Matrix4x4f::getScaleMatrix(ATOM_Vector3f(2.f, 5000.f, 2.f)));
			shapeNode->appendChild (decal.get());

			_currentCreateAssetFileName.clear();
			_currentCreateAssetName.clear();
			_assetRotateX = 0.f;
			_assetRotateY = 0.f;
			_assetRotateZ = 0.f;
			_assetOffsetY = 0.f;
			_assetOffsetY2 = 0.f;

			beginCreateAsset (shapeNode.get(), false);
		}
		else if (e->id == ID_LOAD_NPC)
		{
			HeightGrid *grid = getDlgHeightGen()->getGrid();
			if (grid->getSize().w > 0 && grid->getSize().h > 0)
			{
				DlgLoadNPC *dlgLoadNPC = getDlgLoadNPC ();
				if (dlgLoadNPC)
				{
					dlgLoadNPC->showModal (grid->getBBoxTree ());
				}
			}
			else
			{
				MessageBoxA (ATOM_APP->getMainWindow(), "尚未生成高度!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
			}
		}
		else if (e->id == ID_SAVE_NPC)
		{
			if (_dlgLoadNPC)
			{
				if (!_dlgLoadNPC->save ())
				{
					MessageBoxA (ATOM_APP->getMainWindow(), "保存失败!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
				}
			}
		}
		else if (e->id == ID_EDIT_REGIONS)
		{
			getDlgRegionEdit()->show (true);
		}
		else if (e->id == ID_EDIT_TERRAIN)
		{
			if (_dlgTerrainTools)
			{
				_dlgTerrainTools->show (true);
			}
		}
		else if (e->id == ID_ADD_CGPOSTEFFECT)
		{
			ATOM_HARDREF(ATOM_ColorGradingEffect) cgEffect;
			_scene->getPostEffectChain ()->appendPostEffect (cgEffect.get());
			_postEffectList->addItem (getPostEffectClassDesc(cgEffect.get()), (unsigned long long)cgEffect.get());
			_editor->setDocumentModified (true);
		}
		else if (e->id == ID_ADD_FXAAPOSTEFFECT)
		{
			ATOM_HARDREF(ATOM_FXAAEffect) FXAAEffect;
			_scene->getPostEffectChain ()->appendPostEffect (FXAAEffect.get());
			_postEffectList->addItem (getPostEffectClassDesc(FXAAEffect.get()), (unsigned long long)FXAAEffect.get());
			_editor->setDocumentModified (true);
		}
		else if (e->id == ID_ADD_RAINPOSTEFFECT)				// wangjian added
		{
			ATOM_HARDREF(ATOM_RainEffect) rainEffect;
			_scene->getPostEffectChain ()->appendPostEffect (rainEffect.get());
			_postEffectList->addItem (getPostEffectClassDesc(rainEffect.get()), (unsigned long long)rainEffect.get());
			_editor->setDocumentModified (true);
		}
		else if (e->id == ID_ADD_RAINLAYERPOSTEFFECT)			// wangjian added
		{
			ATOM_HARDREF(ATOM_RainLayerEffect) rainLayerEffect;
			_scene->getPostEffectChain ()->appendPostEffect (rainLayerEffect.get());
			_postEffectList->addItem (getPostEffectClassDesc(rainLayerEffect.get()), (unsigned long long)rainLayerEffect.get());
			_editor->setDocumentModified (true);
		}
		else if (e->id == ID_ADD_SILHOUETEPOSTEFFECT)			// wangjian added
		{
			ATOM_HARDREF(ATOM_SilhouetEffect) SilhoueteEffect;
			_scene->getPostEffectChain ()->appendPostEffect (SilhoueteEffect.get());
			_postEffectList->addItem (getPostEffectClassDesc(SilhoueteEffect.get()), (unsigned long long)SilhoueteEffect.get());
			_editor->setDocumentModified (true);
		}
		else if (e->id == ID_ADD_BLOOMPOSTEFFECT)				// wangjian added
		{
			ATOM_HARDREF(ATOM_BLOOMEffect) BLOOMEffect;
			_scene->getPostEffectChain ()->appendPostEffect (BLOOMEffect.get());
			_postEffectList->addItem (getPostEffectClassDesc(BLOOMEffect.get()), (unsigned long long)BLOOMEffect.get());
			_editor->setDocumentModified (true);
		}
		else if (e->id == ID_ADD_HEATHAZEPOSTEFFECT)			// wangjian added
		{
			ATOM_HARDREF(ATOM_HeatHazeEffect) HeatHazeEffect;
			_scene->getPostEffectChain ()->appendPostEffect (HeatHazeEffect.get());
			_postEffectList->addItem (getPostEffectClassDesc(HeatHazeEffect.get()), (unsigned long long)HeatHazeEffect.get());
			_editor->setDocumentModified (true);
		}
		else if (e->id == ID_ADD_COLORFILTERPOSTEFFECT)			// wangjian added
		{
			ATOM_HARDREF(ATOM_ColorFilterEffect) ColorFilterEffect;
			_scene->getPostEffectChain ()->appendPostEffect (ColorFilterEffect.get());
			_postEffectList->addItem (getPostEffectClassDesc(ColorFilterEffect.get()), (unsigned long long)ColorFilterEffect.get());
			_editor->setDocumentModified (true);
		}
		else if (e->id == ID_ADD_DOFPOSTEFFECT)			// wangjian added
		{
			ATOM_HARDREF(ATOM_DOFEffect) DOFEffect;
			_scene->getPostEffectChain ()->appendPostEffect (DOFEffect.get());
			_postEffectList->addItem (getPostEffectClassDesc(DOFEffect.get()), (unsigned long long)DOFEffect.get());
			_editor->setDocumentModified (true);
		}
		else if (e->id == ID_ADD_SUNSHAFTPOSTEFFECT)			// wangjian added
		{
			ATOM_HARDREF(ATOM_SunShaftEffect) SunShaftEffect;
			_scene->getPostEffectChain ()->appendPostEffect (SunShaftEffect.get());
			_postEffectList->addItem (getPostEffectClassDesc(SunShaftEffect.get()), (unsigned long long)SunShaftEffect.get());
			_editor->setDocumentModified (true);
		}
		else if (e->id == ID_ADD_SSAOPOSTEFFECT)			// wangjian added
		{
			ATOM_HARDREF(ATOM_SSAOEffect) SSAOEffect;
			_scene->getPostEffectChain ()->appendPostEffect (SSAOEffect.get());
			_postEffectList->addItem (getPostEffectClassDesc(SSAOEffect.get()), (unsigned long long)SSAOEffect.get());
			_editor->setDocumentModified (true);
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if (e->id == ID_EDIT_POSTEFFECT)
		{
			int selected = _postEffectList->getSelectIndex ();
			if (selected >= 0)
			{
				ATOM_PostEffect *postEffect = (ATOM_PostEffect*)_postEffectList->getItemData (selected);
				if (postEffect)
				{
					_editor->setDocumentModified (true);

					ATOM_ColorGradingEffect *cgEffect = dynamic_cast<ATOM_ColorGradingEffect*>(postEffect);
					if (cgEffect)
					{
						editColorGradingEffect (cgEffect);
					}
				}
				
				selectPostEffect(postEffect);
			}
		}
		else if (e->id == ID_DEL_POSTEFFECT)
		{
			int selected = _postEffectList->getSelectIndex ();
			if (selected >= 0)
			{
				ATOM_PostEffect *postEffect = (ATOM_PostEffect*)_postEffectList->getItemData (selected);
				if (postEffect)
				{
					_scene->getPostEffectChain ()->removePostEffect (postEffect);
					_postEffectList->removeItem (selected);
					_editor->setDocumentModified (true);
				}
			}
		}
		else if (e->id == ID_NODEEDIT_CLEAR_XFORM)
		{
			if (_selectedNode)
			{
				_editor->doEditOp (ATOM_NEW(MyXFormNodeOp, this, _selectedNode.get(), ATOM_Matrix4x4f::getIdentityMatrix()));
			}
		}
		else if (e->id == ID_NODEEDIT_CLEAR_CHILDREN_XFORM)
		{
			if (_selectedNode)
			{
				for (unsigned i = 0; i < _selectedNode->getNumChildren(); ++i)
				{
					_selectedNode->getChild (i)->setO2T (ATOM_Matrix4x4f::getIdentityMatrix());
				}
			}
		}
	}
	else if (eventId == ATOM_WidgetLButtonDownEvent::eventTypeId())
	{
		if (!_tbTimePreview)
		{
			ATOM_WidgetLButtonDownEvent *e = (ATOM_WidgetLButtonDownEvent*)event;

			if (!_isCreatingAsset)
			{
				if (_beginCreatingRegion)
				{
					endAddRegion (_currentRegion.get(), false);
				}
				else if (_beginCreatePath)
				{
					endAddPath (_currentPath.get(), false);
				}

				if (_dlgTerrainTools && _dlgTerrainTools->isShown())
				{
					bool revert = (e->shiftState & KEYMOD_LSHIFT) != 0;
					_dlgTerrainTools->beginBrushing(revert);
				}
				else if (getDlgEditBlocks()->isShown())
				{
					ATOM_Ray ray;
					_editor->getRealtimeCtrl()->constructRay (e->x, e->y, ray);
					ATOM_Vector3f pos;
					float d;
					if (getDlgHeightGen()->getGrid()->getBBoxTree()->pick (ray, d))
					{
						pos = ray.getOrigin() + ray.getDirection() * d;
						ATOM_Texture *blockTexture = getDlgHeightGen()->getGrid()->getBlockTexture();
						bool set = getDlgEditBlocks()->getBrushMode()==BM_FILL;

						float cellSize = getDlgHeightGen()->getCellSize();
						const ATOM_Rect2Di &region = getDlgHeightGen()->getRegion ();
						int x = (pos.x - /*getDlgHeightGen()->getGrid()->getBBoxTree()->getRootNode()->bbox.getMin().x*/region.point.x * cellSize) / cellSize;
						int y = (pos.z - /*getDlgHeightGen()->getGrid()->getBBoxTree()->getRootNode()->bbox.getMin().z*/region.point.y * cellSize) / cellSize;
						int brushSize = getDlgEditBlocks()->getBrushSize ();
						getDlgHeightGen()->getGrid()->updateBlockData (x-brushSize/2, y-brushSize/2, brushSize, brushSize, getDlgEditBlocks()->getBlockType(), set);
						//ATOM_LOGGER::log ("pos=(%f,%f,%f) cell=(%d,%d)\n", pos.x, pos.y, pos.z, x, y);

						y = region.size.h - 2 - y;
						//float z = getDlgHeightGen()->getGrid()->getBBoxTree()->getRootNode()->bbox.getMax().z - pos.z;
						//y = (z - /*getDlgHeightGen()->getGrid()->getBBoxTree()->getRootNode()->bbox.getMin().z*/region.point.y * cellSize) / cellSize;
						x -= brushSize / 2;
						y -= brushSize / 2;
						int w = brushSize;
						int h = brushSize;
						getDlgHeightGen()->getGrid()->updateBlockTexture (x, y, w, h, getDlgEditBlocks()->getBrushColor(set ? getDlgEditBlocks()->getBlockType() : BT_None));
						//ATOM_LOGGER::log ("tex=(%d,%d)\n", x, y);
					}
				}
				else if (_isEditingHeight)
				{
					ATOM_Ray ray;
					_editor->getRealtimeCtrl()->constructRay (e->x, e->y, ray);
					ATOM_Vector3f pos;
					float d;
					if (getDlgHeightGen()->getGrid()->getBBoxTree()->pick (ray, d))
					{
						pos = ray.getOrigin() + ray.getDirection() * d;

						float cellSize = getDlgHeightGen()->getCellSize();
						const ATOM_Rect2Di &region = getDlgHeightGen()->getRegion ();
						int x = (pos.x - region.point.x * cellSize + 0.5f * cellSize) / cellSize;
						int y = (pos.z - region.point.y * cellSize + 0.5f * cellSize) / cellSize;
						if (x >= 0 && x < region.size.w && y >= 0 && y < region.size.h)
						{
							if (!_heightEditingPoint)
							{
								_heightEditingPoint = ATOM_HARDREF(ATOM_ShapeNode)();
								_heightEditingPoint->setType (ATOM_ShapeNode::SPHERE);
								_heightEditingPoint->skipClipTest (true);
								_heightEditingPoint->setPickable (ATOM_Node::NONPICKABLE);
								_heightEditingPoint->setPrivate (1);
							}
							float tx = (region.point.x + x) * cellSize;
							float ty = getDlgHeightGen()->getGrid()->getBBoxTree()->getHeight (x, y);
							float tz = (region.point.y + y) * cellSize;

							if (!_heightEditingPoint->getParent())
							{
								_scene->getRootNode()->appendChild (_heightEditingPoint.get());
							}

							_heightEditingPoint->setO2T (ATOM_Matrix4x4f::getTranslateMatrix(ATOM_Vector3f(tx, ty, tz)));
							_editingHeightX = x;
							_editingHeightY = y;
							_editor->beginEditNodeTranslation (_heightEditingPoint.get());
						}
					}
				}
				else if ( !_editor->isEditingNodeTransform ())
				{
					bool multiChoose = (e->shiftState & KEYMOD_SHIFT) != 0;

					_editor->getRealtimeCtrl()->pick (e->x, e->y, _pickVisitor);

					if (_pickVisitor.getNumPicked() != 0)
					{
						_pickVisitor.sortResults ();

						if (multiChoose)
						{
							ATOM_DELETE(_pickedNodeList);
							_pickedNodeList = ATOM_NEW(ATOM_PopupMenu, _editor->getGUIRenderer());

							for (unsigned i = 0; i < _pickVisitor.getNumPicked(); ++i)
							{
								ATOM_Node *node = _pickVisitor.getPickResult(i).node.get();

								ATOM_STRING className = node->getClassName();

								if (dynamic_cast<LightGeode*>(node))
								{
									node = node->getParent();
								}
								else if (dynamic_cast<ATOM_Decal*>(node))
								{
									ATOM_ShapeNode *shape = dynamic_cast<ATOM_ShapeNode*>(node->getParent());
									if (shape && shape->getDescribe()==HEIGHT_INDICATOR_DESC)
									{
										continue;
									}
								}

								//--- wangjian added ---//
								if( !checkPickFilterType( className ) )
									continue;
								//----------------------//

								const char *text = node->getDescribe().empty () ? node->getClassName() : node->getDescribe().c_str();
								_pickedNodeList->appendMenuItem (text, ID_PICKEDNODELIST + i);
							}
							_editor->getRealtimeCtrl()->trackPopupMenu (e->x, e->y, _pickedNodeList);
						}
						else
						{
							ATOM_Node *node = _pickVisitor.getPickResult(0).node.get();
							ATOM_STRING className = node->getClassName();
							if (dynamic_cast<LightGeode*>(node))
							{
								node = node->getParent();
							}
							else if (dynamic_cast<ATOM_Decal*>(node))
							{
								ATOM_ShapeNode *shape = dynamic_cast<ATOM_ShapeNode*>(node->getParent());
								if (shape && shape->getDescribe()==HEIGHT_INDICATOR_DESC)
								{
									node = nullptr;
								}
							}

							if (node && node != _selectedNode.get())
							{
								//--- wangjian modified ---//
								if( checkPickFilterType( className ) )
									selectNode (node);
								//-------------------------//
							}
						}
					}
					else
					{
						selectNode (0);
					}
				}
			}
		}
	}
	else if (eventId == ATOM_WidgetLButtonUpEvent::eventTypeId())
	{
		if (!_tbTimePreview)
		{
			ATOM_WidgetLButtonUpEvent *e = (ATOM_WidgetLButtonUpEvent*)event;
			if (_dlgTerrainTools && _dlgTerrainTools->isShown())
			{
				_dlgTerrainTools->endBrushing ();
			}
			else if (_isCreatingAsset)
			{
				endCreateAsset (false, (e->shiftState & KEYMOD_CTRL) != 0);
			}
			else if (_beginCreatingRegion)
			{
				endAddRegion (_currentRegion.get(), true);
			}
			else if (_beginCreatePath)
			{
				endAddPath (_currentPath.get(), true);
			}

		}
	}
	else if (eventId == ATOM_WidgetRButtonDownEvent::eventTypeId())
	{
		if (!_tbTimePreview)
		{
			ATOM_WidgetRButtonDownEvent *e = (ATOM_WidgetRButtonDownEvent*)event;
			if (_isCreatingAsset)
			{
				endCreateAsset (true, false);
			}
			endAddRegion (_currentRegion.get(), true);
			endAddPath (_currentPath.get(), true);
			if (_dlgTerrainTools && _dlgTerrainTools->isShown())
			{
				_dlgTerrainTools->beginOrientation ();
			}
		}
	}
	else if (eventId == ATOM_WidgetRButtonUpEvent::eventTypeId())
	{
		if (_dlgTerrainTools && _dlgTerrainTools->isShown())
		{
			_dlgTerrainTools->endOrientation ();
		}
	}
	else if (eventId == ATOM_WidgetKeyUpEvent::eventTypeId())
	{
		if (!_tbTimePreview)
		{
			ATOM_WidgetKeyUpEvent *e = (ATOM_WidgetKeyUpEvent*)event;
			if (e->key == KEY_DELETE && e->keymod == 0)
			{
				deleteSelectedNode ();
			}
			else if (e->key == KEY_ESCAPE && e->keymod == 0)
			{
				if (_isEditingHeight)
				{
					endEditHeight ();
				}

				if (_isCreatingAsset)
				{
					endCreateAsset (true, false);
				}

				endAddRegion (_currentRegion.get(), true);
				endAddPath (_currentPath.get(), true);

				if (_editor->isEditingNodeTransform ())
				{
					endEditNodeTransform ();
				}
				else
				{
					selectNode (0);
				}
			}
		}
	}
	else if (eventId == ATOM_WidgetResizeEvent::eventTypeId())
	{
		calcUILayout ();
	}
	else if (eventId == ATOM_TreeCtrlDblClickEvent::eventTypeId ())
	{
		ATOM_TreeCtrlDblClickEvent *e = (ATOM_TreeCtrlDblClickEvent*)event;
		if (e->id == _tcSceneGraph->getId())
		{
			ATOM_TreeItem *item = (ATOM_TreeItem*)_tcSceneGraph->getItemByIndex (e->itemIndex);
			ATOM_Node *node = item ? (ATOM_Node*)item->getUserData () : 0;

			if (node)
			{
				lookAtNode (node);
				selectNode (node);
				_editor->getGUIRenderer()->setFocus (_editor->getRealtimeCtrl());
			}
		}
	}
	else if (eventId == ATOM_TreeCtrlSelChangedEvent::eventTypeId())
	{
		ATOM_TreeCtrlSelChangedEvent *e = (ATOM_TreeCtrlSelChangedEvent*)event;
		if (e->id == ID_SCENEGRAPH_TREE)
		{
			ATOM_TreeItem *item = _tcSceneGraph->getSelectedItem ();
			selectNode (item ? (ATOM_Node*)item->getUserData() : NULL);
		}
	}
	else if (eventId == ATOM_WidgetContextMenuEvent::eventTypeId ())
	{
		ATOM_WidgetContextMenuEvent *e = (ATOM_WidgetContextMenuEvent*)event;
		if (e->id == ID_POSTEFFECT_LIST)
		{
			_postEffectList->trackPopupMenu (e->x, e->y, _postEffectListMenu);
		}
		else if (e->id == ID_SCENEGRAPH_TREE)
		{
			ATOM_TreeItem *item = _tcSceneGraph->getItemByIndex (e->item);
			if (item && ((ATOM_Node*)item->getUserData()) == _selectedNode.get())
			{
				item->getParent()->trackPopupMenu (e->x + item->getWidgetRect().point.x + item->getClientRect().point.x, e->y + item->getWidgetRect().point.y + item->getClientRect().point.y, _nodeMenu);
			}
		}
		//ATOM_LOGGER::log ("ContextMenuEvent: id=%d x=%d y=%d\n", e->id, e->x, e->y);
	}
	else if (eventId == ATOMX_TWValueChangedEvent::eventTypeId ())
	{
		ATOMX_TWValueChangedEvent *e = (ATOMX_TWValueChangedEvent*)event;
		if (e->bar == _tbNodeProperties)
		{
			_editor->setDocumentModified (true);

			if (e->bar == _tbNodeProperties && _selectedNode)
			{
				if (dynamic_cast<ATOM_ShapeNode*>(_selectedNode.get()) && e->name == "Material")
				{
					ATOM_AUTOREF(ATOM_Node) node = _selectedNode;
					selectNode (NULL);
					selectNode (node.get());
				}
				else if (e->id == ID_MATERIAL_PARAM)
				{
					ATOM_Material *material = (ATOM_Material*)e->userdata;

					switch (e->newValue.getType ())
					{
					case ATOMX_TBTYPE_FLOAT:
						material->getParameterTable()->setFloat (e->name.c_str(), e->newValue.getF());
						break;
					case ATOMX_TBTYPE_INT32:
						material->getParameterTable()->setInt (e->name.c_str(), e->newValue.getI());
						break;
					case ATOMX_TBTYPE_BOOL:
						material->getParameterTable()->setInt (e->name.c_str(), e->newValue.getB() ? 1 : 0);
						break;
					case ATOMX_TBTYPE_COLOR4F:
					case ATOMX_TBTYPE_VECTOR4F:
						{
							const float *v = e->newValue.get4F ();
							material->getParameterTable()->setVector (e->name.c_str(), ATOM_Vector4f(v[0], v[1], v[2], v[3]));
							break;
						}
					case ATOMX_TBTYPE_DIR3F:
						{
							const float *v = e->newValue.get3F ();
							material->getParameterTable()->setVector (e->name.c_str(), ATOM_Vector4f(v[0], v[1], v[2], 0.f));
							break;
						}
					case ATOMX_TBTYPE_STRING:
						{
							material->getParameterTable()->setTexture (e->name.c_str(), e->newValue.getS());
							break;
						}
					}
				}
				else if (e->name == "Describe")
				{
					ATOM_TreeItem *item = (ATOM_TreeItem*)_selectedNode->getGameEntity ();
					if (item)
					{
						item->setText (createNodeDescStr (_selectedNode.get()).c_str());
					}
				}
			}
		}
		else if (e->bar == _tbTimePreview)
		{
			ATOM_TimeManager::setCurrentTime (e->newValue.getF() * ATOM_TimeManager::numTimeKeys / 24.f);
		}
	}
	else if (eventId == ATOMX_TWCommandEvent::eventTypeId ())
	{
		ATOMX_TWCommandEvent *e = (ATOMX_TWCommandEvent*)event;
		if (e->bar == _tbTimePreview)
		{
			endTimePreview ();
		}
		else if (e->bar == _tbNodeProperties)
		{
			if (e->id == ID_MATERIAL_PARAM)
			{
				if (1 == _editor->getOpenImageFileNames (false, false, "选择贴图"))
				{
					ATOM_Material *material = (ATOM_Material*)e->userdata;
					material->getParameterTable()->setTexture (e->name.c_str(), _editor->getOpenedImageFileName(0));
				}
			}
		}
	}
	else if (eventId == AS_AssetCreateEvent::eventTypeId ())
	{
		endEditNodeTransform ();
		endCreateAsset (true, false);
		endEditHeight ();
		endAddRegion (_currentRegion.get(), true);
		endAddPath (_currentPath.get(), true);

		AS_AssetCreateEvent *e = (AS_AssetCreateEvent*)event;

		AS_Asset *asset = 0;

		if (e->filename)
		{
			asset = _editor->createAssetByFileName (e->filename);
			if (asset && asset->getNode ())
			{
				asset->getNode()->setDescribe (e->desc);
				_currentCreateAssetFileName = e->filename;
				_currentCreateAssetName.clear ();
				_assetRotateX = e->randomRotateX;
				_assetRotateY = e->randomRotateY;
				_assetRotateZ = e->randomRotateZ;
				_assetOffsetY = e->offsetY;
				_assetOffsetY2 = 0.f;
				beginCreateAsset (asset->getNode (), false);
			}
			else
			{
				char msg[512];
				sprintf (msg, "无法通过文件<%s>创建资源, 请检查文件是否正确!", e->filename);
				::MessageBoxA (ATOM_APP->getMainWindow(), msg, "ATOM3D Studio", MB_OK|MB_ICONHAND);
			}
		}
		else
		{
			asset = _editor->createAsset (e->name);
			if (asset && asset->getNode ())
			{
				asset->getNode()->setDescribe (e->desc);
				_currentCreateAssetFileName.clear ();
				_currentCreateAssetName = e->name;
				_assetRotateX = e->randomRotateX;
				_assetRotateY = e->randomRotateY;
				_assetRotateZ = e->randomRotateZ;
				_assetOffsetY = e->offsetY;
				_assetOffsetY2 = 0.f;
				beginCreateAsset (asset->getNode (), false);
			}
			else
			{
				char msg[512];
				sprintf (msg, "无法创建资源<%s>!", e->name);
				::MessageBoxA (ATOM_APP->getMainWindow(), msg, "ATOM3D Studio", MB_OK|MB_ICONHAND);
			}
		}
		if (asset)
		{
			asset->deleteMe ();
		}
	}
	else if (eventId == ATOM_DeviceResetEvent::eventTypeId ())
	{
		//--- wangjian modified ---//
		// 首先将地形的高度和alpha,法线恢复
		if (_dlgTerrainTools)
		{
			_dlgTerrainTools->restoreHeights ();
			_dlgTerrainTools->restoreAlphas ();
			_dlgTerrainTools->calcNormalMap ();
		}
		//-------------------------//

		HeightGrid *grid = getDlgHeightGen()->getGrid();
		if (grid->getSize().w > 0 && grid->getSize().h > 0)
		{
			getDlgHeightGen()->buildHeightGrid (nullptr, false);
		}
		
	}
	else if (eventId == AS_AssetReplaceEvent::eventTypeId ())
	{
		endEditNodeTransform ();
		endCreateAsset (true, false);
		endEditHeight ();
		endAddRegion (_currentRegion.get(), true);
		endAddPath (_currentPath.get(), true);

		AS_AssetReplaceEvent *e = (AS_AssetReplaceEvent*)event;

		AS_Asset *asset = 0;

		if (e->filename)
		{
			asset = _editor->createAssetByFileName (e->filename);
			if (asset && asset->getNode ())
			{
				asset->getNode()->setDescribe (e->desc);
				_currentCreateAssetFileName = e->filename;
				_currentCreateAssetName.clear ();
				_assetRotateX = 0.f;
				_assetRotateY = 0.f;
				_assetRotateZ = 0.f;
				_assetOffsetY = 0.f;
				_assetOffsetY2 = 0.f;
				beginCreateAsset (asset->getNode (), true);
			}
			else
			{
				char msg[512];
				sprintf (msg, "无法通过文件<%s>创建资源, 请检查文件是否正确!", e->filename);
				::MessageBoxA (ATOM_APP->getMainWindow(), msg, "ATOM3D Studio", MB_OK|MB_ICONHAND);
			}
		}
		else
		{
			asset = _editor->createAsset (e->name);
			if (asset && asset->getNode ())
			{
				asset->getNode()->setDescribe (e->desc);
				_currentCreateAssetFileName.clear ();
				_currentCreateAssetName = e->name;
				_assetRotateX = 0.f;
				_assetRotateY = 0.f;
				_assetRotateZ = 0.f;
				_assetOffsetY = 0.f;
				_assetOffsetY2 = 0.f;
				beginCreateAsset (asset->getNode (), true);
			}
			else
			{
				char msg[512];
				sprintf (msg, "无法创建资源<%s>!", e->name);
				::MessageBoxA (ATOM_APP->getMainWindow(), msg, "ATOM3D Studio", MB_OK|MB_ICONHAND);
			}
		}
		if (asset)
		{
			asset->deleteMe ();
		}
	}
}

bool PluginScene::showHeightGrid (bool show)
{
	HeightGrid *grid = getDlgHeightGen()->getGrid();
	if (grid->getSize().w > 0 && grid->getSize().h > 0)
	{
		grid->showGrid (show);
		if (!show)
		{
			endEditHeight ();
		}
	}
	return true;
}

bool PluginScene::isHeightGridShown (void)
{
	HeightGrid *grid = getDlgHeightGen()->getGrid();
	if (grid->getSize().w > 0 && grid->getSize().h > 0)
	{
		return grid->isGridShown ();
	}
	return false;
}

void PluginScene::beginEditTerrain (void)
{
	if (!_dlgTerrainTools)
	{
		return;
	}

	if (!_dlgTerrainTools->isShown())
	{
		if (_isCreatingAsset)
		{
			endCreateAsset (true, false);
		}

		endAddRegion (_currentRegion.get(), true);
		endAddPath (_currentPath.get(), true);

		if (_editor->isEditingNodeTransform ())
		{
			endEditNodeTransform ();
		}

		if (_isEditingHeight)
		{
			endEditHeight ();
		}

		if (getDlgEditBlocks()->isShown ())
		{
			getDlgEditBlocks()->show (false);
		}

		_dlgTerrainTools->show (true);
		_dlgTerrainTools->beginEdit ();
	}
}

void PluginScene::endEditTerrain (void)
{
	if (_dlgTerrainTools && _dlgTerrainTools->isShown())
	{
		_dlgTerrainTools->show (false);
		_dlgTerrainTools->endEdit ();
	}
}

void PluginScene::beginEditHeight (void)
{
	if (!_isEditingHeight)
	{
		HeightGrid *grid = getDlgHeightGen()->getGrid();
		if (grid->getSize().w > 0 && grid->getSize().h > 0)
		{
			selectNode (NULL);

			if (_isCreatingAsset)
			{
				endCreateAsset (true, false);
			}

			endAddRegion (_currentRegion.get(), true);
			endAddPath (_currentPath.get(), true);

			if (_editor->isEditingNodeTransform ())
			{
				endEditNodeTransform ();
			}

			showHeightGrid (true);
			grid->setGridColor (ATOM_Vector4f(0.f, 1.f, 0.f, 1.f));

			_isEditingHeight = true;
		}
		else
		{
			MessageBoxA (ATOM_APP->getMainWindow(), "尚未生成高度!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
		}
	}
}

void PluginScene::endEditHeight (void)
{
	if (_isEditingHeight)
	{
		_isEditingHeight = false;
		getDlgHeightGen()->getGrid()->setGridColor (ATOM_Vector4f(1.f, 1.f, 1.f, 1.f));

		endEditNodeTransform ();
		_heightEditingPoint->getParent()->removeChild (_heightEditingPoint.get());
	}
}

void PluginScene::editColorGradingEffect (ATOM_ColorGradingEffect *effect)
{
	_editor->showColorGradingEditor (true, effect);
}

void PluginScene::beginCreateAsset (ATOM_Node *asset, bool replace)
{
	endEditNodeTransform ();
	endEditHeight ();

	_currentCreateAsset = asset;

	if (_currentCreateAsset)
	{
		_isCreatingAsset = true;
		_replaceAsset = replace;
		_editor->showNodeAxis (asset, true);

		if (_assetRotateX != 0.f || _assetRotateY != 0.f || _assetRotateZ != 0.f)
		{
			srand (ATOM_GetTick ());

			ATOM_Matrix4x4f matrix;
			matrix.makeIdentity();
			if (_assetRotateZ != 0.f)
			{
				float f = float(rand())/float(RAND_MAX) - 0.5f;
				f *= _assetRotateZ;
				matrix >>= ATOM_Matrix4x4f::getRotateZMatrix (f);
			}
			if (_assetRotateY)
			{
				float f = float(rand())/float(RAND_MAX) - 0.5f;
				f *= _assetRotateY;
				matrix >>= ATOM_Matrix4x4f::getRotateYMatrix (f);
			}
			if (_assetRotateX)
			{
				float f = float(rand())/float(RAND_MAX) - 0.5f;
				f *= _assetRotateX;
				matrix >>= ATOM_Matrix4x4f::getRotateXMatrix (f);
			}
			_currentCreateAsset->setO2T (matrix);
		}

		_sceneRoot->appendChild (_currentCreateAsset.get());
		ATOM_GetRenderDevice()->showCustomCursor (ATOM_GetRenderDevice()->getCurrentView(), false);

		_editor->getGUIRenderer()->setFocus (_editor->getRealtimeCtrl());
	}
}

void PluginScene::endCreateAsset (bool cancel, bool localAppend)
{
	ATOM_AUTOREF(ATOM_Node) replaceParent;
	if (cancel && _replaceAsset && _nodeToBeReplaced)
	{
		_nodeToBeReplaced->setShow (ATOM_Node::SHOW);
	}

	if (_currentCreateAsset)
	{
		_isCreatingAsset = false;
		_editor->showNodeAxis (_currentCreateAsset.get(), false);

		ATOM_GetRenderDevice()->showCustomCursor (ATOM_GetRenderDevice()->getCurrentView(), true);

		if (cancel)
		{
			if (_currentCreateAsset->getParent())
			{
				_currentCreateAsset->getParent()->removeChild (_currentCreateAsset.get());
			}
			_currentCreateAsset = 0;
			_currentCreateAssetFileName.clear ();
			_currentCreateAssetName.clear ();
			_cloneSrc = 0;
		}
		else
		{
			ATOM_Node *parent = _currentCreateAsset->getParent ();
			parent->removeChild (_currentCreateAsset.get());

			if (_replaceAsset && _nodeToBeReplaced)
			{
				_nodeToBeReplaced->setShow (ATOM_Node::SHOW);
				_editor->doEditOp (ATOM_NEW(MyReplaceNodeOp, _currentCreateAsset.get(), _nodeToBeReplaced.get(), _tcSceneGraph, this));
			}
			else
			{
				if (localAppend)
				{
					if (!_selectedNode)
					{
						::MessageBoxA (ATOM_APP->getMainWindow(), "当前未选择任何节点!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
						_nodeToBeReplaced = 0;
						return;
					}
					const ATOM_Matrix4x4f &matWorld = _currentCreateAsset->getWorldMatrix ();
					const ATOM_Matrix4x4f &matInvParentWorld = _selectedNode->getInvWorldMatrix ();
					_currentCreateAsset->setO2T (matInvParentWorld >> matWorld);
					parent = _selectedNode.get();
				}

				_editor->doEditOp (ATOM_NEW(MyCreateNodeOp, _currentCreateAsset.get(), parent, _tcSceneGraph, this));
			}
			_editor->setDocumentModified (true);

			if (_cloneSrc)
			{
				ATOM_AUTOREF(ATOM_Node) newNode = _cloneSrc->clone ();
				if (newNode)
				{
					_currentCreateAsset = 0;

					ATOM_LightNode *lightNode = dynamic_cast<ATOM_LightNode*>(_cloneSrc);
					if (lightNode)
					{
						ATOM_HARDREF(LightGeode) lightGeode;
						lightGeode->setLightType (lightNode->getLightType());
						lightGeode->load (ATOM_GetRenderDevice ());
						lightGeode->setPickable (1);
						newNode->appendChild (lightGeode.get());
					}

					beginCreateAsset (newNode.get(), false);
				}
			}
			else if (!dynamic_cast<ATOM_Terrain*>(_currentCreateAsset.get()) && !dynamic_cast<ATOM_BkImage*>(_currentCreateAsset.get()))
			{
				AS_Asset *asset = _currentCreateAssetFileName.empty ()
					? _editor->createAsset (_currentCreateAssetName.c_str())
					: _editor->createAssetByFileName (_currentCreateAssetFileName.c_str());
				if (asset && asset->getNode ())
				{
					asset->getNode()->setDescribe (_currentCreateAsset->getDescribe());
					_currentCreateAsset = 0;

					if (asset->supportBrushing ())
					{
						beginCreateAsset (asset->getNode (), _replaceAsset);
					}
				}
				else
				{
					_currentCreateAsset = 0;
				}

				if (asset)
				{
					asset->deleteMe ();
				}
			}
			else
			{
				_currentCreateAsset = 0;
			}
		}
	}
	_nodeToBeReplaced = 0;

	ATOM_Terrain *terrain = findTerrain ();
	if (!terrain && _dlgTerrainTools)
	{
		ATOM_DELETE(_dlgTerrainTools);
		_dlgTerrainTools = 0;
	}
	else if (terrain)
	{
		if (_dlgTerrainTools && _dlgTerrainTools->getTerrain() != terrain)
		{
			ATOM_DELETE(_dlgTerrainTools);
			_dlgTerrainTools = ATOM_NEW(DlgTerrainTools, this, terrain, _editor->getRealtimeCtrl());
		}
		else if (!_dlgTerrainTools)
		{
			_dlgTerrainTools = ATOM_NEW(DlgTerrainTools, this, terrain, _editor->getRealtimeCtrl());
		}
	}
}

void PluginScene::calcUILayout (void)
{
	ATOM_Rect2Di rc = _editor->getRealtimeCtrl()->getClientRect();

	if (_tcSceneGraph && _postEffectList)
	{
		_tcSceneGraph->resize (ATOM_Rect2Di(0, 0, 200, rc.size.h / 3));
		_postEffectList->resize (ATOM_Rect2Di(0, rc.size.h / 3, 200, rc.size.h / 3));
		_editor->setAssetEditorPosition (0, rc.size.h * 2 / 3, 200, rc.size.h - rc.size.h * 2 / 3);

		int h = _tbNodeProperties ? rc.size.h / 2 : rc.size.h;
		ATOM_Point2Di pt(rc.size.w - 300, 0);
		_editor->getRealtimeCtrl()->clientToGUI (&pt);
		_editor->setScenePropEditorPosition (pt.x, pt.y, 300, h);

		if (_tbNodeProperties)
		{
			_tbNodeProperties->setBarPosition (pt.x, pt.y + h);
			_tbNodeProperties->setBarSize (300, rc.size.h - h);
		}
	}
}

void PluginScene::lookAtNode (ATOM_Node *node)
{
	_editor->lookAtNode (node);
	//const ATOM_BBox &bbox = node->getWorldBoundingbox ();

	//ATOM_Vector3f direction = bbox.getMin() - bbox.getMax();
	//float length = direction.getLength ();
	//if (length < 50.f)
	//{
	//	direction *= (50.f / length);
	//}
	//ATOM_Vector3f position = bbox.getCenter() - direction;
	//direction.normalize ();

	//_editor->getCameraModal ()->setDirection (direction);
	//_editor->getCameraModal ()->setPosition (position);
	//_editor->getCameraModal ()->setUpVector (ATOM_Vector3f (0.f, 1.f, 0.f));
}

ATOM_Scene *PluginScene::getScene (void) const
{
	return _scene;
}

ATOM_Node *PluginScene::getSelectedNode (void) const
{
	return _selectedNode.get();
}

void PluginScene::selectNode (ATOM_Node *node)
{
	endEditHeight ();

	if (node != _selectedNode)
	{
		if (_selectedNode)
		{
			endEditNodeTransform ();
			_selectedNode->setDrawBoundingbox (0);
		}

		_selectedNode = node;

		if (_selectedNode)
		{
			_selectedNode->setDrawBoundingbox (0);
			_nodeTransformBeforeEdit = _selectedNode->getO2T ();

			if (!_tbNodeProperties)
			{
				_tbNodeProperties = ATOM_NEW(ATOMX_PropertyTweakBar, "Properties");
				_tbNodeProperties->setBarColor (0.5f, 0.5f, 0.5f, 1.f);
				_tbNodeProperties->setBarMovable (false);
				_tbNodeProperties->setBarResizable (false);
				calcUILayout ();
			}

			_tbNodeProperties->clear ();
			_tbNodeProperties->addButton ("%%Select", "Select", "Node operations", &NodeSelectionEditCallback, this);
			_tbNodeProperties->addButton ("%%Move", "Move", "Node operations", &NodeTranslationEditCallback, this);
			_tbNodeProperties->addButton ("%%Rotate", "Rotate", "Node operations", &NodeRotationEditCallback, this);
			_tbNodeProperties->addButton ("%%Scale", "Scale", "Node operations", &NodeScaleEditCallback, this);
			_tbNodeProperties->addButton ("%%Delete", "Delete", "Node operations", &NodeDeleteCallback, this);
			_tbNodeProperties->addButton ("%%Clone", "Clone", "Node operations", &NodeCloneCallback, this);
			_tbNodeProperties->setObject (_selectedNode.get(), false);

			ATOM_ShapeNode *shape = dynamic_cast<ATOM_ShapeNode*>(_selectedNode.get());
			if (shape)
			{
				setupMaterialTweakBar (_tbNodeProperties, shape->getMaterial());
			}

			_tbNodeProperties->addButton ("%%Save", "Save", "Node operations", &NodeSaveCallback, this);

			ATOM_TreeItem *item = (ATOM_TreeItem*)_selectedNode->getGameEntity();
			if (item)
			{
				_tcSceneGraph->selectItem (item, false);
			}

			_editor->endEditNodeTransform ();
			_selectedNode->setDrawBoundingbox (1);

			if (_dlgPathEditor)
			{
				int path = _dlgPathEditor->getPathByCtlPointNode (_selectedNode.get());
				if (path >= 0)
				{
					_dlgPathEditor->selectPath (path);
					_dlgPathEditor->setCurrentCtlPoint (path>=0 ? _selectedNode.get() : nullptr);
				}
			}
		}
		else
		{
			_tcSceneGraph->selectItem (0, false);

			ATOM_DELETE(_tbNodeProperties);
			_tbNodeProperties = 0;
			calcUILayout ();

			if (_dlgPathEditor)
			{
				_dlgPathEditor->selectPath (-1);
				_dlgPathEditor->setCurrentCtlPoint (nullptr);
			}
		}
	}
}

//--- wangjian added ---//
void PluginScene::selectPostEffect(ATOM_PostEffect* pe)
{
	selectNode(0);

	endEditHeight ();

	if (pe != _selectPostEffect)
	{
		
		_selectPostEffect = pe;

		if (_selectPostEffect)
		{
			if (!_tbNodeProperties)
			{
				_tbNodeProperties = ATOM_NEW(ATOMX_PropertyTweakBar, "Properties");
				_tbNodeProperties->setBarColor (0.5f, 0.5f, 0.5f, 1.f);
				_tbNodeProperties->setBarMovable (false);
				_tbNodeProperties->setBarResizable (false);
				calcUILayout ();
			}

			_tbNodeProperties->clear ();
			_tbNodeProperties->setObject (_selectPostEffect.get(), false);
		}
		else
		{
			_tcSceneGraph->selectItem (0, false);

			ATOM_DELETE(_tbNodeProperties);
			_tbNodeProperties = 0;
			calcUILayout ();
		}
	}
}
//----------------------//

class MyDeleteNodeOp: public AS_Operation
{
	ATOM_AUTOREF(ATOM_Node) _child;
	ATOM_AUTOREF(ATOM_Node) _parent;
	ATOM_TreeCtrl *_treeCtrl;
	PluginScene *_pluginScene;

public:
	MyDeleteNodeOp (ATOM_Node *nodeChild, ATOM_Node *nodeParent, ATOM_TreeCtrl *treeCtrl, PluginScene *pluginScene)
		: _child(nodeChild)
		, _parent(nodeParent)
		, _treeCtrl(treeCtrl)
		, _pluginScene(pluginScene)
	{

	}

	virtual void undo (void)
	{
		_parent->appendChild (_child.get());

		ATOM_TreeItem *item = (ATOM_TreeItem*)_parent->getGameEntity();
		ATOM_TreeItem *subItem = _treeCtrl->newItem (_child->getDescribe().c_str(), ATOM_IMAGEID_TREECTRL_EXPANDED_IMAGE, ATOM_IMAGEID_TREECTRL_COLLAPSED_IMAGE, item);
		_child->setGameEntity ((ATOM_GameEntity*)subItem);
		subItem->setUserData ((unsigned)_child.get());
		_pluginScene->selectNode (_child.get());
		_pluginScene->refreshPathList ();
	}

	virtual void redo (void)
	{
		_pluginScene->selectNode (0);
		ATOM_TreeItem *item = (ATOM_TreeItem*)_child->getGameEntity();
		_parent->removeChild (_child.get());
		if (item)
		{
			_treeCtrl->deleteItem (item);
		}
		_pluginScene->refreshPathList ();
	}

	virtual void deleteMe (void)
	{
		ATOM_DELETE(this);
	}
};

void PluginScene::deleteSelectedNode (void)
{
	endEditNodeTransform ();

	if (_selectedNode && _selectedNode->getParent() && (!_selectedNode->getPrivate()))
	{
		_editor->doEditOp (ATOM_NEW(MyDeleteNodeOp, _selectedNode.get(), _selectedNode->getParent(), _tcSceneGraph, this));
		//ATOM_TreeItem *item = (ATOM_TreeItem*)_selectedNode->getGameEntity();

		//ATOM_AUTOREF(ATOM_Node) node = _selectedNode;
		//selectNode (0);

		//node->getParent()->removeChild (node.get());

		//if (item)
		//{
		//	_tcSceneGraph->deleteItem (item);
		//}

		ATOM_Terrain *terrain = findTerrain ();
		if (!terrain && _dlgTerrainTools)
		{
			ATOM_DELETE(_dlgTerrainTools);
			_dlgTerrainTools = 0;
		}

		_editor->setDocumentModified (true);
	}
}

void PluginScene::reparentNode (ATOM_Node *childNode, ATOM_Node *parentNode)
{
	endEditNodeTransform ();

	if (childNode && parentNode && childNode->getParent() && !isChildOf (parentNode, childNode))
	{
		_editor->doEditOp (ATOM_NEW(MyReparentNodeOp, childNode, parentNode, _tcSceneGraph, this));

		_editor->setDocumentModified (true);
	}
}

bool __cdecl myFilter (ATOM_Node *node, void *userData)
{
	AS_Editor *editor = (AS_Editor*)userData;
	return !editor->isDummyNodeType (node->getClassName());
}

bool PluginScene::saveFile (const char *filename)
{
	detachDummyNodes ();

	_scene->setFixedCameraVector (_lockedCameraModal->getDirection());
	_scene->setFixedCameraPosition (_lockedCameraModal->getPosition());
	_scene->setPerspectiveParams (_editor->getCameraModal()->getFovY(), _editor->getCameraModal()->getNearPlane(), _editor->getCameraModal()->getFarPlane());
	_scene->setCameraMatrix (_editor->getCameraModal()->getViewMatrix ());
	_scene->clearPathes ();
	if (_dlgPathEditor)
	{
		for (int i = 0; i < _dlgPathEditor->getNumPaths(); ++i)
		{
			ATOM_Path3D path;
			_dlgPathEditor->createPath (i, &path);
			_scene->addPath (_dlgPathEditor->getPathName (i), path);
		}
	}

	bool result = _scene->save (filename, 0, &myFilter, _editor);

	attachDummyNodes ();

	if (result)
	{
		_editor->resetScenePropEditor ();

		if (_dlgTerrainTools)
		{
			_dlgTerrainTools->updateBaseMap ();
			_dlgTerrainTools->getTerrain()->save (NULL);
		}
	}

	if (_selectedNode && _editor->isEditingNodeTransform())
	{
		_nodeTransformBeforeEdit = _selectedNode->getO2T();
	}

	return result;
}

struct GeodeInfo
{
	ATOM_STRING modelFileName;
	ATOM_STRING transform;
};

static void doNodes (ATOM_TiXmlElement *nodeElement, ATOM_VECTOR<GeodeInfo> &geodeRecord, ATOM_VECTOR<ATOM_TiXmlElement*> &removeList, int &numTerrains)
{
	const char *attrib = nodeElement->Attribute ("class");
	if (attrib)
	{
		if (!strcmp (attrib, ATOM_Terrain::_classname()))
		{
			numTerrains++;
		}
		else if (!strcmp (attrib, ATOM_Geode::_classname()))
		{
			const char *modelFileName = nodeElement->Attribute ("ModelFileName");
			const char *transform = nodeElement->Attribute ("matrix");
			if (modelFileName && transform)
			{
				bool exists = false;
				for (unsigned i = 0; i < geodeRecord.size(); ++i)
				{
					if (geodeRecord[i].modelFileName == modelFileName && geodeRecord[i].transform == transform)
					{
						exists = true;
						removeList.push_back (nodeElement);
						break;
					}
				}
				if (!exists)
				{
					geodeRecord.resize (geodeRecord.size() + 1);
					geodeRecord.back().modelFileName = modelFileName;
					geodeRecord.back().transform = transform;
				}
			}
		}
	}

	for (ATOM_TiXmlElement *p = nodeElement->FirstChildElement ("node"); p; p = p->NextSiblingElement ("node"))
	{
		doNodes (p, geodeRecord, removeList, numTerrains);
	}
}

void PluginScene::checkDuplicatedGeodes (const char *filename) const
{
	ATOM_VECTOR<GeodeInfo> geodeRecord;
	ATOM_VECTOR<ATOM_TiXmlElement*> removeList;

	char buffer[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (filename, buffer);
	ATOM_TiXmlDocument doc(buffer);
	if (!doc.LoadFile ())
	{
		return;
	}

	ATOM_TiXmlElement *eRoot = doc.RootElement ();

	if (!eRoot)
	{
		return;
	}

	int numTerrains = 0;

	for (ATOM_TiXmlElement *eNodes = eRoot->FirstChildElement ("Nodes"); eNodes; eNodes = eNodes->NextSiblingElement ("Nodes"))
	{
		doNodes (eNodes, geodeRecord, removeList, numTerrains);
	}

	if (numTerrains > 1)
	{
		char buffer[1024];
		sprintf (buffer, "警告：发现场景中有%d个地形对象，可能会降低渲染效率!", numTerrains);
		::MessageBoxA (ATOM_APP->getMainWindow(), buffer, "ATOM3D Studio", MB_OK|MB_ICONWARNING);
	}

	if (removeList.size() > 0)
	{
		if (IDYES == ::MessageBoxA (ATOM_APP->getMainWindow(), "发现场景中位置重复的模型，是否现在清除？\n注意，此操作不可撤销！", "ATOM3D Studio", MB_YESNO|MB_ICONQUESTION))
		{
			for (unsigned i = 0; i < removeList.size(); ++i)
			{
				ATOM_TiXmlNode *parent = removeList[i]->Parent();
				assert (parent);
				parent->RemoveChild (removeList[i]);
			}

			if (!doc.SaveFile (buffer))
			{
				::MessageBoxA (ATOM_APP->getMainWindow(), "保存场景文件失败.", "ATOM3D Studio", MB_OK|MB_ICONHAND);
			}
			else
			{
				char msg[256];
				sprintf (msg, "%d个重复模型已删除.", removeList.size());
				::MessageBoxA (ATOM_APP->getMainWindow(), msg, "ATOM3D Studio", MB_OK|MB_ICONINFORMATION);
			}
		}
	}
}

void addLightGeodeToLightNodeR (ATOM_Node *node)
{
	ATOM_LightNode *lightNode = dynamic_cast<ATOM_LightNode*>(node);
	if (lightNode)
	{
		ATOM_HARDREF(LightGeode) lightGeode;
		lightGeode->setLightType ((ATOM_Light::LightType)lightNode->getLightType ());
		lightGeode->load (ATOM_GetRenderDevice ());
		node->appendChild (lightGeode.get());
	}

	for (unsigned i = 0; i < node->getNumChildren(); ++i)
	{
		addLightGeodeToLightNodeR (node->getChild (i));
	}
}

void PluginScene::addLightGeodeToScene (void)
{
	addLightGeodeToLightNodeR (_scene->getRootNode ());
}

void detachDummyNodesR (AS_Editor *editor, ATOM_Node *node, ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)> &parents, ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)> &children)
{
	if (editor->isDummyNodeType (node->getClassName()) && node->getParent())
	{
		parents.push_back (node->getParent ());
		children.push_back (node);
		node->getParent()->removeChild (node);
	}
	else
	{
		for (unsigned i = 0; i < node->getNumChildren (); ++i)
		{
			detachDummyNodesR (editor, node->getChild(i), parents, children);
		}
	}
}

void PluginScene::detachDummyNodes (void)
{
	detachDummyNodesR (_editor, _scene->getRootNode(), _tempNodesParent, _tempNodesChild);
}

void PluginScene::attachDummyNodes (void)
{
	for (unsigned i = 0; i < _tempNodesChild.size(); ++i)
	{
		_tempNodesParent[i]->appendChild (_tempNodesChild[i].get());
	}
	_tempNodesParent.resize(0);
	_tempNodesChild.resize(0);
}

unsigned PluginScene::getMinWindowWidth (void) const
{
	return 600;
}

unsigned PluginScene::getMinWindowHeight (void) const
{
	return 450;
}

void PluginScene::beginEditNodeTranslation (void)
{
	if (_selectedNode)
	{
		endEditNodeTransform ();

		_nodeTransformBeforeEdit = _selectedNode->getO2T ();
		_editor->beginEditNodeTranslation (_selectedNode.get());
	}
}

void PluginScene::beginEditNodeRotation (void)
{
	if (_selectedNode)
	{
		endEditNodeTransform ();

		_nodeTransformBeforeEdit = _selectedNode->getO2T ();
		_editor->beginEditNodeRotation (_selectedNode.get());
	}
}

void PluginScene::beginEditNodeScale (void)
{
	if (_selectedNode)
	{
		endEditNodeTransform ();

		_nodeTransformBeforeEdit = _selectedNode->getO2T ();
		_editor->beginEditNodeScaling (_selectedNode.get());
	}
}

void PluginScene::endEditNodeTransform (void)
{
	_editor->endEditNodeTransform ();

	if (_selectedNode)
	{
		if (_selectedNode->getO2T () != _nodeTransformBeforeEdit)
		{
			_editor->setDocumentModified (true);
		}

		if (_dlgPathEditor)
		{
			int path = _dlgPathEditor->getPathByCtlPointNode (_selectedNode.get());
			if (path >= 0)
			{
				_dlgPathEditor->refreshPathTrail (path);
			}
		}
	}
}

void ATOMX_CALL PluginScene::NodeSelectionEditCallback(void *userdata)
{
	PluginScene *p = (PluginScene*)userdata;
	p->endEditNodeTransform ();
}

void ATOMX_CALL PluginScene::NodeTranslationEditCallback(void *userdata)
{
	PluginScene *p = (PluginScene*)userdata;
	p->beginEditNodeTranslation ();
}

void ATOMX_CALL PluginScene::NodeRotationEditCallback(void *userdata)
{
	PluginScene *p = (PluginScene*)userdata;
	p->beginEditNodeRotation ();
}

void ATOMX_CALL PluginScene::NodeScaleEditCallback(void *userdata)
{
	PluginScene *p = (PluginScene*)userdata;
	p->beginEditNodeScale ();
}

void ATOMX_CALL PluginScene::NodeDeleteCallback(void *userdata)
{
	PluginScene *p = (PluginScene*)userdata;
	p->deleteSelectedNode ();
}

void ATOMX_CALL PluginScene::NodeCloneCallback (void *userdata)
{
	PluginScene *p = (PluginScene*)userdata;
	p->cloneSelectedNode ();
}

void ATOMX_CALL PluginScene::NodeSaveCallback (void *userdata)
{
	PluginScene *p = (PluginScene*)userdata;
	if (p->_selectedNode)
	{
		ATOM_FileDlg dlg(ATOM_FileDlg::MODE_SAVEFILE|ATOM_FileDlg::MODE_OVERWRITEPROMPT, NULL, "xml", NULL, "xml文件|*.xml|", ATOM_APP->getMainWindow());
		if (dlg.doModal () == IDOK)
		{
			if (!p->saveSelectedNode (dlg.getSelectedFileName(0)))
			{
				::MessageBoxA (ATOM_APP->getMainWindow(), "文件保存失败!", "ATOM3D Studio", MB_OK|MB_ICONHAND);
			}
		}
	}
}

//--- wangjian added ---//
//void ATOMX_CALL PluginScene::SceneAsyncLoadCallback (void *userdata)
//{
//	PluginScene *p = (PluginScene*)userdata;
//	if( p )
//		p->ProcessAfterSceneAsyncLoad ();
//}
//void PluginScene::ProcessAfterSceneAsyncLoad()
//{
//	//return;
//
//	/*if (1 != _scene->getRootNode()->getNumChildren() || strcmp(_scene->getRootNode()->getChild(0)->getDescribe().c_str(), "World"))
//	{
//		_sceneRoot = ATOM_HARDREF(ATOM_Node)();
//		_sceneRoot->setDescribe("World");
//
//		ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)> nodes;
//		for (unsigned i = 0; i < _scene->getRootNode()->getNumChildren(); ++i)
//		{
//			nodes.push_back(_scene->getRootNode()->getChild(i));
//		}
//		_scene->getRootNode()->clearChildren();
//
//		for (unsigned i = 0;i < nodes.size(); ++i)
//		{
//			_sceneRoot->appendChild (nodes[i].get());
//		}
//		_scene->getRootNode()->appendChild (_sceneRoot.get());
//	}
//	else
//	{
//		_sceneRoot = _scene->getRootNode()->getChild(0);
//	}
//
//	addLightGeodeToScene ();
//
//	createTweakBars ();*/
//	//createSceneGraphTree (_editor->getRealtimeCtrl());
//	//_sceneRoot = _scene->getRootNode()->getChild(0);
//	addLightGeodeToScene ();
//	_tcSceneGraph->clear();
//	CreateTreeItemsR (_editor, this, _tcSceneGraph, 0, /*_sceneRoot.get()*/_scene->getRootNode());
//	/*createPostEffectList (_editor->getRealtimeCtrl());
//	_editor->showAssetEditor (true);
//
//	calcUILayout ();
//
//	setupMenu ();*/
//}
//----------------------//

bool PluginScene::isDocumentModified (void)
{
	endEditNodeTransform ();

	return _editor->isDocumentModified ();
}

void PluginScene::startTimePreview (void)
{
	if (_isCreatingAsset)
	{
		endCreateAsset (true, false);
	}

	if (_isEditingHeight)
	{
		endEditHeight ();
	}

	endAddRegion (_currentRegion.get(), true);
	endAddPath (_currentPath.get(), true);

	if (_pickedNodeList)
	{
		ATOM_DELETE(_pickedNodeList);
		_pickedNodeList = 0;
	}

	endEditNodeTransform ();

	selectNode (0);

	if (_tcSceneGraph)
	{
		_tcSceneGraph->show (ATOM_Widget::Hide);
	}

	if (_postEffectList)
	{
		_postEffectList->show (ATOM_Widget::Hide);
	}

	_editor->showScenePropEditor (false);

	if (_tbNodeProperties)
	{
		_tbNodeProperties->setBarVisible (false);
	}

	_editor->showAssetEditor (false);

	_savedTime = ATOM_TimeManager::getCurrentTime ();

	if (!_tbTimePreview)
	{
		_tbTimePreview = ATOM_NEW(ATOMX_TweakBar, "Time Preview");
		_tbTimePreview->setBarColor (0.5f, 0.5f, 0.5f, 1.f);
		ATOMX_TBValue val;
		val.setF (ATOMX_TBTYPE_FLOAT, _savedTime);
		_tbTimePreview->addVariable ("Time", 0, val, false, 0);
		_tbTimePreview->setVarStep ("Time", 0.1f);
		_tbTimePreview->setVarMinMax ("Time", 0.f, 24.f);
		_tbTimePreview->addButton ("Quit", 0, "Quit", 0);
	}
}

void PluginScene::endTimePreview (void)
{
	ATOM_DELETE(_tbTimePreview);
	_tbTimePreview = 0;
	ATOM_TimeManager::setCurrentTime (_savedTime);

	if (_tcSceneGraph)
	{
		_tcSceneGraph->show (ATOM_Widget::ShowNormal);
	}

	if (_postEffectList)
	{
		_postEffectList->show (ATOM_Widget::ShowNormal);
	}

	_editor->showScenePropEditor (true);
	_editor->refreshScenePropEditor ();

	if (_tbNodeProperties)
	{
		_tbNodeProperties->setBarVisible (true);
	}

	_editor->showAssetEditor (true);
}

template <class T>
T *findNode (ATOM_Node *parent)
{
	T *p = dynamic_cast<T*>(parent);
	if (p)
	{
		return p;
	}
	for (unsigned i = 0; i < parent->getNumChildren(); ++i)
	{
		p = findNode<T>(parent->getChild (i));
		if (p)
		{
			return p;
		}
	}
	return 0;
}

bool getLightDirection (ATOM_Node *parent, ATOM_Vector3f &dir)
{
	ATOM_Sky *sky = findNode<ATOM_Sky>(parent);
	if (sky)
	{
		dir = sky->getLightDir ();
		return true;
	}
	return false;
}

DlgHeightGen *PluginScene::getDlgHeightGen (void)
{
	if (!_dlgHeightGen)
	{
		_dlgHeightGen = ATOM_NEW(DlgHeightGen, _editor->getRealtimeCtrl(), _scene, getDlgEditBlocks(), this);
	}
	return _dlgHeightGen;
}

DlgEditBlocks *PluginScene::getDlgEditBlocks (void)
{
	if (!_dlgEditBlocks)
	{
		_dlgEditBlocks = ATOM_NEW(DlgEditBlocks, this, _editor->getRealtimeCtrl());
		changeBlockType (_dlgEditBlocks->getBlockType());
	}
	return _dlgEditBlocks;
}

DlgLoadNPC *PluginScene::getDlgLoadNPC (void)
{
	if (!_dlgLoadNPC)
	{
		const char *scnFileName = _editor->getDocumentFileName ();
		if (scnFileName && scnFileName[0])
		{
			if (!_sceneNPC)
			{
				_sceneNPC = ATOM_HARDREF(ATOM_Node)();
				_sceneNPC->setO2T (ATOM_Matrix4x4f::getIdentityMatrix());
				_sceneNPC->setPrivate (1);
				_sceneRoot->appendChild (_sceneNPC.get());
			}
			_dlgLoadNPC = ATOM_NEW(DlgLoadNPC, scnFileName, _editor->getRealtimeCtrl(), _sceneNPC.get());
		}
	}
	return _dlgLoadNPC;
}

DlgRegionEdit *PluginScene::getDlgRegionEdit (void)
{
	if (!_dlgRegionEdit)
	{
		_dlgRegionEdit = ATOM_NEW(DlgRegionEdit, this, _editor->getRealtimeCtrl());
	}
	return _dlgRegionEdit;
}

DlgPathEditor *PluginScene::getDlgPathEditor (void)
{
	if (!_dlgPathEditor)
	{
		_dlgPathEditor = ATOM_NEW(DlgPathEditor, _editor->getRealtimeCtrl(), this);
		_dlgPathEditor->getDialog()->moveTo (100, 100);
	}
	return _dlgPathEditor;
}

void PluginScene::setupMaterialTweakBar (ATOMX_TweakBar *bar, ATOM_Material *material)
{
	if (material)
	{
		ATOM_ParameterTable *paramTable = material->getParameterTable ();
		for (int i = 0; i < paramTable->getNumParameters(); ++i)
		{
			ATOM_ParameterTable::ValueHandle value = paramTable->getParameter (i);

			const char *paramName = value->getParameterName();
			switch (value->handle->getEditorType ())
			{
			case ATOM_MaterialParam::ParamEditorType_None:
				{
					switch (value->handle->getParamType ())
					{
					case ATOM_MaterialParam::ParamType_Float:
						bar->addFloatVar (paramName, ID_MATERIAL_PARAM, *value->f, false, "材质", material);
						break;
					case ATOM_MaterialParam::ParamType_Int:
						bar->addIntVar (paramName, ID_MATERIAL_PARAM, *value->i, false, "材质", material);
						break;
					case ATOM_MaterialParam::ParamType_Texture:
						bar->addButton (paramName, ID_MATERIAL_PARAM, paramName, "材质", material);
						break;
					case ATOM_MaterialParam::ParamType_Vector:
						bar->addVector4fVar (paramName, ID_MATERIAL_PARAM, *value->v, false, "材质", material);
						break;
					}
					break;
				}
			case ATOM_MaterialParam::ParamEditorType_Bool:
				{
					bar->addBoolVar (paramName, ID_MATERIAL_PARAM, value->i != 0, false, "材质", material);
					break;
				}
			case ATOM_MaterialParam::ParamEditorType_Color:
				{
					bar->addRGBAVar (paramName, ID_MATERIAL_PARAM, value->v->x, value->v->y, value->v->z, value->v->w, false, "材质", material);
					break;
				}
			case ATOM_MaterialParam::ParamEditorType_Direction:
				{
					bar->addDirVar (paramName, ID_MATERIAL_PARAM, value->v->x, value->v->y, value->v->z, false,"材质", material);
					break;
				}
			}
		}
	}
}

void PluginScene::generateTerrainShadowMap (void)
{
	ATOM_Terrain *terrain = findNode<ATOM_Terrain>(_scene->getRootNode());
	if (!terrain)
	{
		return;
	}

	ATOM_Vector3f lightDir;
	if (!getLightDirection (_scene->getRootNode(), lightDir))
	{
		return;
	}
	lightDir *= -1.f;
	lightDir.normalize ();

	int pixelError = terrain->getMaxPixelError ();
	terrain->setMaxPixelError (0);

	ATOM_RenderDevice *device = ATOM_GetRenderDevice ();

	// save old render settings
	ATOM_DeferredRenderScheme *scheme = (ATOM_DeferredRenderScheme*)_scene->getRenderScheme ();
	unsigned viewportWidth = scheme->getWidth ();
	unsigned viewportHeight = scheme->getHeight ();
	ATOM_Matrix4x4f savedViewMatrix = _scene->getCamera()->getViewMatrix ();
	ATOM_Matrix4x4f savedProjMatrix = _scene->getCamera()->getProjectionMatrix ();
	ATOM_Rect2Di savedViewport = _scene->getCamera()->getViewport ();
	ATOM_Rect2Di savedDeviceViewport = device->getViewport (0);
	ATOM_ColorARGB savedClearColor = device->getClearColor (0);
	ATOM_AUTOREF(ATOM_Texture) oldRenderTarget = device->getRenderTarget (0);
	ATOM_AUTOREF(ATOM_DepthBuffer) oldDepthBuffer = device->getDepthBuffer ();

	// create new resources
	const unsigned renderTargetSize = 1024;
	const unsigned shadowMapSize = 2048;
	const unsigned subDiv = shadowMapSize > 512 ? shadowMapSize * 2 / 512 : 1;

	EditorRenderScheme *editorRenderScheme = (EditorRenderScheme*)ATOM_RenderScheme::createRenderScheme ("editor");
	editorRenderScheme->init (device, renderTargetSize, renderTargetSize);
	ATOM_AUTOREF(ATOM_Texture) renderTargetColor = device->allocTexture (0, 0, renderTargetSize, renderTargetSize, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
	ATOM_AUTOREF(ATOM_DepthBuffer) depthBuffer = device->allocDepthBuffer (renderTargetSize, renderTargetSize);

	// 
	ATOM_BBox terrainBoundingbox = terrain->getWorldBoundingbox ();
	float radius = terrainBoundingbox.getDiagonalSize() * 0.5f;
	ATOM_Vector3f minpt = terrainBoundingbox.getMin ();
	ATOM_Vector3f maxpt = terrainBoundingbox.getMax ();
	float stepX = (maxpt.x - minpt.x) / float(subDiv);
	float stepY = (maxpt.y - minpt.y) / float(subDiv);
	float stepZ = (maxpt.z - minpt.z) / float(subDiv);
	float totalRadius = terrainBoundingbox.getDiagonalSize() * 0.5f;
	float stepRadius = totalRadius / float(subDiv);
	stepRadius += 100.f;
	ATOM_VECTOR<ATOM_Vector4f> textureContent (renderTargetSize * renderTargetSize);
	ATOM_VECTOR<ATOM_ColorARGB> shadowMapBuffer(shadowMapSize * shadowMapSize);
	for (unsigned i = 0; i < shadowMapSize * shadowMapSize; ++i)
	{
		shadowMapBuffer[i].setFloats (0.f, 0.f, 0.f, 1.f);
	}

	device->beginFrame ();

	// begin rendering tasks
	for (unsigned x = 0; x < subDiv; ++x)
		for (unsigned y = 0; y < subDiv; ++y)
			for (unsigned z = 0; z < subDiv; ++z)
			{
				ATOM_Vector3f center;
				center.x = minpt.x + (x + 0.5f) * stepX;
				center.y = minpt.y + (y + 0.5f) * stepY;
				center.z = minpt.z + (z + 0.5f) * stepZ;
				ATOM_Vector3f eye = center + lightDir * 2 * totalRadius;
				_scene->getCamera()->setOrtho (-stepRadius, stepRadius, -stepRadius, stepRadius, 1, 4 * totalRadius + 100.f);
				_scene->getCamera()->lookAt (eye, center, ATOM_Vector3f(0.f, 1.f, 0.f));
				device->setClearColor (0, ATOM_ColorARGB(1.f, 1.f, 1.f, 1.f));

				_scene->setRenderScheme (scheme);
				device->setRenderTarget (0, renderTargetColor.get());
				device->setDepthBuffer (depthBuffer.get());
				_scene->getCamera()->setViewport (0, 0, renderTargetSize, renderTargetSize);
				_scene->render (device, true);

				_scene->setRenderScheme (editorRenderScheme);
				device->setDepthBuffer (scheme->getDepthBuffer());
				_scene->getCamera()->setViewport (0, 0, renderTargetSize, renderTargetSize);
				_scene->render (device, true);

				device->setRenderTarget (0, 0);
				device->setDepthBuffer (0);
				editorRenderScheme->getUVNormalTexture()->getTexImage (ATOM_PIXEL_FORMAT_RGBA32F, &textureContent[0]);

#if 1
				ATOM_LOGGER::log ("%d-%d-%d\n", x, y, z);
#else
				char buffer[256];
				sprintf (buffer, "/uvnormal_%d_%d_%d.dds", x, y, z);
				editorRenderScheme->getUVNormalTexture()->saveToFile (buffer);
#endif

				for (unsigned n = 0; n < renderTargetSize * renderTargetSize; ++n)
				{
					const ATOM_Vector4f &v = textureContent[n];
					if (v.z != 1.f || v.w != 1.f)
					{
						float tu = v.x;
						float tv = v.y;
						unsigned texPosX = ATOM_ftol (shadowMapSize * tu + 0.5f);
						if (texPosX >= shadowMapSize)
						{
							texPosX = shadowMapSize - 1;
						}
						unsigned texPosY = ATOM_ftol (shadowMapSize * tv + 0.5f);
						if (texPosY >= shadowMapSize)
						{
							texPosY = shadowMapSize - 1;
						}
						shadowMapBuffer[texPosX + shadowMapSize * texPosY].setFloats (1.f, 1.f, 1.f, 1.f);
					}
				}
			}

	device->endFrame ();

	terrain->setMaxPixelError (pixelError);

	_scene->setRenderScheme (scheme);
	_scene->getCamera()->setViewMatrix (savedViewMatrix);
	_scene->getCamera()->setProjectionMatrix (savedProjMatrix);
	_scene->getCamera()->setViewport (savedViewport.point.x, savedViewport.point.y, savedViewport.size.w, savedViewport.size.h);
	device->setClearColor (0, savedClearColor);
	device->setRenderTarget (0, oldRenderTarget.get());
	device->setDepthBuffer (oldDepthBuffer.get());
	device->setViewport (0, savedDeviceViewport);

	ATOM_BaseImage image (shadowMapSize, shadowMapSize, ATOM_PIXEL_FORMAT_BGRX8888, &shadowMapBuffer[0], 1);
	for (unsigned i = 0; i < 4; ++i)
	{
		image.resize (shadowMapSize, shadowMapSize);
	}
	image.save (ATOM_AutoFile("/shadowmap.png", ATOM_VFS::write), ATOM_PIXEL_FORMAT_BGRX8888);
}

void PluginScene::handleTransformEdited (ATOM_Node *node)
{
	if (_isEditingHeight && node == _heightEditingPoint.get())
	{
		float cellSize = getDlgHeightGen()->getCellSize();
		const ATOM_Rect2Di &region = getDlgHeightGen()->getRegion ();

		float tx = (region.point.x + _editingHeightX) * cellSize;
		float tz = (region.point.y + _editingHeightY) * cellSize;
		ATOM_Matrix4x4f o2t = node->getO2T();
		if (o2t.m30 != tx || o2t.m32 != tz)
		{
			o2t.m30 = tx;
			o2t.m32 = tz;
			node->setO2T (o2t);
		}
		getDlgHeightGen()->getGrid()->updateHeightValue (_editingHeightX, _editingHeightY, o2t.m31);
	}
	else if (_dlgPathEditor)
	{
		int path = _dlgPathEditor->getPathByCtlPointNode (node);
		if (path >= 0)
		{
			_dlgPathEditor->refreshPathTrail (path);
		}
	}
}

void PluginScene::handleScenePropChanged (void)
{
	_editor->setDocumentModified (true);
}

void PluginScene::changeBlockType (int type)
{
	getDlgHeightGen()->getGrid()->updateBlockTextureByType (type);
}

void PluginScene::clearBlocks (bool set)
{
	getDlgHeightGen()->getGrid()->clearBlockTexture (getDlgEditBlocks()->getBrushColor (set ? getDlgEditBlocks()->getBlockType() : BT_None));
	getDlgHeightGen()->getGrid()->clearBlocks (getDlgEditBlocks()->getBlockType(), set);
}

void PluginScene::clearBlockLessAngle (int angle, bool set)
{
	getDlgHeightGen()->getGrid()->clearBlocksLessAngle (getDlgEditBlocks()->getBlockType(), angle, set);
}

void PluginScene::clearBlockGreaterAngle (int angle, bool set)
{
	getDlgHeightGen()->getGrid()->clearBlocksGreaterAngle (getDlgEditBlocks()->getBlockType(), angle, set);
}

ATOM_Terrain *PluginScene::findTerrain (void) const
{
	class TerrainFinderVisitor: public ATOM_Visitor
	{
	public:
		virtual void visit (ATOM_Node &node) {}
		virtual void visit (ATOM_Terrain &node) { terrain = &node; }
		ATOM_AUTOREF(ATOM_Terrain) terrain;
	};

	TerrainFinderVisitor terrainFinder;
	terrainFinder.traverse (*_scene->getRootNode());
	return terrainFinder.terrain.get();
}

AS_Editor *PluginScene::getEditor (void) const
{
	return _editor;
}

void PluginScene::beginAddPath (ATOM_AUTOREF(ATOM_LineTrail) trail)
{
	endAddRegion (_currentRegion.get(), true);
	endEditHeight ();
	endCreateAsset (true, false);
	endAddPath (_currentPath.get(), true);
	if (_editor->isEditingNodeTransform ())
	{
		endEditNodeTransform ();
	}

	_currentPath = trail;
	_sceneRoot->appendChild (_currentPath.get());

	_beginCreatePath = true;
}

void PluginScene::refreshPathList (void)
{
	if (_dlgPathEditor)
	{
		for (int i = 0; i < _dlgPathEditor->getNumPaths(); ++i)
		{
			_dlgPathEditor->refreshPathTrail (i);
		}
	}
}

void PluginScene::endAddPath (ATOM_LineTrail *trail, bool cancel)
{
	if (_beginCreatePath)
	{
		_beginCreatePath = false;
		if (!cancel)
		{
			getDlgPathEditor()->addPath (trail);

		}
		else
		{
			trail->getParent()->removeChild (trail);
		}
		_currentPath = NULL;
	}
}

void PluginScene::beginAddRegion (ATOM_AUTOREF(ATOM_Decal) decal)
{
	endAddPath (_currentPath.get(), true);
	endEditHeight ();
	endCreateAsset (true, false);
	endAddRegion (_currentRegion.get(), true);
	if (_editor->isEditingNodeTransform ())
	{
		endEditNodeTransform ();
	}

	_currentRegion = decal;
	_sceneRoot->appendChild (_currentRegion.get());

	_beginCreatingRegion = true;
}

void PluginScene::endAddRegion (ATOM_Decal *decal, bool cancel)
{
	if (_beginCreatingRegion)
	{
		_beginCreatingRegion = false;
		if (!cancel)
		{
			getDlgRegionEdit()->addRegion (decal);

		}
		else
		{
			decal->getParent()->removeChild (decal);
		}
		_currentRegion = NULL;
	}
}

class HMInfo: public ATOM_EventTrigger
{
public:
//%%BeginIDList
	enum {
		ID_LBLX = 110,
		ID_LBLY = 101,
		ID_LBLW = 102,
		ID_LBLH = 103,
		ID_BTNOK = 104,
	};
//%%EndIDList

public:
//%%BeginInitFunc
	void initControls(ATOM_Widget *parent) {
		m_lblX = (ATOM_Label*)parent->getChildByIdRecursive(ID_LBLX);
		m_lblY = (ATOM_Label*)parent->getChildByIdRecursive(ID_LBLY);
		m_lblW = (ATOM_Label*)parent->getChildByIdRecursive(ID_LBLW);
		m_lblH = (ATOM_Label*)parent->getChildByIdRecursive(ID_LBLH);
		m_btnOk = (ATOM_Button*)parent->getChildByIdRecursive(ID_BTNOK);
	}
//%%EndInitFunc

public:
	HMInfo (ATOM_Widget *parent)
	{
		m_dlg = (ATOM_Dialog*)parent->loadHierarchy ("/editor/ui/hminfo.ui");
		if (m_dlg)
		{
			initControls (m_dlg);
			m_dlg->setEventTrigger (this);
		}
	}

	virtual ~HMInfo (void)
	{
		if (m_dlg)
		{
			m_dlg->setEventTrigger (0);
			ATOM_DELETE(m_dlg);
			m_dlg = 0;
		}
	}

	void showModal (float x, float y, float w, float h)
	{
		if (m_dlg)
		{
			char buffer[256];
			sprintf (buffer, "X: %f", x);
			m_lblX->setText (buffer);

			sprintf (buffer, "Y: %f", y);
			m_lblY->setText (buffer);

			sprintf (buffer, "W: %f", w);
			m_lblW->setText (buffer);

			sprintf (buffer, "H: %f", h);
			m_lblH->setText (buffer);

			int parentW = m_dlg->getParent()->getWidgetRect().size.w;
			int parentH = m_dlg->getParent()->getWidgetRect().size.h;
			int w = m_dlg->getWidgetRect().size.w;
			int h = m_dlg->getWidgetRect().size.h;
			int x = (parentW - w) / 2;
			int y = (parentH - h) / 2;
			m_dlg->moveTo (x, y);

			m_dlg->showModal ();
		}
	}

	void onCommand (ATOM_WidgetCommandEvent *event)
	{
		m_dlg->endModal (0);
	}

private:
//%%BeginVarList
	ATOM_Label *m_lblX;
	ATOM_Label *m_lblY;
	ATOM_Label *m_lblW;
	ATOM_Label *m_lblH;
	ATOM_Button *m_btnOk;
//%%EndVarList

	ATOM_Dialog *m_dlg;

	ATOM_DECLARE_EVENT_MAP(HMInfo, ATOM_EventTrigger)
};

ATOM_BEGIN_EVENT_MAP(HMInfo, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(HMInfo, ATOM_WidgetCommandEvent, onCommand)
ATOM_END_EVENT_MAP

void PluginScene::showHeightmapInfo (float x, float y, float w, float h)
{
	HMInfo hmInfo (_editor->getRealtimeCtrl ());
	hmInfo.showModal (x, y, w, h);
}

ATOM_ShapeNode *PluginScene::getCurrentHeightIndicator (void) const
{
	ATOM_ShapeNode *shapeNode = dynamic_cast<ATOM_ShapeNode*>(_selectedNode.get());
	if (shapeNode && shapeNode->getDescribe() == HEIGHT_INDICATOR_DESC)
	{
		return shapeNode;
	}
	return nullptr;
}

void PluginScene::changeRenderScheme (void)
{
	_scene->setRenderScheme (_editor->getRenderScheme());
}


enum CUBEMAP_FACES
{
	CUBEMAP_FACE_POSITIVE_X     = 0,
	CUBEMAP_FACE_NEGATIVE_X     = 1,
	CUBEMAP_FACE_POSITIVE_Y     = 2,
	CUBEMAP_FACE_NEGATIVE_Y     = 3,
	CUBEMAP_FACE_POSITIVE_Z     = 4,
	CUBEMAP_FACE_NEGATIVE_Z     = 5,

	CUBEMAP_FACE_FORCE_DWORD    = 0x7fffffff
};
void PluginScene::getCubeMapVector( unsigned face, ATOM_Vector3f & vLookDir, ATOM_Vector3f & vUpDir )
{
	const ATOM_Vector3f & camera_dir_ws = ATOM_Vector3f(0,0,1);/*_lockedCameraModal->getDirection()*/;
	ATOM_Vector3f camera_dir_horizon = ATOM_Vector3f(camera_dir_ws.x,0.0f,camera_dir_ws.z);
	camera_dir_horizon.normalize();

	ATOM_Vector3f camera_right_horizon = crossProduct(ATOM_Vector3f( 0.0f, 1.0f, 0.0f ),camera_dir_horizon);
	switch( face )
	{
	case CUBEMAP_FACE_POSITIVE_X:
		vLookDir = camera_right_horizon;
		vUpDir = ATOM_Vector3f( 0.0f, 1.0f, 0.0f );
		break;
	case CUBEMAP_FACE_NEGATIVE_X:
		vLookDir = -camera_right_horizon;
		vUpDir = ATOM_Vector3f( 0.0f, 1.0f, 0.0f );
		break;
	case CUBEMAP_FACE_POSITIVE_Y:
		vLookDir = ATOM_Vector3f( 0.0f, 1.0f, 0.0f );
		vUpDir = -camera_dir_horizon;
		break;
	case CUBEMAP_FACE_NEGATIVE_Y:
		vLookDir = ATOM_Vector3f( 0.0f, -1.0f, 0.0f );
		vUpDir = camera_dir_horizon;
		break;
	case CUBEMAP_FACE_POSITIVE_Z:
		vLookDir = camera_dir_horizon;
		vUpDir = ATOM_Vector3f( 0.0f, 1.0f, 0.0f );
		break;
	case CUBEMAP_FACE_NEGATIVE_Z:
		vLookDir = -camera_dir_horizon;
		vUpDir = ATOM_Vector3f( 0.0f, 1.0f, 0.0f );
		break;
	}
}

void PluginScene::packDepthEnvMap()
{
	ATOM_DeferredRenderScheme * d_renderscheme = (ATOM_DeferredRenderScheme *)(_scene->getRenderScheme());
	if( d_renderscheme )
	{
		ATOM_AUTOPTR(ATOM_Material) material = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/editor/materials/packDepth.mat");
		if (!material)
		{
			return;
		}
		material->setActiveEffect ("default");

		material->getParameterTable()->setTexture("depthTexture",d_renderscheme->getDepthTexture());

		ATOM_Camera *camera = _scene->getCamera();
		float q = camera->getFarPlane()/(camera->getFarPlane()-camera->getNearPlane());
		float mq = camera->getNearPlane()*q;
		material->getParameterTable()->setVector("linearDepthParam",ATOM_Vector4f(mq, q, 0.f, 0.f));

		d_renderscheme->drawScreenQuad (ATOM_GetRenderDevice(), material.get());
	}
}
void PluginScene::genEnvMap(ATOM_Vector3f cameraPos, 
							float cullradius, 
							unsigned cubeMapSize,
							bool bGlobalEnv /*=true*/,
							bool bSaveToFile /*=false*/, 
							const char* filename/*=0*/)
{
	ATOM_Vector3f cam_pos_ws = cameraPos;

	if( _selectedNode && !stricmp(_selectedNode->getClassName(),"ATOM_ShapeNode") )
	{
		cam_pos_ws = _selectedNode->getWorldTranslation();
		_selectedNode->setShow(0);
	}

	ATOM_AUTOREF(ATOM_Texture) envTexture = 
		ATOM_GetRenderDevice()->allocTexture (	0, 0, 
												cubeMapSize, cubeMapSize, 
												/*ATOM_PIXEL_FORMAT_RGBA16F*/ATOM_PIXEL_FORMAT_BGR888,		// 使用浮点纹理保存深度到ALPHA
												ATOM_Texture::CUBEMAP|ATOM_Texture::RENDERTARGET);
	if( !envTexture )
		return;

	ATOM_AUTOREF(ATOM_DepthBuffer) depthBuffer = ATOM_GetRenderDevice()->allocDepthBuffer (cubeMapSize, cubeMapSize);
	if( !depthBuffer )
		return;

	class MaterialDirtyResetVisitor: public ATOM_Visitor
	{
		ATOM_Vector3f origin;
		float cullRadius;

	public:
		MaterialDirtyResetVisitor (ATOM_Vector3f org,float radius):origin(org),cullRadius(radius)
		{
		}

	public:

		virtual void visit (ATOM_Node &node)
		{
			node.resetMaterialDirtyFlag();
		}
		virtual void visit (ATOM_NodeOctree &node)
		{
			return;
		}
		virtual void visit (ATOM_Terrain &node)
		{
			node.resetMaterialDirtyFlag();
		}
		virtual void visit (ATOM_VisualNode &node)
		{
			if( cullDistance(node) )
				return;

			node.resetMaterialDirtyFlag();
		}
		virtual void visit (ATOM_Geode &node)
		{
			if( cullDistance(node) )
				return;

			node.resetMaterialDirtyFlag();
		}
		virtual void visit (ATOM_ParticleSystem &node)
		{
			return;
		}
		virtual void visit (ATOM_Hud &node)
		{
			node.resetMaterialDirtyFlag();
		}
		virtual void visit (ATOM_GuiHud &node)
		{
			node.resetMaterialDirtyFlag();
		}
		virtual void visit (ATOM_Water &node)
		{
			node.resetMaterialDirtyFlag();
		}
		virtual void visit (ATOM_LightNode &node)
		{
			return;
		}
		virtual void visit (ATOM_Atmosphere &node)
		{
			node.resetMaterialDirtyFlag();
		}
		virtual void visit (ATOM_Atmosphere2 &node)
		{
			node.resetMaterialDirtyFlag();
		}
		virtual void visit (ATOM_Sky &node)
		{
			node.resetMaterialDirtyFlag();
		}
		virtual void visit (ATOM_BkImage &node)
		{
			node.resetMaterialDirtyFlag();
		}
		virtual void visit (ATOM_Actor &node)
		{
			if( cullDistance(node) )
				return;

			node.resetMaterialDirtyFlag();
		}
		virtual void visit (ATOM_Decal &node)
		{
			if( cullDistance(node) )
				return;

			node.resetMaterialDirtyFlag();
		}
		virtual void visit (ATOM_CompositionNode &node)
		{
			if( cullDistance(node) )
				return;

			node.resetMaterialDirtyFlag();
		}
		virtual void visit (ATOM_ShapeNode &node)
		{
			if( cullDistance(node) )
				return;

			node.resetMaterialDirtyFlag();
		}
		virtual void visit (ATOM_RibbonTrail &node)
		{
			if( cullDistance(node) )
				return;

			node.resetMaterialDirtyFlag();
		}
		virtual void visit (ATOM_WeaponTrail &node)
		{
			if( cullDistance(node) )
				return;

			node.resetMaterialDirtyFlag();
		}
		virtual void visit (ClientSimpleCharacter &node)
		{
			if( cullDistance(node) )
				return;

			node.resetMaterialDirtyFlag();
		}

	private:

		bool cullDistance(ATOM_Node & node)
		{
			ATOM_Vector3f dir = node.getTranslation() - origin;
			if( dir.getSquaredLength() < cullRadius * cullRadius )
				return true;
			return false;
		}
	};

	// 关闭阴影
	ATOM_RenderSettings::enableShadow (false);
	// 关闭后处理
	ATOM_RenderSettings::enablePostEffect(false);

	for( unsigned face = CUBEMAP_FACE_POSITIVE_X; face <= CUBEMAP_FACE_NEGATIVE_Z; ++face )
	{
		ATOM_Vector3f vLookDir,vUpDir;
		getCubeMapVector(face,vLookDir,vUpDir);

		ATOM_AUTOREF(ATOM_Texture)		savedRenderTarget		= ATOM_GetRenderDevice()->getRenderTarget (0);
		ATOM_AUTOREF(ATOM_DepthBuffer)	savedDepthBuffer		= ATOM_GetRenderDevice()->getDepthBuffer ();
		ATOM_Rect2Di					savedViewport			= ATOM_GetRenderDevice()->getViewport (NULL);
		ATOM_ColorARGB					savedClearColor			= ATOM_GetRenderDevice()->getClearColor(NULL);

		ATOM_GetRenderDevice()->pushMatrix (ATOM_MATRIXMODE_WORLD);
		ATOM_GetRenderDevice()->pushMatrix (ATOM_MATRIXMODE_VIEW);
		ATOM_GetRenderDevice()->pushMatrix (ATOM_MATRIXMODE_PROJECTION);

		ATOM_GetRenderDevice()->setClearColor (0, 0.f, 0.f, 0.f, 0.f);
		ATOM_GetRenderDevice()->setRenderTarget (0, envTexture.get(), face);
		ATOM_GetRenderDevice()->setDepthBuffer (depthBuffer.get());
		ATOM_GetRenderDevice()->setViewport (0, 0, 0, cubeMapSize, cubeMapSize);
		ATOM_GetRenderDevice()->clear (true, true, true);

		ATOM_Matrix4x4f mat_view;
		mat_view.makeLookatLH(cam_pos_ws,cam_pos_ws+vLookDir,vUpDir);
		_scene->getCamera()->setViewMatrix (mat_view);
		_scene->getCamera()->setPerspective (ATOM_Pi / 2.f, 1.f, _scene->getNearPlane(), _scene->getFarPlane());
		_scene->getCamera()->setViewport (0,0, cubeMapSize,cubeMapSize);

		// 如果是全局的，渲染地形和天空
		if( bGlobalEnv )
		{
			toggleSceneNode();
		}
		else
		{
			MaterialDirtyResetVisitor v(cam_pos_ws,cullradius);
			v.traverse(*_scene->getRootNode());
		}

		_scene->render (ATOM_GetRenderDevice(), true);

		//ATOM_GetRenderDevice()->setRenderTarget (0, envTexture.get(), face);
		//packDepthEnvMap();

		ATOM_GetRenderDevice()->setClearColor (NULL, savedClearColor);
		ATOM_GetRenderDevice()->setViewport (NULL, savedViewport);
		ATOM_GetRenderDevice()->setRenderTarget (0, savedRenderTarget.get());
		ATOM_GetRenderDevice()->setDepthBuffer (savedDepthBuffer.get());

		ATOM_GetRenderDevice()->popMatrix (ATOM_MATRIXMODE_WORLD);
		ATOM_GetRenderDevice()->popMatrix (ATOM_MATRIXMODE_VIEW);
		ATOM_GetRenderDevice()->popMatrix (ATOM_MATRIXMODE_PROJECTION);

		// 如果是全局的，渲染地形和天空
		if( bGlobalEnv )
		{
			toggleSceneNode();
		}
	}

	ATOM_RenderSettings::enablePostEffect(true);
	ATOM_RenderSettings::enableShadow (true);

	if( bSaveToFile )
	{
		if( filename )
			envTexture->saveToFile(filename);
		else
		{
			ATOM_STRING name = _selectedNode->getDescribe();
			name += ".dds";
			envTexture->saveToFile(name.c_str());
		}
	}

	if( _selectedNode && !stricmp(_selectedNode->getClassName(),"ATOM_ShapeNode") )
	{
		_selectedNode->setShow(1);
	}
}

