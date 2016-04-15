#ifndef __ATOM3D_ENGINE_ATMOSPHERE_H
#define __ATOM3D_ENGINE_ATMOSPHERE_H

#if _MSC_VER > 100
# pragma once
#endif

#include "../ATOM_kernel.h"
#include "../ATOM_render.h"
#include "node.h"
#include "cloudplane.h"
#include "drawable.h"

class ATOM_ENGINE_API ATOM_Atmosphere: public ATOM_Node, public ATOM_Drawable
{
	ATOM_CLASS(engine, ATOM_Atmosphere, Atmosphere)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_Atmosphere)

public:
	ATOM_Atmosphere (void);
	virtual ~ATOM_Atmosphere (void);

public:
	virtual void copyAttributesTo (ATOM_Object *obj) const;
	virtual bool loadAttribute(const ATOM_TiXmlElement *xmlelement);
	virtual bool writeAttribute(ATOM_TiXmlElement *xmlelement);
	virtual void accept (ATOM_Visitor &visitor);

public:
	virtual bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);

public:
	void render (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);
	void fini (void);
	void updateTime (ATOM_Camera *camera);

public:
	void setSunIntensityKey (int time, float val);
	void setSunDirectionKey (int time, const ATOM_Vector3f &dir);
	void setGKey (int time, float val);
	void setMieMultiplierKey (int time, float val);
	void setRayleighMultiplierKey (int time, float val);
	void setExposureKey (int time, float val);
	void setLambdaRedKey (int time, float val);
	void setLambdaGreenKey (int time, float val);
	void setLambdaBlueKey (int time, float val);
	void setScaleDepthKey (int time, float val);
	void setCameraHeightKey (int time, float val);
	void setSkydomeRadiusKey (int time, float val);
	void setFadeNearKey (int time, float val);
	void setFadeFarKey (int time, float val);
	void setCloudCoverageKey (int time, float val);
	void setCloudSharpnessKey (int time, float val);
	void setCloudBrightnessKey (int time, float val);
	void setCloudMoveSpeedKey (int time, float val);
	void setCloudMoveDirKey (int time, const ATOM_Vector4f &val);
	void setCloudColorKey (int time, const ATOM_Vector4f &val);
	void setCloudDeformSpeedKey (int time, float val);
	void setMoonColorKey (int time, const ATOM_Vector4f &val);
	void setMoonDirKey (int time, const ATOM_Vector4f &val);
	void setMoonSizeKey (int time, float val);
	float getSunIntensityKey (int time) const;
	const ATOM_Vector3f &getSunDirectionKey (int time) const;
	float getGKey (int time) const;
	float getMieMultiplierKey (int time) const;
	float getRayleighMultiplierKey (int time) const;
	float getExposureKey (int time) const;
	float getLambdaRedKey (int time) const;
	float getLambdaGreenKey (int time) const;
	float getLambdaBlueKey (int time) const;
	float getScaleDepthKey (int time) const;
	float getCameraHeightKey (int time) const;
	float getSkydomeRadiusKey (int time) const;
	float getFadeNearKey (int time) const;
	float getFadeFarKey (int time) const;
	float getCloudCoverageKey (int time) const;
	float getCloudSharpnessKey (int time) const;
	float getCloudBrightnessKey (int time) const;
	float getCloudMoveSpeedKey (int time) const;
	const ATOM_Vector4f &getCloudMoveDirKey (int time) const;
	const ATOM_Vector4f &getCloudColorKey (int time) const;
	float getCloudDeformSpeedKey (int time) const;
	const ATOM_Vector4f &getMoonColorKey (int time) const;
	const ATOM_Vector4f &getMoonDirKey (int time) const;
	float getMoonSizeKey (int time) const;

	float getSunIntensity (void) const;
	const ATOM_Vector3f &getSunDirection (void) const;
	float getG (void) const;
	float getMieMultiplier (void) const;
	float getRayleighMultiplier (void) const;
	float getExposure (void) const;
	float getLambdaRed (void) const;
	float getLambdaGreen (void) const;
	float getLambdaBlue (void) const;
	float getScaleDepth (void) const;
	float getCameraHeight (void) const;
	float getInnerRadius (void) const;
	float getOuterRadius (void) const;
	float getSkydomeRadius (void) const;
	float getFadeNear (void) const;
	float getFadeFar (void) const;
	float getCloudCoverage (void) const;
	float getCloudSharpness (void) const;
	float getCloudBrightness (void) const;
	float getCloudMoveSpeed (void) const;
	const ATOM_Vector4f &getCloudMoveDir (void) const;
	const ATOM_Vector4f &getCloudColor (void) const;
	float getCloudDeformSpeed (void) const;
	const ATOM_Vector4f &getMoonColor (void) const;
	const ATOM_Vector4f &getMoonDir (void) const;
	float getMoonSize (void) const;
	void setNumSamples (int num);
	int getNumSamples (void) const;

public:
	void setMoonTextureFileName (const ATOM_STRING &fileName);
	const ATOM_STRING &getMoonTextureFileName (void) const;
	void setStarfieldTextureFileName (const ATOM_STRING &fileName);
	const ATOM_STRING &getStarfieldTextureFileName (void) const;
	void setStarfieldTextureRepeat (float val);
	float getStarfieldTextureRepeat (void) const;
	ATOM_Material *getMaterial (void) const;

protected:
	virtual bool onLoad (ATOM_RenderDevice *device);

private:
	void loadKeys(const char *name, float defaultValue, ATOM_TimeValueT<float> &keys, const ATOM_TiXmlElement *e) const;
	void writeKeys(const char *name, const ATOM_TimeValueT<float> &keys, ATOM_TiXmlElement *e) const;
	void loadVector4Keys(const char *name, const ATOM_Vector4f &defaultValue, ATOM_TimeValueT<ATOM_Vector4f> &keys, const ATOM_TiXmlElement *e) const;
	void writeVector4Keys(const char *name, const ATOM_TimeValueT<ATOM_Vector4f> &keys, ATOM_TiXmlElement *e) const;
	void loadVector3Keys(const char *name, const ATOM_Vector3f &defaultValue, ATOM_TimeValueT<ATOM_Vector3f> &keys, const ATOM_TiXmlElement *e) const;
	void writeVector3Keys(const char *name, const ATOM_TimeValueT<ATOM_Vector3f> &keys, ATOM_TiXmlElement *e) const;
	ATOM_Vector4f calcSkyColor (const ATOM_Vector3f &direction, const ATOM_Vector3f &sunDir) const;
	ATOM_Vector4f calcSkyColorWithoutPhase (const ATOM_Vector3f &direction, const ATOM_Vector3f &sunDir) const;
	ATOM_Vector4f calcSkyColor2 (const ATOM_Vector3f &position, const ATOM_Vector3f &sunDir) const;
	void syncColor (const ATOM_Vector3f &sunDir, bool ff);
	bool createCloud (ATOM_RenderDevice *device);

protected:
	ATOM_AUTOREF(ATOM_VertexArray) _skyDomeVertices;
	ATOM_AUTOREF(ATOM_VertexArray) _skyDomeColors;
	ATOM_AUTOREF(ATOM_IndexArray) _skyDomeIndices;
	ATOM_VertexDecl _vertexDecl;
	ATOM_AUTOPTR(ATOM_Material) _scatteringMaterial;
	ATOM_STRING _moonTextureFileName;
	ATOM_STRING _starfieldTextureFileName;

	ATOM_TimeValueT<float> _skydomeRadius;
	ATOM_TimeValueT<float> _sunIntensity;
	ATOM_TimeValueT<ATOM_Vector3f> _sunDirection;
	ATOM_TimeValueT<float> _g;
	ATOM_TimeValueT<float> _mieMultiplier;
	ATOM_TimeValueT<float> _rayleighMultiplier;
	ATOM_TimeValueT<float> _exposure;
	ATOM_TimeValueT<float> _lambdaR;
	ATOM_TimeValueT<float> _lambdaG;
	ATOM_TimeValueT<float> _lambdaB;
	ATOM_TimeValueT<float> _scaleDepth;
	ATOM_TimeValueT<float> _cameraHeight;
	ATOM_TimeValueT<float> _fadeNear;
	ATOM_TimeValueT<float> _fadeFar;
	ATOM_TimeValueT<float> _cloudCoverage;
	ATOM_TimeValueT<float> _cloudSharpness;
	ATOM_TimeValueT<float> _cloudBrightness;
	ATOM_TimeValueT<float> _cloudDeformSpeed;
	ATOM_TimeValueT<float> _cloudMoveSpeed;
	ATOM_TimeValueT<ATOM_Vector4f> _cloudMoveDir;
	ATOM_TimeValueT<ATOM_Vector4f> _cloudColor;
	ATOM_TimeValueT<ATOM_Vector4f> _moonColor;
	ATOM_TimeValueT<ATOM_Vector4f> _moonDir;
	ATOM_TimeValueT<float> _moonSize;

	float _skydomeRadiusVal;
	float _sunIntensityVal;
	ATOM_Vector3f _sunDirectionVal;
	float _gVal;
	float _mieMultiplierVal;
	float _rayleighMultiplierVal;
	float _exposureVal;
	float _lambdaRVal;
	float _lambdaGVal;
	float _lambdaBVal;
	float _scaleDepthVal;
	float _cameraHeightVal;
	float _fadeNearVal;
	float _fadeFarVal;
	float _cloudCoverageVal;
	float _cloudSharpnessVal;
	float _cloudBrightnessVal;
	float _cloudDeformSpeedVal;
	float _cloudMoveSpeedVal;
	ATOM_Vector4f _cloudMoveDirVal;
	ATOM_Vector4f _cloudColorVal;
	ATOM_Vector4f _moonColorVal;
	ATOM_Vector4f _moonDirVal;
	float _moonSizeVal;

	float _starfieldTextureRepeat;

	int _numSamples;
	bool _colorChanged;

	//ATOM_CloudPlane *_cloudPlane;

	ATOM_Vector3f _oldSunDirection;
};

#endif // __ATOM3D_ENGINE_ATMOSPHERE_H
