#include "stdafx.h"
#include "checkbox.h"
#include "gui_canvas.h"

ATOM_BEGIN_EVENT_MAP(ATOM_Checkbox, ATOM_Label)
	ATOM_EVENT_HANDLER(ATOM_Checkbox, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	ATOM_EVENT_HANDLER(ATOM_Checkbox, ATOM_WidgetLButtonUpEvent, onLButtonUp)
	ATOM_EVENT_HANDLER(ATOM_Checkbox, ATOM_WidgetMouseEnterEvent, onMouseEnter)
	ATOM_EVENT_HANDLER(ATOM_Checkbox, ATOM_WidgetMouseLeaveEvent, onMouseLeave)
	ATOM_EVENT_HANDLER(ATOM_Checkbox, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(ATOM_Checkbox, ATOM_WidgetDrawFrameEvent, onPaintFrame)
ATOM_END_EVENT_MAP

ATOM_Checkbox::ATOM_Checkbox()
{
	ATOM_STACK_TRACE(ATOM_Checkbox::ATOM_Checkbox);

	_mouseOn = false;

	_checkedImageId = ATOM_IMAGEID_CHECKBOX_CHECKED;
	_clientImageId = ATOM_IMAGEID_CHECKBOX_UNCHECKED;
	_checked = false;

	_clickSound = INVALID_AUDIOID;
	_hoverSound = INVALID_AUDIOID;

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_Checkbox::ATOM_Checkbox (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
:
ATOM_Label (parent, rect, (style & ~ATOM_Widget::TitleBar)|ATOM_Widget::Control, id, showState)
{
	ATOM_STACK_TRACE(ATOM_Checkbox::ATOM_Checkbox);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

	_mouseOn = false;

	_checkedImageId = ATOM_IMAGEID_CHECKBOX_CHECKED;
	_clientImageId = ATOM_IMAGEID_CHECKBOX_UNCHECKED;
	_checked = false;

	_clickSound = INVALID_AUDIOID;
	_hoverSound = INVALID_AUDIOID;

	setBorderMode (ATOM_Widget::Raise);

	resize (rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_Checkbox::~ATOM_Checkbox (void)
{
	ATOM_STACK_TRACE(ATOM_Checkbox::~ATOM_Checkbox);
}

void ATOM_Checkbox::setCheckedImageId(int imageId)
{
	ATOM_STACK_TRACE(ATOM_Checkbox::setCheckedImageId);

	_checkedImageId = imageId;
}

int ATOM_Checkbox::getCheckedImageId() const
{
	ATOM_STACK_TRACE(ATOM_Checkbox::getCheckedImageId);

	return _checkedImageId;
}

void ATOM_Checkbox::setChecked(bool checked)
{
	ATOM_STACK_TRACE(ATOM_Checkbox::setChecked);

	_checked = checked;
}

bool ATOM_Checkbox::getChecked (void) const
{
	ATOM_STACK_TRACE(ATOM_Checkbox::getChecked);

	return _checked;
}

void ATOM_Checkbox::setClickSound(int sound)
{
	ATOM_STACK_TRACE(ATOM_Checkbox::setClickSound);

	_clickSound = sound;
}

int ATOM_Checkbox::getClickSound() const
{
	ATOM_STACK_TRACE(ATOM_Checkbox::getClickSound);

	return _clickSound;
}

void ATOM_Checkbox::setHoverSound(int sound)
{
	ATOM_STACK_TRACE(ATOM_Checkbox::setHoverSound);

	_hoverSound = sound;
}

int ATOM_Checkbox::getHoverSound() const
{
	ATOM_STACK_TRACE(ATOM_Checkbox::getHoverSound);

	return _hoverSound;
}

ATOM_WidgetType ATOM_Checkbox::getType (void) const
{
	ATOM_STACK_TRACE(ATOM_Checkbox::getType);

	return WT_CHECKBOX;
}

void ATOM_Checkbox::onLButtonDown (ATOM_WidgetLButtonDownEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Checkbox::onLButtonDown);

	setBorderMode (ATOM_Widget::Drop);
	setCapture ();
}

void ATOM_Checkbox::onLButtonUp (ATOM_WidgetLButtonUpEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Checkbox::onLButtonUp);

	if (getRenderer()->getCapture() == this)
	{
		setBorderMode (ATOM_Widget::Raise);
		releaseCapture ();

		if (isMouseHover ())
		{
			onClicked ();
			_checked = !_checked;
		}
	}
}

void ATOM_Checkbox::onMouseEnter (ATOM_WidgetMouseEnterEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Checkbox::onMouseEnter);

	if (getRenderer()->getCapture() == this)
	{
		setBorderMode (ATOM_Widget::Drop);
	}
	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->handleEvent(event);
	}
	playSound(_hoverSound);
}

void ATOM_Checkbox::onMouseLeave (ATOM_WidgetMouseLeaveEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Checkbox::onMouseLeave);

	if (getRenderer()->getCapture() == this)
	{
		setBorderMode (ATOM_Widget::Raise);
	}
	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->handleEvent(event);
	}
}

void ATOM_Checkbox::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Checkbox::onPaint);
	int ImageId = _checked?_checkedImageId:_clientImageId;

	if(ATOM_INVALID_IMAGEID != ImageId)
	{
#if 1
		ATOM_GUIImage *image = getValidImage (ImageId);
#else
		const ATOM_GUIImageList *imagelist = getValidImageList ();
		ATOM_GUIImage *image = imagelist->getImage (_checked ? _checkedImageId : _clientImageId);
#endif
		if (!image)
		{
			image = ATOM_GUIImageList::getDefaultImageList().getImage (_checked ? ATOM_IMAGEID_CHECKBOX_CHECKED : ATOM_IMAGEID_CHECKBOX_UNCHECKED);
			ATOM_ASSERT(image);
		}

		if(_customColor)
		{
			image->draw (event->state, event->canvas, ATOM_Rect2Di(ATOM_Point2Di(0, 0), event->canvas->getSize()), &_customColor);
		}
		else
		{
			image->draw (event->state, event->canvas, ATOM_Rect2Di(ATOM_Point2Di(0, 0), event->canvas->getSize()));
		}
	}


	if (!_caption.empty())
	{
		if (_textDirty)
		{
			calcTextLayout ();
			_textDirty = false;
		}

		//event->canvas->drawText (_text.get(), _textPosition.x, _textPosition.y, _fontColor);
		if(_widgetFontOutLineColor.getByteA())
		{
			event->canvas->drawTextOutline (_text.get(), _textPosition.x, _textPosition.y, getFontColor(), _widgetFontOutLineColor);
		}
		else
		{
			event->canvas->drawText (_text.get(), _textPosition.x, _textPosition.y, getFontColor());
		}	
	}
}

void ATOM_Checkbox::onClicked (void)
{
	ATOM_STACK_TRACE(ATOM_Checkbox::onClicked);

	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->queueEvent (ATOM_NEW(ATOM_WidgetCommandEvent, getId()), ATOM_APP);
	}
	playSound(_clickSound);
}

void ATOM_Checkbox::onPaintFrame (ATOM_WidgetDrawFrameEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Checkbox::onPaintFrame);

	drawFrame (event->canvas, getWidgetState ());
}



