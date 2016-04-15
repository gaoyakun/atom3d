#ifndef _POST_EFFECT_HEATHAZE_H_
#define _POST_EFFECT_HEATHAZE_H_


#if _MSC_VER > 1000
# pragma once
#endif

#include "posteffectchain.h"

class ATOM_ENGINE_API ATOM_HeatHazeEffect : public ATOM_PostEffect
{
	ATOM_CLASS(engine, ATOM_HeatHazeEffect, ATOM_HeatHazeEffect)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_HeatHazeEffect)

public:
	//! 构造函数
	ATOM_HeatHazeEffect (void);

	//! 析构函数
	virtual ~ATOM_HeatHazeEffect (void);

public:
	virtual bool render (ATOM_RenderDevice *device);

	virtual bool init(ATOM_RenderDevice *device);
	virtual bool destroy();

	// 属性设置
public:

	void setPerturbationScale(const float perturbscale);
	const float getPerturbationScale(void) const;

	void setPerturbationSpeed(const float perturbspeed);
	const float getPerturbationSpeed(void) const;

	void setPerturbationTiling(const float perturbtiling);
	const float getPerturbationTiling(void) const;

	void setPerturbationDistance(const float perturbdist);
	const float getPerturbationDistance(void) const;

	void setPerturbationTextureFileName (const ATOM_STRING &filename);
	const ATOM_STRING & getPerturbationTextureFileName (void) const;

private:

	float							_perturbScale;
	float							_perturbSpeed;
	float							_perturbTiling;
	float							_perturbDistance;

	ATOM_STRING						_perturbationTexFileName;

	ATOM_AUTOREF(ATOM_Texture)		_perturbationTex;
	ATOM_AUTOPTR(ATOM_Material)		_material;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
