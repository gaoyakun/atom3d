#include "StdAfx.h"
#include "cloudplane.h"
#include "blur.h"

class Noise3D 
{
public:
	float noise(float x, float y, float z) 
	{
		int X = ATOM_ftol(x);
		int Y = ATOM_ftol(y);
		int Z = ATOM_ftol(z);
		x -= X;
		y -= Y;
		z -= Z;
		X &= 15;
		Y &= 15;
		Z &= 15;
		float u = fade(x);                                // COMPUTE FADE CURVES
		float v = fade(y);                                // FOR EACH OF X,Y,Z.
		float w = fade(z);
		int A = p[X]+Y;
		int AA = p[A]+Z;
		int AB = p[A+1]+Z;      // HASH COORDINATES OF
		int B = p[X+1]+Y;
		int BA = p[B]+Z;
		int BB = p[B+1]+Z;      // THE 8 CUBE CORNERS,

		return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z),  // AND ADD
                                     grad(p[BA], x-1, y, z)), // BLENDED
                             lerp(u, grad(p[AB], x, y-1, z),  // RESULTS
                                     grad(p[BB], x-1, y-1, z))),// FROM  8
                     lerp(v, lerp(u, grad(p[AA+1], x, y, z-1),  // CORNERS
                                     grad(p[BA+1], x-1, y, z-1)), // OF CUBE
                             lerp(u, grad(p[AB+1], x, y-1, z-1),
                                     grad(p[BB+1], x-1, y-1, z-1))));
	}

	float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
	float lerp(float t, float a, float b) { return a + t * (b - a); }
	float grad(int hash, float x, float y, float z) 
	{
		int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
		float u = (h<8) ? x : y;                 // INTO 12 GRADIENT DIRECTIONS.
		float v = (h<4) ? y : (h==12||h==14) ? x : z;
		return (((h&1) == 0) ? u : -u) + (((h&2) == 0) ? v : -v);
	}

	int p[32];

	Noise3D (void)
	{
		for (int i=0; i < 16 ; i++) 
		{
			p[i] = rand () & 15;
		}

		for (int i=0; i < 16 ; i++) 
		{
			p[16+i] = p[i]; 
		}
	}
};

static Noise3D noise;

static void generateNoiseMap (float z, unsigned size, unsigned char *buffer)
{
	float invSize = 16.f/size;
	for (unsigned j = 0; j < size; ++j)
	{
		for (unsigned k = 0; k < size; ++k)
		{
			float f = noise.noise(j * invSize, k * invSize, z) * 0.5f + 0.5f;
			buffer[j * size + k] = ATOM_ftol(f * 255);
		}
	}
}

static const char *materialSrcStarfield =
	"material															\n"
	"{																	\n"
	"  effect{															\n"
	"	source \"														\n"
	"	  float4x4 MVP;													\n"
	"	  float4 color;													\n"
	"	  float repeat;													\n"
	"	  struct VS_INPUT												\n"
	"	  {																\n"
	"		float4 position: POSITION0;									\n"
	"		float4 color: COLOR0;										\n"
	"		float2 texcoord: TEXCOORD0;									\n"
	"	  };															\n"
	"	  struct VS_OUTPUT												\n"
	"	  {																\n"
	"		float4 position: POSITION0;									\n"
	"		float2 texcoord: TEXCOORD0;									\n"
	"	  };															\n"
	"	  Texture starfieldTexture;										\n"
	"	  sampler2D starfieldSampler = sampler_state					\n"
	"	  {																\n"
	"		MipFilter = None;											\n"
	"		AddressU = Wrap;											\n"
	"		AddressV = Wrap;											\n"
	"		SRGBTEXTURE = 1;											\n"
	"		Texture = (starfieldTexture);								\n"
	"	  };															\n"
	"	  VS_OUTPUT vs(in VS_INPUT IN)									\n"
	"	  {																\n"
	"		VS_OUTPUT OUT;												\n"
	"		OUT.position = mul(IN.position, MVP);						\n"
	"		OUT.texcoord = IN.texcoord * repeat;						\n"
	"		return OUT;													\n"
	"	  }																\n"
	"	  float4 ps(in VS_OUTPUT IN): COLOR0							\n"
	"	  {																\n"
	"		float3 c = tex2D(starfieldSampler, IN.texcoord).rgb;		\n"
	"		return float4(c, color.a);									\n"
	"	  }																\n"
	"	  technique t0													\n"
	"	  {																\n"
	"		pass P0														\n"
	"		{															\n"
	"		  ZFunc = Always;											\n"
	"		  ZWriteEnable = False;										\n"
	"		  CullMode = None;											\n"
	"		  AlphaBlendEnable = True;									\n"
	"		  SrcBlend = SrcAlpha;										\n"
	"		  DestBlend = One;											\n"
	"		  VertexShader = compile vs_1_1 vs();						\n"
	"		  PixelShader = compile ps_2_0 ps();						\n"
	"		}															\n"
	"	  }\";															\n"
	"	param MVP matrix44f MVPMATRIX;									\n"
	"  }																\n"
	"}																	\n";


static const char *materialSrcMoon =
	"material															\n"
	"{																	\n"
	"  effect{															\n"
	"	source \"														\n"
	"	  float4 color;													\n"
	"	  struct VS_INPUT												\n"
	"	  {																\n"
	"		float4 position: POSITION0;									\n"
	"		float2 texcoord: TEXCOORD0;									\n"
	"	  };															\n"
	"	  struct VS_OUTPUT												\n"
	"	  {																\n"
	"		float4 position: POSITION0;									\n"
	"		float2 texcoord: TEXCOORD0;									\n"
	"	  };															\n"
	"	  Texture moonTexture;											\n"
	"	  sampler2D moonSampler = sampler_state							\n"
	"	  {																\n"
	"		MipFilter = None;											\n"
	"		Texture = (moonTexture);									\n"
	"		SRGBTEXTURE = 1;											\n"
	"	  };															\n"
	"	  VS_OUTPUT vs(in VS_INPUT IN)									\n"
	"	  {																\n"
	"		VS_OUTPUT OUT;												\n"
	"		OUT.position = IN.position;									\n"
	"		OUT.texcoord = IN.texcoord;									\n"
	"		return OUT;													\n"
	"	  }																\n"
	"	  float4 ps(in VS_OUTPUT IN): COLOR0							\n"
	"	  {																\n"
	"		return tex2D(moonSampler, IN.texcoord) * color;				\n"
	"	  }																\n"
	"	  float4 ps2(in VS_OUTPUT IN): COLOR0							\n"
	"	  {																\n"
	"		float4 c = tex2D(moonSampler, IN.texcoord);					\n"
	"		c.a = 1.0 - c.a;											\n"
	"		return c * color;											\n"
	"	  }																\n"
	"	  technique t0													\n"
	"	  {																\n"
	"		pass P0														\n"
	"		{															\n"
	"		  ZEnable = False;											\n"
	"		  CullMode = None;											\n"
	"		  AlphaBlendEnable = True;									\n"
	"		  SrcBlend = SrcAlpha;										\n"
	"		  DestBlend = InvSrcAlpha;									\n"
	"		  VertexShader = compile vs_1_1 vs();						\n"
	"		  PixelShader = compile ps_2_0 ps();						\n"
	"		}															\n"
	"		pass P1														\n"
	"		{															\n"
	"		  DestBlend = One;											\n"
	"		  PixelShader = compile ps_2_0 ps2();						\n"
	"		}															\n"
	"	  }\";															\n"
	"  }																\n"
	"}																	\n";

static const char *materialSrcShade = 
	"material															\n"
	"{																	\n"
	"  effect{															\n"
	"	source \"														\n"
	"	  float3 sunPos;												\n"
	"	  float coverage;												\n"
	"	  float2 offset;												\n"
	"	  struct VS_INPUT												\n"
	"	  {																\n"
	"		float4 position: POSITION0;									\n"
	"		float4 color: COLOR0;										\n"
	"		float2 texcoord: TEXCOORD0;									\n"
	"	  };															\n"
	"	  struct VS_OUTPUT												\n"
	"	  {																\n"
	"		float4 position: POSITION0;									\n"
	"		float2 texcoord: TEXCOORD0;									\n"
	"		float2 texcoord2: TEXCOORD1;								\n"
	"	  };															\n"
	"	  Texture t;													\n"
	"	  sampler2D s = sampler_state									\n"
	"	  {																\n"
	"		MipFilter = None;											\n"
	"		AddressU = Wrap;											\n"
	"		AddressV = Wrap;											\n"
	"		Texture = (t);												\n"
	"	  };															\n"
	"	  VS_OUTPUT vs(in VS_INPUT IN)									\n"
	"	  {																\n"
	"		VS_OUTPUT OUT;												\n"
	"		OUT.position = IN.position;									\n"
	"		OUT.texcoord = IN.texcoord + offset;						\n"
	"		OUT.texcoord2 = IN.texcoord;								\n"
	"		return OUT;													\n"
	"	  }																\n"
	"	  float4 ps(in VS_OUTPUT IN): COLOR0							\n"
	"	  {																\n"
	"		float scattering = 0.f;										\n"
	"		float a = saturate(tex2D(s, IN.texcoord).r - coverage);		\n"
	"		float3 endTracePos = float3(IN.texcoord2, -a * 255);		\n"
	"		float3 sunRay = endTracePos - sunPos;						\n"
	"		float sunDist2D = length(sunRay.xy);						\n"
	"		float3 traceDir = sunRay / 64.f;							\n"
	"		float3 currentTracePos = sunPos;							\n"
	"		for (int i = 0; i < 64; ++i)								\n"
	"		{															\n"
	"			currentTracePos += traceDir;							\n"
	"			float h = saturate(tex2D(s, currentTracePos.xy).r - coverage) * 255;			\n"
	"			scattering += 0.05f * step(currentTracePos.z * 2, h * 2);\n"
	"		}															\n"
	"		float l = 1.f/exp(scattering * 0.4f);						\n"
	"		return float4(l, l, l, sunDist2D);							\n"
	"	  }																\n"
	"	  technique t0													\n"
	"	  {																\n"
	"		pass P0														\n"
	"		{															\n"
	"		  ZEnable = False;											\n"
	"		  CullMode = None;											\n"
	"		  VertexShader = compile vs_3_0 vs();						\n"
	"		  PixelShader = compile ps_3_0 ps();						\n"
	"		}															\n"
	"	  }\";															\n"
	"  }																\n"
	"}																	\n";


static const char *materialSrcCombine = 
	"material															\n"
	"{																	\n"
	"  effect{															\n"
	"	source \"														\n"
	"	  struct VS_INPUT												\n"
	"	  {																\n"
	"		float4 position: POSITION0;									\n"
	"		float2 texcoord: TEXCOORD0;									\n"
	"	  };															\n"
	"	  struct VS_OUTPUT												\n"
	"	  {																\n"
	"		float4 position: POSITION0;									\n"
	"		float2 texcoord: TEXCOORD0;									\n"
	"	  };															\n"
	"	  Texture t;													\n"
	"	  sampler2D s = sampler_state									\n"
	"	  {																\n"
	"		MipFilter = None;											\n"
	"		AddressU = Wrap;											\n"
	"		AddressV = Wrap;											\n"
	"		Texture = (t);												\n"
	"	  };															\n"
	"	  VS_OUTPUT vs(in VS_INPUT IN)									\n"
	"	  {																\n"
	"		VS_OUTPUT OUT;												\n"
	"		OUT.position = IN.position;									\n"
	"		OUT.texcoord = IN.texcoord;									\n"
	"		return OUT;													\n"
	"	  }																\n"
	"	  float4 ps(in VS_OUTPUT IN): COLOR0							\n"
	"	  {																\n"
	"		float r = tex2D(s, IN.texcoord).r;							\n"
	"		float g = tex2D(s, IN.texcoord * 2.f).r;					\n"
	"		float b = tex2D(s, IN.texcoord * 4.f).r;					\n"
	"		float a = tex2D(s, IN.texcoord * 8.f).r;					\n"
	"	    const float4 weight = float4(0.5, 0.25, 0.125, 0.0625);		\n"
	"		float c = dot(weight, float4(r, g, b, a));					\n"
	"		return float4(c, c, c, c);									\n"
	"	  }																\n"
	"	  technique t0													\n"
	"	  {																\n"
	"		pass P0														\n"
	"		{															\n"
	"		  ZEnable = False;											\n"
	"		  CullMode = None;											\n"
	"		  VertexShader = compile vs_1_1 vs();						\n"
	"		  PixelShader = compile ps_2_0 ps();						\n"
	"		}															\n"
	"	  }\";															\n"
	"  }																\n"
	"}																	\n";

static const char *materialSrc = 
	"material															\n"
	"{																	\n"
	"  effect{															\n"
	"	source \"														\n"
	"	  float4x4 MVP;													\n"
	"	  float coverage;												\n"
	"	  float4 color;													\n"
	"	  float2 offset;												\n"
	"	  float sharpness;												\n"
	"	  struct VS_INPUT												\n"
	"	  {																\n"
	"		float4 position: POSITION0;									\n"
	"		float4 color: COLOR0;										\n"
	"		float2 texcoord: TEXCOORD0;									\n"
	"	  };															\n"
	"	  struct VS_OUTPUT												\n"
	"	  {																\n"
	"		float4 position: POSITION0;									\n"
	"		float2 texcoord: TEXCOORD0;									\n"
	"		float2 texcoord2: TEXCOORD1;								\n"
	"		float4 color: TEXCOORD2;									\n"
	"	  };															\n"
	"	  Texture noiseTexture0;										\n"
	"	  sampler2D noiseSampler0 = sampler_state						\n"
	"	  {																\n"
	"		MipFilter = None;											\n"
	"		AddressU = Wrap;											\n"
	"		AddressV = Wrap;											\n"
	"		Texture = (noiseTexture0);									\n"
	"	  };															\n"
	"	  Texture noiseTexture1;										\n"
	"	  sampler2D noiseSampler1 = sampler_state						\n"
	"	  {																\n"
	"		MipFilter = None;											\n"
	"		AddressU = Wrap;											\n"
	"		AddressV = Wrap;											\n"
	"		Texture = (noiseTexture1);									\n"
	"	  };															\n"
	"	  Texture shadeTexture;											\n"
	"	  sampler2D shadeSampler = sampler_state						\n"
	"	  {																\n"
	"		MipFilter = None;											\n"
	"		AddressU = Wrap;											\n"
	"		AddressV = Wrap;											\n"
	"		Texture = (shadeTexture);									\n"
	"	  };															\n"
	"	  VS_OUTPUT vs(in VS_INPUT IN)									\n"
	"	  {																\n"
	"		VS_OUTPUT OUT;												\n"
	"		OUT.position = mul(IN.position, MVP);						\n"
	"		OUT.texcoord = IN.texcoord + offset;						\n"
	"		OUT.texcoord2 = IN.texcoord;								\n"
	"		OUT.color = IN.color;										\n"
	"		return OUT;													\n"
	"	  }																\n"
	"	  float4 ps(in VS_OUTPUT IN): COLOR0							\n"
	"	  {																\n"
	"		float a = tex2D(noiseSampler0, IN.texcoord).r;				\n"
	"		a += tex2D(noiseSampler1, IN.texcoord * 16).r * 0.03125f;	\n"
	"		a += tex2D(noiseSampler1, IN.texcoord * 32).r * 0.015625f;	\n"
	"		a += tex2D(noiseSampler1, IN.texcoord * 64).r * 0.0078125f;	\n"
	"		a += tex2D(noiseSampler1, IN.texcoord * 128).r * 0.0078125f;\n"
	"		float c = saturate(a - coverage);							\n"
	"		float d = 1.0 - pow(sharpness, c * 255);					\n"
	"		float4 shade = tex2D(shadeSampler, IN.texcoord2);			\n"
	"		float3 shadeColor = lerp(color.rgb * shade.r, shade.rrr, shade.a);	\n"
	"		float opacity = 2.f - shade.a;								\n"
	"		float3 shadeColorTweaked = max(shadeColor - 0.75f, float3(0,0,0));	\n"
	"		shadeColor += shadeColorTweaked * max(1 - shade.a * 16, 0);	\n"
	"		float4 result = lerp(float4(shadeColor, d * opacity * IN.color.a), float4(color.rgb,shadeColor.r * d * IN.color.a), shade.a * 2);	\n"
	"		return result;												\n"
	"	  }																\n"
	"	  technique t0													\n"
	"	  {																\n"
	"		pass P0														\n"
	"		{															\n"
	"		  AlphaBlendEnable = True;									\n"
	"		  SrcBlend = SrcAlpha;										\n"
	"		  DestBlend = InvSrcAlpha;									\n"
	"		  ZFunc = Always;											\n"
	"		  ZWriteEnable = False;										\n"
	"		  CullMode = None;											\n"
	"		  VertexShader = compile vs_1_1 vs();						\n"
	"		  PixelShader = compile ps_2_0 ps();						\n"
	"		}															\n"
	"	  }\";															\n"
	"	param MVP matrix44f MVPMATRIX;									\n"
	"  }																\n"
	"}																	\n";

static const char *materialSrcLow = 
	"material															\n"
	"{																	\n"
	"  effect{															\n"
	"	source \"														\n"
	"	  float4x4 MVP;													\n"
	"	  float coverage;												\n"
	"	  float4 color;													\n"
	"	  float2 offset;												\n"
	"	  float sharpness;												\n"
	"	  struct VS_INPUT												\n"
	"	  {																\n"
	"		float4 position: POSITION0;									\n"
	"		float4 color: COLOR0;										\n"
	"		float2 texcoord: TEXCOORD0;									\n"
	"	  };															\n"
	"	  struct VS_OUTPUT												\n"
	"	  {																\n"
	"		float4 position: POSITION0;									\n"
	"		float2 texcoord: TEXCOORD0;									\n"
	"		float2 texcoord2: TEXCOORD1;								\n"
	"		float4 color: TEXCOORD2;									\n"
	"	  };															\n"
	"	  Texture noiseTexture0;										\n"
	"	  sampler2D noiseSampler0 = sampler_state						\n"
	"	  {																\n"
	"		MipFilter = None;											\n"
	"		AddressU = Wrap;											\n"
	"		AddressV = Wrap;											\n"
	"		Texture = (noiseTexture0);									\n"
	"	  };															\n"
	"	  Texture noiseTexture1;										\n"
	"	  sampler2D noiseSampler1 = sampler_state						\n"
	"	  {																\n"
	"		MipFilter = None;											\n"
	"		AddressU = Wrap;											\n"
	"		AddressV = Wrap;											\n"
	"		Texture = (noiseTexture1);									\n"
	"	  };															\n"
	"	  Texture shadeTexture;											\n"
	"	  sampler2D shadeSampler = sampler_state						\n"
	"	  {																\n"
	"		MipFilter = None;											\n"
	"		AddressU = Wrap;											\n"
	"		AddressV = Wrap;											\n"
	"		Texture = (shadeTexture);									\n"
	"	  };															\n"
	"	  VS_OUTPUT vs(in VS_INPUT IN)									\n"
	"	  {																\n"
	"		VS_OUTPUT OUT;												\n"
	"		OUT.position = mul(IN.position, MVP);						\n"
	"		OUT.texcoord = IN.texcoord + offset;						\n"
	"		OUT.texcoord2 = IN.texcoord;								\n"
	"		OUT.color = IN.color;										\n"
	"		return OUT;													\n"
	"	  }																\n"
	"	  float4 ps(in VS_OUTPUT IN): COLOR0							\n"
	"	  {																\n"
	"		float a = tex2D(noiseSampler0, IN.texcoord).r;				\n"
	"		a += tex2D(noiseSampler1, IN.texcoord * 16).r * 0.03125f;	\n"
	"		a += tex2D(noiseSampler1, IN.texcoord * 32).r * 0.015625f;	\n"
	"		a += tex2D(noiseSampler1, IN.texcoord * 64).r * 0.0078125f;	\n"
	"		a += tex2D(noiseSampler1, IN.texcoord * 128).r * 0.0078125f;\n"
	"		float c = saturate(a - coverage);							\n"
	"		float d = 1.0 - pow(sharpness, c * 255);					\n"
	"		return float4 (color.rgb, d * IN.color.a);					\n"
	"	  }																\n"
	"	  technique t0													\n"
	"	  {																\n"
	"		pass P0														\n"
	"		{															\n"
	"		  AlphaBlendEnable = True;									\n"
	"		  SrcBlend = SrcAlpha;										\n"
	"		  DestBlend = InvSrcAlpha;									\n"
	"		  ZFunc = Always;											\n"
	"		  ZWriteEnable = False;										\n"
	"		  CullMode = None;											\n"
	"		  VertexShader = compile vs_1_1 vs();						\n"
	"		  PixelShader = compile ps_2_0 ps();						\n"
	"		}															\n"
	"	  }\";															\n"
	"	param MVP matrix44f MVPMATRIX;									\n"
	"  }																\n"
	"}																	\n";

ATOM_CloudPlane::ATOM_CloudPlane (void)
{
	ATOM_STACK_TRACE(ATOM_CloudPlane::ATOM_CloudPlane);
	_width = 100.f;
	_dist = 0.5f;
	_z = 0.f;
	_sunHeight = 400.f;
	_starfieldTextureRepeat = 1.f;
	_dimension = 10;
	_drawMode = DM_SM3;
	_dirty = true;
	_mdirty = true;
	_lastDrawStamp = 0;
	_lastDrawTick = 0;
	_lastColorUpdateStamp = 0;
	_lastColorUpdateTick = 0;
	_noiseTextureIndex = 0;
	_scrollVelocity.set (0.005f, 0.005f);
	_deformSpeed = 0.01f;
	_offset.set(0.f, 0.f);
	_buffer = (unsigned char*)ATOM_MALLOC(mapRes * mapRes);
}

ATOM_CloudPlane::~ATOM_CloudPlane (void)
{
	ATOM_STACK_TRACE(ATOM_CloudPlane::~ATOM_CloudPlane);
	if (_vertexDecl)
	{
		ATOM_RenderDevice *device = ATOM_GetRenderDevice();
		if (device)
		{
			device->destroyVertexDecl (_vertexDecl);
			_vertexDecl = 0;
		}
	}
	ATOM_FREE(_buffer);
}

void ATOM_CloudPlane::setDeformSpeed (float speed)
{
	_deformSpeed = speed;
}

float ATOM_CloudPlane::getDeformSpeed (void) const
{
	return _deformSpeed;
}

void ATOM_CloudPlane::setScrollVelocity (const ATOM_Vector2f &velocity)
{
	_scrollVelocity = velocity;
}

const ATOM_Vector2f &ATOM_CloudPlane::getScrollVelocity (void) const
{
	return _scrollVelocity;
}

void ATOM_CloudPlane::setWidth (float width)
{
	if (_width != width)
	{
		_width = width;
		_dirty = true;
	}
}

float ATOM_CloudPlane::getWidth (void) const
{
	return _width;
}

void ATOM_CloudPlane::setDistance (float distance)
{
	if (_dist != distance)
	{
		_dist = distance;
		_dirty = true;
	}
}

float ATOM_CloudPlane::getDistance (void) const
{
	return _dist;
}

void ATOM_CloudPlane::setDimension (int dimension)
{
	if (_dimension != dimension)
	{
		_dimension = dimension;
		_dirty = true;
	}
}

int ATOM_CloudPlane::getDimension (void) const
{
	return _dimension;
}

bool ATOM_CloudPlane::create (ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_CloudPlane::create);

	if (_dirty)
	{
		_dirty = false;
		initGeometry (device);
	}

	if (!_vertexArray)
	{
		return false;
	}

	if (_mdirty)
	{
		_mdirty = false;
		initMaterial ();
	}

	if (!_material || !_materialCombine)
	{
		return false;
	}

	if (!_combinedNoiseTexture)
	{
		bool ret = false;
		if (device->beginFrame ())
		{
			ATOM_Rect2Di oldVP = device->getViewport (0);
			ATOM_AUTOREF(ATOM_Texture) oldRT0 = device->getRenderTarget(0);
			ATOM_AUTOREF(ATOM_Texture) oldRT1 = device->getRenderTarget(1);

			ret = initNoiseMaps (device, _z);

			device->setRenderTarget (0, oldRT0.get());
			if (oldRT1)
			{
				device->setRenderTarget (1, oldRT1.get());
			}
			device->setViewport (0, oldVP);
			device->endFrame ();
		}

		if (!ret)
		{
			return false;
		}
	}

	return true;
}

void ATOM_CloudPlane::draw (ATOM_RenderDevice *device, float coverage, float sharpness, const ATOM_Vector4f &color, float moonSize, const ATOM_Vector4f &moonColor, const ATOM_Vector4f &Pos)
{
	/*
	ATOM_STACK_TRACE(ATOM_CloudPlane::draw);

	bool drawStar = _starfieldTexture && moonColor.w > 0.f && _materialStarfield;
	bool drawMoon = _moonTexture && moonColor.w > 0.f && moonSize > 0.f && _materialMoon;
	float aspect;
	ATOM_Vector2f moonPos;

	if (drawMoon)
	{
		ATOM_Matrix4x4f projMatrix;
		device->getTransform (ATOM_MATRIXMODE_PROJECTION, projMatrix);
		float nearPlane = -projMatrix.m32 / projMatrix.m22;

		ATOM_Matrix4x4f vMatrix;
		device->getTransform (ATOM_MATRIXMODE_INV_VIEW, vMatrix);
		ATOM_Vector4f viewPos = vMatrix >> Pos;
		if (viewPos.z < nearPlane)
		{
			drawMoon = false;
		}
		else
		{
			aspect = projMatrix.m11 / projMatrix.m00;
			ATOM_Matrix4x4f vpMatrix;
			device->getTransform (ATOM_MATRIXMODE_VIEWPROJ, vpMatrix);
			moonPos = computeMoonPosition (Pos, vpMatrix);
		}
	}

	if (drawMoon)
	{
		float moonSizeY = moonSize * aspect;
		_materialMoon->getParameterTable()->setTexture ("moonTexture", _moonTexture.get());
		_materialMoon->getParameterTable()->setVector ("color", moonColor);

		float vertices[4 * 5] = {
			moonPos.x - moonSize, moonPos.y - moonSizeY, 1.f, 0.f, 1.f,
			moonPos.x + moonSize, moonPos.y - moonSizeY, 1.f, 1.f, 1.f,
			moonPos.x + moonSize, moonPos.y + moonSizeY, 1.f, 1.f, 0.f,
			moonPos.x - moonSize, moonPos.y + moonSizeY, 1.f, 0.f, 0.f
		};

		unsigned short indices[4] = {
			0, 1, 2, 3
		};

		unsigned numPasses = _materialMoon->begin (device);
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (_materialMoon->beginPass (device, pass))
			{
				device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_FAN, 4, 4, ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2, 5 * sizeof(float), vertices, indices);
				_materialMoon->endPass (device, pass);
			}
		}
		_materialMoon->end (device);
	}

	// draw cloud
	device->pushMatrix (ATOM_MATRIXMODE_PROJECTION);
	ATOM_Matrix4x4f matrix;
	device->getTransform (ATOM_MATRIXMODE_PROJECTION, matrix);
	if (matrix.m33 < 0.1f)
	{
		// perspective
		float f = 1000.f;
		float n = 1.f;
		matrix.m22 = f / (f - n);
		matrix.m32 = -n * matrix.m22;
		device->setTransform (ATOM_MATRIXMODE_PROJECTION, matrix);
	}

	ATOM_Matrix4x4f matWorld = ATOM_Matrix4x4f::getIdentityMatrix ();
	const ATOM_Matrix4x4f matView = ATOM_Scene::getCurrentScene()->getCamera()->getViewMatrix();
	matWorld.m30 = matView.m30;
	matWorld.m31 = matView.m31;
	matWorld.m32 = matView.m32;
	device->setTransform (ATOM_MATRIXMODE_WORLD, matWorld);

	if (drawStar)
	{
		_materialStarfield->getParameterTable()->setTexture ("starfieldTexture", _starfieldTexture.get());
		_materialStarfield->getParameterTable()->setVector ("color", moonColor);
		_materialStarfield->getParameterTable()->setFloat ("repeat", _starfieldTextureRepeat);
		device->setStreamSource (0, _vertexArray.get());
		device->setVertexDecl (_vertexDecl);
		unsigned numPasses = _materialStarfield->begin (device);
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (_materialStarfield->beginPass (device, pass))
			{
				device->renderStreams (ATOM_PRIMITIVE_TRIANGLES, _vertexArray->getNumVertices()/3);
				_materialStarfield->endPass (device, pass);
			}
		}
		_materialStarfield->end (device);
	}

	ATOM_Material *m = _material.get();

	if (_colorTexture && _materialShade && _drawMode == DM_SM3)
	{
		m = _material.get();
		m->getParameterTable()->setTexture ("shadeTexture", _colorTexture.get());
	}
	else
	{
		m = _materialLow.get();
	}
	m->getParameterTable()->setTexture ("noiseTexture0", _combinedNoiseTexture.get());
	m->getParameterTable()->setTexture ("noiseTexture1", _baseNoiseTexture[_noiseTextureIndex].get());
	m->getParameterTable()->setFloat ("sharpness", sharpness);
	m->getParameterTable()->setFloat ("coverage", coverage);
	m->getParameterTable()->setVector ("color", color);
	m->getParameterTable()->setVector ("offset", ATOM_Vector4f(_offset.x, _offset.y, 0.f, 0.f));

	device->setStreamSource (0, _vertexArray.get());
	device->setVertexDecl (_vertexDecl);

	unsigned numPasses = m->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (m->beginPass (device, pass))
		{
			device->renderStreams (ATOM_PRIMITIVE_TRIANGLES, _vertexArray->getNumVertices()/3);
			m->endPass (device, pass);
		}
	}
	m->end (device);
	device->popMatrix (ATOM_MATRIXMODE_PROJECTION);
}

bool ATOM_CloudPlane::initMaterial (void)
{
	ATOM_STACK_TRACE(ATOM_CloudPlane::initMaterial);

	if (!_materialShade && ATOM_RenderSettings::getRenderCaps().shaderModel >= 3)
	{
		_materialShade = ATOM_MaterialManager::createMaterialFromCore(ATOM_GetRenderDevice(), "/materials/builtin/cloud_shade.mat");
		if (!_materialShade)
		{
			_materialShade = 0;
		}
	}

	if (!_materialCombine)
	{
		_materialCombine = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/cloud_combine.mat");
		if (!_materialCombine)
		{
			_materialShade = 0;
			_materialCombine = 0;
			return false;
		}
	}

	if (!_material)
	{
		_material = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/cloud.mat");
		if (!_material)
		{
			_materialShade = 0;
			_materialCombine = 0;
			_material = 0;
			return false;
		}
	}

	if (!_materialLow)
	{
		_materialLow = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/cloud_low.mat");
		if (!_materialLow)
		{
			_materialShade = 0;
			_materialCombine = 0;
			_material = 0;
			_materialLow = 0;
			return false;
		}
	}

	if (!_materialMoon)
	{
		_materialMoon = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/moon.mat");
		if (!_materialMoon)
		{
			_materialMoon = 0;
		}
	}

	if (!_materialStarfield)
	{
		_materialStarfield = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/starfield.mat");
		if (!_materialStarfield)
		{
			_materialStarfield = 0;
		}
	}

	return true;
	*/
}

ATOM_Vector2f ATOM_CloudPlane::transformDirToTextureSpace (const ATOM_Vector3f &dir) const
{
	// sunDir must be normalized
	float cosHalfDist = ATOM_cos(_dist * 0.5f);
	float sinDist = ATOM_sin(_dist);
	float radius = ATOM_sin((ATOM_Pi - _dist) * 0.5f) * _width / sinDist;
	float sinAngleBeta;
	float angleBeta;
	float angleTheta;

	float cosAngleX = ATOM_abs(dir.x);
	float angleX = ATOM_acos (cosAngleX);
	sinAngleBeta = ATOM_sin(ATOM_HalfPi + angleX) * cosHalfDist;
	angleBeta = ATOM_asin (sinAngleBeta);
	angleTheta = ATOM_HalfPi - angleBeta - angleX;
	float tx = radius * cosHalfDist * ATOM_tan(angleTheta) / _width;
	if (dir.x < 0.f) tx = -tx;

	float cosAngleZ = ATOM_abs(dir.z);
	float angleZ = ATOM_acos (cosAngleZ);
	sinAngleBeta = ATOM_sin(ATOM_HalfPi + angleZ) * cosHalfDist;
	angleBeta = ATOM_asin (sinAngleBeta);
	angleTheta = ATOM_HalfPi - angleBeta - angleZ;
	float tz = radius * cosHalfDist * ATOM_tan(angleTheta) / _width;
	if (dir.z < 0.f) tz = -tz;

	return ATOM_Vector2f(0.5f + tx, 0.5f + tz);
}

static ATOM_Vector3f _computeSkyPlaneVertex (float size, int dimension, int x, int z, float alpha)
{
	const float radius = ATOM_sin((ATOM_Pi - alpha) * 0.5f) * size / ATOM_sin(alpha);
	const float deltaAngle = alpha / dimension;
	const float angleX = deltaAngle * x;
	const float angleX2 = (ATOM_Pi - angleX) * 0.5f;
	const float lX = radius * ATOM_sin(angleX) / ATOM_sin(angleX2);
	const float x0 = lX * ATOM_cos((alpha - angleX) * 0.5f) - size * 0.5f;

	const float angleZ = deltaAngle * z;
	const float angleZ2 = (ATOM_Pi - angleZ) * 0.5f;
	const float lZ = radius * ATOM_sin(angleZ) / ATOM_sin(angleZ2);
	const float z0 = lZ * ATOM_cos((alpha - angleZ) * 0.5f) - size * 0.5f;

	float y0_x = lX * ATOM_sin((alpha - angleX) * 0.5f);
	float y0_z = lZ * ATOM_sin((alpha - angleZ) * 0.5f);
	const float y0 = (y0_x + y0_z) * 0.5f;

	return ATOM_Vector3f (x0, y0, z0);
}

#define COLOR_ARGB(a,r,g,b) \
((unsigned)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

bool ATOM_CloudPlane::initGeometry (ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_CloudPlane::initGeometry);

	struct SkyVertex
	{
		ATOM_Vector3f pos;
		unsigned color;
		ATOM_Vector2f tex;
	};

	_vertexArray = ATOM_GetRenderDevice()->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_PRIMARY_COLOR|ATOM_VERTEX_ATTRIB_TEX1_2, ATOM_USAGE_STATIC, _dimension * _dimension * 6, true);
	if (!_vertexArray)
	{
		return false;
	}

	ATOM_VECTOR<ATOM_Vector3f> verts;
	ATOM_VECTOR<ATOM_Vector2f> uvs;
	ATOM_VECTOR<unsigned> colors;
	const float t = _dimension * 0.5f;
	for (unsigned i = 0; i <= _dimension; ++i)
		for (unsigned j = 0; j <= _dimension; ++j)
		{
			verts.push_back(_computeSkyPlaneVertex (_width, _dimension, i, j, _dist));
			uvs.push_back(ATOM_Vector2f(float(i)/(float)_dimension, float(j)/(float)_dimension)); 
			const float ai = (i <= t) ? i/t : 2.f - i/t;
			const float aj = (j <= t) ? j/t : 2.f - j/t;
			colors.push_back (COLOR_ARGB((unsigned)(ai * aj * 255), 255, 255, 255));
		}


	SkyVertex *vertexPtr = (SkyVertex*)_vertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	if (!vertexPtr)
	{
		_vertexArray = 0;
		return false;
	}

	for (unsigned i = 0; i < _dimension; ++i)
	{
		const ATOM_Vector3f *vertRow1 = (&verts[0]) + i * (_dimension+1);
		const ATOM_Vector3f *vertRow2 = vertRow1 + _dimension + 1;
		const unsigned *colorRow1 = (&colors[0]) + i * (_dimension+1);
		const unsigned *colorRow2 = colorRow1 + _dimension + 1;
		const ATOM_Vector2f *uvRow1 = (&uvs[0]) + i * (_dimension+1);
		const ATOM_Vector2f *uvRow2 = uvRow1 + _dimension + 1;

		float ti = _dimension * 0.5f;

		for (unsigned j = 0; j < _dimension; ++j)
		{
			vertexPtr->pos = vertRow1[j];
			vertexPtr->color = colorRow1[j];
			vertexPtr->tex = uvRow1[j];
			vertexPtr++;

			vertexPtr->pos = vertRow1[j+1];
			vertexPtr->color = colorRow1[j+1];
			vertexPtr->tex = uvRow1[j+1];
			vertexPtr++;

			vertexPtr->pos = vertRow2[j+1];
			vertexPtr->color = colorRow2[j+1];
			vertexPtr->tex = uvRow2[j+1];
			vertexPtr++;

			vertexPtr->pos = vertRow1[j];
			vertexPtr->color = colorRow1[j];
			vertexPtr->tex = uvRow1[j];
			vertexPtr++;

			vertexPtr->pos = vertRow2[j+1];
			vertexPtr->color = colorRow2[j+1];
			vertexPtr->tex = uvRow2[j+1];
			vertexPtr++;

			vertexPtr->pos = vertRow2[j];
			vertexPtr->color = colorRow2[j];
			vertexPtr->tex = uvRow2[j];
			vertexPtr++;
		}
	}

	_vertexArray->unlock ();

	_vertexDecl = device->createInterleavedVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_PRIMARY_COLOR|ATOM_VERTEX_ATTRIB_TEX1_2);
	if (!_vertexDecl)
	{
		_vertexArray = 0;
		return false;
	}

	return true;
}

static void interpolate( unsigned char *buffer, int step )
{
	ATOM_STACK_TRACE(interpolate);

    int             i, j;
    float           a, b, c, d, xf, yf;

    for( int y=0; y<256; y+= step )
	{
		for( int x=0; x<256; x+= step )
        {
			a = buffer[(y<<8)+x];
			b = buffer[(y<<8)+((x+step)&255)];
			c = buffer[(((y+step)&255)<<8)+x];
			d = buffer[(((y+step)&255)<<8)+((x+step)&255)];

			for( j=0; j< step; j++ )
			{
				for( i=0; i< step; i++ )
				{
					xf = (float)i/(float)step;
					yf = (float)j/(float)step;

					xf = xf*xf*(3.0f-2.0f*xf);
					yf = yf*yf*(3.0f-2.0f*yf);

					buffer[((y+j)<<8)+(x+i)] =  a + xf*(b-a) +
								   yf*(c-a) + xf*yf*(a-b-c+d);
				}
			}
		}
	}
}

bool ATOM_CloudPlane::initColorMap (ATOM_RenderDevice *device, ATOM_Camera *camera, const ATOM_Vector3f &sunDir, float coverage)
{
	ATOM_STACK_TRACE(ATOM_CloudPlane::initColorMap);

	if (!_colorTexture)
	{
		_colorTexture = device->allocTexture (0, 0, mapRes * 8, mapRes * 8, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOMIPMAP|ATOM_Texture::NOCOMPRESS|ATOM_Texture::RENDERTARGET);
	}
	if (!_colorTexture)
	{
		return false;
	}

	ATOM_Vector3f sunDirAdj = sunDir;
	if (sunDirAdj.y < 0.f)
	{
		sunDirAdj.y = 0.f;
	}
	sunDirAdj.normalize ();

	ATOM_Vector2f sunPosTextureSpace = transformDirToTextureSpace (sunDirAdj);
	ATOM_Vector4f sunPos (sunPosTextureSpace.x, sunPosTextureSpace.y, _sunHeight, 1.f);

	device->setRenderTarget (0, _colorTexture.get());
	device->setViewport (0, 0, 0, _colorTexture->getWidth(), _colorTexture->getHeight());
	_materialShade->getParameterTable()->setTexture ("t", _combinedNoiseTexture.get());
	_materialShade->getParameterTable()->setVector ("sunPos", sunPos);
	_materialShade->getParameterTable()->setFloat ("coverage", coverage);
	_materialShade->getParameterTable()->setVector ("offset", ATOM_Vector4f(_offset.x, _offset.y, 0.f, 0.f));

	float vertices[4 * 5] = {
		-1.f, -1.f, 1.f, 0.f, 1.f,
		 1.f, -1.f, 1.f, 1.f, 1.f,
		 1.f,  1.f, 1.f, 1.f, 0.f,
	    -1.f,  1.f, 1.f, 0.f, 0.f
	};

	unsigned short indices[4] = {
		0, 1, 2, 3
	};

	unsigned numPasses = _materialShade->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (_materialShade->beginPass (device, pass))
		{
			device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_FAN, 4, 4, ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2, 5 * sizeof(float), vertices, indices);
			_materialShade->endPass (device, pass);
		}
	}
	_materialShade->end (device);

	return true;
}

bool ATOM_CloudPlane::initNoiseMaps (ATOM_RenderDevice *device, float z)
{
	ATOM_STACK_TRACE(ATOM_CloudPlane::initNoiseMaps);

	generateNoiseMap (z, mapRes, _buffer);

	if (!_baseNoiseTexture[_noiseTextureIndex])
	{
		_baseNoiseTexture[_noiseTextureIndex] = device->allocTexture (0, _buffer, mapRes, mapRes, ATOM_PIXEL_FORMAT_GREY8, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOMIPMAP|ATOM_Texture::NOCOMPRESS);
	}
	else
	{
		ATOM_Texture::LockedRect lrc;
		if (!_baseNoiseTexture[_noiseTextureIndex]->lock (0, &lrc))
		{
			return false;
		}

		if (lrc.pitch == mapRes)
		{
			memcpy (lrc.bits, _buffer, mapRes * mapRes);
		}
		else
		{
			for (unsigned i = 0; i < mapRes; ++i)
			{
				unsigned char *dst = (unsigned char*)lrc.bits + i * lrc.pitch;
				unsigned char *src = _buffer + i * mapRes;
				memcpy (dst, src, mapRes);
			}
		}
		_baseNoiseTexture[_noiseTextureIndex]->unlock (0);
	}

	bool b = false;
	if (b)
	{
		_baseNoiseTexture[_noiseTextureIndex]->saveToFile ("/noise.png");
	}
	if (!_combinedNoiseTexture)
	{
		ATOM_PixelFormat format = device->isRenderTargetFormat (ATOM_PIXEL_FORMAT_GREY8) ? ATOM_PIXEL_FORMAT_GREY8 : ATOM_PIXEL_FORMAT_BGRA8888;
		_combinedNoiseTexture = device->allocTexture (0, 0, mapRes * 8, mapRes * 8, format, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOMIPMAP|ATOM_Texture::NOCOMPRESS|ATOM_Texture::RENDERTARGET);
	}
	if (!_combinedNoiseTexture)
	{
		return false;
	}

	device->setRenderTarget (0, _combinedNoiseTexture.get());
	if (device->getRenderTarget(1))
	{
		device->setRenderTarget (1, 0);
	}

	device->setViewport (0, 0, 0, _combinedNoiseTexture->getWidth(), _combinedNoiseTexture->getHeight());
	_materialCombine->getParameterTable()->setTexture ("t", _baseNoiseTexture[_noiseTextureIndex].get());

	float vertices[4 * 5] = {
		-1.f, -1.f, 1.f, 0.f, 1.f,
		 1.f, -1.f, 1.f, 1.f, 1.f,
		 1.f,  1.f, 1.f, 1.f, 0.f,
	    -1.f,  1.f, 1.f, 0.f, 0.f
	};

	unsigned short indices[4] = {
		0, 1, 2, 3
	};

	unsigned numPasses = _materialCombine->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (_materialCombine->beginPass (device, pass))
		{
			device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_FAN, 4, 4, ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2, 5 * sizeof(float), vertices, indices);
			_materialCombine->endPass (device, pass);
		}
	}
	_materialCombine->end (device);

	return true;
}

void ATOM_CloudPlane::updateDeformation (ATOM_RenderDevice *device, ATOM_Camera *camera, const ATOM_Vector3f &sunDir, float coverage)
{
	ATOM_STACK_TRACE(ATOM_CloudPlane::updateDeformation);

	// update scroll offset
	unsigned stamp = ATOM_APP->getFrameStamp().frameStamp;
	unsigned tick = ATOM_APP->getFrameStamp().currentTick;

	if (_lastDrawStamp == 0)
	{
		_lastDrawStamp = stamp;
		_lastDrawTick = tick;
		_offset.set(0,0);
	}

	bool updateColor = false;

	if (_drawMode == DM_SM3 && _materialShade)
	{
		if (_lastColorUpdateStamp == 0 || !_colorTexture || _colorTexture->isContentLost())
		{
			updateColor = true;
		}
		else if (_lastColorUpdateStamp != stamp && tick > _lastColorUpdateTick + 100)
		{
			updateColor = true;
		}
	}

	if (_lastDrawStamp != stamp)
	{
		unsigned deltaTick = tick - _lastDrawTick;
		if (deltaTick)
		{
			_lastDrawStamp = stamp;
			_lastDrawTick += deltaTick;

			_offset.x += deltaTick * 0.001f * _scrollVelocity.x;
			_offset.y += deltaTick * 0.001f * _scrollVelocity.y;

			ATOM_Rect2Di oldVP = device->getViewport (0);
			ATOM_AUTOREF(ATOM_Texture) oldRT0 = device->getRenderTarget(0);
			ATOM_AUTOREF(ATOM_Texture) oldRT1 = device->getRenderTarget(1);

			if (updateColor)
			{
				_lastColorUpdateStamp = stamp;
				_lastColorUpdateTick = tick;

				initColorMap (device, camera, sunDir, coverage);

				if (_colorTexture)
				{
					_colorTexture->setContentLost (false);
				}
			}

			if (_deformSpeed != 0.f || (!_combinedNoiseTexture || _combinedNoiseTexture->isContentLost()))
			{
				_z += deltaTick * 0.001f * _deformSpeed;
				initNoiseMaps (device, _z);
				if (_combinedNoiseTexture)
				{
					_combinedNoiseTexture->setContentLost (false);
				}
			}

			device->setRenderTarget (0, oldRT0.get());
			if (oldRT1)
			{
				device->setRenderTarget (1, oldRT1.get());
			}
			device->setViewport (0, oldVP);

			int b = 0;
			if (b)
			{
				_colorTexture->saveToFile ("/color.png");
			}
		}
	}
}

void ATOM_CloudPlane::setDrawMode (DrawMode mode)
{
	_drawMode = mode;
}

ATOM_CloudPlane::DrawMode ATOM_CloudPlane::getDrawMode (void) const
{
	return _drawMode;
}

void ATOM_CloudPlane::setSunHeight (float val)
{
	if (val != _sunHeight)
	{
		_lastColorUpdateStamp = 0;
		_sunHeight = val;
	}
}

float ATOM_CloudPlane::getSunHeight (void) const
{
	return _sunHeight;
}

ATOM_Vector2f ATOM_CloudPlane::computeMoonPosition (const ATOM_Vector4f &moonPos, const ATOM_Matrix4x4f &mvp)
{
	ATOM_Vector4f pos = mvp >> moonPos;
	pos /= pos.w;
	
	return ATOM_Vector2f(pos.x, pos.y);
}

void ATOM_CloudPlane::setMoonTexture (ATOM_Texture *texture)
{
	_moonTexture = texture;
}

ATOM_Texture *ATOM_CloudPlane::getMoonTexture(void) const
{
	return _moonTexture.get();
}

void ATOM_CloudPlane::setStarfieldTexture (ATOM_Texture *texture)
{
	_starfieldTexture = texture;
}

ATOM_Texture *ATOM_CloudPlane::getStarfieldTexture(void) const
{
	return _starfieldTexture.get();
}

void ATOM_CloudPlane::setStarfieldTextureRepeat (float val)
{
	_starfieldTextureRepeat = val;
}

float ATOM_CloudPlane::getStarfieldTextureRepeat (void) const
{
	return _starfieldTextureRepeat;
}

