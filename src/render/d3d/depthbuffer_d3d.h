#ifndef __ATOM3D_RENDER_DEPTHBUFFER_D3D_H
#define __ATOM3D_RENDER_DEPTHBUFFER_D3D_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <d3d9.h>
#include "depthbuffer.h"

class ATOM_DepthBufferD3D: public ATOM_DepthBuffer
{
    ATOM_CLASS(render, ATOM_DepthBufferD3D, ATOM_DepthBufferD3D)
	friend class ATOM_D3D9Device;
	friend class ATOM_D3DTexture;

public:
    ATOM_DepthBufferD3D(void);
	virtual ~ATOM_DepthBufferD3D (void);

public:
	IDirect3DSurface9 *getDepthStencilSurface (void) const;

protected:
	virtual bool create (unsigned width, unsigned height);
	virtual bool create (IDirect3DSurface9 *surface);

protected:
    virtual void invalidateImpl (bool needRestore);
    virtual void restoreImpl (void);

private:
	IDirect3DSurface9 *_depthStencilSurface;
};

#endif // __ATOM3D_RENDER_DEPTHBUFFER_D3D_H
