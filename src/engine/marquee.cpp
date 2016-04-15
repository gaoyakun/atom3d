#include "stdafx.h"
#include "marquee.h"
#include "gui_canvas.h"

ATOM_BEGIN_EVENT_MAP(ATOM_Marquee, ATOM_Widget)
	ATOM_EVENT_HANDLER(ATOM_Marquee, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	ATOM_EVENT_HANDLER(ATOM_Marquee, ATOM_WidgetLButtonUpEvent, onLButtonUp)
	ATOM_EVENT_HANDLER(ATOM_Marquee, ATOM_WidgetMouseEnterEvent, onMouseEnter)
	ATOM_EVENT_HANDLER(ATOM_Marquee, ATOM_WidgetMouseLeaveEvent, onMouseLeave)
	ATOM_EVENT_HANDLER(ATOM_Marquee, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(ATOM_Marquee, ATOM_WidgetDrawFrameEvent, onPaintFrame)
	ATOM_EVENT_HANDLER(ATOM_Marquee, ATOM_WidgetIdleEvent, onIdle)
	ATOM_EVENT_HANDLER(ATOM_Marquee, ATOM_WidgetResizeEvent, onResize)
ATOM_END_EVENT_MAP

ATOM_Marquee::ATOM_Marquee()
{
	ATOM_STACK_TRACE(ATOM_Marquee::ATOM_Marquee);

	_text = NULL;
	_mouseOn = false;
	_textDirty = false;
	_textPosition = 0;
	_textWidth = 0;
	_y = 0;
	_speed = 0.f;
	_dir = MARQUEEDIR_LEFT;
	_offset = 0;

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_Marquee::ATOM_Marquee (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
:
ATOM_Widget (parent, rect, (style & ~ATOM_Widget::TitleBar)|ATOM_Widget::Control|ATOM_Widget::ClipChildren, id, showState)
{
	ATOM_STACK_TRACE(ATOM_Marquee::ATOM_Marquee);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

	_text = ATOM_HARDREF(ATOM_Text) ();
	_text->setFont (ATOM_GUIFont::getFontHandle(getFont()));
	_text->setCharMargin (ATOM_GUIFont::getFontCharMargin(getFont()));
	_text->setZValue (1.f);
	_text->setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
	_mouseOn = false;
	_textDirty = false;
	_textPosition = 0;
	_textWidth = 0;
	_y = 0;
	_speed = 0.f;
	_dir = MARQUEEDIR_LEFT;
	_offset = 0;

	setBorderMode (ATOM_Widget::Raise);

	resize (rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_Marquee::~ATOM_Marquee (void)
{
	ATOM_STACK_TRACE(ATOM_Marquee::~ATOM_Marquee);
}

void ATOM_Marquee::calcTextLayout (void)
{
	ATOM_STACK_TRACE(ATOM_Marquee::calcTextLayout);

	ATOM_FontHandle font = ATOM_GUIFont::getFontHandle (getFont());
	int charmargin = ATOM_GUIFont::getFontCharMargin (getFont());
	ATOM_SetCharMargin (charmargin);
	int l, t, w, h;
	_text->setString (_caption.c_str());
	ATOM_CalcStringBounds(font, _caption.c_str(), _caption.size(), &l, &t, &w, &h);
	//unsigned clip = ATOM_ClipString (font, charset, _caption.c_str(), _clientRect.w, &l, &t, &w, &h);
	//_textPosition.x = 0;
	_y = (_clientRect.size.h - h) / 2 - t;
	_textWidth = w;
	//_text->setString (_caption.substr(0, clip).c_str(), charset);
}

// 设置滚动方向
void ATOM_Marquee::setDir(MARQUEEDIR dir)
{
	ATOM_STACK_TRACE(ATOM_Marquee::setDir);

	_dir = dir;
}

// 取得滚动方向
MARQUEEDIR ATOM_Marquee::getDir() const
{
	ATOM_STACK_TRACE(ATOM_Marquee::getDir);

	return _dir;
}

// 设置速度
void ATOM_Marquee::setSpeed(float speed)
{
	ATOM_STACK_TRACE(ATOM_Marquee::setSpeed);

	_speed = speed;
}

// 取得速度
float ATOM_Marquee::getSpeed() const
{
	ATOM_STACK_TRACE(ATOM_Marquee::getSpeed);

	return _speed;
}

// 设置位置
void ATOM_Marquee::setPosition(float pos)
{
	ATOM_STACK_TRACE(ATOM_Marquee::setPosition);

	_textPosition = pos;
}

// 取得位置
float ATOM_Marquee::getPosition() const
{
	ATOM_STACK_TRACE(ATOM_Marquee::getPosition);

	return _textPosition;
}

// 取得字串宽度
float ATOM_Marquee::getTextWidth() const
{
	ATOM_STACK_TRACE(ATOM_Marquee::getTextWidth);

	return _textWidth;
}

// 设置位置
void ATOM_Marquee::setOffset(float o)
{
	ATOM_STACK_TRACE(ATOM_Marquee::setOffset);

	_offset = o;
}

// 取得位置
float ATOM_Marquee::getOffset() const
{
	ATOM_STACK_TRACE(ATOM_Marquee::getOffset);

	return _offset;
}


void ATOM_Marquee::setText (const char *str)
{
	ATOM_STACK_TRACE(ATOM_Marquee::setText);

	_caption = str ? str : "";
	_textDirty = true;
	invalidate ();
}

ATOM_Text *ATOM_Marquee::getText (void) const
{
	ATOM_STACK_TRACE(ATOM_Marquee::getText);

	return _text.get();
}

void ATOM_Marquee::setFont (ATOM_GUIFont::handle font)
{
	ATOM_Widget::setFont (font);

	_text->setFont (ATOM_GUIFont::getFontHandle (getFont()));
	_text->setCharMargin (ATOM_GUIFont::getFontCharMargin (getFont()));
	_textDirty = true;
}

ATOM_WidgetType ATOM_Marquee::getType (void) const
{
	ATOM_STACK_TRACE(ATOM_Marquee::getType);

	return WT_MARQUEE;
}

void ATOM_Marquee::onLButtonDown (ATOM_WidgetLButtonDownEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Marquee::onLButtonDown);

	setBorderMode (ATOM_Widget::Drop);
	setCapture ();
}

void ATOM_Marquee::onLButtonUp (ATOM_WidgetLButtonUpEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Marquee::onLButtonUp);

	if (getRenderer()->getCapture() == this)
	{
		setBorderMode (ATOM_Widget::Raise);
		releaseCapture ();

		if (isMouseHover ())
		{
			onClicked ();
		}
	}
}

void ATOM_Marquee::onMouseEnter (ATOM_WidgetMouseEnterEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Marquee::onMouseEnter);

	if (getRenderer()->getCapture() == this)
	{
		setBorderMode (ATOM_Widget::Drop);
	}
}

void ATOM_Marquee::onMouseLeave (ATOM_WidgetMouseLeaveEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Marquee::onMouseLeave);

	if (getRenderer()->getCapture() == this)
	{
		setBorderMode (ATOM_Widget::Raise);
	}
}

void ATOM_Marquee::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Marquee::onPaint);

	callParentHandler (event);

	if (!_caption.empty())
	{
		if (_textDirty)
		{
			calcTextLayout ();
			_textDirty = false;
		}

		// 只有字符大于整个的宽度才会滚动
		if(1 || _textWidth > _clientRect.size.w )
		{
			float elapsed = ATOM_APP->getFrameStamp().elapsedTick*0.001f;

			if(MARQUEEDIR_LEFT == _dir)
			{	// 向左滚动
				_textPosition -= elapsed*_speed;
			}
			else
			{	// 向右滚动
				_textPosition += elapsed*_speed;
			}

			while (_textPosition > 0)
			{
				_textPosition -= (_textWidth + _offset);
			}

			int pos = _textPosition;

			ATOM_GUICanvas::DrawTextInfo info;
			info.textObject = _text.get();
			info.textString = _text->getString();
			info.font = _text->getFont();
			//info.x = _textPosition.x;
			info.y = _y;
			info.shadowOffsetX = 0;
			info.shadowOffsetY = 0;
			info.textColor = getFontColor();
			info.outlineColor = _widgetFontOutLineColor;
			info.shadowColor = 0;
			info.underlineColor = 0;
			info.flags = ATOM_GUICanvas::DRAWTEXT_TEXTOBJECT;
			if(getFontOutLineColor().getByteA())
			{
				info.flags |= ATOM_GUICanvas::DRAWTEXT_OUTLINE;
			}

			while (pos < getClientRect().size.w)
			{
				info.x = pos;
				event->canvas->drawTextEx(&info);
				//event->canvas->drawText (getText(), pos, _y, getFontColor());
				pos += (_textWidth + _offset);
			}
		}
		else
		{
			event->canvas->drawText (getText(), _textPosition, _y, getFontColor());
		}


	}
}

void ATOM_Marquee::onClicked (void)
{
	ATOM_STACK_TRACE(ATOM_Marquee::onClicked);

	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->queueEvent (ATOM_NEW(ATOM_WidgetCommandEvent, getId()), ATOM_APP);
	}
}

void ATOM_Marquee::onPaintFrame (ATOM_WidgetDrawFrameEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Marquee::onPaintFrame);

	drawFrame (event->canvas, getWidgetState ());
}

void ATOM_Marquee::onIdle(ATOM_WidgetIdleEvent *event)
{

}

void ATOM_Marquee::onResize (ATOM_WidgetResizeEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Marquee::onResize);

	callParentHandler (event);

	_textDirty = true;
}


