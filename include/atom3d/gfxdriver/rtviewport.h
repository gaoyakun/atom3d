#ifndef __ATOM3D_GFXDRIVER_RTVIEWPORT_H
#define __ATOM3D_GFXDRIVER_RTVIEWPORT_H

#include "viewport.h"

class ATOM_Texture;

class ATOM_RTViewport: public ATOM_Viewport
{
public:
	//! ������ȾĿ����ͼ
	//! \param rendertarget ��ȾĿ����ͼ
	//! \return true�ɹ� falseʧ��
	bool setRenderTarget (ATOM_Texture *rendertarget) = 0;

	//! ��ȡ��ȾĿ����ͼ
	//! \return ��ȾĿ����ͼ��NULL
	ATOM_Texture *getRenderTarget (void) const = 0;
};

#endif // __ATOM3D_GFXDRIVER_RTVIEWPORT_H
