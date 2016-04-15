#include "StdAfx.h"
#include "atom3d_studio.h"
#include "plugin.h"
#include "plugin_model.h"
#include "editor.h"
#include "camera_modal.h"
#include "model_prop_editor.h"

#define ID_MESHLIST					(PLUGIN_ID_START + 500)
#define ID_TRACKLIST				(PLUGIN_ID_START + 501)
#define ID_POINTLIST				(PLUGIN_ID_START + 502)
#define ID_IMPORT					(PLUGIN_ID_START + 503)
#define ID_EXPORT					(PLUGIN_ID_START + 504)
#define ID_TRANSLATE_MODEL			(PLUGIN_ID_START + 505)
#define ID_ROTATE_MODEL				(PLUGIN_ID_START + 506)
#define ID_SCALE_MODEL				(PLUGIN_ID_START + 507)
#define ID_VIEW_TOGGLEGUI			(PLUGIN_ID_START + 508)
#define ID_CREATE_POINT				(PLUGIN_ID_START + 509)
#define ID_DELETE_POINT				(PLUGIN_ID_START + 510)
#define ID_RENAME_POINT				(PLUGIN_ID_START + 511)
#define ID_DELETE_ALL_POINTS		(PLUGIN_ID_START + 512)
#define ID_EDIT_POINT_TRANSLATION	(PLUGIN_ID_START + 513)
#define ID_EDIT_POINT_ROTATION		(PLUGIN_ID_START + 514)
#define ID_EDIT_POINT_SCALE			(PLUGIN_ID_START + 515)
#define ID_EDIT_POINT_ENDEDIT		(PLUGIN_ID_START + 516)
#define ID_EDIT_POINT_PROP			(PLUGIN_ID_START + 517)
#define ID_DELETE_MESH				(PLUGIN_ID_START + 518)
#define ID_POINT_TO_CAMERA			(PLUGIN_ID_START + 519)
#define ID_CAMERA_TO_POINT			(PLUGIN_ID_START + 520)

// wangjian added 
#define ID_GEN_CURVATUREMAP			(PLUGIN_ID_START + 521)
#define ID_SAVE_ISOLATION			(PLUGIN_ID_START + 522)

#define ID_VIEW_TOGGLEGRID			(PLUGIN_ID_START + 523)			// 切换网格显示
#define ID_VIEW_TOGGLEBBOX			(PLUGIN_ID_START + 524)			// 切换包围盒显示
#define ID_SELECT_SCENE				(PLUGIN_ID_START + 525)			// 选择场景
#define ID_SELECT_BKIMAGE			(PLUGIN_ID_START + 526)			// 选择背景
////////////////////////////////////////////////////////////


//%%BeginIDList
	enum {
		ID_EDNAME = 102,
		ID_EDX = 104,
		ID_EDY = 106,
		ID_EDZ = 108,
		ID_BTNOK = 109,
		ID_BTNCANCEL = 110,
	};
//%%EndIDList

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//class ATOM_AnimationContainerTexture
//{
//public:
//	ATOM_AnimationContainerTexture();
//	~ATOM_AnimationContainerTexture();
//
//	static void CreateAnimationTexture(ATOM_SharedModel * model)
//	{
//		if( !model )
//			return;
//
//		int num_track = model->getNumTracks();
//		if( num_track <= 0 )
//			return;
//
//		ATOM_Skeleton * skeleton = model->getSkeleton();
//		if( !skeleton || skeleton->getNumJoints() <= 0 )
//			return;
//
//		// 骨骼数
//		int num_bone = skeleton->getNumJoints();
//		
//		unsigned total_frame = 0;
//		// 循环所有的动画
//		for( int i = 0; i < num_track; ++i )
//		{
//			// 获得该动画
//			ATOM_JointAnimationTrack * track = model->getTrack(i);
//			if( !track )
//				continue;
//
//			// 获得该动画的帧数
//			int num_frames = track->getNumFrames();
//
//			total_frame += num_frames;
//		}
//
//		unsigned texelsPerBone = 4;
//
//		unsigned pixelCount = total_frame * texelsPerBone;    // rowsPerBone lines per matrix, since no projection
//		unsigned texWidth = 0;
//		unsigned texHeight = 0;
//
//		// This basically fits the animation into a roughly square texture where the 
//		//      width is a multiple of rowsPerBone(our size requirement for matrix storage)
//		//      AND both dimensions are power of 2 since it seems to fail without this...
//		texWidth = (int)sqrt((float)pixelCount)+1;    // gives us a starting point
//		texHeight = 1;
//		while(texHeight < texWidth) 
//			texHeight = texHeight<<1;
//		texWidth = texHeight;
//
//		unsigned datasize = texWidth * texHeight * sizeof( ATOM_Vector4f );
//		ATOM_Vector4f * data = ATOM_NEW_ARRAY(ATOM_Vector4f,texWidth * texHeight);
//		memset( data, 0, datasize );
//
//		ATOM_Vector4f * data_ptr = data;
//
//		// 循环所有的动画
//		for( int i = 0; i < num_track; ++i )
//		{
//			// 获得该动画
//			ATOM_JointAnimationTrack * track = model->getTrack(i);
//			if( !track )
//				continue;
//
//			// 获得该动画的帧数
//			int num_frames = track->getNumFrames();
//
//			// 循环所有的帧
//			for( int j = 0; j < num_frames; ++j )
//			{
//				// 获得该帧的动画数据
//				unsigned num_joint = 0;
//				const ATOM_JointTransformInfo * jointTransformdata = track->getFrameJoints(j,&num_joint);
//				if( !jointTransformdata )
//					continue;
//				
//				// 循环该帧的所有骨骼
//				for( int k = 0; k < num_joint; ++k )
//				{
//					ATOM_Matrix4x4f matrix;
//					// 变换为矩阵形式
//					transformToMatrix(skeleton, k, jointTransformdata[k], matrix);
//
//					(*data_ptr++) = matrix.getRow(0);
//					(*data_ptr++) = matrix.getRow(1);
//					(*data_ptr++) = matrix.getRow(2);
//					(*data_ptr++) = matrix.getRow(3);
//				}
//			}
//		}
//
//		// 生成纹理
//		ATOM_AUTOREF(ATOM_Texture) _texture = ATOM_GetRenderDevice ()->allocTexture (	0, data, 
//																						texWidth, texHeight, 
//																						ATOM_PIXEL_FORMAT_RGBA32F, 
//																						ATOM_Texture::TEXTURE2D|ATOM_Texture::NOMIPMAP	);
//
//		model->setAnimationTexture(_texture.get());
//	}
//
//protected:
//
//	static void transformToMatrix( ATOM_Skeleton * skeleton, int boneindex, const ATOM_JointTransformInfo & transform, ATOM_Matrix4x4f & matrix)
//	{
//		transform.rotate.toMatrix (matrix);
//		matrix.m00 *= transform.scale.x;
//		matrix.m01 *= transform.scale.x;
//		matrix.m02 *= transform.scale.x;
//		matrix.m10 *= transform.scale.y;
//		matrix.m11 *= transform.scale.y;
//		matrix.m12 *= transform.scale.y;
//		matrix.m20 *= transform.scale.z;
//		matrix.m21 *= transform.scale.z;
//		matrix.m22 *= transform.scale.z;
//		matrix.m30 = transform.translate.x;
//		matrix.m31 = transform.translate.y;
//		matrix.m32 = transform.translate.z;
//
//#if 1
//		ATOM_Matrix4x4f bindMatrix;
//		skeleton->getJointBindMatrix (boneindex).toMatrix44(bindMatrix);
//#else
//		ATOM_Matrix4x4f bindMatrix = skeleton->getJointBindMatrix (index);
//#endif
//		matrix >>= bindMatrix;
//
//		std::swap (matrix.m00, matrix.m02);
//		std::swap (matrix.m10, matrix.m12);
//		std::swap (matrix.m20, matrix.m22);
//		std::swap (matrix.m30, matrix.m32);
//
//		//--- 将平移分量放到前3行的最后一个元素 ---//
//		std::swap (matrix.m03, matrix.m30);
//		std::swap (matrix.m13, matrix.m31);
//		std::swap (matrix.m23, matrix.m32);
//		//-----------------------------------------//
//	}
//
//private:
//};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginModel::PluginModel (void)
{
	_scene = 0;
	_meshList = 0;
	_trackList = 0;
	_pointList = 0;
	_pointListMenu = 0;
	_meshListMenu = 0;
	_edModelProp = 0;
	_isGUIShown = true;
	_editingModel = false;
	_editingPoint = false;
	_editor = 0;

	_readonlyMode = false;
}

PluginModel::~PluginModel (void)
{
	clear ();
}

unsigned PluginModel::getVersion (void) const
{
	return AS_VERSION;
}

const char *PluginModel::getName (void) const
{
	return "Model editor";
}

void PluginModel::deleteMe (void)
{
	ATOM_DELETE(this);
}

void PluginModel::clear (void)
{
	if (_meshList)
	{
		_meshList->setEventTrigger (0);
		ATOM_DELETE(_meshList);
		_meshList = 0;

		ATOM_DELETE(_meshListMenu);
		_meshListMenu = 0;
	}

	if (_trackList)
	{
		_trackList->setEventTrigger (0);
		ATOM_DELETE(_trackList);
		_trackList = 0;
	}

	if (_pointList)
	{
		_pointList->setEventTrigger (0);
		ATOM_DELETE(_pointList);
		_pointList = 0;

		ATOM_DELETE(_pointListMenu);
		_pointListMenu = 0;
	}

	_editor->setScenePropEditorTarget (nullptr);
	_editor->showScenePropEditor (false);

	ATOM_DELETE(_edModelProp);
	_edModelProp = 0;

	_grid = 0;
	_geode = 0;
	_model = 0;
	_skeleton = 0;
	_action = 0;
	_lightNode[0] = 0;
	_lightNode[1] = 0;
	_lightNode[2] = 0;
	_sharedMaterials.clear ();

	ATOM_DELETE(_scene);
	_scene = 0;

	_points.clear ();

	_editingPoint = false;

	_bkImage = 0;
}

bool PluginModel::initPlugin (AS_Editor *editor)
{
	editor->registerFileType (this, "nm2", "ATOM3D Model 2.0", AS_FILETYPE_CANEDIT);
	_editor = editor;

	return true;
}

void PluginModel::donePlugin (void)
{
	clear ();
}

void PluginModel::setupMenu (void)
{
	ATOM_MenuBar *menubar = _editor->getMenuBar ();

	ATOM_PopupMenu *menuView = menubar->getMenuItem (AS_MENUITEM_VIEW).submenu;
	menuView->appendMenuItem ("切换界面显示(F11)", ID_VIEW_TOGGLEGUI);

	menuView->appendMenuItem ("切换网格显示(F4)", ID_VIEW_TOGGLEGRID);
	menuView->appendMenuItem ("切换包围盒显示(F5)", ID_VIEW_TOGGLEBBOX);
	menuView->appendMenuItem ("选择场景..", ID_SELECT_SCENE);
	menuView->appendMenuItem ("选择背景..", ID_SELECT_BKIMAGE);

	ATOM_PopupMenu *menuFile = menubar->getMenuItem (AS_MENUITEM_FILE).submenu;
	menuFile->insertMenuItem (menuFile->getNumItems()-1, "导入..", ID_IMPORT);
	menuFile->insertMenuItem (menuFile->getNumItems()-1, "导出换皮模型..", ID_EXPORT);

	// wangjian added
	menuFile->insertMenuItem (menuFile->getNumItems()-1, "分离保存..", ID_SAVE_ISOLATION);

	ATOM_PopupMenu *menuEdit = menubar->getMenuItem (AS_MENUITEM_EDIT).submenu;
	menuEdit->appendMenuItem ("平移", ID_TRANSLATE_MODEL);
	menuEdit->appendMenuItem ("旋转", ID_ROTATE_MODEL);
	menuEdit->appendMenuItem ("缩放", ID_SCALE_MODEL);

	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_F11, 0, ID_VIEW_TOGGLEGUI));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_ESCAPE, 0, ID_EDIT_POINT_ENDEDIT));

	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_F4, 0, ID_VIEW_TOGGLEGRID));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_F5, 0, ID_VIEW_TOGGLEBBOX));
}

void PluginModel::cleanupMenu (void)
{
	ATOM_MenuBar *menubar = _editor->getMenuBar ();

	menubar->getMenuItem(AS_MENUITEM_VIEW).submenu->removeMenuItem (menubar->getMenuItem(AS_MENUITEM_VIEW).submenu->getNumItems()-1);
	ATOM_PopupMenu *menuFile = menubar->getMenuItem (AS_MENUITEM_FILE).submenu;
	menuFile->removeMenuItem (menuFile->getNumItems()-2);
	menuFile->removeMenuItem (menuFile->getNumItems()-2);

	ATOM_PopupMenu *menuEdit = menubar->getMenuItem (AS_MENUITEM_EDIT).submenu;
	menuEdit->removeMenuItem (menuEdit->getNumItems()-1);

	for (unsigned i = 0; i < _accelKeys.size(); ++i)
	{
		_editor->unregisterAccelKeyCommand (_accelKeys[i]);
	}
	_accelKeys.clear ();

	menubar->getMenuItem(AS_MENUITEM_VIEW).submenu->removeMenuItemById (ID_VIEW_TOGGLEGRID);
	menubar->getMenuItem(AS_MENUITEM_VIEW).submenu->removeMenuItemById (ID_VIEW_TOGGLEBBOX);
	menubar->getMenuItem(AS_MENUITEM_VIEW).submenu->removeMenuItemById (ID_SELECT_SCENE);
	menubar->getMenuItem(AS_MENUITEM_VIEW).submenu->removeMenuItemById (ID_SELECT_BKIMAGE);
}

void PluginModel::toggleGUI (void)
{
	_isGUIShown = !_isGUIShown;

	if (_isGUIShown)
	{
		if (_meshList)
		{
			_meshList->show (ATOM_Widget::ShowNormal);
		}

		if (_trackList)
		{
			_trackList->show (ATOM_Widget::ShowNormal);
		}

		if (_pointList)
		{
			_pointList->show (ATOM_Widget::ShowNormal);
		}

		if (_edModelProp)
		{
			_edModelProp->getBar()->setBarVisible (true);
		}
	}
	else
	{
		if (_meshList)
		{
			_meshList->show (ATOM_Widget::Hide);
		}

		if (_trackList)
		{
			_trackList->show (ATOM_Widget::Hide);
		}

		if (_pointList)
		{
			_pointList->show (ATOM_Widget::Hide);
		}

		if (_edModelProp)
		{
			_edModelProp->getBar()->setBarVisible (false);
		}
	}
}

void PluginModel::toggleGrid (void)
{
	if (_grid->getShow () != ATOM_Node::SHOW)
		_grid->setShow (ATOM_Node::SHOW);
	else
		_grid->setShow (ATOM_Node::HIDE);
}
void PluginModel::toggleBBox (void)
{
	if(_geode)
		_geode->setDrawBoundingbox(!_geode->getDrawBoundingbox());
}
void PluginModel::selectScene (void)
{
	if (1 == _editor->getOpenFileNames ("3sg", "场景文件(*.3sg)|*.3sg|", false, false, "选择场景"))
	{
		_scene->getRootNode()->clearChildren ();
		_scene->load (_editor->getOpenedFileName(0));
		_scene->getRootNode()->appendChild (_grid.get());
		_scene->getRootNode()->appendChild (_geode.get());
		if (_bkImage)
		{
			_scene->getRootNode()->appendChild (_bkImage.get());
		}
	}
}

void PluginModel::selectBkImage (void)
{
	if (1 == _editor->getOpenImageFileNames (false, false, "选择背景"))
	{
		if (!_bkImage)
		{
			_bkImage = ATOM_HARDREF(ATOM_BkImage)();
			_bkImage->loadAttribute (NULL);
			_bkImage->load (ATOM_GetRenderDevice());
			_scene->getRootNode()->appendChild (_bkImage.get());
		}
		_bkImage->setImageFile (_editor->getOpenedImageFileName (0));
	}
}

bool PluginModel::beginEdit (const char *filename)
{
	if( ATOM_RenderSettings::isUseHWInstancing() )
	{
		//::MessageBoxA(ATOM_APP->getMainWindow(),"在实例化开启模式下无法进行模型编辑，请使用另外的编辑器进行编辑","ATOM3D编辑器",MB_OK|MB_ICONHAND);
		//return false;
	}

	_scene = ATOM_NEW(ATOM_DeferredScene);
	ATOM_RenderScheme *renderScheme = _editor->getRenderScheme ();
	if (!renderScheme)
	{
		return false;
	}
	_scene->setRenderScheme (renderScheme);
	_scene->setDepthBias (0.005f);
	_points.clear ();

	_grid = ATOM_HARDREF(GridNode)();
	_grid->setPickable (0);
	_grid->setSize (100, 100);
	_grid->load (ATOM_GetRenderDevice());
	_grid->setO2T (ATOM_Matrix4x4f::getScaleMatrix(ATOM_Vector3f(100.f, 100.f, 100.f)));
	_scene->getRootNode()->appendChild (_grid.get());
	_scene->setShadowDistance (600.f);
	_scene->setShadowFadeMin (600.f);
	_scene->setShadowFadeMax (800.f);

	ATOM_HARDREF(ATOM_Sky) sky;
	if (sky->load (ATOM_GetRenderDevice()))
	{
		ATOM_Vector3f v(-1.f, -1.f, 0.f);
		v.normalize();
		sky->setLightDir (v);
		sky->setLightScale (1.f);
		sky->setExposure (1.f);
		sky->setLightIntensity (1.f);
		sky->setAmbientLight (0.4f);
		_scene->getRootNode()->appendChild (sky.get());
	}

	ATOM_HARDREF(ATOM_ShapeNode) floor;
	floor->loadAttribute (NULL);
	if (floor->load (ATOM_GetRenderDevice()))
	{
		floor->setType (ATOM_ShapeNode::PLANE);
		floor->setTranslation (ATOM_Vector3f(0.f, -0.5f, 0.f));
		floor->setScale (ATOM_Vector3f(10000.f, 1.f, 10000.f));
		floor->setColor (ATOM_Vector4f(0.2f, 0.2f, 0.2f, 1.f));
		_scene->getRootNode()->appendChild (floor.get());
	}

	ATOM_Vector4f colors[3] = {
		ATOM_Vector4f(1.f, 0.f, 0.f, 1.f),
		ATOM_Vector4f(0.f, 1.f, 0.f, 1.f),
		ATOM_Vector4f(0.f, 0.f, 1.f, 1.f)
	};

#if 0
	for (unsigned i = 0; i < 3; ++i)
	{
		ATOM_HARDREF(ATOM_LightNode) lightNode;
		lightNode->setLightType (ATOM_Light::Point);
		lightNode->setLightAttenuation (ATOM_Vector3f(1.f, 0.f, 0.f));
		lightNode->setLightColor (colors[i]);
		_scene->getRootNode()->appendChild (lightNode.get());
		_lightNode[i] = lightNode;
	}
#endif

	_lightUpdateStamp = 0;
	_lightPositionAngle = 0.f;
	_editingPoint = false;

	AS_CameraModal *camera = _editor->getCameraModal();
	camera->setPosition (ATOM_Vector3f(300.f, 300.f, 0.f));
	camera->setDirection (ATOM_Vector3f(-300.f, -300.f, 0.f));
	camera->setUpVector (ATOM_Vector3f(0.f, 1.f, 0.f));

	_editor->getRealtimeCtrl()->setScene (_scene);

	_geode = ATOM_HARDREF(ATOM_Geode)();
	_geode->setDrawBoundingbox (1);
	_geode->setO2T (ATOM_Matrix4x4f::getScaleMatrix (1.f));
	_scene->getRootNode()->appendChild (_geode.get());

	if (filename)
	{
		_geode->setNodeFileName (filename);
		if (!_geode->load (ATOM_GetRenderDevice()))
		{
			return false;
		}

		_sharedMaterials.resize (_geode->getNumMeshes());
		for (unsigned i = 0; i < _geode->getNumMeshes(); ++i)
		{
			ATOM_Material *sharedMat = _geode->getStaticMesh(i)->getSharedMesh()->getMaterial();
			_sharedMaterials[i] = sharedMat->clone ();
			_geode->getStaticMesh (i)->setMaterial (sharedMat);
		}
		_model = _geode->getComponents(0).getModel();
		//_editor->beginEditNodeRotation (_geode.get());

		if (_model)
		{
			ATOM_UserAttributes *attributes = _model->getAttributes ();
			for (unsigned i = 0; i < attributes->getNumAttributes (); ++i)
			{
				const char *name = attributes->getAttributeName (i);
				const ATOM_Variant &value = attributes->getAttributeValue (i);
				if (value.getType() == ATOM_Variant::MATRIX44)
				{
					newPointShape (name, (*(ATOM_Matrix4x4f*)value.getM()));
				}
			}
			_modelFileName = filename;

			//--- wangjian added ---//
			//ATOM_AnimationContainerTexture::CreateAnimationTexture(_model.get());
			//----------------------//
		}
	}

	_editor->showScenePropEditor (true);
	_editor->setScenePropEditorTarget (_scene);

	createTweakBars ();
	createMeshList (_editor->getRealtimeCtrl());
	createTrackList (_editor->getRealtimeCtrl());
	createPointList (_editor->getRealtimeCtrl());
	calcUILayout ();

	setupMenu ();

	return true;
}

void PluginModel::endEdit (void)
{
	cleanupMenu ();
	_editor->getRealtimeCtrl()->setScene (0);
	clear ();

}

void PluginModel::frameUpdate (void)
{
	unsigned newStamp = ATOM_APP->getFrameStamp().currentTick;

	if (_lightUpdateStamp == 0)
	{
		_lightPositionAngle = 0.f;
		_lightUpdateStamp = newStamp;
	}
	else
	{
		unsigned elapsed = newStamp - _lightUpdateStamp;
		const float speed = 0.0005f;
		_lightPositionAngle = elapsed * speed;
		_lightPositionAngle -= ATOM_TwoPi * ((int)(_lightPositionAngle / ATOM_TwoPi));

#if 0
		for (unsigned i = 0; i < 3; ++i)
		{
			float angle = _lightPositionAngle + i * (ATOM_TwoPi / 3.f);
			ATOM_Vector3f lightPos(1000.f * ATOM_sin(angle), 100.f, 1000.f * ATOM_cos(angle));
			_lightNode[i]->setO2T (ATOM_Matrix4x4f::getTranslateMatrix(lightPos)>>ATOM_Matrix4x4f::getScaleMatrix(ATOM_Vector3f(3000.f)));
		}
#endif
	}
}

void PluginModel::createTweakBars (void)
{
	_edModelProp = ATOM_NEW(ModelPropEditor, _editor);
	_edModelProp->setModel (_geode.get());
	_edModelProp->show (true);
}

void PluginModel::createMeshList (ATOM_RealtimeCtrl *parent)
{
	ATOM_Rect2Di rc = parent->getClientRect();
	rc.size.w = 200;

	if (!_meshList)
	{
		_meshList = ATOM_NEW(ATOM_ListBox, parent, rc, 16, ATOM_Widget::VScroll|ATOM_Widget::Border, ID_MESHLIST, ATOM_Widget::ShowNormal);
		_meshList->setBorderMode (ATOM_Widget::Drop);
	}

	if (!_meshListMenu)
	{
		_meshListMenu = ATOM_NEW(ATOM_PopupMenu, _editor->getGUIRenderer());
		_meshListMenu->appendMenuItem ("删除..", ID_DELETE_MESH);
	}

	_meshList->clearItem ();

	if (_model)
	{
		for (unsigned i = 0; i < _model->getNumMeshes(); ++i)
		{
			char buffer[32];
			sprintf (buffer, "mesh%d", i);
			_meshList->addItem (buffer, (unsigned long long)_model->getMesh (i));
		}
	}
}

void PluginModel::createPointList (ATOM_RealtimeCtrl *parent)
{
	ATOM_Rect2Di rc = parent->getClientRect();
	rc.size.w = 200;

	if (!_pointList)
	{
		_pointList = ATOM_NEW(ATOM_ListBox, parent, rc, 16, ATOM_Widget::VScroll|ATOM_Widget::Border, ID_POINTLIST, ATOM_Widget::ShowNormal);
		_pointList->setBorderMode (ATOM_Widget::Drop);
	}

	refreshPointList ();

	_pointListMenu = ATOM_NEW(ATOM_PopupMenu, _editor->getGUIRenderer());
	_pointListMenu->appendMenuItem ("创建参考点", ID_CREATE_POINT);
	_pointListMenu->appendMenuItem ("删除参考点", ID_DELETE_POINT);
	_pointListMenu->appendMenuItem ("重命名参考点..", ID_RENAME_POINT);
	_pointListMenu->appendMenuItem ("清除所有参考点", ID_DELETE_ALL_POINTS);
	_pointListMenu->appendMenuItem ("参考点平移", ID_EDIT_POINT_TRANSLATION);
	_pointListMenu->appendMenuItem ("参考点旋转", ID_EDIT_POINT_ROTATION);
	_pointListMenu->appendMenuItem ("参考点缩放", ID_EDIT_POINT_SCALE);
	_pointListMenu->appendMenuItem ("参考点->摄像机", ID_POINT_TO_CAMERA);
	_pointListMenu->appendMenuItem ("摄像机->参考点", ID_CAMERA_TO_POINT);
	_pointListMenu->appendMenuItem ("属性..", ID_EDIT_POINT_PROP);
}

void PluginModel::createTrackList (ATOM_RealtimeCtrl *parent)
{
	ATOM_Rect2Di rc = parent->getClientRect();
	rc.size.w = 200;

	if (!_trackList)
	{
		_trackList = ATOM_NEW(ATOM_ListBox, parent, rc, 16, ATOM_Widget::VScroll|ATOM_Widget::Border, ID_TRACKLIST, ATOM_Widget::ShowNormal);
		_trackList->setBorderMode (ATOM_Widget::Drop);
	}

	_trackList->clearItem ();

	if (_model)
	{
		for (unsigned i = 0; i < _model->getNumTracks(); ++i)
		{
			char buffer[256];
			sprintf (buffer, "%s", _model->getTrack(i)->getName());
			_trackList->addItem (buffer, (unsigned long long)_model->getTrack (i));
		}
	}
}

void PluginModel::handleEvent (ATOM_Event *event)
{
	int eventId = event->getEventTypeId ();

	if (eventId == ATOM_WidgetCommandEvent::eventTypeId())
	{
		ATOM_WidgetCommandEvent *commandEvent = (ATOM_WidgetCommandEvent*)event;
		switch (commandEvent->id)
		{
		case ID_TRANSLATE_MODEL:
			{
				beginTranslateModel ();
				break;
			}
		case ID_ROTATE_MODEL:
			{
				beginRotateModel ();
				break;
			}
		case ID_SCALE_MODEL:
			{
				beginScaleModel ();
				break;
			}
		case ID_IMPORT:
			if (!_readonlyMode && _editor->getOpenFileNames ("nm", "nm文件|*.nm|", false, false) == 1)
			{
				_editor->endEditNodeTransform ();

				ATOM_HARDREF(ATOM_Geode) geode;
				ATOM_AUTOREF(ATOM_SharedModel) model = ATOM_CreateObject(ATOM_SharedModel::_classname(), 0);
				if (!model->load_nm (ATOM_GetRenderDevice(), _editor->getOpenedFileName (0)))
				{
					::MessageBox ((HWND)_editor->getRenderWindow()->getWindowId(), _T("读取NM文件失败！"), _T("ATOM3D Studio"), MB_OK|MB_ICONHAND);
					return;
				}
				_model = model;
				_editingPoint = false;

				if (!_geode)
				{
					_geode = ATOM_CreateObject(ATOM_Geode::_classname(), 0);
					_scene->getRootNode()->appendChild (_geode.get());
					//_editor->beginEditNodeRotation (_geode.get());
				}

				_geode->clear ();
				_geode->addSharedModel (_model.get(),ATOM_LoadPriority_IMMEDIATE );
				_geode->clearChildren ();
				_sharedMaterials.resize (_geode->getNumMeshes());
				for (unsigned i = 0; i < _geode->getNumMeshes(); ++i)
				{
					ATOM_Material *sharedMat = _geode->getStaticMesh(i)->getSharedMesh()->getMaterial();
					_sharedMaterials[i] = sharedMat->clone ();
					_geode->getStaticMesh (i)->setMaterial (sharedMat);
				}
				_edModelProp->setModel (_geode.get());
				createMeshList (_editor->getRealtimeCtrl());
				createTrackList (_editor->getRealtimeCtrl());

				_points.clear ();
				refreshPointList ();
			}
			break;

		case ID_EXPORT:
			if (_editor->getOpenFileNames ("csp", "csp文件|*.csp|", false, true) == 1)
			{
				exportAvatar (_editor->getOpenedFileName(0));
			}
			break;

		case ID_VIEW_TOGGLEGUI:
			{
				toggleGUI ();
				break;
			}
		
		case ID_VIEW_TOGGLEGRID:
			{
				toggleGrid ();
				break;
			}
		case ID_VIEW_TOGGLEBBOX:
			{
				toggleBBox ();
				break;
			}
		case ID_SELECT_SCENE:
			{
				selectScene ();
				break;
			}
		case ID_SELECT_BKIMAGE:
			{
				selectBkImage ();
				break;
			}

		case ID_DELETE_MESH:
			{
				if (!_readonlyMode)
				{
					int currentMesh = _edModelProp->getCurrentMesh();
					if (currentMesh >= 0)
					{
						ATOM_ASSERT(currentMesh < _model->getNumMeshes());
						ATOM_SharedMesh *mesh = _model->getMesh (currentMesh);
						for (int i = 0; i < _geode->getNumMeshes(); ++i)
						{
							if (_geode->getStaticMesh (i)->getSharedMesh() == mesh)
							{
								_geode->deleteStaticMesh (i);
								break;
							}
						}
						_edModelProp->setCurrentMesh (-1);
						_model->deleteMesh (currentMesh);
						_meshList->removeItem (currentMesh);
					}
				}
				break;
			}

		// wangjian added 
		case ID_GEN_CURVATUREMAP:
			{
				int currentMesh = _edModelProp->getCurrentMesh();
				if (currentMesh >= 0)
				{
					ATOM_ASSERT(currentMesh < _model->getNumMeshes());
					ATOM_SharedMesh *mesh = _model->getMesh (currentMesh);
					genMeshCurvatureMap( mesh,"" );
				}

				break;
			}
		case ID_SAVE_ISOLATION:
			if (_editor->getOpenFileNames ("nm2", "nm2文件|*.nm2|", false, true) == 1)
			{
				saveFileIsolation (_editor->getOpenedFileName(0));
			}
			break;
		////////////////////////////

		case ID_CREATE_POINT:
			{
				ATOM_STRING name = generatePointName ();
				newPointShape (name.c_str(), ATOM_Matrix4x4f::getIdentityMatrix());
				refreshPointList ();
				break;
			}

		case ID_RENAME_POINT:
			{
				renameCurrentPoint ();
				break;
			}

		case ID_DELETE_POINT:
			{
				deleteCurrentPoint ();
				break;
			}

		case ID_DELETE_ALL_POINTS:
			{
				clearPoints ();
				break;
			}

		case ID_EDIT_POINT_TRANSLATION:
			{
				int index = _pointList->getSelectIndex ();
				if (index >= 0)
				{
					const char *name = _pointList->getItemText (index)->getString ();
					ATOM_MAP<ATOM_STRING, ATOM_AUTOREF(ATOM_Node)>::iterator it = _points.find (name);
					ATOM_ASSERT(it != _points.end ());
					_editor->beginEditNodeTranslation (it->second.get());
					_editingPoint = true;
				}
				break;
			}

		case ID_EDIT_POINT_ROTATION:
			{
				int index = _pointList->getSelectIndex ();
				if (index >= 0)
				{
					const char *name = _pointList->getItemText (index)->getString ();
					ATOM_MAP<ATOM_STRING, ATOM_AUTOREF(ATOM_Node)>::iterator it = _points.find (name);
					ATOM_ASSERT(it != _points.end ());
					_editor->beginEditNodeRotation (it->second.get());
					_editingPoint = true;
				}
				break;
			}

		case ID_EDIT_POINT_SCALE:
			{
				int index = _pointList->getSelectIndex ();
				if (index >= 0)
				{
					const char *name = _pointList->getItemText (index)->getString ();
					ATOM_MAP<ATOM_STRING, ATOM_AUTOREF(ATOM_Node)>::iterator it = _points.find (name);
					ATOM_ASSERT(it != _points.end ());
					_editor->beginEditNodeScaling (it->second.get());
					_editingPoint = true;
				}
				break;
			}

		case ID_EDIT_POINT_ENDEDIT:
			{
				if (_editingModel)
				{
					endTransformModel ();
				}
				else
				{
					_editor->endEditNodeTransform ();
					_editingPoint = false;
				}
				break;
			}
		case ID_CAMERA_TO_POINT:
			{
				setCameraToCurrentPoint ();
				break;
			}
		case ID_POINT_TO_CAMERA:
			{
				setCurrentPointToCamera ();
				break;
			}
		case ID_EDIT_POINT_PROP:
			{
				int index = _pointList->getSelectIndex ();
				if (index >= 0)
				{
					ATOM_Dialog *dlg = dynamic_cast<ATOM_Dialog*>(_editor->getRealtimeCtrl()->loadHierarchy("/editor/ui/pointprop.ui"));
					if (dlg)
					{
						ATOM_Edit *edName = (ATOM_Edit*)dlg->getChildByIdRecursive (ID_EDNAME);
						ATOM_Edit *edX = (ATOM_Edit*)dlg->getChildByIdRecursive (ID_EDX);
						ATOM_Edit *edY = (ATOM_Edit*)dlg->getChildByIdRecursive (ID_EDY);
						ATOM_Edit *edZ = (ATOM_Edit*)dlg->getChildByIdRecursive (ID_EDZ);

						const char *pointName = _pointList->getItemText(index)->getString();
						edName->setString (pointName);
						ATOM_MAP<ATOM_STRING, ATOM_AUTOREF(ATOM_Node)>::iterator it = _points.find (pointName);
						ATOM_ASSERT(it != _points.end ());
						ATOM_Matrix4x4f o2t = it->second->getO2T();
						char buffer[64];
						sprintf (buffer, "%.2f", o2t.m30);
						edX->setString (buffer);
						sprintf (buffer, "%.2f", o2t.m31);
						edY->setString (buffer);
						sprintf (buffer, "%.2f", o2t.m32);
						edZ->setString (buffer);

						dlg->addEndId (ID_BTNOK);
						dlg->addEndId (ID_BTNCANCEL);
						if (dlg->showModal() == ID_BTNOK)
						{
							o2t.m30 = atof (edX->getString().c_str());
							o2t.m31 = atof (edY->getString().c_str());
							o2t.m32 = atof (edZ->getString().c_str());
							it->second->setO2T (o2t);
						}
						ATOM_DELETE(dlg);
					}
				}
				break;
			}
		}
	}
	else if (eventId == ATOM_ListBoxClickEvent::eventTypeId())
	{
		ATOM_ListBoxClickEvent *listBoxClickEvent = (ATOM_ListBoxClickEvent*)event;
		if (listBoxClickEvent->id == ID_MESHLIST)
		{
			_edModelProp->setCurrentMesh (_meshList->getSelectIndex ());
		}
	}
	else if (eventId == ATOM_ListBoxDblClickEvent::eventTypeId())
	{
		ATOM_ListBoxDblClickEvent *listBoxDblClickEvent = (ATOM_ListBoxDblClickEvent*)event;
		if (listBoxDblClickEvent->id == ID_TRACKLIST)
		{
			_geode->doAction (_trackList->getItemText(listBoxDblClickEvent->index)->getString(), ATOM_Geode::ACTIONFLAGS_DOWNSIDE|ATOM_Geode::ACTIONFLAGS_UPSIDE);
		}
	}
	else if (eventId == ATOM_WidgetContextMenuEvent::eventTypeId ())
	{
		ATOM_WidgetContextMenuEvent *e = (ATOM_WidgetContextMenuEvent*)event;
		if (e->id == ID_POINTLIST)
		{
			_pointList->trackPopupMenu (e->x, e->y, _pointListMenu);
		}
		else if (e->id == ID_MESHLIST)
		{
			_meshList->trackPopupMenu (e->x, e->y, _meshListMenu);
		}
	}
	else if (eventId == ATOM_WidgetResizeEvent::eventTypeId())
	{
		calcUILayout ();
	}
}

void PluginModel::newPointShape (const char *name, const ATOM_Matrix4x4f &matrix)
{
	_editor->endEditNodeTransform ();

	if (_geode)
	{
		ATOM_HARDREF(ATOM_Node) node;
		node->setO2T (matrix);
		_geode->appendChild (node.get());

		ATOM_HARDREF(ATOM_ShapeNode) shapeNode;
		shapeNode->loadAttribute (NULL);
		shapeNode->load (ATOM_GetRenderDevice());
		shapeNode->setType (ATOM_ShapeNode::SPHERE);
		shapeNode->setColor (ATOM_Vector4f(ATOM_randomf(0.f, 1.f), ATOM_randomf(0.f, 1.f), ATOM_randomf(0.f, 1.f), 1.f));
		shapeNode->setO2T (ATOM_Matrix4x4f::getScaleMatrix(ATOM_Vector3f(0.2f, 0.2f, 0.2f)));
		node->appendChild (shapeNode.get());

		_points[name] = node;
	}
}

void PluginModel::calcUILayout (void)
{
	ATOM_Rect2Di rc = _editor->getRealtimeCtrl()->getClientRect();
	_meshList->resize (ATOM_Rect2Di(0, 0, 200, rc.size.h/2));
	_trackList->resize (ATOM_Rect2Di(0, rc.size.h/2, 200, rc.size.h - rc.size.h/2));

	ATOM_Point2Di pt(rc.size.w - 200, 0);
	_editor->getRealtimeCtrl()->clientToGUI (&pt);

	_editor->setScenePropEditorPosition (pt.x, pt.y, 200, rc.size.h/3);

	if (_edModelProp->isShown ())
	{
		_edModelProp->setPosition (pt.x, pt.y+rc.size.h/3+1, 200, rc.size.h/3);
		_pointList->resize (ATOM_Rect2Di(pt.x, rc.size.h/3+rc.size.h/3+1, 200, rc.size.h-rc.size.h/3-rc.size.h/3-1));
	}
	else
	{
		_pointList->resize (ATOM_Rect2Di(pt.x, rc.size.h/3+1, 200, rc.size.h-rc.size.h/3-1));
	}
}

bool PluginModel::saveFile (const char *filename)
{
	_model->getAttributes ()->clearAttributes ();
	for (ATOM_MAP<ATOM_STRING, ATOM_AUTOREF(ATOM_Node)>::const_iterator it = _points.begin(); it != _points.end(); ++it)
	{
		_model->getAttributes()->setAttribute (it->first.c_str(), it->second->getO2T());
	}
	if (_model->save (filename))
	{
		_modelFileName = filename;

		// wangjian added
		// 重置共享材质
		resetSharedMaterials();

		return true;
	}
	return false;
}

unsigned PluginModel::getMinWindowWidth (void) const
{
	return 600;
}

unsigned PluginModel::getMinWindowHeight (void) const
{
	return 450;
}

bool PluginModel::isDocumentModified (void)
{
	return _editor->isDocumentModified ();
}

void PluginModel::handleTransformEdited (ATOM_Node *node)
{
}

ATOM_STRING PluginModel::generatePointName (void)
{
	int i = 0;
	char buffer[256];

	for (;;)
	{
		sprintf (buffer, "参考点%d", i++);
		if (_points.find (buffer) == _points.end ())
		{
			return buffer;
		}
	}
}

void PluginModel::refreshPointList (void)
{
	_pointList->clearItem ();

	for (ATOM_MAP<ATOM_STRING, ATOM_AUTOREF(ATOM_Node)>::const_iterator it = _points.begin(); it != _points.end(); ++it)
	{
		_pointList->addItem (it->first.c_str());
	}
}

void PluginModel::clearPoints (void)
{
	_editor->endEditNodeTransform ();
	_geode->clearChildren ();
	_points.clear ();
	refreshPointList ();
}

void PluginModel::deleteCurrentPoint (void)
{
	int index = _pointList->getSelectIndex ();
	if (index >= 0)
	{
		_editor->endEditNodeTransform ();

		ATOM_STRING name = _pointList->getItemText (index)->getString ();
		ATOM_MAP<ATOM_STRING, ATOM_AUTOREF(ATOM_Node)>::iterator it = _points.find (name);
		ATOM_ASSERT(it != _points.end ());
		it->second->getParent()->removeChild (it->second.get());
		_points.erase (it);
		refreshPointList();
	}
}

void PluginModel::setCurrentPointToCamera (void)
{
	int index = _pointList->getSelectIndex ();
	if (index >= 0)
	{
		ATOM_STRING name = _pointList->getItemText (index)->getString ();
		ATOM_MAP<ATOM_STRING, ATOM_AUTOREF(ATOM_Node)>::iterator it = _points.find (name);
		ATOM_ASSERT(it != _points.end ());
		it->second->setO2T(it->second->getParent()->getInvWorldMatrix() >> _editor->getCameraModal()->getViewMatrix());
	}
}

void PluginModel::setCameraToCurrentPoint (void)
{
	int index = _pointList->getSelectIndex ();
	if (index >= 0)
	{
		ATOM_STRING name = _pointList->getItemText (index)->getString ();
		ATOM_MAP<ATOM_STRING, ATOM_AUTOREF(ATOM_Node)>::iterator it = _points.find (name);
		ATOM_ASSERT(it != _points.end ());

		const ATOM_Matrix4x4f &matWorld = it->second->getWorldMatrix ();
		ATOM_Vector3f pos, target, up;
		matWorld.decomposeLookatLH (pos, target, up);
		_editor->getCameraModal()->setPosition (pos);
		_editor->getCameraModal()->setDirection (target - pos);
		_editor->getCameraModal()->setUpVector (ATOM_Vector3f(0.f, 1.f, 0.f));
	}
}

void PluginModel::renameCurrentPoint (void)
{
	int index = _pointList->getSelectIndex ();
	if (index >= 0)
	{
		ATOM_STRING oldName = _pointList->getItemText (index)->getString ();
		ATOM_STRING newName = oldName;

		if (_editor->showRenameDialog (newName))
		{
			if (newName != oldName)
			{
				ATOM_MAP<ATOM_STRING, ATOM_AUTOREF(ATOM_Node)>::iterator it = _points.find (newName);
				if (it != _points.end())
				{
					MessageBoxA (ATOM_APP->getMainWindow(), "名字重复!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
				}
				else
				{
					_points[newName] = _points[oldName];
					_points.erase (_points.find (oldName));

					refreshPointList();
				}
			}
		}
	}
}

static bool transformStream (ATOM_VertexArray *va, const ATOM_Matrix4x4f &matrix, bool transformVector, bool normalize)
{
	bool ret = false;

	ATOM_Vector3f *vertex = (ATOM_Vector3f *)va->lock (ATOM_LOCK_READONLY, 0, 0, false);
	if (vertex)
	{
		ATOM_Vector3f *newVertices = ATOM_NEW_ARRAY(ATOM_Vector3f, va->getNumVertices());
		for (int i = 0; i < va->getNumVertices(); ++i)
		{
			newVertices[i] = transformVector ? matrix.transformVector(vertex[i]) : matrix.transformPoint (vertex[i]);
			if (normalize)
			{
				newVertices[i].normalize();
			}
		}
		va->unlock ();

		ATOM_Vector3f *p = (ATOM_Vector3f*)va->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		if (p)
		{
			memcpy (p, newVertices, va->getNumVertices() * sizeof(ATOM_Vector3f));
			va->unlock ();
			ret = true;
		}
		ATOM_DELETE_ARRAY (newVertices);
	}

	return ret;
}

static bool transformMesh (ATOM_SharedMesh *mesh, const ATOM_Matrix4x4f &matrix)
{
	ATOM_MultiStreamGeometry *geometry = dynamic_cast<ATOM_MultiStreamGeometry*>(mesh->getGeometry());
	if (geometry)
	{
		ATOM_VertexArray *vertices = geometry->getStream (ATOM_VERTEX_ATTRIB_COORD);
		if (vertices)
		{
			if (!transformStream (vertices, matrix, false, false))
			{
				return false;
			}
		}

		ATOM_Matrix4x4f m = matrix;
		m.invertAffine ();
		m.transpose ();

		ATOM_VertexArray *normals = geometry->getStream (ATOM_VERTEX_ATTRIB_NORMAL);
		if (normals)
		{
			if (!transformStream (normals, m, true, true))
			{
				return false;
			}
		}

		ATOM_VertexArray *binormals = geometry->getStream (ATOM_VERTEX_ATTRIB_BINORMAL);
		if (binormals)
		{
			if (!transformStream (binormals, m, true, true))
			{
				return false;
			}
		}

		ATOM_VertexArray *tangents = geometry->getStream (ATOM_VERTEX_ATTRIB_TANGENT);
		if (tangents)
		{
			if (!transformStream (tangents, m, true, true))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

static ATOM_JointTransformInfo convertMatrixToTransformInfo (const ATOM_Matrix4x4f &mat)
{
	ATOM_JointTransformInfo info;
	info.translate.set(mat.m30, mat.m31, mat.m32, 1.f);
	float sx = sqrtf(mat.m00 * mat.m00 + mat.m01 * mat.m01 + mat.m02 * mat.m02);
	float sy = sqrtf(mat.m10 * mat.m10 + mat.m11 * mat.m11 + mat.m12 * mat.m12);
	float sz = sqrtf(mat.m20 * mat.m20 + mat.m21 * mat.m21 + mat.m22 * mat.m22);
	info.scale.set (sx, sy, sz, 0.f);
	ATOM_Matrix4x4f matRotate(
		mat.m00/sx, mat.m01/sx, mat.m02/sx, 0.f,
		mat.m10/sy, mat.m11/sy, mat.m12/sy, 0.f,
		mat.m20/sz, mat.m21/sz, mat.m22/sz, 0.f,
		0.f,        0.f,        0.f,        1.f
		);
	info.rotate.fromMatrix (matRotate);
	return info;
}

bool PluginModel::transformModel (const ATOM_Matrix4x4f &matrix)
{
	for (int i = 0; i < _model->getNumMeshes(); ++i)
	{
		if (!transformMesh(_model->getMesh (i), matrix))
		{
			return false;
		}
	}
	ATOM_Skeleton *skeleton = _model->getSkeleton();
	if (skeleton)
	{
		skeleton->transformJoints (matrix); 
	}

#if 0
	ATOM_JointAnimationDataCache *animationCache = 0;
	for (int i = 0; i < _model->getNumTracks(); ++i)
	{
		if (!animationCache)
		{
			animationCache = _model->getTrack(i)->getAnimationDataCache ();
			if (animationCache)
			{
				for (int i = 0; i < animationCache->getNumFrames(); ++i)
				{
					for (int j = 0; j < animationCache->getNumJoints(i); ++j)
					{
						ATOM_ALIGN(16) ATOM_Matrix3x4f m;
						ATOM_JointTransformInfo &info = animationCache->getJoints(i)[j];
						convertJointQuatToJointMat (&m, &info, 1);
						ATOM_Matrix4x4f m2;
						m.toMatrix44 (m2);
						ATOM_Matrix4x4f mi;
						mi.invertAffineFrom (matrix);
						m2 = m2 >> mi;
						info = convertMatrixToTransformInfo (m2);
					}
				}
				break;
			}
		}
	}
#endif

	return true;
}

void PluginModel::beginTranslateModel (void)
{
	if (_editingPoint)
	{
		_editor->endEditNodeTransform ();
		_editingPoint = false;
	}

	_editor->beginEditNodeTranslation (_geode.get());
	_editingModel = true;
}

void PluginModel::beginRotateModel (void)
{
	if (_editingPoint)
	{
		_editor->endEditNodeTransform ();
		_editingPoint = false;
	}

	_editor->beginEditNodeRotation (_geode.get());
	_editingModel = true;
}

void PluginModel::beginScaleModel (void)
{
	if (_editingPoint)
	{
		_editor->endEditNodeTransform ();
		_editingPoint = false;
	}

	_editor->beginEditNodeScaling (_geode.get());
	_editingModel = true;
}

void PluginModel::endTransformModel (void)
{
	if (_editingModel)
	{
		_editor->endEditNodeTransform ();

		if (IDYES == ::MessageBoxA (ATOM_APP->getMainWindow(), "是否保存对模型的修改？", "ATOM3D编辑器", MB_YESNO|MB_ICONQUESTION))
		{
			if (!transformModel (_geode->getO2T()))
			{
				::MessageBoxA (ATOM_APP->getMainWindow(), "保存失败！", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
			}
		}
		_geode->setO2T (ATOM_Matrix4x4f::getIdentityMatrix());
		_editingModel = false;
	}
}

void PluginModel::exportAvatar (const char *filename)
{
	if (!_geode || _geode->getNumMeshes () == 0)
	{
		::MessageBoxA (ATOM_APP->getMainWindow(), "未找到Mesh!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
		return;
	}

	if (_modelFileName.empty ())
	{
		::MessageBoxA (ATOM_APP->getMainWindow(), "请先保存模型!", "ATOM3D编辑器", MB_OK|MB_ICONASTERISK);
		return; 
	}

	char buffer[ATOM_VFS::max_filename_length];
	char buffer2[ATOM_VFS::max_filename_length];

	ATOM_TiXmlDocument doc;

	ATOM_TiXmlElement eRoot("node");
	eRoot.SetAttribute ("class", "ClientSimpleCharacter");
	eRoot.SetAttribute ("ModelFileName", _modelFileName.c_str());

	for (ATOM_MAP<ATOM_STRING, ATOM_AUTOREF(ATOM_Node)>::const_iterator it = _points.begin(); it != _points.end(); ++it)
	{
		char buffer[512];
		const ATOM_Matrix4x4f &m = it->second->getO2T();
		sprintf (buffer, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f", 
				m.m00, m.m01, m.m02, m.m03,
				m.m10, m.m11, m.m12, m.m13,
				m.m20, m.m21, m.m22, m.m23,
				m.m30, m.m31, m.m32, m.m33);
		ATOM_TiXmlElement ePoint("point");
		ePoint.SetAttribute ("Name", it->first.c_str());
		ePoint.SetAttribute ("Matrix", buffer);
		eRoot.InsertEndChild (ePoint);
	}

	float fov = _editor->getCameraModal()->getFovY();
	ATOM_TiXmlElement eFov("FOV");
	eFov.SetDoubleAttribute("value", fov);
	eRoot.InsertEndChild (eFov);

	ATOM_Sky *sky = (ATOM_Sky*)ATOMX_FindFirstNodeByClassName (_scene->getRootNode(), ATOM_Sky::_classname());
	if (sky)
	{
		ATOM_Light *light = sky->getSunLight();
		ATOM_ColorARGB lightcolor = light->getColorARGB();
		ATOM_TiXmlElement eLightColor("lightcolor");
		eLightColor.SetAttribute("value", int(lightcolor.getRaw()));
		eRoot.InsertEndChild (eLightColor);

		ATOM_Vector3f lightdir = light->getDirection();
		char buffer[128];
		sprintf (buffer, "%.2f,%.2f,%.2f", lightdir.x, lightdir.y, lightdir.z);
		ATOM_TiXmlElement eLightDir("lightdir");
		eLightDir.SetAttribute ("value", buffer);
		eRoot.InsertEndChild (eLightDir);

		float lightIntensity = sky->getLightIntensity();
		ATOM_TiXmlElement eLightIntensity("lightintensity");
		eLightIntensity.SetDoubleAttribute("value", lightIntensity);
		eRoot.InsertEndChild (eLightIntensity);

		const ATOM_Vector4f &ambient = _scene->getAmbientLight();
		ATOM_ColorARGB c(ambient.x, ambient.y, ambient.z, 1.f);
		ATOM_TiXmlElement eAmbient("ambient");
		eAmbient.SetAttribute ("value", int(c.getRaw()));
		eRoot.InsertEndChild (eAmbient);
	}

	strcpy (buffer, filename);
	char *p = strrchr (buffer, '.');
	if (p)
	{
		*p = '\0';
	}

	for (int i = 0; i < _geode->getNumMeshes(); ++i)
	{
		ATOM_Material *mat = _realMaterials.empty() ? _geode->getStaticMesh(i)->getMaterial() : _realMaterials[i].get();
		if (!mat->isSameAs (_sharedMaterials[i].get()))
		{
			sprintf(buffer2, "%s_mat%d.xml", buffer, i);
			if (!mat->saveXML (buffer2))
			{
				::MessageBoxA (ATOM_APP->getMainWindow(), "保存材质失败!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
				return;
			}
			ATOM_TiXmlElement eMat ("mat");
			eMat.SetAttribute ("mesh", i);
			eMat.SetAttribute ("material", buffer2);
			eRoot.InsertEndChild (eMat);
		}
	}

	doc.InsertEndChild (eRoot);

	ATOM_GetNativePathName(filename, buffer);
	if (!doc.SaveFile(buffer))
	{
		::MessageBoxA (ATOM_APP->getMainWindow(), "保存换皮模型失败!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
	}
}

void PluginModel::handleScenePropChanged (void)
{
}

void PluginModel::changeRenderScheme (void)
{
	_scene->setRenderScheme (_editor->getRenderScheme());

	if (!_geode)
	{
		return;
	}

	if (dynamic_cast<ATOM_CustomRenderScheme*>(_editor->getRenderScheme()))
	{
		_realMaterials.resize (0);
		for (int i = 0; i < _geode->getNumMeshes(); ++i)
		{
			_realMaterials.push_back (_geode->getStaticMesh(i)->getMaterial());
		}
		_readonlyMode = true;

		_edModelProp->show (false);
	}
	else
	{
		for (int i = 0; i < _geode->getNumMeshes(); ++i)
		{
			_geode->getStaticMesh(i)->setMaterial(_realMaterials[i].get());
		}
		_realMaterials.resize (0);
		_readonlyMode = false;

		_edModelProp->show (true);
	}

	calcUILayout ();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void drawQuad_genCurvature (ATOM_RenderDevice *device, ATOM_Material *material, int w, int h)
{
	float deltax = (float)0.5f/(float)w;
	float deltay = (float)0.5f/(float)h;

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
		unsigned numPasses = material->begin (device);
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (material->beginPass (device, pass))
			{
				device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_FAN, 4, 4, ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2, 5 * sizeof(float), vertices, indices);
				material->endPass (device, pass);
			}
		}
		material->end (device);
	}
}

//========================================================================//
// wangjian added 
void PluginModel::genMeshCurvatureMap( ATOM_SharedMesh * mesh, const char* filename )
{
	if( !mesh )
		return ;

	if( !filename || filename[0] == '\0 ')
		return;

	ATOM_RenderDevice * device = ATOM_GetRenderDevice();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static ATOM_AUTOPTR(ATOM_Material) material = 0;
	if (!material)
	{
		material = ATOM_MaterialManager::createMaterialFromCore (device, "/editor/materials/genMeshCurvature.mat");
		if (!material)
		{
			return;
		}
	}

	const int width = 2048;
	const int height = 2048;
	static ATOM_AUTOREF(ATOM_DepthBuffer) depthbuffer = device->allocDepthBuffer (width, height);
	static ATOM_AUTOREF(ATOM_Texture) pos_texture[2] = {0};
	static ATOM_AUTOREF(ATOM_Texture) norm_texture[2] = {0};
	static ATOM_AUTOREF(ATOM_Texture) curv_texture[2] = {0};
	if( !pos_texture[0] || !pos_texture[1] )
	{
		pos_texture[0] = device->allocTexture (0, 0, width, height, ATOM_PIXEL_FORMAT_RGBA32F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
		pos_texture[1] = device->allocTexture (0, 0, width, height, ATOM_PIXEL_FORMAT_RGBA32F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
	}
	if( !norm_texture[0] || !norm_texture[1] )
	{
		norm_texture[0] = device->allocTexture (0, 0, width, height, ATOM_PIXEL_FORMAT_RGBA32F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
		norm_texture[1] = device->allocTexture (0, 0, width, height, ATOM_PIXEL_FORMAT_RGBA32F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
	}
	if( !curv_texture[0] || !curv_texture[1] )
	{
		curv_texture[0] = device->allocTexture (0, 0, width, height, ATOM_PIXEL_FORMAT_RGBA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
		curv_texture[1] = device->allocTexture (0, 0, width, height, ATOM_PIXEL_FORMAT_RGBA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
	}

	ATOM_Vector4f texelsize( 1.0/curv_texture[0]->getWidth(),1.0f/curv_texture[0]->getHeight(),0,0);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	pos_texture[0]->clear (0.f, 0.f, 0.f, 0.f);
	norm_texture[0]->clear (0.f, 0.f, 0.f, 0.f);

	ATOM_AUTOREF(ATOM_DepthBuffer) oldDepth = device->getDepthBuffer ();
	ATOM_AUTOREF(ATOM_Texture) oldRT = device->getRenderTarget (0);
	ATOM_Rect2Di oldVP = device->getViewport (NULL);

	device->setRenderTarget(0,pos_texture[0].get());
	device->setRenderTarget(1,norm_texture[0].get());
	device->setViewport (NULL, ATOM_Rect2Di(0, 0, width, height));
	device->setDepthBuffer (depthbuffer.get());
	device->beginFrame ();

	device->setTransform (ATOM_MATRIXMODE_WORLD, ATOM_Matrix4x4f::getIdentityMatrix());

	material->setActiveEffect("genPositionNormal");

	material->getParameterTable()->setVector("texelSize",texelsize);

	unsigned numPasses = material->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (material->beginPass (device, pass))
		{
			mesh->drawWithoutMaterial (device);
			
			material->endPass (device, pass);
		}
	}
	material->end (device);

	device->endFrame ();

	if( 1 )
	{
		pos_texture[0]->saveToFile("positionmap.dds");
		norm_texture[0]->saveToFile("normalmap.dds");
	}

	device->setRenderTarget (1, 0);

	///////////////////////////////////////////////////////////////////////


	curv_texture[0]->clear (0.f, 0.f, 0.f, 0.f);

	material->setActiveEffect("genCurvature");

	device->beginFrame ();

	
	material->getParameterTable()->setVector("texelSize",texelsize);

	material->getParameterTable()->setFloat("curveScale",0.005f);
	material->getParameterTable()->setTexture ("positionTexture", pos_texture[0].get());
	material->getParameterTable()->setTexture ("normalTexture",  norm_texture[0].get());

	device->setRenderTarget (0,curv_texture[0].get());
	device->setViewport (0, ATOM_Rect2Di(0, 0, curv_texture[0]->getWidth(), curv_texture[0]->getHeight()));

	drawQuad_genCurvature (device, material.get(), curv_texture[0]->getWidth(), curv_texture[0]->getHeight());

	device->endFrame ();

	if( 1 )
	{
		curv_texture[0]->saveToFile("curvature.dds");
	}

	///////////////////////////////////////////////////////////////////////

	curv_texture[1]->clear (0.f, 0.f, 0.f, 0.f);

	material->setActiveEffect("blur");
	device->beginFrame ();
	
	device->setRenderTarget (0,curv_texture[1].get());
	
	material->getParameterTable()->setVector("texelSize",texelsize);
	material->getParameterTable()->setTexture ("curvatureTexture", curv_texture[0].get());

	device->setViewport (0, ATOM_Rect2Di(0, 0, curv_texture[1]->getWidth(), curv_texture[1]->getHeight()));
	drawQuad_genCurvature (device, material.get(), curv_texture[1]->getWidth(), curv_texture[1]->getHeight());
	device->endFrame ();

	device->setRenderTarget (1, 0);

	if( 1 )
	{
		curv_texture[1]->saveToFile("curvature_blur.dds");
	}

	///////////////////////////////////////////////////////////////////////


	device->setRenderTarget (0, oldRT.get());
	device->setDepthBuffer (oldDepth.get());
	device->setViewport (NULL, oldVP);
}

bool PluginModel::saveFileIsolation (const char *filename)
{
	_model->getAttributes ()->clearAttributes ();
	for (ATOM_MAP<ATOM_STRING, ATOM_AUTOREF(ATOM_Node)>::const_iterator it = _points.begin(); it != _points.end(); ++it)
	{
		_model->getAttributes()->setAttribute (it->first.c_str(), it->second->getO2T());
	}

	if (_model->saveIsolation (filename))
	{
		_modelFileName = filename;

		// 重置共享材质
		resetSharedMaterials();

		return true;
	}
	return false;
}

void PluginModel::resetSharedMaterials()
{
	_sharedMaterials.resize (_geode->getNumMeshes());
	for (unsigned i = 0; i < _geode->getNumMeshes(); ++i)
	{
		ATOM_Material *sharedMat = _geode->getStaticMesh(i)->getSharedMesh()->getMaterial();
		_sharedMaterials[i] = sharedMat->clone ();
		_geode->getStaticMesh (i)->setMaterial (sharedMat);
	}
}
//========================================================================//