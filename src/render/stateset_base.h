#ifndef __ATOM3D_RENDER_STATESET_BASE_H
#define __ATOM3D_RENDER_STATESET_BASE_H

#include "stateset.h"

class ATOM_MaterialEffect;

class ATOM_AlphaBlendAttributesBase: public ATOM_AlphaBlendAttributes
{
public:
	ATOM_AlphaBlendAttributesBase (void);
	
public:
	virtual void setDefaults (void);
	virtual RenderState getType (void) const;
	virtual void enableAlphaBlending (bool enable);
	virtual bool isAlphaBlendingEnabled (void) const;
	virtual void setSrcBlend (BlendFunc func);
	virtual BlendFunc getSrcBlend (void) const;
	virtual void setDestBlend (BlendFunc func);
	virtual BlendFunc getDestBlend (void) const;
	virtual void setBlendConstant (ATOM_ColorARGB val);
	virtual ATOM_ColorARGB getBlendConstant (void) const;
	
protected:
	bool _enableAlphaBlending;
	BlendFunc _srcBlend, _destBlend;
	ATOM_ColorARGB _blendConstant;
};

class ATOM_ColorWriteAttributesBase: public ATOM_ColorWriteAttributes
{
public:
	ATOM_ColorWriteAttributesBase (void);
	
public:
	virtual void setDefaults (void);
	virtual RenderState getType (void) const;
	virtual void setColorWriteMask (unsigned char mask);
	virtual unsigned char getColorWriteMask (void) const;
	
protected:
	unsigned char _writeMask;
};

class ATOM_DepthAttributesBase: public ATOM_DepthAttributes
{
public:
	ATOM_DepthAttributesBase (void);
	
public:
	virtual void setDefaults (void);
	virtual RenderState getType (void) const;
	virtual void enableDepthWrite (bool enable);
	virtual bool isDepthWriteEnabled (void) const;
	virtual void setDepthFunc (CompareFunc func);
	virtual CompareFunc getDepthFunc (void) const;
	
protected:
	bool _enableDepthWrite;
	CompareFunc _depthFunc;
};

class ATOM_StencilAttributesBase: public ATOM_StencilAttributes
{
public:
	ATOM_StencilAttributesBase (void);
	
public:
	virtual void setDefaults (void);
	virtual RenderState getType (void) const;
	virtual void enableStencil (bool enable);
	virtual bool isStencilEnabled (void) const;
	virtual void setStencilReadMask (unsigned mask);
	virtual unsigned getStencilReadMask (void) const;
	virtual void setStencilWriteMask (unsigned mask);
	virtual unsigned getStencilWriteMask (void) const;
	virtual void setStencilFailOpFront (StencilOp op);
	virtual StencilOp getStencilFailOpFront (void) const;
	virtual void setStencilDepthFailOpFront (StencilOp op);
	virtual StencilOp getStencilDepthFailOpFront (void) const;
	virtual void setStencilPassOpFront (StencilOp op);
	virtual StencilOp getStencilPassOpFront (void) const;
	virtual void setStencilFuncFront (CompareFunc func);
	virtual CompareFunc getStencilFuncFront (void) const;
	virtual void setStencilRef (int ref);
	virtual int getStencilRef (void) const;
	virtual void enableTwoSidedStencil (bool enable);
	virtual bool isTwoSidedStencilEnabled (void) const;
	virtual void setStencilFailOpBack (StencilOp op);
	virtual StencilOp getStencilFailOpBack (void) const;
	virtual void setStencilDepthFailOpBack (StencilOp op);
	virtual StencilOp getStencilDepthFailOpBack (void) const;
	virtual void setStencilPassOpBack (StencilOp op);
	virtual StencilOp getStencilPassOpBack (void) const;
	virtual void setStencilFuncBack (CompareFunc func);
	virtual CompareFunc getStencilFuncBack (void) const;
	
protected:
	bool _enableStencil;
	unsigned _stencilReadMask;
	unsigned _stencilWriteMask;
	int _stencilRef;
	StencilOp _failOpFront;
	StencilOp _depthFailOpFront;
	StencilOp _passOpFront;
	CompareFunc _stencilFuncFront;
	bool _enableTwoSidedStencil;
	StencilOp _failOpBack;
	StencilOp _depthFailOpBack;
	StencilOp _passOpBack;
	CompareFunc _stencilFuncBack;
};

class ATOM_RasterizerAttributesBase: public ATOM_RasterizerAttributes
{
public:
	ATOM_RasterizerAttributesBase (void);
	
public:
	virtual void setDefaults (void);
	virtual RenderState getType (void) const;
	virtual void setFillMode (FillMode mode);
	virtual FillMode getFillMode (void) const;
	virtual void setCullMode (CullMode mode);
	virtual CullMode getCullMode (void) const;
	virtual void setFrontFace (FrontFace mode);
	virtual FrontFace getFrontFace (void) const;
	
protected:
	FillMode _fillMode;
	CullMode _cullMode;
	FrontFace _frontFace;
};

class ATOM_MultisampleAttributesBase: public ATOM_MultisampleAttributes
{
public:
	ATOM_MultisampleAttributesBase (void);
	
public:
	virtual void setDefaults (void);
	virtual RenderState getType (void) const;
	virtual void enableMultisample (bool enable);
	virtual bool isMultisampleEnabled (void) const;
	virtual void enableAlphaToCoverage (bool enable);
	virtual bool isAlphaToCoverageEnabled (void) const;
	
protected:
	bool _enableMultisample;
	bool _enableAlphaToCoverage;
};

class ATOM_SamplerAttributesBase: public ATOM_SamplerAttributes
{
public:
	ATOM_SamplerAttributesBase (void);
	
public:
	virtual void setDefaults (void);
	virtual RenderState getType (void) const;
	virtual void setFilter (SamplerFilter filter);
	virtual SamplerFilter getFilter (void) const;
	virtual void setAddressU (SamplerAddress address);
	virtual SamplerAddress getAddressU (void) const;
	virtual void setAddressV (SamplerAddress address);
	virtual SamplerAddress getAddressV (void) const;
	virtual void setAddressW (SamplerAddress address);
	virtual SamplerAddress getAddressW (void) const;
	virtual void setMipmapLodBias (float val);
	virtual float getMipmapLodBias (void) const;
	virtual void setMaxAnisotropic (unsigned val);
	virtual unsigned getMaxAnisotropic (void) const;
	virtual void setCompareFunc (CompareFunc func);
	virtual CompareFunc getCompareFunc (void) const;
	virtual void setBorderColor (ATOM_ColorARGB color);
	virtual ATOM_ColorARGB getBorderColor (void) const;
	virtual void setMaxLOD (float val);
	virtual float getMaxLOD (void) const;
	virtual void enableSRGBTexture (bool enable);
	virtual bool isSRGBTextureEnabled (void) const;
	virtual void setTexture (ATOM_Texture *texture);
	virtual ATOM_Texture *getTexture (void) const;

protected:
	SamplerFilter _filter;
	SamplerAddress _addressU, _addressV, _addressW;
	CompareFunc _compareFunc;
	float _mipmapLodBias;
	int _maxLOD;
	unsigned _maxAnisotropic;
	ATOM_ColorARGB _borderColor;
	bool _srgbTexture;
	ATOM_AUTOREF(ATOM_Texture) _texture;
};

class ATOM_AlphaTestAttributesBase: public ATOM_AlphaTestAttributes
{
public:
	ATOM_AlphaTestAttributesBase (void);
	
public:
	virtual void setDefaults (void);
	virtual RenderState getType (void) const;
	virtual void enableAlphaTest (bool enable);
	virtual bool isAlphaTestEnabled (void) const;
	virtual void setAlphaFunc (CompareFunc func);
	virtual CompareFunc getAlphaFunc (void) const;
	virtual void setAlphaRef (float val);
	virtual float getAlphaRef (void) const;
	
protected:
	bool _enableAlphaTest;
	CompareFunc _alphaFunc;
	float _alphaRef;
};

class ATOM_RenderStateSetBase: public ATOM_RenderStateSet
{
	ATOM_CLASS(render, ATOM_RenderStateSetBase, ATOM_RenderStateSetBase)

public:
	ATOM_RenderStateSetBase (void);
	virtual ~ATOM_RenderStateSetBase (void);

public:
	virtual ATOM_AlphaBlendAttributes *useAlphaBlending (ATOM_AlphaBlendAttributes *state);
	virtual void unuseAlphaBlending (void);
	virtual ATOM_ColorWriteAttributes *useColorWrite (ATOM_ColorWriteAttributes *state);
	virtual void unuseColorWrite (void);
	virtual ATOM_DepthAttributes *useDepth (ATOM_DepthAttributes *state);
	virtual void unuseDepth (void);
	virtual ATOM_StencilAttributes *useStencil (ATOM_StencilAttributes *state);
	virtual void unuseStencil (void);
	virtual ATOM_RasterizerAttributes *useRasterizer (ATOM_RasterizerAttributes *state);
	virtual void unuseRasterizer (void);
	virtual ATOM_MultisampleAttributes *useMultisample (ATOM_MultisampleAttributes *state);
	virtual void unuseMultisample (void);
	virtual ATOM_SamplerAttributes *useSampler (int index, ATOM_SamplerAttributes *state);
	virtual void unuseSampler (int index);
	virtual ATOM_AlphaTestAttributes *useAlphaTest (ATOM_AlphaTestAttributes *state);
	virtual void unuseAlphaTest (void);
	virtual bool commit (void);
	virtual bool apply (int pass, bool force);
	virtual void replace (ATOM_RenderStateSet *other);
	virtual void setDevice (ATOM_RenderDevice *device);
	virtual ATOM_RenderDevice *getDevice (void) const;

public:
	ATOM_RenderDevice *_device;

	ATOM_AUTOPTR(ATOM_AlphaBlendAttributes) _alphaBlendAttributes;
	ATOM_AUTOPTR(ATOM_ColorWriteAttributes) _colorWriteAttributes;
	ATOM_AUTOPTR(ATOM_DepthAttributes) _depthAttributes;
	ATOM_AUTOPTR(ATOM_StencilAttributes) _stencilAttributes;
	ATOM_AUTOPTR(ATOM_RasterizerAttributes) _rasterizerAttributes;
	ATOM_AUTOPTR(ATOM_MultisampleAttributes) _multisampleAttributes;
	ATOM_AUTOPTR(ATOM_SamplerAttributes) _samplerAttributes[ATOM_RenderAttributes::SamplerCount];
	ATOM_AUTOPTR(ATOM_AlphaTestAttributes) _alphaTestAttributes;
};

#endif // __ATOM3D_RENDER_STATESET_BASE_H
