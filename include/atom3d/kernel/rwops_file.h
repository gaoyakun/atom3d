/**	\file rwops_file.h
 *	�����ļ�������������������.
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_IRWOPSFILE_H
#define __ATOM_KERNEL_IRWOPSFILE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <cstdio>
#include "rwops.h"

//! \class ATOM_RWopsFile
//! �����ļ�IO����������
//! \todo �Ƴ��˽ӿڣ���Ϊ��һ�㻯��������
//! \author ������
class ATOM_RWopsFile : public ATOM_RWops
{
public:
	//! ���캯��
	//! \param filename �����ļ���
	//! \param mode �ļ���ģʽ
	ATOM_RWopsFile(const char* filename, const char* mode);

	//! ��������
	virtual ~ATOM_RWopsFile (void);

public:
	//! \copydoc ATOM_RWops::getType
	virtual RWOPS_TYPE getType (void) const;

	//! \copydoc ATOM_RWops::tell
	virtual int tell (void) const;

	//! \copydoc ATOM_RWops::seek
	virtual int seek(int offset, int whence);

	//! \copydoc ATOM_RWops::read
	virtual unsigned read(void* buffer, unsigned size, unsigned count);

	//! \copydoc ATOM_RWops::write
	virtual unsigned write(const void* buffer, unsigned size, unsigned count);

	//! \copydoc ATOM_RWops::size
	virtual unsigned long size (void);

	//! \copydoc ATOM_RWops::getC
	virtual char getC (void);

	//! \copydoc ATOM_RWops::eof
	virtual bool eof (void) const;

	//! ��ȡ�򿪵��ļ����
	//! \return �ļ������δ���ļ�����NULL
	virtual FILE *getFilePointer (void) const;

private:
	FILE* _M_fp;
};

#endif // __ATOM_KERNEL_IRWOPSFILE_H
/*! @} */
