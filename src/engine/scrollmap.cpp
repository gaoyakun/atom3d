#include "stdafx.h"
#include "scrollmap.h"
#include "gui_canvas.h"
#include "label.h"

ATOM_BEGIN_EVENT_MAP(ATOM_ScrollMap, ATOM_Widget)
ATOM_END_EVENT_MAP

ATOM_ScrollMap::ATOM_ScrollMap()
{
	ATOM_STACK_TRACE(ATOM_ScrollMap::ATOM_ScrollMap);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_ScrollMap::ATOM_ScrollMap (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
: ATOM_Widget (parent, rect, style|ATOM_Widget::Control|ATOM_Widget::ClipChildren, id, showState)
{
	ATOM_STACK_TRACE(ATOM_ScrollMap::ATOM_ScrollMap);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

	resize(rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_ScrollMap::~ATOM_ScrollMap (void)
{
	ATOM_STACK_TRACE(ATOM_ScrollMap::~ATOM_ScrollMap);
}

ATOM_WidgetType ATOM_ScrollMap::getType (void) const
{
	return WT_SCROLLMAP;
}

ATOM_Label* ATOM_ScrollMap::addImage(int imageId, const ATOM_Rect2Di& rc, int id)
{
	ATOM_STACK_TRACE(ATOM_ScrollMap::addImage);

	ATOM_Label* label = ATOM_NEW(ATOM_Label, this, rc, int(ATOM_Widget::NoFocus), id, ATOM_Widget::ShowNormal);
	label->setClientImageId(imageId);
	return label;
}




