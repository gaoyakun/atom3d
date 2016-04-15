#include "StdAfx.h"
#include "postEffectDepthMask.h"

ATOM_DepthMaskEffect::ATOM_DepthMaskEffect (void)
{
	_materialError = false;

	_gammaFlag = AFTER_GAMMA_CORRECT;
}

ATOM_DepthMaskEffect::~ATOM_DepthMaskEffect (void)
{
	//_chain = 0;
}

bool ATOM_DepthMaskEffect::render (ATOM_RenderDevice *device)
{
	if( !_chain )
		return false;

	if( !init(device) )
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

		_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/depthMask.mat");
		if (!_material)
		{
			_materialError = true;
			return false;
		}
		_material->setActiveEffect ("default");
	}

	_material->getParameterTable()->setTexture ("inputTexture", inputTexture.get());

	device->setRenderTarget (0, getRenderTarget());

	// wangjian modified
	int width	= getRenderTarget()->getWidth();
	int height	= getRenderTarget()->getHeight();

	device->setViewport (0, ATOM_Rect2Di(0, 0, width, height));

	// scissor test
	int scissor_width	= (int)( width	* _vp_ratioWidth );
	int scissor_height	= (int)( height * _vp_ratioHeight );
	_chain->beginScissorTest (device, 0, 0, scissor_width, scissor_height);

	// ¿ªÆôSRGBÐ´RT
	//device->enableSRGBWrite(true);

	drawTexturedFullscreenQuad (device, _material.get(), width, height);

	// ¹Ø±ÕSRGBÐ´RT
	//device->enableSRGBWrite(false);

	_chain->endScissorTest(device);

	return true;
}