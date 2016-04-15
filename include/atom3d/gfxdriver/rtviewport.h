#ifndef __ATOM3D_GFXDRIVER_RTVIEWPORT_H
#define __ATOM3D_GFXDRIVER_RTVIEWPORT_H

#include "viewport.h"

class ATOM_Texture;

class ATOM_RTViewport: public ATOM_Viewport
{
public:
	//! 设置渲染目标贴图
	//! \param rendertarget 渲染目标贴图
	//! \return true成功 false失败
	bool setRenderTarget (ATOM_Texture *rendertarget) = 0;

	//! 获取渲染目标贴图
	//! \return 渲染目标贴图或NULL
	ATOM_Texture *getRenderTarget (void) const = 0;
};

#endif // __ATOM3D_GFXDRIVER_RTVIEWPORT_H
