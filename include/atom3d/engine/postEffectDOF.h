#ifndef _POST_EFFECT_DOF_H_
#define _POST_EFFECT_DOF_H_

#if _MSC_VER > 1000
# pragma once
#endif

#include "posteffectchain.h"


class ATOM_ENGINE_API ATOM_DOFEffect: public ATOM_PostEffect
{
	ATOM_CLASS(engine, ATOM_DOFEffect, ATOM_DOFEffect)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_DOFEffect)

public:
	//! 构造函数
	ATOM_DOFEffect (void);

	//! 析构函数
	virtual ~ATOM_DOFEffect (void);

	virtual bool init(ATOM_RenderDevice *device);
	
	virtual bool destroy();

	// enable
	virtual void enable (int b);
	virtual int isEnabled (void) const;

public:

	void setFocusDist(const float focusDist);
	const float getFocusDist(void) const;

	void setFocusRange(const float range);
	const float getFocusRange(void) const;

	void setFocusPower(const float power);
	const float getFocusPower(void) const;

	/*void setFocusZoneNear(const float zoneNear);
	const float getFocusZoneNear(void) const;

	void setFocusZoneFar(const float zoneFar);
	const float getFocusZoneFar(void) const;*/

	void setMinZ(const float range);
	const float getMinZ(void) const;
	void setMinZScale(const float range);
	const float getMinZScale(void) const;
	void setMinZBlendMult(const float range);
	const float getMinZBlendMult(void) const;


	void setBlurCoeff(const float blurCoeff);
	const float getBlurCoeff(void) const;

	void setPPM(const float ppm);
	const float getPPM(void) const;

	void setBokehAngle(const float angle);
	const float getBokehAngle(void) const;
	void setBokehRadius(const float angle);
	const float getBokehRadius(void) const;

	void setBrightness(const float brightness);
	const float getBrightness(void) const;

public:
	virtual bool render (ATOM_RenderDevice *device);

private:
	void genDofBlurMask( ATOM_RenderDevice *device );
	void sceneToSceneScaled ( ATOM_RenderDevice *device );
	void genBlur(ATOM_RenderDevice *device);

	void genBlurBokeh(ATOM_RenderDevice *device);

	bool clearTexture (ATOM_Texture *texture);

	void stretchrect (ATOM_RenderDevice *device,ATOM_Texture *pSrc,ATOM_Texture *pDest,bool bigDownSample);

	bool createRT(ATOM_RenderDevice *device);
	bool createMaterial(ATOM_RenderDevice *device);

	ATOM_AUTOPTR(ATOM_Material) _material;
	bool _materialError;

	ATOM_AUTOREF(ATOM_Texture) _source;				// source
	ATOM_AUTOREF(ATOM_Texture) _dofBlurMaskTex;		// mask texture
	ATOM_AUTOREF(ATOM_Texture) _blurTexs[3];		// 

	unsigned _rtWidth;
	unsigned _rtHeight;

	float	_focusDistance;
	float   _blurCoefficient;
	float   _pixelPerMeter;

	bool    _bokeh;
	float   _bokehAngle;
	float	_radius;
	float   _brightness;

	float	_focusRange;
	float	_focusPower;
	/*float	_focusZoneNear;
	float	_focusZoneFar;*/

	float   _dofMinz;
	float   _dofMinzScale;
	float	_dofMinZBlendMult;

	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)> _maskNode; 


	/*ATOM_Vector2f avSampleOffsets_horizontal[_GAUSSIAN_TAP_COUNT_FINAL];
	ATOM_Vector4f avSampleWeights_horizontal[_GAUSSIAN_TAP_COUNT_FINAL];
	ATOM_Vector2f avSampleOffsets_vertical[_GAUSSIAN_TAP_COUNT_FINAL];
	ATOM_Vector4f avSampleWeights_vertical[_GAUSSIAN_TAP_COUNT_FINAL];*/

	bool 	_bsaveall;

};

#endif
