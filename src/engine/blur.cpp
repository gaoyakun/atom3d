#include "StdAfx.h"
#include "blur.h"

static const char *hBlur3x3 =
	"material																				\n"
	"{																						\n"
	"  effect ds {																			\n"
	"	source \"																			\n"
	"	  float offsets[3];																	\n"
	"	  float weights[3];																	\n"
	"	  texture inputTexture;																\n"
	"	  sampler2D inputSampler = sampler_state											\n"
	"	  {																					\n"
	"	  	Texture = (inputTexture);														\n"
	"		MIPFILTER = NONE;																\n"
	"		MAGFILTER = LINEAR;																\n"
	"		MINFILTER = LINEAR;																\n"
	"	  };																				\n"
	"	  float4 PS (in float2 t : TEXCOORD0): COLOR0										\n"
	"	  {																					\n"
	"		float4 color = { 0.0f, 0.0f, 0.0f, 0.0f };										\n"
	"		for (int i = 0; i < 3; i++)														\n"
	"		{																				\n"
	"			color += (tex2D(inputSampler, t + float2(offsets[i], 0.0f)) * weights[i]);	\n"
	"		}																				\n"
	"		return float4(color.rgb, 1.0f);													\n"
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
	"  }																					\n"
	"}																						\n";

static const char *vBlur3x3 =
	"material																				\n"
	"{																						\n"
	"  effect ds {																			\n"
	"	source \"																			\n"
	"	  float offsets[3];																	\n"
	"	  float weights[3];																	\n"
	"	  texture inputTexture;																\n"
	"	  sampler2D inputSampler = sampler_state											\n"
	"	  {																					\n"
	"	  	Texture = (inputTexture);														\n"
	"		MIPFILTER = NONE;																\n"
	"		MAGFILTER = LINEAR;																\n"
	"		MINFILTER = LINEAR;																\n"
	"	  };																				\n"
	"	  float4 PS (in float2 t : TEXCOORD0): COLOR0										\n"
	"	  {																					\n"
	"		float4 color = { 0.0f, 0.0f, 0.0f, 0.0f };										\n"
	"		for (int i = 0; i < 3; i++)														\n"
	"		{																				\n"
	"			color += (tex2D(inputSampler, t + float2(0.0f, offsets[i])) * weights[i]);	\n"
	"		}																				\n"
	"		return float4(color.rgb, 1.0f);													\n"
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
	"  }																					\n"
	"}																						\n";

static const char *hBlur5x5 =
	"material																				\n"
	"{																						\n"
	"  effect ds {																			\n"
	"	source \"																			\n"
	"	  float offsets[5];																	\n"
	"	  float weights[5];																	\n"
	"	  texture inputTexture;																\n"
	"	  sampler2D inputSampler = sampler_state											\n"
	"	  {																					\n"
	"	  	Texture = (inputTexture);														\n"
	"		MIPFILTER = NONE;																\n"
	"		MAGFILTER = LINEAR;																\n"
	"		MINFILTER = LINEAR;																\n"
	"	  };																				\n"
	"	  float4 PS (in float2 t : TEXCOORD0): COLOR0										\n"
	"	  {																					\n"
	"		float4 color = { 0.0f, 0.0f, 0.0f, 0.0f };										\n"
	"		for (int i = 0; i < 5; i++)														\n"
	"		{																				\n"
	"			color += (tex2D(inputSampler, t + float2(offsets[i], 0.0f)) * weights[i]);	\n"
	"		}																				\n"
	"		return float4(color.rgb, 1.0f);													\n"
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
	"  }																					\n"
	"}																						\n";

static const char *vBlur5x5 =
	"material																				\n"
	"{																						\n"
	"  effect ds {																			\n"
	"	source \"																			\n"
	"	  float offsets[5];																	\n"
	"	  float weights[5];																	\n"
	"	  texture inputTexture;																\n"
	"	  sampler2D inputSampler = sampler_state											\n"
	"	  {																					\n"
	"	  	Texture = (inputTexture);														\n"
	"		MIPFILTER = NONE;																\n"
	"		MAGFILTER = LINEAR;																\n"
	"		MINFILTER = LINEAR;																\n"
	"	  };																				\n"
	"	  float4 PS (in float2 t : TEXCOORD0): COLOR0										\n"
	"	  {																					\n"
	"		float4 color = { 0.0f, 0.0f, 0.0f, 0.0f };										\n"
	"		for (int i = 0; i < 5; i++)														\n"
	"		{																				\n"
	"			color += (tex2D(inputSampler, t + float2(0.0f, offsets[i])) * weights[i]);	\n"
	"		}																				\n"
	"		return float4(color.rgb, 1.0f);													\n"
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
	"  }																					\n"
	"}																						\n";

static const char *hBlur7x7 =
	"material																				\n"
	"{																						\n"
	"  effect ds {																			\n"
	"	source \"																			\n"
	"	  float offsets[7];																	\n"
	"	  float weights[7];																	\n"
	"	  texture inputTexture;																\n"
	"	  sampler2D inputSampler = sampler_state											\n"
	"	  {																					\n"
	"	  	Texture = (inputTexture);														\n"
	"		MIPFILTER = NONE;																\n"
	"		MAGFILTER = LINEAR;																\n"
	"		MINFILTER = LINEAR;																\n"
	"	  };																				\n"
	"	  float4 PS (in float2 t : TEXCOORD0): COLOR0										\n"
	"	  {																					\n"
	"		float4 color = { 0.0f, 0.0f, 0.0f, 0.0f };										\n"
	"		for (int i = 0; i < 7; i++)														\n"
	"		{																				\n"
	"			color += (tex2D(inputSampler, t + float2(offsets[i], 0.0f)) * weights[i]);	\n"
	"		}																				\n"
	"		return float4(color.rgb, 1.0f);													\n"
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
	"  }																					\n"
	"}																						\n";

static const char *vBlur7x7 =
	"material																				\n"
	"{																						\n"
	"  effect ds {																			\n"
	"	source \"																			\n"
	"	  float offsets[7];																	\n"
	"	  float weights[7];																	\n"
	"	  texture inputTexture;																\n"
	"	  sampler2D inputSampler = sampler_state											\n"
	"	  {																					\n"
	"	  	Texture = (inputTexture);														\n"
	"		MIPFILTER = NONE;																\n"
	"		MAGFILTER = LINEAR;																\n"
	"		MINFILTER = LINEAR;																\n"
	"	  };																				\n"
	"	  float4 PS (in float2 t : TEXCOORD0): COLOR0										\n"
	"	  {																					\n"
	"		float4 color = { 0.0f, 0.0f, 0.0f, 0.0f };										\n"
	"		for (int i = 0; i < 7; i++)														\n"
	"		{																				\n"
	"			color += (tex2D(inputSampler, t + float2(0.0f, offsets[i])) * weights[i]);	\n"
	"		}																				\n"
	"		return float4(color.rgb, 1.0f);													\n"
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
	"  }																					\n"
	"}																						\n";

ATOM_AUTOPTR(ATOM_Material) ATOM_Blur::_hBlurMaterial3x3;
ATOM_AUTOPTR(ATOM_Material) ATOM_Blur::_vBlurMaterial3x3;
ATOM_AUTOPTR(ATOM_Material) ATOM_Blur::_hBlurMaterial5x5;
ATOM_AUTOPTR(ATOM_Material) ATOM_Blur::_vBlurMaterial5x5;
ATOM_AUTOPTR(ATOM_Material) ATOM_Blur::_hBlurMaterial7x7;
ATOM_AUTOPTR(ATOM_Material) ATOM_Blur::_vBlurMaterial7x7;
bool ATOM_Blur::blurMaterialsInitialized = false;

void ATOM_Blur::initializeBlurMaterials (void)
{
	ATOM_STACK_TRACE(ATOM_Blur::initializeBlurMaterials);

	blurMaterialsInitialized = true;

	if (!_hBlurMaterial3x3)
	{
		_hBlurMaterial3x3 = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/hblur3x3.mat");
		if (!_hBlurMaterial3x3)
		{
			_hBlurMaterial3x3 = 0;
		}
	}

	if (!_vBlurMaterial3x3)
	{
		_vBlurMaterial3x3 = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/vblur3x3.mat");
		if (!_vBlurMaterial3x3)
		{
			_vBlurMaterial3x3 = 0;
		}
	}

	if (!_hBlurMaterial5x5)
	{
		_hBlurMaterial5x5 = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/hblur5x5.mat");
		if (!_hBlurMaterial5x5)
		{
			_hBlurMaterial5x5 = 0;
		}
	}

	if (!_vBlurMaterial5x5)
	{
		_vBlurMaterial5x5 = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/vblur5x5.mat");
		if (!_vBlurMaterial5x5)
		{
			_vBlurMaterial5x5 = 0;
		}
	}

	if (!_hBlurMaterial7x7)
	{
		_hBlurMaterial7x7 = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/hblur7x7.mat");
		if (!_hBlurMaterial7x7)
		{
			_hBlurMaterial7x7 = 0;
		}
	}

	if (!_vBlurMaterial7x7)
	{
		_vBlurMaterial7x7 = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/vblur7x7.mat");
		if (!_vBlurMaterial7x7)
		{
			_vBlurMaterial7x7 = 0;
		}
	}
}

void ATOM_Blur::drawScreenQuad (ATOM_RenderDevice *device, unsigned w, unsigned h)
{
	ATOM_STACK_TRACE(ATOM_Blur::drawScreenQuad);

	device->pushMatrix (ATOM_MATRIXMODE_VIEW);
	device->pushMatrix (ATOM_MATRIXMODE_PROJECTION);
	device->loadIdentity (ATOM_MATRIXMODE_WORLD);
	device->loadIdentity (ATOM_MATRIXMODE_VIEW);
	device->loadIdentity (ATOM_MATRIXMODE_PROJECTION);

	float deltax = (float)0.5f/(float)w;
	float deltay = (float)0.5f/(float)h;

	float vertices[4 * 5] = {
		-1.f - deltax, -1.f + deltay, 1.f, 0.f, 1.f,
		 1.f - deltax, -1.f + deltay, 1.f, 1.f, 1.f,
		 1.f - deltax,  1.f + deltay, 1.f, 1.f, 0.f,
	    -1.f - deltax,  1.f + deltay, 1.f, 0.f, 0.f
	};

	unsigned short indices[4] = {
		0, 1, 2, 3
	};

	device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_FAN, 4, 4, ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2, 5 * sizeof(float), vertices, indices);

	device->popMatrix (ATOM_MATRIXMODE_VIEW);
	device->popMatrix (ATOM_MATRIXMODE_PROJECTION);
}

bool ATOM_Blur::drawVBlur3x3 (ATOM_RenderDevice *device, ATOM_Texture *sourceTexture, float blurScale)
{
	ATOM_STACK_TRACE(ATOM_Blur::drawVBlur3x3);

	if (!blurMaterialsInitialized)
	{
		initializeBlurMaterials ();
	}

	if (!_vBlurMaterial3x3)
	{
		return false;
	}

	float w = 1.f / sourceTexture->getHeight ();
	float weights[3] = { 0.25f, 0.5f, 0.25f };
	float offsets[3] = { -w * blurScale, 0.f, w * blurScale };

	_vBlurMaterial3x3->getParameterTable()->setVector ("offsets", ATOM_Vector4f(offsets[0], offsets[1], offsets[2], 0.f));
	_vBlurMaterial3x3->getParameterTable()->setVector ("weights", ATOM_Vector4f(weights[0], weights[1], weights[2], 0.f));
	_vBlurMaterial3x3->getParameterTable()->setTexture ("inputTexture", sourceTexture);

	unsigned numPasses = _vBlurMaterial3x3->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (_vBlurMaterial3x3->beginPass (device, pass))
		{
			drawScreenQuad (device, sourceTexture->getWidth(), sourceTexture->getHeight());
			_vBlurMaterial3x3->endPass (device, pass);
		}
	}
	_vBlurMaterial3x3->end (device);

	return true;
}

bool ATOM_Blur::drawHBlur3x3 (ATOM_RenderDevice *device, ATOM_Texture *sourceTexture, float blurScale)
{
	ATOM_STACK_TRACE(ATOM_Blur::drawHBlur3x3);

	if (!blurMaterialsInitialized)
	{
		initializeBlurMaterials ();
	}

	if (!_hBlurMaterial3x3)
	{
		return false;
	}

	float w = 1.f / sourceTexture->getWidth ();
	float weights[3] = { 0.25f, 0.5f, 0.25f };
	float offsets[3] = { -w * blurScale, 0.f, w * blurScale };

	_hBlurMaterial3x3->getParameterTable()->setVector ("offsets", ATOM_Vector4f(offsets[0], offsets[1], offsets[2], 0.f));
	_hBlurMaterial3x3->getParameterTable()->setVector ("weights", ATOM_Vector4f(weights[0], weights[1], weights[2], 0.f));
	_hBlurMaterial3x3->getParameterTable()->setTexture ("inputTexture", sourceTexture);

	unsigned numPasses = _hBlurMaterial3x3->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (_hBlurMaterial3x3->beginPass (device, pass))
		{
			drawScreenQuad (device, sourceTexture->getWidth(), sourceTexture->getHeight());
			_hBlurMaterial3x3->endPass (device, pass);
		}
	}
	_hBlurMaterial3x3->end (device);

	return true;
}

bool ATOM_Blur::drawVBlur5x5 (ATOM_RenderDevice *device, ATOM_Texture *sourceTexture, float blurScale)
{
	ATOM_STACK_TRACE(ATOM_Blur::drawVBlur5x5);

	if (!blurMaterialsInitialized)
	{
		initializeBlurMaterials ();
	}

	if (!_vBlurMaterial5x5)
	{
		return false;
	}

	float w = 1.f / sourceTexture->getHeight ();
	float weights[5] = { 0.2f, 0.2f, 0.2f, 0.2f, 0.2f };
	float offsets[5] = { -2.f * w * blurScale, -1.f * w * blurScale, 0.f, 1.f * w * blurScale, 2.f * w * blurScale };

	_vBlurMaterial5x5->getParameterTable()->setFloatArray ("offsets", offsets, 5);
	_vBlurMaterial5x5->getParameterTable()->setFloatArray ("weights", weights, 5);
	_vBlurMaterial5x5->getParameterTable()->setTexture ("inputTexture", sourceTexture);

	unsigned numPasses = _vBlurMaterial5x5->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (_vBlurMaterial5x5->beginPass (device, pass))
		{
			drawScreenQuad (device, sourceTexture->getWidth(), sourceTexture->getHeight());
			_vBlurMaterial5x5->endPass (device, pass);
		}
	}
	_vBlurMaterial5x5->end (device);

	return true;
}

bool ATOM_Blur::drawHBlur5x5 (ATOM_RenderDevice *device, ATOM_Texture *sourceTexture, float blurScale)
{
	ATOM_STACK_TRACE(ATOM_Blur::drawHBlur5x5);

	if (!blurMaterialsInitialized)
	{
		initializeBlurMaterials ();
	}

	if (!_hBlurMaterial5x5)
	{
		return false;
	}

	float w = 1.f / sourceTexture->getWidth ();
	float weights[5] = { 0.2f, 0.2f, 0.2f, 0.2f, 0.2f };
	float offsets[5] = { -2.f * w * blurScale, -1.f * w * blurScale, 0.f, 1.f * w * blurScale, 2.f * w * blurScale };

	_hBlurMaterial5x5->getParameterTable()->setFloatArray ("offsets", offsets, 5);
	_hBlurMaterial5x5->getParameterTable()->setFloatArray ("weights", weights, 5);
	_hBlurMaterial5x5->getParameterTable()->setTexture ("inputTexture", sourceTexture);

	unsigned numPasses = _hBlurMaterial5x5->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (_hBlurMaterial5x5->beginPass (device, pass))
		{
			drawScreenQuad (device, sourceTexture->getWidth(), sourceTexture->getHeight());
			_hBlurMaterial5x5->endPass (device, pass);
		}
	}
	_hBlurMaterial5x5->end (device);

	return true;
}

bool ATOM_Blur::drawVBlur7x7 (ATOM_RenderDevice *device, ATOM_Texture *sourceTexture, float blurScale)
{
	ATOM_STACK_TRACE(ATOM_Blur::drawVBlur7x7);

	if (!blurMaterialsInitialized)
	{
		initializeBlurMaterials ();
	}

	if (!_vBlurMaterial7x7)
	{
		return false;
	}

	float w = 1.f / sourceTexture->getHeight ();
	float weights[7] = { 0.143f, 0.143f, 0.143f, 0.143f, 0.143f, 0.143f, 0.143f };
	float offsets[7] = { -3.f * w * blurScale, -2.f * w * blurScale, -1.f * w * blurScale, 0.f, 1.f * w * blurScale, 2.f * w * blurScale, 3.f * w * blurScale };

	_vBlurMaterial7x7->getParameterTable()->setFloatArray ("offsets", offsets, 7);
	_vBlurMaterial7x7->getParameterTable()->setFloatArray ("weights", weights, 7);
	_vBlurMaterial7x7->getParameterTable()->setTexture ("inputTexture", sourceTexture);

	unsigned numPasses = _vBlurMaterial7x7->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (_vBlurMaterial7x7->beginPass (device, pass))
		{
			drawScreenQuad (device, sourceTexture->getWidth(), sourceTexture->getHeight());
			_vBlurMaterial7x7->endPass (device, pass);
		}
	}
	_vBlurMaterial7x7->end (device);

	return true;
}

bool ATOM_Blur::drawHBlur7x7 (ATOM_RenderDevice *device, ATOM_Texture *sourceTexture, float blurScale)
{
	ATOM_STACK_TRACE(ATOM_Blur::drawHBlur7x7);

	if (!blurMaterialsInitialized)
	{
		initializeBlurMaterials ();
	}

	if (!_hBlurMaterial7x7)
	{
		return false;
	}

	float w = 1.f / sourceTexture->getWidth ();
	float weights[7] = { 0.143f, 0.143f, 0.143f, 0.143f, 0.143f, 0.143f, 0.143f };
	float offsets[7] = { -3.f * w * blurScale, -2.f * w * blurScale, -1.f * w * blurScale, 0.f, 1.f * w * blurScale, 2.f * w * blurScale, 3.f * w * blurScale };

	_hBlurMaterial7x7->getParameterTable()->setFloatArray ("offsets", offsets, 7);
	_hBlurMaterial7x7->getParameterTable()->setFloatArray ("weights", weights, 7);
	_hBlurMaterial7x7->getParameterTable()->setTexture ("inputTexture", sourceTexture);

	unsigned numPasses = _hBlurMaterial7x7->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (_hBlurMaterial7x7->beginPass (device, pass))
		{
			drawScreenQuad (device, sourceTexture->getWidth(), sourceTexture->getHeight());
			_hBlurMaterial7x7->endPass (device, pass);
		}
	}
	_hBlurMaterial7x7->end (device);

	return true;
}

