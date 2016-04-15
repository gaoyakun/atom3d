/**	\file kernel/var.h
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_VAR_H
#define __ATOM_KERNEL_VAR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"
#include "../ATOM_dbghlp.h"
#include "kernel.h"

//! \class ATOM_ScriptVar
//! 脚本数据封装类.
//! 封装了插件对象的属性数据
//! \todo 和ATOM_Variant类统一
//! \author 高雅昆
class ATOM_KERNEL_API ATOM_ScriptVar
{
public:
	//! 构造函数
	ATOM_ScriptVar (void);

	//! 析构函数
	~ATOM_ScriptVar (void);

	//! 拷贝构造函数
	//! \param rhs 拷贝对象
	ATOM_ScriptVar (const ATOM_ScriptVar &rhs);

	//! 从整型值构造
	//! \param value 初始值
	ATOM_ScriptVar (int value);

	//! 从整型数组构造
	//! \param value 初始值
	ATOM_ScriptVar (const ATOM_VECTOR<int> &value);

	//! 从浮点值构造
	//! \param value 初始值
	ATOM_ScriptVar (float value);

	//! 从浮点数组构造
	//! \param value 初始值
	ATOM_ScriptVar (const ATOM_VECTOR<float> &value);

	//! 从字符串值构造
	//! \param value 初始值
	ATOM_ScriptVar (const ATOM_STRING &value);

	//! 从字符串数组构造
	//! \param value 初始值
	ATOM_ScriptVar (const ATOM_VECTOR<ATOM_STRING> &value);

	//! 从二维向量值构造
	//! \param value 初始值
	ATOM_ScriptVar (const ATOM_Vector2f &value);

	//! 从二维向量数组构造
	//! \param value 初始值
	ATOM_ScriptVar (const ATOM_VECTOR<ATOM_Vector2f> &value);

	//! 从三维向量值构造
	//! \param value 初始值
	ATOM_ScriptVar (const ATOM_Vector3f &value);

	//! 从三维向量数组构造
	//! \param value 初始值
	ATOM_ScriptVar (const ATOM_VECTOR<ATOM_Vector3f> &value);

	//! 从四维向量构造
	//! \param value 初始值
	ATOM_ScriptVar (const ATOM_Vector4f &value);

	//! 从四维向量数组构造
	//! \param value 初始值
	ATOM_ScriptVar (const ATOM_VECTOR<ATOM_Vector4f> &value);

	//! 从3乘3矩阵构造
	//! \param value 初始值
	ATOM_ScriptVar (const ATOM_Matrix3x3f &value);

	//! 从3乘3矩阵数组构造
	//! \param value 初始值
	ATOM_ScriptVar (const ATOM_VECTOR<ATOM_Matrix3x3f> &value);

	//! 从4乘4矩阵构造
	//! \param value 初始值
	ATOM_ScriptVar (const ATOM_Matrix4x4f &value);

	//! 从4乘4矩阵数组构造
	//! \param value 初始值
	ATOM_ScriptVar (const ATOM_VECTOR<ATOM_Matrix4x4f> &value);

	//! 从插件对象构造
	//! \param value 初始值
	ATOM_ScriptVar (ATOM_AUTOREF(ATOM_Object) value);

	//! 从插件对象数组构造
	//! \param value 初始值
	ATOM_ScriptVar (const ATOM_VECTOR< ATOM_AUTOREF(ATOM_Object) > &value);

public:
	//! 赋值操作符重载
	//! \param rhs 赋值对象
	ATOM_ScriptVar & operator = (const ATOM_ScriptVar &rhs);

	//! 获取整数右值
	//! \return 整数值
	const int & asInteger (void) const;

	//! 获取整数数组右值
	//! \return 数组值
	const ATOM_VECTOR<int> & asIntegerArray (void) const;

	//! 获取浮点右值
	//! \return 浮点值
	const float & asFloat(void) const;

	//! 获取浮点数组右值
	//! \return 数组值
	const ATOM_VECTOR<float> & asFloatArray (void) const;

	//! 获取字符串右值
	//! \return 字符串值
	const ATOM_STRING & asString (void) const;

	//! 获取字符串数组右值
	//! \return 数组值
	const ATOM_VECTOR<ATOM_STRING> & asStringArray (void) const;

	//! 获取二维向量右值
	//! \return 二维向量值
	const ATOM_Vector2f & asVector2 (void) const;

	//! 获取二维向量数组右值
	//! \return 数组值
	const ATOM_VECTOR<ATOM_Vector2f> & asVector2Array (void) const;

	//! 获取三维向量右值
	//! \return 三维向量值
	const ATOM_Vector3f & asVector3 (void) const;

	//! 获取三维向量数组右值
	//! \return 数组值
	const ATOM_VECTOR<ATOM_Vector3f> & asVector3Array (void) const;

	//! 获取四维向量右值
	//! \return 四维向量值
	const ATOM_Vector4f & asVector4 (void) const;

	//! 获取四维向量数组右值
	//! \return 数组值
	const ATOM_VECTOR<ATOM_Vector4f> & asVector4Array (void) const;

	//! 获取3乘3矩阵右值
	//! \return 3乘3矩阵值
	const ATOM_Matrix3x3f & asMatrix33 (void) const;

	//! 获取3乘3矩阵数组右值
	//! \return 数组值
	const ATOM_VECTOR<ATOM_Matrix3x3f> & asMatrix33Array(void) const;

	//! 获取4乘4矩阵右值
	//! \return 4乘4矩阵值
	const ATOM_Matrix4x4f & asMatrix44 (void) const;

	//! 获取4乘4矩阵数组右值
	//! \return 数组值
	const ATOM_VECTOR<ATOM_Matrix4x4f> & asMatrix44Array (void) const;

	//! 获取插件对象右值
	//! \return 插件对象值
	const ATOM_AUTOREF(ATOM_Object) & asObject (void) const;

	//! 获取插件对象数组右值
	//! \return 数组值
	const ATOM_VECTOR< ATOM_AUTOREF(ATOM_Object) > & asObjectArray (void) const;

	//! 获取整数左值
	//! \return 整数值
	int & asInteger (void);

	//! 获取整数数组左值
	//! \return 数组值
	ATOM_VECTOR<int> & asIntegerArray (void);

	//! 获取浮点左值
	//! \return 浮点值
	float & asFloat(void);

	//! 获取浮点数组左值
	//! \return 数组值
	ATOM_VECTOR<float> & asFloatArray (void);

	//! 获取字符串左值
	//! \return 字符串值
	ATOM_STRING& asString (void);

	//! 获取字符串数组左值
	//! \return 数组值
	ATOM_VECTOR<ATOM_STRING> & asStringArray (void);

	//! 获取二维向量左值
	//! \return 二维向量值
	ATOM_Vector2f & asVector2 (void);

	//! 获取二维向量数组左值
	//! \return 数组值
	ATOM_VECTOR<ATOM_Vector2f> & asVector2Array (void);

	//! 获取三维向量左值
	//! \return 三维向量值
	ATOM_Vector3f & asVector3 (void);

	//! 获取三维向量数组左值
	//! \return 数组值
	ATOM_VECTOR<ATOM_Vector3f> & asVector3Array (void);

	//! 获取四维向量左值
	//! \return 四维向量值
	ATOM_Vector4f & asVector4 (void);

	//! 获取四维向量数组左值
	//! \return 数组值
	ATOM_VECTOR<ATOM_Vector4f> & asVector4Array (void);

	//! 获取3乘3矩阵左值
	//! \return 3乘3矩阵值
	ATOM_Matrix3x3f & asMatrix33 (void);

	//! 获取3乘3矩阵数组左值
	//! \return 数组值
	ATOM_VECTOR<ATOM_Matrix3x3f> & asMatrix33Array(void);

	//! 获取4乘4矩阵左值
	//! \return 4乘4矩阵值
	ATOM_Matrix4x4f & asMatrix44 (void);

	//! 获取4乘4矩阵数组左值
	//! \return 数组值
	ATOM_VECTOR<ATOM_Matrix4x4f> & asMatrix44Array (void);

	//! 获取插件对象左值
	//! \return 插件对象值
	ATOM_AUTOREF(ATOM_Object) & asObject (void);

	//! 获取插件对象数组左值
	//! \return 数组值
	ATOM_VECTOR< ATOM_AUTOREF(ATOM_Object) > & asObjectArray (void);

public:
	//! 获取整数值.
	//! 如果存放数据类型不是整数或浮点数会抛出异常
	//! \return 整数值
	int getInteger (void) const;

	//! 获取浮点值.
	//! 如果存放数据类型不是整数或浮点数会抛出异常
	//! \return 浮点值
	float getFloat (void) const;

	//! 获取字符串值.
	//! 如果存放数据类型不是字符串会抛出异常
	//! \return 字符串值
	ATOM_STRING getString (void) const;

	//! 获取二维向量值.
	//! 如果存放数据类型不是二维向量会抛出异常
	//! \return 二维向量值
	ATOM_Vector2f getVector2 (void) const;

	//! 获取三维向量值.
	//! 如果存放数据类型不是三维向量会抛出异常
	//! \return 三维向量值
	ATOM_Vector3f getVector3 (void) const;

	//! 获取四维向量值.
	//! 如果存放数据类型不是四维向量会抛出异常
	//! \return 四维向量值
	ATOM_Vector4f getVector4 (void) const;

	//! 获取3乘3矩阵值.
	//! 如果存放数据类型不是3乘3矩阵会抛出异常
	//! \return 3乘3矩阵值
	ATOM_Matrix3x3f getMatrix33 (void) const;

	//! 获取4乘4矩阵值.
	//! 如果存放数据类型不是4乘4矩阵会抛出异常
	//! \return 4乘4矩阵值
	ATOM_Matrix4x4f getMatrix44 (void) const;

	//! 获取插件对象值.
	//! 如果存放数据类型不是插件对象会抛出异常
	//! \return 插件对象值
	ATOM_AUTOREF(ATOM_Object) getObject (void) const;

	//! 获取整数数组.
	//! 如果存放数据类型不是整数数组会抛出异常
	//! \return 整数数组
	ATOM_VECTOR<int> getIntegerArray (void) const;

	//! 获取浮点数组.
	//! 如果存放数据类型不是浮点数组会抛出异常
	//! \return 浮点数组
	ATOM_VECTOR<float> getFloatArray (void) const;

	//! 获取字符串数组.
	//! 如果存放数据类型不是字符串数组会抛出异常
	//! \return 字符串数组
	ATOM_VECTOR<ATOM_STRING> getStringArray (void) const;

	//! 获取二维向量数组.
	//! 如果存放数据类型不是二维向量数组会抛出异常
	//! \return 二维向量数组
	ATOM_VECTOR<ATOM_Vector2f> getVector2Array (void) const;

	//! 获取三维向量数组.
	//! 如果存放数据类型不是三维向量数组会抛出异常
	//! \return 三维向量数组
	ATOM_VECTOR<ATOM_Vector3f> getVector3Array (void) const;

	//! 获取四维向量数组.
	//! 如果存放数据类型不是四维向量数组会抛出异常
	//! \return 四维向量数组
	ATOM_VECTOR<ATOM_Vector4f> getVector4Array (void) const;

	//! 获取3乘3矩阵数组.
	//! 如果存放数据类型不是3乘3矩阵数组会抛出异常
	//! \return 3乘3矩阵数组
	ATOM_VECTOR<ATOM_Matrix3x3f> getMatrix33Array (void) const;

	//! 获取4乘4矩阵数组.
	//! 如果存放数据类型不是4乘4矩阵数组会抛出异常
	//! \return 4乘4矩阵数组
	ATOM_VECTOR<ATOM_Matrix4x4f> getMatrix44Array (void) const;

	//! 获取插件对象数组.
	//! 如果存放数据类型不是插件对象数组会抛出异常
	//! \return 插件对象数组
	ATOM_VECTOR< ATOM_AUTOREF(ATOM_Object) > getObjectArray (void) const;

public:
	//! 相等测试
	//! \param rhs 测试对象
	bool operator == (const ATOM_ScriptVar &rhs) const;

	//! 不等测试
	//! \param rhs 测试对象
	bool operator != (const ATOM_ScriptVar &rhs) const;

private:
	void deleteStorage (void);
	void createStorage (void);
	void reallocStorage (int type);

public:
	//! 获取内部数据类型
	//! \return 数据类型
	int getType (void) const;

	//! 设置内部数据类型
	//! \param type 数据类型
	void setType (int type);

	//! 交换内部数据
	//! \param rhs 交换对象
	void swap (ATOM_ScriptVar &rhs);

private:
	int _M_type;
	union 
	{
		ATOM_VECTOR<int> *_M_integer;
		ATOM_VECTOR<float> *_M_float;
		ATOM_VECTOR<ATOM_STRING> *_M_string;
		ATOM_VECTOR<ATOM_Vector2f> *_M_vec2;
		ATOM_VECTOR<ATOM_Vector3f> *_M_vec3;
		ATOM_VECTOR<ATOM_Vector4f> *_M_vec4;
		ATOM_VECTOR<ATOM_Matrix3x3f> *_M_mat33;
		ATOM_VECTOR<ATOM_Matrix4x4f> *_M_mat44;
		ATOM_VECTOR< ATOM_AUTOREF(ATOM_Object) > *_M_object;
	};
};

#endif // __ATOM_KERNEL_VAR_H
/*! @} */
