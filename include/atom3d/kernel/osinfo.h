/**	\file osinfo.h
 *	操作系统信息类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_OSVER_H
#define __ATOM_KERNEL_OSVER_H

#include "basedefs.h"

#if defined(WIN32)

//! \class ATOM_OSInfo
//! 获取当前计算机系统中操作系统信息的类
//! \note 目前仅支持windows系列
//! \todo 需要支持Windows7
//! \author 高雅昆
class ATOM_KERNEL_API ATOM_OSInfo
{
public:
	//! Windows操作系统类型枚举量
	enum WINDOWS_TYPE
	{
		OS_WINDOWS_NT,
		OS_WINDOWS_2000,
		OS_WINDOWS_XP,
		OS_WINDOWS_95,
		OS_WINDOWS_95_OSR2,
		OS_WINDOWS_98,
		OS_WINDOWS_ME,
		OS_WINDOWS_WIN32s,
		OS_UNKNOWN
	};

public:
	//! 构造函数
	ATOM_OSInfo (void);

	//! 获取当前操作系统类型
	//! \return 类型枚举
	WINDOWS_TYPE getOSType (void) const;

	//! 获取当前操作系统类型的字符串表示
	//! \return 获取当前操作系统类型的字符串表示
	const char *getOSName (void) const;

private:
	WINDOWS_TYPE type;
	char osname[256];
};

#else
# error Not implemented.
#endif // 

#endif // __ATOM_KERNEL_OSVER_H
/*! @} */
