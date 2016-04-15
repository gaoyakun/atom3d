//////////////////////////////////////////////
// wangjian added . 
// 时间 ：	2013/6/13
// 描述 ：	新增一个RT surface的资源类型。
//			用于某些特殊情况下(比如：shadow mapping时需要的NULL RENDER TARGET支持)
//			直接设置RENDER TARGET的SURFACE.
//////////////////////////////////////////////

#include "rendertargetsurface.h"

ATOM_RenderTargetSurface::ATOM_RenderTargetSurface (void)
{
	_width = 0;
	_height = 0;
	_format = ATOM_RTSURFACE_FMT_ALL;
}

ATOM_RenderTargetSurface::~ATOM_RenderTargetSurface (void)
{
}
