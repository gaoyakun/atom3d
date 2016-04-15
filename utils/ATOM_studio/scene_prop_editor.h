#ifndef __ATOM3D_STUDIO_SCENE_PROP_EDITOR_H
#define __ATOM3D_STUDIO_SCENE_PROP_EDITOR_H

#if _MSC_VER > 1000
# pragma once
#endif

#define CB_TYPE_SCENEPROP			"SceneProp"
#define CB_TYPE_SCENEPROP_ALLTIME	"ScenePropAllTime"

#define USE_ATMOSPHERE2

class AS_Editor;
class ScenePropCommandCallback;

static const int BUTTONID_CREATEATMOSPHERE = 0;
static const int BUTTONID_CLOUDTEXTURE = 1;
static const int BUTTONID_SKYMATERIAL = 2;
static const int BUTTONID_COPY = 3;
static const int BUTTONID_COPYALLTIME = 4;
static const int BUTTONID_PASTE = 5;
static const int BUTTONID_APPLY_ATMOSPHERE = 6;
static const int BUTTONID_APPLY_SHADOW = 7;
static const int BUTTONID_APPLY_BLOOM = 8;
static const int BUTTONID_APPLY_MOON = 9;
//--- wangjian added ---//
static const int BUTTONID_SKYRENDERMODE = 10;
static const int BUTTONID_SKYTEXTURE = 11;
static const int BUTTONID_SKYDOMEMODEL = 12;
//----------------------//

class ScenePropEditor
{
	enum
	{
		SCENE_PROP_ATMOSPHERE = (1<<0),
		SCENE_PROP_SHADOW = (1<<1),
		SCENE_PROP_BLOOM = (1<<2),
		SCENE_PROP_MOON = (1<<3),
		SCENE_PROP_SSAO = (1<<4),
		SCENE_PROP_ALL = 0xFFFFFFFF
	};

	struct AtmosphereParams
	{
		ATOM_Vector4f _lightColor;
		ATOM_Vector4f _ambientColor;
		ATOM_Vector3f _lightDir;
		ATOM_Vector3f _rayleigh;
		ATOM_Vector2f _velocity;

		float _exposure;
		float _cameraHeight;
		float _fadeFar;
		float _cloudCover;
		float _lightScale;
		float _lightIntensity;
		float _fogDensity;
		float _fogStart;
		float _fogEnd;
		int _drawSky;

		//--- wangjian added ---//
		int				_renderMode;
		ATOM_Vector3f	_skyDomeOffset;
		ATOM_Vector3f	_skyDomeScale;
		ATOM_Vector4f	_fogColor;
		//----------------------//
	};

	struct ShadowParams
	{
		float _minShadowVariance;
		float _antiLightBleedingValue;
		float _splitLambda;
		float _shadowDistance;
		float _lightDistance;
		float _sceneScale;
		float _lightSize;
		float _depthBias;
		float _fadeMin;
		float _fadeMax;
		//--- wangjian added ---//
		// 阴影相关
		float _shadowBlur;
		float _shadowSlopeBias;
		float _shadowDensity;
		//----------------------//
	};

	struct BloomParams
	{
		float _threshold;
		float _offset;
		float _middleGray;
		//--- wangjian added ---//
		// HDR相关
		float			_bloomMultiplier;
		int				_hdr_enable;
		float			_filmCurveShoulder;
		float			_filmCurveMiddleTone;
		float			_filmCurveToe;
		float			_filmCurveWhitePoint;
		float			_HDRSaturate;
		float			_HDRContrast;
		ATOM_Vector3f	_HDRColorBalance;
		//----------------------//
	};

	struct SSAOParams
	{
		float _scale;
		float _bias;
		float _radius;
		float _intensity;
		float _factor;
	};

	struct ParamBlock
	{
		AtmosphereParams _atmosphereParams;
		ShadowParams _shadowParams;
		BloomParams _bloomParams;
		SSAOParams _ssaoParams;
	};

public:
	ScenePropEditor (AS_Editor *editor, ATOM_DeferredScene *scene);
	~ScenePropEditor (void);

public:
	void setScene (ATOM_DeferredScene *scene);
	void setPosition (int x, int y, int w, int h);
	void frameUpdate (void);
	void show (bool b);
	bool isShown (void) const;
	void refresh (void);
	ATOMX_TweakBar *getBar (void) const;
	AS_Editor *getEditor (void) const;
	ATOM_Sky *getSky (void) const;
	void setParamChanged (bool b);
	void updateTime (void);
	void copyContents (void);
	void copyContentsAllTime (void);
	void pasteContents (void);
	void setMoonTexture (const char *filename);
	void setStarfieldTexture (const char *filename);
	void applyAtmosphereSettingsToAllTime (void);
	void applyShadowSettingsToAllTime (void);
	void applyBloomSettingsToAllTime (void);
	void applyCloudSettingsToAllTime (void);
	void applyMoonSettingsToAllTime (void);

private:
	void createBar (void);
	void loadAtmosphereParams (ParamBlock &params, float time);
	void loadSceneParams (ParamBlock &params, float time, unsigned mask);
	void saveAtmosphereParams (const ParamBlock &params, float time);
	void saveSceneParams (const ParamBlock &params, float time, unsigned mask);
	void createLightParams (void);
	void checkParamChanged (void);

private:
	ATOM_DeferredScene *_scene;
	AS_Editor *_editor;

#ifdef USE_ATMOSPHERE2
	ATOM_AUTOREF(ATOM_Sky) _atmosphere;
#else
	ATOM_AUTOREF(ATOM_Atmosphere) _atmosphere;
#endif

	ATOMX_TweakBar *_bar;
	bool _isShown;

	ParamBlock _params;
	ParamBlock _paramsBackup;
	bool _modified;

	float _fps;
	int _time;
	int _numDrawCalls;
	//--- wangjian added ---//
	int _numDrawCalls_shadowed_approximate;
	int _numBatches_shadowed_approximate;
	int _numDirectionalLight;
	int _numPointLight;
	//----------------------//
	int _numPrimDrawn;
	float _renderTargetScale;
	ATOM_Vector3f _cameraPos;
	ATOM_Vector4f _cameraOrientation;
	float _cameraFOV;
	float _cameraNear;
	float _cameraFar;

	ScenePropCommandCallback *_callback;
};

class ScenePropCommandCallback: public ATOMX_TweakBar::CommandCallback, public ATOMX_TweakBar::ValueChangedCallback // wangjian added
{
public:
	ScenePropEditor *_editor;
	ATOM_DeferredScene *_scene;

	ScenePropCommandCallback (ScenePropEditor *editor, ATOM_DeferredScene *scene): _editor(editor), _scene(scene)
	{
	}

	void setScene (ATOM_DeferredScene *scene);
	void callback (ATOMX_TWCommandEvent *event);

	//--- wangjian added ---//
	void callback (ATOMX_TWValueChangedEvent *event);
	//----------------------//
};

#endif // __ATOM3D_STUDIO_SCENE_PROP_EDITOR_H
