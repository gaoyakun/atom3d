/**	\file refobj.h
 *	引用计数类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_REFOBJ_H
#define __ATOM_KERNEL_REFOBJ_H

#if _MSC_VER > 1000
# pragma once
#endif

//! \class ATOM_Noncopyable
//! 不可复制对象基类.
//! 所有从此类派生的对象均不可复制，也就是说无拷贝构造和赋值操作
//! \author 高雅昆
class ATOM_KERNEL_API ATOM_Noncopyable
{
public:
	//! 构造函数
	ATOM_Noncopyable (void) {}

private:
	ATOM_Noncopyable (const ATOM_Noncopyable&);
	ATOM_Noncopyable & operator = (const ATOM_Noncopyable&);
};

//! \class ATOM_ReferenceObj
//! 引用计数对象基类.
//! 所有从此类派生的对象均可进行引用计数管理
//! \author 高雅昆
class ATOM_KERNEL_API ATOM_ReferenceObj: public ATOM_Noncopyable
{
public:
	//! 构造函数
	ATOM_ReferenceObj (void): _M_reference(0) {}

	//! 析构函数
	virtual ~ATOM_ReferenceObj (void) {}

	//! 增加饮用计数
	//! \return 返回增加后的引用计数值
	virtual int addRef(void) { return ++_M_reference; }

	//! 减少引用计数
	//! \return 返回减少后的引用计数值
	//! \note 引用计数值减少到0后此函数不会删除对象
	virtual int decRef(void) { return --_M_reference; }

	//! 获取引用计数
	//! \return 引用计数值
	virtual int getRef(void) const{ return _M_reference; }

private:
    int _M_reference;
};

#endif // __ATOM_KERNEL_REFOBJ_H
/*! @} */
