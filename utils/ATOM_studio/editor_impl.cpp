#include "StdAfx.h"
#include "app.h"
#include "atom3d_studio.h"
#include "editor.h"
#include "editor_impl.h"
#include "plugin.h"
#include "lightgeom.h"
#include "camera_modal.h"
#include "scene_plugin/locked_camera_modal.h"		// wangjian added
#include "camera_maya.h"
#include "assetmanager.h"
#include "asset.h"
#include "asset_editor.h"
#include "asset_light.h"
#include "asset_decal.h"
#include "asset_bkimage.h"
#include "asset_shape.h"
#include "asset_hud.h"
#include "asset_particlesys.h"
#include "asset_loadable.h"
#include "asset_node.h"
#include "asset_terrain.h"
#include "asset_weapontrail.h"
#include "native_ops.h"
#include "gridnode.h"
#include "rendermanager.h"
#include "clipboard.h"
#include "colorgrading_editor.h"
#include "rename.h"
#include "curved.h"
#include "scene_prop_editor.h"

#define TEST_CURVEEDITOR 0
#define TEST_TRAIL 0

#if TEST_TRAIL

ATOM_AUTOREF(ATOM_Node) targetNode;
ATOM_AUTOREF(ATOM_RibbonTrail) lineTrail;;
const float rotateSpeed = 0.f;

#endif


enum CameraOptionID
{
	CO_FOV = 0,
	CO_NEAR = 1,
	CO_FAR = 2,
	CO_MIN_CONTRIB = 3,
	CO_POSITION = 4,

	CO_VIEWPORT_W = LockedCamera::LCO_END,			// wangjian modified : 从LockedCamera::LCO_END开始
	CO_VIEWPORT_H,
	CO_MOVE_SPEED,
	CO_ZOOM_SPEED,
	CO_ROTATE_SPEED,
	CO_ROTATE_RADIUS,
};

EditorImpl::EditorImpl (void)
{
	_guiRenderer = 0;
	_realtimeCtrl = 0;
	_captureCounter = 0;
	_currentRenderManagerName = 0;
	_currentEditingPlugin = 0;
	_assetEditor = 0;
	_scenePropEditor = 0;
	_colorGradingEditor = 0;
	_cameraModal = 0;
	_defaultCameraModal = ATOM_NEW(CameraMaya);
	_lightAssetManager = 0;
	_decalAssetManager = 0;
	_bkImageAssetManager = 0;
	_shapeAssetManager = 0;
	_hudAssetManager = 0;
	_terrainAssetManager = 0;
	_particleSysAssetManager = 0;
	_nodeAssetManager = 0;
	_weaponTrailAssetManager = 0;
	_loadableAssetManager = 0;
	_curveEditor = 0;
	_D3DCapsView = 0;
	_cameraOptionView = 0;
	_CapsViewCommandCallback = ATOM_NEW(CapsViewCommandCallback, this);
	_currentMouseX = 0;
	_currentMouseY = 0;
	_documentModified = false;
	_renderMode = RM_DEFERRED;
	_schemeMode = SCHEME_DEFERRED;
	_currentOp = 0;
	_isEditingNodeTranslation = false;
	_isEditingNodeRotation = false;
	_isEditingNodeScaling = false;
	_renderSchemeDeferred = 0;
	_renderSchemeForward = 0;
	_clipboard = ATOM_NEW(AS_Clipboard);
}

void EditorImpl::init (ATOM_RenderWindow *renderWindow, ATOM_GUIRenderer *guiRenderer, ATOM_RealtimeCtrl *realtimeCtrl)
{
	_renderWindow = renderWindow;
	_guiRenderer = guiRenderer;
	_realtimeCtrl = realtimeCtrl;
	_cameraModal = _defaultCameraModal;

	markDummyNodeType (GridNode::_classname());
	markDummyNodeType (ATOMX_AxisNode::_classname());
	markDummyNodeType (LightGeode::_classname());
	markDummyNodeType (ATOMX_TrackBallNode::_classname());

	allocClipboardContentType (CB_TYPE_SCENEPROP);
	allocClipboardContentType (CB_TYPE_SCENEPROP_ALLTIME);
}

void EditorImpl::initAssets (void)
{
	_loadableAssetManager = ATOM_NEW(LoadableAssetManager);
	_lightAssetManager = ATOM_NEW(LightAssetManager);
	_decalAssetManager = ATOM_NEW(DecalAssetManager);
	_bkImageAssetManager = ATOM_NEW(BkImageAssetManager);
	_shapeAssetManager = ATOM_NEW(ShapeAssetManager);
	_hudAssetManager = ATOM_NEW(HudAssetManager);
	_terrainAssetManager = ATOM_NEW(TerrainAssetManager);
	_particleSysAssetManager = ATOM_NEW(ParticleSysAssetManager);
	_nodeAssetManager = ATOM_NEW(NodeAssetManager);
	_weaponTrailAssetManager = ATOM_NEW(WeaponTrailAssetManager);

	registerAssetManager (_lightAssetManager, "光源(Light)");
	registerAssetManager (_decalAssetManager, "贴花(Decal)");
	registerAssetManager (_loadableAssetManager, "ATOM3D Nodes");
	registerAssetManager (_bkImageAssetManager, "背景(Background)");
	registerAssetManager (_shapeAssetManager, "形状(Shape)");
	//registerAssetManager (_hudAssetManager, "ATOM3D HUD xx");
	registerAssetManager (_terrainAssetManager, "地形(Terrain)");
	registerAssetManager (_particleSysAssetManager, "粒子系统(ParticleSystem)");
	registerAssetManager (_nodeAssetManager, "空节点(Node)");
	registerAssetManager (_weaponTrailAssetManager, "刀光(WeaponTrail)");
}

EditorImpl::~EditorImpl (void)
{
	for (unsigned i = 0; i < _plugins.size(); ++i)
	{
		_plugins[i]->donePlugin ();
		_plugins[i]->deleteMe ();
	}
	_plugins.clear ();
	_pluginInfos.clear ();

	ATOM_DELETE(_defaultCameraModal);
	ATOM_DELETE(_assetEditor);
	ATOM_DELETE(_scenePropEditor);
	ATOM_DELETE(_colorGradingEditor);
	ATOM_DELETE(_loadableAssetManager);
	ATOM_DELETE(_lightAssetManager);
	ATOM_DELETE(_decalAssetManager);
	ATOM_DELETE(_bkImageAssetManager);
	ATOM_DELETE(_shapeAssetManager);
	ATOM_DELETE(_hudAssetManager);
	ATOM_DELETE(_terrainAssetManager);
	ATOM_DELETE(_particleSysAssetManager);
	ATOM_DELETE(_nodeAssetManager);
	ATOM_DELETE(_weaponTrailAssetManager);
	ATOM_DELETE(_D3DCapsView);
	ATOM_DELETE(_cameraOptionView);
	ATOM_DELETE(_CapsViewCommandCallback);
	ATOM_DELETE(_clipboard);
	ATOM_DELETE(_curveEditor);

	free ((void*)_currentRenderManagerName);
	_currentRenderManagerName = 0;

	ATOM_RenderScheme::destroyRenderScheme (_renderSchemeDeferred);
	_renderSchemeDeferred = 0;

	ATOM_RenderScheme::destroyRenderScheme (_renderSchemeForward);
	_renderSchemeForward = 0;
}

unsigned EditorImpl::getVersion (void) const
{
	return AS_VERSION;
}

ATOM_RenderWindow *EditorImpl::getRenderWindow (void) const
{
	return _renderWindow.get();
}

ATOM_GUIRenderer *EditorImpl::getGUIRenderer (void) const
{
	return _guiRenderer;
}

ATOM_RealtimeCtrl *EditorImpl::getRealtimeCtrl (void) const
{
	return _realtimeCtrl;
}

ATOM_MenuBar *EditorImpl::getMenuBar (void) const
{
	return _realtimeCtrl->getMenuBar ();
}

bool EditorImpl::registerFileType (AS_Plugin *plugin, const char *ext, const char *desc, unsigned editFlags)
{
	if (plugin && ext)
	{
		char ext2[256];
		strcpy (ext2, ext);
		strlwr (ext2);

		_pluginInfos.resize (_pluginInfos.size() + 1);
		_pluginInfos.back().plugin = plugin;
		_pluginInfos.back().desc = desc ? desc : "未知类型";
		_pluginInfos.back().ext = ext2;
		_pluginInfos.back().editFlags = editFlags;

		return true;
	}

	return false;
}

bool EditorImpl::registerRenderManager (AS_RenderManager *manager, const char *name)
{
	if (name && manager)
	{
		char buffer[256];
		strcpy (buffer, name);
		strlwr (buffer);

		if (_renderManagerMap.find (buffer) != _renderManagerMap.end ())
		{
			return false;
		}

		_renderManagerMap[buffer] = manager;
		return true;
	}
	return false;
}

bool EditorImpl::registerAssetManager (AS_AssetManager *manager, const char *name)
{
	if (name && manager)
	{
		char buffer[256];
		strcpy (buffer, name);
		strlwr (buffer);

		if (_assetManagerMap.find (buffer) != _assetManagerMap.end ())
		{
			return false;
		}

		_assetManagerMap[buffer] = manager;
		manager->setEditor (this);
		return true;
	}

	return false;
}

void EditorImpl::addPlugin (AS_Plugin *plugin)
{
	if (plugin && std::find (_plugins.begin(), _plugins.end(), plugin) == _plugins.end ())
	{
		_plugins.push_back (plugin);
	}
}

ATOM_STRING EditorImpl::buildFileFilterString (void) const
{
	ATOM_STRING str;

	for (unsigned i = 0; i < _pluginInfos.size(); ++i)
	{
		const PluginInfo &info = _pluginInfos[i];

		str += info.desc;
		str += "(*.";
		str += info.ext;
		str += ")|*.";
		str += info.ext;
		str += "|";
	}

	return str;
}

bool EditorImpl::isDocumentEditing (void)
{
	return _currentEditingPlugin != NULL;
}

void EditorImpl::closeDocument (void)
{
	if (_currentEditingPlugin)
	{
		if (needPromptForSave ())
		{
			int id = ::MessageBox (_renderWindow->getWindowInfo()->handle, _T("文件已经被更改，是否要保存？"), _T("ATOM3D Studio"), MB_YESNOCANCEL|MB_ICONQUESTION);
			switch (id)
			{
			case IDYES:
				{
					if (!saveDocument ())
					{
						return;
					}
					break;
				}
			case IDNO:
				{
					break;
				}
			case IDCANCEL:
				{
					return;
				}
			}
		}

		endEditNodeTransform ();
		setCameraModal (_defaultCameraModal);

		showAssetEditor (false);
		showColorGradingEditor (false, 0);

		_currentEditingPlugin->endEdit ();
		_currentEditingPlugin = 0;
		_currentFile.clear ();
		setDocumentModified (false);

		updateStudioCaption (false);

		clearUndoList ();

		_editingNode = 0;

		//-------------------------------------------------//
		// wangjian modified
#if 0
		if( !ATOM_RenderSettings::isNonDeferredShading() )
			setSchemeMode (SCHEME_DEFERRED);
#else
		setSchemeMode (SCHEME_DEFERRED);
#endif
		//-------------------------------------------------//
	}
}

void EditorImpl::clearUndoList (void)
{
	for (unsigned i = 0; i < _editOps.size(); ++i)
	{
		_editOps[i]->deleteMe ();
	}
	_editOps.clear ();
	_currentOp = 0;
}

bool EditorImpl::saveDocument (void)
{
	if (_currentEditingPlugin)
	{
		if (_currentFile.empty ())
		{
			return saveDocumentAs ();
		}
		else
		{
			if (_currentEditingPlugin->saveFile (_currentFile.c_str()))
			{
				setDocumentModified (false);
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

bool EditorImpl::saveDocumentAs (void)
{
	if (_currentEditingPlugin)
	{
		ATOM_STRING filter;
		filter += _currentDesc;
		filter += "(*.";
		filter += _currentExt;
		filter += ")|*.";
		filter += _currentExt;
		filter += "|";

		ATOM_FileDlg dlg(ATOM_FileDlg::MODE_SAVEFILE|ATOM_FileDlg::MODE_OVERWRITEPROMPT, NULL, _currentExt.c_str(), NULL, filter.c_str(), _renderWindow->getWindowInfo()->handle);
		if (IDOK == dlg.doModal ())
		{
			ATOM_STRING s = dlg.getSelectedFileName (0);
			if (!_currentEditingPlugin->saveFile (s.c_str()))
			{
				::MessageBox (_renderWindow->getWindowInfo()->handle, _T("保存失败"), _T("ATOM3D Studio"), MB_OK|MB_ICONHAND);
				return false;
			}
			else
			{
				_currentFile = s;
				updateStudioCaption (false);
				setDocumentModified (false);
				return true;
			}
		}
	}
	return false;
}

void EditorImpl::newDocument (unsigned pluginInfoIndex)
{
	if (pluginInfoIndex >= _pluginInfos.size())
	{
		return;
	}

	closeDocument ();
	if (_currentEditingPlugin)
	{
		return;
	}

	const PluginInfo &info = _pluginInfos[pluginInfoIndex];

	initMenuStates (0);
	if (info.plugin->beginEdit (0))
	{
		_currentEditingPlugin = info.plugin;
		_currentFile = "";
		_currentExt = info.ext;
		_currentDesc = info.desc;
		getCameraModal()->reset (_realtimeCtrl);
		updateStudioCaption (false);

		checkForWindowSize (info.plugin);
	}
}

void EditorImpl::openDocument (const char *filename, const char *desc)
{
	char szBuffer[ATOM_VFS::max_filename_length];
	strcpy (szBuffer, filename);

	char *ext = strrchr (szBuffer, '.');
	if (!ext)
	{
		return;
	}
	ext++;
	strlwr (ext);

	for (unsigned i = 0; i < _pluginInfos.size(); ++i)
	{
		const PluginInfo &info = _pluginInfos[i];

		if ((!desc || info.desc == desc) && info.ext == ext)
		{
			AS_Plugin *plugin = info.plugin;

			closeDocument ();
			if (_currentEditingPlugin)
			{
				return;
			}

			initMenuStates (0);

			_currentEditingPlugin = plugin;
			if (plugin->beginEdit (szBuffer))
			{
				_currentFile = szBuffer;
				_currentExt = ext;
				_currentDesc = info.desc;
				getCameraModal()->reset (_realtimeCtrl);
				updateStudioCaption (false);

				checkForWindowSize (plugin);
			}
			else
			{
				_currentEditingPlugin = 0;
				::MessageBoxA (ATOM_APP->getMainWindow(), "无法打开指定的文件，请检查文件是否存在以及文件格式是否正确。", "ATOM Studio", MB_OK|MB_ICONHAND);
			}

			break;
		}
	}
}

void EditorImpl::openDocument (void)
{
	if (_pluginInfos.empty ())
	{
		::MessageBoxA (_renderWindow.get()->getWindowInfo()->handle, "没有注册任何文件类型插件", "ATOM Studio", MB_OK|MB_ICONHAND);
		return;
	}

	ATOM_STRING str = buildFileFilterString ();

	ATOM_FileDlg dlg(0, NULL, NULL, NULL, str.c_str(), _renderWindow.get()->getWindowInfo()->handle);
	if (dlg.doModal () == IDOK)
	{
		openDocument (dlg.getSelectedFileName(0), dlg.getSelectedFileDesc());
	}
}

bool EditorImpl::exit (void)
{
	closeDocument ();

	if (!_currentEditingPlugin)
	{
		ATOM_APP->postQuitEvent (0);
		return true;
	}

	return false;
}

AS_CameraModal *EditorImpl::getCameraModal (void) const
{
	return _cameraModal;
}

void EditorImpl::setCameraModal (AS_CameraModal *cameraModal)
{
	if (cameraModal != _cameraModal)
	{
		if (_cameraOptionView)
		{
			ATOM_DELETE(_cameraOptionView);
			_cameraOptionView = 0;
		}

		_cameraModal = cameraModal ? cameraModal : _defaultCameraModal;
		_cameraModal->reset (getRealtimeCtrl ());
	}
}

void EditorImpl::frameUpdate (void)
{
#if TEST_TRAIL
	if (targetNode)
	{
		unsigned currentTick = ATOM_APP->getFrameStamp().currentTick;
		float rad = currentTick * ATOM_Pi * rotateSpeed / 360.f;
		float s, c;
		ATOM_sincos (rad, &s, &c);
		float x = 500.f * s;
		float y = 0;
		float z = 500.f * c;
		targetNode->setO2T (ATOM_Matrix4x4f::getTranslateMatrix (ATOM_Vector3f(x, y, z)));
	}
#endif

	// Update camera
	ATOM_Scene *scene = getRealtimeCtrl()->getScene();
	ATOM_Camera *camera = scene ? scene->getCamera() : NULL;
	if (camera)
	{
		const ATOM_Rect2Di &rcViewport = getRealtimeCtrl()->getViewport();
		const ATOM_Rect2Di &rcClient = getRealtimeCtrl()->getClientRect();
		int w = rcViewport.size.w ? rcViewport.size.w : rcClient.size.w;
		int h = rcViewport.size.h ? rcViewport.size.h : rcClient.size.h;
		camera->setPerspective (_cameraModal->getFovY(), float(w)/float(h), _cameraModal->getNearPlane(), _cameraModal->getFarPlane());
		camera->setViewMatrix (_cameraModal->getViewMatrix ());
	}

	// Update scene property editor
	if (_scenePropEditor && _scenePropEditor->isShown ())
	{
		_scenePropEditor->frameUpdate();
	}

	// Let plugin do it's own stuff
	if (_currentEditingPlugin)
	{
		_currentEditingPlugin->frameUpdate ();
	}

	// Update gizmos
	if (_isEditingNodeTranslation || _isEditingNodeScaling)
	{
		updateNodeAxis ();
	}
	else if (_isEditingNodeRotation)
	{
		updateNodeTrackball ();
	}

	updateAxises ();

	if (_cameraOptionView)
	{
		getCameraModal()->updateOptions (_cameraOptionView);
		_cameraFOV = getCameraModal()->getFovY();
		_cameraNear = getCameraModal()->getNearPlane();
		_cameraFar = getCameraModal()->getFarPlane();
		_cameraPosition = getCameraModal()->getPosition();
	}
}

void EditorImpl::handleEvent (ATOM_Event *event)
{
	if (_currentEditingPlugin)
	{
		int eventId = event->getEventTypeId ();

		if (eventId == ATOM_WidgetMouseMoveEvent::eventTypeId())
		{
			ATOM_WidgetMouseMoveEvent *e = (ATOM_WidgetMouseMoveEvent*)event;
			if (getCameraModal()->handleMouseMove (e->x, e->y))
			{
				return;
			}
			if (_isEditingNodeTranslation && isTranslating ())
			{
				ATOM_Ray ray;
				getRealtimeCtrl()->constructRay (e->x, e->y, ray);
				ATOM_Vector3f start = _transformBeforeEditInv.transformPoint (ray.getOrigin());
				ATOM_Vector3f end = _transformBeforeEditInv.transformPoint (ray.getOrigin() + ray.getDirection());
				ATOM_Vector3f plane = _axis->getRayProjectPlane (start, end - start, _axisType);
				ATOM_Vector3f projectPoint = _axis->getRayProjectPoint (start, end - start, _axisType, plane);
				projectPoint = _transformBeforeEdit.transformPoint (projectPoint);
				projectPoint -= _projectPoint;

				ATOM_Matrix4x4f matrix = _savedNodeWorldMatrix;
				matrix.m30 += projectPoint.x;
				matrix.m31 += projectPoint.y;
				matrix.m32 += projectPoint.z;

				if (_editingNode->getParent())
				{
					const ATOM_Matrix4x4f &projectionMatrix = getRealtimeCtrl()->getScene()->getCamera()->getProjectionMatrix();
					const ATOM_Matrix4x4f &viewMatrix = getRealtimeCtrl()->getScene()->getCamera()->getViewMatrix();
					ATOM_Matrix4x4f parentMatrix = _editingNode->getParent()->getWorldMatrix();
					parentMatrix.invertAffine ();
					matrix = parentMatrix >> matrix;
				}
				_editingNode->setO2T (matrix);
				_currentEditingPlugin->handleTransformEdited (_editingNode.get());
				return;
			}
			else if (_isEditingNodeScaling && isScaling ())
			{
				ATOM_Ray ray;
				getRealtimeCtrl()->constructRay (e->x, e->y, ray);
				ATOM_Vector3f start = _transformBeforeEditInv.transformPoint (ray.getOrigin());
				ATOM_Vector3f end = _transformBeforeEditInv.transformPoint (ray.getOrigin() + ray.getDirection());
				ATOM_Vector3f plane = _axis->getRayProjectPlane (start, end - start, _axisType);
				ATOM_Vector3f projectPoint = _axis->getRayProjectPoint (start, end - start, _axisType, plane);
				projectPoint = _transformBeforeEdit.transformPoint (projectPoint);

				ATOM_Vector3f oldOrigin (_transformBeforeEdit.m30, _transformBeforeEdit.m31, _transformBeforeEdit.m32);
				float length1 = (_projectPoint - oldOrigin).getLength ();
				float length2 = (projectPoint - oldOrigin).getLength ();
				if (ATOM_abs(length1 < 0.001f))
				{
					length1 = 0.001f;
				}
				float scale = length2 / length1;
				ATOM_Vector3f scaleVector;
				if (_isUniformScaling)
				{
					scaleVector.set(scale, scale, scale);
				}
				else
				{
					switch (_axisType)
					{
					case ATOMX_AXIS_X:
						scaleVector.set(scale, 1.f, 1.f);
						break;
					case ATOMX_AXIS_Y:
						scaleVector.set(1.f, scale, 1.f);
						break;
					case ATOMX_AXIS_Z:
						scaleVector.set(1.f, 1.f, scale);
						break;
					default:
						return;
					}
				}
				_editingNode->setO2T (_savedNodeO2T >> ATOM_Matrix4x4f::getScaleMatrix (scaleVector));
				_currentEditingPlugin->handleTransformEdited (_editingNode.get());
				return;
			}
			else if (_isEditingNodeRotation && isRotating ())
			{
				ATOM_Ray ray;
				getRealtimeCtrl()->constructRay (e->x, e->y, ray);
				if (_trackball->worldRayIntersectionTest (getRealtimeCtrl()->getScene()->getCamera(), ray))
				{
					ATOM_Vector3f trackballPointNearest = _trackball->getHitPointNearest ();
					ATOM_Vector3f trackballPointFar = _trackball->getHitPointFar ();
					float fn = (trackballPointNearest - _lastRotatePoint).getSquaredLength();
					float ff = (trackballPointFar - _lastRotatePoint).getSquaredLength();
					ATOM_Vector3f trackballPoint = fn < ff ? trackballPointNearest : trackballPointFar;
					//ATOM_Vector3f trackballPoint = _trackball->getHitPointNearest ();
					ATOM_Vector3f scale, center;
					ATOM_Matrix4x4f rotation = ATOM_Matrix4x4f::getIdentityMatrix();
					_transformBeforeEdit.decompose (center, rotation, scale);
					ATOM_Vector3f oldVector = _trackballPoint - center;
					float length1 = oldVector.getLength();
					oldVector.normalize ();
					ATOM_Vector3f newVector = trackballPoint - center;
					float length2 = newVector.getLength();
					newVector.normalize ();
					float length3 = _trackball->getScale().x;
					float angle = ATOM_acos (dotProduct(oldVector, newVector));
					if (angle < (1.f * ATOM_TwoPi / 360.f))
					{
						return;
					}
					ATOM_Vector3f axisRotate = crossProduct(oldVector, newVector);
					switch (_axisType)
					{
					case ATOMX_AXIS_X:
						{
							ATOM_Vector3f v(rotation.m00, rotation.m01, rotation.m02);
							float t = dotProduct (axisRotate, v);
							axisRotate = t > 0.f ? v : -v;
							break;
						}
					case ATOMX_AXIS_Y:
						{
							ATOM_Vector3f v(rotation.m10, rotation.m11, rotation.m12);
							float t = dotProduct (axisRotate, v);
							axisRotate = t > 0.f ? v : -v;
							break;
						}
					case ATOMX_AXIS_Z:
						{
							ATOM_Vector3f v(rotation.m20, rotation.m21, rotation.m22);
							float t = dotProduct (axisRotate, v);
							axisRotate = t > 0.f ? v : -v;
							break;
						}
					}
					ATOM_Matrix4x4f matrixRotate = ATOM_Matrix4x4f::getRotateMatrixAngleAxis (angle, axisRotate);

					_savedNodeWorldMatrix.decompose (center, rotation, scale);
					ATOM_Matrix4x4f newWorldMatrix = matrixRotate >> rotation;
					newWorldMatrix.m30 = center.x;
					newWorldMatrix.m31 = center.y;
					newWorldMatrix.m32 = center.z;
					newWorldMatrix >>= ATOM_Matrix4x4f::getScaleMatrix (scale);

					if (_editingNode->getParent())
					{
						ATOM_Scene *scene = getRealtimeCtrl()->getScene();
						if (scene)
						{
							ATOM_Matrix4x4f invParentWorld = _editingNode->getParent()->getInvWorldMatrix ();
							newWorldMatrix = invParentWorld >> newWorldMatrix;
						}
					}

					_editingNode->setO2T (newWorldMatrix);
					_currentEditingPlugin->handleTransformEdited (_editingNode.get());
					return;
				}
			}
		}
		else if (eventId == ATOM_WidgetLButtonDownEvent::eventTypeId())
		{
			ATOM_WidgetLButtonDownEvent *e = (ATOM_WidgetLButtonDownEvent*)event;
			setCapture ();
			if (getCameraModal()->handleMouseButtonDown (BUTTON_LEFT, e->x, e->y, e->shiftState))
			{
				return;
			}
			if (_isEditingNodeTranslation && !isTranslating ())
			{
				if (onTranslationEditLButtonDown (e->x, e->y))
				{
					return;
				}
			}
			if (_isEditingNodeScaling && !isScaling ())
			{
				bool uniformScale = (e->shiftState & KEYMOD_SHIFT) != 0 && (e->shiftState & KEYMOD_ALT) == 0 && (e->shiftState & KEYMOD_CTRL) == 0;
				if (onScaleEditLButtonDown (e->x, e->y, uniformScale))
				{
					return;
				}
			}
			if (_isEditingNodeRotation && !isRotating ())
			{
				if (onRotationEditLButtonDown (e->x, e->y))
				{
					return;
				}
			}
		}
		else if (eventId == ATOM_WidgetLButtonUpEvent::eventTypeId())
		{
			ATOM_WidgetLButtonUpEvent *e = (ATOM_WidgetLButtonUpEvent*)event;
			releaseCapture ();
			if (getCameraModal()->handleMouseButtonUp (BUTTON_LEFT, e->x, e->y, e->shiftState))
			{
				return;
			}
			if (_isEditingNodeTranslation && isTranslating ())
			{
				onTranslationEditLButtonUp ();
				return;
			}
			if (_isEditingNodeScaling && isScaling ())
			{
				onScaleEditLButtonUp ();
				return;
			}
			if (_isEditingNodeRotation && isRotating ())
			{
				onRotationEditLButtonUp ();
				return;
			}
		}
		else if (eventId == ATOM_WidgetMButtonDownEvent::eventTypeId())
		{
			ATOM_WidgetMButtonDownEvent *e = (ATOM_WidgetMButtonDownEvent*)event;
			setCapture ();
			if (getCameraModal()->handleMouseButtonDown (BUTTON_MIDDLE, e->x, e->y, e->shiftState))
			{
				return;
			}
		}
		else if (eventId == ATOM_WidgetMButtonUpEvent::eventTypeId())
		{
			ATOM_WidgetMButtonUpEvent *e = (ATOM_WidgetMButtonUpEvent*)event;
			releaseCapture ();
			if (getCameraModal()->handleMouseButtonUp (BUTTON_MIDDLE, e->x, e->y, e->shiftState))
			{
				return;
			}
		}
		else if (eventId == ATOM_WidgetRButtonDownEvent::eventTypeId())
		{
			ATOM_WidgetRButtonDownEvent *e = (ATOM_WidgetRButtonDownEvent*)event;
			setCapture ();
			if (getCameraModal()->handleMouseButtonDown (BUTTON_RIGHT, e->x, e->y, e->shiftState))
			{
				return;
			}
		}
		else if (eventId == ATOM_WidgetRButtonUpEvent::eventTypeId())
		{
			ATOM_WidgetRButtonUpEvent *e = (ATOM_WidgetRButtonUpEvent*)event;
			releaseCapture ();
			if (getCameraModal()->handleMouseButtonUp (BUTTON_RIGHT, e->x, e->y, e->shiftState))
			{
				return;
			}
		}
		else if (eventId == ATOM_WidgetMouseWheelEvent::eventTypeId())
		{
			ATOM_WidgetMouseWheelEvent *e = (ATOM_WidgetMouseWheelEvent*)event;
			if (getCameraModal()->handleMouseWheel (e->x, e->y, e->delta, e->shiftState))
			{
				return;
			}
		}
		else if (eventId == ATOM_WidgetKeyDownEvent::eventTypeId())
		{
			ATOM_WidgetKeyDownEvent *e = (ATOM_WidgetKeyDownEvent*)event;
			if (getCameraModal()->handleKeyDown (e->key, e->keymod))
			{
				return;
			}
		}
		else if (eventId == ATOM_WidgetKeyUpEvent::eventTypeId())
		{
			ATOM_WidgetKeyUpEvent *e = (ATOM_WidgetKeyUpEvent*)event;
			if (getCameraModal()->handleKeyUp (e->key, e->keymod))
			{
				return;
			}
		}
		else if (eventId == ATOMX_TWValueChangedEvent::eventTypeId())
		{
			ATOMX_TWValueChangedEvent *e = (ATOMX_TWValueChangedEvent*)event;
			if (_scenePropEditor && e->bar == _scenePropEditor->getBar() && _currentEditingPlugin)
			{
				_currentEditingPlugin->handleScenePropChanged ();
			}
			else if (e->bar == _cameraOptionView)
			{
				getCameraModal()->onParameterChanged (e);
				ATOM_Rect2Di rc = getRealtimeCtrl()->getViewport();
				const ATOM_Rect2Di &rcClient = getRealtimeCtrl()->getClientRect();

				switch (e->id)
				{
				case CO_MOVE_SPEED:
					{
						getCameraModal()->setMoveSpeed(e->newValue.getF());
						break;
					}
				case CO_ROTATE_SPEED:
					{
						getCameraModal()->setRotateSpeed(e->newValue.getF());
						break;
					}
				case CO_ZOOM_SPEED:
					{
						getCameraModal()->setZoomSpeed(e->newValue.getF());
						break;
					}
				case CO_ROTATE_RADIUS:
					{
						getCameraModal()->setRotateRadius(e->newValue.getF());
						break;
					}
				case CO_VIEWPORT_W:
					{
						rc.size.w = e->newValue.getI();
						if (rc.size.w > rcClient.size.w)
						{
							rc.size.w = rcClient.size.w;
							e->bar->setI ("W", rc.size.w);
						}
						int w = rc.size.w ? rc.size.w : rcClient.size.w;
						int h = rc.size.h ? rc.size.h : rcClient.size.h;
						rc.point.x = (rcClient.size.w - w) / 2;
						rc.point.y = (rcClient.size.h - h) / 2;
						getRealtimeCtrl()->setViewport (rc);
						break;
					}
				case CO_VIEWPORT_H:
					{
						rc.size.h = e->newValue.getI();
						if (rc.size.h > rcClient.size.h)
						{
							rc.size.h = rcClient.size.h;
							e->bar->setI ("H", rc.size.h);
						}
						int w = rc.size.w ? rc.size.w : rcClient.size.w;
						int h = rc.size.h ? rc.size.h : rcClient.size.h;
						rc.point.x = (rcClient.size.w - w) / 2;
						rc.point.y = (rcClient.size.h - h) / 2;
						getRealtimeCtrl()->setViewport (rc);
						break;
					}
					/*
				case CO_FOV:
					{
						this->getCameraModal()->setFovY (e->newValue.getF() * 3.1415926f / 180.f);
						break;
					}
				case CO_NEAR:
					{
						this->getCameraModal()->setNearPlane (e->newValue.getF());
						break;
					}
				case CO_FAR:
					{
						this->getCameraModal()->setFarPlane (e->newValue.getF());
						break;
					}
				case CO_MIN_CONTRIB:
					{
						ATOM_RenderSettings::setMinScreenContribution (e->newValue.getF());
						break;
					}
				case CO_POSITION:
					{
						const float *v = e->newValue.get3F();
						this->getCameraModal()->setPosition (ATOM_Vector3f(v[0], v[1], v[2]));
						break;
					}
					*/
				}
				return;
			}
		}

		_currentEditingPlugin->handleEvent (event);
	}
}

void EditorImpl::setCapture (void)
{
	++_captureCounter;

	if (_captureCounter == 1)
	{
		_realtimeCtrl->setCapture ();
	}
}

void EditorImpl::releaseCapture (void)
{
	if (_captureCounter)
	{
		--_captureCounter;

		if (_captureCounter == 0)
		{
			_realtimeCtrl->releaseCapture ();
		}
	}
}

AS_Asset *EditorImpl::createAsset (const char *name)
{
	if (name)
	{
		char buffer[256];
		strcpy (buffer, name);
		strlwr (buffer);

		ATOM_HASHMAP<ATOM_STRING, AS_AssetManager*>::iterator it = _assetManagerMap.find (buffer);
		if (it != _assetManagerMap.end ())
		{
			return it->second->createAsset ();
		}
	}

	return 0;
}

AS_Asset *EditorImpl::createAssetByFileName (const char *filename)
{
	if (!filename)
	{
		return 0;
	}

	const char *p = strrchr (filename, '.');
	if (!p)
	{
		return 0;
	}

	for (ATOM_HASHMAP<ATOM_STRING, AS_AssetManager*>::iterator it = _assetManagerMap.begin(); it != _assetManagerMap.end(); ++it)
	{
		for (unsigned i = 0; i < it->second->getNumFileExtensions(); ++i)
		{
			const char *supportExt = it->second->getFileExtension (i);
			if (!supportExt)
			{
				continue;
			}

			if (!stricmp (supportExt, p+1))
			{
				AS_Asset *asset = it->second->createAsset ();
				if (asset && asset->loadFromFile (filename))
				{
					return asset;
				}
			}
		}
	}

	return 0;
}

ATOM_STRING EditorImpl::buildAssetFileFilterString (void) const
{
	ATOM_STRING str;

	for (ATOM_HASHMAP<ATOM_STRING, AS_AssetManager*>::const_iterator it = _assetManagerMap.begin(); it != _assetManagerMap.end(); ++it)
	{
		for (unsigned i = 0; i < it->second->getNumFileExtensions(); ++i)
		{
			const char *desc = it->second->getFileDesc (i);
			const char *ext = it->second->getFileExtension (i);
			if (!ext)
			{
				continue;
			}

			if (desc)
			{
				str += desc;
			}
			else
			{
				str += ext;
				str += "文件";
			}
			str += "(*.";
			str += ext;
			str += ")|*.";
			str += ext;
			str += "|";
		}
	}

	return str;
}


void EditorImpl::showAssetEditor (bool show)
{
	if (show)
	{
		if (!_assetEditor)
		{
			_assetEditor = ATOM_NEW(AssetEditor, this);
		}
		_assetEditor->show (true);
	}
	else if (_assetEditor)
	{
		_assetEditor->show (false);
	}
}

void EditorImpl::showColorGradingEditor (bool show, ATOM_ColorGradingEffect *effect)
{
	if (show)
	{
		if (!_colorGradingEditor)
		{
			_colorGradingEditor = ATOM_NEW(ColorGradingEditor, this, effect);
		}
		_colorGradingEditor->showEditor ();
	}
	else if (_colorGradingEditor)
	{
		_colorGradingEditor->hideEditor ();
	}
}

void EditorImpl::setAssetEditorPosition (int x, int y, int w, int h)
{
	if (!_assetEditor)
	{
		_assetEditor = ATOM_NEW(AssetEditor, this);
	}
	_assetEditor->setPosition (x, y, w, h);
}

void EditorImpl::showScenePropEditor (bool show)
{
	if (show)
	{
		if (!_scenePropEditor)
		{
			_scenePropEditor = ATOM_NEW(ScenePropEditor, this, nullptr);
		}
		_scenePropEditor->show (true);
	}
	else if (_scenePropEditor)
	{
		_scenePropEditor->show (false);
	}
}

void EditorImpl::setScenePropEditorPosition (int x, int y, int w, int h)
{
	if (!_scenePropEditor)
	{
		_scenePropEditor = ATOM_NEW(ScenePropEditor, this, nullptr);
	}
	_scenePropEditor->setPosition (x, y, w, h);
}

void EditorImpl::setScenePropEditorTarget (ATOM_DeferredScene *scene)
{
	if (!_scenePropEditor)
	{
		_scenePropEditor = ATOM_NEW(ScenePropEditor, this, scene);
	}
	else
	{
		_scenePropEditor->setScene (scene);
	}
}

void EditorImpl::resetScenePropEditor (void)
{
	if (_scenePropEditor)
	{
		_scenePropEditor->setParamChanged (false);
	}
}

void EditorImpl::refreshScenePropEditor (void)
{
	if (_scenePropEditor)
	{
		_scenePropEditor->refresh ();
	}
}

unsigned EditorImpl::getOpenAssetFileNames (bool multi, bool save, const char *title)
{
	_openedAssetFileNames.clear ();

	ATOM_STRING str = buildAssetFileFilterString ();
	if (str.empty ())
	{
		return 0;
	}

	int mode;
	if (save)
	{
		mode = ATOM_FileDlg::MODE_SAVEFILE|ATOM_FileDlg::MODE_OVERWRITEPROMPT;
	}
	else
	{
		mode = multi ? ATOM_FileDlg::MODE_MULTISELECT : 0;
	}

	ATOM_FileDlg dlg(mode, 0, 0, 0, str.c_str(), _renderWindow->getWindowInfo()->handle);
	if (title)
	{
		dlg.setTitle (title);
	}

	if (dlg.doModal () == IDOK)
	{
		for (unsigned i = 0; i < dlg.getNumFilesSelected(); ++i)
		{
			_openedAssetFileNames.push_back (dlg.getSelectedFileName(i));
		}
		return _openedAssetFileNames.size();
	}
	return 0;
}

const char *EditorImpl::getOpenedAssetFileName (unsigned index)
{
	return _openedAssetFileNames[index].c_str();
}

unsigned EditorImpl::getOpenImageFileNames (bool multi, bool save, const char *title)
{
	_openedImageFileNames.clear ();

	const char *str = "DDS文件|*.dds|JPG文件|*.jpg|PNG文件|*.png|TGA文件|*.tga|所有文件|*.*|";

	int mode;

	if (save)
	{
		mode = ATOM_FileDlg::MODE_SAVEFILE|ATOM_FileDlg::MODE_OVERWRITEPROMPT;
	}
	else
	{
		mode = multi ? ATOM_FileDlg::MODE_MULTISELECT : 0;
	}

	ATOM_FileDlg dlg(mode, 0, "dds", 0, str, _renderWindow->getWindowInfo()->handle);
	if (title)
	{
		dlg.setTitle (title);
	}

	if (dlg.doModal () == IDOK)
	{
		for (unsigned i = 0; i < dlg.getNumFilesSelected(); ++i)
		{
			_openedImageFileNames.push_back (dlg.getSelectedFileName(i));
		}
		return _openedImageFileNames.size();
	}
	return 0;
}

const char *EditorImpl::getOpenedImageFileName (unsigned index)
{
	return _openedImageFileNames[index].c_str();
}

unsigned EditorImpl::getOpenFileNames (const char *ext, const char *filter, bool multi, bool save, const char *title)
{
	_openedFileNames.clear ();

	int mode;
	if (save)
	{
		mode = ATOM_FileDlg::MODE_SAVEFILE|ATOM_FileDlg::MODE_OVERWRITEPROMPT;
	}
	else
	{
		mode = multi ? ATOM_FileDlg::MODE_MULTISELECT : 0;
	}

	ATOM_FileDlg dlg(mode, 0, ext, 0, filter, _renderWindow->getWindowInfo()->handle);
	if (title)
	{
		dlg.setTitle (title);
	}

	if (dlg.doModal () == IDOK)
	{
		for (unsigned i = 0; i < dlg.getNumFilesSelected(); ++i)
		{
			_openedFileNames.push_back (dlg.getSelectedFileName(i));
		}
		return _openedFileNames.size();
	}
	return 0;
}

const char *EditorImpl::getOpenedFileName (unsigned index)
{
	return _openedFileNames[index].c_str();
}

ATOM_Point2Di EditorImpl::getMousePosition (void) const
{
	return ATOM_Point2Di(_currentMouseX, _currentMouseY);
}

void EditorImpl::setMousePosition (int x, int y)
{
	ATOM_Point2Di pt(x, y);
	getRealtimeCtrl()->viewportToClient (&pt);
	_currentMouseX = pt.x;
	_currentMouseY = pt.y;
}

void EditorImpl::updateStudioCaption (bool modified)
{
	char title[1024];

	if (!_currentEditingPlugin)
	{
		sprintf (title, "ATOM3D Studio v%d.%d.%d", ATOM3D_VERSION_GET_MAJOR(AS_VERSION), ATOM3D_VERSION_GET_MINOR(AS_VERSION), ATOM3D_VERSION_GET_PATCH(AS_VERSION));
	}
	else
	{
		const char *filename = _currentFile.empty () ? "未命名" : _currentFile.c_str();
		sprintf (title, "ATOM3D Studio v%d.%d.%d - [%s%s]", ATOM3D_VERSION_GET_MAJOR(AS_VERSION), ATOM3D_VERSION_GET_MINOR(AS_VERSION), ATOM3D_VERSION_GET_PATCH(AS_VERSION), filename, modified ? "*" : "");
	}

	::SetWindowTextA (_renderWindow->getWindowInfo()->handle, title);
}

const char *EditorImpl::getDocumentFileName (void) const
{
	return _currentFile.c_str();
}

void EditorImpl::setDocumentModified (bool modified)
{
	_documentModified = modified;
	updateStudioCaption (modified);
}

bool EditorImpl::isDocumentModified (void) const
{
	return _documentModified;
}

bool EditorImpl::needPromptForSave (void) const
{
	return _currentEditingPlugin ? _currentEditingPlugin->isDocumentModified () : false;
}

void EditorImpl::doEditOp (AS_Operation *op)
{
	_editOps.resize (_currentOp);
	_editOps.push_back (op);
	_currentOp = _editOps.size ();

	op->redo ();
}

void EditorImpl::doObjectAttribModifyOp (ATOM_Object *object, const char *propName, const ATOM_ScriptVar &oldValue, const ATOM_ScriptVar &newValue)
{
	AS_Operation *op = ATOM_NEW(ObjectAttribModifyOp, object, propName, oldValue, newValue);

	_editOps.resize (_currentOp);
	_editOps.push_back (op);
	_currentOp = _editOps.size ();

	op->redo ();
}

void EditorImpl::doNodeTransformOp (ATOM_Node *node, const ATOM_Matrix4x4f &oldMatrix, const ATOM_Matrix4x4f &newMatrix)
{
	AS_Operation *op = ATOM_NEW(NodeTransformOp, node, oldMatrix, newMatrix);

	_editOps.resize (_currentOp);
	_editOps.push_back (op);
	_currentOp = _editOps.size ();

	op->redo ();
}

void EditorImpl::doNodeCreateOp (ATOM_Node *nodeChild, ATOM_Node *nodeParent)
{
	AS_Operation *op = ATOM_NEW(NodeCreateOp, nodeChild, nodeParent);

	_editOps.resize (_currentOp);
	_editOps.push_back (op);
	_currentOp = _editOps.size ();

	op->redo ();
}

void EditorImpl::doNodeDeleteOp (ATOM_Node *nodeChild, ATOM_Node *nodeParent)
{
	AS_Operation *op = ATOM_NEW(NodeDeleteOp, nodeChild, nodeParent);

	_editOps.resize (_currentOp);
	_editOps.push_back (op);
	_currentOp = _editOps.size ();

	op->redo ();
}

bool EditorImpl::canUndo (void) const
{
	return _currentOp > 0 && _currentOp <= _editOps.size();
}

bool EditorImpl::undo (void)
{
	if (canUndo ())
	{
		_currentOp--;

		_editOps[_currentOp]->undo ();

		return true;
	}

	return false;
}

bool EditorImpl::canRedo (void) const
{
	return _currentOp < _editOps.size();
}

bool EditorImpl::redo (void)
{
	if (canRedo ())
	{
		_editOps[_currentOp]->redo ();

		_currentOp++;

		return true;
	}

	return false;
}

struct AccelKeyInfo
{
	ATOM_Key key;
	bool ctrl;
	bool shift;
	bool alt;
	int commandId;
};

AS_AccelKey EditorImpl::registerAccelKeyCommand (ATOM_Key key, int keymod, int commandId)
{
	bool ctrl = (keymod & KEYMOD_CTRL) != 0;
	bool shift = (keymod & KEYMOD_SHIFT) != 0;
	bool alt = (keymod & KEYMOD_ALT) != 0;

	for (unsigned i = 0; i < _accelKeys.size(); ++i)
	{
		const AccelKeyInfo *info = _accelKeys[i];
		if (info->key == key && info->ctrl == ctrl && info->shift == shift && info->alt == alt)
		{
			return 0;
		}
	}
	_accelKeys.push_back (ATOM_NEW(AccelKeyInfo));
	_accelKeys.back()->key = key;
	_accelKeys.back()->ctrl = ctrl;
	_accelKeys.back()->shift = shift;
	_accelKeys.back()->alt = alt;
	_accelKeys.back()->commandId = commandId;

	return _accelKeys.back ();
}

void EditorImpl::unregisterAccelKeyCommand (AS_AccelKey accelKey)
{
	for (unsigned i = 0; i < _accelKeys.size(); ++i)
	{
		if (_accelKeys[i] == accelKey)
		{
			ATOM_DELETE(_accelKeys[i]);
			_accelKeys.erase (_accelKeys.begin() + i);
			return;
		}
	}
}

bool EditorImpl::checkAndFireAccelKeyCommand (ATOM_Key key, int keymod)
{
	// 有模态窗口则不触发快捷键命令
	if (_guiRenderer->getCurrentModalDialog ())
	{
		return false;
	}

	bool ctrl = (keymod & KEYMOD_CTRL) != 0;
	bool shift = (keymod & KEYMOD_SHIFT) != 0;
	bool alt = (keymod & KEYMOD_ALT) != 0;

	for (unsigned i = 0; i < _accelKeys.size(); ++i)
	{
		const AccelKeyInfo *info = _accelKeys[i];

		if (info->key == key && info->ctrl == ctrl && info->shift == shift && info->alt == alt)
		{
			int commandId = info->commandId;

			getRealtimeCtrl()->queueEvent (ATOM_NEW(ATOM_WidgetCommandEvent, commandId), ATOM_APP);

			return true;
		}
	}
	return false;
}

const ATOM_VECTOR<EditorImpl::PluginInfo> &EditorImpl::getPluginList (void) const
{
	return _pluginInfos;
}

const ATOM_HASHMAP<ATOM_STRING, AS_AssetManager*> &EditorImpl::getAssetManagerMap (void) const
{
	return _assetManagerMap;
}

void EditorImpl::doExit (void)
{
	closeDocument ();

	if (!_currentEditingPlugin)
	{
		ATOM_APP->postQuitEvent (0);
	}
}

void EditorImpl::markDummyNodeType (const char *classname)
{
	if (classname)
	{
		_dummyNodeTypes.insert (classname);
	}
}

bool EditorImpl::isDummyNodeType (const char *classname)
{
	if (classname)
	{
		return _dummyNodeTypes.find (classname) != _dummyNodeTypes.end ();
	}
	return false;
}

unsigned EditorImpl::getMinWindowWidth (void) const
{
	if (_currentEditingPlugin)
	{
		return _currentEditingPlugin->getMinWindowWidth ();
	}
	else
	{
		return 400;
	}
}

unsigned EditorImpl::getMinWindowHeight (void) const
{
	if (_currentEditingPlugin)
	{
		return _currentEditingPlugin->getMinWindowHeight ();
	}
	else
	{
		return 300;
	}
}

void EditorImpl::checkForWindowSize (AS_Plugin *plugin)
{
	RECT rc;
	::GetClientRect (_renderWindow->getWindowInfo()->handle, &rc);
	if (rc.bottom == rc.top)
	{
		return;
	}

	unsigned width = rc.right - rc.left;
	unsigned height = rc.bottom - rc.top;
	unsigned newWidth = ATOM_max2 (width, plugin->getMinWindowWidth ());
	unsigned newHeight = ATOM_max2 (height, plugin->getMinWindowHeight ());
	if (newWidth != width || newHeight != height)
	{
		POINT pt;
		pt.x = 0;
		pt.y = 0;
		::ClientToScreen (_renderWindow->getWindowInfo()->handle, &pt);
		::SetWindowPos (_renderWindow->getWindowInfo()->handle, HWND_NOTOPMOST, pt.x, pt.y, newWidth, newHeight, SWP_NOMOVE);
	}
}

void EditorImpl::createAxisNode (void)
{
	_axis = ATOM_HARDREF(ATOMX_AxisNode)();
	_axis->setPickable (0);
	_axis->setSize (1.f, 100.f);
	_axis->load (ATOM_GetRenderDevice());
}

void EditorImpl::createTrackBallNode (void)
{
	_trackball = ATOM_HARDREF(ATOMX_TrackBallNode)();
	_trackball->setPickable (0);
	_trackball->setRadius (1.f);
	_trackball->load (ATOM_GetRenderDevice());
}

bool EditorImpl::beginEditNodeTranslation (ATOM_Node *node)
{
	if (!node)
	{
		return false;
	}

	if (node == _editingNode.get() && isEditingNodeTranslation ())
	{
		return true;
	}

	endEditNodeTransform ();

	ATOM_Scene *scene = getRealtimeCtrl()->getScene();
	if (!scene)
	{
		return false;
	}

	endEditNodeTransform ();

	if (!_axis)
	{
		createAxisNode ();
	}

	scene->getRootNode()->appendChild (_axis.get ());
	_editingNode = node;
	_isEditingNodeTranslation = true;
	_axisType = ATOMX_AXIS_NONE;

	return true;
}

bool EditorImpl::beginEditNodeRotation (ATOM_Node *node)
{
	if (!node)
	{
		return false;
	}

	if (node == _editingNode.get() && isEditingNodeRotation ())
	{
		return true;
	}

	endEditNodeTransform ();

	ATOM_Scene *scene = getRealtimeCtrl()->getScene ();
	if (!scene)
	{
		return false;
	}

	endEditNodeTransform ();

	if (!_trackball)
	{
		createTrackBallNode ();
	}

	scene->getRootNode()->appendChild (_trackball.get());
	_editingNode = node;
	_isEditingNodeRotation = true;
	_axisType = ATOMX_AXIS_NONE;

	return true;
}

bool EditorImpl::beginEditNodeScaling (ATOM_Node *node, bool forceUniformScale)
{
	if (!node)
	{
		return false;
	}

	ATOM_Scene *scene = getRealtimeCtrl()->getScene();
	if (!scene)
	{
		return false;
	}

	_forceUniformScale = forceUniformScale;

	if (node == _editingNode.get() && isEditingNodeScaling ())
	{
		return true;
	}

	endEditNodeTransform ();

	if (!_axis)
	{
		createAxisNode ();
	}

	scene->getRootNode()->appendChild (_axis.get ());
	_editingNode = node;
	_isEditingNodeScaling = true;
	_axisType = ATOMX_AXIS_NONE;

	return true;
}

void EditorImpl::endEditNodeTransform (void)
{
	if (_axis)
	{
		ATOM_Node *parent = _axis->getParent ();
		if (parent)
		{
			parent->removeChild (_axis.get());
		}
	}
	if (_trackball)
	{
		ATOM_Node *parent = _trackball->getParent ();
		if (parent)
		{
			parent->removeChild (_trackball.get());
		}
	}
	_isEditingNodeTranslation = false;
	_isEditingNodeRotation = false;
	_isEditingNodeScaling = false;
	_isUniformScaling = false;
	_axisType = ATOMX_AXIS_NONE;
	_editingNode = 0;
}

void EditorImpl::updateNodeTrackball (void)
{
	if (_editingNode)
	{
		ATOM_Scene *scene = getRealtimeCtrl()->getScene ();
		ATOM_ASSERT(scene);

		ATOM_Matrix4x4f matWorld = _editingNode->getWorldMatrix ();
		ATOM_Matrix4x4f mRotate = ATOM_Matrix4x4f::getIdentityMatrix();
		ATOM_Vector3f vTranslate;
		ATOM_Vector3f vScale;
		matWorld.decompose (vTranslate, mRotate, vScale);
#if 1
		float maxScale = 500.f;
#else
		float maxScale = ATOM_max3(ATOM_abs(vScale.x), ATOM_abs(vScale.y), ATOM_abs(vScale.z));
		const ATOM_BBox &bbox = _editingNode->getBoundingbox ();
		ATOM_Vector3f bboxSize = bbox.getMax() - bbox.getMin ();
		maxScale *= ATOM_max3(bboxSize.x, bboxSize.y, bboxSize.z);
#endif
		float screenDistance = measureScreenDistance (vTranslate, maxScale);
		float maxScreenDistance = getRealtimeCtrl()->getClientRect().size.h * 0.3f;
		if (screenDistance > maxScreenDistance)
		{
			maxScale *= (maxScreenDistance / screenDistance);
		}
		_trackball->setO2T (matWorld);
		_trackball->setScale (ATOM_Vector3f(maxScale, maxScale, maxScale));
	}
}

void EditorImpl::updateNodeAxis (void)
{
	if (_editingNode)
	{
		ATOM_Scene *scene = getRealtimeCtrl()->getScene ();
		ATOM_ASSERT(scene);

		ATOM_Matrix4x4f matWorld = _editingNode->getWorldMatrix ();
		ATOM_Matrix4x4f mRotate = ATOM_Matrix4x4f::getIdentityMatrix();
		ATOM_Vector3f vTranslate;
		ATOM_Vector3f vScale;
		matWorld.decompose (vTranslate, mRotate, vScale);

		float maxScale = ATOM_max3(ATOM_abs(vScale.x), ATOM_abs(vScale.y), ATOM_abs(vScale.z));
		const ATOM_BBox &bbox = _editingNode->getBoundingbox ();
		ATOM_Vector3f bboxSize = bbox.getMax() - bbox.getMin ();
		maxScale *= ATOM_max3(bboxSize.x, bboxSize.y, bboxSize.z);

		float scale = _axis->getScale ().x;
		if (scale < maxScale)
		{
			scale = maxScale;
		}
		if (scale < 100.f)
		{
			scale = 100.f;
		}
		scale *= 0.01f;

		if (isEditingNodeTranslation ())
		{
			_axis->setO2T (ATOM_Matrix4x4f::getTranslateMatrix (vTranslate));
		}
		else
		{
			_axis->setO2T (matWorld);
		}
		scale = 1.f;
		_axis->setScale (ATOM_Vector3f(scale, scale, scale));
	}
}

bool EditorImpl::onTranslationEditLButtonDown (int x, int y)
{
	ATOM_Ray ray;
	getRealtimeCtrl()->constructRay (x, y, ray);
	const ATOM_Matrix4x4f &projectionMatrix = getRealtimeCtrl()->getScene()->getCamera()->getProjectionMatrix ();
	const ATOM_Matrix4x4f &viewMatrix = getRealtimeCtrl()->getScene()->getCamera()->getViewMatrix ();
	const ATOM_Matrix4x4f &invWorldMatrix= _axis->getInvWorldMatrix ();
	ATOM_Vector3f origin = ray.getOrigin ();
	ATOM_Vector3f end = ray.getOrigin () + ray.getDirection ();
	ATOM_Vector3f originObj = invWorldMatrix.transformPoint (origin);
	ATOM_Vector3f endObj = invWorldMatrix.transformPoint (end);
	ray.set(originObj, endObj - originObj);
	float d;
	if (_axis->rayIntersectionTest(getRealtimeCtrl()->getScene()->getCamera(), ray, &d))
	{
		_axisType = _axis->getHitAxis ();
		_transformBeforeEdit = _axis->getWorldMatrix ();
		_transformBeforeEditInv = invWorldMatrix;
		_projectPlane = _axis->getRayProjectPlane (originObj, endObj - originObj, _axisType);
		_projectPoint = _axis->getRayProjectPoint (originObj, endObj - originObj, _axisType, _projectPlane);
		_projectPoint = _transformBeforeEdit.transformPoint (_projectPoint);
		_savedNodeO2T = _editingNode->getO2T ();
		_savedNodeWorldMatrix = _editingNode->getWorldMatrix ();
		return true;
	}
	return false;
}

void EditorImpl::onTranslationEditLButtonUp (void)
{
	_axisType = ATOMX_AXIS_NONE;

	doNodeTransformOp (_editingNode.get(), _savedNodeO2T, _editingNode->getO2T()); 
}

bool EditorImpl::isTranslating (void) const
{
	return _axisType != ATOMX_AXIS_NONE;
}

bool EditorImpl::onScaleEditLButtonDown (int x, int y, bool uniformScale)
{
	ATOM_Ray ray;
	getRealtimeCtrl()->constructRay (x, y, ray);
	const ATOM_Matrix4x4f &projectionMatrix = getRealtimeCtrl()->getScene()->getCamera()->getProjectionMatrix ();
	const ATOM_Matrix4x4f &viewMatrix = getRealtimeCtrl()->getScene()->getCamera()->getViewMatrix ();
	const ATOM_Matrix4x4f &invWorldMatrix= _axis->getInvWorldMatrix ();
	ATOM_Vector3f origin = ray.getOrigin ();
	ATOM_Vector3f end = ray.getOrigin () + ray.getDirection ();
	ATOM_Vector3f originObj = invWorldMatrix.transformPoint (origin);
	ATOM_Vector3f endObj = invWorldMatrix.transformPoint (end);
	ray.set(originObj, endObj - originObj);
	float d;
	if (_axis->rayIntersectionTest(getRealtimeCtrl()->getScene()->getCamera(), ray, &d))
	{
		_axisType = _axis->getHitAxis ();
		_transformBeforeEdit = _axis->getWorldMatrix ();
		_transformBeforeEditInv = invWorldMatrix;
		_projectPlane = _axis->getRayProjectPlane (originObj, endObj - originObj, _axisType);
		_projectPoint = _axis->getRayProjectPoint (originObj, endObj - originObj, _axisType, _projectPlane);
		_projectPoint = _transformBeforeEdit.transformPoint (_projectPoint);
		_savedNodeO2T = _editingNode->getO2T ();
		_savedNodeWorldMatrix = _editingNode->getWorldMatrix ();
		_isUniformScaling = uniformScale || _forceUniformScale;
		return true;
	}
	return false;
}

void EditorImpl::onScaleEditLButtonUp (void)
{
	_axisType = ATOMX_AXIS_NONE;

	doNodeTransformOp (_editingNode.get(), _savedNodeO2T, _editingNode->getO2T()); 
}

bool EditorImpl::isScaling (void) const
{
	return _axisType != ATOMX_AXIS_NONE;
}

bool EditorImpl::onRotationEditLButtonDown (int x, int y)
{
	ATOM_Ray ray;
	getRealtimeCtrl()->constructRay (x, y, ray);
	if (_trackball->worldRayIntersectionTest (getRealtimeCtrl()->getScene()->getCamera(), ray))
	{
		_axisType = _trackball->getHitAxis ();
		_trackballPoint = _trackball->getHitPoint ();
		_lastRotatePoint = _trackballPoint;
		_transformBeforeEdit = _trackball->getWorldMatrix ();
		_transformBeforeEditInv.invertAffineFrom (_transformBeforeEdit);
		_savedNodeO2T = _editingNode->getO2T ();
		_savedNodeWorldMatrix = _editingNode->getWorldMatrix ();
		return true;
	}
	return false;
}

void EditorImpl::onRotationEditLButtonUp (void)
{
	_axisType = ATOMX_AXIS_NONE;

	doNodeTransformOp (_editingNode.get(), _savedNodeO2T, _editingNode->getO2T()); 
}

bool EditorImpl::isRotating (void) const
{
	return _axisType != ATOMX_AXIS_NONE;
}

float EditorImpl::measureScreenDistance (const ATOM_Vector3f &locationWorld, float distanceWorld) const
{
	const ATOM_Vector3f &cameraPos = getCameraModal()->getPosition ();
	float length = (cameraPos - locationWorld).getLength ();

	// adjust width for easy pick
	float fovY = getCameraModal()->getFovY ();
	float h = length * ATOM_tan(fovY * 0.5f);
	float screenH = getRealtimeCtrl()->getClientRect().size.h / 2;
	return distanceWorld * screenH / h;
}

bool EditorImpl::isEditingNodeTranslation (void) const
{
	return _editingNode && _isEditingNodeTranslation;
}

bool EditorImpl::isEditingNodeRotation (void) const
{
	return _editingNode && _isEditingNodeRotation;
}

bool EditorImpl::isEditingNodeScaling (void) const
{
	return _editingNode && _isEditingNodeScaling;
}

bool EditorImpl::isEditingNodeTransform (void) const
{
	return isEditingNodeTranslation() || isEditingNodeRotation() || isEditingNodeScaling ();
}

void EditorImpl::showNodeAxis (ATOM_Node *node, bool show, float minAxisLength)
{
	if (!node)
	{
		return;
	}

	if (!getRealtimeCtrl()->getScene())
	{
		return;
	}

	for (unsigned i = 0; i < _nodesWithAxis.size(); ++i)
	{
		if (_nodesWithAxis[i].node.get() == node)
		{
			if (!show)
			{
				getRealtimeCtrl()->getScene()->getRootNode()->removeChild (_nodesWithAxis[i].axis.get());
				_nodesWithAxis.erase (_nodesWithAxis.begin() + i);
			}
			return;
		}
	}

	if (show)
	{
		_nodesWithAxis.resize (_nodesWithAxis.size() + 1);
		_nodesWithAxis.back().node = node;
		_nodesWithAxis.back().axis = ATOM_HARDREF(ATOMX_AxisNode)();
		_nodesWithAxis.back().axis->setPickable (0);
		_nodesWithAxis.back().axis->setSize (0.01f, 1.f);
		_nodesWithAxis.back().axis->load (ATOM_GetRenderDevice());
		_nodesWithAxis.back().minLength  = minAxisLength;

		getRealtimeCtrl()->getScene()->getRootNode()->appendChild (_nodesWithAxis.back().axis.get());
	}
}

void EditorImpl::updateAxises (void)
{
	const ATOM_Matrix4x4f &projMatrix = getCameraModal()->getProjectionMatrix ();
	const ATOM_Matrix4x4f &viewMatrix = getCameraModal()->getViewMatrix ();

	for (unsigned i = 0; i < _nodesWithAxis.size(); ++i)
	{
		const AxisNodeInfo & info = _nodesWithAxis[i];
		if (info.node == _editingNode)
		{
			info.axis->setShow (0);
		}
		else
		{
			info.axis->setShow (1);

			ATOM_BBox bbox = info.node->getWorldBoundingbox ();
			ATOM_Vector3f ext = bbox.getMax() - bbox.getMin ();
			float length = ATOM_max3(ext.x, ext.y, ext.z);
			if (length < info.minLength)
			{
				length = info.minLength;
			}

			const ATOM_Matrix4x4f worldMatrix = info.node->getWorldMatrix ();
			ATOM_Vector3f t, s;
			ATOM_Matrix4x4f r = ATOM_Matrix4x4f::getIdentityMatrix ();
			worldMatrix.decompose (t, r, s);
			s.set (length, length, length);
			info.axis->setO2T (ATOM_Matrix4x4f::getTranslateMatrix(t) >> r >> ATOM_Matrix4x4f::getScaleMatrix(s));
		}
	}
}

void EditorImpl::render (void)
{
	if (_currentRenderManagerName)
	{
		ATOM_HASHMAP<ATOM_STRING, AS_RenderManager*>::const_iterator it = _renderManagerMap.find (_currentRenderManagerName);
		if (it != _renderManagerMap.end ())
		{
			ATOM_Scene *scene = getRealtimeCtrl()->getScene();
			if (scene)
			{
				it->second->renderScene (this, scene);
				return;
			}
		}
	}

	renderScene ();
}

void EditorImpl::renderScene (void)
{
	getRealtimeCtrl()->render (ATOM_GetRenderDevice());
}

bool EditorImpl::setRenderManager (const char *name)
{
	if (name)
	{
		char buffer[256];
		strcpy (buffer, name);
		strlwr (buffer);

		ATOM_HASHMAP<ATOM_STRING, AS_RenderManager*>::const_iterator it = _renderManagerMap.find (buffer);
		if (it != _renderManagerMap.end ())
		{
			free ((void*)_currentRenderManagerName);
			_currentRenderManagerName = strdup (buffer);
			return true;
		}
	}
	return false;
}

const char *EditorImpl::getRenderManager (void)
{
	return _currentRenderManagerName;
}

void EditorImpl::setSchemeMode (SchemeMode mode)
{
	if (mode != _schemeMode)
	{
		_schemeMode = mode;

		//-----------------------------------------------//
		// wangjian added
		ATOM_RenderSettings::setNonDeferredShading( _schemeMode == SCHEME_FORWARD ?  true : false );

		// wangjian added
		/*if( ATOM_RenderSettings::isNonDeferredShading() )
		{
		_schemeMode = SCHEME_FORWARD;
		}*/
			
		//-----------------------------------------------//

		if (_currentEditingPlugin)
		{
			_currentEditingPlugin->changeRenderScheme ();
		}
	}
}

void EditorImpl::setRenderMode (RenderMode mode)
{
	switch (mode)
	{
	case RM_DEFERRED:
		ATOM_RenderSettings::enableDebugDeferredNormals (false);
		ATOM_RenderSettings::enableDebugDeferredColors (false);
		ATOM_RenderSettings::enableDebugDeferredDepth (false);
		ATOM_RenderSettings::enableDebugDeferredLightBuffer (false);
		ATOM_RenderSettings::enableDebugShadowMap (false);
		ATOM_RenderSettings::enableDebugShadowMask (false);
		//--- wangjian added ---//
		// * 用以调试GBUFFER RT中保存的HalfLambert.
		ATOM_RenderSettings::enableDebugHalfLambertBuffer (false);
		ATOM_RenderSettings::enableDebugBloomBuffer (false);
		ATOM_RenderSettings::enableDebugHdrLumin (false);
		//----------------------//
		break;
	case RM_DEFERRED_NORMAL:
		ATOM_RenderSettings::enableDebugDeferredNormals (true);
		ATOM_RenderSettings::enableDebugDeferredColors (false);
		ATOM_RenderSettings::enableDebugDeferredDepth (false);
		ATOM_RenderSettings::enableDebugDeferredLightBuffer (false);
		ATOM_RenderSettings::enableDebugShadowMap (false);
		ATOM_RenderSettings::enableDebugShadowMask (false);
		//--- wangjian added ---//
		// * 用以调试GBUFFER RT中保存的HalfLambert.
		ATOM_RenderSettings::enableDebugHalfLambertBuffer (false);
		ATOM_RenderSettings::enableDebugBloomBuffer (false);
		ATOM_RenderSettings::enableDebugHdrLumin (false);
		//----------------------//
		break;
	case RM_DEFERRED_COLOR:
		ATOM_RenderSettings::enableDebugDeferredNormals (false);
		ATOM_RenderSettings::enableDebugDeferredColors (true);
		ATOM_RenderSettings::enableDebugDeferredDepth (false);
		ATOM_RenderSettings::enableDebugDeferredLightBuffer (false);
		ATOM_RenderSettings::enableDebugShadowMap (false);
		ATOM_RenderSettings::enableDebugShadowMask (false);
		//--- wangjian added ---//
		// * 用以调试GBUFFER RT中保存的HalfLambert.
		ATOM_RenderSettings::enableDebugHalfLambertBuffer (false);
		ATOM_RenderSettings::enableDebugBloomBuffer (false);
		ATOM_RenderSettings::enableDebugHdrLumin (false);
		//----------------------//
		break;
	case RM_DEFERRED_DEPTH:
		ATOM_RenderSettings::enableDebugDeferredNormals (false);
		ATOM_RenderSettings::enableDebugDeferredColors (false);
		ATOM_RenderSettings::enableDebugDeferredDepth (true);
		ATOM_RenderSettings::enableDebugDeferredLightBuffer (false);
		ATOM_RenderSettings::enableDebugShadowMap (false);
		ATOM_RenderSettings::enableDebugShadowMask (false);
		//--- wangjian added ---//
		// * 用以调试GBUFFER RT中保存的HalfLambert.
		ATOM_RenderSettings::enableDebugHalfLambertBuffer (false);
		ATOM_RenderSettings::enableDebugBloomBuffer (false);
		ATOM_RenderSettings::enableDebugHdrLumin (false);
		//----------------------//
		break;
	case RM_DEFERRED_LIGHTBUFFER:
		ATOM_RenderSettings::enableDebugDeferredNormals (false);
		ATOM_RenderSettings::enableDebugDeferredColors (false);
		ATOM_RenderSettings::enableDebugDeferredDepth (false);
		ATOM_RenderSettings::enableDebugDeferredLightBuffer (true);
		ATOM_RenderSettings::enableDebugShadowMap (false);
		ATOM_RenderSettings::enableDebugShadowMask (false);
		//--- wangjian added ---//
		// * 用以调试GBUFFER RT中保存的HalfLambert.
		ATOM_RenderSettings::enableDebugHalfLambertBuffer (false);
		ATOM_RenderSettings::enableDebugBloomBuffer (false);
		ATOM_RenderSettings::enableDebugHdrLumin (false);
		//----------------------//
		break;
	case RM_DEFERRED_SHADOWMAP:
		ATOM_RenderSettings::enableDebugDeferredNormals (false);
		ATOM_RenderSettings::enableDebugDeferredColors (false);
		ATOM_RenderSettings::enableDebugDeferredDepth (false);
		ATOM_RenderSettings::enableDebugDeferredLightBuffer (false);
		ATOM_RenderSettings::enableDebugShadowMap (true);
		ATOM_RenderSettings::enableDebugShadowMask (false);
		//--- wangjian added ---//
		// * 用以调试GBUFFER RT中保存的HalfLambert.
		ATOM_RenderSettings::enableDebugHalfLambertBuffer (false);
		ATOM_RenderSettings::enableDebugBloomBuffer (false);
		ATOM_RenderSettings::enableDebugHdrLumin (false);
		//----------------------//
		break;
	case RM_DEFERRED_SHADOWMASK:
		ATOM_RenderSettings::enableDebugDeferredNormals (false);
		ATOM_RenderSettings::enableDebugDeferredColors (false);
		ATOM_RenderSettings::enableDebugDeferredDepth (false);
		ATOM_RenderSettings::enableDebugDeferredLightBuffer (false);
		ATOM_RenderSettings::enableDebugShadowMap (false);
		ATOM_RenderSettings::enableDebugShadowMask (true);
		//--- wangjian added ---//
		// * 用以调试GBUFFER RT中保存的HalfLambert.
		ATOM_RenderSettings::enableDebugHalfLambertBuffer (false);
		ATOM_RenderSettings::enableDebugBloomBuffer (false);
		ATOM_RenderSettings::enableDebugHdrLumin (false);
		//----------------------//
		break;
		//--- wangjian added ---//
		// * 用以调试GBUFFER RT中保存的HalfLambert.
	case RM_DEFERRED_HALFLAMBERT:
		ATOM_RenderSettings::enableDebugHalfLambertBuffer (true);
		ATOM_RenderSettings::enableDebugBloomBuffer (false);
		ATOM_RenderSettings::enableDebugDeferredNormals (false);
		ATOM_RenderSettings::enableDebugDeferredColors (false);
		ATOM_RenderSettings::enableDebugDeferredDepth (false);
		ATOM_RenderSettings::enableDebugDeferredLightBuffer (false);
		ATOM_RenderSettings::enableDebugShadowMap (false);
		ATOM_RenderSettings::enableDebugShadowMask (false);
		ATOM_RenderSettings::enableDebugHdrLumin (false);
		break;
	case RM_HDR_LUMIN:
		ATOM_RenderSettings::enableDebugHdrLumin (true);
		ATOM_RenderSettings::enableDebugHalfLambertBuffer (false);
		ATOM_RenderSettings::enableDebugBloomBuffer (false);
		ATOM_RenderSettings::enableDebugDeferredNormals (false);
		ATOM_RenderSettings::enableDebugDeferredColors (false);
		ATOM_RenderSettings::enableDebugDeferredDepth (false);
		ATOM_RenderSettings::enableDebugDeferredLightBuffer (false);
		ATOM_RenderSettings::enableDebugShadowMap (false);
		ATOM_RenderSettings::enableDebugShadowMask (false);
		break;
	case RM_BLOOM:
		ATOM_RenderSettings::enableDebugBloomBuffer (true);
		ATOM_RenderSettings::enableDebugHdrLumin (false);
		ATOM_RenderSettings::enableDebugHalfLambertBuffer (false);
		ATOM_RenderSettings::enableDebugDeferredNormals (false);
		ATOM_RenderSettings::enableDebugDeferredColors (false);
		ATOM_RenderSettings::enableDebugDeferredDepth (false);
		ATOM_RenderSettings::enableDebugDeferredLightBuffer (false);
		ATOM_RenderSettings::enableDebugShadowMap (false);
		ATOM_RenderSettings::enableDebugShadowMask (false);
		break;
		//----------------------//
		
	}
	_renderMode = mode;
}

void EditorImpl::toggleWireFrame (void)
{
#if 0
	bool wireframe = !getRealtimeCtrl()->isWireframeMode();
	getRealtimeCtrl()->setWireframeMode (wireframe);

	ATOM_DeferredScene *scene = dynamic_cast<ATOM_DeferredScene*>(getRealtimeCtrl()->getScene());
	if (scene)
	{
		scene->enableDeferredRendering (!wireframe);
	}
#endif
}

void EditorImpl::lookAtNode (ATOM_Node *node)
{
	const ATOM_BBox &bbox = node->getWorldBoundingbox ();

	ATOM_Vector3f dim = bbox.getMax()-bbox.getMin();
	float minDist;
	const float offset = 100.f;
	if (dim.x <= dim.y && dim.x <= dim.z)
	{
		minDist = dim.x * 0.5f + offset;
	}
	else if (dim.y <= dim.x && dim.y <= dim.z)
	{
		minDist = dim.y * 0.5f + offset;
	}
	else
	{
		minDist = dim.z * 0.5f + offset;
	}

	ATOM_Vector3f direction(minDist, -minDist, minDist);
	ATOM_Vector3f position = bbox.getCenter() - direction;
	direction.normalize ();

	getCameraModal ()->setDirection (direction);
	getCameraModal ()->setPosition (position);
	getCameraModal ()->setUpVector (ATOM_Vector3f (0.f, 1.f, 0.f));
}

int EditorImpl::allocClipboardContentType (const char *name)
{
	return _clipboard->allocContentType (name);
}

int EditorImpl::getClipboardContentType (const char *name) const
{
	return _clipboard->getContentType (name);
}

const char *EditorImpl::getClipboardContentName (int type) const
{
	return _clipboard->getContentName (type);
}

bool EditorImpl::setClipboardData (int contentType, const void *data, unsigned dataSize)
{
	return _clipboard->setContent (contentType, data, dataSize);
}

bool EditorImpl::setClipboardDataByName (const char *name, const void *data, unsigned dataSize)
{
	return _clipboard->setContent (name, data, dataSize);
}

unsigned EditorImpl::getClipboardDataLength (void) const
{
	return _clipboard->getContentSize ();
}

int EditorImpl::getClipboardData (void *data) const
{
	return _clipboard->getContent (data);
}

void EditorImpl::emptyClipboard (void)
{
	_clipboard->clear ();
}

void CapsViewCommandCallback::callback (ATOMX_TWCommandEvent *event)
{
	// ask for file name
	char filename[260];
	strcpy (filename, "");

	OPENFILENAMEA ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = _editorImpl->getRenderWindow()->getWindowInfo()->handle;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeof(filename);
	ofn.lpstrFilter = "Text File(*.txt)\0*.txt\0";
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = 0;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = 0;
	ofn.lpstrDefExt = "txt";
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (!::GetSaveFileNameA (&ofn))
	{
		return;
	}

	D3DCAPS9 caps;
	StudioApp *app = (StudioApp*)ATOM_APP;
	IDirect3DDevice9 *device = (IDirect3DDevice9*)(((ATOM_RenderDevice*)app->getDevice())->getInternalDevice());
	device->GetDeviceCaps (&caps);

	// save d3d caps to file
	char buffer[1024];
	FILE *fp = fopen (filename, "wt");
	if (!fp)
	{
		::MessageBox (_editorImpl->getRenderWindow()->getWindowInfo()->handle, _T("无法打开文件"), _T("ATOM3D编辑器"), MB_OK|MB_ICONHAND);
		return;
	}

	ATOM_DisplayInfo dispInfo;

	sprintf (buffer, "DeviceDesc=%s\n", dispInfo.getDeviceDescription(0));
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "DeviceManufactor=%s\n", dispInfo.getDeviceManufacturer(0));
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "DeviceChipType=%s\n", dispInfo.getDeviceChipType(0));
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "DeviceMemory=%s\n", dispInfo.getDeviceMemory(0));
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "DeviceDisplayMode=%s\n", dispInfo.getDeviceDisplayMode(0));
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "DeviceDriverName=%s\n", dispInfo.getDeviceDriverName(0));
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "DeviceDriverDate=%s\n", dispInfo.getDeviceDriverDate(0));
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "DeviceDriverVersion=%s\n", dispInfo.getDeviceDriverVersion(0));
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "DirectXVersion=%s\n", dispInfo.getDXVersionString());
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "VendorId=0x%08X\n", dispInfo.getDeviceVendorId(0));
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "DeviceId=0x%08X\n", dispInfo.getDeviceId(0));
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "DDrawAccel=%s\n", dispInfo.isDDrawAccelerationEnabled(0) ? "true" : "false");
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "D3DAccel=%s\n", dispInfo.is3DAccelerationEnabled(0) ? "true" : "false");
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "AGPEnabled=%s\n", dispInfo.isAGPEnabled(0) ? "true" : "false");
	fwrite (buffer, 1, strlen(buffer), fp);

	strcpy (buffer, "\n");
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "TextureNonLocalVideoMemory=%s\n", (caps.DevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM) != 0 ? "true" : "false");
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "TextureSystemMemory=%s\n", (caps.DevCaps & D3DDEVCAPS_TEXTURESYSTEMMEMORY) != 0 ? "true" : "false");
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "CubeMap=%s\n",  (caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP) != 0 ? "true" : "false");
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "CubeMapPow2=%s\n", (caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP_POW2) != 0 ? "true" : "false");
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "VolumeMap=%s\n", (caps.TextureCaps & D3DPTEXTURECAPS_VOLUMEMAP_POW2) != 0 ? "true" : "false");
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "Pow2=%s\n", (caps.TextureCaps & D3DPTEXTURECAPS_POW2) != 0 ? "true" : "false");
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "NonPow2Conditional=%s\n", (caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) != 0 ? "true" : "false");
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "AnisotropicFiltering=%s\n", (caps.RasterCaps & D3DPRASTERCAPS_ANISOTROPY) != 0 ? "true" : "false");
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxTextureWidth=%u\n", caps.MaxTextureWidth);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxTextureHeight=%u\n", caps.MaxTextureHeight);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxVolumeExtent=%u\n", caps.MaxVolumeExtent);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxTextureRepeat=%u\n", caps.MaxTextureRepeat);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxTextureAspectRatio=%u\n", caps.MaxTextureAspectRatio);
	fwrite (buffer, 1, strlen(buffer), fp);

	strcpy (buffer, "\n");
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "maxAnisotropy=%u\n", caps.MaxAnisotropy);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "HardwareRasterization=%s\n", (caps.DevCaps & D3DDEVCAPS_HWRASTERIZATION) != 0 ? "true" : "false");
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "HardwareTL=%s\n", (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0 ? "true" : "false");
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "PureDevice=%s\n", (caps.DevCaps & D3DDEVCAPS_PUREDEVICE) != 0 ? "true" : "false");
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "CanRenderAfterFlip=%s\n", (caps.DevCaps & D3DDEVCAPS_CANRENDERAFTERFLIP) != 0 ? "true" : "false");
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "CanToggleMultiSample=%s\n", (caps.RasterCaps & D3DPRASTERCAPS_MULTISAMPLE_TOGGLE) != 0 ? "true" : "false");
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "ScissorTest=%s\n", (caps.RasterCaps & D3DPRASTERCAPS_SCISSORTEST) != 0 ? "true" : "false");
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxVertexW=%f\n", caps.MaxVertexW);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxActiveLights=%u\n", caps.MaxActiveLights);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxUserClipPlanes=%u\n", caps.MaxUserClipPlanes);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxVertexBlendMatrices=%u\n", caps.MaxVertexBlendMatrices);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxVertexBlendMatrixIndex=%u\n", caps.MaxVertexBlendMatrixIndex);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxPointSize=%f\n", caps.MaxPointSize);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxPrimitiveCount=%u\n", caps.MaxPrimitiveCount);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxVertexIndex=%u\n", caps.MaxVertexIndex);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxStreams=%u\n", caps.MaxStreams);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxStreamStride=%u\n", caps.MaxStreamStride);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxTextureBlendStages=%u\n", caps.MaxTextureBlendStages);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxSimultaneousTextures=%u\n", caps.MaxSimultaneousTextures);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "NumSimultaneousRTs=%u\n", caps.NumSimultaneousRTs);
	fwrite (buffer, 1, strlen(buffer), fp);

	strcpy (buffer, "\n");
	fwrite (buffer, 1, strlen(buffer), fp);

	int version_major = D3DSHADER_VERSION_MAJOR(caps.VertexShaderVersion);
	int version_minor = D3DSHADER_VERSION_MINOR(caps.VertexShaderVersion);
	sprintf (buffer, "%d.%d", version_major, version_minor);
	sprintf (buffer, "VertexShaderVersion=%u.%u\n", version_major, version_minor);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxVertexShaderConst=%u\n", caps.MaxVertexShaderConst);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxVShaderInstructionsExecuted=%u\n", caps.MaxVShaderInstructionsExecuted);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxVertexShader30InstructionSlots=%u\n", caps.MaxVertexShader30InstructionSlots);
	fwrite (buffer, 1, strlen(buffer), fp);

	version_major = D3DSHADER_VERSION_MAJOR(caps.PixelShaderVersion);
	version_minor = D3DSHADER_VERSION_MINOR(caps.PixelShaderVersion);
	sprintf (buffer, "%d.%d", version_major, version_minor);
	sprintf (buffer, "PixelShaderVersion=%u.%u\n", version_major, version_minor);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "PixelShader1xMaxValue=%u\n", caps.PixelShader1xMaxValue);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxPShaderInstructionsExecuted=%u\n", caps.MaxPShaderInstructionsExecuted);
	fwrite (buffer, 1, strlen(buffer), fp);

	sprintf (buffer, "MaxPixelShader30InstructionSlots=%u\n", caps.MaxPixelShader30InstructionSlots);
	fwrite (buffer, 1, strlen(buffer), fp);

	fclose (fp);
}

void EditorImpl::showCurveEditor (bool show)
{
	if (!_curveEditor)
	{
		_curveEditor = ATOM_NEW(CurveEd, this, getRealtimeCtrl());
	}
	show ? _curveEditor->show () : _curveEditor->hide ();
}

void EditorImpl::showCameraParams (void)
{
}

void EditorImpl::editCamera (void)
{
	ATOM_DELETE(_cameraOptionView);

	_cameraOptionView = ATOM_NEW(ATOMX_TweakBar, "Edit Camera");
	_cameraOptionView->setBarColor (0.5f, 0.5f, 0.5f, 1.f);
	_cameraOptionView->setBarMovable (true);
	_cameraOptionView->setBarResizable (true);

	getCameraModal()->setupOptions (_cameraOptionView);

	_cameraOptionView->addIntVar ("W", CO_VIEWPORT_W, getRealtimeCtrl()->getViewport().size.w, false, "Viewport", 0);
	_cameraOptionView->addIntVar ("H", CO_VIEWPORT_H, getRealtimeCtrl()->getViewport().size.h, false, "Viewport", 0);

	_cameraOptionView->addFloatVar ("平移速度", CO_MOVE_SPEED, this->getCameraModal()->getMoveSpeed(), false, "Control", 0);
	_cameraOptionView->addFloatVar ("缩放速度", CO_ZOOM_SPEED, this->getCameraModal()->getZoomSpeed(), false, "Control", 0);
	_cameraOptionView->addFloatVar ("旋转速度", CO_ROTATE_SPEED, this->getCameraModal()->getRotateSpeed(), false, "Control", 0);
	_cameraOptionView->addFloatVar ("旋转半径", CO_ROTATE_RADIUS, this->getCameraModal()->getRotateRadius(), false, "Control", 0);
	/*
	_cameraOptionView->addFloatVar ("FOV", CO_FOV, this->getCameraModal()->getFovY() * 180.f / 3.1415926f, false, "Camera Options");
	_cameraOptionView->setVarMinMax ("FOV", 1.f, 180.f);
	_cameraOptionView->setVarStep ("FOV", 1.f);
	_cameraOptionView->addFloatVar ("Near", CO_NEAR, this->getCameraModal()->getNearPlane(), false, "Camera Options");
	_cameraOptionView->setVarMinMax ("Near", 0.f, 10000000.f);
	_cameraOptionView->addFloatVar ("Far", CO_FAR, this->getCameraModal()->getFarPlane(), false, "Camera Options");
	_cameraOptionView->setVarMinMax ("Far", 0.f, 10000000.f);
	_cameraOptionView->addFloatVar ("MinContrib", CO_MIN_CONTRIB, ATOM_RenderSettings::getMinScreenContribution(), false, "Camera Options");
	_cameraOptionView->setVarMinMax ("MinContrib", 0.f, 4.f);
	_cameraOptionView->setVarStep ("MinContrib", 0.0001f);
	*/
	_cameraOptionView->setBarSize (300, 300);
}

void EditorImpl::showD3DCaps (void)
{
	const unsigned width = 600;
	const unsigned height = 400;

	ATOM_DELETE(_D3DCapsView);

	_D3DCapsView = ATOM_NEW(ATOMX_TweakBar, "Device Information");
	_D3DCapsView->setBarColor (0.5f, 0.5f, 0.5f, 1.f);
	_D3DCapsView->setBarMovable (true);
	_D3DCapsView->setBarResizable (true);

	_D3DCapsView->setCommandCallback (_CapsViewCommandCallback);
	_D3DCapsView->addButton ("SaveCapsToFile", 0, "保存到文件..", "Operations");

	D3DCAPS9 caps;
	StudioApp *app = (StudioApp*)ATOM_APP;
	IDirect3DDevice9 *device = (IDirect3DDevice9*)(((ATOM_RenderDevice*)app->getDevice())->getInternalDevice());
	device->GetDeviceCaps (&caps);

	const char *textureCaps = "Texture";
	const char *deviceCaps = "Device";
	const char *vertexShaderCaps = "Vertex Shader";
	const char *pixelShaderCaps = "Pixel Shader";
	const char *tessellationCaps = "Hardware Tessellation";
	const char *deviceSummary = "Device Summary";
	char buffer[32];
	int version_major;
	int version_minor;

	ATOM_DisplayInfo dispInfo;

	_D3DCapsView->addStringVar ("设备描述", 200, dispInfo.getDeviceDescription(0), true, deviceSummary);
	_D3DCapsView->addStringVar ("制造商", 201, dispInfo.getDeviceManufacturer(0), true, deviceSummary);
	_D3DCapsView->addStringVar ("芯片组", 202, dispInfo.getDeviceChipType(0), true, deviceSummary);
	_D3DCapsView->addStringVar ("显存大小", 203, dispInfo.getDeviceMemory(0), true, deviceSummary);
	_D3DCapsView->addStringVar ("显示模式", 204, dispInfo.getDeviceDisplayMode(0), true, deviceSummary);
	_D3DCapsView->addStringVar ("驱动程序", 205, dispInfo.getDeviceDriverName(0), true, deviceSummary);
	_D3DCapsView->addStringVar ("驱动日期", 206, dispInfo.getDeviceDriverDate(0), true, deviceSummary);
	_D3DCapsView->addStringVar ("驱动版本", 207, dispInfo.getDeviceDriverVersion(0), true, deviceSummary);
	_D3DCapsView->addStringVar ("支持DX版本", 208, dispInfo.getDXVersionString(), true, deviceSummary);
	sprintf (buffer, "0x%08X", dispInfo.getDeviceVendorId(0));
	_D3DCapsView->addStringVar ("供应商ID", 209, buffer, true, deviceSummary);
	sprintf (buffer, "0x%08X", dispInfo.getDeviceId(0));
	_D3DCapsView->addStringVar ("设备ID", 210, buffer, true, deviceSummary);
	_D3DCapsView->addBoolVar ("DirectDraw加速", 211, dispInfo.isDDrawAccelerationEnabled(0), true, deviceSummary);
	_D3DCapsView->addBoolVar ("Direct3D加速", 212, dispInfo.is3DAccelerationEnabled(0), true, deviceSummary);
	_D3DCapsView->addBoolVar ("AGP加速", 211, dispInfo.isAGPEnabled(0), true, deviceSummary);

	_D3DCapsView->addBoolVar ("TextureNonLocalVideoMemory", 0, (caps.DevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM) != 0, true, textureCaps);
	_D3DCapsView->addBoolVar ("TextureSystemMemory", 1, (caps.DevCaps & D3DDEVCAPS_TEXTURESYSTEMMEMORY) != 0, true, textureCaps);
	_D3DCapsView->addBoolVar ("CubeMap", 2, (caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP) != 0, true, textureCaps);
	_D3DCapsView->addBoolVar ("CubeMapPow2", 3, (caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP_POW2) != 0, true, textureCaps);
	_D3DCapsView->addBoolVar ("VolumeMap", 4, (caps.TextureCaps & D3DPTEXTURECAPS_VOLUMEMAP) != 0, true, textureCaps);
	_D3DCapsView->addBoolVar ("VolumeMapPow2", 5, (caps.TextureCaps & D3DPTEXTURECAPS_VOLUMEMAP_POW2) != 0, true, textureCaps);
	_D3DCapsView->addBoolVar ("Pow2", 6, (caps.TextureCaps & D3DPTEXTURECAPS_POW2) != 0, true, textureCaps);
	_D3DCapsView->addBoolVar ("NonPow2Conditional", 7, (caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) != 0, true, textureCaps);
	_D3DCapsView->addBoolVar ("AnisotropicFiltering", 8, (caps.RasterCaps & D3DPRASTERCAPS_ANISOTROPY) != 0, true, textureCaps);
	_D3DCapsView->addIntVar ("MaxTextureWidth", 9, caps.MaxTextureWidth, true, textureCaps);
	_D3DCapsView->addIntVar ("MaxTextureHeight", 10, caps.MaxTextureHeight, true, textureCaps);
	_D3DCapsView->addIntVar ("MaxVolumeExtent", 11, caps.MaxVolumeExtent, true, textureCaps);
	_D3DCapsView->addIntVar ("MaxTextureRepeat", 12, caps.MaxTextureRepeat, true, textureCaps);
	_D3DCapsView->addIntVar ("MaxTextureAspectRatio", 13, caps.MaxTextureAspectRatio, true, textureCaps);
	_D3DCapsView->addIntVar ("MaxAnisotropy", 14, caps.MaxAnisotropy, true, textureCaps);

	_D3DCapsView->addBoolVar ("HardwareRasterization", 15, (caps.DevCaps & D3DDEVCAPS_HWRASTERIZATION) != 0, true, deviceCaps);
	_D3DCapsView->addBoolVar ("HardwareTL", 16, (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0, true, deviceCaps);
	_D3DCapsView->addBoolVar ("PureDevice", 17, (caps.DevCaps & D3DDEVCAPS_PUREDEVICE) != 0, true, deviceCaps);
	_D3DCapsView->addBoolVar ("CanRenderAfterFlip", 18, (caps.DevCaps & D3DDEVCAPS_CANRENDERAFTERFLIP) != 0, true, deviceCaps);
	_D3DCapsView->addBoolVar ("CanToggleMultiSample", 19, (caps.RasterCaps & D3DPRASTERCAPS_MULTISAMPLE_TOGGLE) != 0, true, deviceCaps);
	_D3DCapsView->addBoolVar ("ScissorTest", 20, (caps.RasterCaps & D3DPRASTERCAPS_SCISSORTEST) != 0, true, deviceCaps);
	_D3DCapsView->addFloatVar ("MaxVertexW", 21, caps.MaxVertexW, true, deviceCaps);
	_D3DCapsView->addIntVar ("MaxActiveLights", 22, caps.MaxActiveLights, true, deviceCaps);
	_D3DCapsView->addIntVar ("MaxUserClipPlanes", 23, caps.MaxUserClipPlanes, true, deviceCaps);
	_D3DCapsView->addIntVar ("MaxVertexBlendMatrices", 24, caps.MaxVertexBlendMatrices, true, deviceCaps);
	_D3DCapsView->addIntVar ("MaxVertexBlendMatrixIndex", 25, caps.MaxVertexBlendMatrixIndex, true, deviceCaps);
	_D3DCapsView->addFloatVar ("MaxPointSize", 26, caps.MaxPointSize, true, deviceCaps);
	_D3DCapsView->addIntVar ("MaxPrimitiveCount", 27, caps.MaxPrimitiveCount, true, deviceCaps);
	_D3DCapsView->addIntVar ("MaxVertexIndex", 28, caps.MaxVertexIndex, true, deviceCaps);
	_D3DCapsView->addIntVar ("MaxStreams", 29, caps.MaxStreams, true, deviceCaps);
	_D3DCapsView->addIntVar ("MaxStreamStride", 30, caps.MaxStreamStride, true, deviceCaps);
	_D3DCapsView->addIntVar ("MaxTextureBlendStages", 31, caps.MaxTextureBlendStages, true, deviceCaps);
	_D3DCapsView->addIntVar ("MaxSimultaneousTextures", 32, caps.MaxSimultaneousTextures, true, deviceCaps);
	_D3DCapsView->addIntVar ("NumSimultaneousRTs", 33, caps.NumSimultaneousRTs, true, deviceCaps);

	version_major = D3DSHADER_VERSION_MAJOR(caps.VertexShaderVersion);
	version_minor = D3DSHADER_VERSION_MINOR(caps.VertexShaderVersion);
	sprintf (buffer, "%d.%d", version_major, version_minor);
	_D3DCapsView->addStringVar ("VertexShaderVersion", 34, buffer, true, vertexShaderCaps);
	_D3DCapsView->addIntVar ("MaxVertexShaderConst", 35, caps.MaxVertexShaderConst, true, vertexShaderCaps);
	_D3DCapsView->addIntVar ("MaxVShaderInstructionsExecuted", 36, caps.MaxVShaderInstructionsExecuted, true, vertexShaderCaps);
	_D3DCapsView->addIntVar ("MaxVertexShader30InstructionSlots", 37, caps.MaxVertexShader30InstructionSlots, true, vertexShaderCaps);

	version_major = D3DSHADER_VERSION_MAJOR(caps.PixelShaderVersion);
	version_minor = D3DSHADER_VERSION_MINOR(caps.PixelShaderVersion);
	sprintf (buffer, "%d.%d", version_major, version_minor);
	_D3DCapsView->addStringVar ("PixelShaderVersion", 38, buffer, true, pixelShaderCaps);
	_D3DCapsView->addIntVar ("PixelShader1xMaxValue", 39, caps.PixelShader1xMaxValue, true, pixelShaderCaps);
	_D3DCapsView->addIntVar ("MaxPShaderInstructionsExecuted", 40, caps.MaxPShaderInstructionsExecuted, true, pixelShaderCaps);
	_D3DCapsView->addIntVar ("MaxPixelShader30InstructionSlots", 41, caps.MaxPixelShader30InstructionSlots, true, pixelShaderCaps);

	_D3DCapsView->addBoolVar ("NPatch", 42, (caps.DevCaps & D3DDEVCAPS_NPATCHES) != 0, true, tessellationCaps);
	_D3DCapsView->addFloatVar ("MaxNpatchTessellationLevel", 43, caps.MaxNpatchTessellationLevel, true, tessellationCaps);
	_D3DCapsView->addBoolVar ("RTPatches", 44, (caps.DevCaps & D3DDEVCAPS_RTPATCHES) != 0, true, tessellationCaps);
	_D3DCapsView->addBoolVar ("QuinticRTPatches", 45, (caps.DevCaps & D3DDEVCAPS_QUINTICRTPATCHES) != 0, true, tessellationCaps);
	_D3DCapsView->addBoolVar ("RTPatchHandleZero", 46, (caps.DevCaps & D3DDEVCAPS_RTPATCHHANDLEZERO) != 0, true, tessellationCaps);

	_D3DCapsView->setBarSize (width, height);
}


void EditorImpl::initMenuStates (ATOM_MenuPopupEvent *event)
{
	ATOM_MenuBar *menuBar = this->getRealtimeCtrl()->getMenuBar ();

	bool useDebug = false;
	bool useDeferred = _schemeMode == SCHEME_DEFERRED;
	menuBar->getMenuItem (3).submenu->getMenuItem (0).submenu->setCheck (0, useDeferred);
	menuBar->getMenuItem (3).submenu->getMenuItem (0).submenu->setCheck (1, !useDeferred);

	menuBar->getMenuItem (3).submenu->setCheck (1, false);
	menuBar->getMenuItem (3).submenu->setCheck (2, false);
	menuBar->getMenuItem (3).submenu->setCheck (3, false);
	menuBar->getMenuItem (3).submenu->setCheck (4, false);
	menuBar->getMenuItem (3).submenu->setCheck (5, false);
	menuBar->getMenuItem (3).submenu->setCheck (6, false);
	//--- wangjian added ---//
	// * 用以调试GBUFFER RT中保存的HalfLambert.
	menuBar->getMenuItem (3).submenu->setCheck (7, false);
	menuBar->getMenuItem(3).submenu->setCheck(8,false);
	//----------------------//

	if (ATOM_RenderSettings::isDebugDeferredNormalsEnabled())
	{
		menuBar->getMenuItem (3).submenu->setCheck (2, true);
		useDebug = true;
	}

	if (ATOM_RenderSettings::isDebugDeferredColorsEnabled ())
	{
		menuBar->getMenuItem (3).submenu->setCheck (3, true);
		useDebug = true;
	}

	if (ATOM_RenderSettings::isDebugDeferredDepthEnabled ())
	{
		menuBar->getMenuItem (3).submenu->setCheck (4, true);
		useDebug = true;
	}

	if (ATOM_RenderSettings::isDebugDeferredLightBufferEnabled ())
	{
		menuBar->getMenuItem (3).submenu->setCheck (5, true);
		useDebug = true;
	}

	if (ATOM_RenderSettings::isDebugShadowMapEnabled ())
	{
		menuBar->getMenuItem (3).submenu->setCheck (6, true);
		useDebug = true;
	}

	if (ATOM_RenderSettings::isDebugShadowMaskEnabled ())
	{
		menuBar->getMenuItem (3).submenu->setCheck (7, true);
		useDebug = true;
	}

	//--- wangjian added ---//
	// * 用以调试GBUFFER RT中保存的HalfLambert.
	if (ATOM_RenderSettings::isDebugHalfLambertBufferEnabled ())
	{
		menuBar->getMenuItem (3).submenu->setCheck (8, true);
		useDebug = true;
	}
	//----------------------//

	if (!useDebug)
	{
		menuBar->getMenuItem (3).submenu->setCheck (1, true);
	}

	//--- wangjian modified ---//
	// * 用以调试GBUFFER RT中保存的HalfLambert.
	//menuBar->getMenuItem (3).submenu->setCheck (7, false);
	//-------------------------//

	menuBar->getMenuItem (3).submenu->setCheck (11, ATOM_RenderSettings::isAtmosphereEffectEnabled());
	menuBar->getMenuItem (3).submenu->setCheck (12, ATOM_RenderSettings::getAtmosphereQuality () == ATOM_RenderSettings::QUALITY_HIGH);
	menuBar->getMenuItem (3).submenu->setCheck (13, ATOM_RenderSettings::isSSAOEnabled());
	menuBar->getMenuItem (3).submenu->setCheck (14, ATOM_RenderSettings::isSSAODownsampleEnabled());
	menuBar->getMenuItem (3).submenu->setCheck (15, ATOM_RenderSettings::isSSAOBlurEnabled());

	// 阴影质量
	menuBar->getMenuItem (3).submenu->getMenuItem(16).submenu->setCheck(0, false);
	menuBar->getMenuItem (3).submenu->getMenuItem(16).submenu->setCheck(1, false);
	menuBar->getMenuItem (3).submenu->getMenuItem(16).submenu->setCheck(2, false);
	if (ATOM_RenderSettings::isShadowEnabled ())
	{
		switch (ATOM_RenderSettings::getShadowQuality ())
		{
		case ATOM_RenderSettings::QUALITY_LOW:
			menuBar->getMenuItem (3).submenu->getMenuItem(16).submenu->setCheck(1, true);
			break;
		case ATOM_RenderSettings::QUALITY_HIGH:
			menuBar->getMenuItem (3).submenu->getMenuItem(16).submenu->setCheck(0, true);
			break;
		}
	}
	else
	{
		menuBar->getMenuItem (3).submenu->getMenuItem(16).submenu->setCheck(2, true);
	}

	//menuBar->getMenuItem (3).submenu->setCheck (17, ATOM_RenderSettings::isFSAAEnabled());

	// 水体质量
	menuBar->getMenuItem (3).submenu->getMenuItem(17).submenu->setCheck(0, false);
	menuBar->getMenuItem (3).submenu->getMenuItem(17).submenu->setCheck(1, false);
	menuBar->getMenuItem (3).submenu->getMenuItem(17).submenu->setCheck(2, false);
	switch (ATOM_RenderSettings::getWaterQuality ())
	{
	case ATOM_RenderSettings::QUALITY_LOW:
		menuBar->getMenuItem (3).submenu->getMenuItem(17).submenu->setCheck(0, true);
		break;
	case ATOM_RenderSettings::QUALITY_MEDIUM:
		menuBar->getMenuItem (3).submenu->getMenuItem(17).submenu->setCheck(1, true);
		break;
	case ATOM_RenderSettings::QUALITY_HIGH:
		menuBar->getMenuItem (3).submenu->getMenuItem(17).submenu->setCheck(2, true);
		break;
	}

	// 光源开关
	menuBar->getMenuItem (3).submenu->getMenuItem(18).submenu->setCheck(0, false);
	menuBar->getMenuItem (3).submenu->getMenuItem(18).submenu->setCheck(1, false);
	if(ATOM_RenderSettings::isDirectionalLightTypeEnabled())
	{
		menuBar->getMenuItem (3).submenu->getMenuItem(18).submenu->setCheck(0, true);
	}
	if(ATOM_RenderSettings::isPointLightTypeEnabled())
	{
		menuBar->getMenuItem (3).submenu->getMenuItem(18).submenu->setCheck(1, true);
	}

	// 地形贴图质量开关
	menuBar->getMenuItem (3).submenu->getMenuItem(19).submenu->setCheck(0, false);
	menuBar->getMenuItem (3).submenu->getMenuItem(19).submenu->setCheck(1, false);
	menuBar->getMenuItem (3).submenu->getMenuItem(19).submenu->setCheck(2, false);

	if( ATOM_RenderSettings::isTerrDetailNormalEnabled() )
	{
		menuBar->getMenuItem (3).submenu->getMenuItem(19).submenu->setCheck(2, true);
	}
	else if( !ATOM_RenderSettings::isTerrDetailNormalEnabled() && ATOM_RenderSettings::detailedTerrainEnabled())
	{
		menuBar->getMenuItem (3).submenu->getMenuItem(19).submenu->setCheck(1, true);
	}
	else
	{
		menuBar->getMenuItem (3).submenu->getMenuItem(19).submenu->setCheck(0, true);
	}


	menuBar->getMenuItem (3).submenu->setCheck (20, ATOM_RenderSettings::isToneMappingEnabled());

	if (_currentEditingPlugin && event)
	{
		_currentEditingPlugin->handleEvent (event);
	}
}

ATOM_RenderScheme *EditorImpl::getRenderSchemeDeferred (void) const
{
	// wangjian modified
#if 0
	if (!_renderSchemeDeferred)
	{
		_renderSchemeDeferred = (ATOM_DeferredRenderScheme*)ATOM_RenderScheme::createRenderScheme ("deferred");
		if (!_renderSchemeDeferred->init (ATOM_GetRenderDevice(), getRealtimeCtrl()->getClientRect().size.w, getRealtimeCtrl()->getClientRect().size.h))
		{
			ATOM_RenderScheme::destroyRenderScheme (_renderSchemeDeferred);
			_renderSchemeDeferred = 0;
			return 0;
		}
	}
	return _renderSchemeDeferred;
#else
	if( _renderSchemeForward )
	{
		ATOM_RenderScheme::destroyRenderScheme (_renderSchemeForward);
		_renderSchemeForward = 0;
	}

	if( _renderSchemeDeferred )
	{
		ATOM_RenderScheme::destroyRenderScheme (_renderSchemeDeferred);
		//_renderSchemeDeferred = 0;
	}

	//if (!_renderSchemeDeferred)
	{
		_renderSchemeDeferred = (ATOM_DeferredRenderScheme*)ATOM_RenderScheme::createRenderScheme ("deferred");
		if (!_renderSchemeDeferred->init (ATOM_GetRenderDevice(), getRealtimeCtrl()->getClientRect().size.w, getRealtimeCtrl()->getClientRect().size.h))
		{
			ATOM_RenderScheme::destroyRenderScheme (_renderSchemeDeferred);
			_renderSchemeDeferred = 0;
			return 0;
		}
	}
	return _renderSchemeDeferred;
#endif
}

ATOM_RenderScheme *EditorImpl::getRenderSchemeForward (void) const
{
	// wangjian modified
#if 0
	if (!_renderSchemeForward)
	{
		_renderSchemeForward = (ATOM_CustomRenderScheme*)ATOM_RenderScheme::createRenderScheme ("custom");
		if (!_renderSchemeForward->init (ATOM_GetRenderDevice(), getRealtimeCtrl()->getClientRect().size.w, getRealtimeCtrl()->getClientRect().size.h))
		{
			ATOM_RenderScheme::destroyRenderScheme (_renderSchemeForward);
			_renderSchemeForward = 0;
			return 0;
		}
	}
	return _renderSchemeForward;
#else
	if( _renderSchemeDeferred )
	{
		ATOM_RenderScheme::destroyRenderScheme (_renderSchemeDeferred);
		_renderSchemeDeferred = 0;
		ATOM_DeferredRenderScheme::DestoryAllDSResources();
	}

	if( _renderSchemeForward )
	{
		ATOM_RenderScheme::destroyRenderScheme (_renderSchemeForward);
		//_renderSchemeForward = 0;
	}

	//if (!_renderSchemeForward)
	{
		_renderSchemeForward = (ATOM_CustomRenderScheme*)ATOM_RenderScheme::createRenderScheme ("custom");
		if (!_renderSchemeForward->init (ATOM_GetRenderDevice(), getRealtimeCtrl()->getClientRect().size.w, getRealtimeCtrl()->getClientRect().size.h))
		{
			ATOM_RenderScheme::destroyRenderScheme (_renderSchemeForward);
			_renderSchemeForward = 0;
			return 0;
		}
	}
	return _renderSchemeForward;
#endif
}

// wangjian modified
#if 0
ATOM_RenderScheme *EditorImpl::getRenderScheme (void) const
#else
ATOM_RenderScheme *EditorImpl::getRenderScheme (void)
#endif
{
	//-----------------------------------------------//
	// wangjian added
	if( ATOM_RenderSettings::isNonDeferredShading() )
	{
		_schemeMode = SCHEME_FORWARD;
	}
	else
	{
		_schemeMode = SCHEME_DEFERRED;
	}
	//-----------------------------------------------//

	switch (_schemeMode)
	{
	case SCHEME_DEFERRED:
		return getRenderSchemeDeferred ();
		break;
	case SCHEME_FORWARD:
		return getRenderSchemeForward ();
		break;
	default:
		return 0;
	}
}

void EditorImpl::emergencySave (void)
{
	if (_currentEditingPlugin)
	{
		if (IDYES == ::MessageBoxA (ATOM_APP->getMainWindow(), "<<注意!!>>程序已崩溃, 请珍惜最后一次保存的机会, 点'是'保存当前文档.", "ATOM3D编辑器", MB_YESNO|MB_ICONHAND))
		{
			saveDocumentAs ();
		}
	}
}

bool EditorImpl::showRenameDialog (ATOM_STRING &name)
{
	RenameDialog dlg;

	const char *s = dlg.prompt (getRealtimeCtrl(), 0, name.c_str());

	if (s)
	{
		name = s;
		return true;
	}

	return false;
}

#define ID_SELECT_PROP_TYPE_OK		(PLUGIN_ID_START + 1200)
#define ID_SELECT_PROP_TYPE_CANCEL	(PLUGIN_ID_START + 1201)
#define ID_LIST_ALL_PROP_TYPES		(PLUGIN_ID_START + 1202)
#define ID_EDIT_PROP_NAME			(PLUGIN_ID_START + 1203)

class __PropTypeSelectDialogEventTrigger: public ATOM_EventTrigger
{
public:
	__PropTypeSelectDialogEventTrigger (void)
	{
	}

	void onCommand (ATOM_WidgetCommandEvent *event)
	{
		ATOM_Dialog *dialog = (ATOM_Dialog*)getHost();

		switch (event->id)
		{
		case ID_SELECT_PROP_TYPE_OK:
			{
				ATOM_ListBox *listBoxType = (ATOM_ListBox*)dialog->getChildById (ID_LIST_ALL_PROP_TYPES);
				ATOM_Edit *editName = (ATOM_Edit*)dialog->getChildById (ID_EDIT_PROP_NAME);
				ATOM_STRING s;
				editName->getString (s);
				if (listBoxType->getSelectIndex() >= 0 && !s.empty())
				{
					dialog->endModal (listBoxType->getSelectIndex());
				}
				break;
			}
		case ID_SELECT_PROP_TYPE_CANCEL:
		case ATOM_Widget::CloseButtonId:
			{
				dialog->endModal (-1);
				break;
			}
		}
	}

	ATOM_DECLARE_EVENT_MAP(__PropTypeSelectDialogEventTrigger, ATOM_EventTrigger)
};

ATOM_BEGIN_EVENT_MAP(__PropTypeSelectDialogEventTrigger, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(__PropTypeSelectDialogEventTrigger, ATOM_WidgetCommandEvent, onCommand)
ATOM_END_EVENT_MAP

bool EditorImpl::showNewPropertyDialog (AS_PropertyInfo *info)
{
	ATOM_Desktop *desktop = getGUIRenderer()->getDesktop();
	ATOM_Rect2Di rc = desktop->getClientRect();
	int dlgWidth = 150;
	int dlgHeight = 260;
	int x = (rc.size.w - dlgWidth) / 2;
	int y = (rc.size.h - dlgHeight) / 2;

	ATOM_Dialog *typeDialog = ATOM_NEW(ATOM_Dialog, desktop, ATOM_Rect2Di(x, y, dlgWidth, dlgHeight));

	ATOM_Label *labelPropName = ATOM_NEW(ATOM_Label, typeDialog, ATOM_Rect2Di (20, 5, 110, 20), 0, 0, ATOM_Widget::ShowNormal);
	labelPropName->setText ("属性名称:");
	labelPropName->setAlign (ATOM_Widget::AlignX_Left|ATOM_Widget::AlignY_Middle);
	labelPropName->setFont (ATOM_GUIFont::getDefaultFont(12, 0));

	ATOM_Edit *nameEdit = ATOM_NEW(ATOM_Edit, typeDialog, ATOM_Rect2Di(20, 35, 110, 20), int(ATOM_Widget::Border), int(ID_EDIT_PROP_NAME), ATOM_Widget::ShowNormal);
	nameEdit->setBorderMode (ATOM_Widget::Drop);
	nameEdit->setFont (ATOM_GUIFont::getDefaultFont(12, 0));

	ATOM_Label *labelPropType = ATOM_NEW(ATOM_Label, typeDialog, ATOM_Rect2Di (20, 65, 110, 20), 0, 0, ATOM_Widget::ShowNormal);
	labelPropType->setText ("选择属性类型:");
	labelPropType->setAlign (ATOM_Widget::AlignX_Left|ATOM_Widget::AlignY_Middle);
	labelPropType->setFont (ATOM_GUIFont::getDefaultFont(12, 0));

	ATOM_ListBox *allTypesList = ATOM_NEW(ATOM_ListBox, typeDialog, ATOM_Rect2Di(20, 95, 110, 90), 16, int(ATOM_Widget::Border|ATOM_Widget::VScroll), int(ID_LIST_ALL_PROP_TYPES), ATOM_Widget::ShowNormal);
	allTypesList->setBorderMode (ATOM_Widget::Drop);
	allTypesList->setFont (ATOM_GUIFont::getDefaultFont(12, 0));
	allTypesList->setHoverImageId (ATOM_IMAGEID_LISTITEM);
	allTypesList->addItem ("整数");
	allTypesList->addItem ("浮点数");
	allTypesList->addItem ("字符串");
	allTypesList->addItem ("矢量");
	allTypesList->addItem ("颜色");
	allTypesList->addItem ("开关");
	allTypesList->selectItem (0);

	ATOM_Button *buttonOk = ATOM_NEW(ATOM_Button, typeDialog, ATOM_Rect2Di(20, 195, 40, 20), int(ATOM_Widget::Border), int(ID_SELECT_PROP_TYPE_OK), ATOM_Widget::ShowNormal);
	buttonOk->setText ("确定");
	buttonOk->setFont (ATOM_GUIFont::getDefaultFont (12, 0));

	ATOM_Button *buttonCancel = ATOM_NEW(ATOM_Button, typeDialog, ATOM_Rect2Di(90, 195, 40, 20), int(ATOM_Widget::Border), int(ID_SELECT_PROP_TYPE_CANCEL), ATOM_Widget::ShowNormal);
	buttonCancel->setText ("取消");
	buttonCancel->setFont (ATOM_GUIFont::getDefaultFont (12, 0));

	__PropTypeSelectDialogEventTrigger trigger;
	typeDialog->setEventTrigger (&trigger);
	int index = typeDialog->showModal ();
	nameEdit->getString (info->name);

	ATOM_DELETE(typeDialog);

	switch (index)
	{
	case 0:
		info->type = AS_PROP_TYPE_INT;
		return true;
	case 1:
		info->type = AS_PROP_TYPE_FLOAT;
		return true;
	case 2:
		info->type = AS_PROP_TYPE_STRING;
		return true;
	case 3:
		info->type = AS_PROP_TYPE_VECTOR;
		return true;
	case 4:
		info->type = AS_PROP_TYPE_COLOR4F;
		return true;
	case 5:
		info->type = AS_PROP_TYPE_SWITCH;
		return true;
	default:
		info->type = AS_PROP_TYPE_NONE;
		return false;
	}
}

