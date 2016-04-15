#ifndef __ATOM3D_GFXDRIVER_VIEWPORT_H
#define __ATOM3D_GFXDRIVER_VIEWPORT_H

#include "basedefs.h"

class ATOM_Viewport
{
public:
	//! �����ӿڷ�Χ
	//! \param region ��Χ
	//! \return true�ɹ� falseʧ��
	virtual bool setRegion (const ATOM_Rect2Di &region) = 0;

	//! ��ȡ�ӿڷ�Χ
	//! \return �ӿڷ�Χ
	virtual const ATOM_Rect2Di &getRegion (void) const = 0;

	//! ��ȡ���ɫ
	//! \return ���ɫ
	virtual ATOM_ColorARGB getClearColor(void) const = 0;

	//! �������ɫ
	//! \param color ���ɫ
	virtual void setClearColor(ATOM_ColorARGB color) = 0;

	//! ��ȡ��Ȼ��������ֵ
	//! \return ������ֵ
	virtual float getClearDepth (void) const = 0;

	//! ������Ȼ��������ֵ
	//! \param depth ���ֵ����Χ0��1
	virtual void setClearDepth(float depth) = 0;

	//! ��ȡģ�建�������ֵ
	//! \return ģ�建�������ֵ
	virtual int getClearStencil (void) const = 0;

	//! ������Ⱦ���ڵ�ģ�建�������ֵ
	//! \param stencil ���ֵ����Χ0��255
	virtual void setClearStencil(int stencil) = 0;

	//! ����󱸻�����
	//! \param clearColor true�����ɫ false�������ɫ
	//! \param clearDepth true������ false��������
	//! \param clearDepth true���ģ�� false�����ģ��
	//! \return true�ɹ� falseʧ��
	virtual bool clear (bool clearColor, bool clearDepth, bool clearStencil) = 0;

	//! ���ü�������
	//! \param scissorRect ��������
	virtual void setScissorRect (const ATOM_Rect2Di &scissorRect);

	//! ��ȡ���÷�Χ
	//! \return ��������
	virtual ATOM_Rect2Di getScissorRect (void) const;

	//! �����Ƿ������ӿڼ���
	//! \param enable true��ʾ����false��ʾ������
	virtual void enableScissorTest (bool enable) = 0;

	//! ��ѯ�ӿڼ����Ƿ��
	//! \return true �� false δ��
	//! \sa enableScissorTest setScissorRect getScissorRect
	virtual bool isScissorTestEnabled (void) const = 0;

	//! ��ȡ��ɫ����������
	//! \param rc ��ȡ��Χ
	//! \param format ��ȡ�����ظ�ʽ
	//! \param pixels ��ȡ����������������㹻�Ĵ�С
	//! \return true �ɹ� false ʧ��
	virtual bool readColorBuffer(const ATOM_Rect2Di &rc, ATOM_PixelFormat format, void *pixels) = 0;

	//! ��ȡ��ɫ���������ݲ����浽ͼ���ļ���
	//! \param path �����ļ�·��
	//! \return true �ɹ� false ʧ��
	virtual bool grabScreenToFile (const char *path) = 0;

	//! ��ȡ��ɫ���������ݲ����浽ATOM_Image����
	//! \param image ATOM_Image��ָ��
	//! \return true �ɹ� false ʧ��
	virtual bool grabScreenToImage (ATOM_Image *image) = 0;

	//! ��ʼ��Ⱦ�����ӿ�
	//! \return true�ɹ� falseʧ��
	//! \sa endFrame
	virtual bool beginRender (void) = 0;

	//! �������ӿڵ���Ⱦ
	//! \sa beginRender
	virtual void endRender (void) = 0;
};

#endif // __ATOM3D_GFXDRIVER_VIEWPORT_H
