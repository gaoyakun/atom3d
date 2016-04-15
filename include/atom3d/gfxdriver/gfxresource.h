/**	\file gfxresource.h
 *	�豸�����Դ���������.
 *
 *	\author ������
 *	\addtogroup render
 *	@{
 */

#ifndef __ATOM_GFXDRIVER_GFXRESOURCE_H
#define __ATOM_GFXDRIVER_GFXRESOURCE_H

#include "../ATOM_kernel.h"
#include "basedefs.h"

class ATOM_GfxResource;
class ATOM_GfxDriver;

//! \class ATOM_GfxResource
//! ��Ⱦ�豸�����Դ����
//! \author ������
class ATOM_GfxResource: public ATOM_Object
{
	friend class ATOM_GfxDriver;

protected:
	//! ���캯��
	ATOM_GfxResource (void);

	//! ��������
	virtual ~ATOM_GfxResource (void);

public:
	//! ɾ����Դ.
	//! ���豸��ʧ�˺����ᱻ����
	//! \param needRestore true��ʾ��Ҫ�ؽ�����Դ false��ʾ����Ҫ�ؽ�
	virtual void invalidate (bool needRestore);

	//! ���´�������Դ.
	//! ���豸��λʱ�˺����ᱻ����
	virtual void restore (void);

	//! ��úʹ���Դ��ص���Ⱦ�豸.
	//! \return ��Ⱦ�豸���������Դδ��ʼ��������ɾ���򷵻�NULL
	virtual ATOM_GfxDriver *getGfxDriver (void) const;

	//! ���ù�������Ⱦ�豸.
	//! ��Դ��ʼ������Ҫ���ô˺��������豸
	//! \param device ��Ⱦ�豸
	virtual void setGfxDriver (ATOM_GfxDriver *driver);

	//! ������Դ
	//! \return true �ɹ� false ʧ��
	virtual bool realize (void);

public:
	bool needRestore (void) const;
	void setNeedReset (bool b);
	bool needReset (void) const;
	ATOM_GfxResource *getNext (void) const;
	ATOM_GfxResource *getPrev (void) const;

protected:
	virtual void invalidateImpl (bool needRestore);
	virtual void restoreImpl (void);

protected:
	ATOM_GfxDriver *_driver;
	ATOM_GfxResource *_prev;
	ATOM_GfxResource *_next;
	bool _invalid;
	bool _needReset;
};

#endif // __ATOM_GFXDRIVER_GFXRESOURCE_H
/*! @} */
