#include "stdafx.h"
#include "gui_canvas.h"

ATOM_BEGIN_EVENT_MAP(ATOM_ScrollBar, ATOM_Widget)
	ATOM_EVENT_HANDLER(ATOM_ScrollBar, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(ATOM_ScrollBar, ATOM_WidgetResizeEvent, onResize)
	ATOM_EVENT_HANDLER(ATOM_ScrollBar, ATOM_ScrollEvent, onScroll)
	ATOM_EVENT_HANDLER(ATOM_ScrollBar, ATOM_WidgetCommandEvent, onCommand)
	ATOM_EVENT_HANDLER(ATOM_ScrollBar, ATOM_WidgetMouseWheelEvent, onMouseWheel)
ATOM_END_EVENT_MAP

ATOM_ScrollBar::ATOM_ScrollBar()
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::ATOM_ScrollBar);

	_slider = NULL;
	_buttonWidth = 10;

	_hUpButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_vUpButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_hDownButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_vDownButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_hSliderImageId = ATOM_AUTOMATIC_IMAGEID;
	_vSliderImageId = ATOM_AUTOMATIC_IMAGEID;
	_hSliderHandleImageId = ATOM_AUTOMATIC_IMAGEID;
	_vSliderHandleImageId = ATOM_AUTOMATIC_IMAGEID;

	_upButton = NULL;
	_downButton = NULL;

	_rangeStep = 1.f;

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_ScrollBar::ATOM_ScrollBar (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
:
ATOM_Widget (parent, rect, style|ATOM_Widget::Control, id, showState)
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::ATOM_ScrollBar);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

	_hUpButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_vUpButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_hDownButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_vDownButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_hSliderImageId = ATOM_AUTOMATIC_IMAGEID;
	_vSliderImageId = ATOM_AUTOMATIC_IMAGEID;
	_hSliderHandleImageId = ATOM_AUTOMATIC_IMAGEID;
	_vSliderHandleImageId = ATOM_AUTOMATIC_IMAGEID;

	_slider = ATOM_NEW(ATOM_Slider, this, ATOM_Rect2Di(0, 0, 10, 10), int(ATOM_Widget::NoFocus|ATOM_Widget::NonLayoutable|ATOM_Widget::Control), int(ID_SLIDER), ATOM_Widget::ShowNormal);
	_slider->setPlacement (WP_HORIZONTAL);
	_slider->setWheelDeliver(true);
	_buttonWidth = 10;

	_upButton = ATOM_NEW(ATOM_Button, this, ATOM_Rect2Di(0, 0, 10, 10), int(ATOM_Widget::NoFocus|ATOM_Widget::NonLayoutable|ATOM_Widget::Control|ATOM_Widget::NoFocus), int(ID_BUTTONUP), ATOM_Widget::ShowNormal);
	_downButton = ATOM_NEW(ATOM_Button, this, ATOM_Rect2Di(0, 0, 10, 10), int(ATOM_Widget::NoFocus|ATOM_Widget::NonLayoutable|ATOM_Widget::Control|ATOM_Widget::NoFocus), int(ID_BUTTONDOWN), ATOM_Widget::ShowNormal);
	_rangeStep = 1.f;
	_enableWheelDeliver = true;
	resize (rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_ScrollBar::~ATOM_ScrollBar (void)
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::~ATOM_ScrollBar);
}

void ATOM_ScrollBar::setVerticalImageId(int backImageId, int sliderImageId, int upButtonImageId, int downButtonImageId)
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::setVerticalImageId);

	_vUpButtonImageId = upButtonImageId;
	_vDownButtonImageId = downButtonImageId;
	_vSliderImageId = backImageId;
	_vSliderHandleImageId = sliderImageId;
}

void ATOM_ScrollBar::setHorizontalImageId(int backImageId, int sliderImageId, int upButtonImageId, int downButtonImageId)
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::setHorizontalImageId);

	_hUpButtonImageId = upButtonImageId;
	_hDownButtonImageId = downButtonImageId;
	_hSliderImageId = backImageId;
	_hSliderHandleImageId = sliderImageId;
}

ATOM_WidgetType ATOM_ScrollBar::getType (void) const
{
	return WT_SCROLLBAR;
}

void ATOM_ScrollBar::setPlacement (ATOM_WidgetPlacement placement)
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::setPlacement);

	if (_slider->getPlacement() != placement)
	{
		_slider->setPlacement (placement);
		resize (_widgetRect);
	}
}

ATOM_WidgetPlacement ATOM_ScrollBar::getPlacement (void) const
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::getPlacement);

	return _slider->getPlacement ();
}

void ATOM_ScrollBar::setButtonWidth (int width)
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::setButtonWidth);

	if (width != _buttonWidth)
	{
		_buttonWidth = width;

		resize (getWidgetRect ());
	}
}

int ATOM_ScrollBar::getButtonWidth (void) const
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::getButtonWidth);

	return _buttonWidth;
}

void ATOM_ScrollBar::setRange (float rangeMin, float rangeMax)
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::setRange);

	_slider->setRange (rangeMin, rangeMax);
}

float ATOM_ScrollBar::getRangeMin (void) const
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::getRangeMin);

	return _slider->getRangeMin ();
}

float ATOM_ScrollBar::getRangeMax (void) const
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::getRangeMax);

	return _slider->getRangeMax ();
}

void ATOM_ScrollBar::setStep (float step)
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::setStep);

	_rangeStep = step;
}

float ATOM_ScrollBar::getStep (void) const
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::getStep);

	return _rangeStep;
}

void ATOM_ScrollBar::setPosition (float position, bool postEvent)
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::setPosition);

	_slider->setPosition (position, postEvent);
}

float ATOM_ScrollBar::getPosition (void) const
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::getPosition);

	return _slider->getPosition ();
}

void ATOM_ScrollBar::setHandleWidth (int width)
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::setHandleWidth);

	_slider->setHandleWidth (width);
}

int ATOM_ScrollBar::getHandleWidth (void) const
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::getHandleWidth);

	return _slider->getHandleWidth ();
}

void ATOM_ScrollBar::onResize (ATOM_WidgetResizeEvent *event)
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::onResize);

	callParentHandler (event);

	switch (getPlacement ())
	{
	case WP_VERTICAL:
		{
			_upButton->resize (ATOM_Rect2Di(0, 0, _clientRect.size.w, _buttonWidth));
			_downButton->resize (ATOM_Rect2Di(0, _clientRect.size.h - _buttonWidth, _clientRect.size.w, _buttonWidth));
			_slider->resize (ATOM_Rect2Di(0, _buttonWidth, _clientRect.size.w, _clientRect.size.h - 2 * _buttonWidth));
			break;
		}
	case WP_HORIZONTAL:
		{
			_upButton->resize (ATOM_Rect2Di(0, 0, _buttonWidth, _clientRect.size.h));
			_downButton->resize (ATOM_Rect2Di(_clientRect.size.w - _buttonWidth, 0, _buttonWidth, _clientRect.size.h));
			_slider->resize (ATOM_Rect2Di(_buttonWidth, 0, _clientRect.size.w - 2 * _buttonWidth, _clientRect.size.h));
			break;
		}
	}
}

void ATOM_ScrollBar::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	if (getPlacement() == WP_HORIZONTAL)
	{
		if(!_upButton->isUserExClientImage())
			_upButton->setClientImageId (_hUpButtonImageId == ATOM_AUTOMATIC_IMAGEID ? ATOM_IMAGEID_SCROLLBAR_HBUTTONUP : _hUpButtonImageId);
		_downButton->setClientImageId (_hDownButtonImageId == ATOM_AUTOMATIC_IMAGEID ? ATOM_IMAGEID_SCROLLBAR_HBUTTONDOWN : _hDownButtonImageId);
		_slider->setHorizontalSliderImageId (_hSliderImageId == ATOM_AUTOMATIC_IMAGEID ? ATOM_IMAGEID_SCROLLBAR_HSLIDER : _hSliderImageId);
		_slider->setHorizontalHandleImageId (_hSliderHandleImageId == ATOM_AUTOMATIC_IMAGEID ? ATOM_IMAGEID_SLIDER_HANDLE : _hSliderHandleImageId);
	}
	else
	{
		_upButton->setClientImageId (_vUpButtonImageId == ATOM_AUTOMATIC_IMAGEID ? ATOM_IMAGEID_SCROLLBAR_VBUTTONUP : _vUpButtonImageId);
		_downButton->setClientImageId (_vDownButtonImageId == ATOM_AUTOMATIC_IMAGEID ? ATOM_IMAGEID_SCROLLBAR_VBUTTONDOWN : _vDownButtonImageId);
		_slider->setVerticalSliderImageId (_vSliderImageId == ATOM_AUTOMATIC_IMAGEID ? ATOM_IMAGEID_SCROLLBAR_VSLIDER : _vSliderImageId);
		_slider->setVerticalHandleImageId (_vSliderHandleImageId == ATOM_AUTOMATIC_IMAGEID ? ATOM_IMAGEID_SLIDER_HANDLE : _vSliderHandleImageId);
	}
}

void ATOM_ScrollBar::onScroll (ATOM_ScrollEvent *event)
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::onScroll);

	if (_parent)
	{
		_parent->queueEvent (ATOM_NEW(ATOM_ScrollEvent, _widgetId, event->newPos, event->oldPos), ATOM_APP);
	}
}

void ATOM_ScrollBar::onCommand (ATOM_WidgetCommandEvent *event)
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::onCommand);

	if (_parent)
	{
		switch (event->id)
		{
		case ID_BUTTONUP:
			_slider->setPosition (_slider->getPosition() - _rangeStep);
			break;
		case ID_BUTTONDOWN:
			_slider->setPosition (_slider->getPosition() + _rangeStep);
			break;
		}
	}
}

ATOM_Slider* ATOM_ScrollBar::getSlider()
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::getSlider);

	return _slider;
}

ATOM_Button* ATOM_ScrollBar::getUpButton()
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::getUpButton);

	return _upButton;
}

ATOM_Button* ATOM_ScrollBar::getDownButton()
{
	ATOM_STACK_TRACE(ATOM_ScrollBar::getDownButton);

	return _downButton;
}

void ATOM_ScrollBar::onMouseWheel( ATOM_WidgetMouseWheelEvent *event )
{
	if(_enableWheelDeliver && _parent)
	{
		_parent->queueEvent (ATOM_NEW(ATOM_WidgetMouseWheelEvent, _widgetId, event->x,event->y, event->delta,event->shiftState), ATOM_APP);
	}

}


