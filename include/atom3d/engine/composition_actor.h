#ifndef __ATOM3D_ENGINE_COMPOSITION_ACTOR_H
#define __ATOM3D_ENGINE_COMPOSITION_ACTOR_H

#include "node.h"
#include "composition_track.h"
#include "transform_keyframe.h"
#include "shape_keyframe.h"
#include "trail_keyframe.h"
#include "weapontrail_keyframe.h"
#include "geode_keyframe.h"
#include "visible_keyframe.h"
#include "ps_keyframe.h"
#include "light_keyframe.h"
#include "decal_keyframe.h"
#include "composition_keyframevalue.h"

class ATOM_CompositionKeyFrameValue;
class ATOM_CompositionNode;

class ATOM_ENGINE_API ATOM_CompositionActor: public ATOM_ReferenceObj
{
public:
	ATOM_CompositionActor (void);
	virtual ~ATOM_CompositionActor (void);

public:
	virtual ATOM_CompositionActor *clone (void) const = 0;
	virtual void start (void);
	virtual void pause (void);
	virtual unsigned getNumKeys (void) const;
	virtual long getKeyTime (unsigned index) const;
	virtual bool addKeyFrame (long time, const ATOM_CompositionKeyFrameValue *value);
	virtual bool removeKeyFrame (long time);
	virtual bool getKeyFrame (long time, ATOM_CompositionKeyFrameValue *value) const;
	virtual ATOM_CompositionKeyFrameValue *createKeyFrameValue (void) const;
	virtual void captureKeyFrameValue (ATOM_CompositionKeyFrameValue *value) const;
	virtual void evalKeyFrameValue (long time, ATOM_CompositionKeyFrameValue *value) const;
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual bool loadFromXML (const ATOM_TiXmlElement *xml);
	virtual void doReset (void);

public:
	virtual void update (long time) = 0;
	virtual void show (bool show) = 0;
	virtual ATOM_Node *getNode (void) const = 0;

public:
	unsigned getNumChildActors (void) const;
	ATOM_CompositionActor *getParentActor (void) const;
	ATOM_CompositionActor *getChildActor (unsigned index) const;
	ATOM_CompositionActor *getActorByName (const char *name);
	void addChildActor (ATOM_CompositionActor *actor);
	void removeChildActor (ATOM_CompositionActor *actor);
	void removeAllActors (void);
	void setName (const char *name);
	const char *getName (void) const;
	void makeReference (bool ref);
	bool isReference (void) const;
	void allowReset (bool allow);
	bool isResetAllowed (void) const;
	void reset (void);
	ATOM_CompositionNode *getCompositionNode (void) const;
	void setCompositionNode (ATOM_CompositionNode *node);
	ATOM_CompositionTrackT<ATOM_TranslateKeyFrame> *getTranslateTrack (void) const;
	ATOM_CompositionTrackT<ATOM_ScaleKeyFrame> *getScaleTrack (void) const;
	ATOM_CompositionTrackT<ATOM_RotateKeyFrame> *getRotateTrack (void) const;
	ATOM_CompositionTrackT<ATOM_VisibleKeyFrame> *getVisibleTrack (void) const;
	
	//--- wangjian added ---//
	bool checkNodeAllLoadFinshed();
	//----------------------//

private:
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_CompositionActor)> _childActors;
	ATOM_AUTOPTR(ATOM_CompositionTrackT<ATOM_TranslateKeyFrame>) _translateTrack;
	ATOM_AUTOPTR(ATOM_CompositionTrackT<ATOM_ScaleKeyFrame>) _scaleTrack;
	ATOM_AUTOPTR(ATOM_CompositionTrackT<ATOM_RotateKeyFrame>) _rotateTrack;
	ATOM_AUTOPTR(ATOM_CompositionTrackT<ATOM_VisibleKeyFrame>) _visibleTrack;
	ATOM_STRING _name;
	bool _isReference;
	bool _noReset;
	ATOM_CompositionActor *_parentActor;
	ATOM_CompositionNode *_compositionNode;
};

template <class NodeType>
class ATOM_CompositionActorT: public ATOM_CompositionActor
{
public:
	typedef NodeType node_type;

protected:
	ATOM_AUTOREF(NodeType) _node;

public:
	ATOM_CompositionActorT (void)
	{
		_node = ATOM_HARDREF(NodeType)();
		_node->setPickable (ATOM_Node::INHERIT);
		_node->setPrivate (1);
	}

	virtual void show (bool show)
	{
		if (_node)
		{
			_node->setShow (show ? ATOM_Node::SHOW : ATOM_Node::HIDE);
		}
	}

	virtual ATOM_Node *getNode (void) const {
		return _node.get();
	}

	virtual void update (long time) 
	{
		getTranslateTrack()->apply (time, _node.get());
		getScaleTrack()->apply (time, _node.get());
		getRotateTrack()->apply (time, _node.get());
		getVisibleTrack()->apply (time, _node.get());
	}

public:
	NodeType *getActorNode (void) const 
	{
		return _node.get();
	}

	void setActorNode (NodeType *node) {
		_node = node;
	}
};

template <class ActorType>
ActorType *cloneActor (const ActorType *actor)
{
	ActorType *result = ATOM_NEW(ActorType);
	result->setActorNode ((typename ActorType::node_type*)(actor->getActorNode()->clone ().get()));
	result->makeReference (actor->isReference());
	result->allowReset (actor->isResetAllowed());
	for (int i = 0; i < actor->getNumKeys(); ++i)
	{
		ATOM_CompositionKeyFrameValue *k = actor->createKeyFrameValue ();
		long t = actor->getKeyTime (i);
		actor->getKeyFrame (t, k);
		result->addKeyFrame (t, k);
	}
	return result;
}

class ATOM_ENGINE_API ATOM_ShapeActor: public ATOM_CompositionActorT<ATOM_ShapeNode>
{
protected:
	ATOM_AUTOPTR(ATOM_CompositionTrackT<ATOM_ShapeKeyFrame>) _shapeTrack;

public:
	ATOM_ShapeActor (void);
	virtual ATOM_CompositionActor *clone (void) const;
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual bool loadFromXML (const ATOM_TiXmlElement *xml);
	virtual bool addKeyFrame (long time, const ATOM_CompositionKeyFrameValue *value);
	virtual bool removeKeyFrame (long time);
	virtual void captureKeyFrameValue (ATOM_CompositionKeyFrameValue *value) const;
	virtual void evalKeyFrameValue (long time, ATOM_CompositionKeyFrameValue *value) const;
	virtual bool getKeyFrame (long time, ATOM_CompositionKeyFrameValue *value) const;
	virtual ATOM_CompositionKeyFrameValue *createKeyFrameValue (void) const;
	virtual void doReset (void);
	virtual void update (long time);
};

class ATOM_ENGINE_API ATOM_WeaponTrailActor: public ATOM_CompositionActorT<ATOM_WeaponTrail>
{
protected:
	ATOM_AUTOPTR(ATOM_CompositionTrackT<ATOM_WeaponTrailKeyFrame>) _weaponTrailTrack;

public:
	ATOM_WeaponTrailActor (void);
	virtual ATOM_CompositionActor *clone (void) const;
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual bool loadFromXML (const ATOM_TiXmlElement *xml);
	virtual bool addKeyFrame (long time, const ATOM_CompositionKeyFrameValue *value);
	virtual bool removeKeyFrame (long time);
	virtual void captureKeyFrameValue (ATOM_CompositionKeyFrameValue *value) const;
	virtual void evalKeyFrameValue (long time, ATOM_CompositionKeyFrameValue *value) const;
	virtual bool getKeyFrame (long time, ATOM_CompositionKeyFrameValue *value) const;
	virtual ATOM_CompositionKeyFrameValue *createKeyFrameValue (void) const;
	virtual void doReset (void);
	virtual void update (long time);
};

class ATOM_ENGINE_API ATOM_TrailActor: public ATOM_CompositionActorT<ATOM_RibbonTrail>
{
protected:
	ATOM_AUTOPTR(ATOM_CompositionTrackT<ATOM_TrailKeyFrame>) _trailTrack;

public:
	ATOM_TrailActor (void);
	virtual ATOM_CompositionActor *clone (void) const;
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual bool loadFromXML (const ATOM_TiXmlElement *xml);
	virtual bool addKeyFrame (long time, const ATOM_CompositionKeyFrameValue *value);
	virtual bool removeKeyFrame (long time);
	virtual void captureKeyFrameValue (ATOM_CompositionKeyFrameValue *value) const;
	virtual void evalKeyFrameValue (long time, ATOM_CompositionKeyFrameValue *value) const;
	virtual bool getKeyFrame (long time, ATOM_CompositionKeyFrameValue *value) const;
	virtual ATOM_CompositionKeyFrameValue *createKeyFrameValue (void) const;
	virtual void doReset (void);
	virtual void update (long time);
};

class ATOM_ParticleSystemActor: public ATOM_CompositionActorT<ATOM_ParticleSystem>
{
protected:
	ATOM_AUTOPTR(ATOM_CompositionTrackT<ATOM_ParticleSystemKeyFrame>) _psTrack;

public:
	ATOM_ParticleSystemActor (void);
	virtual ATOM_CompositionActor *clone (void) const;
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual bool loadFromXML (const ATOM_TiXmlElement *xml);
	virtual bool addKeyFrame (long time, const ATOM_CompositionKeyFrameValue *value);
	virtual bool removeKeyFrame (long time);
	virtual void captureKeyFrameValue (ATOM_CompositionKeyFrameValue *value) const;
	virtual void evalKeyFrameValue (long time, ATOM_CompositionKeyFrameValue *value) const;
	virtual bool getKeyFrame (long time, ATOM_CompositionKeyFrameValue *value) const;
	virtual ATOM_CompositionKeyFrameValue *createKeyFrameValue (void) const;
	virtual void doReset (void);
	virtual void update (long time);
};

class ATOM_GeodeActor: public ATOM_CompositionActorT<ATOM_Geode>
{
protected:
	ATOM_AUTOPTR(ATOM_CompositionTrackT<ATOM_GeodeKeyFrame>) _actionTrack;

public:
	ATOM_GeodeActor (void);
	virtual ATOM_CompositionActor *clone (void) const;
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual bool loadFromXML (const ATOM_TiXmlElement *xml);
	virtual bool addKeyFrame (long time, const ATOM_CompositionKeyFrameValue *value);
	virtual bool removeKeyFrame (long time);
	virtual void captureKeyFrameValue (ATOM_CompositionKeyFrameValue *value) const;
	virtual void evalKeyFrameValue (long time, ATOM_CompositionKeyFrameValue *value) const;
	virtual bool getKeyFrame (long time, ATOM_CompositionKeyFrameValue *value) const;
	virtual ATOM_CompositionKeyFrameValue *createKeyFrameValue (void) const;
	virtual void doReset (void);
	virtual void update (long time);
};

class ATOM_LightActor: public ATOM_CompositionActorT<ATOM_LightNode>
{
protected:
	ATOM_AUTOPTR(ATOM_CompositionTrackT<ATOM_LightKeyFrame>) _lightTrack;

public:
	ATOM_LightActor (void);
	virtual ATOM_CompositionActor *clone (void) const;
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual bool loadFromXML (const ATOM_TiXmlElement *xml);
	virtual bool addKeyFrame (long time, const ATOM_CompositionKeyFrameValue *value);
	virtual bool removeKeyFrame (long time);
	virtual void captureKeyFrameValue (ATOM_CompositionKeyFrameValue *value) const;
	virtual void evalKeyFrameValue (long time, ATOM_CompositionKeyFrameValue *value) const;
	virtual bool getKeyFrame (long time, ATOM_CompositionKeyFrameValue *value) const;
	virtual ATOM_CompositionKeyFrameValue *createKeyFrameValue (void) const;
	virtual void doReset (void);
	virtual void update (long time);
};

class ATOM_DecalActor: public ATOM_CompositionActorT<ATOM_Decal>
{
protected:
	ATOM_AUTOPTR(ATOM_CompositionTrackT<ATOM_DecalKeyFrame>) _decalTrack;

public:
	ATOM_DecalActor (void);
	virtual ATOM_CompositionActor *clone (void) const;
	virtual void saveToXML (ATOM_TiXmlElement *xml) const;
	virtual bool loadFromXML (const ATOM_TiXmlElement *xml);
	virtual bool addKeyFrame (long time, const ATOM_CompositionKeyFrameValue *value);
	virtual bool removeKeyFrame (long time);
	virtual void captureKeyFrameValue (ATOM_CompositionKeyFrameValue *value) const;
	virtual void evalKeyFrameValue (long time, ATOM_CompositionKeyFrameValue *value) const;
	virtual bool getKeyFrame (long time, ATOM_CompositionKeyFrameValue *value) const;
	virtual ATOM_CompositionKeyFrameValue *createKeyFrameValue (void) const;
	virtual void doReset (void);
	virtual void update (long time);
};

#endif // __ATOM3D_ENGINE_COMPOSITION_ACTOR_H
