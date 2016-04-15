#ifndef _POST_EFFECT_COLORFILTER_H_
#define _POST_EFFECT_COLORFILTER_H_


#if _MSC_VER > 1000
# pragma once
#endif

#include "posteffectchain.h"

class ATOM_ENGINE_API ATOM_ColorFilterEffect : public ATOM_PostEffect
{
	ATOM_CLASS(engine, ATOM_ColorFilterEffect, ATOM_ColorFilterEffect)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_ColorFilterEffect)

public:
	//! 构造函数
	ATOM_ColorFilterEffect (void);

	//! 析构函数
	virtual ~ATOM_ColorFilterEffect (void);

	virtual int isEnabled (void) const;

public:
	virtual bool render (ATOM_RenderDevice *device);

	virtual bool init(ATOM_RenderDevice *device);

	// 属性设置
public:

	void setFilterColor( const ATOM_Vector3f & filterColor );
	const ATOM_Vector3f & getFilterColor(void) const;

	void setDensity(const float density);
	const float getDensity(void) const;

	// 经过多少时间从0 - 1 (比如:1秒钟强度从0 - 1,则速率为1.0, 2秒则为0.5)
	void setSpeed( const float speed );
	const float getSpeed(void) const;

	// 设置时间（0-1)
	void setTime(const float time);

	// 获得当前的强度
	const float getCurrentDensity(void) const;

	// 遮罩图
	void setMaskTextureFileName (const ATOM_STRING &filename);
	const ATOM_STRING &getMaskTextureFileName (void) const;

	// 直接设置
	void setMaskTexture(ATOM_Texture* maskTexture);


private:

	ATOM_Vector3f					_filterColor;
	float							_density;
	float							_speed;
	float							_currTime;

	float							_currDensity;
	float							_direction;
	
	ATOM_AUTOPTR(ATOM_Material)		_material;

	ATOM_STRING						_maskTexFileName;
	ATOM_AUTOREF(ATOM_Texture)		_maskTexture;

	
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
