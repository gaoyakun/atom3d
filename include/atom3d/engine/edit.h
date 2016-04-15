/**	\file edit.h
 *	编辑器类.
 *
 *	\author 白大使
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_EDIT_H
#define __ATOM3D_ENGINE_EDIT_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_render.h"

#include "gui_font.h"
#include "widget.h"



enum EDITTYPE
{	
	EDITTYPE_INTEGER	= (1<<0),
	EDITTYPE_PASSWORD	= (1<<1),
	EDITTYPE_READONLY	= (1<<2),
	EDITTYPE_NUMBER		= (1<<3),
};

//! 编辑框
class ATOM_ENGINE_API ATOM_Edit: public ATOM_Widget
{
public:
	ATOM_Edit();
	ATOM_Edit (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState = ATOM_Widget::Hide);
	virtual ~ATOM_Edit (void);

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

	//! 取得字符串
	void getString (ATOM_STRING& str) const;

	//! 取得字符串
	ATOM_STRING getString (void) const;

	//! 设置默认字体颜色
	void setDefaultFontColor (const ATOM_ColorARGB& clr);

	//! 取得默认字体颜色
	const ATOM_ColorARGB& getDefaultFontColor() const;

	//! 设置默认字符串
	void setDefaultString (const char *str);

	//! 取得默认字符串
	void getDefaultString (ATOM_STRING& str);


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

	//! 插入一个字串
	void insertText(const char* str);

	//! 删除光标所在的前一个字
	void backSpace();

	//! 删除光标后一个字
	void deleteChar();

	//! 删除范围内的字体
	void deleteString(size_t strart,size_t end);
	//! 偏移光标
	void moveCursor(int offset, bool select = false);

	//! 设置光标位置
	void setCursor(int cursor, bool select = false);

	virtual void calcTextLayout (void);

	bool getClipBoardText(ATOM_STRING& s);

	//! 设置描边颜色
	void setOutlineColor(ATOM_ColorARGB color);

	//! 取得描边颜色
	ATOM_ColorARGB getOutlineColor() const;

	//! 获得选中图片
	int getFrontImageId() const;

	//! 设置选中图片
	void setFrontImageId(int val);
public:
	void onChar (ATOM_WidgetCharEvent *event);
	void onKeyDown (ATOM_WidgetKeyDownEvent *event);
	void onKeyUp (ATOM_WidgetKeyUpEvent *event);
	void onLButtonDown(ATOM_WidgetLButtonDownEvent *event);
	void onLButtonUp(ATOM_WidgetLButtonUpEvent *event);
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onResize (ATOM_WidgetResizeEvent *event);
	void onMouseMove(ATOM_WidgetMouseMoveEvent *event);
	void onMouseLeave(ATOM_WidgetMouseLeaveEvent * event);
	void onReturn();

protected:
	virtual bool supportIME (void) const;
	virtual void pasteFromClipBoard();
	virtual ATOM_STRING getSelectString(size_t &start,size_t& end);
	ATOM_DECLARE_EVENT_MAP(ATOM_Edit, ATOM_Widget)

protected:
	//ATOM_ColorARGB _outlineColor;
	ATOM_WSTRING _string;
	ATOM_AUTOREF(ATOM_Text) _text;
	ATOM_Point2Di _textPosition;
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

	int _selectStart;

	ATOM_AUTOREF(ATOM_Text) _textDefault;
	ATOM_Point2Di _textDefaultPosition;
	ATOM_ColorARGB _fontDefualtColor;
	int _frontImageId;					//前景图片

	ATOM_Point2Di _cusorOldPosition;
	ATOM_Point2Di _cusorNewPosition;
	bool _isEnterMouse;					//是否按下了鼠标
	ATOM_STRING _selectString;
};



#endif // __ATOM3D_ENGINE_EDIT_H
/*! @} */
