#include "StdAfx.h"
#include "panel.h"

ATOM_BEGIN_EVENT_MAP(ATOM_Panel, ATOM_Widget)
	ATOM_EVENT_HANDLER(ATOM_Panel, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	ATOM_EVENT_HANDLER(ATOM_Panel, ATOM_WidgetLButtonUpEvent, onLButtonUp)
	ATOM_EVENT_HANDLER(ATOM_Panel, ATOM_WidgetRButtonDownEvent, onRButtonDown)
	ATOM_EVENT_HANDLER(ATOM_Panel, ATOM_WidgetRButtonUpEvent, onRButtonUp)
	ATOM_EVENT_HANDLER(ATOM_Panel, ATOM_WidgetIdleEvent, onIdle)
	ATOM_EVENT_HANDLER(ATOM_Panel, ATOM_WidgetMouseEnterEvent, onMouseEnter)
	ATOM_EVENT_HANDLER(ATOM_Panel, ATOM_WidgetMouseLeaveEvent, onMouseLeave)
	ATOM_UNHANDLED_EVENT_HANDLER(ATOM_Panel, onUnhandled)
ATOM_END_EVENT_MAP

ATOM_Panel::ATOM_Panel (void)
{
	_eventTransitionEnabled = true;
}

ATOM_Panel::ATOM_Panel (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
	: ATOM_Widget (parent, rect, style, id, showState)
{
	_eventTransitionEnabled = true;
}

ATOM_Panel::~ATOM_Panel (void)
{
}

ATOM_WidgetType ATOM_Panel::getType (void) const
{
	return WT_PANEL;
}

void ATOM_Panel::enableEventTransition (bool enable)
{
	_eventTransitionEnabled = enable;
}

bool ATOM_Panel::isEventTransitionEnabled (void)
{
	return _eventTransitionEnabled;
}

static void convertToParentClientSpace (ATOM_Widget *w, ATOM_Point2Di *pt)
{
	pt->x += w->getClientRect().point.x;
	pt->y += w->getClientRect().point.y;
	pt->x += w->getParent()->getWidgetRect().point.x;
	pt->y += w->getParent()->getWidgetRect().point.y;
	pt->x -= w->getParent()->getClientRect().point.x;
	pt->y -= w->getParent()->getClientRect().point.y;
}

void ATOM_Panel::onLButtonDown (ATOM_WidgetLButtonDownEvent *event)
{
	if (_eventTransitionEnabled && getParent())
	{
		ATOM_Point2Di pt(event->x, event->y);
		convertToParentClientSpace (this, &pt);
		event->x = pt.x;
		event->y = pt.y;
		getParent()->handleEvent (event);
	}
}

void ATOM_Panel::onLButtonUp (ATOM_WidgetLButtonUpEvent *event)
{
	if (_eventTransitionEnabled && getParent())
	{
		ATOM_Point2Di pt(event->x, event->y);
		convertToParentClientSpace (this, &pt);
		event->x = pt.x;
		event->y = pt.y;
		getParent()->handleEvent (event);
	}
}

void ATOM_Panel::onRButtonDown (ATOM_WidgetRButtonDownEvent *event)
{
	if (_eventTransitionEnabled && getParent())
	{
		ATOM_Point2Di pt(event->x, event->y);
		convertToParentClientSpace (this, &pt);
		event->x = pt.x;
		event->y = pt.y;
		getParent()->handleEvent (event);
	}
}

void ATOM_Panel::onRButtonUp (ATOM_WidgetRButtonUpEvent *event)
{
	if (_eventTransitionEnabled && getParent())
	{
		ATOM_Point2Di pt(event->x, event->y);
		convertToParentClientSpace (this, &pt);
		event->x = pt.x;
		event->y = pt.y;
		getParent()->handleEvent (event);
	}
}

void ATOM_Panel::onIdle (ATOM_WidgetIdleEvent *event)
{
}

void ATOM_Panel::onUnhandled (ATOM_Event *event)
{
	if (_eventTransitionEnabled && getParent())
	{
		getParent()->handleEvent (event);
	}
}

void ATOM_Panel::hitTest (int x, int y, ATOM_GUIHitTestResult *result, bool fullTest)
{
	ATOM_Widget::hitTest (x, y, result, fullTest);

	if (_eventTransitionEnabled && (result->hitPoint == HitClient || result->hitPoint == HitFrame))
	{
		result->hitPoint = HitNone;
		result->child = 0;
		result->lug = false;
	}
}

void ATOM_Panel::onMouseEnter( ATOM_WidgetMouseEnterEvent* event )
{
	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->handleEvent(event);
	}
}

void ATOM_Panel::onMouseLeave( ATOM_WidgetMouseLeaveEvent* event )
{
	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->handleEvent(event);
	}
}

