#ifndef __ATOM3D_GFXDRIVER_RENDERSTATES_H
#define __ATOM3D_GFXDRIVER_RENDERSTATES_H

#include "../ATOM_kernel.h"

class ATOM_Texture;
class ATOM_GfxDriver;

class ATOM_RenderAttributes: public ATOM_ReferenceObj
{
public:
	enum PrimitiveType
	{
		PrimitiveType_Unknown = 0,
		PrimitiveType_TriList,
		PrimitiveType_TriStrip,
		PrimitiveType_TriFan,
		PrimitiveType_LineList,
		PrimitiveType_LineStrip,
		PrimitiveType_PointList,
		PrimitiveType_Count
	};

	enum ColorMask
	{
		ColorMask_Red = (1<<0),
		ColorMask_Green = (1<<1),
		ColorMask_Blue = (1<<2),
		ColorMask_Alpha = (1<<3),
		ColorMask_None = 0,
		ColorMask_All = ColorMask_Red|ColorMask_Green|ColorMask_Blue|ColorMask_Alpha
	};

	enum TextureType
	{
		TextureType_Unknown = 0,
		TextureType_2D,
		TextureType_Rect,
		TextureType_3D,
		TextureType_Cube,
		TextureType_Count
	};

	enum RenderState
	{
		RS_AlphaBlending,
		RS_AlphaTest,
		RS_ColorWrite,
		RS_Depth,
		RS_Stencil,
		RS_Rasterizer,
		RS_Sampler,
		RS_TextureStage,
		RS_Multisample,
		RS_Fog,
		
		RS_Count
	};

	enum ShadeMode
	{
		ShadeMode_Unknown = 0,
		ShadeMode_Flat,
		ShadeMode_Gouraud,
		ShadeMode_Count
	};

	// alpha blend functions
	enum BlendFunc
	{
		BlendFunc_Unknown = 0,
		BlendFunc_Zero,
		BlendFunc_One,
		BlendFunc_SrcColor,
		BlendFunc_InvSrcColor,
		BlendFunc_SrcAlpha,
		BlendFunc_InvSrcAlpha,
		BlendFunc_DestAlpha,
		BlendFunc_InvDestAlpha,
		BlendFunc_DestColor,
		BlendFunc_InvDestColor,
		BlendFunc_Constant,
		BlendFunc_InvConstant,
		BlendFunc_Count
	};

	enum BlendOp
	{
		BlendOp_Unknown = 0,
		BlendOp_Add,	
		BlendOp_Sub,
		BlendOp_InvSub,
		BlendOp_Min,
		BlendOp_Max,
		BlendOp_Count
	};

	// comparation functions
	enum CompareFunc
	{
		CompareFunc_Unknown = 0,
		CompareFunc_Never,
		CompareFunc_Always,
		CompareFunc_Less,
		CompareFunc_LessEqual,
		CompareFunc_Greater,
		CompareFunc_GreaterEqual,
		CompareFunc_Equal,
		CompareFunc_NotEqual,
		CompareFunc_Count
	};

	// stencil operations
	enum StencilOp
	{
		StencilOp_Unknown = 0,
		StencilOp_Keep,
		StencilOp_Zero,
		StencilOp_Replace,
		StencilOp_Inc,
		StencilOp_Dec,
		StencilOp_Invert,
		StencilOp_IncWrap,
		StencilOp_DecWrap,
		StencilOp_Count
	};

	enum FillMode
	{
		FillMode_Unknown = 0,
		FillMode_Solid,
		FillMode_Line,
		FillMode_Count
	};

	// Cull mode
	enum CullMode
	{
		CullMode_Unknown = 0,
		CullMode_None,
		CullMode_Front,
		CullMode_Back,
		CullMode_Count
	};

	// Front face
	enum FrontFace
	{
		FrontFace_Unknown = 0,
		FrontFace_CW,
		FrontFace_CCW,
		FrontFace_Count
	};

	// Sampler filter
	enum SamplerFilter
	{
		SamplerFilter_Unknown = 0,
		SamplerFilter_PPP,
		SamplerFilter_PPL,
		SamplerFilter_PLP,
		SamplerFilter_PLL,
		SamplerFilter_LPP,
		SamplerFilter_LPL,
		SamplerFilter_LLP,
		SamplerFilter_LLL,
		SamplerFilter_Anisotropic,
		SamplerFilter_Count
	};

	// Sampler address mode
	enum SamplerAddress
	{
		SamplerAddress_Unknown = 0,
		SamplerAddress_Wrap,
		SamplerAddress_Clamp,
		SamplerAddress_Mirror,
		SamplerAddress_Border,
		SamplerAddress_MirrorOnce,
		SamplerAddress_Count
	};

	// Texture stage blend op
	enum TextureOp
	{
		TextureOp_Unknown = 0,
		TextureOp_Disable,
		TextureOp_Replace,
		TextureOp_Modulate,
		TextureOp_Modulate2x,
		TextureOp_Modulate4x,
		TextureOp_Add,
		TextureOp_AddSigned,
		TextureOp_Sub,
		TextureOp_Dot3,
		TextureOp_Lerp,
		TextureOp_Count
	};

	// texture arg
	enum TextureArg
	{
		TextureArg_Unknown = 0,
		TextureArg_Constant,
		TextureArg_Current,
		TextureArg_Diffuse,
		TextureArg_Texture,
		TextureArg_Count
	};

	enum TexCoordGen
	{
		TexCoordGen_Unknown = 0,
		TexCoordGen_SphereMap,
		TexCoordGen_Auto,
		TexCoordGen_TexCoord0,
		TexCoordGen_TexCoord1,
		TexCoordGen_TexCoord2,
		TexCoordGen_TexCoord3,
		TexCoordGen_TexCoord4,
		TexCoordGen_TexCoord5,
		TexCoordGen_TexCoord6,
		TexCoordGen_TexCoord7,
		TexCoordGen_Count
	};

	// Fog mode
	enum FogMode
	{
		FogMode_Unknown = 0,
		FogMode_None,
		FogMode_Linear,
		FogMode_Exp,
		FogMode_Exp2,
		FogMode_Count
	};

public:
	virtual void setDefaults (void) = 0;
	virtual RenderState getType (void) const = 0;
};

class ATOM_AlphaBlendAttributes: public ATOM_RenderAttributes
{
public:
	virtual void enableAlphaBlending (bool enable) = 0;
	virtual bool isAlphaBlendingEnabled (void) const = 0;
	virtual void setSrcBlend (BlendFunc func) = 0;
	virtual BlendFunc getSrcBlend (void) const = 0;
	virtual void setDestBlend (BlendFunc func) = 0;
	virtual BlendFunc getDestBlend (void) const = 0;
	virtual void setBlendConstant (ATOM_ColorARGB val) = 0;
	virtual ATOM_ColorARGB getBlendConstant (void) const = 0;

};

class ATOM_ColorWriteAttributes: public ATOM_RenderAttributes
{
public:
	virtual void setColorWriteMask (unsigned char mask) = 0;
	virtual unsigned char getColorWriteMask (void) const = 0;
};

class ATOM_DepthAttributes: public ATOM_RenderAttributes
{
public:
	virtual void enableDepthWrite (bool enable) = 0;
	virtual bool isDepthWriteEnabled (void) const = 0;
	virtual void setDepthFunc (CompareFunc func) = 0;
	virtual CompareFunc getDepthFunc (void) const = 0;
};

class ATOM_StencilAttributes: public ATOM_RenderAttributes
{
public:
	virtual void enableStencil (bool enable) = 0;
	virtual bool isStencilEnabled (void) const = 0;
	virtual void enableTwoSidedStencil (bool enable) = 0;
	virtual bool isTwoSidedStencilEnabled (void) const = 0;
	virtual void setStencilReadMask (unsigned mask) = 0;
	virtual unsigned getStencilReadMask (void) const = 0;
	virtual void setStencilWriteMask (unsigned mask) = 0;
	virtual unsigned getStencilWriteMask (void) const = 0;
	virtual void setStencilFailOpFront (StencilOp op) = 0;
	virtual StencilOp getStencilFailOpFront (void) const = 0;
	virtual void setStencilDepthFailOpFront (StencilOp op) = 0;
	virtual StencilOp getStencilDepthFailOpFront (void) const = 0;
	virtual void setStencilPassOpFront (StencilOp op) = 0;
	virtual StencilOp getStencilPassOpFront (void) const = 0;
	virtual void setStencilFuncFront (CompareFunc func) = 0;
	virtual CompareFunc getStencilFuncFront (void) const = 0;
	virtual void setStencilRef (int ref) = 0;
	virtual int getStencilRef (void) const = 0;
	virtual void setStencilFailOpBack (StencilOp op) = 0;
	virtual StencilOp getStencilFailOpBack (void) const = 0;
	virtual void setStencilDepthFailOpBack (StencilOp op) = 0;
	virtual StencilOp getStencilDepthFailOpBack (void) const = 0;
	virtual void setStencilPassOpBack (StencilOp op) = 0;
	virtual StencilOp getStencilPassOpBack (void) const = 0;
	virtual void setStencilFuncBack (CompareFunc func) = 0;
	virtual CompareFunc getStencilFuncBack (void) const = 0;
};

class ATOM_RasterizerAttributes: public ATOM_RenderAttributes
{
public:
	virtual void setFillMode (FillMode mode) = 0;
	virtual FillMode getFillMode (void) const = 0;
	virtual void setCullMode (CullMode mode) = 0;
	virtual CullMode getCullMode (void) const = 0;
	virtual void setFrontFace (FrontFace mode) = 0;
	virtual FrontFace getFrontFace (void) const = 0;
};

class ATOM_MultisampleAttributes: public ATOM_RenderAttributes
{
public:
	virtual void enableMultisample (bool enable) = 0;
	virtual bool isMultisampleEnabled (void) const = 0;
	virtual void enableAlphaToCoverage (bool enable) = 0;
	virtual bool isAlphaToCoverageEnabled (void) const = 0;
};

class ATOM_SamplerAttributes: public ATOM_RenderAttributes
{
public:
	virtual void setFilter (SamplerFilter filter) = 0;
	virtual SamplerFilter getFilter (void) const = 0;
	virtual void setAddressU (SamplerAddress address) = 0;
	virtual SamplerAddress getAddressU (void) const = 0;
	virtual void setAddressV (SamplerAddress address) = 0;
	virtual SamplerAddress getAddressV (void) const = 0;
	virtual void setAddressW (SamplerAddress address) = 0;
	virtual SamplerAddress getAddressW (void) const = 0;
	virtual void setMipmapLodBias (float val) = 0;
	virtual float getMipmapLodBias (void) const = 0;
	virtual void setMaxAnisotropic (unsigned val) = 0;
	virtual unsigned getMaxAnisotropic (void) const = 0;
	virtual void setCompareFunc (CompareFunc func) = 0;
	virtual CompareFunc getCompareFunc (void) const = 0;
	virtual void setBorderColor (ATOM_ColorARGB color) = 0;
	virtual ATOM_ColorARGB getBorderColor (void) const = 0;
	virtual void setMaxLOD (int val) = 0;
	virtual int getMaxLOD (void) const = 0;
	virtual void enableSRGBTexture (bool enable) = 0;
	virtual bool isSRGBTextureEnabled (void) const = 0;
	virtual void setTexture (ATOM_Texture *texture) = 0;
	virtual ATOM_Texture *getTexture (void) const = 0;
};

class ATOM_TextureStageAttributes: public ATOM_RenderAttributes
{
public:
	virtual void setColorOp (TextureOp op) = 0;
	virtual TextureOp getColorOp (void) const = 0;
	virtual void setAlphaOp (TextureOp op) = 0;
	virtual TextureOp getAlphaOp (void) const = 0;
	virtual void setColorArg0 (TextureArg arg) = 0;
	virtual TextureArg getColorArg0 (void) const = 0;
	virtual void setColorArg1 (TextureArg arg) = 0;
	virtual TextureArg getColorArg1 (void) const = 0;
	virtual void setColorArg2 (TextureArg arg) = 0;
	virtual TextureArg getColorArg2 (void) const = 0;
	virtual void setAlphaArg0 (TextureArg arg) = 0;
	virtual TextureArg getAlphaArg0 (void) const = 0;
	virtual void setAlphaArg1 (TextureArg arg) = 0;
	virtual TextureArg getAlphaArg1 (void) const = 0;
	virtual void setAlphaArg2 (TextureArg arg) = 0;
	virtual TextureArg getAlphaArg2 (void) const = 0;
	virtual void setTexCoordGen (TexCoordGen val) = 0;
	virtual TexCoordGen getTexCoordGen (void) const = 0;
	virtual void enableTransform (bool enable) = 0;
	virtual bool isTransformEnabled (void) const = 0;
	virtual void setMatrix (const ATOM_Matrix4x4f &matrix) = 0;
	virtual ATOM_Matrix4x4f getMatrix (void) const = 0;
	virtual void setConstant (ATOM_ColorARGB val) = 0;
	virtual ATOM_ColorARGB getConstant (void) const = 0;
};

class ATOM_FogAttributes: public ATOM_RenderAttributes
{
public:
	virtual void setDefaults (void) = 0;
	virtual void enableFog (bool enable) = 0;
	virtual bool isFogEnabled (void) const = 0;
	virtual void setFogMode (FogMode mode) = 0;
	virtual FogMode getFogMode (void) const = 0;
	virtual void setFogNear (float val) = 0;
	virtual float getFogNear (void) const = 0;
	virtual void setFogFar (float val) = 0;
	virtual float getFogFar (void) const = 0;
	virtual void setFogDensity (float val) = 0;
	virtual float getFogDensity (void) const = 0;
	virtual void setFogColor (ATOM_ColorARGB color) = 0;
	virtual ATOM_ColorARGB getFogColor (void) const = 0;
};

class ATOM_AlphaTestAttributes: public ATOM_RenderAttributes
{
public:
	virtual void setDefaults (void) = 0;
	virtual void enableAlphaTest (bool enable) = 0;
	virtual bool isAlphaTestEnabled (void) const = 0;
	virtual void setAlphaFunc (CompareFunc func) = 0;
	virtual CompareFunc getAlphaFunc (void) const = 0;
	virtual void setAlphaRef (float val) = 0;
	virtual float getAlphaRef (void) const = 0;
};

class ATOM_RenderStateSet: public ATOM_Object
{
public:
	virtual ATOM_AlphaBlendAttributes *useAlphaBlending (ATOM_AlphaBlendAttributes *state) = 0;
	virtual void unuseAlphaBlending (void) = 0;
	virtual ATOM_ColorWriteAttributes *useColorWrite (ATOM_ColorWriteAttributes *state) = 0;
	virtual void unuseColorWrite (void) = 0;
	virtual ATOM_DepthAttributes *useDepth (ATOM_DepthAttributes *state) = 0;
	virtual void unuseDepth (void) = 0;
	virtual ATOM_StencilAttributes *useStencil (ATOM_StencilAttributes *state) = 0;
	virtual void unuseStencil (void) = 0;
	virtual ATOM_RasterizerAttributes *useRasterizer (ATOM_RasterizerAttributes *state) = 0;
	virtual void unuseRasterizer (void) = 0;
	virtual ATOM_MultisampleAttributes *useMultisample (ATOM_MultisampleAttributes *state) = 0;
	virtual void unuseMultisample (void) = 0;
	virtual ATOM_SamplerAttributes *useSampler (int index, ATOM_SamplerAttributes *state) = 0;
	virtual void unuseSampler (int index) = 0;
	virtual ATOM_TextureStageAttributes *useTextureStage (int index, ATOM_TextureStageAttributes *state) = 0;
	virtual void unuseTextureStage (int index) = 0;
	virtual ATOM_FogAttributes *useFog (ATOM_FogAttributes *state) = 0;
	virtual void unuseFog (void) = 0;
	virtual ATOM_AlphaTestAttributes *useAlphaTest (ATOM_AlphaTestAttributes *state) = 0;
	virtual void unuseAlphaTest (void) = 0;
	virtual bool commit (void) = 0;
	virtual bool apply (int pass, bool force) = 0;
	virtual void replace (ATOM_RenderStateSet *other) = 0;
	virtual void setGfxDriver (ATOM_GfxDriver *driver) = 0;
	virtual ATOM_GfxDriver *getGfxDriver (void) const = 0;
};

#endif // __ATOM3D_GFXDRIVER_RENDERSTATES_H
