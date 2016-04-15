#ifndef __ATOM3D_ENGINE_PICKVISITOR_H
#define __ATOM3D_ENGINE_PICKVISITOR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"
#include "../ATOM_geometry.h"
#include "visitor.h"
#include "node.h"
#include "renderqueue.h"

class ATOM_NodeOctree;
class ATOM_Camera;
class ATOM_Octree;

class ATOM_ENGINE_API ATOM_PickVisitor: public ATOM_Visitor
{
public:
	enum
	{
		INTERSECTION_PROBE = (1<<0),
		HAVE_SCREEN_POSITION = (1<<1)
	};

	struct PickResult
	{
		ATOM_AUTOREF(ATOM_Node) node;
		float distance;
		float u, v;	// for HUD
		bool operator < (const PickResult &other) const { return distance < other.distance; }
	};

public:
	ATOM_PickVisitor (void);
	virtual ~ATOM_PickVisitor (void);

public:
	virtual void traverse (ATOM_Node &node);
	virtual void visit (ATOM_Node &node);
	virtual void visit (ATOM_NodeOctree &node);
	virtual void visit (ATOM_Terrain &node);
	virtual void visit (ATOM_VisualNode &node);
	virtual void visit (ATOM_Geode &node);
	virtual void visit (ATOM_Hud &node);
	virtual void visit (ATOM_Water &node);
	virtual void visit (ATOM_LightNode &node);
	virtual void visit (ATOM_Atmosphere &node);
	virtual void visit (ATOM_Atmosphere2 &node);
	virtual void visit (ATOM_Sky &node);
	virtual void visit (ATOM_CompositionNode &node);
	virtual void visit (ATOM_ParticleSystem &node);
	virtual void visit (ATOM_ShapeNode &node);
	virtual void visit (ATOM_WeaponTrail &node);

protected:
	virtual void onBeginVisitNodeTree (ATOM_Node &node);
	virtual void onEndVisitNodeTree (ATOM_Node &node);
	virtual void onResetVisitor (void);
	virtual void onNodePicked (ATOM_Node &node, float distance, float u, float v);

private:
	void PickOctree_r (ATOM_OctreeNode *node);

public:
	unsigned getNumPicked (void) const { return _results.size(); }
	const PickResult &getPickResult (unsigned index) const { return _results[index]; }
	PickResult &getPickResult (unsigned index) { return _results[index]; }
	const ATOM_VECTOR<PickResult> &getPickResults (void) const { return _results; }
	ATOM_VECTOR<PickResult> &getPickResults (void) { return _results; }
	void setScreenPos (int x, int y) { _screenPosX = x; _screenPosY = y; }
	void setRay (const ATOM_Ray &ray) { _ray = ray; }
	const ATOM_Ray &getRay (void) const { return _ray; }
	void setCamera (ATOM_Camera *camera) { _camera = camera; }
	ATOM_Camera *getCamera (void) const { return _camera; }
	void setFlags (unsigned flags) { _flags = flags; }
	unsigned getFlags (void) const { return _flags; }
	bool intersected (void) const { return _intersected; }
	void sortResults (void);
	void setPickAll (bool pickAll);
	bool getPickAll (void) const;

protected:
	ATOM_Ray _ray;
	ATOM_Camera *_camera;
	unsigned _flags;
	bool _intersected;
	bool _pickAll;
	int _screenPosX;
	int _screenPosY;
	ATOM_VECTOR<PickResult> _results;
};

#endif // __ATOM3D_ENGINE_CULLVISITOR_H
