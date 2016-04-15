/**	\file dynlib.h
 *	ATOM_DynamicLib类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_DYNLIB_H
#define __ATOM_KERNEL_DYNLIB_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <string>
#ifdef WIN32
# include <windows.h>
# define DLhandle         HMODULE
# define dlsym(h, sym)    ::GetProcAddress(h, sym)
# define dlopen(fn, mode) ::LoadLibrary(fn)
# define dlclose(fn)      ::FreeLibrary(fn)
#elif defined(POSIX)
# include <dlfcn.h>
# define DLhandle         void*
#else
# error Unsupport platform
#endif

#include "../ATOM_dbghlp.h"

//! \class ATOM_DynamicLib
//! 动态链接库包装类
//! \author 高雅昆
class ATOM_DynamicLib
{
public:
	//! 构造函数
#ifdef WIN32
	ATOM_DynamicLib(const char* ext = 0);
#elif defined(POSIX)
	ATOM_DynamicLib(void);
#else
# error Unsupport platform
#endif // WIN32

	//! 析构函数
	virtual ~ATOM_DynamicLib();

	//! 获取文件名
	const char* getFilename() const;

public:
	//! 载入动态链接库
	//! \param filename 文件名
	virtual bool open(const char* filename);

	//! 卸载动态链接库
	virtual void close (void);

	//! 查询动态库是否已经加载
	virtual bool isOpened (void) const;

	//! 在动态链接库内查找符号
	//! \param sym 符号名称
	//! \return 查找到的地址，未查到返回NULL
	virtual void* querySymbol(const char* sym);

protected:
	DLhandle _M_lib_instance;
	ATOM_STRING _M_lib_filename;
	ATOM_STRING _M_def_extension;
};

#endif // __ATOM_KERNEL_DYNLIB_H
/*! @} */
