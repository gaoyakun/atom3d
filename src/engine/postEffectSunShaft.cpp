#include "StdAfx.h"
#include "postEffectSunShaft.h"


///////////////////////////////////////////////////////////////////////
ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_SunShaftEffect)
	ATOM_ATTRIBUTES_BEGIN(ATOM_SunShaftEffect)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_SunShaftEffect, "RayAttenuation", getRayAttenuation, setRayAttenuation, 0.0f, "group=ATOM_SunShaftEffect;desc='射线衰减'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_SunShaftEffect, "ShaftAmount", getShaftAmount, setShaftAmount, 1.0f, "group=ATOM_SunShaftEffect;desc='shaft强度'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_SunShaftEffect, "RayAmount", getRayAmount, setRayAmount, 1.0f, "group=ATOM_SunShaftEffect;desc='ray强度'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_SunShaftEffect, "CustomColorInfluence", getCustomColorInfluence, setCustomColorInfluence, 0.0f, "group=ATOM_SunShaftEffect;desc='自定义颜色影响'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_SunShaftEffect, "CustomColor", getCustomRayColor, setCustomRayColor, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), "type=rgba;group=ATOM_SunShaftEffect;desc='自定义颜色'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_SunShaftEffect, "CustomLS", getCustomLightSource, setCustomLightSource, ATOM_Vector3f(0.f, 0.f, 0.f), "type=dir;group=ATOM_SunShaftEffect;desc='自定义光源'")
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_SunShaftEffect, ATOM_PostEffect)


ATOM_SunShaftEffect::ATOM_SunShaftEffect (void)
{
	_rayAttenuation = 0.0f;
	_shaftAmount = 1.0f;
	_rayAmount = 1.0f;
	_customColorInfluence = 0.0f;
	_customRayColor = ATOM_Vector4f(1,1,1,1);
	_customLS = ATOM_Vector3f(0.f, 0.f, 0.f);

	_material = 0;

	_maskTex = 0;
	_shaftTex = 0;

	_gammaFlag = BEFORE_GAMMA_CORRECT;
}

ATOM_SunShaftEffect::~ATOM_SunShaftEffect (void)
{
	destroy();
}

void ATOM_SunShaftEffect::enable (int b)
{
	ATOM_PostEffect::enable(b);
	ATOM_RenderSettings::enableSunShaft( b==0 ? false:true );
}
int ATOM_SunShaftEffect::isEnabled (void) const
{
	if(!ATOM_RenderSettings::isSunShaftEnabled())
		return 0;
	return ATOM_PostEffect::isEnabled();
}


//////////////////////////////////////////////////////////////////////////////////////////
void ATOM_SunShaftEffect::setRayAttenuation(const float attenuation)
{
	_rayAttenuation = attenuation;
}
const float ATOM_SunShaftEffect::getRayAttenuation(void) const
{
	return _rayAttenuation;
}
void ATOM_SunShaftEffect::setShaftAmount(const float amount)
{
	_shaftAmount = amount;
}
const float ATOM_SunShaftEffect::getShaftAmount(void) const
{
	return _shaftAmount;
}
void ATOM_SunShaftEffect::setRayAmount(const float amount)
{
	_rayAmount = amount;
}
const float ATOM_SunShaftEffect::getRayAmount(void) const
{
	return _rayAmount;
}
void ATOM_SunShaftEffect::setCustomColorInfluence(const float customInflence)
{
	_customColorInfluence = customInflence;
	ATOM_saturate(_customColorInfluence);
}
const float ATOM_SunShaftEffect::getCustomColorInfluence(void) const
{
	return _customColorInfluence;
}
void ATOM_SunShaftEffect::setCustomRayColor (const ATOM_Vector4f & color)
{
	_customRayColor = color;
}
const ATOM_Vector4f & ATOM_SunShaftEffect::getCustomRayColor (void) const
{
	return _customRayColor;
}
void ATOM_SunShaftEffect::setCustomLightSource(const ATOM_Vector3f customLS)
{
	_customLS = customLS;
}
const ATOM_Vector3f & ATOM_SunShaftEffect::getCustomLightSource(void) const
{
	return _customLS;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool ATOM_SunShaftEffect::init(ATOM_RenderDevice *device)
{
	if( !ATOM_PostEffect::init(device) )
		return false;

	if (!_material)
	{
		_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/pp_sunshaft.mat");
		if (!_material)
			return false;
	}

	ATOM_AUTOREF(ATOM_Texture) inputTexture = getSourceInputTex();
	int rtWidth = inputTexture->getWidth();
	int rtHeight = inputTexture->getHeight();

	int id = 0;

	if( !_maskTex )
	{
		// wangjian modified : 使用ATOM_PostEffectRTMgr创建全局的RenderTarget
#if 0
		_maskTex = device->allocTexture ( 0, 0, rtWidth / 4, rtHeight / 4, ATOM_PIXEL_FORMAT_RGBA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
#else
		_maskTex = ATOM_PostEffectRTMgr::getRT ( rtWidth / 4, rtHeight / 4, ATOM_PIXEL_FORMAT_RGBA8888, id );
		id++;
#endif
		if( !_maskTex )
			return false;
	}

	

	if( !_shaftTex )
	{
		// wangjian modified : 使用ATOM_PostEffectRTMgr创建全局的RenderTarget
#if 0
		_shaftTex = device->allocTexture ( 0, 0, rtWidth / 4, rtHeight / 4, ATOM_PIXEL_FORMAT_RGBA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
#else
		_shaftTex = ATOM_PostEffectRTMgr::getRT ( rtWidth / 4, rtHeight / 4, ATOM_PIXEL_FORMAT_RGBA8888, id );
#endif
		if( !_shaftTex )
			return false;
	}

	return true;
}

bool ATOM_SunShaftEffect::destroy()
{
	_maskTex = 0;
	_shaftTex = 0;
	_material = 0;

	return ATOM_PostEffect::destroy();
}

bool ATOM_SunShaftEffect::render (ATOM_RenderDevice *device)
{
	if( !init(device) )
		return false;

	genShaftMask(device);
	genSunShaft(device);
	displayShaft(device);

	return true;
}

void ATOM_SunShaftEffect::genShaftMask(ATOM_RenderDevice *device)
{
	ATOM_Scene* currentScene = ATOM_RenderScheme::getCurrentScene ();
	if( !currentScene )
		return;

	ATOM_Camera * camera = currentScene->getCamera();
	if( !camera )
		return;

	_material->setActiveEffect ("genShaftMask");

	device->setRenderTarget ( 0, _maskTex.get() );
	device->setViewport (0, ATOM_Rect2Di(0, 0, _maskTex->getWidth(), _maskTex->getHeight()));

	ATOM_Vector4f screenSize(	ATOM_RenderScheme::getCurrentRenderScheme()->getWidth (),
								ATOM_RenderScheme::getCurrentRenderScheme()->getHeight(),
								1.0f / ATOM_RenderScheme::getCurrentRenderScheme()->getWidth (),
								1.0f / ATOM_RenderScheme::getCurrentRenderScheme()->getHeight() );

	_material->getParameterTable()->setVector("screenSize",screenSize);

	float nearPlane = camera->getNearPlane();
	float farPlane = camera->getFarPlane();
	ATOM_Vector4f nearFarFactor( nearPlane, farPlane, 1.0f / (farPlane-nearPlane), 0 );
	_material->getParameterTable()->setVector ( "cameraNearFarFactor", nearFarFactor );

	drawTexturedFullscreenQuad (	device,
									_material.get(),
									_maskTex->getWidth(),
									_maskTex->getHeight()	);

	if( 0 )
	{
		device->setRenderTarget ( 0, 0 );
		_maskTex->saveToFile("/sunshaft_mask.dds");
	}
}
void ATOM_SunShaftEffect::genSunShaft(ATOM_RenderDevice *device)
{
	_material->setActiveEffect ("sunShaftsGen");

	ATOM_Vector4f screenSize(	ATOM_RenderScheme::getCurrentRenderScheme()->getWidth (),
								ATOM_RenderScheme::getCurrentRenderScheme()->getHeight(),
								1.0f / ATOM_RenderScheme::getCurrentRenderScheme()->getWidth (),
								1.0f / ATOM_RenderScheme::getCurrentRenderScheme()->getHeight() );

	ATOM_Vector4f sunPosScreen(0,0,0,0);
	ATOM_Vector3f sunDirNormalized;

	if( !_customLS.almostEqual(ATOM_Vector3f(0)) )
	{
		sunDirNormalized.normalizeFrom( -_customLS );
	}
	else
	{
		ATOM_Scene* currentScene = ATOM_RenderScheme::getCurrentScene ();
		ATOM_Sky * sky = 0;
		if( currentScene && ( sky = currentScene->getSkyNode() ) )
		{
			sunDirNormalized.normalizeFrom( -sky->getLightDir() );
		}
		else
		{
			ATOM_LOGGER::error("%s : can't find the scene sky node!",__FUNCTION__);
			return;
		}
	}

	ATOM_Vector4f pParamSunPos= ATOM_Vector4f(	sunDirNormalized.x * 5000.0f, 
												sunDirNormalized.y * 5000.0f, 
												sunDirNormalized.z * 5000.0f, 
												1.0f	);

	const ATOM_Matrix4x4f & matViewProj = ATOM_RenderScheme::getCurrentScene()->getCamera ()->getViewProjectionMatrix();

	// no need to waste gpu to compute sun screen pos
	sunPosScreen = pParamSunPos << matViewProj;
	sunPosScreen.x = ( ( sunPosScreen.x + sunPosScreen.w) * 0.5f ) / (1e-6f + sunPosScreen.w);   
	sunPosScreen.y = ( (-sunPosScreen.y + sunPosScreen.w) * 0.5f ) / (1e-6f + sunPosScreen.w);
	/*if( sunPosScreen.x < 0.01 )
		sunPosScreen.x = 0.01;
	if( sunPosScreen.x > 0.99 )
		sunPosScreen.x = 0.99;
	if( sunPosScreen.y < 0.01 )
		sunPosScreen.y = 0.01;
	if( sunPosScreen.y > 0.99 )
		sunPosScreen.y = 0.99;*/

	ATOM_Vector3f pEye, pAt, pUp;
	ATOM_RenderScheme::getCurrentScene()->getCamera ()->getCameraPos( &pEye, &pAt, &pUp);
	ATOM_Vector3f eyedir = pAt - pEye;
	eyedir.normalize();
	sunPosScreen.w = dotProduct(sunDirNormalized,eyedir);
	

	ATOM_Vector4f params(0,0,0,0);

	/////////////////////////////////////////////////////////////////////////////////////////////
	// pass 1 :  8 samples
	device->setRenderTarget ( 0, _shaftTex.get() );
	device->setViewport (0, ATOM_Rect2Di(0, 0, _shaftTex->getWidth(), _shaftTex->getHeight()));

	_material->getParameterTable()->setVector("screenSize",screenSize);
	_material->getParameterTable()->setVector("sunPosition",sunPosScreen);
	
	// big radius, project until end of screen
	params.x = 0.1f;
	params.y = _rayAttenuation;
	_material->getParameterTable()->setVector("sunShaftParams",params);

	_material->getParameterTable()->setTexture("shaftMaskTexture",_maskTex.get());

	drawTexturedFullscreenQuad (	device,
									_material.get(), 
									_shaftTex->getWidth(), 
									_shaftTex->getHeight()	);

	if( 0 )
	{
		device->setRenderTarget ( 0, 0 );
		_shaftTex->saveToFile("/sunshaft1.dds");
	}

	//////////////////////////////////////////////////////////////////////////////
	// pass 2 :  64 samples
	device->setRenderTarget ( 0, _maskTex.get() );
	device->setViewport (0, ATOM_Rect2Di(0, 0, _maskTex->getWidth(), _maskTex->getHeight()));

	_material->getParameterTable()->setVector("screenSize",screenSize);
	_material->getParameterTable()->setVector("sunPosition",sunPosScreen);

	// big radius, project until end of screen
	params.x = 0.025f;
	_material->getParameterTable()->setVector("sunShaftParams",params);

	_material->getParameterTable()->setTexture("shaftMaskTexture",_shaftTex.get());

	drawTexturedFullscreenQuad (	device,
									_material.get(), 
									_maskTex->getWidth(), 
									_maskTex->getHeight()	);

	if( 0 )
	{
		device->setRenderTarget ( 0, 0 );
		_maskTex->saveToFile("/sunshaft2.dds");
	}

	//////////////////////////////////////////////////////////////////////////////
	// pass 2 :  512 samples
	device->setRenderTarget ( 0, _shaftTex.get() );
	device->setViewport (0, ATOM_Rect2Di(0, 0, _shaftTex->getWidth(), _shaftTex->getHeight()));

	_material->getParameterTable()->setVector("screenSize",screenSize);
	_material->getParameterTable()->setVector("sunPosition",sunPosScreen);

	// big radius, project until end of screen
	params.x = 0.025f;
	_material->getParameterTable()->setVector("sunShaftParams",params);

	_material->getParameterTable()->setTexture("shaftMaskTexture",_maskTex.get());

	drawTexturedFullscreenQuad (	device,
									_material.get(), 
									_shaftTex->getWidth(), 
									_shaftTex->getHeight()	);

	if( 0 )
	{
		device->setRenderTarget ( 0, 0 );
		_shaftTex->saveToFile("/sunshaft3.dds");
	}
}
void ATOM_SunShaftEffect::displayShaft(ATOM_RenderDevice *device)
{
	_material->setActiveEffect ("default");

	ATOM_Scene* currentScene = ATOM_RenderScheme::getCurrentScene ();
	ATOM_Sky * sky = 0;
	if( currentScene && ( sky = currentScene->getSkyNode() ) )
	{
		ATOM_Vector4f sunColor( sky->getLightColor() );
		sunColor = sunColor*(1.0-_customColorInfluence) + _customRayColor * (_customColorInfluence);
		_material->getParameterTable()->setVector("sunColor",sunColor);

		ATOM_Vector4f shaftParams(0,0,0,0);
		const float fSunVisThreshold = 0;
		ATOM_Vector3f pEye, pAt, pUp;
		ATOM_RenderScheme::getCurrentScene()->getCamera ()->getCameraPos( &pEye, &pAt, &pUp);
		ATOM_Vector3f eyedir = pAt - pEye;
		eyedir.normalize();

		ATOM_Vector3f sunDirNormalized;

		if( !_customLS.almostEqual(ATOM_Vector3f(0)) )
		{
			sunDirNormalized.normalizeFrom( _customLS );
		}
		else
		{
			sunDirNormalized.normalizeFrom( sky->getLightDir() );
		}
		
		float fLdotV = ATOM_saturate( -( dotProduct( sunDirNormalized, eyedir ) - fSunVisThreshold ) * 4.0f ); 
		shaftParams.x = ATOM_saturate( _shaftAmount * fLdotV );
		shaftParams.y = _rayAmount;
		_material->getParameterTable()->setVector("sunShaftParams",shaftParams);
	}
	else
	{
		ATOM_LOGGER::error("%s : can't find the scene sky node!",__FUNCTION__);
		return;
	}
	
	ATOM_AUTOREF(ATOM_Texture) inputTexture = getSourceInputTex();
	_material->getParameterTable()->setTexture ("inputTexture",  inputTexture.get());
	_material->getParameterTable()->setTexture ("shaftTexture",  _shaftTex.get());

	device->setRenderTarget (0, getRenderTarget());
	device->setViewport (0, ATOM_Rect2Di(0, 0, getRenderTarget()->getWidth(), getRenderTarget()->getHeight()));

	drawTexturedFullscreenQuad (device, _material.get(), getRenderTarget()->getWidth(), getRenderTarget()->getHeight());
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////