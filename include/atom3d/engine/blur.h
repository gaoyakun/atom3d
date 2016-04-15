#ifndef __ATOM3D_ENGINE_BLUR_H
#define __ATOM3D_ENGINE_BLUR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"
#include "basedefs.h"

class ATOM_Blur
{
public:
	static bool drawVBlur3x3 (ATOM_RenderDevice *device, ATOM_Texture *sourceTexture, float blurScale = 1.f);
	static bool drawHBlur3x3 (ATOM_RenderDevice *device, ATOM_Texture *sourceTexture, float blurScale = 1.f);
	static bool drawVBlur5x5 (ATOM_RenderDevice *device, ATOM_Texture *sourceTexture, float blurScale = 1.f);
	static bool drawHBlur5x5 (ATOM_RenderDevice *device, ATOM_Texture *sourceTexture, float blurScale = 1.f);
	static bool drawVBlur7x7 (ATOM_RenderDevice *device, ATOM_Texture *sourceTexture, float blurScale = 1.f);
	static bool drawHBlur7x7 (ATOM_RenderDevice *device, ATOM_Texture *sourceTexture, float blurScale = 1.f);

private:
	static void initializeBlurMaterials (void);
	static void drawScreenQuad (ATOM_RenderDevice *device, unsigned w, unsigned h);

private:
	static bool blurMaterialsInitialized;
	static ATOM_AUTOPTR(ATOM_Material) _hBlurMaterial3x3;
	static ATOM_AUTOPTR(ATOM_Material) _vBlurMaterial3x3;
	static ATOM_AUTOPTR(ATOM_Material) _hBlurMaterial5x5;
	static ATOM_AUTOPTR(ATOM_Material) _vBlurMaterial5x5;
	static ATOM_AUTOPTR(ATOM_Material) _hBlurMaterial7x7;
	static ATOM_AUTOPTR(ATOM_Material) _vBlurMaterial7x7;
};

#endif // __ATOM3D_ENGINE_BLUR_H

