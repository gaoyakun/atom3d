#include "StdAfx.h"
#include "plugin.h"
#include "editor.h"
#include "app.h"
#include "gridnode.h"
#include "clipboard.h"
#include "scene_prop_editor.h"

static const char *PARAMNAME_STATISTICS = "性能概况";

//--- wangjian added ---//
static const char *PARAMNAME_LIGHT = "光源统计";
//----------------------//

static const char *PARAMNAME_CAMERA = "摄像机数值";
static const char *PARAMNAME_SKYRADIUS = "天空半径";
static const char *PARAMNAME_SUNINTENSITY = "阳光强度";
static const char *PARAMNAME_SUNBRIGHTNESS = "太阳亮度";
static const char *PARAMNAME_FOGDENSITY = "大气浓度指数";
static const char *PARAMNAME_FOGNEAR = "雾效近距离";
static const char *PARAMNAME_FOGFAR = "雾效远距离";
static const char *PARAMNAME_DRAWSKY = "渲染天空";
static const char *PARAMNAME_SUNDIRECTION = "阳光方向";
static const char *PARAMNAME_G = "G";
static const char *PARAMNAME_MIEMULTIPLIER = "Mie Multiplier";
static const char *PARAMNAME_RAYLEIGHMULTIPLIER = "Rayleigh Multiplier";
static const char *PARAMNAME_EXPOSURE = "曝光程度";
static const char *PARAMNAME_RAYLEIGH = "Rayleigh";
static const char *PARAMNAME_LAMBDA_R = "Lambda Red";
static const char *PARAMNAME_LAMBDA_G = "Lambda Green";
static const char *PARAMNAME_LAMBDA_B = "Lambda Blue";
static const char *PARAMNAME_CAMERA_HEIGHT = "Camera Height";
static const char *PARAMNAME_FADE_FAR = "远截面";
static const char *PARAMNAME_CLOUD_COVER = "云层密度";
//--------------------- wangjian added -----------------------//
static const char *PARAMNAME_SKY_RENDERMODE = "天空渲染模式";
static const char *PARAMNAME_SKY_TEXTURE = "天空纹理";
static const char *PARAMNAME_SKYDOME_MODEL = "天空球模型";
static const char *PARAMNAME_SKYDOME_OFFSET = "天空球坐标偏移";
static const char *PARAMNAME_SKYDOME_SCALE = "天空球缩放";
static const char *PARAMNAME_FOGCOLOR = "雾颜色";
//-----------------------------------------------------------//
#if 0
static const char *PARAMNAME_CLOUD_MOVESPEED = "Cloud Move";
static const char *PARAMNAME_CLOUD_MOVEDIR = "Cloud Dir";
static const char *PARAMNAME_MOON_COLOR = "Moon Color";
static const char *PARAMNAME_MOON_DIR = "Moon Dir";
static const char *PARAMNAME_MOON_SIZE = "Moon Size";
static const char *PARAMNAME_STARFIELD_TEXTURE_REPEAT = "Star Field Repeat";
static const char *PARAMNAME_SSAO_SCALE = "Scale";
static const char *PARAMNAME_SSAO_BIAS = "Bias";
static const char *PARAMNAME_SSAO_RADIUS = "Radius";
static const char *PARAMNAME_SSAO_INTENSITY = "Intensity";
#endif
static const char *GROUP_ATMOSPHERE = "大气层";
static const char *GROUP_SHADOW = "阴影效果(Shadow)";
static const char *GROUP_BLOOM = "泛光效果(Bloom)";
static const char *GROUP_SSAO = "屏幕环境遮挡(SSAO)";
#if 0
static const char *GROUP_EDIT = "编辑功能";
#endif

static ATOM_Vector3f getCameraPosition (ATOM_Camera *camera)
{
	return camera->getViewMatrix().getRow3(3);
}

static ATOM_Vector4f getCameraOrientation (ATOM_Camera *camera)
{
	ATOM_Matrix4x4f m = camera->getViewMatrix ();
	ATOM_Vector3f pos, scale;
	ATOM_Matrix4x4f rot;
	m.decompose (pos, rot, scale);
	ATOM_Quaternion q;
	q.fromMatrix (rot);
	return ATOM_Vector4f(q.x, q.y, q.z, q.w);
}

static float getCameraFOV (ATOM_Camera *camera)
{
	return ATOM_atan(1.f / camera->getInvTanHalfFovy()) * 2.f;
}

static float getCameraNear (ATOM_Camera *camera)
{
	return camera->getNearPlane ();
}

static float getCameraFar (ATOM_Camera *camera)
{
	return camera->getFarPlane ();
}

void ScenePropCommandCallback::setScene (ATOM_DeferredScene *scene)
{
	_scene = scene;
}

void ScenePropCommandCallback::callback (ATOMX_TWCommandEvent *event)
{
	if (!_scene)
	{
		return;
	}

	switch (event->id)
	{
	case BUTTONID_CREATEATMOSPHERE:
		{
#ifdef USE_ATMOSPHERE2
			ATOM_HARDREF(ATOM_Sky) sky;
#else
			ATOM_HARDREF(ATOM_Atmosphere) atmosphere;
#endif
			if (sky->load (ATOM_GetRenderDevice()))
			{
				_scene->getRootNode()->appendChild (sky.get());
				_editor->setParamChanged (true);
				_editor->getEditor()->setDocumentModified (true);

				// After refresh THIS pointer will be destroyed!!
				_editor->refresh ();
			}
			break;
		}
	case BUTTONID_CLOUDTEXTURE:
		{
			AS_Editor *editor = _editor->getEditor();
			unsigned n = editor->getOpenImageFileNames (false, false);
			if (n == 1)
			{
				ATOM_Sky *sky = _editor->getSky ();
				if (sky)
				{
					sky->setCloudTextureFileName (editor->getOpenedImageFileName (0));
				}
			}
			break;
		}
	case BUTTONID_SKYMATERIAL:
		{
			AS_Editor *editor = _editor->getEditor();
			unsigned n = editor->getOpenFileNames ("mat", "atom3d material file|*.mat|", false, false);
			if (n == 1)
			{
				ATOM_Sky *sky = _editor->getSky ();
				if (sky)
				{
					sky->setCloudMaterialFileName (editor->getOpenedFileName (0));
				}
			}
			break;
		}
	case BUTTONID_COPY:
		{
			_editor->copyContents();
			break;
		}
	case BUTTONID_COPYALLTIME:
		{
			_editor->copyContentsAllTime();
			break;
		}
	case BUTTONID_PASTE:
		{
			_editor->pasteContents();
			break;
		}
	case BUTTONID_APPLY_ATMOSPHERE:
		{
			_editor->applyAtmosphereSettingsToAllTime();
			break;
		}
	case BUTTONID_APPLY_SHADOW:
		{
			_editor->applyShadowSettingsToAllTime();
			break;
		}
	case BUTTONID_APPLY_MOON:
		{
			_editor->applyMoonSettingsToAllTime();
			break;
		}
	case BUTTONID_APPLY_BLOOM:
		{
			_editor->applyBloomSettingsToAllTime();
			break;
		}
	//-------- wangjian added --------//
	case BUTTONID_SKYTEXTURE:
		{
			AS_Editor *editor = _editor->getEditor();
			unsigned n = editor->getOpenImageFileNames (false, false);
			if (n == 1)
			{
				ATOM_Sky *sky = _editor->getSky ();
				if (sky)
				{
					sky->setSkyTextureFile(editor->getOpenedImageFileName (0));
				}
			}
			break;
		}
	case BUTTONID_SKYDOMEMODEL:
		{
			AS_Editor *editor = _editor->getEditor();
			unsigned n = editor->getOpenImageFileNames (false, false);
			if (n == 1)
			{
				ATOM_Sky *sky = _editor->getSky ();
				if (sky)
				{
					sky->setSkyDomeModelFile(editor->getOpenedImageFileName (0));
				}
			}
			break;
		}
	//--------------------------------//
	}
}

//--- wangjian added ---//
void ScenePropCommandCallback::callback (ATOMX_TWValueChangedEvent *event)
{
	if (!_scene)
	{
		return;
	}

	switch (event->id)
	{
	case BUTTONID_SKYRENDERMODE:
		{
			ATOM_Sky *sky = _editor->getSky ();
			if (sky)
			{
				int rendermode = event->newValue.getI();
				sky->setRenderMode(rendermode);
			}
			break;
		}
	}
}
//----------------------//

ScenePropEditor::ScenePropEditor (AS_Editor *editor, ATOM_DeferredScene *scene)
{
	_editor = editor;
	_bar = 0;
	_isShown = false;
	_callback = 0;

	setScene (scene);

	_fps = ATOM_APP->getFrameStamp().FPS;
	_numDrawCalls = ATOM_GetRenderDevice()->getNumDrawCalls();
	_numPrimDrawn = ATOM_GetRenderDevice()->getNumPrimitivesDrawn();
	_renderTargetScale = ATOM_RenderSettings::getDeferredBufferScale ();

	_modified = false;

	//--- wangjian added ---//
	_numDrawCalls_shadowed_approximate = ATOM_RenderScheme::_shadowBatchCount;
	_numBatches_shadowed_approximate = ATOM_RenderScheme::_shadowDPCount;

	_numDirectionalLight = 0;
	_numPointLight = 0;
	//----------------------//
}

ScenePropEditor::~ScenePropEditor (void)
{
	ATOM_DELETE(_bar);
	ATOM_DELETE(_callback);
}

void ScenePropEditor::setScene (ATOM_DeferredScene *scene)
{
	if (scene == _scene)
	{
		return;
	}

	_scene = scene;
#ifdef USE_ATMOSPHERE2
	_atmosphere = _scene ? ATOMX_FindFirstNodeByClassName (_scene->getRootNode(), ATOM_Sky::_classname()) : NULL;
#else
	_atmosphere = _scene ? ATOMX_FindFirstNodeByClassName (_scene->getRootNode(), ATOM_Atmosphere2::_classname()) : NULL;
#endif

	if (_callback)
	{
		_callback->setScene (_scene);
	}
	else
	{
		_callback = ATOM_NEW(ScenePropCommandCallback, this, _scene);
	}

	updateTime ();

	_paramsBackup = _params;

	if (_scene)
	{
		_cameraPos = getCameraPosition (_scene->getCamera());
		_cameraOrientation = getCameraOrientation (_scene->getCamera());
		_cameraFOV = getCameraFOV (_scene->getCamera());
		_cameraNear = getCameraNear (_scene->getCamera());
		_cameraFar = getCameraFar (_scene->getCamera());
	}

	refresh ();
}

void ScenePropEditor::loadSceneParams (ParamBlock &params, float time, unsigned mask)
{
	if (!_scene)
	{
		return;
	}

	if ((mask & SCENE_PROP_ATMOSPHERE) != 0)
	{
		if (_atmosphere)
		{
			loadAtmosphereParams (params, time);
		}
		params._atmosphereParams._ambientColor = _scene->getAmbientLight();
	}

	if ((mask & SCENE_PROP_SHADOW) != 0)
	{
		params._shadowParams._splitLambda = _scene->getSplitLambda ();
		params._shadowParams._shadowDistance = _scene->getShadowDistance ();
		params._shadowParams._lightDistance = _scene->getLightDistance ();
		params._shadowParams._sceneScale = _scene->getSceneScale ();
		params._shadowParams._lightSize = _scene->getLightSize ();
		params._shadowParams._depthBias = _scene->getDepthBias ();
		params._shadowParams._fadeMin = _scene->getShadowFadeMin ();
		params._shadowParams._fadeMax = _scene->getShadowFadeMax ();
		//--- wangjian added ---//
		// 阴影相关
		params._shadowParams._shadowBlur = _scene->getShadowBlur();
		params._shadowParams._shadowSlopeBias = _scene->getShadowSlopeBias();
		params._shadowParams._shadowDensity = _scene->getShadowDensity();
		//----------------------//
	}

	params._bloomParams._threshold = _scene->getBloomThreshold();
	params._bloomParams._offset = _scene->getBloomOffset();
	ATOM_DeferredRenderScheme *deferredScheme = dynamic_cast<ATOM_DeferredRenderScheme*>(_scene->getRenderScheme());
	if (deferredScheme)
	{
		params._bloomParams._middleGray = deferredScheme->getHDRMiddleGray();
	}
	//--- wangjian added ---//
	// HDR相关
	params._bloomParams._bloomMultiplier = _scene->getBloomMultiplier();
	params._bloomParams._hdr_enable = _scene->getHDRToggle() ? 1 : 0;
	_scene->getHDRFilmCurveParams(params._bloomParams._filmCurveShoulder,params._bloomParams._filmCurveMiddleTone,params._bloomParams._filmCurveToe,params._bloomParams._filmCurveWhitePoint);
	_scene->getHDRColorParams(params._bloomParams._HDRSaturate,params._bloomParams._HDRContrast,params._bloomParams._HDRColorBalance);
	//----------------------//

	params._ssaoParams._scale = ATOM_RenderSettings::getSSAOScale ();
	params._ssaoParams._bias = ATOM_RenderSettings::getSSAOBias ();
	params._ssaoParams._radius = ATOM_RenderSettings::getSSAORadius ();
	params._ssaoParams._intensity = ATOM_RenderSettings::getSSAOIntensity ();
	params._ssaoParams._factor = ATOM_RenderSettings::getSSAODownsampleFactor ();

#ifndef USE_ATMOSPHERE2
	if (_atmosphere && (mask & SCENE_PROP_MOON) != 0)
	{
		params._moonParams._moonColor = _atmosphere->getMoonColorKey (time);
		params._moonParams._moonDir = _atmosphere->getMoonDirKey (time).getVector3();
		params._moonParams._moonSize = _atmosphere->getMoonSizeKey (time);
		params._moonParams._starfieldTextureRepeat = _atmosphere->getStarfieldTextureRepeat();
	}
#endif
}

void ScenePropEditor::updateTime (void)
{
	checkParamChanged ();

	float currentTime = ATOM_TimeManager::getCurrentTime ();
	_time = currentTime * (24 / ATOM_TimeManager::numTimeKeys);

	loadSceneParams (_params, currentTime, SCENE_PROP_ALL);

	_paramsBackup = _params;

}

void ScenePropEditor::setPosition (int x, int y, int w, int h)
{
	if (!_bar)
	{
		createBar ();
	}

	_bar->setBarPosition (x, y);
	_bar->setBarSize (w, h);
}

void ScenePropEditor::checkParamChanged (void)
{
	if (!_modified && memcmp (&_params, &_paramsBackup, sizeof(ParamBlock)))
	{
		_modified = true;
		_editor->setDocumentModified (true);
	}
}

void ScenePropEditor::setParamChanged (bool b)
{
	if (!b)
	{
		memcpy (&_paramsBackup, &_params, sizeof(ParamBlock));
		_modified = false;
	}
	else
	{
		_modified = true;
	}
}

void ScenePropEditor::saveSceneParams (const ParamBlock &params, float time, unsigned mask)
{
	if (!_scene)
	{
		return;
	}

	if ((mask & SCENE_PROP_ATMOSPHERE) != 0)
	{
		if (_atmosphere)
		{
			saveAtmosphereParams (params, time);
		}
		_scene->setAmbientLight (params._atmosphereParams._ambientColor);
	}

	if ((mask & SCENE_PROP_SHADOW) != 0)
	{
		_scene->setSplitLambda (params._shadowParams._splitLambda);
		_scene->setShadowDistance (params._shadowParams._shadowDistance);
		_scene->setLightDistance (params._shadowParams._lightDistance);
		_scene->setSceneScale (params._shadowParams._sceneScale);
		_scene->setLightSize (params._shadowParams._lightSize);
		_scene->setDepthBias (params._shadowParams._depthBias);
		_scene->setShadowFadeMin (params._shadowParams._fadeMin);
		_scene->setShadowFadeMax (params._shadowParams._fadeMax);
		//--- wangjian added ---//
		// 阴影相关
		_scene->setShadowBlur (params._shadowParams._shadowBlur);
		_scene->setShadowSlopeBias (params._shadowParams._shadowSlopeBias);
		_scene->setShadowDensity(params._shadowParams._shadowDensity);
		//----------------------//

	}

	if ((mask & SCENE_PROP_BLOOM) != 0)
	{
		_scene->setBloomThreshold(params._bloomParams._threshold);
		_scene->setBloomOffset(params._bloomParams._offset);

		ATOM_DeferredRenderScheme* deferredScheme = dynamic_cast<ATOM_DeferredRenderScheme*>(_scene->getRenderScheme());
		if (deferredScheme)
		{
			deferredScheme->setHDRMiddleGray(params._bloomParams._middleGray);
		}

		//--- wangjian added ---//
		// HDR相关
		_scene->setBloomMultiplier(params._bloomParams._bloomMultiplier);
		_scene->setHDRToggle(params._bloomParams._hdr_enable);
		_scene->setHDRFilmCurveParams(params._bloomParams._filmCurveShoulder,params._bloomParams._filmCurveMiddleTone,params._bloomParams._filmCurveToe,params._bloomParams._filmCurveWhitePoint);
		_scene->setHDRColorParams(params._bloomParams._HDRSaturate,params._bloomParams._HDRContrast,params._bloomParams._HDRColorBalance);
		//----------------------//
	}

	if ((mask & SCENE_PROP_SSAO) != 0)
	{
		ATOM_RenderSettings::setSSAOScale (params._ssaoParams._scale);
		ATOM_RenderSettings::setSSAOBias (params._ssaoParams._bias);
		ATOM_RenderSettings::setSSAORadius (params._ssaoParams._radius);
		ATOM_RenderSettings::setSSAOIntensity (params._ssaoParams._intensity);
		ATOM_RenderSettings::setSSAODownsampleFactor (params._ssaoParams._factor);
	}

#ifndef USE_ATMOSPHERE2
	if (_atmosphere && (mask & SCENE_PROP_MOON) != 0)
	{
		_atmosphere->setMoonColorKey (time, params._moonParams._moonColor);
		_atmosphere->setMoonDirKey (time, ATOM_Vector4f(params._moonParams._moonDir));
		_atmosphere->setMoonSizeKey (time, params._moonParams._moonSize);
		_atmosphere->setStarfieldTextureRepeat (params._moonParams._starfieldTextureRepeat);
	}
#endif
}

void ScenePropEditor::frameUpdate (void)
{
	_fps = ATOM_APP->getFrameStamp().FPS;
	_numDrawCalls = ATOM_GetRenderDevice()->getNumDrawCalls();
	_numPrimDrawn = ATOM_GetRenderDevice()->getNumPrimitivesDrawn();
	ATOM_RenderSettings::setDeferredBufferScale (_renderTargetScale);

	if (_scene)
	{
		_cameraPos = getCameraPosition (_scene->getCamera());
		_cameraOrientation = getCameraOrientation (_scene->getCamera());
		_cameraFOV = getCameraFOV (_scene->getCamera());
		_cameraNear = getCameraNear (_scene->getCamera());
		_cameraFar = getCameraFar (_scene->getCamera());

		ATOM_DeferredRenderScheme *deferredScheme = dynamic_cast<ATOM_DeferredRenderScheme*>(_scene->getRenderScheme());
		if (deferredScheme)
		{
			_numDirectionalLight = deferredScheme->getNumDirectionalLight();
			_numPointLight = deferredScheme->getNumPointLight();
		}
	}

	checkParamChanged ();

	float currentTime = ATOM_TimeManager::getCurrentTime();
	saveSceneParams (_params, currentTime, SCENE_PROP_ALL);

	//--- wangjian added ---//
	_numDrawCalls_shadowed_approximate = ATOM_RenderScheme::_shadowDPCount;
	_numBatches_shadowed_approximate = ATOM_RenderScheme::_shadowBatchCount;
	ATOM_RenderScheme::_shadowDPCount = 0;
	ATOM_RenderScheme::_shadowBatchCount = 0;
	//----------------------//
}

void ScenePropEditor::refresh (void)
{
#ifdef USE_ATMOSPHERE2
	ATOM_Node *node = _scene ? ATOMX_FindFirstNodeByClassName (_scene->getRootNode(), ATOM_Sky::_classname()) : NULL;
#else
	ATOM_Node *node = _scene ? ATOMX_FindFirstNodeByClassName (_scene->getRootNode(), ATOM_Atmosphere::_classname()) : NULL;
#endif

	_atmosphere = node;

	updateTime ();

	if (_bar)
	{
		_bar->clear ();
	}

	createBar ();
}

void ScenePropEditor::loadAtmosphereParams (ParamBlock &params, float time)
{
#ifdef USE_ATMOSPHERE2
	params._atmosphereParams._lightDir = _atmosphere->getLightDir();
	params._atmosphereParams._lightColor = _atmosphere->getLightColor();
	params._atmosphereParams._exposure = _atmosphere->getExposure();
	params._atmosphereParams._rayleigh = _atmosphere->getRayleigh();
	params._atmosphereParams._cameraHeight = _atmosphere->getCameraHeight();
	params._atmosphereParams._fadeFar = _atmosphere->getFarClip();
	params._atmosphereParams._cloudCover = _atmosphere->getCloudCover();
	params._atmosphereParams._lightScale = _atmosphere->getLightScale();
	params._atmosphereParams._lightIntensity = _atmosphere->getLightIntensity();
	params._atmosphereParams._fogDensity = _atmosphere->getFogDensity() * 1000;
	params._atmosphereParams._fogStart = _atmosphere->getFogNear();
	params._atmosphereParams._fogEnd = _atmosphere->getFogFar();
	params._atmosphereParams._drawSky = _atmosphere->isDrawSkyEnabled();
	//--- wangjian added ---//
	//params._atmosphereParams._renderMode	= _atmosphere->getRenderMode();
	params._atmosphereParams._skyDomeOffset = _atmosphere->getSkyDomeTranslateOffset();
	params._atmosphereParams._skyDomeScale	= _atmosphere->getSkyDomeScale();
	params._atmosphereParams._fogColor		= _atmosphere->getFogColor();
	//----------------------//
#else
	params._atmosphereParams._skyRadius = _atmosphere->getSkydomeRadiusKey(time);
	params._atmosphereParams._sunIntensity = _atmosphere->getSunIntensityKey(time);
	params._atmosphereParams._lightDir = _atmosphere->getSunDirectionKey(time);
	params._atmosphereParams._G = _atmosphere->getGKey(time);
	params._atmosphereParams._mieMultiplier = _atmosphere->getMieMultiplierKey(time);
	params._atmosphereParams._rayleighMultiplier = _atmosphere->getRayleighMultiplierKey(time);
	params._atmosphereParams._exposure = _atmosphere->getExposureKey(time);
	params._atmosphereParams._lambdaR = _atmosphere->getLambdaRedKey(time);
	params._atmosphereParams._lambdaG = _atmosphere->getLambdaGreenKey(time);
	params._atmosphereParams._lambdaB = _atmosphere->getLambdaBlueKey(time);
	params._atmosphereParams._cameraHeight = _atmosphere->getCameraHeightKey(time);
	params._atmosphereParams._fadeNear = _atmosphere->getFadeNearKey(time);
	params._atmosphereParams._fadeFar = _atmosphere->getFadeFarKey(time);
	params._cloudParams._cloudCoverage = _atmosphere->getCloudCoverageKey(time);
	params._cloudParams._cloudSharpness = _atmosphere->getCloudSharpnessKey(time);
	params._cloudParams._cloudBrightness = _atmosphere->getCloudBrightnessKey(time);
	params._cloudParams._cloudDeformSpeed = _atmosphere->getCloudDeformSpeedKey(time);
	params._cloudParams._cloudMoveSpeed = _atmosphere->getCloudMoveSpeedKey(time);
	params._cloudParams._cloudMoveDir = _atmosphere->getCloudMoveDirKey(time).getVector3();
	params._cloudParams._cloudColor = _atmosphere->getCloudColorKey(time);
	params._moonParams._moonColor = _atmosphere->getMoonColorKey(time);
	params._moonParams._moonDir = _atmosphere->getMoonDirKey(time).getVector3();
	params._moonParams._moonSize = _atmosphere->getMoonSizeKey(time);
	params._moonParams._starfieldTextureRepeat = _atmosphere->getStarfieldTextureRepeat();
#endif
}

void ScenePropEditor::saveAtmosphereParams (const ParamBlock &params, float time)
{
#ifdef USE_ATMOSPHERE2
	_atmosphere->setLightDir (params._atmosphereParams._lightDir);
	_atmosphere->setLightColor (params._atmosphereParams._lightColor);
	_atmosphere->setExposure (params._atmosphereParams._exposure);
	_atmosphere->setRayleigh (params._atmosphereParams._rayleigh);
	_atmosphere->setCameraHeight (params._atmosphereParams._cameraHeight);
	_atmosphere->setFarClip (params._atmosphereParams._fadeFar);
	_atmosphere->setCloudCover (params._atmosphereParams._cloudCover);
	_atmosphere->setLightScale (params._atmosphereParams._lightScale);
	_atmosphere->setLightIntensity (params._atmosphereParams._lightIntensity);
	_atmosphere->setFogDensity (params._atmosphereParams._fogDensity * 0.001f);
	_atmosphere->setFogNear (params._atmosphereParams._fogStart);
	_atmosphere->setFogFar (params._atmosphereParams._fogEnd);
	_atmosphere->enableDrawSky (params._atmosphereParams._drawSky);
	//--- wangjian added ---//
	_atmosphere->setSkyDomeTranslateOffset(params._atmosphereParams._skyDomeOffset);
	_atmosphere->setSkyDomeScale(params._atmosphereParams._skyDomeScale);
	_atmosphere->setFogColor( ATOM_Vector3f(params._atmosphereParams._fogColor.x,
											params._atmosphereParams._fogColor.y,
											params._atmosphereParams._fogColor.z) );
	//----------------------//
#else
	_atmosphere->setSkydomeRadiusKey (time, params._atmosphereParams._skyRadius);
	_atmosphere->setSunIntensityKey (time, params._atmosphereParams._sunIntensity);
	_atmosphere->setSunDirectionKey (time, params._atmosphereParams._lightDir);
	_atmosphere->setGKey (time, params._atmosphereParams._G);
	_atmosphere->setMieMultiplierKey (time, params._atmosphereParams._mieMultiplier);
	_atmosphere->setRayleighMultiplierKey (time, params._atmosphereParams._rayleighMultiplier);
	_atmosphere->setExposureKey (time, params._atmosphereParams._exposure);
	_atmosphere->setLambdaRedKey (time, params._atmosphereParams._lambdaR);
	_atmosphere->setLambdaGreenKey (time, params._atmosphereParams._lambdaG);
	_atmosphere->setLambdaBlueKey (time, params._atmosphereParams._lambdaB);
	_atmosphere->setCameraHeightKey (time, params._atmosphereParams._cameraHeight);
	_atmosphere->setFadeNearKey (time, params._atmosphereParams._fadeNear);
	_atmosphere->setFadeFarKey (time, params._atmosphereParams._fadeFar);
	_atmosphere->setCloudCoverageKey (time, params._cloudParams._cloudCoverage);
	_atmosphere->setCloudSharpnessKey (time, params._cloudParams._cloudSharpness);
	_atmosphere->setCloudBrightnessKey (time, params._cloudParams._cloudBrightness);
	_atmosphere->setCloudDeformSpeedKey (time, params._cloudParams._cloudDeformSpeed);
	_atmosphere->setCloudMoveSpeedKey (time, params._cloudParams._cloudMoveSpeed);
	_atmosphere->setCloudMoveDirKey (time, ATOM_Vector4f(params._cloudParams._cloudMoveDir.x, params._cloudParams._cloudMoveDir.y, params._cloudParams._cloudMoveDir.z, 0.f));
	_atmosphere->setCloudColorKey (time, params._cloudParams._cloudColor);
	_atmosphere->setMoonColorKey (time, params._moonParams._moonColor);
	_atmosphere->setMoonDirKey (time, ATOM_Vector4f(params._moonParams._moonDir));
	_atmosphere->setMoonSizeKey (time, params._moonParams._moonSize);
	_atmosphere->setStarfieldTextureRepeat (params._moonParams._starfieldTextureRepeat);
#endif
}

void ScenePropEditor::show (bool b)
{
	if (b && !_bar)
	{
		createBar ();
	}

	_isShown = b;

	if (_bar)
	{
		_bar->setBarVisible (b);
	}
}

bool ScenePropEditor::isShown (void) const
{
	return _isShown;
}

void ScenePropEditor::createLightParams (void)
{
#if 0
	_bar->addImmediateVariable ("Scale", ATOMX_TBTYPE_FLOAT, &_params._ssaoParams._scale, false, GROUP_SSAO);
	_bar->addImmediateVariable ("Bias", ATOMX_TBTYPE_FLOAT, &_params._ssaoParams._bias, false, GROUP_SSAO);
	_bar->addImmediateVariable ("Radius", ATOMX_TBTYPE_FLOAT, &_params._ssaoParams._radius, false, GROUP_SSAO);
	_bar->addImmediateVariable ("Intensity", ATOMX_TBTYPE_FLOAT, &_params._ssaoParams._intensity, false, GROUP_SSAO);
	_bar->addImmediateVariable ("Downsample factor", ATOMX_TBTYPE_FLOAT, &_params._ssaoParams._factor, false, GROUP_SSAO);

	_bar->addImmediateVariable ("BloomMiddleGray", ATOMX_TBTYPE_FLOAT, &_params._bloomParams._middleGray, false, GROUP_BLOOM);
	_bar->setVarMinMax ("BloomMiddleGray", 0.f, 100.f);
	_bar->setVarStep ("BloomMiddleGray", 0.001f);
	_bar->setVarPrecision ("BloomMiddleGray", 3);

	_bar->addButton ("ApplyBloom", BUTTONID_APPLY_BLOOM, "Apply to all time", GROUP_BLOOM);

	_bar->addImmediateVariable ("SplitLambda", ATOMX_TBTYPE_FLOAT, &_params._shadowParams._splitLambda, false, GROUP_SHADOW);
	_bar->setVarMinMax ("SplitLambda", 0.f, 1.f);
	_bar->setVarStep ("SplitLambda", 0.01f);
#endif

	_bar->addImmediateVariable ("BloomThreshold", ATOMX_TBTYPE_FLOAT, &_params._bloomParams._threshold, false, GROUP_BLOOM);
	_bar->setVarMinMax ("BloomThreshold", 0.f, 100.f);
	_bar->setVarStep ("BloomThreshold", 0.001f);
	_bar->setVarPrecision ("BloomThreshold", 3);
	_bar->addImmediateVariable ("BloomOffset", ATOMX_TBTYPE_FLOAT, &_params._bloomParams._offset, false, GROUP_BLOOM);
	_bar->setVarMinMax ("BloomOffset", -100.f, 100.f);
	_bar->setVarStep ("BloomOffset", 0.001f);
	_bar->setVarPrecision ("BloomOffset", 3);
	
	//--- wangjian added ---//
	// HDR相关
	_bar->addImmediateVariable ("BloomMultiplier", ATOMX_TBTYPE_FLOAT, &_params._bloomParams._bloomMultiplier, false, GROUP_BLOOM);
	_bar->setVarMinMax ("BloomMultiplier", 0.f, 100.f);
	_bar->setVarStep ("BloomMultiplier", 0.001f);
	_bar->setVarPrecision ("BloomMultiplier", 3);

	_bar->addImmediateVariable ("HDR效果", ATOMX_TBTYPE_BOOL, &_params._bloomParams._hdr_enable, false, GROUP_BLOOM);

	_bar->addImmediateVariable ("FilmCurveShoulder", ATOMX_TBTYPE_FLOAT, &_params._bloomParams._filmCurveShoulder, false, GROUP_BLOOM);
	_bar->setVarMinMax ("FilmCurveShoulder", 0.f, 100.f);
	_bar->setVarStep ("FilmCurveShoulder", 0.001f);
	_bar->setVarPrecision ("FilmCurveShoulder", 3);
	_bar->addImmediateVariable ("FilmCurveMiddleTone", ATOMX_TBTYPE_FLOAT, &_params._bloomParams._filmCurveMiddleTone, false, GROUP_BLOOM);
	_bar->setVarMinMax ("FilmCurveMiddleTone", 0.f, 100.f);
	_bar->setVarStep ("FilmCurveMiddleTone", 0.001f);
	_bar->setVarPrecision ("FilmCurveMiddleTone", 3);
	_bar->addImmediateVariable ("FilmCurveToe", ATOMX_TBTYPE_FLOAT, &_params._bloomParams._filmCurveToe, false, GROUP_BLOOM);
	_bar->setVarMinMax ("FilmCurveToe", 0.f, 100.f);
	_bar->setVarStep ("FilmCurveToe", 0.001f);
	_bar->setVarPrecision ("FilmCurveToe", 3);
	_bar->addImmediateVariable ("FilmCurveWhitePoint", ATOMX_TBTYPE_FLOAT, &_params._bloomParams._filmCurveWhitePoint, false, GROUP_BLOOM);
	_bar->setVarMinMax ("FilmCurveWhitePoint", 0.f, 100.f);
	_bar->setVarStep ("FilmCurveWhitePoint", 0.001f);
	_bar->setVarPrecision ("FilmCurveWhitePoint", 3);
	_bar->addImmediateVariable ("HDRSaturate", ATOMX_TBTYPE_FLOAT, &_params._bloomParams._HDRSaturate, false, GROUP_BLOOM);
	_bar->setVarMinMax ("HDRSaturate", 0.f, 100.f);
	_bar->setVarStep ("HDRSaturate", 0.001f);
	_bar->setVarPrecision ("HDRSaturate", 3);
	_bar->addImmediateVariable ("HDRContrast", ATOMX_TBTYPE_FLOAT, &_params._bloomParams._HDRContrast, false, GROUP_BLOOM);
	_bar->setVarMinMax ("HDRContrast", 0.f, 100.f);
	_bar->setVarStep ("HDRContrast", 0.001f);
	_bar->setVarPrecision ("HDRContrast", 3);
	_bar->addImmediateVariable ("HDRColorBalance", ATOMX_TBTYPE_COLOR3F, &_params._bloomParams._HDRColorBalance, false, GROUP_BLOOM);
	//----------------------//

	_bar->addImmediateVariable ("阴影范围", ATOMX_TBTYPE_FLOAT, &_params._shadowParams._shadowDistance, false, GROUP_SHADOW);
	_bar->setVarMinMax ("阴影范围", 1.f, 10000.f);
	_bar->setVarStep ("阴影范围", 100.f);

	_bar->addImmediateVariable ("遮挡物范围", ATOMX_TBTYPE_FLOAT, &_params._shadowParams._lightDistance, false, GROUP_SHADOW);
	_bar->setVarMinMax ("遮挡物范围", 1.f, 10000.f);
	_bar->setVarStep ("遮挡物范围", 1.f);

#if 0
	_bar->addImmediateVariable ("SceneScale", ATOMX_TBTYPE_FLOAT, &_params._shadowParams._sceneScale, false, GROUP_SHADOW);
	_bar->setVarMinMax ("SceneScale", 0.01f, 100.f);
	_bar->setVarStep ("SceneScale", 0.1f);

	_bar->addImmediateVariable ("LightSize", ATOMX_TBTYPE_FLOAT, &_params._shadowParams._lightSize, false, GROUP_SHADOW);
	_bar->setVarMinMax ("LightSize", 0.01f, 0.1f);
	_bar->setVarStep ("LightSize", 0.001f);
#endif

	_bar->addImmediateVariable ("深度偏移", ATOMX_TBTYPE_FLOAT, &_params._shadowParams._depthBias, false, GROUP_SHADOW);
	_bar->setVarMinMax ("深度偏移", 0.f, 500.f);
	_bar->setVarStep ("深度偏移", 0.0001f);

	_bar->addImmediateVariable ("淡出开始点", ATOMX_TBTYPE_FLOAT, &_params._shadowParams._fadeMin, false, GROUP_SHADOW);
	_bar->setVarMinMax ("淡出开始点", 0.f, 10000.f);
	_bar->setVarStep ("淡出开始点", 0.1f);

	_bar->addImmediateVariable ("淡出结束点", ATOMX_TBTYPE_FLOAT, &_params._shadowParams._fadeMax, false, GROUP_SHADOW);
	_bar->setVarMinMax ("淡出结束点", 0.f, 10000.f);
	_bar->setVarStep ("淡出结束点", 0.1f);

	//--- wangjian added ---//
	// 阴影相关
	_bar->addImmediateVariable ("阴影模糊", ATOMX_TBTYPE_FLOAT, &_params._shadowParams._shadowBlur, false, GROUP_SHADOW);
	_bar->setVarMinMax ("阴影模糊", 0.f, 2.0f);
	_bar->setVarStep ("阴影模糊", 0.01f);

	_bar->addImmediateVariable ("阴影坡度偏移因子", ATOMX_TBTYPE_FLOAT, &_params._shadowParams._shadowSlopeBias, false, GROUP_SHADOW);
	_bar->setVarMinMax ("阴影坡度偏移因子", 0.f, 100.0f);
	_bar->setVarStep ("阴影坡度偏移因子", 0.0001f);

	_bar->addImmediateVariable ("阴影浓度", ATOMX_TBTYPE_FLOAT, &_params._shadowParams._shadowDensity, false, GROUP_SHADOW);
	_bar->setVarMinMax ("阴影浓度", 0.f, 1.0f);
	_bar->setVarStep ("阴影浓度", 0.01f);
	//----------------------//

#if 0
	_bar->addButton ("ApplyShadow", BUTTONID_APPLY_SHADOW, "Apply to all time", GROUP_SHADOW);
#endif
}

void ScenePropEditor::createBar (void)
{
	if (!_bar)
	{
		_bar = ATOM_NEW(ATOMX_TweakBar, "场景属性");
		_bar->setBarColor (0.5f, 0.5f, 0.5f, 1.f);
		_bar->setBarMovable (false);
		_bar->setBarResizable (false);
		_bar->setBarIconifiable (false);
		_bar->enableNotifying (true);
		_bar->setCommandCallback (_callback);
		//--- wangjian added ---//
		_bar->setValueChangedCallback(_callback);
		//----------------------//
	}

	_bar->addImmediateVariable ("帧率(FPS)", ATOMX_TBTYPE_FLOAT, &_fps, true, PARAMNAME_STATISTICS);
	_bar->addImmediateVariable ("渲染调用次数", ATOMX_TBTYPE_INT32, &_numDrawCalls, true, PARAMNAME_STATISTICS);
	_bar->addImmediateVariable ("渲染面片数", ATOMX_TBTYPE_INT32, &_numPrimDrawn, true, PARAMNAME_STATISTICS);
	//--- wangjian added ---//
	_bar->addImmediateVariable ("阴影遍DP数", ATOMX_TBTYPE_INT32, &_numDrawCalls_shadowed_approximate, true, PARAMNAME_STATISTICS);
	_bar->addImmediateVariable ("阴影遍BATCH数", ATOMX_TBTYPE_INT32, &_numBatches_shadowed_approximate, true, PARAMNAME_STATISTICS);
	
	_bar->addImmediateVariable ("方向光源数", ATOMX_TBTYPE_INT32, &_numDirectionalLight, true, PARAMNAME_LIGHT);
	_bar->addImmediateVariable ("点光源数", ATOMX_TBTYPE_INT32, &_numPointLight, true, PARAMNAME_LIGHT);
	//----------------------//

	_bar->addImmediateVariable ("位置", ATOMX_TBTYPE_VECTOR3F, &_cameraPos, true, PARAMNAME_CAMERA);
	_bar->addImmediateVariable ("旋转", ATOMX_TBTYPE_VECTOR4F, &_cameraOrientation, true, PARAMNAME_CAMERA);
	_bar->addImmediateVariable ("视角", ATOMX_TBTYPE_FLOAT, &_cameraFOV, true, PARAMNAME_CAMERA);
	_bar->addImmediateVariable ("近截面", ATOMX_TBTYPE_FLOAT, &_cameraNear, true, PARAMNAME_CAMERA);
	_bar->addImmediateVariable ("远截面", ATOMX_TBTYPE_FLOAT, &_cameraFar, true, PARAMNAME_CAMERA);

#if 0
	_bar->addImmediateVariable ("Time", ATOMX_TBTYPE_INT32, &_time, true, PARAMNAME_STATISTICS);
	_bar->addImmediateVariable ("RenderTargetScale", ATOMX_TBTYPE_FLOAT, &_renderTargetScale, false, PARAMNAME_STATISTICS);
#endif

	createLightParams ();

	if (!_atmosphere)
	{
		_bar->addButton ("CreateAtmosphere", BUTTONID_CREATEATMOSPHERE, "创建天空", GROUP_ATMOSPHERE);
	}
	else
	{
		_bar->addButton ("CloudTexture", BUTTONID_CLOUDTEXTURE, "云层贴图..", GROUP_ATMOSPHERE);

		_bar->addButton ("SkyMaterial", BUTTONID_SKYMATERIAL, "天空材质..", GROUP_ATMOSPHERE);

		_bar->addImmediateVariable ("太阳光颜色", ATOMX_TBTYPE_COLOR3F, &_params._atmosphereParams._lightColor, false, GROUP_ATMOSPHERE);

		_bar->addImmediateVariable ("环境光亮度", ATOMX_TBTYPE_COLOR3F, &_params._atmosphereParams._ambientColor, false, GROUP_ATMOSPHERE);

		_bar->addImmediateVariable (PARAMNAME_SUNBRIGHTNESS, ATOMX_TBTYPE_FLOAT, &_params._atmosphereParams._lightScale, false, GROUP_ATMOSPHERE);
		_bar->setVarMinMax (PARAMNAME_SUNBRIGHTNESS, 0.f, 500.f);
		_bar->setVarStep (PARAMNAME_SUNBRIGHTNESS, 0.1f);
		
		_bar->addImmediateVariable (PARAMNAME_SUNINTENSITY, ATOMX_TBTYPE_FLOAT, &_params._atmosphereParams._lightIntensity, false, GROUP_ATMOSPHERE);
		_bar->setVarMinMax (PARAMNAME_SUNINTENSITY, 0.f, 500.f);
		_bar->setVarStep (PARAMNAME_SUNINTENSITY, 0.1f);
		_bar->setVarPrecision (PARAMNAME_SUNINTENSITY, 2);
		
		_bar->addImmediateVariable (PARAMNAME_FOGDENSITY, ATOMX_TBTYPE_FLOAT, &_params._atmosphereParams._fogDensity, false, GROUP_ATMOSPHERE);
		_bar->setVarMinMax (PARAMNAME_FOGDENSITY, 0.f, 100000.f);
		_bar->setVarStep (PARAMNAME_FOGDENSITY, 0.01f);
		
		_bar->addImmediateVariable (PARAMNAME_FOGNEAR, ATOMX_TBTYPE_FLOAT, &_params._atmosphereParams._fogStart, false, GROUP_ATMOSPHERE);
		_bar->setVarMinMax (PARAMNAME_FOGNEAR, 0.f, 20000.f);
		_bar->setVarStep (PARAMNAME_FOGNEAR, 1.f);
		_bar->setVarPrecision (PARAMNAME_FOGNEAR, 3);
		
		_bar->addImmediateVariable (PARAMNAME_FOGFAR, ATOMX_TBTYPE_FLOAT, &_params._atmosphereParams._fogEnd, false, GROUP_ATMOSPHERE);
		_bar->setVarMinMax (PARAMNAME_FOGFAR, 0.f, 20000.f);
		_bar->setVarStep (PARAMNAME_FOGFAR, 1.f);
		_bar->setVarPrecision (PARAMNAME_FOGFAR, 3);
		
		_bar->addImmediateVariable (PARAMNAME_DRAWSKY, ATOMX_TBTYPE_BOOL, &_params._atmosphereParams._drawSky, false, GROUP_ATMOSPHERE);
		
		_bar->addImmediateVariable (PARAMNAME_RAYLEIGH, ATOMX_TBTYPE_VECTOR3F, &_params._atmosphereParams._rayleigh, false, GROUP_ATMOSPHERE);
		_bar->setVarStep (PARAMNAME_RAYLEIGH, 0.001f);
		_bar->setVarPrecision (PARAMNAME_RAYLEIGH, 3);

		_bar->addImmediateVariable (PARAMNAME_SUNDIRECTION, ATOMX_TBTYPE_DIR3F, &_params._atmosphereParams._lightDir, false, GROUP_ATMOSPHERE);
		
		_bar->addImmediateVariable (PARAMNAME_EXPOSURE, ATOMX_TBTYPE_FLOAT, &_params._atmosphereParams._exposure, false, GROUP_ATMOSPHERE);
		_bar->setVarMinMax (PARAMNAME_EXPOSURE, 0.f, 100.f);
		_bar->setVarStep (PARAMNAME_EXPOSURE, 0.0001f);
		_bar->setVarPrecision (PARAMNAME_EXPOSURE, 4);

		//------- wangjian added ---------//
		ATOMX_TBEnum rendermodeEnum;
		rendermodeEnum.addEnum ("Scatter", 0);
		rendermodeEnum.addEnum ("天空盒", 1);
		rendermodeEnum.addEnum ("天空球", 2);
		_bar->addEnum(PARAMNAME_SKY_RENDERMODE,BUTTONID_SKYRENDERMODE,ATOM_Sky::SKY_DOME,rendermodeEnum,false,GROUP_ATMOSPHERE,0);

		_bar->addButton ("SkyTexture", BUTTONID_SKYTEXTURE, PARAMNAME_SKY_TEXTURE, GROUP_ATMOSPHERE);
		_bar->addButton ("SkyDomeModel", BUTTONID_SKYDOMEMODEL, PARAMNAME_SKYDOME_MODEL, GROUP_ATMOSPHERE);

		_bar->addImmediateVariable (PARAMNAME_SKYDOME_OFFSET, ATOMX_TBTYPE_VECTOR3F, &_params._atmosphereParams._skyDomeOffset, false, GROUP_ATMOSPHERE);
		_bar->setVarStep (PARAMNAME_SKYDOME_OFFSET, 0.001f);
		_bar->setVarPrecision (PARAMNAME_SKYDOME_OFFSET, 3);

		_bar->addImmediateVariable (PARAMNAME_SKYDOME_SCALE, ATOMX_TBTYPE_VECTOR3F, &_params._atmosphereParams._skyDomeScale, false, GROUP_ATMOSPHERE);
		_bar->setVarStep (PARAMNAME_SKYDOME_SCALE, 0.001f);
		_bar->setVarPrecision (PARAMNAME_SKYDOME_SCALE, 3);

		_bar->addImmediateVariable (PARAMNAME_FOGCOLOR, ATOMX_TBTYPE_COLOR3F, &_params._atmosphereParams._fogColor, false, GROUP_ATMOSPHERE);
		//---------------------------------//
#if 0
		_bar->addImmediateVariable (PARAMNAME_CAMERA_HEIGHT, ATOMX_TBTYPE_FLOAT, &_params._atmosphereParams._cameraHeight, false, GROUP_ATMOSPHERE);
		_bar->setVarMinMax (PARAMNAME_CAMERA_HEIGHT, 0.f, 10000.f);
		_bar->setVarStep (PARAMNAME_CAMERA_HEIGHT, 0.01f);
		_bar->setVarPrecision (PARAMNAME_CAMERA_HEIGHT, 2);
#endif
		_bar->addImmediateVariable (PARAMNAME_FADE_FAR, ATOMX_TBTYPE_FLOAT, &_params._atmosphereParams._fadeFar, false, GROUP_ATMOSPHERE);
		_bar->setVarMinMax (PARAMNAME_FADE_FAR, 0.f, 100000.f);
		_bar->setVarStep (PARAMNAME_FADE_FAR, 1.f);
		_bar->addImmediateVariable (PARAMNAME_CLOUD_COVER, ATOMX_TBTYPE_FLOAT, &_params._atmosphereParams._cloudCover, false, GROUP_ATMOSPHERE);
		_bar->setVarMinMax (PARAMNAME_CLOUD_COVER, 0.f, 1.f);
		_bar->setVarStep (PARAMNAME_CLOUD_COVER, 0.01f);
	}

#if 0
	_bar->addButton ("Copy", BUTTONID_COPY, "Copy", GROUP_EDIT);
	_bar->addButton ("CopyAllTime", BUTTONID_COPYALLTIME, "Copy All Time", GROUP_EDIT);
	_bar->addButton ("Paste", BUTTONID_PASTE, "Paste", GROUP_EDIT);
#endif
}

ATOMX_TweakBar *ScenePropEditor::getBar (void) const
{
	return _bar;
}

AS_Editor *ScenePropEditor::getEditor (void) const
{
	return _editor;
}

ATOM_Sky *ScenePropEditor::getSky (void) const
{
	return _atmosphere.get();
}

void ScenePropEditor::copyContents (void)
{
	_editor->setClipboardDataByName (CB_TYPE_SCENEPROP, &_params, sizeof(_params));
}

void ScenePropEditor::copyContentsAllTime (void)
{
	ParamBlock params[ATOM_TimeManager::numTimeKeys];

	for (int i = 0; i < ATOM_TimeManager::numTimeKeys; ++i)
	{
		loadSceneParams (params[i], i, SCENE_PROP_ALL);
	}
	_editor->setClipboardDataByName (CB_TYPE_SCENEPROP_ALLTIME, params, sizeof(params));
}

void ScenePropEditor::pasteContents (void)
{
	int type = _editor->getClipboardData (0);
	if (_editor->getClipboardContentType (CB_TYPE_SCENEPROP) == type)
	{
		ParamBlock params;
		_editor->getClipboardData (&params);
		saveSceneParams (params, ATOM_TimeManager::getCurrentTime(), SCENE_PROP_ALL);
	}
	else if (_editor->getClipboardContentType (CB_TYPE_SCENEPROP_ALLTIME) == type)
	{
		ParamBlock params[ATOM_TimeManager::numTimeKeys];
		_editor->getClipboardData (params);
		for (int i = 0; i < ATOM_TimeManager::numTimeKeys; ++i)
		{
			saveSceneParams (params[i], i, SCENE_PROP_ALL);
		}
	}

	updateTime ();

	_modified = true;
	_editor->setDocumentModified (true);
}

void ScenePropEditor::setMoonTexture (const char *filename)
{
#ifndef USE_ATMOSPHERE2
	if (_atmosphere)
	{
		_atmosphere->setMoonTextureFileName (filename);
	}
#endif
}

void ScenePropEditor::setStarfieldTexture (const char *filename)
{
#ifndef USE_ATMOSPHERE2
	if (_atmosphere)
	{
		_atmosphere->setStarfieldTextureFileName (filename);
	}
#endif
}

void ScenePropEditor::applyAtmosphereSettingsToAllTime (void)
{
	for (int i = 0; i < ATOM_TimeManager::numTimeKeys; ++i)
	{
		saveSceneParams (_params, i, SCENE_PROP_ATMOSPHERE);
	}
	updateTime();
	_modified = true;
	_editor->setDocumentModified (true);
}

void ScenePropEditor::applyShadowSettingsToAllTime (void)
{
	for (int i = 0; i < ATOM_TimeManager::numTimeKeys; ++i)
	{
		saveSceneParams (_params, i, SCENE_PROP_SHADOW);
	}
	updateTime();
	_modified = true;
	_editor->setDocumentModified (true);
}

void ScenePropEditor::applyBloomSettingsToAllTime (void)
{
	for (int i = 0; i < ATOM_TimeManager::numTimeKeys; ++i)
	{
		saveSceneParams (_params, i, SCENE_PROP_BLOOM);
	}
	updateTime();
	_modified = true;
	_editor->setDocumentModified (true);
}

void ScenePropEditor::applyMoonSettingsToAllTime (void)
{
	for (int i = 0; i < ATOM_TimeManager::numTimeKeys; ++i)
	{
		saveSceneParams (_params, i, SCENE_PROP_MOON);
	}
	updateTime();
	_modified = true;
	_editor->setDocumentModified (true);
}

