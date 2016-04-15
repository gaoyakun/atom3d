#include "StdAfx.h"
#include "cascadedshadowmap_manager.h"
#include "shadowscene.h"
#include "blur.h"

static const char *strShadowMaskMaterial = 
	"material																									\n"
	"{																											\n"
	"	effect E {																								\n"
	"		source \"																							\n"
	"			float4x4 lightViewProj[4];																		\n"
	"			float splitDist[4];																				\n"
	"			float viewAspect;																				\n"
	"			float invTanHalfFov;																			\n"
	"			float bias = 0.0001f;																				\n"
	"			float sceneScale = 0.2f;																		\n"
	"			float lightSize = 0.05f;																		\n"
	"			texture gbufferTexture;																			\n"
	"			sampler2D gbufferSampler = sampler_state														\n"
	"			{																								\n"
	"				Texture = (gbufferTexture);																	\n"
	"				MipFilter = None;																			\n"
	"				MinFilter = Point;																			\n"
	"				MagFilter = Point;																			\n"
	"			};																								\n"
	"			texture shadowMap;																				\n"
	"			sampler2D shadowMapSampler = sampler_state														\n"
	"			{																								\n"
	"				Texture = (shadowMap);																		\n"
	"				MipFilter = None;																			\n"
	"			};																								\n"
	"			struct VS_INPUT																					\n"
	"			{																								\n"
	"				float3 position : POSITION;																	\n"
	"				float2 uv : TEXCOORD0;																		\n"
	"			};																								\n"
	"																											\n"
	"			struct VS_OUTPUT																				\n"
	"			{																								\n"
	"				float4 position : POSITION;																	\n"
	"				float2 uv : TEXCOORD0;																		\n"
	"				float3 eyedir : TEXCOORD1;																	\n"
	"			};																								\n"
	"																											\n"
	"			struct PS_OUTPUT																				\n"
	"			{																								\n"
	"				float4 color : COLOR0;																		\n"
	"			};																								\n"
	"																											\n"
	"			VS_OUTPUT vs (in VS_INPUT IN) 																	\n"
	"			{																								\n"
	"				VS_OUTPUT output;																			\n"
	"				output.position = float4(IN.position, 1.0);													\n"
	"				output.uv = IN.uv;																			\n"
	"				output.eyedir = float3(output.position.x * viewAspect, output.position.y, invTanHalfFov);	\n"
	"				return output;																				\n"
	"			}																								\n"
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
	"				half stepSize = 2 * filterWidth / numSamples;												\n"
	"				uv = uv - half2(filterWidth,filterWidth);													\n"
	"				half sum = 0;  // sum of successful depth tests												\n"
	"				for (int i=0; i<numSamples; i++) {															\n"
	"					for (int j=0; j<numSamples; j++) {														\n"
	"						half shadMapDepth = 0;																\n"
	"                       shadMapDepth = tex2D(shadowMapSampler, uv + half2(i*stepSize,j*stepSize)).x;		\n"
	"						half shad = LP < shadMapDepth;														\n"
	"						sum += shad;																		\n"
	"					}																						\n"
	"				}																							\n"
	"				return sum / (numSamples*numSamples);														\n"
	"			}																								\n"
	"																											\n"
	"			half ShadowContribution (half2 lightTexCoord, half DistanceToLight)								\n"
	"			{																								\n"
	"				half searchSamples = 6;																		\n"
	"				half blocker = findBlocker (lightTexCoord, DistanceToLight, sceneScale * lightSize / DistanceToLight, searchSamples);	\n"
	"				if (blocker > 998)																			\n"
	"					return 1.0;																				\n"
	"				half penumbra = estimatePenumbra (lightTexCoord, DistanceToLight, blocker);					\n"
	"				half samples = 8;																			\n"
	"				half shadowed = PCF_Filter(lightTexCoord, DistanceToLight, penumbra, samples);				\n"
	"				return shadowed;																			\n"
	"			}																								\n"
	"																											\n"
	"			PS_OUTPUT ps (in VS_OUTPUT IN)																	\n"
	"			{																								\n"
	"				PS_OUTPUT output;																			\n"
	"				half4 Gbuffer = tex2D(gbufferSampler, IN.uv);												\n"
	"				half4 eyePos = float4(normalize(IN.eyedir) * Gbuffer.w, 1.0);								\n"
	"				half shadow = 1.0;																			\n"
	"				if (eyePos.z < splitDist[3])																\n"
	"				{																							\n"
	"					half4 nearVec = float4(0.f, splitDist[0], splitDist[1], splitDist[2]);					\n"
	"					half4 farVec = float4(splitDist[0], splitDist[1], splitDist[2], splitDist[3]);			\n"
	"					half4 mask0 = eyePos.zzzz >= nearVec.xyzw;												\n"
	"					half4 mask1 = eyePos.zzzz < farVec.xyzw;												\n"
	"					half index = dot(mask0 * mask1, float4(0.f, 1.f, 2.f, 3.f));							\n"
	"					half4 shadowTex = mul(eyePos, lightViewProj[index]);									\n"
	"					shadow = ShadowContribution (shadowTex.xy, shadowTex.z - bias);							\n"
	"				}																							\n"
	"				output.color = half4(shadow, 0.0, 0.0, 1.0);												\n"
	"				return output;																				\n"
	"			}																								\n"
	"																											\n"
	"			technique t0																					\n"
	"			{																								\n"
	"				pass P0																						\n"
	"				{																							\n"
	"					CullMode = None;																		\n"
	"					ZFunc = Always;																			\n"
	"					ZWriteEnable = False;																	\n"
	"					VertexShader = compile vs_3_0 vs();														\n"
	"					PixelShader = compile ps_3_0 ps();														\n"
	"				}																							\n"
	"			}\";																							\n"
	"	}																										\n"
	"}																											\n";


ATOM_CascadedShadowMapManager::ATOM_CascadedShadowMapManager (void)
{
	ATOM_STACK_TRACE(ATOM_CascadedShadowMapManager::ATOM_CascadedShadowMapManager);

	_lightDistance = 3000;
	_splitLambda = 0.3f;
}

ATOM_CascadedShadowMapManager::~ATOM_CascadedShadowMapManager (void)
{
	ATOM_STACK_TRACE(ATOM_CascadedShadowMapManager::~ATOM_CascadedShadowMapManager);
}

void ATOM_CascadedShadowMapManager::setLightDistance (float lightDistance)
{
	_lightDistance = lightDistance;
}

float ATOM_CascadedShadowMapManager::getLightDistance (void) const
{
	return _lightDistance;
}

void ATOM_CascadedShadowMapManager::setSplitLambda (float lambda)
{
	_splitLambda = lambda;
}

float ATOM_CascadedShadowMapManager::getSplitLambda (void) const
{
	return _splitLambda;
}

ATOM_Texture *ATOM_CascadedShadowMapManager::getShadowMapTexture (void) const
{
	return _shadowMap.get();
}

bool ATOM_CascadedShadowMapManager::isValid (void) const
{
	return ATOM_RenderSettings::getRenderCaps().supportShadow && ATOM_RenderSettings::getRenderCaps().shaderModel >= 3 && ATOM_GetRenderDevice()->getBestHDRFormatR(0) != ATOM_PIXEL_FORMAT_UNKNOWN;
}

bool ATOM_CascadedShadowMapManager::onRenderShadowMap (ATOM_DeferredScene *scene, ATOM_Camera *camera)
{
	ATOM_STACK_TRACE(ATOM_CascadedShadowMapManager::onRenderShadowMap);

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

	calculateSplitDistances (camera);

	for (unsigned i = 0; i < 4; ++i)
	{
		// Calculate light view transform matrices
		ATOM_Matrix4x4f lightViewMatrix;
		ATOM_Matrix4x4f lightProjMatrix;
		calculateLightMatrices (i, scene->getCamera(), scene->getPrecalculatedBoundingBox(), scene->getLight()->getDirection(), lightViewMatrix, lightProjMatrix);

		const unsigned w = _shadowMap->getWidth() / 2;
		const unsigned h = _shadowMap->getHeight() / 2;
		int row = i % 2;
		int col = i / 2;

		// Render the shadow map
		ATOM_ShadowScene shadowScene(*scene);
		shadowScene.getCamera()->setViewMatrix (lightViewMatrix);
		shadowScene.getCamera()->setProjectionMatrix (lightProjMatrix);
		shadowScene.getCamera()->setViewport (col * w, row * h, w, h);
		shadowScene.getCamera()->synchronise (device, 0);

		shadowScene.render (device, false);

		ATOM_Matrix4x4f tcAdj;
		tcAdj.setRow (0, 0.5f,        0.0f,       0.0f, 0.0f);
		tcAdj.setRow (1, 0.0f,        0.5f,       0.0f, 0.0f);
		tcAdj.setRow (2, 0.0f,        0.0f,       1.0f, 0.0f);
		tcAdj.setRow (3, col * 0.5f,  row * 0.5f, 0.0f, 1.0f);
		_lightViewProj[i] <<= tcAdj;
	}


	// Restore device states
	device->setRenderTarget (0, rtOld.get());
	device->setDepthBuffer (depthOld.get());
	device->setViewport (0, vpOld);
	device->popMatrix (ATOM_MATRIXMODE_VIEW);
	device->popMatrix (ATOM_MATRIXMODE_PROJECTION);
	device->setClearColor (0, cc.getFloatR(), cc.getFloatG(), cc.getFloatB(), cc.getFloatA());

	int xx = 0;
	if (xx)
	{
		_shadowMap->saveToFile ("/xxxx.dds");
	}
	_viewAspect = camera->getAspect ();
	_invTanHalfFovY = camera->getInvTanHalfFovy ();


	return true;
}

bool ATOM_CascadedShadowMapManager::onCreateShadowMaskTexture (ATOM_Texture *shadowMaskTexture, ATOM_Texture *gbufferTexture)
{
	ATOM_STACK_TRACE(ATOM_CascadedShadowMapManager::onCreateShadowMaskTexture);

	ATOM_RenderDevice *device = ATOM_GetRenderDevice();

	if (!_shadowMaskMaterial)
	{
		_shadowMaskMaterial = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/csm.mat");
		if (!_shadowMaskMaterial)
		{
			return false;
		}
	}

	_shadowMaskMaterial->getParameterTable()->setTexture ("gbufferTexture", gbufferTexture);
	_shadowMaskMaterial->getParameterTable()->setTexture ("shadowMap", _shadowMap.get());
	_shadowMaskMaterial->getParameterTable()->setMatrix44Array ("lightViewProj", _lightViewProj, 4);
	_shadowMaskMaterial->getParameterTable()->setFloat ("viewAspect", _viewAspect);
	_shadowMaskMaterial->getParameterTable()->setFloat ("invTanHalfFov", _invTanHalfFovY);
	_shadowMaskMaterial->getParameterTable()->setVector ("splitDist", ATOM_Vector4f(_splitDistances[1], _splitDistances[2], _splitDistances[3], _splitDistances[4]));
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

bool ATOM_CascadedShadowMapManager::initializeShadowMapTexture (unsigned w, unsigned h)
{
	ATOM_STACK_TRACE(ATOM_CascadedShadowMapManager::initializeShadowMapTexture);

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

void ATOM_CascadedShadowMapManager::calculateLightMatrices (int split, ATOM_Camera *camera, const ATOM_BBox &sceneBBox, const ATOM_Vector3f &lightDir, ATOM_Matrix4x4f &viewMatrix, ATOM_Matrix4x4f &projMatrix)
{
	ATOM_STACK_TRACE(ATOM_CascadedShadowMapManager::calculateLightMatrices);

	ATOM_Vector3f corners[9];
	calculateFrustumCorners (split, camera, corners);

	float l = (sceneBBox.getMax() - sceneBBox.getMin()).getLength ();
	ATOM_Vector3f lightEye = corners[8] - lightDir * l;
	ATOM_Ray ray(lightEye, lightDir);
	float t;
	if (ray.intersectionTestEx (sceneBBox, t) && t > 0.f)
	{
		l -= t;
		l += 1000.f;
		lightEye = corners[8] - lightDir * l;
	}

	int i = 0;
	if (ATOM_abs(lightDir.y) < ATOM_abs(lightDir.x)) i = 1;
	if (ATOM_abs(lightDir.z) < ATOM_abs(lightDir.xyz[i])) i = 2;
	ATOM_Vector3f lightUp(0.f, 0.f, 0.f);
	lightUp.xyz[i] = 1.f;
	viewMatrix.makeLookatLH (lightEye, corners[8], lightUp);
	viewMatrix.invertAffine ();

	float maxX = 0.f;
	float maxY = 0.f;
	float maxZ = 0.f;
	for (unsigned i = 0; i < 8; ++i)
	{
		corners[i] <<= viewMatrix;
		float x = ATOM_abs(corners[i].x);
		if (x > maxX) maxX = x;
		float y = ATOM_abs(corners[i].y);
		if (y > maxY) maxY = y;
		if (corners[i].z > maxZ) maxZ = corners[i].z;
	}

	projMatrix.makeOrthoLH (maxX * 2.f + 10.f, maxY * 2.f + 10.f, 0.f, maxZ + 100.f);

	float shadowMapW = getShadowMapWidth();
	float shadowMapH = getShadowMapHeight();

#if 1
	viewMatrix.makeLookatLH (ATOM_Vector3f(0.f, 0.f, 0.f), lightDir, lightUp);
	viewMatrix.transpose ();

	ATOM_Vector3f basePos = viewMatrix >> corners[8];
	float texelSizeX = (maxX * 2.f + 10.f) / shadowMapW;
	float texelSizeY = (maxY * 2.f + 10.f) / shadowMapH;
	basePos.x = ATOM_floor (basePos.x / texelSizeX) * texelSizeX;
	basePos.y = ATOM_floor (basePos.y / texelSizeY) * texelSizeY;
	viewMatrix.transpose ();
	corners[8] = viewMatrix >> basePos;
	viewMatrix.makeLookatLH (corners[8] - lightDir * l, corners[8], lightUp);
#else
	viewMatrix.invertAffine ();
#endif

	maxX = 0.f;
	maxY = 0.f;
	float minX = FLT_MAX;
	float minY = FLT_MAX;
	for (unsigned i = 0; i < 8; ++i)
	{
		ATOM_Vector4f v (projMatrix >> corners[i]);
		v /= v.w;
		if (v.x > maxX) maxX = v.x;
		if (v.x < minX) minX = v.x;
		if (v.y > maxY) maxY = v.y;
		if (v.y < minY) minY = v.y;
	}
	maxX = ATOM_min2(1.f, ATOM_max2(-1.f, maxX));
	minX = ATOM_min2(1.f, ATOM_max2(-1.f, minX));
	maxY = ATOM_min2(1.f, ATOM_max2(-1.f, maxY));
	minY = ATOM_min2(1.f, ATOM_max2(-1.f, minY));

	float scaleX = 2.f / (maxX - minX);
	float scaleY = 2.f / (maxY - minY);
	float offsetX = -0.5f * (maxX + minX) * scaleX;
	float offsetY = -0.5f * (maxY + minY) * scaleY;
	ATOM_Matrix4x4f cropMatrix;
	cropMatrix.setRow(0, scaleX, 0, 0, 0);
	cropMatrix.setRow(1, 0, scaleY, 0, 0);
	cropMatrix.setRow(2, 0, 0, 1, 0);
	cropMatrix.setRow(3, offsetX, offsetY, 0, 1);
	projMatrix <<= cropMatrix;

	ATOM_Matrix4x4f invLightViewMatrix;
	invLightViewMatrix.invertAffineFrom (viewMatrix);

	ATOM_Matrix4x4f adj;
	float offsX = 0.5f + 0.5f / shadowMapW;
	float offsY = 0.5f + 0.5f / shadowMapH;
	adj.setRow (0, 0.5f,  0.0f, 0.0f, 0.0f);
	adj.setRow (1, 0.0f,  -0.5f, 0.0f, 0.0f);
	adj.setRow (2, 0.0f,  0.0f,	1.0f, 0.0f);
	adj.setRow (3, offsX,  offsY, 0.0f, 1.0f);
	_lightViewProj[split] = adj >> projMatrix >> invLightViewMatrix >> camera->getViewMatrix();
}

void ATOM_CascadedShadowMapManager::calculateSplitDistances (ATOM_Camera *camera)
{
	ATOM_STACK_TRACE(ATOM_CascadedShadowMapManager::calculateSplitDistances);

	float farPlane = camera->getFarPlane ();
	float nearPlane = camera->getNearPlane ();
	float shadowRegion = getShadowDistance ();
	if (farPlane < shadowRegion)
	{
		shadowRegion = farPlane;
	}

	float lambda = ATOM_saturate (_splitLambda);
	for (unsigned i = 0; i < 4; ++i)
	{
		float fIDM = i/4.f;
		float fLog = nearPlane * ATOM_pow ((shadowRegion/nearPlane), fIDM);
		float fUniform = nearPlane + (shadowRegion - nearPlane) * fIDM;
		_splitDistances[i] = fLog * lambda + fUniform * (1.f - lambda);
	}
	_splitDistances[0] = nearPlane;
	_splitDistances[4] = shadowRegion;

}

void ATOM_CascadedShadowMapManager::calculateFrustumCorners (int split, ATOM_Camera *camera, ATOM_Vector3f corners[9]) const
{
	ATOM_STACK_TRACE(ATOM_CascadedShadowMapManager::calculateFrustumCorners);

	float nearPlane = _splitDistances[split];
	float farPlane = _splitDistances[split+1];
	float scale = 1.1f;
	const ATOM_Matrix4x4f &viewMatrix = camera->getViewMatrix ();
	ATOM_Vector3f eye, center, up;
	viewMatrix.decomposeLookatLH (eye, center, up);
	ATOM_Vector3f z = center - eye;
	ATOM_Vector3f x = crossProduct (up, z);
	ATOM_Vector3f y = crossProduct (z, x);

	float tanHalfFov = camera->getTanHalfFovy ();
	float aspect = camera->getAspect();

	float nearPlaneHeight = tanHalfFov * nearPlane;
	float nearPlaneWidth = nearPlaneHeight * aspect;
	float farPlaneHeight = tanHalfFov * farPlane;
	float farPlaneWidth = farPlaneHeight * aspect;

	ATOM_Vector3f nearPlaneCenter = eye + z * nearPlane;
	ATOM_Vector3f farPlaneCenter = eye + z * farPlane;

	corners[0] = nearPlaneCenter - x * nearPlaneWidth - y * nearPlaneHeight;
	corners[1] = nearPlaneCenter - x * nearPlaneWidth + y * nearPlaneHeight;
	corners[2] = nearPlaneCenter + x * nearPlaneWidth + y * nearPlaneHeight;
	corners[3] = nearPlaneCenter + x * nearPlaneWidth - y * nearPlaneHeight;
	corners[4] = farPlaneCenter - x * farPlaneWidth - y * farPlaneHeight;
	corners[5] = farPlaneCenter - x * farPlaneWidth + y * farPlaneHeight;
	corners[6] = farPlaneCenter + x * farPlaneWidth + y * farPlaneHeight;
	corners[7] = farPlaneCenter + x * farPlaneWidth - y * farPlaneHeight;
	corners[8].set(0.f, 0.f, 0.f);

	ATOM_Vector3f c(0.f, 0.f, 0.f);
	for (unsigned i = 0; i < 8; ++i)
	{
		corners[8] += corners[i];
	}
	corners[8] *= 0.125f;

	for (unsigned i = 0; i < 8; ++i)
	{
		corners[i] += (corners[i] - corners[8]) * (scale - 1.f);
	}
}

void ATOM_CascadedShadowMapManager::setMinShadowVariance (float value)
{
	_minShadowVariance = value;
}

float ATOM_CascadedShadowMapManager::getMinShadowVariance (void) const
{
	return _minShadowVariance;
}

void ATOM_CascadedShadowMapManager::setAntiLightBleedingValue (float value)
{
	_antiLightBleedingValue = value;
}

float ATOM_CascadedShadowMapManager::getAntiLightBleedingValue (void) const
{
	return _antiLightBleedingValue;
}

ATOM_CascadedShadowMapManager *ATOM_CascadedShadowMapManager::getInstance (void)
{
	static ATOM_CascadedShadowMapManager manager;
	return &manager;
}

const float *ATOM_CascadedShadowMapManager::getSplitDistances (void) const
{
	return _splitDistances;
}

const ATOM_Matrix4x4f *ATOM_CascadedShadowMapManager::getShadowMatrices (void) const
{
	return _lightViewProj;
}

void ATOM_CascadedShadowMapManager::setSceneScale (float value)
{
	_sceneScale = value;
}

float ATOM_CascadedShadowMapManager::getSceneScale (void) const
{
	return _sceneScale;
}

void ATOM_CascadedShadowMapManager::setLightSize (float value)
{
	_lightSize = value;
}

float ATOM_CascadedShadowMapManager::getLightSize (void) const
{
	return _lightSize;
}

void ATOM_CascadedShadowMapManager::setDepthBias (float value)
{
	_depthBias = value;
}

float ATOM_CascadedShadowMapManager::getDepthBias (void) const
{
	return _depthBias;
}

