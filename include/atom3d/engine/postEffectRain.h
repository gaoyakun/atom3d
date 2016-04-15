#ifndef _POST_EFFECT_RAIN_H_
#define _POST_EFFECT_RAIN_H_


#if _MSC_VER > 1000
# pragma once
#endif

#include "posteffectchain.h"

class ATOM_ENGINE_API ATOM_RainEffect : public ATOM_PostEffect
{
	ATOM_CLASS(engine, ATOM_RainEffect, ATOM_RainEffect)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_RainEffect)

public:
	//! 构造函数
	ATOM_RainEffect (void);

	//! 析构函数
	virtual ~ATOM_RainEffect (void);

	virtual void enable (int b);
	virtual int isEnabled (void) const;

	virtual bool init(ATOM_RenderDevice *device);
	virtual bool destroy();

public:
	virtual bool render (ATOM_RenderDevice *device);

	void setQualityLevel(int quality);
	int getQualityLevel()const;

	// 属性设置
public:
	/*
	x: fdropspeed,y: fhighlight,z: fsize,w: fMultiplier
	*/
	//void setRainDropParams(const ATOM_Vector4f & params);
	//const ATOM_Vector4f & getRainDropParams(void) const;

	void setDropSpeed(const ATOM_Vector4f & speed);
	const ATOM_Vector4f & getDropSpeed(void) const;

	void setHighLight(const ATOM_Vector4f & highlight);
	const ATOM_Vector4f & getHighLight(void) const;

	void setConeSize(const ATOM_Vector4f & conesize);
	const ATOM_Vector4f & getConeSize(void) const;

	void setTexTilingFactor01(const ATOM_Vector4f & tiling);
	const ATOM_Vector4f & getTexTilingFactor01(void) const;

	void setTexTilingFactor23(const ATOM_Vector4f & tiling);
	const ATOM_Vector4f & getTexTilingFactor23(void) const;

	void setRainFallTextureFileName (const ATOM_STRING &filename);
	const ATOM_STRING &getRainFallTextureFileName (void) const;

	void setWindSpeed(const ATOM_Vector4f & windSpeed);
	const ATOM_Vector4f & getWindSpeed(void) const;
	void setTopScale(const ATOM_Vector4f & topScale);
	const ATOM_Vector4f & getTopScale(void) const;

private:

	enum 
	{
		RAIN_QUALITY_LOW = 0,
		RAIN_QUALITY_MEDIUM,
		RAIN_QUALITY_HIGH,
	};
	struct SVF_P3F_T2F
	{
		ATOM_Vector3f xyz;
		ATOM_Vector2f st;
	};

	bool			_recreateRainRT;
	int				_width_rt;
	int				_height_rt;
	int				_qualityLevel;
	int				_layerCount;

	//////////////////////////////////////////////////////////////////////////
	ATOM_Vector4f	_fRainDropsSpeed;		// = 1.0f;			// 4层的速度
	ATOM_Vector4f	_fHighlight;			// = 1.0f;			// 4层的亮度
	ATOM_Vector4f	_fSizeMult;				// = 5.0f;			// 4层的大小
	ATOM_Vector4f	_vTexTiling[2];			// = 0.15f			// 4层的纹理平铺
	ATOM_Vector4f	_fWindSpeed;			// = 1.0f;			// 4层的摆动幅度 
	ATOM_Vector4f	_fTopScale;				// = 0.5f;			// 4层的顶层缩放因子 
	//////////////////////////////////////////////////////////////////////////
	//float	_fMultiplier;

	ATOM_AUTOREF(ATOM_Texture)		_rainRT;
	ATOM_AUTOREF(ATOM_Texture)		_rainFallTex;
	ATOM_AUTOPTR(ATOM_Material)		_material;
	ATOM_AUTOREF(ATOM_VertexArray)	_vertexArrayCone;
	ATOM_VertexDecl					_vertexDecl;

	int								_coneVertexCount;

	ATOM_STRING						_rainFallTexFileName;

	bool createBuffer();
	bool createRainRT();
	bool createMaterial();
	bool createRainFallTex();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
