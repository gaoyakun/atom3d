#include "StdAfx.h"
#include "postEffectSilhouet.h"

///////////////////////////////////////////////////////////////////////

ATOM_SilhouetEffect::ATOM_SilhouetEffect (void)
{
	_qualityLevel = RAINLAYER_QUALITY_MEDIUM;
	_color = ATOM_Vector4f(1.0f,0.0f,0.0f,1.0f);

	_gammaFlag = AFTER_GAMMA_CORRECT;
}

ATOM_SilhouetEffect::~ATOM_SilhouetEffect (void)
{

}

void ATOM_SilhouetEffect::setQualityLevel(int quality)
{
	if( _qualityLevel != quality )
	{
		_qualityLevel = quality;
	}
}
int ATOM_SilhouetEffect::getQualityLevel()const
{
	return _qualityLevel;
}
bool ATOM_SilhouetEffect::render (ATOM_RenderDevice *device)
{
	if( !_material )
	{
		_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/silhouet.mat");
		if(_material)
			_material->setActiveEffect ("default");
		else
			return false;
	}

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

	_material->getParameterTable()->setTexture ("inputTexture", inputTexture.get());

	_material->getParameterTable()->setVector ("silcolor", _color);

	ATOM_Vector4f invScreenSize(1.0f / ATOM_RenderScheme::getCurrentRenderScheme()->getWidth (),
								1.0f / ATOM_RenderScheme::getCurrentRenderScheme()->getHeight(),
								0,0 );
	_material->getParameterTable()->setVector ("silInvScreenSize", invScreenSize);

	device->setRenderTarget (0, getRenderTarget());
	device->setViewport (0, ATOM_Rect2Di(0, 0, getRenderTarget()->getWidth(), getRenderTarget()->getHeight()));

	drawTexturedFullscreenQuad (device, _material.get(), inputTexture->getWidth(), inputTexture->getHeight());

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////