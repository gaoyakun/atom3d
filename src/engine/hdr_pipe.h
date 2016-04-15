#ifndef __ATOM3D_ENGINE_HDR_PIPE_H
#define __ATOM3D_ENGINE_HDR_PIPE_H

#include <ATOM_render.h>
#include "basedefs.h"

#define MAX_SAMPLES           16      // Maximum number of texture grabs

#define EMISSIVE_COEFFICIENT  39.78f  // Emissive color multiplier for each lumen
// of light intensity                                    
#define NUM_TONEMAP_TEXTURES  4       // Number of stages in the 4x4 down-scaling 
// of average luminance textures
#define NUM_STAR_TEXTURES     12      // Number of textures used for the star
// post-processing effect
#define NUM_BLOOM_TEXTURES    3       // Number of textures used for the bloom

struct BloomParams
{
	float blur_deviation;
	float blur_multiplier;
	float bright_threshold;
	float bright_offset;
};

class HDR_Pipeline
{
	struct CoordRect
	{
		float leftU, topV;
		float rightU, bottomV;
	};

public:
	HDR_Pipeline (void);
	~HDR_Pipeline (void);

public:
	void setSource (ATOM_RenderDevice *device, ATOM_Texture *hdrSource);
	void setDestination (ATOM_RenderDevice *device, ATOM_Texture *ldrDestination);
	void processHDRPipeLine (ATOM_RenderDevice *device, bool RGBL);
	void setMiddleGray (float value);
	float getMiddleGray (void) const;
	void setBrightPassThreshold (float value);
	float getBrightPassThreshold (void) const;
	void setBrightPassOffset (float value);
	float getBrightPassOffset (void) const;
	//--- wangjian added ---//
	virtual void setHDREnable(bool enable);
	virtual bool getHDREnable(void) const;
	ATOM_Texture * getAdaptedLumCurrent(void);
	void SaveAllRT();
	bool DebugAllRT(ATOM_RenderDevice *device);
	bool bsaveall;
	bool bshowall;
	void setUseFilmicMapping(bool enable);
	bool getUseFilmicMapping(void) const;
	void setHDRFilmCurveParams(float filmCurveShoulder,float filmCurveMiddleTone,float filmCurveToe, float filmCurveWhitePoint);
	void getHDRFilmCurveParams(float & filmCurveShoulder,float & filmCurveMiddleTone,float & filmCurveToe, float & filmCurveWhitePoint) const;
	void setHDRColorParams(float HDRSaturate,float HDRContrast,ATOM_Vector3f HDRColorBalance);
	void getHDRColorParams(float & HDRSaturate,float & HDRContrast,ATOM_Vector3f & HDRColorBalance) const;
	//----------------------//

private:
	bool sceneToSceneScaled (ATOM_RenderDevice *device);
	bool sceneScaledToBrightPass (ATOM_RenderDevice *device);
	bool brightPassToStarSource (ATOM_RenderDevice *device);
	bool starSourceToBloomSource (ATOM_RenderDevice *device);
	bool getSampleOffsets_GaussBlur5x5 (unsigned texWidth, unsigned texHeight, ATOM_Vector2f *texCoordOffset, ATOM_Vector4f *sampleWeight, float multiplier = 1.f);
	bool getSampleOffsets_Bloom (unsigned texSize, float texCoordOffsets[15], ATOM_Vector4f *colorWeight, float deviation, float multiplier = 1.f);
	bool getSampleOffsets_Star (unsigned texSize, float texCoordOffsets[15], ATOM_Vector4f *colorWeight, float deviation);
	bool getSampleOffsets_DownScale4x4 (unsigned width, unsigned height, ATOM_Vector2f sampleOffsets[]);
	bool getSampleOffsets_DownScale2x2 (unsigned width, unsigned height, ATOM_Vector2f sampleOffsets[]);
	bool measureLuminance (ATOM_RenderDevice *device);
	bool calculateAdaptation (ATOM_RenderDevice *device);
	bool renderStar (ATOM_RenderDevice *device);
	bool renderBloom (ATOM_RenderDevice *device);
	bool drawFullScreenQuad (ATOM_RenderDevice *device, float leftU, float topV, float rightU, float bottomV);
	bool drawFullScreenQuad (ATOM_RenderDevice *device, const CoordRect &coords);
	bool getTextureCoords( ATOM_Texture *srcTex, const ATOM_Rect2Di *pRectSrc, ATOM_Texture *destTex, const ATOM_Rect2Di *pRectDest, CoordRect* pCoords);
	bool clearTexture (ATOM_Texture *texture);
	void beginScissorTest (ATOM_RenderDevice *device, int scissorX, int scissorY, int scissorW, int scissorH);
	void endScissorTest (ATOM_RenderDevice *device);

	
	bool DebugRT( ATOM_RenderDevice *device,ATOM_Texture *RT, float left, float top, float right, float bottom, bool blumin );

private:
	ATOM_AUTOREF(ATOM_Texture) _source;
	ATOM_AUTOREF(ATOM_Texture) _dest;
	ATOM_AUTOREF(ATOM_Texture) _sourceScaled;
	ATOM_AUTOREF(ATOM_Texture) _brightPass;
	ATOM_AUTOREF(ATOM_Texture) _adaptedLumCurrent;
	ATOM_AUTOREF(ATOM_Texture) _adaptedLumLast;
	ATOM_AUTOREF(ATOM_Texture) _starSource;
	ATOM_AUTOREF(ATOM_Texture) _bloomSource;
	ATOM_AUTOREF(ATOM_Texture) _bloomTextures[NUM_BLOOM_TEXTURES];
	ATOM_AUTOREF(ATOM_Texture) _starTextures[NUM_STAR_TEXTURES];
	ATOM_AUTOREF(ATOM_Texture) _toneMapTextures[NUM_TONEMAP_TEXTURES];
	ATOM_AUTOPTR(ATOM_Material) _hdrMaterial;

	unsigned _lastSourceWidth;
	unsigned _lastSourceHeight;
	bool _toneMap;
	bool _blueShift;
	bool _adaptationInvalid;
	float _keyValue;
	float _brightPassThreshold;
	float _brightPassOffset;
	unsigned _cropWidth;
	unsigned _cropHeight;
	bool _savedScissorTest;
	int _savedScissorRect[4];
	//--- wangjian added ---//
	bool _hdr_enabled;
	bool _use_filmic_mapping;
	float _filmCurveShoulder;
	float _filmCurveMiddleTone;
	float _filmCurveToe;
	float _filmCurveWhitePoint;
	float _HDRSaturate;
	float _HDRContrast;
	ATOM_Vector3f _HDRColorBalance;
	//----------------------//
};

#endif // __ATOM3D_ENGINE_HDR_PIPE_H
