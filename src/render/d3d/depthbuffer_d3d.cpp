#include "StdAfx.h"
#include "depthbuffer_d3d.h"
#include "d3d9device.h"

ATOM_DepthBufferD3D::ATOM_DepthBufferD3D(void)
{
	ATOM_STACK_TRACE(ATOM_DepthBufferD3D::ATOM_DepthBufferD3D);

	_depthStencilSurface = 0;
}

ATOM_DepthBufferD3D::~ATOM_DepthBufferD3D (void)
{
	ATOM_STACK_TRACE(ATOM_DepthBufferD3D::~ATOM_DepthBufferD3D);

	setRenderDevice (0);
}

bool ATOM_DepthBufferD3D::create (IDirect3DSurface9 *surface)
{
	if (_depthStencilSurface != surface)
	{
		if (_depthStencilSurface)
		{
			_depthStencilSurface->Release ();
		}

		if (surface)
		{
			surface->AddRef ();
		}

		_depthStencilSurface = surface;
	}

	if (_depthStencilSurface)
	{
		D3DSURFACE_DESC desc;
		_depthStencilSurface->GetDesc (&desc);
		_width = desc.Width;
		_height = desc.Height;
	}
	else
	{
		_width = 0;
		_height = 0;
	}

	return true;
}

bool ATOM_DepthBufferD3D::create (unsigned width, unsigned height)
{
	ATOM_STACK_TRACE(ATOM_DepthBufferD3D::create);

	if (_depthStencilSurface)
	{
		if (_width == width && _height == height)
		{
			return true;
		}
		else
		{
			_depthStencilSurface->Release();
			_depthStencilSurface = 0;
		}
	}

	ATOM_D3D9Device *device = (ATOM_D3D9Device*)_M_device;
	if (_M_device)
	{
		IDirect3DSurface9 *surface = 0;
		D3DFORMAT format = device->getD3DPresentParams()->AutoDepthStencilFormat;
		HRESULT hr = device->getD3DDevice()->CreateDepthStencilSurface(width, height, device->getD3DPresentParams()->AutoDepthStencilFormat, D3DMULTISAMPLE_NONE, 0, TRUE, &surface, NULL);

		if (FAILED(hr))
		{
			ATOM_LOGGER::error ("%s(%d, %d) CreateDepthStencilSurface() failed.\n", __FUNCTION__, width, height);
			ATOM_CHECK_D3D_RESULT(hr);
			return false;
		}

		_width = width;
		_height = height;
		_depthStencilSurface = surface;
		return true;
	}
	return false;
}

void ATOM_DepthBufferD3D::invalidateImpl (bool needRestore)
{
	ATOM_STACK_TRACE(ATOM_DepthBufferD3D::invalidateImpl);

	ATOM_DepthBuffer::invalidateImpl (needRestore);

	if (_depthStencilSurface)
	{
		_depthStencilSurface->Release ();
		_depthStencilSurface = 0;
	}
}

void ATOM_DepthBufferD3D::restoreImpl (void)
{
	ATOM_STACK_TRACE(ATOM_DepthBufferD3D::restoreImpl);

	create (_width, _height);
}

IDirect3DSurface9 *ATOM_DepthBufferD3D::getDepthStencilSurface (void) const
{
	return _depthStencilSurface;
}

