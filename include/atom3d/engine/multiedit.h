/**	\file multiedit.h
 *	���б༭����.
 *
 *	\author �״�ʹ
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
	bool		_nextline;//!<�Ƿ����ֶ�����
};

//! ���б༭����
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
	//! ���ñ༭������
	void setEditType(int type);

	//! ȡ�ñ༭������
	int getEditType() const;

	//! �����ַ���
	void setString (const char *str);

	//! ���ÿ��ַ���
	void setWString (const wchar_t *str);

	//! ȡ���ַ���
	void getString (ATOM_STRING& str);

	//! ȡ�ÿ��ַ���
	const ATOM_WSTRING &getWString (void) const;

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

	//! ����һ�еĸ߶�
	void setLineHeight(int lineHeight);

	//! ȡ��һ�еĸ߶�
	int getLineHeight() const;

	//! ���ù�굽��ʼλ��
	void moveCursorStart();

	//! ���ù�굽����λ�� 
	void moveCursorEnd();

	//! �ƶ����
	void moveCursor(int offset);

	//! ȡ������
	unsigned getNumLines (void) const;

	void insertText(const char* str);


	//! ���ѡ��ͼƬ
	int getFrontImageId() const;

	//! ����ѡ��ͼƬ
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
	int _frontImageId;					//ǰ��ͼƬ
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
