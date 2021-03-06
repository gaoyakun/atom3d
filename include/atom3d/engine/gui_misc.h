#ifndef __ATOM3D_ENGINE_GUI_MISC_H
#define __ATOM3D_ENGINE_GUI_MISC_H

#if _MSC_VER > 1000
# pragma once
#endif

class ATOM_Widget;

enum ATOM_WidgetType
{
	WT_UNKNOWN = -1,
	WT_WIDGET,
	WT_TOPWINDOW,
	WT_DIALOG,
	WT_DESKTOP,
	WT_POPUPMENU,
	WT_EDIT,
	WT_BUTTON,
	WT_LABEL,
	WT_PROGRESSBAR,
	WT_RICHEDIT,
	WT_CELLDATA,
	WT_CELL,
	WT_REALTIMECTRL,
	WT_LISTBOX,
	WT_SCROLLBAR,
	WT_TREEITEM,
	WT_TREECTRL,
	WT_CHECKBOX,
	WT_CIRCLEPROGRESS,
	WT_SCROLLMAP,
	WT_LISTVIEW,
	WT_SLIDER,
	WT_PROPERTYLIST,
	WT_MARQUEE,
	WT_HYPERLINK,
	WT_MULTIEDIT,
	WT_COMBOBOX,
	WT_FLASHCTRL,
	WT_PANEL,
	WT_VIEWSTACK,
	WT_CURVEEDITOR,
	WT_LAST,
};

enum ATOM_WidgetState
{
	WST_UNKNOWN = 0xFFFFFFFF,
	WST_START = 0,
	WST_NORMAL = WST_START,
	WST_HOVER,
	WST_HOLD,
	WST_DISABLED,
	WST_COUNT
};

enum ATOM_WidgetPlacement
{
	WP_VERTICAL,
	WP_HORIZONTAL
};

enum ATOM_GUIHitPoint
{
  HitNone,
  HitFrame,
  HitClient,
  HitChild
};

struct ATOM_GUIHitTestResult
{
  ATOM_GUIHitPoint hitPoint;
  bool lug;
  ATOM_Widget *child;
  ATOM_Widget *dragSite;
};

struct ATOM_GUILayout
{
  ATOM_Rect2Di widgetRect; // in/out
  ATOM_Rect2Di titleBarRect; // out
  ATOM_Rect2Di clientRect; // out
};

#endif // __ATOM3D_ENGINE_GUI_MISC_H
