#include "stdafx.h"
#include "button.h"
#include "gui_canvas.h"


unsigned ATOM_Button::_clickTick = 0;
unsigned ATOM_Button::_clickInterval = 0;

ATOM_BEGIN_EVENT_MAP(ATOM_Button, ATOM_Label)
	ATOM_EVENT_HANDLER(ATOM_Button, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	ATOM_EVENT_HANDLER(ATOM_Button, ATOM_WidgetLButtonUpEvent, onLButtonUp)
	ATOM_EVENT_HANDLER(ATOM_Button, ATOM_WidgetLButtonDblClickEvent, onLButtonDblClick)
	ATOM_EVENT_HANDLER(ATOM_Button, ATOM_WidgetRButtonDownEvent, onRButtonDown)
	ATOM_EVENT_HANDLER(ATOM_Button, ATOM_WidgetRButtonUpEvent, onRButtonUp)
	ATOM_EVENT_HANDLER(ATOM_Button, ATOM_WidgetMouseEnterEvent, onMouseEnter)
	ATOM_EVENT_HANDLER(ATOM_Button, ATOM_WidgetMouseLeaveEvent, onMouseLeave)
	ATOM_EVENT_HANDLER(ATOM_Button, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(ATOM_Button, ATOM_WidgetDrawFrameEvent, onPaintFrame)
	ATOM_EVENT_HANDLER(ATOM_Button, ATOM_WidgetIdleEvent, onIdle)
ATOM_END_EVENT_MAP

ATOM_Button::ATOM_Button()
{
	ATOM_STACK_TRACE(ATOM_Button::ATOM_Button);

	_mouseOn = false;

	_mouseEnterAnimator = NULL;
	_mouseLeaveAnimator = NULL;
	_currentAnimator = NULL;

	_clickSound = INVALID_AUDIOID;
	_hoverSound = INVALID_AUDIOID;

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_Button::ATOM_Button (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
:
ATOM_Label (parent, rect, (style & ~ATOM_Widget::TitleBar)|ATOM_Widget::Control, id, showState)
{
	ATOM_STACK_TRACE(ATOM_Button::ATOM_Button);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

	_mouseOn = false;

	_mouseEnterAnimator = NULL;
	_mouseLeaveAnimator = NULL;
	_currentAnimator = NULL;

	_clickSound = INVALID_AUDIOID;
	_hoverSound = INVALID_AUDIOID;

	setBorderMode (ATOM_Widget::Raise);

	resize (rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_Button::~ATOM_Button (void)
{
	ATOM_STACK_TRACE(ATOM_Button::~ATOM_Button);

	if(_mouseEnterAnimator)
	{
		WidgetAnimatorFactory::destroy(_mouseEnterAnimator);
		_mouseEnterAnimator = NULL;
	}
	if(_mouseLeaveAnimator)
	{
		WidgetAnimatorFactory::destroy(_mouseLeaveAnimator);
		_mouseLeaveAnimator = NULL;
	}
}

void ATOM_Button::setClickSound(int sound)
{
	ATOM_STACK_TRACE(ATOM_Button::setClickSound);

	_clickSound = sound;
}

int ATOM_Button::getClickSound() const
{
	ATOM_STACK_TRACE(ATOM_Button::getClickSound);

	return _clickSound;
}

void ATOM_Button::setHoverSound(int sound)
{
	ATOM_STACK_TRACE(ATOM_Button::setHoverSound);

	_hoverSound = sound;
}

int ATOM_Button::getHoverSound() const
{
	ATOM_STACK_TRACE(ATOM_Button::getHoverSound);

	return _hoverSound;
}

void ATOM_Button::setClickInterval(unsigned interval)
{
	ATOM_STACK_TRACE(ATOM_Button::setClickInterval);

	_clickInterval = interval;
}

unsigned ATOM_Button::getClickInterval() 
{
	ATOM_STACK_TRACE(ATOM_Button::getClickInterval);

	return _clickInterval;
}

void ATOM_Button::loadMouseEnterAnimator(ATOM_TiXmlElement* elem)
{
	ATOM_STACK_TRACE(ATOM_Button::loadMouseEnterAnimator);

	if(_mouseEnterAnimator)
	{
		WidgetAnimatorFactory::destroy(_mouseEnterAnimator);
	}

	if(elem)
	{
		_mouseEnterAnimator = WidgetAnimatorFactory::create(this, elem);
	}
}

void ATOM_Button::loadMouseLeaveAnimator(ATOM_TiXmlElement* elem)
{
	ATOM_STACK_TRACE(ATOM_Button::loadMouseLeaveAnimator);

	if(_mouseLeaveAnimator)
	{
		WidgetAnimatorFactory::destroy(_mouseLeaveAnimator);
	}

	if(elem)
	{
		_mouseLeaveAnimator = WidgetAnimatorFactory::create(this, elem);
	}
}

ATOM_WidgetType ATOM_Button::getType (void) const
{
	ATOM_STACK_TRACE(ATOM_Button::getType);

	return WT_BUTTON;
}

void ATOM_Button::onLButtonDown (ATOM_WidgetLButtonDownEvent * /* event */)
{
	setBorderMode (ATOM_Widget::Drop);
	setCapture ();
}

void ATOM_Button::onLButtonDblClick (ATOM_WidgetLButtonDblClickEvent *event)
{
	setBorderMode (ATOM_Widget::Drop);
	setCapture ();
}

void ATOM_Button::onLButtonUp (ATOM_WidgetLButtonUpEvent * event)
{
	ATOM_STACK_TRACE(ATOM_Button::onLButtonUp);

	if (getRenderer()->getCapture() == this)
	{
		setBorderMode (ATOM_Widget::Raise);
		releaseCapture ();

		if (isMouseHover())
		{
			onClicked (event->shiftState);
		}
	}
}

void ATOM_Button::onRButtonDown (ATOM_WidgetRButtonDownEvent * /* event */)
{
	ATOM_STACK_TRACE(ATOM_Button::onRButtonDown);

	setBorderMode (ATOM_Widget::Drop);
	setCapture ();
}

void ATOM_Button::onRButtonUp (ATOM_WidgetRButtonUpEvent * event)
{
	ATOM_STACK_TRACE(ATOM_Button::onRButtonUp);

	if (getRenderer()->getCapture() == this)
	{
		setBorderMode (ATOM_Widget::Raise);
		releaseCapture ();

		if (isMouseHover ())
		{
			onRClicked (event->shiftState);
		}
	}
}

void ATOM_Button::onMouseEnter (ATOM_WidgetMouseEnterEvent *  event )
{
	ATOM_STACK_TRACE(ATOM_Button::onMouseEnter);

	if (getRenderer()->getCapture() == this)
	{
		setBorderMode (ATOM_Widget::Drop);
	}

	if(_mouseEnterAnimator)
	{
		_currentAnimator = _mouseEnterAnimator;
		ATOM_WidgetAnimatorStartEvent startEvent;
		_currentAnimator->handleEvent(&startEvent);
	}
	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->handleEvent(event);
	}
	playSound(_hoverSound);
}

void ATOM_Button::onMouseLeave (ATOM_WidgetMouseLeaveEvent *  event )
{
	ATOM_STACK_TRACE(ATOM_Button::onMouseLeave);

	if (getRenderer()->getCapture() == this)
	{
		setBorderMode (ATOM_Widget::Raise);
	}

	if(_mouseLeaveAnimator)
	{
		_currentAnimator = _mouseLeaveAnimator;
		ATOM_WidgetAnimatorStartEvent startEvent;
		_currentAnimator->handleEvent(&startEvent);
	}
	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->handleEvent(event);
	}
}

void ATOM_Button::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Button::onPaint);

	callParentHandler (event);
}

void ATOM_Button::onClicked (unsigned keymod)
{
	ATOM_STACK_TRACE(ATOM_Button::onClicked);

	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->queueEvent (ATOM_NEW(ATOM_WidgetCommandEvent, getId()), ATOM_APP);
	}
	playSound(_clickSound);
}

void ATOM_Button::onRClicked (unsigned keymod)
{
	ATOM_STACK_TRACE(ATOM_Button::onRClocked);

	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->queueEvent (ATOM_NEW(ATOM_WidgetRClickEvent, getId(), keymod), ATOM_APP);
	}
}


void ATOM_Button::onPaintFrame (ATOM_WidgetDrawFrameEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Button::onPaintFrame);

	drawFrame (event->canvas, getWidgetState ());
}

void ATOM_Button::onIdle(ATOM_WidgetIdleEvent * /* event */)
{
	ATOM_STACK_TRACE(ATOM_Button::onIdle);

	if(_currentAnimator)
	{
		// 更新
		ATOM_WidgetAnimatorUpdateEvent updateEvent;
		_currentAnimator->handleEvent(&updateEvent);

		// 如果处理完毕，则关闭
		if(updateEvent.done)
		{
			ATOM_WidgetAnimatorStopEvent stopEvent;
			_currentAnimator->handleEvent(&stopEvent);
			_currentAnimator = NULL;
		}
	}
}

void ATOM_Button::setLua(const char* pParam)
{
	_strParam = pParam ? pParam : "";
}

const char* ATOM_Button::getLua(void)
{
	return _strParam.c_str();
}



