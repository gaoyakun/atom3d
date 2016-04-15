#ifndef __ATOM_GEOMETRY_AABBTREE_H
#define __ATOM_GEOMETRY_AABBTREE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <vector>

#include "../ATOM_math.h"

class ATOM_Ray;

class ATOM_AABBTree
{
	friend class ATOM_AABBTreeBuilder;

public:
	enum PrimitiveType
	{
		PT_TRIANGLES,
		PT_TRIANGLEFAN,
		PT_TRIANGLESTRIP,
		PT_LINES,
		PT_LINESTRIP,
		PT_POINTS
	};

	struct AABBTreeNode
	{
		int left;
		int right;
		unsigned primitive;
		unsigned numPrimitives;
		ATOM_BBox bbox;
	};

public:
	ATOM_AABBTree (void);
	virtual ~ATOM_AABBTree (void);

public:
	void save (void *handle) const;
	void load (void *handle);
	unsigned getStoreSize (void) const;
	PrimitiveType getPrimitiveType (void) const;
	const std::vector<AABBTreeNode> & getNodes (void) const;
	const std::vector<ATOM_Vector3f> & getVertices (void) const;
	const std::vector<unsigned> & getLeafPrimitiveList (void) const;

public:
	int rayIntersect (const ATOM_Ray &ray, float &d, bool boxTestOnly = false) const;
	bool rayIntersectionTest (const ATOM_Ray &ray, bool boxTestOnly = false) const;

protected:
	virtual unsigned writeFile (void *handle, const void *buffer, unsigned size) const;
	virtual unsigned readFile (void *handle, void *buffer, unsigned size) const;

private:
	void getPrimitive (unsigned index, const unsigned short *indices, PrimitiveType primitiveType, unsigned short *out) const;
	void rayIntersectR (unsigned node, const ATOM_Ray &ray, float &d, int &t, bool testOnly, bool boxTestOnly, int *indices = 0) const;

protected:
	PrimitiveType _primitiveType;
	std::vector<AABBTreeNode> _nodes;
	std::vector<ATOM_Vector3f> _vertices;
	std::vector<unsigned short> _indices;
	std::vector<unsigned> _leafPrimitiveLists;
};

#endif // __ATOM_GEOMETRY_AABBTREE_H
