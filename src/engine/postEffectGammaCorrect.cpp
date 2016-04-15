#include "StdAfx.h"
#include "postEffectGammaCorrect.h"

ATOM_GammaCorrectEffect::ATOM_GammaCorrectEffect (void)
{
	_materialError = false;

	_gammaFlag = GAMMA_CORRECT;
}

ATOM_GammaCorrectEffect::~ATOM_GammaCorrectEffect (void)
{
	//_chain = 0;
}

bool ATOM_GammaCorrectEffect::render (ATOM_RenderDevice *device)
{
	if( !_chain )
		return false;

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

		_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/gamma_correct.mat");
		if (!_material)
		{
			_materialError = true;
			return false;
		}
		_material->setActiveEffect ("default");
	}

	_material->getParameterTable()->setTexture ("inputTexture", inputTexture.get());

	device->setRenderTarget (0, getRenderTarget());
	device->setViewport (0, ATOM_Rect2Di(0, 0, getRenderTarget()->getWidth(), getRenderTarget()->getHeight()));

	// ¿ªÆôSRGBÐ´RT
	device->enableSRGBWrite(true);

	drawTexturedFullscreenQuad (device, _material.get(), inputTexture->getWidth(), inputTexture->getHeight());

	// ¹Ø±ÕSRGBÐ´RT
	device->enableSRGBWrite(false);



#if 0
	unsigned numPasses = _material->begin (device);
	for (unsigned i = 0; i < numPasses; ++i)
	{
		if (_material->beginPass (device, i))
		{
			_material->endPass (device, i);
		}
	}
	_material->end (device);
#endif
	return true;
}