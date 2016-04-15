#ifndef _POST_EFFECT_BLOOM_H_
#define _POST_EFFECT_BLOOM_H_

#if _MSC_VER > 1000
# pragma once
#endif

#include "posteffectchain.h"

#define _GAUSSIAN_TAP_COUNT_FINAL 5/*9*//*15*/
#define _BLOOM_FILTER_PASS 3

class ATOM_ENGINE_API ATOM_BLOOMEffect: public ATOM_PostEffect
{
	ATOM_CLASS(engine, ATOM_BLOOMEffect, ATOM_BLOOMEffect)

public:
	//! 构造函数
	ATOM_BLOOMEffect (void);

	//! 析构函数
	virtual ~ATOM_BLOOMEffect (void);

	// enable
	virtual void enable (int b);
	virtual int isEnabled (void) const;

	virtual bool init(ATOM_RenderDevice *device);
	virtual bool destroy();

public:
	virtual bool render (ATOM_RenderDevice *device);

private:

	struct CoordRect
	{
		float leftU, topV;
		float rightU, bottomV;
	};

	void sceneToSceneScaled (ATOM_RenderDevice *device);
	void sceneScaledToBloomSource (ATOM_RenderDevice *device);
	void genBloom(ATOM_RenderDevice *device);

	float GaussianDistribution( float x, float y, float rho );
	bool getTextureCoords( ATOM_Texture *srcTex, const ATOM_Rect2Di *pRectSrc, ATOM_Texture *destTex, const ATOM_Rect2Di *pRectDest, CoordRect* pCoords);
	bool getSampleOffsets_DownScale2x2 (unsigned width, unsigned height, ATOM_Vector2f sampleOffsets[]);
	bool getSampleOffsets_DownScale4x4 (unsigned width, unsigned height, ATOM_Vector2f sampleOffsets[]);
	bool getSampleOffsets_GaussBlur5x5 (unsigned texWidth, unsigned texHeight, ATOM_Vector2f *texCoordOffset, ATOM_Vector4f *sampleWeight, float multiplier = 1.0f);
	bool getSampleOffsets_Bloom (unsigned texSize, float texCoordOffset[15], ATOM_Vector4f *colorWeight, float deviation, float multiplier);

	bool drawFullScreenQuad (ATOM_RenderDevice *device, float leftU, float topV, float rightU, float bottomV);
	bool drawFullScreenQuad (ATOM_RenderDevice *device, const CoordRect &coords);

	bool clearTexture (ATOM_Texture *texture);

	void stretchrect (ATOM_RenderDevice *device,ATOM_Texture *pSrc,ATOM_Texture *pDest,bool bigDownSample);

	bool createRT(ATOM_RenderDevice *device);
	bool createMaterial(ATOM_RenderDevice *device);

	ATOM_AUTOPTR(ATOM_Material) _material;
	bool _materialError;

	ATOM_AUTOREF(ATOM_Texture) _source;				// source
	ATOM_AUTOREF(ATOM_Texture) _bloomTexs[2];		// for pingpong

	unsigned _rtWidth;
	unsigned _rtHeight;

	unsigned _cropWidth;
	unsigned _cropHeight;

	float	_brightPassThreshold;
	float	_brightPassOffset;
	float	_bloomScale;

	ATOM_Vector2f avSampleOffsets_horizontal[_GAUSSIAN_TAP_COUNT_FINAL];
	ATOM_Vector4f avSampleWeights_horizontal[_GAUSSIAN_TAP_COUNT_FINAL];
	ATOM_Vector2f avSampleOffsets_vertical[_GAUSSIAN_TAP_COUNT_FINAL];
	ATOM_Vector4f avSampleWeights_vertical[_GAUSSIAN_TAP_COUNT_FINAL];

	bool 	_bsaveall;

};

#endif
