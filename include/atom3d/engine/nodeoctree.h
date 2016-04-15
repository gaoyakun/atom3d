#ifndef __ATOM3D_ENGINE_NODEOCTREE_H
#define __ATOM3D_ENGINE_NODEOCTREE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"

#include "octree.h"

class ATOM_Octree;

class ATOM_ENGINE_API ATOM_NodeOctree: public ATOM_Node
{
	ATOM_CLASS(engine, ATOM_NodeOctree, Noctree);

	static const float OCTREE_SIZE;
	static const float OCTREE_LEAF_SIZE;

public:
	ATOM_NodeOctree (void);
	virtual ~ATOM_NodeOctree (void);

public:
	virtual void accept (ATOM_Visitor &visitor);
	virtual ATOM_AUTOREF(ATOM_Node) clone (void) const;

public:
	ATOM_Octree * getOctree (void) const;

private:
	ATOM_Octree *_octree;
};

#endif // __ATOM3D_ENGINE_NODEOCTREE_H

