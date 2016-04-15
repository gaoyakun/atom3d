/** \file quaternion.h
 *	\旋转四元数类
 *	
 *	\author 高雅昆
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
  //! 旋转四元数类.
  class ATOM_Quaternion
  {
  public:
	  //! 构造函数.
	  ATOM_Quaternion (void);

	  //! 构造函数.
	  //! \param x_ x
	  //! \param y_ y
	  //! \param z_ z
	  //! \param w_ w
	  ATOM_Quaternion (float x_, float y_, float z_, float w_);

	  //! 设置四元数各个元素的值.
	  //! \param x_ x
	  //! \param y_ y
	  //! \param z_ z
	  //! \param w_ w
	  void set (float x_, float y_, float z_, float w_);

	  //! 规一化四元数.
	  void normalize (void);

	  //! 从另一个四元数拷贝并规一化.
	  //! \param other 从这个四元数拷贝
	  void normalizeFrom (const ATOM_Quaternion &other);

	  //! 单位化四元数.
	  //! 设置为(0, 0, 0, 1)
	  void identity (void);

	  //! 转换为它的共轭.
	  void conjugate (void);

	  //! 从另一个四元素拷贝并转换为共轭.
	  //! \param other 从这个四元数拷贝
	  void conjugateFrom (const ATOM_Quaternion &other);

	  //! 将这个四元数表示的旋转转换为旋转轴和旋转角度.
	  //! \param angle 旋转角度，输出参数
	  //! \param axis 旋转轴，输出参数
	  void toAxisAngle (float &angle, ATOM_Vector3f &axis) const;

	  //! 通过旋转轴和旋转角度设置此四元数.
	  //! \param angle 旋转角度，输入参数
	  //! \param axis 旋转轴，输入参数
	  void fromAxisAngle (float angle, const ATOM_Vector3f &axis);

	  //! 通过3乘3旋转矩阵设置
	  //! \param matrix 旋转矩阵
	  void fromMatrix (const ATOM_Matrix3x3f &matrix);

	  //! 转换为3乘3旋转矩阵.
	  //! \param matrix 旋转矩阵，输出参数
	  void toMatrix (ATOM_Matrix3x3f &matrix) const;

	  //! 通过4乘4旋转矩阵设置
	  //! \param matrix 旋转矩阵
	  void fromMatrix (const ATOM_Matrix4x4f &matrix);

	  //! 转换为4乘4旋转矩阵.
	  //! \param matrix 旋转矩阵，输出参数
	  void toMatrix (ATOM_Matrix4x4f &matrix) const;

	  //! 转换为欧拉角
	  //! \return 欧拉角(x, y, z)
	  ATOM_Vector3f toEulerXYZ (void) const;

	  //! 通过欧拉角设置
	  //! \param eulerAngle 欧拉角
	  void fromEulerXYZ (const ATOM_Vector3f &eulerXYZ);

	  //! 设置四元数，使其表示为从一个矢量到另一个矢量的旋转.
	  //! \param from 旋转前的矢量
	  //! \param to 旋转后的矢量
	  //! \note 当from趋近于-to的时候该函数会变得数值不稳定，此情况内部并未检查处理
	  void setRotationArc (const ATOM_Vector3f &from, const ATOM_Vector3f &to);

	  //! 用此四元数旋转一个矢量.
	  //! \param from 旋转前的矢量，输入参数
	  //! \param to 旋转后的矢量，输出参数
	  void rotateVector (const ATOM_Vector3f &from, ATOM_Vector3f &to) const;

	  //! 球面插值.
	  //! \param q1 第一个四元数
	  //! \param q2 第二个四元数
	  //! \param t 插值因子
	  //! \return 插值后的四元数
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
