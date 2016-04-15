#ifndef __ATOM3D_ENGINE_UPDATEVISITOR_H
#define __ATOM3D_ENGINE_UPDATEVISITOR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"
#include "visitor.h"

class ATOM_Octree;
class ATOM_Camera;

class ATOM_ENGINE_API ATOM_UpdateVisitor: public ATOM_Visitor
{
	friend class ATOM_CullVisitor;

public:
	ATOM_UpdateVisitor (void);
	virtual ~ATOM_UpdateVisitor (void);

public:
	virtual void visit (ATOM_Node &node);
	virtual void visit (ATOM_NodeOctree &node);
	virtual void visit (ATOM_ParticleSystem &node);
	virtual void visit (ATOM_Terrain &node);
	virtual void visit (ATOM_Hud &node);
	virtual void visit (ATOM_GuiHud &node);
	virtual void visit (ATOM_Water &node);
	virtual void visit (ATOM_Atmosphere &node);
	virtual void visit (ATOM_Atmosphere2 &node);
	virtual void visit (ATOM_Sky &node);
	virtual void visit (ATOM_VisualNode &node);
	virtual void visit (ATOM_Geode &node);
	virtual void visit (ATOM_CompositionNode &node);
	virtual void visit (ClientSimpleCharacter &node);

protected:
	virtual void onBeginVisitNodeTree (ATOM_Node &node);
	virtual void onEndVisitNodeTree (ATOM_Node &node);
	virtual void onResetVisitor (void);

private:
	void updateNodePlacementOfOctree (ATOM_Node &node) const;
	void setOctree (ATOM_Octree *octree);
  
public:
	ATOM_Octree *getOctree (void) const;
	void setCamera (ATOM_Camera *camera);
	ATOM_Camera * getCamera (void) const;

protected:
	ATOM_Octree *_octree;
	ATOM_Camera *_camera;
	ATOM_NodeOctree *_octreeNode;
	unsigned _frameStamp;
};

#endif // __ATOM3D_ENGINE_UPDATEVISITOR_H
