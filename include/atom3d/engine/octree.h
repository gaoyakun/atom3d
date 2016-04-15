#ifndef __ATOM3D_ENGINE_OCTREE_H
#define __ATOM3D_ENGINE_OCTREE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_math.h"

#include "spatialcell.h"

class ATOM_OctreeNodeChunk;
class ATOM_Octree;

class ATOM_OctreeNode: public ATOM_SpatialCell
{
public:
	enum Placement
	{
		PLACEMENTBEGIN = 0,
		PXPYPZ = PLACEMENTBEGIN,
		PXPYNZ = 1,
		PXNYPZ = 2,
		PXNYNZ = 3,
		NXPYPZ = 4,
		NXPYNZ = 5,
		NXNYPZ = 6,
		NXNYNZ = 7,
		PLACEMENTEND
	};
	typedef unsigned long NodeIndex;

public:
	ATOM_OctreeNode (void);
	void setChunk (ATOM_OctreeNodeChunk *chunk);
	ATOM_OctreeNodeChunk *getChunk (void) const;
	void setPosition (NodeIndex position);
	NodeIndex getPosition (void) const;
	ATOM_Vector3f getMinPoint (void) const;
	ATOM_Vector3f getMaxPoint (void) const;
	ATOM_Vector3f getMinPointLoosed (void) const;
	ATOM_Vector3f getMaxPointLoosed (void) const;
	unsigned getReference (void) const;

public:
	ATOM_OctreeNode *getChild (Placement placement) const;
	ATOM_OctreeNode *getOrCreateChild (Placement placement) const;
	ATOM_OctreeNode *getParent (void) const;
	ATOM_OctreeNode *getOrCreateParent (void) const;
	void createChildren (void) const;
	bool tidy (void);

public:
	void setUserData (void *data);
	void *getUserData (void) const;

private:
	ATOM_OctreeNodeChunk *_M_chunk;
	NodeIndex _M_position;
	unsigned _M_reference;
	void *_M_userdata;
};

class ATOM_OctreeNodeChunk
{
	friend class ATOM_Octree;
	typedef ATOM_HASHMAP<ATOM_OctreeNode::NodeIndex, ATOM_OctreeNode*> node_map;
	typedef node_map::iterator node_iterator;
	typedef node_map::const_iterator node_const_iterator;

public:
	ATOM_OctreeNodeChunk (ATOM_Octree *octree);
	~ATOM_OctreeNodeChunk (void);

public:
	ATOM_OctreeNode *getNode (ATOM_OctreeNode::NodeIndex index) const;
	ATOM_OctreeNode *getOrCreateNode (ATOM_OctreeNode::NodeIndex index);
	ATOM_OctreeNode *getOrCreateNodeChain (ATOM_OctreeNode::NodeIndex index);
	void freeNode (ATOM_OctreeNode::NodeIndex index);
	void freeNode (ATOM_OctreeNode *node);
	void clearNodes (void);
	ATOM_OctreeNode::NodeIndex getChildIndex (ATOM_OctreeNode::NodeIndex index, ATOM_OctreeNode::Placement placement) const;
	ATOM_OctreeNode::NodeIndex getParentIndex (ATOM_OctreeNode::NodeIndex index) const;

private:
	void setLevel (unsigned level);
	void setDimension (unsigned dimension);
	void setNodeSize (float size);
	void setNodeSizeLoosed (float size);
	void setNext (ATOM_OctreeNodeChunk *chunk);
	void setPrev (ATOM_OctreeNodeChunk *chunk);

public:
	float getNodeSize (void) const;
	float getNodeSizeLoosed (void) const;
	float getWorldSize (void) const;
	unsigned getDimension (void) const;
	unsigned getLevel (void) const;
	bool empty (void) const;
	ATOM_OctreeNodeChunk *getNext (void) const;
	ATOM_OctreeNodeChunk *getPrev (void) const;
	ATOM_Octree *getOctree (void) const;

private:
	unsigned _M_level;
	unsigned _M_dimension;
	float _M_node_size;
	float _M_loose_size;
	ATOM_OctreeNodeChunk *_M_next;
	ATOM_OctreeNodeChunk *_M_prev;
	ATOM_Octree *_M_octree;
	node_map _M_node_map;
};

class ATOM_Octree
{
public:
	ATOM_Octree (void);
	~ATOM_Octree (void);

public:
	void initialize (float rootsize, float leafsize);
	void finalize (void);
	float getRootSize (void) const;
	float getLeafSize (void) const;
	ATOM_OctreeNode *locateNodeChain (const ATOM_Vector3f &center, float radius);
	ATOM_OctreeNode *locateNodeChain2 (ATOM_OctreeNode *candidate, const ATOM_Vector3f &center, float radius);
	ATOM_OctreeNode *getRootNode (void);
	unsigned getNumChunks (void) const;
	ATOM_OctreeNodeChunk *getChunk (unsigned level) const;

private:
	ATOM_VECTOR<ATOM_OctreeNodeChunk*> _M_chunks;
	float _M_root_size;
	float _M_leaf_size;
	ATOM_OctreeNode *_M_root_node;
};

inline void ATOM_OctreeNode::setUserData (void *data)
{
  _M_userdata = data;
}

inline void *ATOM_OctreeNode::getUserData (void) const
{
  return _M_userdata;
}

#endif // __ATOM3D_ENGINE_OCTREE_H
