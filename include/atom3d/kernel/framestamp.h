/**	\file framestamp.h
 *	֡ʱ�����
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_FRAMESTAMP_H
#define __ATOM_KERNEL_FRAMESTAMP_H

#if _MSC_VER > 1000
# pragma once
#endif

//! \class ATOM_FrameStamp
//! ֡ʱ�����
//! \author ������
class ATOM_KERNEL_API ATOM_FrameStamp
{
public:
	//! ���캯��
	ATOM_FrameStamp (void);

public:
	//! ��λ�����г�Ա����
	void reset (float scale = 1.f);

	//! ����(�ڲ�ʹ��)
	void update (void);
	
public:
	//! ��ǰ֡�ţ�ÿ֡����
	unsigned frameStamp;

	//! ��ǰʱ�䣬�Ժ���Ϊ��λ
	unsigned currentTick;

	//! ����һ֡�ļ��ʱ�䣬�Ժ���Ϊ��λ
	unsigned elapsedTick;

	//! ֡���ʣ�����Ϊ��λ
	unsigned FPS;

	//! ʱ�������ٶȱ�������
	float timeScale;

private:
	unsigned renderTick;
	unsigned oldTick;

	unsigned lastFPSTimeMark;
	unsigned lastFPSCounter;
};

#endif // __ATOM_KERNEL_FRAMESTAMP_H
/*! @} */
