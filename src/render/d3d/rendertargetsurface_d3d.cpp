//////////////////////////////////////////////
// wangjian added . 
// 时间 ：	2013/6/13
// 描述 ：	新增一个RT surface的资源类型。
//			用于某些特殊情况下(比如：shadow mapping时需要的NULL RENDER TARGET支持)
//			直接设置RENDER TARGET的SURFACE.
//////////////////////////////////////////////

#include "StdAfx.h"
#include "rendertargetsurface_d3d.h"
#include "d3d9device.h"

ATOM_RenderTargetSurfaceD3D::ATOM_RenderTargetSurfaceD3D(void)
{
	ATOM_STACK_TRACE(ATOM_RenderTargetSurfaceD3D::ATOM_RenderTargetSurfaceD3D);

	_d3dSurface = 0;
}

ATOM_RenderTargetSurfaceD3D::~ATOM_RenderTargetSurfaceD3D (void)
{
	ATOM_STACK_TRACE(ATOM_RenderTargetSurfaceD3D::~ATOM_RenderTargetSurfaceD3D);

	setRenderDevice (0);
}

bool ATOM_RenderTargetSurfaceD3D::create (IDirect3DSurface9 *surface)
{
	if (_d3dSurface != surface)
	{
		if (_d3dSurface)
		{
			_d3dSurface->Release ();
		}

		if (surface)
		{
			surface->AddRef ();
		}

		_d3dSurface = surface;
	}

	if (_d3dSurface)
	{
		D3DSURFACE_DESC desc;
		_d3dSurface->GetDesc (&desc);
		_width = desc.Width;
		_height = desc.Height;
		_format = getFormatFromD3D(desc.Format);
	}
	else
	{
		_width = 0;
		_height = 0;
		_format = ATOM_RTSURFACE_FMT_ALL;
	}

	return true;
}

D3DFORMAT ATOM_RenderTargetSurfaceD3D::chooseFormat( ATOM_RTSURFACE_FORMAT fmt ) const
{
	switch(fmt)
	{
	case ATOM_RTSURFACE_FMT_NULLRT :
		return ((D3DFORMAT)MAKEFOURCC('N','U','L','L'));
	case ATOM_RTSURFACE_FMT_ALL:
		return D3DFMT_FORCE_DWORD;
	}
	return D3DFMT_FORCE_DWORD;
}

ATOM_RenderTargetSurface::ATOM_RTSURFACE_FORMAT ATOM_RenderTargetSurfaceD3D::getFormatFromD3D( D3DFORMAT fmt ) const
{
	switch(fmt)
	{
	case  ((D3DFORMAT)MAKEFOURCC('N','U','L','L')):
		return ATOM_RTSURFACE_FMT_NULLRT;
	case D3DFMT_FORCE_DWORD:
		return ATOM_RTSURFACE_FMT_ALL;
	}
	return ATOM_RTSURFACE_FMT_ALL;
}

bool ATOM_RenderTargetSurfaceD3D::create (unsigned width, unsigned height, ATOM_RTSURFACE_FORMAT fmt)
{
	ATOM_STACK_TRACE(ATOM_RenderTargetSurfaceD3D::create);

	if (_d3dSurface)
	{
		if (_width == width && _height == height && _format == fmt )
		{
			return true;
		}
		else
		{
			_d3dSurface->Release();
			_d3dSurface = 0;
		}
	}

	ATOM_D3D9Device *device = (ATOM_D3D9Device*)_M_device;
	if (device)
	{
		IDirect3DSurface9 *surface = 0;
		ATOM_ASSERT( fmt!=ATOM_RTSURFACE_FMT_ALL );
		D3DFORMAT format = chooseFormat(fmt);
		HRESULT hr = device->getD3DDevice()->CreateRenderTarget(	width, height, 
																	format, 
																	D3DMULTISAMPLE_NONE, 
																	0,  
																	FALSE,
																	&surface, 
																	NULL);

		if (FAILED(hr))
		{
			ATOM_LOGGER::error ("%s(%d, %d) CreateRenderTarget() failed.\n", __FUNCTION__, width, height);
			ATOM_CHECK_D3D_RESULT(hr);
			return false;
		}

		_width = width;
		_height = height;
		_format = fmt;
		_d3dSurface = surface;
		return true;
	}
	return false;
}

void ATOM_RenderTargetSurfaceD3D::invalidateImpl (bool needRestore)
{
	ATOM_STACK_TRACE(ATOM_RenderTargetSurfaceD3D::invalidateImpl);

	ATOM_RenderTargetSurface::invalidateImpl (needRestore);

	if (_d3dSurface)
	{
		_d3dSurface->Release ();
		_d3dSurface = 0;
	}
}

void ATOM_RenderTargetSurfaceD3D::restoreImpl (void)
{
	ATOM_STACK_TRACE(ATOM_RenderTargetSurfaceD3D::restoreImpl);

	create (_width, _height, _format);
}

IDirect3DSurface9 *ATOM_RenderTargetSurfaceD3D::getD3DSurface (void) const
{
	return _d3dSurface;
}

