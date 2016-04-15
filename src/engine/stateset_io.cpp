#include "StdAfx.h"
#include "stateset_io.h"
#include "parameter.h"
#include "material.h"
#include "paramcallback.h"

////////////////////////////////// utilities /////////////////////////////////


static bool processParameter (const char *name, ATOM_MaterialParam::ParamCallback callback, void *state, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	ATOM_MaterialParam *param = material->getParameter (name);
	if (!param)
	{
		return false;
	}

	if (callback)
	{
		param->addApplyCallback (callback, state, 0, effect, pass);
	}
	param->setFlags (param->getFlags()|ATOM_MaterialParam::Flag_StateValue);

	return true;
}

static ATOM_AUTOREF(ATOM_Texture) parseTexture (ATOM_RenderDevice *device, const char *str, void *state, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect, ATOM_MaterialTextureParam **texParam)
{
	ATOM_AUTOREF(ATOM_Texture) result;
	*texParam = nullptr;

	if (str)
	{
		if (str[0] == '$')
		{
			ATOM_MaterialParam *param = material->getParameter (str+1);
			if (param && param->getParamType()==ATOM_MaterialParam::ParamType_Texture)
			{
				*texParam = (ATOM_MaterialTextureParam*)param;
			}
			//processParameter (str+1, &stateParamCallback_Texture, state, material, pass, effect);
		}
		else
		{
			result = device->createTextureResource (str);
		}
	}
	return result;
}

static ATOM_RenderAttributes::SamplerAddress parseSamplerAddress (const char *str, ATOM_MaterialParam::ParamCallback callback, void *state, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	if (str)
	{
		if (str[0] == '$')
		{
			processParameter (str+1, callback, state, material, pass, effect);
		}
		else
		{
			if (!stricmp (str, "wrap"))
				return ATOM_RenderAttributes::SamplerAddress_Wrap;

			if (!stricmp (str, "clamp"))
				return ATOM_RenderAttributes::SamplerAddress_Clamp;

			if (!stricmp (str, "mirror"))
				return ATOM_RenderAttributes::SamplerAddress_Mirror;

			if (!stricmp (str, "border"))
				return ATOM_RenderAttributes::SamplerAddress_Border;

			if (!stricmp (str, "mirror_once"))
				return ATOM_RenderAttributes::SamplerAddress_MirrorOnce;
		}
	}

	return ATOM_RenderAttributes::SamplerAddress_Unknown;
}

static ATOM_RenderAttributes::SamplerFilter parseSamplerFilter (const char *str, void *state, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	if (str)
	{
		if (str[0] == '$')
		{
			processParameter (str+1, &stateParamCallback_Filter, state, material, pass, effect);
		}
		else
		{
			if (!stricmp (str, "anisotropic"))
				return ATOM_RenderAttributes::SamplerFilter_AnisotropicL;

			if (!stricmp (str, "ppp"))
				return ATOM_RenderAttributes::SamplerFilter_PPP;

			if (!stricmp (str, "ppl"))
				return ATOM_RenderAttributes::SamplerFilter_PPL;

			if (!stricmp (str, "ppn"))
				return ATOM_RenderAttributes::SamplerFilter_PPN;

			if (!stricmp (str, "plp"))
				return ATOM_RenderAttributes::SamplerFilter_PLP;

			if (!stricmp (str, "pll"))
				return ATOM_RenderAttributes::SamplerFilter_PLL;

			if (!stricmp (str, "pln"))
				return ATOM_RenderAttributes::SamplerFilter_PLN;

			if (!stricmp (str, "lpp"))
				return ATOM_RenderAttributes::SamplerFilter_LPP;

			if (!stricmp (str, "lpl"))
				return ATOM_RenderAttributes::SamplerFilter_LPL;

			if (!stricmp (str, "lpn"))
				return ATOM_RenderAttributes::SamplerFilter_LPN;

			if (!stricmp (str, "llp"))
				return ATOM_RenderAttributes::SamplerFilter_LLP;

			if (!stricmp (str, "lll"))
				return ATOM_RenderAttributes::SamplerFilter_LLL;

			if (!stricmp (str, "lln"))
				return ATOM_RenderAttributes::SamplerFilter_LLN;
		}
	}

	return ATOM_RenderAttributes::SamplerFilter_Unknown;
}

static ATOM_RenderAttributes::CullMode parseCullMode (const char *str, void *state, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	if (str)
	{
		if (str[0] == '$')
		{
			processParameter (str+1, &stateParamCallback_CullMode, state, material, pass, effect);
		}
		else
		{
			if (!stricmp (str, "none"))
				return ATOM_RenderAttributes::CullMode_None;

			if (!stricmp (str, "back"))
				return ATOM_RenderAttributes::CullMode_Back;

			if (!stricmp (str, "front"))
				return ATOM_RenderAttributes::CullMode_Front;
		}
	}

	return ATOM_RenderAttributes::CullMode_Unknown;
}

static ATOM_RenderAttributes::FrontFace parseFrontFace (const char *str, void *state, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	if (str)
	{
		if (str[0] == '$')
		{
			processParameter (str+1, &stateParamCallback_FrontFace, state, material, pass, effect);
		}
		else
		{
			if (!stricmp (str, "cw"))
				return ATOM_RenderAttributes::FrontFace_CW;

			if (!stricmp (str, "ccw"))
				return ATOM_RenderAttributes::FrontFace_CCW;
		}
	}

	return ATOM_RenderAttributes::FrontFace_Unknown;
}

static ATOM_RenderAttributes::FillMode parseFillMode (const char *str, void *state, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	if (str)
	{
		if (str[0] == '$')
		{
			processParameter (str+1, &stateParamCallback_FillMode, state, material, pass, effect);
		}
		else
		{
			if (!stricmp (str, "solid"))
				return ATOM_RenderAttributes::FillMode_Solid;

			if (!stricmp (str, "line"))
				return ATOM_RenderAttributes::FillMode_Line;
		}
	}

	return ATOM_RenderAttributes::FillMode_Unknown;
}

static unsigned parseUInt (const char *str, ATOM_MaterialParam::ParamCallback callback, void *state, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	unsigned value = 0;

	if (str)
	{
		if (str[0] == '$')
		{
			processParameter (str+1, callback, state, material, pass, effect);
		}
		else
		{
			if (1 == sscanf (str, "%u", &value))
			{
				return value;
			}
			if (1 == sscanf (str, "0x%x", &value))
			{
				return value;
			}
		}
	}
	return value;
}

//=== WANGJIAN MODIFIED ===//
// 2013/5/30
// 为什么是BOOL型？
static /*bool*/float parseFloat (const char *str, ATOM_MaterialParam::ParamCallback callback, void *state, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	float value = 0.f;

	if (str)
	{
		if (str[0] == '$')
		{
			processParameter (str+1, callback, state, material, pass, effect);
		}
		else
		{
			sscanf (str, "%f", &value);
		}
	}
	return value;
}

static bool parseBool (const char *str, ATOM_MaterialParam::ParamCallback callback, void *state, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	if (str)
	{
		if (str[0] == '$')
		{
			processParameter (str+1, callback, state, material, pass, effect);
		}
		else
		{
			if (!stricmp (str, "true"))
			{
				return true;
			}
			else if (!stricmp (str, "false"))
			{
				return false;
			}
		}
	}
	return false;
}

static bool parseInt4 (const char *str, int *value, ATOM_MaterialParam::ParamCallback callback, void *state, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	value[0] = 0;
	value[1] = 0;
	value[2] = 0;
	value[3] = 0;

	if (str)
	{
		if (str[0] == '$')
		{
			return processParameter (str+1, callback, state, material, pass, effect);
		}
		if (4 == sscanf (str, "%d,%d,%d,%d", &value[0], &value[1], &value[2], &value[3]))
		{
			return true;
		}
	}

	return false;
}

static bool parseVector4 (const char *str, ATOM_Vector4f &v, ATOM_MaterialParam::ParamCallback callback, void *state, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	v.set (0.f, 0.f, 0.f, 0.f);
	
	if (str)
	{
		if (str[0] == '$')
		{
			return processParameter (str+1, callback, state, material, pass, effect);
		}

		if (4 == sscanf (str, "%f,%f,%f,%f", &v.x, &v.y, &v.z, &v.w))
		{
			return true;
		}
	}

	return false;
}

static bool parseMatrix44 (const char *str, ATOM_Matrix4x4f &m, ATOM_MaterialParam::ParamCallback callback, void *state, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	m.makeIdentity ();

	if (str)
	{
		if (str[0] == '$')
		{
			return processParameter (str+1, callback, state, material, pass, effect);
		}

		if (16 == sscanf (str, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", 
			&m.m00, &m.m01, &m.m02, &m.m03, 
			&m.m10, &m.m11, &m.m12, &m.m13,
			&m.m20, &m.m21, &m.m22, &m.m23,
			&m.m30, &m.m31, &m.m32, &m.m33))
		{
			return true;
		}
	}

	return false;
}

static ATOM_RenderAttributes::CompareFunc parseCompareFunc (const char *str, ATOM_MaterialParam::ParamCallback callback, void *state, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	if (str)
	{
		if (str[0] == '$')
		{
			processParameter (str+1, callback, state, material, pass, effect);
		}
		else
		{
			if (!stricmp (str, "never"))
				return ATOM_RenderAttributes::CompareFunc_Never;

			if (!stricmp (str, "always"))
				return ATOM_RenderAttributes::CompareFunc_Always;

			if (!stricmp (str, "less"))
				return ATOM_RenderAttributes::CompareFunc_Less;

			if (!stricmp (str, "lessequal"))
				return ATOM_RenderAttributes::CompareFunc_LessEqual;

			if (!stricmp (str, "greater"))
				return ATOM_RenderAttributes::CompareFunc_Greater;

			if (!stricmp (str, "greaterequal"))
				return ATOM_RenderAttributes::CompareFunc_GreaterEqual;

			if (!stricmp (str, "equal"))
				return ATOM_RenderAttributes::CompareFunc_Equal;

			if (!stricmp (str, "notequal"))
				return ATOM_RenderAttributes::CompareFunc_NotEqual;
		}
	}

	return ATOM_RenderAttributes::CompareFunc_Unknown;
}

static ATOM_RenderAttributes::StencilOp parseStencilOp (const char *str, ATOM_MaterialParam::ParamCallback callback, void *state, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	if (str)
	{
		if (str[0] == '$')
		{
			processParameter (str+1, callback, state, material, pass, effect);
		}
		else
		{
			if (!stricmp (str, "keep"))
				return ATOM_RenderAttributes::StencilOp_Keep;

			if (!stricmp (str, "replace"))
				return ATOM_RenderAttributes::StencilOp_Replace;

			if (!stricmp (str, "zero"))
				return ATOM_RenderAttributes::StencilOp_Zero;

			if (!stricmp (str, "inc"))
				return ATOM_RenderAttributes::StencilOp_Inc;

			if (!stricmp (str, "dec"))
				return ATOM_RenderAttributes::StencilOp_Dec;

			if (!stricmp (str, "invert"))
				return ATOM_RenderAttributes::StencilOp_Invert;

			if (!stricmp (str, "inc_wrap"))
				return ATOM_RenderAttributes::StencilOp_IncWrap;

			if (!stricmp (str, "dec_wrap"))
				return ATOM_RenderAttributes::StencilOp_DecWrap;
		}
	}

	return ATOM_RenderAttributes::StencilOp_Unknown;
}

static ATOM_RenderAttributes::BlendFunc parseBlendFunc (const char *str, ATOM_MaterialParam::ParamCallback callback, void *state, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	if (str)
	{
		if (str[0] == '$')
		{
			processParameter (str+1, callback, state, material, pass, effect);
		}
		else
		{
			if (!stricmp (str, "zero"))
				return ATOM_RenderAttributes::BlendFunc_Zero;

			if (!stricmp (str, "one"))
				return ATOM_RenderAttributes::BlendFunc_One;

			if (!stricmp (str, "srccolor"))
				return ATOM_RenderAttributes::BlendFunc_SrcColor;

			if (!stricmp (str, "invsrccolor"))
				return ATOM_RenderAttributes::BlendFunc_InvSrcColor;

			if (!stricmp (str, "srcalpha"))
				return ATOM_RenderAttributes::BlendFunc_SrcAlpha;

			if (!stricmp (str, "invsrcalpha"))
				return ATOM_RenderAttributes::BlendFunc_InvSrcAlpha;

			if (!stricmp (str, "destalpha"))
				return ATOM_RenderAttributes::BlendFunc_DestAlpha;

			if (!stricmp (str, "invdestalpha"))
				return ATOM_RenderAttributes::BlendFunc_InvDestAlpha;

			if (!stricmp (str, "destcolor"))
				return ATOM_RenderAttributes::BlendFunc_DestColor;

			if (!stricmp (str, "invdestcolor"))
				return ATOM_RenderAttributes::BlendFunc_InvDestColor;

			if (!stricmp (str, "constant"))
				return ATOM_RenderAttributes::BlendFunc_Constant;

			if (!stricmp (str, "invconstant"))
				return ATOM_RenderAttributes::BlendFunc_InvConstant;
		}
	}

	return ATOM_RenderAttributes::BlendFunc_Unknown;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

bool loadAlphaBlendStates (ATOM_RenderDevice *device, ATOM_AlphaBlendAttributes *states, ATOM_TiXmlElement *xmlElement, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	ATOM_TiXmlElement *enableElement = xmlElement->FirstChildElement ("enable");
	const char *enable = enableElement ? enableElement->Attribute ("value") : 0;
	bool b = enable ? parseBool (enable, &stateParamCallback_AlphaBlendEnable, states, material, pass, effect) : false;
	states->enableAlphaBlending (b);

	ATOM_TiXmlElement *srcblendElement = xmlElement->FirstChildElement ("srcblend");
	const char *srcblend = srcblendElement ? srcblendElement->Attribute ("value") : 0;
	states->setSrcBlend (parseBlendFunc (srcblend, &stateParamCallback_SrcBlend, states, material, pass, effect));

	ATOM_TiXmlElement *destblendElement = xmlElement->FirstChildElement ("destblend");
	const char *destblend = destblendElement ? destblendElement->Attribute ("value") : 0;
	states->setDestBlend (parseBlendFunc (destblend, &stateParamCallback_DestBlend, states, material, pass, effect));

	ATOM_TiXmlElement *blendconstElement = xmlElement->FirstChildElement ("blendcolor");
	const char *blendcolor = blendconstElement ? blendconstElement->Attribute ("value") : 0;
	ATOM_Vector4f colorValue(1.f);
	if (blendcolor && !parseVector4 (blendcolor, colorValue, &stateParamCallback_BlendColor, states, material, pass, effect))
	{
		return false;
	}
	states->setBlendConstant (ATOM_ColorARGB(colorValue.x, colorValue.y, colorValue.z, colorValue.w));

	return true;
}

bool loadColorWriteStates (ATOM_RenderDevice *device, ATOM_ColorWriteAttributes *states, ATOM_TiXmlElement *xmlElement, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	ATOM_TiXmlElement *redElement = xmlElement->FirstChildElement ("red");
	if (!redElement)
		return false;

	const char *value = redElement->Attribute ("value");
	bool red = parseBool (value, &stateParamCallback_ColorWriteRed, states, material, pass, effect);

	ATOM_TiXmlElement *greenElement = xmlElement->FirstChildElement ("green");
	if (!greenElement)
		return false;

	value = greenElement->Attribute ("value");
	bool green = parseBool (value, &stateParamCallback_ColorWriteGreen, states, material, pass, effect);

	ATOM_TiXmlElement *blueElement = xmlElement->FirstChildElement ("blue");
	if (!blueElement)
		return false;

	value = blueElement->Attribute ("value");
	bool blue = parseBool (value, &stateParamCallback_ColorWriteBlue, states, material, pass, effect);

	ATOM_TiXmlElement *alphaElement = xmlElement->FirstChildElement ("alpha");
	if (!alphaElement)
		return false;

	value = alphaElement->Attribute ("value");
	bool alpha = parseBool (value, &stateParamCallback_ColorWriteAlpha, states, material, pass, effect);

	unsigned mask = 0;
	if (red) mask |= ATOM_RenderAttributes::ColorMask_Red;
	if (green) mask |= ATOM_RenderAttributes::ColorMask_Green;
	if (blue) mask |= ATOM_RenderAttributes::ColorMask_Blue;
	if (alpha) mask |= ATOM_RenderAttributes::ColorMask_Alpha;

	states->setColorWriteMask (mask);

	return true;
}

bool loadDepthStates (ATOM_RenderDevice *device, ATOM_DepthAttributes *states, ATOM_TiXmlElement *xmlElement, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	ATOM_TiXmlElement *zfuncElement = xmlElement->FirstChildElement ("zfunc");
	const char *zfunc = zfuncElement ? zfuncElement->Attribute ("value") : 0;
	states->setDepthFunc (parseCompareFunc (zfunc, &stateParamCallback_DepthFunc, states, material, pass, effect));

	ATOM_TiXmlElement *zwriteElement = xmlElement->FirstChildElement ("zwrite");
	const char *zwrite = zwriteElement ? zwriteElement->Attribute ("value") : 0;
	states->enableDepthWrite (zwrite ? parseBool (zwrite, &stateParamCallback_DepthWriteEnable, states, material, pass, effect) : true);

	return true;
}

bool loadStencilStates (ATOM_RenderDevice *device, ATOM_StencilAttributes *states, ATOM_TiXmlElement *xmlElement, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	ATOM_TiXmlElement *enableElement = xmlElement->FirstChildElement ("enable");
	const char *enable = enableElement ? enableElement->Attribute ("value") : 0;
	bool b = enable ? parseBool (enable, &stateParamCallback_StencilEnable, states, material, pass, effect) : false;
	states->enableStencil (b);

	ATOM_TiXmlElement *twosidedElement = xmlElement->FirstChildElement ("twosided");
	const char *twosided = twosidedElement ? twosidedElement->Attribute ("value") : 0;
	bool twosidedStencil = twosided ? parseBool (twosided, &stateParamCallback_StencilTwoSideEnable, states, material, pass, effect) : false;
	states->enableTwoSidedStencil (twosidedStencil);

	ATOM_TiXmlElement *readMaskElement = xmlElement->FirstChildElement ("readmask");
	const char *readmask = readMaskElement ? readMaskElement->Attribute ("value") : 0;
	states->setStencilReadMask (readmask ? parseUInt (readmask, &stateParamCallback_StencilReadMask, states, material, pass, effect) : 0xFFFFFFFF);

	ATOM_TiXmlElement *writeMaskElement = xmlElement->FirstChildElement ("writemask");
	const char *writemask = writeMaskElement ? writeMaskElement->Attribute ("value") : 0;
	states->setStencilWriteMask (writemask ? parseUInt (writemask, &stateParamCallback_StencilWriteMask, states, material, pass, effect) : 0xFFFFFFFF);

	ATOM_TiXmlElement *refElement = xmlElement->FirstChildElement ("ref");
	const char *ref = refElement ? refElement->Attribute ("value") : 0;
	states->setStencilRef (ref ? parseUInt (ref, &stateParamCallback_StencilRef, states, material, pass, effect) : 0);

	ATOM_TiXmlElement *failopFrontElement = xmlElement->FirstChildElement ("failop_front");
	const char *failFront = failopFrontElement ? failopFrontElement->Attribute ("value") : 0;
	states->setStencilFailOpFront (parseStencilOp (failFront, &stateParamCallback_StencilFailOpFront, states, material, pass, effect));

	ATOM_TiXmlElement *zfailopFrontElement = xmlElement->FirstChildElement ("zfailop_front");
	const char *zfailFront = zfailopFrontElement ? zfailopFrontElement->Attribute ("value") : 0;
	states->setStencilDepthFailOpFront (parseStencilOp (zfailFront, &stateParamCallback_StencilZFailOpFront, states, material, pass, effect));

	ATOM_TiXmlElement *passopFrontElement = xmlElement->FirstChildElement ("passop_front");
	const char *passFront = passopFrontElement ? passopFrontElement->Attribute ("value") : 0;
	states->setStencilPassOpFront (parseStencilOp (passFront, &stateParamCallback_StencilPassOpFront, states, material, pass, effect));

	ATOM_TiXmlElement *funcFrontElement = xmlElement->FirstChildElement ("func_front");
	const char *funcFront = funcFrontElement ? funcFrontElement->Attribute ("value") : 0;
	states->setStencilFuncFront (parseCompareFunc (funcFront, &stateParamCallback_StencilFuncFront, states, material, pass, effect));

	ATOM_TiXmlElement *failopBackElement = xmlElement->FirstChildElement ("failop_back");
	const char *failBack = failopBackElement ? failopBackElement->Attribute ("value") : 0;
	states->setStencilFailOpBack (parseStencilOp (failBack, &stateParamCallback_StencilFailOpBack, states, material, pass, effect));

	ATOM_TiXmlElement *zfailopBackElement = xmlElement->FirstChildElement ("zfailop_back");
	const char *zfailBack = zfailopBackElement ? zfailopBackElement->Attribute ("value") : 0;
	states->setStencilDepthFailOpBack (parseStencilOp (zfailBack, &stateParamCallback_StencilZFailOpBack, states, material, pass, effect));

	ATOM_TiXmlElement *passopBackElement = xmlElement->FirstChildElement ("passop_back");
	const char *passBack = passopBackElement ? passopBackElement->Attribute ("value") : 0;
	states->setStencilPassOpBack (parseStencilOp (passBack, &stateParamCallback_StencilPassOpBack, states, material, pass, effect));

	ATOM_TiXmlElement *funcBackElement = xmlElement->FirstChildElement ("func_back");
	const char *funcBack = funcBackElement ? funcBackElement->Attribute ("value") : 0;
	states->setStencilFuncBack (parseCompareFunc (funcBack, &stateParamCallback_StencilFuncBack, states, material, pass, effect));

	return true;
}

bool loadRasterizerStates (ATOM_RenderDevice *device, ATOM_RasterizerAttributes *states, ATOM_TiXmlElement *xmlElement, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	ATOM_TiXmlElement *cullModeElement = xmlElement->FirstChildElement ("cullmode");
	const char *cullmode = cullModeElement ? cullModeElement->Attribute("value") : 0;
	states->setCullMode (parseCullMode (cullmode, states, material, pass, effect));

	ATOM_TiXmlElement *frontFaceElement = xmlElement->FirstChildElement ("frontface");
	const char *frontface = frontFaceElement ? frontFaceElement->Attribute("value") : 0;
	states->setFrontFace (parseFrontFace (frontface, states, material, pass, effect));

	ATOM_TiXmlElement *fillModeElement = xmlElement->FirstChildElement ("fillmode");
	const char *fillmode = fillModeElement ? fillModeElement->Attribute ("value") : 0;
	states->setFillMode (parseFillMode (fillmode, states, material, pass, effect));

	return true;
}

bool loadMultisampleStates (ATOM_RenderDevice *device, ATOM_MultisampleAttributes *states, ATOM_TiXmlElement *xmlElement, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	ATOM_TiXmlElement *enableElement = xmlElement->FirstChildElement ("enable");
	const char *enable = enableElement ? enableElement->Attribute ("value") : 0;
	bool b = enable ? parseBool (enable, &stateParamCallback_MultisampleEnable, states, material, pass, effect) : false;
	states->enableMultisample (b);

	ATOM_TiXmlElement *atocElement = xmlElement->FirstChildElement ("atoc");
	const char *atoc = atocElement ? atocElement->Attribute ("value") : 0;
	states->enableAlphaToCoverage (atoc ? parseBool (atoc, &stateParamCallback_AlphaToCoverageEnable, states, material, pass, effect) : false);

	return true;
}

bool loadSamplerStates (ATOM_RenderDevice *device, ATOM_SamplerAttributes *states, ATOM_TiXmlElement *xmlElement, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect, ATOM_MaterialTextureParam **texParam)
{
	ATOM_TiXmlElement *filterElement = xmlElement->FirstChildElement ("filter");
	const char *filter = filterElement ? filterElement->Attribute ("value") : 0;
	states->setFilter (parseSamplerFilter (filter, states, material, pass, effect));

	ATOM_TiXmlElement *addressuElement = xmlElement->FirstChildElement ("addressu");
	const char *addressu = addressuElement ? addressuElement->Attribute ("value") : 0;
	states->setAddressU (parseSamplerAddress (addressu, &stateParamCallback_AddressU, states, material, pass, effect));

	ATOM_TiXmlElement *addressvElement = xmlElement->FirstChildElement ("addressv");
	const char *addressv = addressvElement ? addressvElement->Attribute ("value") : 0;
	states->setAddressV (parseSamplerAddress (addressv, &stateParamCallback_AddressV, states, material, pass, effect));

	ATOM_TiXmlElement *addresswElement = xmlElement->FirstChildElement ("addressw");
	const char *addressw = addresswElement ? addresswElement->Attribute ("value") : 0;
	states->setAddressW (parseSamplerAddress (addressw, &stateParamCallback_AddressW, states, material, pass, effect));

	ATOM_TiXmlElement *lodbiasElement = xmlElement->FirstChildElement ("lodbias");
	float lodbias = lodbiasElement ? parseFloat (lodbiasElement->Attribute("value"), &stateParamCallback_MipmapLODBias, states, material, pass, effect) : 0.f;
	states->setMipmapLodBias (lodbias);

	ATOM_TiXmlElement *maxlodElement = xmlElement->FirstChildElement ("maxlod");
	int maxlod = maxlodElement ? parseUInt (maxlodElement->Attribute("value"), &stateParamCallback_MaxLOD, states, material, pass, effect) : 0.f;
	states->setMaxLOD (maxlod);

	ATOM_TiXmlElement *maxAnisotropyElement = xmlElement->FirstChildElement ("max_anisotropy");
	int maxAnisotropy = maxAnisotropyElement ? parseUInt (maxAnisotropyElement->Attribute("value"), &stateParamCallback_MaxAnisotropic, states, material, pass, effect) : 1;
	states->setMaxAnisotropic (maxAnisotropy);

	ATOM_TiXmlElement *comparefuncElement = xmlElement->FirstChildElement ("compare");
	const char *compare = comparefuncElement ? comparefuncElement->Attribute ("value") : 0;
	states->setCompareFunc (parseCompareFunc (compare, &stateParamCallback_CompareFunc, states, material, pass, effect));

	ATOM_TiXmlElement *borderColorElement = xmlElement->FirstChildElement ("bordercolor");
	const char *bordercolor = borderColorElement ? borderColorElement->Attribute ("value") : 0;
	ATOM_Vector4f color(0.f);
	if (bordercolor) parseVector4 (bordercolor, color, &stateParamCallback_BorderColor, states, material, pass, effect);
	states->setBorderColor (ATOM_ColorARGB (color.x, color.y, color.z, color.w));

	ATOM_TiXmlElement *srgbElement = xmlElement->FirstChildElement ("srgb");
	const char *srgbEnable = srgbElement ? srgbElement->Attribute ("value") : 0;
	bool srgb = parseBool (srgbEnable, &stateParamCallback_SRGBTexture, states, material, pass, effect);
	states->enableSRGBTexture (srgb);

	ATOM_TiXmlElement *textureElement = xmlElement->FirstChildElement ("texture");
	ATOM_AUTOREF(ATOM_Texture) tex;
	if (!textureElement) 
	{
		*texParam = nullptr;
	}
	else
	{
		const char *texture = textureElement->Attribute("value");
		tex = parseTexture (device, texture, states, material, pass, effect, texParam);
	}
	states->setTexture (tex.get());

	return true;
}

bool loadAlphaTestStates (ATOM_RenderDevice *device, ATOM_AlphaTestAttributes *states, ATOM_TiXmlElement *xmlElement, ATOM_CoreMaterial *material, ATOM_MaterialPass *pass, ATOM_MaterialEffect *effect)
{
	ATOM_TiXmlElement *enableElement = xmlElement->FirstChildElement ("enable");
	const char *enable = enableElement ? enableElement->Attribute ("value") : 0;
	bool b = enable ? parseBool (enable, &stateParamCallback_AlphaTestEnable, states, material, pass, effect) : false;
	states->enableAlphaTest (b);

	ATOM_TiXmlElement *alphafuncElement = xmlElement->FirstChildElement ("alphafunc");
	const char *alphafunc = alphafuncElement ? alphafuncElement->Attribute ("value") : 0;
	states->setAlphaFunc (parseCompareFunc (alphafunc, &stateParamCallback_AlphaFunc, states, material, pass, effect));

	ATOM_TiXmlElement *alpharefElement = xmlElement->FirstChildElement ("alpharef");
	float alpharef = alpharefElement ? parseFloat(alpharefElement->Attribute("value"), &stateParamCallback_AlphaRef, states, material, pass, effect) : 0.f;
	states->setAlphaRef (alpharef);

	return true;
}

