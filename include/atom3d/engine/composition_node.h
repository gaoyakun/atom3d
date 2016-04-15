#ifndef __ATOM3D_ENGINE_COMPOSITION_NODE_H
#define __ATOM3D_ENGINE_COMPOSITION_NODE_H

#include "visualnode.h"
#include "composition_actor.h"

//! \class ATOM_CompositionNode
//! 特效组合节点
//! \author 高雅昆
//! \ingroup engine
class ATOM_ENGINE_API ATOM_CompositionNode: public ATOM_VisualNode
{
	ATOM_CLASS(engine, ATOM_CompositionNode, ATOM_CompositionNode)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_CompositionNode)

public:
	ATOM_CompositionNode (void);
	virtual ~ATOM_CompositionNode (void);

public:
	virtual void accept (ATOM_Visitor &visitor);
	virtual void buildBoundingbox (void) const;
	virtual void setupRenderQueue (ATOM_CullVisitor *visitor);
	virtual bool rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const;
	virtual void update (ATOM_Camera *camera);
	virtual bool loadAttribute(const ATOM_TiXmlElement *xmlelement);
	virtual bool writeAttribute(ATOM_TiXmlElement *xmlelement);

protected:
	virtual bool onLoad (ATOM_RenderDevice *device);

public:
	bool loadFromXML (ATOM_RenderDevice *device, const ATOM_TiXmlElement *xml);
	bool saveToXML (ATOM_TiXmlElement *xml);
	bool isActorNode (ATOM_Node *node) const;
	void setTimeLineDuration (unsigned duration);
	unsigned getTimeLineDuration (void) const;
	bool seekTimeLine (unsigned position);
	unsigned getTimeLinePosition (void) const;
	void reset (void);
	void play (unsigned loopCount = 0, bool autoRemove = false);
	void stop (void);
	bool isPlaying (void) const;
	void setSpeed (float speed);
	float getSpeed (void) const;
	void addActor (ATOM_CompositionActor *actor);
	unsigned getNumTopActors (void) const;
	ATOM_CompositionActor *getTopActor (unsigned index) const;
	ATOM_CompositionActor *getActorByName (const char *name) const;
	void removeActor (ATOM_CompositionActor *actor);
	ATOM_Node *getActorParentNode (void) const;
	void setCompositionFileName (const ATOM_STRING &filename);
	const ATOM_STRING &getCompositionFileName (void) const;
	bool isLoadReferenceActors (void) const;
	void setLoadReferenceActors (bool enable);
	unsigned getStartTick (void) const;
	void invalidateGroup (void);
	void cullGroups (ATOM_CullVisitor *v);

public:
	void att_setPlaying (int play);
	int att_getPlaying (void) const;

private:
	void updateR (ATOM_CompositionActor *actor, ATOM_Camera *camera);
	void updateGroups (void);

	//--- wangjian added ---//
	// 直接加载接口
	bool loadSync(ATOM_STRING & file);
	// 异步加载：检查异步加载是否完成标记
	bool checkAsyncLoadFlag();
	int _updateCache;

protected:
	virtual void resetMaterialDirtyFlag_impl();

public:
	virtual bool	checkNodeAllFinshed();
	virtual void	onLoadFinished();
	//----------------------//

private:
	unsigned _timeDuration;
	unsigned _currentPosition;
	unsigned _playTick;
	unsigned _startTick;
	unsigned _loopCount;
	unsigned _desiredLoopCount;
	bool _playing;
	bool _loadRef;
	float _speed;
	bool _attribLoaded;
	bool _autoRemove;
	bool _groupDirty;
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_CompositionActor)> _actors;
	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)> _groupNodes;
	ATOM_VECTOR<int> _groupNumbers;
	ATOM_AUTOREF(ATOM_Node) _actorParent;
	ATOM_STRING _compositionFileName;

	ATOM_DECLARE_NODE_FILE_TYPE(ATOM_CompositionNode, ATOM_CompositionNode, "cps", "ATOM3D Composition Node")
};

#endif // __ATOM3D_ENGINE_COMPOSITION_NODE_H
