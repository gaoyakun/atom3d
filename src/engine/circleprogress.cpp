#include "stdafx.h"
#include "circleprogress.h"
#include "gui_canvas.h"
#include "polygondrawer.h"

ATOM_BEGIN_EVENT_MAP(ATOM_CircleProgress, ATOM_Widget)
	ATOM_EVENT_HANDLER(ATOM_CircleProgress, ATOM_WidgetDrawClientEvent, onPaint)
ATOM_END_EVENT_MAP

ATOM_CircleProgress::ATOM_CircleProgress()
{
	ATOM_STACK_TRACE(ATOM_CircleProgress::ATOM_CircleProgress);

	_frontImageId = ATOM_IMAGEID_PROGRESSBAR;
	_invert = false;

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_CircleProgress::ATOM_CircleProgress (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
:
ATOM_Widget (parent, rect, style|ATOM_Widget::Control, id, showState)
{
	ATOM_STACK_TRACE(ATOM_CircleProgress::ATOM_CircleProgress);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

	_frontImageId = ATOM_IMAGEID_PROGRESSBAR;
	setMax(100);
	setCurrent(0);
	_invert = false;

	resize(rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_CircleProgress::~ATOM_CircleProgress (void)
{
	ATOM_STACK_TRACE(ATOM_CircleProgress::~ATOM_CircleProgress);
}

void ATOM_CircleProgress::setFrontImageId (int id)
{
	ATOM_STACK_TRACE(ATOM_CircleProgress::setFrontImageId);

	if (id != _frontImageId)
	{
		_frontImageId = id;
		invalidate ();
	}
}

int ATOM_CircleProgress::getFrontImageId() const
{
	ATOM_STACK_TRACE(ATOM_CircleProgress::getFrontImageId);

	return _frontImageId;
}

ATOM_WidgetType ATOM_CircleProgress::getType (void) const
{
	ATOM_STACK_TRACE(ATOM_CircleProgress::getType);

	return WT_CIRCLEPROGRESS;
}

void ATOM_CircleProgress::setCurrent(int val)
{
	ATOM_STACK_TRACE(ATOM_CircleProgress::setCurrent);

	if(val < 0)
	{
		val = 0;
	}
	else if(val > _maxValue)
	{
		val = _maxValue;
	}

	if(_currentValue == val)
	{
		return;
	}
	_currentValue = val;

	if (getShowState () != ATOM_Widget::Hide)
	{
		invalidate ();
	}
}

void ATOM_CircleProgress::setMax(int maxValue)
{
	ATOM_STACK_TRACE(ATOM_CircleProgress::setMax);

	if(maxValue < 0)
	{
		maxValue = 0;
	}
	_maxValue = maxValue;

	if (getShowState () != ATOM_Widget::Hide)
	{
		invalidate ();
	}
}

int ATOM_CircleProgress::getCurrent() const
{
	ATOM_STACK_TRACE(ATOM_CircleProgress::getCurrent);

	return _currentValue;
}

int ATOM_CircleProgress::getMax() const
{
	ATOM_STACK_TRACE(ATOM_CircleProgress::getMax);

	return _maxValue;
}

void ATOM_CircleProgress::setInvert(bool invert)
{
	ATOM_STACK_TRACE(ATOM_CircleProgress::setInvert);

	_invert = invert;
}

bool ATOM_CircleProgress::isInvert() const
{
	ATOM_STACK_TRACE(ATOM_CircleProgress::isInvert);

	return _invert;
}

void ATOM_CircleProgress::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_CircleProgress::onPaint);

	ATOM_GUICanvas* canvas = event->canvas;
	ATOM_WidgetState state = event->state;

	callParentHandler (event);

	// get client rect
	ATOM_Rect2Di rc;
	rc.point.x = 0;
	rc.point.y = 0;
	rc.size = canvas->getSize();

	// »æÖÆÇ°¾°
	if (_frontImageId != ATOM_INVALID_IMAGEID && _currentValue > 0)
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

		if(_currentValue >= _maxValue)
		{
			image->draw (event->state, event->canvas, ATOM_Rect2Di(ATOM_Point2Di(0, 0), event->canvas->getSize()));
		}
		else
		{
			ATOM_Texture *texture = image->getTexture (WST_NORMAL);
			ATOM_CircleDrawer drawer(event->canvas, texture);
			if(_invert)
			{
				drawer.InvertDrawAdd(rc, float(_currentValue)/float(_maxValue));
			}
			else
			{
				drawer.DrawAdd(rc, float(_currentValue)/float(_maxValue));
			}
		}
	}
}




