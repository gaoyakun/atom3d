/**	\file file.h
 *	�����ļ���
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_FILE_H
#define __ATOM_KERNEL_FILE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_vfs.h"

#include "basedefs.h"

//! \class ATOM_File
//! �����ļ�����
//! \author ������
//! \ingroup kernel
class ATOM_KERNEL_API ATOM_File
{
public:
	//! ���캯��
	//! \param handle �����ļ����
	ATOM_File(ATOM_VFS::handle handle);

	//! ��������
	virtual ~ATOM_File (void);

public:
	//! ��ȡ�����ļ����
	//! \return �����ļ����
	virtual ATOM_VFS::handle getHandle (void) const;

	//! ��ȡ�ļ���
	//! \return �ļ���
	virtual const char* getName (void) const;

	//! ��ȡ�ļ�
	//! \param buffer ��ȡ������
	//! \param size Ҫ��ȡ���ֽ���
	//! \return ʵ�ʶ�ȡ���ֽ���
	//! \note �����ı��ļ�����Ϊ��Ҫת�����з�����ʹ��ȡ�ɹ������ص�ʵ�ʴ�СҲ����С����Ҫ��ȡ�Ĵ�С
	virtual unsigned long read (void* buffer, unsigned long size);

	//! д���ļ�
	//! \param buffer д�뻺����
	//! \param size ��Ҫд����ֽ���
	//! \return ʵ��д����ֽ���
	virtual unsigned long write (const void* buffer, unsigned long size);

	//! ��ȡ��ǰ�ļ�λ��ָ��
	//! \return �ļ�λ�ã�������ļ���ʼ
	virtual unsigned long getPosition (void) const;

	//! �ƶ���ǰ�ļ�λ��ָ��
	//! \param offset ƫ����
	//! \param origin ���ƫ��λ�ã�������ATOM_VFS::begin, ATOM_VFS::current����ATOM_VFS::end
	//! \return true �ɹ� false ʧ��
	virtual bool seek(long offset, int origin);

	//! ��ȡ�ļ���С
	//! \return �ļ���С���ֽ���
	virtual unsigned long size (void) const;

	//! ��ѯ�ļ�λ��ָ���Ƿ��Ѿ��ﵽ�ļ�ĩβ
	//! \return true �ﵽĩβ false δ�ﵽĩβ
	virtual bool eof (void) const;

	//! ��ʽ���ļ�д��
	//! \param format �ļ���ʽ��
	//! \return д����ֽ���
	virtual unsigned long printf(const char *format, ...);

private:
	ATOM_VFS::handle _M_handle;
};

#endif // __ATOM_KERNEL_FILE_H
/*! @} */
