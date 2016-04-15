#ifndef __ATOM_GEOMETRY_COLLISIONOBJECT_H
#define __ATOM_GEOMETRY_COLLISIONOBJECT_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"

class ATOM_CollisionObject
  {
  public:
    virtual ~ATOM_CollisionObject () {}
    virtual void addTriangle (const ATOM_Vector3f &v1, const ATOM_Vector3f &v2, const ATOM_Vector3f &v3) = 0;
    virtual void prepare () = 0;
    virtual void setTransform (const ATOM_Matrix4x4f &modelview) = 0;
    virtual bool collisionTest (ATOM_CollisionObject *other, const float *transform) = 0;
    virtual bool rayIntersect (const ATOM_Vector3f &origin, const ATOM_Vector3f &direction, bool closest) = 0;
    virtual bool segmentIntersect (const ATOM_Vector3f &origin, const ATOM_Vector3f &direction, bool closest) = 0;
    virtual bool sphereIntersect (const ATOM_Vector3f &origin, float radius) = 0;
    virtual bool getCollisionPoint (ATOM_Vector3f &point, bool modelspace) = 0;

  public:
    static ATOM_CollisionObject * create (bool dynamic, unsigned reserved = 0);
    static void              destroy (ATOM_CollisionObject *object);
  };

#endif // __ATOM_GEOMETRY_COLLISIONOBJECT_H
