#ifndef __ATOM3D_GFXDRIVER_MRTVIEWPORT_H
#define __ATOM3D_GFXDRIVER_MRTVIEWPORT_H

#include "viewport.h"

class ATOM_Texture;

class ATOM_MRTViewport: public ATOM_Viewport
{
public:
	//! 设置渲染目标贴图数量
	//! \param num 目标贴图数量
	//! \return true成功 false失败
	bool setNumRenderTargets (unsigned num) = 0;

	//! 获取渲染目标贴图数量
	//! \return 目标贴图数量
	unsigned getNumRenderTargets (void) const = 0;

	//! 设置渲染目标贴图
	//! \param index 索引
	//! \param rendertarget 渲染目标贴图
	//! \return true成功 false失败
	bool setRenderTarget (unsigned index, ATOM_Texture *rendertarget) = 0;

	//! 获取渲染目标贴图
	//! \param index 索引
	//! \return 渲染目标贴图或NULL
	ATOM_Texture *getRenderTarget (unsigned index) const = 0;
};

#endif // __ATOM3D_GFXDRIVER_MRTVIEWPORT_H
