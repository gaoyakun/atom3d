/**	\file drivestate.h
 *	ATOM_DriveStats�������.
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_DRIVESTATE_H_
#define __ATOM_KERNEL_DRIVESTATE_H_

#include "basedefs.h"

//! \class ATOM_DriveStats
//! ������״̬��
//! \author ������
class ATOM_KERNEL_API ATOM_DriveStats
{
	friend class ATOM_DriveInfo;

public:      
	//! ��ȡ����������
	//! \return ����������
	const char *getName(void) const;

	//! ��ȡ����������
	//! \return ������������Ϣ
	const char *getType(void) const;

	//! ��ȡ������������
	//! \return ������������Ϣ
	const char *getTotalSpaceStr(void) const;

	//! ��ȡ��������������
	//! \return ����������������Ϣ
	const char *getFreeSpaceStr(void) const;

private:       // attributes
	ATOM_STRING  m_strName;          // drive name
	ATOM_STRING  m_strType;          // drive type
	ATOM_STRING  m_strTotalSpace;    // total drive space
	ATOM_STRING  m_strFreeSpace;     // total free space
};

#endif // __ATOM_KERNEL_DRIVESTATE_H_
/*! @} */
