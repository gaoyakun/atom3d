#include "StdAfx.h"
#include "atom3d_studio.h"
#include "plugin.h"
#include "gridnode.h"
#include "plugin_composition.h"
#include "editor.h"
#include "camera_modal.h"
#include "timeline.h"
#include "dlg_new_particlesys.h"

#undef TEST_HURT_NUMBER_EFFECT

#define CAMERA_SOURCE_NAME	"$CS"
#define CAMERA_TARGET_NAME	"$CT"

#define ID_TRANSLATE			(PLUGIN_ID_START + 200)
#define ID_SCALE				(PLUGIN_ID_START + 201)
#define ID_ROTATE				(PLUGIN_ID_START + 202)
#define ID_END_EDIT_TRANSFORM	(PLUGIN_ID_START + 203)
#define ID_VIEW_TOGGLEGUI		(PLUGIN_ID_START + 204)
#define ID_SELECT_SCENE			(PLUGIN_ID_START + 205)
#define ID_SELECT_BKIMAGE		(PLUGIN_ID_START + 206)
#define ID_FILE_EXPORT			(PLUGIN_ID_START + 207)
#define ID_VIEW_TOGGLEGRID		(PLUGIN_ID_START + 208)
#define ID_DELETE_ACTOR			(PLUGIN_ID_START + 209)
#define ID_ACTOR_RENAME			(PLUGIN_ID_START + 210)
#define ID_TEST_CAMERA_PATH		(PLUGIN_ID_START + 211)
#define ID_VIEW_TOGGLEBBOX		(PLUGIN_ID_START + 212)

#define ID_MAKEREFERENCE		(PLUGIN_ID_START + 300)
#define ID_NORESET				(PLUGIN_ID_START + 301)
#define ID_ROTATE_KEY			(PLUGIN_ID_START + 302)
#define ID_TRANSLATE_KEY		(PLUGIN_ID_START + 303)
#define ID_SCALE_KEY			(PLUGIN_ID_START + 304)
#define ID_ACTION_KEY			(PLUGIN_ID_START + 305)
#define ID_ACTION_LOOP_KEY		(PLUGIN_ID_START + 306)
#define ID_ACTION_SPEED_KEY		(PLUGIN_ID_START + 307)
#define ID_ACTION_FADE_TIME_KEY	(PLUGIN_ID_START + 308)
#define ID_MATERIAL_PARAM		(PLUGIN_ID_START + 309)
#define ID_ROTATE_WAVETYPE		(PLUGIN_ID_START + 310)
#define ID_TRANSLATE_WAVETYPE	(PLUGIN_ID_START + 311)
#define ID_SCALE_WAVETYPE		(PLUGIN_ID_START + 312)

#ifdef TEST_HURT_NUMBER_EFFECT
static ATOM_AUTOREF(ATOM_ShapeNode) numberShapeNode;

float ScreenToWorld (ATOM_Camera *camera, float screenDistance, const ATOM_Vector3f &destPoint)
{
	float length = (camera->getViewMatrix().getRow3(3) - destPoint).getLength();

	float h = length * camera->getTanHalfFovy();
	float screenH = camera->getViewport().size.h * 0.5f;

	return h / screenH;
}

static void ATOM_CALL numberEffectCoroutine (void *param)
{
	float pixelsPerSec_y = 200;
	float pixelsPerSec_x = 50;

	ATOM_Camera *camera = (ATOM_Camera *)param;
	ATOM_Vector3f eye, to, up;
	camera->getViewMatrix().decomposeLookatLH (eye, to, up);
	ATOM_Vector3f viewDir = eye - to;
	ATOM_Vector3f viewAxisY = up;
	ATOM_Vector3f viewAxisX = crossProduct (viewAxisY, viewDir);

	ATOM_Vector3f destPoint = numberShapeNode->getWorldMatrix().getRow3(3);
	unsigned tick = ATOM_APP->getFrameStamp().currentTick;

	while (true)
	{
		ATOM_Coroutine::yieldTo (NULL, 0);
		const ATOM_FrameStamp &frameStamp = ATOM_APP->getFrameStamp();
		if (frameStamp.currentTick - tick > 5000)
		{
			break;
		}
		float s2w_y = ScreenToWorld (camera, pixelsPerSec_y, destPoint);
		float s2w_x = ScreenToWorld (camera, pixelsPerSec_x, destPoint);
		float d_y = s2w_y * pixelsPerSec_y * frameStamp.elapsedTick * 0.001f;
		float d_x = s2w_x * pixelsPerSec_x * frameStamp.elapsedTick * 0.001f;
		ATOM_Matrix4x4f m = numberShapeNode->getWorldMatrix ();
		m.setRow3 (3, m.getRow3(3) + d_y * viewAxisY + d_x * viewAxisX);
		numberShapeNode->setO2T (numberShapeNode->getParent()->getInvWorldMatrix() >> m);
		ATOM_Vector4f color = numberShapeNode->getColor();
		color.w = ATOM_max2(pixelsPerSec_x, pixelsPerSec_y) / 200.f;
		numberShapeNode->setColor (color);
		pixelsPerSec_y -= 4.f;
		pixelsPerSec_x -= 1.f;
	}
}

static ATOM_AUTOREF(ATOM_ShapeNode) createMissShapeNode (const ATOM_Vector4f &color, float scale)
{
	static bool error = false;
	if (error)
	{
		return 0;
	}

	static ATOM_AUTOREF(ATOM_Texture) texture; 
	if (!texture)
	{
		const char *missTextureFileName = "/textures/miss.png";
		if (!missTextureFileName)
		{
			error = true;
			return 0;
		}

		texture = ATOM_CreateTextureResource(missTextureFileName);
		if (!texture)
		{
			error = true;
			return 0;
		}
	}

	static ATOM_AUTOPTR(ATOM_Material) shapeMaterial;
	if (!shapeMaterial)
	{
		shapeMaterial = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/shape.mat");;
		if (!shapeMaterial)
		{
			error = true;
			return 0;
		}
		shapeMaterial->getParameterTable()->setTexture ("diffuseTexture", texture.get());
		shapeMaterial->getParameterTable()->setInt ("enableAlphaBlend", 1);
		shapeMaterial->getParameterTable()->setInt ("hasTexture", 1);
		shapeMaterial->getParameterTable()->setInt ("cullmode", ATOM_RenderAttributes::CullMode_None);
	}

	ATOM_HARDREF(ATOM_ShapeNode) shapeNode;
	shapeNode->loadAttribute (NULL);
	shapeNode->setType (ATOM_ShapeNode::BILLBOARD);
	shapeNode->setTransparency (1.f);
	if (!shapeNode->load (ATOM_GetRenderDevice()))
	{
		return 0;
	}
	shapeNode->setMaterial (shapeMaterial.get());
	shapeNode->setColor (color);

	float ratio = float(texture->getWidth())/float(texture->getHeight());
	shapeNode->setSize (ATOM_Vector3f(ratio*scale, scale, 1.f));

	return shapeNode;
}

ATOM_Node *NewHurtNumberEffect (ATOM_Node *parent, int number, const ATOM_Vector4f &color, const char *textureFileName)
{
	if (!parent)
	{
		return 0;
	}

	ATOM_AUTOREF(ATOM_ShapeNode) shapeNode = createMissShapeNode (color, 6);
	if (!shapeNode)
	{
		return 0;
	}

	parent->appendChild (shapeNode.get());

	return shapeNode.get();
}
#endif

PluginComposition::PluginComposition (void)
{
	_editor = 0;
	_scene = 0;
	_leftPanel = 0;
	_timeLine = 0;
	_actorPropertyBar = 0;
	_keyValueBar = 0;
}

PluginComposition::~PluginComposition (void)
{
	clear ();
}

unsigned PluginComposition::getVersion (void) const
{
	return AS_VERSION;
}

const char *PluginComposition::getName (void) const
{
	return "Composition editor";
}

void PluginComposition::deleteMe (void)
{
	ATOM_DELETE(this);
}

bool PluginComposition::initPlugin (AS_Editor *editor)
{
	editor->registerFileType (this, "cps", "ATOM3D 特效组合", AS_FILETYPE_CANEDIT);
	editor->allocClipboardContentType (CB_TYPE_COMPOSITION_ACTOR);

	_editor = editor;

	return true;
}

void PluginComposition::donePlugin (void)
{
	clear ();
}

bool PluginComposition::beginEdit (const char *filename)
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
	_grid->setO2T (ATOM_Matrix4x4f::getScaleMatrix(ATOM_Vector3f(10.f, 10.f, 10.f)));
	_scene->getRootNode()->appendChild (_grid.get());

	_scene->setShadowDistance (600.f);
	_scene->setShadowFadeMin (300.f);
	_scene->setShadowFadeMax (550.f);

	// wangjian modified
	//ATOM_HARDREF(ATOM_Sky) sky;
	//if (sky->load (ATOM_GetRenderDevice()))
	//{
	//	ATOM_Vector3f v(1.f, -2.f, 1.f);
	//	v.normalize();
	//	sky->setLightDir (v);
	//	sky->setLightScale (6.f);
	//	sky->setExposure (2.f);
	//	sky->setLightIntensity (3.f);
	//	sky->setAmbientLight (1.f);
	//	_scene->getRootNode()->appendChild (sky.get());
	//}


	AS_CameraModal *camera = _editor->getCameraModal();
	camera->setPosition (ATOM_Vector3f(0.f, 50.f, -50.f));
	camera->setDirection (ATOM_Vector3f(0.f, -50.f, 50.f));
	camera->setUpVector (ATOM_Vector3f(0.f, 1.f, 0.f));

	_editor->getRealtimeCtrl()->setScene (_scene);

	_node = ATOM_HARDREF(ATOM_CompositionNode)();
	_node->setDrawBoundingbox (1);
	_node->setLoadReferenceActors (true);
	_node->setO2T (ATOM_Matrix4x4f::getIdentityMatrix ());
	_scene->getRootNode()->appendChild (_node.get());

#ifdef TEST_HURT_NUMBER_EFFECT
	numberShapeNode = NewHurtNumberEffect (_scene->getRootNode(), 4, ATOM_Vector4f(1.f, 1.f, 0.f, 1.f), "/textures/number.png");
	numberShapeNode->setO2T(ATOM_Matrix4x4f::getScaleMatrix(ATOM_Vector3f(10.f)));
	ATOM_Coroutine::createCoroutine (&numberEffectCoroutine, _scene->getCamera());
#endif
	if (filename)
	{
		_node->setCompositionFileName (filename);
	}
	else
	{
		_node->loadAttribute (NULL);
	}

	_node->setLoadPriority (ATOM_LoadPriority_IMMEDIATE);
	if (!_node->load (ATOM_GetRenderDevice()))
	{
		clear ();
		return false;
	}
	_node->stop ();

	if (!updateActorMap ()) 
	{
		MessageBoxA (ATOM_APP->getMainWindow(), "角色名字重复!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
		clear ();
		return false;
	}
	_editor->lookAtNode (_node.get());

	if (!initGUI ())
	{
		clear ();
		return false;
	}

	calcUILayout ();
	setupMenu ();

	_editor->getRealtimeCtrl()->enableDrop (true);

	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_t, 0, ID_TRANSLATE));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_s, 0, ID_SCALE));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_r, 0, ID_ROTATE));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_ESCAPE, 0, ID_END_EDIT_TRANSFORM));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_F11, 0, ID_VIEW_TOGGLEGUI));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_F4, 0, ID_VIEW_TOGGLEGRID));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_F5, 0, ID_VIEW_TOGGLEBBOX));
	_accelKeys.push_back (_editor->registerAccelKeyCommand (KEY_DELETE, 0, ID_DELETE_ACTOR));

	selectActorByName (_node->getNumTopActors()>0 ? _node->getTopActor(0)->getName() : "");
	_timeLine->setDuration (_node->getTimeLineDuration ());

	_guiShown = true;

	return true;
}


void PluginComposition::endEdit (void)
{
	cleanupMenu ();
	clear ();

	for (unsigned i = 0; i < _accelKeys.size(); ++i)
	{
		_editor->unregisterAccelKeyCommand (_accelKeys[i]);
	}
	_accelKeys.clear ();

	_editor->getRealtimeCtrl()->enableDrop (false);
}

void PluginComposition::frameUpdate (void)
{
	if (_node->isPlaying ())
	{
		//ATOM_LOGGER::log ("%d\n", _node->getTimeLinePosition());
		_timeLine->setCurrentSlice (_timeLine->timeToSliceIndex(_node->getTimeLinePosition()), _node.get());
	}

	updateKeyframeBarTransform ();
}

void PluginComposition::handleTransformEdited (ATOM_Node *node)
{
	if (_timeLine->getActor() && _timeLine->getActor()->getNode() == node)
	{
		_timeLine->updateKeyFrame (_transformMode);
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

static const char * const WeaponTrailKeys[] = {
	"TrailColor",
	"Power"
};

static const char * const TrailKeys[] = {
	"StartColor",
	"EndColor"
};

static const char * const DecalKeys[] = {
	"DecalColor"
};

static const char * const PSKeys[] = {
	"EmitInterval",
	"EmitCount",
	"Gravity",
	"Wind",
	"Scalar",
	"EmitterConeRadius",
	"EmitterConeRadiusVar",
	"EmitterSize",
	"EmitterSizeVar",
	"Velocity",
	"VelocityVar",
	"Life",
	"LifeVar",
	"Size1",
	"Size1Var",
	"Size2",
	"Size2Var",
	"Accel",
	"AccelVar",
	"Alpha",
	"Color",
	"ColorMultiplier"		// wangjian added
};

static const char * const GeodeKeys[] = {
	"Transparency",
	"ColorMultiplier"		// wangjian added
};

static const char * const ShapeKeys[] = {
	"Transparency",
	"ShapeColor",
	"Shearing",
	"ShapeSize",
	"UVRotation",
	"UVScaleOffset",
	"ColorMultiplier",		// wangjian added
	"ResolveScale"			// wangjian added
};

static const char * const LightKeys[] = {
	"Color",
	"Type",
	"Attenuation"
};

void PluginComposition::setupKeyframeBarLight (ATOM_CompositionActor *actor)
{
	_keyValueBar->setObject (actor ? actor->getNode() : NULL);
	setupKeyframeBarTransform (actor);
}

void PluginComposition::setupKeyframeBarTrail (ATOM_CompositionActor *actor)
{
	_keyValueBar->setObject (actor ? actor->getNode() : NULL);
	setupKeyframeBarTransform (actor);
}

void PluginComposition::setupKeyframeBarWeaponTrail (ATOM_CompositionActor *actor)
{
	_keyValueBar->setObject (actor ? actor->getNode() : NULL);
}

void PluginComposition::setupKeyframeBarDecal (ATOM_CompositionActor *actor)
{
	_keyValueBar->setObject (actor ? actor->getNode() : NULL);
	setupKeyframeBarTransform (actor);
}

void PluginComposition::setupKeyframeBarShape (ATOM_CompositionActor *actor)
{
	_keyValueBar->setObject (actor ? actor->getNode() : NULL);
	setupKeyframeBarTransform (actor);
}

void PluginComposition::setupKeyframeBarPS (ATOM_CompositionActor *actor)
{
	_keyValueBar->setObject (actor ? actor->getNode() : NULL);
	setupKeyframeBarTransform (actor);
}

void PluginComposition::setupKeyframeBarGeode (ATOM_CompositionActor *actor)
{
	ATOM_Geode *geode = (ATOM_Geode*)actor->getNode();
	_keyValueBar->setObject (actor ? actor->getNode() : NULL);

	if (geode->getNumTracks() > 0)
	{
		ATOM_GeodeKeyFrameValue value;
		actor->captureKeyFrameValue (&value);

		ATOMX_TBEnum enumActions;
		enumActions.addEnum ("无", -1);
		for (unsigned i = 0; i < geode->getNumTracks(); ++i)
		{
			enumActions.addEnum (geode->getTrack(i)->getName(), i);
		}
		_keyValueBar->addEnum ("动作", ID_ACTION_KEY, value.getData().actionIndex, enumActions, false, 0);
		_keyValueBar->addIntVar ("循环次数", ID_ACTION_LOOP_KEY, value.getData().actionIndex < 0 ? 0 : value.getData().loop, false, 0);
		_keyValueBar->addFloatVar ("速度", ID_ACTION_SPEED_KEY, value.getData().actionIndex < 0 ? 1.f : value.getData().speed, false, 0);
		_keyValueBar->addIntVar ("融合时间", ID_ACTION_FADE_TIME_KEY, value.getData().actionIndex < 0 ? 500 : value.getData().fadeTime, false, 0);
	}
	setupKeyframeBarTransform (actor);
}

void PluginComposition::setupKeyframeBarTransform (ATOM_CompositionActor *actor)
{
	ATOM_Vector3f t = actor->getTranslateTrack()->eval (_node->getTimeLinePosition ());
	ATOM_Vector3f s = actor->getScaleTrack()->eval (_node->getTimeLinePosition ());
	ATOM_Vector3f r = actor->getRotateTrack()->eval (_node->getTimeLinePosition ());

	_keyValueBar->addVector3fVar ("位移", ID_TRANSLATE_KEY, t, false, "ATOM_Node");
	_keyValueBar->addVector3fVar ("缩放", ID_SCALE_KEY, s, false, "ATOM_Node");
	_keyValueBar->addVector3fVar ("旋转", ID_ROTATE_KEY, r, false, "ATOM_Node");

	ATOMX_TBEnum enumWaveType;
	enumWaveType.addEnum ("线形", ATOM_WAVE_TYPE_LINEAR);
	enumWaveType.addEnum ("曲线", ATOM_WAVE_TYPE_SPLINE);
	enumWaveType.addEnum ("跳跃", ATOM_WAVE_TYPE_STEP);
	_keyValueBar->addEnum ("位移插值", ID_TRANSLATE_WAVETYPE, actor->getTranslateTrack()->getWaveType(), enumWaveType, false, "插值选项");
	_keyValueBar->addEnum ("缩放插值", ID_SCALE_WAVETYPE, actor->getScaleTrack()->getWaveType(), enumWaveType, false, "插值选项");
	_keyValueBar->addEnum ("旋转插值", ID_ROTATE_WAVETYPE, actor->getRotateTrack()->getWaveType(), enumWaveType, false, "插值选项");
}

void PluginComposition::updateKeyframeBarTransform (void)
{
	ATOM_CompositionActor *actor = _timeLine->getActor ();
	if (actor)
	{
		ATOM_Vector3f t = actor->getTranslateTrack()->eval (_node->getTimeLinePosition ());
		ATOM_Vector3f s = actor->getScaleTrack()->eval (_node->getTimeLinePosition ());
		ATOM_Vector3f r = actor->getRotateTrack()->eval (_node->getTimeLinePosition ());

		_keyValueBar->set3F ("位移", t, false);
		_keyValueBar->set3F ("缩放", s, false);
		_keyValueBar->set3F ("旋转", r * (180.f / ATOM_Pi), false);
	}
}

bool PluginComposition::isKeyPropertyWeaponTrail (const char *name) const
{
	for (unsigned i = 0; i < sizeof(WeaponTrailKeys)/sizeof(WeaponTrailKeys[0]); ++i)
	{
		if (!strcmp (name, WeaponTrailKeys[i]))
			return true;
	}
	return false;
}

bool PluginComposition::isKeyPropertyTrail (const char *name) const
{
	for (unsigned i = 0; i < sizeof(TrailKeys)/sizeof(TrailKeys[0]); ++i)
	{
		if (!strcmp (name, TrailKeys[i]))
			return true;
	}
	return false;
}

bool PluginComposition::isKeyPropertyDecal (const char *name) const
{
	for (unsigned i = 0; i < sizeof(DecalKeys)/sizeof(DecalKeys[0]); ++i)
	{
		if (!strcmp (name, DecalKeys[i]))
			return true;
	}
	return false;
}

bool PluginComposition::isKeyPropertyPS (const char *name) const
{
	for (unsigned i = 0; i < sizeof(PSKeys)/sizeof(PSKeys[0]); ++i)
	{
		if (!strcmp (name, PSKeys[i]))
			return true;
	}
	return false;
}

bool PluginComposition::isKeyPropertyGeode (const char *name) const
{
	for (unsigned i = 0; i < sizeof(GeodeKeys)/sizeof(GeodeKeys[0]); ++i)
	{
		if (!strcmp (name, GeodeKeys[i]))
			return true;
	}
	return false;
}

bool PluginComposition::isKeyPropertyShape (const char *name) const
{
	for (unsigned i = 0; i < sizeof(ShapeKeys)/sizeof(ShapeKeys[0]); ++i)
	{
		if (!strcmp (name, ShapeKeys[i]))
			return true;
	}
	return false;
}

bool PluginComposition::isKeyPropertyLight (const char *name) const
{
	for (unsigned i = 0; i < sizeof(LightKeys)/sizeof(LightKeys[0]); ++i)
	{
		if (!strcmp (name, LightKeys[i]))
			return true;
	}
	return false;
}

void PluginComposition::handleEvent (ATOM_Event *event)
{
	int eventId = event->getEventTypeId ();
	if (eventId == ATOM_WidgetResizeEvent::eventTypeId())
	{
		calcUILayout ();
	}
	else if (eventId == ATOMX_TWAddVariableEvent::eventTypeId())
	{
		ATOMX_TWAddVariableEvent *e = (ATOMX_TWAddVariableEvent*)event;
		if (e->bar == _actorPropertyBar)
		{
			if (dynamic_cast<ATOM_ParticleSystem*>(_actorPropertyBar->getObject()))
			{
				e->allow = !ATOM_FindAttrib(ATOM_ParticleSystem::_classname(), e->name) || !isKeyPropertyPS (e->name);
			}
			else if (dynamic_cast<ATOM_Geode*>(_actorPropertyBar->getObject()))
			{
				e->allow = !ATOM_FindAttrib(ATOM_Geode::_classname(), e->name) || !isKeyPropertyGeode (e->name);
			}
			else if (dynamic_cast<ATOM_ShapeNode*>(_actorPropertyBar->getObject()))
			{
				e->allow = !ATOM_FindAttrib(ATOM_ShapeNode::_classname(), e->name) || !isKeyPropertyShape (e->name);
			}
			else if (dynamic_cast<ATOM_RibbonTrail*>(_actorPropertyBar->getObject()))
			{
				e->allow = !ATOM_FindAttrib(ATOM_RibbonTrail::_classname(), e->name) || !isKeyPropertyTrail (e->name);
			}
			else if (dynamic_cast<ATOM_WeaponTrail*>(_actorPropertyBar->getObject()))
			{
				e->allow = !ATOM_FindAttrib(ATOM_WeaponTrail::_classname(), e->name) || !isKeyPropertyWeaponTrail (e->name);
			}
			else if (dynamic_cast<ATOM_Decal*>(_actorPropertyBar->getObject()))
			{
				e->allow = !ATOM_FindAttrib(ATOM_Decal::_classname(), e->name) || !isKeyPropertyDecal (e->name);
			}
		}
		else if (e->bar == _keyValueBar)
		{
			if (dynamic_cast<ATOM_ParticleSystem*>(_keyValueBar->getObject()))
			{
				e->allow = !ATOM_FindAttrib(ATOM_ParticleSystem::_classname(), e->name) || isKeyPropertyPS (e->name);
			}
			else if (dynamic_cast<ATOM_Geode*>(_keyValueBar->getObject()))
			{
				e->allow = !ATOM_FindAttrib(ATOM_Geode::_classname(), e->name) || isKeyPropertyGeode (e->name);
			}
			else if (dynamic_cast<ATOM_ShapeNode*>(_actorPropertyBar->getObject()))
			{
				e->allow = !ATOM_FindAttrib(ATOM_ShapeNode::_classname(), e->name) || isKeyPropertyShape (e->name);
			}
			else if (dynamic_cast<ATOM_RibbonTrail*>(_actorPropertyBar->getObject()))
			{
				e->allow = !ATOM_FindAttrib(ATOM_RibbonTrail::_classname(), e->name) || isKeyPropertyTrail (e->name);
			}
			else if (dynamic_cast<ATOM_WeaponTrail*>(_actorPropertyBar->getObject()))
			{
				e->allow = !ATOM_FindAttrib(ATOM_WeaponTrail::_classname(), e->name) || isKeyPropertyWeaponTrail (e->name);
			}
			else if (dynamic_cast<ATOM_Decal*>(_actorPropertyBar->getObject()))
			{
				e->allow = !ATOM_FindAttrib(ATOM_Decal::_classname(), e->name) || isKeyPropertyDecal (e->name);
			}
		}
	}
	else if (eventId == ATOMX_TWCommandEvent::eventTypeId ())
	{
		ATOMX_TWCommandEvent *e = (ATOMX_TWCommandEvent*)event;
		if (e->bar == _actorPropertyBar)
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
	else if (eventId == ATOMX_TWValueChangedEvent::eventTypeId())
	{
		ATOMX_TWValueChangedEvent *e = (ATOMX_TWValueChangedEvent*)event;
		if (e->id == ID_MATERIAL_PARAM)
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
		else if (e->name =="isRef")
		{
			_timeLine->getActor()->makeReference (e->newValue.getB());
		}
		else if (e->name =="noReset")
		{
			_timeLine->getActor()->allowReset (!e->newValue.getB());
		}
		else if (e->bar == _keyValueBar)
		{
			if (e->id == ID_TRANSLATE_WAVETYPE)
			{
				ATOM_WaveType wt = (ATOM_WaveType)e->newValue.getI();
				_timeLine->getActor()->getTranslateTrack()->setWaveType (wt);
			}
			else if (e->id == ID_ROTATE_WAVETYPE)
			{
				ATOM_WaveType wt = (ATOM_WaveType)e->newValue.getI();
				_timeLine->getActor()->getRotateTrack()->setWaveType (wt);
			}
			else if (e->id == ID_SCALE_WAVETYPE)
			{
				ATOM_WaveType wt = (ATOM_WaveType)e->newValue.getI();
				_timeLine->getActor()->getScaleTrack()->setWaveType (wt);
			}
			else if (_timeLine->isKeyFrame (_timeLine->getCurrentSlice()))
			{
				if (dynamic_cast<ATOM_GeodeActor*>(_timeLine->getActor()))
				{
					ATOM_Geode *geode = (ATOM_Geode*)_timeLine->getActor()->getNode();
					int actionIndex = _keyValueBar->getI ("动作");
					int fadeTime = _keyValueBar->getI ("融合时间");
					float speed = _keyValueBar->getF ("速度");
					int loop = _keyValueBar->getI ("循环次数");

					ATOM_GeodeKeyFrameValue *v = (ATOM_GeodeKeyFrameValue*)_timeLine->getKeyFrameValue();
					ATOM_GeodeKeyFrameData data = v->getData();
					data.actionIndex = actionIndex;
					data.fadeTime = fadeTime;
					data.speed = speed;
					data.loop = loop;
					v->setData (data);

					if (actionIndex >= 0 && actionIndex < geode->getNumTracks())
					{
						geode->doAction (geode->getTrack(actionIndex)->getName(), ATOM_Geode::ACTIONFLAGS_DOWNSIDE|ATOM_Geode::ACTIONFLAGS_UPSIDE, loop, false, fadeTime, speed);
					}
					else
					{
						geode->doAction (0, ATOM_Geode::ACTIONFLAGS_DOWNSIDE|ATOM_Geode::ACTIONFLAGS_UPSIDE, loop, false, fadeTime, speed);
					}
				}

				if (e->id == ID_TRANSLATE_KEY)
				{
					ATOM_Vector3f *t = (ATOM_Vector3f*)e->newValue.get3F();
					_timeLine->getKeyFrameValue()->setTranslation (*t);
				}
				else if (e->id == ID_ROTATE_KEY)
				{
					ATOM_Vector3f euler = *((ATOM_Vector3f*)e->newValue.get3F());
					euler *= (ATOM_Pi / 180.f);
					_timeLine->getKeyFrameValue()->setRotation (euler);
				}
				else if (e->id == ID_SCALE_KEY)
				{
					ATOM_Vector3f *s = (ATOM_Vector3f*)e->newValue.get3F();
					_timeLine->getKeyFrameValue()->setScale (*s);
				}
			}
			_timeLine->updateKeyFrame (TRANSFORMMODE_NONE);
		}
	}
	else if (eventId == ATOM_TreeItemDragStartEvent::eventTypeId())
	{
		ATOM_TreeItemDragStartEvent *e = (ATOM_TreeItemDragStartEvent*)event;
		e->dragSource->setIndicatorImageId (0);
		e->dragSource->setIndicatorRect (ATOM_Rect2Di(-10, -5, 20, 10));
		e->dragSource->addProperty ("Type", "RP");
		e->dragSource->addProperty ("index", e->index);
	}
	else if (eventId == ATOM_TreeItemDragOverEvent::eventTypeId())
	{
		ATOM_TreeItemDragOverEvent *e = (ATOM_TreeItemDragOverEvent*)event;
		//ATOM_LOGGER::log ("TreeItem (%s) dragging over\n", m_ActorTree->getItemByIndex(e->index)->getText()->getString());
	}
	else if (eventId == ATOM_TreeItemDragDropEvent::eventTypeId())
	{
		ATOM_TreeItemDragDropEvent *e = (ATOM_TreeItemDragDropEvent*)event;
		if (e->dragSource->getProperty("Type") == "RP")
		{
			int sourceIndex = e->dragSource->getProperty("index").getI();
			int destIndex = e->index; 
			bool dup = (e->keymod & KEYMOD_CTRL) != 0;
			if (sourceIndex != destIndex)
			{
				ATOM_AUTOPTR(ATOM_CompositionActor) sourceActor = (ATOM_CompositionActor*)m_ActorTree->getItemByIndex (sourceIndex)->getUserData();
				if (destIndex == m_ActorTree->getFirstChildItem (NULL)->getIndex())
				{
					if (dup)
					{
						ATOM_AUTOPTR(ATOM_CompositionActor) actor = sourceActor->clone ();
						actor->getNode()->setO2T (sourceActor->getNode()->getO2T());
						actor->setName (newName().c_str());
						_actorMap[actor->getName()] = actor;
						_node->addActor (actor.get());
					}
					else
					{
						ATOM_CompositionActor *a = sourceActor->getParentActor();
						if (a) 
						{
							a->removeChildActor (sourceActor.get());
						}
						_node->addActor (sourceActor.get());
					}
					refreshActorList ();
				}
				else
				{
					ATOM_AUTOPTR(ATOM_CompositionActor) destActor = (ATOM_CompositionActor*)m_ActorTree->getItemByIndex (destIndex)->getUserData();
					if (sourceActor && destActor)
					{
						if (dup)
						{
							ATOM_AUTOPTR(ATOM_CompositionActor) actor = sourceActor->clone ();
							actor->getNode()->setO2T (sourceActor->getNode()->getO2T());
							actor->setName (newName().c_str());
							_actorMap[actor->getName()] = actor;

							destActor->addChildActor (actor.get());
							refreshActorList ();
						}
						else
						{
							ATOM_AUTOREF(ATOM_Node) sourceNode = sourceActor->getNode();
							ATOM_AUTOREF(ATOM_Node) destNode  = destActor->getNode();

							if (!isChildOf (destNode.get(), sourceNode.get()))
							{
								ATOM_CompositionActor *a = sourceActor->getParentActor();
								if (a)
								{
									a->removeChildActor (sourceActor.get());
								}
								else
								{
									_node->removeActor (sourceActor.get());
								}
								destActor->addChildActor (sourceActor.get());
								refreshActorList ();
							}
						}
					}
				}
			}
		}
	}
	else if (eventId == ATOM_TreeCtrlSelChangedEvent::eventTypeId())
	{
		ATOM_TreeCtrlSelChangedEvent *e = (ATOM_TreeCtrlSelChangedEvent*)event;
		int sel = e->selectedIndex;
		int desel = e->deselectedIndex;
		ATOM_TreeItem *item = m_ActorTree->getItemByIndex(e->selectedIndex);
		if (item && item->getUserData())
		{
			ATOM_CompositionActor *actor = (ATOM_CompositionActor*)item->getUserData();
			selectActorByName (actor->getName());
		}
		else
		{
			selectActorByName ("");
		}
	}
	else if (eventId == ATOM_WidgetCommandEvent::eventTypeId())
	{
		ATOM_WidgetCommandEvent *e = (ATOM_WidgetCommandEvent*)event;

		switch (e->id)
		{
		case ID_DELETE_ACTOR:
			{
				deleteCurrentActor ();
				break;
			}
		case ID_ACTOR_RENAME:
			{
				renameCurrentActor ();
				break;
			}
		case ID_FILE_EXPORT:
			{
				int n = _editor->getOpenFileNames ("pth", "摄像机路径(*.pth)|*.pth|", false, true, "导出");
				if (n == 1)
				{
					const char *filename = _editor->getOpenedFileName(0);
					const char *ext = strrchr(filename, '.');
					if (ext)
					{
						if (!stricmp(ext, ".pth"))
						{
							if (!exportCameraPath (filename))
							{
								::MessageBoxA (ATOM_APP->getMainWindow(), "导出失败!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
							}
						}
					}
				}
				break;
			}
		case ID_VIEW_TOGGLEGUI:
			{
				toggleGUI ();
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
		case ID_TEST_CAMERA_PATH:
			{
				testCameraPath ();
				break;
			}
		case ID_VIEW_TOGGLEGRID:
			{
				toggleGrid ();
				break;
			}
		case ID_VIEW_TOGGLEBBOX:
			{
				if(_node)
					_node->setDrawBoundingbox(!_node->getDrawBoundingbox());
				break;
			}
		case ID_TIMELINE_PLAY:
			{
				_editor->endEditNodeTransform ();
				_node->reset ();
				_node->play ();
				break;
			}
		case ID_TIMELINE_STOP:
			{
				_editor->endEditNodeTransform ();
				_node->stop ();
				break;
			}
		case ID_ROTATE:
			{
				if (!_timeLine->getActor ())
				{
					_editor->beginEditNodeRotation (_node.get());
				}
				else if (!_node->isPlaying() && _timeLine->isKeyFrame (_timeLine->getCurrentSlice()))
				{
					_editor->beginEditNodeRotation (_timeLine->getActor()->getNode());
					_transformMode = TRANSFORMMODE_ROTATION;
				}
				break;
			}
		case ID_TRANSLATE:
			{
				if (!_timeLine->getActor ())
				{
					_editor->beginEditNodeTranslation (_node.get());
				}
				else if (!_node->isPlaying() && _timeLine->isKeyFrame (_timeLine->getCurrentSlice()))
				{
					_editor->beginEditNodeTranslation (_timeLine->getActor()->getNode());
					_transformMode = TRANSFORMMODE_TRANSLATE;
				}
				break;
			}
		case ID_SCALE:
			{
				if (!_timeLine->getActor ())
				{
					_editor->beginEditNodeScaling (_node.get());
				}
				else if (!_node->isPlaying() && _timeLine->isKeyFrame (_timeLine->getCurrentSlice()))
				{
					_editor->beginEditNodeScaling (_timeLine->getActor()->getNode());
					_transformMode = TRANSFORMMODE_SCALE;
				}
				break;
			}
		case ID_END_EDIT_TRANSFORM:
			{
				_editor->endEditNodeTransform ();
				break;
			}
		}
	}
	else if (eventId == TimeChangeEvent::eventTypeId())
	{
		TimeChangeEvent *e = (TimeChangeEvent*)event;
		_node->seekTimeLine (e->time);

		if (dynamic_cast<ATOM_GeodeActor*>(_timeLine->getActor()))
		{
			ATOM_GeodeKeyFrameValue *v = (ATOM_GeodeKeyFrameValue*)_timeLine->getKeyFrameValue();
			const ATOM_GeodeKeyFrameData &data = v->getData();
			_keyValueBar->setI ("动作", data.actionIndex, false);
			_keyValueBar->setI ("融合时间", data.fadeTime, false);
			_keyValueBar->setF ("速度", data.speed, false);
			_keyValueBar->setI ("循环次数", data.loop, false);
		}
	}
	else if (eventId == ATOM_EditTextChangedEvent::eventTypeId())
	{
		ATOM_EditTextChangedEvent *e = (ATOM_EditTextChangedEvent*)event;
		if (e->id == ID_TIMELINE_DURATION)
		{
			unsigned duration = atoi (e->text.c_str());
			_node->setTimeLineDuration (duration);
			_timeLine->setDuration (duration);
		}
	}
	else if (eventId == ATOM_TreeCtrlClickEvent::eventTypeId())
	{
		ATOM_TreeCtrlClickEvent *e = (ATOM_TreeCtrlClickEvent*)event;
		ATOM_TreeItem *item = m_ActorTree->getSelectedItem ();
		if (!item)
		{
			selectActorByName (NULL);
		}
		else
		{
			selectActorByName (item->getText()->getString());
		}
	}
	else if (eventId == ATOM_CellDragStartEvent::eventTypeId())
	{
		ATOM_CellDragStartEvent *e = (ATOM_CellDragStartEvent*)event;
		if (_node->isPlaying ())
		{
			e->allow = false;
		}
		else
		{
			e->dragSource->addProperty ("Type", "CA");
			e->dragSource->addProperty ("x", e->x);
			e->dragSource->addProperty ("y", e->y);
		}
	}
	else if (eventId == ATOM_WidgetDragOverEvent::eventTypeId())
	{
		ATOM_WidgetDragOverEvent *e = (ATOM_WidgetDragOverEvent*)event;
		if (e->id == _editor->getRealtimeCtrl()->getId())
		{
			_createPosition.x = e->x;
			_createPosition.y = e->y;
		}
	}
	else if (eventId == ATOM_WidgetDragDropEvent::eventTypeId())
	{
		ATOM_WidgetDragDropEvent *e = (ATOM_WidgetDragDropEvent*)event;
		if (e->dragSource->getProperty("Type")=="CA" && e->id == _editor->getRealtimeCtrl()->getId() && !_node->isPlaying())
		{
			ATOM_AUTOPTR(ATOM_CompositionActor) actor;
			int x = e->dragSource->getProperty("x").getI();
			int y = e->dragSource->getProperty("y").getI();
			if (x == 0 && y == 0)
			{
				actor = ATOM_NEW(ATOM_ParticleSystemActor);
			}
			else if (x == 1 && y == 0)
			{
				actor = ATOM_NEW(ATOM_ShapeActor);
			}
			else if (x == 2 && y == 0)
			{
				actor = ATOM_NEW(ATOM_GeodeActor);
			}
			else if (x == 0 && y == 1)
			{
				actor = ATOM_NEW(ATOM_LightActor);
			}
			else if (x == 1 && y == 1)
			{
				actor = ATOM_NEW(ATOM_TrailActor);
			} 
			else if (x == 2 && y == 1)
			{
				actor = ATOM_NEW(ATOM_WeaponTrailActor);
			}
			else if (x == 0 && y == 2)
			{
				actor = ATOM_NEW(ATOM_DecalActor);
			}
			if (actor)
			{
				// 清除变换编辑状态
				_editor->endEditNodeTransform ();

					// 根据鼠标位置计算节点偏移
					float distance = 1000.f;

					ATOM_Ray ray;
					_editor->getRealtimeCtrl()->constructRay (_createPosition.x, _createPosition.y, ray);
					float Ry = ray.getDirection().y;
					float s = (Ry > 0.f) ? 1.f : -1.f;
					if (ATOM_abs(Ry) < 0.001f)
					{
						Ry = 0.001f * s;
					}
					const ATOM_Matrix4x4f &matProj = _scene->getCamera()->getProjectionMatrix();
					const ATOM_Matrix4x4f &matView = _scene->getCamera()->getViewMatrix();
					const ATOM_Matrix4x4f &matWorld = _node->getWorldMatrix();
					distance = (matWorld.m31 -_editor->getCameraModal()->getPosition().y) / ray.getDirection().y;
					if (distance < 0.f)
					{
						distance = 0.f;
					}
					ATOM_Vector3f cameraPos = _editor->getCameraModal()->getPosition();
					ATOM_Vector3f rayDirection = ray.getDirection(); 
					rayDirection.normalize ();
					ATOM_Vector3f objectPos = cameraPos + rayDirection * distance;
					ATOM_Matrix4x4f m = ATOM_Matrix4x4f::getTranslateMatrix (objectPos);

					actor->getNode()->loadAttribute (NULL);
					actor->getNode()->load (ATOM_GetRenderDevice());
					_node->addActor (actor.get());
					actor->getNode()->setO2T (actor->getNode()->getParent()->getInvWorldMatrix() >> m);
				actor->setName (newName().c_str());

				_actorMap[actor->getName()] = actor;

				selectActorByName (actor->getName());

				refreshActorList ();
			}
		}
	}
}

int PluginComposition::refreshActorListT (ATOM_Node *node)
{
	ATOM_TreeItem *root = 0;
	int index = -1; 

	if (node != _node->getActorParentNode())
	{
		ATOM_Node *parentNode = node->getParent ();
		if (parentNode == _node->getActorParentNode())
		{
			root = m_ActorTree->getFirstChildItem (NULL);
		}
		else
		{
			const char *actor = findActorByNode (parentNode);
			root = actor ? m_ActorTree->getItemByUserData ((unsigned long long)(_actorMap[actor].get())) : 0;
		}

		if (root)
		{
			const char *actor = findActorByNode (node);
			if (actor)
			{
				ATOM_TreeItem *item = m_ActorTree->newItem (actor, ATOM_IMAGEID_TREECTRL_EXPANDED_IMAGE, ATOM_IMAGEID_TREECTRL_COLLAPSED_IMAGE, root, ATOM_Widget::AnyId, false);
				item->setUserData ((unsigned long long)_actorMap[actor].get());

				if (_actorMap[actor].get() == _timeLine->getActor())
				{
					index = item->getIndex();
				}
			}
		}
	}

	for (unsigned i = 0; i < node->getNumChildren (); ++i)
	{
		int n = refreshActorListT (node->getChild (i));
		if (n != -1)
		{
			index = n;
		}
	}

	return index;
}

void PluginComposition::refreshActorList (void)
{
	m_ActorTree->clear ();
	m_ActorTree->newItem ("Root", ATOM_IMAGEID_TREECTRL_EXPANDED_IMAGE, ATOM_IMAGEID_TREECTRL_COLLAPSED_IMAGE, NULL);

	int index = refreshActorListT (_node->getActorParentNode());

	m_ActorTree->selectItem (m_ActorTree->getItemByIndex(index));
}

bool PluginComposition::saveFile (const char *filename)
{
	char buffer[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (filename, buffer);
	ATOM_TiXmlDocument doc(buffer);

	ATOM_TiXmlDeclaration eDecl("1.0", "gb2312", "");
	doc.InsertEndChild (eDecl);

	ATOM_TiXmlElement eRoot("Composition");

	//--- wangjian modified ---//
	if( !_node->saveToXML (&eRoot) )
	{
		MessageBoxA (ATOM_APP->getMainWindow(), "保存失败(请检查 bbox_min 和 bbox_max )!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
	}
	//-------------------------//

	doc.InsertEndChild (eRoot);

	return doc.SaveFile();
}

unsigned PluginComposition::getMinWindowWidth (void) const
{
	return 600;
}

unsigned PluginComposition::getMinWindowHeight (void) const
{
	return 450;
}

bool PluginComposition::isDocumentModified (void)
{
	return _editor->isDocumentModified ();
}

void PluginComposition::setupMenu (void)
{
	ATOM_PopupMenu *menuFile = _editor->getMenuBar()->getMenuItem (AS_MENUITEM_FILE).submenu;
	menuFile->insertMenuItem (menuFile->getNumItems()-1, "导出..", ID_FILE_EXPORT);

	ATOM_PopupMenu *menuView = _editor->getMenuBar()->getMenuItem (AS_MENUITEM_VIEW).submenu;
	menuView->appendMenuItem ("切换界面显示(F11)", ID_VIEW_TOGGLEGUI);
	menuView->appendMenuItem ("切换网格显示(F4)", ID_VIEW_TOGGLEGRID);
	menuView->appendMenuItem ("切换包围盒显示(F5)", ID_VIEW_TOGGLEBBOX);
	menuView->appendMenuItem ("选择场景..", ID_SELECT_SCENE);
	menuView->appendMenuItem ("选择背景..", ID_SELECT_BKIMAGE);
	menuView->appendMenuItem ("测试摄像机路径", ID_TEST_CAMERA_PATH);

	_editor->getMenuBar()->insertMenuItem (AS_MENUITEM_CUSTOM, "演员", 0);
	ATOM_PopupMenu *menu = _editor->getMenuBar()->createSubMenu (AS_MENUITEM_CUSTOM);
	menu->appendMenuItem ("重命名..", ID_ACTOR_RENAME);
	menu->appendMenuItem ("平移(T)", ID_TRANSLATE);
	menu->appendMenuItem ("缩放(S)", ID_SCALE);
	menu->appendMenuItem ("旋转(R)", ID_ROTATE);
	menu->appendMenuItem ("删除(DEL)", ID_DELETE_ACTOR);
}

void PluginComposition::cleanupMenu (void)
{
	ATOM_MenuBar *menubar = _editor->getMenuBar ();
	menubar->getMenuItem(AS_MENUITEM_FILE).submenu->removeMenuItemById (ID_FILE_EXPORT);
	menubar->getMenuItem(AS_MENUITEM_VIEW).submenu->removeMenuItemById (ID_VIEW_TOGGLEGUI);
	menubar->getMenuItem(AS_MENUITEM_VIEW).submenu->removeMenuItemById (ID_VIEW_TOGGLEGRID);
	menubar->getMenuItem(AS_MENUITEM_VIEW).submenu->removeMenuItemById (ID_VIEW_TOGGLEBBOX);
	menubar->getMenuItem(AS_MENUITEM_VIEW).submenu->removeMenuItemById (ID_SELECT_SCENE);
	menubar->getMenuItem(AS_MENUITEM_VIEW).submenu->removeMenuItemById (ID_SELECT_BKIMAGE);

	menubar->removeMenuItem (AS_MENUITEM_CUSTOM);
}

void PluginComposition::clear (void)
{
	ATOM_DELETE(_leftPanel);
	m_ActorTree = 0;
	m_ActorCell = 0;
	_leftPanel = 0;

	ATOM_DELETE(_timeLine);
	_timeLine = 0;

	ATOM_DELETE(_actorPropertyBar);
	_actorPropertyBar = 0;

	ATOM_DELETE(_keyValueBar);
	_keyValueBar = 0;

	_editor->getRealtimeCtrl()->setScene (NULL);
	ATOM_DELETE(_scene);
	_scene = 0;

	_grid = 0;
	_node = 0;
	_currentActor = "";

	_bkImage = 0;

	_actorMap.clear ();
}

bool PluginComposition::initGUI (void)
{
	_leftPanel = _editor->getRealtimeCtrl()->loadHierarchy ("/editor/ui/composition_left_bar.ui");

	if (_leftPanel)
	{
		initControls (_leftPanel);

		int particleSysId = m_ActorCell->getAttributes()->getAttribute ("ParticleSysId").getI();
		int shapeId = m_ActorCell->getAttributes()->getAttribute ("ShapeId").getI();
		int modelId = m_ActorCell->getAttributes()->getAttribute ("ModelId").getI();
		int lightId = m_ActorCell->getAttributes()->getAttribute ("lightId").getI();
		int trailId = m_ActorCell->getAttributes()->getAttribute ("TrailId").getI();
		int weaponTrailId = m_ActorCell->getAttributes()->getAttribute ("WeaponTrailId").getI();
		int decalId = m_ActorCell->getAttributes()->getAttribute ("DecalId").getI();
		m_ActorCell->setCellData (0, 0, particleSysId, NULL, 0, 0);
		m_ActorCell->setCellData (1, 0, shapeId, NULL, 0, 0);
		m_ActorCell->setCellData (2, 0, modelId, NULL, 0, 0);
		m_ActorCell->setCellData (0, 1, lightId, NULL, 0, 0);
		m_ActorCell->setCellData (1, 1, trailId, NULL, 0, 0);
		m_ActorCell->setCellData (2, 1, weaponTrailId, NULL, 0, 0);
		m_ActorCell->setCellData (0, 2, decalId, NULL, 0, 0);
		m_ActorCell->allowCellDragDrop (true);

		m_ActorTree->allowItemDragDrop (true);
		m_ActorTree->setTextImageId (ATOM_IMAGEID_TREEITEM_HILIGHT);
		m_ActorTree->setStyle (m_ActorTree->getStyle()|ATOM_Widget::ClipChildren);

		_timeLine = ATOM_NEW(TimeLine, _editor->getRealtimeCtrl(), ATOM_Rect2Di(0, 0, 100, 100), ATOM_Widget::Control, ID_TIMELINE, _node->getTimeLineDuration());
		_timeLine->setCurrentSlice (_node->getTimeLinePosition (), 0);

		_actorPropertyBar = ATOM_NEW(ATOMX_PropertyTweakBar, "属性");
		_actorPropertyBar->setBarColor (0.5f, 0.5f, 0.5f, 1.f);
		_actorPropertyBar->setBarMovable (false);
		_actorPropertyBar->setBarResizable (false);
		_actorPropertyBar->setBarIconifiable (false);

		_keyValueBar = ATOM_NEW(ATOMX_PropertyTweakBar, "关键帧数据");
		_keyValueBar->setBarColor (0.5f, 0.5f, 0.5f, 1.f);
		_keyValueBar->setBarMovable (false);
		_keyValueBar->setBarResizable (false);
		_keyValueBar->setBarIconifiable (false);

		refreshActorList ();

		return true;
	}

	return false;
}

void PluginComposition::calcUILayout (void)
{
	ATOM_Rect2Di rc = _editor->getRealtimeCtrl()->getClientRect();

	_leftPanel->resize (ATOM_Rect2Di(0, 0, 210, rc.size.h - 40));
	_timeLine->resize (ATOM_Rect2Di(0, rc.size.h - 40, rc.size.w, 40));
	
	ATOM_Point2Di pt(rc.size.w - 250, 0);
	_editor->getRealtimeCtrl()->clientToGUI (&pt);
	_actorPropertyBar->setBarPosition (pt.x, pt.y);
	_actorPropertyBar->setBarSize (250, rc.size.h - 400);
	_keyValueBar->setBarPosition (pt.x, pt.y + rc.size.h - 400 + 1);
	_keyValueBar->setBarSize (250, 400 -1 - 40);
}

ATOM_CompositionActor *PluginComposition::findActorByName (const char *name) const
{
	if (name)
	{
		ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_CompositionActor)>::const_iterator it = _actorMap.find (name);
		return it == _actorMap.end () ? 0 : it->second.get();
	}
	return 0;
}

void PluginComposition::setupActorPropertyBar (ATOM_CompositionActor *actor)
{
			_actorPropertyBar->setObject (actor ? actor->getNode() : NULL);

	ATOM_ShapeNode *shapeNode = actor ? dynamic_cast<ATOM_ShapeNode*>(actor->getNode()) : 0;
	if (shapeNode)
	{
		setupMaterialTweakBar (_actorPropertyBar, shapeNode->getMaterial());
	}
}

void PluginComposition::setupMaterialTweakBar (ATOMX_TweakBar *bar, ATOM_Material *material)
{
	if (material)
	{
		ATOM_ParameterTable *paramTable = material->getParameterTable ();
		for (int i = 0; i < paramTable->getNumParameters(); ++i)
		{
			ATOM_ParameterTable::ValueHandle value = paramTable->getParameter (i);

			if (value)
			{
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
						bar->addBoolVar (paramName, ID_MATERIAL_PARAM, (*value->i) != 0, false, "材质", material);
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
				case ATOM_MaterialParam::ParamEditorType_Constant_BlendFunc:
					{
						ATOMX_TBEnum enumBlendFunc;
						enumBlendFunc.addEnum ("One", ATOM_RenderAttributes::BlendFunc_One);
						enumBlendFunc.addEnum ("Zero", ATOM_RenderAttributes::BlendFunc_Zero);
						enumBlendFunc.addEnum ("SrcAlpha", ATOM_RenderAttributes::BlendFunc_SrcAlpha);
						enumBlendFunc.addEnum ("InvSrcAlpha", ATOM_RenderAttributes::BlendFunc_InvSrcAlpha);
						enumBlendFunc.addEnum ("SrcColor", ATOM_RenderAttributes::BlendFunc_SrcColor);
						enumBlendFunc.addEnum ("InvSrcColor", ATOM_RenderAttributes::BlendFunc_InvSrcColor);
						bar->addEnum (paramName, ID_MATERIAL_PARAM, *value->i, enumBlendFunc, false, "材质", material);
						break;
					}
				case ATOM_MaterialParam::ParamEditorType_Constant_CullMode:
					{
						ATOMX_TBEnum enumCullMode;
						enumCullMode.addEnum ("None", ATOM_RenderAttributes::CullMode_None);
						enumCullMode.addEnum ("Front", ATOM_RenderAttributes::CullMode_Front);
						enumCullMode.addEnum ("Back", ATOM_RenderAttributes::CullMode_Back);
						bar->addEnum (paramName, ID_MATERIAL_PARAM, *value->i, enumCullMode, false, "材质", material);
						break;
					}
				}
			}
		}
	}
}

void PluginComposition::selectActorByName (const char *name)
{
	const char *newActorName = name ? name : "";

	_currentActor = newActorName;

	ATOM_CompositionActor *actor = findActorByName (_currentActor.c_str());

	setupActorPropertyBar (actor);

	if (dynamic_cast<ATOM_ParticleSystemActor*>(actor))
	{
		setupKeyframeBarPS (actor);
	}
	else if (dynamic_cast<ATOM_GeodeActor*>(actor))
	{
		setupKeyframeBarGeode (actor);
	}
	else if (dynamic_cast<ATOM_ShapeActor*>(actor))
	{
		setupKeyframeBarShape (actor);
	}
	else if (dynamic_cast<ATOM_LightActor*>(actor))
	{
		setupKeyframeBarLight (actor);
	}
	else if (dynamic_cast<ATOM_TrailActor*>(actor))
	{
		setupKeyframeBarTrail (actor);
	}
	else if (dynamic_cast<ATOM_WeaponTrailActor*>(actor))
	{
		setupKeyframeBarWeaponTrail (actor);
	}
	else if (dynamic_cast<ATOM_DecalActor*>(actor))
	{
		setupKeyframeBarDecal (actor);
	}
	else
	{
		_keyValueBar->setObject (0);
	}

	if (actor)
	{
		_actorPropertyBar->addBoolVar ("isRef", ID_MAKEREFERENCE, actor->isReference(), false, "演员属性");
		_actorPropertyBar->setVarLabel ("isRef", "参考物");
		_actorPropertyBar->addBoolVar ("noReset", ID_NORESET, !actor->isResetAllowed(), false, "演员属性");
		_actorPropertyBar->setVarLabel ("noReset", "循环不归零");
	}
	else
	{
		_actorPropertyBar->remove ("isRef");
	}

	_actorPropertyBar->refresh ();

	_timeLine->setActor (actor);
}

const char *PluginComposition::findActorByNode (ATOM_Node *node) const
{
	for (ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_CompositionActor)>::const_iterator it = _actorMap.begin(); it != _actorMap.end(); ++it)
	{
		if (it->second->getNode() == node)
		{
			return it->first.c_str();
		}
	}
	return 0;
}

bool PluginComposition::updateActorMap (void)
{
	_actorMap.clear ();

	for (unsigned i = 0; i < _node->getNumTopActors(); ++i)
	{
		if (!updateActorMapR (_node->getTopActor (i)))
		{
			return false;
		}
	}

	return true;
}

bool PluginComposition::updateActorMapR (ATOM_CompositionActor *actor)
{
	if (_actorMap.find (actor->getName()) != _actorMap.end ())
	{
		return false;
	}
	_actorMap[actor->getName()] = actor;

	for (unsigned i = 0; i < actor->getNumChildActors(); ++i)
	{
		if (!updateActorMapR (actor->getChildActor (i)))
		{
			return false;
		}
	}
	return true;
}

void PluginComposition::toggleGUI (void)
{
	_guiShown = !_guiShown;

	if (_guiShown)
	{
		_leftPanel->show (ATOM_Widget::ShowNormal);
		_timeLine->show (true);
		_actorPropertyBar->setBarVisible (true);
		_keyValueBar->setBarVisible (true);
	}
	else
	{
		_leftPanel->show (ATOM_Widget::Hide);
		_timeLine->show (false);
		_actorPropertyBar->setBarVisible (false);
		_keyValueBar->setBarVisible (false);
	}
}

void PluginComposition::toggleGrid (void)
{
	if (_grid->getShow () != ATOM_Node::SHOW)
		_grid->setShow (ATOM_Node::SHOW);
	else
		_grid->setShow (ATOM_Node::HIDE);
}

void PluginComposition::selectScene (void)
{
	if (1 == _editor->getOpenFileNames ("3sg", "场景文件(*.3sg)|*.3sg|", false, false, "选择场景"))
	{
		_scene->getRootNode()->clearChildren ();
		_scene->load (_editor->getOpenedFileName(0));
		_scene->getRootNode()->appendChild (_grid.get());
		_scene->getRootNode()->appendChild (_node.get());
		if (_bkImage)
		{
			_scene->getRootNode()->appendChild (_bkImage.get());
		}
	}
}

void PluginComposition::selectBkImage (void)
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

void PluginComposition::renameCurrentActor (void)
{
	if (!_currentActor.empty ())
	{
		ATOM_STRING newName = _currentActor;

		if (_editor->showRenameDialog (newName))
		{
			if (newName != _currentActor)
			{
				if (_actorMap.find (newName) != _actorMap.end())
				{
					MessageBoxA (ATOM_APP->getMainWindow(), "名字重复!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
				}
				else
				{
					ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_CompositionActor)>::iterator itOld = _actorMap.find(_currentActor);
					itOld->second->setName (newName.c_str());
					_actorMap[newName] = itOld->second;
					_actorMap.erase (itOld);

					refreshActorList ();
				}
			}
		}
	}
}

void PluginComposition::deleteCurrentActor (void)
{
	if (!_currentActor.empty ())
	{
		ATOM_CompositionActor *actor = findActorByName (_currentActor.c_str());
		ATOM_CompositionActor *parentActor = actor->getParentActor ();
		if (parentActor)
		{
			parentActor->removeChildActor (actor);
		}
		else
		{
			_node->removeActor (actor);
		}
		_actorMap.erase (_actorMap.find (_currentActor));

		selectActorByName ("");

		refreshActorList ();
	}
}

ATOM_STRING PluginComposition::newName (void) const
{
	const char *name = "新建";
	int i = 1;
	char buffer[256];

	for (;;)
	{
		sprintf (buffer, "%s%d", name, i++);
		if (_actorMap.find (buffer) == _actorMap.end ())
		{
			return buffer;
		}
	}
}

class CameraPathTrigger: public ATOM_EventTrigger
{
public:

	void begin (ATOM_Widget *parent, ATOM_Camera *camera, ATOM_CompositionNode *pathNode)
	{
		if (!pathNode)
		{
			return;
		}

		ATOM_CompositionActor *actorCameraSource = pathNode->getActorByName (CAMERA_SOURCE_NAME);
		ATOM_CompositionActor *actorCameraTarget = pathNode->getActorByName (CAMERA_TARGET_NAME);
		if (!actorCameraSource || !actorCameraTarget)
		{
			return;
		}

		_sourceKey = actorCameraSource->getTranslateTrack ();
		_targetKey = actorCameraTarget->getTranslateTrack ();

		int numKeySource = _sourceKey->getNumKeys ();
		if (!numKeySource)
		{
			return;
		}

		unsigned minKeyTimeSource = _sourceKey->getKeyTime (0);
		unsigned maxKeyTimeSource = _sourceKey->getKeyTime (numKeySource-1);

		int numKeyTarget = _targetKey->getNumKeys ();
		unsigned minKeyTimeTarget = numKeyTarget>0 ? _targetKey->getKeyTime (0) : minKeyTimeSource;
		unsigned maxKeyTimeTarget = numKeyTarget>0 ? _targetKey->getKeyTime (numKeyTarget-1) : minKeyTimeSource;

		_minKeyTime = ATOM_min2(minKeyTimeSource, minKeyTimeTarget);
		_maxKeyTime = ATOM_max2(maxKeyTimeSource, maxKeyTimeTarget);
		_fixedTarget = numKeyTarget == 0;
		if (_fixedTarget)
		{
			const ATOM_Matrix4x4f &Ma = actorCameraTarget->getNode()->getWorldMatrix ();
			ATOM_Matrix4x4f Mc = pathNode->getInvWorldMatrix();
			ATOM_Matrix4x4f M = Mc >> Ma;
			_fixedTargetPosition.set(M.m30, M.m31, M.m32);
		}

		_showTime = ATOM_APP->getFrameStamp().currentTick;
		_camera = camera;

		ATOM_Matrix4x4f viewMatrixOld = _camera->getViewMatrix();

		ATOM_Dialog dlg(parent, ATOM_Rect2Di(0, 0, 1, 1), 0);
		dlg.setClientImageId (ATOM_INVALID_IMAGEID);
		dlg.setEventTrigger (this);
		dlg.showModal ();

		_camera->setViewMatrix (viewMatrixOld);
	}

private:
	void onIdle (ATOM_WidgetIdleEvent *event)
	{
		setAutoCallHost (false);

		unsigned t = _minKeyTime + ATOM_APP->getFrameStamp().currentTick - _showTime;
		if (t > _maxKeyTime)
		{
			ATOM_Dialog *dlg = (ATOM_Dialog*)getHost();
			dlg->endModal (0);
		}
		else
		{
			ATOM_Vector3f eye = _sourceKey->eval (t);
			ATOM_Vector3f at = _fixedTarget ? _fixedTargetPosition : _targetKey->eval (t);
			_camera->lookAt (eye, at, ATOM_Vector3f(0.f, 1.f, 0.f));
		}
	}

private:
	ATOM_AUTOPTR(ATOM_CompositionTrackT<ATOM_TranslateKeyFrame>) _sourceKey;
	ATOM_AUTOPTR(ATOM_CompositionTrackT<ATOM_TranslateKeyFrame>) _targetKey;
	ATOM_Camera *_camera;
	long _minKeyTime;
	long _maxKeyTime;
	long _showTime;
	bool _fixedTarget;
	ATOM_Vector3f _fixedTargetPosition;

	ATOM_DECLARE_EVENT_MAP(CameraPathTrigger, ATOM_EventTrigger)
};

ATOM_BEGIN_EVENT_MAP(CameraPathTrigger, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(CameraPathTrigger, ATOM_WidgetIdleEvent, onIdle)
ATOM_END_EVENT_MAP

void PluginComposition::testCameraPath (void)
{
	CameraPathTrigger trigger;
	trigger.begin (_editor->getRealtimeCtrl(), _editor->getRealtimeCtrl()->getScene()->getCamera(), _node.get());
}

bool PluginComposition::exportCameraPath (const char *filename)
{
	if (!_node)
	{
		return false;
	}

	ATOM_CompositionActor *actorCameraSource = _node->getActorByName (CAMERA_SOURCE_NAME);
	ATOM_CompositionActor *actorCameraTarget = _node->getActorByName (CAMERA_TARGET_NAME);
	if (!actorCameraSource || !actorCameraTarget)
	{
		return false;
	}

	ATOM_CompositionTrackT<ATOM_TranslateKeyFrame> *trackSource = actorCameraSource->getTranslateTrack ();
	ATOM_CompositionTrackT<ATOM_TranslateKeyFrame> *trackTarget = actorCameraTarget->getTranslateTrack ();

	char buffer[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (filename, buffer);
	ATOM_TiXmlDocument doc(buffer);

	int numKeySource = trackSource->getNumKeys ();
	if (!numKeySource)
	{
		return false;
	}
	unsigned minKeyTimeSource = trackSource->getKeyTime (0);
	unsigned maxKeyTimeSource = trackSource->getKeyTime (numKeySource-1);

	int numKeyTarget = trackTarget->getNumKeys ();
	unsigned minKeyTimeTarget = numKeyTarget>0 ? trackTarget->getKeyTime (0) : minKeyTimeSource;
	unsigned maxKeyTimeTarget = numKeyTarget>0 ? trackTarget->getKeyTime (numKeyTarget-1) : minKeyTimeSource;

	unsigned minKeyTime = ATOM_min2(minKeyTimeSource, minKeyTimeTarget);
	unsigned maxKeyTime = ATOM_max2(maxKeyTimeSource, maxKeyTimeTarget);

	ATOM_TiXmlElement ePath("CameraPath");
	ePath.SetAttribute ("Duration", (int)(maxKeyTime - minKeyTime));
	if (numKeyTarget == 0)
	{
		const ATOM_Matrix4x4f &Ma = actorCameraTarget->getNode()->getWorldMatrix ();
		ATOM_Matrix4x4f Mc = _node->getInvWorldMatrix();
		ATOM_Matrix4x4f M = Mc >> Ma;
		sprintf (buffer, "%f,%f,%f", M.m30, M.m31, M.m32);
		ePath.SetAttribute ("Target", buffer);
	}

	ATOM_TiXmlElement eSource("Source");
	eSource.SetAttribute ("WaveType", int(trackSource->getWaveType()));
	eSource.SetAttribute ("Address", int(trackSource->getAddress()));
	for (int i = 0; i < numKeySource; ++i)
	{
		ATOM_TiXmlElement eKey("Key");
		eKey.SetAttribute("Time", (int)(trackSource->getKeyTime(i)-minKeyTime));
		const ATOM_Vector3f &value = trackSource->getKeyValueByIndex(i);
		sprintf (buffer, "%f,%f,%f", value.x, value.y, value.z);
		eKey.SetAttribute("Value", buffer);

		eSource.InsertEndChild (eKey);
	}
	ePath.InsertEndChild (eSource);

	if (numKeyTarget)
	{
		ATOM_TiXmlElement eTarget("Target");
		eTarget.SetAttribute ("WaveType", int(trackTarget->getWaveType()));
		eTarget.SetAttribute ("Address", int(trackTarget->getAddress()));
		for (int i = 0; i < numKeyTarget; ++i)
		{
			ATOM_TiXmlElement eKey("Key");
			eKey.SetAttribute("Time", (int)(trackTarget->getKeyTime(i)-minKeyTime));
			const ATOM_Vector3f &value = trackTarget->getKeyValueByIndex(i);
			sprintf (buffer, "%f,%f,%f", value.x, value.y, value.z);
			eKey.SetAttribute("Value", buffer);

			eTarget.InsertEndChild (eKey);
		}
		ePath.InsertEndChild (eTarget);
	}

	doc.InsertEndChild (ePath);

	return doc.SaveFile ();
}

void PluginComposition::handleScenePropChanged (void)
{
}

void PluginComposition::changeRenderScheme (void)
{
	_scene->setRenderScheme (_editor->getRenderScheme());
}

