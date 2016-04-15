/**	\file soundinfo.h
 *	系统声音设备信息类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_SOUNDINFO_H_
#define __ATOM_KERNEL_SOUNDINFO_H_

#include "basedefs.h"

//! \class ATOM_SoundInfo
//! 获取当前计算机系统中声音设备信息的类
//! \author 高雅昆
class ATOM_KERNEL_API ATOM_SoundInfo
{
public:
	//! 构造函数
	ATOM_SoundInfo (void);

	//! 析构函数
	~ATOM_SoundInfo (void);

	//! 获取系统中声音设备的数量
	//! \return 系统中声音设备的数量
	unsigned getNumDevices (void) const;

	//! 获取声音设备的描述
	//! \param device 设备索引
	//! \return 声音设备的描述
	const char *getDeviceDescription (unsigned device) const;

	//! 获取声音设备的驱动名称
	//! \param device 设备索引
	//! \return 声音设备的驱动名称
	const char *getDeviceDriverName (unsigned device) const;

private:       // attributes
	ATOM_VECTOR<struct SoundDeviceInfo*> devices;
};

#endif // __ATOM_KERNEL_SOUNDINFO_H_
/*! @} */
