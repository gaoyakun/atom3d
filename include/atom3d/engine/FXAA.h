/**	\file FXAA.h
 *	ATOM_FXAAEffect类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_FXAA_H
#define __ATOM3D_ENGINE_FXAA_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "posteffectchain.h"

//! \class ATOM_FXAAEffect
//! FXAA抗锯齿后处理效果类
//! \author 高雅昆
//! \ingroup engine
class ATOM_ENGINE_API ATOM_FXAAEffect: public ATOM_PostEffect
{
	ATOM_CLASS(engine, ATOM_FXAAEffect, ATOM_FXAAEffect)

public:
	//! 构造函数
	ATOM_FXAAEffect (void);

	//! 析构函数
	virtual ~ATOM_FXAAEffect (void);

	virtual void enable (int b);
	virtual int isEnabled (void) const;

	virtual bool init(ATOM_RenderDevice *device);

public:
	virtual bool render (ATOM_RenderDevice *device);

private:
	ATOM_AUTOPTR(ATOM_Material) _material;
	bool _materialError;
};


#endif // __ATOM3D_ENGINE_FXAA_H

/*! @} */
