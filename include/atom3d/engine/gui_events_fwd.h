#ifndef __ATOM3D_ENGINE_GUI_EVENTS_FWD_H
#define __ATOM3D_ENGINE_GUI_EVENTS_FWD_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

class ATOM_WidgetIdleEvent;

class ATOM_WidgetCommandEvent;

class ATOM_WidgetRClickEvent;

class ATOM_WidgetDragEvent;

class ATOM_WidgetResizeEvent;

class ATOM_WidgetSetFocusEvent;

class ATOM_WidgetLostFocusEvent;

class ATOM_WidgetMouseButtonEvent;

class ATOM_WidgetLButtonDownEvent;

class ATOM_WidgetLButtonUpEvent;

class ATOM_WidgetMButtonDownEvent;

class ATOM_WidgetMButtonUpEvent;

class ATOM_WidgetRButtonDownEvent;

class ATOM_WidgetRButtonUpEvent;

class ATOM_WidgetMouseMoveEvent;

class ATOM_WidgetMouseWheelEvent;

class ATOM_WidgetMouseEnterEvent;

class ATOM_WidgetMouseLeaveEvent;

class ATOM_WidgetHitTestEvent;

class ATOM_WidgetKeyEvent;

class ATOM_WidgetKeyDownEvent;

class ATOM_WidgetKeyUpEvent;

class ATOM_WidgetCharEvent;

class ATOM_WidgetDrawClientEvent;

class ATOM_WidgetDrawFrameEvent;

// Top window events
class ATOM_TopWindowCloseEvent;

// ATOM_Cell evnets

class ATOM_CellLButtonDownEvent;

class ATOM_CellLButtonUpEvent;

class ATOM_CellLeftClickEvent;

class ATOM_CellRButtonDownEvent;

class ATOM_CellRButtonUpEvent;

class ATOM_CellRightClickEvent;

class ATOM_CellMouseEnterEvent;

class ATOM_CellMouseLeaveEvent;

// listctrl evnets

class ATOM_ListBoxClickEvent;

class ATOM_ScrollEvent;

// edit events

class ATOM_EditSysKeyEvent;

class ATOM_EditTextChangedEvent;

// treectrl events

class ATOM_TreeCtrlClickEvent;

// combobox events

class ATOM_ComboBoxListShowEvent;
// listView events
class ATOM_ListViewMouseEnterRowEvent;

// animator events

class ATOM_WidgetAnimatorStartEvent;

class ATOM_WidgetAnimatorStopEvent;

class ATOM_WidgetAnimatorUpdateEvent;

class ATOM_DialogInitEvent;


#endif // __ATOM3D_ENGINE_GUI_EVENTS_FWD_H
