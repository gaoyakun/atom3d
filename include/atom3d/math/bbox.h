#ifndef __ATOM_MATH_BBOX_H
#define __ATOM_MATH_BBOX_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <float.h>
#include "defs.h"
#include "vector3.h"
#include "vector4.h"
#include "matrix44.h"
#include "frustum3d.h"

class ATOM_BBox
{
public:
  enum
  {
    ClipLeft = (1<<0),
    ClipRight = (1<<1),
    ClipBottom = (1<<2),
    ClipTop = (1<<3),
    ClipNear = (1<<4),
    ClipFar = (1<<5)
  };

  enum ClipState
  {
    ClipOutside,
    ClipInside,
    Clipped
  };

  ATOM_BBox (void);
  ATOM_BBox (const ATOM_Vector3f &vMin, const ATOM_Vector3f &vMax);
  const ATOM_Vector3f &getMin (void) const;
  const ATOM_Vector3f &getMax (void) const;
  void setMin (const ATOM_Vector3f &vMin);
  void setMax (const ATOM_Vector3f &vMax);
  ATOM_Vector3f getCenter (void) const;
  ATOM_Vector3f getExtents (void) const;
  ATOM_Vector3f getSize (void) const;
  float getDiagonalSize (void) const;
  void transform (const ATOM_Matrix4x4f &matrix);
  bool intersects (const ATOM_BBox &other) const;
  bool contains (const ATOM_BBox &other) const;
  bool contains (const ATOM_Vector3f &point) const;
  ClipState getClipState (const ATOM_BBox &other) const;
  ClipState getClipState (const ATOM_Matrix4x4f &viewProjection) const;
  ClipState getClipStateMask (const ATOM_Matrix4x4f &viewProjection, unsigned mask) const;
  ClipState getClipState (const ATOM_Matrix4x4f &viewProjection, float minScreenContribute) const;
  ClipState getClipState (const ATOM_Frustum3d &frustum) const;
  ClipState getClipStateMask (const ATOM_Frustum3d &frustum, unsigned mask) const;
  bool intersect (const ATOM_Vector3f &lineBegin, const ATOM_Vector3f &lineVector, ATOM_Vector3f &intersectionPoint) const;
  ATOM_Vector3f point(int i) const;

public:
  void beginExtend (void);
  void extend (const ATOM_Vector3f &v);
  void extend (const ATOM_Vector3f *verts, unsigned numVerts);
  bool isValid (void) const;

public:
	friend inline bool operator == (const ATOM_BBox &bbox1, const ATOM_BBox &bbox2);
	friend inline bool operator != (const ATOM_BBox &bbox1, const ATOM_BBox &bbox2);

private:
  bool _intersectConstX (const float x, const ATOM_Vector3f &lineBegin, const ATOM_Vector3f &lineVector, ATOM_Vector3f &out) const;
  bool _intersectConstY (const float y, const ATOM_Vector3f &lineBegin, const ATOM_Vector3f &lineVector, ATOM_Vector3f &out) const;
  bool _intersectConstZ (const float z, const ATOM_Vector3f &lineBegin, const ATOM_Vector3f &lineVector, ATOM_Vector3f &out) const;
  bool _pipConstX (const ATOM_Vector3f &v) const;
  bool _pipConstY (const ATOM_Vector3f &v) const;
  bool _pipConstZ (const ATOM_Vector3f &v) const;

protected:
  ATOM_Vector3f _minPoint;
  ATOM_Vector3f _maxPoint;
};

#include "bbox.inl"

#endif // __ATOM_MATH_BBOX_H
