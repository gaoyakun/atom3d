#include "stdafx.h"
#include "label.h"
#include "gui_canvas.h"

ATOM_BEGIN_EVENT_MAP(ATOM_Label, ATOM_Panel)
	ATOM_EVENT_HANDLER(ATOM_Label, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(ATOM_Label, ATOM_WidgetResizeEvent, onResize)
ATOM_END_EVENT_MAP

ATOM_Label::ATOM_Label()
{
	ATOM_STACK_TRACE(ATOM_Label::ATOM_Label);

	_text = ATOM_HARDREF(ATOM_Text) ();
	_text->setFont (ATOM_GUIFont::getFontHandle(getFont()));
	_text->setCharMargin (ATOM_GUIFont::getFontCharMargin(getFont()));
	_text->setZValue (1.f);
	_text->setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
	_align = AlignX_Middle|AlignY_Middle;
	_textDirty = false;
	_textPosition.x = 0;
	_textPosition.y = 0;
	_customColor = 0;
	_offset.x = _offset.y = 0;
	_rotation = 0.f;
	//_outlineColor.setRaw(0);
	_underlineColor.setRaw(0);

	setBorderMode (ATOM_Widget::Raise);
	enableEventTransition (false);

	resize (ATOM_Rect2Di(0,0,100,100));

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_Label::ATOM_Label (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
:
ATOM_Panel (parent, rect, style | ATOM_Widget::Control, id, showState)
{
	ATOM_STACK_TRACE(ATOM_Label::ATOM_Label);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

	_text = ATOM_HARDREF(ATOM_Text) ();
	_text->setFont (ATOM_GUIFont::getFontHandle(getFont()));
	_text->setCharMargin (ATOM_GUIFont::getFontCharMargin(getFont()));
	_text->setZValue (1.f);
	_text->setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
	_align = AlignX_Middle|AlignY_Middle;
	_textDirty = false;
	_textPosition.x = 0;
	_textPosition.y = 0;
	_customColor = 0;
	_offset.x = _offset.y = 0;
	_rotation = 0.f;
//	_outlineColor.setRaw(0);
	_underlineColor.setRaw(0);

	setBorderMode (ATOM_Widget::Raise);
	enableEventTransition (false);

	resize (rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_Label::~ATOM_Label (void)
{
	ATOM_STACK_TRACE(ATOM_Label::~ATOM_Label);
}

void ATOM_Label::setText (const char *str)
{
	ATOM_STACK_TRACE(ATOM_Label::setText);

	_caption = str ? str : "";
	_textDirty = true;
	invalidate ();
}

const char* ATOM_Label::getText (void) const
{
	ATOM_STACK_TRACE(ATOM_Label::getText);

	return _caption.c_str();
}

void ATOM_Label::setFont (ATOM_GUIFont::handle font)
{
	ATOM_Panel::setFont (font);

	_text->setFont (ATOM_GUIFont::getFontHandle (getFont()));
	_text->setCharMargin (ATOM_GUIFont::getFontCharMargin (getFont()));
	_textDirty = true;
}

void ATOM_Label::setAlign(int align)
{
	ATOM_STACK_TRACE(ATOM_Label::setAlign);

	_align = align;
	_textDirty = true;
	invalidate ();
}

int ATOM_Label::getAlign() const
{
	ATOM_STACK_TRACE(ATOM_Label::getAlign);

	return _align;
}

// 设置文字偏移
void ATOM_Label::setOffset(const ATOM_Point2Di& offset)
{
	ATOM_STACK_TRACE(ATOM_Label::setOffset);

	_offset = offset;
}

// 取得文字偏移
const ATOM_Point2Di& ATOM_Label::getOffset() const
{
	ATOM_STACK_TRACE(ATOM_Label::getOffset);

	return _offset;
}

void ATOM_Label::setColor(ATOM_ColorARGB color)
{
	ATOM_STACK_TRACE(ATOM_Label::setColor);

	_customColor = color;
}

ATOM_ColorARGB ATOM_Label::getColor() const
{
	ATOM_STACK_TRACE(ATOM_Label::getColor);

	return _customColor;
}

void ATOM_Label::setRotation(float f)
{
	ATOM_STACK_TRACE(ATOM_Label::setRotation);

	_rotation = f;
}

float ATOM_Label::getRotation() const
{
	ATOM_STACK_TRACE(ATOM_Label::getRotation);

	return _rotation;
}

void ATOM_Label::setOutlineColor(ATOM_ColorARGB color)
{
	ATOM_STACK_TRACE(ATOM_Label::setOutlineColor);

	_widgetFontOutLineColor = color;
}

ATOM_ColorARGB ATOM_Label::getOutlineColor() const
{
	ATOM_STACK_TRACE(ATOM_Label::getOutlineColor);

	return _widgetFontOutLineColor;
}

void ATOM_Label::setUnderlineColor(ATOM_ColorARGB color)
{
	ATOM_STACK_TRACE(ATOM_Label::setUnderlineColor);

	_underlineColor = color;
}

ATOM_ColorARGB ATOM_Label::getUnderlineColor() const
{
	ATOM_STACK_TRACE(ATOM_Label::getUnderlineColor);

	return _underlineColor;
}

ATOM_WidgetType ATOM_Label::getType (void) const
{
	ATOM_STACK_TRACE(ATOM_Label::getType);

	return WT_LABEL;
}

void ATOM_Label::calcTextLayout (void)
{
	ATOM_STACK_TRACE(ATOM_Label::calcTextLayout);

	ATOM_FontHandle font = ATOM_GUIFont::getFontHandle (getFont());
	int charmargin = ATOM_GUIFont::getFontCharMargin (getFont());
	ATOM_SetCharMargin (charmargin);
	int l, t, w, h;
	unsigned clip = ATOM_ClipString (font, _caption.c_str(), _clientRect.size.w, &l, &t, &w, &h);

	// 对齐
	if(_align & AlignX_Left)
	{
		_textPosition.x = _offset.x;
	}
	else if(_align & AlignX_Right)
	{
		_textPosition.x = _clientRect.size.w - w - _offset.x;
	}
	else
	{
		_textPosition.x = (_clientRect.size.w - w) / 2;
	}

	const char *testStr = "gf";
	ATOM_CalcStringBounds (font, testStr, strlen(testStr), &l, &t, &w, &h);

	if(_align & AlignY_Top)
	{
		_textPosition.y = -t + _offset.y;
	}
	else if(_align & AlignY_Bottom)
	{
		_textPosition.y = _clientRect.size.h - h - t - _offset.y;
	}
	else
	{
		_textPosition.y = (_clientRect.size.h - h) / 2 - t;
	}


	_text->setString (_caption.substr(0, clip).c_str());
}

void ATOM_Label::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Label::onPaint);

	if (_clientImageId != ATOM_INVALID_IMAGEID)
	{
#if 1
		if(!_isExClientImage || !_clientImage)
		{
			_clientImage = getValidImage (_clientImageId);
			_isExClientImage = false;
		}
#else
		const ATOM_GUIImageList *imagelist = getValidImageList ();
		ATOM_GUIImage *image = imagelist->getImage (_clientImageId);
#endif
		if (!_clientImage)
		{
			_clientImage = ATOM_GUIImageList::getDefaultImageList().getImage (ATOM_IMAGEID_WINDOW_BKGROUND);
			ATOM_ASSERT(_clientImage);
		}
		if(_customColor)
		{
			_clientImage->draw (event->state, event->canvas, ATOM_Rect2Di(ATOM_Point2Di(0, 0), event->canvas->getSize()), &_customColor, _rotation);
		}
		else
		{
			_clientImage->draw (event->state, event->canvas, ATOM_Rect2Di(ATOM_Point2Di(0, 0), event->canvas->getSize()), NULL, _rotation);
		}

	}

	if (!_caption.empty())
	{
		if (_textDirty)
		{
			calcTextLayout ();
			_textDirty = false;
		}

		/*
		if(_outlineColor.getRaw())
		{
			event->canvas->drawTextOutline (_text.get(), _textPosition.x, _textPosition.y, _fontColor, _outlineColor);
		}
		else
		{
			event->canvas->drawText (_text.get(), _textPosition.x, _textPosition.y, _fontColor);

		}
		*/

		ATOM_GUICanvas::DrawTextInfo info;
		info.textObject = _text.get();
		info.textString = _text->getString();
		info.font = _text->getFont();
		info.x = _textPosition.x;
		info.y = _textPosition.y;
		info.shadowOffsetX = 0;
		info.shadowOffsetY = 0;
		info.textColor = getFontColor();
		info.outlineColor = _widgetFontOutLineColor;
		info.shadowColor = 0;
		info.underlineColor = _underlineColor;
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
}

void ATOM_Label::onResize (ATOM_WidgetResizeEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Label::onResize);

	callParentHandler (event);

	_textDirty = true;
}


