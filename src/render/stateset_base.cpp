#include "StdAfx.h"
#include "stateset_base.h"
#include "statecache.h"

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
	_mipmapLodBias = -1.5f;
	_maxLOD = 0;
	_maxAnisotropic = 16;
	_borderColor = 0;
	_srgbTexture = false;
}

void ATOM_SamplerAttributesBase::setDefaults (void)
{
	_filter = SamplerFilter_LLP;
	_addressU = SamplerAddress_Wrap;
	_addressV = SamplerAddress_Wrap;
	_addressW = SamplerAddress_Wrap;
	_compareFunc = ATOM_RenderAttributes::CompareFunc_Always;
	_mipmapLodBias = -1.5f;
	_maxLOD = 0;
	_maxAnisotropic = 16;
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

void ATOM_SamplerAttributesBase::setMaxLOD (float val)
{
	_maxLOD = val;
}

float ATOM_SamplerAttributesBase::getMaxLOD (void) const
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
:_device (0)
{
}

ATOM_RenderStateSetBase::~ATOM_RenderStateSetBase (void)
{
}

void ATOM_RenderStateSetBase::setDevice (ATOM_RenderDevice *device)
{
	_device = device;
}

ATOM_RenderDevice *ATOM_RenderStateSetBase::getDevice (void) const
{
	return _device;
}

ATOM_AlphaBlendAttributes *ATOM_RenderStateSetBase::useAlphaBlending (ATOM_AlphaBlendAttributes *state)
{
	if (state)
	{
		_alphaBlendAttributes = state;
	}
	else if (!_alphaBlendAttributes)
	{
		_alphaBlendAttributes = _device->allocRenderAttributes(ATOM_RenderAttributes::RS_AlphaBlending);
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
		_colorWriteAttributes = _device->allocRenderAttributes(ATOM_RenderAttributes::RS_ColorWrite);
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
		_depthAttributes = _device->allocRenderAttributes(ATOM_RenderAttributes::RS_Depth);
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
		_stencilAttributes = _device->allocRenderAttributes(ATOM_RenderAttributes::RS_Stencil);
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
		_rasterizerAttributes = _device->allocRenderAttributes(ATOM_RenderAttributes::RS_Rasterizer);
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
		_multisampleAttributes = _device->allocRenderAttributes(ATOM_RenderAttributes::RS_Multisample);
	}
	return _multisampleAttributes.get();
}

void ATOM_RenderStateSetBase::unuseMultisample (void)
{
	_multisampleAttributes = 0;
}

ATOM_SamplerAttributes *ATOM_RenderStateSetBase::useSampler (int index, ATOM_SamplerAttributes *state)
{
	if (index < 0 || index >= ATOM_RenderAttributes::SamplerCount)
	{
		return 0;
	}

	if (state)
	{
		_samplerAttributes[index] = state;
	}
	else if (!_samplerAttributes[index])
	{
		_samplerAttributes[index] = _device->allocRenderAttributes(ATOM_RenderAttributes::RS_Sampler);
	}

	return _samplerAttributes[index].get();
}

void ATOM_RenderStateSetBase::unuseSampler (int index)
{
	if (index >= 0 && index < ATOM_RenderAttributes::SamplerCount)
	{
		_samplerAttributes[index] = 0;
	}
}

ATOM_AlphaTestAttributes *ATOM_RenderStateSetBase::useAlphaTest (ATOM_AlphaTestAttributes *state)
{
	if (state)
	{
		_alphaTestAttributes = state;
	}
	else if (!_alphaTestAttributes)
	{
		_alphaTestAttributes = _device->allocRenderAttributes(ATOM_RenderAttributes::RS_AlphaTest);
	}
	return _alphaTestAttributes.get();
}

void ATOM_RenderStateSetBase::unuseAlphaTest (void)
{
	_alphaTestAttributes = 0;
}

void ATOM_RenderStateSetBase::replace (ATOM_RenderStateSet *other)
{
	if (1 || _alphaBlendAttributes)
	{
		ATOM_AlphaBlendAttributes *otherStates = other->useAlphaBlending (0);
		_alphaBlendAttributes = otherStates;
	}

	if (1 || _colorWriteAttributes)
	{
		ATOM_ColorWriteAttributes *otherStates = other->useColorWrite(0);
		_colorWriteAttributes = otherStates;
	}

	if (1 || _depthAttributes)
	{
		ATOM_DepthAttributes *otherStates = other->useDepth (0);
		_depthAttributes = otherStates;
	}

	if (1 || _stencilAttributes)
	{
		ATOM_StencilAttributes *otherStates = other->useStencil (0);
		_stencilAttributes = otherStates;
	}

	if (1 || _rasterizerAttributes)
	{
		ATOM_RasterizerAttributes *otherStates = other->useRasterizer (0);
		_rasterizerAttributes = otherStates;
	}

	if (1 || _multisampleAttributes)
	{
		ATOM_MultisampleAttributes *otherStates = other->useMultisample (0);
		_multisampleAttributes = otherStates;
	}

	if (1 || _alphaTestAttributes)
	{
		ATOM_AlphaTestAttributes *otherStates = other->useAlphaTest (0);
		_alphaTestAttributes = otherStates;
	}

	for (unsigned i = 0; i < ATOM_RenderAttributes::SamplerCount; ++i)
	{
		if (1 || _samplerAttributes[i])
		{
			ATOM_SamplerAttributes *otherStates = other->useSampler (i, 0);
			_samplerAttributes[i] = otherStates;
		}
	}
}

bool ATOM_RenderStateSetBase::apply (int pass, bool force)
{
	if (_alphaBlendAttributes)
	{
		getDevice()->applyAlphaBlendState (_alphaBlendAttributes.get(), pass, force);
	}

	if (_colorWriteAttributes)
	{
		getDevice()->applyColorWriteState (_colorWriteAttributes.get(), pass, force);
	}

	if (_depthAttributes)
	{
		getDevice()->applyDepthState (_depthAttributes.get(), pass, force);
	}

	if (_stencilAttributes)
	{
		getDevice()->applyStencilState (_stencilAttributes.get(), pass, force);
	}

	if (_rasterizerAttributes)
	{
		getDevice()->applyRasterizerState (_rasterizerAttributes.get(), pass, force);
	}

	if (_multisampleAttributes)
	{
		getDevice()->applyMultisampleState (_multisampleAttributes.get(), pass, force);
	}

	if (_alphaTestAttributes)
	{
		getDevice()->applyAlphaTestState (_alphaTestAttributes.get(), pass, force);
	}

	for (unsigned i = 0; i < ATOM_RenderAttributes::SamplerCount; ++i)
	{
		if (_samplerAttributes[i])
		{
			getDevice()->applySamplerState (_samplerAttributes[i].get(), i, pass, force);
		}
	}

	return true;
}

bool ATOM_RenderStateSetBase::commit (void)
{
	ATOM_RenderStateSet *desired = getDevice ()->getStateCache ()->getDesiredStates();
	
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

	if (_alphaTestAttributes)
	{
		desired->useAlphaTest (_alphaTestAttributes.get());
	}

	for (unsigned i = 0; i < ATOM_RenderAttributes::SamplerCount; ++i)
	{
		if (_samplerAttributes[i])
		{
			desired->useSampler (i, _samplerAttributes[i].get());
		}
		else
		{
			desired->unuseSampler (i);
		}
	}

	return true;
}

