/**	\file rwops_file.h
 *	�����ļ�������������������.
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_IRWOPSVFS_H
#define __ATOM_KERNEL_IRWOPSVFS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "rwops.h"

//! \class ATOM_RWopsVFS
//! �����ļ�IO����������
//! \todo �Ƴ��˽ӿڣ���Ϊ��һ�㻯��������
//! \author ������
class ATOM_RWopsVFS : public ATOM_RWops
{
public:
	//! ���캯��
	//! \param file �����ļ�
	ATOM_RWopsVFS (ATOM_File* file);

	//! ��������
	virtual ~ATOM_RWopsVFS (void);

public:
	//! \copydoc ATOM_RWops::getType
	virtual RWOPS_TYPE getType (void) const;

	//! \copydoc ATOM_RWops::tell
	virtual int tell (void) const;

	//! \copydoc ATOM_RWops::seek
	virtual int seek (int offset, int whence);

	//! \copydoc ATOM_RWops::read
	virtual unsigned read (void* buffer, unsigned size, unsigned count);

	//! \copydoc ATOM_RWops::write
	virtual unsigned write (const void* buffer, unsigned size, unsigned count);

	//! \copydoc ATOM_RWops::size
	virtual unsigned long size (void);

	//! \copydoc ATOM_RWops::getC
	virtual char getC (void);

	//! \copydoc ATOM_RWops::eof
	virtual bool eof (void) const;

private:
	ATOM_File* _M_file;
};

#endif // __ATOM_KERNEL_IRWOPSVFS_H
/*! @} */
