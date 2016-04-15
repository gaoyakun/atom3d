#ifndef __ATOM_MATH_MATRIX34_H
#define __ATOM_MATH_MATRIX34_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "defs.h"
#include "matrix33.h"
#include "vector3.h"
#include "vector4.h"

struct ATOM_Matrix3x4f
{
  union
  {
    struct
    {
      float m00, m01, m02, m03;
      float m10, m11, m12, m13;
      float m20, m21, m22, m23;
    };
    float m[3*4];
  };

  ATOM_Matrix3x4f (void);
  ATOM_Matrix3x4f (const ATOM_Matrix3x4f &other);
  explicit ATOM_Matrix3x4f (float fillval);
  ATOM_Matrix3x4f (	float m00_, float m01_, float m02_, float m03_,
					float m10_, float m11_, float m12_, float m13_,
					float m20_, float m21_, float m22_, float m23_);
  explicit ATOM_Matrix3x4f (const ATOM_Matrix3x3f &upper3);
  ATOM_Matrix3x4f (const ATOM_Matrix3x3f &upper3, const ATOM_Vector3f &translation);
  explicit ATOM_Matrix3x4f (const ATOM_Matrix4x4f &other);
  ATOM_Matrix3x4f & operator = (const ATOM_Matrix3x4f &other);
  ATOM_Matrix3x4f & operator = (const ATOM_Matrix4x4f &other);

  bool almostEqual (const ATOM_Matrix3x4f &other) const;
  ATOM_Vector4f getRow (int row) const;
  ATOM_Vector3f getCol (int col) const;
  void setRow (int row, const ATOM_Vector4f &v);
  void setCol (int col, const ATOM_Vector3f &v);
  void fill (float val);
  void set (float m00_, float m01_, float m02_, float m03_,
            float m10_, float m11_, float m12_, float m13_,
            float m20_, float m21_, float m22_, float m23_);
  void set (const ATOM_Matrix4x4f &m44);
  void set (const ATOM_Matrix3x3f &m33);
  void set (const ATOM_Matrix3x3f &m33, const ATOM_Vector3f &translation);
  ATOM_Matrix3x3f getUpper3 (void) const;
  void setUpper3 (const ATOM_Matrix3x3f &upper3);
  float operator () (int row, int col) const;
  float & operator () (int row, int col);
  bool isNAN (void) const;
  void toMatrix44 (ATOM_Matrix4x4f &m44) const;

  friend ATOM_Matrix3x4f operator >> (const ATOM_Matrix3x4f &m1, const ATOM_Matrix3x4f &m2);
  friend ATOM_Matrix3x4f operator << (const ATOM_Matrix3x4f &m1, const ATOM_Matrix3x4f &m2);
  friend ATOM_Vector4f operator >> (const ATOM_Matrix3x4f &m, const ATOM_Vector4f &v);
  friend ATOM_Vector4f operator << (const ATOM_Vector4f &v, const ATOM_Matrix3x4f &m);
  friend ATOM_Vector3f operator >> (const ATOM_Matrix3x4f &m, const ATOM_Vector3f &v);
  friend ATOM_Vector3f operator << (const ATOM_Vector3f &v, const ATOM_Matrix3x4f &m);
  friend ATOM_Matrix3x4f operator + (const ATOM_Matrix3x4f &m1, const ATOM_Matrix3x4f &m2);
  friend ATOM_Matrix3x4f operator - (const ATOM_Matrix3x4f &m1, const ATOM_Matrix3x4f &m2);
  friend ATOM_Matrix3x4f operator * (const ATOM_Matrix3x4f &m1, const ATOM_Matrix3x4f &m2);
  friend ATOM_Matrix3x4f operator * (const ATOM_Matrix3x4f &m, float f);
  friend ATOM_Matrix3x4f operator * (float f, const ATOM_Matrix3x4f &m);
  friend ATOM_Matrix3x4f operator / (const ATOM_Matrix3x4f &m1, const ATOM_Matrix3x4f &m2);
  friend ATOM_Matrix3x4f operator / (const ATOM_Matrix3x4f &m1, float f);
  friend bool operator == (const ATOM_Matrix3x4f &m1, const ATOM_Matrix3x4f &m2);
  friend bool operator != (const ATOM_Matrix3x4f &m1, const ATOM_Matrix3x4f &m2);

  ATOM_Matrix3x4f & operator >>= (const ATOM_Matrix3x4f &other);
  ATOM_Matrix3x4f & operator <<= (const ATOM_Matrix3x4f &other);
  ATOM_Matrix3x4f & operator += (const ATOM_Matrix3x4f &other);
  ATOM_Matrix3x4f & operator -= (const ATOM_Matrix3x4f &other);
  ATOM_Matrix3x4f & operator *= (const ATOM_Matrix3x4f &other);
  ATOM_Matrix3x4f & operator *= (float f);
  ATOM_Matrix3x4f & operator /= (const ATOM_Matrix3x4f &other);
  ATOM_Matrix3x4f & operator /= (float f);

  ATOM_Vector3f transformPoint (const ATOM_Vector3f &v) const;
  ATOM_Vector3f transformVector(const ATOM_Vector3f &v) const;
  ATOM_Vector4f transformPoint (const ATOM_Vector4f &v) const;
  ATOM_Vector3f transformPointAffine (const ATOM_Vector3f &v) const;
  ATOM_Vector3f transformVectorAffine (const ATOM_Vector3f &v) const;
  ATOM_Vector4f transformPointAffine (const ATOM_Vector4f &v) const;
  void transformPointInplace (ATOM_Vector3f &v) const;
  void transformPointInplace (ATOM_Vector4f &v) const;
  void transformVectorInplace (ATOM_Vector3f &v) const;
  void transformPointAffineInplace (ATOM_Vector3f &v) const;
  void transformPointAffineInplace (ATOM_Vector4f &v) const;
  void transformVectorAffineInplace (ATOM_Vector3f &v) const;
  void makeIdentity (void);

  static const ATOM_Matrix3x4f &getIdentityMatrix (void);
};

#if defined(_MSC_VER)
# pragma warning(push)
# pragma warning(disable:4700)
#endif

#include "matrix34.inl"

#if defined(_MSC_VER)
# pragma warning(pop)
#endif

#endif // __ATOM_MATH_MATRIX34_H
