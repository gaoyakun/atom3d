#ifndef _POST_EFFECT_SUNSHAFT_H_
#define _POST_EFFECT_SUNSHAFT_H_


#if _MSC_VER > 1000
# pragma once
#endif

#include "posteffectchain.h"

class ATOM_ENGINE_API ATOM_SunShaftEffect : public ATOM_PostEffect
{
	ATOM_CLASS(engine, ATOM_SunShaftEffect, ATOM_SunShaftEffect)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_SunShaftEffect)

public:
	//! 构造函数
	ATOM_SunShaftEffect (void);

	//! 析构函数
	virtual ~ATOM_SunShaftEffect (void);

public:

	void enable (int b);
	int isEnabled (void) const;

	virtual bool render (ATOM_RenderDevice *device);

	virtual bool init(ATOM_RenderDevice *device);
	virtual bool destroy();

	// 属性设置
public:

	void setRayAttenuation(const float attenuation);
	const float getRayAttenuation(void) const;

	void setShaftAmount(const float amount);
	const float getShaftAmount(void) const;

	void setRayAmount(const float amount);
	const float getRayAmount(void) const;

	void setCustomColorInfluence(const float customInflence);
	const float getCustomColorInfluence(void) const;

	void setCustomRayColor(const ATOM_Vector4f & color);
	const ATOM_Vector4f & getCustomRayColor(void) const;

	void setCustomLightSource(const ATOM_Vector3f customLS);
	const ATOM_Vector3f & getCustomLightSource(void) const;

private:

	void genShaftMask(ATOM_RenderDevice *device);
	void genSunShaft(ATOM_RenderDevice *device);
	void displayShaft(ATOM_RenderDevice *device);

	ATOM_AUTOPTR(ATOM_Material)		_material;

	ATOM_AUTOREF(ATOM_Texture)		_maskTex;
	ATOM_AUTOREF(ATOM_Texture)		_shaftTex;

	float							_rayAttenuation;
	float							_shaftAmount;
	float							_rayAmount;
	float							_customColorInfluence;
	ATOM_Vector4f					_customRayColor;

	ATOM_Vector3f					_customLS;	
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
