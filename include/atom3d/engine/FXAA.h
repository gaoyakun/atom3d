/**	\file FXAA.h
 *	ATOM_FXAAEffect�������.
 *
 *	\author ������
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
//! FXAA����ݺ���Ч����
//! \author ������
//! \ingroup engine
class ATOM_ENGINE_API ATOM_FXAAEffect: public ATOM_PostEffect
{
	ATOM_CLASS(engine, ATOM_FXAAEffect, ATOM_FXAAEffect)

public:
	//! ���캯��
	ATOM_FXAAEffect (void);

	//! ��������
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
