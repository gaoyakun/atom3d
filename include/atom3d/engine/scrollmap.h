/**	\file scrollmap.h
 *	æÌ÷·¿‡.
 *
 *	\author ∞◊¥Û π
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_SCROLLMAP_H
#define __ATOM3D_ENGINE_SCROLLMAP_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_render.h"

#include "gui_font.h"
#include "widget.h"
#include "label.h"



//! æÌ÷·
class ATOM_ENGINE_API ATOM_ScrollMap: public ATOM_Widget
{
public:
	ATOM_ScrollMap();
	ATOM_ScrollMap (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState = ATOM_Widget::Hide);
	virtual ~ATOM_ScrollMap (void);

public:
	virtual ATOM_WidgetType getType (void) const;

	//! ÃÌº”Õº∆¨
	ATOM_Label* addImage(int imageId, const ATOM_Rect2Di& rc, int id = ATOM_Widget::AnyId);

public:

	ATOM_DECLARE_EVENT_MAP(ATOM_ScrollMap, ATOM_Widget)

protected:
};



#endif // __ATOM3D_ENGINE_SCROLLMAP_H
/*! @} */
