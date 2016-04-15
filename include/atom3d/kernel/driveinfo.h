/**	\file driveinfo.h
 *	ATOM_DriveInfo类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_DRIVEINFO_H_
#define __ATOM_KERNEL_DRIVEINFO_H_

#include "../ATOM_dbghlp.h"
#include "basedefs.h"
#include "drivestate.h"

//! \class ATOM_DriveInfo
//! 获取当前计算机系统中驱动器信息的类
//! \author 高雅昆
class ATOM_KERNEL_API ATOM_DriveInfo
{
public:
	//! 构造函数
	ATOM_DriveInfo(void);

	//! 查询系统中驱动器的个数
	//! \return 驱动器个数
	unsigned getNumDrives (void) const;

	//! 获取某个驱动器的当前状态
	//! \param i 驱动器索引
	//! \return 驱动器状态
	//! \sa ATOM_DriveStats
	const ATOM_DriveStats &getDriveState (unsigned i) const;

private:
	ATOM_VECTOR<ATOM_DriveStats> m_vdriveStats;
};

#endif // __ATOM_KERNEL_DRIVEINFO_H_
/*! @} */
