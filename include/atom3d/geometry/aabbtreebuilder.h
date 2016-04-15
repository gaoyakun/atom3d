#ifndef __ATOM_GEOMETRY_AABBTREEBUILER_H
#define __ATOM_GEOMETRY_AABBTREEBUILER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"
#include "aabbtree.h"

class ATOM_AABBTree;

class ATOM_AABBTreeBuilder
{
public:
	ATOM_AABBTreeBuilder (void);
	virtual ~ATOM_AABBTreeBuilder (void);

public:
	bool buildFromPrimitives (ATOM_AABBTree *tree, ATOM_AABBTree::PrimitiveType prim, const ATOM_Vector3f *verts, unsigned numVerts, const unsigned short *indices, unsigned numPrimitives, unsigned maxLeafPrimitiveCount, int maxDepth = 0);

private:
	struct WorkingItem
	{
		int depth;
		int left;
		int right;
		unsigned prim;
		unsigned numPrims;
		std::vector<unsigned> primitives;
		ATOM_BBox aabb;
	};

	struct Box
	{
		ATOM_BBox box;
		int reference;
	};

	void workOnItem (ATOM_AABBTree::PrimitiveType prim, WorkingItem &item, unsigned maxLeafPrimitiveCount, int maxDepth);
	int classifyPrimitive (unsigned primitive, int axis, float middle, float &minval, float &maxval);
	void takeMinMax (unsigned primitive, int axis, float &minval, float &maxval);
	float calcAvgPoint (WorkingItem &item, int axis);
	bool buildFinalTree (ATOM_AABBTree *tree, ATOM_AABBTree::PrimitiveType prim);
	void reset (void);

	unsigned allocPrimitiveList (unsigned numPrimitives);
	unsigned calcIndexCount (ATOM_AABBTree::PrimitiveType primitveType, unsigned numPrimitives) const;
	void getPrimitive (unsigned index, const unsigned short *indices, ATOM_AABBTree::PrimitiveType primitiveType, unsigned short *out) const;

private:
	std::vector<ATOM_Vector3f> _vertices;
	std::vector<unsigned short> _indices;
	std::vector<WorkingItem*> _items;
	std::vector<unsigned> _leafPrimitiveLists;
	std::vector<unsigned short> _extractedIndices;
};

#endif // __ATOM_GEOMETRY_AABBTREEBUILER_H

