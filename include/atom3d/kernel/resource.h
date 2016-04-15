/**	\file kernel/resource.h
 *	资源类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_RESOURCE_H
#define __ATOM_KERNEL_RESOURCE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <string>
#include "functionbinding.h"

class ATOM_File;

//! \class ATOM_Resource
//! 资源类基类
//! 所有从此类派生的对象均支持从某个文件载入功能
//! \todo 移除此接口，使用更科学的方法管理资源
//! \author 高雅昆
class ATOM_KERNEL_API ATOM_Resource : public ATOM_Object
{
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_Resource)

protected:
	ATOM_Resource (void);
	virtual ~ATOM_Resource (void);

protected:
	//! 从文件中读取资源
	//! \param file 虚拟文件
	//! \param userdata 自定义数据，这个自定义数据在ATOM_LoadResource函数中传入
	//! \return true 成功 false 不成功
	virtual bool readFromFile(ATOM_File* file, void *userdata = 0) = 0;

public:
	//! 从文件中读取资源
	//! \param file 虚拟文件
	//! \param userdata 自定义数据，这个自定义数据在ATOM_LoadResource函数中传入
	//! \return true 成功 false 不成功
	//! \sa ATOM_LoadResource
	bool loadFromFile (ATOM_File *file, void *userdata = 0);
};

#endif // __ATOM_KERNEL_RESOURCE_H
/*! @} */
