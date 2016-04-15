#include "StdAfx.h"
#include "composition_actor.h"
#include "composition_keyframevalue.h"

ATOM_CompositionActor::ATOM_CompositionActor (void)
{
	_parentActor = nullptr;
	_isReference = false;
	_noReset = false;
	_compositionNode = nullptr;
	_translateTrack = ATOM_NEW(ATOM_CompositionTrackT<ATOM_TranslateKeyFrame>, ATOM_WAVE_TYPE_SPLINE);
	_scaleTrack = ATOM_NEW(ATOM_CompositionTrackT<ATOM_ScaleKeyFrame>, ATOM_WAVE_TYPE_LINEAR);
	_rotateTrack = ATOM_NEW(ATOM_CompositionTrackT<ATOM_RotateKeyFrame>, ATOM_WAVE_TYPE_LINEAR);
	_visibleTrack = ATOM_NEW(ATOM_CompositionTrackT<ATOM_VisibleKeyFrame>, ATOM_WAVE_TYPE_STEP);
}

ATOM_CompositionActor::~ATOM_CompositionActor (void)
{
}

void ATOM_CompositionActor::captureKeyFrameValue (ATOM_CompositionKeyFrameValue *value) const
{
	ATOM_Node *node = getNode ();
	const ATOM_Matrix4x4f &mat = node ? node->getO2T() : ATOM_Matrix4x4f::getIdentityMatrix();
	ATOM_Vector3f t, s, euler;
	ATOM_Matrix4x4f r;
	if (node)
	{
		mat.decompose (t, r, s);
		ATOM_Quaternion q;
		q.fromMatrix (r);
		euler = q.toEulerXYZ ();
	}
	else
	{
		t.set (0.f, 0.f, 0.f);
		s.set (1.f, 1.f, 1.f);
		euler.set (0.f, 0.f, 0.f);
	}

	value->setTranslation (t);
	value->setScale (s);
	value->setRotation (euler);
	value->setVisible (node && node->getShow() == ATOM_Node::SHOW ? 1 : 0);
}

unsigned ATOM_CompositionActor::getNumChildActors (void) const
{
	return _childActors.size();
}

ATOM_CompositionActor *ATOM_CompositionActor::getParentActor (void) const
{
	return _parentActor;
}

ATOM_CompositionActor *ATOM_CompositionActor::getChildActor (unsigned index) const
{
	return _childActors[index].get();
}

ATOM_CompositionActor *ATOM_CompositionActor::getActorByName (const char *name)
{
	if (!name)
	{
		return 0;
	}

	if (!strcmp (getName(), name))
	{
		return this;
	}

	for (int i = 0; i < _childActors.size(); ++i)
	{
		ATOM_CompositionActor *actor = _childActors[i]->getActorByName (name);
		if (actor)
		{
			return actor;
		}
	}

	return 0;
}

void ATOM_CompositionActor::addChildActor (ATOM_CompositionActor *actor)
{
	_childActors.push_back (actor);
	actor->_parentActor = this;
	getNode()->appendChild (actor->getNode());

	ATOM_CompositionNode *node = getCompositionNode ();
	if (node)
	{
		node->invalidateGroup ();
	}
}

void ATOM_CompositionActor::removeChildActor (ATOM_CompositionActor *actor)
{
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_CompositionActor)>::iterator it = std::find (_childActors.begin(), _childActors.end(), actor);
	if (it != _childActors.end())
	{
		getNode()->removeChild ((*it)->getNode());
		(*it)->_parentActor = 0;
		_childActors.erase (it);

		ATOM_CompositionNode *node = getCompositionNode ();
		if (node)
		{
			node->invalidateGroup ();
		}
	}
}

void ATOM_CompositionActor::removeAllActors (void)
{
	while (!_childActors.empty ())
	{
		removeChildActor (_childActors.back().get());
	}
}

ATOM_CompositionNode *ATOM_CompositionActor::getCompositionNode (void) const
{
	return _parentActor ? _parentActor->getCompositionNode() : _compositionNode;
}

void ATOM_CompositionActor::setCompositionNode (ATOM_CompositionNode *node)
{
	_compositionNode = node;
}

void ATOM_CompositionActor::reset (void)
{
	if (!_noReset)
	{
		doReset ();
	}

	for (unsigned i = 0; i < _childActors.size(); ++i)
	{
		_childActors[i]->reset ();
	}
}

void ATOM_CompositionActor::setName (const char *name)
{
	if (_name != name)
	{
		_name = name;

		ATOM_CompositionNode *node = getCompositionNode ();
		if (node)
		{
			node->invalidateGroup ();
		}
	}
}

const char *ATOM_CompositionActor::getName (void) const
{
	return _name.c_str();
}

void ATOM_CompositionActor::makeReference (bool ref)
{
	_isReference = ref;
}

bool ATOM_CompositionActor::isReference (void) const
{
	return _isReference;
}

void ATOM_CompositionActor::allowReset (bool allow)
{
	_noReset = !allow;
}

bool ATOM_CompositionActor::isResetAllowed (void) const
{
	return !_noReset;
}

void ATOM_CompositionActor::saveToXML (ATOM_TiXmlElement *xml) const
{
	xml->SetAttribute ("Name", getName());

	if (_translateTrack->getNumKeys() > 0)
	{
		ATOM_TiXmlElement eTranslate("TranslateTrack");
		_translateTrack->saveToXML (&eTranslate);
		xml->InsertEndChild (eTranslate);

		ATOM_TiXmlElement eScale("ScaleTrack");
		_scaleTrack->saveToXML (&eScale);
		xml->InsertEndChild (eScale);

		ATOM_TiXmlElement eRotate("RotationTrack");
		_rotateTrack->saveToXML (&eRotate);
		xml->InsertEndChild (eRotate);

		ATOM_TiXmlElement eVisible("VisibleTrack");
		_visibleTrack->saveToXML (&eVisible);
		xml->InsertEndChild (eVisible);
	}
}

bool ATOM_CompositionActor::loadFromXML (const ATOM_TiXmlElement *xml)
{
	const char *name = xml->Attribute ("Name");
	setName (name ? name : "");

	const ATOM_TiXmlElement *eTranslate = xml->FirstChildElement ("TranslateTrack");
	if (eTranslate)
	{
		_translateTrack->loadFromXML (eTranslate);
	}

	const ATOM_TiXmlElement *eScale = xml->FirstChildElement ("ScaleTrack");
	if (eScale)
	{
		_scaleTrack->loadFromXML (eScale);
	}

	const ATOM_TiXmlElement *eRotate = xml->FirstChildElement ("RotationTrack");
	if (eRotate)
	{
		_rotateTrack->loadFromXML (eRotate);
	}
	else
	{
		for (unsigned i = 0; i < _translateTrack->getNumKeys (); ++i)
		{
			_rotateTrack->insertKeyFrame (_translateTrack->getKeyTime(i), ATOM_NEW(ATOM_RotateKeyFrame, 0.f));
		}
	}

	const ATOM_TiXmlElement *eVisible = xml->FirstChildElement ("VisibleTrack");
	if (eVisible)
	{
		_visibleTrack->loadFromXML (eVisible);
	}

	return true;
}

ATOM_CompositionTrackT<ATOM_TranslateKeyFrame> *ATOM_CompositionActor::getTranslateTrack (void) const
{
	return _translateTrack.get();
}

ATOM_CompositionTrackT<ATOM_ScaleKeyFrame> *ATOM_CompositionActor::getScaleTrack (void) const
{
	return _scaleTrack.get();
}

ATOM_CompositionTrackT<ATOM_RotateKeyFrame> *ATOM_CompositionActor::getRotateTrack (void) const
{
	return _rotateTrack.get();
}

ATOM_CompositionTrackT<ATOM_VisibleKeyFrame> *ATOM_CompositionActor::getVisibleTrack (void) const
{
	return _visibleTrack.get();
}

unsigned ATOM_CompositionActor::getNumKeys (void) const
{
	return _translateTrack->getNumKeys();
}

long ATOM_CompositionActor::getKeyTime (unsigned index) const
{
	return _translateTrack->getKeyTime (index);
}

bool ATOM_CompositionActor::addKeyFrame (long time, const ATOM_CompositionKeyFrameValue *value)
{
	_translateTrack->insertKeyFrame (time, ATOM_NEW(ATOM_TranslateKeyFrame, value->getTranslation()));
	_scaleTrack->insertKeyFrame (time, ATOM_NEW(ATOM_ScaleKeyFrame, value->getScale()));
	_rotateTrack->insertKeyFrame (time, ATOM_NEW(ATOM_RotateKeyFrame, value->getRotation()));
	_visibleTrack->insertKeyFrame (time, ATOM_NEW(ATOM_VisibleKeyFrame, value->getVisible()));

	return true;
}

bool ATOM_CompositionActor::removeKeyFrame (long time)
{
	unsigned index = _translateTrack->findKeyFrame (time);
	if (index == (unsigned)-1)
	{
		return false;
	}

	_translateTrack->removeKeyFrame (index);
	_scaleTrack->removeKeyFrame (index);
	_rotateTrack->removeKeyFrame (index);
	_visibleTrack->removeKeyFrame (index);

	return true;
}

void ATOM_CompositionActor::evalKeyFrameValue (long time, ATOM_CompositionKeyFrameValue *value) const
{
	const ATOM_Vector3f &t = _translateTrack->eval (time);
	const ATOM_Vector3f &s = _scaleTrack->eval (time);
	const ATOM_Vector3f &r = _rotateTrack->eval (time);

	value->setTranslation(t);
	value->setScale(s);
	value->setRotation(r);
	value->setVisible (_visibleTrack->eval(time));
}

bool ATOM_CompositionActor::getKeyFrame (long time, ATOM_CompositionKeyFrameValue *value) const
{
	unsigned i = _translateTrack->findKeyFrame (time);
	if (i != (unsigned)-1)
	{
		value->setTranslation (_translateTrack->getKeyValueByIndex (i));
		value->setScale (_scaleTrack->getKeyValueByIndex (i));
		value->setRotation (_rotateTrack->getKeyValueByIndex (i));
		value->setVisible (_visibleTrack->getKeyValueByIndex (i));

		return true;
	}

	return false;
}

ATOM_CompositionKeyFrameValue *ATOM_CompositionActor::createKeyFrameValue (void) const
{
	return ATOM_NEW(ATOM_CompositionKeyFrameValue);
}

void ATOM_CompositionActor::start (void)
{
}

void ATOM_CompositionActor::pause (void)
{
}

void ATOM_CompositionActor::doReset (void)
{
}

//--- wangjian added ---//
bool ATOM_CompositionActor::checkNodeAllLoadFinshed()
{
	if( getNode() )
	{
		if( !getNode()->checkNodeAllFinshed() )
			return false;
	}
	for( int i = 0 ; i < getNumChildActors(); ++i )
	{
		ATOM_CompositionActor * actor_child = getChildActor(i);
		if( actor_child )
		{
			if( !actor_child->checkNodeAllLoadFinshed() )
				return false;
		}
	}
	return true;
}
//----------------------//

///////////////////////////////////////////////////////////////////////////////////////////

ATOM_ParticleSystemActor::ATOM_ParticleSystemActor (void)
{
	_psTrack = ATOM_NEW(ATOM_CompositionTrackT<ATOM_ParticleSystemKeyFrame>, ATOM_WAVE_TYPE_LINEAR);
}

ATOM_CompositionActor *ATOM_ParticleSystemActor::clone (void) const
{
	return cloneActor (this);
}

void ATOM_ParticleSystemActor::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_CompositionActorT<ATOM_ParticleSystem>::saveToXML (xml);

	if (_psTrack->getNumKeys() > 0)
	{
		ATOM_TiXmlElement ePS("PSTrack");
		_psTrack->saveToXML (&ePS);
		xml->InsertEndChild (ePS);
	}
}

bool ATOM_ParticleSystemActor::loadFromXML (const ATOM_TiXmlElement *xml)
{
	if (!ATOM_CompositionActorT<ATOM_ParticleSystem>::loadFromXML (xml))
	{
		return false;
	}

	const ATOM_TiXmlElement *ePS = xml->FirstChildElement ("PSTrack");
	if (ePS)
	{
		_psTrack->loadFromXML (ePS);
	}
	else
	{
		for (unsigned i = 0; i < getNumKeys(); ++i)
		{
			ATOM_AUTOPTR(ATOM_ParticleSystemKeyFrame) keyFrame = ATOM_NEW(ATOM_ParticleSystemKeyFrame);
			ATOM_ParticleSystem *ps = (ATOM_ParticleSystem*)getNode();
			ATOM_PSKeyFrameData data;
			data.emitInterval = ps->getEmitInterval();
			data.emitCount = ps->getEmitCount();
			data.emitShapeSize = ps->getEmitterShapeSize();
			data.emitShapeSizeVar  = ps->getEmitterShapeSizeVar();
			data.emitCone = ps->getEmitterConeRadius();
			data.emitConeVar = ps->getEmitterConeRadiusVar();
			data.velocity = ps->getParticleVelocity();
			data.velocityVar = ps->getParticleVelocityVar();
			data.life = ps->getParticleLife();
			data.lifeVar = ps->getParticleLifeVar();
			data.size1 = ps->getParticleSize1();
			data.size1Var = ps->getParticleSize1Var();
			data.size2 = ps->getParticleSize2();
			data.size2Var = ps->getParticleSize2Var();
			data.accel = ps->getParticleAccel();
			data.accelVar = ps->getParticleAccelVar();
			data.gravity = ps->getGravity();
			data.wind = ps->getWind();
			data.scale = ps->getScalar();
			data.transparency = ps->getTransparency();
			data.color = ps->getColor();
			//--- wangjian added ---//
			data.colorMultiplier = ps->getColorMultiplier();
			//----------------------//
			keyFrame->setValue(data);

			_psTrack->insertKeyFrame (getKeyTime(i), keyFrame.get());
		}
	}

	return true;
}

bool ATOM_ParticleSystemActor::addKeyFrame (long time, const ATOM_CompositionKeyFrameValue *value)
{
	if (!ATOM_CompositionActorT<ATOM_ParticleSystem>::addKeyFrame (time, value))
	{
		return false;
	}

	ATOM_ParticleSystemKeyFrameValue *keyFrameValue = (ATOM_ParticleSystemKeyFrameValue*)value;
	const ATOM_PSKeyFrameData &data = keyFrameValue->getParticleSystemParams ();
	_psTrack->insertKeyFrame (time, ATOM_NEW(ATOM_ParticleSystemKeyFrame, data));

	return true;
}

bool ATOM_ParticleSystemActor::removeKeyFrame (long time)
{
	if (!ATOM_CompositionActorT<ATOM_ParticleSystem>::removeKeyFrame (time))
	{
		return false;
	}

	unsigned index = _psTrack->findKeyFrame (time);
	if (index == (unsigned)-1)
	{
		return false;
	}

	_psTrack->removeKeyFrame (index);

	return true;
}

void ATOM_ParticleSystemActor::captureKeyFrameValue (ATOM_CompositionKeyFrameValue *value) const
{
	ATOM_CompositionActorT<ATOM_ParticleSystem>::captureKeyFrameValue(value);

	ATOM_ParticleSystemKeyFrameValue *keyFrameValue = (ATOM_ParticleSystemKeyFrameValue*)value;
	ATOM_ParticleSystem *ps = (ATOM_ParticleSystem*)getNode();
	ATOM_PSKeyFrameData data;
	data.emitInterval = ps->getEmitInterval();
	data.emitCount = ps->getEmitCount();
	data.emitShapeSize = ps->getEmitterShapeSize();
	data.emitShapeSizeVar  = ps->getEmitterShapeSizeVar();
	data.emitCone = ps->getEmitterConeRadius();
	data.emitConeVar = ps->getEmitterConeRadiusVar();
	data.velocity = ps->getParticleVelocity();
	data.velocityVar = ps->getParticleVelocityVar();
	data.life = ps->getParticleLife();
	data.lifeVar = ps->getParticleLifeVar();
	data.size1 = ps->getParticleSize1();
	data.size1Var = ps->getParticleSize1Var();
	data.size2 = ps->getParticleSize2();
	data.size2Var = ps->getParticleSize2Var();
	data.accel = ps->getParticleAccel();
	data.accelVar = ps->getParticleAccelVar();
	data.gravity = ps->getGravity();
	data.wind = ps->getWind();
	data.scale = ps->getScalar();
	data.transparency = ps->getTransparency();
	data.color = ps->getColor();
	//--- wangjian added ---//
	data.colorMultiplier = ps->getColorMultiplier();

	keyFrameValue->setParticleSystemParams (data);
}

void ATOM_ParticleSystemActor::evalKeyFrameValue (long time, ATOM_CompositionKeyFrameValue *value) const
{
	ATOM_CompositionActorT<ATOM_ParticleSystem>::evalKeyFrameValue (time, value);

	ATOM_ParticleSystemKeyFrameValue *keyFrameValue = (ATOM_ParticleSystemKeyFrameValue*)value;
	keyFrameValue->setParticleSystemParams (_psTrack->eval (time));
}

bool ATOM_ParticleSystemActor::getKeyFrame (long time, ATOM_CompositionKeyFrameValue *value) const
{
	if (!ATOM_CompositionActorT<ATOM_ParticleSystem>::getKeyFrame (time, value))
	{
		return false;
	}

	ATOM_ParticleSystemKeyFrameValue *keyFrameValue = (ATOM_ParticleSystemKeyFrameValue*)value;
	unsigned i = getTranslateTrack()->findKeyFrame (time);
	if (i != (unsigned)-1)
	{
		keyFrameValue->setParticleSystemParams (_psTrack->getKeyValueByIndex (i));
		return true;
	}

	return false;
}

ATOM_CompositionKeyFrameValue *ATOM_ParticleSystemActor::createKeyFrameValue (void) const
{
	return ATOM_NEW(ATOM_ParticleSystemKeyFrameValue);
}

void ATOM_ParticleSystemActor::doReset (void) 
{
	((ATOM_ParticleSystem*)getNode())->reset (true);
}

void ATOM_ParticleSystemActor::update (long time) 
{
	ATOM_CompositionActorT<ATOM_ParticleSystem>::update (time);
	_psTrack->apply (time, _node.get());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_ShapeActor::ATOM_ShapeActor (void)
{
	_shapeTrack = ATOM_NEW(ATOM_CompositionTrackT<ATOM_ShapeKeyFrame>, ATOM_WAVE_TYPE_LINEAR);
}

ATOM_CompositionActor *ATOM_ShapeActor::clone (void) const
{
	return cloneActor (this);
}

void ATOM_ShapeActor::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_CompositionActorT<ATOM_ShapeNode>::saveToXML (xml);

	if (_shapeTrack->getNumKeys() > 0)
	{
		ATOM_TiXmlElement eShape("ShapeTrack");
		_shapeTrack->saveToXML (&eShape);
		xml->InsertEndChild (eShape);
	}
}

bool ATOM_ShapeActor::loadFromXML (const ATOM_TiXmlElement *xml)
{
	ATOM_CompositionActorT<ATOM_ShapeNode>::loadFromXML (xml);

	const ATOM_TiXmlElement *eShape = xml->FirstChildElement ("ShapeTrack");
	if (eShape)
	{
		_shapeTrack->loadFromXML (eShape);
	}
	else
	{
		for (unsigned i = 0; i < getNumKeys(); ++i)
		{
			ATOM_ShapeKeyFrameData data;
			data.transparency = 1.f;
			data.shearing = 0.f;
			data.uvRotation = 0.f;
			data.uvScaleOffset.set(1.f, 1.f, 0.f, 0.f);
			data.size.set (1.f, 1.f, 1.f);
			data.color.set (1.f, 1.f, 1.f, 1.f);
			//--- wangjian added ---//
			data.colorMultiplier = 1.0f;
			data.resolveScale = 0.0f;
			//---------------------//
			 
			_shapeTrack->insertKeyFrame (getTranslateTrack()->getKeyTime(i), ATOM_NEW(ATOM_ShapeKeyFrame, data));
		}
	}

	return true;
}

bool ATOM_ShapeActor::addKeyFrame (long time, const ATOM_CompositionKeyFrameValue *value)
{
	ATOM_CompositionActorT<ATOM_ShapeNode>::addKeyFrame (time, value);

	const ATOM_ShapeKeyFrameValue *shapeKeyFrameValue = (const ATOM_ShapeKeyFrameValue*)value;
	_shapeTrack->insertKeyFrame (time, ATOM_NEW(ATOM_ShapeKeyFrame, shapeKeyFrameValue->getData()));

	return true;
}

bool ATOM_ShapeActor::removeKeyFrame (long time)
{
	if (ATOM_CompositionActorT<ATOM_ShapeNode>::removeKeyFrame (time))
	{
		_shapeTrack->removeKeyFrame (_shapeTrack->findKeyFrame (time));
		return true;
	}
	return false;
}

void ATOM_ShapeActor::captureKeyFrameValue (ATOM_CompositionKeyFrameValue *value) const
{
	ATOM_CompositionActorT<ATOM_ShapeNode>::captureKeyFrameValue (value);
	ATOM_ShapeKeyFrameValue *shapeKeyFrameValue = (ATOM_ShapeKeyFrameValue*)value;
	ATOM_ShapeKeyFrameData data;
	data.transparency = ((ATOM_ShapeNode*)getNode())->getTransparency();
	data.shearing = ((ATOM_ShapeNode*)getNode())->getShearing();
	data.uvRotation = ((ATOM_ShapeNode*)getNode())->getUVRotation();
	data.uvScaleOffset = ((ATOM_ShapeNode*)getNode())->getUVScaleOffset();
	data.size = ((ATOM_ShapeNode*)getNode())->getSize();
	data.color = ((ATOM_ShapeNode*)getNode())->getColor();
	//--- wangjian added ---//
	data.colorMultiplier = ((ATOM_ShapeNode*)getNode())->getColorMultiplier();
	data.resolveScale = ((ATOM_ShapeNode*)getNode())->getResolveScale();
	//----------------------//
	shapeKeyFrameValue->setData (data);
}

void ATOM_ShapeActor::evalKeyFrameValue (long time, ATOM_CompositionKeyFrameValue *value) const
{
	ATOM_CompositionActorT<ATOM_ShapeNode>::evalKeyFrameValue (time, value);
	ATOM_ShapeKeyFrameValue *shapeKeyFrameValue = (ATOM_ShapeKeyFrameValue*)value;
	shapeKeyFrameValue->setData (_shapeTrack->eval(time));
}

bool ATOM_ShapeActor::getKeyFrame (long time, ATOM_CompositionKeyFrameValue *value) const
{
	if (!ATOM_CompositionActorT<ATOM_ShapeNode>::getKeyFrame (time, value))
	{
		return false;
	}

	ATOM_ShapeKeyFrameValue *keyFrameValue = (ATOM_ShapeKeyFrameValue*)value;
	unsigned i = _shapeTrack->findKeyFrame (time);
	if (i != (unsigned)-1)
	{
		keyFrameValue->setData(_shapeTrack->getKeyValueByIndex (i));
		return true;
	}

	return false;
}

ATOM_CompositionKeyFrameValue *ATOM_ShapeActor::createKeyFrameValue (void) const
{
	return ATOM_NEW(ATOM_ShapeKeyFrameValue);
}

void ATOM_ShapeActor::doReset (void) 
{
}

void ATOM_ShapeActor::update (long time) 
{
	ATOM_CompositionActorT<ATOM_ShapeNode>::update (time);

	_shapeTrack->apply (time, getNode());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_WeaponTrailActor::ATOM_WeaponTrailActor (void)
{
	_weaponTrailTrack = ATOM_NEW(ATOM_CompositionTrackT<ATOM_WeaponTrailKeyFrame>, ATOM_WAVE_TYPE_LINEAR);
}

ATOM_CompositionActor *ATOM_WeaponTrailActor::clone (void) const
{
	return cloneActor (this);
}

void ATOM_WeaponTrailActor::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_CompositionActorT<ATOM_WeaponTrail>::saveToXML (xml);

	if (_weaponTrailTrack->getNumKeys() > 0)
	{
		ATOM_TiXmlElement eTrail("WeaponTrailTrack");
		_weaponTrailTrack->saveToXML (&eTrail);
		xml->InsertEndChild (eTrail);
	}
}

bool ATOM_WeaponTrailActor::loadFromXML (const ATOM_TiXmlElement *xml)
{
	ATOM_CompositionActorT<ATOM_WeaponTrail>::loadFromXML (xml);

	const ATOM_TiXmlElement *eTrail = xml->FirstChildElement ("WeaponTrailTrack");
	if (eTrail)
	{
		_weaponTrailTrack->loadFromXML (eTrail);
	}
	else
	{
		for (unsigned i = 0; i < getNumKeys(); ++i)
		{
			ATOM_WeaponTrailKeyFrameData data;
			data.color.set (1.f, 1.f, 1.f, 1.f);
			_weaponTrailTrack->insertKeyFrame (getTranslateTrack()->getKeyTime(i), ATOM_NEW(ATOM_WeaponTrailKeyFrame, data));
		}
	}

	return true;
}

bool ATOM_WeaponTrailActor::addKeyFrame (long time, const ATOM_CompositionKeyFrameValue *value)
{
	ATOM_CompositionActorT<ATOM_WeaponTrail>::addKeyFrame (time, value);

	const ATOM_WeaponTrailKeyFrameValue *weaponTrailKeyFrameValue = (const ATOM_WeaponTrailKeyFrameValue*)value;
	_weaponTrailTrack->insertKeyFrame (time, ATOM_NEW(ATOM_WeaponTrailKeyFrame, weaponTrailKeyFrameValue->getData()));

	return true;
}

bool ATOM_WeaponTrailActor::removeKeyFrame (long time)
{
	if (ATOM_CompositionActorT<ATOM_WeaponTrail>::removeKeyFrame (time))
	{
		_weaponTrailTrack->removeKeyFrame (_weaponTrailTrack->findKeyFrame (time));
		return true;
	}
	return false;
}

void ATOM_WeaponTrailActor::captureKeyFrameValue (ATOM_CompositionKeyFrameValue *value) const
{
	ATOM_CompositionActorT<ATOM_WeaponTrail>::captureKeyFrameValue (value);
	ATOM_WeaponTrailKeyFrameValue *weaponTrailKeyFrameValue = (ATOM_WeaponTrailKeyFrameValue*)value;
	ATOM_WeaponTrailKeyFrameData data;
	data.color = ((ATOM_WeaponTrail*)getNode())->getColor();
	weaponTrailKeyFrameValue->setData (data);
}

void ATOM_WeaponTrailActor::evalKeyFrameValue (long time, ATOM_CompositionKeyFrameValue *value) const
{
	ATOM_CompositionActorT<ATOM_WeaponTrail>::evalKeyFrameValue (time, value);
	ATOM_WeaponTrailKeyFrameValue *weaponTrailKeyFrameValue = (ATOM_WeaponTrailKeyFrameValue*)value;
	weaponTrailKeyFrameValue->setData (_weaponTrailTrack->eval(time));
}

bool ATOM_WeaponTrailActor::getKeyFrame (long time, ATOM_CompositionKeyFrameValue *value) const
{
	if (!ATOM_CompositionActorT<ATOM_WeaponTrail>::getKeyFrame (time, value))
	{
		return false;
	}

	ATOM_WeaponTrailKeyFrameValue *keyFrameValue = (ATOM_WeaponTrailKeyFrameValue*)value;
	unsigned i = _weaponTrailTrack->findKeyFrame (time);
	if (i != (unsigned)-1)
	{
		keyFrameValue->setData(_weaponTrailTrack->getKeyValueByIndex (i));
		return true;
	}

	return false;
}

ATOM_CompositionKeyFrameValue *ATOM_WeaponTrailActor::createKeyFrameValue (void) const
{
	return ATOM_NEW(ATOM_WeaponTrailKeyFrameValue);
}

void ATOM_WeaponTrailActor::doReset (void) 
{
	((ATOM_WeaponTrail*)getNode())->reset ();
}

void ATOM_WeaponTrailActor::update (long time) 
{
	ATOM_WeaponTrail *trail = (ATOM_WeaponTrail*)getNode();
	if (trail)
	{
		trail->setTarget (trail->getParent());

		ATOM_CompositionActorT<ATOM_WeaponTrail>::update (time);

		_weaponTrailTrack->apply (time, getNode());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_TrailActor::ATOM_TrailActor (void)
{
	_trailTrack = ATOM_NEW(ATOM_CompositionTrackT<ATOM_TrailKeyFrame>, ATOM_WAVE_TYPE_LINEAR);
}

ATOM_CompositionActor *ATOM_TrailActor::clone (void) const
{
	return cloneActor (this);
}

void ATOM_TrailActor::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_CompositionActorT<ATOM_RibbonTrail>::saveToXML (xml);

	if (_trailTrack->getNumKeys() > 0)
	{
		ATOM_TiXmlElement eTrail("TrailTrack");
		_trailTrack->saveToXML (&eTrail);
		xml->InsertEndChild (eTrail);
	}
}

bool ATOM_TrailActor::loadFromXML (const ATOM_TiXmlElement *xml)
{
	ATOM_CompositionActorT<ATOM_RibbonTrail>::loadFromXML (xml);

	const ATOM_TiXmlElement *eTrail = xml->FirstChildElement ("TrailTrack");
	if (eTrail)
	{
		_trailTrack->loadFromXML (eTrail);
	}
	else
	{
		for (unsigned i = 0; i < getNumKeys(); ++i)
		{
			ATOM_TrailKeyFrameData data;
			data.colorStart.set (1.f, 1.f, 1.f, 1.f);
			data.colorEnd.set (1.f, 1.f, 1.f, 1.f);
			_trailTrack->insertKeyFrame (getTranslateTrack()->getKeyTime(i), ATOM_NEW(ATOM_TrailKeyFrame, data));
		}
	}

	return true;
}

bool ATOM_TrailActor::addKeyFrame (long time, const ATOM_CompositionKeyFrameValue *value)
{
	ATOM_CompositionActorT<ATOM_RibbonTrail>::addKeyFrame (time, value);

	const ATOM_TrailKeyFrameValue *trailKeyFrameValue = (const ATOM_TrailKeyFrameValue*)value;
	_trailTrack->insertKeyFrame (time, ATOM_NEW(ATOM_TrailKeyFrame, trailKeyFrameValue->getData()));

	return true;
}

bool ATOM_TrailActor::removeKeyFrame (long time)
{
	if (ATOM_CompositionActorT<ATOM_RibbonTrail>::removeKeyFrame (time))
	{
		_trailTrack->removeKeyFrame (_trailTrack->findKeyFrame (time));
		return true;
	}
	return false;
}

void ATOM_TrailActor::captureKeyFrameValue (ATOM_CompositionKeyFrameValue *value) const
{
	ATOM_CompositionActorT<ATOM_RibbonTrail>::captureKeyFrameValue (value);
	ATOM_TrailKeyFrameValue *trailKeyFrameValue = (ATOM_TrailKeyFrameValue*)value;
	ATOM_TrailKeyFrameData data;
	data.colorStart = ((ATOM_RibbonTrail*)getNode())->getStartColor();
	data.colorEnd = ((ATOM_RibbonTrail*)getNode())->getEndColor();
	trailKeyFrameValue->setData (data);
}

void ATOM_TrailActor::evalKeyFrameValue (long time, ATOM_CompositionKeyFrameValue *value) const
{
	ATOM_CompositionActorT<ATOM_RibbonTrail>::evalKeyFrameValue (time, value);
	ATOM_TrailKeyFrameValue *trailKeyFrameValue = (ATOM_TrailKeyFrameValue*)value;
	trailKeyFrameValue->setData (_trailTrack->eval(time));
}

bool ATOM_TrailActor::getKeyFrame (long time, ATOM_CompositionKeyFrameValue *value) const
{
	if (!ATOM_CompositionActorT<ATOM_RibbonTrail>::getKeyFrame (time, value))
	{
		return false;
	}

	ATOM_TrailKeyFrameValue *keyFrameValue = (ATOM_TrailKeyFrameValue*)value;
	unsigned i = _trailTrack->findKeyFrame (time);
	if (i != (unsigned)-1)
	{
		keyFrameValue->setData(_trailTrack->getKeyValueByIndex (i));
		return true;
	}

	return false;
}

ATOM_CompositionKeyFrameValue *ATOM_TrailActor::createKeyFrameValue (void) const
{
	return ATOM_NEW(ATOM_TrailKeyFrameValue);
}

void ATOM_TrailActor::doReset (void) 
{
	((ATOM_RibbonTrail*)getNode())->reset ();
}

void ATOM_TrailActor::update (long time) 
{
	ATOM_CompositionActorT<ATOM_RibbonTrail>::update (time);

	_trailTrack->apply (time, getNode());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_GeodeActor::ATOM_GeodeActor (void)
{
	_actionTrack = ATOM_NEW(ATOM_CompositionTrackT<ATOM_GeodeKeyFrame>, ATOM_WAVE_TYPE_LINEAR);
}

ATOM_CompositionActor *ATOM_GeodeActor::clone (void) const
{
	return cloneActor (this);
}

void ATOM_GeodeActor::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_CompositionActorT<ATOM_Geode>::saveToXML (xml);

	if (_actionTrack->getNumKeys() > 0)
	{
		ATOM_TiXmlElement eAction("ActionTrack");
		_actionTrack->saveToXML (&eAction);
		xml->InsertEndChild (eAction);
	}
}

bool ATOM_GeodeActor::loadFromXML (const ATOM_TiXmlElement *xml)
{
	if (!ATOM_CompositionActorT<ATOM_Geode>::loadFromXML (xml))
	{
		return false;
	}

	const ATOM_TiXmlElement *eAction = xml->FirstChildElement ("ActionTrack");
	if (eAction)
	{
		_actionTrack->loadFromXML (eAction);
	}
	else
	{
		for (unsigned i = 0; i < getNumKeys(); ++i)
		{
			ATOM_AUTOPTR(ATOM_GeodeKeyFrame) keyFrame = ATOM_NEW(ATOM_GeodeKeyFrame);
			ATOM_GeodeKeyFrameData data;
			data.actionIndex = -1;
			data.fadeTime = 500;
			data.speed = 1.f;
			data.loop = 0;
			data.transparency = 1.f;
			//--- wangjian added ---//
			data.colorMultiplier = 1.0f;

			keyFrame->setValue(data);

			_actionTrack->insertKeyFrame (getKeyTime(i), keyFrame.get());
		}
	}

	return true;
}

bool ATOM_GeodeActor::addKeyFrame (long time, const ATOM_CompositionKeyFrameValue *value)
{
	if (!ATOM_CompositionActorT<ATOM_Geode>::addKeyFrame (time, value))
	{
		return false;
	}

	ATOM_GeodeKeyFrameValue *keyFrameValue = (ATOM_GeodeKeyFrameValue*)value;
	const ATOM_GeodeKeyFrameData &data = keyFrameValue->getData();
	_actionTrack->insertKeyFrame (time, ATOM_NEW(ATOM_GeodeKeyFrame, data));

	return true;
}

bool ATOM_GeodeActor::removeKeyFrame (long time)
{
	if (!ATOM_CompositionActorT<ATOM_Geode>::removeKeyFrame (time))
	{
		return false;
	}

	unsigned index = _actionTrack->findKeyFrame (time);
	if (index == (unsigned)-1)
	{
		return false;
	}

	_actionTrack->removeKeyFrame (index);

	return true;
}

void ATOM_GeodeActor::captureKeyFrameValue (ATOM_CompositionKeyFrameValue *value) const
{
	ATOM_CompositionActorT<ATOM_Geode>::captureKeyFrameValue(value);

	ATOM_GeodeKeyFrameValue *keyFrameValue = (ATOM_GeodeKeyFrameValue*)value;
	ATOM_Geode *geode = (ATOM_Geode*)getNode();
	ATOM_GeodeKeyFrameData data;

	data.actionIndex = -1;
	for (int i = 0; i < geode->getNumTracks(); ++i)
	{
		if (geode->getActionMixer()->getUpsideTrack() == geode->getTrack(i))
		{
			data.actionIndex = i;
			break;
		}
	}
	data.fadeTime = geode->getActionMixer()->getUpsideFadeTime ();
	data.speed = geode->getActionMixer()->getUpsideSpeed ();
	data.loop = geode->getActionMixer()->getUpsideDesiredLoopCount ();
	data.transparency = geode->getTransparency ();
	//--- wangjian added ---//
	data.colorMultiplier = geode->getColorMultiplier();

	keyFrameValue->setData (data);
}

void ATOM_GeodeActor::evalKeyFrameValue (long time, ATOM_CompositionKeyFrameValue *value) const
{
	ATOM_CompositionActorT<ATOM_Geode>::evalKeyFrameValue (time, value);

	ATOM_GeodeKeyFrameValue *keyFrameValue = (ATOM_GeodeKeyFrameValue*)value;
	keyFrameValue->setData (_actionTrack->eval (time));
}

bool ATOM_GeodeActor::getKeyFrame (long time, ATOM_CompositionKeyFrameValue *value) const
{
	if (!ATOM_CompositionActorT<ATOM_Geode>::getKeyFrame (time, value))
	{
		return false;
	}

	ATOM_GeodeKeyFrameValue *keyFrameValue = (ATOM_GeodeKeyFrameValue*)value;
	unsigned i = _actionTrack->findKeyFrame (time);
	if (i != (unsigned)-1)
	{
		keyFrameValue->setData (_actionTrack->getKeyValueByIndex (i));
		return true;
	}

	return false;
}

ATOM_CompositionKeyFrameValue *ATOM_GeodeActor::createKeyFrameValue (void) const
{
	return ATOM_NEW(ATOM_GeodeKeyFrameValue);
}

void ATOM_GeodeActor::doReset (void) 
{
	((ATOM_Geode*)getNode())->resetActions (ATOM_Geode::ACTIONFLAGS_DOWNSIDE|ATOM_Geode::ACTIONFLAGS_UPSIDE);
}

void ATOM_GeodeActor::update (long time) 
{
	ATOM_CompositionActorT<ATOM_Geode>::update (time);

	_actionTrack->apply (time, _node.get());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_LightActor::ATOM_LightActor (void)
{
	_lightTrack = ATOM_NEW(ATOM_CompositionTrackT<ATOM_LightKeyFrame>, ATOM_WAVE_TYPE_LINEAR);
}

ATOM_CompositionActor *ATOM_LightActor::clone (void) const
{
	return cloneActor (this);
}

void ATOM_LightActor::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_CompositionActorT<ATOM_LightNode>::saveToXML (xml);

	if (_lightTrack->getNumKeys() > 0)
	{
		ATOM_TiXmlElement eLight("LightTrack");
		_lightTrack->saveToXML (&eLight);
		xml->InsertEndChild (eLight);
	}
}

bool ATOM_LightActor::loadFromXML (const ATOM_TiXmlElement *xml)
{
	if (!ATOM_CompositionActorT<ATOM_LightNode>::loadFromXML (xml))
	{
		return false;
	}

	const ATOM_TiXmlElement *eLight = xml->FirstChildElement ("LightTrack");
	if (eLight)
	{
		_lightTrack->loadFromXML (eLight);
	}
	else
	{
		for (unsigned i = 0; i < getNumKeys(); ++i)
		{
			ATOM_AUTOPTR(ATOM_LightKeyFrame) keyFrame = ATOM_NEW(ATOM_LightKeyFrame);
			ATOM_LightKeyFrameData data;
			data.type = ATOM_Light::Point;
			data.color.set (1.f, 1.f, 1.f, 1.f);
			data.atten.set (1.f, 0.1f, 0.001f);
			keyFrame->setValue(data);

			_lightTrack->insertKeyFrame (getKeyTime(i), keyFrame.get());
		}
	}

	return true;
}

bool ATOM_LightActor::addKeyFrame (long time, const ATOM_CompositionKeyFrameValue *value)
{
	if (!ATOM_CompositionActorT<ATOM_LightNode>::addKeyFrame (time, value))
	{
		return false;
	}

	ATOM_LightKeyFrameValue *keyFrameValue = (ATOM_LightKeyFrameValue*)value;
	const ATOM_LightKeyFrameData &data = keyFrameValue->getData();
	_lightTrack->insertKeyFrame (time, ATOM_NEW(ATOM_LightKeyFrame, data));

	return true;
}

bool ATOM_LightActor::removeKeyFrame (long time)
{
	if (!ATOM_CompositionActorT<ATOM_LightNode>::removeKeyFrame (time))
	{
		return false;
	}

	unsigned index = _lightTrack->findKeyFrame (time);
	if (index == (unsigned)-1)
	{
		return false;
	}

	_lightTrack->removeKeyFrame (index);

	return true;
}

void ATOM_LightActor::captureKeyFrameValue (ATOM_CompositionKeyFrameValue *value) const
{
	ATOM_CompositionActorT<ATOM_LightNode>::captureKeyFrameValue(value);

	ATOM_LightKeyFrameValue *keyFrameValue = (ATOM_LightKeyFrameValue*)value;
	ATOM_LightNode *light = (ATOM_LightNode*)getNode();
	ATOM_LightKeyFrameData data;

	data.type = light->getLightType ();
	data.color = light->getLightColor ();
	data.atten = light->getLightAttenuation ();
	keyFrameValue->setData (data);
}

void ATOM_LightActor::evalKeyFrameValue (long time, ATOM_CompositionKeyFrameValue *value) const
{
	ATOM_CompositionActorT<ATOM_LightNode>::evalKeyFrameValue (time, value);

	ATOM_LightKeyFrameValue *keyFrameValue = (ATOM_LightKeyFrameValue*)value;
	keyFrameValue->setData (_lightTrack->eval (time));
}

bool ATOM_LightActor::getKeyFrame (long time, ATOM_CompositionKeyFrameValue *value) const
{
	if (!ATOM_CompositionActorT<ATOM_LightNode>::getKeyFrame (time, value))
	{
		return false;
	}

	ATOM_LightKeyFrameValue *keyFrameValue = (ATOM_LightKeyFrameValue*)value;
	unsigned i = _lightTrack->findKeyFrame (time);
	if (i != (unsigned)-1)
	{
		keyFrameValue->setData (_lightTrack->getKeyValueByIndex (i));
		return true;
	}

	return false;
}

ATOM_CompositionKeyFrameValue *ATOM_LightActor::createKeyFrameValue (void) const
{
	return ATOM_NEW(ATOM_LightKeyFrameValue);
}

void ATOM_LightActor::doReset (void) 
{
}

void ATOM_LightActor::update (long time) 
{
	ATOM_CompositionActorT<ATOM_LightNode>::update (time);

	_lightTrack->apply (time, _node.get());
}

////////////////////////////////////////////////////////////////////////////////////////////

ATOM_DecalActor::ATOM_DecalActor (void)
{
	_decalTrack = ATOM_NEW(ATOM_CompositionTrackT<ATOM_DecalKeyFrame>, ATOM_WAVE_TYPE_LINEAR);
}

ATOM_CompositionActor *ATOM_DecalActor::clone (void) const
{
	return cloneActor (this);
}

void ATOM_DecalActor::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_CompositionActorT<ATOM_Decal>::saveToXML (xml);

	if (_decalTrack->getNumKeys() > 0)
	{
		ATOM_TiXmlElement eDecal("DecalTrack");
		_decalTrack->saveToXML (&eDecal);
		xml->InsertEndChild (eDecal);
	}
}

bool ATOM_DecalActor::loadFromXML (const ATOM_TiXmlElement *xml)
{
	if (!ATOM_CompositionActorT<ATOM_Decal>::loadFromXML (xml))
	{
		return false;
	}

	const ATOM_TiXmlElement *eDecal = xml->FirstChildElement ("DecalTrack");
	if (eDecal)
	{
		_decalTrack->loadFromXML (eDecal);
	}
	else
	{
		for (unsigned i = 0; i < getNumKeys(); ++i)
		{
			ATOM_AUTOPTR(ATOM_DecalKeyFrame) keyFrame = ATOM_NEW(ATOM_DecalKeyFrame);
			ATOM_DecalKeyFrameData data;
			data.color.set (1.f, 1.f, 1.f, 1.f);
			keyFrame->setValue(data);

			_decalTrack->insertKeyFrame (getKeyTime(i), keyFrame.get());
		}
	}

	return true;
}

bool ATOM_DecalActor::addKeyFrame (long time, const ATOM_CompositionKeyFrameValue *value)
{
	if (!ATOM_CompositionActorT<ATOM_Decal>::addKeyFrame (time, value))
	{
		return false;
	}

	ATOM_DecalKeyFrameValue *keyFrameValue = (ATOM_DecalKeyFrameValue*)value;
	const ATOM_DecalKeyFrameData &data = keyFrameValue->getData();
	_decalTrack->insertKeyFrame (time, ATOM_NEW(ATOM_DecalKeyFrame, data));

	return true;
}

bool ATOM_DecalActor::removeKeyFrame (long time)
{
	if (!ATOM_CompositionActorT<ATOM_Decal>::removeKeyFrame (time))
	{
		return false;
	}

	unsigned index = _decalTrack->findKeyFrame (time);
	if (index == (unsigned)-1)
	{
		return false;
	}

	_decalTrack->removeKeyFrame (index);

	return true;
}

void ATOM_DecalActor::captureKeyFrameValue (ATOM_CompositionKeyFrameValue *value) const
{
	ATOM_CompositionActorT<ATOM_Decal>::captureKeyFrameValue(value);

	ATOM_DecalKeyFrameValue *keyFrameValue = (ATOM_DecalKeyFrameValue*)value;
	ATOM_Decal *decal = (ATOM_Decal*)getNode();
	ATOM_DecalKeyFrameData data;
	data.color = decal->getColor();
	keyFrameValue->setData (data);
}

void ATOM_DecalActor::evalKeyFrameValue (long time, ATOM_CompositionKeyFrameValue *value) const
{
	ATOM_CompositionActorT<ATOM_Decal>::evalKeyFrameValue (time, value);

	ATOM_DecalKeyFrameValue *keyFrameValue = (ATOM_DecalKeyFrameValue*)value;
	keyFrameValue->setData (_decalTrack->eval (time));
}

bool ATOM_DecalActor::getKeyFrame (long time, ATOM_CompositionKeyFrameValue *value) const
{
	if (!ATOM_CompositionActorT<ATOM_Decal>::getKeyFrame (time, value))
	{
		return false;
	}

	ATOM_DecalKeyFrameValue *keyFrameValue = (ATOM_DecalKeyFrameValue*)value;
	unsigned i = _decalTrack->findKeyFrame (time);
	if (i != (unsigned)-1)
	{
		keyFrameValue->setData (_decalTrack->getKeyValueByIndex (i));
		return true;
	}

	return false;
}

ATOM_CompositionKeyFrameValue *ATOM_DecalActor::createKeyFrameValue (void) const
{
	return ATOM_NEW(ATOM_DecalKeyFrameValue);
}

void ATOM_DecalActor::doReset (void) 
{
}

void ATOM_DecalActor::update (long time) 
{
	ATOM_CompositionActorT<ATOM_Decal>::update (time);

	_decalTrack->apply (time, _node.get());
}

