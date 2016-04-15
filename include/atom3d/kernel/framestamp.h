/**	\file framestamp.h
 *	帧时间戳类
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_FRAMESTAMP_H
#define __ATOM_KERNEL_FRAMESTAMP_H

#if _MSC_VER > 1000
# pragma once
#endif

//! \class ATOM_FrameStamp
//! 帧时间戳类
//! \author 高雅昆
class ATOM_KERNEL_API ATOM_FrameStamp
{
public:
	//! 构造函数
	ATOM_FrameStamp (void);

public:
	//! 复位，所有成员归零
	void reset (float scale = 1.f);

	//! 更新(内部使用)
	void update (void);
	
public:
	//! 当前帧号，每帧递增
	unsigned frameStamp;

	//! 当前时间，以毫秒为单位
	unsigned currentTick;

	//! 与上一帧的间隔时间，以毫秒为单位
	unsigned elapsedTick;

	//! 帧速率，以秒为单位
	unsigned FPS;

	//! 时间流逝速度比例因子
	float timeScale;

private:
	unsigned renderTick;
	unsigned oldTick;

	unsigned lastFPSTimeMark;
	unsigned lastFPSCounter;
};

#endif // __ATOM_KERNEL_FRAMESTAMP_H
/*! @} */
