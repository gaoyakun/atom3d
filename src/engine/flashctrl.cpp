#include "stdafx.h"
#include "flashcontrol.h"
#include "flashctrl.h"
#include "gui_canvas.h"

ATOM_BEGIN_EVENT_MAP(ATOM_FlashCtrl, ATOM_Widget)
	ATOM_EVENT_HANDLER(ATOM_FlashCtrl, ATOM_WidgetCharEvent, onChar)
	ATOM_EVENT_HANDLER(ATOM_FlashCtrl, ATOM_WidgetKeyDownEvent, onKeyDown)
	ATOM_EVENT_HANDLER(ATOM_FlashCtrl, ATOM_WidgetKeyUpEvent, onKeyUp)
	ATOM_EVENT_HANDLER(ATOM_FlashCtrl, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	ATOM_EVENT_HANDLER(ATOM_FlashCtrl, ATOM_WidgetLButtonUpEvent, onLButtonUp)
	ATOM_EVENT_HANDLER(ATOM_FlashCtrl, ATOM_WidgetMouseMoveEvent, onMouseMove)
	ATOM_EVENT_HANDLER(ATOM_FlashCtrl, ATOM_AppWMEvent, onWinMessage)
	ATOM_EVENT_HANDLER(ATOM_FlashCtrl, ATOM_WidgetIdleEvent, onIdle)
	ATOM_EVENT_HANDLER(ATOM_FlashCtrl, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(ATOM_FlashCtrl, ATOM_WidgetResizeEvent, onResize)
	ATOM_EVENT_HANDLER(ATOM_FlashCtrl, ATOM_FlashCallEvent, onFlashCall)
	ATOM_EVENT_HANDLER(ATOM_FlashCtrl, ATOM_WidgetHitTestEvent, onHitTest)
ATOM_END_EVENT_MAP

ATOM_FlashCtrl::ATOM_FlashCtrl()
{
	_flashControl = 0;
	_initFunc = "initSwf";
	_clearFunc = "clearSwf";
	_activateFunc = "showSwf";
	_deactivateFunc = "hideSwf";
	_FPS = 24;
	_tick = 0;
}

ATOM_FlashCtrl::ATOM_FlashCtrl (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
: ATOM_Widget (parent, rect, style|ATOM_Widget::ClipChildren, id, showState)
{
	_flashControl = 0;
	_initFunc = "initSwf";
	_clearFunc = "clearSwf";
	_activateFunc = "showSwf";
	_deactivateFunc = "hideSwf";
	_FPS = 24;
	_tick = 0;
}

ATOM_FlashCtrl::~ATOM_FlashCtrl (void)
{
	if (_flashControl)
	{
		if (!_clearFunc.empty ())
		{
			_flashControl->callFunction (_clearFunc.c_str());
		}
		ATOM_DELETE(_flashControl);
	}
}

ATOM_WidgetType ATOM_FlashCtrl::getType (void) const
{
	return WT_FLASHCTRL;
}

void ATOM_FlashCtrl::loadSWF (const char *filename)
{
	if (_flashControl)
	{
		if (!_clearFunc.empty ())
		{
			_flashControl->callFunction (_clearFunc.c_str());
		}
		ATOM_DELETE (_flashControl);
		_flashControl = 0;
	}

	if (filename)
	{
		_flashControl = ATOM_NEW (FlashControl, this, "", getClientRect().size.w, getClientRect().size.h);
		if (_flashControl->load (filename))
		{
			if (!_initFunc.empty ())
			{
				_flashControl->callFunction (_initFunc.c_str());
				_filename = filename;
			}
			//_flashControl->play ();
		}
		else
		{
			ATOM_DELETE(_flashControl);
			_flashControl = 0;
		}
	}

	_tick = 0;
}

void ATOM_FlashCtrl::onKeyDown (ATOM_WidgetKeyDownEvent *event)
{
}

void ATOM_FlashCtrl::onKeyUp (ATOM_WidgetKeyUpEvent *event)
{
}

void ATOM_FlashCtrl::onChar (ATOM_WidgetCharEvent *event)
{
	if (_flashControl)
	{
		_flashControl->handleCharEvent (event->unicc);
	}
}

void ATOM_FlashCtrl::onLButtonDown(ATOM_WidgetLButtonDownEvent *event)
{
	if (_flashControl)
	{
		_flashControl->injectMouseDown (event->x, event->y);
	}
}

void ATOM_FlashCtrl::onLButtonUp(ATOM_WidgetLButtonUpEvent *event)
{
	if (_flashControl)
	{
		_flashControl->injectMouseUp (event->x, event->y);
	}
}

void ATOM_FlashCtrl::onMouseMove(ATOM_WidgetMouseMoveEvent *event)
{
	if (_flashControl)
	{
		_flashControl->injectMouseMove (event->x, event->y);
	}
}

void ATOM_FlashCtrl::onWinMessage (ATOM_AppWMEvent *event)
{
	if (_flashControl)
	{
		_flashControl->handleKeyEvent (event);
	}
}

void ATOM_FlashCtrl::onIdle (ATOM_WidgetIdleEvent *event)
{
	if (_flashControl)
	{
		unsigned tickNow = ATOM_APP->getFrameStamp().currentTick;
		if (_tick == 0)
		{
			_tick = tickNow;
			_flashControl->update ();
		}
		else
		{
			unsigned elapsed = tickNow - _tick;
			if (elapsed >= 1000/_FPS)
			{
				_flashControl->update ();
				_tick = tickNow;
			}
		}
	}
}

void ATOM_FlashCtrl::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	if (_flashControl)
	{
		event->canvas->drawTexturedRect (_flashControl->getMaterial(), ATOM_Rect2Di(0, 0, getClientRect().size.w, getClientRect().size.h), 0xFFFFFFFF, _flashControl->getTexture(), false);
		//event->canvas->drawMaterialRect (ATOM_Rect2Di(0, 0, getClientRect().size.w, getClientRect().size.h), _flashControl->getMaterial());
	}
}

FlashControl *ATOM_FlashCtrl::getFlashControl (void) const
{
	return _flashControl;
}

void ATOM_FlashCtrl::bind(const char *funcName, const FlashDelegate& callback)
{
	if (_flashControl)
	{
		_flashControl->bind (funcName, callback);
	}
}

void ATOM_FlashCtrl::unbind(const char *funcName)
{
	if (_flashControl)
	{
		_flashControl->unbind (funcName);
	}
}

ATOM_FlashValue ATOM_FlashCtrl::callFunction(const char *funcName, const ATOM_FlashArguments& args)
{
	return _flashControl ? _flashControl->callFunction (funcName, args) : FLASH_VOID;
}

void ATOM_FlashCtrl::onResize (ATOM_WidgetResizeEvent *event)
{
	ATOM_Widget::onResize (event);

	if (_flashControl)
	{
		_flashControl->resize (event->widgetRect.size.w, event->widgetRect.size.h);
	}
	/*
	if (_flashControl && !_filename.empty() && (getClientRect().size.w != _flashControl->getWidth() || getClientRect().size.h != _flashControl->getHeight()))
	{

		loadSWF (_filename.c_str());
	}
	*/
}

void ATOM_FlashCtrl::onFlashCall (ATOM_FlashCallEvent *event)
{
	if (_parent)
	{
		_parent->handleEvent (event);
	}
}

void ATOM_FlashCtrl::onHitTest (ATOM_WidgetHitTestEvent *event)
{
	ATOM_Widget::onHitTest (event);

	if (_flashControl && _flashControl->getTransparency (event->x, event->y) < 5)
	{
		event->hitTestResult.hitPoint = HitNone;
		event->hitTestResult.child = 0;
		event->hitTestResult.lug = false;
	}
}

const char *ATOM_FlashCtrl::getSWFFileName (void) const
{
	return _filename.c_str();
}

void ATOM_FlashCtrl::setInitCallback (const char *name)
{
	_initFunc = name ? name : "";
}

const char *ATOM_FlashCtrl::getInitCallback (void) const
{
	return _initFunc.c_str();
}

void ATOM_FlashCtrl::setClearCallback (const char *name)
{
	_clearFunc = name ? name : "";
}

const char *ATOM_FlashCtrl::getClearCallback (void) const
{
	return _clearFunc.c_str();
}

void ATOM_FlashCtrl::setActivateCallback (const char *name)
{
	_activateFunc = name ? name : "";
}

const char *ATOM_FlashCtrl::getActivateCallback (void) const
{
	return _activateFunc.c_str();
}

void ATOM_FlashCtrl::setDeactivateCallback (const char *name)
{
	_deactivateFunc = name ? name : "";
}

const char *ATOM_FlashCtrl::getDeactivateCallback (void) const
{
	return _deactivateFunc.c_str();
}

void ATOM_FlashCtrl::setFPS (int FPS)
{
	_FPS = FPS ? FPS : 1;
}

int ATOM_FlashCtrl::getFPS (void) const
{
	return _FPS;
}

void ATOM_FlashCtrl::activate (void)
{
	if (!_active)
	{
		_active = true;

		if (_flashControl && !_activateFunc.empty ())
		{
			_flashControl->callFunction (_activateFunc.c_str());
		}
	}
}

void ATOM_FlashCtrl::deactivate (void)
{
	if (_active)
	{
		_active = false;

		if (_flashControl && !_deactivateFunc.empty ())
		{
			_flashControl->callFunction (_deactivateFunc.c_str());
		}
	}
}

bool ATOM_FlashCtrl::isActive (void) const
{
	return _active;
}

