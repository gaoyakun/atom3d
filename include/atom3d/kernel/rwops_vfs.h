/**	\file rwops_file.h
 *	虚拟文件输入输出操作类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_IRWOPSVFS_H
#define __ATOM_KERNEL_IRWOPSVFS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "rwops.h"

//! \class ATOM_RWopsVFS
//! 虚拟文件IO操作器基类
//! \todo 移除此接口，改为更一般化的流操作
//! \author 高雅昆
class ATOM_RWopsVFS : public ATOM_RWops
{
public:
	//! 构造函数
	//! \param file 虚拟文件
	ATOM_RWopsVFS (ATOM_File* file);

	//! 析构函数
	virtual ~ATOM_RWopsVFS (void);

public:
	//! \copydoc ATOM_RWops::getType
	virtual RWOPS_TYPE getType (void) const;

	//! \copydoc ATOM_RWops::tell
	virtual int tell (void) const;

	//! \copydoc ATOM_RWops::seek
	virtual int seek (int offset, int whence);

	//! \copydoc ATOM_RWops::read
	virtual unsigned read (void* buffer, unsigned size, unsigned count);

	//! \copydoc ATOM_RWops::write
	virtual unsigned write (const void* buffer, unsigned size, unsigned count);

	//! \copydoc ATOM_RWops::size
	virtual unsigned long size (void);

	//! \copydoc ATOM_RWops::getC
	virtual char getC (void);

	//! \copydoc ATOM_RWops::eof
	virtual bool eof (void) const;

private:
	ATOM_File* _M_file;
};

#endif // __ATOM_KERNEL_IRWOPSVFS_H
/*! @} */
