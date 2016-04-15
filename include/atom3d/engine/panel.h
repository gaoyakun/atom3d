#ifndef __ATOM3D_ENGINE_PANEL_H
#define __ATOM3D_ENGINE_PANEL_H

#include "widget.h"

class ATOM_ENGINE_API ATOM_Panel: public ATOM_Widget
{
public:
	ATOM_Panel (void);
	ATOM_Panel (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);
	virtual ~ATOM_Panel (void);

public:
	void enableEventTransition (bool enable);
	bool isEventTransitionEnabled (void);

public:
	virtual ATOM_WidgetType getType (void) const;
	virtual void hitTest (int x, int y, ATOM_GUIHitTestResult *result, bool fullTest);

public:
	void onLButtonDown (ATOM_WidgetLButtonDownEvent *event);
	void onLButtonUp (ATOM_WidgetLButtonUpEvent *event);
	void onRButtonDown (ATOM_WidgetRButtonDownEvent *event);
	void onRButtonUp (ATOM_WidgetRButtonUpEvent *event);
	void onIdle (ATOM_WidgetIdleEvent *event);
	void onUnhandled (ATOM_Event *event);
	void onMouseEnter (ATOM_WidgetMouseEnterEvent *event);
	void onMouseLeave (ATOM_WidgetMouseLeaveEvent *event);
private:
	bool _eventTransitionEnabled;

	ATOM_DECLARE_EVENT_MAP(ATOM_Panel, ATOM_Widget)
};




#endif // __ATOM3D_ENGINE_PANEL_H
