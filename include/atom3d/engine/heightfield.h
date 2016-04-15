#ifndef __ATOM3D_ENGINE_HEIGHTFIELD_H
#define __ATOM3D_ENGINE_HEIGHTFIELD_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"
#include "../ATOM_kernel.h"
#include "../ATOM_geometry.h"
#include "basedefs.h"

#if 1
struct HeightFieldBBoxTreeNode
{
	ATOM_BBox bbox;
	ATOM_Rect2Di rc;
	int v[4];
	HeightFieldBBoxTreeNode *left;
	HeightFieldBBoxTreeNode *right;
};

class HeightFieldBBoxTree: private ATOM_Noncopyable
{
public:
	HeightFieldBBoxTree (int res_x, int res_y, const ATOM_Vector4f *vertices);
	~HeightFieldBBoxTree (void);

	bool pick (const ATOM_Ray &ray, float &d);
	bool create (int res_x, int res_y, const ATOM_Vector4f *vertices);
	float getHeight (int x, int y) const;
	float getRealHeight (float x, float y) const;
	HeightFieldBBoxTreeNode *getRootNode (void) const;
	float *getHeights (void) const;
	void updateHeightValue (int x, int z, float height);

private:
	void deleteNode (HeightFieldBBoxTreeNode *node);
	HeightFieldBBoxTreeNode *allocNode (void);
	bool createChildNode (HeightFieldBBoxTreeNode *node, int x, int y, int w, int h, const ATOM_Vector4f *vertices);
	void updateChildNode (HeightFieldBBoxTreeNode *node, int x, int y, float height);
	HeightFieldBBoxTreeNode *rayIntersectionTestR (HeightFieldBBoxTreeNode *node, const ATOM_Ray &ray, float *d) const;

private:
	int _resX;
	int _resY;
	float *_heights;
	HeightFieldBBoxTreeNode *_rootNode;

	struct NodeCacheEntry
	{
		HeightFieldBBoxTreeNode *nodeCache;
		unsigned cacheSize;
		unsigned nextNodeIndex;
	};
	ATOM_VECTOR<NodeCacheEntry> _nodeCache;
};

class ATOM_ENGINE_API ATOM_HeightField
{
public:
	ATOM_HeightField (void);
	~ATOM_HeightField (void);

public:
	bool init (int sizeX, int sizeZ, float offset_x, float offset_z, float spacing_x, float spacing_z, float vscale, const float *heights);
	bool resetHeights (const float *heights);
	bool init (int sizeX, int sizeZ, const ATOM_Vector4f *vertices);
	void clear ();
	bool rayIntersect (const ATOM_Vector3f &origin, const ATOM_Vector3f &direction, float &t, bool cull) const;
	bool rayIntersectionTest (const ATOM_Vector3f &origin, const ATOM_Vector3f &direction, bool cull) const;
	float getRealHeight (float x, float z) const;
	void computeNormals (ATOM_Vector3f *normals) const;
	void updateHeightValue (int x, int z, float height);
	HeightFieldBBoxTree *getBBoxTree (void) const;

public:
	float getSpacingX (void) const;
	float getSpacingZ (void) const;
	float getVerticalScale (void) const;
	int getSizeX (void) const;
	int getSizeZ (void) const;
	float getOffsetX (void) const;
	float getOffsetZ (void) const;
	float getHeight (unsigned x, unsigned z) const;
	ATOM_BBox getBoundingbox (void) const;
	float *getHeights (void) const;

private:
	ATOM_Vector4f	m_v4Range;
	ATOM_Vector3f	m_scale;
	int m_sizeX;
	int m_sizeZ;
	HeightFieldBBoxTree	*m_bboxTree;
};

#else

class ATOM_ENGINE_API ATOM_HeightField
{
	struct bound
	{
		float y_min;
		float y_max;
	};

public:
	ATOM_HeightField (void);
	~ATOM_HeightField (void);

public:
	bool init (int size, float offset_x, float offset_z, float spacing_x, float spacing_z, float vscale, const float *heights, int blocksize = 2);
	void clear ();
	bool rayIntersect (const ATOM_Vector3f &origin, const ATOM_Vector3f &direction, float &t, bool cull) const;
	bool lssIntersect (const ATOM_Vector3f &origin, const ATOM_Vector3f &direction, float radius, float &t) const;
	bool rayIntersectionTest (const ATOM_Vector3f &origin, const ATOM_Vector3f &direction, bool cull) const;
	ATOM_Vector3f *getVertices (void) const;
	float getRealHeight (float x, float z) const;
	void computeNormals (ATOM_Vector3f *normals) const;

public:
	float getSpacingX (void) const;
	float getSpacingZ (void) const;
	float getVerticalScale (void) const;
	int getSize (void) const;
	float getOffsetX (void) const;
	float getOffsetZ (void) const;
	float getHeight (unsigned x, unsigned z) const;
	ATOM_BBox getBoundingbox (int level, int col, int row) const;
	float *getHeights (void) const;
	void lock (void) const;
	void unlock (void) const;

private:
	bool rayIntersect_R (const ATOM_Ray &ray, int level, int col, int row, float &dist, bool cull, bool testOnly) const;
	bool lssIntersect_R (const ATOM_Ray &ray, float radius, int level, int col, int row, float &dist) const;
	bool computeMinMax (int blocksize);
	void getMinMax (int level, int col, int row, float &minx, float &maxx, float &minz, float &maxz) const;

private:
	int _blocksize;
	int _size;
	int _num_levels;
	float _spacing_x;
	float _spacing_z;
	float _vertical_scale;
	float _offset_x;
	float _offset_z;
	bound *_minmax;
	float *_heights;
};

static inline int nodeCount (int depth)
{
    return 0x55555555 & ((1 << depth * 2) - 1);
}

static inline int nodeIndex (int level, int col, int row)
{
    ATOM_ASSERT (col >= 0 && col < (1 << level));
    ATOM_ASSERT (row >= 0 && row < (1 << level));
    return nodeCount (level) + (row << level) + col;
}

// inline member functions
inline float ATOM_HeightField::getSpacingX (void) const 
{
  return _spacing_x;
}

inline float ATOM_HeightField::getSpacingZ (void) const 
{
  return _spacing_z;
}

inline float ATOM_HeightField::getOffsetX (void) const
{
	return _offset_x;
}

inline float ATOM_HeightField::getOffsetZ (void) const
{
	return _offset_z;
}

inline float ATOM_HeightField::getVerticalScale (void) const 
{
  return _vertical_scale;
}

inline int ATOM_HeightField::getSize (void) const 
{
  return _size;
}

inline ATOM_BBox ATOM_HeightField::getBoundingbox (int level, int col, int row) const 
{
  float x_min, x_max, z_min, z_max;
  getMinMax (level, col, row, x_min, x_max, z_min, z_max);
  int index = nodeIndex (level, col, row);
  ATOM_Vector3f minPoint(x_min, _minmax[index].y_min, z_min);
  ATOM_Vector3f maxPoint(x_max, _minmax[index].y_max, z_max);
  return ATOM_BBox(minPoint, maxPoint);
}

inline float ATOM_HeightField::getHeight (unsigned x, unsigned z) const 
{
  return _heights[z * _size + x] * _vertical_scale; 
}

inline void ATOM_HeightField::getMinMax (int level, int col, int row, float &minx, float &maxx, float &minz, float &maxz) const
{
	unsigned scalar = 1 << (_num_levels-1-level);
	float bsx = _blocksize * scalar * _spacing_x;
	float bsz = _blocksize * scalar * _spacing_z;;
	minx = col * bsx + _offset_x;
	maxx = minx + bsx;
	minz = row * bsz + _offset_z;
	maxz = minz + bsz;
}

inline float *ATOM_HeightField::getHeights (void) const
{
	return _heights;
}

#endif

#endif // __ATOM3D_ENGINE_HEIGHTFIELD_H
