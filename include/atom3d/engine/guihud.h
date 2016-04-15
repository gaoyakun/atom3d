/**	\file guihud.h
 *	2Dƽ��GUI����ͼ��ڵ���.
 *
 *	\author ������
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
//! 2Dƽ��GUI����ͼ��ڵ�.
//! ��ATOM_Billboard�಻ͬ���˽ڵ㲻���н���ԶС��͸��Ч��
//! \note ���ô˽ڵ���İ�Χ�к�����Ч
//! \author ������
//! \ingroup engine
class ATOM_ENGINE_API ATOM_GuiHud: public ATOM_Hud
{
	ATOM_CLASS(engine, ATOM_GuiHud, GuiHud)

public:
	//! ���캯��.
	ATOM_GuiHud (void);

	//! ��������.
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
