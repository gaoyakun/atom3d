#ifndef __ATOM3D_GFXDRIVER_DEPTHBUFFER_H
#define __ATOM3D_GFXDRIVER_DEPTHBUFFER_H

#include "basedefs.h"
#include "gfxresource.h"

class ATOM_DepthBuffer: public ATOM_GfxResource
{
protected:
    ATOM_DepthBuffer(void);
	virtual ~ATOM_DepthBuffer (void);

public:
    virtual unsigned getWidth (void) const;
    virtual unsigned getHeight (void) const;

protected:
	virtual bool create (unsigned width, unsigned height) = 0;

protected:
	unsigned _width;
	unsigned _height;
};

#endif // __ATOM3D_GFXDRIVER_DEPTHBUFFER_H
