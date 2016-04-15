#include "stdafx.h"
#include "propertylist.h"
#include "gui_canvas.h"

ATOM_PropertyList::ATOM_PropertyList()
{
	ATOM_STACK_TRACE(ATOM_PropertyList::ATOM_PropertyList);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_PropertyList::ATOM_PropertyList (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
: ATOM_ListView (parent, rect, style|ATOM_Widget::Control, id, showState)
{
	ATOM_STACK_TRACE(ATOM_PropertyList::ATOM_PropertyList);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

	setColumnCount(2);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_PropertyList::~ATOM_PropertyList (void)
{
	ATOM_STACK_TRACE(ATOM_PropertyList::~ATOM_PropertyList);
}

void ATOM_PropertyList::setColumnWidth(int nameWidth, int valueWidth)
{
	ATOM_STACK_TRACE(ATOM_PropertyList::setColumnWidth);

	ATOM_ListView::setColumnWidth(0, nameWidth);
	ATOM_ListView::setColumnWidth(1, valueWidth);
}

ATOM_Label* ATOM_PropertyList::setLabel(int raw, const char* name, const char* str, int id, unsigned style, int customButtonId)
{
	ATOM_STACK_TRACE(ATOM_PropertyList::setLabel);

	if(ATOM_Widget::AnyId == customButtonId)
	{
		ATOM_ListView::setLabel(0, raw, name, ATOM_Widget::AnyId, style);
	}
	else
	{
		ATOM_ListView::setButton(0, raw, name, customButtonId, style);
	}
	return ATOM_ListView::setLabel(1, raw, str, id, style);
}

ATOM_Edit* ATOM_PropertyList::setEdit(int raw, const char* name, const char* str, int id, unsigned style, int customButtonId)
{
	ATOM_STACK_TRACE(ATOM_PropertyList::setEdit);

	if(ATOM_Widget::AnyId == customButtonId)
	{
		ATOM_ListView::setLabel(0, raw, name, ATOM_Widget::AnyId, style);
	}
	else
	{
		ATOM_ListView::setButton(0, raw, name, customButtonId, style);
	}
	return ATOM_ListView::setEdit(1, raw, str, id, style);
}

ATOM_Button* ATOM_PropertyList::setButton(int raw, const char* name, const char* str, int id, unsigned style, int customButtonId)
{
	ATOM_STACK_TRACE(ATOM_PropertyList::setButton);

	if(ATOM_Widget::AnyId == customButtonId)
	{
		ATOM_ListView::setLabel(0, raw, name, ATOM_Widget::AnyId, style);
	}
	else
	{
		ATOM_ListView::setButton(0, raw, name, customButtonId, style);
	}
	return ATOM_ListView::setButton(1, raw, str, id, style);
}

ATOM_Slider* ATOM_PropertyList::setHSlider(int raw, const char* name, int min, int max, int pos, int id, unsigned style, int customButtonId)
{
	ATOM_STACK_TRACE(ATOM_PropertyList::setHSlider);

	if(ATOM_Widget::AnyId == customButtonId)
	{
		ATOM_ListView::setLabel(0, raw, name, ATOM_Widget::AnyId, style);
	}
	else
	{
		ATOM_ListView::setButton(0, raw, name, customButtonId, style);
	}
	return ATOM_ListView::setHSlider(1, raw, min, max, pos, id, style);
}

ATOM_ScrollBar* ATOM_PropertyList::setHScrollBar(int raw, const char* name, float min, float max, float pos, float step, int id, unsigned style, int customButtonId)
{
	ATOM_STACK_TRACE(ATOM_PropertyList::setHScrollBar);

	if(ATOM_Widget::AnyId == customButtonId)
	{
		ATOM_ListView::setLabel(0, raw, name, ATOM_Widget::AnyId, style);
	}
	else
	{
		ATOM_ListView::setButton(0, raw, name, customButtonId, style);
	}
	ATOM_ScrollBar* bar = ATOM_ListView::setHScrollBar(1, raw, min, max, pos, step, id, style);
	bar->getSlider()->setFontColor(getFontColor());
	bar->getSlider()->setFont(getFont());
	bar->getSlider()->showText(true);
	return bar;
}

ATOM_Checkbox* ATOM_PropertyList::setCheckbox(int raw, const char* name, bool checked, int id, unsigned style, int customButtonId)
{
	ATOM_STACK_TRACE(ATOM_PropertyList::setCheckbox);

	if(ATOM_Widget::AnyId == customButtonId)
	{
		ATOM_ListView::setLabel(0, raw, name, ATOM_Widget::AnyId, style);
	}
	else
	{
		ATOM_ListView::setButton(0, raw, name, customButtonId, style);
	}
	return ATOM_ListView::setCheckbox(1, raw, checked, id, style);
}

ATOM_WidgetType ATOM_PropertyList::getType (void) const
{
	ATOM_STACK_TRACE(ATOM_PropertyList::getType);

	return WT_PROPERTYLIST;
}

ATOM_Widget* ATOM_PropertyList::getNameWidget(int raw)
{
	ATOM_STACK_TRACE(ATOM_PropertyList::getNameWidget);

	return getWidget(0, raw);
}

ATOM_Widget* ATOM_PropertyList::getValueWidget(int raw)
{
	ATOM_STACK_TRACE(ATOM_PropertyList::getValueWidget);

	return getWidget(1, raw);
}



