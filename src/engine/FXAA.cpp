#include "StdAfx.h"
#include "FXAA.h"

ATOM_FXAAEffect::ATOM_FXAAEffect (void)
{
	_materialError = false;

	// wangjian added
	_gammaFlag = AFTER_GAMMA_CORRECT;
}

ATOM_FXAAEffect::~ATOM_FXAAEffect (void)
{

}

// wangjian added
void ATOM_FXAAEffect::enable (int b)
{
	ATOM_PostEffect::enable(b);
	ATOM_RenderSettings::enableFSAA( b==0 ? false : true );
}
int ATOM_FXAAEffect::isEnabled (void) const
{
	if( !ATOM_RenderSettings::isFSAAEnabled() )
		return 0;
	return ATOM_PostEffect::isEnabled();
}

bool ATOM_FXAAEffect::init(ATOM_RenderDevice *device)
{
	if( !ATOM_PostEffect::init(device) )
		return false;

	if (!_material)
	{
		if (_materialError)
		{
			return false;
		}

		_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/FXAA.mat");
		if (!_material)
		{
			_materialError = true;
			return false;
		}
		_material->setActiveEffect ("default");
	}

	return true;
}

bool ATOM_FXAAEffect::render (ATOM_RenderDevice *device)
{
	if( !init(device) )
		return false;

	ATOM_PostEffect *prevEffect = getPreviousEffect ();
	ATOM_Texture *inputTexture = prevEffect ? prevEffect->getRenderTarget() : _chain->getInputTexture();
	if (!inputTexture)
	{
		return false;
	}

	_material->getParameterTable()->setTexture ("inputTexture", inputTexture);
	_material->getParameterTable()->setVector (	"invTexSize", 
												ATOM_Vector4f(1.f/inputTexture->getWidth(), 1.f/inputTexture->getHeight(), 0.f, 0.f));

	device->setRenderTarget (0, getRenderTarget());
	
	// wangjian modified
	int width	= getRenderTarget()->getWidth();
	int height	= getRenderTarget()->getHeight();
#if 0
	width	= (int)( width	* _vp_ratioWidth );
	height	= (int)( height * _vp_ratioHeight );
#endif

	device->setViewport (0, ATOM_Rect2Di(0, 0, width, height));

	//////////////////////////////////////////////////////////
	// scissor test
#if 1
	int scissor_width	= (int)( width	* _vp_ratioWidth );
	int scissor_height	= (int)( height * _vp_ratioHeight );
#endif
	_chain->beginScissorTest (device, 0, 0, scissor_width, scissor_height);

	drawTexturedFullscreenQuad ( device, _material.get(), width, height );

	_chain->endScissorTest(device);
	/////////////////////////////////////////////////////////

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

