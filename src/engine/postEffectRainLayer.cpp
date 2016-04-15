#include "StdAfx.h"
#include "parameterchannel.h"
#include "postEffectRainLayer.h"

// global for water puddles
static float g_sequenceTime = 1.0f;
static int g_sequenceCount = 33;
static float g_globalDensity = 1.0f;
static float g_puddleTiling = 0.25f;
static float g_puddleAmount = 1.0f;
static float g_attenStartDistance = 300.0f;
static float g_attenEndDistance = 500.0f;

struct sRainLayerParameterChannel
{
	sRainLayerParameterChannel (void)
	{
		ATOM_ParameterChannel::registerChannel ("RainLayer_PuddleTexture", &ATOM_RainLayerEffect::rainPuddleTexture_Callback, 0, 0, ATOM_MaterialParam::ParamType_Texture);
		ATOM_ParameterChannel::registerChannel ("RainLayer_PuddleParams", &ATOM_RainLayerEffect::rainPuddleParam_Callback, 0, 0, ATOM_MaterialParam::ParamType_Vector);
		ATOM_ParameterChannel::registerChannel ("RainLayer_LayerParams", &ATOM_RainLayerEffect::rainLayerParam_Callback, 0, 0, ATOM_MaterialParam::ParamType_Vector);
	}

	~sRainLayerParameterChannel (void)
	{
		ATOM_ParameterChannel::unregisterChannel("RainLayer_PuddleTexture");
		ATOM_ParameterChannel::unregisterChannel("RainLayer_PuddleParams");
		ATOM_ParameterChannel::unregisterChannel("RainLayer_LayerParams");
	}
};
sRainLayerParameterChannel rainLayerParamChannel;

///////////////////////////////////////////////////////////////////////
ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_RainLayerEffect)
	ATOM_ATTRIBUTES_BEGIN(ATOM_RainLayerEffect)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_RainLayerEffect, "BumpShiftTexture", getBumpShiftTextureFileName, setBumpShiftTextureFileName, "", "group=ATOM_RainLayerEffect;type=vfilename;desc='雨层法线偏移纹理'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_RainLayerEffect, "SequenceTime", getSequenceTime, setSequenceTime, 1.0f, "group=ATOM_RainLayerEffect;desc='序列帧总时间'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_RainLayerEffect, "SequenceFrameCount", getSequenceFrameCount, setSequenceFrameCount, 33, "group=ATOM_RainLayerEffect;desc='序列帧总帧数'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_RainLayerEffect, "GlobalDensity", getGlobalDensity, setGlobalDensity, 1.0f, "group=ATOM_RainLayerEffect;desc='雨层全局强度'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_RainLayerEffect, "PuddleTiling", getPuddleTiling, setPuddleTiling, 0.05f, "group=ATOM_RainLayerEffect;desc='Puddle平铺因子'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_RainLayerEffect, "PuddleAmount", getPuddleAmount, setPuddleAmount, 1.0f, "group=ATOM_RainLayerEffect;desc='Puddle强度因子'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_RainLayerEffect, "AttenStartDistance", getAttenStartDistance, setAttenStartDistance, 300.0f, "group=ATOM_RainLayerEffect;desc='衰减开始距离'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_RainLayerEffect, "AttenEndDistance", getAttenEndDistance, setAttenEndDistance, 500.0f, "group=ATOM_RainLayerEffect;desc='衰减结束距离'")
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_RainLayerEffect, ATOM_PostEffect)

ATOM_RainLayerEffect::ATOM_RainLayerEffect (void)
{
	//_qualityLevel = RAINLAYER_QUALITY_MEDIUM;
	_material = 0;
	//_rainWavesTex = 0;
	_rainBumpShiftTex = 0;

	_sequenceTime = 1.0f;
	_sequenceCount = 33;
	_globalDensity = 1.0f;
	_puddleTiling = 0.25f;
	_puddleAmount = 1.0f;
	_attenStartDistance = 300.0f;
	_attenEndDistance = 500.0f;

	_materialError = false;

	_gammaFlag = BEFORE_GAMMA_CORRECT;

	// 开启
	ATOM_RenderSettings::enableRainLayer(true);

	// 刷新水效果
	ATOM_RenderSettings::setWaterQuality(ATOM_RenderSettings::getWaterQuality());
}

void ATOM_RainLayerEffect::rainPuddleTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	static ATOM_VECTOR<ATOM_AUTOREF(ATOM_Texture)>		puddleTexs;
	static bool bTexLoaded = true;

	if( puddleTexs.empty() && bTexLoaded )
	{
		for( int i = 0; i < g_sequenceCount; ++i )
		{
			char rainpuddlename[128]= {0};
			sprintf(rainpuddlename,"/textures/Rain/puddle%d.dds",i);

			ATOM_AUTOREF(ATOM_Texture) puddleTex = ATOM_CreateTextureResource(rainpuddlename,ATOM_PIXEL_FORMAT_UNKNOWN,1/*ATOM_LoadPriority_IMMEDIATE*/);
			if( puddleTex )
			{
				puddleTexs.push_back(puddleTex);
			}
			else
			{
				bTexLoaded = false;
				ATOM_LOGGER::warning("the caustics sequence texture load failed!\n");
				break;
			}
		}
	}

	if( bTexLoaded && !puddleTexs.empty() )
	{
		float timeInterval = g_sequenceTime / g_sequenceCount;
		int _frame = (int( ATOM_APP->getFrameStamp().currentTick * 0.001f / timeInterval )) % g_sequenceCount;
		if( _frame >= 0 && _frame < puddleTexs.size() )
			param->setTexture (puddleTexs[_frame].get());
			//((ATOM_MaterialTextureParam*)param)->setValue (puddleTexs[_frame].get());
	}
}

void ATOM_RainLayerEffect::rainPuddleParam_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	const float fTime = ATOM_APP->getFrameStamp().currentTick * 0.333f;
	const ATOM_Vector4f vPuddleParams = ATOM_Vector4f(	sinf(fTime) * 0.25f, 
														cosf(fTime) * 0.25f, 
														g_puddleAmount, 
														g_puddleTiling	);

	param->setVector (vPuddleParams);
	if (vsConstantPtr)
	{
		*vsConstantPtr = *param->v;
	}
	if (psConstantPtr)
	{
		*psConstantPtr = *param->v;
	}
	//((ATOM_MaterialVectorParam*)param)->setValue (vPuddleParams);
}

void ATOM_RainLayerEffect::rainLayerParam_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	const ATOM_Vector4f vRainLayerParams = ATOM_Vector4f( g_globalDensity, g_attenStartDistance, g_attenEndDistance, 0 );
	param->setVector (vRainLayerParams);
	if (vsConstantPtr)
	{
		*vsConstantPtr = *param->v;
	}
	if (psConstantPtr)
	{
		*psConstantPtr = *param->v;
	}
	//((ATOM_MaterialVectorParam*)param)->setValue (vRainLayerParams);
}

ATOM_RainLayerEffect::~ATOM_RainLayerEffect (void)
{
	_rainWavesTexs.clear();
	_rainBumpShiftTex = 0;
	_material = 0;

	// 禁用
	ATOM_RenderSettings::enableRainLayer(false);
	// 刷新水效果
	ATOM_RenderSettings::setWaterQuality(ATOM_RenderSettings::getWaterQuality());
}

//void ATOM_RainLayerEffect::setQualityLevel(int quality)
//{
//	if( _qualityLevel != quality )
//	{
//		_qualityLevel = quality;
//	}
//}
//int ATOM_RainLayerEffect::getQualityLevel()const
//{
//	return _qualityLevel;
//}

void ATOM_RainLayerEffect::setSequenceTime(const float sequenceTime)
{
	_sequenceTime = sequenceTime;
	g_sequenceTime = _sequenceTime;
}
const float ATOM_RainLayerEffect::getSequenceTime(void) const
{
	return _sequenceTime;
}

void ATOM_RainLayerEffect::setSequenceFrameCount(const int sequenceFrameCount)
{
	_sequenceCount = sequenceFrameCount;
	g_sequenceCount = _sequenceCount;
}
const int ATOM_RainLayerEffect::getSequenceFrameCount(void) const
{
	return _sequenceCount;
}

void ATOM_RainLayerEffect::setGlobalDensity(const float density)
{
	_globalDensity = density;
	g_globalDensity = _globalDensity;
}
const float ATOM_RainLayerEffect::getGlobalDensity(void) const
{
	return _globalDensity; 
}

void ATOM_RainLayerEffect::setPuddleTiling(const float puddleTiling)
{
	_puddleTiling = puddleTiling;
	g_puddleTiling = _puddleTiling;
}
const float ATOM_RainLayerEffect::getPuddleTiling(void) const
{
	return _puddleTiling;
}

void ATOM_RainLayerEffect::setPuddleAmount(const float puddleAmount)
{
	_puddleAmount = puddleAmount;
	g_puddleAmount = _puddleAmount;
}
const float ATOM_RainLayerEffect::getPuddleAmount(void) const
{
	return _puddleAmount;
}

void ATOM_RainLayerEffect::setAttenStartDistance(const float distance)
{
	_attenStartDistance = distance;
	g_attenStartDistance = _attenStartDistance;
}
const float ATOM_RainLayerEffect::getAttenStartDistance(void) const
{
	return _attenStartDistance;
}
void ATOM_RainLayerEffect::setAttenEndDistance(const float distance)
{
	_attenEndDistance = distance;
	g_attenEndDistance = _attenEndDistance;
}
const float ATOM_RainLayerEffect::getAttenEndDistance(void) const
{
	return _attenEndDistance;
}

void ATOM_RainLayerEffect::setBumpShiftTextureFileName (const ATOM_STRING &filename)
{
	if ( !filename.empty () && _rainBumpShiftFileName != filename )
	{
		_rainBumpShiftFileName = filename;

		ATOM_RenderDevice *device = ATOM_GetRenderDevice();

		ATOM_AUTOREF(ATOM_Texture) newTexture = ATOM_CreateTextureResource (_rainBumpShiftFileName.c_str(), 
																			ATOM_PIXEL_FORMAT_UNKNOWN, 
																			ATOM_LoadPriority_IMMEDIATE	);

		if (newTexture != _rainBumpShiftTex)
		{
			_rainBumpShiftTex = newTexture;
			_material = 0;
		}
	}
	else if ( filename.empty () && _rainBumpShiftTex)
	{
		_rainBumpShiftTex = 0;
		_material = 0;
		_rainBumpShiftFileName = "";
	}
}
const ATOM_STRING & ATOM_RainLayerEffect::getBumpShiftTextureFileName (void) const
{
	return _rainBumpShiftFileName;
}

///////////////////////////////////////////////////////////////////////////////////
void ATOM_RainLayerEffect::enable (int b)
{
	ATOM_PostEffect::enable(b);
	//ATOM_RenderSettings::enableRainLayer( b==0 ? false:true );
}
int ATOM_RainLayerEffect::isEnabled (void) const
{
	bool bRainExist = _chain && _chain->getEffect("ATOM_RainEffect");
	if( !bRainExist )
		return 0;

	if( !ATOM_RenderSettings::isRainEnabled() )
		return 0;

	if( !ATOM_RenderSettings::isRainLayerEnabled() )
		return 0;

	return ATOM_PostEffect::isEnabled() ;
}

bool ATOM_RainLayerEffect::render (ATOM_RenderDevice *device)
{
	if( !init(device) )
		return false;

	////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 0 
	ATOM_PostEffect *prevEffect = getPreviousEffect ();
	ATOM_Texture *inputTexture = prevEffect ? prevEffect->getRenderTarget() : _chain->getInputTexture();
#else
	ATOM_AUTOREF(ATOM_Texture) inputTexture = getSourceInputTex();
#endif
	if (!inputTexture)
	{
		return false;
	}

//	float timeInterval = _sequenceTime / _sequcenCount;
//	int frame = (int( ATOM_APP->getFrameStamp().currentTick * 0.001f / timeInterval )) % _sequcenCount;
//#if 0
//	ATOM_LOGGER::log("+++++ Rain Layer sequence : %d ++++\n", frame);
//#endif
//	_material->getParameterTable()->setTexture ("rainWavesTexture", _rainWavesTexs[frame].get()/*_rainWavesTex.get()*/);
	_material->getParameterTable()->setTexture ("rainBumpShiftTexture", _rainBumpShiftTex.get());
	_material->getParameterTable()->setTexture ("inputTexture", inputTexture.get());

	float viewAspectRatio = ATOM_RenderScheme::getCurrentScene()->getCamera()->getAspect();
	_material->getParameterTable()->setFloat("viewAspectRatioRain", viewAspectRatio);

	float invTanHalfFov = ATOM_RenderScheme::getCurrentScene()->getCamera()->getInvTanHalfFovy();
	_material->getParameterTable()->setFloat("invTanHalfFovRain", invTanHalfFov);

	ATOM_Camera *camera = ATOM_RenderScheme::getCurrentScene()->getCamera();
	float q = camera->getFarPlane()/(camera->getFarPlane()-camera->getNearPlane());
	float mq = camera->getNearPlane()*q;
	_material->getParameterTable()->setVector("linearDepthParamRain",ATOM_Vector4f(mq, q, 0.f, 0.f));

	ATOM_Vector3f eye = camera->getPosition();
	_material->getParameterTable()->setVector("eyeWSPos", eye);
	
	ATOM_Matrix4x4f matView;
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_VIEW, matView);
	_material->getParameterTable()->setMatrix44("matView", matView);

	ATOM_Matrix4x4f matViewIT;
	ATOM_GetRenderDevice()->getTransform (ATOM_MATRIXMODE_VIEW, matViewIT);
	matViewIT.invertTransposeAffine();
	_material->getParameterTable()->setMatrix44("matViewIT", matViewIT);

	
	const float fTime = ATOM_APP->getFrameStamp().currentTick * 0.333f;
	const ATOM_Vector4f vPuddleParams = ATOM_Vector4f(	sinf(fTime) * 0.25f, 
														cosf(fTime) * 0.25f, 
														_puddleAmount, 
														_puddleTiling	);
	_material->getParameterTable()->setVector ("rainPuddleParams", vPuddleParams);

	const ATOM_Vector4f vRainLayerParams = ATOM_Vector4f( _globalDensity, _attenStartDistance, _attenEndDistance, 0 );
	_material->getParameterTable()->setVector ("rainLayerParams", vRainLayerParams);

	device->setRenderTarget (0, getRenderTarget());
	device->setViewport (0, ATOM_Rect2Di(0, 0, getRenderTarget()->getWidth(), getRenderTarget()->getHeight()));

	drawTexturedFullscreenQuad (device, _material.get(), inputTexture->getWidth(), inputTexture->getHeight());

	return true;
}

bool ATOM_RainLayerEffect::init(ATOM_RenderDevice *device)
{
	if( !ATOM_PostEffect::init(device) )
		return false;

	if( !_material )
	{
		if (_materialError)
		{
			return false;
		}

		_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/rainLayer.mat");
		if(_material)
		{
			_material->setActiveEffect ("default");
		}
		else
		{
			_materialError = true;
			return false;
		}
	}

	/*_rainWavesTex = ATOM_CreateTextureResource("/textures/puddle0.dds",ATOM_PIXEL_FORMAT_UNKNOWN,false);
		if( !_rainWavesTex )
			return false;*/
	//if( _rainWavesTexs.empty() )
	//{
	//	for( int i = 0; i < _sequenceCount; ++i )
	//	{
	//		char rainpuddlename[128]= {0};
	//		sprintf(rainpuddlename,"/textures/Rain/puddle%d.dds",i);

	//		ATOM_AUTOREF(ATOM_Texture) puddleTex = ATOM_CreateTextureResource(rainpuddlename,ATOM_PIXEL_FORMAT_UNKNOWN,1/*ATOM_LoadPriority_IMMEDIATE*/);
	//		if( !puddleTex )
	//			return false;

	//		_rainWavesTexs.push_back(puddleTex);
	//	}
	//	ATOM_ASSERT(_rainWavesTexs.size()==_sequcenCount);
	//}

	if( !_rainBumpShiftTex )
	{
		_rainBumpShiftTex = ATOM_CreateTextureResource("/textures/perlinNoiseNormal.dds",ATOM_PIXEL_FORMAT_UNKNOWN,1/*ATOM_LoadPriority_IMMEDIATE*/);
		if( !_rainBumpShiftTex )
			return false;
	}
	
	return true;
}

bool ATOM_RainLayerEffect::destroy()
{
	_rainWavesTexs.clear();
	_rainBumpShiftTex = 0;
	_material = 0;

	return ATOM_PostEffect::destroy();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////