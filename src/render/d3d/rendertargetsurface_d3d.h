//////////////////////////////////////////////
// wangjian added . 
// ʱ�� ��	2013/6/13
// ���� ��	����һ��D3D RT surface����Դ���͡�
//			����ĳЩ���������(���磺shadow mappingʱ��Ҫ��NULL RENDER TARGET֧��)
//			ֱ������RENDER TARGET��SURFACE.
//////////////////////////////////////////////

#ifndef __ATOM3D_RENDER_RTSURFACE_D3D_H
#define __ATOM3D_RENDER_RTSURFACE_D3D_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <d3d9.h>
#include "rendertargetsurface.h"

class ATOM_RenderTargetSurfaceD3D: public ATOM_RenderTargetSurface
{
    ATOM_CLASS(render, ATOM_RenderTargetSurfaceD3D, ATOM_RenderTargetSurfaceD3D)
	friend class ATOM_D3D9Device;
	friend class ATOM_D3DTexture;

public:
    ATOM_RenderTargetSurfaceD3D(void);
	virtual ~ATOM_RenderTargetSurfaceD3D (void);

public:
	IDirect3DSurface9 *getD3DSurface (void) const;

protected:
	virtual bool create (unsigned width, unsigned height,ATOM_RTSURFACE_FORMAT fmt);
	virtual bool create (IDirect3DSurface9 *surface);
	

protected:
    virtual void invalidateImpl (bool needRestore);
    virtual void restoreImpl (void);

private:
	IDirect3DSurface9 *_d3dSurface;

	D3DFORMAT chooseFormat( ATOM_RTSURFACE_FORMAT fmt ) const;
	ATOM_RenderTargetSurface::ATOM_RTSURFACE_FORMAT getFormatFromD3D( D3DFORMAT fmt ) const;
};

#endif // __ATOM3D_RENDER_RTSURFACE_D3D_H
