#include "stdafx.h"
#include "menu.h"
#include "gui_events.h"
#include "gui_canvas.h"

ATOM_BEGIN_EVENT_MAP(ATOM_PopupMenu, ATOM_Widget)
  ATOM_EVENT_HANDLER(ATOM_PopupMenu, ATOM_WidgetLostFocusEvent, onLostFocus)
  ATOM_EVENT_HANDLER(ATOM_PopupMenu, ATOM_WidgetMouseMoveEvent, onMouseMove)
  ATOM_EVENT_HANDLER(ATOM_PopupMenu, ATOM_WidgetMouseLeaveEvent, onMouseLeave)
  ATOM_EVENT_HANDLER(ATOM_PopupMenu, ATOM_WidgetLButtonDownEvent, onLButtonDown)
  ATOM_EVENT_HANDLER(ATOM_PopupMenu, ATOM_WidgetLButtonUpEvent, onLButtonUp)
  ATOM_EVENT_HANDLER(ATOM_PopupMenu, ATOM_WidgetResizeEvent, onResize)
  ATOM_EVENT_HANDLER(ATOM_PopupMenu, ATOM_WidgetDrawClientEvent, onPaint)
ATOM_END_EVENT_MAP

ATOM_PopupMenu::ATOM_PopupMenu(int id)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::ATOM_PopupMenu);

	_widgetId = id;
	_guiRenderer = NULL;
	_selected = -1;
	_selectColor = ATOM_ColorARGB(0.f, 0.f, 0.f, 1.f);

	_tracking = 0;
	_owner = 0;
	_expand = 0;
	_prev = 0;

	_trackWidget = 0;
	_desiredItemSize.w = 0;
	_desiredItemSize.h = 0;
	_checkedFlagSize.w = 12;
	_checkedFlagSize.h = 12;
	_verticalMargin = 4;
	_horizontalMargin = 4;
	_checkImageId = ATOM_IMAGEID_CHECKED;
	_enableDrawText = true;
	_uniformWidth = true;
	_autoPopup = true;
	_popping = true;
	_clickonce = false;

	setLayoutable (false);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_PopupMenu::ATOM_PopupMenu (ATOM_GUIRenderer *renderer, int id)
:
ATOM_Widget(0, ATOM_Rect2Di(0, 0, 0, 0), ATOM_Widget::Border|ATOM_Widget::NoClip|ATOM_Widget::NonScrollable|ATOM_Widget::NonLayoutable|ATOM_Widget::Popup, id, ATOM_Widget::Hide)
{
  ATOM_STACK_TRACE(ATOM_PopupMenu::ATOM_PopupMenu);

  _guiRenderer = renderer;
  _selected = -1;
  _selectColor = ATOM_ColorARGB(0.f, 0.f, 0.f, 1.f);

  _tracking = 0;
  _owner = 0;
  _expand = 0;
  _prev = 0;

  _trackWidget = 0;
  _desiredItemSize.w = 0;
  _desiredItemSize.h = 0;
  _checkedFlagSize.w = 12;
  _checkedFlagSize.h = 12;
  _verticalMargin = 4;
  _horizontalMargin = 4;
  _checkImageId = ATOM_IMAGEID_CHECKED;
  _enableDrawText = true;
  _uniformWidth = true;
  _autoPopup = true;
  _popping = true;
  _clickonce = false;

  resize (_widgetRect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_PopupMenu::~ATOM_PopupMenu (void)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::~ATOM_PopupMenu);

	if (_prev && _prev->_expand == this)
	{
		_prev->_expand = 0;
	}

	if (_owner && _owner->_tracking == this)
	{
		_owner->_tracking = 0;
	}

	for (unsigned i = 0; i < _items.size(); ++i)
	{
		if (_items[i].submenu)
		{
			ATOM_DELETE(_items[i].submenu);
			_items[i].submenu = 0;
		}
	}
}

bool ATOM_PopupMenu::isMenu (void) const
{
  return true;
}

unsigned ATOM_PopupMenu::appendMenuItem (const char *text, int id)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::appendMenuItem);

	_items.resize (_items.size() + 1);
	ATOM_MenuItem &item = _items.back();
	item.title = text;
	item.text = ATOM_HARDREF(ATOM_Text)();
	item.text->setCharMargin (ATOM_GUIFont::getFontCharMargin(getFont()));
	item.text->setFont (ATOM_GUIFont::getFontHandle (getFont()));
	item.text->setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
	item.id = id;
	item.submenu = 0;
	item.checked = false;
	item.imageId = ATOM_IMAGEID_CONTROL_BKGROUND;
	item.hilightImageId = ATOM_IMAGEID_MENUITEM_HILIGHT;

	if (getParent())
	{
		ATOM_WidgetResizeEvent resizeEvent(getId(), _widgetRect);
		handleEvent (&resizeEvent);
	}

	return _items.size() - 1;
}

bool ATOM_PopupMenu::insertMenuItem (unsigned index, const char *text, int id)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::insertMenuItem);

	if (index >= _items.size())
	{
        return false;
	}

	_items.resize (_items.size() + 1);
	for (unsigned i = _items.size() - 1; i > index; --i)
	{
		_items[i] = _items[i-1];
	}

	ATOM_MenuItem &item = _items[index];
	item.title = text;
	item.text = ATOM_HARDREF(ATOM_Text)();
	item.text->setCharMargin (ATOM_GUIFont::getFontCharMargin(getFont()));
	item.text->setFont (ATOM_GUIFont::getFontHandle(getFont()));
	item.text->setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
	item.id = id;
	item.submenu = 0;
	item.checked = false;
	item.imageId = ATOM_IMAGEID_CONTROL_BKGROUND;
	item.hilightImageId = ATOM_IMAGEID_MENUITEM_HILIGHT;

	if (getParent())
	{
		ATOM_WidgetResizeEvent resizeEvent(getId(), _widgetRect);
		handleEvent (&resizeEvent);
	}

	return true;
}

void ATOM_PopupMenu::removeMenuItemById (int id)
{
	for (unsigned i = 0; i < _items.size(); ++i)
	{
		if (_items[i].id == id)
		{
			_items.erase (_items.begin() + i);
			if (getParent())
			{
				ATOM_WidgetResizeEvent resizeEvent(getId(), _widgetRect);
				handleEvent (&resizeEvent);
				invalidate ();
			}
			break;
		}
	}
}

void ATOM_PopupMenu::removeMenuItem (unsigned index)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::removeMenuItem);

	if (index < _items.size())
	{
		ATOM_DELETE(_items[index].submenu);
		_items.erase (_items.begin() + index);

		if (getParent())
		{
			ATOM_WidgetResizeEvent resizeEvent(getId(), _widgetRect);
			handleEvent (&resizeEvent);

			invalidate ();
		}
	}
}

void ATOM_PopupMenu::clear (void)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::clear);

	if (_items.size() > 0)
	{
		for (unsigned i = 0; i < _items.size(); ++i)
		{
			ATOM_DELETE(_items[i].submenu);
			_items[i].submenu = 0;
		}

		_items.clear ();

		if (getParent ())
		{
			ATOM_WidgetResizeEvent resizeEvent(getId(), _widgetRect);
			handleEvent (&resizeEvent);
			invalidate ();
		}
	}
}

unsigned ATOM_PopupMenu::getNumItems (void) const
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::getNumItems);

	return _items.size();
}

ATOM_PopupMenu *ATOM_PopupMenu::createSubMenu (unsigned item)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::createSubMenu);

	if (item >= _items.size())
	{
		return 0;
	}

	if (_items[item].submenu)
	{
		// already has a submenu
		return 0;
	}

	ATOM_PopupMenu *subMenu = ATOM_NEW(ATOM_PopupMenu, _guiRenderer, _items[item].id);
	subMenu->setStyle (subMenu->getStyle() | ATOM_Widget::NoFocus);
	subMenu->_tracking = 0;
	subMenu->_owner = _owner ? _owner : this;
	subMenu->_expand = 0;
	subMenu->_prev = this;

	_items[item].submenu = subMenu;

	invalidate ();

	return subMenu;
}

void ATOM_PopupMenu::deleteSubMenu (unsigned item)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::deleteSubMenu);

	if (item < _items.size() && _items[item].submenu)
	{
		if (_expand == _items[item].submenu)
		{
			invalidate ();
		}

		ATOM_DELETE(_items[item].submenu);
		_items[item].submenu = 0;
	}
}

void ATOM_PopupMenu::collapse (void)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::collapse);

	if (_prev && _prev->_expand == this)
	{
		_prev->_expand = 0;
	}

	if (_owner && _owner->_tracking == this)
	{
		_owner->_tracking = 0;
	}

	show (ATOM_Widget::Hide);

	ATOM_Widget *parent = getParent ();

	if (parent)
	{
		parent->detach (this);
	}

	if (_expand)
	{
		_expand->collapse ();
	}

	_clickonce = false;
}

ATOM_Point2Di ATOM_PopupMenu::calcSubMenuPosition (unsigned item) const
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::calcSubMenuPosition);

	ATOM_ASSERT(item < _items.size());

	return ATOM_Point2Di(_widgetRect.point.x + _widgetRect.size.w, _widgetRect.point.y + _menuLayout.itemRects[item].point.y);
}

void ATOM_PopupMenu::expand (unsigned item)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::expand);

	if (item < _items.size())
	{
		ATOM_PopupMenu *subMenu = _items[item].submenu;

		if (subMenu)
		{
			if (_expand != subMenu && _expand)
			{
				_expand->collapse ();
			}

			ATOM_ASSERT(_parent);

			ATOM_MenuPopupEvent e(subMenu->getId());
			getParent()->handleEvent (&e);

			_parent->attach (subMenu);
			ATOM_Rect2Di rcItem = _menuLayout.itemRects[item];
			subMenu->resize (ATOM_Rect2Di(calcSubMenuPosition(item), ATOM_Size2Di(100, 100)));
			subMenu->setSelected (-1);
			subMenu->show (ATOM_Widget::ShowNormal);

			_expand = subMenu;
		}
	}
}

void ATOM_PopupMenu::setFont (ATOM_GUIFont::handle font)
{
	ATOM_Widget::setFont (font);

	if (getParent ())
	{
		ATOM_WidgetResizeEvent resizeEvent(getId(), _widgetRect);
		handleEvent (&resizeEvent);

		invalidate ();
	}
}

void ATOM_PopupMenu::setText (unsigned index, const char *text)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::setText);

	if (index < _items.size())
	{
		ATOM_MenuItem &item = _items[index];
		item.title = text;

		if (getParent())
		{
			ATOM_WidgetResizeEvent resizeEvent(getId(), _widgetRect);
			handleEvent (&resizeEvent);

			invalidate ();
		}
	}
}

const char *ATOM_PopupMenu::getText (unsigned index) const
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::getText);

	if (index < _items.size())
	{
		return _items[index].title.c_str();
	}
	return 0;
}

void ATOM_PopupMenu::setCheck (unsigned index, bool check)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::setCheck);

	if (index < _items.size())
	{
		_items[index].checked = check;
	}
}

void ATOM_PopupMenu::setCheckById (int id, bool check)
{
	for (unsigned i = 0; i < _items.size(); ++i)
	{
		if (_items[i].id == id)
		{
			_items[i].checked = check;
			break;
		}
	}
}

bool ATOM_PopupMenu::getCheck (unsigned index) const
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::getCheck);

	if (index < _items.size())
	{
		return _items[index].checked;
	}
	return false;
}

bool ATOM_PopupMenu::getCheckById (int id) const
{
	for (unsigned i = 0; i < _items.size(); ++i)
	{
		if (_items[i].id == id)
		{
			return _items[i].checked;
		}
	}
	return false;
}

void ATOM_PopupMenu::setItemImageId (unsigned index, int id)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::setItemImageId);

	if (index < _items.size() && _items[index].imageId != id)
	{
		_items[index].imageId = id;
		invalidate ();
	}
}

int ATOM_PopupMenu::getItemImageId (unsigned index) const
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::getItemImageId);

	return (index < _items.size()) ? _items[index].imageId : -1;
}

void ATOM_PopupMenu::setHilightItemImageId (unsigned index, int id)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::setHilightItemImageId);

	if (index < _items.size() && _items[index].hilightImageId != id)
	{
		_items[index].hilightImageId = id;
		invalidate ();
	}
}

int ATOM_PopupMenu::getHilightItemImageId (unsigned index) const
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::getHilightItemImageId);

	return (index < _items.size()) ? _items[index].hilightImageId : -1;
}

void ATOM_PopupMenu::setDesiredItemSize (const ATOM_Size2Di &size)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::setDesiredItemSize);

	if (size != _desiredItemSize)
	{
		_desiredItemSize = size;

		ATOM_Widget *trackWidget = getTrackWidget();
		ATOM_Widget *widget = ATOM_Widget::isValidWidget(trackWidget) ? trackWidget : getParent();
		if (widget)
		{
			queueEvent (ATOM_NEW(ATOM_WidgetResizeEvent, getId(), _widgetRect), ATOM_APP);
		}
	}
}

const ATOM_Size2Di &ATOM_PopupMenu::getDesiredItemSize (void) const
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::getDesiredItemSize);

	return _desiredItemSize;
}

void ATOM_PopupMenu::setCheckedFlagSize (const ATOM_Size2Di &size)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::setCheckedFlagSize);

	if (size != _checkedFlagSize)
	{
		_checkedFlagSize = size;

		ATOM_Widget *trackWidget = getTrackWidget();
		ATOM_Widget *widget = ATOM_Widget::isValidWidget(trackWidget) ? trackWidget : getParent();
		if (widget)
		{
			queueEvent (ATOM_NEW(ATOM_WidgetResizeEvent, getId(), _widgetRect), ATOM_APP);
		}
	}
}

const ATOM_Size2Di &ATOM_PopupMenu::getCheckedFlagSize (void) const
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::getCheckedFlagSize);

	return _checkedFlagSize;
}

void ATOM_PopupMenu::setCheckImageId (int id)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::setCheckImageId);

	if (id != _checkImageId)
	{
		invalidate ();
	}
}

int ATOM_PopupMenu::getCheckImageId (void) const
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::getCheckImageId);

	return _checkImageId;
}

void ATOM_PopupMenu::enableDrawText (bool enable)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::enableDrawText);

	if (enable != _enableDrawText)
	{
		_enableDrawText = enable;

		ATOM_Widget *trackWidget = getTrackWidget();
		ATOM_Widget *widget = ATOM_Widget::isValidWidget(trackWidget) ? trackWidget : getParent();
		if (widget)
		{
			queueEvent (ATOM_NEW(ATOM_WidgetResizeEvent, getId(), _widgetRect), ATOM_APP);
		}
	}
}

bool ATOM_PopupMenu::isDrawTextEnabled (void) const
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::isDrawTextEnabled);

	return _enableDrawText;
}

void ATOM_PopupMenu::setSelected (int index)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::setSelected);

	if (index != _selected && (index < _items.size() || index == (int)invalid_index))
	{
		_selected = index;
		invalidate ();
	}
}

unsigned ATOM_PopupMenu::getSelected (void) const
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::getSelected);

	return _selected;
}

const ATOM_MenuItem &ATOM_PopupMenu::getMenuItem (unsigned index) const
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::getMenuItem);

	return _items[index];
}

void ATOM_PopupMenu::setSelectColor(ATOM_ColorARGB clr)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::setSelectColor);

	_selectColor = clr;
}

ATOM_ColorARGB ATOM_PopupMenu::getSelectColor() const
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::getSelectColor);

	return _selectColor;
}

ATOM_WidgetType ATOM_PopupMenu::getType (void) const
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::getType);

	return WT_POPUPMENU;
}

void ATOM_PopupMenu::onLostFocus (ATOM_WidgetLostFocusEvent *event)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::onLostFocus);

	ATOM_ASSERT(!_owner);

	collapse ();

	_trackWidget = 0;
	_clickonce = false;
}

void ATOM_PopupMenu::onMouseMove (ATOM_WidgetMouseMoveEvent *event)
{
  ATOM_STACK_TRACE(ATOM_PopupMenu::onMouseMove);

  int dx = _clientRect.point.x;
  int dy = _clientRect.point.y;
  int x = event->x - dx;
  int y = event->y - dy;

  for (int i = 0; i < _menuLayout.itemRects.size(); ++i)
  {
    ATOM_Rect2Di rc = _menuLayout.itemRects[i];
	rc.size.w = _menuLayout.actualWidth[i];

    if (rc.isPointIn (x, y))
    {
	  setSelected (i);

	  if (_items[i].submenu != _expand)
	  {
		  if (_popping)
		  {
			  if (_expand)
			  {
				  _expand->collapse ();
			  }

			  if (_items[i].submenu)
			  {
				  expand (i);
			  }
		  }
	  }

      return;
    }
  }

  setSelected (-1);
}

void ATOM_PopupMenu::onMouseLeave (ATOM_WidgetMouseLeaveEvent *event)
{
  ATOM_STACK_TRACE(ATOM_PopupMenu::onMouseLeave);

  setSelected (-1);
}

void ATOM_PopupMenu::onLButtonDown (ATOM_WidgetLButtonDownEvent *event)
{
  ATOM_STACK_TRACE(ATOM_PopupMenu::onLButtonDown);

  setCapture ();

  if (!_autoPopup && getParent() && _selected != (unsigned)-1 && _items[_selected].submenu)
  {
	  if (_expand != _items[_selected].submenu)
	  {
		  expand (_selected);
		  _popping = true;
		  _clickonce = true;
	  }
  }
}

void ATOM_PopupMenu::onLButtonUp (ATOM_WidgetLButtonUpEvent *event)
{
  ATOM_STACK_TRACE(ATOM_PopupMenu::onLButtonUp);

  if (isCaptured ())
  {
    releaseCapture ();

    if (getParent() && _selected != (unsigned)-1)
    {
		if (!_items[_selected].submenu)
		{
			ATOM_Widget *trackWidget = getTrackWidget();
			ATOM_Widget *widget = ATOM_Widget::isValidWidget(trackWidget) ? trackWidget : getParent();
			if (widget)
			{
				widget->queueEvent (ATOM_NEW(ATOM_WidgetCommandEvent, getMenuItem(_selected).id), ATOM_APP);
			}

			if (_owner)
			{
				_owner->collapse ();
			}
			else
			{
				collapse ();
			}
		}
		else if (!_autoPopup && !_clickonce)
		{
			if (_expand == _items[_selected].submenu)
			{
				_expand->collapse ();
				_popping = false;
			}
		}
    }

	_clickonce = false;
  }
}

void ATOM_PopupMenu::calcLayout (const ATOM_Rect2Di &rc)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::calcLayout);

	ATOM_FontHandle font = ATOM_GUIFont::getFontHandle (getFont());
	int charmargin = ATOM_GUIFont::getFontCharMargin (getFont());
	ATOM_SetCharMargin (charmargin);

	_menuLayout.itemRects.resize (_items.size());
	_menuLayout.actualWidth.resize (_items.size());
	_menuLayout.textPositions.resize (_items.size());

	const unsigned minWidth = _checkedFlagSize.w + 3 * _horizontalMargin;
	const unsigned minHeight = _checkedFlagSize.h + 2 * _verticalMargin;
	unsigned width = (_desiredItemSize.w > minWidth) ? _desiredItemSize.w : minWidth;
	unsigned height = (_desiredItemSize.h > minHeight) ? _desiredItemSize.h : minHeight;

	bool fixedWidth = _desiredItemSize.w > 0;
	int textPositionY = 0;
	if (_enableDrawText)
	{
		const char *testStr = "gf";
		int l, t, w, h;
		ATOM_CalcStringBounds (font, testStr, strlen(testStr), &l, &t, &w, &h);
		if (height < h + 2 * _verticalMargin)
		{
			height = h + 2 * _verticalMargin;
		}
		textPositionY = (height - h) / 2 - t;
	}

	for (unsigned i = 0; i < _items.size(); ++i)
	{
		const ATOM_STRING &title = _items[i].title;

		int l, t, w, h;

		if (!_uniformWidth)
		{
			if (ATOM_CalcStringBounds (font, title.c_str(), title.length(), &l, &t, &w, &h))
			{
				_menuLayout.itemRects[i].size.w = w + minWidth;
				_menuLayout.actualWidth[i] = w + minWidth;
				_items[i].text->setString (title.c_str());
			}
			else
			{
				_menuLayout.itemRects[i].size.w = minWidth;
				_menuLayout.actualWidth[i] = minWidth;
				_items[i].text->setString ("");
			}
		}
		else if (!fixedWidth)
		{
			if (ATOM_CalcStringBounds (font, title.c_str(), title.length(), &l, &t, &w, &h) && w > width - minWidth)
			{
				width = minWidth + w;
			}
			_items[i].text->setString (title.c_str());
		}
		else
		{
			unsigned n = ATOM_ClipString (font, title.c_str(), width - minWidth, &l, &t, &w, &h);
			_items[i].text->setString (title.substr(0, n).c_str());
		}

		_menuLayout.itemRects[i].point.x = 0;
		_menuLayout.itemRects[i].point.y = i * height;
		_menuLayout.itemRects[i].size.h = height;
		_menuLayout.textPositions[i].x = minWidth - _horizontalMargin;
		_menuLayout.textPositions[i].y = i * height + textPositionY;
	}

	_clientRect.size.w = width;
	_clientRect.size.h = height * _items.size();
	_widgetRect.point = rc.point;
	_widgetRect.size.w = _clientRect.size.w + 2 * _clientRect.point.x;
	_widgetRect.size.h = _clientRect.size.h + 2 * _clientRect.point.y;
	_titleBarRect.size.w = 0;
	_titleBarRect.size.h = 0;

	if (_uniformWidth)
	{
		for (unsigned i = 0; i < _items.size(); ++i)
		{
			_menuLayout.itemRects[i].size.w = width;
			_menuLayout.actualWidth[i] = width;
		}
	}
}

void ATOM_PopupMenu::onResize (ATOM_WidgetResizeEvent *event)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::onResize);

	callParentHandler (event);

#if 1
	calcLayout (event->widgetRect);
#else
	ATOM_PopupMenu *menu = this;
	MenuLayout *menuLayout = &_menuLayout;
	const int menuHeight = 18;
	const int menuLeftMargin = 8;
	const int menuRightMargin = 8;

	ATOM_GUIFont::handle menuFont = menu->getFont();
	ATOM_FontHandle font = ATOM_GUIFont::getFontHandle (menuFont);
	int charmargin = ATOM_GUIFont::getFontCharMargin (menuFont);
	int charset = ATOM_GUIFont::getFontCharSet (menuFont);
	ATOM_SetCharMargin (charmargin);

	menuLayout->itemRects.resize (menu->getNumItems());
	menuLayout->textPositions.resize (menu->getNumItems());

	const unsigned minWidth = _checkedFlagSize.w + 3 * _horizontalMargin;
	const unsigned minHeight = _checkedFlagSize.h + 2 * _verticalMargin;
	unsigned width = _desiredItemSize.w > minWidth ? _desiredItemSize.w : minWidth;
	unsigned height = _desiredItemSize.h > minHeight ? _desiredItemSize.h : minHeight;

	bool fixedWidth = _desiredItemSize.w > 0;
	int textPositionY = 0;
	if (_enableDrawText)
	{
		const char *testStr = "gf";
		int l, t, w, h;
		ATOM_CalcStringBounds (font, charset, testStr, strlen(testStr), &l, &t, &w, &h);
		if (height < h + 2 * _verticalMargin)
		{
			height = h + 2 * _verticalMargin;
		}
		textPositionY = (height - h) / 2 - t;
	}

	for (unsigned i = 0; i < menu->getNumItems(); ++i)
	{
		int l, t, w, h;

		if (!fixedWidth)
		{
			if (ATOM_CalcStringBounds (font, charset, menu->getText(i), strlen(menu->getText(i)), &l, &t, &w, &h) && w > width - minWidth)
			{
				width = minWidth + w;
			}
			menu->_items[i].text->setString (menu->getText(i), charset);
		}
		else
		{
			unsigned n = ATOM_ClipString (font, charset, menu->getText(i), width - minWidth, &l, &t, &w, &h);
			ATOM_STRING s(menu->getText(i), 0, n);
			menu->_items[i].text->setString (s.c_str(), charset);
		}

		menuLayout->itemRects[i].x = 0;
		menuLayout->itemRects[i].y = i * height;
		menuLayout->itemRects[i].h = height;
		menuLayout->textPositions[i].x = minWidth - _horizontalMargin;
		menuLayout->textPositions[i].y = i * height + textPositionY;
	}

	_clientRect.size.w = width;
	_clientRect.size.h = height * menu->getNumItems();
	_widgetRect.point = event->widgetRect.point;
	_widgetRect.size.w = _clientRect.size.w + 2 * _clientRect.point.x;
	_widgetRect.size.h = _clientRect.size.h + 2 * _clientRect.point.y;
	_titleBarRect.w = 0;
	_titleBarRect.h = 0;

	for (unsigned i = 0; i < menu->getNumItems(); ++i)
	{
		menuLayout->itemRects[i].w = width;
	}
#endif
}

void ATOM_PopupMenu::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_PopupMenu::onPaint);

    ATOM_Rect2Di rc;
    rc.point.x = 0;
    rc.point.y = 0;
    rc.size = event->canvas->getSize();

	ATOM_PopupMenu *menu = this;
	const ATOM_GUIImageList *imagelist = getValidImageList ();

	MenuLayout *layout = &_menuLayout;
	for (unsigned i = 0; i < menu->getNumItems (); ++i)
	{
		bool selected = menu->getSelected() == i;

		ATOM_GUIImage *imgItem = imagelist->getImage (selected ? _items[i].hilightImageId : _items[i].imageId);

		if (!imgItem)
		{
			imgItem = ATOM_GUIImageList::getDefaultImageList().getImage (selected ? ATOM_IMAGEID_MENUITEM_HILIGHT : ATOM_IMAGEID_CONTROL_BKGROUND);
			ATOM_ASSERT(imgItem);
		}

		ATOM_Rect2Di rcItem = layout->itemRects[i];
		rcItem.size.w = layout->actualWidth[i];
		imgItem->draw (WST_NORMAL, event->canvas, rcItem);

		if (_items[i].checked)
		{
			ATOM_GUIImage *imgChecked = imagelist->getImage (_checkImageId);
			if (!imgChecked)
			{
				imgChecked = ATOM_GUIImageList::getDefaultImageList().getImage (ATOM_IMAGEID_CHECKBOX_CHECKED);
				ATOM_ASSERT(imgChecked);
			}
			imgChecked->draw (WST_NORMAL, event->canvas, ATOM_Rect2Di(rcItem.point.x + _horizontalMargin, (rcItem.size.h - _checkedFlagSize.h) / 2 + rcItem.point.y, _checkedFlagSize.w, _checkedFlagSize.h));
		}

		ATOM_Point2Di textPosition = layout->textPositions[i];
		event->canvas->drawText (menu->getMenuItem(i).text.get(), textPosition.x, textPosition.y, (i == menu->getSelected()) ? _selectColor : getFontColor());
	}
}

void ATOM_PopupMenu::setTrackWidget (ATOM_Widget *widget)
{
	_trackWidget = widget;
}

ATOM_Widget *ATOM_PopupMenu::getTrackWidget (void) const
{
	return _trackWidget ? _trackWidget : (_owner ? _owner->getTrackWidget() : 0);
}

