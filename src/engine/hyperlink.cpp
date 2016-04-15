#include "stdafx.h"
#include "hyperlink.h"
#include "gui_canvas.h"


ATOM_BEGIN_EVENT_MAP(ATOM_HyperLink, ATOM_Widget)
	ATOM_EVENT_HANDLER(ATOM_HyperLink, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	ATOM_EVENT_HANDLER(ATOM_HyperLink, ATOM_WidgetLButtonUpEvent, onLButtonUp)
	ATOM_EVENT_HANDLER(ATOM_HyperLink, ATOM_WidgetMouseEnterEvent, onMouseEnter)
	ATOM_EVENT_HANDLER(ATOM_HyperLink, ATOM_WidgetMouseLeaveEvent, onMouseLeave)
	ATOM_EVENT_HANDLER(ATOM_HyperLink, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(ATOM_HyperLink, ATOM_WidgetDrawFrameEvent, onPaintFrame)
	ATOM_EVENT_HANDLER(ATOM_HyperLink, ATOM_WidgetIdleEvent, onIdle)
	ATOM_EVENT_HANDLER(ATOM_HyperLink, ATOM_WidgetResizeEvent, onResize)
ATOM_END_EVENT_MAP

ATOM_HyperLink::ATOM_HyperLink()
{
	ATOM_STACK_TRACE(ATOM_HyperLink::ATOM_HyperLink);

	_normalColor = ATOM_ColorARGB(0.f, 0.f, 0.f, 1.f);
	_hoverColor = ATOM_ColorARGB(0.f, 0.f, 0.f, 1.f);

	_clickSound = INVALID_AUDIOID;
	_hoverSound = INVALID_AUDIOID;

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_HyperLink::ATOM_HyperLink (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
:
ATOM_Label (parent, rect, (style & ~ATOM_Widget::TitleBar)|ATOM_Widget::Control, id, showState)
{
	ATOM_STACK_TRACE(ATOM_HyperLink::ATOM_HyperLink);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

	_normalColor = ATOM_ColorARGB(0.f, 0.f, 0.f, 1.f);
	_hoverColor = ATOM_ColorARGB(0.f, 0.f, 0.f, 1.f);

	_clickSound = INVALID_AUDIOID;
	_hoverSound = INVALID_AUDIOID;

	setBorderMode (ATOM_Widget::Raise);

	resize (rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_HyperLink::~ATOM_HyperLink (void)
{
	ATOM_STACK_TRACE(ATOM_HyperLink::~ATOM_HyperLink);
}


void ATOM_HyperLink::setText (const char *str, bool suitWidth)
{
	ATOM_STACK_TRACE(ATOM_HyperLink::setText);

	_caption = str ? str : "";
	_textDirty = true;
	if(!_caption.empty() && suitWidth)
	{
		ATOM_FontHandle font = ATOM_GUIFont::getFontHandle (getFont());
		int charmargin = ATOM_GUIFont::getFontCharMargin (getFont());
		ATOM_SetCharMargin (charmargin);
		int l, t, w, h;
		ATOM_CalcStringBounds (font, _caption.c_str(), _caption.size(), &l, &t, &w, &h);
		ATOM_Rect2Di rc = _widgetRect;
		rc.size.w = _widgetRect.size.w - _clientRect.size.w + w;
		resize(rc);
	}
	invalidate ();
}

void ATOM_HyperLink::setHoverFontColor(const ATOM_ColorARGB& clr)
{
	ATOM_STACK_TRACE(ATOM_HyperLink::setHoverFontColor);

	_hoverColor = clr;
}

const ATOM_ColorARGB& ATOM_HyperLink::getHoverFontColor() const
{
	ATOM_STACK_TRACE(ATOM_HyperLink::getHoverFontColor);

	return _hoverColor;
}

void ATOM_HyperLink::setClickSound(int sound)
{
	ATOM_STACK_TRACE(ATOM_HyperLink::setClickSound);

	_clickSound = sound;
}

int ATOM_HyperLink::getClickSound() const
{
	ATOM_STACK_TRACE(ATOM_HyperLink::getClickSound);

	return _clickSound;
}

void ATOM_HyperLink::setHoverSound(int sound)
{
	ATOM_STACK_TRACE(ATOM_HyperLink::ATOM_HyperLink);

	_hoverSound = sound;
}

int ATOM_HyperLink::getHoverSound() const
{
	ATOM_STACK_TRACE(ATOM_HyperLink::getHoverSound);

	return _hoverSound;
}

ATOM_WidgetType ATOM_HyperLink::getType (void) const
{
	ATOM_STACK_TRACE(ATOM_HyperLink::getType);

	return WT_HYPERLINK;
}

void ATOM_HyperLink::onLButtonDown (ATOM_WidgetLButtonDownEvent *event)
{
	ATOM_STACK_TRACE(ATOM_HyperLink::onLButtonDown);

	setBorderMode (ATOM_Widget::Drop);
	setCapture ();
}

void ATOM_HyperLink::onLButtonUp (ATOM_WidgetLButtonUpEvent *event)
{
	ATOM_STACK_TRACE(ATOM_HyperLink::onLButtonUp);

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

void ATOM_HyperLink::onMouseEnter (ATOM_WidgetMouseEnterEvent *event)
{
	ATOM_STACK_TRACE(ATOM_HyperLink::onMouseEnter);

	if (getRenderer()->getCapture() == this)
	{
		setBorderMode (ATOM_Widget::Drop);
	}
	playSound(_hoverSound);
}

void ATOM_HyperLink::onMouseLeave (ATOM_WidgetMouseLeaveEvent *event)
{
	ATOM_STACK_TRACE(ATOM_HyperLink::onMouseLeave);

	if (getRenderer()->getCapture() == this)
	{
		setBorderMode (ATOM_Widget::Raise);
	}
}

void ATOM_HyperLink::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_HyperLink::onPaint);

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

	if (!_caption.empty())
	{
		if (_textDirty)
		{
			calcTextLayout ();
			_textDirty = false;
		}

 		ATOM_ColorARGB clr = (WST_HOVER == getWidgetState()) ? _hoverColor : getFontColor();
		//if(_widgetFontOutLineColor.getByteA())
		//{
		//	event->canvas->drawTextOutline (_text.get(), _textPosition.x, _textPosition.y, clr, _widgetFontOutLineColor);
		//}
		//else
		//{
		//	event->canvas->drawText(_text.get(), _textPosition.x, _textPosition.y, clr);
		//}

		ATOM_GUICanvas::DrawTextInfo info;
		info.textObject = _text.get();
		info.textString = _text->getString();
		info.font = _text->getFont();
		info.x = _textPosition.x;
		info.y = _textPosition.y;
		info.shadowOffsetX = 0;
		info.shadowOffsetY = 0;
		info.textColor = clr;
		info.outlineColor = _widgetFontOutLineColor;
		info.shadowColor = 0;
		info.underlineColor =  (WST_HOVER == getWidgetState()) ? _hoverColor : _underlineColor;
		info.flags = ATOM_GUICanvas::DRAWTEXT_TEXTOBJECT;
		if(getFontOutLineColor().getByteA())
		{
			info.flags |= ATOM_GUICanvas::DRAWTEXT_OUTLINE;
		}
		if(_underlineColor.getByteA())
		{
			info.flags |= ATOM_GUICanvas::DRAWTEXT_UNDERLINE;
		}
		event->canvas->drawTextEx(&info);
	}

	/*
	callParentHandler (event);

	if (!_caption.empty())
	{
		if (_textDirty)
		{
			calcTextLayout ();
			_textDirty = false;
		}

		ATOM_ColorARGB clr = (WST_HOVER == getWidgetState()) ? _hoverColor : _fontColor;
		event->canvas->drawText (_text.get(), _textPosition.x, _textPosition.y, clr);
	}
	*/
}

void ATOM_HyperLink::onClicked (void)
{
	ATOM_STACK_TRACE(ATOM_HyperLink::onClicked);

	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->queueEvent (ATOM_NEW(ATOM_WidgetCommandEvent, getId()), ATOM_APP);
	}
	playSound(_clickSound);
}

void ATOM_HyperLink::onPaintFrame (ATOM_WidgetDrawFrameEvent *event)
{
	ATOM_STACK_TRACE(ATOM_HyperLink::onPaintFrame);

	drawFrame (event->canvas, getWidgetState ());
}

void ATOM_HyperLink::onIdle(ATOM_WidgetIdleEvent *event)
{
}

void ATOM_HyperLink::onResize (ATOM_WidgetResizeEvent *event)
{
	ATOM_STACK_TRACE(ATOM_HyperLink::onResize);

	callParentHandler (event);

	_textDirty = true;
}

void ATOM_HyperLink::SetLua(const char* pParam)
{
	if (pParam)
	{
		m_strParam = pParam;
	}
	else
	{
		m_strParam.clear();
	}
}

/**
*@brief µÃµ½luaº¯Êý
**/
const char* ATOM_HyperLink::GetLua(void)
{ 
	return m_strParam.c_str(); 
}

void ATOM_HyperLink::SetParam(int param)
{
	m_iParam = param;
}

int ATOM_HyperLink::GetParam()
{
	return m_iParam;
}
