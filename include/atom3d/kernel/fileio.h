/**	\file fileio.h
 *	系统文件操作辅助函数
 *
 *	\author 高雅昆
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

//! 查找第一个文件
//! \param dir 查找目录
//! \param handle 查找句柄
//! \param data 包含查找到的文件信息
//! \return 查到的文件名,没查到返回NULL
ATOM_KERNEL_API const char*  ATOM_SysFindFirst(const char* dir, ATOM_FIND_HANDLE* handle, ATOM_DIRECTORY_TYPE* data);

//! 查找下一个文件
//! \param handle 查找句柄
//! \param data 包含查找到的文件信息
//! \return 查到的文件名，没查到文件返回NULL
ATOM_KERNEL_API const char*  ATOM_SysFindNext(ATOM_FIND_HANDLE handle, ATOM_DIRECTORY_TYPE* data);

//! 结束查找
//! \param handle 查找句柄
ATOM_KERNEL_API void ATOM_SysCloseFind(ATOM_FIND_HANDLE handle);

//! 查询某个文件或目录是否存在
//! \param path 文件名
//! \return true 存在 false 不存在
ATOM_KERNEL_API bool ATOM_SysFileExists(const char* path);

//! 查询某个目录是否存在
//! \param path 目录名
//! \return true 存在 false 不存在
ATOM_KERNEL_API bool ATOM_SysIsDirectory(const char* path);

//! 查询是否空文件或空目录
//! \param path 要查询的文件名
//! \return true 是空文件或空目录 false 不是
ATOM_KERNEL_API bool ATOM_SysIsEmpty(const char* path);

//! 创建一个目录
//! \param path 要创建的目录名
//! \return true 创建成功 false 创建失败
//! \note 该目录已经存在或父目录不存在的情况下会创建失败
ATOM_KERNEL_API bool ATOM_SysCreateDirectory(const char* path);

//! 删除一个文件或者目录
//! \param path 要删除的文件名
//! \return true 删除成功 false 删除失败
//! \note 非空目录删除会失败
ATOM_KERNEL_API bool ATOM_SysRemove(const char* path);

//! 删除一个目录树以及其中的所有文件
//! \param path 要删除的目录树
//! \return 删除掉的文件数
ATOM_KERNEL_API long  ATOM_SysRemoveAll(const char* path);

//! 重命名一个文件或者目录
//! \param old_path 要重命名的文件名
//! \param new_path 新的文件名
//! \return true 成功 false 失败
ATOM_KERNEL_API bool ATOM_SysRename(const char* old_path, const char* new_path);

//! 复制文件
//! \param from 源文件
//! \param to 目标文件
//! \return true 成功 false 失败
//! \note 如果目标文件存在则会覆盖之
ATOM_KERNEL_API bool ATOM_SysCopyFile(const char* from, const char* to);

//! 目录树深拷贝
//! \param from 源目录
//! \param to 目标目录
//! \param recurse true 拷贝子目录 false 不拷贝子目录
//! \return true 成功 false 失败
ATOM_KERNEL_API bool ATOM_SysDeepCopy(const char* from, const char* to, bool recurse);

//! 获取系统临时目录的文件名
//! \param buffer 接受文件名的缓冲区
//! \param size 缓冲区大小
//! \return 写入缓冲区的字符数目 
ATOM_KERNEL_API unsigned ATOM_SysGetTempDir(char* buffer, unsigned size);

//! 获取系统当前目录
//! \param buffer 接受文件名的缓冲区
//! \param size 缓冲区大小
//! \return 写入缓冲区的字符数目 
ATOM_KERNEL_API unsigned     ATOM_SysGetCWD(char* buffer, unsigned size);

//! 设置系统当前目录
//! \param path 要设置的当前目录
//! \return true 成功 false 失败
ATOM_KERNEL_API bool ATOM_SysSetCWD(const char* path);

//! 系统当前目录入栈
ATOM_KERNEL_API void ATOM_SysPushCWD (void);

//! 弹出系统当前目录
ATOM_KERNEL_API void ATOM_SysPopCWD (void);

//! 合并文件名
//! \param path 文件名前部分
//! \param relative 文件名后部分
//! \param buffer 接受文件名的缓冲区
//! \param size 缓冲区大小
//! \return 写入缓冲区的字符数目 
ATOM_KERNEL_API unsigned ATOM_SysMergePath(const char* path, const char* relative, char* buffer, unsigned size);

} // extern"C"

#endif // __ATOM_KERNEL_FILEIO_H
/*! @} */
