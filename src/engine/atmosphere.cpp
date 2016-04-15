#include "StdAfx.h"
#include "atmosphere.h"

static const char *strSkyMaterial = 
	"material										\n"
	"{												\n"
	"	effect E {									\n"
	"		source \"								\n"
	"			float4x4 WorldViewProj;																			\n"
	"			float3 v3LightDir;		// Light direction														\n"
	"			float3 v3CameraPos;		// Camera's current position											\n"
	"			float3 v3InvWavelength;	// 1 / pow(wavelength, 4) for RGB channels								\n"
	"			float  fCameraHeight;																			\n"
	"			float  fSkydomeRadius;																			\n"
	"			float  KrESun;			// Kr * ESun															\n"
	"			float  KmESun;			// Km * ESun															\n"
	"			float4 g;				//(2*g, g*g, 1.5*((1-g*g)/(2+g*g)), 1+g*g)								\n"
	"			float  fExposure;	// Exposure parameter for pixel shader										\n"
	"			struct a2v																						\n"
	"			{																								\n"
	"				float4 Position : POSITION0;																\n"
	"				float4 Diffuse : COLOR0;																	\n"
	"			};																								\n"
	"			struct v2p																						\n"
	"			{																								\n"
	"				float4 Position			: POSITION0;														\n"
	"				float3 Direction		: TEXCOORD0;														\n"
	"				float4 RayleighColor    : TEXCOORD1;														\n"
	"				float4 MieColor			: TEXCOORD2;														\n"
	"			};																								\n"
	"			struct p2c																						\n"
	"			{																								\n"
	"				float4 color : COLOR0;																		\n"
	"			};																								\n"
	"			void RenderSkyVS(in a2v IN, out v2p OUT)														\n"
	"			{																								\n"
	"				float3 v3Pos = IN.Position.xyz * fSkydomeRadius;											\n"
	"				OUT.Position = mul(IN.Position, WorldViewProj);												\n"
	"				v3Pos.y = max(v3Pos.y, fCameraHeight);														\n"
	"				float3 direction = v3CameraPos - v3Pos;														\n"
	"				float cos = dot(v3LightDir, normalize(direction));											\n"
	"				float rayleighPhase = 0.75*(1.0+cos*cos);													\n"
	"				OUT.RayleighColor.rgb = IN.Diffuse.rgb * rayleighPhase * (v3InvWavelength * KrESun);		\n"
	"				OUT.MieColor.rgb = IN.Diffuse.rgb * KmESun;													\n"
	"				OUT.Direction = direction;																	\n"
	"			}																								\n"
	"																											\n"
	"			p2c RenderSkyPS(in v2p IN)																		\n"
	"			{																								\n"
	"				p2c result;																					\n"
	"				float cos = dot(v3LightDir, normalize(IN.Direction));										\n"
	"				float miePhase = g.z*(1.0f + cos*cos) / pow(g.w - g.x * cos, 1.5f);							\n"
	"				float3 color = IN.RayleighColor + miePhase * IN.MieColor;									\n"
	"				result.color.rgb = 1 - exp(-fExposure * color);												\n"
	"				result.color.a = 1.0;																		\n"
	"				return result;																				\n"
	"			}																								\n"
	"			technique RenderSky																				\n"
	"			{																								\n"
	"				pass p0																						\n"
	"				{																							\n"
	"					CullMode = None;																		\n"
	"					VertexShader = compile vs_2_0 RenderSkyVS();											\n"
	"					PixelShader = compile ps_2_0 RenderSkyPS();												\n"
	"					ZWriteEnable = 0;																		\n"
	"					ZFunc = Always;																		\n"
	"				}																							\n"
	"			}\";																							\n"
	"	}																										\n"
	"}																											\n";

static const char *strSkySimple = 
	"material										\n"
	"{												\n"
	"	effect E {									\n"
	"		source \"								\n"
	"			technique RenderSky																				\n"
	"			{																								\n"
	"				pass p0																						\n"
	"				{																							\n"
	"					ColorOp[0] = SelectArg1;																\n"
	"					ColorArg1[0] = Diffuse;																	\n"
	"					ColorOp[1] = Disable;																	\n"
	"					VertexShader = null;																	\n"
	"					PixelShader = null;																		\n"
	"					ZWriteEnable = 0;																		\n"
	"					ZFunc = Always;																			\n"
	"				}																							\n"
	"			}\";																							\n"
	"	}																										\n"
	"}																											\n";

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_Atmosphere)
	ATOM_ATTRIBUTES_BEGIN(ATOM_Atmosphere)
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere, "MoonTexture", getMoonTextureFileName, setMoonTextureFileName, "", "group=ATOM_Atmosphere;type=vfilename;desc='VFS moon texture file name'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere, "StarfieldTexture", getStarfieldTextureFileName, setStarfieldTextureFileName, "", "group=ATOM_Atmosphere;type=vfilename;desc='VFS star field texture file name'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere, "StarfieldTextureRepeat", getStarfieldTextureRepeat, setStarfieldTextureRepeat, 1.f, "group=ATOM_Atmosphere")
		//ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere, "G", getG, setG, -0.99f, 0)
		//ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere, "MieMultiplier", getMieMultiplier, setMieMultiplier, 0.0010f, 0)
		//ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere, "RayleighMultiplier", getRayleighMultiplier, setRayleighMultiplier, 0.0025f, 0)
		//ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere, "Exposure", getExposure, setExposure, 2.f, 0)
		//ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere, "LambdaR", getLambdaRed, setLambdaRed, 0.65f, 0)
		//ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere, "LambdaG", getLambdaGreen, setLambdaGreen, 0.57f, 0)
		//ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere, "LambdaB", getLambdaBlue, setLambdaBlue, 0.47f, 0)
		//ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere, "CameraHeight", getCameraHeight, setCameraHeight, 0.01f, 0)
		//ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere, "FadeNear", getFadeNear, setFadeNear, 100.f, 0)
		//ATOM_ATTRIBUTE_PERSISTENT(ATOM_Atmosphere, "FadeFar", getFadeFar, setFadeFar, 1000.0f, 0)
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_Atmosphere, ATOM_Node)

const float atmosphereHeightScale = 1.025f;

ATOM_Atmosphere::ATOM_Atmosphere (void)
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::ATOM_Atmosphere);

	_enableGeoClipping = false;
	_vertexDecl = 0;
	_skydomeRadius.setKeyValues(102500.f);
	_sunIntensity.setKeyValues(50.f);
	_sunDirection.setKeyValues(ATOM_Vector3f(0.5f, 0.5f, 0.5f));
	_g.setKeyValues(-0.99f);
	_mieMultiplier.setKeyValues(0.0010f);
	_rayleighMultiplier.setKeyValues(0.0025f);
	_exposure.setKeyValues(2.f);
	_lambdaR.setKeyValues(0.65f);
	_lambdaG.setKeyValues(0.57f);
	_lambdaB.setKeyValues(0.47f);
	_scaleDepth.setKeyValues(0.25f);
	_cameraHeight.setKeyValues(0.001f);
	_fadeNear.setKeyValues(100.f);
	_fadeFar.setKeyValues(1000.f);
	_cloudCoverage.setKeyValues(0.5f);
	_cloudSharpness.setKeyValues(0.92f);
	_cloudBrightness.setKeyValues(400.f);
	_cloudMoveSpeed.setKeyValues(0.005f);
	_cloudMoveDir.setKeyValues(ATOM_Vector4f(1.f, 0.f, 0.f, 0.f));
	_cloudColor.setKeyValues(ATOM_Vector4f(1.f, 1.f, 1.f, 1.f));
	_cloudDeformSpeed.setKeyValues (0.01f);
	_moonColor.setKeyValues (ATOM_Vector4f(1.f, 1.f, 1.f, 1.f));
	_moonDir.setKeyValues (ATOM_Vector4f(1.f, 1.f, 1.f, 0.f));
	_moonSize.setKeyValues (0.1f);
	_numSamples = 3;
	_starfieldTextureRepeat = 1.f;

	_colorChanged = true;
	//_cloudPlane = 0;
	_oldSunDirection.set (0.f, 0.f, 0.f);

	updateTime (0);
}

ATOM_Atmosphere::~ATOM_Atmosphere (void)
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::~ATOM_Atmosphere);

	//ATOM_DELETE(_cloudPlane);
}

bool ATOM_Atmosphere::onLoad (ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::onLoad);

	_vertexDecl = device->createMultiStreamVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_3);

	bool forceff = ATOM_RenderSettings::isFixedFunction();
	if (!forceff)
	{
		_scatteringMaterial = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/atmosphere.mat");
		if (!_scatteringMaterial)
		{
			fini ();
			return false;
		}
		
		/*
		if (!_cloudPlane && ATOM_RenderSettings::isCloudCoverEnabled())
		{
			createCloud (device);
		}
		else if (_cloudPlane && !ATOM_RenderSettings::isCloudCoverEnabled())
		{
			ATOM_DELETE(_cloudPlane);
			_cloudPlane = 0;
		}
		*/
	}

	return true;
}

bool ATOM_Atmosphere::createCloud (ATOM_RenderDevice *device)
{
	/*
	_cloudPlane = ATOM_NEW(ATOM_CloudPlane);
	if (!_cloudPlane->create (device))
	{
		ATOM_DELETE(_cloudPlane);
		_cloudPlane = 0;
		return false;
	}
	else
	{
		_cloudPlane->setDrawMode ((ATOM_RenderSettings::getRenderCaps().shaderModel >= 3) ? ATOM_CloudPlane::DM_SM3 : ATOM_CloudPlane::DM_SM2);

		ATOM_AUTOREF(ATOM_Texture) textureMoon = _moonTextureFileName.empty() ? 0 : ATOM_CreateTextureResource (_moonTextureFileName.c_str());
		_cloudPlane->setMoonTexture (textureMoon.get());

		ATOM_AUTOREF(ATOM_Texture) textureStarfield = _starfieldTextureFileName.empty() ? 0 : ATOM_CreateTextureResource (_starfieldTextureFileName.c_str());
		_cloudPlane->setStarfieldTexture (textureStarfield.get());

		_cloudPlane->setStarfieldTextureRepeat (_starfieldTextureRepeat);
		return true;
	}
	*/
	return true;
}

void ATOM_Atmosphere::render (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::render);

	ATOM_Vector3f sunDirection = getSunDirection();
	if (0 && sunDirection.y < 0.005f)
	{
		sunDirection = 0.005f;
	}
	sunDirection.normalize();

	if (sunDirection != _oldSunDirection)
	{
		_colorChanged = true;
		_oldSunDirection = sunDirection;
	}

	syncColor (sunDirection, ATOM_RenderSettings::isFixedFunction());

	ATOM_Matrix4x4f matProjection = camera->getProjectionMatrix ();
	float znear = -matProjection.m32/matProjection.m22;
	if (znear > 0.1f)
	{
		znear = 0.1f;
	}
	float zfar = matProjection.m32/(1.f - matProjection.m22);
	if (zfar < _skydomeRadiusVal * 2.f)
	{
		zfar = _skydomeRadiusVal * 2.f;
	}
	matProjection.m22 = zfar / (zfar - znear);
	matProjection.m32 = -znear * matProjection.m22;

	float fOuterRadius = _skydomeRadiusVal;
	float fInnerRadius = _skydomeRadiusVal * (2.f - atmosphereHeightScale);

	ATOM_Matrix4x4f matView = camera->getViewMatrix ();
	matView.m30 = 0.f;
	matView.m31 = fInnerRadius + _cameraHeightVal * (fOuterRadius - fInnerRadius);
	matView.m32 = 0.f;
	ATOM_Matrix4x4f matInvView;
	matInvView.invertAffineFrom (matView);
	ATOM_Matrix4x4f matMVP = matProjection >> matInvView >> ATOM_Matrix4x4f::getScaleMatrix (ATOM_Vector3f(_skydomeRadius.getCurrentValue()));

	//ATOM_Vector3f sunDirection = ATOM_Vector3f(0.f, 2.f, 1.f);//-((ATOM_SDLScene*)ATOM_Scene::getCurrentScene ())->getLight()->getDirection ();
	//sunDirection.normalize ();

	ATOM_Vector4f g;
	g.x = _gVal * 2.f;
	g.y = _gVal * _gVal;
	g.z = 1.5f * ((1.f - g.y) / (2.f + g.y));
	g.w = 1.f + g.y;

	float fInnerRadius2 = fInnerRadius * fInnerRadius;
	float fOuterRadius2 = fOuterRadius * fOuterRadius;
	float fScale = 1.f / (fOuterRadius - fInnerRadius);
	float fScaleDepth = _scaleDepthVal;
	float fScaleOverScaleDepth = fScale / fScaleDepth;
	float fKr4Pi = _rayleighMultiplierVal * 4 * ATOM_Pi;
	float fKm4Pi = _mieMultiplierVal * 4 * ATOM_Pi;
	float fKrESun = _rayleighMultiplierVal * _sunIntensityVal;
	float fKmESun = _mieMultiplierVal * _sunIntensityVal;
	int numSamples = _numSamples;
	ATOM_Vector3f invWaveLength (1.f/ATOM_pow(_lambdaRVal, 4.f), 1.f/ATOM_pow(_lambdaGVal, 4.f), 1.f/ATOM_pow(_lambdaBVal, 4.f));

	float y = fInnerRadius + _cameraHeightVal * (fOuterRadius - fInnerRadius);
	float y2 = y * y;
	ATOM_Vector3f vecCamera(matView.m30, matView.m31, matView.m32);

	ATOM_Material *m = material;

	m->getParameterTable()->setMatrix44 ("WorldViewProj", matMVP);
	m->getParameterTable()->setVector ("v3LightDir", ATOM_Vector4f(sunDirection));
	m->getParameterTable()->setVector ("v3CameraPos", ATOM_Vector4f(vecCamera));
	m->getParameterTable()->setVector ("v3InvWaveLength", ATOM_Vector4f(invWaveLength));
	m->getParameterTable()->setFloat ("fCameraHeight", y);
	m->getParameterTable()->setFloat ("KrESun", fKrESun);
	m->getParameterTable()->setFloat ("KmESun", fKmESun);
	m->getParameterTable()->setFloat ("fSkydomeRadius", _skydomeRadiusVal);
	m->getParameterTable()->setVector ("g", g);
	m->getParameterTable()->setFloat ("fExposure", _exposureVal);

	device->setVertexDecl (_vertexDecl);
	device->setStreamSource (0, _skyDomeVertices.get());
	device->setStreamSource (1, _skyDomeColors.get());

	unsigned numPasses = m->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (m->beginPass (device, pass))
		{
			device->renderStreamsIndexed (_skyDomeIndices.get(), ATOM_PRIMITIVE_TRIANGLE_STRIP, _skyDomeIndices->getNumIndices() -2, 0);
			m->endPass (device, pass);
		}
	}
	m->end (device);
}

void ATOM_Atmosphere::fini (void)
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::fini);

	ATOM_RenderDevice *device = ATOM_GetRenderDevice();
	if (device)
	{
		device->destroyVertexDecl (_vertexDecl);
		_vertexDecl = 0;
	}

	_skyDomeVertices = 0;
	_skyDomeColors = 0;
	_skyDomeIndices = 0;
	_scatteringMaterial = 0;
}

void ATOM_Atmosphere::accept (ATOM_Visitor &visitor)
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::accept);

	visitor.visit (*this);
}

float ATOM_Atmosphere::getSunIntensity (void) const
{
	return _sunIntensityVal;
}

const ATOM_Vector3f &ATOM_Atmosphere::getSunDirection (void) const
{
	return _sunDirectionVal;
}

float ATOM_Atmosphere::getG (void) const
{
	return _gVal;
}

float ATOM_Atmosphere::getMieMultiplier (void) const
{
	return _mieMultiplierVal;
}

float ATOM_Atmosphere::getRayleighMultiplier (void) const
{
	return _rayleighMultiplierVal;
}

float ATOM_Atmosphere::getExposure (void) const
{
	return _exposureVal;
}

float ATOM_Atmosphere::getLambdaRed (void) const
{
	return _lambdaRVal;
}

float ATOM_Atmosphere::getLambdaGreen (void) const
{
	return _lambdaGVal;
}

float ATOM_Atmosphere::getLambdaBlue (void) const
{
	return _lambdaBVal;
}

float ATOM_Atmosphere::getInnerRadius (void) const
{
	return _skydomeRadiusVal * (2.f - atmosphereHeightScale);
}

float ATOM_Atmosphere::getOuterRadius (void) const
{
	return _skydomeRadiusVal;
}

float ATOM_Atmosphere::getSkydomeRadius (void) const
{
	return _skydomeRadiusVal;
}

float ATOM_Atmosphere::getScaleDepth (void) const
{
	return _scaleDepthVal;
}

void ATOM_Atmosphere::setNumSamples (int num)
{
	if (num != _numSamples)
	{
		_numSamples = num;
		_colorChanged = true;
	}
}

int ATOM_Atmosphere::getNumSamples (void) const
{
	return _numSamples;
}

float ATOM_Atmosphere::getCameraHeight (void) const
{
	return _cameraHeightVal;
}

float ATOM_Atmosphere::getFadeNear (void) const
{
	return _fadeNearVal;
}

float ATOM_Atmosphere::getFadeFar (void) const
{
	return _fadeFarVal;
}

float ATOM_Atmosphere::getCloudCoverage (void) const
{
	return _cloudCoverageVal;
}

float ATOM_Atmosphere::getCloudSharpness (void) const
{
	return _cloudSharpnessVal;
}

float ATOM_Atmosphere::getCloudBrightness (void) const
{
	return _cloudBrightnessVal;
}

float ATOM_Atmosphere::getCloudMoveSpeed (void) const
{
	return _cloudMoveSpeedVal;
}

const ATOM_Vector4f &ATOM_Atmosphere::getCloudMoveDir (void) const
{
	return _cloudMoveDirVal;
}

const ATOM_Vector4f &ATOM_Atmosphere::getCloudColor (void) const
{
	return _cloudColorVal;
}

float ATOM_Atmosphere::getCloudDeformSpeed (void) const
{
	return _cloudDeformSpeedVal;
}

const ATOM_Vector4f &ATOM_Atmosphere::getMoonColor (void) const
{
	return _moonColorVal;
}

const ATOM_Vector4f &ATOM_Atmosphere::getMoonDir (void) const
{
	return _moonDirVal;
}

float ATOM_Atmosphere::getMoonSize (void) const
{
	return _moonSizeVal;
}

void ATOM_Atmosphere::updateTime (ATOM_Camera *camera)
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::updateTime);

	_skydomeRadiusVal = _skydomeRadius.getCurrentValue();
	_sunIntensityVal = _sunIntensity.getCurrentValue();
	_sunDirectionVal = _sunDirection.getCurrentValue();
	_gVal = _g.getCurrentValue();
	_mieMultiplierVal = _mieMultiplier.getCurrentValue();
	_rayleighMultiplierVal = _rayleighMultiplier.getCurrentValue();
	_exposureVal = _exposure.getCurrentValue();
	_lambdaRVal = _lambdaR.getCurrentValue();
	_lambdaGVal = _lambdaG.getCurrentValue();
	_lambdaBVal = _lambdaB.getCurrentValue();
	_scaleDepthVal = _scaleDepth.getCurrentValue();
	_cameraHeightVal = _cameraHeight.getCurrentValue();
	_fadeNearVal = _fadeNear.getCurrentValue();
	_fadeFarVal = _fadeFar.getCurrentValue();

	if (camera)
	{
		ATOM_Vector4f rayDirC (0.f, 0.f, 1.f, 0.f);
		rayDirC <<= camera->getViewMatrix ();
		rayDirC.y = 0.2f;
		rayDirC.normalize();
		ATOM_Vector4f fogColor = calcSkyColor (rayDirC.getVector3(), getSunDirection());

		/*
		if (!_cloudPlane && ATOM_RenderSettings::isCloudCoverEnabled())
		{
			createCloud (ATOM_GetRenderDevice());
		}
		else if (_cloudPlane && !ATOM_RenderSettings::isCloudCoverEnabled())
		{
			ATOM_DELETE(_cloudPlane);
			_cloudPlane = 0;
		}

		if (_cloudPlane)
		{
			_cloudCoverageVal = _cloudCoverage.getCurrentValue();
			_cloudSharpnessVal = _cloudSharpness.getCurrentValue();
			_cloudBrightnessVal = _cloudBrightness.getCurrentValue();
			_cloudMoveSpeedVal = _cloudMoveSpeed.getCurrentValue();
			_cloudMoveDirVal = _cloudMoveDir.getCurrentValue();
			_cloudColorVal = _cloudColor.getCurrentValue();
			_cloudDeformSpeedVal = _cloudDeformSpeed.getCurrentValue();
			_cloudPlane->setScrollVelocity (ATOM_Vector2f(_cloudMoveSpeedVal * _cloudMoveDirVal.x, _cloudMoveSpeedVal * _cloudMoveDirVal.z));
			_cloudPlane->setDeformSpeed (_cloudDeformSpeedVal);
			_cloudPlane->setSunHeight (_cloudBrightnessVal);
			_cloudPlane->updateDeformation (ATOM_GetRenderDevice(), camera, sunDir, _cloudCoverageVal);
		}
		*/
	}
}

void ATOM_Atmosphere::setSunIntensityKey (int time, float val)
{
	if (val != _sunIntensity.getKeyValue (time))
	{
		_sunIntensity.setKeyValue (time, val);
		_sunIntensityVal = _sunIntensity.getCurrentValue();
		_colorChanged = true;
	}
}

void ATOM_Atmosphere::setSunDirectionKey (int time, const ATOM_Vector3f &val)
{
	if (val != _sunDirection.getKeyValue (time))
	{
		_sunDirection.setKeyValue (time, val);
		_sunDirectionVal = _sunDirection.getCurrentValue();
		_colorChanged = true;
	}
}

void ATOM_Atmosphere::setGKey (int time, float val)
{
	if (val != _g.getKeyValue (time))
	{
		_g.setKeyValue (time, val);
		_gVal = _g.getCurrentValue();
		_colorChanged = true;
	}
}

void ATOM_Atmosphere::setMieMultiplierKey (int time, float val)
{
	if (val != _mieMultiplier.getKeyValue (time))
	{
		_mieMultiplier.setKeyValue (time, val);
		_mieMultiplierVal = _mieMultiplier.getCurrentValue();
		_colorChanged = true;
	}
}

void ATOM_Atmosphere::setRayleighMultiplierKey (int time, float val)
{
	if (val != _rayleighMultiplier.getKeyValue (time))
	{
		_rayleighMultiplier.setKeyValue (time, val);
		_rayleighMultiplierVal = _rayleighMultiplier.getCurrentValue();
		_colorChanged = true;
	}
}

void ATOM_Atmosphere::setExposureKey (int time, float val)
{
	if (val != _exposure.getKeyValue (time))
	{
		_exposure.setKeyValue (time, val);
		_exposureVal = _exposure.getCurrentValue();
		_colorChanged = true;
	}
}

void ATOM_Atmosphere::setLambdaRedKey (int time, float val)
{
	if (val != _lambdaR.getKeyValue (time))
	{
		_lambdaR.setKeyValue (time, val);
		_lambdaRVal = _lambdaR.getCurrentValue();
		_colorChanged = true;
	}
}

void ATOM_Atmosphere::setLambdaGreenKey (int time, float val)
{
	if (val != _lambdaG.getKeyValue (time))
	{
		_lambdaG.setKeyValue (time, val);
		_lambdaGVal = _lambdaG.getCurrentValue();
		_colorChanged = true;
	}
}

void ATOM_Atmosphere::setLambdaBlueKey (int time, float val)
{
	if (val != _lambdaB.getKeyValue (time))
	{
		_lambdaB.setKeyValue (time, val);
		_lambdaBVal = _lambdaB.getCurrentValue();
		_colorChanged = true;
	}
}

void ATOM_Atmosphere::setScaleDepthKey (int time, float val)
{
	if (val != _scaleDepth.getKeyValue (time))
	{
		_scaleDepth.setKeyValue (time, val);
		_scaleDepthVal = _scaleDepth.getCurrentValue();
		_colorChanged = true;
	}
}

void ATOM_Atmosphere::setCameraHeightKey (int time, float val)
{
	if (val != _cameraHeight.getKeyValue (time))
	{
		_cameraHeight.setKeyValue (time, val);
		_cameraHeightVal = _cameraHeight.getCurrentValue();
		_colorChanged = true;
	}
}

void ATOM_Atmosphere::setSkydomeRadiusKey (int time, float val)
{
	if (val != _skydomeRadius.getKeyValue (time))
	{
		_skydomeRadius.setKeyValue (time, val);
		_skydomeRadiusVal = _skydomeRadius.getCurrentValue();
		_colorChanged = true;
	}
}

void ATOM_Atmosphere::setFadeNearKey (int time, float val)
{
	if (val != _fadeNear.getKeyValue (time))
	{
		_fadeNear.setKeyValue (time, val);
		_fadeNearVal = _fadeNear.getCurrentValue();
		_colorChanged = true;
	}
}

void ATOM_Atmosphere::setFadeFarKey (int time, float val)
{
	if (val != _fadeFar.getKeyValue (time))
	{
		_fadeFar.setKeyValue (time, val);
		_fadeFarVal = _fadeFar.getCurrentValue();
		_colorChanged = true;
	}
}

void ATOM_Atmosphere::setCloudCoverageKey (int time, float val)
{
	_cloudCoverage.setKeyValue (time, val);
	_cloudCoverageVal = _cloudCoverage.getCurrentValue();
}

void ATOM_Atmosphere::setCloudSharpnessKey (int time, float val)
{
	_cloudSharpness.setKeyValue (time, val);
	_cloudSharpnessVal = _cloudSharpness.getCurrentValue();
}

void ATOM_Atmosphere::setCloudBrightnessKey (int time, float val)
{
	_cloudBrightness.setKeyValue (time, val);
	_cloudBrightnessVal = _cloudBrightness.getCurrentValue ();
}

void ATOM_Atmosphere::setCloudMoveSpeedKey (int time, float val)
{
	_cloudMoveSpeed.setKeyValue (time, val);
	_cloudMoveSpeedVal = _cloudMoveSpeed.getCurrentValue ();
}

void ATOM_Atmosphere::setCloudMoveDirKey (int time, const ATOM_Vector4f &val)
{
	_cloudMoveDir.setKeyValue (time, val);
	_cloudMoveDirVal = _cloudMoveDir.getCurrentValue ();
}

void ATOM_Atmosphere::setCloudColorKey (int time, const ATOM_Vector4f &val)
{
	_cloudColor.setKeyValue (time, val);
	_cloudColorVal = _cloudColor.getCurrentValue();
}

void ATOM_Atmosphere::setCloudDeformSpeedKey (int time, float val)
{
	_cloudDeformSpeed.setKeyValue (time, val);
	_cloudDeformSpeedVal = _cloudDeformSpeed.getCurrentValue();
}

void ATOM_Atmosphere::setMoonColorKey (int time, const ATOM_Vector4f &val)
{
	_moonColor.setKeyValue (time, val);
	_moonColorVal = _moonColor.getCurrentValue();
}

void ATOM_Atmosphere::setMoonDirKey (int time, const ATOM_Vector4f &val)
{
	_moonDir.setKeyValue (time, val);
	_moonDirVal = _moonDir.getCurrentValue();
}

void ATOM_Atmosphere::setMoonSizeKey (int time, float val)
{
	_moonSize.setKeyValue (time, val);
	_moonSizeVal = _moonSize.getCurrentValue();
}

float ATOM_Atmosphere::getSunIntensityKey (int time) const
{
	return _sunIntensity.getKeyValue (time);
}

const ATOM_Vector3f &ATOM_Atmosphere::getSunDirectionKey (int time) const
{
	return _sunDirection.getKeyValue (time);
}

float ATOM_Atmosphere::getGKey (int time) const
{
	return _g.getKeyValue (time);
}

float ATOM_Atmosphere::getMieMultiplierKey (int time) const
{
	return _mieMultiplier.getKeyValue (time);
}

float ATOM_Atmosphere::getRayleighMultiplierKey (int time) const
{
	return _rayleighMultiplier.getKeyValue (time);
}

float ATOM_Atmosphere::getExposureKey (int time) const
{
	return _exposure.getKeyValue (time);
}

float ATOM_Atmosphere::getLambdaRedKey (int time) const
{
	return _lambdaR.getKeyValue (time);
}

float ATOM_Atmosphere::getLambdaGreenKey (int time) const
{
	return _lambdaG.getKeyValue (time);
}

float ATOM_Atmosphere::getLambdaBlueKey (int time) const
{
	return _lambdaB.getKeyValue (time);
}

float ATOM_Atmosphere::getScaleDepthKey (int time) const
{
	return _scaleDepth.getKeyValue (time);
}

float ATOM_Atmosphere::getCameraHeightKey (int time) const
{
	return _cameraHeight.getKeyValue (time);
}

float ATOM_Atmosphere::getSkydomeRadiusKey (int time) const
{
	return _skydomeRadius.getKeyValue (time);
}

float ATOM_Atmosphere::getFadeNearKey (int time) const
{
	return _fadeNear.getKeyValue (time);
}

float ATOM_Atmosphere::getFadeFarKey (int time) const
{
	return _fadeFar.getKeyValue (time);
}

float ATOM_Atmosphere::getCloudCoverageKey (int time) const
{
	return _cloudCoverage.getKeyValue (time);
}

float ATOM_Atmosphere::getCloudSharpnessKey (int time) const
{
	return _cloudSharpness.getKeyValue (time);
}

float ATOM_Atmosphere::getCloudBrightnessKey (int time) const
{
	return _cloudBrightness.getKeyValue (time);
}

float ATOM_Atmosphere::getCloudMoveSpeedKey (int time) const
{
	return _cloudMoveSpeed.getKeyValue (time);
}

const ATOM_Vector4f &ATOM_Atmosphere::getCloudMoveDirKey (int time) const
{
	return _cloudMoveDir.getKeyValue (time);
}

const ATOM_Vector4f &ATOM_Atmosphere::getCloudColorKey (int time) const
{
	return _cloudColor.getKeyValue (time);
}

float ATOM_Atmosphere::getCloudDeformSpeedKey (int time) const
{
	return _cloudDeformSpeed.getKeyValue (time);
}

const ATOM_Vector4f &ATOM_Atmosphere::getMoonColorKey (int time) const
{
	return _moonColor.getKeyValue (time);
}

const ATOM_Vector4f &ATOM_Atmosphere::getMoonDirKey (int time) const
{
	return _moonDir.getKeyValue (time);
}

float ATOM_Atmosphere::getMoonSizeKey (int time) const
{
	return _moonSize.getKeyValue (time);
}

void ATOM_Atmosphere::copyAttributesTo (ATOM_Object *obj) const
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::copyAttributesTo);

	ATOM_Node::copyAttributesTo (obj);

	ATOM_Atmosphere *other = dynamic_cast<ATOM_Atmosphere*>(obj);
	if (other)
	{
		other->_skydomeRadius = _skydomeRadius;
		other->_sunIntensity = _sunIntensity;
		other->_g = _g;
		other->_mieMultiplier = _mieMultiplier;
		other->_rayleighMultiplier = _rayleighMultiplier;
		other->_exposure = _exposure;
		other->_lambdaR = _lambdaR;
		other->_lambdaG = _lambdaG;
		other->_lambdaB = _lambdaB;
		other->_scaleDepth = _scaleDepth;
		other->_cameraHeight = _cameraHeight;
		other->_fadeNear = _fadeNear;
		other->_fadeFar = _fadeFar;
		other->_cloudCoverage = _cloudCoverage;
		other->_cloudSharpness = _cloudSharpness;
		other->_cloudBrightness = _cloudBrightness;
		other->_cloudMoveSpeed = _cloudMoveSpeed;
		other->_cloudMoveDir = _cloudMoveDir;
		other->_cloudColor = _cloudColor;
		other->_cloudDeformSpeed = _cloudDeformSpeed;
		other->_moonColor = _moonColor;
		other->_moonDir = _moonDir;
		other->_moonSize = _moonSize;
		other->_numSamples = _numSamples;
	}
}

bool ATOM_Atmosphere::loadAttribute(const ATOM_TiXmlElement *xmlelement)
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::loadAttribute);

	if (!ATOM_Node::loadAttribute (xmlelement))
	{
		return false;
	}

	loadKeys ("SunIntensity", 50.f, _sunIntensity, xmlelement);
	loadKeys ("G", -0.99f, _g, xmlelement);
	loadKeys ("MieMultiplier", 0.0010f, _mieMultiplier, xmlelement);
	loadKeys ("RayleighMultiplier", 0.0025f, _rayleighMultiplier, xmlelement);
	loadKeys ("Exposure", 2.f, _exposure, xmlelement);
	loadKeys ("LambdaR", 0.65f, _lambdaR, xmlelement);
	loadKeys ("LambdaG", 0.57f, _lambdaG, xmlelement);
	loadKeys ("LambdaB", 0.47f, _lambdaB, xmlelement);
	loadKeys ("CameraHeight", 0.01f, _cameraHeight, xmlelement);
	loadKeys ("FadeNear", 100.f, _fadeNear, xmlelement);
	loadKeys ("FadeFar", 1000.f, _fadeFar, xmlelement);
	loadKeys ("CloudCoverage", 0.5f, _cloudCoverage, xmlelement);
	loadKeys ("CloudSharpness", 0.85f, _cloudSharpness, xmlelement);
	loadKeys ("CloudBrightness", 400.f, _cloudBrightness, xmlelement);
	loadKeys ("CloudMoveSpeed", 0.005f, _cloudMoveSpeed, xmlelement);
	loadVector4Keys ("CloudMoveDir", ATOM_Vector4f(1.f, 0.f, 0.f, 0.f), _cloudMoveDir, xmlelement);
	loadKeys ("CloudDeformSpeed", 0.05f, _cloudDeformSpeed, xmlelement);
	loadVector4Keys ("CloudColor", ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), _cloudColor, xmlelement);
	loadVector4Keys ("MoonColor", ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), _moonColor, xmlelement);
	loadVector4Keys ("MoonDir", ATOM_Vector4f(1.f, 1.f, 1.f, 0.f), _moonDir, xmlelement);
	loadKeys ("MoonSize", 0.1f, _moonSize, xmlelement);
	
	return true;
}

bool ATOM_Atmosphere::writeAttribute(ATOM_TiXmlElement *xmlelement)
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::writeAttribute);

	if (!ATOM_Node::writeAttribute (xmlelement))
	{
		return false;
	}

	writeKeys ("SunIntensity", _sunIntensity, xmlelement);
	writeKeys ("G", _g, xmlelement);
	writeKeys ("MieMultiplier", _mieMultiplier, xmlelement);
	writeKeys ("RayleighMultiplier", _rayleighMultiplier, xmlelement);
	writeKeys ("Exposure", _exposure, xmlelement);
	writeKeys ("LambdaR", _lambdaR, xmlelement);
	writeKeys ("LambdaG", _lambdaG, xmlelement);
	writeKeys ("LambdaB", _lambdaB, xmlelement);
	writeKeys ("CameraHeight", _cameraHeight, xmlelement);
	writeKeys ("FadeNear", _fadeNear, xmlelement);
	writeKeys ("FadeFar", _fadeFar, xmlelement);
	writeKeys ("CloudCoverage", _cloudCoverage, xmlelement);
	writeKeys ("CloudSharpness", _cloudSharpness, xmlelement);
	writeKeys ("CloudBrightness", _cloudBrightness, xmlelement);
	writeKeys ("CloudMoveSpeed", _cloudMoveSpeed, xmlelement);
	writeVector4Keys ("CloudMoveDir", _cloudMoveDir, xmlelement);
	writeKeys ("CloudDeformSpeed", _cloudDeformSpeed, xmlelement);
	writeVector4Keys ("CloudColor", _cloudColor, xmlelement);
	writeVector4Keys ("MoonColor", _moonColor, xmlelement);
	writeVector4Keys ("MoonDir", _moonDir, xmlelement);
	writeKeys ("MoonSize", _moonSize, xmlelement);

	return true;
}

void ATOM_Atmosphere::writeVector4Keys(const char *name, const ATOM_TimeValueT<ATOM_Vector4f> &keys, ATOM_TiXmlElement *e) const
{
	char buffer[256];
	ATOM_TiXmlElement eProp(name);
	for (int i = 0; i < ATOM_TimeManager::numTimeKeys; ++i)
	{
		ATOM_TiXmlElement eKey("Key");
		const ATOM_Vector4f &v = keys.getKeyValue(i);
		sprintf (buffer, "%f %f %f %f", v.x, v.y, v.z, v.w);
		eKey.SetAttribute ("Value", buffer);
		eProp.InsertEndChild (eKey);
	}
	e->InsertEndChild (eProp);
}

void ATOM_Atmosphere::writeKeys(const char *name, const ATOM_TimeValueT<float> &keys, ATOM_TiXmlElement *e) const
{
	ATOM_TiXmlElement eProp(name);
	for (int i = 0; i < ATOM_TimeManager::numTimeKeys; ++i)
	{
		ATOM_TiXmlElement eKey("Key");
		eKey.SetDoubleAttribute ("Value", keys.getKeyValue (i));
		eProp.InsertEndChild (eKey);
	}
	e->InsertEndChild (eProp);
}

void ATOM_Atmosphere::loadVector3Keys(const char *name, const ATOM_Vector3f &defaultValue, ATOM_TimeValueT<ATOM_Vector3f> &keys, const ATOM_TiXmlElement *e) const
{
	const ATOM_TiXmlElement *Keys = e->FirstChildElement(name);
	if (!Keys)
	{
		const char *s = e->Attribute (name);
		if (s)
		{
			ATOM_Vector3f v;
			if (3 != sscanf (s, "%f %f %f", &v.x, &v.y, &v.z))
			{
				v = defaultValue;
			}
			keys.setKeyValues (v);
		}
		else
		{
			keys.setKeyValues (defaultValue);
		}
	}
	else
	{
		const ATOM_TiXmlElement *Key = 0;
		for (int i = 0; i < ATOM_TimeManager::numTimeKeys; ++i)
		{
			Key = (i == 0) ? Keys->FirstChildElement ("Key") : Key->NextSiblingElement("Key");
			if (!Key)
			{
				keys.setKeyValues (defaultValue);
				break;
			}
			else
			{
				const char *s = Key->Attribute ("Value");
				if (s)
				{
					ATOM_Vector3f v;
					if (3 != sscanf (s, "%f %f %f", &v.x, &v.y, &v.z))
					{
						v = defaultValue;
					}
					keys.setKeyValue (i, v);
				}
				else
				{
					keys.setKeyValue (i, defaultValue);
				}
			}
		}
	}
}

void ATOM_Atmosphere::writeVector3Keys(const char *name, const ATOM_TimeValueT<ATOM_Vector3f> &keys, ATOM_TiXmlElement *e) const
{
	char buffer[256];
	ATOM_TiXmlElement eProp(name);
	for (int i = 0; i < ATOM_TimeManager::numTimeKeys; ++i)
	{
		ATOM_TiXmlElement eKey("Key");
		const ATOM_Vector3f &v = keys.getKeyValue(i);
		sprintf (buffer, "%f %f %f", v.x, v.y, v.z);
		eKey.SetAttribute ("Value", buffer);
		eProp.InsertEndChild (eKey);
	}
	e->InsertEndChild (eProp);
}

void ATOM_Atmosphere::loadVector4Keys(const char *name, const ATOM_Vector4f &defaultValue, ATOM_TimeValueT<ATOM_Vector4f> &keys, const ATOM_TiXmlElement *e) const
{
	const ATOM_TiXmlElement *Keys = e->FirstChildElement(name);
	if (!Keys)
	{
		const char *s = e->Attribute (name);
		if (s)
		{
			ATOM_Vector4f v;
			if (4 != sscanf (s, "%f %f %f %f", &v.x, &v.y, &v.z, &v.w))
			{
				v = defaultValue;
			}
			keys.setKeyValues (v);
		}
		else
		{
			keys.setKeyValues (defaultValue);
		}
	}
	else
	{
		const ATOM_TiXmlElement *Key = 0;
		for (int i = 0; i < ATOM_TimeManager::numTimeKeys; ++i)
		{
			Key = (i == 0) ? Keys->FirstChildElement ("Key") : Key->NextSiblingElement("Key");
			if (!Key)
			{
				keys.setKeyValues (defaultValue);
				break;
			}
			else
			{
				const char *s = Key->Attribute ("Value");
				if (s)
				{
					ATOM_Vector4f v;
					if (4 != sscanf (s, "%f %f %f %f", &v.x, &v.y, &v.z, &v.w))
					{
						v = defaultValue;
					}
					keys.setKeyValue (i, v);
				}
				else
				{
					keys.setKeyValue (i, defaultValue);
				}
			}
		}
	}
}

void ATOM_Atmosphere::loadKeys(const char *name, float defaultValue, ATOM_TimeValueT<float> &keys, const ATOM_TiXmlElement *e) const
{
	const ATOM_TiXmlElement *Keys = e->FirstChildElement(name);
	if (!Keys)
	{
		double val = defaultValue;
		e->QueryDoubleAttribute (name, &val);
		keys.setKeyValues (val);
	}
	else
	{
		const ATOM_TiXmlElement *Key = 0;
		for (int i = 0; i < ATOM_TimeManager::numTimeKeys; ++i)
		{
			Key = (i == 0) ? Keys->FirstChildElement ("Key") : Key->NextSiblingElement("Key");
			if (!Key)
			{
				keys.setKeyValues (defaultValue);
				break;
			}
			else
			{
				double val = defaultValue;
				Key->Attribute ("Value", &val);
				keys.setKeyValue (i, val);
			}
		}
	}
}

bool ATOM_Atmosphere::draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::render);

	render (device, camera, material);

	return true;
}

void ATOM_Atmosphere::setMoonTextureFileName (const ATOM_STRING &fileName)
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::setMoonTextureFileName);

	_moonTextureFileName = fileName;

	/*
	if (_cloudPlane)
	{
		ATOM_AUTOREF(ATOM_Texture) texture = _moonTextureFileName.empty() ? 0 : ATOM_CreateTextureResource (_moonTextureFileName.c_str());
		_cloudPlane->setMoonTexture (texture.get());
	}
	*/
}

const ATOM_STRING &ATOM_Atmosphere::getMoonTextureFileName (void) const
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::getMoonTextureFileName);

	return _moonTextureFileName;
}

void ATOM_Atmosphere::setStarfieldTextureFileName (const ATOM_STRING &fileName)
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::setStarfieldTextureFileName);

	_starfieldTextureFileName = fileName;

	/*
	if (_cloudPlane)
	{
		ATOM_AUTOREF(ATOM_Texture) texture = _starfieldTextureFileName.empty() ? 0 : ATOM_CreateTextureResource (_starfieldTextureFileName.c_str());
		_cloudPlane->setStarfieldTexture (texture.get());
	}
	*/
}

const ATOM_STRING &ATOM_Atmosphere::getStarfieldTextureFileName (void) const
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::getStarfieldTextureFileName);

	return _starfieldTextureFileName;
}

void ATOM_Atmosphere::setStarfieldTextureRepeat (float val)
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::setStarfieldTextureRepeat);

	_starfieldTextureRepeat = val;

	/*
	if (_cloudPlane)
	{
		_cloudPlane->setStarfieldTextureRepeat (_starfieldTextureRepeat);
	}
	*/
}

float ATOM_Atmosphere::getStarfieldTextureRepeat (void) const
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::getStarfieldTextureRepeat);

	return _starfieldTextureRepeat;
}

static float scale (float cos, float fScaleDepth)
{
	float x = 1.f - cos;
	return fScaleDepth * ATOM_exp(-0.00287f + x * (0.459f + x * (3.83f + x * (-6.8f + x * 5.25f))));
}

static bool raySphereIntersection (const ATOM_Vector3f &p1, const ATOM_Vector3f &p2, float r, float *d, float *d1, float *d2)
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::raySphereIntersection);

   ATOM_Vector3f dp = p2 - p1;

   double a = dp.getSquaredLength ();
   double b = 2.f * dotProduct (dp, p1);
   double c = p1.getSquaredLength () - r * r;
   double bb4ac = b * b - 4 * a * c;
   if (ATOM_abs(a) < 0.0001f || bb4ac < 0.f) 
   {
      return false;
   }

   *d1 = (-b + sqrt(bb4ac)) / (2.0 * a);
   *d2 = (-b - sqrt(bb4ac)) / (2.0 * a);
   if (*d1 < 0.f)
   {
	   if (*d2 < 0.f)
	   {
		   return false;
	   }
	   else
	   {
		   *d = *d2;
		   return true;
	   }
   }
   else
   {
	   if (*d2 < 0.f)
	   {
		   *d = *d1;
		   return true;
	   }
	   else
	   {
		   *d = ATOM_min2(*d1, *d2);
		   return true;
	   }
   }
}

ATOM_Vector4f ATOM_Atmosphere::calcSkyColor2 (const ATOM_Vector3f &position, const ATOM_Vector3f &sunDir) const
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::calcSkyColor2);

	const int samples = 8;
	ATOM_Vector3f sunDirection = sunDir;

	float fOuterRadius = _skydomeRadiusVal;
	float fInnerRadius = _skydomeRadiusVal * (2.f - atmosphereHeightScale);
	float fScale = 1.f / (fOuterRadius - fInnerRadius);
	float fScaleDepth = _scaleDepthVal;
	float fScaleOverScaleDepth = fScale / fScaleDepth;
	ATOM_Vector3f invWaveLength (1.f/ATOM_pow(_lambdaRVal, 4.f), 1.f/ATOM_pow(_lambdaGVal, 4.f), 1.f/ATOM_pow(_lambdaBVal, 4.f));
	float fKr4Pi = _rayleighMultiplierVal * 4 * ATOM_Pi;
	float fKm4Pi = _mieMultiplierVal * 4 * ATOM_Pi;
	float fKrESun = _rayleighMultiplierVal * _sunIntensityVal;

	float cameraHeight = fInnerRadius + _cameraHeightVal * (fOuterRadius - fInnerRadius);
	ATOM_Vector3f cameraPos(0.f, cameraHeight, 0.f);

	ATOM_Vector3f pos = position;
	pos.y = ATOM_max2(pos.y, cameraHeight);

	ATOM_Vector3f ray = pos - cameraPos;
	float fFar = ray.getLength();
	ray /= fFar;

	ATOM_Vector3f start = cameraPos;
	float fHeight = start.getLength();
	float fDepth = ATOM_exp (fScaleOverScaleDepth * (fInnerRadius - cameraHeight));
	float fStartAngle = dotProduct (ray, start) / fHeight;
	float fStartOffset = fDepth * scale (fStartAngle, fScaleDepth);
	float fSampleLength  = fFar / samples;
	float fScaledLength = fSampleLength * fScale;
	ATOM_Vector3f sampleRay = ray * fSampleLength;
	ATOM_Vector3f samplePoint = start + sampleRay * 0.5f;

	ATOM_Vector3f color(0.f, 0.f, 0.f);
	for (int i = 0; i < samples; ++i)
	{
		float height = samplePoint.getLength();
		float depth = ATOM_exp(fScaleOverScaleDepth * (fInnerRadius - height));
		float lightAngle = dotProduct (sunDirection, samplePoint) / height;
		float cameraAngle = dotProduct (ray, samplePoint) / height;
		float scatter = (fStartOffset + depth * (scale(lightAngle, fScaleDepth) - scale(cameraAngle, fScaleDepth)));
		ATOM_Vector3f attenuate;
		attenuate.x = ATOM_exp(-scatter * (invWaveLength.x * fKr4Pi + fKm4Pi));
		attenuate.y = ATOM_exp(-scatter * (invWaveLength.y * fKr4Pi + fKm4Pi));
		attenuate.z = ATOM_exp(-scatter * (invWaveLength.z * fKr4Pi + fKm4Pi));
		attenuate *= (depth * fScaledLength);
		color += attenuate;
		samplePoint += sampleRay;
	}

	ATOM_Vector3f rayleighColor = color * invWaveLength * fKrESun;
	ATOM_Vector3f dir = cameraPos - pos;
	dir.normalize ();

	float cos = dotProduct (sunDirection, dir);
	float rayleighPhase = 0.75f * (1.f + cos * cos);

	ATOM_Vector3f result = rayleighPhase * rayleighColor;

	return ATOM_Vector4f(result.x, result.y, result.z, 1.f);
}

ATOM_Vector4f ATOM_Atmosphere::calcSkyColorWithoutPhase (const ATOM_Vector3f &direction, const ATOM_Vector3f &sunDir) const
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::calcSkyColorWithoutPhase);

	const int samples = 3;
	ATOM_Vector3f sunDirection = sunDir;

	float fOuterRadius = _skydomeRadiusVal;
	float fInnerRadius = _skydomeRadiusVal * (2.f - atmosphereHeightScale);
	float fScale = 1.f / (fOuterRadius - fInnerRadius);
	float fScaleDepth = _scaleDepthVal;
	float fScaleOverScaleDepth = fScale / fScaleDepth;
	ATOM_Vector3f invWaveLength (1.f/ATOM_pow(_lambdaRVal, 4.f), 1.f/ATOM_pow(_lambdaGVal, 4.f), 1.f/ATOM_pow(_lambdaBVal, 4.f));
	float fKr4Pi = _rayleighMultiplierVal * 4 * ATOM_Pi;
	float fKrESun = _rayleighMultiplierVal * _sunIntensityVal;

	float cameraHeight = fInnerRadius + _cameraHeightVal * (fOuterRadius - fInnerRadius);
	ATOM_Vector3f cameraPos(0.f, cameraHeight, 0.f);

	float d, d1, d2;
	raySphereIntersection (cameraPos, cameraPos + direction, fOuterRadius, 	&d, &d1, &d2);
	ATOM_Vector3f pos = cameraPos + direction * d;
	//pos.y = ATOM_max2(pos.y, cameraHeight);

	ATOM_Vector3f ray = pos - cameraPos;
	float fFar = ray.getLength();
	ray /= fFar;

	ATOM_Vector3f start = cameraPos;
	float fHeight = start.getLength();
	float fDepth = ATOM_exp (fScaleOverScaleDepth * (fInnerRadius - cameraHeight));
	float fStartAngle = dotProduct (ray, start) / fHeight;
	float fStartOffset = fDepth * scale (fStartAngle, fScaleDepth);
	float fSampleLength  = fFar / samples;
	float fScaledLength = fSampleLength * fScale;
	ATOM_Vector3f sampleRay = ray * fSampleLength;
	ATOM_Vector3f samplePoint = start + sampleRay * 0.5f;

	ATOM_Vector3f color(0.f, 0.f, 0.f);
	for (int i = 0; i < samples; ++i)
	{
		float height = samplePoint.getLength();
		float depth = ATOM_exp(fScaleOverScaleDepth * (fInnerRadius - height));
		float lightAngle = dotProduct (sunDirection, samplePoint) / height;
		float cameraAngle = dotProduct (ray, samplePoint) / height;
		float scatter = (fStartOffset + depth * (scale(lightAngle, fScaleDepth) - scale(cameraAngle, fScaleDepth)));
		ATOM_Vector3f attenuate;
		attenuate.x = ATOM_exp(-scatter * (invWaveLength.x * fKr4Pi));
		attenuate.y = ATOM_exp(-scatter * (invWaveLength.y * fKr4Pi));
		attenuate.z = ATOM_exp(-scatter * (invWaveLength.z * fKr4Pi));
		attenuate *= (depth * fScaledLength);
		color += attenuate;
		samplePoint += sampleRay;
	}

	return ATOM_Vector4f(color.x, color.y, color.z, 1.f);
}

ATOM_Vector4f ATOM_Atmosphere::calcSkyColor (const ATOM_Vector3f &direction, const ATOM_Vector3f &sunDir) const
{
	const int samples = 3;
	ATOM_Vector3f sunDirection = sunDir;

	float fOuterRadius = _skydomeRadiusVal;
	float fInnerRadius = _skydomeRadiusVal * (2.f - atmosphereHeightScale);
	float fScale = 1.f / (fOuterRadius - fInnerRadius);
	float fScaleDepth = _scaleDepthVal;
	float fScaleOverScaleDepth = fScale / fScaleDepth;
	ATOM_Vector3f invWaveLength (1.f/ATOM_pow(_lambdaRVal, 4.f), 1.f/ATOM_pow(_lambdaGVal, 4.f), 1.f/ATOM_pow(_lambdaBVal, 4.f));
	float fKr4Pi = _rayleighMultiplierVal * 4 * ATOM_Pi;
	float fKrESun = _rayleighMultiplierVal * _sunIntensityVal;

	float cameraHeight = fInnerRadius + _cameraHeightVal * (fOuterRadius - fInnerRadius);
	ATOM_Vector3f cameraPos(0.f, cameraHeight, 0.f);

	float d, d1, d2;
	raySphereIntersection (cameraPos, cameraPos + direction, fOuterRadius, 	&d, &d1, &d2);
	ATOM_Vector3f pos = cameraPos + direction * d;
	pos.y = ATOM_max2(pos.y, cameraHeight);

	ATOM_Vector3f ray = pos - cameraPos;
	float fFar = ray.getLength();
	ray /= fFar;

	ATOM_Vector3f start = cameraPos;
	float fHeight = start.getLength();
	float fDepth = ATOM_exp (fScaleOverScaleDepth * (fInnerRadius - cameraHeight));
	float fStartAngle = dotProduct (ray, start) / fHeight;
	float fStartOffset = fDepth * scale (fStartAngle, fScaleDepth);
	float fSampleLength  = fFar / samples;
	float fScaledLength = fSampleLength * fScale;
	ATOM_Vector3f sampleRay = ray * fSampleLength;
	ATOM_Vector3f samplePoint = start + sampleRay * 0.5f;

	ATOM_Vector3f color(0.f, 0.f, 0.f);
	for (int i = 0; i < samples; ++i)
	{
		float height = samplePoint.getLength();
		float depth = ATOM_exp(fScaleOverScaleDepth * (fInnerRadius - height));
		float lightAngle = dotProduct (sunDirection, samplePoint) / height;
		float cameraAngle = dotProduct (ray, samplePoint) / height;
		float scatter = (fStartOffset + depth * (scale(lightAngle, fScaleDepth) - scale(cameraAngle, fScaleDepth)));
		ATOM_Vector3f attenuate;
		attenuate.x = ATOM_exp(-scatter * (invWaveLength.x * fKr4Pi));
		attenuate.y = ATOM_exp(-scatter * (invWaveLength.y * fKr4Pi));
		attenuate.z = ATOM_exp(-scatter * (invWaveLength.z * fKr4Pi));
		attenuate *= (depth * fScaledLength);
		color += attenuate;
		samplePoint += sampleRay;
	}

	ATOM_Vector3f rayleighColor = color * invWaveLength * fKrESun;
	ATOM_Vector3f dir = cameraPos - pos;
	dir.normalize ();

	float cos = dotProduct (sunDirection, dir);
	float rayleighPhase = 0.75f * (1.f + cos * cos);

	ATOM_Vector3f result = rayleighPhase * rayleighColor;

	return ATOM_Vector4f(result.x, result.y, result.z, 1.f);
}

void ATOM_Atmosphere::syncColor (const ATOM_Vector3f &sunDir, bool ff)
{
	ATOM_STACK_TRACE(ATOM_Atmosphere::syncColor);

	if (!_skyDomeColors || _skyDomeColors->isContentLost())
	{
		_colorChanged = true;
	}

	if (!_colorChanged && _skyDomeColors)
	{
		return;
	}

	_colorChanged = false;

	int nRings = 60;
	int nSegments = 40;
	unsigned numVerts = (nRings + 1) * (nSegments + 1);
	unsigned numIndices = 2 * nRings * (nSegments + 1);

	if (_skyDomeVertices && _skyDomeVertices->getNumVertices() != numVerts)
	{
		_skyDomeVertices = 0;
		_skyDomeColors = 0;
		_skyDomeIndices = 0;
	}

	if (_skyDomeColors && _skyDomeColors->getNumVertices() != numVerts)
	{
		_skyDomeVertices = 0;
		_skyDomeColors = 0;
		_skyDomeIndices = 0;
	}

	if (!_skyDomeColors || !_skyDomeVertices || !_skyDomeIndices)
	{
		_skyDomeVertices = ATOM_GetRenderDevice()->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD, ATOM_USAGE_STATIC, numVerts, true);
		_skyDomeColors = ATOM_GetRenderDevice()->allocVertexArray (ATOM_VERTEX_ATTRIB_TEX1_3, ATOM_USAGE_DYNAMIC, numVerts, true);
		_skyDomeIndices = ATOM_GetRenderDevice()->allocIndexArray (ATOM_USAGE_STATIC, numIndices, false, true);
	}

	ATOM_Vector3f *pXYZ = (ATOM_Vector3f*)_skyDomeVertices->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	ATOM_Vector3f *pColors = (ATOM_Vector3f*)_skyDomeColors->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	unsigned short *pIndices = (unsigned short*)_skyDomeIndices->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);

	float fDeltaRingAngle = ( ATOM_Pi / nRings );
	float fDeltaSegAngle = ( ATOM_TwoPi / nSegments );
	unsigned short wVerticeIndex = 0 ; 

	float ratio = getInnerRadius()/getOuterRadius();
	float upAngle = ATOM_acos(ratio);
	float angle1 = upAngle / (nRings*0.9f);
	float angle2 = (ATOM_Pi - upAngle)/(nRings*0.1f);
	// Generate the group of rings for the sphere
	for( int ring = 0; ring < nRings + 1 ; ring++ )
	{
		float r0, y0;

		float alpha = (ring < nRings*0.9f) ? ring*angle1 : upAngle+angle2*(ring-nRings*0.9f);
		ATOM_sincos (alpha, &r0, &y0);

		// Generate the group of segments for the current ring
		for( int seg = 0; seg < nSegments + 1 ; seg++ )
		{
			float x0, z0;
			ATOM_sincos (seg * fDeltaSegAngle, &x0, &z0);
			x0 *= r0;
			z0 *= r0;

			ATOM_Vector3f p(x0, y0, z0);
			pXYZ->x = x0;
			pXYZ->y = y0;
			pXYZ->z = z0;
			pXYZ++;

			p *= _skydomeRadius.getCurrentValue();

			ATOM_Vector4f color = ff ? calcSkyColor2(p, sunDir) : calcSkyColorWithoutPhase (p, sunDir);
			*pColors++ = color.getVector3();

			if ( ring != nRings ) 
			{
				*pIndices++ = wVerticeIndex + (unsigned short)(nSegments + 1); 
				*pIndices++ = wVerticeIndex ; 
				wVerticeIndex++ ; 
			}
		} // end for seg 
	} // end for ring 

	_skyDomeVertices->unlock();
	_skyDomeColors->unlock();
	_skyDomeIndices->unlock();
}

ATOM_Material *ATOM_Atmosphere::getMaterial (void) const
{
	return _scatteringMaterial.get();
}

