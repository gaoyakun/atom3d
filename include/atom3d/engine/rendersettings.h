#ifndef __ATOM3D_ENGINE_RENDERSETTINGS_H
#define __ATOM3D_ENGINE_RENDERSETTINGS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "ATOM_render.h"
#include "basedefs.h"
#include "timevalue.h"

class ATOM_ENGINE_API ATOM_RenderSettings
{
public:
	enum Quality
	{
		QUALITY_HIGH,
		QUALITY_MEDIUM,
		QUALITY_LOW
	};

public:
	struct RenderCaps
	{
		bool supportShadow;
		bool supportInstancing;
		bool supportDeferredShading;
		bool supportHDRDeferredShading;
		bool supportMtAnimation;
		bool supportSRGBRead;
		bool mrtIndependentBitDepth;
		int numberMRT;
		int shaderModel;
		int shaderModelExt;
		int maxVertexShaderConst;
	};

public:
	// initialization
	static void initialize (ATOM_RenderDevice *device, bool forceff, int argc, char **argv);

	// FSAA
	static void enableFSAA (bool enable);
	static bool isFSAAEnabled (void);

	// shadow
	static void enableShadow (bool enabled);
	static bool isShadowEnabled (void);
	static void setShadowMapSize (unsigned size);
	static unsigned getShadowMapSize (void);
	static void setShadowRegion (float size);
	static float getShadowRegion (void);
	static int getShadowMapFormat (void);
	static void setShadowQuality (ATOM_RenderSettings::Quality val);
	static ATOM_RenderSettings::Quality getShadowQuality (void);

	// misc
	static const RenderCaps & getRenderCaps (void);
	static ATOM_PixelFormat getGBufferFormatN (void);
	static ATOM_PixelFormat getGBufferFormatC (void);
	static bool isMultithreadedAnimationEnabled (void);
	static void enableMultithreadedAnimation (bool b);
	static void enableAutoIMEChange (bool b);
	static bool isAutoIMEChangeEnabled (void);
	static void setTopMostEnvMap (ATOM_Texture *texture);
	static ATOM_Texture *getTopMostEnvMap (void);
	static void enableOctree (bool b);
	static bool isOctreeEnabled (void);

	// Sky
	static void enableCloudCover (bool enable);
	static bool isCloudCoverEnabled (void);

	// quality control
	static void setTextureSizeScale (float scale);
	static float getTextureSizeScale (void);
	static void setMaxTextureSizeLimit (unsigned size);
	static unsigned getMaxTextureSizeLimit (void);
	static void setMinTextureSizeLimit (unsigned size);
	static unsigned getMinTextureSizeLimit (void);
	static void enableDetailedTerrain (bool enable);
	static bool detailedTerrainEnabled (void);
	static void setWaterQuality (ATOM_RenderSettings::Quality val);
	static ATOM_RenderSettings::Quality getWaterQuality (void);
	static void setAtmosphereQuality (ATOM_RenderSettings::Quality val);
	static ATOM_RenderSettings::Quality getAtmosphereQuality (void);
	static void enableOcclusionTest (bool enable);
	static bool occlusionTestEnabled (void);
	static void setDeferredBufferScale (float f);
	static float getDeferredBufferScale (void);

	// for debugging
	static void enableDebugDeferredDepth (bool b);
	static bool isDebugDeferredDepthEnabled (void);
	static void enableDebugDeferredNormals (bool b);
	static bool isDebugDeferredNormalsEnabled (void);
	static void enableDebugDeferredColors (bool b);
	static bool isDebugDeferredColorsEnabled (void);
	//---- wangjian added ----//
	static void enableDebugHalfLambertBuffer (bool b);
	static bool isDebugHalfLambertBufferEnabled (void);
	static void enableDebugBloomBuffer (bool b);
	static bool isDebugBloomBufferEnabled (void);
	static void enableHDRRendering (bool b);
	static bool isHDRRenderingEnabled(void);
	static void enableFloatShading (bool b);
	static bool isFloatShadingEnabled(void);
	static void enableDebugHdrLumin (bool b);
	static bool isDebugHdrLuminEnabled (void);
	static void enableDebugHdrRTs (bool b);
	static bool isDebugHdrRTsEnabled (void);
	static void enableFilmicMapping (bool b);
	static bool isFilmicMappingEnabled(void);
	static void saveHDRRTs (bool b);
	static bool isSaveHDRRTsEnabled(void);
	static void enableStateSorting(bool b);
	static bool isStateSortingEnabled(void);
	static void setShadowDrawItemUpdateFrameCache(int cacheFrame);
	static int  getShadowDrawItemUpdateFrameCache(void);
	static void setSceneDrawItemUpdateFrameCache(int cacheFrame);
	static int  getSceneDrawItemUpdateFrameCache(void);
	static void setCullUpdateViewDirThreshold(float thresh);
	static float  getCullUpdateViewDirThreshold(void);
	static void setCullUpdateViewPosThreshold(float thresh);
	static float  getCullUpdateViewPosThreshold(void);
	// DX9几何体实例化相关
	static void		setUseHWInstancing(bool use);
	static bool		isUseHWInstancing(void);
	static void		setUseInstancingStaticBuffer(bool use);
	static bool		isUseInstancingStaticBuffer(void);
	static void		setInstancingSBCountThreshold(unsigned thresh);
	static unsigned getInstancingSBCountThreshold(void);
	static void		setShadowMaskSizeScale(unsigned scale);
	static unsigned getShadowMaskSizeScale(void);
	static void     enableStreamingLog(bool enable);

	static bool     isPostEffectEnabled(void);
	static void     enablePostEffect(bool enable);
	static bool		isRainEnabled(void);
	static void		enableRain(bool enable);
	static bool		isRainLayerEnabled(void);
	static void		enableRainLayer(bool enable);
	static bool		isBloomEnabled(void);
	static void		enableBloom(bool enable);
	static bool		isDofEnabled(void);
	static void		enableDof(bool enable);
	static bool		isSunShaftEnabled(void);
	static void     enableSunShaft(bool enable);
	static bool		isTerrDetailNormalEnabled(void);
	static void		enableTerrDetailNormal(bool enable);

	// light toggle
	static bool isDirectionalLightTypeEnabled(void);
	static void enableDirectionalLightType(bool enable);
	static bool isPointLightTypeEnabled(void);
	static void enablePointLightType(bool enable);
	static bool isAllLightTypeEnabled(void);
	static void enableAllLightType(bool enable);

	static void enableSSSRendering (bool b);
	static bool isSSSRenderingEnabled(void);

	static void enableModelCompressedFormat (bool b);
	static bool isModelCompressedFormatEnabled(void);

	static void enableEditorMode (bool enable);
	static bool isEditorModeEnabled (void);

	static void enableGrassTerrainBlend(bool enable);
	static bool isGrassTerrainBlendEnabled(void);

	static void enableShaderBinaryCompile (bool enable);
	static bool isShaderBinaryCompileEnabled (void);
	//------------------------//

	static void enableDebugDeferredLightBuffer (bool b);
	static bool isDebugDeferredLightBufferEnabled (void);
	static void enableDebugShadowMap (bool b);
	static bool isDebugShadowMapEnabled (void);
	static void enableDebugShadowMask (bool b);
	static bool isDebugShadowMaskEnabled (void);
	static void enableAtmosphereEffect (bool b);
	static bool isAtmosphereEffectEnabled (void);
	static void enableToneMapping (bool b);
	static bool isToneMappingEnabled (void);

	// light time control
	static void setLightTimeFactors (const ATOM_TimeValueT<ATOM_Vector4f> &factors);
	static const ATOM_Vector4f &getLightTimeFactor (void);

	// SSAO settings
	static void setSSAOScale (float val);
	static float getSSAOScale (void);
	static void setSSAOBias (float val);
	static float getSSAOBias (void);
	static void setSSAORadius (float val);
	static float getSSAORadius (void);
	static void setSSAOIntensity (float val);
	static float getSSAOIntensity (void);
	static void enableSSAO (bool enable);
	static bool isSSAOEnabled (void);
	static void enableSSAODownsample (bool enable);
	static bool isSSAODownsampleEnabled (void);
	static void enableSSAOBlur (bool enable);
	static bool isSSAOBlurEnabled (void);
	static void enableDebugSSAO (bool enable);
	static bool isDebugSSAOEnabled (void);
	static void setSSAODownsampleFactor (float val);
	static float getSSAODownsampleFactor (void);

	// optimizations
	static void setMinScreenContribution (float value);
	static float getMinScreenContribution (void);
	static bool isFixedFunction (void);

	//---------------------------------------------------------//
	// wangjian added
	static bool		isNonDeferredShading(void);
	static void		setNonDeferredShading(bool b);
	static ATOM_STRING getCoreMaterialName(const char* material);
	//---------------------------------------------------------//

	// internal use
	static void setDepthScale (float val);
	static float getDepthScale (void);

private:
	static bool _enableShadow;
	static float _shadowRegion;
	static unsigned _shadowMapSize;
	static RenderCaps _renderCaps;
	static float _textureSizeScale;
	static unsigned _maxTextureSizeLimit;
	static unsigned _minTextureSizeLimit;
	static bool _detailTerrainEnabled;
	static bool _occlusionTestEnabled;
	static bool _multithreadedAnimation;
	static bool _debugDeferredDepth;
	static bool _debugDeferredNormals;
	static bool _debugDeferredColors;
	//--- wangjian added ---//
	static bool _enableHDRRendering;
	static bool _enableFloatShading;
	static bool _debugHalfLambertBuffer;
	static bool _debugBloomBuffer;
	static bool _debugHdrLumin;
	static bool _debugHDRRTs;
	static bool _useFilmicMapping;
	static bool _saveHDRRTs;
	static bool _stateSorting;
	static int  _shadowDrawItemUpdateFrameCache;
	static int  _sceneDrawItemUpdateFrameCache;
	static float _cullUpdateViewDirThreshold;
	static float _cullUpdateViewPosThreshold;
	static bool  _bUseHWInstancing;					// use hw geometry instancing
	static bool	 _bUseInstancingStaticBuffer;		// use static buffer for instance data
	static unsigned _InstancingSBCountThreshold;	// if use static buffer, then the max instance count
	static unsigned _shadowMaskScale;				// 

	static bool		_postEffectEnabled;				// enable post effect
	static bool		_rainEnabled;					// enable rain effect
	static bool		_rainLayerEnabled;				// enable rain effect
	static bool		_bloomEnabled;					// enable bloom effect
	static bool		_dofEnabled;					// enable dof effect
	static bool		_sunShaftEnabled;				// enable sun shaft effect

	enum eLIGHTTYPETOGGLE
	{
		LIGHT_DIRECTIONAL_TOGGLE = 0X1,
		LIGHT_POINT_TOGGLE = 0X2,
		LIGHT_ALL_TOGGLE = 0X3,
	};
	static unsigned	_enableLight;					// enable light [ 1 : directional / 2 ：point ]

	static bool		_enableTerrainDetailNormal;		//	enable terrain detail normal
	
	static bool		_enableSSS;

	static bool		_enableModelCompression;		// enable compressed model info format

	static bool		_editorMode;					// enable editor mode

	static bool		_grassTerrainBlend;				// enable grass terrain blending

	static bool		_enableShaderBinaryCompile;		// eanble shader compileing( fxb generation )

	static bool		_forceNonDeferredShading;		// force disable the deferred
	//----------------------//

	static bool _enableOctree;
	static bool _debugDeferredLightBuffer;
	static bool _enableAtmosphereEffect;
	static bool _enableSSAO;
	static bool _enableSSAODownsample;
	static bool _enableSSAOBlur;
	static bool _debugSSAO;
	static bool _debugShadowMap;
	static bool _debugShadowMask;
	static bool _enableToneMap;
	static float _ssaoDownsampleScale;
	static float _deferredBufferScale;
	static bool _enableFSAA;
	static bool _enableCloudCover;
	static ATOM_PixelFormat _gbufferFormatN;
	static ATOM_PixelFormat _gbufferFormatC;
	static ATOM_TimeValueT<ATOM_Vector4f> _lightTimeFactors;
	static float _minScreenContribution;
	static float _depthScale;
	static Quality _waterQuality;
	static Quality _shadowQuality;
	static Quality _atmosphereQuality;
	static bool _forceff;
	static bool _autoIMEChange;
	static ATOM_AUTOREF(ATOM_Texture) _topmostEnvMap;
	static float _ssaoScale;
	static float _ssaoBias;
	static float _ssaoRadius;
	static float _ssaoIntensity;
};

#endif // __ATOM3D_ENGINE_RENDERSETTINGS_H
