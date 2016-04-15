#ifndef __ATOM3D_GFXDRIVER_MRTVIEWPORT_H
#define __ATOM3D_GFXDRIVER_MRTVIEWPORT_H

#include "viewport.h"

class ATOM_Texture;

class ATOM_MRTViewport: public ATOM_Viewport
{
public:
	//! ������ȾĿ����ͼ����
	//! \param num Ŀ����ͼ����
	//! \return true�ɹ� falseʧ��
	bool setNumRenderTargets (unsigned num) = 0;

	//! ��ȡ��ȾĿ����ͼ����
	//! \return Ŀ����ͼ����
	unsigned getNumRenderTargets (void) const = 0;

	//! ������ȾĿ����ͼ
	//! \param index ����
	//! \param rendertarget ��ȾĿ����ͼ
	//! \return true�ɹ� falseʧ��
	bool setRenderTarget (unsigned index, ATOM_Texture *rendertarget) = 0;

	//! ��ȡ��ȾĿ����ͼ
	//! \param index ����
	//! \return ��ȾĿ����ͼ��NULL
	ATOM_Texture *getRenderTarget (unsigned index) const = 0;
};

#endif // __ATOM3D_GFXDRIVER_MRTVIEWPORT_H
