/**	\file fileio.h
 *	ϵͳ�ļ�������������
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_FILEIO_H
#define __ATOM_KERNEL_FILEIO_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"
#ifdef WIN32
# include <windows.h>
#elif defined(POSIX)
# include <sys/stat.h>
# include <dirent.h>
# include <unistd.h>
# include <fcntl.h>
#else
# error Only WIN32 and Linux platform currently supported
#endif

#ifdef WIN32
# define ATOM_FIND_HANDLE               HANDLE
# define ATOM_INVALID_FIND_HANDLE_VALUE ((HANDLE)INVALID_HANDLE_VALUE)
# define ATOM_DIRECTORY_TYPE            WIN32_FIND_DATAA
#else
# define ATOM_FIND_HANDLE               DIR*
# define ATOM_INVALID_FIND_HANDLE_VALUE 0
# define ATOM_DIRECTORY_TYPE            struct dirent*
#endif

extern"C" {

//! ���ҵ�һ���ļ�
//! \param dir ����Ŀ¼
//! \param handle ���Ҿ��
//! \param data �������ҵ����ļ���Ϣ
//! \return �鵽���ļ���,û�鵽����NULL
ATOM_KERNEL_API const char*  ATOM_SysFindFirst(const char* dir, ATOM_FIND_HANDLE* handle, ATOM_DIRECTORY_TYPE* data);

//! ������һ���ļ�
//! \param handle ���Ҿ��
//! \param data �������ҵ����ļ���Ϣ
//! \return �鵽���ļ�����û�鵽�ļ�����NULL
ATOM_KERNEL_API const char*  ATOM_SysFindNext(ATOM_FIND_HANDLE handle, ATOM_DIRECTORY_TYPE* data);

//! ��������
//! \param handle ���Ҿ��
ATOM_KERNEL_API void ATOM_SysCloseFind(ATOM_FIND_HANDLE handle);

//! ��ѯĳ���ļ���Ŀ¼�Ƿ����
//! \param path �ļ���
//! \return true ���� false ������
ATOM_KERNEL_API bool ATOM_SysFileExists(const char* path);

//! ��ѯĳ��Ŀ¼�Ƿ����
//! \param path Ŀ¼��
//! \return true ���� false ������
ATOM_KERNEL_API bool ATOM_SysIsDirectory(const char* path);

//! ��ѯ�Ƿ���ļ����Ŀ¼
//! \param path Ҫ��ѯ���ļ���
//! \return true �ǿ��ļ����Ŀ¼ false ����
ATOM_KERNEL_API bool ATOM_SysIsEmpty(const char* path);

//! ����һ��Ŀ¼
//! \param path Ҫ������Ŀ¼��
//! \return true �����ɹ� false ����ʧ��
//! \note ��Ŀ¼�Ѿ����ڻ�Ŀ¼�����ڵ�����»ᴴ��ʧ��
ATOM_KERNEL_API bool ATOM_SysCreateDirectory(const char* path);

//! ɾ��һ���ļ�����Ŀ¼
//! \param path Ҫɾ�����ļ���
//! \return true ɾ���ɹ� false ɾ��ʧ��
//! \note �ǿ�Ŀ¼ɾ����ʧ��
ATOM_KERNEL_API bool ATOM_SysRemove(const char* path);

//! ɾ��һ��Ŀ¼���Լ����е������ļ�
//! \param path Ҫɾ����Ŀ¼��
//! \return ɾ�������ļ���
ATOM_KERNEL_API long  ATOM_SysRemoveAll(const char* path);

//! ������һ���ļ�����Ŀ¼
//! \param old_path Ҫ���������ļ���
//! \param new_path �µ��ļ���
//! \return true �ɹ� false ʧ��
ATOM_KERNEL_API bool ATOM_SysRename(const char* old_path, const char* new_path);

//! �����ļ�
//! \param from Դ�ļ�
//! \param to Ŀ���ļ�
//! \return true �ɹ� false ʧ��
//! \note ���Ŀ���ļ�������Ḳ��֮
ATOM_KERNEL_API bool ATOM_SysCopyFile(const char* from, const char* to);

//! Ŀ¼�����
//! \param from ԴĿ¼
//! \param to Ŀ��Ŀ¼
//! \param recurse true ������Ŀ¼ false ��������Ŀ¼
//! \return true �ɹ� false ʧ��
ATOM_KERNEL_API bool ATOM_SysDeepCopy(const char* from, const char* to, bool recurse);

//! ��ȡϵͳ��ʱĿ¼���ļ���
//! \param buffer �����ļ����Ļ�����
//! \param size ��������С
//! \return д�뻺�������ַ���Ŀ 
ATOM_KERNEL_API unsigned ATOM_SysGetTempDir(char* buffer, unsigned size);

//! ��ȡϵͳ��ǰĿ¼
//! \param buffer �����ļ����Ļ�����
//! \param size ��������С
//! \return д�뻺�������ַ���Ŀ 
ATOM_KERNEL_API unsigned     ATOM_SysGetCWD(char* buffer, unsigned size);

//! ����ϵͳ��ǰĿ¼
//! \param path Ҫ���õĵ�ǰĿ¼
//! \return true �ɹ� false ʧ��
ATOM_KERNEL_API bool ATOM_SysSetCWD(const char* path);

//! ϵͳ��ǰĿ¼��ջ
ATOM_KERNEL_API void ATOM_SysPushCWD (void);

//! ����ϵͳ��ǰĿ¼
ATOM_KERNEL_API void ATOM_SysPopCWD (void);

//! �ϲ��ļ���
//! \param path �ļ���ǰ����
//! \param relative �ļ����󲿷�
//! \param buffer �����ļ����Ļ�����
//! \param size ��������С
//! \return д�뻺�������ַ���Ŀ 
ATOM_KERNEL_API unsigned ATOM_SysMergePath(const char* path, const char* relative, char* buffer, unsigned size);

} // extern"C"

#endif // __ATOM_KERNEL_FILEIO_H
/*! @} */
