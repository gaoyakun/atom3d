/**	\file file.h
 *	虚拟文件类
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_FILE_H
#define __ATOM_KERNEL_FILE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_vfs.h"

#include "basedefs.h"

//! \class ATOM_File
//! 虚拟文件基类
//! \author 高雅昆
//! \ingroup kernel
class ATOM_KERNEL_API ATOM_File
{
public:
	//! 构造函数
	//! \param handle 虚拟文件句柄
	ATOM_File(ATOM_VFS::handle handle);

	//! 析构函数
	virtual ~ATOM_File (void);

public:
	//! 获取虚拟文件句柄
	//! \return 虚拟文件句柄
	virtual ATOM_VFS::handle getHandle (void) const;

	//! 获取文件名
	//! \return 文件名
	virtual const char* getName (void) const;

	//! 读取文件
	//! \param buffer 读取缓冲区
	//! \param size 要读取的字节数
	//! \return 实际读取的字节数
	//! \note 对于文本文件，因为需要转换换行符，即使读取成功，返回的实际大小也可能小于想要读取的大小
	virtual unsigned long read (void* buffer, unsigned long size);

	//! 写入文件
	//! \param buffer 写入缓冲区
	//! \param size 想要写入的字节数
	//! \return 实际写入的字节数
	virtual unsigned long write (const void* buffer, unsigned long size);

	//! 获取当前文件位置指针
	//! \return 文件位置，相对于文件起始
	virtual unsigned long getPosition (void) const;

	//! 移动当前文件位置指针
	//! \param offset 偏移量
	//! \param origin 相对偏移位置，可以是ATOM_VFS::begin, ATOM_VFS::current或者ATOM_VFS::end
	//! \return true 成功 false 失败
	virtual bool seek(long offset, int origin);

	//! 获取文件大小
	//! \return 文件大小的字节数
	virtual unsigned long size (void) const;

	//! 查询文件位置指针是否已经达到文件末尾
	//! \return true 达到末尾 false 未达到末尾
	virtual bool eof (void) const;

	//! 格式化文件写入
	//! \param format 文件格式串
	//! \return 写入的字节数
	virtual unsigned long printf(const char *format, ...);

private:
	ATOM_VFS::handle _M_handle;
};

#endif // __ATOM_KERNEL_FILE_H
/*! @} */
