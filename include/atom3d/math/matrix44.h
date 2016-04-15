#ifndef __ATOM_MATH_MATRIX44_H
#define __ATOM_MATH_MATRIX44_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "defs.h"
#include "matrix33.h"
#include "vector3.h"
#include "vector4.h"

struct ATOM_Matrix4x4f
{
  union
  {
    struct
    {
      float m00, m01, m02, m03;
      float m10, m11, m12, m13;
      float m20, m21, m22, m23;
      float m30, m31, m32, m33;
    };
    float m[4*4];
  };

  ATOM_Matrix4x4f (void);
  ATOM_Matrix4x4f (float fillval);
  ATOM_Matrix4x4f (float m00_, float m01_, float m02_, float m03_,
              float m10_, float m11_, float m12_, float m13_,
              float m20_, float m21_, float m22_, float m23_,
              float m30_, float m31_, float m32_, float m33_);
  ATOM_Matrix4x4f (const ATOM_Matrix3x3f &upper3);
  ATOM_Matrix4x4f (const ATOM_Matrix3x3f &upper3, const ATOM_Vector3f &translation);
  ATOM_Matrix4x4f (const ATOM_Matrix4x4f &other);
  ATOM_Matrix4x4f & operator = (const ATOM_Matrix4x4f &other);

  bool almostEqual (const ATOM_Matrix4x4f &other) const;
  ATOM_Vector4f getRow (int row) const;
  ATOM_Vector4f getCol (int col) const;
  void getRow (int row, ATOM_Vector4f &v) const;
  void getCol (int col, ATOM_Vector4f &v) const;
  void setRow (int row, const ATOM_Vector4f &v);
  void setCol (int col, const ATOM_Vector4f &v);
  void setRow (int row, float x, float y, float z, float w);
  void setCol (int col, float x, float y, float z, float w);
  void setRow (int row, const ATOM_Vector3f &v, float f);
  void setCol (int col, const ATOM_Vector3f &v, float f);
  ATOM_Vector3f getRow3 (int row) const;
  ATOM_Vector3f getCol3 (int col) const;
  void getRow3 (int row, ATOM_Vector3f &v) const;
  void getCol3 (int col, ATOM_Vector3f &v) const;
  void setRow3 (int row, const ATOM_Vector3f &v);
  void setCol3 (int col, const ATOM_Vector3f &v);
  void setRow3 (int row, float x, float y, float z);
  void setCol3 (int col, float x, float y, float z);
  void fill (float val);
  void set (float m00_, float m01_, float m02_, float m03_,
            float m10_, float m11_, float m12_, float m13_,
            float m20_, float m21_, float m22_, float m23_,
            float m30_, float m31_, float m32_, float m33_);
  ATOM_Matrix3x3f getUpper3 (void) const;
  void setUpper3 (const ATOM_Matrix3x3f &upper3);
  float operator () (int row, int col) const;
  float & operator () (int row, int col);
  bool isNAN (void) const;
  void decompose (ATOM_Vector3f &translate, ATOM_Matrix4x4f &rotation, ATOM_Vector3f &scale) const;
  void decompose (ATOM_Vector3f &translate, ATOM_Matrix3x3f &rotation, ATOM_Vector3f &scale) const;
  void toEulerXYZ (float &x, float &y, float &z) const;

  float getDeterminant (void) const;
  void transpose (void);
  void transposeFrom (const ATOM_Matrix4x4f &other);
  void invert (void);
  void invertAffine (void);
  void invertFrom (const ATOM_Matrix4x4f &other);
  void invertAffineFrom (const ATOM_Matrix4x4f &other);
  void invertTranspose (void);
  void invertTransposeFrom (const ATOM_Matrix4x4f &other);
  void invertTransposeAffine (void);
  void invertTransposeAffineFrom (const ATOM_Matrix4x4f &other);

  friend ATOM_Matrix4x4f operator >> (const ATOM_Matrix4x4f &m1, const ATOM_Matrix4x4f &m2);
  friend ATOM_Matrix4x4f operator << (const ATOM_Matrix4x4f &m1, const ATOM_Matrix4x4f &m2);
  friend ATOM_Vector4f operator >> (const ATOM_Matrix4x4f &m, const ATOM_Vector4f &v);
  friend ATOM_Vector4f operator << (const ATOM_Vector4f &v, const ATOM_Matrix4x4f &m);
  friend ATOM_Vector3f operator >> (const ATOM_Matrix4x4f &m, const ATOM_Vector3f &v);
  friend ATOM_Vector3f operator << (const ATOM_Vector3f &v, const ATOM_Matrix4x4f &m);
  friend ATOM_Matrix4x4f operator + (const ATOM_Matrix4x4f &m1, const ATOM_Matrix4x4f &m2);
  friend ATOM_Matrix4x4f operator - (const ATOM_Matrix4x4f &m1, const ATOM_Matrix4x4f &m2);
  friend ATOM_Matrix4x4f operator * (const ATOM_Matrix4x4f &m1, const ATOM_Matrix4x4f &m2);
  friend ATOM_Matrix4x4f operator * (const ATOM_Matrix4x4f &m, float f);
  friend ATOM_Matrix4x4f operator * (float f, const ATOM_Matrix4x4f &m);
  friend ATOM_Matrix4x4f operator / (const ATOM_Matrix4x4f &m1, const ATOM_Matrix4x4f &m2);
  friend ATOM_Matrix4x4f operator / (const ATOM_Matrix4x4f &m1, float f);
  friend bool operator == (const ATOM_Matrix4x4f &m1, const ATOM_Matrix4x4f &m2);
  friend bool operator != (const ATOM_Matrix4x4f &m1, const ATOM_Matrix4x4f &m2);

  ATOM_Matrix4x4f & operator >>= (const ATOM_Matrix4x4f &other);
  ATOM_Matrix4x4f & operator <<= (const ATOM_Matrix4x4f &other);
  ATOM_Matrix4x4f & operator += (const ATOM_Matrix4x4f &other);
  ATOM_Matrix4x4f & operator -= (const ATOM_Matrix4x4f &other);
  ATOM_Matrix4x4f & operator *= (const ATOM_Matrix4x4f &other);
  ATOM_Matrix4x4f & operator *= (float f);
  ATOM_Matrix4x4f & operator /= (const ATOM_Matrix4x4f &other);
  ATOM_Matrix4x4f & operator /= (float f);

  ATOM_Vector3f transformPoint (const ATOM_Vector3f &v) const;
  ATOM_Vector3f transformVector(const ATOM_Vector3f &v) const;
  ATOM_Vector4f transformPoint (const ATOM_Vector4f &v) const;
  ATOM_Vector4f transformVector (const ATOM_Vector4f &v) const;
  ATOM_Vector3f transformPointAffine (const ATOM_Vector3f &v) const;
  ATOM_Vector3f transformVectorAffine (const ATOM_Vector3f &v) const;
  ATOM_Vector4f transformPointAffine (const ATOM_Vector4f &v) const;
  ATOM_Vector4f transformVectorAffine (const ATOM_Vector4f &v) const;
  void transformPointInplace (ATOM_Vector3f &v) const;
  void transformPointInplace (ATOM_Vector4f &v) const;
  void transformVectorInplace (ATOM_Vector3f &v) const;
  void transformVectorInplace (ATOM_Vector4f &v) const;
  void transformPointAffineInplace (ATOM_Vector3f &v) const;
  void transformPointAffineInplace (ATOM_Vector4f &v) const;
  void transformVectorAffineInplace (ATOM_Vector3f &v) const;
  void transformVectorAffineInplace (ATOM_Vector4f &v) const;

  void makeIdentity (void);
  void makeRotateX (float angle);
  void makeRotateY (float angle);
  void makeRotateZ (float angle);
  void makeRotateAngleAxis (float angle, const ATOM_Vector3f &axis);
  void makeRotateAngleNormalizedAxis (float angle, const ATOM_Vector3f &normalizedAxis);
  void makeScale (const ATOM_Vector3f &scale);
  void makeTranslate (const ATOM_Vector3f &translation);
  void makeLookatLH (const ATOM_Vector3f &eye, const ATOM_Vector3f &to, const ATOM_Vector3f &up);
  void makeLookatRH (const ATOM_Vector3f &eye, const ATOM_Vector3f &to, const ATOM_Vector3f &up);
  void makePerspectiveFovLH (float fovY, float aspect, float znear, float zfar);
  void makePerspectiveFovRH (float fovY, float aspect, float znear, float zfar);
  void makePerspectiveFrustumLH (float l, float r, float b, float t, float n, float f);
  void makePerspectiveFrustumRH (float l, float r, float b, float t, float n, float f);
  void makeOrthoLH (float w, float h, float znear, float zfar);
  void makeOrthoRH (float w, float h, float znear, float zfar);
  void makeOrthoFrustumLH (float l, float r, float b, float t, float n, float f);
  void makeOrthoFrustumRH (float l, float r, float b, float t, float n, float f);
  void makeBillboard (const ATOM_Vector3f &eye, const ATOM_Vector3f &to, const ATOM_Vector3f &up);
  void makeReflection (const ATOM_Vector4f &plane);
  void decomposeLookatLH (ATOM_Vector3f &eye, ATOM_Vector3f &to, ATOM_Vector3f &up) const;
  void decomposeLookatRH (ATOM_Vector3f &eye, ATOM_Vector3f &to, ATOM_Vector3f &up) const;

  static const ATOM_Matrix4x4f &getIdentityMatrix (void);
  static ATOM_Matrix4x4f getRotateXMatrix (float angle);
  static ATOM_Matrix4x4f getRotateYMatrix (float angle);
  static ATOM_Matrix4x4f getRotateZMatrix (float angle);
  static ATOM_Matrix4x4f getRotateMatrixAngleAxis (float angle, const ATOM_Vector3f &axis);
  static ATOM_Matrix4x4f getRotateMatrixAngleNormalizedAxis (float angle, const ATOM_Vector3f &normalizedAxis);
  static ATOM_Matrix4x4f getScaleMatrix (const ATOM_Vector3f &scale);
  static ATOM_Matrix4x4f getTranslateMatrix (const ATOM_Vector3f &translation);
  static ATOM_Matrix4x4f getLookatLHMatrix (const ATOM_Vector3f &eye, const ATOM_Vector3f &to, const ATOM_Vector3f &up);
  static ATOM_Matrix4x4f getLookatRHMatrix (const ATOM_Vector3f &eye, const ATOM_Vector3f &to, const ATOM_Vector3f &up);
  static ATOM_Matrix4x4f getPerspectiveFovLHMatrix (float fovY, float aspect, float znear, float zfar);
  static ATOM_Matrix4x4f getPerspectiveFovRHMatrix (float fovY, float aspect, float znear, float zfar);
  static ATOM_Matrix4x4f getPerspectiveFrustumLHMatrix (float l, float r, float b, float t, float n, float f);
  static ATOM_Matrix4x4f getPerspectiveFrustumRHMatrix (float l, float r, float b, float t, float n, float f);
  static ATOM_Matrix4x4f getOrthoLHMatrix (float w, float h, float znear, float zfar);
  static ATOM_Matrix4x4f getOrthoRHMatrix (float w, float h, float znear, float zfar);
  static ATOM_Matrix4x4f getOrthoFrustumLHMatrix (float l, float r, float b, float t, float n, float f);
  static ATOM_Matrix4x4f getOrthoFrustumRHMatrix (float l, float r, float b, float t, float n, float f);
  static ATOM_Matrix4x4f getBillboardMatrix (const ATOM_Vector3f &eye, const ATOM_Vector3f &to, const ATOM_Vector3f &up);
  static ATOM_Matrix4x4f getReflectionMatrix (const ATOM_Vector4f &plane);
};

#if defined(_MSC_VER)
# pragma warning(push)
# pragma warning(disable:4700)
#endif

#include "matrix44.inl"

#if defined(_MSC_VER)
# pragma warning(pop)
#endif

#endif // __ATOM_MATH_MATRIX44_H
