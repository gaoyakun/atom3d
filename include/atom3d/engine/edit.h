/**	\file edit.h
 *	�༭����.
 *
 *	\author �״�ʹ
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

//! �༭��
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
	//! ���ñ༭������
	void setEditType(int type);

	//! ȡ�ñ༭������
	int getEditType() const;

	//! �����ַ���
	void setString (const char *str);

	//! ȡ���ַ���
	void getString (ATOM_STRING& str) const;

	//! ȡ���ַ���
	ATOM_STRING getString (void) const;

	//! ����Ĭ��������ɫ
	void setDefaultFontColor (const ATOM_ColorARGB& clr);

	//! ȡ��Ĭ��������ɫ
	const ATOM_ColorARGB& getDefaultFontColor() const;

	//! ����Ĭ���ַ���
	void setDefaultString (const char *str);

	//! ȡ��Ĭ���ַ���
	void getDefaultString (ATOM_STRING& str);


	//! ȡ���ַ�������
	int getStringLength();

	//! ���ù��λ��
	//void setCursorPosition (int position);

	//! ȡ�ù��
	int getCursorPosition (void) const;	

	//! ����ƫ��
	void setOffsetX(int x);

	//! ȡ��ƫ��
	int getOffsetX() const;

	//! ���ù��ͼƬ
	void setCursorImageId(int imageId);

	//! ȡ�ù��ͼƬ
	int getCursorImageId() const;

	//! ���ù��ƫ��
	void setCursorOffset(int offset);

	//! ȡ�ù��ƫ��
	int getCursorOffset() const;

	//! ���ù����
	void setCursorWidth(int offset);

	//! ȡ�ù����
	int getCursorWidth() const;

	//! ��������
	void setMaxLength(int maxLength);

	//! ȡ����������
	int getMaxLength() const;

	//! ����һ���ִ�
	void insertText(const char* str);

	//! ɾ��������ڵ�ǰһ����
	void backSpace();

	//! ɾ������һ����
	void deleteChar();

	//! ɾ����Χ�ڵ�����
	void deleteString(size_t strart,size_t end);
	//! ƫ�ƹ��
	void moveCursor(int offset, bool select = false);

	//! ���ù��λ��
	void setCursor(int cursor, bool select = false);

	virtual void calcTextLayout (void);

	bool getClipBoardText(ATOM_STRING& s);

	//! ���������ɫ
	void setOutlineColor(ATOM_ColorARGB color);

	//! ȡ�������ɫ
	ATOM_ColorARGB getOutlineColor() const;

	//! ���ѡ��ͼƬ
	int getFrontImageId() const;

	//! ����ѡ��ͼƬ
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
	int _frontImageId;					//ǰ��ͼƬ

	ATOM_Point2Di _cusorOldPosition;
	ATOM_Point2Di _cusorNewPosition;
	bool _isEnterMouse;					//�Ƿ��������
	ATOM_STRING _selectString;
};



#endif // __ATOM3D_ENGINE_EDIT_H
/*! @} */
