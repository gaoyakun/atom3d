#include "StdAfx.h"
#include "viewstack.h"

ATOM_BEGIN_EVENT_MAP(ATOM_ViewStack, ATOM_Panel)
	ATOM_EVENT_HANDLER(ATOM_ViewStack, ATOM_WidgetIdleEvent, onIdle)
ATOM_END_EVENT_MAP

ATOM_ViewStack::ATOM_ViewStack (void)
{
	_activePageId = ATOM_Widget::AnyId;
}

ATOM_ViewStack::ATOM_ViewStack (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
	: ATOM_Panel(parent, rect, style, id, showState)
{
	_activePageId = ATOM_Widget::AnyId;
}

ATOM_ViewStack::~ATOM_ViewStack (void)
{
}

ATOM_WidgetType ATOM_ViewStack::getType (void) const
{
	return WT_VIEWSTACK;
}

void ATOM_ViewStack::setActivePage (int id)
{
	if (_activePageId != id)
	{
		if (getParent())
		{
			getParent()->queueEvent (ATOM_NEW(ATOM_ViewStackChangedEvent, getId(), id, _activePageId), ATOM_APP);
		}
		_activePageId = id;
	}
}

int ATOM_ViewStack::getActivePage (void) const
{
	return _activePageId;
}

void ATOM_ViewStack::onIdle (ATOM_WidgetIdleEvent *event)
{
	ATOM_Widget *activeWidget = 0;

	bool needUpdateLayout = false;

	for (unsigned i = 0; i < getNumChildren(); ++i)
	{
		ATOM_Widget *c = getChild (i);
		if (c->isPrivate ())
		{
			continue;
		}

		if (!activeWidget && _activePageId != -1 && c->getId() == _activePageId)
		{
			activeWidget = c;

			needUpdateLayout = c->isLayoutable() && c->getShowState() != ATOM_Widget::ShowNormal;
			c->show (ATOM_Widget::ShowNormal);
		}
		else
		{
			c->show (ATOM_Widget::Hide);
		}
	}

	if (needUpdateLayout)
	{
		refreshLayout ();
	}
}

