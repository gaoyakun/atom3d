/**	\file soundinfo.h
 *	ϵͳ�����豸��Ϣ�������.
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_SOUNDINFO_H_
#define __ATOM_KERNEL_SOUNDINFO_H_

#include "basedefs.h"

//! \class ATOM_SoundInfo
//! ��ȡ��ǰ�����ϵͳ�������豸��Ϣ����
//! \author ������
class ATOM_KERNEL_API ATOM_SoundInfo
{
public:
	//! ���캯��
	ATOM_SoundInfo (void);

	//! ��������
	~ATOM_SoundInfo (void);

	//! ��ȡϵͳ�������豸������
	//! \return ϵͳ�������豸������
	unsigned getNumDevices (void) const;

	//! ��ȡ�����豸������
	//! \param device �豸����
	//! \return �����豸������
	const char *getDeviceDescription (unsigned device) const;

	//! ��ȡ�����豸����������
	//! \param device �豸����
	//! \return �����豸����������
	const char *getDeviceDriverName (unsigned device) const;

private:       // attributes
	ATOM_VECTOR<struct SoundDeviceInfo*> devices;
};

#endif // __ATOM_KERNEL_SOUNDINFO_H_
/*! @} */
