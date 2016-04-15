/**	\file multiedit.h
 *	多行编辑器类.
 *
 *	\author 白大使
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_MULTIEDIT_H
#define __ATOM3D_ENGINE_MULTIEDIT_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_render.h"

#include "gui_font.h"
#include "widget.h"



enum MULTIEDITTYPE
{	
	MULTIEDITTYPE_READONLY		= (1<<0),
};

struct ME_LineInfo
{
	ATOM_AUTOREF(ATOM_Text) _text;
	ATOM_Point2Di _textPosition;
	UINT16		_lineWidth;
	bool		_nextline;//!<是否是手动换行
};

//! 多行编辑器框
class ATOM_ENGINE_API ATOM_MultiEdit: public ATOM_Widget
{
public:
	ATOM_MultiEdit();
	ATOM_MultiEdit (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState = ATOM_Widget::Hide);
	virtual ~ATOM_MultiEdit (void);

public:
	virtual ATOM_WidgetType getType (void) const;
	virtual void setFont (ATOM_GUIFont::handle font);

public:
	//! 设置编辑器类型
	void setEditType(int type);

	//! 取得编辑器类型
	int getEditType() const;

	//! 设置字符串
	void setString (const char *str);

	//! 设置宽字符串
	void setWString (const wchar_t *str);

	//! 取得字符串
	void getString (ATOM_STRING& str);

	//! 取得宽字符串
	const ATOM_WSTRING &getWString (void) const;

	//! 取得字符串长度
	int getStringLength();

	//! 设置光标位置
	//void setCursorPosition (int position);

	//! 取得光标
	int getCursorPosition (void) const;	

	//! 设置偏移
	void setOffsetX(int x);

	//! 取得偏移
	int getOffsetX() const;

	//! 设置光标图片
	void setCursorImageId(int imageId);

	//! 取得光标图片
	int getCursorImageId() const;

	//! 设置光标偏移
	void setCursorOffset(int offset);

	//! 取得光标偏移
	int getCursorOffset() const;

	//! 设置光标宽度
	void setCursorWidth(int offset);

	//! 取得光标宽度
	int getCursorWidth() const;

	//! 字数限制
	void setMaxLength(int maxLength);

	//! 取得字数限制
	int getMaxLength() const;

	//! 设置一行的高度
	void setLineHeight(int lineHeight);

	//! 取得一行的高度
	int getLineHeight() const;

	//! 设置光标到开始位置
	void moveCursorStart();

	//! 设置光标到结束位置 
	void moveCursorEnd();

	//! 移动光标
	void moveCursor(int offset);

	//! 取得行数
	unsigned getNumLines (void) const;

	void insertText(const char* str);


	//! 获得选中图片
	int getFrontImageId() const;

	//! 设置选中图片
	void setFrontImageId(int val);
protected:
	void backSpace();
	void deleteChar();
	void moveCursor(int x, int y);
	void moveCursorUp();
	void moveCursorDown();
	void setCursor(int cursor);
	const ATOM_STRING getSelectString(size_t &start,size_t& end);
	ATOM_STRING getSelectString(size_t &start,size_t& end,int line,int x1,int x2);
	void deleteString(size_t start,size_t end);
	virtual void calcTextLayout (void);
	virtual bool supportIME (void) const;
	bool getClipBoardText (ATOM_STRING& sText);

public:
	void onChar (ATOM_WidgetCharEvent *event);
	void onKeyDown (ATOM_WidgetKeyDownEvent *event);
	void onKeyUp (ATOM_WidgetKeyUpEvent *event);
	void onLButtonDown(ATOM_WidgetLButtonDownEvent *event);
	void onMouseMove(ATOM_WidgetMouseMoveEvent *event);
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onReturn();

	ATOM_DECLARE_EVENT_MAP(ATOM_MultiEdit, ATOM_Widget)

protected:
	ATOM_WSTRING _string;
	bool _textDirty;

	int _startPosition;

	int _cursor;
	int _cursorImageId;
	int _cursorOffset;
	int _cursorWidth;
	ATOM_Point2Di _cursorPosition;

	int _offsetX;

	int _editType;
	int _maxLength;

	int _lineHeight;

	ATOM_VECTOR<ME_LineInfo> _lineList;
	int _frontImageId;					//前景图片
	ATOM_Point2Di _cusorNewPosition;
	ATOM_Point2Di _cusorOldPosition;
	ATOM_Point2Di _frontPosition;
	ATOM_Point2Di _backPosition;
	int _oldCursor;
	int _newCursor;
	ATOM_STRING _selectString;
};



#endif // __ATOM3D_ENGINE_MULTIEDIT_H
/*! @} */
