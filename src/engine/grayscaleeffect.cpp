#include "StdAfx.h"
#include "grayscaleeffect.h"

static const char *grayscaleMaterialSrc =
	"material																				\n"
	"{																						\n"
	"  effect gs {																		\n"
	"	source \"																			\n"
	"	  float4x4 MVP;																		\n"
	"	  texture inputTexture;																\n"
	"	  sampler2D inputSampler = sampler_state											\n"
	"	  {																					\n"
	"	  	Texture = (inputTexture);														\n"
	"		MIPFILTER = NONE;																\n"
	"		MAGFILTER = POINT;																\n"
	"		MINFILTER = POINT;																\n"
	"	  };																				\n"
	"	  struct VS_OUTPUT																	\n"
	"	  {																					\n"
	"		float4 Position : POSITION;														\n"
	"		float2 Tex0 : TEXCOORD0;														\n"
	"	  };																				\n"
	"	  struct PS_OUTPUT																	\n"
	"	  {																					\n"
	"		float4 color : COLOR0;															\n"
	"	  };																				\n"
	"	  PS_OUTPUT PS (in VS_OUTPUT IN)													\n"
	"	  {																					\n"
	"		PS_OUTPUT OUT;																	\n"
	"		float4 c = tex2D(inputSampler, IN.Tex0);										\n"
	"		float g = dot(c.rgb, float3(0.3, 0.59, 0.11));									\n"
	"		OUT.color = float4(g, g, g, c.a);												\n"
	"		return OUT;																		\n"
	"	  }																					\n"
	"	  technique t0																		\n"
	"	  {																					\n"
	"		pass P0																			\n"
	"		{																				\n"
	"		  ZEnable = False;																\n"
	"		  CullMode = None;																\n"
	"		  VertexShader = null;															\n"
	"		  PixelShader = compile ps_2_0 PS();											\n"	
	"		}																				\n"
	"	  }\";																				\n"
	"	param MVP matrix44f MVPMATRIX;														\n"
	"  }																					\n"
	"}																						\n";

ATOM_GrayScaleEffect::ATOM_GrayScaleEffect (ATOM_PixelFormat pixelFormat): _pixelFormat(pixelFormat)
{
	ATOM_STACK_TRACE(ATOM_GrayScaleEffect::ATOM_GrayScaleEffect);
}

bool ATOM_GrayScaleEffect::doRender (ATOM_RenderDevice *device, ATOM_Texture *inputColor)
{
	ATOM_STACK_TRACE(ATOM_GrayScaleEffect::doRender);

	if (!_material)
	{
		_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/grayscaleeffect.mat");

		if (!_material)
		{
			return false;
		}
	}

	_material->getParameterTable()->setTexture ("inputTexture", inputColor);

	unsigned numPasses = _material->begin (device);
	for (unsigned i = 0; i < numPasses; ++i)
	{
		if (_material->beginPass (device, i))
		{
			drawTexturedFullscreenQuad (device, inputColor->getWidth(), inputColor->getHeight());
			_material->endPass (device, i);
		}
	}
	_material->end (device);

	return true;
}

ATOM_AUTOREF(ATOM_Texture) ATOM_GrayScaleEffect::doAllocOutputColorTexture (ATOM_RenderDevice *device, ATOM_Texture *inputColor)
{
	ATOM_STACK_TRACE(ATOM_GrayScaleEffect::doAllocOutputColorTexture);

	return device->allocTexture (0, 0, inputColor->getWidth(), inputColor->getHeight(), _pixelFormat, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
}



