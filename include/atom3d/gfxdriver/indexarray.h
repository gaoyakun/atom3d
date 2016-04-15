/**	\file indexarray.h
 *	�����������������.
 *
 *	\author ������
 *	\addtogroup render
 *	@{
 */

#ifndef __ATOM_GFXDRIVER_IINDEXARRAY_H
#define __ATOM_GFXDRIVER_IINDEXARRAY_H

#include "basedefs.h"
#include "gfxresource.h"

//! \class ATOM_IndexArray
//! ������������
//! \author ������
class ATOM_IndexArray: public ATOM_GfxResource
{
public:
	enum LockMode
	{
		ReadOnly,
		WriteOnly,
		ReadWrite
	};

	enum Usage
	{
		Static,
		Dynamic
	};

protected:
    ATOM_IndexArray(void);

public:
	//! ��ȡ����������
	//! \return ����������
	virtual unsigned getNumIndices (void) const;

	//! ��ȡʹ�÷�ʽ
	//! \return ʹ�÷�ʽ
	virtual Usage getUsage (void) const;

	//! ���������������Ը�������
	//! \param mode �������
	//! \param offset ����λ���������ʼλ�õ��ֽ�ƫ����
	//! \param size �����Ĵ�С�ֽ���
	//! \param overwrite false��ʾ������Χ����ͱ�������������������Ⱦ��������Χ�ص�
	//! \return ����������������ָ�룬ʧ�ܷ���NULL
    virtual void * lock(LockMode mode, unsigned offset, unsigned size, bool overwrite) = 0;

	//! ������ݸ�����������������
    virtual void unlock(void) = 0;

	//! ��ѯ�������ڵ������Ƿ���Ϊ�豸��ʧ��ʧЧ
	//! \return true ��ʧЧ false δʧЧ
	virtual bool isContentLost (void) const;

	//! �������ݵ�ʧЧ���
	//! \param b true��ʾʧЧ false��ʾδʧЧ
	virtual void setContentLost (bool b);

protected:
    virtual void invalidateImpl (bool needRestore) = 0;

protected:
	unsigned _numIndices;
	Usage _usage;
	bool _32bit;
	bool _contentLost;
};

#endif // __ATOM_GFXDRIVER_IINDEXARRAY_H
/*! @} */
