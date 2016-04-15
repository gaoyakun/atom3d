#ifndef __ATOM_MATH_MATRIX33_H
#define __ATOM_MATH_MATRIX33_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <memory.h>
#include "defs.h"
#include "vector3.h"

struct ATOM_Matrix3x3f
{
  union
  {
    struct
    {
      float m00, m01, m02;
      float m10, m11, m12;
      float m20, m21, m22;
    };
    float m[3*3];
  };

  ATOM_Matrix3x3f (void);
  ATOM_Matrix3x3f (float fillval);
  ATOM_Matrix3x3f (float m00_, float m01_, float m02_, float m10_, float m11_, float m12_, float m20_, float m21_, float m22_);

  bool almostEqual (const ATOM_Matrix3x3f &other) const;
  ATOM_Vector3f getRow (int row) const;
  ATOM_Vector3f getCol (int col) const;
  void getRow (int row, ATOM_Vector3f &v) const;
  void getCol (int col, ATOM_Vector3f &v) const;
  void setRow (int row, const ATOM_Vector3f &v);
  void setCol (int col, const ATOM_Vector3f &v);
  void setRow (int row, float x, float y, float z);
  void setCol (int col, float x, float y, float z);
  void fill (float val);
  void set (float m00_, float m01_, float m02_, float m10_, float m11_, float m12_, float m20_, float m21_, float m22_);

  float operator () (int row, int col) const;
  float & operator () (int row, int col);
  bool isNAN (void) const;

  float getDeterminant (void) const;
  void transpose (void);
  void transposeFrom (const ATOM_Matrix3x3f &other);
  bool invert (void);
  bool invertFrom (const ATOM_Matrix3x3f &other);
  bool invertTranspose (void);
  bool invertTransposeFrom (const ATOM_Matrix3x3f &other);

  friend ATOM_Matrix3x3f operator >> (const ATOM_Matrix3x3f &m1, const ATOM_Matrix3x3f &m2);
  friend ATOM_Matrix3x3f operator << (const ATOM_Matrix3x3f &m1, const ATOM_Matrix3x3f &m2);
  friend ATOM_Vector3f operator >> (const ATOM_Matrix3x3f &m, const ATOM_Vector3f &v);
  friend ATOM_Vector3f operator << (const ATOM_Vector3f &v, const ATOM_Matrix3x3f &m);
  friend ATOM_Matrix3x3f operator + (const ATOM_Matrix3x3f &m1, const ATOM_Matrix3x3f &m2);
  friend ATOM_Matrix3x3f operator - (const ATOM_Matrix3x3f &m1, const ATOM_Matrix3x3f &m2);
  friend ATOM_Matrix3x3f operator * (const ATOM_Matrix3x3f &m1, const ATOM_Matrix3x3f &m2);
  friend ATOM_Matrix3x3f operator * (const ATOM_Matrix3x3f &m, float f);
  friend ATOM_Matrix3x3f operator * (float f, const ATOM_Matrix3x3f &m);
  friend ATOM_Matrix3x3f operator / (const ATOM_Matrix3x3f &m1, const ATOM_Matrix3x3f &m2);
  friend ATOM_Matrix3x3f operator / (const ATOM_Matrix3x3f &m1, float f);
  friend bool operator == (const ATOM_Matrix3x3f &m1, const ATOM_Matrix3x3f &m2);
  friend bool operator != (const ATOM_Matrix3x3f &m1, const ATOM_Matrix3x3f &m2);

  ATOM_Matrix3x3f & operator >>= (const ATOM_Matrix3x3f &other);
  ATOM_Matrix3x3f & operator <<= (const ATOM_Matrix3x3f &other);
  ATOM_Matrix3x3f & operator += (const ATOM_Matrix3x3f &other);
  ATOM_Matrix3x3f & operator -= (const ATOM_Matrix3x3f &other);
  ATOM_Matrix3x3f & operator *= (const ATOM_Matrix3x3f &other);
  ATOM_Matrix3x3f & operator *= (float f);
  ATOM_Matrix3x3f & operator /= (const ATOM_Matrix3x3f &other);
  ATOM_Matrix3x3f & operator /= (float f);

  ATOM_Vector3f transform (const ATOM_Vector3f &v) const;
  void transformInplace (ATOM_Vector3f &v) const;

  void makeIdentity (void);
  void makeRotateX (float angle);
  void makeRotateY (float angle);
  void makeRotateZ (float angle);
  void makeRotateAngleAxis (float angle, const ATOM_Vector3f &axis);
  void makeRotateAngleNormalizedAxis (float angle, const ATOM_Vector3f &normalizedAxis);
  void makeScale (const ATOM_Vector3f &scale);

  static ATOM_Matrix3x3f getIdentityMatrix (void);
  static ATOM_Matrix3x3f getRotateXMatrix (float angle);
  static ATOM_Matrix3x3f getRotateYMatrix (float angle);
  static ATOM_Matrix3x3f getRotateZMatrix (float angle);
  static ATOM_Matrix3x3f getRotateAngleAxisMatrix (float angle, const ATOM_Vector3f &axis);
  static ATOM_Matrix3x3f getRotateAngleNormalizedAxisMatrix (float angle, const ATOM_Vector3f &normalizedAxis);
  static ATOM_Matrix3x3f getScaleMatrix (const ATOM_Vector3f &scale);
};

#include "matrix33.inl"

#endif // __ATOM_MATH_MATRIX33_H
