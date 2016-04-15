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
	//! ���캯��
	ATOM_ColorFilterEffect (void);

	//! ��������
	virtual ~ATOM_ColorFilterEffect (void);

	virtual int isEnabled (void) const;

public:
	virtual bool render (ATOM_RenderDevice *device);

	virtual bool init(ATOM_RenderDevice *device);

	// ��������
public:

	void setFilterColor( const ATOM_Vector3f & filterColor );
	const ATOM_Vector3f & getFilterColor(void) const;

	void setDensity(const float density);
	const float getDensity(void) const;

	// ��������ʱ���0 - 1 (����:1����ǿ�ȴ�0 - 1,������Ϊ1.0, 2����Ϊ0.5)
	void setSpeed( const float speed );
	const float getSpeed(void) const;

	// ����ʱ�䣨0-1)
	void setTime(const float time);

	// ��õ�ǰ��ǿ��
	const float getCurrentDensity(void) const;

	// ����ͼ
	void setMaskTextureFileName (const ATOM_STRING &filename);
	const ATOM_STRING &getMaskTextureFileName (void) const;

	// ֱ������
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
