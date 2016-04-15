#include "stdafx.h"
#include "multiedit.h"
#include "gui_canvas.h"

ATOM_BEGIN_EVENT_MAP(ATOM_MultiEdit, ATOM_Widget)
	ATOM_EVENT_HANDLER(ATOM_MultiEdit, ATOM_WidgetCharEvent, onChar)
	ATOM_EVENT_HANDLER(ATOM_MultiEdit, ATOM_WidgetKeyDownEvent, onKeyDown)
	ATOM_EVENT_HANDLER(ATOM_MultiEdit, ATOM_WidgetKeyUpEvent, onKeyUp)
	ATOM_EVENT_HANDLER(ATOM_MultiEdit, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	ATOM_EVENT_HANDLER(ATOM_MultiEdit, ATOM_WidgetMouseMoveEvent, onMouseMove)
	ATOM_EVENT_HANDLER(ATOM_MultiEdit, ATOM_WidgetDrawClientEvent, onPaint)
ATOM_END_EVENT_MAP

ATOM_MultiEdit::ATOM_MultiEdit()
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::ATOM_MultiEdit);

	_textDirty = false;

	_cursor = 0;
	_cursorImageId = ATOM_INVALID_IMAGEID;
	_cursorOffset = 0;
	_cursorWidth = 0;
	_cursorPosition.x = 0;
	_cursorPosition.y = 0;

	_lineHeight = 20;

	_offsetX = 0;

	_editType = 0;
	_maxLength = 0;
	_widgetFontColor = 0;
	_frontImageId = 1;
	_cusorNewPosition = _cusorOldPosition = _cursorPosition;
	_oldCursor = _cursor;
#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_MultiEdit::ATOM_MultiEdit (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
: ATOM_Widget (parent, rect, style|ATOM_Widget::Control|ATOM_Widget::ClipChildren, id, showState)
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::ATOM_MultiEdit);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

	_textDirty = false;

//	_startPosition = 0;

	_cursor = 0;
	_cursorImageId = ATOM_INVALID_IMAGEID;
	_cursorOffset = 0;
	_cursorWidth = 0;
	_cursorPosition.x = 0;
	_cursorPosition.y = 0;

	_lineHeight = 20;

	_offsetX = 0;

	_editType = 0;
	_maxLength = 0;
	_frontImageId = 1;
	_cusorNewPosition = _cusorOldPosition = _cursorPosition;
	resize (rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_MultiEdit::~ATOM_MultiEdit (void)
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::~ATOM_MultiEdit);
}

ATOM_WidgetType ATOM_MultiEdit::getType (void) const
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::getType);

	return WT_MULTIEDIT;
}

void ATOM_MultiEdit::setEditType(int type)
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::setEditType);

	_editType = type;
}

int ATOM_MultiEdit::getEditType() const
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::getEditType);

	return _editType;
}

void ATOM_MultiEdit::setFont (ATOM_GUIFont::handle font)
{
	ATOM_Widget::setFont (font);

	_textDirty = true;
}

void ATOM_MultiEdit::setWString (const wchar_t *str)
{
	_string = str ? str : L"";
	moveCursor(getStringLength());

	_textDirty = true;

	ATOM_EditTextChangedEvent event(getId(), "");
	getString (event.text);
	_parent->handleEvent(&event);
}

void ATOM_MultiEdit::setString(const char *str)
{
	if (!str)
	{
		setWString (NULL);
	}
	else
	{
		ATOM_WSTRING wstr = ATOM_Ansi2Wide (str, strlen(str));
		setWString (wstr.c_str());
	}
}

void ATOM_MultiEdit::getString (ATOM_STRING& str)
{
	str = ATOM_Wide2Ansi (_string.c_str(), _string.length());
}

const ATOM_WSTRING &ATOM_MultiEdit::getWString (void) const
{
	return _string;
}

// ������ʾ������
void ATOM_MultiEdit::calcTextLayout (void)
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::calcTextLayout);

	_lineList.clear();
	_cursorPosition.x = _offsetX;
	_cursorPosition.y = 0;

	size_t alllen = wcslen(_string.c_str());

	// ���û������
	if(alllen <= 0)
	{
		// ���¼���canvas��С
		setScrollValue(ATOM_Point2Di(0,0));
		setCanvasSize (_widgetRect.size);
		return;
	}

	// ���ֶ��س�����
	const wchar_t* wstart = _string.c_str();
	const wchar_t* wend = wstart + alllen;
	const wchar_t* wtemp = NULL;
	ATOM_VECTOR<ATOM_STRING> stringList;
	size_t cursorLen = 0;
	size_t cursorLine = 0;
	bool cursorChecked = false;
	const wchar_t* wcursor = _string.c_str()+_cursor;

	while(wstart < wend)
	{
		stringList.resize(stringList.size()+1);

		// ����
		wtemp = wcsstr(wstart, L"\n");
		if(NULL == wtemp)
		{
			wtemp = wend;
		}

		// ֻ��һ�������ַ�
		if(*wstart == L'\n')
		{
			if(!cursorChecked && wcursor >= wstart && wcursor <= wtemp)
			{
				cursorLine = stringList.size()-1;
				cursorLen = 0;
				cursorChecked = true;
			}
			++wstart;
			continue;
		}

		ATOM_STRING& s = stringList.back();
		// ת���ַ���
		if(!cursorChecked && wcursor >= wstart && wcursor <= wtemp)
		{
			// ��Ҫ�����
			if(wcursor == wstart)
			{
				// ����������ʼλ��
				cursorLen = 0;
				s = ATOM_Wide2Ansi (wstart, wtemp - wstart);
			}
			else if(wcursor == wtemp)
			{
				// ��������ĩβ
				s = ATOM_Wide2Ansi (wstart, wtemp - wstart);
				cursorLen = strlen(s.c_str());
			}
			else
			{
				// �������λ��
				s = ATOM_Wide2Ansi (wstart, wcursor - wstart);
				cursorLen = strlen(s.c_str());
				s += ATOM_Wide2Ansi (wcursor, wtemp-wcursor);
			}
			cursorLine = stringList.size()-1;
			cursorChecked = true;
		}
		else
		{
			s = ATOM_Wide2Ansi (wstart, wtemp - wstart);
		}
		wstart = (*wtemp == '\n') ? wtemp+1 : wtemp;
	}

	// ���һ��
	if(*(wend-1) == L'\n')
	{
		stringList.resize(stringList.size()+1);
	}

	if(false == cursorChecked)
	{
		cursorLine = stringList.size()-1;
		cursorLen = strlen(stringList[cursorLine].c_str());
		cursorChecked = true;
	}

	// �Զ�����
	int widthMax = getClientRect().size.w-_offsetX*2;
	float scrollValueY = getScrollValue().y;
	ME_LineInfo lineInfo;
	int l = 0, t = 0, w = 0, h = 0;
	ATOM_FontHandle font = ATOM_GUIFont::getFontHandle (getFont());
	int charmargin = ATOM_GUIFont::getFontCharMargin (getFont()); 
	ATOM_SetCharMargin (charmargin);
	const char *testStr = "gf";
	int tl = 0, tt = 0, tw = 0, th = 0;
	ATOM_CalcStringBounds (font, testStr, strlen(testStr), &tl, &tt, &tw, &th);
	cursorChecked = false;
	for(size_t i=0; i<stringList.size(); ++i)
	{
		ATOM_STRING& s = stringList[i];
		const char* startString = s.c_str();
		size_t len = strlen(startString);
		const char* endString = startString+len;
		if(0 == len)
		{
			if(!cursorChecked && cursorLine == i)
			{
				_cursorPosition.x = _offsetX;
				_cursorPosition.y = getLineHeight()*_lineList.size();
				cursorChecked = true;
			}

			lineInfo._text = NULL;
			lineInfo._textPosition.x = _offsetX;
			lineInfo._textPosition.y = getLineHeight()*_lineList.size();
			_lineList.push_back(lineInfo);
			continue;
		}

		unsigned num = ATOM_ClipString(font, startString, widthMax, &l, &t, &w, &h);
		while(startString < endString && num)
		{
			// �����������һЩ�ַ���
			if( !ATOM_CalcStringBounds(font, startString, num, &l, &t, &w, &h))
			{
				l = t = w = h = 0;
			}

			std::string st(startString, num);
			
			lineInfo._text = ATOM_HARDREF(ATOM_Text) ();
			lineInfo._text->setFont (font);
			lineInfo._text->setCharMargin (charmargin);
			lineInfo._text->setZValue (1.f);
			lineInfo._text->setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
			lineInfo._text->setString (st.c_str());
			lineInfo._textPosition.x = _offsetX+l;
			lineInfo._textPosition.y = getLineHeight()*_lineList.size() + (getLineHeight()-th)/2 - tt;
			lineInfo._lineWidth = w;
			if(startString+num < endString)
				lineInfo._nextline = false;
			else
				lineInfo._nextline = true;
			// �������Ƿ������������
			if(!cursorChecked && cursorLine == i)
			{
				if(s.c_str() + cursorLen >= startString && s.c_str() + cursorLen <= startString + num)
				{
					if (ATOM_CalcStringBounds (font, startString, cursorLen-(startString-s.c_str()), &l, &t, &w, &h) )
					{
						_cursorPosition.x = _offsetX + l + w;
					}
					else
					{
						_cursorPosition.x = _offsetX;
					}
					_cursorPosition.y = getLineHeight()*_lineList.size();
					cursorChecked = true;
				}
			}

			_lineList.push_back(lineInfo);
			startString += num;

			// ��������������
			if(startString >= endString)
			{
				break;
			}

			num = ATOM_ClipString(font, startString, widthMax, &l, &t, &w, &h);
		}
	}

	if(false == cursorChecked)
	{
		_cursorPosition.x = _offsetX + l + w;
		_cursorPosition.y = getLineHeight()*(_lineList.size()-1);
		cursorChecked = true;
	}

	// ���ݹ��λ������scrollValue
	ATOM_Point2Di scrollValue = getScrollValue();
	if(_cursorPosition.y < scrollValue.y)
	{
		scrollValue.y = _cursorPosition.y;
		setScrollValue(scrollValue);
	}
	else if(_cursorPosition.y + getLineHeight() > scrollValue.y + _widgetRect.size.h )
	{
		scrollValue.y = _cursorPosition.y + getLineHeight() - _widgetRect.size.h;
		setScrollValue(scrollValue);
	}

	const ATOM_Point2Di &scroll = getScrollValue ();

	// ���¼���canvas��С
	ATOM_Size2Di canvas(0, 0);
	if(!_lineList.empty())
	{
		canvas.h = _lineList.back()._textPosition.y + getLineHeight();
	}
	canvas.w = ATOM_max2(_widgetRect.size.w+scroll.x, canvas.w);
	canvas.h = ATOM_max2(_widgetRect.size.h+scroll.y, canvas.h);
	setCanvasSize (canvas);
}

void ATOM_MultiEdit::insertText(const char* str)
{
	if (!str)
	{
		return;
	}

	size_t strLen = strlen(str);

	if(_maxLength > 0)
	{
		ATOM_STRING s = ATOM_Wide2Ansi (_string.c_str(), _string.length());
		if(strlen(s.c_str())+strLen > _maxLength)
		{
			return;
		}
	}

	// ת��Ϊ���ֽ�
	ATOM_WSTRING wstr = ATOM_Ansi2Wide (str, strLen);
	size_t wstrLen = wcslen(wstr.c_str());

	// ����
	_string.insert(_cursor, wstr.c_str());
	_cursor += wstrLen;

	_textDirty = true;

	ATOM_EditTextChangedEvent event(getId(), "");
	getString (event.text);
	_parent->handleEvent(&event);
	_cusorOldPosition = _cusorNewPosition;
}

void ATOM_MultiEdit::backSpace()
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::backSpace);

	if(_cursor <= 0)
	{
		return;
	}

	_cursor -= 1;
	_string.erase(_cursor, 1);

	_textDirty = true;

	ATOM_EditTextChangedEvent event(getId(), "");
	getString (event.text);
	_parent->handleEvent(&event);
}

void ATOM_MultiEdit::deleteChar()
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::deleteChar);

	if(_cursor >= getStringLength())
	{
		return;
	}

	_string.erase(_cursor, 1);

	_textDirty = true;

	ATOM_EditTextChangedEvent event(getId(), "");
	getString (event.text);
	_parent->handleEvent(&event);
}

void ATOM_MultiEdit::moveCursor(int offset)
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::moveCursor);

	_cursor += offset;
	if(_cursor<0)
	{
		_cursor = 0;
	}
	size_t length = getStringLength();
	if(_cursor > length)
	{
		_cursor = length;
	}

	_textDirty = true;
}

void ATOM_MultiEdit::moveCursor(int x, int y)
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::moveCursor);

	ATOM_Point2Di scrollValue = getScrollValue();
	x -= _offsetX;
	//y += scrollValue.y;
	int cursor = 0;
	int line = y/getLineHeight();
	if(line < 0)
	{
		line = 0;
	}

	if(line >= _lineList.size())
	{
		cursor = _string.size();
		line = _lineList.size()-1;
	}
	else
	{
		ME_LineInfo* lineInfo;
		for(int i=0; i<line; ++i)
		{
			lineInfo = &_lineList[i];
			if(lineInfo->_text.get())
			{
				const char* str = lineInfo->_text->getString();
				size_t strLen = strlen(str);
				ATOM_WSTRING wstr = ATOM_Ansi2Wide(str, strLen);
				if(lineInfo->_nextline)
					cursor += wstr.length()+1;
				else
					cursor += wstr.length();
			}
			else
			{
				cursor += 1;
			}
		}
		lineInfo = &_lineList[line];
		if(lineInfo->_text.get())
		{
			ATOM_FontHandle fh = ATOM_GUIFont::getFontHandle (getFont());
			int charmargin = ATOM_GUIFont::getFontCharMargin (getFont());
			ATOM_SetCharMargin (charmargin);
			ATOM_StringClipInfo info;
			ATOM_ClipStringEx (fh, lineInfo->_text->getString(), x, &info);
			int clip = (info.clipRatio < 0.5f) ? info.clippedBefore : info.clippedAfter;
			if(clip > 0)
			{
				ATOM_WSTRING wstr = ATOM_Ansi2Wide(lineInfo->_text->getString(), clip);
				cursor += wstr.length();
			}
		}
		else
		{
			//cursor += 1;
		}
	}
	setCursor(cursor);
}

void ATOM_MultiEdit::moveCursorUp()
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::moveCursorUp);

	int x = _cursorPosition.x;
	int y = _cursorPosition.y - getLineHeight()/2;
	if(y < 0)
	{
		y = 0;
	}
	moveCursor(x, y);
}

void ATOM_MultiEdit::moveCursorDown()
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::moveCursorDown);

	int x = _cursorPosition.x;
	int y = _cursorPosition.y + getLineHeight()*3/2;
	moveCursor(x, y);
}

void ATOM_MultiEdit::setCursor(int cursor)
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::setCursor);

	_cursor = cursor;
	if(_cursor<0)
	{
		_cursor = 0;
	}
	size_t length = getStringLength();
	if(_cursor > length)
	{
		_cursor = length;
	}

	_textDirty = true;
}


int ATOM_MultiEdit::getStringLength()
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::getStringLength);

	return wcslen(_string.c_str());
}

//void ATOM_MultiEdit::setCursorPosition (int pos)
//{
//	if (_cursor != pos)
//	{
//		_cursor = pos;
//		_textDirty = true;
//	}
//}

int ATOM_MultiEdit::getCursorPosition (void) const
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::getCursorPosition);

	return _cursor;
}

// ����ƫ��
void ATOM_MultiEdit::setOffsetX(int x)
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::setOffsetX);

	_offsetX = x;
}

// ȡ��ƫ��
int ATOM_MultiEdit::getOffsetX() const
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::getOffsetX);

	return _offsetX;
}

// ���ù��ͼƬ
void ATOM_MultiEdit::setCursorImageId(int imageId)
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::setCursorImageId);

	_cursorImageId = imageId;
}

// ȡ�ù��ͼƬ
int ATOM_MultiEdit::getCursorImageId() const
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::getCursorImageId);

	return _cursorImageId;
}

// ���ù��ƫ��
void ATOM_MultiEdit::setCursorOffset(int offset)
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::setCursorOffset);

	_cursorOffset = offset;
}

// ȡ�ù��ƫ��
int ATOM_MultiEdit::getCursorOffset() const
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::getCursorOffset);

	return _cursorOffset;
}

//! ���ù����
void ATOM_MultiEdit::setCursorWidth(int width)
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::setCursorWidth);

	_cursorWidth = width;
}

//! ȡ�ù����
int ATOM_MultiEdit::getCursorWidth() const
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::getCursorWidth);

	return _cursorWidth;
}

//! ������������
void ATOM_MultiEdit::setMaxLength(int maxLength)
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::setMaxLength);

	_maxLength = maxLength;
}

//! ȡ����������
int ATOM_MultiEdit::getMaxLength() const
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::getMaxLength);

	return _maxLength;
}


//! ����һ�еĸ߶�
void ATOM_MultiEdit::setLineHeight(int lineHeight)
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::setLineHeight);

	_lineHeight = lineHeight;
}

//! ȡ��һ�еĸ߶�
int ATOM_MultiEdit::getLineHeight() const
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::getLineHeight);

	return _lineHeight;
}

void ATOM_MultiEdit::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::onPaint);

	ATOM_GUICanvas* canvas = event->canvas;
	ATOM_WidgetState state = event->state;

	if (_textDirty)
	{
		calcTextLayout ();
		_textDirty = false;
	}

	callParentHandler (event);
	int endline = _cursorPosition.y/getLineHeight();
	int startline = _cusorOldPosition.y/getLineHeight();
	if(endline < startline)
	{
		endline = startline;
		startline = _cursorPosition.y/getLineHeight();
	}
	bool  bDrawFrontImageId = 0;
	if(_cusorNewPosition != _cusorOldPosition)
	{
		bDrawFrontImageId = true;
		ATOM_GUIImage *image = getValidImage (_frontImageId);
		ATOM_ASSERT(image);
		if(startline == endline)
		{
			if(_cursorPosition.x - _cusorOldPosition.x > 0)
				image->draw (event->state, event->canvas, ATOM_Rect2Di(_cusorOldPosition.x, _cursorPosition.y, _cursorPosition.x - _cusorOldPosition.x, _lineHeight));
			else
				image->draw (event->state, event->canvas, ATOM_Rect2Di( _cursorPosition.x , _cursorPosition.y, _cusorOldPosition.x - _cursorPosition.x, _lineHeight));
			bDrawFrontImageId = false;
		}
		if(bDrawFrontImageId)
		{
			if(_cursorPosition.y < 	_cusorOldPosition.y)
			{
				_frontPosition = _cursorPosition;
				_backPosition = _cusorOldPosition;
			}
			else
			{
				_frontPosition = _cusorOldPosition;
				_backPosition = _cursorPosition;
			}
		}
		
	}
	// render text
	for(size_t i=0,j = 0; i<_lineList.size(); ++i,++j)
	{
		ME_LineInfo& info = _lineList[i];
		if(info._text.get())
		{
			const char *text = info._text->getString();
			if (strcmp (text, ""))
			{
				//����ǰ��
				if(bDrawFrontImageId)
				{
					ATOM_GUIImage *image = getValidImage (_frontImageId);
					ATOM_ASSERT(image);
					if(i >= startline && i <= endline)
					{
						if(i == startline)
							image->draw (event->state, event->canvas, ATOM_Rect2Di(_frontPosition.x, _frontPosition.y, info._lineWidth-_frontPosition.x, _lineHeight));
						else if(i == endline)
							image->draw (event->state, event->canvas, ATOM_Rect2Di(info._textPosition.x, endline*_lineHeight, _backPosition.x-info._textPosition.x,_lineHeight));
						else
							image->draw (event->state, event->canvas, ATOM_Rect2Di(info._textPosition.x, i*_lineHeight, info._lineWidth,_lineHeight));
					}
				}
				canvas->drawTextOutline (info._text.get(), info._textPosition.x, info._textPosition.y, getFontColor(),getFontOutLineColor());

			}
		}
	}

	// render cursor
	if(0 == (MULTIEDITTYPE_READONLY & _editType))
	{	// ֻ����ʱ�򣬲���ʾ���
		if (_cursorImageId != ATOM_INVALID_IMAGEID)
		{
			if((getRenderer()->getFocus() == this) && (ATOM_APP->getFrameStamp().currentTick % 1000 < 500))
			{
				ATOM_GUIImage *image = getValidImage (_cursorImageId);
				if (!image)
				{
					image = ATOM_GUIImageList::getDefaultImageList().getImage (ATOM_IMAGEID_WINDOW_BKGROUND);
					ATOM_ASSERT(image);
				}

				image->draw (event->state, event->canvas, ATOM_Rect2Di(_cursorPosition.x, _cursorPosition.y, _cursorWidth, _lineHeight));
			}
		}
	}

}

void ATOM_MultiEdit::onChar(ATOM_WidgetCharEvent *event)
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::onChar);

	// ֻ�����������������
	if(MULTIEDITTYPE_READONLY & _editType)
	{
		return;
	}
	bool bSelect = false;
	unsigned short mbcc = event->mbcc;
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
	//�˸��
	if (mbcc == 8)
	{
		if(!bSelect)
			backSpace();
		return;
	}

	//if (mbcc == '\\')
	//{
	//	return;
	//}


	//����
	char *tc = reinterpret_cast<char*>(&mbcc);
	char tc2[3];
	ZeroMemory(tc2, sizeof(tc2));
	memcpy(tc2, tc, 2);

	insertText(tc2);
}

// ���ù�굽��ʼλ��
void ATOM_MultiEdit::moveCursorStart()
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::moveCursorStart);

	moveCursor(-getStringLength());
}

// ���ù�굽����λ��
void ATOM_MultiEdit::moveCursorEnd()
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::moveCursorEnd);

	moveCursor(getStringLength());
}

void ATOM_MultiEdit::onKeyDown(ATOM_WidgetKeyDownEvent *event)
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::onKeyDown);

	// ֻ�����������������
	if(MULTIEDITTYPE_READONLY & _editType)
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

	case KEY_UP:
		moveCursorUp();
		break;

	case KEY_DOWN:
		moveCursorDown();
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

	case KEY_RETURN:
		{
			insertText("\n");
			ATOM_EditSysKeyEvent keyevent(getId(), event->key);
			_parent->handleEvent(&keyevent);
		}
		break;

	case KEY_ESCAPE:
		{
			ATOM_EditSysKeyEvent keyevent(getId(), event->key);
			_parent->handleEvent(&keyevent);
		}
		break;

	case KEY_v:
		if((event->keymod & KEYMOD_CTRL) != 0)
		{
			ATOM_STRING s;
			if(getClipBoardText(s))
			{
				ATOM_STRING str = ATOM_Wide2Ansi(_string.c_str(), _string.length());
				if(strlen(str.c_str())+s.size() > _maxLength)
				{
					int len = (_maxLength-strlen(str.c_str()));
					if(len>0)
						s = s.substr(0,len);
				}
				insertText(s.c_str());
			}
		}
		break;
	case KEY_a:
		if(GetAsyncKeyState(VK_LCONTROL)&0x8000  || GetAsyncKeyState(VK_RCONTROL)&0x8000 )
		{
			setCursor(getStringLength());
			_cusorOldPosition.x = _offsetX;
			_cusorOldPosition.y = 0;
			_cusorNewPosition = _cursorPosition;
		}
		break;
	case KEY_c:
		if(GetAsyncKeyState(VK_LCONTROL) &0x8000|| GetAsyncKeyState(VK_RCONTROL)&0x8000)
		{	
			size_t start = 0,end = 0;
			_selectString = getSelectString(start,end);
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
			size_t start = 0,end = 0;
			_selectString = getSelectString(start,end);
			if(_selectString.empty())
				return;
			deleteString(start,end);
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
			_selectString = "";
			_cusorNewPosition =_cusorOldPosition =  _cursorPosition;
		}
		break;
	}
}

void ATOM_MultiEdit::onKeyUp(ATOM_WidgetKeyUpEvent *event)
{
}

void ATOM_MultiEdit::onLButtonDown(ATOM_WidgetLButtonDownEvent *event)
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::onLButtonDown);

	ATOM_Point2Di scrollValue = getScrollValue();
	moveCursor(event->x, event->y+scrollValue.y);
	_oldCursor = _cursor;
	_cusorOldPosition = _cursorPosition;
	_cusorNewPosition = _cusorOldPosition;
}

bool ATOM_MultiEdit::getClipBoardText (ATOM_STRING& sText)
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::getClipBoardText);

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

	while(1)
	{
		ATOM_STRING::size_type n = sText.find("\r\n", 0);
		if(n >= 0 && n < sText.size())
		{
			sText.replace(n, 2, "\n");
		}
		else
		{
			break;
		}
	}

	return true;
}

bool ATOM_MultiEdit::supportIME (void) const
{
	return true;
}

unsigned ATOM_MultiEdit::getNumLines (void) const
{
	return _lineList.size();
}

void ATOM_MultiEdit::onMouseMove( ATOM_WidgetMouseMoveEvent *event )
{
	if( event->leftDown)
	{
		ATOM_Point2Di scrollValue = getScrollValue();
		moveCursor(event->x,event->y+scrollValue.y);
		_cusorNewPosition = _cursorPosition;
		_newCursor = _cursor;
	}
}

const ATOM_STRING ATOM_MultiEdit::getSelectString(size_t &start,size_t& end)
{
	if(_lineList.empty())
		return "";
	if(_cursorPosition != _cusorOldPosition)
	{
		int endline = _cursorPosition.y/getLineHeight();
		int startline = _cusorOldPosition.y/getLineHeight();
		if(endline < startline)
		{
			endline = startline;
			startline = _cursorPosition.y/getLineHeight();
		}
		size_t s,t = 0;
		ATOM_STRING str;
		start = end = 0;
		if(startline < 0 || endline < 0 ||startline >= _lineList.size() || endline >= _lineList.size())
			return "";
		if(startline == endline)
		{
			int x1 = _cusorOldPosition.x - _offsetX;
			int x2 = _cursorPosition.x - _offsetX;
			str = getSelectString(s,t,startline,x1,x2);
			int templength = 0;
			for(int i = 0; i < startline; ++i)
			{
				if(_lineList[i]._text.get())
					templength += strlen(_lineList[i]._text->getString());
			}
			start = templength + s;
			end = templength + t;
		}
		else
		{
			int x1 = _frontPosition.x - _offsetX;
			int x2 = _lineList[startline]._lineWidth;
			str = getSelectString(s,t,startline,x1,x2);
			int templength = 0;
			for(int i = 0; i < startline; ++i)
			{
				if(_lineList[i]._text.get())
					templength += strlen(_lineList[i]._text->getString());
			}
			start = templength + s;
			end = templength + t;
			for(int i = startline+1; i < endline; ++i)
			{
				if(_lineList[i]._text.get())
				{
					str += _lineList[i]._text->getString();
					end += strlen(_lineList[i]._text->getString());
				}

			}
			x1 = _lineList[endline]._textPosition.x;
			x2 = _backPosition.x;
			ATOM_STRING strEnd =  getSelectString(s,t,endline,x1,x2);
			str += strEnd;
			end += t-s;
		}
		return str;
	}
	return "";
}
ATOM_STRING ATOM_MultiEdit::getSelectString(size_t &start,size_t& end,int line,int x1,int x2)
{
	ATOM_STACK_TRACE(ATOM_MultiEdit::getSelectString);
	ATOM_STRING s;
	if(_cusorOldPosition == _cursorPosition)
		return s;

	if(x1<0 || x2 < 0)
	{
		return s;
	}
	
	if(line < 0 || line >= _lineList.size())
		return s; 
	if(_lineList[line]._text.get())
		s = _lineList[line]._text->getString();
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
			start = clip2;
			end = clip1;
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

void ATOM_MultiEdit::deleteString(size_t start,size_t end)
{
	// ֻ�����������������
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
	ATOM_EditTextChangedEvent event(getId(), "");
	getString (event.text);
	_parent->handleEvent(&event);
}

int ATOM_MultiEdit::getFrontImageId() const
{
	return _frontImageId;
}

void ATOM_MultiEdit::setFrontImageId( int val )
{
	_frontImageId = val;
}










