/**	\file rwops_file.h
 *	物理文件输入输出操作类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_IRWOPSFILE_H
#define __ATOM_KERNEL_IRWOPSFILE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <cstdio>
#include "rwops.h"

//! \class ATOM_RWopsFile
//! 物理文件IO操作器基类
//! \todo 移除此接口，改为更一般化的流操作
//! \author 高雅昆
class ATOM_RWopsFile : public ATOM_RWops
{
public:
	//! 构造函数
	//! \param filename 物理文件名
	//! \param mode 文件打开模式
	ATOM_RWopsFile(const char* filename, const char* mode);

	//! 析构函数
	virtual ~ATOM_RWopsFile (void);

public:
	//! \copydoc ATOM_RWops::getType
	virtual RWOPS_TYPE getType (void) const;

	//! \copydoc ATOM_RWops::tell
	virtual int tell (void) const;

	//! \copydoc ATOM_RWops::seek
	virtual int seek(int offset, int whence);

	//! \copydoc ATOM_RWops::read
	virtual unsigned read(void* buffer, unsigned size, unsigned count);

	//! \copydoc ATOM_RWops::write
	virtual unsigned write(const void* buffer, unsigned size, unsigned count);

	//! \copydoc ATOM_RWops::size
	virtual unsigned long size (void);

	//! \copydoc ATOM_RWops::getC
	virtual char getC (void);

	//! \copydoc ATOM_RWops::eof
	virtual bool eof (void) const;

	//! 获取打开的文件句柄
	//! \return 文件句柄，未打开文件返回NULL
	virtual FILE *getFilePointer (void) const;

private:
	FILE* _M_fp;
};

#endif // __ATOM_KERNEL_IRWOPSFILE_H
/*! @} */
