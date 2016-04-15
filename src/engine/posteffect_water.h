#ifndef __ATOM_ENGINE_POSTEFFECT_WATER
#define __ATOM_ENGINE_POSTEFFECT_WATER

#if _MSC_VER > 1000
# pragma once
#endif

#include "ATOM_render.h"

class ATOM_Water;

class PostEffectWater
{
public:
	static bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Texture *colorMap, ATOM_Texture *positionMap);
	static void addWater (ATOM_Water *water);
	static void setReflectionTexture (ATOM_Texture *texture);
	static void setFoamTexture (ATOM_Texture *foamTexture);
	static void setNormalTexture (ATOM_Texture *normalTexture);
	static void setHeightTexture (ATOM_Texture *heightTexture);
	static void requireRender (void);
	static bool isRequireRender (void);
	static void setWaterLevel (float level);
	static void setWaveHeight (float height);
	static void setTextureProjMatrix (const ATOM_Matrix4x4f &matrix);
	static void setLightDir (const ATOM_Vector3f &lightDir);
	static void setFresnel (float fresnel);
	static void setWaveScale (const ATOM_Vector2f &scale);
	static void setShoreHardness (float value);
	static void setNormalScale (float value);
	static void setFadeSpeed (float value);
	static void setDisplace (float value);
	static void setVisibility (float value);
	static void setWind (const ATOM_Vector2f &wind);
	static void setSurfaceColor (const ATOM_Vector4f &color);
	static void setDepthColor (const ATOM_Vector4f &color);
	static void setSunScale (float sunScale);
	static void setExtinction (const ATOM_Vector4f &extinction);
	static void setShininess (float shininess);
	static void setHeightModifier (const ATOM_Vector4f &heightModifier);
	static void setFoamExistence (const ATOM_Vector3f &foamExistence);
	static void setSpecularColor (const ATOM_Vector4f &color);

private:
	static ATOM_VECTOR<ATOM_Water*> _waterList;
	static ATOM_AUTOPTR(ATOM_Material) _material;
	static ATOM_AUTOREF(ATOM_Texture) _reflectionTexture;
	static ATOM_AUTOREF(ATOM_Texture) _foamTexture;
	static ATOM_AUTOREF(ATOM_Texture) _normalTexture;
	static ATOM_AUTOREF(ATOM_Texture) _heightTexture;
	static bool _initialized;
	static bool _requireRender;
	static float _waterLevel;
	static float _waveHeight;
	static ATOM_Matrix4x4f _matTextureProj;
	static ATOM_Vector4f _lightDir;
	static float _fresnel;
	static ATOM_Vector2f _waveScale;
	static float _shoreHardness;
	static float _normalScale;
	static float _fadeSpeed;
	static float _displace;
	static float _visibility;
	static ATOM_Vector2f _wind;
	static ATOM_Vector4f _surfaceColor;
	static ATOM_Vector4f _depthColor;
	static ATOM_Vector4f _specularColor;
	static float _sunScale;
	static ATOM_Vector4f _extinction;
	static ATOM_Vector4f _heightModifier;
	static ATOM_Vector4f _foamExistence;
	static float _shininess;
};

#endif // __ATOM_ENGINE_POSTEFFECT_WATER
