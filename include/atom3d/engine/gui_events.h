/**	\file gui_events.h
 *	GUI事件类.
 *
 *	\author 高雅昆
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_GUI_EVENTS_H
#define __ATOM3D_ENGINE_GUI_EVENTS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"

#include "basedefs.h"
#include "gui_misc.h"
#include "widget.h"
#include "flashvalue.h"
#include "dragdrop.h"

class ATOM_GUICanvas;

class ATOM_WidgetEvent: public ATOM_Event
{
public:
	ATOM_WidgetEvent (void): id(0) {}
	ATOM_WidgetEvent (int id_): id(id_) {}
	int id;

	ATOM_DECLARE_EVENT(ATOM_WidgetEvent)
};

class ATOM_WidgetIdleEvent: public ATOM_WidgetEvent
{
public:
	ATOM_WidgetIdleEvent (void) {}
	ATOM_WidgetIdleEvent (int id_): ATOM_WidgetEvent(id_) {}

	ATOM_DECLARE_EVENT(ATOM_WidgetIdleEvent)
};

class ATOM_WidgetContextMenuEvent: public ATOM_WidgetEvent
{
public:
	ATOM_WidgetContextMenuEvent (void): item(-1), x(0), y(0) {}
	ATOM_WidgetContextMenuEvent (int id_, int item_, int x_, int y_): ATOM_WidgetEvent(id_), item(item_), x(x_), y(y_) {}
	int x;
	int y;
	int item;

	ATOM_DECLARE_EVENT(ATOM_WidgetContextMenuEvent)
};

class ATOM_WidgetCommandEvent: public ATOM_WidgetEvent
{
public:
	ATOM_WidgetCommandEvent (void) {}
	ATOM_WidgetCommandEvent (int id_): ATOM_WidgetEvent(id_) {}

	ATOM_DECLARE_EVENT(ATOM_WidgetCommandEvent)
};

class ATOM_MenuPopupEvent: public ATOM_WidgetEvent
{
public:
	ATOM_MenuPopupEvent (void) {}
	ATOM_MenuPopupEvent (int id_): ATOM_WidgetEvent(id_) {}

	ATOM_DECLARE_EVENT(ATOM_MenuPopupEvent)
};

class ATOM_WidgetRClickEvent: public ATOM_WidgetEvent
{
public:
	ATOM_WidgetRClickEvent (void): keymod(0) {}
	ATOM_WidgetRClickEvent (int id_, unsigned keymod_): ATOM_WidgetEvent(id_), keymod(keymod_) {}
	unsigned keymod;

	ATOM_DECLARE_EVENT(ATOM_WidgetRClickEvent)
};

class ATOM_WidgetDragEvent: public ATOM_WidgetEvent
{
public:
	ATOM_WidgetDragEvent (void): cx(0), cy(0) {}
	ATOM_WidgetDragEvent (int id_, int cx_, int cy_): ATOM_WidgetEvent(id_), cx(cx_), cy(cy_) {}
	int cx;
	int cy;

	ATOM_DECLARE_EVENT(ATOM_WidgetDragEvent)
};

class ATOM_WidgetResizeEvent: public ATOM_WidgetEvent
{
public:
	ATOM_WidgetResizeEvent (void): widgetRect(0,0,0,0) {}
	ATOM_WidgetResizeEvent (int id_, int x, int y, int w, int h): ATOM_WidgetEvent(id_), widgetRect(x,y,w,h) {}
	ATOM_WidgetResizeEvent (int id_, const ATOM_Rect2Di &rc): ATOM_WidgetEvent(id_), widgetRect(rc) {}

	ATOM_Rect2Di widgetRect;

	ATOM_DECLARE_EVENT(ATOM_WidgetResizeEvent)
};

class ATOM_WidgetSetFocusEvent: public ATOM_WidgetEvent
{
public:
	ATOM_WidgetSetFocusEvent (void) {}
	ATOM_WidgetSetFocusEvent (int id_): ATOM_WidgetEvent(id_) {}

	ATOM_DECLARE_EVENT(ATOM_WidgetSetFocusEvent)
};

class ATOM_WidgetLostFocusEvent: public ATOM_WidgetEvent
{
public:
	ATOM_WidgetLostFocusEvent (void) {}
	ATOM_WidgetLostFocusEvent (int id_): ATOM_WidgetEvent(id_) {}

	ATOM_DECLARE_EVENT(ATOM_WidgetLostFocusEvent)
};

class ATOM_WidgetMouseButtonEvent: public ATOM_WidgetEvent
{
public:
	ATOM_WidgetMouseButtonEvent (void): x(0), y(0), shiftState(0) {}
	ATOM_WidgetMouseButtonEvent (int id_, int x_, int y_, unsigned shiftState_): ATOM_WidgetEvent(id_), x(x_), y(y_), shiftState(shiftState_) {}

	int x;
	int y;
	unsigned shiftState;
};

class ATOM_WidgetLButtonDblClickEvent: public ATOM_WidgetMouseButtonEvent
{
public:
	ATOM_WidgetLButtonDblClickEvent (void) {}
	ATOM_WidgetLButtonDblClickEvent (int id_, int x_, int y_, unsigned shiftState_): ATOM_WidgetMouseButtonEvent(id_, x_, y_, shiftState_) {}

	ATOM_DECLARE_EVENT(ATOM_WidgetLButtonDblClickEvent)
};

class ATOM_WidgetMButtonDblClickEvent: public ATOM_WidgetMouseButtonEvent
{
public:
	ATOM_WidgetMButtonDblClickEvent (void) {}
	ATOM_WidgetMButtonDblClickEvent (int id_, int x_, int y_, unsigned shiftState_): ATOM_WidgetMouseButtonEvent(id_, x_, y_, shiftState_) {}

	ATOM_DECLARE_EVENT(ATOM_WidgetMButtonDblClickEvent)
};

class ATOM_WidgetRButtonDblClickEvent: public ATOM_WidgetMouseButtonEvent
{
public:
	ATOM_WidgetRButtonDblClickEvent (void) {}
	ATOM_WidgetRButtonDblClickEvent (int id_, int x_, int y_, unsigned shiftState_): ATOM_WidgetMouseButtonEvent(id_, x_, y_, shiftState_) {}

	ATOM_DECLARE_EVENT(ATOM_WidgetRButtonDblClickEvent)
};

class ATOM_WidgetLButtonDownEvent: public ATOM_WidgetMouseButtonEvent
{
public:
	ATOM_WidgetLButtonDownEvent (void) {}
	ATOM_WidgetLButtonDownEvent (int id_, int x_, int y_, unsigned shiftState_): ATOM_WidgetMouseButtonEvent(id_, x_, y_, shiftState_) {}

	ATOM_DECLARE_EVENT(ATOM_WidgetLButtonDownEvent)
};

class ATOM_WidgetLButtonUpEvent: public ATOM_WidgetMouseButtonEvent
{
public:
	ATOM_WidgetLButtonUpEvent (void) {}
	ATOM_WidgetLButtonUpEvent (int id_, int x_, int y_, unsigned shiftState_): ATOM_WidgetMouseButtonEvent(id_, x_, y_, shiftState_) {}

	ATOM_DECLARE_EVENT(ATOM_WidgetLButtonUpEvent)
};

class ATOM_WidgetMButtonDownEvent: public ATOM_WidgetMouseButtonEvent
{
public:
	ATOM_WidgetMButtonDownEvent (void) {}
	ATOM_WidgetMButtonDownEvent (int id_, int x_, int y_, unsigned shiftState_): ATOM_WidgetMouseButtonEvent(id_, x_, y_, shiftState_) {}

	ATOM_DECLARE_EVENT(ATOM_WidgetMButtonDownEvent)
};

class ATOM_WidgetMButtonUpEvent: public ATOM_WidgetMouseButtonEvent
{
public:
	ATOM_WidgetMButtonUpEvent (void) {}
	ATOM_WidgetMButtonUpEvent (int id_, int x_, int y_, unsigned shiftState_): ATOM_WidgetMouseButtonEvent(id_, x_, y_, shiftState_) {}

	ATOM_DECLARE_EVENT(ATOM_WidgetMButtonUpEvent)
};

class ATOM_WidgetRButtonDownEvent: public ATOM_WidgetMouseButtonEvent
{
public:
	ATOM_WidgetRButtonDownEvent (void) {}
	ATOM_WidgetRButtonDownEvent (int id_, int x_, int y_, unsigned shiftState_): ATOM_WidgetMouseButtonEvent(id_, x_, y_, shiftState_) {}

	ATOM_DECLARE_EVENT(ATOM_WidgetRButtonDownEvent)
};

class ATOM_WidgetRButtonUpEvent: public ATOM_WidgetMouseButtonEvent
{
public:
	ATOM_WidgetRButtonUpEvent (void) {}
	ATOM_WidgetRButtonUpEvent (int id_, int x_, int y_, unsigned shiftState_): ATOM_WidgetMouseButtonEvent(id_, x_, y_, shiftState_) {}

	ATOM_DECLARE_EVENT(ATOM_WidgetRButtonUpEvent)
};

class ATOM_WidgetMouseMoveEvent: public ATOM_WidgetEvent
{
public:
	ATOM_WidgetMouseMoveEvent (void): x(0), y(0), xrel(0), yrel(0), shiftState(0), leftDown(false), middleDown(false), rightDown(false) {}
	ATOM_WidgetMouseMoveEvent (int id_, int x_, int y_, int xrel_, int yrel_, bool leftDown_, bool middleDown_, bool rightDown_, unsigned shiftState_)
		: ATOM_WidgetEvent(id_)
		, x(x_)
		, y(y_)
		, xrel(xrel_)
		, yrel(yrel_)
		, leftDown(leftDown_)
		, middleDown(middleDown_)
		, rightDown(rightDown_)
		, shiftState(shiftState_) 
	{
	}

	int x;
	int y;
	int xrel;
	int yrel;
	bool leftDown;
	bool middleDown;
	bool rightDown;
	unsigned shiftState;

	ATOM_DECLARE_EVENT(ATOM_WidgetMouseMoveEvent)
};

class ATOM_WidgetMouseWheelEvent: public ATOM_WidgetEvent
{
public:
	ATOM_WidgetMouseWheelEvent (void): x(0), y(0), delta(0), shiftState(0) {}
	ATOM_WidgetMouseWheelEvent (int id_, int x_, int y_, int delta_, unsigned shiftState_): ATOM_WidgetEvent(id_), x(x_), y(y_), delta(delta_), shiftState(shiftState_) {}
	int x;
	int y;
	int delta;
	unsigned shiftState;

	ATOM_DECLARE_EVENT(ATOM_WidgetMouseWheelEvent)
};

class ATOM_WidgetMouseEnterEvent: public ATOM_WidgetEvent
{
public:
	ATOM_WidgetMouseEnterEvent (void) {}
	ATOM_WidgetMouseEnterEvent (int id_): ATOM_WidgetEvent(id_) {}

	ATOM_DECLARE_EVENT(ATOM_WidgetMouseEnterEvent)
};

class ATOM_WidgetMouseLeaveEvent: public ATOM_WidgetEvent
{
public:
	ATOM_WidgetMouseLeaveEvent (void) {}
	ATOM_WidgetMouseLeaveEvent (int id_): ATOM_WidgetEvent(id_) {}

	ATOM_DECLARE_EVENT(ATOM_WidgetMouseLeaveEvent)
};

class ATOM_WidgetHitTestEvent: public ATOM_Event
{
public:
	ATOM_WidgetHitTestEvent (void): x(0), y(0) {}
	ATOM_WidgetHitTestEvent (int x_, int y_): x(x_), y(y_) {}
	int x;
	int y;
	ATOM_GUIHitTestResult hitTestResult;

	ATOM_DECLARE_EVENT(ATOM_WidgetHitTestEvent)
};

class ATOM_WidgetKeyEvent: public ATOM_WidgetEvent
{
public:
	ATOM_WidgetKeyEvent (void): eat (true) {}
	ATOM_WidgetKeyEvent(int id_, bool eat_): ATOM_WidgetEvent(id_), eat(eat_) {}
	bool eat;
};

class ATOM_WidgetKeyDownEvent: public ATOM_WidgetKeyEvent
{
public:
	ATOM_WidgetKeyDownEvent(void): key(KEY_UNKNOWN), keymod(0) {}
	ATOM_WidgetKeyDownEvent(int id_, ATOM_Key key_, unsigned keymod_): ATOM_WidgetKeyEvent(id_, true), key(key_), keymod(keymod_) {}
	ATOM_Key key;
	unsigned keymod;

	ATOM_DECLARE_EVENT(ATOM_WidgetKeyDownEvent)
};

class ATOM_WidgetKeyUpEvent: public ATOM_WidgetKeyEvent
{
public:
	ATOM_WidgetKeyUpEvent(void): key(KEY_UNKNOWN), keymod(0) {}
	ATOM_WidgetKeyUpEvent(int id_, ATOM_Key key_, unsigned keymod_): ATOM_WidgetKeyEvent(id_, true), key(key_), keymod(keymod_) {}
	ATOM_Key key;
	unsigned keymod;

	ATOM_DECLARE_EVENT(ATOM_WidgetKeyUpEvent)
};

class ATOM_WidgetCharEvent: public ATOM_WidgetKeyEvent
{
public:
	ATOM_WidgetCharEvent(void): mbcc(0), unicc(0), codec(ATOM_CC_UNKNOWN) {}
	ATOM_WidgetCharEvent(int id_, unsigned short mbcc_, unsigned short unicc_, int codec_): ATOM_WidgetKeyEvent(id_, true), mbcc(mbcc_), unicc(unicc_), codec(codec_) {}
	unsigned short mbcc, unicc;
	int codec;

	ATOM_DECLARE_EVENT(ATOM_WidgetCharEvent)
};

class ATOM_WidgetDrawClientEvent: public ATOM_Event
{
public:
	ATOM_WidgetDrawClientEvent (void): canvas(0), state(WST_NORMAL) {}
	ATOM_WidgetDrawClientEvent (ATOM_GUICanvas *canvas_, ATOM_WidgetState state_): canvas(canvas_), state(state_) {}
	ATOM_GUICanvas *canvas;
	ATOM_WidgetState state;

	ATOM_DECLARE_EVENT(ATOM_WidgetDrawClientEvent)
};

class ATOM_WidgetDrawOverlayEvent: public ATOM_Event
{
public:
	ATOM_WidgetDrawOverlayEvent (void): canvas(0) {}
	ATOM_WidgetDrawOverlayEvent (ATOM_GUICanvas *canvas_): canvas(canvas_) {}
	ATOM_GUICanvas *canvas;

	ATOM_DECLARE_EVENT(ATOM_WidgetDrawOverlayEvent)
};

class ATOM_WidgetDrawFrameEvent: public ATOM_Event
{
public:
	ATOM_WidgetDrawFrameEvent (void): canvas(0), state(WST_NORMAL) {}
	ATOM_WidgetDrawFrameEvent (ATOM_GUICanvas *canvas_, ATOM_WidgetState state_): canvas(canvas_), state(state_) {}
	ATOM_GUICanvas *canvas;
	ATOM_WidgetState state;

	ATOM_DECLARE_EVENT(ATOM_WidgetDrawFrameEvent)
};

// Top window events
class ATOM_TopWindowCloseEvent: public ATOM_WidgetEvent
{
public:
	ATOM_TopWindowCloseEvent (void) {}
	ATOM_TopWindowCloseEvent (int id_): ATOM_WidgetEvent(id_) {}

	ATOM_DECLARE_EVENT(ATOM_TopWindowCloseEvent)
};

// ATOM_Cell events

class ATOM_CellLButtonDownEvent : public ATOM_WidgetEvent
{
public:
	ATOM_CellLButtonDownEvent (void): x(0), y(0) {}
	ATOM_CellLButtonDownEvent (int id_, int x_, int y_): ATOM_WidgetEvent(id_), x(x_), y(y_) {}
	int x;
	int y;

	ATOM_DECLARE_EVENT(ATOM_CellLButtonDownEvent)
};

class ATOM_CellLButtonUpEvent : public ATOM_WidgetEvent
{
public:
	ATOM_CellLButtonUpEvent (void): x(0), y(0), from(NULL), fromX(0), fromY(0) {}
	ATOM_CellLButtonUpEvent (int id_, int x_, int y_, ATOM_Widget* from_, int fromX_, int fromY_): ATOM_WidgetEvent(id_), x(x_), y(y_), from(from_), fromX(fromX_), fromY(fromY_) {}
	int x;
	int y;
	ATOM_Widget* from;
	int fromX;
	int fromY;

	ATOM_DECLARE_EVENT(ATOM_CellLButtonUpEvent)
};

class ATOM_CellLeftClickEvent: public ATOM_WidgetEvent
{
public:
	ATOM_CellLeftClickEvent (void): x(0), y(0) {}
	ATOM_CellLeftClickEvent (int id_, int x_, int y_): ATOM_WidgetEvent(id_), x(x_), y(y_) {}
	int x;
	int y;

	ATOM_DECLARE_EVENT(ATOM_CellLeftClickEvent)
};

class ATOM_CellDblClickEvent: public ATOM_WidgetEvent
{
public:
	ATOM_CellDblClickEvent (void): x(0), y(0) {}
	ATOM_CellDblClickEvent (int id_, int x_, int y_): ATOM_WidgetEvent(id_), x(x_), y(y_) {}
	int x;
	int y;

	ATOM_DECLARE_EVENT(ATOM_CellDblClickEvent)
};

class ATOM_CellRButtonDownEvent : public ATOM_WidgetEvent
{
public:
	ATOM_CellRButtonDownEvent (void): x(0), y(0) {}
	ATOM_CellRButtonDownEvent (int id_, int x_, int y_): ATOM_WidgetEvent(id_), x(x_), y(y_) {}
	int x;
	int y;

	ATOM_DECLARE_EVENT(ATOM_CellRButtonDownEvent)
};

class ATOM_CellRButtonUpEvent : public ATOM_WidgetEvent
{
public:
	ATOM_CellRButtonUpEvent (void): x(0), y(0), from(NULL), fromX(0), fromY(0) {}
	ATOM_CellRButtonUpEvent (int id_, int x_, int y_, ATOM_Widget* from_, int fromX_, int fromY_): ATOM_WidgetEvent(id_), x(x_), y(y_), from(from_), fromX(fromX_), fromY(fromY_) {}
	int x;
	int y;
	ATOM_Widget* from;
	int fromX;
	int fromY;

	ATOM_DECLARE_EVENT(ATOM_CellRButtonUpEvent)
};

class ATOM_CellRightClickEvent: public ATOM_WidgetEvent
{
public:
	ATOM_CellRightClickEvent (void): x(0), y(0) {}
	ATOM_CellRightClickEvent (int id_, int x_, int y_): ATOM_WidgetEvent(id_), x(x_), y(y_) {}
	int x;
	int y;

	ATOM_DECLARE_EVENT(ATOM_CellRightClickEvent)
};

class ATOM_CellMouseEnterEvent: public ATOM_WidgetEvent
{
public:
	ATOM_CellMouseEnterEvent (void): x(0), y(0) {}
	ATOM_CellMouseEnterEvent (int id_, int x_, int y_): ATOM_WidgetEvent(id_), x(x_), y(y_) {}
	int x;
	int y;

	ATOM_DECLARE_EVENT(ATOM_CellMouseEnterEvent)
};

class ATOM_CellMouseLeaveEvent: public ATOM_WidgetEvent
{
public:
	ATOM_CellMouseLeaveEvent (void): x(0), y(0) {}
	ATOM_CellMouseLeaveEvent (int id_, int x_, int y_): ATOM_WidgetEvent(id_), x(x_), y(y_) {}
	int x;
	int y;

	ATOM_DECLARE_EVENT(ATOM_CellMouseLeaveEvent)
};

class ATOM_CellDragEvent: public ATOM_WidgetEvent
{
public:
	ATOM_CellDragEvent (void):x(0), y(0) {}
	ATOM_CellDragEvent (int id_, ATOM_DragSource *dragSource_, int x_, int y_): ATOM_WidgetEvent(id_), dragSource(dragSource_), x(x_), y(y_) {}

	int x;
	int y;
	ATOM_AUTOPTR(ATOM_DragSource) dragSource;

	ATOM_DECLARE_EVENT(ATOM_CellDragEvent)
};

class ATOM_CellDragStartEvent: public ATOM_CellDragEvent
{
public:
	ATOM_CellDragStartEvent (void) {}
	ATOM_CellDragStartEvent (int id_, ATOM_DragSource *dragSource_, int x_, int y_, bool allow_): ATOM_CellDragEvent(id_, dragSource_, x_, y_), allow(allow_) {}

	bool allow;

	ATOM_DECLARE_EVENT(ATOM_CellDragStartEvent)
};

class ATOM_CellDragOverEvent: public ATOM_CellDragEvent
{
public:
	ATOM_CellDragOverEvent (void) {}
	ATOM_CellDragOverEvent (int id_, ATOM_DragSource *dragSource_, int x_, int y_, bool accept_): ATOM_CellDragEvent(id_, dragSource_, x_, y_), accept(accept_) {}

	bool accept;

	ATOM_DECLARE_EVENT(ATOM_CellDragOverEvent)
};

class ATOM_CellDragDropEvent: public ATOM_CellDragEvent
{
public:
	ATOM_CellDragDropEvent (void) {}
	ATOM_CellDragDropEvent (int id_, ATOM_DragSource *dragSource_, int x_, int y_): ATOM_CellDragEvent(id_, dragSource_, x_, y_) {}

	ATOM_DECLARE_EVENT(ATOM_CellDragDropEvent)
};

// listctrl evnets

class ATOM_ListBoxHoverItemChangedEvent: public ATOM_WidgetEvent
{
public:
	ATOM_ListBoxHoverItemChangedEvent (void): oldIndex(-1), newIndex(-1) {}
	ATOM_ListBoxHoverItemChangedEvent (int id_, int oldIndex_, int newIndex_): ATOM_WidgetEvent(id_), oldIndex(oldIndex_), newIndex(newIndex_) {}
	int oldIndex;
	int newIndex;

	ATOM_DECLARE_EVENT(ATOM_ListBoxHoverItemChangedEvent)
};

class ATOM_ListBoxClickEvent: public ATOM_WidgetEvent
{
public:
	ATOM_ListBoxClickEvent (void): index(0), type(0) {}
	ATOM_ListBoxClickEvent (int id_, int index_, int type_): ATOM_WidgetEvent(id_), index(index_), type(type_) {}
	int index;
	int type;

	ATOM_DECLARE_EVENT(ATOM_ListBoxClickEvent)
};

class ATOM_ListBoxDblClickEvent: public ATOM_WidgetEvent
{
public:
	ATOM_ListBoxDblClickEvent (void): index(0), type(0) {}
	ATOM_ListBoxDblClickEvent (int id_, int index_, int type_): ATOM_WidgetEvent(id_), index(index_), type(type_) {}
	int index;
	int type;

	ATOM_DECLARE_EVENT(ATOM_ListBoxDblClickEvent)
};

class ATOM_ScrollEvent: public ATOM_WidgetEvent
{
public:
	ATOM_ScrollEvent (void): newPos(0), oldPos(0) {}
	ATOM_ScrollEvent (int id_, int newPos_, int oldPos_): ATOM_WidgetEvent(id_), newPos(newPos_), oldPos(oldPos_) {}
	int newPos;
	int oldPos;

	ATOM_DECLARE_EVENT(ATOM_ScrollEvent)
};

// edit events

class ATOM_EditSysKeyEvent: public ATOM_WidgetEvent
{
public:
	ATOM_EditSysKeyEvent (void): key(KEY_UNKNOWN) {}
	ATOM_EditSysKeyEvent (int id_, ATOM_Key key_): ATOM_WidgetEvent(id_), key(key_) {}
	ATOM_Key key;

	ATOM_DECLARE_EVENT(ATOM_EditSysKeyEvent)
};

class ATOM_EditTextChangedEvent : public ATOM_WidgetEvent
{
public:
	ATOM_EditTextChangedEvent (void) {}
	ATOM_EditTextChangedEvent (int id_, const char *text_): ATOM_WidgetEvent(id_), text(text_) {}

	ATOM_STRING text;

	ATOM_DECLARE_EVENT(ATOM_EditTextChangedEvent)
};

// treectrl events
class ATOM_TreeCtrl;
class ATOM_TreeItem;

class ATOM_TreeCtrlSelChangedEvent : public ATOM_WidgetEvent
{
public:
	ATOM_TreeCtrlSelChangedEvent (void): selectedIndex(-1), deselectedIndex(-1) {}
	ATOM_TreeCtrlSelChangedEvent (int id_, int selected_, int deselected_): ATOM_WidgetEvent(id_), selectedIndex(selected_), deselectedIndex(deselected_) {}
	int selectedIndex;
	int deselectedIndex;

	ATOM_DECLARE_EVENT(ATOM_TreeCtrlSelChangedEvent)
};

class ATOM_TreeCtrlClickEvent : public ATOM_WidgetEvent
{
public:
	ATOM_TreeCtrlClickEvent (void): itemIndex(-1) {}
	ATOM_TreeCtrlClickEvent (int id_, int item_): ATOM_WidgetEvent(id_), itemIndex(item_) {}
	int itemIndex;

	ATOM_DECLARE_EVENT(ATOM_TreeCtrlClickEvent)
};

class ATOM_TreeCtrlDblClickEvent : public ATOM_WidgetEvent
{
public:
	ATOM_TreeCtrlDblClickEvent (void): itemIndex(-1) {}
	ATOM_TreeCtrlDblClickEvent (int id_, int item_): ATOM_WidgetEvent(id_), itemIndex(item_) {}
	int itemIndex;

	ATOM_DECLARE_EVENT(ATOM_TreeCtrlDblClickEvent)
};

class ATOM_TreeCtrlMClickEvent : public ATOM_WidgetEvent
{
public:
	ATOM_TreeCtrlMClickEvent (void): itemIndex(-1) {}
	ATOM_TreeCtrlMClickEvent (int id_, int item_): ATOM_WidgetEvent(id_), itemIndex(item_) {}
	int itemIndex;

	ATOM_DECLARE_EVENT(ATOM_TreeCtrlMClickEvent)
};

class ATOM_TreeCtrlRClickEvent : public ATOM_WidgetEvent
{
public:
	ATOM_TreeCtrlRClickEvent (void): itemIndex(-1) {}
	ATOM_TreeCtrlRClickEvent (int id_, int item_): ATOM_WidgetEvent(id_), itemIndex(item_) {}
	int itemIndex;

	ATOM_DECLARE_EVENT(ATOM_TreeCtrlRClickEvent)
};

class ATOM_TreeItemDragEvent: public ATOM_WidgetEvent
{
public:
	ATOM_TreeItemDragEvent (void):index(-1) {}
	ATOM_TreeItemDragEvent (int id_, ATOM_DragSource *dragSource_, int index_): ATOM_WidgetEvent(id_), dragSource(dragSource_), index(index_) {}

	int index;
	ATOM_AUTOPTR(ATOM_DragSource) dragSource;

	ATOM_DECLARE_EVENT(ATOM_TreeItemDragEvent)
};

class ATOM_TreeItemDragStartEvent: public ATOM_TreeItemDragEvent
{
public:
	ATOM_TreeItemDragStartEvent (void) {}
	ATOM_TreeItemDragStartEvent (int id_, ATOM_DragSource *dragSource_, int index_, bool allow_): ATOM_TreeItemDragEvent(id_, dragSource_, index_), allow(allow_) {}

	bool allow;

	ATOM_DECLARE_EVENT(ATOM_TreeItemDragStartEvent)
};

class ATOM_TreeItemDragOverEvent: public ATOM_TreeItemDragEvent
{
public:
	ATOM_TreeItemDragOverEvent (void) {}
	ATOM_TreeItemDragOverEvent (int id_, ATOM_DragSource *dragSource_, int index_, bool accept_): ATOM_TreeItemDragEvent(id_, dragSource_, index_), accept(accept_) {}

	bool accept;

	ATOM_DECLARE_EVENT(ATOM_TreeItemDragOverEvent)
};

class ATOM_TreeItemDragDropEvent: public ATOM_TreeItemDragEvent
{
public:
	ATOM_TreeItemDragDropEvent (void): keymod(0) {}
	ATOM_TreeItemDragDropEvent (int id_, ATOM_DragSource *dragSource_, int index_, int keymod_): ATOM_TreeItemDragEvent(id_, dragSource_, index_), keymod(keymod_) {}

	int keymod;

	ATOM_DECLARE_EVENT(ATOM_TreeItemDragDropEvent)
};

// combobox events

class ATOM_ComboBoxListShowEvent : public ATOM_WidgetEvent
{
public:
	ATOM_ComboBoxListShowEvent (void) {}
	ATOM_ComboBoxListShowEvent (int id_): ATOM_WidgetEvent(id_) {}	

	ATOM_DECLARE_EVENT(ATOM_ComboBoxListShowEvent)
};

class ATOM_ComboBoxSelectionChangedEvent : public ATOM_WidgetEvent
{
public:
	ATOM_ComboBoxSelectionChangedEvent (void) {}
	ATOM_ComboBoxSelectionChangedEvent (int id_, int index_): ATOM_WidgetEvent(id_), index(index_) {}

	int index;

	ATOM_DECLARE_EVENT(ATOM_ComboBoxSelectionChangedEvent)
};

// listview
class ATOM_ListViewSelectEvent : public ATOM_WidgetEvent
{
public:
	ATOM_ListViewSelectEvent (void) {}
	ATOM_ListViewSelectEvent (int id_, int index_): ATOM_WidgetEvent(id_), index(index_) {}	

	int index;
	ATOM_DECLARE_EVENT(ATOM_ListViewSelectEvent)
};

//! listView 鼠标移动进入某行的事件
class ATOM_ListViewMouseEnterRowEvent : public ATOM_WidgetEvent
{
public:
	ATOM_ListViewMouseEnterRowEvent(void):oldRowIndex(-1),newRowIndex(-1){}
	ATOM_ListViewMouseEnterRowEvent(int id_,int oldIndex_,int newIndex_)
		:ATOM_WidgetEvent(id_),oldRowIndex(oldIndex_),newRowIndex(newIndex_){}

	int oldRowIndex;
	int newRowIndex;
	
	ATOM_DECLARE_EVENT(ATOM_ListViewMouseEnterRowEvent);
};
// animator events

class ATOM_WidgetAnimatorStartEvent: public ATOM_Event
{
public:
	ATOM_DECLARE_EVENT(ATOM_WidgetAnimatorStartEvent)
};

class ATOM_WidgetAnimatorStopEvent: public ATOM_Event
{
public:
	ATOM_DECLARE_EVENT(ATOM_WidgetAnimatorStopEvent)
};

class ATOM_WidgetAnimatorUpdateEvent: public ATOM_Event
{
public:
	ATOM_WidgetAnimatorUpdateEvent (void): done(false) {}
	bool done;

	ATOM_DECLARE_EVENT(ATOM_WidgetAnimatorUpdateEvent)
};

class ATOM_DialogInitEvent: public ATOM_WidgetEvent
{
public:
	ATOM_DialogInitEvent (void) {}
	ATOM_DialogInitEvent (int id_): ATOM_WidgetEvent(id_) {}

	ATOM_DECLARE_EVENT(ATOM_DialogInitEvent)
};

class ATOM_CurveChangedEvent: public ATOM_WidgetEvent
{
public:
	ATOM_CurveChangedEvent (void) {}
	ATOM_CurveChangedEvent (int id_): ATOM_WidgetEvent(id_) {}

	ATOM_DECLARE_EVENT(ATOM_CurveChangedEvent)
};

// richedit events
class ATOM_RichEditCommandEvent: public ATOM_WidgetEvent
{
public:
	ATOM_RichEditCommandEvent (void): commandId(0) {}
	ATOM_RichEditCommandEvent (int id_, int commandId_): ATOM_WidgetEvent(id_), commandId(commandId_) {}

	int commandId;

	ATOM_DECLARE_EVENT(ATOM_RichEditCommandEvent)
};

// flash control events
class ATOM_FlashFSCommandEvent: public ATOM_WidgetEvent
{
public:
	ATOM_FlashFSCommandEvent (void) {}
	ATOM_FlashFSCommandEvent (int id_, const wchar_t *command_, const wchar_t *args_): ATOM_WidgetEvent(id_), command(command_), args(args_) {}

	ATOM_WSTRING command;
	ATOM_WSTRING args;

	ATOM_DECLARE_EVENT(ATOM_FlashFSCommandEvent)
};

class ATOM_FlashCallEvent: public ATOM_WidgetEvent
{
public:
	ATOM_FlashCallEvent (void) {}
	ATOM_FlashCallEvent (int id_, const char *request_, const ATOM_FlashArguments &args_): ATOM_WidgetEvent(id_), request(request_), args(args_) {}

	ATOM_STRING request;
	ATOM_FlashArguments args;
	ATOM_FlashValue returnVal;

	ATOM_DECLARE_EVENT(ATOM_FlashCallEvent)
};

// ViewStack events
class ATOM_ViewStackChangedEvent: public ATOM_WidgetEvent
{
public:
	ATOM_ViewStackChangedEvent (void): newPageId(-1), oldPageId(-1) {}
	ATOM_ViewStackChangedEvent (int id_, int newPageId_, int oldPageId_): ATOM_WidgetEvent(id_), newPageId(newPageId_), oldPageId(oldPageId_) {}

	int newPageId;
	int oldPageId;

	ATOM_DECLARE_EVENT(ATOM_ViewStackChangedEvent)
};

// Drag&Drop events
class ATOM_WidgetDragStartEvent: public ATOM_WidgetEvent
{
public:
	ATOM_WidgetDragStartEvent (void): x(0), y(0), allow(true) {}
	ATOM_WidgetDragStartEvent (int id_, int x_, int y_, bool allow_, ATOM_DragSource *dragSource_): ATOM_WidgetEvent(id_), x(x_), y(y_), allow(allow_), dragSource(dragSource_) {}

	int x;
	int y;
	bool allow;
	ATOM_AUTOPTR(ATOM_DragSource) dragSource;

	ATOM_DECLARE_EVENT(ATOM_WidgetDragStartEvent)
};

class ATOM_WidgetDragOverEvent: public ATOM_WidgetEvent
{
public:
	ATOM_WidgetDragOverEvent (void): x(0), y(0), accept(false) {}
	ATOM_WidgetDragOverEvent (int id_, int x_, int y_, bool accept_, ATOM_DragSource *dragSource_): ATOM_WidgetEvent(id_), x(x_), y(y_), accept(accept_), dragSource(dragSource_) {}

	int x;
	int y;
	bool accept;
	ATOM_AUTOPTR(ATOM_DragSource) dragSource;

	ATOM_DECLARE_EVENT(ATOM_WidgetDragOverEvent)
};

class ATOM_WidgetDragDropEvent: public ATOM_WidgetEvent
{
public:
	ATOM_WidgetDragDropEvent (void): x(0), y(0), keymod(0) {}
	ATOM_WidgetDragDropEvent (int id_, int x_, int y_, unsigned keymod_, ATOM_DragSource *dragSource_)
		: ATOM_WidgetEvent(id_)
		, x(x_)
		, y(y_)
		, keymod(keymod_)
		, dragSource(dragSource_) {}

	int x;
	int y;
	unsigned keymod;
	ATOM_AUTOPTR(ATOM_DragSource) dragSource;

	ATOM_DECLARE_EVENT(ATOM_WidgetDragDropEvent)
};

#endif // __ATOM3D_ENGINE_GUI_EVENTS_H
/*! @} */
