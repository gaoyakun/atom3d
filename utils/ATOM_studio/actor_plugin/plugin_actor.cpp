#include "StdAfx.h"
#include "atom3d_studio.h"
#include "plugin.h"
#include "gridnode.h"
#include "editor.h"
#include "plugin_actor.h"
#include "camera_modal.h"

enum PropertyType
{
	PROP_TYPE_NONE = -1,
	PROP_TYPE_INT = 0,
	PROP_TYPE_FLOAT = 1,
	PROP_TYPE_STRING = 2,
	PROP_TYPE_VECTOR = 3,
	PROP_TYPE_COLOR4F = 4,
	PROP_TYPE_SWITCH = 5
};

#define ID_PART_LISTBOX (PLUGIN_ID_START + 10)

#define ID_NEW_PART					(PLUGIN_ID_START + 100)
#define ID_DEL_PART					(PLUGIN_ID_START + 101)
#define ID_EDIT_PART_NAME			(PLUGIN_ID_START + 102)
#define ID_LIST_PART_TYPE			(PLUGIN_ID_START + 103)
#define ID_LIST_ALL_PARTS			(PLUGIN_ID_START + 104)
#define ID_LIST_SEL_PARTS			(PLUGIN_ID_START + 105)
#define ID_NEW_PART_OK				(PLUGIN_ID_START + 106)
#define ID_NEW_PART_CANCEL			(PLUGIN_ID_START + 107)
#define ID_SELECT_PART				(PLUGIN_ID_START + 108)
#define ID_DESELECT_PART			(PLUGIN_ID_START + 109)
#define ID_AFFECT_PARTS_OK			(PLUGIN_ID_START + 110)
#define ID_AFFECT_PARTS_CANCEL		(PLUGIN_ID_START + 111)
#define MENU_ITEM_ID_PART			(PLUGIN_ID_START + 112)
#define MENU_ITEM_ID_ACTIONU		(PLUGIN_ID_START + 113)
#define MENU_ITEM_ID_ACTIOND		(PLUGIN_ID_START + 114)
#define MENU_ITEM_ID_VIEWJOINTS		(PLUGIN_ID_START + 115)
#define MENU_ITEM_ID_VIEWMODE		(PLUGIN_ID_START + 116)
#define ID_SELECT_PROP_TYPE_OK		(PLUGIN_ID_START + 117)
#define ID_SELECT_PROP_TYPE_CANCEL	(PLUGIN_ID_START + 118)
#define ID_LIST_ALL_PROP_TYPES		(PLUGIN_ID_START + 119)
#define ID_EDIT_PROP_NAME			(PLUGIN_ID_START + 120)
#define ID_IMPORT					(PLUGIN_ID_START + 121)
#define ID_EXPORT					(PLUGIN_ID_START + 122)

#define ID_POINTLIST				(PLUGIN_ID_START + 150)
#define ID_CREATE_POINT				(PLUGIN_ID_START + 151)
#define ID_DELETE_POINT				(PLUGIN_ID_START + 152)
#define ID_RENAME_POINT				(PLUGIN_ID_START + 153)
#define ID_DELETE_ALL_POINTS		(PLUGIN_ID_START + 154)
#define ID_EDIT_POINT_TRANSLATION	(PLUGIN_ID_START + 155)
#define ID_EDIT_POINT_ROTATION		(PLUGIN_ID_START + 156)
#define ID_EDIT_POINT_SCALE			(PLUGIN_ID_START + 157)
#define ID_EDIT_POINT_ENDEDIT		(PLUGIN_ID_START + 158)
#define ID_EDIT_POINT_PROP			(PLUGIN_ID_START + 159)
#define ID_POINT_TO_CAMERA			(PLUGIN_ID_START + 160)
#define ID_CAMERA_TO_POINT			(PLUGIN_ID_START + 161)

#define ID_CANDIDATE_BAR_CURRENT_ID				(PLUGIN_ID_START + 200)
#define ID_CANDIDATE_BAR_NEW					(PLUGIN_ID_START + 201)
#define ID_CANDIDATE_BAR_NEW_CANDIDATE			(PLUGIN_ID_START + 202)
#define ID_CANDIDATE_BAR_COMP_FILENAME			(PLUGIN_ID_START + 203)
#define ID_CANDIDATE_BAR_COLOR_COLOR			(PLUGIN_ID_START + 204)
#define ID_CANDIDATE_BAR_TEXTURE_ALBEDO			(PLUGIN_ID_START + 205)
#define ID_CANDIDATE_BAR_TEXTURE_NORMAL			(PLUGIN_ID_START + 206)
#define ID_CANDIDATE_BAR_ID						(PLUGIN_ID_START + 207)
#define ID_CANDIDATE_BAR_DELETE					(PLUGIN_ID_START + 208)
#define ID_CANDIDATE_BAR_AFFECT_PARTS			(PLUGIN_ID_START + 209)
#define ID_CANDIDATE_BAR_BINDING_FILENAME		(PLUGIN_ID_START + 210)
#define ID_CANDIDATE_BAR_BINDING_BONE_NAME		(PLUGIN_ID_START + 211)
#define ID_CANDIDATE_BAR_SCALE					(PLUGIN_ID_START + 212)
#define ID_NEW_PROPERTY							(PLUGIN_ID_START + 213)
#define ID_CLEAR_PROPERTIES						(PLUGIN_ID_START + 214)
#define ID_PROPERTY_VALUE						(PLUGIN_ID_START + 215)
#define ID_PROPERTY_DELETE						(PLUGIN_ID_START + 216)
#define ID_VIEWMODE_MODEL						(PLUGIN_ID_START + 217)
#define ID_VIEWMODE_SKELETON					(PLUGIN_ID_START + 218)

#define ID_ACTION_NAME_STARTU					(PLUGIN_ID_START + 500)
#define ID_ACTION_NAME_STARTD					(PLUGIN_ID_START + 600)
#define ID_JOINT_NAMES							(PLUGIN_ID_START + 700)

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

PluginActor::PluginActor (void)
{
	_editor = 0;
	_scene = 0;
	_tbPartCandidates = 0;
	_tbProperties = 0;
	_partListBox = 0;
	_pointList = 0;
	_pointListMenu = 0;
	_currentPart = 0;
	_editingPoint = false;
}

PluginActor::~PluginActor (void)
{
	clear ();
}

unsigned PluginActor::getVersion (void) const
{
	return AS_VERSION;
}

const char *PluginActor::getName (void) const
{
	return "Actor editor";
}

void PluginActor::deleteMe (void)
{
	ATOM_DELETE(this);
}

bool PluginActor::initPlugin (AS_Editor *editor)
{
	editor->registerFileType (this, "act", "ATOM3D 角色", AS_FILETYPE_CANEDIT);

	_editor = editor;

	return true;
}

void PluginActor::donePlugin (void)
{
	clear ();
}

bool PluginActor::beginEdit (const char *filename)
{
	_scene = ATOM_NEW(ATOM_DeferredScene);
	ATOM_RenderScheme *renderScheme = _editor->getRenderScheme ();
	if (!renderScheme)
	{
		return false;
	}
	_scene->setRenderScheme (renderScheme);

	_grid = ATOM_HARDREF(GridNode)();
	_grid->setPickable (0);
	_grid->setSize (100, 100);
	_grid->load (ATOM_GetRenderDevice());
	_grid->setO2T (ATOM_Matrix4x4f::getScaleMatrix(ATOM_Vector3f(100.f, 100.f, 100.f)));
	_scene->getRootNode()->appendChild (_grid.get());

	ATOM_HARDREF(ATOM_Sky) sky;
	if (sky->load (ATOM_GetRenderDevice()))
	{
		ATOM_Vector3f v(1.f, -1.f, 1.f);
		v.normalize();
		sky->setLightDir (v);
		sky->setLightIntensity (3.f);
		sky->setAmbientLight (1.f);
		_scene->getRootNode()->appendChild (sky.get());
	}

	_editingPoint = false;

	AS_CameraModal *camera = _editor->getCameraModal();
	camera->setPosition (ATOM_Vector3f(0.f, 500.f, -500.f));
	camera->setDirection (ATOM_Vector3f(0.f, -500.f, 500.f));
	camera->setUpVector (ATOM_Vector3f(0.f, 1.f, 0.f));

	_editor->getRealtimeCtrl()->setScene (_scene);

	_actor = ATOM_HARDREF(ATOM_Actor)();
	_actor->setO2T (ATOM_Matrix4x4f::getScaleMatrix(ATOM_Vector3f(10.f, 10.f, 10.f)));
	_actor->setDrawBoundingbox (1);
	_scene->getRootNode()->appendChild (_actor.get());

	_skeletonVisualizer = ATOM_HARDREF(ATOMX_SkeletonVisualizerNode)();
	_skeletonVisualizer->setO2T (ATOM_Matrix4x4f::getScaleMatrix(ATOM_Vector3f(10.f, 10.f, 10.f)));
	_skeletonVisualizer->setSource (_actor->getInternalGeode ());

	bool loaded;
	if (filename)
	{
		loaded = _actor->loadFromFile (ATOM_GetRenderDevice(), filename);
		ATOM_ContentStream::waitForAllDone ();
		_editor->lookAtNode (_actor.get());

		_propertyTypes.clear ();

		// load Editor-Specific contents
		ATOM_AutoFile f(filename, ATOM_VFS::read|ATOM_VFS::text);
		if (f)
		{
			unsigned size = f->size ();
			char *p = ATOM_NEW_ARRAY(char, size);
			size = f->read (p, size);
			p[size] = '\0';
			ATOM_TiXmlDocument doc;
			doc.Parse (p);
			ATOM_DELETE_ARRAY(p);
			if (!doc.Error())
			{
				ATOM_TiXmlElement *eEditorSpecific = doc.FirstChildElement ("EditorSpec");
				if (eEditorSpecific)
				{
					if (!loadPropertyTypes (*eEditorSpecific))
					{
						clear ();
						return false;
					}
				}
			}
		}
	}
	else
	{
		loaded = _actor->load (ATOM_GetRenderDevice());

#if 0
		ATOM_HARDREF(ATOM_Geode) pdGeode;
		ATOM_VECTOR<ATOM_STRING> modelFileNames;
		modelFileNames.push_back("/zy/ryzsb01_animation.nm");
		modelFileNames.push_back("/zy/ryzsb01/ryzsb01_mesh_s.nm");
		modelFileNames.push_back("/zy/ryzsb01/ryzsb01_mesh_t.nm");
		modelFileNames.push_back("/zy/ryzsb01/ryzsb01_mesh_x.nm");
		modelFileNames.push_back("/zy/ryzsb01/ryzsb01_mesh_y.nm");
		pdGeode->setModelFileNames (modelFileNames);
		if (pdGeode->mtload (ATOM_GetRenderDevice(), 0, 0, 0, 0))
		{
			pdGeode->doAction ("idle", ATOM_Geode::ACTIONFLAGS_DOWNSIDE|ATOM_Geode::ACTIONFLAGS_UPSIDE);
			pdGeode->setDrawBoundingbox (true);
			_scene->getRootNode()->appendChild (pdGeode.get());
		}
#endif
	}
	if (!loaded)
	{
		clear ();
		return false;
	}

	createPartListBox ();
	createPointList ();
	createCandidatesBar ();
	createPropertyBar ();
	setupPropertyBar ();

	calcUILayout ();

	setupMenu ();

	return true;
}

void PluginActor::endEdit (void)
{
	cleanupMenu ();

	clear ();
}

const char *jointName = 0;

void PluginActor::frameUpdate (void)
{
	if (jointName)
	{
		ATOM_Matrix4x4f matrix;
		if (_actor)
		{
			int jointIndex = _actor->getInternalGeode()->getBoneIndex (jointName);
			_actor->getInternalGeode()->getBoneMatrix (jointIndex, matrix, true);
			matrix.m00 = 1.0f;
			matrix.m01 = 0.0f;
			matrix.m02 = 0.0f;
			matrix.m10 = 0.0f;
			matrix.m11 = 1.0f;
			matrix.m12 = 0.0f;
			matrix.m20 = 0.0f;
			matrix.m21 = 0.0f;
			matrix.m22 = 1.0f;
			_actor->getInternalGeode()->getActionMixer()->enableManualBoneTransform (jointIndex, matrix);
		}
	}
}

void PluginActor::handleEvent (ATOM_Event *event)
{
	int eventId = event->getEventTypeId ();

	if (eventId == ATOM_WidgetCommandEvent::eventTypeId ())
	{
		ATOM_WidgetCommandEvent *e = (ATOM_WidgetCommandEvent*)event;

		if (e->id >= ID_JOINT_NAMES)
		{
			const char *jointName2 = _jointMenu->getMenuItem (e->id - ID_JOINT_NAMES).title.c_str();
			if (jointName)
			{
				_actor->getInternalGeode()->getActionMixer()->disableManualBoneTransform (_actor->getInternalGeode()->getBoneIndex (jointName));
			}

			if (jointName2 == jointName)
			{
				jointName = 0;
			}
			else
			{
				jointName = jointName2;
			}
			//_actor->getInternalGeode()->getActionMixer()->setJointLocalOrientation (_actor->getInternalGeode()->getBoneIndex(jointName), ATOM_Quaternion(0.f, 0.f, 0.f, 1.f));
		}
		else if (e->id >= ID_ACTION_NAME_STARTD)
		{
			const char *actionName = _actor->getInternalGeode()->getTrack(e->id - ID_ACTION_NAME_STARTD)->getName();
			if (actionName)
			{
				_actor->doAction (actionName, ATOM_Geode::ACTIONFLAGS_DOWNSIDE, 0, false, 300);
			}
		}
		else if (e->id >= ID_ACTION_NAME_STARTU)
		{
			const char *actionName = _actor->getInternalGeode()->getTrack(e->id - ID_ACTION_NAME_STARTU)->getName();
			if (actionName)
			{
				_actor->doAction (actionName, ATOM_Geode::ACTIONFLAGS_UPSIDE, 0, false, 300);
			}
		}
		else
		{
			switch (e->id)
			{
			case ID_NEW_PART:
				{
					onNewPart ();
					break;
				}
			case ID_DEL_PART:
				{
					onDeletePart ();
					break;
				}
			case ID_IMPORT:
				{
					_editingPoint = false;

					if (_editor->getOpenFileNames ("cp", "CP文件|*.cp|", false, false, "导入角色定义文件") > 0)
					{
						const char *filename = _editor->getOpenedFileName (0);
						if (!importCP (filename))
						{
							::MessageBoxA (ATOM_APP->getMainWindow(), "导入失败！", "错误", MB_OK|MB_ICONHAND);
						}
						refreshPointList ();
					}
					break;
				}
			case ID_EXPORT:
				{
					if (_editor->getOpenFileNames ("cp", "CP文件|*.cp|", false, true, "导出角色定义文件") > 0)
					{
						const char *filename = _editor->getOpenedFileName (0);
						if (!exportCP (filename))
						{
							::MessageBoxA (ATOM_APP->getMainWindow(), "导出失败!", "错误", MB_OK|MB_ICONHAND);
						}
					}
					break;
				}
			case ID_VIEWMODE_MODEL:
				{
					_scene->getRootNode()->removeChild (_skeletonVisualizer.get());
					_scene->getRootNode()->appendChild (_actor.get());
					break;
				}
			case ID_VIEWMODE_SKELETON:
				{
					_scene->getRootNode()->removeChild (_actor.get());
					_scene->getRootNode()->appendChild (_skeletonVisualizer.get());
					break;
				}
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
					_editor->endEditNodeTransform ();
					_editingPoint = false;
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
	}
	else if (eventId == ATOM_MenuPopupEvent::eventTypeId())
	{
		ATOM_MenuPopupEvent *e = (ATOM_MenuPopupEvent*)event;
		if (e->id == MENU_ITEM_ID_ACTIONU)
		{
			ATOM_ContentStream::waitForAllDone();

			_actionMenuUp->clear ();

			for (unsigned i = 0; i < _actor->getInternalGeode()->getNumTracks (); ++i)
			{
				ATOM_JointAnimationTrack *track = _actor->getInternalGeode()->getTrack (i);
				_actionMenuUp->appendMenuItem (track->getName(), ID_ACTION_NAME_STARTU + i);
			}
		}
		if (e->id == MENU_ITEM_ID_ACTIOND)
		{
			ATOM_ContentStream::waitForAllDone();

			_actionMenuDown->clear ();

			for (unsigned i = 0; i < _actor->getInternalGeode()->getNumTracks (); ++i)
			{
				ATOM_JointAnimationTrack *track = _actor->getInternalGeode()->getTrack (i);
				_actionMenuDown->appendMenuItem (track->getName(), ID_ACTION_NAME_STARTD + i);
			}
		}
		if (e->id == MENU_ITEM_ID_VIEWJOINTS)
		{
			ATOM_ContentStream::waitForAllDone();

			_jointMenu->clear ();
			ATOM_Geode *geode = _actor->getInternalGeode ();
			if (geode)
			{
				ATOM_Skeleton *skeleton = geode->getSkeleton ();
				if (skeleton)
				{
					const ATOM_HASHMAP<ATOM_STRING, unsigned> &joints = skeleton->getAttachPoints ();
					int idx = ID_JOINT_NAMES;
					for (ATOM_HASHMAP<ATOM_STRING, unsigned>::const_iterator it = joints.begin(); it != joints.end(); ++it, ++idx)
					{
						_jointMenu->appendMenuItem (it->first.c_str(), idx);
					}
				}
			}
		}
	}
	else if (eventId == ATOMX_TWCommandEvent::eventTypeId())
	{
		ATOMX_TWCommandEvent *e = (ATOMX_TWCommandEvent*)event;
		if (e->bar == _tbProperties)
		{
			switch (e->id)
			{
			case ID_CLEAR_PROPERTIES:
				{
					_propertyTypes.clear ();
					setupPropertyBar ();
					break;
				}
			case ID_PROPERTY_DELETE:
				{
					ATOM_HASHMAP<ATOM_STRING, PropertyInfo>::iterator it = _propertyTypes.find ((const char*)e->userdata);
					if (it != _propertyTypes.end ())
					{
						_propertyTypes.erase (it);
						setupPropertyBar ();
					}
					break;
				}
			case ID_NEW_PROPERTY:
				{
					ATOM_STRING name;
					int propertyType = newProperty (name);
					if (!_tbProperties)
					{
						// editor may be closed
						return;
					}

					if (propertyType != PROP_TYPE_NONE)
					{
						if (_propertyTypes.find (name) != _propertyTypes.end())
						{
							::MessageBoxA (ATOM_APP->getMainWindow(), "该属性已经存在", "错误", MB_OK|MB_ICONHAND);
						}
						else
						{
							_propertyTypes[name].type = propertyType;

							switch (propertyType)
							{
							case PROP_TYPE_INT:
								_propertyTypes[name].value.setI (0);
								break;
							case PROP_TYPE_FLOAT:
								_propertyTypes[name].value.setF (0.f);
								break;
							case PROP_TYPE_STRING:
								_propertyTypes[name].value.setS ("");
								break;
							case PROP_TYPE_VECTOR:
								_propertyTypes[name].value.setV (ATOM_Vector4f(0.f, 0.f, 0.f, 0.f));
								break;
							case PROP_TYPE_COLOR4F:
								_propertyTypes[name].value.setV (ATOM_Vector4f(1.f, 1.f, 1.f, 1.f));
								break;
							case PROP_TYPE_SWITCH:
								_propertyTypes[name].value.setI (1);
								break;
							}

							ATOM_HASHMAP<ATOM_STRING, PropertyInfo>::const_iterator it = _propertyTypes.find(name);
							if (it != _propertyTypes.end ())
							{
								setupProperty (_propertyTypes.find(name)->first.c_str());
							}
						}
					}
					break;
				}
			}
		}
		else if (e->bar == _tbPartCandidates)
		{
			switch (e->id)
			{
			case ID_CANDIDATE_BAR_NEW:
				{
					_currentPart->addCandidate ("");
					setupCandidatesBar (_currentPart);
					break;
				}
			case ID_CANDIDATE_BAR_DELETE:
				{
					ATOM_ActorPart::CandidateId Id = (ATOM_ActorPart::CandidateId)e->userdata;
					if (_currentPart->removeCandidate (Id))
					{
						setupCandidatesBar (_currentPart);
					}
					break;
				}
			case ID_CANDIDATE_BAR_AFFECT_PARTS:
				{
					editAffectParts ();
					break;
				}
			case ID_CANDIDATE_BAR_TEXTURE_ALBEDO:
				{
					unsigned i = _editor->getOpenImageFileNames (false, false);
					if ( i == 1)
					{
						const char *filename = _editor->getOpenedImageFileName (0);
						ATOM_ActorPart::CandidateId Id = (ATOM_ActorPart::CandidateId)e->userdata;
						const char *oldDesc = _currentPart->getCandidateDesc (Id);
						const char *sep = strchr (oldDesc, '|');
						if (sep)
						{
							char buffer[1024];
							sprintf (buffer, "%s|%s", filename, sep + 1);
							_currentPart->setCandidateDesc (Id, buffer);
						}
						else
						{
							_currentPart->setCandidateDesc (Id, filename);
						}
						const char *s = strrchr (filename, '/');
						_tbPartCandidates->setVarLabel (e->name.c_str(), s + 1);
					}
					break;
				}
			case ID_CANDIDATE_BAR_TEXTURE_NORMAL:
				{
					unsigned i = _editor->getOpenImageFileNames (false, false);
					if ( i == 1)
					{
						const char *filename = _editor->getOpenedImageFileName (0);
						ATOM_ActorPart::CandidateId Id = (ATOM_ActorPart::CandidateId)e->userdata;
						const char *oldDesc = _currentPart->getCandidateDesc (Id);
						const char *sep = strchr (oldDesc, '|');
						char buffer[1024];
						if (sep)
						{
							strncpy (buffer, oldDesc, sep - oldDesc + 1);
							strcat (buffer, filename);
							_currentPart->setCandidateDesc (Id, buffer);
						}
						else
						{
							strcpy (buffer, oldDesc);
							strcat (buffer, "|");
							strcat (buffer, filename);
							_currentPart->setCandidateDesc (Id, buffer);
						}
						const char *s = strrchr (filename, '/');
						_tbPartCandidates->setVarLabel (e->name.c_str(), s + 1);
					}
					break;
				}
			case ID_CANDIDATE_BAR_COMP_FILENAME:
				{
					const char *filter = "ATOM3D 模型(*.nm)|*.nm|ATOM3D 模型(*.nm2)|*.nm2|";
					ATOM_FileDlg dlg(0, 0, NULL, NULL, filter, ATOM_APP->getMainWindow());
					if (dlg.doModal () == IDOK)
					{
						ATOM_ActorPart::CandidateId Id = (ATOM_ActorPart::CandidateId)e->userdata;
						_currentPart->setCandidateDesc (Id, dlg.getSelectedFileName(0));
						const char *s = strrchr(dlg.getSelectedFileName(0), '/');
						if (s)
						{
							_tbPartCandidates->setVarLabel (e->name.c_str(), s + 1);
						}
					}
					break;
				}
			case ID_CANDIDATE_BAR_BINDING_FILENAME:
				{
					const char *filter = "所有文件(*.*)|*.*|";
					ATOM_FileDlg dlg(0, 0, NULL, NULL, filter, ATOM_APP->getMainWindow());
					if (dlg.doModal () == IDOK)
					{
						ATOM_ActorPart::CandidateId Id = (ATOM_ActorPart::CandidateId)e->userdata;
						ATOM_ActorBindingPart *bindingPart = dynamic_cast<ATOM_ActorBindingPart*>(_currentPart);
						ATOM_ActorBindingPart::BindingInfo info;
						bindingPart->getBindingInfoById (Id, info);
						info.nodeFileName = dlg.getSelectedFileName (0);
						char newDesc[2048];
						sprintf (newDesc, "%s|%s|%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", info.nodeFileName.c_str(), info.boneName.c_str(),
							info.bindMatrix.m00, info.bindMatrix.m01, info.bindMatrix.m02, info.bindMatrix.m03,
							info.bindMatrix.m10, info.bindMatrix.m11, info.bindMatrix.m12, info.bindMatrix.m13,
							info.bindMatrix.m20, info.bindMatrix.m21, info.bindMatrix.m22, info.bindMatrix.m23,
							info.bindMatrix.m30, info.bindMatrix.m31, info.bindMatrix.m32, info.bindMatrix.m33);
						_currentPart->setCandidateDesc (Id, newDesc);
						const char *s = strrchr(dlg.getSelectedFileName(0), '/');
						if (s)
						{
							_tbPartCandidates->setVarLabel (e->name.c_str(), s + 1);
						}
					}
					break;
				}
			}
		}
	}
	else if (eventId == ATOMX_TWValueChangedEvent::eventTypeId())
	{
		ATOMX_TWValueChangedEvent *e = (ATOMX_TWValueChangedEvent*)event;
		if (e->bar == _tbProperties)
		{
			switch (e->id)
			{
			case ID_PROPERTY_VALUE:
				{
					const char *name = (const char *)e->userdata;
					PropertyInfo &info = _propertyTypes[name];
					switch (info.type)
					{
					case PROP_TYPE_INT:
						{
							info.value.setI (e->newValue.getI());
							break;
						}
					case PROP_TYPE_FLOAT:
						{
							info.value.setF (e->newValue.getF());
							break;
						}
					case PROP_TYPE_STRING:
						{
							info.value.setS (e->newValue.getS());
							break;
						}
					case PROP_TYPE_VECTOR:
					case PROP_TYPE_COLOR4F:
						{
							const float *v = e->newValue.get4F ();
							info.value.setV (ATOM_Vector4f(v[0], v[1], v[2], v[3]));
							break;
						}
					case PROP_TYPE_SWITCH:
						{
							info.value.setI (e->newValue.getB() ? 1 : 0);
							break;
						}
					}
				}
			default:
				{
					break;
				}
			}
		}
		else if (e->bar == _tbPartCandidates)
		{
			switch (e->id)
			{
			case ID_CANDIDATE_BAR_CURRENT_ID:
				{
					if (!_currentPart->setCurrentCandidate (e->newValue.getI()))
					{
						_tbPartCandidates->setI (e->name.c_str(), e->oldValue.getI());
					}
					break;
				}
			case ID_CANDIDATE_BAR_ID:
				{
					ATOM_ActorPart::CandidateId id = (ATOM_ActorPart::CandidateId)e->userdata;
					int index = _currentPart->getCandidateIndexById (id);
					if (index < 0 || !_currentPart->setCandidateId (index, e->newValue.getI()))
					{
						_tbPartCandidates->setI (e->name.c_str(), e->oldValue.getI());
					}
					else
					{
						_tbPartCandidates->setVarUserData (e->name.c_str(), (void*)e->newValue.getI());
						_tbPartCandidates->setI ("Current", _currentPart->getCurrentCandidate());
					}
					break;
				}
			case ID_CANDIDATE_BAR_COLOR_COLOR:
				{
					ATOM_ActorPart::CandidateId Id = (ATOM_ActorPart::CandidateId)e->userdata;
					char desc[256];
					const float *c = e->newValue.get3F();
					int r = (int(c[0] * 255)) % 256;
					int g = (int(c[1] * 255)) % 256;
					int b = (int(c[2] * 255)) % 256;
					int a = 255;
					sprintf (desc, "%d,%d,%d,%d", r, g, b, a);
					_currentPart->setCandidateDesc (Id, desc);
					break;
				}
			case ID_CANDIDATE_BAR_BINDING_BONE_NAME:
				{
					ATOM_ActorPart::CandidateId Id = (ATOM_ActorPart::CandidateId)e->userdata;
					ATOM_ActorBindingPart *bindingPart = dynamic_cast<ATOM_ActorBindingPart*>(_currentPart);
					ATOM_ActorBindingPart::BindingInfo info;
					bindingPart->getBindingInfoById (Id, info);
					info.boneName = e->newValue.getS();
					char newDesc[2048];
					sprintf (newDesc, "%s|%s|%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", info.nodeFileName.c_str(), info.boneName.c_str(),
						info.bindMatrix.m00, info.bindMatrix.m01, info.bindMatrix.m02, info.bindMatrix.m03,
						info.bindMatrix.m10, info.bindMatrix.m11, info.bindMatrix.m12, info.bindMatrix.m13,
						info.bindMatrix.m20, info.bindMatrix.m21, info.bindMatrix.m22, info.bindMatrix.m23,
						info.bindMatrix.m30, info.bindMatrix.m31, info.bindMatrix.m32, info.bindMatrix.m33);
					_currentPart->setCandidateDesc (Id, newDesc);
					break;
				}
			case ID_CANDIDATE_BAR_SCALE:
				{
					ATOM_ActorPart::CandidateId Id = (ATOM_ActorPart::CandidateId)e->userdata;
					float scale = e->newValue.getF();
					ATOM_Matrix4x4f scaleMatrix = ATOM_Matrix4x4f::getScaleMatrix (ATOM_Vector3f(scale, scale, scale));
					char newDesc[2048];
					sprintf (newDesc, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
						scaleMatrix.m00, scaleMatrix.m01, scaleMatrix.m02, scaleMatrix.m03,
						scaleMatrix.m10, scaleMatrix.m11, scaleMatrix.m12, scaleMatrix.m13,
						scaleMatrix.m20, scaleMatrix.m21, scaleMatrix.m22, scaleMatrix.m23,
						scaleMatrix.m30, scaleMatrix.m31, scaleMatrix.m32, scaleMatrix.m33);
					_currentPart->setCandidateDesc (Id, newDesc);
					break;
				}
			}
		}
	}
	else if (eventId == ATOM_WidgetResizeEvent::eventTypeId())
	{
		calcUILayout ();
	}
	else if (eventId == ATOM_ListBoxClickEvent::eventTypeId())
	{
		ATOM_ListBoxClickEvent *e = (ATOM_ListBoxClickEvent*)event;
		if (e->id == ID_PART_LISTBOX)
		{
			if (e->type == LBCLICK_LEFT)
			{
				if (e->index < 0)
				{
					_currentPart = 0;
					_tbPartCandidates->clear ();
				}
				else
				{
					const char *name = _partListBox->getItemText(e->index)->getString ();
					_currentPart = _actor->getPartDefine().getPartByName (name);
					if (_currentPart)
					{
						setupCandidatesBar (_currentPart);
					}
					else
					{
						_tbPartCandidates->clear ();
					}
				}
			}
		}
	}
	else if (eventId == ATOM_WidgetContextMenuEvent::eventTypeId ())
	{
		ATOM_WidgetContextMenuEvent *e = (ATOM_WidgetContextMenuEvent*)event;
		if (e->id == ID_POINTLIST)
		{
			_pointList->trackPopupMenu (e->x, e->y, _pointListMenu);
		}
	}
}

bool PluginActor::saveFile (const char *filename)
{
	char nativeFileName[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (filename, nativeFileName);
	ATOM_TiXmlDocument doc (nativeFileName);

	// sync actor properties
	_actor->removeAllProperties ();
	for (ATOM_HASHMAP<ATOM_STRING, PropertyInfo>::const_iterator it = _propertyTypes.begin(); it != _propertyTypes.end(); ++it)
	{
		_actor->setProperty (it->first.c_str(), it->second.value);
	}

	_actor->saveToXML (doc);

	// Save Editor-Specific contents
	ATOM_TiXmlElement eEditorSpecific("EditorSpec");
	savePropertyTypes (eEditorSpecific);
	doc.InsertEndChild (eEditorSpecific);

	return doc.SaveFile();
}

void PluginActor::savePropertyTypes (ATOM_TiXmlElement &xmlElement) const
{
	for (ATOM_HASHMAP<ATOM_STRING, PropertyInfo>::const_iterator it = _propertyTypes.begin(); it != _propertyTypes.end(); ++it)
	{
		ATOM_TiXmlElement e("Property");
		e.SetAttribute ("Name", it->first.c_str());
		e.SetAttribute ("Type", it->second.type);
		xmlElement.InsertEndChild (e);
	}
}

ATOM_STRING PluginActor::genPartName (void) const
{
	int id = 1;
	char buffer[256];

	for (;;)
	{
		sprintf (buffer, "component%d", id++);
		if (!_actor->getPartDefine().getPartByName(buffer))
		{
			return buffer;
		}
	}
}

bool PluginActor::importCP (const char *filename)
{
	ATOM_AutoFile fCP(filename, ATOM_VFS::read|ATOM_VFS::text);
	if (!fCP)
	{
		return false;
	}
	unsigned size = fCP->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = fCP->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument document;
	document.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (document.Error())
	{
		return false;
	}

	ATOM_TiXmlElement *root = document.RootElement ();
	if (!root)
	{
		return false;
	}

	const char *coreFileName = root->Attribute ("CoreFile");
	if (!coreFileName)
	{
		return false;
	}

	ATOM_AutoFile fCCP(coreFileName, ATOM_VFS::read|ATOM_VFS::text);
	if (!fCCP)
	{
		return false;
	}
	size = fCCP->size ();
	p = ATOM_NEW_ARRAY(char, size);
	size = fCCP->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument documentCCP;
	documentCCP.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (documentCCP.Error())
	{
		return false;
	}

	ATOM_TiXmlElement *rootCCP = documentCCP.RootElement ();
	if (!rootCCP)
	{
		return false;
	}

	_actor->getInternalGeode()->stopAction();
	_actor->getPartDefine ().removeAllParts ();
	_partListBox->selectItem (-1);
	_partListBox->clearItem ();
	_tbPartCandidates->clear ();

	const char *skeletonFileName = rootCCP->Attribute ("skeleton");
	if (skeletonFileName)
	{
		ATOM_ActorComponentsPart *part = _actor->getPartDefine ().newComponentsPart ("skeleton");
		part->addCandidate (1, skeletonFileName);
		part->setCurrentCandidate (1);
		_partListBox->addItem (part->getName());
	}

	for (ATOM_TiXmlElement *eAction = rootCCP->FirstChildElement("action"); eAction; eAction = eAction->NextSiblingElement("action"))
	{
		const char *filename = eAction->Attribute ("file");
		if (filename)
		{
			const char *p1 = strrchr (filename, '/');
			const char *p2 = strrchr (filename, '.');
			if (p1 && p2)
			{
				ATOM_STRING s(p1+1, p2-p1-1);
				ATOM_ActorComponentsPart *part = _actor->getPartDefine ().newComponentsPart (s.c_str());
				part->addCandidate (1, filename);
				part->setCurrentCandidate (1);
				_partListBox->addItem (part->getName());
			}
		}
	}

	for (ATOM_TiXmlElement *eComponent = rootCCP->FirstChildElement("component"); eComponent; eComponent = eComponent->NextSiblingElement("component"))
	{
		const char *name = eComponent->Attribute ("name");
		ATOM_STRING s;
		if (!name || _actor->getPartDefine().getPartByName (name))
		{
			s = genPartName ();
			name = s.c_str();
		}
		ATOM_ActorComponentsPart *part = _actor->getPartDefine ().newComponentsPart (name);
		_partListBox->addItem (part->getName());

		for (ATOM_TiXmlElement *eMesh = eComponent->FirstChildElement("mesh"); eMesh; eMesh = eMesh->NextSiblingElement("mesh"))
		{
			int id = -1;
			eMesh->QueryIntAttribute ("id", &id);
			if (id < 0)
			{
				return false;
			}
			if (part->getCandidateIndexById (id) >= 0)
			{
				return false;
			}

			const char *filename = eMesh->Attribute ("file");
			if (filename)
			{
				part->addCandidate (id, filename);
			}
		}

		if (part->getNumCandidates() > 0)
		{
			part->setCurrentCandidate (part->getCandidateId (0));
		}
	}

	_points.clear ();
	for (ATOM_TiXmlElement *ePoint = rootCCP->FirstChildElement("point"); ePoint; ePoint = ePoint->NextSiblingElement("point"))
	{
		const char *name = ePoint->Attribute("Name");
		if (!name)
		{
			continue;
		}

		const char *mtx = ePoint->Attribute("Matrix");
		if (!mtx)
		{
			continue;
		}

		ATOM_Matrix4x4f matrix;
		if (16 != sscanf(mtx, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", 
			&matrix.m00, &matrix.m01, &matrix.m02, &matrix.m03,
			&matrix.m10, &matrix.m11, &matrix.m12, &matrix.m13,
			&matrix.m20, &matrix.m21, &matrix.m22, &matrix.m23,
			&matrix.m30, &matrix.m31, &matrix.m32, &matrix.m33))
		{
			continue;
		}

		ATOM_HARDREF(ATOM_Node) node;
		node->setO2T (matrix);
		_actor->appendChild (node.get());

		ATOM_HARDREF(ATOM_ShapeNode) shapeNode;
		shapeNode->loadAttribute (NULL);
		shapeNode->load (ATOM_GetRenderDevice());
		shapeNode->setType (ATOM_ShapeNode::SPHERE);
		shapeNode->setColor (ATOM_Vector4f(ATOM_randomf(0.f, 1.f), ATOM_randomf(0.f, 1.f), ATOM_randomf(0.f, 1.f), 1.f));
		shapeNode->setO2T (ATOM_Matrix4x4f::getScaleMatrix(ATOM_Vector3f(0.2f, 0.2f, 0.2f)));
		node->appendChild (shapeNode.get());

		_points[name] = node;
	}

	return true;
}

bool PluginActor::exportCP (const char *filename)
{
	static const int FLAG_BONE = 1;
	static const int FLAG_MESH = 2;
	static const int FLAG_ACTION = 3;

	char errmsg[512];

	static const char *cpTemplate = "<node class=\"ClientCharacter\" CoreFile=\"%s\"/>\n";
	char buffer[1024];
	char ccpFileName[256];
	strcpy (ccpFileName, filename);
	char *p = strrchr (ccpFileName, '.');
	strcpy (p, ".ccp");

	ATOM_GetNativePathName(ccpFileName, buffer);
	ATOM_TiXmlDocument doc(buffer);

	ATOM_TiXmlElement root("root");

	for (int i = 0; i < _actor->getPartDefine().getNumParts(); ++i)
	{
		ATOM_ActorComponentsPart *part = dynamic_cast<ATOM_ActorComponentsPart*>(_actor->getPartDefine().getPart(i));
		int partFlag = 0;
		ATOM_TiXmlElement eMeshes("component");

		for (int j = 0; j < part->getNumCandidates(); ++j)
		{
			ATOM_ActorPart::CandidateId id = part->getCandidateId(j);
			if (!part->setCurrentCandidate(id) || id != part->getCurrentCandidate())
			{
				sprintf (errmsg, "部件加载错误:\n%s", part->getCandidateDesc(id));
				::MessageBoxA (ATOM_APP->getMainWindow(), errmsg, "错误", MB_OK|MB_ICONHAND);
				return false;
			}

			ATOM_Components comp = part->getCurrentComponents();
			const char *filename = part->getCandidateDesc (part->getCurrentCandidate());

			if (comp.getSkeleton())
			{
				if (partFlag != 0)
				{
					sprintf (errmsg, "在部件<%s>中发现非预期的骨骼部件!", part->getName());
					::MessageBoxA (ATOM_APP->getMainWindow(), errmsg, "错误！", MB_OK|MB_ICONHAND);
					return false;
				}
				partFlag = FLAG_BONE;

				if (comp.getNumMeshes() || comp.getNumTracks())
				{
					::MessageBoxA (ATOM_APP->getMainWindow(), "不支持导出复合模型！", "错误", MB_OK|MB_ICONHAND);
					return false;
				}
				if (root.Attribute ("skeleton"))
				{
					::MessageBoxA (ATOM_APP->getMainWindow(), "骨骼最多只能是一个!", "错误", MB_OK|MB_ICONHAND);
					return false;
				}
				root.SetAttribute ("skeleton", filename);
			}
			else if (comp.getNumTracks())
			{
				if (partFlag != 0)
				{
					sprintf (errmsg, "在部件<%s>中发现非预期的动作部件!", part->getName());
					::MessageBoxA (ATOM_APP->getMainWindow(), errmsg, "错误！", MB_OK|MB_ICONHAND);
					return false;
				}
				partFlag = FLAG_ACTION;

				// 动作
				ATOM_TiXmlElement eAction("action");
				eAction.SetAttribute ("file", filename);
				root.InsertEndChild (eAction);
			}
			else
			{
				if (partFlag != 0 && partFlag != FLAG_MESH)
				{
					sprintf (errmsg, "在部件<%s>中发现非预期的网格部件!", part->getName());
					::MessageBoxA (ATOM_APP->getMainWindow(), errmsg, "错误！", MB_OK|MB_ICONHAND);
					return false;
				}

				if (partFlag == 0)
				{
					eMeshes.SetAttribute("name", part->getName());
				}
				partFlag = FLAG_MESH;

				ATOM_TiXmlElement eMesh("mesh");
				eMesh.SetAttribute ("id", part->getCandidateId(j));
				eMesh.SetAttribute ("file", filename);
				eMeshes.InsertEndChild (eMesh);
			}
		}

		if (partFlag == FLAG_MESH)
		{
			root.InsertEndChild (eMeshes);
		}
	}

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
		root.InsertEndChild (ePoint);
	}

	doc.InsertEndChild (root);

	if (!doc.SaveFile ())
	{
		::MessageBoxA (ATOM_APP->getMainWindow(), "文件保存失败!", "错误！", MB_OK|MB_ICONHAND);
		return false;
	}

	{
		sprintf (buffer, cpTemplate, ccpFileName);
		ATOM_AutoFile fCP (filename, ATOM_VFS::write|ATOM_VFS::text);
		if (!fCP)
		{
			::MessageBoxA (ATOM_APP->getMainWindow(), "文件保存失败!", "错误！", MB_OK|MB_ICONHAND);
			return false;
		}
		fCP->write (buffer, strlen(buffer));
	}

	return true;
}

bool PluginActor::loadPropertyTypes (ATOM_TiXmlElement &xmlElement)
{
	_propertyTypes.clear ();

	ATOM_TiXmlElement *e = xmlElement.FirstChildElement ("Property");
	while (e)
	{
		const char *name = e->Attribute ("Name");
		if (!name)
		{
			::MessageBoxA (ATOM_APP->getMainWindow(), "发现属性匹配错误，可能角色文件被手工修改过", "警告", MB_OK|MB_ICONWARNING);
		}
		else
		{
			const ATOM_Variant &val = _actor->getProperty (name);

			switch (val.getType())
			{
			case ATOM_Variant::NONE:
				::MessageBoxA (ATOM_APP->getMainWindow(), "发现属性匹配错误，可能角色文件被手工修改过", "警告", MB_OK|MB_ICONWARNING);
				break;
			case ATOM_Variant::INT:
				{
					int type = PROP_TYPE_INT;
					if (e->QueryIntAttribute ("Type", &type) == ATOM_TIXML_SUCCESS)
					{
						if (type != PROP_TYPE_SWITCH)
						{
							type = PROP_TYPE_INT;
						}
					}
					_propertyTypes[name].value = val;
					_propertyTypes[name].type = type;
					break;
				}
			case ATOM_Variant::FLOAT:
				{
					_propertyTypes[name].value = val;
					_propertyTypes[name].type = PROP_TYPE_FLOAT;
					break;
				}
			case ATOM_Variant::VECTOR4:
				{
					int type = PROP_TYPE_VECTOR;
					if (e->QueryIntAttribute ("Type", &type) == ATOM_TIXML_SUCCESS)
					{
						if (type != PROP_TYPE_COLOR4F)
						{
							type = PROP_TYPE_VECTOR;
						}
					}
					_propertyTypes[name].value = val;
					_propertyTypes[name].type = type;
					break;
				}
			case ATOM_Variant::STRING:
				{
					_propertyTypes[name].value = val;
					_propertyTypes[name].type = PROP_TYPE_STRING;
					break;
				}
			}
		}
		e = e->NextSiblingElement ("Property");
	}

	for (unsigned i = 0; i < _actor->getNumProperties(); ++i)
	{
		const char *name = _actor->getPropertyName (i);

		if (_propertyTypes.find (name) == _propertyTypes.end ())
		{
			::MessageBoxA (ATOM_APP->getMainWindow(), "发现属性匹配错误，可能角色文件被手工修改过", "警告", MB_OK|MB_ICONWARNING);

			const ATOM_Variant &v = _actor->getProperty (name);
			switch (v.getType())
			{
			case ATOM_Variant::INT:
				_propertyTypes[name].value = v;
				_propertyTypes[name].type = PROP_TYPE_INT;
				break;
			case ATOM_Variant::FLOAT:
				_propertyTypes[name].value = v;
				_propertyTypes[name].type = PROP_TYPE_FLOAT;
				break;
			case ATOM_Variant::STRING:
				_propertyTypes[name].value = v;
				_propertyTypes[name].type = PROP_TYPE_STRING;
				break;
			case ATOM_Variant::VECTOR4:
				_propertyTypes[name].value = v;
				_propertyTypes[name].type = PROP_TYPE_VECTOR;
				break;
			default:
				::MessageBoxA (ATOM_APP->getMainWindow(), "发现无法识别的属性类型", "错误", MB_OK|MB_ICONHAND);
				return false;
			}
		}
	}
	return true;
}

unsigned PluginActor::getMinWindowWidth (void) const
{
	return 600;
}

unsigned PluginActor::getMinWindowHeight (void) const
{
	return 450;
}

bool PluginActor::isDocumentModified (void)
{
	return _editor->isDocumentModified ();
}

void PluginActor::setupMenu (void)
{
	ATOM_MenuBar *menubar = _editor->getMenuBar ();

	ATOM_PopupMenu *menuFile = menubar->getMenuItem (AS_MENUITEM_FILE).submenu;
	menuFile->insertMenuItem (menuFile->getNumItems()-1, "导入CP..", ID_IMPORT);
	menuFile->insertMenuItem (menuFile->getNumItems()-1, "导出CP..", ID_EXPORT);

	menubar->insertMenuItem (AS_MENUITEM_CUSTOM, "部件", MENU_ITEM_ID_PART);
	ATOM_PopupMenu *menuPart = menubar->createSubMenu (AS_MENUITEM_CUSTOM);
	menuPart->appendMenuItem ("创建(Ctrl+Insert)..", ID_NEW_PART);
	menuPart->appendMenuItem ("删除(Ctrl+Delete)..", ID_DEL_PART);

	menubar->insertMenuItem (AS_MENUITEM_CUSTOM + 1, "上身动作", MENU_ITEM_ID_ACTIONU);
	_actionMenuUp = menubar->createSubMenu (AS_MENUITEM_CUSTOM + 1);

	menubar->insertMenuItem (AS_MENUITEM_CUSTOM + 2, "下身动作", MENU_ITEM_ID_ACTIOND);
	_actionMenuDown = menubar->createSubMenu (AS_MENUITEM_CUSTOM + 2);

	menubar->insertMenuItem (AS_MENUITEM_CUSTOM + 3, "查看骨骼", MENU_ITEM_ID_VIEWJOINTS);
	_jointMenu = menubar->createSubMenu (AS_MENUITEM_CUSTOM + 3);

	menubar->insertMenuItem (AS_MENUITEM_CUSTOM + 4, "观察模式", MENU_ITEM_ID_VIEWMODE);
	ATOM_PopupMenu *menuViewMode = menubar->createSubMenu (AS_MENUITEM_CUSTOM + 4);
	menuViewMode->appendMenuItem ("模型", ID_VIEWMODE_MODEL);
	menuViewMode->appendMenuItem ("骨骼", ID_VIEWMODE_SKELETON);

	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_INSERT, KEYMOD_CTRL, ID_NEW_PART));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_DELETE, KEYMOD_CTRL, ID_DEL_PART));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_ESCAPE, 0, ID_EDIT_POINT_ENDEDIT));
}

void PluginActor::cleanupMenu (void)
{
	ATOM_MenuBar *menubar = _editor->getMenuBar ();

	ATOM_PopupMenu *menuFile = menubar->getMenuItem (AS_MENUITEM_FILE).submenu;
	menuFile->removeMenuItem (menuFile->getNumItems()-2);
	menuFile->removeMenuItem (menuFile->getNumItems()-2);

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

void PluginActor::createCandidatesBar (void)
{
	_tbPartCandidates = ATOM_NEW(ATOMX_TweakBar, "Actor Part Define");
	_tbPartCandidates->setBarColor (0.5f, 0.5f, 0.5f, 1.f);
	_tbPartCandidates->setBarMovable (false);
	_tbPartCandidates->setBarIconifiable (false);
	_tbPartCandidates->setBarResizable (false);
	_tbPartCandidates->setBarFontResizable (false);
}

void PluginActor::createPropertyBar (void)
{
	_tbProperties = ATOM_NEW(ATOMX_TweakBar, "Actor Properties");
	_tbProperties->setBarColor (0.5f, 0.5f, 0.5f, 1.f);
	_tbProperties->setBarMovable (false);
	_tbProperties->setBarIconifiable (false);
	_tbProperties->setBarResizable (false);
	_tbProperties->setBarFontResizable (false);
}

void PluginActor::setupPropertyBar (void)
{
	_tbProperties->clear ();

	_tbProperties->addButton ("NewProp", ID_NEW_PROPERTY, "New property..", 0);
	_tbProperties->addButton ("ClearProp", ID_CLEAR_PROPERTIES, "Remove All", 0);

	for (ATOM_HASHMAP<ATOM_STRING, PropertyInfo>::const_iterator it = _propertyTypes.begin(); it != _propertyTypes.end(); ++it)
	{
		setupProperty (it->first.c_str());
	}
}

void PluginActor::setupProperty (const char *name)
{
	const PropertyInfo &info = _propertyTypes[name];
	switch (info.type)
	{
	case PROP_TYPE_INT:
		setupIntProperty (name, info.value);
		break;
	case PROP_TYPE_FLOAT:
		setupFloatProperty (name, info.value);
		break;
	case PROP_TYPE_STRING:
		setupStringProperty (name, info.value);
		break;
	case PROP_TYPE_VECTOR:
		setupVectorProperty (name, info.value);
		break;
	case PROP_TYPE_COLOR4F:
		setupColorProperty (name, info.value);
		break;
	case PROP_TYPE_SWITCH:
		setupSwitchProperty (name, info.value);
		break;
	default:
		return;
	}
	char buttonDelete[256];
	sprintf (buttonDelete, "PropDel_%s", name);
	_tbProperties->addButton (buttonDelete, ID_PROPERTY_DELETE, "Delete", name, (void*)name);
}

void PluginActor::setupIntProperty (const char *name, const ATOM_Variant &value)
{
	char ValueName[256];
	sprintf (ValueName, "PropValue_%s", name);

	ATOMX_TBValue val;
	val.setI (ATOMX_TBTYPE_INT32, value.getI());

	_tbProperties->addVariable (ValueName, ID_PROPERTY_VALUE, val, false, name, (void*)name);
	_tbProperties->setVarLabel (ValueName, "Value");
}

void PluginActor::setupFloatProperty (const char *name, const ATOM_Variant &value)
{
	char ValueName[256];
	sprintf (ValueName, "PropValue_%s", name);

	ATOMX_TBValue val;
	val.setF (ATOMX_TBTYPE_FLOAT, value.getF());

	_tbProperties->addVariable (ValueName, ID_PROPERTY_VALUE, val, false, name, (void*)name);
	_tbProperties->setVarLabel (ValueName, "Value");
	_tbProperties->setVarStep (ValueName, 0.001f);
}

void PluginActor::setupStringProperty (const char *name, const ATOM_Variant &value)
{
	char ValueName[256];
	sprintf (ValueName, "PropValue_%s", name);

	ATOMX_TBValue val;
	val.setS (ATOMX_TBTYPE_STRING, value.getS());

	_tbProperties->addVariable (ValueName, ID_PROPERTY_VALUE, val, false, name, (void*)name);
	_tbProperties->setVarLabel (ValueName, "Value");
}

void PluginActor::setupVectorProperty (const char *name, const ATOM_Variant &value)
{
	char ValueName[256];
	sprintf (ValueName, "PropValue_%s", name);

	ATOMX_TBValue val;
	const float *f = value.getV ();
	val.set4F (ATOMX_TBTYPE_VECTOR4F, f[0], f[1], f[2], f[3]);

	_tbProperties->addVariable (ValueName, ID_PROPERTY_VALUE, val, false, name, (void*)name);
	_tbProperties->setVarLabel (ValueName, "Value");
}

void PluginActor::setupColorProperty (const char *name, const ATOM_Variant &value)
{
	char ValueName[256];
	sprintf (ValueName, "PropValue_%s", name);

	ATOMX_TBValue val;
	const float *v = value.getV ();
	val.set4F (ATOMX_TBTYPE_COLOR4F, v[0], v[1], v[2], v[3]);

	_tbProperties->addVariable (ValueName, ID_PROPERTY_VALUE, val, false, name, (void*)name);
	_tbProperties->setVarLabel (ValueName, "Value");
}

void PluginActor::setupSwitchProperty (const char *name, const ATOM_Variant &value)
{
	char ValueName[256];
	sprintf (ValueName, "PropValue_%s", name);

	ATOMX_TBValue val;
	val.setB (ATOMX_TBTYPE_BOOL, value.getI() != 0);

	_tbProperties->addVariable (ValueName, ID_PROPERTY_VALUE, val, false, name, (void*)name);
	_tbProperties->setVarLabel (ValueName, "Value");
}

void PluginActor::setupCandidatesBar (ATOM_ActorPart *part)
{
	_tbPartCandidates->clear ();

	ATOM_ActorPart::CandidateId id = part->getCurrentCandidate ();
	_tbPartCandidates->addIntVar ("Current", ID_CANDIDATE_BAR_CURRENT_ID, id, false, 0);
	_tbPartCandidates->setVarMinMax ("Current", -1, 100000);
	_tbPartCandidates->setVarStep ("Current", 1);

	if (dynamic_cast<ATOM_ActorModifierPart*>(part))
	{
		_tbPartCandidates->addButton ("AffectParts", ID_CANDIDATE_BAR_AFFECT_PARTS, "Parts", 0);
	}
	_tbPartCandidates->addButton ("NewCandidate", ID_CANDIDATE_BAR_NEW, "New Candidate", 0);

	ATOM_ActorComponentsPart *componentsPart = dynamic_cast<ATOM_ActorComponentsPart*>(part);
	if (componentsPart)
	{
		setupComponentsCandidateBar (componentsPart);
	}
	else
	{
		ATOM_ActorColorPart *colorPart = dynamic_cast<ATOM_ActorColorPart*>(part);
		if (colorPart)
		{
			setupColorCandidateBar (colorPart);
		}
		else
		{
			ATOM_ActorTexturePart *texturePart = dynamic_cast<ATOM_ActorTexturePart*>(part);
			if (texturePart)
			{
				setupTextureCandidateBar (texturePart);
			}
			else
			{
				ATOM_ActorBindingPart *bindingPart = dynamic_cast<ATOM_ActorBindingPart*>(part);
				if (bindingPart)
				{
					setupBindingCandidateBar (bindingPart);
				}
				else
				{
					ATOM_ActorTransformPart *transformPart = dynamic_cast<ATOM_ActorTransformPart*>(part);
					if (transformPart)
					{
						setupTransformCandidateBar (transformPart);
					}
				}
			}
		}
	}
}

void PluginActor::setupComponentsCandidateBar (ATOM_ActorComponentsPart *part)
{
	for (unsigned i = 0; i < part->getNumCandidates(); ++i)
	{
		ATOM_ActorPart::CandidateId Id = part->getCandidateId(i);

		char group[128];
		char desc[128];
		char id[128];
		char del[128];
		sprintf (group, "Candidate%d", i);
		sprintf (desc, "FileName%d", i);
		sprintf (id, "Id%d", i);
		sprintf (del, "Del%d", i);

		const char *filename = part->getCandidateDesc (Id);
		if (!filename || !filename[0])
		{
			filename = "<No FileName>";
		}
		else
		{
			filename = strrchr (filename, '/');
			if (!filename)
			{
				filename = "<No FileName>";
			}
			else
			{
				filename++;
			}
		}
		_tbPartCandidates->addButton (desc, ID_CANDIDATE_BAR_COMP_FILENAME, filename, group, (void*)Id);
		_tbPartCandidates->addIntVar (id, ID_CANDIDATE_BAR_ID, Id, false, group, (void*)Id);
		_tbPartCandidates->setVarLabel (id, "Id");
		_tbPartCandidates->setVarMinMax (id, 1, 100000);
		_tbPartCandidates->setVarStep (id, 1);
		_tbPartCandidates->addButton (del, ID_CANDIDATE_BAR_DELETE, "Delete", group, (void*)Id);
	}
}

void PluginActor::setupColorCandidateBar (ATOM_ActorColorPart *part)
{
	for (unsigned i = 0; i < part->getNumCandidates(); ++i)
	{
		ATOM_ActorPart::CandidateId Id = part->getCandidateId(i);

		char group[128];
		char desc[128];
		char id[128];
		char del[128];
		sprintf (group, "Candidate%d", i);
		sprintf (desc, "Color%d", i);
		sprintf (id, "Id%d", i);
		sprintf (del, "Del%d", i);

		ATOM_Vector4f color = part->getColorById (Id);
		_tbPartCandidates->addRGBVar (desc, ID_CANDIDATE_BAR_COLOR_COLOR, color.x, color.y, color.z, false, group, (void*)Id);
		_tbPartCandidates->setVarLabel (desc, "Color");
		_tbPartCandidates->addIntVar (id, ID_CANDIDATE_BAR_ID, Id, false, group, (void*)Id);
		_tbPartCandidates->setVarLabel (id, "Id");
		_tbPartCandidates->setVarMinMax (id, 1, 100000);
		_tbPartCandidates->setVarStep (id, 1);
		_tbPartCandidates->addButton (del, ID_CANDIDATE_BAR_DELETE, "Delete", group, (void*)Id);
	}
}

void PluginActor::setupTextureCandidateBar (ATOM_ActorTexturePart *part)
{
	for (unsigned i = 0; i < part->getNumCandidates(); ++i)
	{
		ATOM_ActorPart::CandidateId Id = part->getCandidateId(i);

		char group[128];
		char descAlbedo[128];
		char descNormalmap[128];
		char id[128];
		char del[128];
		sprintf (group, "Candidate%d", i);
		sprintf (descAlbedo, "Albedo%d", i);
		sprintf (descNormalmap, "Normal%d", i);
		sprintf (id, "Id%d", i);
		sprintf (del, "Del%d", i);

		ATOM_STRING albedo = part->getAlbedoFileNameById (Id);
		ATOM_STRING normalmap = part->getNormalMapFileNameById (Id);

		_tbPartCandidates->addButton (descAlbedo, ID_CANDIDATE_BAR_TEXTURE_ALBEDO, albedo.empty() ? "<No FileName>" : albedo.c_str(), group, (void*)Id);
		_tbPartCandidates->addButton (descNormalmap, ID_CANDIDATE_BAR_TEXTURE_NORMAL, normalmap.empty() ? "<No FileName>" : normalmap.c_str(), group, (void*)Id);
		_tbPartCandidates->addIntVar (id, ID_CANDIDATE_BAR_ID, Id, false, group, (void*)Id);
		_tbPartCandidates->setVarLabel (id, "Id");
		_tbPartCandidates->setVarMinMax (id, 1, 100000);
		_tbPartCandidates->setVarStep (id, 1);
		_tbPartCandidates->addButton (del, ID_CANDIDATE_BAR_DELETE, "Delete", group, (void*)Id);
	}
}

void PluginActor::setupTransformCandidateBar (ATOM_ActorTransformPart *part)
{
	for (unsigned i = 0; i < part->getNumCandidates(); ++i)
	{
		ATOM_ActorPart::CandidateId Id = part->getCandidateId(i);

		char group[128];
		char transform[128];
		char id[128];
		char del[128];
		sprintf (group, "Candidate%d", i);
		sprintf (transform, "Transform%d", i);
		sprintf (id, "Id%d", i);
		sprintf (del, "Del%d", i);

		float scale = part->getCurrentTransform ().m00;

		_tbPartCandidates->addFloatVar (transform, ID_CANDIDATE_BAR_SCALE, scale, false, group, (void*)Id);
		_tbPartCandidates->setVarLabel (transform, "Scale");
		_tbPartCandidates->setVarStep (transform, 0.0001f);
		_tbPartCandidates->addIntVar (id, ID_CANDIDATE_BAR_ID, Id, false, group, (void*)Id);
		_tbPartCandidates->setVarLabel (id, "Id");
		_tbPartCandidates->setVarMinMax (id, 1, 100000);
		_tbPartCandidates->setVarStep (id, 1);
		_tbPartCandidates->addButton (del, ID_CANDIDATE_BAR_DELETE, "Delete", group, (void*)Id);
	}
}

void PluginActor::setupBindingCandidateBar (ATOM_ActorBindingPart *part)
{
	for (unsigned i = 0; i < part->getNumCandidates(); ++i)
	{
		ATOM_ActorPart::CandidateId Id = part->getCandidateId(i);

		char group[128];
		char filename[128];
		char transform[128];
		char bone[128];
		char id[128];
		char del[128];
		sprintf (group, "Candidate%d", i);
		sprintf (filename, "FileName%d", i);
		sprintf (transform, "Transform%d", i);
		sprintf (bone, "Bone%d", i);
		sprintf (id, "Id%d", i);
		sprintf (del, "Del%d", i);

		ATOM_ActorBindingPart::BindingInfo info;
		part->getBindingInfoById (Id, info);

		const char *p = strrchr(info.nodeFileName.c_str(), '/');
		const char *fn = p ? p + 1 : "<No FileName>";
		const char *bn = info.boneName.empty () ? "<No Bone>" : info.boneName.c_str();

		_tbPartCandidates->addButton (filename, ID_CANDIDATE_BAR_BINDING_FILENAME, fn, group, (void*)Id);
		_tbPartCandidates->addStringVar (bone, ID_CANDIDATE_BAR_BINDING_BONE_NAME, bn, false, group, (void*)Id);
		//_tbPartCandidates->addButton (transform, ID_CANDIDATE_BAR_BINDING_TRANSFORM, "Transform", group, (void*)Id);
		_tbPartCandidates->addIntVar (id, ID_CANDIDATE_BAR_ID, Id, false, group, (void*)Id);
		_tbPartCandidates->setVarLabel (id, "Id");
		_tbPartCandidates->setVarMinMax (id, 1, 100000);
		_tbPartCandidates->setVarStep (id, 1);
		_tbPartCandidates->addButton (del, ID_CANDIDATE_BAR_DELETE, "Delete", group, (void*)Id);
	}
}

void PluginActor::clear (void)
{
	_editor->getRealtimeCtrl()->setScene (0);

	ATOM_DELETE(_tbPartCandidates);
	_tbPartCandidates = 0;

	ATOM_DELETE(_tbProperties);
	_tbProperties = 0;

	ATOM_DELETE(_partListBox);
	_partListBox = 0;

	if (_pointList)
	{
		_pointList->setEventTrigger (0);
		ATOM_DELETE(_pointList);
		_pointList = 0;

		ATOM_DELETE(_pointListMenu);
		_pointListMenu = 0;
	}

	ATOM_DELETE(_scene);
	_scene = 0;

	_grid = 0;
	_actor = 0;
	_currentPart = 0;
	_editingPoint = false;
}

void PluginActor::createPartListBox (void)
{
	ATOM_Rect2Di rc = _editor->getRealtimeCtrl()->getClientRect();
	rc.size.w = 200;

	_partListBox = ATOM_NEW(ATOM_ListBox, _editor->getRealtimeCtrl(), rc, 16, ATOM_Widget::VScroll|ATOM_Widget::Border, ID_PART_LISTBOX, ATOM_Widget::ShowNormal);
	_partListBox->setBorderMode (ATOM_Widget::Drop);
	_partListBox->setFont (ATOM_GUIFont::getDefaultFont(12, 0));
	_partListBox->setHoverImageId (ATOM_IMAGEID_LISTITEM);

	for(unsigned i = 0; i < _actor->getPartDefine().getNumParts(); ++i)
	{
		_partListBox->addItem (_actor->getPartDefine().getPart (i)->getName());
	}
}

void PluginActor::createPointList (void)
{
	ATOM_Rect2Di rc = _editor->getRealtimeCtrl()->getClientRect();
	rc.size.w = 200;

	if (!_pointList)
	{
		_pointList = ATOM_NEW(ATOM_ListBox, _editor->getRealtimeCtrl(), rc, 16, ATOM_Widget::VScroll|ATOM_Widget::Border, ID_POINTLIST, ATOM_Widget::ShowNormal);
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

ATOM_STRING PluginActor::generatePointName (void)
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

void PluginActor::newPointShape (const char *name, const ATOM_Matrix4x4f &matrix)
{
	_editor->endEditNodeTransform ();

	if (_actor)
	{
		ATOM_HARDREF(ATOM_Node) node;
		node->setO2T (matrix);
		_actor->appendChild (node.get());

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

void PluginActor::refreshPointList (void)
{
	_pointList->clearItem ();

	for (ATOM_MAP<ATOM_STRING, ATOM_AUTOREF(ATOM_Node)>::const_iterator it = _points.begin(); it != _points.end(); ++it)
	{
		_pointList->addItem (it->first.c_str());
	}
}

void PluginActor::renameCurrentPoint (void)
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

void PluginActor::setCurrentPointToCamera (void)
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

void PluginActor::setCameraToCurrentPoint (void)
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

void PluginActor::deleteCurrentPoint (void)
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

void PluginActor::clearPoints (void)
{
	_editor->endEditNodeTransform ();

	for (ATOM_MAP<ATOM_STRING, ATOM_AUTOREF(ATOM_Node)>::iterator it = _points.begin(); it != _points.end(); ++it)
	{
		ATOM_AUTOREF(ATOM_Node) node = it->second;
		if (node->getParent())
		{
			node->getParent()->removeChild (node.get());
		}
	}
	_points.clear ();
	refreshPointList ();
}

void PluginActor::calcUILayout (void)
{
	ATOM_Rect2Di rc (ATOM_Point2Di(0, 0), _editor->getRealtimeCtrl()->getClientRect().size);
	_partListBox->resize (ATOM_Rect2Di(rc.point.x, rc.point.y, 200, rc.size.h/2));
	_partListBox->setScrollValue (ATOM_Point2Di(0, 0));

	ATOM_Point2Di pt0(rc.point.x, rc.size.h / 2);
	_editor->getRealtimeCtrl()->clientToGUI (&pt0);
	_tbProperties->setBarPosition (pt0.x, pt0.y);
	_tbProperties->setBarSize (200, rc.size.h - rc.size.h / 2);

	ATOM_Point2Di pt(rc.size.w - 200, 0);
	_editor->getRealtimeCtrl()->clientToGUI (&pt);

	_tbPartCandidates->setBarPosition (pt.x, pt.y);
	_tbPartCandidates->setBarSize (200, rc.size.h/2);

	_pointList->resize(ATOM_Rect2Di(pt.x,rc.size.h/2+1,200, rc.size.h-rc.size.h/2-1));
}

class NewPartDialogEventTrigger: public ATOM_EventTrigger
{
	ATOM_ActorPartDefine *_partDefine;
	ATOM_AUTOREF(ATOM_ActorPart) _partAdded;

public:
	ATOM_ActorPart *getAddedPart (void) const
	{
		return _partAdded.get();
	}

	NewPartDialogEventTrigger (ATOM_ActorPartDefine *partDefine)
	{
		_partDefine = partDefine;
	}

	void onCommand (ATOM_WidgetCommandEvent *event)
	{
		ATOM_Dialog *dialog = (ATOM_Dialog*)getHost();

		switch (event->id)
		{
		case ID_NEW_PART_OK:
			{
				ATOM_Edit *nameEdit = (ATOM_Edit*)dialog->getChildById (ID_EDIT_PART_NAME);
				ATOM_STRING name;
				nameEdit->getString (name);

				if (!name.empty ())
				{
					ATOM_ListBox *typeList = (ATOM_ListBox*)dialog->getChildById (ID_LIST_PART_TYPE);

					switch (typeList->getSelectIndex ())
					{
					case 0:
						_partAdded = ATOM_HARDREF(ATOM_ActorComponentsPart)();
						break;
					case 1:
						_partAdded = ATOM_HARDREF(ATOM_ActorColorPart)();
						break;
					case 2:
						_partAdded = ATOM_HARDREF(ATOM_ActorTexturePart)();
						break;
					case 3:
						_partAdded = ATOM_HARDREF(ATOM_ActorBindingPart)();
						break;
					case 4:
						_partAdded = ATOM_HARDREF(ATOM_ActorTransformPart)();
						break;
					default:
						break;
					}

					if (_partAdded)
					{
						if (_partAdded->setName (name.c_str()) && _partDefine->addPart (_partAdded.get()))
						{
							dialog->endModal (1);
						}
						else
						{
							_partAdded = 0;
						}
					}
				}
				break;
			}
		case ID_NEW_PART_CANCEL:
		case ATOM_Widget::CloseButtonId:
			{
				_partAdded = 0;
				dialog->endModal (0);
				break;
			}
		}
	}

	ATOM_DECLARE_EVENT_MAP(NewPartDialogEventTrigger, ATOM_EventTrigger)
};

ATOM_BEGIN_EVENT_MAP(NewPartDialogEventTrigger, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(NewPartDialogEventTrigger, ATOM_WidgetCommandEvent, onCommand)
	ATOM_END_EVENT_MAP

	void PluginActor::onNewPart (void)
{
	ATOM_Desktop *desktop = _editor->getGUIRenderer()->getDesktop();
	ATOM_Rect2Di rc = desktop->getClientRect();
	int dlgWidth = 200;
	int dlgHeight = 240;
	int x = (rc.size.w - dlgWidth) / 2;
	int y = (rc.size.h - dlgHeight) / 2;

	ATOM_Dialog *newPartDialog = ATOM_NEW(ATOM_Dialog, desktop, ATOM_Rect2Di(x, y, dlgWidth, dlgHeight));
	newPartDialog->setText ("创建部件");

	ATOM_Label *labelName = ATOM_NEW(ATOM_Label, newPartDialog, ATOM_Rect2Di (20, 10, 160, 20), 0, 0, ATOM_Widget::ShowNormal);
	labelName->setText ("部件名称:");
	labelName->setAlign (ATOM_Widget::AlignX_Left|ATOM_Widget::AlignY_Middle);
	labelName->setFont (ATOM_GUIFont::getDefaultFont(12, 0));

	ATOM_Edit *nameEdit = ATOM_NEW(ATOM_Edit, newPartDialog, ATOM_Rect2Di(20, 30, 160, 20), int(ATOM_Widget::Border), int(ID_EDIT_PART_NAME), ATOM_Widget::ShowNormal);
	nameEdit->setBorderMode (ATOM_Widget::Drop);
	nameEdit->setFont (ATOM_GUIFont::getDefaultFont(12, 0));

	ATOM_Label *labelType = ATOM_NEW(ATOM_Label, newPartDialog, ATOM_Rect2Di (20, 50, 160, 20), 0, 0, ATOM_Widget::ShowNormal);
	labelType->setText ("部件类型:");
	labelType->setAlign (ATOM_Widget::AlignX_Left|ATOM_Widget::AlignY_Middle);
	labelType->setFont (ATOM_GUIFont::getDefaultFont(12, 0));

	ATOM_ListBox *typeList = ATOM_NEW(ATOM_ListBox, newPartDialog, ATOM_Rect2Di(20, 70, 160, 100), 16, ATOM_Widget::Border|ATOM_Widget::VScroll, ID_LIST_PART_TYPE, ATOM_Widget::ShowNormal);
	typeList->setBorderMode (ATOM_Widget::Drop);
	typeList->setFont (ATOM_GUIFont::getDefaultFont(12, 0));
	typeList->setHoverImageId (ATOM_IMAGEID_LISTITEM);
	typeList->addItem ("模型/动作");
	typeList->addItem ("颜色");
	typeList->addItem ("贴图");
	typeList->addItem ("骨骼绑定部件");

	bool hasScale = false;
	for (unsigned i = 0; i < _actor->getPartDefine().getNumParts(); ++i)
	{
		if (dynamic_cast<ATOM_ActorTransformPart*>(_actor->getPartDefine().getPart(i)))
		{
			hasScale = true;
			break;
		}
	}
	if (!hasScale)
	{
		typeList->addItem ("模型缩放");
	}

	typeList->selectItem (0);

	ATOM_Button *buttonOk = ATOM_NEW(ATOM_Button, newPartDialog, ATOM_Rect2Di(20, 190, 50, 20), int(ATOM_Widget::Border), int(ID_NEW_PART_OK), ATOM_Widget::ShowNormal);
	buttonOk->setText ("确定");
	buttonOk->setFont (ATOM_GUIFont::getDefaultFont (12, 0));

	ATOM_Button *buttonCancel  = ATOM_NEW(ATOM_Button, newPartDialog, ATOM_Rect2Di(130, 190, 50, 20), int(ATOM_Widget::Border), int(ID_NEW_PART_CANCEL), ATOM_Widget::ShowNormal);
	buttonCancel->setText ("取消");
	buttonCancel->setFont (ATOM_GUIFont::getDefaultFont (12, 0));

	NewPartDialogEventTrigger trigger(&_actor->getPartDefine());
	newPartDialog->setEventTrigger (&trigger);
	newPartDialog->showModal ();
	ATOM_DELETE(newPartDialog);

	ATOM_AUTOREF(ATOM_ActorPart) addedPart = trigger.getAddedPart ();
	if (addedPart)
	{
		_partListBox->selectItem (_partListBox->addItem (addedPart->getName()));
	}
}

void PluginActor::onDeletePart (void)
{
	int selected = _partListBox->getSelectIndex ();

	if (selected < 0)
	{
		return;
	}

	const char *partName = _partListBox->getItemText(selected)->getString ();

	_actor->getPartDefine ().removePartByName (partName);

	_partListBox->selectItem (-1);
	_partListBox->removeItem (selected);
	_tbPartCandidates->clear ();
}

void PluginActor::onSelectPart (void)
{
	int selected = _partListBox->getSelectIndex ();

	if (selected < 0)
	{
		return;
	}
}

class AffectPartsDialogEventTrigger: public ATOM_EventTrigger
{
	ATOM_ActorModifierPart *_part;

public:
	AffectPartsDialogEventTrigger (ATOM_ActorModifierPart *part): _part (part)
	{
	}

	void onCommand (ATOM_WidgetCommandEvent *event)
	{
		ATOM_Dialog *dialog = (ATOM_Dialog*)getHost();

		switch (event->id)
		{
		case ID_SELECT_PART:
			{
				ATOM_ListBox *listBoxAllParts = (ATOM_ListBox*)dialog->getChildById (ID_LIST_ALL_PARTS);
				int index = listBoxAllParts->getSelectIndex ();
				if (index >= 0)
				{
					ATOM_ListBox *listBoxSelectedParts = (ATOM_ListBox*)dialog->getChildById (ID_LIST_SEL_PARTS);
					listBoxSelectedParts->addItem (listBoxAllParts->getItemText(index)->getString());
					listBoxAllParts->removeItem (index);
				}
				break;
			}
		case ID_DESELECT_PART:
			{
				ATOM_ListBox *listBoxSelectedParts = (ATOM_ListBox*)dialog->getChildById (ID_LIST_SEL_PARTS);
				int index = listBoxSelectedParts->getSelectIndex ();
				if (index >= 0)
				{
					ATOM_ListBox *listBoxAllParts = (ATOM_ListBox*)dialog->getChildById (ID_LIST_ALL_PARTS);
					listBoxAllParts->addItem (listBoxSelectedParts->getItemText(index)->getString());
					listBoxSelectedParts->removeItem (index);
				}
				break;
			}
		case ID_AFFECT_PARTS_OK:
			{
				if (_part)
				{
					ATOM_ListBox *listBoxSelectedParts = (ATOM_ListBox*)dialog->getChildById (ID_LIST_SEL_PARTS);
					_part->clearAllParts ();
					for (unsigned i = 0; i < listBoxSelectedParts->getItemCount (); ++i)
					{
						_part->addPart (listBoxSelectedParts->getItemText (i)->getString());
					}
				}
				dialog->endModal (1);
				break;
			}
		case ID_AFFECT_PARTS_CANCEL:
		case ATOM_Widget::CloseButtonId:
			{
				dialog->endModal (0);
				break;
			}
		}
	}

	ATOM_DECLARE_EVENT_MAP(AffectPartsDialogEventTrigger, ATOM_EventTrigger)
};

ATOM_BEGIN_EVENT_MAP(AffectPartsDialogEventTrigger, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(AffectPartsDialogEventTrigger, ATOM_WidgetCommandEvent, onCommand)
	ATOM_END_EVENT_MAP


	void PluginActor::editAffectParts (void)
{
	ATOM_ActorModifierPart *part = dynamic_cast<ATOM_ActorModifierPart*>(_currentPart);
	if (!part)
	{
		return;
	}

	ATOM_Desktop *desktop = _editor->getGUIRenderer()->getDesktop();
	ATOM_Rect2Di rc = desktop->getClientRect();
	int dlgWidth = 300;
	int dlgHeight = 250;
	int x = (rc.size.w - dlgWidth) / 2;
	int y = (rc.size.h - dlgHeight) / 2;

	ATOM_Dialog *partsDialog = ATOM_NEW(ATOM_Dialog, desktop, ATOM_Rect2Di(x, y, dlgWidth, dlgHeight));

	ATOM_Label *labelAllParts = ATOM_NEW(ATOM_Label, partsDialog, ATOM_Rect2Di (20, 30, 110, 20), 0, 0, ATOM_Widget::ShowNormal);
	labelAllParts->setText ("所有部件:");
	labelAllParts->setAlign (ATOM_Widget::AlignX_Left|ATOM_Widget::AlignY_Middle);
	labelAllParts->setFont (ATOM_GUIFont::getDefaultFont(12, 0));

	ATOM_Label *labelSelectedParts = ATOM_NEW(ATOM_Label, partsDialog, ATOM_Rect2Di (170, 30, 110, 20), 0, 0, ATOM_Widget::ShowNormal);
	labelSelectedParts->setText ("已选部件:");
	labelSelectedParts->setAlign (ATOM_Widget::AlignX_Left|ATOM_Widget::AlignY_Middle);
	labelSelectedParts->setFont (ATOM_GUIFont::getDefaultFont(12, 0));

	ATOM_ListBox *allPartsList = ATOM_NEW(ATOM_ListBox, partsDialog, ATOM_Rect2Di(20, 60, 110, 150), 16, ATOM_Widget::Border|ATOM_Widget::VScroll, ID_LIST_ALL_PARTS, ATOM_Widget::ShowNormal);
	allPartsList->setBorderMode (ATOM_Widget::Drop);
	allPartsList->setFont (ATOM_GUIFont::getDefaultFont(12, 0));
	allPartsList->setHoverImageId (ATOM_IMAGEID_LISTITEM);
	for (unsigned i = 0; i < _actor->getPartDefine().getNumParts (); ++i)
	{
		const char *name = _actor->getPartDefine().getPart(i)->getName();
		for (unsigned j = 0; j < part->getNumParts(); ++j)
		{
			if (!strcmp (name, part->getPart (j)))
			{
				name = 0;
				break;
			}
		}
		if (name)
		{
			allPartsList->addItem (name);
		}
	}

	ATOM_ListBox *selectedPartsList = ATOM_NEW(ATOM_ListBox, partsDialog, ATOM_Rect2Di(170, 60, 110, 150), 16, ATOM_Widget::Border|ATOM_Widget::VScroll, ID_LIST_SEL_PARTS, ATOM_Widget::ShowNormal);
	selectedPartsList->setBorderMode (ATOM_Widget::Drop);
	selectedPartsList->setFont (ATOM_GUIFont::getDefaultFont(12, 0));
	selectedPartsList->setHoverImageId (ATOM_IMAGEID_LISTITEM);
	for (unsigned j = 0; j < part->getNumParts(); ++j)
	{
		selectedPartsList->addItem (part->getPart (j));
	}

	ATOM_Button *buttonSelect = ATOM_NEW(ATOM_Button, partsDialog, ATOM_Rect2Di(140, 80, 20, 20), int(ATOM_Widget::Border), int(ID_SELECT_PART), ATOM_Widget::ShowNormal);
	buttonSelect->setText (">");
	buttonSelect->setFont (ATOM_GUIFont::getDefaultFont (12, 0));

	ATOM_Button *buttonDeselect = ATOM_NEW(ATOM_Button, partsDialog, ATOM_Rect2Di(140, 120, 20, 20), int(ATOM_Widget::Border), int(ID_DESELECT_PART), ATOM_Widget::ShowNormal);
	buttonDeselect->setText ("<");
	buttonDeselect->setFont (ATOM_GUIFont::getDefaultFont (12, 0));

	ATOM_Button *buttonOk = ATOM_NEW(ATOM_Button, partsDialog, ATOM_Rect2Di(20, 220, 40, 20), int(ATOM_Widget::Border), int(ID_AFFECT_PARTS_OK), ATOM_Widget::ShowNormal);
	buttonOk->setText ("确定");
	buttonOk->setFont (ATOM_GUIFont::getDefaultFont (12, 0));

	ATOM_Button *buttonCancel = ATOM_NEW(ATOM_Button, partsDialog, ATOM_Rect2Di(240, 220, 40, 20), int(ATOM_Widget::Border), int(ID_AFFECT_PARTS_CANCEL), ATOM_Widget::ShowNormal);
	buttonCancel->setText ("取消");
	buttonCancel->setFont (ATOM_GUIFont::getDefaultFont (12, 0));

	AffectPartsDialogEventTrigger trigger(part);
	partsDialog->setEventTrigger (&trigger);

	partsDialog->showModal ();

	ATOM_DELETE(partsDialog);
}

class PropTypeSelectDialogEventTrigger: public ATOM_EventTrigger
{
public:
	PropTypeSelectDialogEventTrigger (void)
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

	ATOM_DECLARE_EVENT_MAP(PropTypeSelectDialogEventTrigger, ATOM_EventTrigger)
};

ATOM_BEGIN_EVENT_MAP(PropTypeSelectDialogEventTrigger, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(PropTypeSelectDialogEventTrigger, ATOM_WidgetCommandEvent, onCommand)
ATOM_END_EVENT_MAP

int PluginActor::newProperty (ATOM_STRING &name) const
{
	ATOM_Desktop *desktop = _editor->getGUIRenderer()->getDesktop();
	ATOM_Rect2Di rc = desktop->getClientRect();
	int dlgWidth = 150;
	int dlgHeight = 260;
	int x = (rc.size.w - dlgWidth) / 2;
	int y = (rc.size.h - dlgHeight) / 2;

	ATOM_Dialog *typeDialog = ATOM_NEW(ATOM_Dialog, desktop, ATOM_Rect2Di(x, y, dlgWidth, dlgHeight));

	ATOM_Label *labelPropName = ATOM_NEW(ATOM_Label, typeDialog, ATOM_Rect2Di (20, 30, 110, 20), 0, 0, ATOM_Widget::ShowNormal);
	labelPropName->setText ("属性名称:");
	labelPropName->setAlign (ATOM_Widget::AlignX_Left|ATOM_Widget::AlignY_Middle);
	labelPropName->setFont (ATOM_GUIFont::getDefaultFont(12, 0));

	ATOM_Edit *nameEdit = ATOM_NEW(ATOM_Edit, typeDialog, ATOM_Rect2Di(20, 60, 110, 20), int(ATOM_Widget::Border), int(ID_EDIT_PROP_NAME), ATOM_Widget::ShowNormal);
	nameEdit->setBorderMode (ATOM_Widget::Drop);
	nameEdit->setFont (ATOM_GUIFont::getDefaultFont(12, 0));

	ATOM_Label *labelPropType = ATOM_NEW(ATOM_Label, typeDialog, ATOM_Rect2Di (20, 90, 110, 20), 0, 0, ATOM_Widget::ShowNormal);
	labelPropType->setText ("选择属性类型:");
	labelPropType->setAlign (ATOM_Widget::AlignX_Left|ATOM_Widget::AlignY_Middle);
	labelPropType->setFont (ATOM_GUIFont::getDefaultFont(12, 0));

	ATOM_ListBox *allTypesList = ATOM_NEW(ATOM_ListBox, typeDialog, ATOM_Rect2Di(20, 120, 110, 90), 16, int(ATOM_Widget::Border|ATOM_Widget::VScroll), int(ID_LIST_ALL_PROP_TYPES), ATOM_Widget::ShowNormal);
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

	ATOM_Button *buttonOk = ATOM_NEW(ATOM_Button, typeDialog, ATOM_Rect2Di(20, 220, 40, 20), int(ATOM_Widget::Border), int(ID_SELECT_PROP_TYPE_OK), ATOM_Widget::ShowNormal);
	buttonOk->setText ("确定");
	buttonOk->setFont (ATOM_GUIFont::getDefaultFont (12, 0));

	ATOM_Button *buttonCancel = ATOM_NEW(ATOM_Button, typeDialog, ATOM_Rect2Di(90, 220, 40, 20), int(ATOM_Widget::Border), int(ID_SELECT_PROP_TYPE_CANCEL), ATOM_Widget::ShowNormal);
	buttonCancel->setText ("取消");
	buttonCancel->setFont (ATOM_GUIFont::getDefaultFont (12, 0));

	PropTypeSelectDialogEventTrigger trigger;
	typeDialog->setEventTrigger (&trigger);
	int index = typeDialog->showModal ();
	nameEdit->getString (name);

	ATOM_DELETE(typeDialog);

	switch (index)
	{
	case 0:
		return PROP_TYPE_INT;
	case 1:
		return PROP_TYPE_FLOAT;
	case 2:
		return PROP_TYPE_STRING;
	case 3:
		return PROP_TYPE_VECTOR;
	case 4:
		return PROP_TYPE_COLOR4F;
	case 5:
		return PROP_TYPE_SWITCH;
	default:
		return PROP_TYPE_NONE;
	}
}

void PluginActor::handleTransformEdited (ATOM_Node *node)
{
}

void PluginActor::handleScenePropChanged (void)
{
}

void PluginActor::changeRenderScheme (void)
{
	_scene->setRenderScheme (_editor->getRenderScheme());
}

