/**	\file treectrl.h
 *	���ؼ���.
 *
 *	\author �״�ʹ
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



//! ���ؼ�Ԫ��.
class ATOM_ENGINE_API ATOM_TreeItem: public ATOM_Widget
{
	friend class ATOM_TreeCtrl;

public:
	ATOM_TreeItem();
	//! ���캯��
	ATOM_TreeItem (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_TreeItem* root, ATOM_Widget::ShowState showState = ATOM_Widget::ShowNormal);

	//! ��������
	virtual ~ATOM_TreeItem (void);

public:
	//! ȡ������
	virtual ATOM_WidgetType getType (void) const;

	//! ��ʾ
	virtual void show (ATOM_Widget::ShowState showState);

	virtual void setFont (ATOM_GUIFont::handle font);
	//! ѡ��
	void select(bool sel);

public:
	//! ��������
	void setText (const char *str);

	//! ȡ������
	ATOM_Text *getText (void) const;

	//! ���������ɫ
	void setOutlineColor(ATOM_ColorARGB color);

	//! ȡ�������ɫ
	ATOM_ColorARGB getOutlineColor() const;

	//! ����չ����ʱ���ͼƬ
	void setExpandImageId(int imageId);

	//! ȡ��չ��ʱ��ͼƬ
	int getExpandImageId() const;

	//! ����δչ����ʱ���ͼƬ
	void setUnexpandImageId(int imageId);

	//! ȡ��δչ��ʱ���ͼƬ
	int getUnexpandImageId() const;

	//! ����ͼ��Ĵ�С
	void setIconSize(int size);

	//! ȡ��ͼ��Ĵ�С
	int getIconSize() const;

	//! ����ӽڵ�
	void append(ATOM_TreeItem* item);

	//! �Ƴ��ӽڵ�
	void remove(ATOM_TreeItem* item);

	//! չ��
	void expand(bool val, bool postEvent = false);

	//! �Ƿ�չ��
	bool getExpand() const;

	//! ȡ�ø��ڵ�
	ATOM_TreeItem* getRoot();

	//! ȡ���ӽڵ�
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

//! ���ؼ�
class ATOM_ENGINE_API ATOM_TreeCtrl: public ATOM_Widget
{
	friend class ATOM_TreeItem;

public:
	ATOM_TreeCtrl();
	//! ���캯��
	ATOM_TreeCtrl (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, int imageSize, int lineHeight, int indent, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);

	//! ��������
	virtual ~ATOM_TreeCtrl (void);

public:
	//! ȡ������
	virtual ATOM_WidgetType getType (void) const;

	//! �������ֱ���
	void setTextImageId(int imageId);

	//! ȡ�����ֱ���
	int getTextImageId(void) const;

	//! ȡ��ͼƬ��С
	int getImageSize(void) const;

	//! ȡ���и߶�
	int getLineHeight(void) const;

	//! ȡ������
	int getIndent (void) const;

	//! ���������ɫ
	void setOutlineColor(ATOM_ColorARGB color);

	//! ȡ�������ɫ
	ATOM_ColorARGB getOutlineColor() const;

	//! �������
	ATOM_TreeItem* newItem(const char* str, int expandImageId = ATOM_INVALID_IMAGEID, int unexpandImageId = ATOM_INVALID_IMAGEID, ATOM_TreeItem* root = NULL, int id = ATOM_Widget::AnyId, bool expend = true,bool isAlwaysShowClientImage=false);

	//! ɾ����
	void deleteItem(ATOM_TreeItem* item);

	//! ���������
	void clear();

	//! ѡ��
	void selectItem(int index, bool postEvent = true);

	//! ѡ��
	void selectItem(ATOM_TreeItem* item, bool postEvent = true);

	//! ȡ��ѡ��
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
	// ɾ����
	void doDeleteItem(ATOM_TreeItem* item);

	// ����ӽڵ�
	void append(ATOM_TreeItem* item);

	// �Ƴ��ӽڵ�
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
