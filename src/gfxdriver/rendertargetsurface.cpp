//////////////////////////////////////////////
// wangjian added . 
// ʱ�� ��	2013/6/13
// ���� ��	����һ��RT surface����Դ���͡�
//			����ĳЩ���������(���磺shadow mappingʱ��Ҫ��NULL RENDER TARGET֧��)
//			ֱ������RENDER TARGET��SURFACE.
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
