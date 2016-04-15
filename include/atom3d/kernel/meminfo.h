/**	\file meminfo.h
 *	内存信息类MemoryInfo的声明.
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_MEMINFO_H_
#define __ATOM_KERNEL_MEMINFO_H_

#include "basedefs.h"

//! \class ATOM_MemoryInfo
//! 获取当前计算机系统中CPU信息的类
//! \author 高雅昆
class ATOM_KERNEL_API ATOM_MemoryInfo
{
public:
	//! 构造函数
	ATOM_MemoryInfo (void);

	//! 得到内存总量
	//! \return 内存总量，单位为字节
	unsigned getTotalRam (void) const;

	//! 得到已提交的虚拟内存页面文件大小
	//! \return 已提交的虚拟内存页面文件大小，单位为字节
	unsigned getTotalPageFile (void) const;

	//! 得到虚拟内存地址空间大小
	//! \return 虚拟内存地址空间大小，单位为字节
	unsigned getTotalVirtual (void) const;

	//! 得到内存总量的字符串表示
	//! \return 内存总量的字符串表示
	const char *getTotalRamStr (void) const;

	//! 得到已提交的虚拟内存页面文件大小的字符串表示
	//! \return 已提交的虚拟内存页面文件大小的字符串表示
	const char *getTotalPageFileStr (void) const;

	//! 得到虚拟内存地址空间大小的字符串表示
	//! \return 得到虚拟内存地址空间大小的字符串表示
	const char *getTotalVirtualStr (void) const;

private:
	unsigned totalRam;
	unsigned totalPageFile;
	unsigned totalVirtual;
	char totalRamString[64];
	char totalPageFileString[64];
	char totalVirtualString[64];
};

#endif // __ATOM_KERNEL_MEMINFO_H_
/*! @} */
