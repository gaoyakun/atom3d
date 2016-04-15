#include "StdAfx.h"
#include "stdshadowmap_manager.h"
#include "shadowscene.h"

#if 0
static const char *strShadowMaskMaterial = 
	"material													\n"
	"{															\n"
	"	effect E {												\n"
	"		source \"											\n"
	"			float4x4 lightViewProj;							\n"
	"			float shadowDistance;							\n"
	"			float viewAspect;								\n"
	"			float invTanHalfFov;							\n"
	"			float minVariance = 0.003;						\n"
	"			float amount = 0.01;								\n"
	"			float bias = 0.0001f;																				\n"
	"			float sceneScale = 0.2f;																		\n"
	"			float lightSize = 0.05f;																		\n"
	"			texture gbufferTexture;							\n"
	"			sampler2D gbufferSampler = sampler_state		\n"
	"			{												\n"
	"				Texture = (gbufferTexture);					\n"
	"				MipFilter = None;							\n"
	"				MinFilter = Point;							\n"
	"				MagFilter = Point;							\n"
	"			};												\n"
	"			texture shadowMap;								\n"
	"			sampler2D shadowMapSampler = sampler_state		\n"
	"			{												\n"
	"				Texture = (shadowMap);						\n"
	"				BorderColor = 0xFFFFFFFF;					\n"
	"				MipFilter = None;							\n"
	"				MinFilter = Point;							\n"
	"				MagFilter = Point;							\n"
	"				AddressU = Border;							\n"
	"				AddressV = Border;							\n"
	"			};												\n"
	"			struct VS_INPUT									\n"
	"			{												\n"
	"				float3 position : POSITION;					\n"
	"				float2 uv : TEXCOORD0;						\n"
	"			};												\n"
	"															\n"
	"			struct VS_OUTPUT								\n"
	"			{												\n"
	"				float4 position : POSITION;					\n"
	"				float2 uv : TEXCOORD0;						\n"
	"				float3 eyedir : TEXCOORD1;					\n"
	"			};												\n"
	"																											\n"
	"			half findBlocker(half2 uv, half LP, half searchWidth, half numSamples)							\n"
	"			{																								\n"
	"				half stepSize = 2 * searchWidth / numSamples;												\n"
	"				uv = uv - half2(searchWidth, searchWidth);													\n"
	"		        half blockerSum = 0;																		\n"
	"				half receiver = LP;																			\n"
	"				half blockerCount = 0;																		\n"
	"				half foundBlocker = 0;																		\n"
	"				for (int i=0; i<numSamples; i++) {															\n"
	"					for (int j=0; j<numSamples; j++) {														\n"
	"						half shadMapDepth = tex2D(shadowMapSampler, uv + half2(i*stepSize,j*stepSize)).x;	\n"
	"						if (shadMapDepth < receiver) {														\n"
	"							blockerSum += shadMapDepth;														\n"
	"							blockerCount++;																	\n"
	"							foundBlocker = 1;																\n"
	"						}																					\n"
	"					}																						\n"
	"				}																							\n"
	"				half result;																				\n"
	"				if (foundBlocker == 0) {																	\n"
	"					result = 999;																			\n"
	"				}																							\n"
	"				else {																						\n"
	"					result = blockerSum / blockerCount;														\n"
	"				}																							\n"
	"				return result;																				\n"
	"			}																								\n"
	"			half estimatePenumbra(half2 uv, half LP, half blocker)											\n"
	"			{																								\n"
	"				half receiver = LP;																			\n"
	"				half penumbra = (receiver - blocker) * lightSize / blocker;									\n"
	"				return penumbra;																			\n"
	"			}																								\n"
	"			half PCF_Filter(half2 uv, half LP, half filterWidth, half numSamples)							\n"
	"			{																								\n"
	"				half stepSize = 0.5f / 2048.f;																\n"
	"				half sum = 0;																		\n"
	"				for (int i=-1; i<=1; i++) {															\n"
	"					for (int j=-1; j<=1; j++) {														\n"
	"                       half shadMapDepth = tex2D(shadowMapSampler, uv + half2(i*stepSize,j*stepSize)).x;		\n"
	"						half shad = LP < shadMapDepth;														\n"
	"						sum += shad;																		\n"
	"					}																						\n"
	"				}																							\n"
	"				return sum / 9;														\n"
	"				#if 0																						\n"
	"				float2 FilterTaps[10] =																		\n"
	"				{																							\n"
	"					// First test, still the best.															\n"
	"					{-0.84052f, -0.073954f},																\n"
	"					{-0.326235f, -0.40583f},																\n"
	"					{-0.698464f, 0.457259f},																\n"
	"					{-0.203356f, 0.6205847f},																\n"
	"					{0.96345f, -0.194353f},																	\n"
	"					{0.473434f, -0.480026f},																\n"
	"					{0.519454f, 0.767034f},																	\n"
	"					{0.185461f, -0.8945231f},																\n"
	"					{0.507351f, 0.064963f},																	\n"
	"					{-0.321932f, 0.5954349f}																\n"
	"				};																							\n"
	"				float resultDepth = 0;																		\n"
	"				for (int i=0; i<10; i++)																	\n"
	"					resultDepth += (LP < tex2D(shadowMapSampler,uv+FilterTaps[i]*(1.0/2048.0)).r) ? 0.1 : 0.0;	\n"
	"				return resultDepth;																			\n"
	"				#endif																						\n"
	"			}																								\n"
	"																											\n"
	"			half ShadowContribution (half2 lightTexCoord, half DistanceToLight)								\n"
	"			{																								\n"
	"				half samples = 5;																			\n"
	"				half shadowed = PCF_Filter(lightTexCoord, DistanceToLight, 0.0005f, samples);				\n"
	"				return shadowed;																			\n"
	"			}																								\n"
	"															\n"
	"			struct PS_OUTPUT								\n"
	"			{												\n"
	"				float4 color : COLOR0;						\n"
	"			};												\n"
	"															\n"
	"			VS_OUTPUT vs (in VS_INPUT IN)					\n"
	"			{												\n"
	"				VS_OUTPUT output;							\n"
	"				output.position = float4(IN.position, 1.0);	\n"
	"				output.uv = IN.uv;							\n"
	"				output.eyedir = float3(output.position.x * viewAspect, output.position.y, invTanHalfFov);	\n"
	"				return output;								\n"
	"			}												\n"
	"															\n"
	"			PS_OUTPUT ps (in VS_OUTPUT IN)					\n"
	"			{												\n"
	"				PS_OUTPUT output;							\n"
	"				float4 Gbuffer = tex2D(gbufferSampler, IN.uv);	\n"
	"				float3 eyePos = normalize(IN.eyedir) * Gbuffer.w;	\n"
	"				float factor = min(eyePos.z * shadowDistance, 1.0);	\n"
	"				float4 shadowTex = mul (float4(eyePos, 1.0), lightViewProj);	\n"
	"				float shadow = ShadowContribution (shadowTex.xy, shadowTex.z-bias);							\n"
	"				output.color = float4(shadow, 0.0, 0.0, 1.0);	\n"
	"				return output;								\n"
	"			}												\n"
	"															\n"
	"			technique t0									\n"
	"			{												\n"
	"				pass P0										\n"
	"				{											\n"
	"					CullMode = None;						\n"
	"					ZFunc = Always;							\n"
	"					ZWriteEnable = False;					\n"
	"					VertexShader = compile vs_3_0 vs();		\n"
	"					PixelShader = compile ps_3_0 ps();		\n"
	"				}											\n"
	"			}\";											\n"
	"	}														\n"
	"}															\n";
#endif

ATOM_StandardShadowMapManager::ATOM_StandardShadowMapManager (void)
{
	ATOM_STACK_TRACE(ATOM_StandardShadowMapManager::ATOM_StandardShadowMapManager);

	_lightDistance = 2000;
}

ATOM_StandardShadowMapManager::~ATOM_StandardShadowMapManager (void)
{
	ATOM_STACK_TRACE(ATOM_StandardShadowMapManager::~ATOM_StandardShadowMapManager);
}

void ATOM_StandardShadowMapManager::setLightDistance (float lightDistance)
{
	_lightDistance = lightDistance;
}

float ATOM_StandardShadowMapManager::getLightDistance (void) const
{
	return _lightDistance;
}

ATOM_Texture *ATOM_StandardShadowMapManager::getShadowMapTexture (void) const
{
	return _shadowMap.get();
}

bool ATOM_StandardShadowMapManager::isValid (void) const
{
	return ATOM_RenderSettings::getRenderCaps().supportShadow && ATOM_GetRenderDevice()->getBestHDRFormatR(0) != ATOM_PIXEL_FORMAT_UNKNOWN;
}

bool ATOM_StandardShadowMapManager::onRenderShadowMap (ATOM_DeferredScene *scene, ATOM_Camera *camera)
{
	ATOM_STACK_TRACE(ATOM_StandardShadowMapManager::onRenderShadowMap);

	if (!_shadowMap)
	{
		return false;
	}

	ATOM_RenderDevice *device = ATOM_GetRenderDevice();

	// Save device states
	ATOM_Rect2Di vpOld = device->getViewport (0);
	ATOM_AUTOREF(ATOM_Texture) rtOld = device->getRenderTarget(0);
	ATOM_AUTOREF(ATOM_DepthBuffer) depthOld = device->getDepthBuffer();
	ATOM_ColorARGB cc = device->getClearColor (0);
	device->pushMatrix (ATOM_MATRIXMODE_VIEW);
	device->pushMatrix (ATOM_MATRIXMODE_PROJECTION);

	// Set shadow texture as render target
	device->setRenderTarget (0, _shadowMap.get());
	device->setDepthBuffer (_shadowDepthBuffer.get());
	device->setClearColor (0, 1.f, 1.f, 1.f, 1.f);
	device->clear (true, true, true);

	// Calculate light view transform matrices
	ATOM_Matrix4x4f lightViewMatrix;
	ATOM_Matrix4x4f lightProjMatrix;
	ATOM_Vector3f origin;
	float radius;
	scene->getCamera ()->calculateOrigin (1.f, ATOM_min2(scene->getCamera()->getFarPlane(), getShadowDistance()), &origin, &radius);
	calculateLightMatrices (scene->getLight()->getDirection(), origin, radius, lightViewMatrix, lightProjMatrix);

	// Render the shadow map
	ATOM_ShadowScene shadowScene(*scene);
	shadowScene.getCamera()->setViewMatrix (lightViewMatrix);
	shadowScene.getCamera()->setProjectionMatrix (lightProjMatrix);
	shadowScene.getCamera()->setViewport (0, 0, _shadowMap->getWidth(), _shadowMap->getHeight());
	shadowScene.getCamera()->synchronise (device, 0);

	shadowScene.render (device, false);

	// Restore device states
	device->setRenderTarget (0, rtOld.get());
	device->setDepthBuffer (depthOld.get());
	device->setViewport (0, vpOld);
	device->popMatrix (ATOM_MATRIXMODE_VIEW);
	device->popMatrix (ATOM_MATRIXMODE_PROJECTION);
	device->setClearColor (0, cc.getFloatR(), cc.getFloatG(), cc.getFloatB(), cc.getFloatA());

	_lightViewProj.invertAffineFrom (lightViewMatrix);
	_lightViewProj <<= lightProjMatrix;

	float offsX = 0.5f + 0.5f / _shadowMap->getWidth();
	float offsY = 0.5f + 0.5f / _shadowMap->getHeight();
	ATOM_Matrix4x4f adj;
	adj.setRow (0, 0.5f,   0.0f,  0.0f, 0.0f);
	adj.setRow (1, 0.0f,  -0.5f,  0.0f, 0.0f);
	adj.setRow (2, 0.0f,   0.0f,  1.0f, 0.0f);
	adj.setRow (3, offsX,  offsY, 0.0f, 1.0f);
	_lightViewProj <<= adj;
	_lightViewProj >>= scene->getCamera()->getViewMatrix();

	_viewAspect = camera->getAspect ();
	_invTanHalfFovY = camera->getInvTanHalfFovy ();

	return true;
}

bool ATOM_StandardShadowMapManager::onCreateShadowMaskTexture (ATOM_Texture *shadowMaskTexture, ATOM_Texture *gbufferTexture)
{
	ATOM_STACK_TRACE(ATOM_StandardShadowMapManager::onCreateShadowMaskTexture);

	ATOM_RenderDevice *device = ATOM_GetRenderDevice();

	if (!_shadowMaskMaterial)
	{
		_shadowMaskMaterial = ATOM_MaterialManager::createMaterialFromCore(ATOM_GetRenderDevice(), "/materials/builtin/stdshadowmap.mat");
		if (!_shadowMaskMaterial)
		{
			_shadowMaskMaterial = 0;
			return false;
		}
	}

	_shadowMaskMaterial->getParameterTable()->setTexture ("gbufferTexture", gbufferTexture);
	_shadowMaskMaterial->getParameterTable()->setTexture ("shadowMap", _shadowMap.get());
	_shadowMaskMaterial->getParameterTable()->setMatrix44 ("lightViewProj", _lightViewProj);
	_shadowMaskMaterial->getParameterTable()->setFloat ("viewAspect", _viewAspect);
	_shadowMaskMaterial->getParameterTable()->setFloat ("invTanHalfFov", _invTanHalfFovY);
	float sd = 1.f / getShadowDistance();
	_shadowMaskMaterial->getParameterTable()->setFloat ("shadowDistance", sd);
	_shadowMaskMaterial->getParameterTable()->setFloat ("sceneScale", getSceneScale());
	_shadowMaskMaterial->getParameterTable()->setFloat ("lightSize", getLightSize());
	_shadowMaskMaterial->getParameterTable()->setFloat ("bias", getDepthBias());

	struct Vertex
	{
		float xyz[3];
		float uv[2];
	};

	float deltax = (float)0.5f/(float)gbufferTexture->getWidth();
	float deltay = (float)0.5f/(float)gbufferTexture->getHeight();
	const Vertex vertices[4] = {
		{ -1.f - deltax, -1.f + deltay, 0.f, 0.f, 1.f },
		{  1.f - deltax, -1.f + deltay, 0.f, 1.f, 1.f },
		{  1.f - deltax,  1.f + deltay, 0.f, 1.f, 0.f },
		{ -1.f - deltax,  1.f + deltay, 0.f, 0.f, 0.f }
	};

	const unsigned short indices[4] = {
		0, 1, 2, 3
	};

	unsigned numPasses = _shadowMaskMaterial->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (_shadowMaskMaterial->beginPass (device, pass))
		{
			device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_FAN, 4, 4, ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2, sizeof(Vertex), vertices, indices);
			_shadowMaskMaterial->endPass (device, pass);
		}
	}
	_shadowMaskMaterial->end (device);

	return true;
}

bool ATOM_StandardShadowMapManager::initializeShadowMapTexture (unsigned w, unsigned h)
{
	ATOM_STACK_TRACE(ATOM_StandardShadowMapManager::initializeShadowMapTexture);

	_shadowMap = 0;
	_shadowDepthBuffer = 0;

	int filtering;
	ATOM_PixelFormat format = ATOM_GetRenderDevice()->getBestHDRFormatR (&filtering);
	if (format == ATOM_PIXEL_FORMAT_UNKNOWN)
	{
		return false;
	}

	_shadowMap = ATOM_GetRenderDevice()->allocTexture (0, 0, w, h, format, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
	_shadowDepthBuffer = ATOM_GetRenderDevice()->allocDepthBuffer (w, h);

	if (_shadowMap && _shadowDepthBuffer)
	{
		return true;
	}

	_shadowMap = 0;
	_shadowDepthBuffer = 0;
	return false;
}

void ATOM_StandardShadowMapManager::calculateLightMatrices (const ATOM_Vector3f &lightDir, const ATOM_Vector3f &origin, float radius, ATOM_Matrix4x4f &viewMatrix, ATOM_Matrix4x4f &projMatrix) const
{
	ATOM_Vector3f lightPos = origin - lightDir * _lightDistance;

	int i = 0;
	if (ATOM_abs(lightDir.y) < ATOM_abs(lightDir.x)) i = 1;
	if (ATOM_abs(lightDir.z) < ATOM_abs(lightDir.xyz[i])) i = 2;
	ATOM_Vector3f lightUp(0.f, 0.f, 0.f);
	lightUp.xyz[i] = 1.f;
	viewMatrix.makeLookatLH (lightPos, origin, lightUp);

	projMatrix.makeOrthoLH (radius * 2.f, radius * 2.f, 0.f, _lightDistance + radius);
}

void ATOM_StandardShadowMapManager::setSceneScale (float value)
{
	_sceneScale = value;
}

float ATOM_StandardShadowMapManager::getSceneScale (void) const
{
	return _sceneScale;
}

void ATOM_StandardShadowMapManager::setLightSize (float value)
{
	_lightSize = value;
}

float ATOM_StandardShadowMapManager::getLightSize (void) const
{
	return _lightSize;
}

void ATOM_StandardShadowMapManager::setDepthBias (float value)
{
	_depthBias = value;
}

float ATOM_StandardShadowMapManager::getDepthBias (void) const
{
	return _depthBias;
}

ATOM_StandardShadowMapManager *ATOM_StandardShadowMapManager::getInstance (void)
{
	static ATOM_StandardShadowMapManager manager;
	return &manager;
}

