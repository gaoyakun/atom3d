/**	\file osinfo.h
 *	����ϵͳ��Ϣ�������.
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_OSVER_H
#define __ATOM_KERNEL_OSVER_H

#include "basedefs.h"

#if defined(WIN32)

//! \class ATOM_OSInfo
//! ��ȡ��ǰ�����ϵͳ�в���ϵͳ��Ϣ����
//! \note Ŀǰ��֧��windowsϵ��
//! \todo ��Ҫ֧��Windows7
//! \author ������
class ATOM_KERNEL_API ATOM_OSInfo
{
public:
	//! Windows����ϵͳ����ö����
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
	//! ���캯��
	ATOM_OSInfo (void);

	//! ��ȡ��ǰ����ϵͳ����
	//! \return ����ö��
	WINDOWS_TYPE getOSType (void) const;

	//! ��ȡ��ǰ����ϵͳ���͵��ַ�����ʾ
	//! \return ��ȡ��ǰ����ϵͳ���͵��ַ�����ʾ
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
