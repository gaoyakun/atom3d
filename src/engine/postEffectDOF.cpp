#include "StdAfx.h"
#include "postEffectDOF.h"

#define USE_OPTIMIZATION 1		// 优化版本： 去除dof的模糊MASK RT纹理

/////////////////////////////////////////////////////////////////////////
ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_DOFEffect)
	ATOM_ATTRIBUTES_BEGIN(ATOM_DOFEffect)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_DOFEffect, "FocusDist", getFocusDist, setFocusDist, 60.0f, "group=ATOM_DOFEffect;step='0.01f';desc='焦距'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_DOFEffect, "FocusRange", getFocusRange, setFocusRange, 50.0f, "group=ATOM_DOFEffect;step='0.01f';desc='聚焦范围'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_DOFEffect, "FocusPower", getFocusPower, setFocusPower, 0.8f, "group=ATOM_DOFEffect;desc='聚焦Power'")
	/*ATOM_ATTRIBUTE_PERSISTENT(ATOM_DOFEffect, "FocusZoneNear", getFocusZoneNear, setFocusZoneNear, 20.0f, "group=ATOM_DOFEffect;desc='聚焦区近平面'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_DOFEffect, "FocusZoneFar", getFocusZoneFar, setFocusZoneFar, 20.0f, "group=ATOM_DOFEffect;desc='聚焦区远平面'")*/
	//ATOM_ATTRIBUTE_PERSISTENT(ATOM_DOFEffect, "MinZ", getMinZ, setMinZ, 1000.0f, "group=ATOM_DOFEffect;desc='最小距离'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_DOFEffect, "MinZScale", getMinZScale, setMinZScale, 0.0f, "group=ATOM_DOFEffect;desc='近距模糊缩放'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_DOFEffect, "MinZBlendMult", getMinZBlendMult, setMinZBlendMult, 0.0f, "group=ATOM_DOFEffect;desc='近距模糊强度'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_DOFEffect, "BlurCoeff", getBlurCoeff, setBlurCoeff, 1.0f, "group=ATOM_DOFEffect;step='0.01f';desc='模糊强度'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_DOFEffect, "BokehAngle", getBokehAngle, setBokehAngle, 30.0f, "group=ATOM_DOFEffect;desc='Bokeh角度'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_DOFEffect, "BokehRaidus", getBokehRadius, setBokehRadius, 3.0f, "group=ATOM_DOFEffect;desc='Bokeh半径'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_DOFEffect, "Brightness", getBrightness, setBrightness, 1.0f, "group=ATOM_DOFEffect;desc='Bokeh亮度'")
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_DOFEffect, ATOM_PostEffect)

ATOM_DOFEffect::ATOM_DOFEffect (void)
{
	_rtWidth = 0;
	_rtHeight = 0;

	_focusDistance = 60.0f;
	_focusRange = 50.0f;
	_focusPower = 0.8f;

	_dofMinz = 100.0f;
	_dofMinzScale = 0.0f;
	_dofMinZBlendMult = 0.0f;

	_blurCoefficient = 2.0f;
	_pixelPerMeter = 10.0f;

	_bokeh = true;
	_bokehAngle = 30.0f;
	_radius = 3.0f;
	_brightness = 1.0f;

	_materialError = false;
	_bsaveall = false;

	_material = 0;

	_gammaFlag = BEFORE_GAMMA_CORRECT;

	/*memset( avSampleOffsets_horizontal,0,sizeof(ATOM_Vector2f) * _GAUSSIAN_TAP_COUNT_FINAL );
	memset( avSampleWeights_horizontal,0,sizeof(ATOM_Vector4f) * _GAUSSIAN_TAP_COUNT_FINAL );
	memset( avSampleOffsets_vertical,0,sizeof(ATOM_Vector2f) * _GAUSSIAN_TAP_COUNT_FINAL );
	memset( avSampleWeights_vertical,0,sizeof(ATOM_Vector4f) * _GAUSSIAN_TAP_COUNT_FINAL );*/
}

ATOM_DOFEffect::~ATOM_DOFEffect (void)
{
	
}

bool ATOM_DOFEffect::init(ATOM_RenderDevice *device)
{
	if( !ATOM_PostEffect::init(device) )
		return false;

	// create the material
	if( !createMaterial(device) )
		return false;


	// create the rt
	if( !createRT(device) )
		return false;

	return true;
}

bool ATOM_DOFEffect::destroy()
{
	// reset the bloom rt
	_blurTexs[0] = 0;
	_blurTexs[1] = 0;
	_blurTexs[2] = 0;

	_material = 0;

	// reset the rt size
	_rtWidth = 0;
	_rtHeight = 0;

	_source = 0;

	return ATOM_PostEffect::destroy();
}

void ATOM_DOFEffect::enable (int b)
{
	ATOM_PostEffect::enable(b);
	ATOM_RenderSettings::enableDof( b==0 ? false : true );
}
int ATOM_DOFEffect::isEnabled (void) const
{
	if( !ATOM_RenderSettings::isDofEnabled() )
		return 0;
	return ATOM_PostEffect::isEnabled();
}

// 生成DOF的模糊MASK
void ATOM_DOFEffect::genDofBlurMask (ATOM_RenderDevice *device)
{
	_material->setActiveEffect ("GenDofBlurMask");

	ATOM_Vector4f texel_size(1.0f/_blurTexs[0]->getWidth(),1.0f/_blurTexs[0]->getHeight(),0,0);
	_material->getParameterTable()->setVector("texelSize",texel_size);

	// 焦距参数
	float focus_range_far	= _focusRange * 0.5f;
	float focus_range_near	= -_focusRange * 0.5f;
	ATOM_Vector4f vDofParamsFocus0;
	vDofParamsFocus0.x = 1.0f / (focus_range_far + 1e-6f);
	vDofParamsFocus0.y = -_focusDistance / (focus_range_far + 1e-6f);
	vDofParamsFocus0.z = 1.0f / (focus_range_near + 1e-6f);
	vDofParamsFocus0.w = -_focusDistance / (focus_range_near + 1e-6f);
	_material->getParameterTable()->setVector("vDofParamsFocus0", vDofParamsFocus0);

	ATOM_Vector4f vDofParamsFocus1;
	vDofParamsFocus1.x = _dofMinz;
	vDofParamsFocus1.y = _dofMinzScale;
	vDofParamsFocus1.z = _dofMinZBlendMult;
	vDofParamsFocus1.w = _blurCoefficient;
	_material->getParameterTable()->setVector("vDofParamsFocus1", vDofParamsFocus1);

	_material->getParameterTable()->setFloat("focusPower", _focusPower);

	//_material->getParameterTable()->setTexture ("tex0", _source.get());

#if USE_OPTIMIZATION
	device->setRenderTarget (0, _source.get());					// 优化 使用源RT作为渲染目标，只写ALPHA
	device->setViewport (0, ATOM_Rect2Di(0, 0,_source->getWidth(), _source->getHeight()));
	drawTexturedFullscreenQuad (device, _material.get(), _source->getWidth(), _source->getHeight());
#else
	device->setRenderTarget (0, _dofBlurMaskTex.get());
	device->setViewport (0, ATOM_Rect2Di(0, 0,_dofBlurMaskTex->getWidth(), _dofBlurMaskTex->getHeight()));
	drawTexturedFullscreenQuad (device, _material.get(), _dofBlurMaskTex->getWidth(), _dofBlurMaskTex->getHeight());
#endif

	if( 0 )
	{
		device->setRenderTarget (0, 0);
		_dofBlurMaskTex->saveToFile ("/textures/rt_test/dofblurMask.dds");
	}
}

// 生成缩放场景图 作为模糊的源
void ATOM_DOFEffect::sceneToSceneScaled (ATOM_RenderDevice *device)
{
	stretchrect(device,_source.get(),_blurTexs[1].get(), true );

	if( 0 )
	{
		device->setRenderTarget (0, 0);
		_blurTexs[1]->saveToFile ("/textures/rt_test/sceneScaled.dds");
	}
}

void ATOM_DOFEffect::genBlur(ATOM_RenderDevice *device)
{
	_material->setActiveEffect ("Blur");

	ATOM_Vector4f params( _focusDistance, _blurCoefficient, _pixelPerMeter, 0.0f );
	_material->getParameterTable()->setVector("params",params);

	float _oneoverTexSize_x = 1.0f/_blurTexs[0]->getWidth();
	float _oneoverTexSize_y = 1.0f/_blurTexs[0]->getHeight();

	ATOM_Vector4f texel_offset(0,0,0,0);

#if 1
	for( int i = 0; i < 1; ++i )
	{
		//  horizontal
		texel_offset.x = _oneoverTexSize_x;
		texel_offset.y = 0.0;

		_material->getParameterTable()->setVector("texelOffset",texel_offset);
		_material->getParameterTable()->setTexture ("tex0", _blurTexs[1].get());
		device->setRenderTarget (0, _blurTexs[0].get());
		device->setViewport (0, ATOM_Rect2Di(0, 0,_blurTexs[0]->getWidth(), _blurTexs[0]->getHeight()));
		drawTexturedFullscreenQuad (device, _material.get(), _blurTexs[0]->getWidth(), _blurTexs[0]->getHeight());

		// vertical
		texel_offset.x = 0.0f;
		texel_offset.y = _oneoverTexSize_y;
		_material->getParameterTable()->setVector("texelOffset",texel_offset);
		_material->getParameterTable()->setTexture ("tex0", _blurTexs[0].get());
		device->setRenderTarget (0, _blurTexs[1].get());
		device->setViewport (0, ATOM_Rect2Di(0, 0, _blurTexs[1]->getWidth(), _blurTexs[1]->getHeight()));
		drawTexturedFullscreenQuad (device, _material.get(), _blurTexs[1]->getWidth(), _blurTexs[1]->getHeight());

		if( 0 )
		{
			device->setRenderTarget (0, 0);
			_blurTexs[0]->saveToFile ("/BloomTexture0.png");
			_blurTexs[1]->saveToFile ("/BloomTexture1.png");
		}
	}
#else

	
	texel_offset.x = 0;
	texel_offset.y = _oneoverTexSize_y;

	_material->getParameterTable()->setVector("texelOffset",texel_offset);
	_material->getParameterTable()->setTexture ("tex0", _blurTexs[1].get());
	device->setRenderTarget (0, _blurTexs[0].get());
	device->setViewport (0, ATOM_Rect2Di(0, 0,_blurTexs[0]->getWidth(), _blurTexs[0]->getHeight()));
	drawTexturedFullscreenQuad (device, _material.get(), _blurTexs[0]->getWidth(), _blurTexs[0]->getHeight());

	// vertical
	texel_offset.x = _oneoverTexSize_x * sinf( 3.14159 / 3 );
	texel_offset.y = -_oneoverTexSize_y * cosf( 3.14159 / 3 );
	_material->getParameterTable()->setVector("texelOffset",texel_offset);
	_material->getParameterTable()->setTexture ("tex0", _blurTexs[0].get());
	device->setRenderTarget (0, _blurTexs[1].get());
	device->setViewport (0, ATOM_Rect2Di(0, 0, _blurTexs[1]->getWidth(), _blurTexs[1]->getHeight()));
	drawTexturedFullscreenQuad (device, _material.get(), _blurTexs[1]->getWidth(), _blurTexs[1]->getHeight());


#endif
}

void ATOM_DOFEffect::genBlurBokeh(ATOM_RenderDevice *device)
{
	// Generate
	ATOM_Vector2f dir[3];
	for (int i = 0; i < 3; i++) 
	{
		float a = _bokehAngle / 180 * 3.1415926f + i * 3.1415926f * 2 / 3;
		dir[i] = ATOM_Vector2f(	_radius * ATOM_sin(a) / _blurTexs[0]->getWidth(),
								_radius * ATOM_cos(a) / _blurTexs[0]->getHeight() );
	}

	if( _brightness < -1.0f )
		_brightness = -1.0f;
	if( _brightness > 1.0f )
		_brightness = 1.0f;

	float power = ATOM_pow(10.0f, _brightness);

	/////////////////////////////////////////// pre pass //////////////////////////////////////////////

	_material->setActiveEffect ("Bokeh_prepass");

	_material->getParameterTable()->setFloat("power",power);

	//_material->getParameterTable()->setTexture("dofBlurMaskTex",_dofBlurMaskTex.get());

	_material->getParameterTable()->setTexture ("tex0", _blurTexs[1].get());
	device->setRenderTarget (0, _blurTexs[0].get());
	device->setViewport (0, ATOM_Rect2Di(0, 0,_blurTexs[0]->getWidth(), _blurTexs[0]->getHeight()));
	drawTexturedFullscreenQuad (device, _material.get(), _blurTexs[0]->getWidth(), _blurTexs[0]->getHeight());

	if( 0 )
	{
		device->setRenderTarget (0, 0);
		_blurTexs[0]->saveToFile ("/textures/rt_test/prepass.dds");
	}

	///////////////////////////////////////// bokeh blur pass 0 /////////////////////////////////////////////////////////

	_material->setActiveEffect ("Blur_Bokeh_0");

	ATOM_Vector4f bokehDelta = ATOM_Vector4f(dir[0].x,dir[0].y,dir[2].x,dir[2].y);

	_material->getParameterTable()->setVector("bokehDelta",bokehDelta);

	_material->getParameterTable()->setTexture ("tex0", _blurTexs[0].get());
	device->setRenderTarget (0, _blurTexs[1].get());
	device->setViewport (0, ATOM_Rect2Di(0, 0,_blurTexs[1]->getWidth(), _blurTexs[1]->getHeight()));
	drawTexturedFullscreenQuad (device, _material.get(), _blurTexs[1]->getWidth(), _blurTexs[1]->getHeight());

	if( 0 )
	{
		device->setRenderTarget (0, 0);
		_blurTexs[1]->saveToFile ("/textures/rt_test/Blur_Bokeh_0.dds");
	}

	///////////////////////////////////// bokeh blur pass 1 /////////////////////////////////////////////////////////

	_material->setActiveEffect ("Blur_Bokeh_1");

	bokehDelta.x = dir[1].x;
	bokehDelta.y = dir[1].y;

	_material->getParameterTable()->setVector("bokehDelta",bokehDelta);

	_material->getParameterTable()->setTexture ("tex0", _blurTexs[1].get());
	device->setRenderTarget (0, _blurTexs[2].get());
	device->setViewport (0, ATOM_Rect2Di(0, 0,_blurTexs[2]->getWidth(), _blurTexs[2]->getHeight()));
	drawTexturedFullscreenQuad (device, _material.get(), _blurTexs[2]->getWidth(), _blurTexs[2]->getHeight());

	if( 0 )
	{
		device->setRenderTarget (0, 0);
		_blurTexs[2]->saveToFile ("/textures/rt_test/Blur_Bokeh_1.dds");
	}

	///////////////////////////////////////// bokeh blur pass 0 /////////////////////////////////////////////////////////

	_material->setActiveEffect ("Blur_Bokeh_0");

	_material->getParameterTable()->setVector("bokehDelta",bokehDelta);

	_material->getParameterTable()->setTexture ("tex0", _blurTexs[0].get());
	device->setRenderTarget (0, _blurTexs[1].get());
	device->setViewport (0, ATOM_Rect2Di(0, 0,_blurTexs[1]->getWidth(), _blurTexs[1]->getHeight()));
	drawTexturedFullscreenQuad (device, _material.get(), _blurTexs[1]->getWidth(), _blurTexs[1]->getHeight());

	///////////////////////////////////////// bokeh blur pass 2 /////////////////////////////////////////////////////////

	_material->setActiveEffect ("Blur_Bokeh_2");

	_material->getParameterTable()->setFloat("power",1.0f/power);
	_material->getParameterTable()->setVector("bokehDelta",bokehDelta);

	_material->getParameterTable()->setTexture ("tex0", _blurTexs[1].get());
	_material->getParameterTable()->setTexture ("tex1", _blurTexs[2].get());

	device->setRenderTarget (0, _blurTexs[0].get());
	device->setViewport (0, ATOM_Rect2Di(0, 0,_blurTexs[0]->getWidth(), _blurTexs[0]->getHeight()));
	drawTexturedFullscreenQuad (device, _material.get(), _blurTexs[0]->getWidth(), _blurTexs[0]->getHeight());

	if( 0 )
	{
		device->setRenderTarget (0, 0);
		_blurTexs[0]->saveToFile ("/textures/rt_test/Blur_Bokeh_2.dds");
	}

}

////////////////////////////////////////////////////////////////

void ATOM_DOFEffect::setFocusDist(const float focusDist)
{
	_focusDistance = focusDist;
}
const float ATOM_DOFEffect::getFocusDist(void) const
{
	return _focusDistance;
}
void ATOM_DOFEffect::setBlurCoeff(const float blurCoeff)
{
	_blurCoefficient = blurCoeff;
}
const float ATOM_DOFEffect::getBlurCoeff(void) const
{
	return _blurCoefficient;
}
void ATOM_DOFEffect::setPPM(const float ppm)
{
	_pixelPerMeter = ppm;
}
const float ATOM_DOFEffect::getPPM(void) const
{
	return _pixelPerMeter;
}

void ATOM_DOFEffect::setBokehAngle(const float angle)
{
	_bokehAngle = angle;
}
const float ATOM_DOFEffect::getBokehAngle(void) const
{
	return _bokehAngle;
}

void ATOM_DOFEffect::setBokehRadius(const float radius)
{
	_radius = radius;
}
const float ATOM_DOFEffect::getBokehRadius(void) const
{
	return _radius;
}

void ATOM_DOFEffect::setBrightness(const float brightness)
{
	_brightness = brightness;
}
const float ATOM_DOFEffect::getBrightness(void) const
{
	return _brightness;
}

void ATOM_DOFEffect::setFocusRange(const float range)
{
	_focusRange = range;
}
const float ATOM_DOFEffect::getFocusRange(void) const
{
	return _focusRange;
}

void ATOM_DOFEffect::setFocusPower(const float power)
{
	_focusPower = power;
}
const float ATOM_DOFEffect::getFocusPower(void) const
{
	return _focusPower;
}
//
//void ATOM_DOFEffect::setFocusZoneNear(const float zoneNear)
//{
//	_focusZoneNear = zoneNear;
//}
//const float ATOM_DOFEffect::getFocusZoneNear(void) const
//{
//	return _focusZoneNear;
//}
//
//void ATOM_DOFEffect::setFocusZoneFar(const float zoneFar)
//{
//	_focusZoneFar = zoneFar;
//}
//const float ATOM_DOFEffect::getFocusZoneFar(void) const
//{
//	return _focusZoneFar;
//}

void ATOM_DOFEffect::setMinZ(const float minz)
{
	_dofMinz = minz;
}
const float ATOM_DOFEffect::getMinZ(void) const
{
	return _dofMinz;
}
void ATOM_DOFEffect::setMinZScale(const float minzScale)
{
	_dofMinzScale = minzScale;
}
const float ATOM_DOFEffect::getMinZScale(void) const
{
	return _dofMinzScale;
}
void ATOM_DOFEffect::setMinZBlendMult(const float minzBlendMult)
{
	_dofMinZBlendMult = minzBlendMult;
}
const float ATOM_DOFEffect::getMinZBlendMult(void) const
{
	return _dofMinZBlendMult;
}

////////////////////////////////////////////////////////////////


bool ATOM_DOFEffect::render (ATOM_RenderDevice *device)
{
	// 创建RT 和 材质
	if( !init(device) )
		return false;

	// 开启SRGB写RT
	device->enableSRGBWrite(true);

	////////////////////////////////////////////////////////////////////////////////////////////////

	genDofBlurMask(device);

	sceneToSceneScaled (device);
	
	if( _bokeh )
	{
		genBlurBokeh(device);

		_material->setActiveEffect ("Bokeh");

		//ATOM_Vector4f params( _focusDistance, _blurCoefficient, _pixelPerMeter, 0.0f );
		//_material->getParameterTable()->setVector("params", params);

		ATOM_Vector4f texel_size(1.0f/_blurTexs[0]->getWidth(),1.0f/_blurTexs[0]->getHeight(),0,0);
		_material->getParameterTable()->setVector("texelSize",texel_size);
		
		float focus_range_far	= _focusRange * 0.5f;
		float focus_range_near	= -_focusRange * 0.5f;
		ATOM_Vector4f vDofParamsFocus0;
		vDofParamsFocus0.x = 1.0f / (focus_range_far + 1e-6f);
		vDofParamsFocus0.y = -_focusDistance / (focus_range_far + 1e-6f);
		vDofParamsFocus0.z = 1.0f / (focus_range_near + 1e-6f);
		vDofParamsFocus0.w = -_focusDistance / (focus_range_near + 1e-6f);
		_material->getParameterTable()->setVector("vDofParamsFocus0", vDofParamsFocus0);

		ATOM_Vector4f vDofParamsFocus1;
		vDofParamsFocus1.x = _dofMinz;
		vDofParamsFocus1.y = _dofMinzScale;
		vDofParamsFocus1.z = _dofMinZBlendMult;
		vDofParamsFocus1.w = _blurCoefficient;
		_material->getParameterTable()->setVector("vDofParamsFocus1", vDofParamsFocus1);

		_material->getParameterTable()->setFloat("focusPower", _focusPower);

		/*ATOM_Vector4f vDofParamsFocus2;
		vDofParamsFocus2.x = _focusDistance;
		vDofParamsFocus2.y = _focusDistance;
		vDofParamsFocus2.z = _focusZoneNear;
		vDofParamsFocus2.w = _focusZoneFar;
		_material->getParameterTable()->setVector("vDofParamsFocus2", vDofParamsFocus2);
*/
		_material->getParameterTable()->setTexture ("inputTexture", _source.get());
		_material->getParameterTable()->setTexture ("bluredTexture",  _blurTexs[0].get());

		device->setRenderTarget (0, getRenderTarget());
		device->setViewport (0, ATOM_Rect2Di(0, 0, getRenderTarget()->getWidth(), getRenderTarget()->getHeight()));

		drawTexturedFullscreenQuad (device, _material.get(), _source->getWidth(), _source->getHeight());
	}
	else
	{
		genBlur(device);

		_material->setActiveEffect ("default");

		ATOM_Vector4f params( _focusDistance, _blurCoefficient, _pixelPerMeter, 0.0f );
		_material->getParameterTable()->setVector("params", params);

		_material->getParameterTable()->setTexture ("inputTexture", _source.get());
		_material->getParameterTable()->setTexture ("bluredTexture",  _blurTexs[1].get());

		device->setRenderTarget (0, getRenderTarget());
		device->setViewport (0, ATOM_Rect2Di(0, 0, getRenderTarget()->getWidth(), getRenderTarget()->getHeight()));

		drawTexturedFullscreenQuad (device, _material.get(), _source->getWidth(), _source->getHeight());
	}

	// 关闭SRGB写
	device->enableSRGBWrite(false);

	return true;
}

bool ATOM_DOFEffect::clearTexture (ATOM_Texture *texture)
{
	return texture->clear (0.f, 0.f, 0.f, 0.f);
}

bool ATOM_DOFEffect::createRT(ATOM_RenderDevice *device)
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

	_source = inputTexture;

	// 如果RT未创建，或者RT的大小和当前屏幕大小不一致 重建RT
	if( !_dofBlurMaskTex || !_blurTexs[0] || !_blurTexs[1] || !_blurTexs[2] ||
		_rtWidth != inputTexture->getWidth() || _rtHeight != inputTexture->getHeight() )
	{
		_rtWidth = inputTexture->getWidth();
		_rtHeight = inputTexture->getHeight();

		// wangjian modified : 使用ATOM_PostEffectRTMgr创建全局的RenderTarget
#if 0
		_dofBlurMaskTex = device->allocTexture (0, 0, _rtWidth, _rtHeight, ATOM_PIXEL_FORMAT_RGBA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
		if( !_dofBlurMaskTex )
			return false;
		clearTexture (_dofBlurMaskTex.get());

		for( int i = 0; i < 3; ++i )
		{
			_blurTexs[i] = device->allocTexture (0, 0, _rtWidth/8, _rtHeight/8, ATOM_PIXEL_FORMAT_RGBA16F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
			if( !_blurTexs[i] )
				return false;
			clearTexture (_blurTexs[i].get());
		}
#else

#if !USE_OPTIMIZATION
		_dofBlurMaskTex = ATOM_PostEffectRTMgr::getRT( _rtWidth, _rtHeight, ATOM_PIXEL_FORMAT_RGBA8888, 0 );
		if( !_dofBlurMaskTex )
			return false;
		clearTexture (_dofBlurMaskTex.get());
#endif

		for( int i = 0; i < 3; ++i )
		{
			_blurTexs[i] = ATOM_PostEffectRTMgr::getRT( _rtWidth/8, _rtHeight/8, ATOM_PIXEL_FORMAT_RGBA32F, i );
			if( !_blurTexs[i] )
				return false;
			clearTexture (_blurTexs[i].get());
		}
#endif
	
	}

	return true;
}

bool ATOM_DOFEffect::createMaterial(ATOM_RenderDevice *device)
{
	if (!_material)
	{
		if (_materialError)
		{
			return false;
		}

		_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/pp_dof.mat");
		if (!_material)
		{
			_materialError = true;
			return false;
		}
	}

	return true;
}

void ATOM_DOFEffect::stretchrect (ATOM_RenderDevice *device,ATOM_Texture *pSrc,ATOM_Texture *pDst,bool bigDownSample)
{
	if(!pSrc || !pDst) 
	{
		return;
	}

#if 1

	bool bResample=0;

	if(pSrc->getWidth()!=pDst->getWidth() && pSrc->getHeight()!=pDst->getHeight())
	{
		bResample = 1;
	}

	device->setRenderTarget (0, pDst);
	device->setViewport (0, ATOM_Rect2Di(0, 0, pDst->getWidth(), pDst->getHeight()));

	if(!bResample)
	{
		_material->setActiveEffect ("textureToTexture");
	}
	else
	{ 
		_material->setActiveEffect ("textureToTextureResampled");
	}

	// Get sample size ratio (based on empirical "best look" approach)
	float fSampleSize = ((float)pSrc->getWidth()/(float)pDst->getWidth()) * 0.5f;

	// Set samples position
	//float s1 = fSampleSize / (float) pSrc->GetWidth();  // 2.0 better results on lower res images resizing        
	//float t1 = fSampleSize / (float) pSrc->GetHeight();       

	ATOM_Texture *pOffsetTex = bigDownSample ? pDst : pSrc;

	float s1 = 0.5f / (float) pOffsetTex->getWidth();  // 2.0 better results on lower res images resizing        
	float t1 = 0.5f / (float) pOffsetTex->getHeight();       

	ATOM_Vector4f pParams0, pParams1;

	if (bigDownSample)
	{
		// Use rotated grid + middle sample (~quincunx)
		pParams0=ATOM_Vector4f(s1*0.96f, t1*0.25f, -s1*0.25f, t1*0.96f); 
		pParams1=ATOM_Vector4f(-s1*0.96f, -t1*0.25f, s1*0.25f, -t1*0.96f);  
	}
	else
	{
		// Use box filtering (faster - can skip bilinear filtering, only 4 taps)
		pParams0=ATOM_Vector4f(-s1, -t1, s1, -t1); 
		pParams1=ATOM_Vector4f(s1, t1, -s1, t1);  
	}

#else

	device->setRenderTarget (0, pDst);
	device->setViewport (0, ATOM_Rect2Di(0, 0, pDst->getWidth(), pDst->getHeight()));
	
	_material->setActiveEffect ("textureToTextureResampled");

	ATOM_Texture *pOffsetTex = pSrc;

	float s1 = 1.5f / (float) pOffsetTex->getWidth();  // 2.0 better results on lower res images resizing        
	float t1 = 1.5f / (float) pOffsetTex->getHeight();       

	ATOM_Vector4f pParams0, pParams1;

	// Use box filtering (faster - can skip bilinear filtering, only 4 taps)
	pParams0=ATOM_Vector4f(s1, t1, -s1, t1); 
	pParams1=ATOM_Vector4f(-s1, -t1, s1, -t1);

#endif

	_material->getParameterTable()->setVector ("texToTexParams0", pParams0);
	_material->getParameterTable()->setVector ("texToTexParams1", pParams1);

#if USE_OPTIMIZATION
	_material->getParameterTable()->setTexture ("tex0", pSrc);	// _source作为源RT，其A通道保存的是DOF的模糊mask
#else
	_material->getParameterTable()->setTexture ("dofBlurMaskTex", _dofBlurMaskTex.get());
	_material->getParameterTable()->setTexture ("tex0", pSrc);
#endif
	

	drawTexturedFullscreenQuad (device, _material.get(), pDst->getWidth(), pDst->getHeight());
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////