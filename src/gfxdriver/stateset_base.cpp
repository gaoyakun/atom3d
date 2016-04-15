#include "stateset_base.h"
#include "statecache.h"
#include "gfxdriver.h"

ATOM_AlphaBlendAttributesBase::ATOM_AlphaBlendAttributesBase (void)
{
	_enableAlphaBlending = false;
	_srcBlend = BlendFunc_Unknown;
	_destBlend = BlendFunc_Unknown;
	_blendConstant = 0xFFFFFFFF;
}

void ATOM_AlphaBlendAttributesBase::setDefaults (void)
{
	_enableAlphaBlending = false;
	_srcBlend = BlendFunc_One;
	_destBlend = BlendFunc_Zero;
	_blendConstant = 0xFFFFFFFF;
}

ATOM_RenderAttributes::RenderState ATOM_AlphaBlendAttributesBase::getType (void) const
{
	return ATOM_RenderAttributes::RS_AlphaBlending;
}

void ATOM_AlphaBlendAttributesBase::enableAlphaBlending (bool enable)
{
	_enableAlphaBlending = enable;
}

bool ATOM_AlphaBlendAttributesBase::isAlphaBlendingEnabled (void) const
{
	return _enableAlphaBlending;
}

void ATOM_AlphaBlendAttributesBase::setSrcBlend (ATOM_RenderAttributes::BlendFunc func)
{
	_srcBlend = func;
}

ATOM_RenderAttributes::BlendFunc ATOM_AlphaBlendAttributesBase::getSrcBlend (void) const
{
	return _srcBlend;
}

void ATOM_AlphaBlendAttributesBase::setDestBlend (ATOM_RenderAttributes::BlendFunc func)
{
	_destBlend = func;
}

ATOM_RenderAttributes::BlendFunc ATOM_AlphaBlendAttributesBase::getDestBlend (void) const
{
	return _destBlend;
}

void ATOM_AlphaBlendAttributesBase::setBlendConstant (ATOM_ColorARGB val)
{
	_blendConstant = val;
}

ATOM_ColorARGB ATOM_AlphaBlendAttributesBase::getBlendConstant (void) const
{
	return _blendConstant;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_ColorWriteAttributesBase::ATOM_ColorWriteAttributesBase (void)
{
	_writeMask = ColorMask_All;
}

void ATOM_ColorWriteAttributesBase::setDefaults (void)
{
	_writeMask = ColorMask_All;
}

ATOM_RenderAttributes::RenderState ATOM_ColorWriteAttributesBase::getType (void) const
{
	return ATOM_RenderAttributes::RS_ColorWrite;
}

void ATOM_ColorWriteAttributesBase::setColorWriteMask (unsigned char mask)
{
	_writeMask = mask;
}

unsigned char ATOM_ColorWriteAttributesBase::getColorWriteMask (void) const
{
	return _writeMask;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_DepthAttributesBase::ATOM_DepthAttributesBase (void)
{
	_enableDepthWrite = false;
	_depthFunc = CompareFunc_Unknown;
}

void ATOM_DepthAttributesBase::setDefaults (void)
{
	_enableDepthWrite = true;
	_depthFunc = ATOM_RenderAttributes::CompareFunc_LessEqual;
}

ATOM_RenderAttributes::RenderState ATOM_DepthAttributesBase::getType (void) const
{
	return ATOM_RenderAttributes::RS_Depth;
}

void ATOM_DepthAttributesBase::enableDepthWrite (bool enable)
{
	_enableDepthWrite = enable;
}

bool ATOM_DepthAttributesBase::isDepthWriteEnabled (void) const
{
	return _enableDepthWrite;
}

void ATOM_DepthAttributesBase::setDepthFunc (ATOM_RenderAttributes::CompareFunc func)
{
	_depthFunc = func;
}

ATOM_RenderAttributes::CompareFunc ATOM_DepthAttributesBase::getDepthFunc (void) const
{
	return _depthFunc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_StencilAttributesBase::ATOM_StencilAttributesBase (void)
{
	_enableStencil = false;
	_enableTwoSidedStencil = false;
	_stencilReadMask = 0xFFFFFFFF;
	_stencilWriteMask = 0xFFFFFFFF;
	_stencilRef = 0;
	_failOpFront = ATOM_RenderAttributes::StencilOp_Unknown;
	_depthFailOpFront = ATOM_RenderAttributes::StencilOp_Unknown;
	_passOpFront = ATOM_RenderAttributes::StencilOp_Unknown;
	_stencilFuncFront = ATOM_RenderAttributes::CompareFunc_Unknown;
	_failOpBack = ATOM_RenderAttributes::StencilOp_Unknown;
	_depthFailOpBack = ATOM_RenderAttributes::StencilOp_Unknown;
	_passOpBack = ATOM_RenderAttributes::StencilOp_Unknown;
	_stencilFuncBack = ATOM_RenderAttributes::CompareFunc_Unknown;
}

void ATOM_StencilAttributesBase::setDefaults (void)
{
	_enableStencil = false;
	_enableTwoSidedStencil = false;
	_stencilReadMask = 0xFFFFFFFF;
	_stencilWriteMask = 0xFFFFFFFF;
	_stencilRef = 0;
	_failOpFront = ATOM_RenderAttributes::StencilOp_Keep;
	_depthFailOpFront = ATOM_RenderAttributes::StencilOp_Keep;
	_passOpFront = ATOM_RenderAttributes::StencilOp_Keep;
	_stencilFuncFront = ATOM_RenderAttributes::CompareFunc_Always;
	_failOpBack = ATOM_RenderAttributes::StencilOp_Keep;
	_depthFailOpBack = ATOM_RenderAttributes::StencilOp_Keep;
	_passOpBack = ATOM_RenderAttributes::StencilOp_Keep;
	_stencilFuncBack = ATOM_RenderAttributes::CompareFunc_Always;
}

ATOM_RenderAttributes::RenderState ATOM_StencilAttributesBase::getType (void) const
{
	return ATOM_RenderAttributes::RS_Stencil;
}

void ATOM_StencilAttributesBase::enableStencil (bool enable)
{
	_enableStencil = enable;
}

bool ATOM_StencilAttributesBase::isStencilEnabled (void) const
{
	return _enableStencil;
}

void ATOM_StencilAttributesBase::setStencilReadMask (unsigned mask)
{
	_stencilReadMask = mask;
}

unsigned ATOM_StencilAttributesBase::getStencilReadMask (void) const
{
	return _stencilReadMask;
}

void ATOM_StencilAttributesBase::setStencilWriteMask (unsigned mask)
{
	_stencilWriteMask = mask;
}

unsigned ATOM_StencilAttributesBase::getStencilWriteMask (void) const
{
	return _stencilWriteMask;
}

void ATOM_StencilAttributesBase::setStencilFailOpFront (ATOM_RenderAttributes::StencilOp op)
{
	_failOpFront = op;
}

ATOM_RenderAttributes::StencilOp ATOM_StencilAttributesBase::getStencilFailOpFront (void) const
{
	return _failOpFront;
}

void ATOM_StencilAttributesBase::setStencilDepthFailOpFront (ATOM_RenderAttributes::StencilOp op)
{
	_depthFailOpFront = op;
}

ATOM_RenderAttributes::StencilOp ATOM_StencilAttributesBase::getStencilDepthFailOpFront (void) const
{
	return _depthFailOpFront;
}

void ATOM_StencilAttributesBase::setStencilPassOpFront (ATOM_RenderAttributes::StencilOp op)
{
	_passOpFront = op;
}

ATOM_RenderAttributes::StencilOp ATOM_StencilAttributesBase::getStencilPassOpFront (void) const
{
	return _passOpFront;
}

void ATOM_StencilAttributesBase::setStencilFuncFront (ATOM_RenderAttributes::CompareFunc func)
{
	_stencilFuncFront = func;
}

ATOM_RenderAttributes::CompareFunc ATOM_StencilAttributesBase::getStencilFuncFront (void) const
{
	return _stencilFuncFront;
}

void ATOM_StencilAttributesBase::setStencilRef (int ref)
{
	_stencilRef = ref;
}

int ATOM_StencilAttributesBase::getStencilRef (void) const
{
	return _stencilRef;
}

void ATOM_StencilAttributesBase::enableTwoSidedStencil (bool enable)
{
	_enableTwoSidedStencil = enable;
}

bool ATOM_StencilAttributesBase::isTwoSidedStencilEnabled (void) const
{
	return _enableTwoSidedStencil;
}

void ATOM_StencilAttributesBase::setStencilFailOpBack (ATOM_RenderAttributes::StencilOp op)
{
	_failOpBack = op;
}

ATOM_RenderAttributes::StencilOp ATOM_StencilAttributesBase::getStencilFailOpBack (void) const
{
	return _failOpBack;
}

void ATOM_StencilAttributesBase::setStencilDepthFailOpBack (ATOM_RenderAttributes::StencilOp op)
{
	_depthFailOpBack = op;
}

ATOM_RenderAttributes::StencilOp ATOM_StencilAttributesBase::getStencilDepthFailOpBack (void) const
{
	return _depthFailOpBack;
}

void ATOM_StencilAttributesBase::setStencilPassOpBack (ATOM_RenderAttributes::StencilOp op)
{
	_passOpBack = op;
}

ATOM_RenderAttributes::StencilOp ATOM_StencilAttributesBase::getStencilPassOpBack (void) const
{
	return _passOpBack;
}

void ATOM_StencilAttributesBase::setStencilFuncBack (ATOM_RenderAttributes::CompareFunc func)
{
	_stencilFuncBack = func;
}

ATOM_RenderAttributes::CompareFunc ATOM_StencilAttributesBase::getStencilFuncBack (void) const
{
	return _stencilFuncBack;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_RasterizerAttributesBase::ATOM_RasterizerAttributesBase (void)
{
	_fillMode = FillMode_Unknown;
	_cullMode = CullMode_Unknown;
	_frontFace = FrontFace_Unknown;
}

void ATOM_RasterizerAttributesBase::setDefaults (void)
{
	_fillMode = FillMode_Solid;
	_cullMode = CullMode_Back;
	_frontFace = FrontFace_CW;
}

ATOM_RenderAttributes::RenderState ATOM_RasterizerAttributesBase::getType (void) const
{
	return ATOM_RenderAttributes::RS_Rasterizer;
}

void ATOM_RasterizerAttributesBase::setFillMode (ATOM_RenderAttributes::FillMode mode)
{
	_fillMode = mode;
}

ATOM_RenderAttributes::FillMode ATOM_RasterizerAttributesBase::getFillMode (void) const
{
	return _fillMode;
}

void ATOM_RasterizerAttributesBase::setCullMode (CullMode mode)
{
	_cullMode = mode;
}

ATOM_RenderAttributes::CullMode ATOM_RasterizerAttributesBase::getCullMode (void) const
{
	return _cullMode;
}

void ATOM_RasterizerAttributesBase::setFrontFace (FrontFace mode)
{
	_frontFace = mode;
}

ATOM_RenderAttributes::FrontFace ATOM_RasterizerAttributesBase::getFrontFace (void) const
{
	return _frontFace;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_MultisampleAttributesBase::ATOM_MultisampleAttributesBase (void)
{
	_enableMultisample = false;
	_enableAlphaToCoverage = false;
}

void ATOM_MultisampleAttributesBase::setDefaults (void)
{
	_enableMultisample = false;
	_enableAlphaToCoverage = false;
}

ATOM_RenderAttributes::RenderState ATOM_MultisampleAttributesBase::getType (void) const
{
	return ATOM_RenderAttributes::RS_Multisample;
}

void ATOM_MultisampleAttributesBase::enableMultisample (bool enable)
{
	_enableMultisample = enable;
}

bool ATOM_MultisampleAttributesBase::isMultisampleEnabled (void) const
{
	return _enableMultisample;
}

void ATOM_MultisampleAttributesBase::enableAlphaToCoverage (bool enable)
{
	_enableAlphaToCoverage = enable;
}

bool ATOM_MultisampleAttributesBase::isAlphaToCoverageEnabled (void) const
{
	return _enableAlphaToCoverage;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_SamplerAttributesBase::ATOM_SamplerAttributesBase (void)
{
	_filter = SamplerFilter_Unknown;
	_addressU = SamplerAddress_Unknown;
	_addressV = SamplerAddress_Unknown;
	_addressW = SamplerAddress_Unknown;
	_compareFunc = ATOM_RenderAttributes::CompareFunc_Unknown;
	_mipmapLodBias = 0.f;
	_maxLOD = 0;
	_maxAnisotropic = 1;
	_borderColor = 0;
	_srgbTexture = false;
}

void ATOM_SamplerAttributesBase::setDefaults (void)
{
	_filter = SamplerFilter_LLP;
	_addressU = SamplerAddress_Clamp;
	_addressV = SamplerAddress_Clamp;
	_addressW = SamplerAddress_Clamp;
	_compareFunc = ATOM_RenderAttributes::CompareFunc_Always;
	_mipmapLodBias = 0.f;
	_maxLOD = 0;
	_maxAnisotropic = 1;
	_borderColor = 0;
	_srgbTexture = false;
}

ATOM_RenderAttributes::RenderState ATOM_SamplerAttributesBase::getType (void) const
{
	return ATOM_RenderAttributes::RS_Sampler;
}

void ATOM_SamplerAttributesBase::setFilter (ATOM_RenderAttributes::SamplerFilter filter)
{
	_filter = filter;
}

ATOM_RenderAttributes::SamplerFilter ATOM_SamplerAttributesBase::getFilter (void) const
{
	return _filter;
}

void ATOM_SamplerAttributesBase::setAddressU (SamplerAddress address)
{
	_addressU = address;
}

ATOM_RenderAttributes::SamplerAddress ATOM_SamplerAttributesBase::getAddressU (void) const
{
	return _addressU;
}

void ATOM_SamplerAttributesBase::setAddressV (SamplerAddress address)
{
	_addressV = address;
}

ATOM_RenderAttributes::SamplerAddress ATOM_SamplerAttributesBase::getAddressV (void) const
{
	return _addressV;
}

void ATOM_SamplerAttributesBase::setAddressW (SamplerAddress address)
{
	_addressW = address;
}

ATOM_RenderAttributes::SamplerAddress ATOM_SamplerAttributesBase::getAddressW (void) const
{
	return _addressW;
}

void ATOM_SamplerAttributesBase::setMipmapLodBias (float val)
{
	_mipmapLodBias = val;
}

float ATOM_SamplerAttributesBase::getMipmapLodBias (void) const
{
	return _mipmapLodBias;
}

void ATOM_SamplerAttributesBase::setMaxAnisotropic (unsigned val)
{
	_maxAnisotropic = val;
}

unsigned ATOM_SamplerAttributesBase::getMaxAnisotropic (void) const
{
	return _maxAnisotropic;
}

void ATOM_SamplerAttributesBase::setCompareFunc (ATOM_RenderAttributes::CompareFunc func)
{
	_compareFunc = func;
}

ATOM_RenderAttributes::CompareFunc ATOM_SamplerAttributesBase::getCompareFunc (void) const
{
	return _compareFunc;
}

void ATOM_SamplerAttributesBase::setBorderColor (ATOM_ColorARGB color)
{
	_borderColor = color;
}

ATOM_ColorARGB ATOM_SamplerAttributesBase::getBorderColor (void) const
{
	return _borderColor;
}

void ATOM_SamplerAttributesBase::setMaxLOD (int val)
{
	_maxLOD = val;
}

int ATOM_SamplerAttributesBase::getMaxLOD (void) const
{
	return _maxLOD;
}

void ATOM_SamplerAttributesBase::enableSRGBTexture (bool enable)
{
	_srgbTexture = enable;
}

bool ATOM_SamplerAttributesBase::isSRGBTextureEnabled (void) const
{
	return _srgbTexture;
}

void ATOM_SamplerAttributesBase::setTexture (ATOM_Texture *texture)
{
	_texture = texture;
}

ATOM_Texture *ATOM_SamplerAttributesBase::getTexture (void) const
{
	return _texture.get();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_TextureStageAttributesBase::ATOM_TextureStageAttributesBase (void)
{
	_colorOp = TextureOp_Unknown;
	_alphaOp = TextureOp_Unknown;
	_colorArg0 = TextureArg_Unknown;
	_colorArg1 = TextureArg_Unknown;
	_colorArg2 = TextureArg_Unknown;
	_alphaArg0 = TextureArg_Unknown;
	_alphaArg1 = TextureArg_Unknown;
	_alphaArg2 = TextureArg_Unknown;
	_texCoordGen = TexCoordGen_Unknown;
	_enableTransform = false;
	_matrix.makeIdentity ();
	_constant = 0xFFFFFFFF;
}

void ATOM_TextureStageAttributesBase::setDefaults (void)
{
	_colorOp = TextureOp_Disable;
	_alphaOp = TextureOp_Modulate;
	_colorArg0 = TextureArg_Current;
	_colorArg1 = TextureArg_Texture;
	_colorArg2 = TextureArg_Current;
	_alphaArg0 = TextureArg_Current;
	_alphaArg1 = TextureArg_Texture;
	_alphaArg2 = TextureArg_Current;
	_texCoordGen = TexCoordGen_Auto;
	_enableTransform = false;
	_matrix.makeIdentity ();
	_constant = 0xFFFFFFFF;
}

ATOM_RenderAttributes::RenderState ATOM_TextureStageAttributesBase::getType (void) const
{
	return ATOM_RenderAttributes::RS_TextureStage;
}

void ATOM_TextureStageAttributesBase::setColorOp (TextureOp op)
{
	_colorOp = op;
}

ATOM_RenderAttributes::TextureOp ATOM_TextureStageAttributesBase::getColorOp (void) const
{
	return _colorOp;
}

void ATOM_TextureStageAttributesBase::setAlphaOp (ATOM_RenderAttributes::TextureOp op)
{
	_alphaOp = op;
}

ATOM_RenderAttributes::TextureOp ATOM_TextureStageAttributesBase::getAlphaOp (void) const
{
	return _alphaOp;
}

void ATOM_TextureStageAttributesBase::setColorArg0 (ATOM_RenderAttributes::TextureArg arg)
{
	_colorArg0 = arg;
}

ATOM_RenderAttributes::TextureArg ATOM_TextureStageAttributesBase::getColorArg0 (void) const
{
	return _colorArg0;
}

void ATOM_TextureStageAttributesBase::setColorArg1 (ATOM_RenderAttributes::TextureArg arg)
{
	_colorArg1 = arg;
}

ATOM_RenderAttributes::TextureArg ATOM_TextureStageAttributesBase::getColorArg1 (void) const
{
	return _colorArg1;
}

void ATOM_TextureStageAttributesBase::setColorArg2 (ATOM_RenderAttributes::TextureArg arg)
{
	_colorArg2 = arg;
}

ATOM_RenderAttributes::TextureArg ATOM_TextureStageAttributesBase::getColorArg2 (void) const
{
	return _colorArg2;
}

void ATOM_TextureStageAttributesBase::setAlphaArg0 (ATOM_RenderAttributes::TextureArg arg)
{
	_alphaArg0 = arg;
}

ATOM_RenderAttributes::TextureArg ATOM_TextureStageAttributesBase::getAlphaArg0 (void) const
{
	return _alphaArg0;
}

void ATOM_TextureStageAttributesBase::setAlphaArg1 (ATOM_RenderAttributes::TextureArg arg)
{
	_alphaArg1 = arg;
}

ATOM_RenderAttributes::TextureArg ATOM_TextureStageAttributesBase::getAlphaArg1 (void) const
{
	return _alphaArg1;
}

void ATOM_TextureStageAttributesBase::setAlphaArg2 (ATOM_RenderAttributes::TextureArg arg)
{
	_alphaArg2 = arg;
}

ATOM_RenderAttributes::TextureArg ATOM_TextureStageAttributesBase::getAlphaArg2 (void) const
{
	return _alphaArg2;
}

void ATOM_TextureStageAttributesBase::setTexCoordGen (ATOM_RenderAttributes::TexCoordGen val)
{
	_texCoordGen = val;
}

ATOM_RenderAttributes::TexCoordGen ATOM_TextureStageAttributesBase::getTexCoordGen (void) const
{
	return _texCoordGen;
}

void ATOM_TextureStageAttributesBase::enableTransform (bool enable)
{
	_enableTransform = enable;
}

bool ATOM_TextureStageAttributesBase::isTransformEnabled (void) const
{
	return _enableTransform;
}

void ATOM_TextureStageAttributesBase::setMatrix (const ATOM_Matrix4x4f &matrix)
{
	_matrix = matrix;
}

ATOM_Matrix4x4f ATOM_TextureStageAttributesBase::getMatrix (void) const
{
	return _matrix;
}

void ATOM_TextureStageAttributesBase::setConstant (ATOM_ColorARGB val)
{
	_constant = val;
}

ATOM_ColorARGB ATOM_TextureStageAttributesBase::getConstant (void) const
{
	return _constant;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_FogAttributesBase::ATOM_FogAttributesBase (void)
{
	_enabled = false;
	_fogMode = FogMode_Unknown;
	_fogNear = 0.f;
	_fogFar = 1.f;
	_fogDensity = 1.f;
	_fogColor = 0;
}

void ATOM_FogAttributesBase::setDefaults (void)
{
	_enabled = false;
	_fogMode = FogMode_None;
	_fogNear = 0.f;
	_fogFar = 1.f;
	_fogDensity = 1.f;
	_fogColor = 0;
}

ATOM_RenderAttributes::RenderState ATOM_FogAttributesBase::getType (void) const
{
	return ATOM_RenderAttributes::RS_Fog;
}

void ATOM_FogAttributesBase::enableFog (bool enable)
{
	_enabled = enable;
}

bool ATOM_FogAttributesBase::isFogEnabled (void) const
{
	return _enabled;
}

void ATOM_FogAttributesBase::setFogMode (FogMode mode)
{
	_fogMode = mode;
}

ATOM_RenderAttributes::FogMode ATOM_FogAttributesBase::getFogMode (void) const
{
	return _fogMode;
}

void ATOM_FogAttributesBase::setFogNear (float val)
{
	_fogNear = val;
}

float ATOM_FogAttributesBase::getFogNear (void) const
{
	return _fogNear;
}

void ATOM_FogAttributesBase::setFogFar (float val)
{
	_fogFar = val;
}

float ATOM_FogAttributesBase::getFogFar (void) const
{
	return _fogFar;
}

void ATOM_FogAttributesBase::setFogDensity (float val)
{
	_fogDensity = val;
}

float ATOM_FogAttributesBase::getFogDensity (void) const
{
	return _fogDensity;
}

void ATOM_FogAttributesBase::setFogColor (ATOM_ColorARGB color)
{
	_fogColor = color;
}

ATOM_ColorARGB ATOM_FogAttributesBase::getFogColor (void) const
{
	return _fogColor;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_AlphaTestAttributesBase::ATOM_AlphaTestAttributesBase (void)
{
	_enableAlphaTest = false;
	_alphaFunc = ATOM_RenderAttributes::CompareFunc_Unknown;
	_alphaRef = 0.f;
}

void ATOM_AlphaTestAttributesBase::setDefaults (void)
{
	_enableAlphaTest = false;
	_alphaFunc = ATOM_RenderAttributes::CompareFunc_Greater;
	_alphaRef = 0.f;
}

ATOM_RenderAttributes::RenderState ATOM_AlphaTestAttributesBase::getType (void) const
{
	return ATOM_RenderAttributes::RS_AlphaTest;
}

void ATOM_AlphaTestAttributesBase::enableAlphaTest (bool enable)
{
	_enableAlphaTest = enable;
}

bool ATOM_AlphaTestAttributesBase::isAlphaTestEnabled (void) const
{
	return _enableAlphaTest;
}

void ATOM_AlphaTestAttributesBase::setAlphaFunc (ATOM_RenderAttributes::CompareFunc func)
{
	_alphaFunc = func;
}

ATOM_RenderAttributes::CompareFunc ATOM_AlphaTestAttributesBase::getAlphaFunc (void) const
{
	return _alphaFunc;
}

void ATOM_AlphaTestAttributesBase::setAlphaRef (float val)
{
	_alphaRef = val;
}

float ATOM_AlphaTestAttributesBase::getAlphaRef (void) const
{
	return _alphaRef;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_RenderStateSetBase::ATOM_RenderStateSetBase ()
:_driver (0)
{
}

ATOM_RenderStateSetBase::~ATOM_RenderStateSetBase (void)
{
}

void ATOM_RenderStateSetBase::setGfxDriver (ATOM_GfxDriver *driver)
{
	_driver = driver;
}

ATOM_GfxDriver *ATOM_RenderStateSetBase::getGfxDriver (void) const
{
	return _driver;
}

ATOM_AlphaBlendAttributes *ATOM_RenderStateSetBase::useAlphaBlending (ATOM_AlphaBlendAttributes *state)
{
	if (state)
	{
		_alphaBlendAttributes = state;
	}
	else if (!_alphaBlendAttributes)
	{
		_alphaBlendAttributes = _driver->allocRenderAttributes(ATOM_RenderAttributes::RS_AlphaBlending);
	}
	return _alphaBlendAttributes.get();
}

void ATOM_RenderStateSetBase::unuseAlphaBlending (void)
{
	_alphaBlendAttributes = 0;
}

ATOM_ColorWriteAttributes *ATOM_RenderStateSetBase::useColorWrite (ATOM_ColorWriteAttributes *state)
{
	if (state)
	{
		_colorWriteAttributes = state;
	}
	else if (!_colorWriteAttributes)
	{
		_colorWriteAttributes = _driver->allocRenderAttributes(ATOM_RenderAttributes::RS_ColorWrite);
	}
	return _colorWriteAttributes.get();
}

void ATOM_RenderStateSetBase::unuseColorWrite (void)
{
	_colorWriteAttributes = 0;
}

ATOM_DepthAttributes *ATOM_RenderStateSetBase::useDepth (ATOM_DepthAttributes *state)
{
	if (state)
	{
		_depthAttributes = state;
	}
	else if (!_depthAttributes)
	{
		_depthAttributes = _driver->allocRenderAttributes(ATOM_RenderAttributes::RS_Depth);
	}
	return _depthAttributes.get();
}

void ATOM_RenderStateSetBase::unuseDepth (void)
{
	_depthAttributes = 0;
}

ATOM_StencilAttributes *ATOM_RenderStateSetBase::useStencil (ATOM_StencilAttributes *state)
{
	if (state)
	{
		_stencilAttributes = state;
	}
	else if (!_stencilAttributes)
	{
		_stencilAttributes = _driver->allocRenderAttributes(ATOM_RenderAttributes::RS_Stencil);
	}
	return _stencilAttributes.get();
}

void ATOM_RenderStateSetBase::unuseStencil (void)
{
	_stencilAttributes = 0;
}

ATOM_RasterizerAttributes *ATOM_RenderStateSetBase::useRasterizer (ATOM_RasterizerAttributes *state)
{
	if (state)
	{
		_rasterizerAttributes = state;
	}
	else if (!_rasterizerAttributes)
	{
		_rasterizerAttributes = _driver->allocRenderAttributes(ATOM_RenderAttributes::RS_Rasterizer);
	}
	return _rasterizerAttributes.get();
}

void ATOM_RenderStateSetBase::unuseRasterizer (void)
{
	_rasterizerAttributes = 0;
}

ATOM_MultisampleAttributes *ATOM_RenderStateSetBase::useMultisample (ATOM_MultisampleAttributes *state)
{
	if (state)
	{
		_multisampleAttributes = state;
	}
	else if (!_multisampleAttributes)
	{
		_multisampleAttributes = _driver->allocRenderAttributes(ATOM_RenderAttributes::RS_Multisample);
	}
	return _multisampleAttributes.get();
}

void ATOM_RenderStateSetBase::unuseMultisample (void)
{
	_multisampleAttributes = 0;
}

ATOM_SamplerAttributes *ATOM_RenderStateSetBase::useSampler (int index, ATOM_SamplerAttributes *state)
{
	if (index < 0 || index >= 16)
	{
		return 0;
	}

	if (state)
	{
		_samplerAttributes[index] = state;
	}
	else if (!_samplerAttributes[index])
	{
		_samplerAttributes[index] = _driver->allocRenderAttributes(ATOM_RenderAttributes::RS_Sampler);
	}

	return _samplerAttributes[index].get();
}

void ATOM_RenderStateSetBase::unuseSampler (int index)
{
	if (index >= 0 && index < 16)
	{
		_samplerAttributes[index] = 0;
	}
}

ATOM_TextureStageAttributes *ATOM_RenderStateSetBase::useTextureStage (int index, ATOM_TextureStageAttributes *state)
{
	if (index < 0 || index >= 16)
	{
		return 0;
	}

	if (state)
	{
		_textureStageAttributes[index] = state;
	}
	else if (!_textureStageAttributes[index])
	{
		_textureStageAttributes[index] = _driver->allocRenderAttributes(ATOM_RenderAttributes::RS_TextureStage);
	}

	return _textureStageAttributes[index].get();
}

void ATOM_RenderStateSetBase::unuseTextureStage (int index)
{
	if (index >= 0 && index < 16)
	{
		_textureStageAttributes[index] = 0;
	}
}

ATOM_FogAttributes *ATOM_RenderStateSetBase::useFog (ATOM_FogAttributes *state)
{
	if (state)
	{
		_fogAttributes = state;
	}
	else if (!_fogAttributes)
	{
		_fogAttributes = _driver->allocRenderAttributes(ATOM_RenderAttributes::RS_Fog);
	}
	return _fogAttributes.get();
}

void ATOM_RenderStateSetBase::unuseFog (void)
{
	_fogAttributes = 0;
}

ATOM_AlphaTestAttributes *ATOM_RenderStateSetBase::useAlphaTest (ATOM_AlphaTestAttributes *state)
{
	if (state)
	{
		_alphaTestAttributes = state;
	}
	else if (!_alphaTestAttributes)
	{
		_alphaTestAttributes = _driver->allocRenderAttributes(ATOM_RenderAttributes::RS_AlphaTest);
	}
	return _alphaTestAttributes.get();
}

void ATOM_RenderStateSetBase::unuseAlphaTest (void)
{
	_alphaTestAttributes = 0;
}

void ATOM_RenderStateSetBase::replace (ATOM_RenderStateSet *other)
{
	if (_alphaBlendAttributes)
	{
		ATOM_AlphaBlendAttributes *otherStates = other->useAlphaBlending (0);
		_alphaBlendAttributes = (_alphaBlendAttributes == otherStates) ? 0 : otherStates;
	}

	if (_colorWriteAttributes)
	{
		ATOM_ColorWriteAttributes *otherStates = other->useColorWrite(0);
		_colorWriteAttributes = (_colorWriteAttributes == otherStates) ? 0 : otherStates;
	}

	if (_depthAttributes)
	{
		ATOM_DepthAttributes *otherStates = other->useDepth (0);
		_depthAttributes = (_depthAttributes == otherStates) ? 0 : otherStates;
	}

	if (_stencilAttributes)
	{
		ATOM_StencilAttributes *otherStates = other->useStencil (0);
		_stencilAttributes = (_stencilAttributes == otherStates) ? 0 : otherStates;
	}

	if (_rasterizerAttributes)
	{
		ATOM_RasterizerAttributes *otherStates = other->useRasterizer (0);
		_rasterizerAttributes = (_rasterizerAttributes == otherStates) ? 0 : otherStates;
	}

	if (_multisampleAttributes)
	{
		ATOM_MultisampleAttributes *otherStates = other->useMultisample (0);
		_multisampleAttributes = (_multisampleAttributes == otherStates) ? 0 : otherStates;
	}

	if (_fogAttributes)
	{
		ATOM_FogAttributes *otherStates = other->useFog (0);
		_fogAttributes = (_fogAttributes == otherStates) ? 0 : otherStates;
	}

	if (_alphaTestAttributes)
	{
		ATOM_AlphaTestAttributes *otherStates = other->useAlphaTest (0);
		_alphaTestAttributes = (_alphaTestAttributes == otherStates) ? 0 : otherStates;
	}

	for (unsigned i = 0; i < 16; ++i)
	{
		if (_samplerAttributes[i])
		{
			ATOM_SamplerAttributes *otherStates = other->useSampler (i, 0);
			_samplerAttributes[i] = (_samplerAttributes[i] == otherStates) ? 0 : otherStates;
		}

		if (_textureStageAttributes[i])
		{
			ATOM_TextureStageAttributes *otherStates = other->useTextureStage (i, 0);
			_textureStageAttributes[i] = (_textureStageAttributes[i] == otherStates) ? 0 : otherStates;
		}
	}
}

bool ATOM_RenderStateSetBase::apply (int pass, bool force)
{
	if (_alphaBlendAttributes)
	{
		getGfxDriver()->applyAlphaBlendState (_alphaBlendAttributes.get(), pass, force);
	}

	if (_colorWriteAttributes)
	{
		getGfxDriver()->applyColorWriteState (_colorWriteAttributes.get(), pass, force);
	}

	if (_depthAttributes)
	{
		getGfxDriver()->applyDepthState (_depthAttributes.get(), pass, force);
	}

	if (_stencilAttributes)
	{
		getGfxDriver()->applyStencilState (_stencilAttributes.get(), pass, force);
	}

	if (_rasterizerAttributes)
	{
		getGfxDriver()->applyRasterizerState (_rasterizerAttributes.get(), pass, force);
	}

	if (_multisampleAttributes)
	{
		getGfxDriver()->applyMultisampleState (_multisampleAttributes.get(), pass, force);
	}

	if (_fogAttributes)
	{
		getGfxDriver()->applyFogState (_fogAttributes.get(), pass, force);
	}

	if (_alphaTestAttributes)
	{
		getGfxDriver()->applyAlphaTestState (_alphaTestAttributes.get(), pass, force);
	}

	for (unsigned i = 0; i < 16; ++i)
	{
		if (_samplerAttributes[i])
		{
			getGfxDriver()->applySamplerState (_samplerAttributes[i].get(), i, pass, force);
		}

		if (_textureStageAttributes[i])
		{
			getGfxDriver()->applyTextureStageState (_textureStageAttributes[i].get(), i, pass, force);
		}
	}

	return true;
}

bool ATOM_RenderStateSetBase::commit (void)
{
	ATOM_RenderStateSet *desired = getGfxDriver ()->getStateCache ()->getDesiredStates();
	
	if (_alphaBlendAttributes)
	{
		desired->useAlphaBlending (_alphaBlendAttributes.get());
	}

	if (_colorWriteAttributes)
	{
		desired->useColorWrite (_colorWriteAttributes.get());
	}

	if (_depthAttributes)
	{
		desired->useDepth (_depthAttributes.get());
	}

	if (_stencilAttributes)
	{
		desired->useStencil (_stencilAttributes.get());
	}

	if (_rasterizerAttributes)
	{
		desired->useRasterizer (_rasterizerAttributes.get());
	}

	if (_multisampleAttributes)
	{
		desired->useMultisample (_multisampleAttributes.get());
	}

	if (_fogAttributes)
	{
		desired->useFog (_fogAttributes.get());
	}

	if (_alphaTestAttributes)
	{
		desired->useAlphaTest (_alphaTestAttributes.get());
	}

	for (unsigned i = 0; i < 16; ++i)
	{
		if (_samplerAttributes[i])
		{
			desired->useSampler (i, _samplerAttributes[i].get());
		}

		if (_textureStageAttributes[i])
		{
			desired->useTextureStage (i, _textureStageAttributes[i].get());
		}
	}

	return true;
}

