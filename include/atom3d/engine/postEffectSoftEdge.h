#ifndef _POST_EFFECT_SOFTEDGE_H_
#define _POST_EFFECT_SOFTEDGE_H_


#if _MSC_VER > 1000
# pragma once
#endif

#include "posteffectchain.h"

class ATOM_ENGINE_API ATOM_SoftEdgeEffect: public ATOM_PostEffect
{
	ATOM_CLASS(engine, ATOM_SoftEdgeEffect, ATOM_SoftEdgeEffect)

public:
	//! 构造函数
	ATOM_SoftEdgeEffect (void);

	//! 析构函数
	virtual ~ATOM_SoftEdgeEffect (void);

	void setSoftEdgeTexture(const char* file);

public:
	virtual bool render (ATOM_RenderDevice *device);

private:

	ATOM_AUTOPTR(ATOM_Material) _material;

	ATOM_STRING					_softEdgeMaskTexture;
	ATOM_AUTOREF(ATOM_Texture)	_softTexture;

	bool _materialError;
};

#endif
