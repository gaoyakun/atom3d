#include "StdAfx.h"
#include "hdr_pipe.h"

#define DEBUG_HDR 1

static inline float GaussianDistribution( float x, float y, float rho )
{
    float g = 1.0f / sqrtf( 2.0f * ATOM_Pi * rho * rho );
    g *= ATOM_exp( -( x * x + y * y ) / ( 2 * rho * rho ) );

    return g;
}

HDR_Pipeline::HDR_Pipeline (void)
{
	_blueShift = true;
	_adaptationInvalid = true;
	_keyValue = 0.8f;
	_brightPassThreshold = 1.f;
	_brightPassOffset = 10.f;
	_lastSourceWidth = 0;
	_lastSourceHeight = 0;
	//--- wangjian added ---//
	// HDR
	_hdr_enabled = false;				// 默认关闭HDR渲染
	bsaveall = false;
	_use_filmic_mapping = false;
	_filmCurveShoulder = 1.f;
	_filmCurveMiddleTone = 1.f;
	_filmCurveToe = 1.f;
	_filmCurveWhitePoint = 4.f;
	_HDRSaturate = 1.f;
	_HDRContrast = 1.f;
	_HDRColorBalance = ATOM_Vector3f(1,1,1);
	//----------------------//
}

HDR_Pipeline::~HDR_Pipeline (void)
{
}

void HDR_Pipeline::setSource (ATOM_RenderDevice *device, ATOM_Texture *hdrSource)
{
	if (_source.get() == hdrSource && hdrSource->getWidth() == _lastSourceWidth && hdrSource->getHeight() == _lastSourceHeight)
	{
		return;
	}

	_source = hdrSource;
	_lastSourceWidth = hdrSource->getWidth();
	_lastSourceHeight = hdrSource->getHeight();

	_cropWidth = hdrSource->getWidth() - (hdrSource->getWidth() % 8);
	_cropHeight = hdrSource->getHeight() - (hdrSource->getHeight() % 8);

	//--- wangjian modified for FLOAT BUFFER test ---//

	_sourceScaled = device->allocTexture (0, 0, _cropWidth/4, _cropHeight/4, /*ATOM_PIXEL_FORMAT_BGRA8888*/ATOM_PIXEL_FORMAT_RGBA16F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
	_brightPass = device->allocTexture (0, 0, _cropWidth/4+2, _cropHeight/4+2, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
	_starSource = device->allocTexture (0, 0, _cropWidth/4+2, _cropHeight/4+2, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
	_bloomSource = device->allocTexture (0, 0, _cropWidth/8+2, _cropHeight/8+2, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
	_adaptedLumCurrent = device->allocTexture (0, 0, 1, 1, /*ATOM_PIXEL_FORMAT_GREY8*/ATOM_PIXEL_FORMAT_R16F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
	_adaptedLumLast = device->allocTexture (0, 0, 1, 1, /*ATOM_PIXEL_FORMAT_GREY8*/ATOM_PIXEL_FORMAT_R16F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);

    for(int i = 0; i < NUM_TONEMAP_TEXTURES; ++i)
    {
        int iSampleLen = 1 << ( 2 * i );
		_toneMapTextures[i] = device->allocTexture (0, 0, iSampleLen, iSampleLen, /*ATOM_PIXEL_FORMAT_GREY8*/ATOM_PIXEL_FORMAT_R16F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
    }

    for(int i = 1; i < NUM_BLOOM_TEXTURES; ++i)
    {
		_bloomTextures[i] = device->allocTexture (0, 0, _cropWidth/8+2, _cropHeight/8+2, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
        clearTexture( _bloomTextures[i].get() );
    }

	_bloomTextures[0] = device->allocTexture (0, 0, _cropWidth/8, _cropHeight/8, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);

    for(int i = 0; i < NUM_STAR_TEXTURES; ++i)
    {
		_starTextures[i] = device->allocTexture (0, 0, _cropWidth/4, _cropHeight/4, ATOM_PIXEL_FORMAT_BGRA8888/*ATOM_PIXEL_FORMAT_RGBA16F*/, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
    }

	clearTexture (_adaptedLumCurrent.get());
	clearTexture (_adaptedLumLast.get());
	clearTexture (_bloomSource.get());
	clearTexture (_brightPass.get());
	clearTexture (_starSource.get());

	_hdrMaterial = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/hdr.mat");
	_hdrMaterial->getParameterTable()->setFloat( "g_fBloomScale", 1.0f );
    _hdrMaterial->getParameterTable()->setFloat( "g_fStarScale", 0.5f );

#if DEBUG_HDR
	if( bsaveall )
	{
		device->setRenderTarget (0, 0);
		_source->saveToFile ("/source.png");
	}
#endif

}

void HDR_Pipeline::setDestination (ATOM_RenderDevice *device, ATOM_Texture *ldrDestination)
{
	_dest = ldrDestination;
}

bool HDR_Pipeline::clearTexture (ATOM_Texture *texture)
{
	return texture->clear (0.f, 0.f, 0.f, 0.f);
}

bool HDR_Pipeline::sceneToSceneScaled (ATOM_RenderDevice *device)
{
	ATOM_Vector2f sampleOffsets[MAX_SAMPLES];

	ATOM_Rect2Di rc;
	rc.point.x = (_source->getWidth() - _cropWidth) / 2;
	rc.point.y = (_source->getHeight() - _cropHeight) / 2;
	rc.size.w = rc.point.x + _cropWidth;
	rc.size.h = rc.point.y + _cropHeight;

	CoordRect coords;
	getTextureCoords (_source.get(), &rc, _sourceScaled.get(), 0, &coords);

	_hdrMaterial->setActiveEffect ("DownScale4x4");
	getSampleOffsets_DownScale4x4 (_source->getWidth(), _source->getHeight(), sampleOffsets);
	_hdrMaterial->getParameterTable()->setVectorArray ("g_avSampleOffsets", sampleOffsets, MAX_SAMPLES);
	_hdrMaterial->getParameterTable()->setTexture ("tex0", _source.get());

	device->setRenderTarget (0, _sourceScaled.get());

	//--- added ---//
	device->setViewport (0, ATOM_Rect2Di(0, 0,_sourceScaled->getWidth(), _sourceScaled->getHeight()));

	drawFullScreenQuad (device, coords);

#if DEBUG_HDR
	if( bsaveall )
	{
		device->setRenderTarget (0, 0);
		_sourceScaled->saveToFile ("/sceneScaled.dds");
	}
#endif

	return true;
}

bool HDR_Pipeline::sceneScaledToBrightPass (ATOM_RenderDevice *device)
{
	ATOM_Rect2Di rectSrc;
	rectSrc.point.x = 1;
	rectSrc.point.y = 1;
	rectSrc.size.w = _sourceScaled->getWidth()-2;
	rectSrc.size.h = _sourceScaled->getHeight()-2;

	ATOM_Rect2Di rectDest;
	rectDest.point.x = 1;
	rectDest.point.y = 1;
	rectDest.size.w = _brightPass->getWidth()-2;
	rectDest.size.h = _brightPass->getHeight()-2;

	CoordRect coords;
	getTextureCoords (_sourceScaled.get(), &rectSrc, _brightPass.get(), &rectDest, &coords);

	_hdrMaterial->setActiveEffect ("BrightPassFilter");
	_hdrMaterial->getParameterTable()->setFloat ("g_brightPassThreshold", _brightPassThreshold);
	_hdrMaterial->getParameterTable()->setFloat ("g_brightPassOffset", _brightPassOffset);
	//--- wangjian added ---//
	if( _use_filmic_mapping )
	{
		ATOM_Vector4f filmicCurveParam(_filmCurveShoulder,_filmCurveMiddleTone,_filmCurveToe,_filmCurveWhitePoint);
		_hdrMaterial->getParameterTable()->setVector ("g_filmicCurveParam", filmicCurveParam);
		_hdrMaterial->getParameterTable()->setFloat ("g_HDRSaturate", _HDRSaturate);
		_hdrMaterial->getParameterTable()->setFloat ("g_HDRContrast", _HDRContrast);
		_hdrMaterial->getParameterTable()->setVector ("g_HDRColorBalance", _HDRColorBalance);
	}
	//----------------------//
	_hdrMaterial->getParameterTable()->setFloat ("g_fMiddleGray", _keyValue);
	_hdrMaterial->getParameterTable()->setTexture ("tex0", _sourceScaled.get());
	_hdrMaterial->getParameterTable()->setTexture ("tex1", _adaptedLumCurrent.get());

	device->setRenderTarget (0, _brightPass.get());

	//--- added ---//
	device->setViewport (0, ATOM_Rect2Di(0, 0,_brightPass->getWidth(), _brightPass->getHeight()));

	beginScissorTest (device, rectDest.point.x, rectDest.point.y, rectDest.size.w, rectDest.size.h);
	drawFullScreenQuad (device, coords);
	endScissorTest (device);

#if DEBUG_HDR
	if( bsaveall )
	{
		device->setRenderTarget (0, 0);
		_brightPass->saveToFile ("/brightpass.dds");
	}
#endif

	return true;
}

bool HDR_Pipeline::brightPassToStarSource (ATOM_RenderDevice *device)
{
	ATOM_Vector2f avSampleOffsets[MAX_SAMPLES];
	ATOM_Vector4f avSampleWeights[MAX_SAMPLES];

	ATOM_Rect2Di rectDest;
	rectDest.point.x = 1;
	rectDest.point.y = 1;
	rectDest.size.w = _starSource->getWidth()-2;
	rectDest.size.h = _starSource->getHeight()-2;

	CoordRect coords;
	getTextureCoords (_brightPass.get(), 0, _starSource.get(), &rectDest, &coords);

	getSampleOffsets_GaussBlur5x5(_brightPass->getWidth(), _brightPass->getHeight(), avSampleOffsets, avSampleWeights);

	_hdrMaterial->setActiveEffect ("GaussBlur5x5");
	_hdrMaterial->getParameterTable()->setVectorArray ("g_avSampleOffsets", avSampleOffsets, MAX_SAMPLES);
	_hdrMaterial->getParameterTable()->setVectorArray ("g_avSampleWeights", avSampleWeights, MAX_SAMPLES);
	_hdrMaterial->getParameterTable()->setTexture ("tex0", _brightPass.get());

	device->setRenderTarget (0, _starSource.get());

	//--- added ---//
	device->setViewport (0, ATOM_Rect2Di(0, 0,_starSource->getWidth(), _starSource->getHeight()));

	beginScissorTest (device, rectDest.point.x, rectDest.point.y, rectDest.size.w, rectDest.size.h);
	drawFullScreenQuad (device, coords);
	endScissorTest (device);

#if DEBUG_HDR
	if( bsaveall )
	{
		device->setRenderTarget (0, 0);
		_starSource->saveToFile ("/starSource.dds");
	}
#endif

	return true;
}

bool HDR_Pipeline::starSourceToBloomSource (ATOM_RenderDevice *device)
{
	ATOM_Vector2f avSampleOffsets[MAX_SAMPLES];

	ATOM_Rect2Di rectSrc;
	rectSrc.point.x = 1;
	rectSrc.point.y = 1;
	rectSrc.size.w = _starSource->getWidth()-2;
	rectSrc.size.h = _starSource->getHeight()-2;

	ATOM_Rect2Di rectDest;
	rectDest.point.x = 1;
	rectDest.point.y = 1;
	rectDest.size.w = _bloomSource->getWidth()-2;
	rectDest.size.h = _bloomSource->getHeight()-2;

	CoordRect coords;
	getTextureCoords (_starSource.get(), &rectSrc, _bloomSource.get(), &rectDest, &coords);

	getSampleOffsets_DownScale2x2(_brightPass->getWidth(), _brightPass->getHeight(), avSampleOffsets);

	_hdrMaterial->setActiveEffect("DownScale2x2");
	_hdrMaterial->getParameterTable()->setVectorArray ("g_avSampleOffsets", avSampleOffsets, MAX_SAMPLES);
	_hdrMaterial->getParameterTable()->setTexture ("tex0", _starSource.get());

	device->setRenderTarget (0, _bloomSource.get());

	//--- added ---//
	device->setViewport (0, ATOM_Rect2Di(0, 0,_bloomSource->getWidth(), _bloomSource->getHeight()));

	beginScissorTest (device, rectDest.point.x, rectDest.point.y, rectDest.size.w, rectDest.size.h);
	drawFullScreenQuad (device, coords);
	endScissorTest (device);

#if DEBUG_HDR
	if( bsaveall )
	{
		device->setRenderTarget (0, 0);
		_bloomSource->saveToFile ("/bloomSource.dds");
	}
#endif

	return true;

}

bool HDR_Pipeline::getSampleOffsets_GaussBlur5x5 (unsigned texWidth, unsigned texHeight, ATOM_Vector2f *texCoordOffset, ATOM_Vector4f *sampleWeight, float multiplier)
{
    float tu = 1.0f / ( float )texWidth;
    float tv = 1.0f / ( float )texHeight;

    ATOM_Vector4f vWhite( 1.0f, 1.0f, 1.0f, 1.0f );

    float totalWeight = 0.0f;
    int index = 0;
    for( int x = -2; x <= 2; x++ )
    {
        for( int y = -2; y <= 2; y++ )
        {
            // Exclude pixels with a block distance greater than 2. This will
            // create a kernel which approximates a 5x5 kernel using only 13
            // sample points instead of 25; this is necessary since 2.0 shaders
            // only support 16 texture grabs.
            if( ATOM_abs( x ) + abs( y ) > 2 )
                continue;

            // Get the unscaled Gaussian intensity for this offset
            texCoordOffset[index] = ATOM_Vector2f( x * tu, y * tv );
            sampleWeight[index] = vWhite * GaussianDistribution( ( float )x, ( float )y, 1.0f );
            totalWeight += sampleWeight[index].x;

            index++;
        }
    }

    // Divide the current weight by the total weight of all the samples; Gaussian
    // blur kernels add to 1.0f to ensure that the intensity of the image isn't
    // changed when the blur occurs. An optional multiplier variable is used to
    // add or remove image intensity during the blur.
    for( int i = 0; i < index; i++ )
    {
        sampleWeight[i] /= totalWeight;
        sampleWeight[i] *= multiplier;
    }

    return true;
}

bool HDR_Pipeline::getSampleOffsets_Bloom (unsigned texSize, float texCoordOffset[15], ATOM_Vector4f *colorWeight, float deviation, float multiplier)
{
    int i = 0;
    float tu = 1.0f / ( float )texSize;

    // Fill the center texel
    float weight = multiplier * GaussianDistribution( 0, 0, deviation );
    colorWeight[0].set (weight, weight, weight, 1.0f );

    texCoordOffset[0] = 0.0f;

    // Fill the first half
    for( i = 1; i < 8; i++ )
    {
        // Get the Gaussian intensity for this offset
        weight = multiplier * GaussianDistribution( ( float )i, 0, deviation );
        texCoordOffset[i] = i * tu;

        colorWeight[i].set (weight, weight, weight, 1.0f );
    }

    // Mirror to the second half
    for( i = 8; i < 15; i++ )
    {
        colorWeight[i] = colorWeight[i - 7];
        texCoordOffset[i] = -texCoordOffset[i - 7];
    }

    return true;
}

bool HDR_Pipeline::getSampleOffsets_Star (unsigned texSize, float texCoordOffset[15], ATOM_Vector4f *colorWeight, float deviation)
{
    int i = 0;
    float tu = 1.0f / ( float )texSize;

    // Fill the center texel
    float weight = 1.0f * GaussianDistribution( 0, 0, deviation );
    colorWeight[0].set( weight, weight, weight, 1.0f );

    texCoordOffset[0] = 0.0f;

    // Fill the first half
    for( i = 1; i < 8; i++ )
    {
        // Get the Gaussian intensity for this offset
        weight = 1.0f * GaussianDistribution( ( float )i, 0, deviation );
        texCoordOffset[i] = i * tu;

        colorWeight[i].set( weight, weight, weight, 1.0f );
    }

    // Mirror to the second half
    for( i = 8; i < 15; i++ )
    {
        colorWeight[i] = colorWeight[i - 7];
        texCoordOffset[i] = -texCoordOffset[i - 7];
    }

    return true;
}

bool HDR_Pipeline::getSampleOffsets_DownScale4x4 (unsigned width, unsigned height, ATOM_Vector2f sampleOffsets[])
{
    float tU = 1.0f / width;
    float tV = 1.0f / height;

    // Sample from the 16 surrounding points. Since the center point will be in
    // the exact center of 16 texels, a 0.5f offset is needed to specify a texel
    // center.
    int index = 0;
    for( int y = 0; y < 4; y++ )
    {
        for( int x = 0; x < 4; x++ )
        {
            sampleOffsets[ index ].x = ( x - 1.5f ) * tU;
            sampleOffsets[ index ].y = ( y - 1.5f ) * tV;

            index++;
        }
    }

    return true;
}

bool HDR_Pipeline::getSampleOffsets_DownScale2x2 (unsigned width, unsigned height, ATOM_Vector2f sampleOffsets[])
{
    float tU = 1.0f / width;
    float tV = 1.0f / height;

    // Sample from the 4 surrounding points. Since the center point will be in
    // the exact center of 4 texels, a 0.5f offset is needed to specify a texel
    // center.
    int index = 0;
    for( int y = 0; y < 2; y++ )
    {
        for( int x = 0; x < 2; x++ )
        {
            sampleOffsets[ index ].x = ( x - 0.5f ) * tU;
            sampleOffsets[ index ].y = ( y - 0.5f ) * tV;

            index++;
        }
    }

    return true;
}

bool HDR_Pipeline::measureLuminance (ATOM_RenderDevice *device)
{
	ATOM_Vector2f avSampleOffsets[MAX_SAMPLES];

	unsigned curTexture = NUM_TONEMAP_TEXTURES-1;

	float tU = 1.f / (3.f * _toneMapTextures[curTexture]->getWidth());
	float tV = 1.f / (3.f * _toneMapTextures[curTexture]->getHeight());
	int index = 0;

	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			avSampleOffsets[index].x = x * tU;
			avSampleOffsets[index].y = y * tV;
			index++;
		}
	}

	_hdrMaterial->setActiveEffect ("SampleAvgLum");
	_hdrMaterial->getParameterTable()->setVectorArray ("g_avSampleOffsets", avSampleOffsets, MAX_SAMPLES);
	_hdrMaterial->getParameterTable()->setTexture ("tex0", _sourceScaled.get());

	device->setRenderTarget (0, _toneMapTextures[curTexture].get());

	//--- added ---//
	device->setViewport (0, ATOM_Rect2Di(0, 0,_toneMapTextures[curTexture]->getWidth(), _toneMapTextures[curTexture]->getHeight()));

	drawFullScreenQuad (device, 0.f, 0.f, 1.f, 1.f);
	curTexture--;

	while (curTexture > 0)
	{
		getSampleOffsets_DownScale4x4 (_toneMapTextures[curTexture+1]->getWidth(), _toneMapTextures[curTexture+1]->getHeight(), avSampleOffsets);

		_hdrMaterial->setActiveEffect ("ResampleAvgLum");
		_hdrMaterial->getParameterTable()->setVectorArray ("g_avSampleOffsets", avSampleOffsets, MAX_SAMPLES);
		_hdrMaterial->getParameterTable()->setTexture ("tex0", _toneMapTextures[curTexture+1].get());

		device->setRenderTarget (0, _toneMapTextures[curTexture].get());

		//--- added ---//
		device->setViewport (0, ATOM_Rect2Di(0, 0,_toneMapTextures[curTexture]->getWidth(), _toneMapTextures[curTexture]->getHeight()));

		drawFullScreenQuad (device, 0.f, 0.f, 1.f, 1.f);

		curTexture--;
	}

	//--- wangjian modified ---//
	// 应该为_toneMapTextures[1] 而不是_toneMapTextures[0]
	getSampleOffsets_DownScale4x4 (_toneMapTextures[1]->getWidth(), _toneMapTextures[1]->getHeight(), avSampleOffsets);
	_hdrMaterial->setActiveEffect ("ResampleAvgLumExp");
	_hdrMaterial->getParameterTable()->setVectorArray ("g_avSampleOffsets", avSampleOffsets, MAX_SAMPLES);
	_hdrMaterial->getParameterTable()->setTexture ("tex0", _toneMapTextures[1].get());

	device->setRenderTarget (0, _toneMapTextures[0].get());

	//--- added ---//
	device->setViewport (0, ATOM_Rect2Di(0, 0,_toneMapTextures[0]->getWidth(), _toneMapTextures[0]->getHeight()));

	drawFullScreenQuad (device, 0.f, 0.f, 1.f, 1.f);





#if DEBUG_HDR
	if( bsaveall )
	{
		device->setRenderTarget (0, 0);
		for (unsigned i = 0; i < NUM_TONEMAP_TEXTURES; ++i)
		{
			char buffer[256];
			sprintf (buffer, "/tonemap%d.dds", i);
			_toneMapTextures[i]->saveToFile (buffer);
		}
	}
#endif

	return true;
}

bool HDR_Pipeline::calculateAdaptation (ATOM_RenderDevice *device)
{
	ATOM_AUTOREF(ATOM_Texture) texSwap = _adaptedLumLast;
	_adaptedLumLast = _adaptedLumCurrent;
	_adaptedLumCurrent = texSwap;

	_hdrMaterial->setActiveEffect ("CalculateAdaptedLum");

	float fElapsedTime = 1 - powf( 0.98f, 33.0f * ATOM_APP->getFrameStamp().elapsedTick * 0.001f );
	_hdrMaterial->getParameterTable()->setFloat ("g_fElapsedTime", fElapsedTime);
	_hdrMaterial->getParameterTable()->setTexture ("tex0", _adaptedLumLast.get());
	_hdrMaterial->getParameterTable()->setTexture ("tex1", _toneMapTextures[0].get());

	device->setRenderTarget (0, _adaptedLumCurrent.get());

	//--- added ---//
	device->setViewport (0, ATOM_Rect2Di(0, 0,_adaptedLumCurrent->getWidth(), _adaptedLumCurrent->getHeight()));

	drawFullScreenQuad (device, 0.f, 0.f, 1.f, 1.f);

#if DEBUG_HDR
	if( bsaveall )
	{
		device->setRenderTarget (0, 0);
		_adaptedLumCurrent->saveToFile ("/adaptedlum.dds");
	}
#endif

	return true;
}

bool HDR_Pipeline::renderStar (ATOM_RenderDevice *device)
{
	_starTextures[0]->clear (0.f, 0.f, 0.f, 0.f);

	return true;
}

bool HDR_Pipeline::renderBloom (ATOM_RenderDevice *device)
{
	ATOM_Vector2f avSampleOffsets[MAX_SAMPLES];
	ATOM_Vector4f avSampleWeights[MAX_SAMPLES];
	float afSampleOffsets[MAX_SAMPLES];

	_bloomTextures[0]->clear (0.f, 0.f, 0.f, 0.f);

	ATOM_Rect2Di rectSrc;
	rectSrc.point.x = 1;
	rectSrc.point.y = 1;
	rectSrc.size.w = _bloomSource->getWidth()-2;
	rectSrc.size.h = _bloomSource->getHeight()-2;

	ATOM_Rect2Di rectDest;
	rectDest.point.x = 1;
	rectDest.point.y = 1;
	rectDest.size.w = _bloomTextures[2]->getWidth()-2;
	rectDest.size.h = _bloomTextures[2]->getHeight()-2;

	CoordRect coords;
	getTextureCoords (_bloomSource.get(), &rectSrc, _bloomTextures[2].get(), &rectDest, &coords);

	getSampleOffsets_GaussBlur5x5(_bloomSource->getWidth(), _bloomSource->getHeight(), avSampleOffsets, avSampleWeights);

	_hdrMaterial->setActiveEffect ("GaussBlur5x5");
	_hdrMaterial->getParameterTable()->setVectorArray ("g_avSampleOffsets", avSampleOffsets, MAX_SAMPLES);
	_hdrMaterial->getParameterTable()->setVectorArray ("g_avSampleWeights", avSampleWeights, MAX_SAMPLES);
	_hdrMaterial->getParameterTable()->setTexture ("tex0", _bloomSource.get());
	device->setRenderTarget (0, _bloomTextures[2].get());

	//--- added ---//
	device->setViewport (0, ATOM_Rect2Di(0, 0,_bloomTextures[2]->getWidth(), _bloomTextures[2]->getHeight()));

	beginScissorTest (device, rectDest.point.x, rectDest.point.y, rectDest.size.w, rectDest.size.h);
	drawFullScreenQuad (device, coords);
	endScissorTest (device);

	// horizontal
	getSampleOffsets_Bloom (_bloomTextures[2]->getWidth(), afSampleOffsets, avSampleWeights, 3.f, 2.f);
	for (int i = 0; i < MAX_SAMPLES; ++i)
	{
		avSampleOffsets[i].set (afSampleOffsets[i], 0.f);
	}

	_hdrMaterial->setActiveEffect ("Bloom");
	_hdrMaterial->getParameterTable()->setVectorArray ("g_avSampleOffsets", avSampleOffsets, MAX_SAMPLES);
	_hdrMaterial->getParameterTable()->setVectorArray ("g_avSampleWeights", avSampleWeights, MAX_SAMPLES);
	_hdrMaterial->getParameterTable()->setTexture ("tex0", _bloomTextures[2].get());
	device->setRenderTarget (0, _bloomTextures[1].get());

	//--- added ---//
	device->setViewport (0, ATOM_Rect2Di(0, 0,_bloomTextures[1]->getWidth(), _bloomTextures[1]->getHeight()));

	beginScissorTest (device, rectDest.point.x, rectDest.point.y, rectDest.size.w, rectDest.size.h);
	drawFullScreenQuad (device, coords);
	endScissorTest (device);

	// vertical
	getSampleOffsets_Bloom (_bloomTextures[1]->getHeight(), afSampleOffsets, avSampleWeights, 3.f, 2.f);
	for (int i = 0; i < MAX_SAMPLES; ++i)
	{
		avSampleOffsets[i].set (0.f, afSampleOffsets[i]);
	}

	rectSrc.point.x = 1;
	rectSrc.point.y = 1;
	rectSrc.size.w = _bloomTextures[1]->getWidth()-2;
	rectSrc.size.h = _bloomTextures[1]->getHeight()-2;

	getTextureCoords (_bloomTextures[1].get(), &rectSrc, _bloomTextures[0].get(), 0, &coords);

	_hdrMaterial->setActiveEffect ("Bloom");
	_hdrMaterial->getParameterTable()->setVectorArray ("g_avSampleOffsets", avSampleOffsets, MAX_SAMPLES);
	_hdrMaterial->getParameterTable()->setVectorArray ("g_avSampleWeights", avSampleWeights, MAX_SAMPLES);
	device->setRenderTarget (0, _bloomTextures[0].get());
	device->setViewport (0, ATOM_Rect2Di(0, 0, _bloomTextures[0]->getWidth(), _bloomTextures[0]->getHeight()));
	_hdrMaterial->getParameterTable()->setTexture ("tex0", _bloomTextures[1].get());
	drawFullScreenQuad (device, coords);

#if DEBUG_HDR
	if( bsaveall )
	{
		device->setRenderTarget (0, 0);
		_bloomTextures[0]->saveToFile ("/BloomTexture0.png");
		_bloomTextures[1]->saveToFile ("/BloomTexture1.png");
		_bloomTextures[2]->saveToFile ("/BloomTexture2.png");
	}
#endif

	return true;

	/*
	_bloomTextures[0]->clear (0.f, 0.f, 0.f, 0.f);

	return true;
	*/
}

bool HDR_Pipeline::drawFullScreenQuad (ATOM_RenderDevice *device, const HDR_Pipeline::CoordRect &coords)
{
	return drawFullScreenQuad (device, coords.leftU, coords.topV, coords.rightU, coords.bottomV);
}

bool HDR_Pipeline::drawFullScreenQuad (ATOM_RenderDevice *device, float leftU, float topV, float rightU, float bottomV)
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

	unsigned numPasses = _hdrMaterial->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (_hdrMaterial->beginPass (device, pass))
		{
			device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_STRIP, 4, 4, ATOM_VERTEX_ATTRIB_COORD_XYZRHW|ATOM_VERTEX_ATTRIB_TEX1_2, sizeof(ScreenVertex), quad, indices);
			_hdrMaterial->endPass (device, pass);
		}
	}
	_hdrMaterial->end (device);

	return true;
}

bool HDR_Pipeline::DebugAllRT(ATOM_RenderDevice *device)
{
	float left = -0.5f;
	float top = -0.5f;
	float width = _source->getWidth();
	float height = _source->getHeight();

	const int num = 12;
	const int col = 4;
	const int row = 3;
	const float width_rt = width / col;
	const float height_rt = height / row;

	struct RT
	{
		ATOM_AUTOREF(ATOM_Texture) rt;
		bool blumin;
	};
	RT RTs[12] = {	{_sourceScaled,false},
					{_brightPass,false}, 
					{_starSource,false},
					{_bloomSource,false},
					{_toneMapTextures[3],true},
					{_toneMapTextures[2],true}, 
					{_toneMapTextures[1],true}, 
					{_toneMapTextures[0],true},
					{_adaptedLumCurrent,true},
					{_bloomTextures[2],false},
					{_bloomTextures[1],false},
					{_bloomTextures[0],false} };
	for( int i = 0; i < row; ++i)
	{
		for( int j = 0; j < col; ++j )
		{
			int index = i * col + j;
			float l = left + j * width_rt;
			float t = top + i * height_rt;
			float r = l + width_rt;
			float b = t + height_rt;
			DebugRT(device,RTs[index].rt.get(),l,t,r,b,RTs[index].blumin);
		}
	}

	return true;
}
bool HDR_Pipeline::DebugRT( ATOM_RenderDevice *device, ATOM_Texture *RT, float left, float top, float right, float bottom, bool blumin )
{
	if(!RT)
		return false;

	if( blumin )
		_hdrMaterial->setActiveEffect ("DEBUG_RT_LUMIN");
	else
		_hdrMaterial->setActiveEffect ("DEBUG_RT");

	_hdrMaterial->getParameterTable()->setTexture ("tex0", RT);

	float width = right - left;
	float height = bottom - top;

	//device->setViewport (0, ATOM_Rect2Di(left, top, width, height));

	

	/*float width = RT->getWidth();
	float height = RT->getHeight();*/

	

	struct ScreenVertex
	{
		float x, y, z, rhw;
		float u, v;
	};

	ScreenVertex quad[4] = {
		{left, top, 0.5f, 1.f, 0, 0},
		{right, top, 0.5f, 1.f, 1, 0},
		{left, bottom, 0.5f, 1.f, 0, 1},
		{right, bottom, 0.5f, 1.f, 1, 1}
	};

	unsigned short indices[4] = { 0, 1, 2, 3 };

	unsigned numPasses = _hdrMaterial->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (_hdrMaterial->beginPass (device, pass))
		{
			device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_STRIP, 4, 4, ATOM_VERTEX_ATTRIB_COORD_XYZRHW|ATOM_VERTEX_ATTRIB_TEX1_2, sizeof(ScreenVertex), quad, indices);
			_hdrMaterial->endPass (device, pass);
		}
	}
	_hdrMaterial->end (device);

	return true;
}
void HDR_Pipeline::setUseFilmicMapping(bool enable)
{
	_use_filmic_mapping = enable;
}
bool HDR_Pipeline::getUseFilmicMapping(void) const
{
	return _use_filmic_mapping;
}
void HDR_Pipeline::setHDRFilmCurveParams(	float filmCurveShoulder,
											float filmCurveMiddleTone,
											float filmCurveToe, 
											float filmCurveWhitePoint	)
{
	_filmCurveShoulder		= filmCurveShoulder;
	_filmCurveMiddleTone	= filmCurveMiddleTone;
	_filmCurveToe			= filmCurveToe;
	_filmCurveWhitePoint	= filmCurveWhitePoint;
}
void HDR_Pipeline::getHDRFilmCurveParams(	float & filmCurveShoulder,
											float & filmCurveMiddleTone,
											float & filmCurveToe, 
											float & filmCurveWhitePoint	) const
{
	filmCurveShoulder	= _filmCurveShoulder;
	filmCurveMiddleTone = _filmCurveMiddleTone;
	filmCurveToe		= _filmCurveToe;
	filmCurveWhitePoint = _filmCurveWhitePoint;
}
void HDR_Pipeline::setHDRColorParams(float HDRSaturate,float HDRContrast,ATOM_Vector3f HDRColorBalance)
{
	_HDRSaturate		= HDRSaturate;
	_HDRContrast		= HDRContrast;
	_HDRColorBalance	= HDRColorBalance;
}
void HDR_Pipeline::getHDRColorParams(float & HDRSaturate,float & HDRContrast,ATOM_Vector3f & HDRColorBalance) const
{
	HDRSaturate		= _HDRSaturate;
	HDRContrast		= _HDRContrast;
	HDRColorBalance	= _HDRColorBalance;
}
//----------------------//

bool HDR_Pipeline::getTextureCoords( ATOM_Texture *srcTex, const ATOM_Rect2Di *pRectSrc, ATOM_Texture *destTex, const ATOM_Rect2Di *pRectDest, CoordRect* pCoords)
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

void HDR_Pipeline::setMiddleGray (float value)
{
	_keyValue = value;
}

float HDR_Pipeline::getMiddleGray (void) const
{
	return _keyValue;
}

void HDR_Pipeline::setBrightPassThreshold (float value)
{
	_brightPassThreshold = value;
}

float HDR_Pipeline::getBrightPassThreshold (void) const
{
	return _brightPassThreshold;
}

void HDR_Pipeline::setBrightPassOffset (float value)
{
	_brightPassOffset = value;
}

float HDR_Pipeline::getBrightPassOffset (void) const
{
	return _brightPassOffset;
}

//--- wangjian added ---//
void HDR_Pipeline::setHDREnable(bool enable)
{
	_hdr_enabled = enable;
}
bool HDR_Pipeline::getHDREnable(void) const
{
	return _hdr_enabled;
}
//----------------------//

void HDR_Pipeline::processHDRPipeLine (ATOM_RenderDevice *device, bool RGBL)
{
	//--- wangjian modified ---//
	if(_hdr_enabled)
	{
		SaveAllRT();

		setUseFilmicMapping(ATOM_RenderSettings::isFilmicMappingEnabled());

		_adaptationInvalid = true;

		/////////////////////////////////////////////////////////////////

		sceneToSceneScaled (device);

		if (ATOM_RenderSettings::isToneMappingEnabled())
		{
			measureLuminance (device);
		}

		if (_adaptationInvalid)
		{
			_adaptationInvalid = false;

			calculateAdaptation (device);
		}

		sceneScaledToBrightPass (device);
		brightPassToStarSource (device);
		starSourceToBloomSource (device);
		renderBloom (device);
		renderStar (device);

		_hdrMaterial->setActiveEffect (RGBL ? "FinalScenePassRGBL" : "FinalScenePass");
		_hdrMaterial->getParameterTable()->setFloat ("g_fMiddleGray", _keyValue);
		_hdrMaterial->getParameterTable()->setTexture ("tex0", _source.get());
		_hdrMaterial->getParameterTable()->setTexture ("tex1", _bloomTextures[0].get());
		_hdrMaterial->getParameterTable()->setTexture ("tex2", _starTextures[0].get());
		_hdrMaterial->getParameterTable()->setTexture ("tex3", _adaptedLumCurrent.get());
		_hdrMaterial->getParameterTable()->setFloat ("g_bEnableToneMap", ATOM_RenderSettings::isToneMappingEnabled()?1.f:0.f);
		_hdrMaterial->getParameterTable()->setFloat ("g_bEnableBlueShift", _blueShift?1.f:0.f);
		_hdrMaterial->getParameterTable()->setFloat ("g_bEnableFilmicMapping", 
													ATOM_RenderSettings::isToneMappingEnabled() && _use_filmic_mapping ? 1.f : 0.f );

		
	}
	else
	{
		_hdrMaterial->setActiveEffect (RGBL?"FinalScenePassNoHDRRGBL":"FinalScenePassNoHDR");
		_hdrMaterial->getParameterTable()->setTexture ("tex0", _source.get());
	}

	device->setRenderTarget (0, _dest.get());

	//--- added ---//
	device->setViewport (0, ATOM_Rect2Di(0, 0,_dest->getWidth(), _dest->getHeight()));

	drawFullScreenQuad (device, 0.f, 0.f, 1.f, 1.f);

	if( bsaveall )
	{
		bsaveall = false;
		ATOM_RenderSettings::saveHDRRTs(false);
	}
	//----------------------------------------------------------------------//
}

void HDR_Pipeline::beginScissorTest (ATOM_RenderDevice *device, int scissorX, int scissorY, int scissorW, int scissorH)
{
	_savedScissorTest = device->isScissorTestEnabled (0);
	device->getScissorRect (0, &_savedScissorRect[0], &_savedScissorRect[1], &_savedScissorRect[2], &_savedScissorRect[3]);
	device->enableScissorTest (0, true);
	device->setScissorRect (0, scissorX, scissorY, scissorW, scissorH);
}

void HDR_Pipeline::endScissorTest (ATOM_RenderDevice *device)
{
	device->enableScissorTest (0, _savedScissorTest);
	device->setScissorRect (0, _savedScissorRect[0], _savedScissorRect[1], _savedScissorRect[2], _savedScissorRect[3]);
}

//--- wangjian added ---//
ATOM_Texture * HDR_Pipeline::getAdaptedLumCurrent(void)
{
	return _adaptedLumCurrent.get();
}
void HDR_Pipeline::SaveAllRT()
{
	// 显示所有的RT
	if( ATOM_RenderSettings::isDebugHdrRTsEnabled() )
	{
		bshowall = true;
	}
	else
	{
		bshowall = false;
	}

	// 保存所有的RT
	if( ATOM_RenderSettings::isSaveHDRRTsEnabled() )
	{
		bsaveall = true;
	}
}
//----------------------//

