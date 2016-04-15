#ifndef __ATOM_MATH_VECTOR2_H
#define __ATOM_MATH_VECTOR2_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "defs.h"
#include "globals.h"

struct ATOM_Vector2f
{
  union
  {
    struct
    {
      float x;
      float y;
    };
    float xy[2];
  };

  ATOM_Vector2f (void);
  ATOM_Vector2f (float fillval);
  ATOM_Vector2f (float x_, float y_);

  void set (float x_, float y_);
  bool almostEqual (const ATOM_Vector2f &other) const;
  float getLength (void) const;
  float getSquaredLength (void) const;
  void normalize (void);
  void normalizeFrom (const ATOM_Vector2f &v);
  void normalize_lp (void);
  void normalizeFrom_lp (const ATOM_Vector2f &v);

  friend ATOM_Vector2f operator + (const ATOM_Vector2f &v1, const ATOM_Vector2f &v2);
  friend ATOM_Vector2f operator - (const ATOM_Vector2f &v1, const ATOM_Vector2f &v2);
  friend ATOM_Vector2f operator * (const ATOM_Vector2f &v1, const ATOM_Vector2f &v2);
  friend ATOM_Vector2f operator * (const ATOM_Vector2f &v, float f);
  friend ATOM_Vector2f operator * (float f, const ATOM_Vector2f &v);
  friend ATOM_Vector2f operator / (const ATOM_Vector2f &v1, const ATOM_Vector2f &v2);
  friend ATOM_Vector2f operator / (const ATOM_Vector2f &v, float f);
  friend bool operator == (const ATOM_Vector2f &v1, const ATOM_Vector2f &v2);
  friend bool operator != (const ATOM_Vector2f &v1, const ATOM_Vector2f &v2);
  friend float dotProduct (const ATOM_Vector2f &v1, const ATOM_Vector2f &v2);

  ATOM_Vector2f operator - (void) const;
  ATOM_Vector2f & operator += (const ATOM_Vector2f &v);
  ATOM_Vector2f & operator -= (const ATOM_Vector2f &v);
  ATOM_Vector2f & operator *= (const ATOM_Vector2f &v);
  ATOM_Vector2f & operator *= (float f);
  ATOM_Vector2f & operator /= (const ATOM_Vector2f &v);
  ATOM_Vector2f & operator /= (float f);

  float operator [] (int i) const;
  float & operator [] (int i);
};

#include "vector2.inl"

#endif // __ATOM_MATH_VECTOR2_H
