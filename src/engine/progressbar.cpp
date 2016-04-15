#include "stdafx.h"
#include "progressbar.h"
#include "gui_canvas.h"

ATOM_BEGIN_EVENT_MAP(ATOM_ProgressBar, ATOM_Widget)
ATOM_EVENT_HANDLER(ATOM_ProgressBar, ATOM_WidgetDrawClientEvent, onPaint)
ATOM_END_EVENT_MAP

ATOM_ProgressBar::ATOM_ProgressBar()
{
	ATOM_STACK_TRACE(ATOM_ProgressBar::ATOM_ProgressBar);

	_frontImageId = ATOM_IMAGEID_PROGRESSBAR;
	_drawText = true;

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_ProgressBar::ATOM_ProgressBar (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState,bool isHorizontal)
:
ATOM_Label(parent, rect, style|ATOM_Widget::Control, id, showState)
{
	ATOM_STACK_TRACE(ATOM_ProgressBar::ATOM_ProgressBar);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

	_frontImageId = ATOM_IMAGEID_PROGRESSBAR;
	setMinMax(0, 100);
	setCurrent(0);

	setBorderMode (ATOM_Widget::Raise);

	resize (rect);
	_isHorizontal = isHorizontal;
	_drawText = true;

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_ProgressBar::~ATOM_ProgressBar (void)
{
	ATOM_STACK_TRACE(ATOM_ProgressBar::~ATOM_ProgressBar);
}

ATOM_WidgetType ATOM_ProgressBar::getType (void) const
{
	ATOM_STACK_TRACE(ATOM_ProgressBar::getType);

	return WT_PROGRESSBAR;
}

void ATOM_ProgressBar::setFrontImageId (int id)
{
	ATOM_STACK_TRACE(ATOM_ProgressBar::setFrontImageId);

	if (id != _frontImageId)
	{
		_frontImageId = id;
		invalidate ();
	}
}

int ATOM_ProgressBar::getFrontImageId() const
{
	ATOM_STACK_TRACE(ATOM_ProgressBar::getFrontImageId);

	return _frontImageId;
}

void ATOM_ProgressBar::setCurrent(int val, bool resetText)
{
	ATOM_STACK_TRACE(ATOM_ProgressBar::setCurrent);

	if(val < _minValue)
	{
		val = _minValue;
	}
	else if(val > _maxValue)
	{
		val = _maxValue;
	}

	if(!resetText && _currentValue == val)
	{
		return;
	}
	_currentValue = val;

	// FIXME:这里应该去掉
	if(resetText)
	{
		char buf[64];
		sprintf(buf, "%d/%d", _currentValue, _maxValue);
		setText(buf);
	}
}

void ATOM_ProgressBar::setMinMax(int minValue, int maxValue)
{
	ATOM_STACK_TRACE(ATOM_ProgressBar::setMinMax);

	if(minValue >= maxValue)
	{
		maxValue = minValue+1;
	}
	_minValue = minValue;
	_maxValue = maxValue;

	if (getShowState () != ATOM_Widget::Hide)
	{
		invalidate ();
	}
}

int ATOM_ProgressBar::getCurrent() const
{
	ATOM_STACK_TRACE(ATOM_ProgressBar::getCurrent);

	return _currentValue;
}

int ATOM_ProgressBar::getMin() const
{
	ATOM_STACK_TRACE(ATOM_ProgressBar::getMin);

	return _minValue;
}

int ATOM_ProgressBar::getMax() const
{
	ATOM_STACK_TRACE(ATOM_ProgressBar::getMax);

	return _maxValue;
}


void ATOM_ProgressBar::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_ProgressBar::onPaint);

	ATOM_GUICanvas* canvas = event->canvas;
	ATOM_WidgetState state = event->state;

	if (_clientImageId != ATOM_INVALID_IMAGEID)
	{
#if 1
		ATOM_GUIImage *image = getValidImage (_clientImageId);
#else
		const ATOM_GUIImageList *imagelist = getValidImageList ();
		ATOM_GUIImage *image = imagelist->getImage (_clientImageId);
#endif
		if (!image)
		{
			image = ATOM_GUIImageList::getDefaultImageList().getImage (ATOM_IMAGEID_WINDOW_BKGROUND);
			ATOM_ASSERT(image);
		}
		if(_customColor)
		{
			image->draw (event->state, event->canvas, ATOM_Rect2Di(ATOM_Point2Di(0, 0), event->canvas->getSize()), &_customColor, _rotation);
		}
		else
		{
			image->draw (event->state, event->canvas, ATOM_Rect2Di(ATOM_Point2Di(0, 0), event->canvas->getSize()), NULL, _rotation);
		}
	}

	// get client rect
	ATOM_Rect2Di rc;
	rc.point.x = 0;
	rc.point.y = 0;
	rc.size = canvas->getSize();
	// 绘制前景
	if(getCurrent() > getMin())
	{
#if 1
		ATOM_GUIImage *image = getValidImage (_frontImageId);
#else
		const ATOM_GUIImageList *imagelist = getValidImageList ();
		ATOM_GUIImage *image = imagelist->getImage (_frontImageId);
#endif
		if (!image)
		{
			image = ATOM_GUIImageList::getDefaultImageList().getImage (ATOM_IMAGEID_PROGRESSBAR);
			ATOM_ASSERT(image);
		}

		float f = (float)(getCurrent() - getMin());
		f /= (getMax() - getMin());
		ATOM_Rect2Di rcDst = rc;
		ATOM_Rect2Di rcSrc;
		if(_isHorizontal)
		{
			rcDst.size.w = f*rcDst.size.w;
			rcSrc = image->getRegion(event->state);
			if(0 == rcSrc.size.w || 0 == rcSrc.size.h)
			{
				rcSrc.point.x = rcSrc.point.y = 0;
				rcSrc.size.w = image->getTextureWidth (event->state);
				rcSrc.size.h = image->getTextureHeight (event->state);
			}
			rcSrc.size.w = f*rcSrc.size.w;
		}
		else
		{
			rcDst.size.h = f*rcDst.size.h;
			rcDst.point.y = rc.size.h-rcDst.size.h;
			rcSrc = image->getRegion(event->state);
			if(0 == rcSrc.size.w || 0 == rcSrc.size.h)
			{
				rcSrc.point.x = rcSrc.point.y = 0;
				rcSrc.size.w = image->getTextureWidth (event->state);
				rcSrc.size.h = image->getTextureHeight (event->state);

			}
			
			rcSrc.size.h = f*rcSrc.size.h;
			rcSrc.point.y = rcDst.point.y;
		}
		image->drawRect (event->state, event->canvas, rcDst, rcSrc);
	}

	// 绘制文字
	if (_drawText && !_caption.empty())
	{
		if (_textDirty)
		{
			calcTextLayout ();
			_textDirty = false;
		}

		event->canvas->drawText (_text.get(), _textPosition.x, _textPosition.y, getFontColor());
	}
}

void ATOM_ProgressBar::enableDrawText (bool draw)
{
	_drawText = draw;
}

bool ATOM_ProgressBar::isDrawTextEnabled (void) const
{
	return _drawText;
}

void ATOM_ProgressBar::setIsHorizontal( bool isHorizontal )
{
	_isHorizontal = isHorizontal;
}




