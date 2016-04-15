#ifndef _POST_EFFECT_SILHOUET_H_
#define _POST_EFFECT_SILHOUET_H_


#if _MSC_VER > 1000
# pragma once
#endif

#include "posteffectchain.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ATOM_ENGINE_API ATOM_SilhouetEffect : public ATOM_PostEffect
{
	ATOM_CLASS(engine, ATOM_SilhouetEffect, ATOM_SilhouetEffect)

public:
	//! 构造函数
	ATOM_SilhouetEffect (void);

	//! 析构函数
	virtual ~ATOM_SilhouetEffect (void);

public:
	virtual bool render (ATOM_RenderDevice *device);

	void	setQualityLevel(int quality);
	int		getQualityLevel()const;
	/*void	setRainLayerParams(float fdropspeed,float fhighlight,float fsize,float fMultiplier);
	void	getRainLayerParams(float& fdropspeed, float& fhighlight,float & fsize, float & fMultiplier) const;*/

private:

	enum 
	{
		RAINLAYER_QUALITY_LOW = 0,
		RAINLAYER_QUALITY_MEDIUM,
		RAINLAYER_QUALITY_HIGH,
	};

	int		_qualityLevel;

	ATOM_Vector4f _color;

	ATOM_AUTOPTR(ATOM_Material)		_material;
};



#endif
