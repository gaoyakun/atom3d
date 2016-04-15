#include "StdAfx.h"
#include "postEffectHeatHaze.h"


///////////////////////////////////////////////////////////////////////
ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_HeatHazeEffect)
	ATOM_ATTRIBUTES_BEGIN(ATOM_HeatHazeEffect)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_HeatHazeEffect, "PerturbationTexture", getPerturbationTextureFileName, setPerturbationTextureFileName, "", "group=ATOM_HeatHazeEffect;type=vfilename;desc='扰动贴图'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_HeatHazeEffect, "PerturbationScale", getPerturbationScale, setPerturbationScale, 1.0f, "group=ATOM_HeatHazeEffect;desc='扰动量'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_HeatHazeEffect, "PerturbationSpeed", getPerturbationSpeed, setPerturbationSpeed, 1.0f, "group=ATOM_HeatHazeEffect;desc='扰动速度'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_HeatHazeEffect, "PerturbationTiling", getPerturbationTiling, setPerturbationTiling, 1.0f, "group=ATOM_HeatHazeEffect;desc='扰动密度'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_HeatHazeEffect, "PerturbationDistance", getPerturbationDistance, setPerturbationDistance, 1.0f, "group=ATOM_HeatHazeEffect;desc='扰动距离'")
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_HeatHazeEffect, ATOM_PostEffect)


ATOM_HeatHazeEffect::ATOM_HeatHazeEffect (void)
{
	_perturbScale = 1.0f;
	_perturbSpeed = 1.0f;
	_perturbTiling = 5.0f;
	_perturbDistance = 500.0f;

	_perturbationTexFileName = "";

	_perturbationTex = 0;
	_material = 0;

	_gammaFlag = AFTER_GAMMA_CORRECT;
}

ATOM_HeatHazeEffect::~ATOM_HeatHazeEffect (void)
{
	destroy();
}

void ATOM_HeatHazeEffect::setPerturbationScale(const float perturbscale)
{
	_perturbScale = perturbscale;
}
const float ATOM_HeatHazeEffect::getPerturbationScale(void) const
{
	return _perturbScale;
}
void ATOM_HeatHazeEffect::setPerturbationSpeed(const float perturbspeed)
{
	_perturbSpeed = perturbspeed;
}
const float ATOM_HeatHazeEffect::getPerturbationSpeed(void) const
{
	return _perturbSpeed;
}
void ATOM_HeatHazeEffect::setPerturbationTiling(const float perturbtiling)
{
	_perturbTiling = perturbtiling;
}
const float ATOM_HeatHazeEffect::getPerturbationTiling(void) const
{
	return _perturbTiling;
}
void ATOM_HeatHazeEffect::setPerturbationDistance(const float perturbdist)
{
	_perturbDistance = perturbdist;
}
const float ATOM_HeatHazeEffect::getPerturbationDistance(void) const
{
	return _perturbDistance;
}
void ATOM_HeatHazeEffect::setPerturbationTextureFileName (const ATOM_STRING &filename)
{
	ATOM_STACK_TRACE(ATOM_HeatHazeEffect::setTextureFileName);

	_perturbationTexFileName = filename;

	if (!_perturbationTexFileName.empty ())
	{
		ATOM_RenderDevice *device = ATOM_GetRenderDevice();

		ATOM_AUTOREF(ATOM_Texture) newTexture = ATOM_CreateTextureResource (_perturbationTexFileName.c_str(), 
																			ATOM_PIXEL_FORMAT_UNKNOWN, 
																			1/*ATOM_LoadPriority_IMMEDIATE*/	);

		if (newTexture != _perturbationTex)
		{
			_perturbationTex = newTexture;
			_material = 0;
		}
	}
	else if (_perturbationTex)
	{
		_perturbationTex = 0;
		_material = 0;
	}
}
const ATOM_STRING & ATOM_HeatHazeEffect::getPerturbationTextureFileName (void) const
{
	return _perturbationTexFileName;
}

bool ATOM_HeatHazeEffect::init(ATOM_RenderDevice *device)
{
	if( !ATOM_PostEffect::init(device) )
		return false;

	if( !_perturbationTex )
	{
		_perturbationTex = ATOM_CreateTextureResource("/textures/rainfall.dds",ATOM_PIXEL_FORMAT_UNKNOWN,1/*ATOM_LoadPriority_IMMEDIATE*/);
		if(!_perturbationTex )
			return false;
	}

	if (!_material)
	{
		_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/HeatHaze.mat");
		if (!_material)
			return false;
	}

	return true;
}

bool ATOM_HeatHazeEffect::destroy()
{
	_perturbationTex = 0;
	_material = 0;

	return ATOM_PostEffect::destroy();
}

bool ATOM_HeatHazeEffect::render (ATOM_RenderDevice *device)
{
	if( !init(device) )
		return false;

	_material->setActiveEffect ("default");

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
	
	device->setRenderTarget (0, getRenderTarget());
	device->setViewport (0, ATOM_Rect2Di(0, 0, getRenderTarget()->getWidth(), getRenderTarget()->getHeight()));
	
	ATOM_Vector4f invScreenSize(1.0f / ATOM_RenderScheme::getCurrentRenderScheme()->getWidth (),
								1.0f / ATOM_RenderScheme::getCurrentRenderScheme()->getHeight(),
								0,
								0 );

	ATOM_Vector4f perturbFactor(_perturbScale,
								1.0f / _perturbDistance,
								_perturbSpeed,
								_perturbTiling );

	_material->getParameterTable()->setVector("invScreenSize",invScreenSize);
	_material->getParameterTable()->setVector("perturbScale",perturbFactor);

	_material->getParameterTable()->setTexture ("inputTexture", inputTexture.get());
	_material->getParameterTable()->setTexture ("perturbTexture", _perturbationTex.get());

	drawTexturedFullscreenQuad (	device, 
									_material.get(), 
									inputTexture->getWidth(), 
									inputTexture->getHeight()	);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////