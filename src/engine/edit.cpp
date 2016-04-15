#include "stdafx.h"
#include "edit.h"
#include "gui_canvas.h"


ATOM_BEGIN_EVENT_MAP(ATOM_Edit, ATOM_Widget)
	ATOM_EVENT_HANDLER(ATOM_Edit, ATOM_WidgetCharEvent, onChar)
	ATOM_EVENT_HANDLER(ATOM_Edit, ATOM_WidgetKeyDownEvent, onKeyDown)
	ATOM_EVENT_HANDLER(ATOM_Edit, ATOM_WidgetKeyUpEvent, onKeyUp)
	ATOM_EVENT_HANDLER(ATOM_Edit, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	ATOM_EVENT_HANDLER(ATOM_Edit, ATOM_WidgetLButtonUpEvent, onLButtonUp)
	ATOM_EVENT_HANDLER(ATOM_Edit, ATOM_WidgetResizeEvent, onResize)
	ATOM_EVENT_HANDLER(ATOM_Edit, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(ATOM_Edit,ATOM_WidgetMouseMoveEvent,onMouseMove)
	ATOM_EVENT_HANDLER(ATOM_Edit,ATOM_WidgetMouseLeaveEvent,onMouseLeave)
ATOM_END_EVENT_MAP

static bool isNumber (const wchar_t *str)
{
	if (str)
	{
		int len = wcslen (str);
		if (len == 0)
		{
			return false;
		}

		bool dot = false;
		for (int n = 0; n < len; ++n)
		{
			wchar_t c = str[n];
			if (c == L'.')
			{
				if (dot)
				{
					return false;
				}
				dot = true;
			}
			else if (c < L'0' || c > L'9')
			{
				return false;
			}
		}

		return true;
	}

	return false;
}
static bool isInteger (const wchar_t *str)
{
	if (str)
	{
		int len = wcslen (str);
		if (len == 0)
		{
			return false;
		}
		for (int n = 0; n < len; ++n)
		{
			wchar_t c = str[n];
			if (c < L'0' || c > L'9')
			{
				return false;
			}
		}

		return true;
	}

	return false;
}
ATOM_Edit::ATOM_Edit()
{
	ATOM_STACK_TRACE(ATOM_Edit::ATOM_Edit);

	_text = NULL;
	_textPosition.x = 0;
	_textPosition.y = 0;
	_textDirty = false;

	_textDefault = NULL;
	_fontDefualtColor = ATOM_ColorARGB(1.f, 1.f, 1.f, 1.f);
	//_outlineColor.setRaw(0);
	_textDefaultPosition.x = 0;
	_textDefaultPosition.y = 0;
	_frontImageId = 1;
	_startPosition = 0;

	_cursor = 0;
	_cursorImageId = ATOM_IMAGEID_EDIT_CURSOR;
	_cursorOffset = 0;
	_cursorWidth = 2;
	_cursorPosition.x = 0;
	_cursorPosition.y = 0;

	_offsetX = 0;

	_editType = 0;
	_maxLength = 0;

	_selectStart = -1;

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_Edit::ATOM_Edit (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
: ATOM_Widget (parent, rect, style|ATOM_Widget::Control|ATOM_Widget::ClipChildren, id, showState)
{
	ATOM_STACK_TRACE(ATOM_Edit::ATOM_Edit);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

	_text = ATOM_HARDREF(ATOM_Text) ();
	_text->setFont (ATOM_GUIFont::getFontHandle(getFont()));
	_text->setCharMargin (0);
	_text->setZValue (1.f);
	_text->setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
	//_outlineColor.setRaw(0);
	_textPosition.x = 0;
	_textPosition.y = 0;
	_textDirty = false;

	_textDefault = ATOM_HARDREF(ATOM_Text) ();
	_textDefault->setFont (ATOM_GUIFont::getFontHandle(getFont()));
	_textDefault->setCharMargin (0);
	_textDefault->setZValue (1.f);
	_textDefault->setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
	_fontDefualtColor = ATOM_ColorARGB(1.f, 1.f, 1.f, 1.f);
	_textDefaultPosition.x = 0;
	_textDefaultPosition.y = 0;

	_startPosition = 0;

	_cursor = 0;
	_cursorImageId = ATOM_IMAGEID_EDIT_CURSOR;
	_cursorOffset = 0;
	_cursorWidth = 2;
	_cursorPosition.x = 0;
	_cursorPosition.y = 0;
	_cusorNewPosition = _cusorOldPosition = _cursorPosition;
	_offsetX = 0;

	_editType = 0;
	_maxLength = 0;
	_frontImageId = 1;
	_selectStart = -1;
	resize (rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_Edit::~ATOM_Edit (void)
{
	ATOM_STACK_TRACE(ATOM_Edit::~ATOM_Edit);
}

ATOM_WidgetType ATOM_Edit::getType (void) const
{
	ATOM_STACK_TRACE(ATOM_Edit::getType);

	return WT_EDIT;
}

void ATOM_Edit::setEditType(int type)
{
	ATOM_STACK_TRACE(ATOM_Edit::setEditType);

	_editType = type;
}

int ATOM_Edit::getEditType() const
{
	ATOM_STACK_TRACE(ATOM_Edit::getEditType);

	return _editType;
}

void ATOM_Edit::setFont (ATOM_GUIFont::handle font)
{
	ATOM_Widget::setFont (font);

	_text->setFont (ATOM_GUIFont::getFontHandle (getFont()));
	_text->setCharMargin (ATOM_GUIFont::getFontCharMargin (getFont()));
	_textDefault->setFont (ATOM_GUIFont::getFontHandle (getFont()));
	_textDefault->setCharMargin (ATOM_GUIFont::getFontCharMargin (getFont()));
	_textDirty = true;
}

void ATOM_Edit::setString(const char *str)
{
	ATOM_STACK_TRACE(ATOM_Edit::setString);

	_startPosition = 0;

	_string = ATOM_Ansi2Wide(str, strlen(str));
	moveCursor(getStringLength());

	_selectStart = -1;
	calcTextLayout ();
	_textDirty = false;

	if (_parent)
	{
		_parent->queueEvent (ATOM_NEW(ATOM_EditTextChangedEvent, getId(), _text->getString()), ATOM_APP);
	}
	_cusorOldPosition = _cusorNewPosition;
}

void ATOM_Edit::getString (ATOM_STRING& str) const
{
	str = ATOM_Wide2Ansi (_string.c_str(), _string.size());
}

ATOM_STRING ATOM_Edit::getString (void) const
{
	ATOM_STRING s;
	getString (s);
	return s;
}

void ATOM_Edit::setDefaultFontColor(const ATOM_ColorARGB& clr)
{
	ATOM_STACK_TRACE(ATOM_Edit::setDefaultFontColor);

	_fontDefualtColor = clr;
}

const ATOM_ColorARGB& ATOM_Edit::getDefaultFontColor() const
{
	ATOM_STACK_TRACE(ATOM_Edit::getDefaultFontColor);

	return _fontDefualtColor;
}

void ATOM_Edit::setDefaultString(const char *str)
{
	ATOM_STACK_TRACE(ATOM_Edit::setDefaultString);

	_textDefault->setString(str);
	_textDirty = true;
}

void ATOM_Edit::getDefaultString (ATOM_STRING& str)
{
	ATOM_STACK_TRACE(ATOM_Edit::getDefaultString);

	str = _textDefault->getString() ? _textDefault->getString() : "";
}

void ATOM_Edit::calcTextLayout (void)
{
	ATOM_STACK_TRACE(ATOM_Edit::calcTextLayout);

	//
	// 计算显示多少文字
	//

	// 调整
	if(_startPosition < 0)
	{
		_startPosition = 0;
	}

	if(_cursor < _startPosition)
	{
		_startPosition = _cursor;
	}

	// 初始化
	int l, t, w, h;
	ATOM_FontHandle font = ATOM_GUIFont::getFontHandle (getFont());
	int charmargin = ATOM_GUIFont::getFontCharMargin (getFont());
	ATOM_SetCharMargin (charmargin);
	int frameWidth = _clientRect.size.w - _offsetX;
	if (frameWidth < 0)
		frameWidth = 0;

	ATOM_WSTRING passString;
	if(_editType & EDITTYPE_PASSWORD)
	{
		size_t passStringLen = wcslen(_string.c_str());
		passString.resize(passStringLen);
		for(size_t i=0; i<passStringLen; ++i)
		{
			passString[i] = L'*';
		}
	}
	const ATOM_WSTRING& ws = (_editType & EDITTYPE_PASSWORD) ? passString : _string;
	ATOM_STRING s;

	if( wcslen(ws.c_str()) > 0)
	{
		// start 到 cursor的长度
		size_t len = _cursor - _startPosition;
		s = ATOM_Wide2Ansi (ws.c_str()+_startPosition, len);

		if(!ATOM_CalcStringBounds (font, s.c_str(), strlen(s.c_str()), &l, &t, &w, &h))
		{
			l = t = w = h = 0;
		}

		if(w > frameWidth)
		{
			// 大于最长宽度,start 向前推
			++_startPosition;
			--len;
			while(1)
			{
				s = ATOM_Wide2Ansi (ws.c_str()+_startPosition, len);

				int l, t, w, h;
				if(!ATOM_CalcStringBounds (font, s.c_str(), strlen(s.c_str()), &l, &t, &w, &h))
				{
					l = t = w = h = 0;
				}
				if(w > frameWidth)
				{
					++_startPosition;
					--len;
				}
				else
				{
					break;
				}
			}
		}
		else if(w < frameWidth)
		{
			// 小于最长宽度,cursor向后推
			size_t maxlen = wcslen(ws.c_str()+_startPosition);
			if(len < maxlen)
			{
				++len;
				while(1)
				{
					s = ATOM_Wide2Ansi (ws.c_str()+_startPosition, len);

					int l, t, w, h;
					if(!ATOM_CalcStringBounds (font, s.c_str(), strlen(s.c_str()), &l, &t, &w, &h))
					{
						l = t = w = h = 0;
					}
					if(w < frameWidth)
					{
						if(len >= maxlen)
						{
							break;
						}
						++len;
					}
					else if( w > frameWidth)
					{
						--len;
						s = ATOM_Wide2Ansi (ws.c_str()+_startPosition, len);
						break;
					}
					else
					{
						break;
					}
				}
			}
		}
	}

	// 设置text
	if (strcmp (_text->getString(), s.c_str()))
	{
		_text->setString (s.c_str());

		if (getShowState () != ATOM_Widget::Hide)
		{
			invalidate ();
		}
	}

	//
	// 计算绘制的位置
	//

	// 计算文字 text
	const char *text = _text->getString();
	if (strcmp (text, ""))
	{
		if(!ATOM_CalcStringBounds (font, text, strlen(text), &l, &t, &w, &h))
		{
			l = t = w = h = 0;
		}
		_textPosition.x = _offsetX + l;

		const char *testStr = "gf";
		ATOM_CalcStringBounds (font, testStr, strlen(testStr), &l, &t, &w, &h);
		_textPosition.y = (_clientRect.size.h - h) / 2 - t;
	}
	else
	{
		_textPosition.x = _offsetX;
		_textPosition.y = 0;
	}

	// 计算默认文字
	const char *textDefault = _textDefault->getString();
	if (strcmp (textDefault, ""))
	{
		if(!ATOM_CalcStringBounds (font, textDefault, strlen(textDefault), &l, &t, &w, &h))
		{
			l = t = w = h = 0;
		}
		_textDefaultPosition.x = _offsetX + l;

		const char *testStr = "gf";
		ATOM_CalcStringBounds (font, testStr, strlen(testStr), &l, &t, &w, &h);
		_textDefaultPosition.y = (_clientRect.size.h - h) / 2 - t;
	}
	else
	{
		_textDefaultPosition.x = _offsetX;
		_textDefaultPosition.y = 0;
	}

	// 计算焦点
	if(_cursor-_startPosition > 0)
	{
		ATOM_STRING s2 = ATOM_Wide2Ansi (ws.c_str()+_startPosition, _cursor-_startPosition);

		if(!ATOM_CalcStringBounds (font, s2.c_str(), strlen(s2.c_str()), &l, &t, &w, &h))
		{
			l = t = w = h = 0;
		}
		_cursorPosition.x = _offsetX + l + w;
		_cursorPosition.y = 0;
	}
	else
	{
		_cursorPosition.x = _offsetX;
		_cursorPosition.y = 0;
	}
}

void ATOM_Edit::insertText(const char* str)
{
	ATOM_STACK_TRACE(ATOM_Edit::insertText);

	size_t strLen = strlen(str);

	if(_maxLength > 0)
	{
		ATOM_STRING s = ATOM_Wide2Ansi (_string.c_str(), _string.size());
		if(strlen(s.c_str())+strLen > _maxLength)
		{
			return;
		}
	}

	// 转化为宽字节
	ATOM_WSTRING wstr = ATOM_Ansi2Wide(str, strlen(str));
	size_t wstrLen = wcslen(wstr.c_str());

	// 插入
	if (_editType & EDITTYPE_NUMBER)
	{
		// 检查是否是合法数字
		ATOM_WSTRING stmp = _string;
		stmp.insert(_cursor, wstr.c_str());
		if (!isNumber (stmp.c_str()))
		{
			return;
		}
		_string = stmp;
	}
	else if (_editType & EDITTYPE_INTEGER)
	{
		// 检查是否是合法整数
		ATOM_WSTRING stmp = _string;
		stmp.insert(_cursor, wstr.c_str());
		if (!isInteger(stmp.c_str()))
		{
			return;
		}
		_string = stmp;
	}
	else
	{
		_string.insert(_cursor, wstr.c_str());
	}
	_cursor += wstrLen;

	calcTextLayout ();
	_textDirty = false;

	ATOM_EditTextChangedEvent event(getId(), _text->getString());
	_parent->handleEvent(&event);
	_cusorOldPosition = _cusorNewPosition;
}

void ATOM_Edit::backSpace()
{
	ATOM_STACK_TRACE(ATOM_Edit::backSpace);

	{
		ATOM_EditSysKeyEvent event(getId(), KEY_BACKSPACE);
		_parent->handleEvent(&event);
	}

	if(_cursor <= 0)
	{
		return;
	}

	_cursor -= 1;
	if(_startPosition>0)
		_startPosition--;
	_string.erase(_cursor, 1);

	calcTextLayout ();
	_textDirty = false;

	ATOM_EditTextChangedEvent event(getId(), _text->getString());
	_parent->handleEvent(&event);
	_cusorOldPosition = _cusorNewPosition;
}

void ATOM_Edit::deleteChar()
{
	ATOM_STACK_TRACE(ATOM_Edit::deleteChar);

	if(_cursor >= getStringLength())
	{
		return;
	}

	_string.erase(_cursor, 1);

	calcTextLayout ();
	_textDirty = false;

	ATOM_EditTextChangedEvent event(getId(), _text->getString());
	_parent->handleEvent(&event);
	_cusorOldPosition = _cusorNewPosition;
}

void ATOM_Edit::moveCursor(int offset, bool select)
{
	ATOM_STACK_TRACE(ATOM_Edit::moveCursor);

	size_t length = getStringLength();
	if(select)
	{
		if(_selectStart < 0 || _selectStart > length)
		{
			_selectStart = _cursor;
		}
	}
	else
	{
		_selectStart = -1;
	}

	if(_cursor == _startPosition && offset < 0)
	{
		_startPosition += offset;
	}
	int oldPosition = _cursor;

	_cursor += offset;
	if(_cursor<0)
	{
		_cursor = 0;
	}
	if(_cursor > length)
	{
		_cursor = length;
	}

	_textDirty = true;
}

void ATOM_Edit::setCursor(int cursor, bool select)
{
	ATOM_STACK_TRACE(ATOM_Edit::setCursor);

	size_t length = getStringLength();
	if(select)
	{
		if(_selectStart < 0 || _selectStart > length)
		{
			_selectStart = _cursor;
		}
	}
	else
	{
		_selectStart = -1;
	}

	if(_cursor == _startPosition && cursor < _cursor)
	{
		_startPosition = cursor;
	}
	int oldPosition = _cursor;

	_cursor = cursor;
	if(_cursor<0)
	{
		_cursor = 0;
	}
	if(_cursor > length)
	{
		_cursor = length;
	}

	_textDirty = true;
}


int ATOM_Edit::getStringLength()
{
	ATOM_STACK_TRACE(ATOM_Edit::getStringLength);

	return wcslen(_string.c_str());
}

//void ATOM_Edit::setCursorPosition (int pos)
//{
//	if (_cursor != pos)
//	{
//		_cursor = pos;
//		_textDirty = true;
//	}
//}

int ATOM_Edit::getCursorPosition (void) const
{
	ATOM_STACK_TRACE(ATOM_Edit::getCursorPosition);

	return _cursor;
}

// 设置偏移
void ATOM_Edit::setOffsetX(int x)
{
	ATOM_STACK_TRACE(ATOM_Edit::setOffsetX);

	_offsetX = x;
}

// 取得偏移
int ATOM_Edit::getOffsetX() const
{
	ATOM_STACK_TRACE(ATOM_Edit::getOffsetX);

	return _offsetX;
}

// 设置光标图片
void ATOM_Edit::setCursorImageId(int imageId)
{
	ATOM_STACK_TRACE(ATOM_Edit::setCursorImageId);

	_cursorImageId = imageId;
}

// 取得光标图片
int ATOM_Edit::getCursorImageId() const
{
	ATOM_STACK_TRACE(ATOM_Edit::getCursorImageId);

	return _cursorImageId;
}

// 设置光标偏移
void ATOM_Edit::setCursorOffset(int offset)
{
	ATOM_STACK_TRACE(ATOM_Edit::setCursorOffset);

	_cursorOffset = offset;
}

// 取得光标偏移
int ATOM_Edit::getCursorOffset() const
{
	ATOM_STACK_TRACE(ATOM_Edit::getCursorOffset);

	return _cursorOffset;
}

//! 设置光标宽度
void ATOM_Edit::setCursorWidth(int width)
{
	ATOM_STACK_TRACE(ATOM_Edit::setCursorWidth);

	_cursorWidth = width;
}

//! 取得光标宽度
int ATOM_Edit::getCursorWidth() const
{
	ATOM_STACK_TRACE(ATOM_Edit::getCursorWidth);

	return _cursorWidth;
}

//! 设置字数限制
void ATOM_Edit::setMaxLength(int maxLength)
{
	ATOM_STACK_TRACE(ATOM_Edit::setMaxLength);

	_maxLength = maxLength;
}

//! 取得字数限制
int ATOM_Edit::getMaxLength() const
{
	ATOM_STACK_TRACE(ATOM_Edit::getMaxLength);

	return _maxLength;
}

void ATOM_Edit::onResize (ATOM_WidgetResizeEvent *event)
{
	ATOM_Widget::onResize (event);

	_textDirty = true;
}

void ATOM_Edit::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Edit::onPaint);

	ATOM_GUICanvas* canvas = event->canvas;
	ATOM_WidgetState state = event->state;

	if (_textDirty)
	{
		calcTextLayout ();
		_textDirty = false;
	}

	callParentHandler (event);
	//render select image
	// 绘制前景
	if(_cusorOldPosition != _cusorNewPosition)
	{
		ATOM_GUIImage *image = getValidImage (_frontImageId);
		ATOM_ASSERT(image);
		if(_cursorPosition.x - _cusorOldPosition.x > 0)
		{
			image->draw (event->state, event->canvas, ATOM_Rect2Di(_cusorOldPosition.x, _cursorPosition.y, _cursorPosition.x - _cusorOldPosition.x, _clientRect.size.h));
		}
		else
			image->draw (event->state, event->canvas, ATOM_Rect2Di( _cursorPosition.x , _cursorPosition.y, _cusorOldPosition.x - _cursorPosition.x, _clientRect.size.h));
	}
	// render text
	const char *text = _text->getString();
	if (strcmp (text, ""))
	{
		if(_widgetFontOutLineColor.getByteA())
		{
			canvas->drawTextOutline (_text.get(), _textPosition.x, _textPosition.y, getFontColor(), _widgetFontOutLineColor);
		}
		else
		{
			canvas->drawText (_text.get(), _textPosition.x, _textPosition.y, getFontColor());
		}
	}

	// render cursor
	if(0 == (EDITTYPE_READONLY & _editType))
	{	// 只读的时候，不显示光标
		if (_cursorImageId != ATOM_INVALID_IMAGEID)
		{
			if((getRenderer()->getFocus() == this) && (ATOM_APP->getFrameStamp().currentTick % 1000 < 500))
			{
#if 1
				ATOM_GUIImage *image = getValidImage (_cursorImageId);
#else
				const ATOM_GUIImageList *imagelist = getValidImageList ();
				ATOM_GUIImage *image = imagelist->getImage (_cursorImageId);
#endif
				if (!image)
				{
					image = ATOM_GUIImageList::getDefaultImageList().getImage (ATOM_IMAGEID_WINDOW_BKGROUND);
					ATOM_ASSERT(image);
				}

				image->draw (event->state, event->canvas, ATOM_Rect2Di(_cursorPosition.x, _cursorPosition.y, _cursorWidth, _clientRect.size.h));
			}
		}
	}

	// render default text
	if(0 == (EDITTYPE_READONLY & _editType))
	{	// 只读的时候，不显示默认字体
		if((getRenderer()->getFocus() != this) && !strcmp (_text->getString(), ""))
		{
			const char *textDefault = _textDefault->getString();
			if (strcmp (textDefault, ""))
			{
				if(_widgetFontOutLineColor.getByteA())
				{
					canvas->drawTextOutline (_textDefault.get(), _textDefaultPosition.x, _textDefaultPosition.y, _fontDefualtColor, _widgetFontOutLineColor);
				}
				else
				{
					canvas->drawText (_textDefault.get(), _textDefaultPosition.x, _textDefaultPosition.y, _fontDefualtColor);
				}
			}
		}
	}
}

void ATOM_Edit::onChar(ATOM_WidgetCharEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Edit::onChar);

	// 只读情况，不允许输入
	if(EDITTYPE_READONLY & _editType)
	{
		return;
	}
	bool bSelect = false;
	unsigned short mbcc = event->mbcc;
	//if (mbcc == '\\')
	//{
	//	return;
	//}
	//ATOM_Edit不处理的键值
	if( mbcc>= 0 && mbcc < 32 && mbcc != 8)
	{
		return;
	}
	if(_cusorOldPosition != _cusorNewPosition)
	{
		size_t s = 0,t = 0;
		_selectString = getSelectString(s,t);
		if(!_selectString.empty())
		{
			deleteString(s,t);
			bSelect = true;
		}

	}
	//退格键
	if (mbcc == 8)
	{
		if(!bSelect)
			backSpace();
		return;
	}


	//处理
	char *tc = reinterpret_cast<char*>(&mbcc);
	char tc2[3];
	ZeroMemory(tc2, sizeof(tc2));
	memcpy(tc2, tc, 2);

	insertText(tc2);
}

void ATOM_Edit::deleteString( size_t start,size_t end )
{
	// 只读情况，不允许输入
	if(EDITTYPE_READONLY & _editType)
	{
		return;
	}
	int s,t;
	s = min(start,end);
	t = max(start,end);
	ATOM_STRING str = ATOM_Wide2Ansi (_string.c_str(), _string.size());
	if(t > _string.size())
	{
		end = ATOM_STRING::npos;
	}
	str.erase(s,t-s);
	_string = ATOM_Ansi2Wide(str.c_str(), strlen(str.c_str())); 
	calcTextLayout ();
	_textDirty = false;
	if(_cursor<0)
	{
		_cursor = 0;
	}
	if(_cursor > getStringLength())
	{
		_cursor = getStringLength();
	}
	_cusorOldPosition = _cusorNewPosition;
	ATOM_EditTextChangedEvent event(getId(), _text->getString());
	_parent->handleEvent(&event);

}

void ATOM_Edit::onKeyDown(ATOM_WidgetKeyDownEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Edit::onKeyDown);

	// 只读情况，不允许输入
	if(EDITTYPE_READONLY & _editType)
	{
		return;
	}

	switch(event->key)
	{
	case KEY_LEFT:
		moveCursor(-1);
		break;

	case KEY_RIGHT:
		moveCursor(1);
		break;

	case KEY_DELETE:
		deleteChar();
		break;

	case KEY_HOME:
		moveCursor(-getStringLength());
		break;

	case KEY_END:
		moveCursor(getStringLength());
		break;

	case KEY_UP:
	case KEY_DOWN:
	case KEY_RETURN:
	case KEY_KP_ENTER:
	case KEY_TAB:
	case KEY_ESCAPE:
	case KEY_SPACE:
	case KEY_COLON:
		{
			ATOM_EditSysKeyEvent keyevent(getId(), event->key);
			_parent->handleEvent(&keyevent);
		}
		break;

	case KEY_v:
		if(GetAsyncKeyState(VK_LCONTROL) &0x8000 || GetAsyncKeyState(VK_RCONTROL) &0x8000)
		{
			pasteFromClipBoard();
		}
		break;
	case KEY_a:
		if(GetAsyncKeyState(VK_LCONTROL) &0x8000 || GetAsyncKeyState(VK_RCONTROL) &0x8000)
		{
			setCursor(_startPosition+_string.size());
			_cusorOldPosition.x = _offsetX;
			_cusorOldPosition.y = 0;
			_cusorNewPosition = _cursorPosition;
		}
		break;
	case KEY_c:
		if(GetAsyncKeyState(VK_LCONTROL) &0x8000|| GetAsyncKeyState(VK_RCONTROL)&0x8000)
		{	
			size_t s = 0,t = 0;
			_selectString = getSelectString(s,t);
			if(_selectString.empty())
				return;
			if(OpenClipboard(NULL))
			{

				HGLOBAL hmem=GlobalAlloc(GHND,_selectString.size()+1);
				char *pmem=(char*)GlobalLock(hmem);

				EmptyClipboard();
				memcpy(pmem,_selectString.c_str(),_selectString.size());
				SetClipboardData(CF_TEXT,hmem);
				CloseClipboard();
				GlobalFree(hmem); 
			}
		}
		break;
	case KEY_x:
		if(GetAsyncKeyState(VK_LCONTROL)&0x8000 || GetAsyncKeyState(VK_RCONTROL)&0x8000)
		{
			size_t s = 0,t = 0;
			_selectString = getSelectString(s,t);
			deleteString(s,t);
			if(_selectString.empty())
				return;
			if(OpenClipboard(NULL))
			{

				HGLOBAL hmem=GlobalAlloc(GHND,_selectString.size()+1);
				char *pmem=(char*)GlobalLock(hmem);

				EmptyClipboard();
				memcpy(pmem,_selectString.c_str(),_selectString.size());
				SetClipboardData(CF_TEXT,hmem);
				CloseClipboard();
				GlobalFree(hmem); 
			}
		}
		break;
	}
	if(_parent)
	{
		_parent->queueEvent (ATOM_NEW(ATOM_WidgetKeyDownEvent, getId(),event->key,event->keymod), ATOM_APP);
	}
}

void ATOM_Edit::pasteFromClipBoard()
{
	ATOM_STRING s;
	if(getClipBoardText(s))
	{
		if(_maxLength > 0)
		{
			ATOM_STRING str = ATOM_Wide2Ansi (_string.c_str(), _string.size());
			if(strlen(str.c_str())+s.size() > _maxLength)
			{
				int len = (_maxLength-strlen(str.c_str()));
				if(len>0)
					s = s.substr(0,len);
			}
		}
		insertText(s.c_str());
	}
}

void ATOM_Edit::onKeyUp(ATOM_WidgetKeyUpEvent *event)
{
}

void ATOM_Edit::onLButtonDown(ATOM_WidgetLButtonDownEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Edit::onLButtonDown);

	int x = event->x - _offsetX;
	if(x<=0)
	{
		setCursor(_startPosition);
		return;
	}

	ATOM_STRING s;
	getString(s);
	ATOM_FontHandle fh = ATOM_GUIFont::getFontHandle (getFont());
	int charmargin = ATOM_GUIFont::getFontCharMargin (getFont());
	ATOM_SetCharMargin (charmargin);
	ATOM_StringClipInfo info;
	ATOM_ClipStringEx (fh, s.c_str(), x, &info);
	int clip = (info.clipRatio < 0.5f) ? info.clippedBefore : info.clippedAfter;
	ATOM_WSTRING wstr;
	if(clip > 0)
	{
		ATOM_WSTRING wstr = ATOM_Ansi2Wide(s.c_str(), clip);
		setCursor(_startPosition+wstr.size());
	}
	else
	{
		setCursor(_startPosition);
	}
	_isEnterMouse = true;
	_cusorOldPosition = _cursorPosition;
	_cusorNewPosition = _cusorOldPosition;
}

void ATOM_Edit::onLButtonUp(ATOM_WidgetLButtonUpEvent *event)
{
	_isEnterMouse = false;
}

bool ATOM_Edit::getClipBoardText (ATOM_STRING& sText)
{
	ATOM_STACK_TRACE(ATOM_Edit::getClipBoardText);

	if (!OpenClipboard (NULL))
	{
		return false;
	}

	HANDLE hData = GetClipboardData(CF_TEXT);
	if (NULL == hData)
	{
		CloseClipboard();
		return false;
	}

	char *buffer = (char*)GlobalLock(hData);
	sText = buffer;
	GlobalUnlock(hData);
	CloseClipboard();

	ATOM_STRING::size_type n = sText.find("\r\n", 0);
	if(n >= 0 && n <sText.size())
	{
		sText[n] = '\0';
	}

	n = sText.find("\n", 0);
	if(n >= 0 && n <sText.size())
	{
		sText[n] = '\0';
	}

	return true;
}


void ATOM_Edit::setOutlineColor(ATOM_ColorARGB color)
{
	ATOM_STACK_TRACE(ATOM_Edit::setOutlineColor);

	_widgetFontOutLineColor = color;
}

ATOM_ColorARGB ATOM_Edit::getOutlineColor() const
{
	ATOM_STACK_TRACE(ATOM_Edit::getOutlineColor);

	return _widgetFontOutLineColor;
}

bool ATOM_Edit::supportIME (void) const
{
	return true;
}

void ATOM_Edit::onMouseMove( ATOM_WidgetMouseMoveEvent *event )
{
	if(_isEnterMouse && event->leftDown)
	{
		int x = event->x - _offsetX;
		if(x<=0)
		{
			setCursor(_startPosition);
			_cusorNewPosition = _cursorPosition;
			return;
		}

		ATOM_STRING s;
		getString(s);
		ATOM_FontHandle fh = ATOM_GUIFont::getFontHandle (getFont());
		int charmargin = ATOM_GUIFont::getFontCharMargin (getFont());
		ATOM_SetCharMargin (charmargin);
		ATOM_StringClipInfo info;
		ATOM_ClipStringEx (fh, s.c_str(), x, &info);
		int clip = (info.clipRatio < 0.5f) ? info.clippedBefore : info.clippedAfter;
		ATOM_WSTRING wstr;
		if(clip > 0)
		{
			ATOM_WSTRING wstr = ATOM_Ansi2Wide(s.c_str(), clip);
			setCursor(_startPosition+wstr.size());
		}
		else
		{
			setCursor(_startPosition);
		}
		_cusorNewPosition = _cursorPosition;
		
	}
}

void ATOM_Edit::onMouseLeave( ATOM_WidgetMouseLeaveEvent * event )
{
	//_isEnterMouse = false;
	//_cusorNewPosition = _cusorOldPosition;
}

ATOM_STRING ATOM_Edit::getSelectString(size_t &start,size_t& end)
{
	ATOM_STACK_TRACE(ATOM_Edit::getSelectString);
	ATOM_STRING s;
	if(_cusorOldPosition == _cursorPosition)
		return s;
	int x1 = _cusorOldPosition.x - _offsetX;
	int x2 = _cursorPosition.x - _offsetX;
	if(x1<0 || x2 < 0)
	{
		return s;
	}
	getString(s);
	ATOM_FontHandle fh = ATOM_GUIFont::getFontHandle (getFont());
	int charmargin = ATOM_GUIFont::getFontCharMargin (getFont());
	ATOM_SetCharMargin (charmargin);
	ATOM_StringClipInfo info;
	ATOM_ClipStringEx (fh, s.c_str(), x1, &info);
	int clip1 = (info.clipRatio < 0.5f) ? info.clippedBefore : info.clippedAfter;
	ATOM_ClipStringEx (fh, s.c_str(), x2, &info);
	int clip2 = (info.clipRatio < 0.5f) ? info.clippedBefore : info.clippedAfter;
	ATOM_WSTRING wstr;
	if(clip1 < 0)
		clip1 = 0;
	if(clip2 < 0)
		clip2 = 0;
	if(clip1 >= 0 && clip1 <= s.size() && clip2 >= 0 && clip2 <= s.size())
	{

		if(clip1 > clip2)
		{
			
			s = s.substr(clip2,clip1-clip2);
			start = clip1;
			end = clip2;
		}
		else
		{
			s = s.substr(clip1,clip2-clip1);
			start = clip1;
			end = clip2;
		}
		return s;
	}
	return "";
}

int ATOM_Edit::getFrontImageId() const
{
	return _frontImageId;
}

void ATOM_Edit::setFrontImageId( int val )
{
	_frontImageId = val;
}

