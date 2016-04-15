/**	\file drivestate.h
 *	ATOM_DriveStats类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_DRIVESTATE_H_
#define __ATOM_KERNEL_DRIVESTATE_H_

#include "basedefs.h"

//! \class ATOM_DriveStats
//! 驱动器状态类
//! \author 高雅昆
class ATOM_KERNEL_API ATOM_DriveStats
{
	friend class ATOM_DriveInfo;

public:      
	//! 获取驱动器名称
	//! \return 驱动器名称
	const char *getName(void) const;

	//! 获取驱动器类型
	//! \return 驱动器类型信息
	const char *getType(void) const;

	//! 获取驱动器总容量
	//! \return 驱动器容量信息
	const char *getTotalSpaceStr(void) const;

	//! 获取驱动器空闲容量
	//! \return 驱动器空闲容量信息
	const char *getFreeSpaceStr(void) const;

private:       // attributes
	ATOM_STRING  m_strName;          // drive name
	ATOM_STRING  m_strType;          // drive type
	ATOM_STRING  m_strTotalSpace;    // total drive space
	ATOM_STRING  m_strFreeSpace;     // total free space
};

#endif // __ATOM_KERNEL_DRIVESTATE_H_
/*! @} */
