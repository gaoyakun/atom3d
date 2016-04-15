/**	\file autofile.h
 *	ATOM_AutoFile�������.
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_AUTOFILE_H
#define __ATOM_KERNEL_AUTOFILE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <string>
#include "kernel.h"

//! \class ATOM_AtomInt
//! �����ļ�ϵͳ�ļ�ʵ����װ��.
//! \author ������
class ATOM_AutoFile
{
	struct ReferencedFileHandle
    {
        ATOM_File *handle;
        unsigned reference;
	};

public:
	//! ���캯��
	inline ATOM_AutoFile (void);

	//! ���캯��
	//! \param path Ҫ�򿪵������ļ���
	//! \param mode ��ģʽ
	inline ATOM_AutoFile (const char* path, unsigned mode);

	//! �������캯��
	inline ATOM_AutoFile (const ATOM_AutoFile& rhs);

	//! ��������
	inline ~ATOM_AutoFile (void);

	//! ��ֵ����������
	inline  ATOM_AutoFile& operator = (const ATOM_AutoFile& rhs);

	//! ����
	inline void Swap (ATOM_AutoFile& rhs);

public:
	//! ת��Ϊ�����ļ�ʵ��ָ��
	inline operator ATOM_File *(void) const;

	//! �����������ļ�ʵ��ָ��һ������
	inline ATOM_File* operator ->(void) const;

	//! �����Ƿ����
	friend inline bool operator ==(const ATOM_AutoFile& f1, const ATOM_AutoFile& f2);

	//! �����Ƿ񲻵�
	friend inline bool operator !=(const ATOM_AutoFile& f1, const ATOM_AutoFile& f2);

private:
	ReferencedFileHandle* _M_referenced_handle;
};

// ATOM_AutoFile inline member functions

ATOM_AutoFile::ATOM_AutoFile (void): _M_referenced_handle(0) 
{
}

ATOM_AutoFile::ATOM_AutoFile(const char* path, unsigned mode): _M_referenced_handle(0) 
{
	if ( path)
	{
		ATOM_File* handle = ATOM_OpenFile(path, mode);
		if ( handle)
		{
			_M_referenced_handle = (ReferencedFileHandle*)ATOM_MALLOC(sizeof(ReferencedFileHandle));//new ReferencedFileHandle;
			_M_referenced_handle->handle = handle;
			_M_referenced_handle->reference = 1;
		}
	}
};

ATOM_AutoFile::ATOM_AutoFile(const ATOM_AutoFile& rhs): _M_referenced_handle(rhs._M_referenced_handle) 
{
	if ( _M_referenced_handle)
	{
		++_M_referenced_handle->reference;
	}
}

ATOM_AutoFile::~ATOM_AutoFile(void) 
{
	if ( _M_referenced_handle && !--_M_referenced_handle->reference)
	{
		ATOM_CloseFile (_M_referenced_handle->handle);
		ATOM_FREE (_M_referenced_handle);
	}
}

void ATOM_AutoFile::Swap(ATOM_AutoFile& rhs) 
{
	std::swap(_M_referenced_handle, rhs._M_referenced_handle);
}

ATOM_AutoFile& ATOM_AutoFile::operator =(const ATOM_AutoFile& rhs) 
{
	ATOM_AutoFile tmp(rhs);
	Swap(tmp);
	return *this;
}

ATOM_AutoFile::operator ATOM_File *() const 
{
	return _M_referenced_handle ? _M_referenced_handle->handle : 0;
}

ATOM_File* ATOM_AutoFile::operator ->() const 
{
	return _M_referenced_handle ? _M_referenced_handle->handle : 0;
}

bool operator ==(const ATOM_AutoFile& f1, const ATOM_AutoFile& f2) 
{
	if ( f1._M_referenced_handle == f2._M_referenced_handle)
	{
		return true;
	}

	if ( !f1._M_referenced_handle || !f2._M_referenced_handle)
	{
		return false;
	}

	return f1._M_referenced_handle->handle == f2._M_referenced_handle->handle;
}

bool operator != (const ATOM_AutoFile& f1, const ATOM_AutoFile& f2) 
{
	return !operator == (f1, f2);
}

#endif // __ATOM_KERNEL_AUTOFILE_H

/*! @} */
