/**	\file rwops_mem.h
 *	�ڴ�������������������.
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_IRWOPSMEM_H
#define __ATOM_KERNEL_IRWOPSMEM_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "rwops.h"

//! \class ATOM_RWopsMem
//! �ڴ�IO����������
//! \todo �Ƴ��˽ӿڣ���Ϊ��һ�㻯��������
//! \author ������
class ATOM_RWopsMem : public ATOM_RWops
{
public:
	//! ���캯��
	//! \param mem �ڴ滺������ʼ��ַ
	//! \param length �ڴ滺�����ֽ���
	ATOM_RWopsMem (void* mem, unsigned length);

	//! ��������
	virtual ~ATOM_RWopsMem (void);

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

protected:
	char* _M_start;
	char* _M_pos;
	char* _M_end;
	bool _M_eof;
};

#endif // __ATOM_KERNEL_IRWOPSMEM_H
/*! @} */
