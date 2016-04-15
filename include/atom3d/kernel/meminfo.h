/**	\file meminfo.h
 *	�ڴ���Ϣ��MemoryInfo������.
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_MEMINFO_H_
#define __ATOM_KERNEL_MEMINFO_H_

#include "basedefs.h"

//! \class ATOM_MemoryInfo
//! ��ȡ��ǰ�����ϵͳ��CPU��Ϣ����
//! \author ������
class ATOM_KERNEL_API ATOM_MemoryInfo
{
public:
	//! ���캯��
	ATOM_MemoryInfo (void);

	//! �õ��ڴ�����
	//! \return �ڴ���������λΪ�ֽ�
	unsigned getTotalRam (void) const;

	//! �õ����ύ�������ڴ�ҳ���ļ���С
	//! \return ���ύ�������ڴ�ҳ���ļ���С����λΪ�ֽ�
	unsigned getTotalPageFile (void) const;

	//! �õ������ڴ��ַ�ռ��С
	//! \return �����ڴ��ַ�ռ��С����λΪ�ֽ�
	unsigned getTotalVirtual (void) const;

	//! �õ��ڴ��������ַ�����ʾ
	//! \return �ڴ��������ַ�����ʾ
	const char *getTotalRamStr (void) const;

	//! �õ����ύ�������ڴ�ҳ���ļ���С���ַ�����ʾ
	//! \return ���ύ�������ڴ�ҳ���ļ���С���ַ�����ʾ
	const char *getTotalPageFileStr (void) const;

	//! �õ������ڴ��ַ�ռ��С���ַ�����ʾ
	//! \return �õ������ڴ��ַ�ռ��С���ַ�����ʾ
	const char *getTotalVirtualStr (void) const;

private:
	unsigned totalRam;
	unsigned totalPageFile;
	unsigned totalVirtual;
	char totalRamString[64];
	char totalPageFileString[64];
	char totalVirtualString[64];
};

#endif // __ATOM_KERNEL_MEMINFO_H_
/*! @} */
