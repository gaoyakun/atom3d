#ifndef __ATOM3D_ENGINE_POSTEFFECT_H
#define __ATOM3D_ENGINE_POSTEFFECT_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"
#include "../ATOM_render.h"
#include "basedefs.h"

class ATOM_ENGINE_API ATOM_BasePostEffect: public ATOM_ReferenceObj
{
public:
	virtual ~ATOM_BasePostEffect (void);

public:
	virtual void setNextEffect (ATOM_BasePostEffect *nextEffect);
	virtual ATOM_BasePostEffect *getNextEffect (void) const;
	virtual void chain (ATOM_BasePostEffect *tailEffect);
	virtual bool render (ATOM_RenderDevice *device, ATOM_Texture *inputColor);
	virtual void drawTexturedFullscreenQuad (ATOM_RenderDevice *device, unsigned w, unsigned h);

protected:
	virtual bool doRender (ATOM_RenderDevice *device, ATOM_Texture *inputColor) = 0;
	virtual ATOM_AUTOREF(ATOM_Texture) doAllocOutputColorTexture (ATOM_RenderDevice *device, ATOM_Texture *inputColor) = 0;

protected:
	ATOM_AUTOPTR(ATOM_BasePostEffect) _nextEffect;
	ATOM_AUTOREF(ATOM_Texture) _outputColor;
};

#endif // __ATOM3D_ENGINE_POSTEFFECT_H
