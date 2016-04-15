#include "StdAfx.h"
#include "postEffectColorFilter.h"


///////////////////////////////////////////////////////////////////////
ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_ColorFilterEffect)
	ATOM_ATTRIBUTES_BEGIN(ATOM_ColorFilterEffect)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ColorFilterEffect, "FilterColor", getFilterColor, setFilterColor, ATOM_Vector3f(1.0f,0.0f,0.0f), "group=ATOM_ColorFilterEffect;type=vfilename;desc='过滤颜色'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ColorFilterEffect, "Density", getDensity, setDensity, 0.0f, "group=ATOM_ColorFilterEffect;desc='颜色强度'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ColorFilterEffect, "Speed", getSpeed, setSpeed, 1.0f, "group=ATOM_ColorFilterEffect;desc='渐变速率'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ColorFilterEffect, "maskTexture", getMaskTextureFileName, setMaskTextureFileName, "", "group=ATOM_ColorFilterEffect;type=vfilename;desc='遮罩图'")
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_ColorFilterEffect, ATOM_PostEffect)


ATOM_ColorFilterEffect::ATOM_ColorFilterEffect (void)
{
	_filterColor = ATOM_Vector3f(1.0f,1.0f,1.0f);
	_density = 0.0f;
	_speed = 1.0f;

	_direction = 0.0f;
	_currDensity = 0.0f;

	_material = 0;

	_maskTexFileName = "";
	_maskTexture = 0;

	_gammaFlag = AFTER_GAMMA_CORRECT;

	_enabled = false;
}

ATOM_ColorFilterEffect::~ATOM_ColorFilterEffect (void)
{
	
}

int ATOM_ColorFilterEffect::isEnabled (void) const
{
	return _enabled;
}

void ATOM_ColorFilterEffect::setFilterColor(const ATOM_Vector3f & filterColor)
{
	if( _filterColor != filterColor )
	{
		_filterColor = filterColor;

		_filterColor.x = ATOM_saturate( _filterColor.x );
		_filterColor.y = ATOM_saturate( _filterColor.y );
		_filterColor.z = ATOM_saturate( _filterColor.z );
	}
}
const ATOM_Vector3f & ATOM_ColorFilterEffect::getFilterColor(void) const
{
	return _filterColor;
}
void ATOM_ColorFilterEffect::setDensity(const float density)
{
	if( density != _density )
	{
		_density = ATOM_min2( density, 1.0f );
		_density = ATOM_max2( _density, 0.0f );
		_direction = _density > _currDensity ?	1.0f : 
												( _density < _currDensity ) ? - 1.0f : 0.0f;

		_currTime = ATOM_APP->getFrameStamp().currentTick * 0.001f;

		_enabled = true;
	}
}
const float ATOM_ColorFilterEffect::getDensity(void) const
{
	return _density;
}

#if 0 
void ATOM_ColorFilterEffect::setSpeed(const float speed)
{
	_speed = speed;
}
#else
void ATOM_ColorFilterEffect::setSpeed(const float time)
{
	_speed = 1.0f / time;
}
#endif
const float ATOM_ColorFilterEffect::getSpeed(void) const
{
	return _speed;
}

// 获得当前的强度
const float ATOM_ColorFilterEffect::getCurrentDensity(void) const
{
	return _currDensity;
}

void ATOM_ColorFilterEffect::setMaskTextureFileName (const ATOM_STRING &filename)
{
	ATOM_STACK_TRACE(ATOM_ColorFilterEffect::setTextureFileName);

	if ( !filename.empty () && _maskTexFileName != filename )
	{
		_maskTexFileName = filename;

		ATOM_RenderDevice *device = ATOM_GetRenderDevice();

		ATOM_AUTOREF(ATOM_Texture) newTexture = ATOM_CreateTextureResource (_maskTexFileName.c_str(), 
																			ATOM_PIXEL_FORMAT_UNKNOWN, 
																			1/*ATOM_LoadPriority_IMMEDIATE*/	);

		if ( newTexture != _maskTexture )
		{
			_maskTexture = newTexture;
			_material = 0;
		}
	}
	else if ( filename.empty () && _maskTexture)
	{
		_maskTexture = 0;
		_material = 0;
	}
}

const ATOM_STRING & ATOM_ColorFilterEffect::getMaskTextureFileName (void) const
{
	return _maskTexFileName;
}

// 直接设置
void ATOM_ColorFilterEffect::setMaskTexture(ATOM_Texture* maskTexture)
{
	if( maskTexture && _maskTexture != maskTexture )
		_maskTexture = maskTexture;
}

bool ATOM_ColorFilterEffect::init(ATOM_RenderDevice *device)
{
	if( !ATOM_PostEffect::init(device) )
		return false;

	if (!_material)
	{
		_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/colorFilter.mat");
		if (!_material)
			return false;
	}

	if(!_maskTexture)
	{
		_maskTexture = ATOM_CreateTextureResource (	_maskTexFileName.c_str(), 
													ATOM_PIXEL_FORMAT_UNKNOWN, 
													1/*ATOM_LoadPriority_IMMEDIATE*/	);
		if (!_maskTexture)
			return false;
	}

	return true;
}


bool ATOM_ColorFilterEffect::render (ATOM_RenderDevice *device)
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

	// update the density
	if( _currDensity != _density )
	{

#if 0
		_currDensity += ( ATOM_APP->getFrameStamp().elapsedTick * 0.001f ) * _speed * _direction;
#else
		float currentTime = ATOM_APP->getFrameStamp().currentTick * 0.001f;
		_currDensity += ( currentTime - _currTime ) * _speed * _direction;
		_currTime = currentTime;
#endif

		if( _direction > 0 )
		{
			if( _currDensity > _density )
			{
				_currDensity = _density;
				if( _currDensity == 0.0f )
					_enabled = false;
			}
		}
		else if( _direction < 0 )
		{
			if( _currDensity < _density )
			{
				_currDensity = _density;
				if( _currDensity == 0.0f )
					_enabled = false;
			}
		}
		else
		{
			_currDensity = _density;
			if( _currDensity == 0.0f )
				_enabled = false;
		}

		
	}
	
	ATOM_Vector4f params( _currDensity == 0.0f ? ATOM_Vector3f(0.0f) : _filterColor );
	params.w = _currDensity;
	_material->getParameterTable()->setVector("filterColorParams", params );

	_material->getParameterTable()->setTexture ("inputTexture", inputTexture.get());

	_material->getParameterTable()->setTexture ("maskTexture",
												(_maskTexture && _maskTexture->getAsyncLoader()->IsLoadAllFinished() ) ? _maskTexture.get() : ATOM_GetColorTexture(0X00FFFFFF) );

	drawTexturedFullscreenQuad (device, 
								_material.get(), 
								inputTexture->getWidth(), 
								inputTexture->getHeight());

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////