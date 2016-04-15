#ifndef __ATOM_MATH_VECTOR4_H
#define __ATOM_MATH_VECTOR4_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "defs.h"
#include "vector3.h"

struct ATOM_Matrix4x4f;

struct ATOM_Vector4f
{
  union
  {
    struct
    {
      float x;
      float y;
      float z;
      float w;
    };
    float xyzw[4];
  };

  ATOM_Vector4f (void);
  ATOM_Vector4f (float fillval);
  ATOM_Vector4f (float x_, float y_, float z_, float w_);
  ATOM_Vector4f (const ATOM_Vector4f &v);
  ATOM_Vector4f (const ATOM_Vector3f &v);
  ATOM_Vector4f & operator = (const ATOM_Vector4f &v);

  void set (float x_, float y_, float z_, float w_);
  void homogeneous (void);
  bool isHomogeneous (void) const;
  bool almostEqual (const ATOM_Vector4f &other) const;
  float getLength (void) const;
  float getSquaredLength (void) const;
  float getLength3 (void) const;
  float getSquaredLength3 (void) const;
  void normalize (void);
  void normalizeFrom (const ATOM_Vector4f &v);
  void normalize3 (void);
  void normalize3From (const ATOM_Vector4f &v);
  void normalize3From (const ATOM_Vector3f &v);
  void normalize_lp (void);
  void normalizeFrom_lp (const ATOM_Vector4f &v);
  void normalize3_lp (void);
  void normalize3From_lp (const ATOM_Vector4f &v);
  void normalize3From_lp (const ATOM_Vector3f &v);
  void setVector3 (const ATOM_Vector3f &v);
  ATOM_Vector3f getVector3 (void) const;
  friend ATOM_Vector4f operator + (const ATOM_Vector4f &v1, const ATOM_Vector4f &v2);
  friend ATOM_Vector4f operator - (const ATOM_Vector4f &v1, const ATOM_Vector4f &v2);
  friend ATOM_Vector4f operator * (const ATOM_Vector4f &v1, const ATOM_Vector4f &v2);
  friend ATOM_Vector4f operator * (const ATOM_Vector4f &v, float f);
  friend ATOM_Vector4f operator * (float f, const ATOM_Vector4f &v);
  friend ATOM_Vector4f operator / (const ATOM_Vector4f &v1, const ATOM_Vector4f &v2);
  friend ATOM_Vector4f operator / (const ATOM_Vector4f &v, float f);
  friend bool operator == (const ATOM_Vector4f &v1, const ATOM_Vector4f &v2);
  friend bool operator != (const ATOM_Vector4f &v1, const ATOM_Vector4f &v2);
  friend float dotProduct (const ATOM_Vector4f &v1, const ATOM_Vector4f &v2);
  friend float dotProduct3 (const ATOM_Vector4f &v1, const ATOM_Vector4f &v2);
  friend ATOM_Vector4f crossProduct3 (const ATOM_Vector4f &v1, const ATOM_Vector4f &v2);

  ATOM_Vector4f operator - (void) const;
  ATOM_Vector4f & operator += (const ATOM_Vector4f &v);
  ATOM_Vector4f & operator -= (const ATOM_Vector4f &v);
  ATOM_Vector4f & operator *= (const ATOM_Vector4f &v);
  ATOM_Vector4f & operator *= (float f);
  ATOM_Vector4f & operator /= (const ATOM_Vector4f &v);
  ATOM_Vector4f & operator /= (float f);
  ATOM_Vector4f & operator <<= (const ATOM_Matrix4x4f &m);

  float operator [] (int i) const;
  float & operator [] (int i);
};

#include "vector4.inl"
#endif // __ATOM_MATH_VECTOR4_H
