/**	\file refobj.h
 *	���ü����������.
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_REFOBJ_H
#define __ATOM_KERNEL_REFOBJ_H

#if _MSC_VER > 1000
# pragma once
#endif

//! \class ATOM_Noncopyable
//! ���ɸ��ƶ������.
//! ���дӴ��������Ķ�������ɸ��ƣ�Ҳ����˵�޿�������͸�ֵ����
//! \author ������
class ATOM_KERNEL_API ATOM_Noncopyable
{
public:
	//! ���캯��
	ATOM_Noncopyable (void) {}

private:
	ATOM_Noncopyable (const ATOM_Noncopyable&);
	ATOM_Noncopyable & operator = (const ATOM_Noncopyable&);
};

//! \class ATOM_ReferenceObj
//! ���ü����������.
//! ���дӴ��������Ķ�����ɽ������ü�������
//! \author ������
class ATOM_KERNEL_API ATOM_ReferenceObj: public ATOM_Noncopyable
{
public:
	//! ���캯��
	ATOM_ReferenceObj (void): _M_reference(0) {}

	//! ��������
	virtual ~ATOM_ReferenceObj (void) {}

	//! �������ü���
	//! \return �������Ӻ�����ü���ֵ
	virtual int addRef(void) { return ++_M_reference; }

	//! �������ü���
	//! \return ���ؼ��ٺ�����ü���ֵ
	//! \note ���ü���ֵ���ٵ�0��˺�������ɾ������
	virtual int decRef(void) { return --_M_reference; }

	//! ��ȡ���ü���
	//! \return ���ü���ֵ
	virtual int getRef(void) const{ return _M_reference; }

private:
    int _M_reference;
};

#endif // __ATOM_KERNEL_REFOBJ_H
/*! @} */
