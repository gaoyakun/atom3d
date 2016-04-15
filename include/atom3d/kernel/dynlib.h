/**	\file dynlib.h
 *	ATOM_DynamicLib�������.
 *
 *	\author ������
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
//! ��̬���ӿ��װ��
//! \author ������
class ATOM_DynamicLib
{
public:
	//! ���캯��
#ifdef WIN32
	ATOM_DynamicLib(const char* ext = 0);
#elif defined(POSIX)
	ATOM_DynamicLib(void);
#else
# error Unsupport platform
#endif // WIN32

	//! ��������
	virtual ~ATOM_DynamicLib();

	//! ��ȡ�ļ���
	const char* getFilename() const;

public:
	//! ���붯̬���ӿ�
	//! \param filename �ļ���
	virtual bool open(const char* filename);

	//! ж�ض�̬���ӿ�
	virtual void close (void);

	//! ��ѯ��̬���Ƿ��Ѿ�����
	virtual bool isOpened (void) const;

	//! �ڶ�̬���ӿ��ڲ��ҷ���
	//! \param sym ��������
	//! \return ���ҵ��ĵ�ַ��δ�鵽����NULL
	virtual void* querySymbol(const char* sym);

protected:
	DLhandle _M_lib_instance;
	ATOM_STRING _M_lib_filename;
	ATOM_STRING _M_def_extension;
};

#endif // __ATOM_KERNEL_DYNLIB_H
/*! @} */
