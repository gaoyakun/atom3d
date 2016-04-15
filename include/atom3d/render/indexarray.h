/**	\file animatedtexturechannel.h
 *	�����������������.
 *
 *	\author ������
 *	\addtogroup render
 *	@{
 */

#ifndef __ATOM_GLRENDER_IINDEXARRAY_H
#define __ATOM_GLRENDER_IINDEXARRAY_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"
#include "gfxtypes.h"
#include "gfxresource.h"

//! \class ATOM_IndexArray
//! ������������
//! \author ������
class ATOM_IndexArray: public ATOM_GfxResource
{
protected:
    ATOM_IndexArray(void);

public:
	//! ��ȡ����������
	//! \return ����������
	virtual unsigned getNumIndices (void) const;

	//! ���������������Ը�������
	//! \param mode �������
	//! \param offset ����λ���������ʼλ�õ��ֽ�ƫ����
	//! \param size �����Ĵ�С�ֽ���
	//! \param overwrite false��ʾ������Χ����ͱ�������������������Ⱦ��������Χ�ص�
	//! \return ����������������ָ�룬ʧ�ܷ���NULL
    virtual void * lock(ATOM_LockMode mode, unsigned offset, unsigned size, bool overwrite) = 0;

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
	unsigned _M_num_indices;
	int _M_usage;
	bool _M_32bit;
	bool _M_content_lost;
	void *_preservedContent;
};

#endif // __ATOM_GLRENDER_IINDEXARRAY_H
/*! @} */
