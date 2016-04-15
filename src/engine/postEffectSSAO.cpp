#include "StdAfx.h"
#include "postEffectSSAO.h"

///////////////////////////////////////////////////////////////////////
ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_SSAOEffect)
	ATOM_ATTRIBUTES_BEGIN(ATOM_SSAOEffect)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_SSAOEffect, "AoAmount", getAoAmount, setAoAmount, 50.0f, "group=ATOM_SSAOEffect;desc='AO强度'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_SSAOEffect, "AoRadius", getAoRadius, setAoRadius, 10.0f, "group=ATOM_SSAOEffect;desc='AO半径'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_SSAOEffect, "AoBias", getAoBias, setAoBias, 0.45f, "group=ATOM_SSAOEffect;desc='法线角度偏移'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_SSAOEffect, "mipLogMaxOffset", getMipLogMaxOffset, setMipLogMaxOffset, 3, "group=ATOM_SSAOEffect;desc='Mip偏移'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_SSAOEffect, "AoBlur", getAoBlur, setAoBlur, 4, "group=ATOM_SSAOEffect;desc='开启模糊'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_SSAOEffect, "UseDownScaledBlur", getUseDownScaledBlur, setUseDownScaledBlur, 1, "group=ATOM_SSAOEffect;desc='使用DS模糊'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_SSAOEffect, "DistanceThreshold", getDistanceThreshold, setDistanceThreshold, 4.0f, "group=ATOM_SSAOEffect;desc='距离阈值'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_SSAOEffect, "ProjScale", getProjScale, setProjScale, 500.0f, "group=ATOM_SSAOEffect;desc='投影缩放'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_SSAOEffect, "BilateralDepthThresh", getBilateralDepthThresh, setBilateralDepthThresh, 8000.0f, "group=ATOM_SSAOEffect;desc='深度阈值'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_SSAOEffect, "AoRadiusParam", getRadiusParam, setRadiusParam, ATOM_Vector4f(1.0f,0.1f,100.0f,500.0f), "group=ATOM_SSAOEffect;desc='半径参数'")
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_SSAOEffect, ATOM_PostEffect)


ATOM_SSAOEffect::ATOM_SSAOEffect (void)
{
	_aoAmount			= 50.0f;
	_aoRadius			= 10.0f;
	_aoBias				= 0.45f;			/** Bias to avoid AO in smooth corners, e.g., 0.01m */
	_distanceThreshold	= 4.0f;
	_aoblur				= 4;
	_projScale			= 500.0f;
	_bilateralDepthThresh = 8000.0f;
	_aoRadiusParam = ATOM_Vector4f(1.0f,0.1f,100.0f,500.0f);

	_useDownScaledBlur = 1;			// 默认不使用DOWNSCALED模糊
	_recreateRT	= false;			


	_mipLogMaxOffset = 3;

	_randomRotateTex = 0;
	_material = 0;
	_materialError = false;

	_downScaledDepth = 0;
	_AORawBuffer = 0;
	_AOBuffer[0] = 0;
	_AOBuffer[1] = 0;

	_rtWidth = 0;
	_rtHeight = 0;
	memset( avSampleOffsets_horizontal,0,sizeof(ATOM_Vector2f) * _GAUSSIAN_TAP_COUNT_FINAL );
	memset( avSampleWeights_horizontal,0,sizeof(ATOM_Vector4f) * _GAUSSIAN_TAP_COUNT_FINAL );
	memset( avSampleOffsets_vertical,0,sizeof(ATOM_Vector2f) * _GAUSSIAN_TAP_COUNT_FINAL );
	memset( avSampleWeights_vertical,0,sizeof(ATOM_Vector4f) * _GAUSSIAN_TAP_COUNT_FINAL );

	_gammaFlag = AFTER_GAMMA_CORRECT;

	_enabled = false;
}

ATOM_SSAOEffect::~ATOM_SSAOEffect (void)
{
	destroy();
}

void ATOM_SSAOEffect::setAoAmount(const float amount)
{
	_aoAmount = amount;
}
const float ATOM_SSAOEffect::getAoAmount(void) const
{
	return _aoAmount;
}
void ATOM_SSAOEffect::setAoRadius(const float radius)
{
	_aoRadius = radius;
}
const float ATOM_SSAOEffect::getAoRadius(void) const
{
	return _aoRadius;
}

void ATOM_SSAOEffect::setDistanceThreshold(const float bias)
{
	_distanceThreshold = bias;
}
const float ATOM_SSAOEffect::getDistanceThreshold(void) const
{
	return _distanceThreshold;
}
void ATOM_SSAOEffect::setAoBias(const float bias)
{
	_aoBias = bias;
}
const float ATOM_SSAOEffect::getAoBias(void) const
{
	return _aoBias;
}
void ATOM_SSAOEffect::setAoBlur(const int blur)
{
	_aoblur = blur;
}
const int ATOM_SSAOEffect::getAoBlur(void) const
{
	return _aoblur;
}
void ATOM_SSAOEffect::setProjScale(const float scale)
{
	_projScale = scale;
}
const float ATOM_SSAOEffect::getProjScale(void) const
{
	return _projScale;
}
void ATOM_SSAOEffect::setBilateralDepthThresh(const float threshold)
{
	_bilateralDepthThresh = threshold;
}
const float ATOM_SSAOEffect::getBilateralDepthThresh(void) const
{
	return _bilateralDepthThresh;
}
void ATOM_SSAOEffect::setMipLogMaxOffset(const float offset)
{
	_mipLogMaxOffset = offset;
}
const float ATOM_SSAOEffect::getMipLogMaxOffset(void) const
{
	return _mipLogMaxOffset;
}
void ATOM_SSAOEffect::setUseDownScaledBlur(const int downscale)
{
	if( downscale != _useDownScaledBlur )
	{
		_recreateRT = true;
	}
	_useDownScaledBlur = downscale;
}
const int ATOM_SSAOEffect::getUseDownScaledBlur(void) const
{
	return _useDownScaledBlur;
}
void ATOM_SSAOEffect::setRadiusParam(const ATOM_Vector4f & aoRadiusParam)
{
	_aoRadiusParam = aoRadiusParam;
}
const ATOM_Vector4f & ATOM_SSAOEffect::getRadiusParam(void) const
{
	return _aoRadiusParam;
}
//---------------------------------------------------------------------//

bool ATOM_SSAOEffect::createRT(ATOM_RenderDevice *device)
{
	float viewportWidth		= (float)ATOM_RenderScheme::getCurrentRenderScheme()->getWidth();
	float viewportHeight	= (float)ATOM_RenderScheme::getCurrentRenderScheme()->getHeight();

	// 如果RT未创建，或者RT的大小和当前屏幕大小不一致 重建RT
	bool bNoRT = /*!_depthBufferMips[0] || !_cszBuffer || */!_downScaledDepth || !_AOBuffer[0] || !_AOBuffer[1];
	if( _useDownScaledBlur )
	{
		bNoRT = bNoRT || !_AORawBuffer;
	}
	
	if(  bNoRT || 
		_rtWidth != viewportWidth || _rtHeight != viewportHeight ||
		_recreateRT	)
	{
		_rtWidth	= viewportWidth;
		_rtHeight	= viewportHeight;

		// 创建 一个DOWNSCALED的DEPTHBUFFER
		_downScaledDepth = ATOM_PostEffectRTMgr::getRT( _rtWidth>>1, _rtHeight>>1, 
														ATOM_PIXEL_FORMAT_R32F/*ATOM_PIXEL_FORMAT_RGBA32F*//*ATOM_PIXEL_FORMAT_RGBA8888*/, 
														0	);
		if( !_downScaledDepth )
			return false;
		_downScaledDepth->clear (0.f, 0.f, 0.f, 0.f);

		// 如果使用DOWNSCALED模糊BUFFER， 首先创建一个FULLSCALED的RAWBUFFER
		if( _useDownScaledBlur )
		{
			_AORawBuffer = ATOM_PostEffectRTMgr::getRT( _rtWidth, _rtHeight, ATOM_PIXEL_FORMAT_RG32F/*ATOM_PIXEL_FORMAT_RGBA8888*/, 0 );
			_AORawBuffer->clear (0.f, 0.f, 0.f, 0.f);
		}

		unsigned int blurWidth = _rtWidth;
		unsigned int blurHeight = _rtHeight;

		// 如果使用DOWNSCALED模糊BUFFER
		if( _useDownScaledBlur )
		{
			blurWidth	>>=	1;
			blurHeight	>>=	1;
		}
		for( int i = 0; i < 2; ++i )
		{
			_AOBuffer[i] = ATOM_PostEffectRTMgr::getRT( blurWidth/* / 2*/, 
														blurHeight/* / 2*/, 
														ATOM_PIXEL_FORMAT_RG32F/*ATOM_PIXEL_FORMAT_RGBA8888*/, i );
			if( !_AOBuffer[i] )
				return false;
			_AOBuffer[i]->clear (0.f, 0.f, 0.f, 0.f);
		}

		//----------------------------------------------------------------------------------------------------------//
		float afSampleOffsets[16] = {0.0f};
		float deviation_x	= 2.0f;
		float deviation_y	= 2.0f;
		float multiplier_x	= 1.0f;
		float multiplier_y	= 1.0f;
		// horizontal
		getSampleOffsets_Blur (_rtWidth / 2, afSampleOffsets, avSampleWeights_horizontal, deviation_x, multiplier_x);
		for (int i = 0; i < _GAUSSIAN_TAP_COUNT_FINAL; ++i)
		{
			avSampleOffsets_horizontal[i].set(afSampleOffsets[i], 0.f);
		}
		// vertical
		getSampleOffsets_Blur (_rtHeight / 2, afSampleOffsets, avSampleWeights_vertical, deviation_y, multiplier_y);
		for (int i = 0; i < _GAUSSIAN_TAP_COUNT_FINAL; ++i)
		{
			avSampleOffsets_vertical[i].set (0.f, afSampleOffsets[i]);
		}

		_recreateRT = false;
	}

	return true;
}

bool ATOM_SSAOEffect::createMaterial(ATOM_RenderDevice *device)
{
	if (!_material)
	{
		if (_materialError)
		{
			return false;
		}

		_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/pp_sao.mat");
		
		if (!_material)
		{
			_materialError = true;
			return false;
		}
	}

	return true;
}

bool ATOM_SSAOEffect::init(ATOM_RenderDevice *device)
{
	/*if( !ATOM_PostEffect::init(device) )
		return false;*/

	if( !_randomRotateTex )
	{
		
			ATOM_ColorARGB *data = 0;
#if 0
			data = ATOM_NEW_ARRAY(ATOM_ColorARGB, 32 * 32);
			for (unsigned i = 0; i < 32 * 32; ++i)
			{
				float rotateAngle = ATOM_TwoPi * (float(rand())/float(RAND_MAX));
				float s = ATOM_saturate (ATOM_sin (rotateAngle) * 0.5f + 0.5f);
				float c = ATOM_saturate (ATOM_cos (rotateAngle) * 0.5f + 0.5f);
				data[i].setFloats (s, c, 0.f, 0.f);
			}
			_randomRotateTex = ATOM_GetRenderDevice()->allocTexture (0, data, 32, 32, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOMIPMAP|ATOM_Texture::NOCOMPRESS);
			ATOM_DELETE_ARRAY(data);
#else
			_randomRotateTex = ATOM_CreateTextureResource("/textures/screenSpace/rotrandom8b.dds",ATOM_PIXEL_FORMAT_UNKNOWN,ATOM_LoadPriority_IMMEDIATE);
#endif

#if 0
			data = ATOM_NEW_ARRAY(ATOM_ColorARGB, 256 * 256);
			for( unsigned row = 0; row < 256; ++row )
			{
				for( unsigned col = 0; col < 256; ++col )
				{
					unsigned v = row ^ col;
					data[row*256+col].setBytes((unsigned char)v, 0, 0, 0);
				}
			}
			_xor256Tex = ATOM_GetRenderDevice()->allocTexture (0, data, 256, 256, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOMIPMAP|ATOM_Texture::NOCOMPRESS);
			ATOM_DELETE_ARRAY(data);
#endif

		if(!_randomRotateTex )
			return false;
	}

	// create the material
	if( !createMaterial(device) )
		return false;

	// create the rt
	if( !createRT(device) )
		return false;

	return true;
}

bool ATOM_SSAOEffect::destroy()
{
	_randomRotateTex = 0;
	_xor256Tex = 0;
	_cszBuffer = 0;
	_downScaledDepth = 0;
	_AORawBuffer = 0;
	_AOBuffer[0] = 0;
	_AOBuffer[1] = 0;

	for( int i = 0; i < MAX_MIP_LEVEL; ++i )
	{
		_depthBufferMips[i] = 0;
	}

	_material = 0;

	// reset the rt size
	_rtWidth = 0;
	_rtHeight = 0;

	return ATOM_PostEffect::destroy();
}

ATOM_Texture * ATOM_SSAOEffect::getAOBuffer() const
{
	return _AOBuffer[0].get();
}

void ATOM_SSAOEffect::debugDraw(int b)
{
	ATOM_PostEffect::debugDraw(b);
	_enabled = b;
}

void ATOM_SSAOEffect::computeCSZ(ATOM_RenderDevice *device)
{
	_material->setActiveEffect ("ReconstructCSZ");

	device->setRenderTarget (0, _cszBuffer.get());
	device->setViewport (0, ATOM_Rect2Di(0, 0, _cszBuffer->getWidth(), _cszBuffer->getHeight()));

	/*ATOM_SDLScene* currentScene = (ATOM_SDLScene*)ATOM_RenderScheme::getCurrentScene();
	ATOM_Camera * camera = currentScene->getCamera();
	float nearPlane = camera->getNearPlane();
	float farPlane = camera->getFarPlane();
	ATOM_Vector4f nearFarFactor( nearPlane, farPlane, 1.0f / (farPlane-nearPlane), (farPlane-nearPlane) );
	_material->getParameterTable()->setVector ( "cameraNearFarFactor", nearFarFactor );*/

	drawTexturedFullscreenQuad (	device, 
									_material.get(), 
									_cszBuffer->getWidth(), 
									_cszBuffer->getHeight()	);
	if( isSaveAllRTs() )
	{
		device->setRenderTarget (0, 0);
		_cszBuffer->saveToFileEx ("/wangjian_art/_cszBuffer.dds",ATOM_PIXEL_FORMAT_R32F);
	}
}
void ATOM_SSAOEffect::downSampleDepth(ATOM_RenderDevice *device)
{
	
		ATOM_Vector4f	pParams0, 
						pParams1;
		float			s1;
		float			t1;
		int				width;
		int				height;

		_material->setActiveEffect ("DownscaleZBufferNoMip");

		width	= _downScaledDepth->getWidth();
		height	= _downScaledDepth->getHeight();

		device->setRenderTarget (0, _downScaledDepth.get());
		device->setViewport (0, ATOM_Rect2Di(0, 0, width, height));

		// Set samples position
		s1 = .5f / (float) _rtWidth;
		t1 = .5f / (float) _rtHeight;

		// Use rotated grid + middle sample (~quincunx)
		pParams0=ATOM_Vector4f(s1*0.96f, t1*0.25f, -s1*0.25f, t1*0.96f);
		pParams1=ATOM_Vector4f(-s1*0.96f, -t1*0.25f, s1*0.25f, -t1*0.96f);

		_material->getParameterTable()->setVector ("texToTexParams0", pParams0);
		_material->getParameterTable()->setVector ("texToTexParams1", pParams1);

		drawTexturedFullscreenQuad (device, 
									_material.get(), 
									width, 
									height);

		if( isSaveAllRTs() )
		{
			device->setRenderTarget (0, 0);
			_downScaledDepth->saveToFileEx ("/wangjian_art/_downScaledDepth.dds",ATOM_PIXEL_FORMAT_R32F);
		}
}
void ATOM_SSAOEffect::genRawAO(ATOM_RenderDevice *device)
{
	
		_material->setActiveEffect ("GenRawAO");

		// 如果使用DOWNSCALED的模糊，则先渲染到_AORawBuffer，否则直接渲染到_AOBuffer[0]
		int width	= _useDownScaledBlur ? _AORawBuffer->getWidth() : _AOBuffer[0]->getWidth();
		int height	= _useDownScaledBlur ? _AORawBuffer->getHeight() : _AOBuffer[0]->getHeight();
		device->setRenderTarget (0, _useDownScaledBlur ? _AORawBuffer.get() : _AOBuffer[0].get() );
		device->setViewport (0, ATOM_Rect2Di(0, 0, width, height));

		device->setClearColor(0,1,1,1,1);
		device->clear(true,false,false);
		
		ATOM_Vector4f vConst(	_projScale, 
								_aoRadius, 
								_aoBias, 
								_aoAmount/pow(_aoRadius, 6.0f)	);
		_material->getParameterTable()->setVector ( "SSAO_params", vConst );

		ATOM_Vector4f vConst1(	_distanceThreshold, 
								0,0,0	);
		_material->getParameterTable()->setVector ( "SSAO_params1", vConst1 );

		ATOM_Vector4f vSizeParam( (float)width, (float)height, 1.0f/width, 1.0f/height	);
		_material->getParameterTable()->setVector ( "Size_param", vSizeParam );

		ATOM_SDLScene* currentScene = (ATOM_SDLScene*)ATOM_RenderScheme::getCurrentScene();
		ATOM_Camera * camera = currentScene->getCamera();
		float nearPlane = camera->getNearPlane();
		float farPlane = camera->getFarPlane();
		ATOM_Vector4f nearFarFactor( nearPlane, /*2000.0f*/farPlane, 1.0f / (farPlane/*2000.0f*/-nearPlane), (farPlane/*2000.0f*/-nearPlane) );
		_material->getParameterTable()->setVector ( "cameraNearFarFactor", nearFarFactor );

		float mipLogMaxOffset = _mipLogMaxOffset;
		mipLogMaxOffset = 10;
		ATOM_Vector4f vMipParam( mipLogMaxOffset, 0, 0, 0 );
		_material->getParameterTable()->setVector ( "Mip_params", vMipParam );

		_material->getParameterTable()->setTexture("rotationTexture", _randomRotateTex.get());

#if 0
		_material->getParameterTable()->setTexture("XOR32Texture", _xor256Tex.get());
#endif	

		// 如果使用MIPLEVEL 则设置_cszBuffer， 否则设置_downScaledDepth[在SAMPLE TAP时采样]
		_material->getParameterTable()->setTexture("cszTexture", _downScaledDepth.get());

		drawTexturedFullscreenQuad (device, 
									_material.get(), 
									width, 
									height);
	
	if( isSaveAllRTs() )
	{
		device->setRenderTarget (0, 0);
		if( _useDownScaledBlur )
			_AORawBuffer->saveToFileEx ("/wangjian_art/_AORawBuffer.dds",ATOM_PIXEL_FORMAT_RG32F);
		else
			_AOBuffer[0]->saveToFileEx ("/wangjian_art/_AORawBuffer.dds",ATOM_PIXEL_FORMAT_RG32F);
	}
}

struct CoordRect
{
	float leftU, topV;
	float rightU, bottomV;
};

bool getTextureCoords( ATOM_Texture *srcTex, const ATOM_Rect2Di *pRectSrc, ATOM_Texture *destTex, const ATOM_Rect2Di *pRectDest, CoordRect* pCoords)
{
	float tU, tV;

	// Start with a default mapping of the complete source surface to complete 
	// destination surface
	pCoords->leftU = 0.0f;
	pCoords->topV = 0.0f;
	pCoords->rightU = 1.0f;
	pCoords->bottomV = 1.0f;

	// If not using the complete source surface, adjust the coordinates
	if( pRectSrc != NULL )
	{
		// These delta values are the distance between source texel centers in 
		// texture address space
		tU = 1.0f / srcTex->getWidth();
		tV = 1.0f / srcTex->getHeight();

		pCoords->leftU += pRectSrc->point.x * tU;
		pCoords->topV += pRectSrc->point.y * tV;
		pCoords->rightU -= ( srcTex->getWidth() - pRectSrc->point.x - pRectSrc->size.w) * tU;
		pCoords->bottomV -= ( srcTex->getHeight() - pRectSrc->point.y - pRectSrc->size.h) * tV;
	}

	// If not drawing to the complete destination surface, adjust the coordinates
	if( pRectDest != NULL )
	{
		// These delta values are the distance between source texel centers in 
		// texture address space
		tU = 1.0f / destTex->getWidth();
		tV = 1.0f / destTex->getHeight();

		pCoords->leftU -= pRectDest->point.x * tU;
		pCoords->topV -= pRectDest->point.y * tV;
		pCoords->rightU += ( destTex->getWidth() - pRectDest->point.x - pRectDest->size.w) * tU;
		pCoords->bottomV += ( destTex->getHeight() - pRectDest->point.y - pRectDest->size.h ) * tV;
	}

	return true;
}
void ATOM_SSAOEffect::blur(ATOM_RenderDevice *device)
{
	if( !_aoblur )
		return;

	int width;
	int height;

	//-----------------------------------------------------------------------------//
	// 如果使用DOWNSCALED的模糊 首先DOWNSCALE _AORawBuffer到_AOBuffer[0]
	if( /*_useSAO*/_useDownScaledBlur )
	{
#if 0
		_material->setActiveEffect ("Copy");
		width	= _AOBuffer[0]->getWidth();
		height	= _AOBuffer[0]->getHeight();
		device->setRenderTarget (0, _AOBuffer[0].get());
		device->setViewport (0, ATOM_Rect2Di(0, 0, width, height));
		_material->getParameterTable()->setTexture("texSrc", _AORawBuffer.get());
		drawTexturedFullscreenQuad (device, 
									_material.get(), 
									width, 
									height);
#else
		_material->setActiveEffect ("DownscaleAOBuffer");

		width	= _AOBuffer[0]->getWidth();
		height	= _AOBuffer[0]->getHeight();

		device->setRenderTarget (0, _AOBuffer[0].get());
		device->setViewport (0, ATOM_Rect2Di(0, 0, width, height));

		// Set samples position
		const float s1 = .5f / (float) _AORawBuffer->getWidth(); 
		const float t1 = .5f / (float) _AORawBuffer->getHeight(); 

		// Use rotated grid + middle sample (~quincunx)
		ATOM_Vector4f pParams0=ATOM_Vector4f(s1*0.96f, t1*0.25f, -s1*0.25f, t1*0.96f);
		ATOM_Vector4f pParams1=ATOM_Vector4f(-s1*0.96f, -t1*0.25f, s1*0.25f, -t1*0.96f);

		_material->getParameterTable()->setVector ("texToTexParams0", pParams0);
		_material->getParameterTable()->setVector ("texToTexParams1", pParams1);
		_material->getParameterTable()->setTexture("texHighLevel", _AORawBuffer.get());

		drawTexturedFullscreenQuad (device, 
									_material.get(), 
									width, 
									height);
#endif
	}
	
	//-----------------------------------------------------------------------------//

	width	= _AOBuffer[0]->getWidth();
	height	= _AOBuffer[0]->getHeight();

	ATOM_Texture * blurSrc = _AOBuffer[0].get();
	ATOM_Texture * blurDst = _AOBuffer[1].get();

	_material->setActiveEffect ("Blur");

	/*int width	= blurDst->getWidth();
	int height	= blurDst->getHeight();
	#if 1
	width	=	(int)ceilf(width  * _vp_ratioWidth);
	height	=	(int)ceilf(height * _vp_ratioHeight);
	#endif

	ATOM_Rect2Di rectSrc;
	rectSrc.point.x = 0;
	rectSrc.point.y = 0;
	rectSrc.size.w = width;
	rectSrc.size.h = height;

	ATOM_Rect2Di rectDest;
	rectDest.point.x = 0;
	rectDest.point.y = 0;
	rectDest.size.w = width;
	rectDest.size.h = height;

	CoordRect coords;
	getTextureCoords (blurSrc, &rectSrc, blurDst, &rectDest, &coords);

	_chain->beginScissorTest (device, 0, 0, width, height);

	_material->getParameterTable()->setVector("screenCoordScaleBias_bloom",ATOM_Vector4f(_vp_ratioWidth,_vp_ratioHeight,0,0));*/

	//_material->getParameterTable()->setFloat( "BilateralDepthThresh", _bilateralDepthThresh );

	// _aoblur是模糊次数
	for( int i = 0; i < _aoblur; ++i )
	{
		//--------------------------//
		//  horizontal
		device->setRenderTarget (0, blurDst);
		device->setViewport (0, ATOM_Rect2Di(0, 0, blurDst->getWidth(), blurDst->getHeight()));

	#if 0
		_material->getParameterTable()->setVectorArray ("g_avSampleOffsets", avSampleOffsets_horizontal, _GAUSSIAN_TAP_COUNT_FINAL);
		_material->getParameterTable()->setVectorArray ("g_avSampleWeights", avSampleWeights_horizontal, _GAUSSIAN_TAP_COUNT_FINAL);
	#else
		_material->getParameterTable()->setVector ("axis", ATOM_Vector4f(1,0,0,_bilateralDepthThresh));
		ATOM_Vector4f vSizeParam( (float)width, (float)height, 1.0f/width, 1.0f/height );
		_material->getParameterTable()->setVector ( "Size_param", vSizeParam );
	#endif
		//_bloomTexs[0].get()->clear (0.f, 0.f, 0.f, 1.f);

		_material->getParameterTable()->setTexture ("aoTexture", blurSrc);

		drawTexturedFullscreenQuad (	device, 
										_material.get(), 
										blurDst->getWidth(), 
										blurDst->getHeight()/*, coords.leftU, coords.topV, coords.rightU, coords.bottomV*/ );

		//--------------------------//
		ATOM_Texture * tmp = blurSrc;
		blurSrc = blurDst;
		blurDst = tmp;
		//--------------------------//

		device->setRenderTarget (0, blurDst);
		device->setViewport (0, ATOM_Rect2Di(0, 0, blurDst->getWidth(), blurDst->getHeight()));

		// vertical
	#if 0
		_material->getParameterTable()->setVectorArray ("g_avSampleOffsets", avSampleOffsets_vertical, _GAUSSIAN_TAP_COUNT_FINAL);
		_material->getParameterTable()->setVectorArray ("g_avSampleWeights", avSampleWeights_vertical, _GAUSSIAN_TAP_COUNT_FINAL);
	#else
		_material->getParameterTable()->setVector ("axis", ATOM_Vector4f(0,1,0,_bilateralDepthThresh));
		_material->getParameterTable()->setVector ( "Size_param", vSizeParam );
	#endif

		//_bloomTexs[1].get()->clear (0.f, 0.f, 0.f, 1.f);

		_material->getParameterTable()->setTexture ("aoTexture", blurSrc);
		
		drawTexturedFullscreenQuad (device, 
									_material.get(), 
									blurDst->getWidth(), 
									blurDst->getHeight()/*, coords.leftU, coords.topV, coords.rightU, coords.bottomV*/);

		//--------------------------//
		tmp = blurSrc;
		blurSrc = blurDst;
		blurDst = tmp;
		//--------------------------//
	}

	//_chain->endScissorTest (device);

	if( isSaveAllRTs() )
	{
		device->setRenderTarget (0, 0);
		blurSrc->saveToFileEx ("/BlurTexture0.dds",/*ATOM_PIXEL_FORMAT_RGBA32F*/ATOM_PIXEL_FORMAT_RG32F);
		blurDst->saveToFileEx ("/BlurTexture1.dds",/*ATOM_PIXEL_FORMAT_RGBA32F*/ATOM_PIXEL_FORMAT_RG32F);
	}
}
void ATOM_SSAOEffect::applyAO(ATOM_RenderDevice *device)
{
	_material->setActiveEffect ("Composition");
	device->setRenderTarget (0, getRenderTarget());

#if 1
	int width	=	getRenderTarget()->getWidth();
	int height	=	getRenderTarget()->getHeight();
#else
	int width	=	ATOM_RenderScheme::getCurrentRenderScheme()->getWidth();
	int height	=	ATOM_RenderScheme::getCurrentRenderScheme()->getHeight();
#endif

	device->setViewport (0, ATOM_Rect2Di(0, 0, width, height));

	_material->getParameterTable()->setTexture ("inputTexture", ATOM_GetColorTexture(0XFFFFFFFF));
	_material->getParameterTable()->setTexture ("aoTexture",  _AOBuffer[0].get());

	drawTexturedFullscreenQuad (device, _material.get(), width, height);
}
float GaussianDistribution( float x, float y, float rho )
{
	float g = 1.0f / sqrtf( 2.0f * ATOM_Pi * rho * rho );
	g *= ATOM_exp( -( x * x + y * y ) / ( 2 * rho * rho ) );

	return g;
}
bool ATOM_SSAOEffect::getSampleOffsets_Blur (unsigned texSize, float texCoordOffset[15], ATOM_Vector4f *colorWeight, float deviation, float multiplier)
{

#if 0

	int i = 0;
	float tu = 1.0f / ( float )texSize;

	// Fill the center texel
	float weight = multiplier * GaussianDistribution( 0, 0, deviation );
	colorWeight[0].set (weight, weight, weight, 1.0f );

	texCoordOffset[0] = 0.0f;

	// Fill the first half
	for( i = 1; i < _GAUSSIAN_TAP_COUNT_FINAL/2+1; i++ )
	{
		// Get the Gaussian intensity for this offset
		weight = multiplier * GaussianDistribution( ( float )i, 0, deviation );
		texCoordOffset[i] = i * tu;

		colorWeight[i].set (weight, weight, weight, 1.0f );
	}

	// Mirror to the second half
	for( i = _GAUSSIAN_TAP_COUNT_FINAL/2+1; i < _GAUSSIAN_TAP_COUNT_FINAL; i++ )
	{
		colorWeight[i] = colorWeight[i - _GAUSSIAN_TAP_COUNT_FINAL/2];
		texCoordOffset[i] = -texCoordOffset[i - _GAUSSIAN_TAP_COUNT_FINAL/2];
	}

	/////////////////////////////////////////////////////////////////////////
#else

	int i = 0;
	float tu = 1.0f / ( float )texSize;

	unsigned count = (_GAUSSIAN_TAP_COUNT_FINAL-1)*2+1;

	ATOM_VECTOR<ATOM_Vector4f> weights;
	weights.resize(count);

	ATOM_VECTOR<float> texoffsets;
	texoffsets.resize(count);

	// Fill the center texel
	float weight = multiplier * GaussianDistribution( 0, 0, deviation );
	colorWeight[0].set (weight, weight, weight, 1.0f );

	texCoordOffset[0] = 0.0f;


	// Fill the first half
	for( i = 1; i < _GAUSSIAN_TAP_COUNT_FINAL; i++ )
	{
		// Get the Gaussian intensity for this offset
		weight = multiplier * GaussianDistribution( ( float )i, 0, deviation );
		texoffsets[i] = i;// * tu;

		weights[i].set (weight, weight, weight, 1.0f );
	}

	// post process the weights and offsets so use linear blur

	ATOM_Vector4f weight_sum;
	int index = 1;
	i = 1;
	int halfsize = _GAUSSIAN_TAP_COUNT_FINAL / 2;
	for(; i < _GAUSSIAN_TAP_COUNT_FINAL; )
	{
		weight_sum = weights[i] + weights[i+1];

		texCoordOffset[index] = ( texoffsets[i] * weights[i].x + texoffsets[i+1] * weights[i+1].x ) / weight_sum.x;
		texCoordOffset[index] *= tu;

		colorWeight[index]	= weight_sum;
		colorWeight[index].w = 1;

		colorWeight[index+halfsize]		= colorWeight[index];
		texCoordOffset[index+halfsize]  = -texCoordOffset[index];

		i+=2;
		index++;
	}

#endif

	return true;
}
bool ATOM_SSAOEffect::render (ATOM_RenderDevice *device)
{
	if( _enabled && !_debugDraw )
	{
		_enabled = false;
		return true;
	}

	if( !init(device) )
		return false;

	// 如果使用SAO 并且使用MIPLEVEL,首先计算出CSZ
	downSampleDepth(device);
	genRawAO(device);
	blur(device);

	if( _debugDraw )
	{
		device->enableSRGBWrite(true);
		applyAO(device);
		device->enableSRGBWrite(false);
	}
	

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////