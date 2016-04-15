#include "StdAfx.h"
#include "slider.h"
#include "gui_canvas.h"

ATOM_BEGIN_EVENT_MAP(ATOM_Slider, ATOM_Widget)
	ATOM_EVENT_HANDLER(ATOM_Slider, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(ATOM_Slider, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	ATOM_EVENT_HANDLER(ATOM_Slider, ATOM_WidgetLButtonUpEvent, onLButtonUp)
	ATOM_EVENT_HANDLER(ATOM_Slider, ATOM_WidgetMouseMoveEvent, onMouseMove)
ATOM_END_EVENT_MAP

ATOM_Slider::ATOM_Slider()
{
	ATOM_STACK_TRACE(ATOM_Slider::ATOM_Slider);

	_clientImageId = ATOM_IMAGEID_SLIDER_HRULER;
	_hHandleImageId = ATOM_IMAGEID_SLIDER_HANDLE;
	_vHandleImageId = ATOM_IMAGEID_SLIDER_HANDLE;
	_hClientImageId = ATOM_IMAGEID_SLIDER_HRULER;
	_vClientImageId = ATOM_IMAGEID_SLIDER_VRULER;
	_rangeMin = 0;
	_rangeMax = 100;
	_rangePos = 0;
	_handleWidth = 6;
	_handleState = WST_NORMAL;
	_placement = WP_HORIZONTAL;
	_text = NULL;
	_showText = false;

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_Slider::ATOM_Slider (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
	:
ATOM_Widget (parent, rect, style|ATOM_Widget::Control, id, showState)
{
	ATOM_STACK_TRACE(ATOM_Slider::ATOM_Slider);

	_clientImageId = ATOM_IMAGEID_SLIDER_HRULER;
	_hHandleImageId = ATOM_IMAGEID_SLIDER_HANDLE;
	_vHandleImageId = ATOM_IMAGEID_SLIDER_HANDLE;
	_hClientImageId = ATOM_IMAGEID_SLIDER_HRULER;
	_vClientImageId = ATOM_IMAGEID_SLIDER_VRULER;
	_rangeMin = 0;
	_rangeMax = 100;
	_rangePos = 0;
	_handleWidth = 6;
	_handleState = WST_NORMAL;
	_placement = WP_HORIZONTAL;
	_text = ATOM_HARDREF(ATOM_Text) ();
	_text->setFont (ATOM_GUIFont::getFontHandle(getFont()));
	_text->setCharMargin (0);
	_text->setZValue (1.f);
	_text->setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
	_showText = false;

	resize (rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_Slider::~ATOM_Slider (void)
{
	ATOM_STACK_TRACE(ATOM_Slider::~ATOM_Slider);
}

ATOM_WidgetType ATOM_Slider::getType (void) const
{
	return WT_SLIDER;
}

void ATOM_Slider::setVerticalHandleImageId (int imageId)
{
	ATOM_STACK_TRACE(ATOM_Slider::setVerticalHandleImageId);

	if (imageId != _vHandleImageId)
	{
		_vHandleImageId = imageId;
		_handle = 0;

		if (_placement == WP_VERTICAL)
		{
			invalidate ();
		}
	}
}

int ATOM_Slider::getVerticalHandleImageId (void) const
{
	ATOM_STACK_TRACE(ATOM_Slider::getVerticalHandleImageId);

	return _vHandleImageId;
}

void ATOM_Slider::setHorizontalHandleImageId (int imageId)
{
	ATOM_STACK_TRACE(ATOM_Slider::setHorizontalHandleImageId);

	if (imageId != _hHandleImageId)
	{
		_hHandleImageId = imageId;
		_handle = 0;

		if (_placement == WP_HORIZONTAL)
		{
			invalidate ();
		}
	}
}

int ATOM_Slider::getHorizontalHandleImageId (void) const
{
	ATOM_STACK_TRACE(ATOM_Slider::getHorizontalHandleImageId);

	return _hHandleImageId;
}

void ATOM_Slider::setVerticalSliderImageId (int imageId)
{
	ATOM_STACK_TRACE(ATOM_Slider::setVerticalSliderImageId);

	if (_vClientImageId != imageId)
	{
		_vClientImageId = imageId;

		if (_placement == WP_VERTICAL)
		{
			setClientImageId (imageId);
		}
	}
}

int ATOM_Slider::getVerticalSliderImageId (void) const
{
	ATOM_STACK_TRACE(ATOM_Slider::getVerticalSliderImageId);

	return _vClientImageId;
}

void ATOM_Slider::setHorizontalSliderImageId (int imageId)
{
	ATOM_STACK_TRACE(ATOM_Slider::setHorizontalSliderImageId);

	if (_hClientImageId != imageId)
	{
		_hClientImageId = imageId;

		if (_placement == WP_HORIZONTAL)
		{
			setClientImageId (imageId);
		}
	}
}

int ATOM_Slider::getHorizontalSliderImageId (void) const
{
	ATOM_STACK_TRACE(ATOM_Slider::getHorizontalSliderImageId);

	return _hClientImageId;
}

void ATOM_Slider::setRange (int rangeMin, int rangeMax)
{
	ATOM_STACK_TRACE(ATOM_Slider::setRange);

	if (rangeMin != _rangeMin || rangeMax != _rangeMax)
	{
		_rangeMin = rangeMin;
		_rangeMax = rangeMax;

		if (_rangePos < _rangeMin)
		{
			_rangePos = _rangeMin;
		}

		if (_rangePos > _rangeMax)
		{
			_rangePos = _rangeMax;
		}

		invalidate ();
	}
}

int ATOM_Slider::getRangeMin (void) const
{
	ATOM_STACK_TRACE(ATOM_Slider::getRangeMin);

	return _rangeMin;
}

int ATOM_Slider::getRangeMax (void) const
{
	ATOM_STACK_TRACE(ATOM_Slider::getRangeMax);

	return _rangeMax;
}

void ATOM_Slider::setPosition (int position, bool postEvent)
{
	ATOM_STACK_TRACE(ATOM_Slider::setPosition);

	int pos = position < _rangeMin ? _rangeMin : (position > _rangeMax ? _rangeMax : position);

	if (pos != _rangePos)
	{
		if(_showText)
		{
			char str[128];
			_itoa(pos, str, 10);
			if (strcmp (_text->getString(), str))
			{
				_text->setString (str);
				_text->realize ();
			}
		}

		if (postEvent && _parent)
		{
			_parent->queueEvent (ATOM_NEW(ATOM_ScrollEvent, _widgetId, pos, _rangePos), ATOM_APP);
		}

		_rangePos = pos;

		invalidate ();
	}
}

int ATOM_Slider::getPosition (void) const
{
	ATOM_STACK_TRACE(ATOM_Slider::getPosition);

	return _rangePos;
}

void ATOM_Slider::setPlacement (ATOM_WidgetPlacement placement)
{
	ATOM_STACK_TRACE(ATOM_Slider::setPlacement);

	if (_placement != placement)
	{
		_placement = placement;
		setClientImageId ((_placement == WP_VERTICAL) ? _vClientImageId : _hClientImageId);
		invalidate ();
	}
}

ATOM_WidgetPlacement ATOM_Slider::getPlacement (void) const
{
	ATOM_STACK_TRACE(ATOM_Slider::getPlacement);

	return _placement;
}

void ATOM_Slider::setHandleWidth (int w)
{
	ATOM_STACK_TRACE(ATOM_Slider::setHandleWidth);

	if (w != _handleWidth)
	{
		_handleWidth = w;
		invalidate ();
	}
}

int ATOM_Slider::getHandleWidth (void) const
{
	ATOM_STACK_TRACE(ATOM_Slider::getHandleWidth);

	return _handleWidth;
}

void ATOM_Slider::updatePosition (int x, int y)
{
	ATOM_STACK_TRACE(ATOM_Slider::updatePosition);

	int pos;
	float t;

	switch (_placement)
	{
	case WP_HORIZONTAL:
		{
			t = float(x - _handleWidth / 2) / float(_clientRect.size.w - _handleWidth);
			break;
		}
	case WP_VERTICAL:
		{
			t = float(y - _handleWidth / 2) / float(_clientRect.size.h - _handleWidth);
			break;
		}
	default:
		{
			return;
		}
	}

	if (t < 0.f) t = 0.f;
	if (t > 1.f) t = 1.f;
	pos = _rangeMin + (_rangeMax - _rangeMin) * t;

	if (pos != _rangePos)
	{
		if(_showText)
		{
			char str[128];
			_itoa(pos, str, 10);
			if (strcmp (_text->getString(), str))
			{
				_text->setString (str);
				_text->realize ();
			}
		}


		if (_parent)
		{
			_parent->queueEvent (ATOM_NEW(ATOM_ScrollEvent, _widgetId, pos, _rangePos), ATOM_APP);
		}
		_rangePos = pos;
		invalidate ();
	}


}

void ATOM_Slider::onLButtonDown (ATOM_WidgetLButtonDownEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Slider::onLButtonDown);

	setCapture ();


	updatePosition (event->x, event->y);
}

void ATOM_Slider::onLButtonUp (ATOM_WidgetLButtonUpEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Slider::onLButtonUp);

	releaseCapture ();
}

void ATOM_Slider::onMouseMove (ATOM_WidgetMouseMoveEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Slider::onMouseMove);

	if (isCaptured ())
	{
		updatePosition (event->x, event->y);
	}
}

void ATOM_Slider::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Slider::onPaint);

	callParentHandler (event);

	if (_rangeMax <= _rangeMin)
	{
		return;
	}

	const ATOM_GUIImageList *imagelist = getValidImageList ();
	switch (_placement)
	{
	case WP_VERTICAL:
		{
			if(!_handle)
				_handle = imagelist->getImage (_vHandleImageId);
			if (!_handle)
			{
				_handle = ATOM_GUIImageList::getDefaultImageList().getImage (ATOM_IMAGEID_SLIDER_HANDLE);
				ATOM_ASSERT(_handle);
			}
			int pos = (_clientRect.size.h - _handleWidth) * _rangePos / (_rangeMax - _rangeMin);
			_handle->draw (event->state, event->canvas, ATOM_Rect2Di(0, pos, event->canvas->getSize().w, _handleWidth));
			break;
		}
	case WP_HORIZONTAL:
		{
			if(!_handle)
				_handle = imagelist->getImage (_hHandleImageId);
			if (!_handle)
			{
				_handle = ATOM_GUIImageList::getDefaultImageList().getImage (ATOM_IMAGEID_SLIDER_HANDLE);
				ATOM_ASSERT(_handle);
			}

			int pos = (_clientRect.size.w - _handleWidth) * (_rangePos - _rangeMin) / (_rangeMax - _rangeMin);
			_handle->draw (event->state, event->canvas, ATOM_Rect2Di(pos, 0, _handleWidth, event->canvas->getSize().h));
			break;
		}
	}

	// »æÖÆÎÄ×Ö
	if(_showText)
	{
		const char *text = _text->getString ();
		if (strcmp (text, ""))
		{
			ATOM_FontHandle font = ATOM_GUIFont::getFontHandle (getFont());
			int charmargin = ATOM_GUIFont::getFontCharMargin (getFont());
			ATOM_SetCharMargin (charmargin);
			int l, t, w, h;
			ATOM_CalcStringBounds (font, text, strlen(text), &l, &t, &w, &h);
			int x = (_clientRect.size.w - w) / 2;
			int y = (_clientRect.size.h - h) / 2 - t;
			event->canvas->drawText (_text.get(), x, y, getFontColor());
		}
	}
}

void ATOM_Slider::showText(bool b)
{
	ATOM_STACK_TRACE(ATOM_Slider::showText);

	if(_showText == b)
	{
		return;
	}

	_showText = b;
	if(_showText)
	{
		char str[128];
		_itoa(_rangePos, str, 10);
		if (strcmp (_text->getString(), str))
		{
			_text->setString (str);
			_text->realize ();
		}
		invalidate ();
	}
}

void ATOM_Slider::setFont (ATOM_GUIFont::handle font)
{
	ATOM_Widget::setFont (font);

	_text->setFont (ATOM_GUIFont::getFontHandle (getFont()));
}

void ATOM_Slider::setHandleImage( ATOM_GUIImage * image )
{
	_handle = image;
}

