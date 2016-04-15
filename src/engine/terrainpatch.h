#ifndef __ATOM3D_ENGINE_TERRAINPATCH_H
#define __ATOM3D_ENGINE_TERRAINPATCH_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <ATOM_render.h>

#include "basedefs.h"
#include "geometry.h"

class ATOM_ENGINE_API ATOM_TerrainQuadtree;
class ATOM_ENGINE_API ATOM_Camera;

class ATOM_ENGINE_API ATOM_TerrainPatch
{
public:
  enum PatchPosition
  {
    LeftTop = 0,
    RightTop,
    LeftBottom,
    RightBottom
  };

public:
  ATOM_TerrainPatch (void);
  virtual ~ATOM_TerrainPatch (void);

public:
  bool initialize (ATOM_TerrainQuadtree *quadtree, ATOM_TerrainPatch *parent, PatchPosition position, ATOM_VertexArray *baseVertices);
  void setupCamera (int viewportH, float tanHalfFovy, unsigned maxPixelError);
  void setupVertices (float skirtLength, ATOM_VertexArray *baseVertices);
  unsigned getMipLevel (void) const;
  PatchPosition getPosition (void) const;
  unsigned getOffsetX (void) const;
  unsigned getOffsetZ (void) const;
  unsigned getStep (void) const;
  float getLodDistance (void) const;
  const ATOM_BBox &getBoundingbox (void) const;
  bool insideDetailRegion (const ATOM_Vector3f &eyePos, float distance) const;

public:
	virtual void render (ATOM_RenderDevice *device);

private:
  float getHeight (unsigned x, unsigned z) const;
  float computeMaxError (void) const;
  float computeErrorMetric (ATOM_TerrainPatch *other) const;
  float computeLodDistance (int viewportH, float tanHalfFovy, unsigned maxPixelError) const;
  float computeSkirtLength (void) const;
  float computeBoundingBox (ATOM_BBox &bbox) const;
  void computeHeightBound (float &maxHeight, float &minHeight) const;

private:
  ATOM_MultiStreamGeometry *_geometry;
  ATOM_TerrainQuadtree *_quadtree;
  unsigned _mipLevel;
  unsigned _offsetX;
  unsigned _offsetZ;
  unsigned _step;
  PatchPosition _position;
  ATOM_BBox _boundingBox;
  float _boxRadius;
  float _lodDistance;
  float _maxError;
  ATOM_TerrainPatch *_parent;
};

#endif // __ATOM3D_ENGINE_TERRAINPATCH_H
