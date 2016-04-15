/**	\file driveinfo.h
 *	ATOM_DriveInfo�������.
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_DRIVEINFO_H_
#define __ATOM_KERNEL_DRIVEINFO_H_

#include "../ATOM_dbghlp.h"
#include "basedefs.h"
#include "drivestate.h"

//! \class ATOM_DriveInfo
//! ��ȡ��ǰ�����ϵͳ����������Ϣ����
//! \author ������
class ATOM_KERNEL_API ATOM_DriveInfo
{
public:
	//! ���캯��
	ATOM_DriveInfo(void);

	//! ��ѯϵͳ���������ĸ���
	//! \return ����������
	unsigned getNumDrives (void) const;

	//! ��ȡĳ���������ĵ�ǰ״̬
	//! \param i ����������
	//! \return ������״̬
	//! \sa ATOM_DriveStats
	const ATOM_DriveStats &getDriveState (unsigned i) const;

private:
	ATOM_VECTOR<ATOM_DriveStats> m_vdriveStats;
};

#endif // __ATOM_KERNEL_DRIVEINFO_H_
/*! @} */
