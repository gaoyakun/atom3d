/**	\file listbox.h
 *	列表框类.
 *
 *	\author 白大使
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

/// 列表框
class ATOM_ENGINE_API ATOM_ListBox : public ATOM_Widget
{
public:
	ATOM_ListBox();
	ATOM_ListBox (ATOM_Widget *parent, const ATOM_Rect2Di &rect, int itemHeight, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);
	virtual ~ATOM_ListBox (void);

public:
	virtual ATOM_WidgetType getType (void) const;

public:
	/// 添加项
	virtual int addItem(const char* str, unsigned long long userData = 0);

	/// 插入项
	virtual int insertItem(int index, const char* str, unsigned long long userData = 0);

	/// 插入项
	virtual int insertItem(int index, const char* str, ATOM_ColorARGB _color, unsigned long long userData = 0);

	/// 删除项
	virtual void removeItem(int index);

	/// 清空项
	virtual void clearItem();

	/// 选择项
	virtual void selectItem(int index, int postEvent = LBCLICK_NULL);

	/// 取得选中项
	int getSelectIndex() const;

	/// 设置项高度
	void setItemHeight(int height);

	/// 取得项高度
	int getItemHeight() const;

	/// 取得有多少项
	int getItemCount();

	/// 设置选中图片
	virtual void setSelectImageId(int id);

	virtual int getSelectImageId() const;

	virtual void setHoverImageId(int id);
	
	virtual int getHoverImageId() const;

	/// 设置文字
	virtual void setItemText(int index, const char* str);

	/// 取得用户数据
	unsigned long long getItemData(int index);

	/// 设置用户数据
	virtual void setItemData(int index, unsigned long long userData);

	/// 取得文字
	ATOM_Text* getItemText (int index);

	/// 查找文字
	int findString (const char *str, bool caseSensitive) const;

	/// 取得指定位置下的项
	int getItemByPosition (int x, int y) const;

	/// 设置光标
	void moveShowItemCursor(int offset);

	/// 设置字体颜色
	void setItemColor (int index, ATOM_ColorARGB clr);

	/// 取得字体颜色
	ATOM_ColorARGB getItemColor(int index) const;

	/// 去除冗余空间 
	void shrink (void);
	
	//! 外部传入背景图片，应仅被combobox调用
	void setExtendedClientImage(ATOM_GUIImage * image);

	//! 外部传入高亮图片，应仅被combobox调用
	void setExtendedHoverImage(ATOM_GUIImage * image);
	//! 外部传入滚动条资源，应仅被combobox调用
	void setExtendedScrollImage(ATOM_GUIImage * backImage,
	ATOM_GUIImage * sliderImage,ATOM_GUIImage * upButtonImage,ATOM_GUIImage * downButtonImage);
	//! 设置文字横向偏移
	void setOffset(int offsetX);

	//! 获取文字横向偏移
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
