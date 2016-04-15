#include "StdAfx.h"
#include "dialog.h"
#include "gui_events.h"

ATOM_VECTOR<ATOM_Dialog::LockScreenInfo> ATOM_Dialog::_lockScreenInfoStack;

ATOM_BEGIN_EVENT_MAP(ATOM_Dialog, ATOM_TopWindow)
	ATOM_EVENT_HANDLER(ATOM_Dialog, ATOM_TopWindowCloseEvent, onClose)
	ATOM_EVENT_HANDLER(ATOM_Dialog, ATOM_WidgetCommandEvent, onCommand)
ATOM_END_EVENT_MAP

ATOM_Dialog::ATOM_Dialog()
{
	ATOM_STACK_TRACE(ATOM_Dialog::ATOM_Dialog);

	_clientImageId = ATOM_IMAGEID_CONTROL_BKGROUND;
	_initialized = false;
	_isShowModal = false;
	_exitModalCode = 0;

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_Dialog::ATOM_Dialog (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
	:
ATOM_TopWindow (parent, rect, style, id, showState)
{
	ATOM_STACK_TRACE(ATOM_Dialog::ATOM_Dialog);

	_clientImageId = ATOM_IMAGEID_CONTROL_BKGROUND;
	_initialized = false;
	_isShowModal = false;
	_exitModalCode = 0;

	resize (rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_Dialog::~ATOM_Dialog (void)
{
	ATOM_STACK_TRACE(ATOM_Dialog::~ATOM_Dialog);

	if (_isShowModal)
	{
		endModal (0);
	}
}

ATOM_WidgetType ATOM_Dialog::getType (void) const
{
	ATOM_STACK_TRACE(ATOM_Dialog::getType);

	return WT_DIALOG;
}

int ATOM_Dialog::showModal (ATOM_ColorARGB darkColor)
{
	ATOM_STACK_TRACE(ATOM_Dialog::showModal);

	if (!_isShowModal)
	{
		ATOM_ASSERT(_parent);

		ATOM_DialogInitEvent e(getId());
		handleEvent (&e);

		show (ATOM_Widget::ShowNormal);
		_isShowModal = true;
		_guiRenderer->setModalDialog (this);
		_guiRenderer->setFocus (this);
		_guiRenderer->setCapture (nullptr);

		_lockScreenInfoStack.resize (_lockScreenInfoStack.size() + 1);
		_lockScreenInfoStack.back().lockColor = darkColor;
		_lockScreenInfoStack.back().lockDialog = this;

		int retVal;
		while (!ATOM_APP->pumpEvents (&retVal))
		{
			ATOM_ContentStream::processRequests();

			if (!_isShowModal)
			{
				_lockScreenInfoStack.pop_back ();
				return _exitModalCode;
			}
		}

		_lockScreenInfoStack.pop_back ();
		ATOM_APP->postQuitEvent (retVal);
	}

	return _exitModalCode;
}

void ATOM_Dialog::endModal (int retVal)
{
	ATOM_STACK_TRACE(ATOM_Dialog::endModal);

	if (_isShowModal)
	{
		show (ATOM_Widget::Hide);
		_guiRenderer->removeModalDialog (this);
		_isShowModal = false;
		_exitModalCode = retVal;
	}
}

void ATOM_Dialog::onCommand (ATOM_WidgetCommandEvent *event)
{
	if (_endIDs.find (event->id) != _endIDs.end ())
	{
		endModal (event->id);
	}
}

void ATOM_Dialog::onClose (ATOM_TopWindowCloseEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Dialog::onClose);

	if (_isShowModal)
	{
		endModal (0);
	}
	else
	{
		show (ATOM_Widget::Hide);
	}
}

void ATOM_Dialog::addEndId (int id)
{
	_endIDs.insert (id);
}

void ATOM_Dialog::removeEndId (int id)
{
	ATOM_SET<int>::iterator it = _endIDs.find (id);
	if (it != _endIDs.end ())
	{
		_endIDs.erase (id);
	}
}

void ATOM_Dialog::draw (void)
{
	if (!_lockScreenInfoStack.empty() && _lockScreenInfoStack.back().lockDialog == this && _lockScreenInfoStack.back().lockColor.getRaw() != 0xFFFFFFFF)
	{
		_guiRenderer->drawDark (_lockScreenInfoStack.back().lockColor);
	}

	ATOM_TopWindow::draw ();
}

