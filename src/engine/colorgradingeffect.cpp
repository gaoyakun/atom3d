#include "StdAfx.h"
#include "colorgradingeffect.h"

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_ColorGradingEffect)
	ATOM_ATTRIBUTES_BEGIN(ATOM_ColorGradingEffect)
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_ColorGradingEffect, "LUT", getLUTTextureFileName, setLUTTextureFileName, "", "group=ATOM_ColorGradingEffect;type=vfilename;desc='VFS LUT texture file name'")
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_ColorGradingEffect, ATOM_Object)

ATOM_ColorGradingEffect::ATOM_ColorGradingEffect (void)
{
	_textureDirty = false;
	_materialError = false;
}

ATOM_ColorGradingEffect::ATOM_ColorGradingEffect (ATOM_Texture *lutTexture)
{
	_lutTexture = lutTexture;
	_materialError = false;
}

ATOM_ColorGradingEffect::~ATOM_ColorGradingEffect (void)
{
}

bool ATOM_ColorGradingEffect::render (ATOM_RenderDevice *device)
{
#if 0 
	ATOM_PostEffect *prevEffect = getPreviousEffect ();
	ATOM_Texture *inputTexture = prevEffect ? prevEffect->getRenderTarget() : _chain->getInputTexture();
#else
	ATOM_AUTOREF(ATOM_Texture) inputTexture = getSourceInputTex();
#endif
	if (!inputTexture)
	{
		return false;
	}

	if (!_material)
	{
		if (_materialError)
		{
			return false;
		}

		_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/colorgradingeffect.mat");
		if (!_material)
		{
			_materialError = true;
			return false;
		}
		_material->setActiveEffect ("default");
	}

	if (_textureDirty)
	{
		if (!_lutTextureFileName.empty ())
		{
			_lutTexture = ATOM_CreateTextureResource (_lutTextureFileName.c_str());
		}
		_textureDirty = false;
	}

	if (!_lutTexture)
	{
		createDefaultLutTexture (device);
		if (!_lutTexture)
		{
			return false;
		}
	}

	_material->getParameterTable()->setTexture ("inputTexture", inputTexture.get());
	_material->getParameterTable()->setTexture ("lutTexture", _lutTexture.get());

	ATOM_Vector4f halfTexSize(0.5f/_lutTexture->getWidth());
	_material->getParameterTable()->setVector ("halfTexSize", halfTexSize);

	device->setRenderTarget (0, getRenderTarget());
	device->setViewport (0, ATOM_Rect2Di(0, 0, getRenderTarget()->getWidth(), getRenderTarget()->getHeight()));

	drawTexturedFullscreenQuad (device, _material.get(), inputTexture->getWidth(), inputTexture->getHeight());
#if 0
	unsigned numPasses = _material->begin (device);
	for (unsigned i = 0; i < numPasses; ++i)
	{
		if (_material->beginPass (device, i))
		{
			drawTexturedFullscreenQuad (device, inputTexture->getWidth(), inputTexture->getHeight());
			_material->endPass (device, i);
		}
	}
	_material->end (device);
#endif
	return true;
}

void ATOM_ColorGradingEffect::createDefaultLutTexture (ATOM_RenderDevice *device)
{
	int w = 16;
	int h = 16;
	int d = 16;

	ATOM_HARDREF(ATOM_Image) image;

	if (!image)
	{
		return;
	}

	image->init (w, h * d, ATOM_PIXEL_FORMAT_BGRA8888);

	ATOM_ColorARGB *colors = (ATOM_ColorARGB*)image->getData ();
	unsigned rScale = 255/(w-1);
	unsigned gScale = 255/(h-1);
	unsigned bScale = 255/(d-1);

	for (unsigned i = 0; i < h * d; ++i)
	{
		for (unsigned j = 0; j < w; ++j)
		{
			unsigned r = j % w;
			unsigned g = i % h;
			unsigned b = i / d;
			colors->setBytes (r*rScale, g*gScale, b*bScale, 255);
			colors++;
		}
	}

	_lutTexture = device->allocVolumeTexture (0, image->getData(), w, h, d, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::NOMIPMAP|ATOM_Texture::NOCOMPRESS|ATOM_Texture::TEXTURE3D);
}

void ATOM_ColorGradingEffect::setLUT (ATOM_Texture *lutTexture)
{
	_lutTexture = lutTexture;
	_textureDirty = false;
}

ATOM_Texture *ATOM_ColorGradingEffect::getLUT (void) const
{
	return _lutTexture.get();
}

void ATOM_ColorGradingEffect::setLUTTextureFileName (const ATOM_STRING &fileName)
{
	if (_lutTextureFileName != fileName)
	{
		_lutTextureFileName = fileName;
		_textureDirty = true;
	}
}

const ATOM_STRING &ATOM_ColorGradingEffect::getLUTTextureFileName (void) const
{
	return _lutTextureFileName;
}
