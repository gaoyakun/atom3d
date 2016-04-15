/**	\file animatedtexturechannel.h
 *	�豸�����Դ���������.
 *
 *	\author ������
 *	\addtogroup render
 *	@{
 */

#ifndef __ATOM_GLRENDER_GFXRESOURCE_H
#define __ATOM_GLRENDER_GFXRESOURCE_H

#include "../ATOM_kernel.h"
#include "basedefs.h"

class ATOM_GfxResource;
class ATOM_RenderDevice;

//! \class ATOM_GfxResource
//! ��Ⱦ�豸�����Դ����
//! \author ������
class ATOM_GfxResource: public ATOM_Object
{
	friend class ATOM_RenderDevice;
	friend class ATOM_D3D9Device;

public:
	//! ��Դ������״̬
	enum LOADINGSTATE
	{
		//! δ����
		LS_NOTLOADED,
		//! ������
		LS_LOADED,
		//! ����ʧ��
		LS_LOADFAILED,
		//! ��������
		LS_LOADING
	};

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
	virtual ATOM_RenderDevice *getRenderDevice (void) const;

	//! ���ù�������Ⱦ�豸.
	//! ��Դ��ʼ������Ҫ���ô˺��������豸
	//! \param device ��Ⱦ�豸
	virtual void setRenderDevice (ATOM_RenderDevice *device);

	//! ������Դ
	//! \return true �ɹ� false ʧ��
	virtual bool realize (void);

	//! ��������״̬
	//! \param val ״̬
	virtual void setLoadingState (LOADINGSTATE val);

	//! ��ȡ����״̬
	//! \return ����״̬
	virtual LOADINGSTATE getLoadingState (void) const;

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
	ATOM_RenderDevice *_M_device;
	ATOM_GfxResource *_M_prev;
	ATOM_GfxResource *_M_next;
	unsigned _M_last_used_tick;
	bool _M_invalid;
	bool _M_need_reset;
	LOADINGSTATE _M_loading_state;
};

#endif // __ATOM_GLRENDER_GFXRESOURCE_H
/*! @} */
