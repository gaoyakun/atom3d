#ifndef __ATOM3D_ENGINE_TERRAINQUADTREE_H
#define __ATOM3D_ENGINE_TERRAINQUADTREE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <ATOM_render.h>

#include "basedefs.h"

class ATOM_TerrainQuadtreeNode;
class ATOM_Camera;
class ATOM_RenderQueue;
class ATOM_Terrain;
class ATOM_HeightField;
class ATOM_TerrainRenderDatas;

class ATOM_ENGINE_API ATOM_TerrainQuadtree
{
public:
  ATOM_TerrainQuadtree (ATOM_Terrain *terrain);
  ~ATOM_TerrainQuadtree (void);

public:
  bool build (unsigned patchSize, unsigned rootSize, float *elevations, float scaleX, float scaleZ, unsigned vertexCacheSize);
  void strip (ATOM_VECTOR<unsigned short> &indices, unsigned vertexCacheSize);
  void setupCamera (int viewportH, float tanHalfFovy, unsigned maxPixelError);
  void cull (ATOM_Camera *camera, const ATOM_Vector3f &viewPoint, const ATOM_Matrix4x4f &worldMatrix, ATOM_TerrainRenderDatas *rd, bool drawBasemap);
  void getBoundingbox (ATOM_BBox &bbox) const;

public:
  unsigned getPatchSize (void) const;
  unsigned getRootSize (void) const;
  ATOM_Terrain *getTerrain (void) const;
  const float *getElevations (void) const;
  float getScaleX (void) const;
  float getScaleZ (void) const;
  ATOM_IndexArray *getIndices (void) const;
  unsigned getPrimitiveCount (void) const;
  int getPrimitiveType (void) const;
  bool rayIntersect (const ATOM_Vector3f &origin, const ATOM_Vector3f &direction, float &t) const;
  ATOM_HeightField *getHeightField (void) const;

private:
	void cull_r (ATOM_Camera *camera, ATOM_TerrainQuadtreeNode *node, const ATOM_Vector3f &viewPoint, const ATOM_Matrix4x4f &mvp, const ATOM_Matrix4x4f &worldMatrix, bool cliptest, ATOM_TerrainRenderDatas *rd, bool drawBasemap);

private:
	ATOM_AUTOREF(ATOM_VertexArray) _baseVertices;
	ATOM_AUTOREF(ATOM_IndexArray) _indices;
	float _scaleX;
	float _scaleZ;
	unsigned _patchSize;
	unsigned _rootSize;
	unsigned _primitiveCount;
	unsigned _primitiveType;
	ATOM_TerrainQuadtreeNode *_rootNode;
	ATOM_Terrain *_terrain;
	ATOM_HeightField *_heightfield;
};

#endif // __ATOM3D_ENGINE_TERRAINQUADTREE_H
