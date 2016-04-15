/**	\file variant.h
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_VARIANT_H
#define __ATOM_KERNEL_VARIANT_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <algorithm>
#include "basedefs.h"

//! \class ATOM_Variant
//! 数据封装类.
//! \author 高雅昆
class ATOM_KERNEL_API ATOM_Variant
{
	friend ATOM_KERNEL_API bool ATOM_CALL operator == (const ATOM_Variant &left, const ATOM_Variant &right);
	friend ATOM_KERNEL_API bool ATOM_CALL operator != (const ATOM_Variant &left, const ATOM_Variant &right);

public:
	//! 数据类型
	enum Type
	{
		NONE = 0,
		INT = 1,
		INT_ARRAY = ~INT,
		FLOAT = 2,
		FLOAT_ARRAY = ~FLOAT,
		STRING = 3,
		POINTER = 4,
		VECTOR4 = 5,
		MATRIX44 = 6
	};

	struct VariantInternal
	{
		ATOM_Variant::Type type;
		unsigned count;
		unsigned memsize;

		union
		{
			int *i_array;
			float *f_array;
			int i;
			float f;
			char *s;
			void *p;
			void *dummy;
			float v[4];
			float m[16];
		};

		VariantInternal (void): type(ATOM_Variant::NONE), count(0), memsize(0), i_array(0) {}
	};

public:
	//! 构造函数
	ATOM_Variant (void);

	//! 从整数构造
	//! \param i 初始值
	ATOM_Variant (int i);

	//! 从整数数组构造
	//! \param i 数组地址
	//! \param count 数组长度
	ATOM_Variant (const int *i, unsigned count);

	//! 从浮点数构造
	//! \param f 初始值
	ATOM_Variant (float f);

	//! 从浮点数组构造
	//! \param f 数组地址
	//! \param count 数组长度
	ATOM_Variant (const float *f, unsigned count);

	//! 从字符串构造
	//! \param s 字符串
	ATOM_Variant (const char *s);

	//! 从指针构造
	//! \param p 初始值
	ATOM_Variant (void *p);

	//! 从四维向量构造
	//! \param v 初始值
	ATOM_Variant (const ATOM_Vector4f &v);

	//! 从4乘4矩阵构造
	//! \param m 初始值
	ATOM_Variant (const ATOM_Matrix4x4f &m);

	//! 拷贝构造函数
	//! \param rhs 拷贝对象
	ATOM_Variant (const ATOM_Variant &rhs);

	//! 析构函数
	~ATOM_Variant (void);

public:
	//! 赋值操作符重载
	//! \param rhs 赋值对象
	ATOM_Variant &operator = (const ATOM_Variant &rhs);

	//! 重置为无类型状态
	void invalidate (void);

	//! 测试是否数组类型
	//! \return true 是数组类型 false 不是数组类型
	bool isArray (void) const;

	//! 测试是否是无类型状态
	//! \return true 是 false 不是
	bool isEmpty (void) const;

	//! 获取数组长度
	//! \return 数组长度， 如果不是数组则返回1
	unsigned getArraySize (void) const;

	//! 交换
	//! \param rhs 交换对象
	void swap (ATOM_Variant &rhs);

	//! 获取整数值
	//! \return 整数值
	int getI (void) const;

	//! 获取整数数组的元素值
	//! \param index 元素索引
	//! \return 整数值
	int getI (unsigned index) const;

	//! 获取整数数组
	//! \return 数组地址 
	const int *getIntArray (void) const;

	//! 设置整数值
	//! \param i 整数值
	void setI (int i);

	//! 设置整数数组
	//! \param i 数组地址
	//! \param count 数组长度
	void setIntArray (const int *i, unsigned count);

	//! 获取浮点值
	//! \return 浮点值
	float getF (void) const;

	//! 获取浮点数组的元素值
	//! \param index 元素索引
	//! \return 浮点值
	float getF (unsigned index) const;

	//! 获取浮点数组
	//! \return 数组地址 
	const float *getFloatArray (void) const;

	//! 设置浮点值
	//! \param f 浮点值
	void setF (float f);

	//! 设置浮点数组
	//! \param f 数组地址
	//! \param count 数组长度
	void setFloatArray (const float *f, unsigned count);

	//! 获取字符串值
	//! \return 字符串值
	const char *getS (void) const;

	//! 设置字符串值
	//! \param s 字符串值
	void setS (const char *s);

	//! 获取指针值
	//! \return 指针值
	void *getP (void) const;

	//! 设置指针值
	//! \param p 指针值
	void setP (void *p);

	//! 设置四维向量值
	//! \param v 四维向量值
	void setV (const ATOM_Vector4f &v);

	//! 获取四维向量值
	//! \return 四维向量值
	const float *getV (void) const;

	//! 设置4乘4矩阵值
	//! \param m 4乘4矩阵值
	void setM (const ATOM_Matrix4x4f &m);

	//! 获取4乘4矩阵值
	//! \return 指向矩阵值
	const float *getM (void) const;

	//! 获取内部数据类型
	//! \return 内部数据类型
	ATOM_Variant::Type getType (void) const;

private:
	VariantInternal _data;
};

//! 测试两个ATOM_Variant是否相等
ATOM_KERNEL_API bool ATOM_CALL operator == (const ATOM_Variant &left, const ATOM_Variant &right);

//! 测试两个ATOM_Variant是否相等
ATOM_KERNEL_API bool ATOM_CALL operator != (const ATOM_Variant &left, const ATOM_Variant &right);

namespace std
{
  template <>
  inline void swap (ATOM_Variant &left, ATOM_Variant &right)
  {
    left.swap (right);
  }
}

#endif // __CRAZY3D_UTILS_VARIANT_H__
/*! @} */
