/**	\file colorgradingeffect.h
 *	ATOM_ColorGradingEffect类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_COLORGRADINGEFFECT_H
#define __ATOM3D_ENGINE_COLORGRADINGEFFECT_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "posteffectchain.h"

//! \class ATOM_ColorGradingEffect
//! 校色后处理效果类
//! \author 高雅昆
//! \ingroup engine
class ATOM_ENGINE_API ATOM_ColorGradingEffect: public ATOM_PostEffect
{
	ATOM_CLASS(engine, ATOM_ColorGradingEffect, ATOM_ColorGradingEffect)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_ColorGradingEffect)

public:
	//! 构造函数
	ATOM_ColorGradingEffect (void);

	//! 构造函数
	//! \param lutTexture 颜色查表贴图，由编辑器生成
	//! \param linearFilter true使用双线性过滤 false使用点采样
	ATOM_ColorGradingEffect (ATOM_Texture *lutTexture);

	//! 析构函数
	virtual ~ATOM_ColorGradingEffect (void);

public:
	//! 设置颜色查表贴图
	//! \param lutTexture 颜色查表贴图，由编辑器生成
	void setLUT (ATOM_Texture *lutTexture);

	//! 获取颜色查表贴图
	//! \return 颜色查表贴图
	ATOM_Texture *getLUT (void) const;

	//! 设置颜色查表贴图
	//! \param lutTexture 颜色查表贴图，由编辑器生成
	void setLUTTextureFileName (const ATOM_STRING &fileName);

	//! 获取颜色查表贴图
	//! \return 颜色查表贴图
	const ATOM_STRING &getLUTTextureFileName (void) const;

private:
	void createDefaultLutTexture (ATOM_RenderDevice *device);

public:
	virtual bool render (ATOM_RenderDevice *device);

private:
	ATOM_AUTOREF(ATOM_Texture) _lutTexture;
	ATOM_AUTOPTR(ATOM_Material) _material;
	ATOM_STRING _lutTextureFileName;
	bool _textureDirty;
	bool _materialError;
};


#endif // __ATOM3D_ENGINE_COLORGRADINGEFFECT_H

/*! @} */
