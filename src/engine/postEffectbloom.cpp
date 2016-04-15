#include "StdAfx.h"
#include "postEffectbloom.h"


/////////////////////////////////////////////////////////////////////////



ATOM_BLOOMEffect::ATOM_BLOOMEffect (void)
{
	_rtWidth = 0;
	_rtHeight = 0;

	_materialError = false;
	_bsaveall = false;
	_brightPassThreshold = 0.3f;
	_brightPassOffset = 3.f;
	_bloomScale = 10.0f;
	_material = 0;

	_gammaFlag = BEFORE_GAMMA_CORRECT;

	memset( avSampleOffsets_horizontal,0,sizeof(ATOM_Vector2f) * _GAUSSIAN_TAP_COUNT_FINAL );
	memset( avSampleWeights_horizontal,0,sizeof(ATOM_Vector4f) * _GAUSSIAN_TAP_COUNT_FINAL );
	memset( avSampleOffsets_vertical,0,sizeof(ATOM_Vector2f) * _GAUSSIAN_TAP_COUNT_FINAL );
	memset( avSampleWeights_vertical,0,sizeof(ATOM_Vector4f) * _GAUSSIAN_TAP_COUNT_FINAL );
}

ATOM_BLOOMEffect::~ATOM_BLOOMEffect (void)
{
}

void ATOM_BLOOMEffect::enable (int b)
{
	ATOM_PostEffect::enable(b);
	//ATOM_RenderSettings::enableBloom( b==0 ? false : true );
}
int ATOM_BLOOMEffect::isEnabled (void) const
{
	// 如果开启了HDR 则不开启bloom
	if( ATOM_RenderSettings::isHDRRenderingEnabled() )
		return 0;
	if( !ATOM_RenderSettings::isBloomEnabled() )
		return 0;
	return ATOM_PostEffect::isEnabled();
}

bool ATOM_BLOOMEffect::init(ATOM_RenderDevice *device)
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

bool ATOM_BLOOMEffect::destroy()
{
	// reset the bloom rt
	_bloomTexs[0] = 0;
	_bloomTexs[1] = 0;

	_material = 0;

	// reset the rt size
	_rtWidth = 0;
	_rtHeight = 0;

	_source = 0;

	return ATOM_PostEffect::destroy();
}

float ATOM_BLOOMEffect::GaussianDistribution( float x, float y, float rho )
{
	float g = 1.0f / sqrtf( 2.0f * ATOM_Pi * rho * rho );
	g *= ATOM_exp( -( x * x + y * y ) / ( 2 * rho * rho ) );

	return g;
}

bool ATOM_BLOOMEffect::getSampleOffsets_Bloom (unsigned texSize, float texCoordOffset[15], ATOM_Vector4f *colorWeight, float deviation, float multiplier)
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

void ATOM_BLOOMEffect::sceneToSceneScaled (ATOM_RenderDevice *device)
{
	stretchrect(device,_source.get(),/*_sourceScaled*/_bloomTexs[1].get(),true );

	if( 0 )
	{
		device->setRenderTarget (0, 0);
	 	_source->saveToFileEx ("/source_bloom.dds",ATOM_PIXEL_FORMAT_RGBA8888);
		_bloomTexs[1]->saveToFileEx ("/sceneScaled_bloom.dds",ATOM_PIXEL_FORMAT_RGBA8888);
	}
}

void ATOM_BLOOMEffect::sceneScaledToBloomSource(ATOM_RenderDevice *device)
{
	_material->setActiveEffect ("BrightPassFilter");
	ATOM_SDLScene* scene = (ATOM_SDLScene*)ATOM_RenderScheme::getCurrentScene();
	if( scene )
	{
		_brightPassThreshold = scene->getBloomThreshold();
		_brightPassOffset = scene->getBloomOffset();
	}
	_material->getParameterTable()->setFloat ("fBrightPassThreshold", _brightPassThreshold);
	_material->getParameterTable()->setFloat ("fBrightPassOffset", _brightPassOffset);
	_material->getParameterTable()->setTexture ("tex0", /*_sourceScaled*/_bloomTexs[0].get());

	device->setRenderTarget (0, _bloomTexs[1].get());

	drawTexturedFullscreenQuad (device, _material.get(), _bloomTexs[1]->getWidth(), _bloomTexs[1]->getHeight());

	if( _bsaveall )
	{
		device->setRenderTarget (0, 0);

		_bloomTexs[1]->saveToFile ("/bloomSource.dds");
	}
}
void ATOM_BLOOMEffect::genBloom(ATOM_RenderDevice *device)
{
	_material->setActiveEffect ("Bloom");

	int width	= _bloomTexs[0]->getWidth();
	int height	= _bloomTexs[0]->getHeight();
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
	getTextureCoords (_bloomTexs[1].get(), &rectSrc, _bloomTexs[0].get(), &rectDest, &coords);

	/*device->setClearColor (NULL, 0);
	device->clear (true, false, false);*/

	_chain->beginScissorTest (device, 0, 0, width, height);

	_material->getParameterTable()->setVector("screenCoordScaleBias_bloom",ATOM_Vector4f(_vp_ratioWidth,_vp_ratioHeight,0,0));

	for( int i = 0; i < 1; ++i )
	{
		//  horizontal
		_material->getParameterTable()->setVectorArray ("g_avSampleOffsets", avSampleOffsets_horizontal, _GAUSSIAN_TAP_COUNT_FINAL);
		_material->getParameterTable()->setVectorArray ("g_avSampleWeights", avSampleWeights_horizontal, _GAUSSIAN_TAP_COUNT_FINAL);

		_bloomTexs[0].get()->clear (0.f, 0.f, 0.f, 1.f);

		_material->getParameterTable()->setTexture ("tex0", _bloomTexs[1].get());
		device->setRenderTarget (0, _bloomTexs[0].get());

		device->setViewport (0, ATOM_Rect2Di(0, 0, _bloomTexs[0]->getWidth(), _bloomTexs[0]->getHeight()));

		drawTexturedFullscreenQuad ( device, _material.get(), _bloomTexs[0]->getWidth(), _bloomTexs[0]->getHeight()/*, coords.leftU, coords.topV, coords.rightU, coords.bottomV*/ );

		// vertical
		_material->getParameterTable()->setVectorArray ("g_avSampleOffsets", avSampleOffsets_vertical, _GAUSSIAN_TAP_COUNT_FINAL);
		_material->getParameterTable()->setVectorArray ("g_avSampleWeights", avSampleWeights_vertical, _GAUSSIAN_TAP_COUNT_FINAL);

		_bloomTexs[1].get()->clear (0.f, 0.f, 0.f, 1.f);

		_material->getParameterTable()->setTexture ("tex0", _bloomTexs[0].get());
		device->setRenderTarget (0, _bloomTexs[1].get());

		device->setViewport (0, ATOM_Rect2Di(0, 0, _bloomTexs[0]->getWidth(), _bloomTexs[0]->getHeight()));

		drawTexturedFullscreenQuad (device, _material.get(), _bloomTexs[0]->getWidth(), _bloomTexs[0]->getHeight()/*, coords.leftU, coords.topV, coords.rightU, coords.bottomV*/);
	}

	_chain->endScissorTest (device);

	if( 0 )
	{
		device->setRenderTarget (0, 0);
		_bloomTexs[0]->saveToFileEx ("/BloomTexture0.dds",ATOM_PIXEL_FORMAT_RGBA8888);
		_bloomTexs[1]->saveToFileEx ("/BloomTexture1.dds",ATOM_PIXEL_FORMAT_RGBA8888);
	}
}

bool ATOM_BLOOMEffect::render (ATOM_RenderDevice *device)
{
	// 创建RT 和 材质
	if( !init(device) )
		return false;

	// 开启SRGB写RT
	device->enableSRGBWrite(true);

	////////////////////////////////////////////////////////////////////////////////////////////////

	_material->getParameterTable()->setVector("screenCoordScaleBias_bloom",ATOM_Vector4f(_vp_ratioWidth,_vp_ratioHeight,0,0));

	sceneToSceneScaled (device);
	//sceneScaledToBloomSource(device);
	genBloom(device);

	_material->setActiveEffect ("default");

	ATOM_SDLScene* scene = (ATOM_SDLScene*)ATOM_RenderScheme::getCurrentScene();
	if( scene )
	{
		_bloomScale = scene->getBloomMultiplier();
	}
	else
		_bloomScale = 1.0f;

	_material->getParameterTable()->setFloat ("fBloomScale", _bloomScale);

	if( ATOM_RenderSettings::isDebugBloomBufferEnabled() )
		_material->getParameterTable()->setTexture ("inputTexture",(ATOM_Texture*)0);
	else
		_material->getParameterTable()->setTexture ("inputTexture", _source.get());

	_material->getParameterTable()->setTexture ("bloomTexture",  _bloomTexs[1].get());

	device->setRenderTarget (0, getRenderTarget());

#if 0
	int width	=	getRenderTarget()->getWidth();
	int height	=	getRenderTarget()->getHeight();
#else
	int width	=	ATOM_RenderScheme::getCurrentRenderScheme()->getWidth();
	int height	=	ATOM_RenderScheme::getCurrentRenderScheme()->getHeight();
#endif
	device->setViewport (0, ATOM_Rect2Di(0, 0, width, height));


	drawTexturedFullscreenQuad (device, _material.get(), width, height);

	// 关闭SRGB写
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

bool ATOM_BLOOMEffect::clearTexture (ATOM_Texture *texture)
{
	return texture->clear (0.f, 0.f, 0.f, 0.f);
}

bool ATOM_BLOOMEffect::createRT(ATOM_RenderDevice *device)
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
	if( !_bloomTexs[0] || !_bloomTexs[1] ||
		_rtWidth != inputTexture->getWidth() || _rtHeight != inputTexture->getHeight() )
	{
		_rtWidth = inputTexture->getWidth();
		_rtHeight = inputTexture->getHeight();

		_cropWidth	= _rtWidth;// - (_rtWidth % 8);
		_cropHeight = _rtHeight;// - (_rtHeight % 8);

		int bloom_width		= _cropWidth/4;
		int bloom_height	= _cropHeight/4;

		bool needCreateRT = true;
		if( _bloomTexs[0] && _bloomTexs[1] )
		{
			if( _bloomTexs[0]->getWidth() == bloom_width && _bloomTexs[0]->getHeight() == bloom_height )
				needCreateRT = false;
		}

		//_sourceScaled = device->allocTexture (0, 0, _rtWidth/8, _rtHeight/8, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);

		// wangjian modified : 使用ATOM_PostEffectRTMgr创建全局的RenderTarget
#if 0
		for( int i = 0; i < 2; ++i )
		{
			_bloomTexs[i] = device->allocTexture (0, 0, _rtWidth / 4, _rtHeight / 4, ATOM_PIXEL_FORMAT_RGBA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
			clearTexture(_bloomTexs[i].get());
		}
#else
		if( needCreateRT )
		{
			for( int i = 0; i < 2; ++i )
			{
				_bloomTexs[i] = ATOM_PostEffectRTMgr::getRT( bloom_width, bloom_height, ATOM_PIXEL_FORMAT_RGBA8888, i );
				if( !_bloomTexs[i] )
					return false;

				clearTexture(_bloomTexs[i].get());
			}
		}
#endif

		float afSampleOffsets[16] = {0.0f};

		// horizontal
		float deviation_x	= 1.0f;
		float deviation_y	= 1.0f;
		float multiplier_x	= 1.0f;
		float multiplier_y	= 1.0f;
#if 1
		ATOM_DeferredRenderScheme * d_scheme = static_cast<ATOM_DeferredRenderScheme*>(ATOM_RenderScheme::getCurrentRenderScheme());
		if( d_scheme && d_scheme->getSchemeFlag() & ATOM_DeferredRenderScheme::DSF_NOMAINSCHEME )
		{
			//bloom_width		*= 2;
			//bloom_height	*= 2;
			deviation_x		*= 0.75f;
			deviation_y		*= 0.75f;
			//multiplier_x	*= 0.8f;
			//multiplier_y	*= 0.8f;
		}
#endif

		getSampleOffsets_Bloom (bloom_width, afSampleOffsets, avSampleWeights_horizontal, deviation_x, multiplier_x);
		for (int i = 0; i < _GAUSSIAN_TAP_COUNT_FINAL; ++i)
		{
			avSampleOffsets_horizontal[i].set(afSampleOffsets[i], 0.f);
		}
		// vertical
		getSampleOffsets_Bloom (bloom_height, afSampleOffsets, avSampleWeights_vertical, deviation_y, multiplier_y);
		for (int i = 0; i < _GAUSSIAN_TAP_COUNT_FINAL; ++i)
		{
			avSampleOffsets_vertical[i].set (0.f, afSampleOffsets[i]);
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#if 0
//
//		avSampleOffsets_horizontal[0].x = 0.0;
//		avSampleOffsets_horizontal[1].x = -1.3846153846f / _bloomTexs[0]->getWidth();
//		avSampleOffsets_horizontal[2].x = -3.2307692308f / _bloomTexs[0]->getWidth();
//		avSampleOffsets_horizontal[3].x = 1.3846153846f / _bloomTexs[0]->getWidth();
//		avSampleOffsets_horizontal[4].x = 3.2307692308f / _bloomTexs[0]->getWidth();
//
//		avSampleWeights_horizontal[0].x = avSampleWeights_horizontal[0].y = avSampleWeights_horizontal[0].z = avSampleWeights_horizontal[0].w = 0.2270270270f;
//		avSampleWeights_horizontal[1].x = avSampleWeights_horizontal[1].y = avSampleWeights_horizontal[1].z = avSampleWeights_horizontal[1].w = 0.3162162162f;
//		avSampleWeights_horizontal[2].x = avSampleWeights_horizontal[2].y = avSampleWeights_horizontal[2].z = avSampleWeights_horizontal[2].w = 0.0702702703f;
//		avSampleWeights_horizontal[3].x = avSampleWeights_horizontal[3].y = avSampleWeights_horizontal[3].z = avSampleWeights_horizontal[3].w = 0.3162162162f;
//		avSampleWeights_horizontal[4].x = avSampleWeights_horizontal[4].y = avSampleWeights_horizontal[4].z = avSampleWeights_horizontal[4].w = 0.0702702703f;
//
//		avSampleOffsets_vertical[0].y = 0.0;
//		avSampleOffsets_vertical[1].y = -1.3846153846f / _bloomTexs[0]->getHeight();
//		avSampleOffsets_vertical[2].y = -3.2307692308f / _bloomTexs[0]->getHeight();
//		avSampleOffsets_vertical[3].y = 1.3846153846f / _bloomTexs[0]->getHeight();
//		avSampleOffsets_vertical[4].y = 3.2307692308f / _bloomTexs[0]->getHeight();
//
//		avSampleWeights_vertical[0].x = avSampleWeights_vertical[0].y = avSampleWeights_vertical[0].z = avSampleWeights_vertical[0].w = 0.2270270270f;
//		avSampleWeights_vertical[1].x = avSampleWeights_vertical[1].y = avSampleWeights_vertical[1].z = avSampleWeights_vertical[1].w = 0.3162162162f;
//		avSampleWeights_vertical[2].x = avSampleWeights_vertical[2].y = avSampleWeights_vertical[2].z = avSampleWeights_vertical[2].w = 0.0702702703f;
//		avSampleWeights_vertical[3].x = avSampleWeights_vertical[3].y = avSampleWeights_vertical[3].z = avSampleWeights_vertical[3].w = 0.3162162162f;
//		avSampleWeights_vertical[4].x = avSampleWeights_vertical[4].y = avSampleWeights_vertical[4].z = avSampleWeights_vertical[4].w = 0.0702702703f;
//
//#else
//
//		avSampleOffsets_horizontal[0].x = -1.5f / _bloomTexs[0]->getWidth();
//		avSampleOffsets_horizontal[1].x = -0.5f / _bloomTexs[0]->getWidth();
//		avSampleOffsets_horizontal[2].x = 0.5f / _bloomTexs[0]->getWidth();
//		avSampleOffsets_horizontal[3].x = 1.5f / _bloomTexs[0]->getWidth();
//
//		avSampleOffsets_vertical[0].y = -1.5f / _bloomTexs[0]->getHeight();
//		avSampleOffsets_vertical[1].y = -0.5f / _bloomTexs[0]->getHeight();
//		avSampleOffsets_vertical[2].y = 0.5f / _bloomTexs[0]->getHeight();
//		avSampleOffsets_vertical[3].y = 1.5f / _bloomTexs[0]->getHeight();
//
//#endif
	
	}

	return true;
}

bool ATOM_BLOOMEffect::createMaterial(ATOM_RenderDevice *device)
{
	if (!_material)
	{
		if (_materialError)
		{
			return false;
		}

		_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/Bloom.mat");
		if (!_material)
		{
			_materialError = true;
			return false;
		}
	}

	return true;
}

bool ATOM_BLOOMEffect::drawFullScreenQuad (ATOM_RenderDevice *device, const ATOM_BLOOMEffect::CoordRect &coords)
{
	return drawFullScreenQuad (device, coords.leftU, coords.topV, coords.rightU, coords.bottomV);
}

bool ATOM_BLOOMEffect::drawFullScreenQuad (ATOM_RenderDevice *device, float leftU, float topV, float rightU, float bottomV)
{
	ATOM_AUTOREF(ATOM_Texture) rt = device->getRenderTarget (0);
	float width5 = rt->getWidth() - 0.5f;
	float height5 = rt->getHeight() - 0.5f;

	struct ScreenVertex
	{
		float x, y, z, rhw;
		float u, v;
	};

	ScreenVertex quad[4] = {
		{-0.5f, -0.5f, 0.5f, 1.f, leftU, topV},
		{width5, -0.5f, 0.5f, 1.f, rightU, topV},
		{-0.5f, height5, 0.5f, 1.f, leftU, bottomV},
		{width5, height5, 0.5f, 1.f, rightU, bottomV}
	};

	unsigned short indices[4] = { 0, 1, 2, 3 };

	unsigned numPasses = _material->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (_material->beginPass (device, pass))
		{
			device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_STRIP, 4, 4, ATOM_VERTEX_ATTRIB_COORD_XYZRHW|ATOM_VERTEX_ATTRIB_TEX1_2, sizeof(ScreenVertex), quad, indices);
			_material->endPass (device, pass);
		}
	}
	_material->end (device);

	return true;
}

void ATOM_BLOOMEffect::stretchrect (ATOM_RenderDevice *device,ATOM_Texture *pSrc,ATOM_Texture *pDst,bool bigDownSample)
{
	if(!pSrc || !pDst) 
	{
		return;
	}

	ATOM_Rect2Di rc;
	rc.point.x = (pSrc->getWidth() - _cropWidth) / 2;
	rc.point.y = (pSrc->getHeight() - _cropHeight) / 2;
	rc.size.w = _cropWidth;
	rc.size.h = _cropHeight;

	CoordRect coords;
	getTextureCoords (pSrc, &rc, pDst, 0, &coords);

#if 1

	bool bResample=0;

	if(pSrc->getWidth()!=pDst->getWidth() && pSrc->getHeight()!=pDst->getHeight())
	{
		bResample = 1;
	}

	pDst->clear (0.f, 0.f, 0.f, 1.f);

	device->setRenderTarget (0, pDst);

	

	/*device->setClearColor (NULL, 0);
	device->clear (true, false, false);*/

	int width	= pDst->getWidth();
	int height	= pDst->getHeight();
#if 1
	width	= (int)ceilf(width * _vp_ratioWidth);
	height	= (int)ceilf(height * _vp_ratioHeight);
#endif

	//beginScissorTest (device, 0, 0, width, height);

	device->setViewport (0, ATOM_Rect2Di(0, 0, width, height));

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

	float s1 = 0.5f / ( (float) pOffsetTex->getWidth() );		 // 2.0 better results on lower res images resizing        
	float t1 = 0.5f / ( (float) pOffsetTex->getHeight() );       

	ATOM_DeferredRenderScheme * d_scheme = static_cast<ATOM_DeferredRenderScheme*>(ATOM_RenderScheme::getCurrentRenderScheme());
	if( d_scheme && d_scheme->getSchemeFlag() & ATOM_DeferredRenderScheme::DSF_NOMAINSCHEME )
	{
		s1 *= t1 = 0.0f;
	}

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

	/*ATOM_SDLScene* scene = (ATOM_SDLScene*)ATOM_RenderScheme::getCurrentScene();
	if( scene )
	{
		_bloomScale = scene->getBloomMultiplier();
	}
	else
		_bloomScale = 1.0f;

	_material->getParameterTable()->setFloat ("fBloomScale", _bloomScale);*/

	ATOM_SDLScene* scene = (ATOM_SDLScene*)ATOM_RenderScheme::getCurrentScene();
	if( scene )
	{
		_brightPassThreshold = scene->getBloomThreshold();
		_brightPassOffset = scene->getBloomOffset();
	}
	_material->getParameterTable()->setFloat ("fBrightPassThreshold", _brightPassThreshold);

	ATOM_Vector4f screenCoordThreshold( coords.leftU * _vp_ratioWidth, coords.topV * _vp_ratioHeight, coords.rightU * _vp_ratioWidth, coords.bottomV * _vp_ratioHeight );
	_material->getParameterTable()->setVector ("screenCoordThreshold", screenCoordThreshold);

	/*int nFilter = (bResample && bBigDownsample) ? FILTER_LINEAR: FILTER_POINT;*/
	_material->getParameterTable()->setTexture ("tex0", pSrc);

	drawTexturedFullscreenQuad ( device, _material.get(), width, height/*, coords.leftU, coords.topV, coords.rightU, coords.bottomV*/ );
}

bool ATOM_BLOOMEffect::getTextureCoords( ATOM_Texture *srcTex, const ATOM_Rect2Di *pRectSrc, ATOM_Texture *destTex, const ATOM_Rect2Di *pRectDest, CoordRect* pCoords)
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////