/**	\file menu.h
 *	弹出菜单类.
 *
 *	\author 高雅坤
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_MENU_H
#define __ATOM3D_ENGINE_MENU_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_render.h"

#include "topwindow.h"
#include "gui_font.h"

class ATOM_PopupMenu;
class ATOM_GUIRenderer;

struct ATOM_MenuItem
{
	ATOM_STRING title;
	ATOM_AUTOREF(ATOM_Text) text;
	int id;
	int imageId;
	int hilightImageId;
	bool checked;
	ATOM_PopupMenu *submenu;
};

struct MenuLayout
{
  ATOM_VECTOR<ATOM_Rect2Di> itemRects;
  ATOM_VECTOR<int> actualWidth;
  ATOM_VECTOR<ATOM_Point2Di> textPositions;
};

class ATOM_ENGINE_API ATOM_PopupMenu: public ATOM_Widget
{
public:
	ATOM_PopupMenu(int id = ATOM_Widget::AnyId);
	ATOM_PopupMenu (ATOM_GUIRenderer *renderer, int id = ATOM_Widget::AnyId);
	virtual ~ATOM_PopupMenu (void);

	virtual void setFont (ATOM_GUIFont::handle font);

	static const unsigned invalid_index = 0xFFFFFFFF;

public:
  unsigned appendMenuItem (const char *text, int id);
  bool insertMenuItem (unsigned index, const char *text, int id);
  void removeMenuItem (unsigned index);
  void removeMenuItemById (int id);
  void clear (void);
  unsigned getNumItems (void) const;
  const ATOM_MenuItem & getMenuItem (unsigned index) const;
  ATOM_PopupMenu *createSubMenu (unsigned item);
  void deleteSubMenu (unsigned item);
  void setText (unsigned index, const char *text);
  const char *getText (unsigned index) const;
  void setCheck (unsigned index, bool check);
  void setCheckById (int id, bool check);
  bool getCheck (unsigned index) const;
  bool getCheckById (int id) const;
  void setSelected (int index);
  unsigned getSelected (void) const;
  void setSelectColor (ATOM_ColorARGB clr);
  ATOM_ColorARGB getSelectColor() const;
  void setItemImageId (unsigned index, int id);
  int getItemImageId (unsigned index) const;
  void setHilightItemImageId (unsigned index, int id);
  int getHilightItemImageId (unsigned index) const;
  void setDesiredItemSize (const ATOM_Size2Di &size);
  const ATOM_Size2Di &getDesiredItemSize (void) const;
  void setCheckedFlagSize (const ATOM_Size2Di &size);
  const ATOM_Size2Di &getCheckedFlagSize (void) const;
  void setCheckImageId (int id);
  int getCheckImageId (void) const;
  void enableDrawText (bool enable);
  bool isDrawTextEnabled (void) const;
  void setVerticalMargin (int margin);
  int getVerticalMargin (void) const;
  void setHorizontalMargin (int margin);
  int getHorizontalMargin (void) const;
  void setTrackWidget (ATOM_Widget *widget);
  ATOM_Widget *getTrackWidget (void) const;

  virtual void calcLayout (const ATOM_Rect2Di &rc);
  virtual ATOM_Point2Di calcSubMenuPosition (unsigned item) const;
  virtual void collapse (void);
  virtual void expand (unsigned item);

protected:
  unsigned _selected;
  int _id;
  ATOM_Widget *_trackWidget;
  ATOM_VECTOR<ATOM_MenuItem> _items;
  MenuLayout _menuLayout;
  ATOM_Size2Di _desiredItemSize;
  ATOM_Size2Di _checkedFlagSize;
  int _checkImageId;
  int _verticalMargin;
  int _horizontalMargin;
  bool _enableDrawText;
  bool _uniformWidth;
  bool _autoPopup;
  bool _popping;
  bool _clickonce;
  ATOM_ColorARGB _selectColor;

  ATOM_PopupMenu *_tracking;
  ATOM_PopupMenu *_owner;
  ATOM_PopupMenu *_expand;
  ATOM_PopupMenu *_prev;

public:
  virtual ATOM_WidgetType getType (void) const;
  virtual bool isMenu (void) const;

public:
  void onLostFocus (ATOM_WidgetLostFocusEvent *event);
  void onMouseMove (ATOM_WidgetMouseMoveEvent *event);
  void onMouseLeave (ATOM_WidgetMouseLeaveEvent *event);
  void onLButtonDown (ATOM_WidgetLButtonDownEvent *event);
  void onLButtonUp (ATOM_WidgetLButtonUpEvent *event);
  void onResize (ATOM_WidgetResizeEvent *event);
  void onPaint (ATOM_WidgetDrawClientEvent *event);

  ATOM_DECLARE_EVENT_MAP(ATOM_PopupMenu, ATOM_TopWindow)
};

#endif // __ATOM3D_ENGINE_MENU_H
/*! @} */
