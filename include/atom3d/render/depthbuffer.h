#ifndef __ATOM3D_RENDER_DEPTHBUFFER_H
#define __ATOM3D_RENDER_DEPTHBUFFER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"
#include "gfxtypes.h"
#include "gfxresource.h"

class ATOM_DepthBuffer: public ATOM_GfxResource
{
protected:
    ATOM_DepthBuffer(void);
	virtual ~ATOM_DepthBuffer (void);

public:
    virtual void setRenderDevice (ATOM_RenderDevice *device);
    virtual unsigned getWidth (void) const;
    virtual unsigned getHeight (void) const;

protected:
	virtual bool create (unsigned width, unsigned height) = 0;

protected:
	unsigned _width;
	unsigned _height;
};

#endif // __ATOM3D_RENDER_DEPTHBUFFER_H
