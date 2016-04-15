#include "StdAfx.h"
#include "menubar.h"



ATOM_BEGIN_EVENT_MAP(ATOM_MenuBar, ATOM_PopupMenu)
	  ATOM_EVENT_HANDLER(ATOM_MenuBar, ATOM_WidgetDrawClientEvent, onPaint)
ATOM_END_EVENT_MAP

ATOM_MenuBar::ATOM_MenuBar (void)
{
	ATOM_STACK_TRACE(ATOM_MenuBar::ATOM_MenuBar);

	_checkedFlagSize.w = 0;
	_checkedFlagSize.h = 0;
	_uniformWidth = false;
	_autoPopup = false;
	_popping = false;

	setLayoutable (false);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_MenuBar::ATOM_MenuBar (ATOM_GUIRenderer *renderer): ATOM_PopupMenu(renderer)
{
	ATOM_STACK_TRACE(ATOM_MenuBar::ATOM_MenuBar);

	_creationStyle &= ~ATOM_Widget::Popup;
	_checkedFlagSize.w = 0;
	_checkedFlagSize.h = 0;
	_uniformWidth = false;
	_autoPopup = false;
	_popping = false;
	setStyle ((getStyle()|ATOM_Widget::TopMost|ATOM_Widget::NonLayoutable) & ~ATOM_Widget::Border);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_MenuBar::~ATOM_MenuBar (void)
{
	ATOM_STACK_TRACE(ATOM_MenuBar::~ATOM_MenuBar);

	if (_parent)
	{
		_parent->setMenuBar (0);
	}
}

void ATOM_MenuBar::calcLayout (const ATOM_Rect2Di &rc)
{
	ATOM_STACK_TRACE(ATOM_MenuBar::calcLayout);

	ATOM_PopupMenu::calcLayout (rc);

	unsigned x_accum = 0;
	unsigned y_accum = 0;

	for (unsigned i = 0; i < _menuLayout.itemRects.size(); ++i)
	{
		ATOM_Rect2Di &itemRect = _menuLayout.itemRects[i];
		const ATOM_Point2Di textOffset = _menuLayout.textPositions[i] - itemRect.point;

		if (x_accum > 0 && x_accum + itemRect.size.w > rc.size.w)
		{
			_menuLayout.itemRects[i-1].size.w += rc.size.w - x_accum;
			x_accum = 0;
			y_accum += itemRect.size.h;
		}

		itemRect.point.x = x_accum;
		itemRect.point.y = y_accum;
		x_accum += itemRect.size.w;

		_menuLayout.textPositions[i] = itemRect.point + textOffset;
	}

	if (x_accum == 0 && y_accum == 0)
	{
		_widgetRect = rc;
		_widgetRect.size.h = 0;
		_clientRect = _widgetRect;
		_titleBarRect = _widgetRect;
	}
	else
	{
		_menuLayout.itemRects.back().size.w += rc.size.w - x_accum;
		_widgetRect = rc;
		_widgetRect.size.h = y_accum + _menuLayout.itemRects[0].size.h;
		_clientRect = _widgetRect;
		_clientRect.point.x = 0;
		_clientRect.point.y = 0;
		_titleBarRect = _widgetRect;
		_titleBarRect.size.h = 0;
	}
}

ATOM_Point2Di ATOM_MenuBar::calcSubMenuPosition (unsigned item) const
{
	ATOM_STACK_TRACE(ATOM_MenuBar::calcSubMenuPosition);

	ATOM_ASSERT(item < _menuLayout.itemRects.size());
	const ATOM_Rect2Di &rcItem = _menuLayout.itemRects[item];
	return ATOM_Point2Di (_widgetRect.point.x + rcItem.point.x, _widgetRect.point.y + rcItem.point.y + rcItem.size.h);
}

void ATOM_MenuBar::collapse (void)
{
	ATOM_STACK_TRACE(ATOM_MenuBar::collapse);

	if (_expand)
	{
		_expand->collapse ();
	}

	_popping = false;
	_clickonce = false;
}

void ATOM_MenuBar::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_MenuBar::onPaint);

	ATOM_Widget::onPaint (event);

	callParentHandler (event);
}


