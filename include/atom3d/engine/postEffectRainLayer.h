#ifndef _POST_EFFECT_RAINLAYER_H_
#define _POST_EFFECT_RAINLAYER_H_


#if _MSC_VER > 1000
# pragma once
#endif

#include "posteffectchain.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_RainLayerEffect : public ATOM_PostEffect
{
	ATOM_CLASS(engine, ATOM_RainLayerEffect, ATOM_RainLayerEffect)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_RainLayerEffect)

public:
	//! 构造函数
	ATOM_RainLayerEffect (void);

	//! 析构函数
	virtual ~ATOM_RainLayerEffect (void);

	virtual void enable (int b);
	virtual int isEnabled (void) const;

	virtual bool init(ATOM_RenderDevice *device);
	virtual bool destroy();

	static void rainPuddleTexture_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void rainPuddleParam_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);
	static void rainLayerParam_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS);

public:
	virtual bool render (ATOM_RenderDevice *device);

	/*void	setQualityLevel(int quality);
	int		getQualityLevel()const;*/
	
	void setSequenceTime(const float sequenceTime);
	const float getSequenceTime(void) const;

	void setSequenceFrameCount(const int sequenceFrameCount);
	const int getSequenceFrameCount(void) const;

	void setGlobalDensity(const float density);
	const float getGlobalDensity(void) const;

	void setPuddleTiling(const float tiling);
	const float getPuddleTiling(void) const;

	void setPuddleAmount(const float amount);
	const float getPuddleAmount(void) const;

	void setAttenStartDistance(const float distance);
	const float getAttenStartDistance(void) const;
	void setAttenEndDistance(const float distance);
	const float getAttenEndDistance(void) const;

	void setBumpShiftTextureFileName (const ATOM_STRING &filename);
	const ATOM_STRING &getBumpShiftTextureFileName (void) const;

private:

	/*
	enum 
	{
		RAINLAYER_QUALITY_LOW = 0,
		RAINLAYER_QUALITY_MEDIUM,
		RAINLAYER_QUALITY_HIGH,
	};

	int		_qualityLevel;
	*/

	ATOM_AUTOPTR(ATOM_Material)					_material;

	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Texture)>		_rainWavesTexs;
	
	ATOM_STRING									_rainBumpShiftFileName;
	ATOM_AUTOREF(ATOM_Texture)					_rainBumpShiftTex;

	float   _sequenceTime;
	int		_sequenceCount;
	float   _globalDensity;
	float	_puddleTiling;
	float	_puddleAmount;
	float	_attenStartDistance;
	float	_attenEndDistance;

	bool	_materialError;
};



#endif
