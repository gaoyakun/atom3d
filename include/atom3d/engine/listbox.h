/**	\file listbox.h
 *	�б����.
 *
 *	\author �״�ʹ
 *	\addtogroup engine
 *	@{
 */


#ifndef __ATOM3D_ENGINE_LISTBOX_H
#define __ATOM3D_ENGINE_LISTBOX_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"

#include "gui_font.h"
#include "widget.h"



enum
{
	LBCLICK_NULL,
	LBCLICK_LEFT,
	LBCLICK_RIGHT,
	LBDBLCLICK_LEFT
};

struct ATOM_ListItem
{
	ATOM_AUTOREF(ATOM_Text)	_text;
	unsigned long long _userData;
	ATOM_ColorARGB _color;
};

/// �б��
class ATOM_ENGINE_API ATOM_ListBox : public ATOM_Widget
{
public:
	ATOM_ListBox();
	ATOM_ListBox (ATOM_Widget *parent, const ATOM_Rect2Di &rect, int itemHeight, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);
	virtual ~ATOM_ListBox (void);

public:
	virtual ATOM_WidgetType getType (void) const;

public:
	/// �����
	virtual int addItem(const char* str, unsigned long long userData = 0);

	/// ������
	virtual int insertItem(int index, const char* str, unsigned long long userData = 0);

	/// ������
	virtual int insertItem(int index, const char* str, ATOM_ColorARGB _color, unsigned long long userData = 0);

	/// ɾ����
	virtual void removeItem(int index);

	/// �����
	virtual void clearItem();

	/// ѡ����
	virtual void selectItem(int index, int postEvent = LBCLICK_NULL);

	/// ȡ��ѡ����
	int getSelectIndex() const;

	/// ������߶�
	void setItemHeight(int height);

	/// ȡ����߶�
	int getItemHeight() const;

	/// ȡ���ж�����
	int getItemCount();

	/// ����ѡ��ͼƬ
	virtual void setSelectImageId(int id);

	virtual int getSelectImageId() const;

	virtual void setHoverImageId(int id);
	
	virtual int getHoverImageId() const;

	/// ��������
	virtual void setItemText(int index, const char* str);

	/// ȡ���û�����
	unsigned long long getItemData(int index);

	/// �����û�����
	virtual void setItemData(int index, unsigned long long userData);

	/// ȡ������
	ATOM_Text* getItemText (int index);

	/// ��������
	int findString (const char *str, bool caseSensitive) const;

	/// ȡ��ָ��λ���µ���
	int getItemByPosition (int x, int y) const;

	/// ���ù��
	void moveShowItemCursor(int offset);

	/// ����������ɫ
	void setItemColor (int index, ATOM_ColorARGB clr);

	/// ȡ��������ɫ
	ATOM_ColorARGB getItemColor(int index) const;

	/// ȥ������ռ� 
	void shrink (void);
	
	//! �ⲿ���뱳��ͼƬ��Ӧ����combobox����
	void setExtendedClientImage(ATOM_GUIImage * image);

	//! �ⲿ�������ͼƬ��Ӧ����combobox����
	void setExtendedHoverImage(ATOM_GUIImage * image);
	//! �ⲿ�����������Դ��Ӧ����combobox����
	void setExtendedScrollImage(ATOM_GUIImage * backImage,
	ATOM_GUIImage * sliderImage,ATOM_GUIImage * upButtonImage,ATOM_GUIImage * downButtonImage);
	//! �������ֺ���ƫ��
	void setOffset(int offsetX);

	//! ��ȡ���ֺ���ƫ��
	int getOffset()const;

public:
	void onLButtonDown (ATOM_WidgetLButtonDownEvent *event);
	void onLButtonUp (ATOM_WidgetLButtonUpEvent *event);
	void onRButtonDown (ATOM_WidgetLButtonDownEvent *event);
	void onRButtonUp (ATOM_WidgetLButtonUpEvent *event);
	void onKeyDown (ATOM_WidgetKeyDownEvent *event);
	void onMouseMove (ATOM_WidgetMouseMoveEvent *event);
	void onLButtonDblClick (ATOM_WidgetLButtonDblClickEvent *event);
	void onCommand (ATOM_WidgetCommandEvent *event);
	void onMouseEnter (ATOM_WidgetMouseEnterEvent *event);
	void onMouseLeave (ATOM_WidgetMouseLeaveEvent *event);
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onResize (ATOM_WidgetResizeEvent *event);

	ATOM_DECLARE_EVENT_MAP(ATOM_ListBox, ATOM_Widget)

protected:
	typedef ATOM_LIST<ATOM_ListItem> ItemVsl;
	ItemVsl _items;
	int _itemHeight;
	int _selectIndex;
	int _selectImageId;
	int _hoverIndex;
	int _hoverImageId;
	int _showItemCursor;
	ATOM_GUIImage * _ExtendClientImage;
	ATOM_GUIImage * _ExtendHoverImage;
	bool _isExHoverImage;
	int _offsetX;
};



#endif // __ATOM3D_ENGINE_LISTBOX_H
/*! @} */
