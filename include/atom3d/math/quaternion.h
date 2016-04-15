/** \file quaternion.h
 *	\��ת��Ԫ����
 *	
 *	\author ������
 *	\ingroup math
 */

#ifndef __ATOM_MATH_QUATERNION_H
#define __ATOM_MATH_QUATERNION_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "defs.h"
#include "vector3.h"
#include "matrix33.h"
#include "matrix44.h"

  //! \class ATOM_Quaternion
  //! ��ת��Ԫ����.
  class ATOM_Quaternion
  {
  public:
	  //! ���캯��.
	  ATOM_Quaternion (void);

	  //! ���캯��.
	  //! \param x_ x
	  //! \param y_ y
	  //! \param z_ z
	  //! \param w_ w
	  ATOM_Quaternion (float x_, float y_, float z_, float w_);

	  //! ������Ԫ������Ԫ�ص�ֵ.
	  //! \param x_ x
	  //! \param y_ y
	  //! \param z_ z
	  //! \param w_ w
	  void set (float x_, float y_, float z_, float w_);

	  //! ��һ����Ԫ��.
	  void normalize (void);

	  //! ����һ����Ԫ����������һ��.
	  //! \param other �������Ԫ������
	  void normalizeFrom (const ATOM_Quaternion &other);

	  //! ��λ����Ԫ��.
	  //! ����Ϊ(0, 0, 0, 1)
	  void identity (void);

	  //! ת��Ϊ���Ĺ���.
	  void conjugate (void);

	  //! ����һ����Ԫ�ؿ�����ת��Ϊ����.
	  //! \param other �������Ԫ������
	  void conjugateFrom (const ATOM_Quaternion &other);

	  //! �������Ԫ����ʾ����תת��Ϊ��ת�����ת�Ƕ�.
	  //! \param angle ��ת�Ƕȣ��������
	  //! \param axis ��ת�ᣬ�������
	  void toAxisAngle (float &angle, ATOM_Vector3f &axis) const;

	  //! ͨ����ת�����ת�Ƕ����ô���Ԫ��.
	  //! \param angle ��ת�Ƕȣ��������
	  //! \param axis ��ת�ᣬ�������
	  void fromAxisAngle (float angle, const ATOM_Vector3f &axis);

	  //! ͨ��3��3��ת��������
	  //! \param matrix ��ת����
	  void fromMatrix (const ATOM_Matrix3x3f &matrix);

	  //! ת��Ϊ3��3��ת����.
	  //! \param matrix ��ת�����������
	  void toMatrix (ATOM_Matrix3x3f &matrix) const;

	  //! ͨ��4��4��ת��������
	  //! \param matrix ��ת����
	  void fromMatrix (const ATOM_Matrix4x4f &matrix);

	  //! ת��Ϊ4��4��ת����.
	  //! \param matrix ��ת�����������
	  void toMatrix (ATOM_Matrix4x4f &matrix) const;

	  //! ת��Ϊŷ����
	  //! \return ŷ����(x, y, z)
	  ATOM_Vector3f toEulerXYZ (void) const;

	  //! ͨ��ŷ��������
	  //! \param eulerAngle ŷ����
	  void fromEulerXYZ (const ATOM_Vector3f &eulerXYZ);

	  //! ������Ԫ����ʹ���ʾΪ��һ��ʸ������һ��ʸ������ת.
	  //! \param from ��תǰ��ʸ��
	  //! \param to ��ת���ʸ��
	  //! \note ��from������-to��ʱ��ú���������ֵ���ȶ���������ڲ���δ��鴦��
	  void setRotationArc (const ATOM_Vector3f &from, const ATOM_Vector3f &to);

	  //! �ô���Ԫ����תһ��ʸ��.
	  //! \param from ��תǰ��ʸ�����������
	  //! \param to ��ת���ʸ�����������
	  void rotateVector (const ATOM_Vector3f &from, ATOM_Vector3f &to) const;

	  //! �����ֵ.
	  //! \param q1 ��һ����Ԫ��
	  //! \param q2 �ڶ�����Ԫ��
	  //! \param t ��ֵ����
	  //! \return ��ֵ�����Ԫ��
	  static ATOM_Quaternion slerp (const ATOM_Quaternion &q1, const ATOM_Quaternion &q2, float t);
	  static ATOM_Quaternion fastSlerp (const ATOM_Quaternion &q1, const ATOM_Quaternion &q2, float t);
	  static ATOM_Quaternion lerp (const ATOM_Quaternion &q1, const ATOM_Quaternion &q2, float t);

	  ATOM_Quaternion & operator >>= (const ATOM_Quaternion &other);
	  ATOM_Quaternion & operator <<= (const ATOM_Quaternion &other);

	  friend ATOM_Quaternion operator >> (const ATOM_Quaternion &q1, const ATOM_Quaternion &q2);
	  friend ATOM_Quaternion operator << (const ATOM_Quaternion &q1, const ATOM_Quaternion &q2);
	  friend bool operator == (const ATOM_Quaternion &q1, const ATOM_Quaternion &q2);
	  friend bool operator != (const ATOM_Quaternion &q1, const ATOM_Quaternion &q2);

	  float x, y, z, w;
  };

#include "quaternion.inl"

#endif // __ATOM_MATH_QUATERNION_H
