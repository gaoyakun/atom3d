/**	\file guihud.h
 *	2D平面GUI矩形图像节点类.
 *
 *	\author 高雅昆
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_GUIHUD_H
#define __ATOM3D_ENGINE_GUIHUD_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "hud.h"

class ATOM_GUIRenderer;

//! \class ATOM_GuiHud
//! 2D平面GUI矩形图像节点.
//! 和ATOM_Billboard类不同，此节点不会有近大远小的透视效果
//! \note 调用此节点类的包围盒函数无效
//! \author 高雅昆
//! \ingroup engine
class ATOM_ENGINE_API ATOM_GuiHud: public ATOM_Hud
{
	ATOM_CLASS(engine, ATOM_GuiHud, GuiHud)

public:
	//! 构造函数.
	ATOM_GuiHud (void);

	//! 析构函数.
	virtual ~ATOM_GuiHud (void);

public:
	ATOM_GUIRenderer *getGuiRenderer (void) const;
	void enableUserZValue (bool enable);

public:
	//! \copydoc ATOM_Node::accept
	virtual void accept (ATOM_Visitor &visitor);

public:
	//! \copydoc ATOM_BaseRenderDatas::render
    virtual bool draw (ATOM_RenderDevice* device, ATOM_Camera *camera, ATOM_Material *material);

public:
	virtual void update (const ATOM_Rect2Di &viewport, const ATOM_Matrix4x4f &mvp);

private:
	bool _userZValueEnabled;
	ATOM_GUIRenderer *_guiRenderer;
};

#endif // __ATOM3D_ENGINE_GUIHUD_H
/*! @} */
