#include "StdAfx.h"
#include "rendersettings.h"
bool ATOM_RenderSettings::_enableOctree = true;
bool ATOM_RenderSettings::_enableShadow = true;
float ATOM_RenderSettings::_shadowRegion = 3000.f;
unsigned ATOM_RenderSettings::_shadowMapSize = 2048;
ATOM_RenderSettings::RenderCaps ATOM_RenderSettings::_renderCaps;
float ATOM_RenderSettings::_textureSizeScale = 1.f;
unsigned ATOM_RenderSettings::_maxTextureSizeLimit = 0;
unsigned ATOM_RenderSettings::_minTextureSizeLimit = 0;
bool ATOM_RenderSettings::_detailTerrainEnabled = true;		// wangjian modified for test
bool ATOM_RenderSettings::_occlusionTestEnabled = true;
bool ATOM_RenderSettings::_multithreadedAnimation = true;
bool ATOM_RenderSettings::_debugDeferredDepth = false;
bool ATOM_RenderSettings::_debugDeferredNormals = false;
bool ATOM_RenderSettings::_debugDeferredColors = false;
//--- wangjian added ---//
bool		ATOM_RenderSettings::_debugHalfLambertBuffer			= false;
bool		ATOM_RenderSettings::_debugBloomBuffer					= false;
bool		ATOM_RenderSettings::_enableHDRRendering				= false;
bool		ATOM_RenderSettings::_enableFloatShading				= false;
bool		ATOM_RenderSettings::_debugHdrLumin						= false;
bool		ATOM_RenderSettings::_debugHDRRTs						= false;
bool		ATOM_RenderSettings::_useFilmicMapping					= false;
bool		ATOM_RenderSettings::_saveHDRRTs						= false;
bool		ATOM_RenderSettings::_stateSorting						= true;
int			ATOM_RenderSettings::_shadowDrawItemUpdateFrameCache	= 6;
int			ATOM_RenderSettings::_sceneDrawItemUpdateFrameCache		= 0;
float		ATOM_RenderSettings::_cullUpdateViewDirThreshold		= 0.995f;
float		ATOM_RenderSettings::_cullUpdateViewPosThreshold		= 10.0f;
bool		ATOM_RenderSettings::_bUseHWInstancing					= false;		// use hardware geometry instancing( sm3.0 )
bool		ATOM_RenderSettings::_bUseInstancingStaticBuffer		= true;		// use static buffer for instance data
unsigned	ATOM_RenderSettings::_InstancingSBCountThreshold		= 50;		// if use static buffer, then the max instance count
unsigned	ATOM_RenderSettings::_shadowMaskScale					= 1;		// the size scale of shadow mask map

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool		ATOM_RenderSettings::_postEffectEnabled					= true;		
bool		ATOM_RenderSettings::_rainEnabled						= true;		// enable rain effect
bool		ATOM_RenderSettings::_rainLayerEnabled					= false;	// enable rain layer effect
bool		ATOM_RenderSettings::_bloomEnabled						= true;		// enable bloom effect 
bool		ATOM_RenderSettings::_dofEnabled						= true;		// enable dof
bool		ATOM_RenderSettings::_sunShaftEnabled					= true;		// enable sun shaft
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned	ATOM_RenderSettings::_enableLight						= ATOM_RenderSettings::LIGHT_ALL_TOGGLE;	// enable all light type default
bool		ATOM_RenderSettings::_enableTerrainDetailNormal			= false;		// enable terrain detail normal
bool		ATOM_RenderSettings::_enableSSS							= false;

bool		ATOM_RenderSettings::_enableModelCompression			= false;	// default is disabled
bool		ATOM_RenderSettings::_editorMode						= false;	// default is game mode

bool		ATOM_RenderSettings::_grassTerrainBlend					= false;	// default is disabled

bool		ATOM_RenderSettings::_enableShaderBinaryCompile			= false;	// default is disabled

bool		ATOM_RenderSettings::_forceNonDeferredShading			= false;	// default is enable Deferred shading mode

//----------------------//

bool ATOM_RenderSettings::_debugDeferredLightBuffer = false;
bool ATOM_RenderSettings::_enableAtmosphereEffect = true;
bool ATOM_RenderSettings::_enableFSAA = true;
ATOM_PixelFormat ATOM_RenderSettings::_gbufferFormatN = ATOM_PIXEL_FORMAT_UNKNOWN;
ATOM_PixelFormat ATOM_RenderSettings::_gbufferFormatC = ATOM_PIXEL_FORMAT_UNKNOWN;
float ATOM_RenderSettings::_deferredBufferScale = 1.0;
ATOM_TimeValueT<ATOM_Vector4f> ATOM_RenderSettings::_lightTimeFactors;
float ATOM_RenderSettings::_depthScale = 1.0;
float ATOM_RenderSettings::_minScreenContribution = 0.f;
bool ATOM_RenderSettings::_forceff = false;
bool ATOM_RenderSettings::_autoIMEChange = true;
bool ATOM_RenderSettings::_enableCloudCover = true;
float ATOM_RenderSettings::_ssaoScale = 1.f;
float ATOM_RenderSettings::_ssaoBias = 0.f;
float ATOM_RenderSettings::_ssaoRadius = 2.f;
float ATOM_RenderSettings::_ssaoIntensity = 4.f;
bool ATOM_RenderSettings::_enableSSAO = false;
bool ATOM_RenderSettings::_enableSSAODownsample = true;
bool ATOM_RenderSettings::_enableSSAOBlur = true;
bool ATOM_RenderSettings::_debugSSAO = false;
bool ATOM_RenderSettings::_debugShadowMap = false;
bool ATOM_RenderSettings::_debugShadowMask = false;
bool ATOM_RenderSettings::_enableToneMap = true;
float ATOM_RenderSettings::_ssaoDownsampleScale = 0.5f;
ATOM_RenderSettings::Quality ATOM_RenderSettings::_waterQuality = ATOM_RenderSettings::QUALITY_HIGH;
ATOM_RenderSettings::Quality ATOM_RenderSettings::_shadowQuality = ATOM_RenderSettings::QUALITY_HIGH;
ATOM_RenderSettings::Quality ATOM_RenderSettings::_atmosphereQuality = ATOM_RenderSettings::QUALITY_HIGH;
ATOM_AUTOREF(ATOM_Texture) ATOM_RenderSettings::_topmostEnvMap;

static const char *findArg (int argc, char **argv, const char *tobeFound)
{
	for (int i = 0; i < argc; ++i)
	{
		if (!strcmp (argv[i], tobeFound))
		{
			if (i < argc - 1)
			{
				return argv[i+1];
			}
			else
			{
				return "";
			}
		}
	}
	return 0;
}

void ATOM_RenderSettings::initialize (ATOM_RenderDevice *device, bool forceff, int argc, char **argv)
{
	ATOM_STACK_TRACE(ATOM_RenderSettings::initialize);
	_renderCaps.supportMtAnimation = true;

	const ATOM_RenderDeviceCaps &caps = device->getCapabilities();

	if (findArg (argc, argv, "--forceff"))
	{
		forceff = true;
	}

	//--- wangjian added ---//
	//_forceNonDeferredShading = false;
	if (findArg (argc, argv, "--forceNonDS"))
	{
		_forceNonDeferredShading = true;
	}
	//----------------------//

	_renderCaps.maxVertexShaderConst	= caps.max_vertex_program_uniforms;
	_renderCaps.supportSRGBRead			= caps.texture_sRGB_read;

	//----------------------//
	// wangjian modified
	// 是否支持延迟着色：
	// 1 : 支持MRT
	// 2 ：SM大于等于2
	// 3 ：支持INTZ等DEPTH TEXTURE(READ/WRITE)
	bool supportDeferredShading =	caps.max_rendertarget_count >= 4 && 
									caps.shader_model > 2 && 
									caps.texture_depth;
	//----------------------//

	if (supportDeferredShading && !forceff && !_forceNonDeferredShading)
	{
		_renderCaps.shaderModel = caps.shader_model;
		_renderCaps.mrtIndependentBitDepth = caps.mrt_independent_bit_depth;
		_renderCaps.shaderModelExt = caps.shader_model_ext;
		_renderCaps.numberMRT = caps.max_rendertarget_count;
		_renderCaps.supportInstancing = caps.hardware_instancing;

		const ATOM_PixelFormat fmt1 = ATOM_PIXEL_FORMAT_RGBA16F;
		bool rt1 = device->supportRenderTarget (fmt1);
		bool rtPPB1 = device->supportRenderTargetPostPixelShaderBlending (fmt1);
		bool rtFiltering1 = device->supportRenderTargetFiltering (fmt1);
		if (!rt1)
		{
			ATOM_LOGGER::log ("No RGBA16F render target support.\n");
		}
		else
		{
			if (!rtPPB1)
			{
				ATOM_LOGGER::log ("No RGBA16F render target post pixel shader blending support.\n");
			}
			if (!rtFiltering1)
			{
				ATOM_LOGGER::log ("No RGBA16F render target filtering support.\n");
			}
		}

		const ATOM_PixelFormat fmt3 = ATOM_PIXEL_FORMAT_RGBA32F;
		bool rt3 = device->supportRenderTarget (fmt3);
		bool rtPPB3 = device->supportRenderTargetPostPixelShaderBlending (fmt3);
		bool rtFiltering3 = device->supportRenderTargetFiltering (fmt3);
		if (!rt3)
		{
			ATOM_LOGGER::log ("No RGBA32F render target support.\n");
		}
		else
		{
			if (!rtPPB3)
			{
				ATOM_LOGGER::log ("No RGBA32F render target post pixel shader blending support.\n");
			}
			if (!rtFiltering3)
			{
				ATOM_LOGGER::log ("No RGBA32F render target filtering support.\n");
			}
		}

		ATOM_LOGGER::log ("MRT support: %d\n", _renderCaps.numberMRT);

		_gbufferFormatN = ATOM_PIXEL_FORMAT_UNKNOWN;
		_gbufferFormatC = ATOM_PIXEL_FORMAT_UNKNOWN;
		_renderCaps.supportDeferredShading = true;
		_renderCaps.supportHDRDeferredShading = false;
		if (rt1 && rtPPB1)
		{
			_gbufferFormatC = fmt1;
			_gbufferFormatN = fmt1;
			_renderCaps.supportHDRDeferredShading = true;
			ATOM_LOGGER::log ("Position texture format set to RGBA16F\n");
			ATOM_LOGGER::log ("Color texture format set to RGBA16F\n");
		}
		else if (rt3 && rtPPB3)
		{
			_gbufferFormatC = fmt3;
			_gbufferFormatN = fmt3;
			_renderCaps.supportHDRDeferredShading = true;
			ATOM_LOGGER::log ("Position texture format set to RGBA32F\n");
			ATOM_LOGGER::log ("Color texture format set to RGBA32F\n");
		}
		else
		{
			_gbufferFormatC = ATOM_PIXEL_FORMAT_BGRA8888;
			_gbufferFormatN = ATOM_PIXEL_FORMAT_BGRA8888;
			ATOM_LOGGER::log ("Position texture format set to BGRA8888\n");
			ATOM_LOGGER::log ("Normal texture format set to BGRA8888\n");
			ATOM_LOGGER::log ("Color texture format set to BGRA8888\n");
		}

		bool rt = device->supportRenderTarget (ATOM_PIXEL_FORMAT_RG16F);
		bool rtFiltering = device->supportRenderTargetFiltering (ATOM_PIXEL_FORMAT_RG16F);
		_renderCaps.supportShadow = rt && rtFiltering;

		_forceNonDeferredShading = false;		// wangjian added
		_forceff = false;
		enableShadow(true);
		enableDetailedTerrain(true);
		enableTerrDetailNormal(false);
		enablePostEffect(true);
		enableFSAA(true);
		setUseHWInstancing(true);
		_waterQuality = QUALITY_HIGH;
	}
	//--------------------------------//
	// wangjian added
	else if( _forceNonDeferredShading || caps.shader_model >= 2 )
	{
		ATOM_LOGGER::log ("Use forward shading\n");

		_renderCaps.mrtIndependentBitDepth		= caps.mrt_independent_bit_depth;
		_renderCaps.numberMRT					= caps.max_rendertarget_count;

		_renderCaps.supportInstancing			= caps.hardware_instancing && supportDeferredShading;	// 设备支持实例化 且 支持延迟渲染
		ATOM_LOGGER::log ("%s support geometry instancing\n", _renderCaps.supportInstancing ? "DO" : "DO NOT");

		_renderCaps.shaderModel					= caps.shader_model;
		_renderCaps.shaderModelExt				= caps.shader_model_ext;

		_renderCaps.supportDeferredShading		= supportDeferredShading;
		_renderCaps.supportHDRDeferredShading	= false;
		_renderCaps.supportShadow				= false;

		_forceNonDeferredShading				= true;
		_forceff								= false;

		enableShadow(false);
		enableDetailedTerrain(false);
		enableTerrDetailNormal(false);
		enablePostEffect(false);
		enableFSAA(false);
		setUseHWInstancing(false);
		_waterQuality = QUALITY_LOW;
	}
	//--------------------------------//
	else
	{
		_forceNonDeferredShading	= false;
		_forceff = true;

		_renderCaps.mrtIndependentBitDepth		= false;
		_renderCaps.numberMRT					= 1;
		_renderCaps.shaderModel					= 0;
		_renderCaps.shaderModelExt				= 0;
		_renderCaps.supportDeferredShading		= supportDeferredShading;
		_renderCaps.supportHDRDeferredShading	= false;
		_renderCaps.supportInstancing			= false;
		_renderCaps.supportShadow				= false;

		enableShadow(false);
		enableDetailedTerrain(false);
		enableTerrDetailNormal(false);
		enablePostEffect(false);
		enableFSAA(false);
		setUseHWInstancing(false);
		_waterQuality = QUALITY_LOW;
	}

	if (findArg (argc, argv, "--noinstancing"))
	{
		_renderCaps.supportInstancing = false;
	}

	// wangjian added
	// 如果是在编辑器模式下 默认是关闭实例化的
	if( ATOM_RenderSettings::isEditorModeEnabled() )
	{
		_renderCaps.supportInstancing = false;
		setUseHWInstancing(false);

		// 除非是显示将其开启
		if (findArg (argc, argv, "--instancing"))
		{
			_renderCaps.supportInstancing = true;
			setUseHWInstancing(true);
		}
	}
}

void ATOM_RenderSettings::enableShadow (bool enabled)
{
	// 当前阴影渲染需要SM3.0
	_enableShadow = enabled/* && _renderCaps.shaderModel > 2 && !_forceNonDeferredShading*/;
}

bool ATOM_RenderSettings::isShadowEnabled (void)
{
	// 当前阴影渲染需要SM3.0
	return _enableShadow && _renderCaps.shaderModel > 2 && !_forceNonDeferredShading;
}

void ATOM_RenderSettings::setShadowMapSize (unsigned size)
{
	_shadowMapSize = size;
}

unsigned ATOM_RenderSettings::getShadowMapSize (void)
{
	return _shadowMapSize;
}

void ATOM_RenderSettings::setShadowQuality (ATOM_RenderSettings::Quality val)
{
	_shadowQuality = val;
}

ATOM_RenderSettings::Quality ATOM_RenderSettings::getShadowQuality (void)
{
	return _shadowQuality;
}

void ATOM_RenderSettings::setShadowRegion (float size)
{
	_shadowRegion = size;
}

float ATOM_RenderSettings::getShadowRegion (void)
{
	return _shadowRegion;
}

const ATOM_RenderSettings::RenderCaps & ATOM_RenderSettings::getRenderCaps (void)
{
	return _renderCaps;
}

void ATOM_RenderSettings::setTextureSizeScale (float scale)
{
	_textureSizeScale = scale;
}

float ATOM_RenderSettings::getTextureSizeScale (void)
{
	return _textureSizeScale;
}

void ATOM_RenderSettings::setMaxTextureSizeLimit (unsigned size)
{
	_maxTextureSizeLimit = size;
}

unsigned ATOM_RenderSettings::getMaxTextureSizeLimit (void)
{
	return _maxTextureSizeLimit;
}

void ATOM_RenderSettings::setMinTextureSizeLimit (unsigned size)
{
	_minTextureSizeLimit = size;
}

unsigned ATOM_RenderSettings::getMinTextureSizeLimit (void)
{
	return _minTextureSizeLimit;
}

void ATOM_RenderSettings::enableDetailedTerrain (bool enable)
{
	_detailTerrainEnabled = enable/* && 
							!_forceNonDeferredShading &&
							_renderCaps.shaderModel >= 2*/;
}

bool ATOM_RenderSettings::detailedTerrainEnabled (void)
{
	return	_detailTerrainEnabled && 
			!_forceNonDeferredShading &&
			_renderCaps.shaderModel >= 2;
}

void ATOM_RenderSettings::enableOcclusionTest (bool enable)
{
	_occlusionTestEnabled = enable;
}

bool ATOM_RenderSettings::occlusionTestEnabled (void)
{
	return _occlusionTestEnabled;
}

bool ATOM_RenderSettings::isMultithreadedAnimationEnabled (void)
{
	return false;//_multithreadedAnimation && _renderCaps.supportMtAnimation;
}

void ATOM_RenderSettings::enableMultithreadedAnimation (bool b)
{
	_multithreadedAnimation = b;
}

void ATOM_RenderSettings::enableDebugDeferredDepth (bool b)
{
	_debugDeferredDepth = b;
}

bool ATOM_RenderSettings::isDebugDeferredDepthEnabled (void)
{
	return _debugDeferredDepth;
}

void ATOM_RenderSettings::enableDebugDeferredLightBuffer (bool b)
{
	_debugDeferredLightBuffer = b;
}

bool ATOM_RenderSettings::isDebugDeferredLightBufferEnabled (void)
{
	return _debugDeferredLightBuffer;
}

void ATOM_RenderSettings::enableDebugDeferredNormals (bool b)
{
	_debugDeferredNormals = b;
}

bool ATOM_RenderSettings::isDebugDeferredNormalsEnabled (void)
{
	return _debugDeferredNormals;
}

void ATOM_RenderSettings::enableDebugDeferredColors (bool b)
{
	_debugDeferredColors = b;
}
bool ATOM_RenderSettings::isDebugDeferredColorsEnabled (void)
{
	return _debugDeferredColors;
}
//---- wangjian added ----//
void ATOM_RenderSettings::enableDebugHalfLambertBuffer (bool b)
{
	_debugHalfLambertBuffer = b;
}
bool ATOM_RenderSettings::isDebugHalfLambertBufferEnabled (void)
{
	return _debugHalfLambertBuffer;
}
void ATOM_RenderSettings::enableDebugBloomBuffer (bool b)
{
	_debugBloomBuffer = b;
}
bool ATOM_RenderSettings::isDebugBloomBufferEnabled (void)
{
	return _debugBloomBuffer;
}
void ATOM_RenderSettings::enableHDRRendering (bool b)
{
	_enableHDRRendering = b;
}
bool ATOM_RenderSettings::isHDRRenderingEnabled(void)
{
	return _enableHDRRendering;
}

void ATOM_RenderSettings::enableFloatShading (bool b)
{
	_enableFloatShading = b;
}
bool ATOM_RenderSettings::isFloatShadingEnabled(void)
{
	return _enableFloatShading;
}

void ATOM_RenderSettings::enableDebugHdrLumin (bool b)
{
	_debugHdrLumin = b;
}
bool ATOM_RenderSettings::isDebugHdrLuminEnabled(void)
{
	return _debugHdrLumin;
}
void ATOM_RenderSettings::enableDebugHdrRTs (bool b)
{
	_debugHDRRTs = b;
}
bool ATOM_RenderSettings::isDebugHdrRTsEnabled(void)
{
	return _debugHDRRTs;
}
void ATOM_RenderSettings::enableFilmicMapping (bool b)
{
	_useFilmicMapping = b;
}
bool ATOM_RenderSettings::isFilmicMappingEnabled(void)
{
	return _useFilmicMapping;
}
void ATOM_RenderSettings::saveHDRRTs (bool b)
{
	_saveHDRRTs = b;
}
bool ATOM_RenderSettings::isSaveHDRRTsEnabled ()
{
	return _saveHDRRTs;
}
void ATOM_RenderSettings::enableStateSorting(bool b)
{
	_stateSorting = b;
}
bool ATOM_RenderSettings::isStateSortingEnabled(void)
{
	return _stateSorting;
}
void ATOM_RenderSettings::setShadowDrawItemUpdateFrameCache(int cacheFrame)
{
	_shadowDrawItemUpdateFrameCache = cacheFrame;
}
int ATOM_RenderSettings::getShadowDrawItemUpdateFrameCache(void)
{
	return _shadowDrawItemUpdateFrameCache;
}
void ATOM_RenderSettings::setSceneDrawItemUpdateFrameCache(int cacheFrame)
{
	_sceneDrawItemUpdateFrameCache = cacheFrame;
}
int ATOM_RenderSettings::getSceneDrawItemUpdateFrameCache(void)
{
	return _sceneDrawItemUpdateFrameCache;
}
void ATOM_RenderSettings::setCullUpdateViewDirThreshold(float thresh)
{
	_cullUpdateViewDirThreshold = thresh;
}
float ATOM_RenderSettings::getCullUpdateViewDirThreshold(void)
{
	return _cullUpdateViewDirThreshold;
}
void ATOM_RenderSettings::setCullUpdateViewPosThreshold(float thresh)
{
	_cullUpdateViewPosThreshold = thresh;
}
float ATOM_RenderSettings::getCullUpdateViewPosThreshold(void)
{
	return _cullUpdateViewPosThreshold;
}
// DX9几何体实例化相关
void ATOM_RenderSettings::setUseHWInstancing(bool use)
{
	if( use && !ATOM_RenderSettings::getRenderCaps().supportInstancing )
	{
		_bUseHWInstancing = false;
		return;
	}
	_bUseHWInstancing = use;
}
bool ATOM_RenderSettings::isUseHWInstancing(void)
{
	// 支持实例化 并且 开启了实例化 并且 是延迟渲染模式
	if( ATOM_RenderSettings::getRenderCaps().supportInstancing && _bUseHWInstancing && !_forceNonDeferredShading )
		return true;

	return false;
}
void ATOM_RenderSettings::setUseInstancingStaticBuffer(bool use)
{
	if( use && !ATOM_RenderSettings::isUseHWInstancing() )
	{
		_bUseInstancingStaticBuffer = false;
		return;
	}

	_bUseInstancingStaticBuffer = use;
}
bool ATOM_RenderSettings::isUseInstancingStaticBuffer(void)
{
	// 如果使用实例化 那么总是使用静态的实例化BUFFRR
#if 0

	if( ATOM_RenderSettings::isUseHWInstancing() && 
		_bUseInstancingStaticBuffer && 
		ATOM_RenderSettings::getInstancingSBCountThreshold() > 0 )
		return true;
	return false;

#else

	return true;

#endif
}
void ATOM_RenderSettings::setInstancingSBCountThreshold(unsigned thresh)
{
	if( thresh > 0 && !ATOM_RenderSettings::isUseInstancingStaticBuffer() )
	{
		_InstancingSBCountThreshold = 0;
		return;
	}
	_InstancingSBCountThreshold = thresh;
}
unsigned ATOM_RenderSettings::getInstancingSBCountThreshold(void)
{
	return _InstancingSBCountThreshold;
}
void ATOM_RenderSettings::setShadowMaskSizeScale(unsigned scale)
{
	_shadowMaskScale = scale;
}
unsigned ATOM_RenderSettings::getShadowMaskSizeScale(void)
{
	return _shadowMaskScale;
}
void ATOM_RenderSettings::enableStreamingLog(bool enable)
{
	ATOM_AsyncLoader::setEnableLog(enable?1:0);
}
//=============================================================//
// POST EFFECT TOGGLE
bool ATOM_RenderSettings::isPostEffectEnabled(void)
{
	return _postEffectEnabled && !_forceNonDeferredShading;
}
void ATOM_RenderSettings::enablePostEffect(bool enable)
{
	_postEffectEnabled = enable/* && !_forceNonDeferredShading*/;
}
bool ATOM_RenderSettings::isFSAAEnabled (void)
{
	return _enableFSAA && isPostEffectEnabled();
}
void ATOM_RenderSettings::enableFSAA (bool enable)
{
	_enableFSAA = enable/* && !_forceNonDeferredShading*/;
}
bool ATOM_RenderSettings::isRainEnabled(void)
{
	return _rainEnabled && isPostEffectEnabled();
}
void ATOM_RenderSettings::enableRain(bool enable)
{
	_rainEnabled = enable/* && _postEffectEnabled*/;

	//setWaterQuality(_waterQuality);
}
bool ATOM_RenderSettings::isRainLayerEnabled(void)
{
	return _rainLayerEnabled && isPostEffectEnabled();
}
void ATOM_RenderSettings::enableRainLayer(bool enable)
{
	_rainLayerEnabled = enable/* && _postEffectEnabled*/;
}
bool ATOM_RenderSettings::isBloomEnabled(void)
{
	return _bloomEnabled && isPostEffectEnabled();
}
void ATOM_RenderSettings::enableBloom(bool enable)
{
	_bloomEnabled = enable/* && _postEffectEnabled*/;
}
bool ATOM_RenderSettings::isDofEnabled(void)
{
	return _dofEnabled && isPostEffectEnabled();
}
void ATOM_RenderSettings::enableDof(bool enable)
{
	_dofEnabled = enable/* && _postEffectEnabled*/;
}

bool ATOM_RenderSettings::isSunShaftEnabled(void)
{
	return _sunShaftEnabled && isPostEffectEnabled();
}
void ATOM_RenderSettings::enableSunShaft(bool enable)
{
	_sunShaftEnabled = enable/* && _postEffectEnabled*/;
}

/////////////////////////////////////////////////////////////////////
bool ATOM_RenderSettings::isTerrDetailNormalEnabled(void)
{
	return	_detailTerrainEnabled && 
			_enableTerrainDetailNormal && 
			!_forceNonDeferredShading && 
			_renderCaps.shaderModel >= 2;
}
void ATOM_RenderSettings::enableTerrDetailNormal(bool enable)
{
	//如果开启细节法线 那么细节纹理相应必须得开启
	if( enable && !detailedTerrainEnabled() )
		enableDetailedTerrain(true);

	_enableTerrainDetailNormal = enable/* && 
									   !_forceNonDeferredShading &&
									   _detailTerrainEnabled && 
									   _renderCaps.shaderModel >= 2*/;
}
// light toggle
bool ATOM_RenderSettings::isDirectionalLightTypeEnabled(void)
{
	return _enableLight & LIGHT_DIRECTIONAL_TOGGLE;
}
void ATOM_RenderSettings::enableDirectionalLightType(bool enable)
{
	if( enable )
		_enableLight |= LIGHT_DIRECTIONAL_TOGGLE;
	else
		_enableLight &= ~LIGHT_DIRECTIONAL_TOGGLE;
}
bool ATOM_RenderSettings::isPointLightTypeEnabled(void)
{
	return (_enableLight & LIGHT_POINT_TOGGLE) && !_forceNonDeferredShading;
}
void ATOM_RenderSettings::enablePointLightType(bool enable)
{
	if( enable )
		_enableLight |= LIGHT_POINT_TOGGLE;
	else
		_enableLight &= ~LIGHT_POINT_TOGGLE;
}
bool ATOM_RenderSettings::isAllLightTypeEnabled(void)
{
	return _enableLight & LIGHT_ALL_TOGGLE;
}
void ATOM_RenderSettings::enableAllLightType(bool enable)
{
	if( enable )
		_enableLight |= LIGHT_ALL_TOGGLE;
	else
		_enableLight &= ~LIGHT_ALL_TOGGLE;
}

void ATOM_RenderSettings::enableSSSRendering (bool b)
{
	_enableSSS = b;
}
bool ATOM_RenderSettings::isSSSRenderingEnabled(void)
{
	return _enableSSS;
}
void ATOM_RenderSettings::enableModelCompressedFormat (bool b)
{
	_enableModelCompression = b;
}
bool ATOM_RenderSettings::isModelCompressedFormatEnabled(void)
{
	return _enableModelCompression;
}

void ATOM_RenderSettings::enableEditorMode (bool enable)
{
	_editorMode = enable;
}
bool ATOM_RenderSettings::isEditorModeEnabled (void)
{
	return _editorMode;
}

void ATOM_RenderSettings::enableGrassTerrainBlend (bool enable)
{
	_grassTerrainBlend = enable;
}
bool ATOM_RenderSettings::isGrassTerrainBlendEnabled (void)
{
	return _grassTerrainBlend;
}

void ATOM_RenderSettings::enableShaderBinaryCompile (bool enable)
{
	_enableShaderBinaryCompile = enable;
}
bool ATOM_RenderSettings::isShaderBinaryCompileEnabled (void)
{
	return _enableShaderBinaryCompile;
}

//-------------------------//

void ATOM_RenderSettings::enableAtmosphereEffect (bool b)
{
	_enableAtmosphereEffect = b;
}
bool ATOM_RenderSettings::isAtmosphereEffectEnabled (void)
{
	return _enableAtmosphereEffect;
}

void ATOM_RenderSettings::setDeferredBufferScale (float f)
{
	_deferredBufferScale = f;
}

float ATOM_RenderSettings::getDeferredBufferScale (void)
{
	return _deferredBufferScale;
}

void ATOM_RenderSettings::setLightTimeFactors (const ATOM_TimeValueT<ATOM_Vector4f> &factors)
{
	_lightTimeFactors = factors;
}

const ATOM_Vector4f &ATOM_RenderSettings::getLightTimeFactor (void)
{
	return _lightTimeFactors.getCurrentValue ();
}

ATOM_PixelFormat ATOM_RenderSettings::getGBufferFormatN (void)
{
	return _gbufferFormatN;
}

ATOM_PixelFormat ATOM_RenderSettings::getGBufferFormatC (void)
{
	return _gbufferFormatC;
}

void ATOM_RenderSettings::setDepthScale (float val)
{
	_depthScale = val;
}

float ATOM_RenderSettings::getDepthScale (void)
{
	return _depthScale;
}

void ATOM_RenderSettings::setMinScreenContribution (float value)
{
	_minScreenContribution = value;
}

float ATOM_RenderSettings::getMinScreenContribution (void)
{
	return _minScreenContribution;
}

bool ATOM_RenderSettings::isFixedFunction (void)
{
	return _forceff || _renderCaps.shaderModel < 2;
}

//----------------------------------------------------------------------------------//
// wangjian added
bool ATOM_RenderSettings::isNonDeferredShading(void)
{
	return _forceNonDeferredShading;
}
void ATOM_RenderSettings::setNonDeferredShading(bool b)
{
	_forceNonDeferredShading = b;
	if( _forceNonDeferredShading )
	{
		enableShadow(false);
		enableDetailedTerrain(false);
		enableTerrDetailNormal(false);
		enablePostEffect(false);
		enableFSAA(false);
		setUseHWInstancing(false);
		_waterQuality = QUALITY_LOW;
	}
	else
	{
		enableShadow(true);
		enableDetailedTerrain(true);
		enableTerrDetailNormal(false);
		enablePostEffect(true);
		enableFSAA(true);
		setUseHWInstancing(true);
		_waterQuality = QUALITY_HIGH;
	}
}
ATOM_STRING ATOM_RenderSettings::getCoreMaterialName(const char* material)
{
	if( !material )
		return "";

#if 1

	// 如果当前设备不支持SM3 或 INTZ等深度BUFFER格式
	/*const ATOM_RenderDeviceCaps &caps = ATOM_GetRenderDevice()->getCapabilities();*/
	bool bUseForwardMaterial = !_renderCaps.supportDeferredShading;
	if( bUseForwardMaterial )
	{
		static ATOM_MAP<ATOM_STRING,ATOM_STRING> _ds_to_forward;

		_ds_to_forward["/materials/builtin/sky.mat"]									= "/materials/builtin/forward/sky.mat";
		_ds_to_forward["/materials/builtin/water_simple.mat"]							= "/materials/builtin/forward/water_simple.mat";
		_ds_to_forward["/materials/builtin/particlesys.mat"]							= "/materials/builtin/forward/particlesys.mat";
		_ds_to_forward["/materials/builtin/shape.mat"]									= "/materials/builtin/forward/shape.mat";

		_ds_to_forward["/materials/builtin/terrain.mat"]								= "/materials/builtin/forward/terrain.mat";
		_ds_to_forward["/materials/builtin/grass.mat"]									= "/materials/builtin/forward/grass.mat";
		_ds_to_forward["/editor/materials/terrain.mat"]									= "/editor/materials/forward/terrain.mat";		// editor version
		_ds_to_forward["/editor/materials/grass.mat"]									= "/editor/materials/forward/grass.mat";			// editor version

		_ds_to_forward["/materials/builtin/model_default.mat"]							= "/materials/builtin/forward/model_custom_forward.mat";
		_ds_to_forward["/materials/builtin/model_bumpmap.mat"]							= "/materials/builtin/forward/model_custom_forward.mat";
		_ds_to_forward["/materials/builtin/model_flux.mat"]								= "/materials/builtin/forward/model_custom_forward.mat";
		_ds_to_forward["/materials/builtin/model_default_rimlight.mat"]					= "/materials/builtin/forward/model_custom_forward.mat";
		_ds_to_forward["/materials/builtin/model_bumpmap_rimlight.mat"]					= "/materials/builtin/forward/model_custom_forward.mat";
		_ds_to_forward["/materials/builtin/model_flux_rimlight.mat"]					= "/materials/builtin/forward/model_custom_forward.mat";

		_ds_to_forward["/materials/builtin/model_default_instancing_staticbuffer.mat"]	= "/materials/builtin/forward/model_custom_forward.mat";
		_ds_to_forward["/materials/builtin/stdshadowmap.mat"]							= "";
		_ds_to_forward["/materials/builtin/deferredrendering.mat"]						= "";
		_ds_to_forward["/materials/builtin/decal.mat"]									= "";
		_ds_to_forward["/materials/builtin/fog_height.mat"]								= "";
		_ds_to_forward["/materials/builtin/fxaa.mat"]									= "";
		_ds_to_forward["/materials/builtin/bloom.mat"]									= "";
		_ds_to_forward["/materials/builtin/depthmask.mat"]								= "";
		_ds_to_forward["/materials/builtin/colorFilter.mat"]							= "";

		// game specific material , todo: should remove from here
		_ds_to_forward["/materials/model_default_game.mat"]								= "/materials/forward/model_custom_forward_game.mat";
		_ds_to_forward["/materials/model_bumpmap_game.mat"]								= "/materials/forward/model_custom_forward_game.mat";
		_ds_to_forward["/materials/model_flux_game.mat"]								= "/materials/forward/model_custom_forward_game.mat";
		_ds_to_forward["/materials/model_default_rimlight_game.mat"]					= "/materials/forward/model_custom_forward_game.mat";
		_ds_to_forward["/materials/model_bumpmap_rimlight_game.mat"]					= "/materials/forward/model_custom_forward_game.mat";
		_ds_to_forward["/materials/model_flux_rimlight_game.mat"]						= "/materials/forward/model_custom_forward_game.mat";

		ATOM_MAP<ATOM_STRING,ATOM_STRING>::iterator iter = _ds_to_forward.find(material);
		if( iter != _ds_to_forward.end() )
			return iter->second;
	}

#endif
	
	return material;
}
//----------------------------------------------------------------------------------//

void ATOM_RenderSettings::setWaterQuality (ATOM_RenderSettings::Quality val)
{
	if( _forceff || _forceNonDeferredShading )
	{
		_waterQuality = QUALITY_LOW;
		return;
	}

	_waterQuality = val;

	//--- wangjian added ---//
	ATOM_AUTOREF(ATOM_CoreMaterial) water_core_mat = ATOM_MaterialManager::loadCoreMaterial (ATOM_GetRenderDevice(), "/materials/builtin/water_simple.mat");
	
	if( water_core_mat )
	{
		water_core_mat->getEffectByName("genCaustics")->enable(_waterQuality <= QUALITY_MEDIUM ? true : false);

		water_core_mat->getEffectByName("default")->enable(_waterQuality > QUALITY_HIGH ? true : false);

		water_core_mat->getEffectByName("default_highspec")->enable(false);
		water_core_mat->getEffectByName("default_highspec_rain")->enable(false);

		if( _waterQuality < QUALITY_MEDIUM )
		{

			if( ATOM_RenderSettings::isRainEnabled() && ATOM_RenderSettings::isRainLayerEnabled() && _renderCaps.shaderModel > 2 )
			{
				water_core_mat->getEffectByName("default_highspec_rain")->enable(true);
			}
			else
			{
				water_core_mat->getEffectByName("default_highspec")->enable(true);
			}
		}
	}
	//---------------------//
}

ATOM_RenderSettings::Quality ATOM_RenderSettings::getWaterQuality (void)
{
	return _waterQuality;
}

void ATOM_RenderSettings::setAtmosphereQuality (ATOM_RenderSettings::Quality val)
{
	_atmosphereQuality = val;
}

ATOM_RenderSettings::Quality ATOM_RenderSettings::getAtmosphereQuality (void)
{
	return _atmosphereQuality;
}

void ATOM_RenderSettings::enableAutoIMEChange (bool b)
{
	_autoIMEChange = b;

#if ATOM3D_COMPILER_MSVC
	if (b)
	{
		::ImmAssociateContextEx (ATOM_APP->getMainWindow(), NULL, IACE_DEFAULT);
	}
#endif
}

bool ATOM_RenderSettings::isAutoIMEChangeEnabled (void)
{
	return _autoIMEChange;
}

void ATOM_RenderSettings::setTopMostEnvMap (ATOM_Texture *texture)
{
	_topmostEnvMap = texture;
}

ATOM_Texture *ATOM_RenderSettings::getTopMostEnvMap (void)
{
	return _topmostEnvMap.get();
}

void ATOM_RenderSettings::enableCloudCover (bool enable)
{
	_enableCloudCover = enable;
}

bool ATOM_RenderSettings::isCloudCoverEnabled (void)
{
	return _enableCloudCover;
}

void ATOM_RenderSettings::setSSAOScale (float val)
{
	_ssaoScale = val;
}

float ATOM_RenderSettings::getSSAOScale (void)
{
	return _ssaoScale;
}

void ATOM_RenderSettings::setSSAOBias (float val)
{
	_ssaoBias = val;
}

float ATOM_RenderSettings::getSSAOBias (void)
{
	return _ssaoBias;
}

void ATOM_RenderSettings::setSSAORadius (float val)
{
	_ssaoRadius = val;
}

float ATOM_RenderSettings::getSSAORadius (void)
{
	return _ssaoRadius;
}

void ATOM_RenderSettings::setSSAOIntensity (float val)
{
	_ssaoIntensity = val;
}

float ATOM_RenderSettings::getSSAOIntensity (void)
{
	return _ssaoIntensity;
}

void ATOM_RenderSettings::enableSSAO (bool enable)
{
	_enableSSAO = enable;
}

bool ATOM_RenderSettings::isSSAOEnabled (void)
{
	return _enableSSAO;
}

void ATOM_RenderSettings::enableSSAODownsample (bool enable)
{
	_enableSSAODownsample = enable;
}

bool ATOM_RenderSettings::isSSAODownsampleEnabled (void)
{
	return _enableSSAODownsample;
}

void ATOM_RenderSettings::enableSSAOBlur (bool enable)
{
	_enableSSAOBlur = enable;
}

bool ATOM_RenderSettings::isSSAOBlurEnabled (void)
{
	return _enableSSAOBlur;
}

void ATOM_RenderSettings::enableDebugSSAO (bool enable)
{
	_debugSSAO = enable;
}

bool ATOM_RenderSettings::isDebugSSAOEnabled (void)
{
	return _debugSSAO;
}

void ATOM_RenderSettings::enableDebugShadowMap (bool enable)
{
	_debugShadowMap = enable;
}

bool ATOM_RenderSettings::isDebugShadowMapEnabled (void)
{
	return _debugShadowMap;
}

void ATOM_RenderSettings::enableDebugShadowMask (bool enable)
{
	_debugShadowMask = enable;
}

bool ATOM_RenderSettings::isDebugShadowMaskEnabled (void)
{
	return _debugShadowMask;
}

void ATOM_RenderSettings::setSSAODownsampleFactor (float val)
{
	_ssaoDownsampleScale = val;
}

float ATOM_RenderSettings::getSSAODownsampleFactor (void)
{
	return _ssaoDownsampleScale;
}

void ATOM_RenderSettings::enableToneMapping (bool b)
{
	_enableToneMap = b;
}

bool ATOM_RenderSettings::isToneMappingEnabled (void)
{
	return _enableToneMap;
}

void ATOM_RenderSettings::enableOctree (bool b)
{
	_enableOctree = b;
}

bool ATOM_RenderSettings::isOctreeEnabled (void)
{
	return _enableOctree;
}

