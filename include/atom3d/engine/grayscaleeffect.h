#ifndef __ATOM3D_ENGINE_GRAYSCALEEFFECT_H
#define __ATOM3D_ENGINE_GRAYSCALEEFFECT_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_image.h"
#include "posteffect.h"

class ATOM_ENGINE_API ATOM_GrayScaleEffect: public ATOM_BasePostEffect
{
public:
	ATOM_GrayScaleEffect (ATOM_PixelFormat pixelFormat = ATOM_PIXEL_FORMAT_BGRX8888);

protected:
	virtual bool doRender (ATOM_RenderDevice *device, ATOM_Texture *inputColor);
	virtual ATOM_AUTOREF(ATOM_Texture) doAllocOutputColorTexture (ATOM_RenderDevice *device, ATOM_Texture *inputColor);

private:
	ATOM_PixelFormat _pixelFormat;
	ATOM_AUTOPTR(ATOM_Material) _material;
};


#endif // __ATOM3D_ENGINE_GRAYSCALEEFFECT_H

