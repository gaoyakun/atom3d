/**	\file rwops.h
 *	输入输出操作基类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_IRWOPS_H
#define __ATOM_KERNEL_IRWOPS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

class ATOM_File;

//! \class ATOM_RWops
//! 输入输出操作器基类
//! \todo 移除此接口，改为更一般化的流操作
//! \author 高雅昆
class ATOM_KERNEL_API ATOM_RWops
{
public:
	//! 输入输出类型
	enum RWOPS_TYPE
	{
		//! 物理文件IO
		TYPE_FILE,
		//! 内存IO
		TYPE_MEM,
		//! 虚拟文件IO
		TYPE_VFS,
		//! 内存映射文件IO
		TYPE_MMAP
	};

	//! 创建物理文件IO操作器
	//! \param fn 物理文件名 
	//! \param mode 文件打开模式
	//! \return 物理文件IO操作器
	static ATOM_RWops* createFileRWops(const char* fn, const char* mode);

	//! 创建内存IO操作器
	//! \param mem 内存缓冲区起始地址
	//! \param size 内存缓冲区大小
	//! \return 内存IO操作器
	static ATOM_RWops* createMemRWops(void* mem, unsigned size);

	//! 创建虚拟文件IO操作器
	//! \param file 虚拟文件
	//! \return 虚拟文件IO操作器
	static ATOM_RWops* createVFSRWops(ATOM_File* file);

	//! 删除IO操作器
	//! \param rwops IO操作器
	static void destroyRWops(ATOM_RWops* rwops);    

public:
	//! 析构函数
	virtual ~ATOM_RWops (void);

public:
	//! 获取操作器类型
	//! \return 操作器类型
	virtual RWOPS_TYPE getType (void) const = 0;

	//! 获取操作器当前IO位置
	//! \return 相对于起始的字节偏移量
	virtual int tell (void) const = 0;

	//! 移动IO当前位置
	//! \param offset 偏移量
	//! \param whence 相对偏移对象， 可以是SEEK_SET, SEEK_CUR或SEEK_END
	//! \return 移动以后的当前位置
	virtual int seek (int offset, int whence) = 0;

	//! 从目标读取
	//! \param buffer 读取缓冲区
	//! \param size 每个读取单元的字节大小
	//! \param count 读取单元数
	//! \return 成功读取的单元数
	virtual unsigned read(void* buffer, unsigned size, unsigned count) = 0;

	//! 写入到目标
	//! \param buffer 写入缓冲区
	//! \param size 每个写入单元的字节大小
	//! \param count 写入单元数
	//! \return 成功写入的单元数
	virtual unsigned write(const void* buffer, unsigned size, unsigned count) = 0;

	//! 目标的总大小
	//! \return 目标的总大小，单位为字节
	virtual unsigned long size() = 0;

	//! 从目标读取一个字符
	//! \return 读取的字符
	virtual char getC (void) = 0;

	//! 查询目标的IO指针是否已经位于目标的末尾
	//! \return true 到末尾 false 没到末尾
	virtual bool eof (void) const = 0;
};

template <class T>
inline T ATOM_RWopsRead (ATOM_RWops *rwops)
{
	T val;
	rwops->read (&val, sizeof(T), 1);
	return val;
}


#endif // __ATOM_KERNEL_IRWOPS_H
/*! @} */
