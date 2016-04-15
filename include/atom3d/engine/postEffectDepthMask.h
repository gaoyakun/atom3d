#ifndef _POST_EFFECT_DEPTHMASK_H_
#define _POST_EFFECT_DEPTHMASK_H_


#if _MSC_VER > 1000
# pragma once
#endif

#include "posteffectchain.h"

class ATOM_ENGINE_API ATOM_DepthMaskEffect: public ATOM_PostEffect
{
	ATOM_CLASS(engine, ATOM_DepthMaskEffect, ATOM_DepthMaskEffect)

public:
	//! 构造函数
	ATOM_DepthMaskEffect (void);

	//! 析构函数
	virtual ~ATOM_DepthMaskEffect (void);

public:
	virtual bool render (ATOM_RenderDevice *device);

private:

	ATOM_AUTOPTR(ATOM_Material) _material;
	bool _materialError;
};

#endif
