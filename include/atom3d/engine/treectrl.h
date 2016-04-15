/**	\file treectrl.h
 *	树控件类.
 *
 *	\author 白大使
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_TREECTRL_H
#define __ATOM3D_ENGINE_TREECTRL_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"

#include "gui_font.h"
#include "widget.h"



//! 树控件元素.
class ATOM_ENGINE_API ATOM_TreeItem: public ATOM_Widget
{
	friend class ATOM_TreeCtrl;

public:
	ATOM_TreeItem();
	//! 构造函数
	ATOM_TreeItem (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_TreeItem* root, ATOM_Widget::ShowState showState = ATOM_Widget::ShowNormal);

	//! 析构函数
	virtual ~ATOM_TreeItem (void);

public:
	//! 取得类型
	virtual ATOM_WidgetType getType (void) const;

	//! 显示
	virtual void show (ATOM_Widget::ShowState showState);

	virtual void setFont (ATOM_GUIFont::handle font);
	//! 选中
	void select(bool sel);

public:
	//! 设置文字
	void setText (const char *str);

	//! 取得文字
	ATOM_Text *getText (void) const;

	//! 设置描边颜色
	void setOutlineColor(ATOM_ColorARGB color);

	//! 取得描边颜色
	ATOM_ColorARGB getOutlineColor() const;

	//! 设置展开的时候的图片
	void setExpandImageId(int imageId);

	//! 取得展开时候图片
	int getExpandImageId() const;

	//! 设置未展开的时候的图片
	void setUnexpandImageId(int imageId);

	//! 取得未展开时候的图片
	int getUnexpandImageId() const;

	//! 设置图标的大小
	void setIconSize(int size);

	//! 取得图标的大小
	int getIconSize() const;

	//! 添加子节点
	void append(ATOM_TreeItem* item);

	//! 移除子节点
	void remove(ATOM_TreeItem* item);

	//! 展开
	void expand(bool val, bool postEvent = false);

	//! 是否展开
	bool getExpand() const;

	//! 取得跟节点
	ATOM_TreeItem* getRoot();

	//! 取得子节点
	ATOM_LIST<ATOM_TreeItem*>& getChildren();

	int getIndex (void) const;

	void changeRoot (ATOM_TreeItem *newRoot);

	void setOffset (int offset);

	int getOffset (void) const;
	void setIsAlawysShowClientImage(bool isAlawysShowClientImage);
protected:
	virtual void onClicked (void);
	virtual void onMClicked (void);
	virtual void onRClicked (void);

private:
	void calcTextLayout (void);

protected:
	bool _textDirty;
	ATOM_STRING _caption;
	ATOM_AUTOREF(ATOM_Text) _text;
	ATOM_Point2Di _textPosition;
//	ATOM_ColorARGB _outlineColor;

	ATOM_LIST<ATOM_TreeItem*> _children;
	ATOM_TreeItem* _root;
	bool _expand;
	//unsigned _lastClickTime;
	bool _isAlwaysShowClientImage;
	int _iconSize;	
	int _expandImageId;
	int _unexpandImageId;
	int _index;
	int _offset;
	bool _select;
	ATOM_LIST<ATOM_TreeItem*>::iterator _iter;

public:
	void onLButtonDown (ATOM_WidgetLButtonDownEvent *event);
	void onLButtonUp (ATOM_WidgetLButtonUpEvent *event);
	void onMButtonDown (ATOM_WidgetMButtonDownEvent *event);
	void onMButtonUp (ATOM_WidgetMButtonUpEvent *event);
	void onRButtonDown (ATOM_WidgetRButtonDownEvent *event);
	void onRButtonUp (ATOM_WidgetRButtonUpEvent *event);
	void onLeftDblClick (ATOM_WidgetLButtonDblClickEvent *event);
	void onMouseWheel (ATOM_WidgetMouseWheelEvent *event);
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onResize (ATOM_WidgetResizeEvent *event);
	void onDragStart (ATOM_WidgetDragStartEvent *event);
	void onDragOver (ATOM_WidgetDragOverEvent *event);
	void onDragDrop (ATOM_WidgetDragDropEvent *event);
	void onUnhandled (ATOM_Event *event);

	ATOM_DECLARE_EVENT_MAP(ATOM_TreeItem, ATOM_Widget)
};

//! 树控件
class ATOM_ENGINE_API ATOM_TreeCtrl: public ATOM_Widget
{
	friend class ATOM_TreeItem;

public:
	ATOM_TreeCtrl();
	//! 构造函数
	ATOM_TreeCtrl (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, int imageSize, int lineHeight, int indent, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);

	//! 析构函数
	virtual ~ATOM_TreeCtrl (void);

public:
	//! 取得类型
	virtual ATOM_WidgetType getType (void) const;

	//! 设置文字背景
	void setTextImageId(int imageId);

	//! 取得文字背景
	int getTextImageId(void) const;

	//! 取得图片大小
	int getImageSize(void) const;

	//! 取得行高度
	int getLineHeight(void) const;

	//! 取得缩进
	int getIndent (void) const;

	//! 设置描边颜色
	void setOutlineColor(ATOM_ColorARGB color);

	//! 取得描边颜色
	ATOM_ColorARGB getOutlineColor() const;

	//! 添加新项
	ATOM_TreeItem* newItem(const char* str, int expandImageId = ATOM_INVALID_IMAGEID, int unexpandImageId = ATOM_INVALID_IMAGEID, ATOM_TreeItem* root = NULL, int id = ATOM_Widget::AnyId, bool expend = true,bool isAlwaysShowClientImage=false);

	//! 删除项
	void deleteItem(ATOM_TreeItem* item);

	//! 清空所有项
	void clear();

	//! 选择
	void selectItem(int index, bool postEvent = true);

	//! 选择
	void selectItem(ATOM_TreeItem* item, bool postEvent = true);

	//! 取得选中
	ATOM_TreeItem* getSelectedItem();

	//!
	ATOM_TreeItem* getFirstChildItem (ATOM_TreeItem *item) const;

	//!
	ATOM_TreeItem* getPrevSiblingItem (ATOM_TreeItem *item) const;

	//!
	ATOM_TreeItem* getNextSiblingItem (ATOM_TreeItem *item) const;

	//
	ATOM_TreeItem *getItemByIndex (int index) const;

	//
	ATOM_TreeItem *getItemByUserData (unsigned long long userData) const;

	//
	void calcLayout (void);

	//
	void allowItemDragDrop (bool allow);

	bool isItemDragDropAllowed (void) const;

protected:
	// 删除项
	void doDeleteItem(ATOM_TreeItem* item);

	// 添加子节点
	void append(ATOM_TreeItem* item);

	// 移除子节点
	void remove(ATOM_TreeItem* item);

	ATOM_TreeItem *getItemByUserDataR (unsigned long long userData, ATOM_TreeItem *root) const;
	ATOM_TreeItem *getItemByIndexR (int index, ATOM_TreeItem *root) const;
	// 
	int getNextIndex (void);

	// 
	void calcItemLayout (ATOM_TreeItem* item, int& line);

public:
	void onClick(ATOM_TreeCtrlClickEvent *event);
	void onCommand (ATOM_WidgetCommandEvent *event);
	void onKeyDown(ATOM_WidgetKeyDownEvent *event);
	void onRClick(ATOM_TreeCtrlRClickEvent *event);
	void onMClick(ATOM_TreeCtrlMClickEvent *event);
	void onDblClick(ATOM_TreeCtrlDblClickEvent *event);
	void onItemDragStart(ATOM_TreeItemDragStartEvent *event);
	void onItemDragOver(ATOM_TreeItemDragOverEvent *event);
	void onItemDragDrop(ATOM_TreeItemDragDropEvent *event);

	ATOM_DECLARE_EVENT_MAP(ATOM_TreeCtrl, ATOM_Widget)

private:

protected:
//	ATOM_ColorARGB _outlineColor;
	ATOM_LIST<ATOM_TreeItem*> _children;
	int _imageSize;
	int _indent;
	int _lineHeight;
	int _textImageId;
	int _nextIndex;
	bool _allowItemDragDrop;
	ATOM_TreeItem* _selected;
};



#endif // __ATOM3D_ENGINE_TREECTRL_H
/*! @} */
