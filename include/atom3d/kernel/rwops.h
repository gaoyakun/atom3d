/**	\file rwops.h
 *	��������������������.
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_IRWOPS_H
#define __ATOM_KERNEL_IRWOPS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

class ATOM_File;

//! \class ATOM_RWops
//! �����������������
//! \todo �Ƴ��˽ӿڣ���Ϊ��һ�㻯��������
//! \author ������
class ATOM_KERNEL_API ATOM_RWops
{
public:
	//! �����������
	enum RWOPS_TYPE
	{
		//! �����ļ�IO
		TYPE_FILE,
		//! �ڴ�IO
		TYPE_MEM,
		//! �����ļ�IO
		TYPE_VFS,
		//! �ڴ�ӳ���ļ�IO
		TYPE_MMAP
	};

	//! ���������ļ�IO������
	//! \param fn �����ļ��� 
	//! \param mode �ļ���ģʽ
	//! \return �����ļ�IO������
	static ATOM_RWops* createFileRWops(const char* fn, const char* mode);

	//! �����ڴ�IO������
	//! \param mem �ڴ滺������ʼ��ַ
	//! \param size �ڴ滺������С
	//! \return �ڴ�IO������
	static ATOM_RWops* createMemRWops(void* mem, unsigned size);

	//! ���������ļ�IO������
	//! \param file �����ļ�
	//! \return �����ļ�IO������
	static ATOM_RWops* createVFSRWops(ATOM_File* file);

	//! ɾ��IO������
	//! \param rwops IO������
	static void destroyRWops(ATOM_RWops* rwops);    

public:
	//! ��������
	virtual ~ATOM_RWops (void);

public:
	//! ��ȡ����������
	//! \return ����������
	virtual RWOPS_TYPE getType (void) const = 0;

	//! ��ȡ��������ǰIOλ��
	//! \return �������ʼ���ֽ�ƫ����
	virtual int tell (void) const = 0;

	//! �ƶ�IO��ǰλ��
	//! \param offset ƫ����
	//! \param whence ���ƫ�ƶ��� ������SEEK_SET, SEEK_CUR��SEEK_END
	//! \return �ƶ��Ժ�ĵ�ǰλ��
	virtual int seek (int offset, int whence) = 0;

	//! ��Ŀ���ȡ
	//! \param buffer ��ȡ������
	//! \param size ÿ����ȡ��Ԫ���ֽڴ�С
	//! \param count ��ȡ��Ԫ��
	//! \return �ɹ���ȡ�ĵ�Ԫ��
	virtual unsigned read(void* buffer, unsigned size, unsigned count) = 0;

	//! д�뵽Ŀ��
	//! \param buffer д�뻺����
	//! \param size ÿ��д�뵥Ԫ���ֽڴ�С
	//! \param count д�뵥Ԫ��
	//! \return �ɹ�д��ĵ�Ԫ��
	virtual unsigned write(const void* buffer, unsigned size, unsigned count) = 0;

	//! Ŀ����ܴ�С
	//! \return Ŀ����ܴ�С����λΪ�ֽ�
	virtual unsigned long size() = 0;

	//! ��Ŀ���ȡһ���ַ�
	//! \return ��ȡ���ַ�
	virtual char getC (void) = 0;

	//! ��ѯĿ���IOָ���Ƿ��Ѿ�λ��Ŀ���ĩβ
	//! \return true ��ĩβ false û��ĩβ
	virtual bool eof (void) const = 0;
};

template <class T>
inline T ATOM_RWopsRead (ATOM_RWops *rwops)
{
	T val;
	rwops->read (&val, sizeof(T), 1);
	return val;
}


#endif // __ATOM_KERNEL_IRWOPS_H
/*! @} */
