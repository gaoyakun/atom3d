#ifndef __ATOM3D_ENGINE_VIEWSTACK_H
#define __ATOM3D_ENGINE_VIEWSTACK_H

#include "panel.h"

class ATOM_ENGINE_API ATOM_ViewStack: public ATOM_Panel
{
public:
	ATOM_ViewStack (void);
	ATOM_ViewStack (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);
	virtual ~ATOM_ViewStack (void);

public:
	virtual ATOM_WidgetType getType (void) const;

public:
	void setActivePage (int id);
	int getActivePage (void) const;

public:
	void onIdle (ATOM_WidgetIdleEvent *event);

private:
	int _activePageId;

	ATOM_DECLARE_EVENT_MAP(ATOM_ViewStack, ATOM_Panel)
};

#endif // __ATOM3D_ENGINE_VIEWSTACK_H

