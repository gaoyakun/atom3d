#ifndef __ATOM_MATH_VECTOR3_H
#define __ATOM_MATH_VECTOR3_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "defs.h"
#include "globals.h"

struct ATOM_Matrix3x3f;
struct ATOM_Matrix4x4f;

struct ATOM_Vector3f
{
  union
  {
    struct
    {
      float x;
      float y;
      float z;
    };
    float xyz[3];
  };

  ATOM_Vector3f (void);
  ATOM_Vector3f (float fillval);
  ATOM_Vector3f (float x_, float y_, float z_);

  void set (float x_, float y_, float z_);
  bool almostEqual (const ATOM_Vector3f &other) const;
  float getLength (void) const;
  float getSquaredLength (void) const;
  void normalize (void);
  void normalizeFrom (const ATOM_Vector3f &v);
  void normalize_lp (void);
  void normalizeFrom_lp (const ATOM_Vector3f &v);

  friend ATOM_Vector3f operator + (const ATOM_Vector3f &v1, const ATOM_Vector3f &v2);
  friend ATOM_Vector3f operator - (const ATOM_Vector3f &v1, const ATOM_Vector3f &v2);
  friend ATOM_Vector3f operator * (const ATOM_Vector3f &v1, const ATOM_Vector3f &v2);
  friend ATOM_Vector3f operator * (const ATOM_Vector3f &v, float f);
  friend ATOM_Vector3f operator * (float f, const ATOM_Vector3f &v);
  friend ATOM_Vector3f operator / (const ATOM_Vector3f &v1, const ATOM_Vector3f &v2);
  friend ATOM_Vector3f operator / (const ATOM_Vector3f &v, float f);
  friend bool operator == (const ATOM_Vector3f &v1, const ATOM_Vector3f &v2);
  friend bool operator != (const ATOM_Vector3f &v1, const ATOM_Vector3f &v2);
  friend float dotProduct (const ATOM_Vector3f &v1, const ATOM_Vector3f &v2);
  friend ATOM_Vector3f crossProduct (const ATOM_Vector3f &v1, const ATOM_Vector3f &v2);

  ATOM_Vector3f operator - (void) const;
  ATOM_Vector3f & operator += (const ATOM_Vector3f &v);
  ATOM_Vector3f & operator -= (const ATOM_Vector3f &v);
  ATOM_Vector3f & operator *= (const ATOM_Vector3f &v);
  ATOM_Vector3f & operator *= (float f);
  ATOM_Vector3f & operator /= (const ATOM_Vector3f &v);
  ATOM_Vector3f & operator /= (float f);
  ATOM_Vector3f & operator <<= (const ATOM_Matrix3x3f &m);
  ATOM_Vector3f & operator <<= (const ATOM_Matrix4x4f &m);

  float operator [] (int i) const;
  float & operator [] (int i);
};

#include "vector3.inl"

#endif // __ATOM_MATH_VECTOR3_H
