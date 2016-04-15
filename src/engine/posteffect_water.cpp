#include "StdAfx.h"
#include "posteffect_water.h"

static const char *postEffectWaterMaterialSrcNoReflection = 
	"material																								\n"
	"{																										\n"
	"	effect E {																							\n"
	"		source \"																						\n"
	"			Texture heightTexture;																		\n"
	"			sampler2D heightMap = sampler_state															\n"
	"			{																							\n"
	"				Texture = (heightTexture);																\n"
	"				AddressU = Wrap;																		\n"
	"				AddressV = Wrap;																		\n"
	"				MipFilter = None;																		\n"
	"			};																							\n"
	"			Texture backBufferTexture;																	\n"
	"			sampler2D backBufferMap = sampler_state														\n"
	"			{																							\n"
	"				Texture = (backBufferTexture);															\n"
	"				AddressU = Wrap;																		\n"
	"				AddressV = Wrap;																		\n"
	"				MipFilter = None;																		\n"
	"				MinFilter = Point;																		\n"
	"				MagFilter = Point;																		\n"
	"			};																							\n"
	"			Texture positionTexture;																	\n"
	"			sampler positionMap = sampler_state															\n"
	"			{																							\n"
	"				Texture = (positionTexture);															\n"
	"				AddressU = Wrap;																		\n"
	"				AddressV = Wrap;																		\n"
	"				MipFilter = None;																		\n"
	"				MinFilter = Point;																		\n"
	"				MagFilter = Point;																		\n"
	"			};																							\n"
	"			Texture normalTexture;																		\n"
	"			sampler normalMap = sampler_state															\n"
	"			{																							\n"
	"				Texture = (normalTexture);																\n"
	"				AddressU = Wrap;																		\n"
	"				AddressV = Wrap;																		\n"
	"				MipFilter = None;																		\n"
	"			};																							\n"
	"			// We need this matrix to restore position in view space									\n"
	"			float viewAspect;																			\n"
	"			float invTanHalfFov;																		\n"
	"			// We need this matrix to restore position in world space									\n"
	"			float4x4 matViewInverse;																	\n"
	"			// Level at which water surface begins														\n"
	"			float waterLevel = 0.0f;																	\n"
	"			// Position of the camera																	\n"
	"			float3 cameraPos;																			\n"
	"			// How fast will colours fade out. You can also think about this							\n"
	"			// values as how clear water is. Therefore use smaller values (eg. 0.05f)					\n"
	"			// to have crystal clear water and bigger to achieve muddy water.							\n"
	"			float fadeSpeed = 0.15f;																	\n"
	"			// Timer																					\n"
	"			float timer;																				\n"
	"			// Normals scaling factor																	\n"
	"			float normalScale = 10.0f;																	\n"
	"			float detailNormalScale = 10.f;																\n"
	"			// R0 is a constant related to the index of refraction (IOR).								\n"
	"			// It should be computed on the CPU and passed to the shader.								\n"
	"			float R0 = 0.5f;																			\n"
	"			// Maximum waves amplitude																	\n"
	"			float maxAmplitude = 1.0f;																	\n"
	"			// Direction of the light																	\n"
	"			float3 lightDir = {0.0f, 1.0f, 0.0f};														\n"
	"			// Colour of the sun																		\n"
	"			float3 sunColor = {1.0f, 1.0f, 1.0f};														\n"
	"			// The smaller this value is, the more soft the transition between							\n"
	"			// shore and water. If you want hard edges use very big value.								\n"
	"			// Default is 1.0f.																			\n"
	"			float shoreHardness = 1.0f;																	\n"
	"			// This value modifies current fresnel term. If you want to weaken							\n"
	"			// reflections use bigger value. If you want to empasize them use							\n"
	"			// value smaller then 0. Default is 0.3f.													\n"
	"			float refractionStrength = 0.0f;															\n"
	"			//float refractionStrength = -0.3f;															\n"
	"			// Modifies 4 sampled normals. Increase first values to have more							\n"
	"			// smaller waves or last to have more bigger waves											\n"
	"			float4 heightModifier;																		\n"
	"			// Strength of displacement along normal.													\n"
	"			float displace = 1.7f;																		\n"
	"			// Describes at what depth foam starts to fade out and										\n"
	"			// at what it is completely invisible. The fird value is at									\n"
	"			// what height foam for waves appear (+ waterLevel).										\n"
	"			float3 foamExistence = {0.65f, 1.35f, 0.5f};												\n"
	"			float4x4 matTextureProj; 																	\n"
	"			float shininess = 32.f;																		\n"
	"			// Colour of the water surface																\n"
	"			float3 depthColour = {0.0078f, 0.5176f, 0.7f};												\n"
	"			// Colour of the water depth																\n"
	"			float3 bigDepthColour = {0.0039f, 0.00196f, 0.145f};										\n"
	"			float3 extinction = {7.0f, 30.0f, 40.0f};			// Horizontal							\n"
	"			// Water transparency along eye vector.														\n"
	"			float visibility = 4.0f;																	\n"
	"			// Increase this value to have more smaller waves.											\n"
	"			float2 scale = {0.006f, 0.006f};															\n"
	"			float refractionScale = 0.005f;																\n"
	"			// Wind force in x and z axes.																\n"
	"			float2 wind = {-0.3f, 0.7f};																\n"
	"			// VertexShader inputs																		\n"
	"			struct VertexInput																			\n"
	"			{																							\n"
	"				float3 position : POSITION;																\n"
	"				float2 uv : TEXCOORD0;																	\n"
	"			};																							\n"
	"			// VertexShader results																		\n"
	"			struct VertexOutput																			\n"
	"			{																							\n"
	"				float4 position : POSITION0;															\n"
	"				float2 texCoord : TEXCOORD0;															\n"
	"				float3 eyeDir : TEXCOORD1;																\n"
	"			};																							\n"
	"			struct PS_OUTPUT																			\n"
	"			{																							\n"
	"				float4 diffuse: COLOR0;																	\n"
	"				float4 normal: COLOR1;																	\n"
	"				float4 position: COLOR2;																\n"
	"			};																							\n"
	"			VertexOutput vs (in VertexInput IN)															\n"
	"			{																							\n"
	"				VertexOutput output;																	\n"
	"				output.position = float4(IN.position, 1.0);												\n"
	"				output.texCoord = IN.uv;																\n"
	"				output.eyeDir = float3(output.position.x * viewAspect, output.position.y, invTanHalfFov);	\n"
	"				return output;																			\n"
	"			}																							\n"
	"			// Function calculating fresnel term.														\n"
	"			// - normal - normalized normal vector														\n"
	"			// - eyeVec - normalized eye vector															\n"
	"			float fresnelTerm(float3 normal, float3 eyeVec)												\n"
	"			{																							\n"
	"				float angle = 1.0f - saturate(dot(normal, eyeVec));										\n"
	"				float fresnel = angle * angle;															\n"
	"				fresnel = fresnel * fresnel;															\n"
	"				fresnel = fresnel * angle;																\n"
	"				return saturate(fresnel * (1.0f - saturate(R0)) + R0 - refractionStrength);				\n"
	"			}																							\n"
	"			float4 ps(VertexOutput IN): COLOR0															\n"
	"			{																							\n"
	"				float3 color2 = tex2D(backBufferMap, IN.texCoord).rgb;									\n"
	"				float3 color = color2;																	\n"
	"				float4 Gbuffer = tex2D(positionMap, IN.texCoord);										\n"
	"				float3 eyePos = normalize(IN.eyeDir) * Gbuffer.w;										\n"
	"				float3 position = mul(float4(eyePos, 1.0f), matViewInverse).xyz;						\n"
	"				float level = waterLevel;																\n"
	"				float depth = abs(level - position.y);													\n"
	"				float3 eyeVec = position - cameraPos;													\n"	
	"				float3 eyeVecNorm = normalize(eyeVec);													\n"
	"				float heightdelta = 0.f;																\n"
	"				float2 tc = IN.texCoord + wind;															\n"
	"				heightdelta += tex2D(heightMap, IN.texCoord * heightModifier.x + wind).r * heightModifier.z;	\n"
	"				heightdelta += tex2D(heightMap, IN.texCoord * heightModifier.y + wind).r * heightModifier.w;	\n"
	"				float height1 = tex2D(heightMap, tc + float2(-1.f/128.f, 0.f)).r;						\n"
	"				float height2 = tex2D(heightMap, tc + float2( 1.f/128.f, 0.f)).r;						\n"
	"				float height3 = tex2D(heightMap, tc + float2(0.f, -1.f/128.f)).r;						\n"
	"				float height4 = tex2D(heightMap, tc + float2(0.f,  1.f/128.f)).r;						\n"
	"				float3 normal = normalize(float3(height1 - height2 + heightdelta, normalScale, height3 - height4 + heightdelta));		\n"
	"				float2 texCoordRefr = IN.texCoord + 0.1 * normal.xz;								\n"
	"																												\n"
	"				float3 reflect = depthColour.rgb;									\n"
	"				float3 refraction = tex2D(backBufferMap, texCoordRefr).rgb;								\n"
	"				float fresnel = fresnelTerm(normal, eyeVecNorm);											\n"
	"																												\n"
	"				float3 depthN = depth * fadeSpeed;															\n"
	"				refraction = lerp(lerp(refraction, depthColour, saturate(depthN / visibility)),				\n"
	"									  bigDepthColour, saturate(depth / extinction));							\n"
	"																												\n"
	"				color = lerp(refraction, reflect, fresnel);																								\n"
	"				color = lerp(refraction, color, saturate(depth * 0.02));																		\n"
	"																																							\n"
	"				return float4(color, 1.0f);																													\n"
	"			}																																				\n"
	"			technique t0																																	\n"
	"			{																																				\n"
	"				pass P0																																		\n"
	"				{																																			\n"
	"					CullMode = None;																														\n"
	"					ZFunc = Always;																															\n"
	"					ZWriteEnable = False;																													\n"
	"					StencilEnable = True;																													\n"
	"					StencilMask = 0x7F;																												\n"
	"					StencilFunc = Equal;																													\n"
	"					StencilPass = Keep;																														\n"
	"					StencilRef = 2;																															\n"
	"					VertexShader = compile vs_1_1 vs();																										\n"
	"					PixelShader = compile ps_2_0 ps();																										\n"
	"				}																																			\n"
	"			}\";																																			\n"
	"	}																																						\n"
	"}																																							\n";


static const char *postEffectWaterMaterialSrc = 
	"material																								\n"
	"{																										\n"
	"	effect E {																							\n"
	"		source \"																						\n"
	"			Texture heightTexture;																		\n"
	"			sampler2D heightMap = sampler_state															\n"
	"			{																							\n"
	"				Texture = (heightTexture);																\n"
	"				AddressU = Wrap;																		\n"
	"				AddressV = Wrap;																		\n"
	"				MipFilter = None;																		\n"
	"			};																							\n"
	"			Texture backBufferTexture;																	\n"
	"			sampler2D backBufferMap = sampler_state														\n"
	"			{																							\n"
	"				Texture = (backBufferTexture);															\n"
	"				AddressU = Wrap;																		\n"
	"				AddressV = Wrap;																		\n"
	"				MipFilter = None;																		\n"
	"				MinFilter = Point;																		\n"
	"				MagFilter = Point;																		\n"
	"			};																							\n"
	"			Texture positionTexture;																	\n"
	"			sampler positionMap = sampler_state															\n"
	"			{																							\n"
	"				Texture = (positionTexture);															\n"
	"				AddressU = Wrap;																		\n"
	"				AddressV = Wrap;																		\n"
	"				MipFilter = None;																		\n"
	"				MinFilter = Point;																		\n"
	"				MagFilter = Point;																		\n"
	"			};																							\n"
	"			Texture normalTexture;																		\n"
	"			sampler normalMap = sampler_state															\n"
	"			{																							\n"
	"				Texture = (normalTexture);																\n"
	"				AddressU = Wrap;																		\n"
	"				AddressV = Wrap;																		\n"
	"				MipFilter = None;																		\n"
	"			};																							\n"
	"			Texture foamTexture;																		\n"
	"			sampler foamMap = sampler_state																\n"
	"			{																							\n"
	"				Texture = (foamTexture);																\n"
	"				AddressU = Wrap;																		\n"
	"				AddressV = Wrap;																		\n"
	"				SRGBTEXTURE = 1;																		\n"
	"			};																							\n"
	"			Texture reflectionTexture;																	\n"
	"			sampler reflectionMap = sampler_state														\n"
	"			{																							\n"
	"				Texture = (reflectionTexture);															\n"
	"				AddressU = Wrap;																		\n"
	"				AddressV = Wrap;																		\n"
	"				MipFilter = None;																		\n"
	"			};																							\n"
	"			// We need this matrix to restore position in view space									\n"
	"			float viewAspect;																			\n"
	"			float invTanHalfFov;																		\n"
	"			// We need this matrix to restore position in world space									\n"
	"			float4x4 matViewInverse;																	\n"
	"			// Level at which water surface begins														\n"
	"			float waterLevel = 0.0f;																	\n"
	"			// Position of the camera																	\n"
	"			float3 cameraPos;																			\n"
	"			// How fast will colours fade out. You can also think about this							\n"
	"			// values as how clear water is. Therefore use smaller values (eg. 0.05f)					\n"
	"			// to have crystal clear water and bigger to achieve muddy water.							\n"
	"			float fadeSpeed = 0.15f;																	\n"
	"			// Timer																					\n"
	"			float timer;																				\n"
	"			// Normals scaling factor																	\n"
	"			float normalScale = 10.0f;																	\n"
	"			float detailNormalScale = 10.f;																\n"
	"			// R0 is a constant related to the index of refraction (IOR).								\n"
	"			// It should be computed on the CPU and passed to the shader.								\n"
	"			float R0 = 0.5f;																			\n"
	"			// Maximum waves amplitude																	\n"
	"			float maxAmplitude = 1.0f;																	\n"
	"			// Direction of the light																	\n"
	"			float3 lightDir = {0.0f, 1.0f, 0.0f};														\n"
	"			// Colour of the sun																		\n"
	"			float3 sunColor = {1.0f, 1.0f, 1.0f};														\n"
	"			// The smaller this value is, the more soft the transition between							\n"
	"			// shore and water. If you want hard edges use very big value.								\n"
	"			// Default is 1.0f.																			\n"
	"			float shoreHardness = 1.0f;																	\n"
	"			// This value modifies current fresnel term. If you want to weaken							\n"
	"			// reflections use bigger value. If you want to empasize them use							\n"
	"			// value smaller then 0. Default is 0.3f.													\n"
	"			float refractionStrength = 0.0f;															\n"
	"			//float refractionStrength = -0.3f;															\n"
	"			// Modifies 4 sampled normals. Increase first values to have more							\n"
	"			// smaller waves or last to have more bigger waves											\n"
	"			float4 heightModifier;																		\n"
	"			// Strength of displacement along normal.													\n"
	"			float displace = 1.7f;																		\n"
	"			// Describes at what depth foam starts to fade out and										\n"
	"			// at what it is completely invisible. The fird value is at									\n"
	"			// what height foam for waves appear (+ waterLevel).										\n"
	"			float3 foamExistence = {0.65f, 1.35f, 0.5f};												\n"
	"			float4x4 matTextureProj; 																	\n"
	"			float shininess = 32.f;																		\n"
	"			// Colour of the water surface																\n"
	"			float3 depthColour = {0.0078f, 0.5176f, 0.7f};												\n"
	"			// Colour of the water depth																\n"
	"			float3 bigDepthColour = {0.0039f, 0.00196f, 0.145f};										\n"
	"			float3 extinction = {7.0f, 30.0f, 40.0f};			// Horizontal							\n"
	"			// Water transparency along eye vector.														\n"
	"			float visibility = 4.0f;																	\n"
	"			// Increase this value to have more smaller waves.											\n"
	"			float2 scale = {0.006f, 0.006f};															\n"
	"			float refractionScale = 0.005f;																\n"
	"			// Wind force in x and z axes.																\n"
	"			float2 wind = {-0.3f, 0.7f};																\n"
	"			// VertexShader inputs																		\n"
	"			struct VertexInput																			\n"
	"			{																							\n"
	"				float3 position : POSITION;																\n"
	"				float2 uv : TEXCOORD0;																	\n"
	"			};																							\n"
	"			// VertexShader results																		\n"
	"			struct VertexOutput																			\n"
	"			{																							\n"
	"				float4 position : POSITION0;															\n"
	"				float2 texCoord : TEXCOORD0;															\n"
	"				float3 eyeDir : TEXCOORD1;																\n"
	"			};																							\n"
	"			struct PS_OUTPUT																			\n"
	"			{																							\n"
	"				float4 diffuse: COLOR0;																	\n"
	"				float4 normal: COLOR1;																	\n"
	"				float4 position: COLOR2;																\n"
	"			};																							\n"
	"			VertexOutput vs (in VertexInput IN)															\n"
	"			{																							\n"
	"				VertexOutput output;																	\n"
	"				output.position = float4(IN.position, 1.0);												\n"
	"				output.texCoord = IN.uv;																\n"
	"				output.eyeDir = float3(output.position.x * viewAspect, output.position.y, invTanHalfFov);	\n"
	"				return output;																			\n"
	"			}																							\n"
	"			float3x3 compute_tangent_frame(float3 N, float3 P, float2 UV)								\n"
	"			{																							\n"
	"				float3 dp1 = ddx(P);																	\n"
	"				float3 dp2 = ddy(P);																	\n"
	"				float2 duv1 = ddx(UV);																	\n"
	"				float2 duv2 = ddy(UV);																	\n"
	"				float3x3 M = float3x3(dp1, dp2, cross(dp1, dp2));										\n"
	"				float2x3 inverseM = float2x3( cross( M[1], M[2] ), cross( M[2], M[0] ) );				\n"
	"				float3 T = mul(float2(duv1.x, duv2.x), inverseM);										\n"
	"				float3 B = mul(float2(duv1.y, duv2.y), inverseM);										\n"
	"				return float3x3(normalize(T), normalize(B), N);											\n"
	"			}																							\n"
	"			// Function calculating fresnel term.														\n"
	"			// - normal - normalized normal vector														\n"
	"			// - eyeVec - normalized eye vector															\n"
	"			float fresnelTerm(float3 normal, float3 eyeVec)												\n"
	"			{																							\n"
	"				float angle = 1.0f - saturate(dot(normal, eyeVec));										\n"
	"				float fresnel = angle * angle;															\n"
	"				fresnel = fresnel * fresnel;															\n"
	"				fresnel = fresnel * angle;																\n"
	"				return saturate(fresnel * (1.0f - saturate(R0)) + R0 - refractionStrength);				\n"
	"			}																							\n"
	"			float4 ps(VertexOutput IN): COLOR0															\n"
	"			{																							\n"
	"				float3 color2 = tex2D(backBufferMap, IN.texCoord).rgb;									\n"
	"				float3 color = color2;																	\n"
	"				float4 Gbuffer = tex2D(positionMap, IN.texCoord);										\n"
	"				float3 eyePos = normalize(IN.eyeDir) * Gbuffer.w;										\n"
	"				float3 position = mul(float4(eyePos, 1.0f), matViewInverse).xyz;						\n"
	"				float level = waterLevel;																		\n"
	"				float depth = 0.0f;																				\n"
	"				// If we are underwater let's leave out complex computations									\n"
	"				if(level >= cameraPos.y)																		\n"
	"					return float4(color2, 1.0f);																\n"
	"				if(position.y <= level + maxAmplitude)															\n"
	"				{																								\n"
	"					float3 eyeVec = position - cameraPos;														\n"	
	"					float diff = level - position.y;															\n"
	"					float cameraDepth = cameraPos.y - position.y;												\n"
	"					// Find intersection with water surface														\n"
	"					float3 eyeVecNorm = normalize(eyeVec);														\n"
	"					float t = (level - cameraPos.y) / eyeVecNorm.y;												\n"
	"					float3 surfacePoint = cameraPos + eyeVecNorm * t;											\n"
	"					eyeVecNorm = normalize(eyeVecNorm);															\n"
	"					float2 texCoord;																			\n"
	"					float2 movement = timer * 0.000005f * wind;													\n"
	"					float2 step = eyeVecNorm.xz * 0.1f * scale + movement;										\n"
	"					for(int i = 0; i < 10; ++i)																	\n"
	"					{																							\n"
	"						texCoord = surfacePoint.xz * scale + step;												\n"
	"						float bias = tex2D(heightMap, texCoord).r;												\n"
	"						bias *= 0.1f;																			\n"
	"						level += bias * maxAmplitude;															\n"
	"						t = (level - cameraPos.y) / eyeVecNorm.y;												\n"
	"						surfacePoint = cameraPos + eyeVecNorm * t;												\n"
	"					}																							\n"
	"					depth = length(position - surfacePoint);													\n"
	"					float depth2 = surfacePoint.y - position.y;													\n"
	"					eyeVecNorm = normalize(cameraPos - surfacePoint);											\n"
	"					float heightdelta = 0.f;																	\n"
	"					heightdelta += tex2D(heightMap, texCoord * heightModifier.x + wind).r * heightModifier.z;	\n"
	"					heightdelta += tex2D(heightMap, texCoord * heightModifier.y + wind).r * heightModifier.w;	\n"
	"					float height1 = tex2D(heightMap, texCoord + float2(-1.f/128.f, 0.f)).r;						\n"
	"					float height2 = tex2D(heightMap, texCoord + float2( 1.f/128.f, 0.f)).r;						\n"
	"					float height3 = tex2D(heightMap, texCoord + float2(0.f, -1.f/128.f)).r;						\n"
	"					float height4 = tex2D(heightMap, texCoord + float2(0.f,  1.f/128.f)).r;						\n"
	"					float3 normal = normalize(float3(height1 - height2 + heightdelta, normalScale, height3 - height4 + heightdelta));		\n"
	"																												\n"
	"					float3 waterPosition = surfacePoint.xyz;													\n"
	"					waterPosition.y -= (level - waterLevel);													\n"
	"					float4 texCoordProj = mul(float4(waterPosition, 1.0f), matTextureProj);						\n"
	"					texCoordProj.xz += displace * normal.xz;													\n"
	"					texCoordProj /= texCoordProj.w;																\n"
	"																												\n"
	"					float3 reflect = tex2D(reflectionMap, texCoordProj.xy).rgb;									\n"
	"					float2 clipPos = (texCoordProj.xy * 2 - 1) * float2(1.0, -1.0);								\n"
	"					float3 eyedir = float3(clipPos.x * viewAspect, clipPos.y, invTanHalfFov);					\n"
	"					float3 eyepos = normalize(eyedir) * tex2D(positionMap, texCoordProj.xy).w;					\n"
	"					float3 refraction = tex2D(backBufferMap, texCoordProj.xy).rgb;								\n"
	"					if(mul(float4(eyepos, 1.0f), matViewInverse).y > level)										\n"
	"						refraction = color2;																	\n"
	"																												\n"
	"					float fresnel = fresnelTerm(normal, eyeVecNorm);											\n"
	"																												\n"
	"					float3 depthN = depth * fadeSpeed;															\n"
	"					refraction = lerp(lerp(refraction, depthColour, saturate(depthN / visibility)),				\n"
	"									  bigDepthColour, saturate(depth2 / extinction));							\n"
	"																												\n"
	"					texCoord = (surfacePoint.xz + eyeVecNorm.xz * 0.1) * 0.05 + timer * 0.00001f * wind + sin(timer * 0.001 + position.x) * 0.005;			\n"
	"					float4 d4 = float4(depth2, depth2, depth2, depth2);																						\n"
	"					float4 foamNear = float4(-1000.0, 0.0, foamExistence.x, foamExistence.y);																\n"
	"					float4 foamFar = float4(0.0, foamExistence.x, foamExistence.y, 1000.0);																	\n"
	"					float4 mask0 = d4 >= foamNear;																											\n"
	"					float4 mask1 = d4 < foamFar;																											\n"
	"					float foam = dot(mask0 * mask1, float4(0.0, 1.0, (foamExistence.y - depth2)/(foamExistence.y - foamExistence.x), 0.0));					\n"
	"					if(maxAmplitude - foamExistence.z > 0.0001f)																							\n"
	"					{																																		\n"
	"						foam += saturate((level - (waterLevel + foamExistence.z)) / (maxAmplitude - foamExistence.z));										\n"
	"					}																																		\n"
	"																																							\n"
	"					half3 specular = 0.0f;																													\n"
	"																																							\n"
	"					half3 mirrorEye = (2.0f * dot(eyeVecNorm, normal) * normal - eyeVecNorm);																\n"
	"					half dotSpec = saturate(dot(mirrorEye.xyz, -lightDir) * 0.5f + 0.5f);																	\n"
	"					specular = (1.0f - fresnel) * pow(dotSpec, shininess) * sunColor;																		\n"
	//"					specular += specular * 25 * saturate(shininess - 0.05f) * sunColor;																		\n"
	"																																							\n"
	"					color = lerp(refraction, reflect, fresnel);																								\n"
	"					color = saturate(color + max(specular, tex2D(foamMap, texCoord) * foam * sunColor));													\n"
	"																																							\n"
	"					color = lerp(refraction, color, saturate(depth * shoreHardness));																		\n"
	"				}																																			\n"
	"																																							\n"
	"				if(position.y > level)																														\n"
	"					color = color2;																															\n"
	"																																							\n"
	"				return float4(color, 1.0f);																													\n"
	"			}																																				\n"
	"			technique t0																																	\n"
	"			{																																				\n"
	"				pass P0																																		\n"
	"				{																																			\n"
	"					CullMode = None;																														\n"
	"					ZFunc = Always;																															\n"
	"					ZWriteEnable = False;																													\n"
	"					StencilEnable = True;																													\n"
	"					StencilMask = 0x7F;																												\n"
	"					StencilFunc = Equal;																													\n"
	"					StencilPass = Keep;																														\n"
	"					StencilRef = 2;																															\n"
	"					VertexShader = compile vs_3_0 vs();																										\n"
	"					PixelShader = compile ps_3_0 ps();																										\n"
	"				}																																			\n"
	"			}\";																																			\n"
	"	}																																						\n"
	"}																																							\n";

ATOM_AUTOPTR(ATOM_Material) PostEffectWater::_material;
ATOM_AUTOREF(ATOM_Texture) PostEffectWater::_reflectionTexture;
ATOM_AUTOREF(ATOM_Texture) PostEffectWater::_foamTexture;
ATOM_AUTOREF(ATOM_Texture) PostEffectWater::_normalTexture;
ATOM_AUTOREF(ATOM_Texture) PostEffectWater::_heightTexture;
bool PostEffectWater::_initialized = false;
bool PostEffectWater::_requireRender = false;
ATOM_VECTOR<ATOM_Water*> PostEffectWater::_waterList;
float PostEffectWater::_waterLevel = 0.f;
float PostEffectWater::_waveHeight = 1.f;
ATOM_Matrix4x4f PostEffectWater::_matTextureProj;
ATOM_Vector4f PostEffectWater::_lightDir = ATOM_Vector4f(0.f, 1.f, 0.f, 0.f);
float PostEffectWater::_fresnel = 0.f;
ATOM_Vector2f PostEffectWater::_waveScale(0.006f, 0.006f);
float PostEffectWater::_shoreHardness = 1.f;
float PostEffectWater::_normalScale = 10.f;
float PostEffectWater::_fadeSpeed = 0.15f;
float PostEffectWater::_displace = 1.7f;
float PostEffectWater::_visibility = 4.f;
float PostEffectWater::_sunScale = 3.f;
float PostEffectWater::_shininess = 64.f;
ATOM_Vector2f PostEffectWater::_wind(-0.3f, 0.7f);
ATOM_Vector4f PostEffectWater::_surfaceColor(0.0078f, 0.5176f, 0.7f, 1.f);
ATOM_Vector4f PostEffectWater::_depthColor(0.0039f, 0.00196f, 0.145f, 1.f);
ATOM_Vector4f PostEffectWater::_extinction(1.0f, 30.0f/7.f, 40.0f/7.f, 1.f);
ATOM_Vector4f PostEffectWater::_heightModifier(16.f, 8.f, 0.1f, 0.2f);
ATOM_Vector4f PostEffectWater::_foamExistence(0.65f, 1.35f, 0.5f, 1.0f);
ATOM_Vector4f PostEffectWater::_specularColor(1.f, 1.f, 1.f, 1.f);

static void drawScreenQuad (ATOM_RenderDevice *device, unsigned w, unsigned h)
{
	ATOM_STACK_TRACE(drawScreenQuad);

	device->pushMatrix (ATOM_MATRIXMODE_WORLD);
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

	device->popMatrix (ATOM_MATRIXMODE_WORLD);
	device->popMatrix (ATOM_MATRIXMODE_VIEW);
	device->popMatrix (ATOM_MATRIXMODE_PROJECTION);
}

bool PostEffectWater::draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Texture *colorMap, ATOM_Texture *positionMap)
{
#if 1
	return false;
#else
	ATOM_STACK_TRACE(PostEffectWater::draw);

	_requireRender = false;

	if (_waterList.size() == 0)
	{
		return true;
	}

	for (unsigned i = 0; i < _waterList.size(); ++i)
	{
		_waterList[i]->drawStencil (device);
	}
	_waterList.resize(0);

	bool drawReflection = ATOM_RenderSettings::getWaterQuality() == ATOM_RenderSettings::QUALITY_HIGH  && ATOM_RenderSettings::getRenderCaps().shaderModel >= 3 && ATOM_RenderSettings::getRenderCaps().supportHDRDeferredShading;

	if (!_initialized)
	{
		_initialized = true;
		_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/water_posteffect.mat");
		if (!_material)
		{
			_material = 0;
		}
	}

	if (!_material)
	{
		return false;
	}

	const ATOM_Matrix4x4f &viewMatrix = camera->getViewMatrix();
	const ATOM_Matrix4x4f &invViewMatrix = camera->getInvViewMatrix();

	ATOM_Vector4f sunColor = dynamic_cast<ATOM_SDLScene*>(ATOM_Scene::getCurrentScene())->getLight()->getColor4f();
	sunColor.w = 0.f;
	float waterCol = ATOM_saturate (sunColor.getLength() / _sunScale);
	ATOM_Vector4f surfaceColor = _surfaceColor * waterCol;
	surfaceColor.w = 1.f;
	ATOM_Vector4f depthColor = _depthColor * waterCol;
	depthColor.w = 1.f;

	_material->getParameterTable()->setTexture ("heightTexture", _heightTexture.get());
	_material->getParameterTable()->setTexture ("normalTexture", _normalTexture.get());
	_material->getParameterTable()->setTexture ("backBufferTexture", colorMap);
	_material->getParameterTable()->setTexture ("positionTexture", positionMap);
	_material->getParameterTable()->setTexture ("foamTexture", _foamTexture.get());
	_material->getParameterTable()->setTexture ("reflectionTexture", _reflectionTexture.get());
	_material->getParameterTable()->setFloat ("viewAspect", camera->getAspect());
	_material->getParameterTable()->setFloat ("invTanHalfFov", camera->getInvTanHalfFovy());
	_material->getParameterTable()->setMatrix44 ("matViewInverse", viewMatrix);
	_material->getParameterTable()->setVector ("cameraPos", ATOM_Vector4f(viewMatrix.m30, viewMatrix.m31, viewMatrix.m32, 1.f));
	_material->getParameterTable()->setMatrix44 ("matTextureProj", _matTextureProj);
	_material->getParameterTable()->setFloat ("waterLevel", _waterLevel);
	_material->getParameterTable()->setFloat ("maxAmplitude", _waveHeight);
	_material->getParameterTable()->setFloat ("timer", ATOM_APP->getFrameStamp().currentTick);
	_material->getParameterTable()->setFloat ("refractionStrength", _fresnel);
	_material->getParameterTable()->setVector ("scale", ATOM_Vector4f(_waveScale.x, _waveScale.y, 0.f, 0.f));
	_material->getParameterTable()->setFloat ("shoreHardness", _shoreHardness);
	_material->getParameterTable()->setFloat ("normalScale", _normalScale);
	_material->getParameterTable()->setFloat ("fadeSpeed", _fadeSpeed);
	_material->getParameterTable()->setFloat ("displace", _displace);
	_material->getParameterTable()->setFloat ("visibility", _visibility);
	_material->getParameterTable()->setFloat ("shininess", _shininess);
	_material->getParameterTable()->setVector ("wind", ATOM_Vector4f(_wind.x, _wind.y, 0.f, 0.f));
	_material->getParameterTable()->setVector ("lightDir", _lightDir);
	_material->getParameterTable()->setVector ("depthColour", surfaceColor);
	_material->getParameterTable()->setVector ("bigDepthColour", depthColor);
	_material->getParameterTable()->setVector ("sunColor", dynamic_cast<ATOM_SDLScene*>(ATOM_Scene::getCurrentScene())->getLight()->getColor4f());
	_material->getParameterTable()->setVector ("extinction", _extinction);
	_material->getParameterTable()->setVector ("heightModifier", _heightModifier);
	_material->getParameterTable()->setVector ("foamExistence", _foamExistence);

	unsigned numPasses = _material->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (_material->beginPass (device, pass))
		{
			drawScreenQuad (device, positionMap->getWidth(), positionMap->getHeight());
			_material->endPass (device, pass);
		}
	}
	_material->end (device);

	return true;
#endif
}

void PostEffectWater::addWater (ATOM_Water *water)
{
	ATOM_STACK_TRACE(PostEffectWater::addWater);

	if (water)
	{
		_waterList.push_back (water);
	}
}

void PostEffectWater::setReflectionTexture (ATOM_Texture *texture)
{
	_reflectionTexture = texture;
}

void PostEffectWater::setFoamTexture (ATOM_Texture *foamTexture)
{
	_foamTexture = foamTexture;
}

void PostEffectWater::setNormalTexture (ATOM_Texture *normalTexture)
{
	_normalTexture = normalTexture;
}

void PostEffectWater::setHeightTexture (ATOM_Texture *heightTexture)
{
	_heightTexture = heightTexture;
}

void PostEffectWater::requireRender (void)
{
	_requireRender = true;
}

bool PostEffectWater::isRequireRender (void)
{
	return _requireRender;
}

void PostEffectWater::setWaterLevel (float level)
{
	_waterLevel = level;
}

void PostEffectWater::setWaveHeight (float height)
{
	_waveHeight = height;
}

void PostEffectWater::setTextureProjMatrix (const ATOM_Matrix4x4f &matrix)
{
	_matTextureProj = matrix;
}

void PostEffectWater::setLightDir (const ATOM_Vector3f &lightDir)
{
	_lightDir.set(lightDir.x, lightDir.y, lightDir.z, 0.f);
}

void PostEffectWater::setFresnel (float fresnel)
{
	_fresnel = fresnel;
}

void PostEffectWater::setWaveScale (const ATOM_Vector2f &scale)
{
	_waveScale = scale;
}

void PostEffectWater::setShoreHardness (float value)
{
	_shoreHardness = value;
}

void PostEffectWater::setNormalScale (float value)
{
	_normalScale = value;
}

void PostEffectWater::setFadeSpeed (float value)
{
	_fadeSpeed = value;
}

void PostEffectWater::setDisplace (float value)
{
	_displace = value;
}

void PostEffectWater::setVisibility (float value)
{
	_visibility = value;
}

void PostEffectWater::setWind (const ATOM_Vector2f &wind)
{
	_wind = wind;
}

void PostEffectWater::setSurfaceColor (const ATOM_Vector4f &color)
{
	_surfaceColor = color;
}

void PostEffectWater::setDepthColor (const ATOM_Vector4f &color)
{
	_depthColor = color;
}

void PostEffectWater::setSunScale (float sunScale)
{
	_sunScale = sunScale;
}

void PostEffectWater::setExtinction (const ATOM_Vector4f &extinction)
{
	_extinction = extinction;
}

void PostEffectWater::setHeightModifier (const ATOM_Vector4f &heightModifier)
{
	_heightModifier = heightModifier;
}

void PostEffectWater::setFoamExistence (const ATOM_Vector3f &foamExistence)
{
	_foamExistence.set (foamExistence.x, foamExistence.y, foamExistence.z, 1.0f);
}

void PostEffectWater::setShininess (float shininess)
{
	_shininess = shininess;
}

void PostEffectWater::setSpecularColor (const ATOM_Vector4f &color)
{
	_specularColor = color;
}

