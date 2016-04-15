#include "stdafx.h"
#include "widget.h"
#include "gui_renderer.h"
#include "gui_events.h"
#include "menu.h"
#include "imagelist.h"
#include "gui_canvas.h"
#include "button.h"
#include "widget_io.h"

unsigned widgetCount = 0;

class WidgetCloseButton: public ATOM_Button
{
public:
	WidgetCloseButton (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
		:
	ATOM_Button(parent, rect, style|ATOM_Widget::NonLayoutable|ATOM_Widget::NoClip|ATOM_Widget::NoFocus|ATOM_Widget::NonScrollable|ATOM_Widget::Private, id, showState)
	{
		ATOM_STACK_TRACE(ATOM_CloseButton::ATOM_CloseButton);
#if defined(USE_WIDGET_DEBUG_INFO)
		ATOM_Widget::registerWidgetDebugInfo (this);
#endif
	}

protected:
	virtual void onClicked (unsigned keymod)
	{
		ATOM_Widget *parent = getParent ();
		if (parent)
		{
			parent->queueEvent (ATOM_NEW(ATOM_TopWindowCloseEvent), ATOM_APP);
		}
	}

public:
	void onPaint (ATOM_WidgetDrawClientEvent *event)
	{
		ATOM_STACK_TRACE(ATOM_CloseButton::onPaint);

		callParentHandler (event);

		const ATOM_Size2Di &rc = event->canvas->getSize();
		const int margin = 2;

		bool active = false;
		ATOM_Widget *parent = getParent ();
		ATOM_Widget *focus = _guiRenderer->getFocus ();
		if (parent && parent->isTopWindow() && ATOM_Widget::isValidWidget(focus) && _guiRenderer->belongsTo (focus, parent))
		{
			active = true;
		}

		const ATOM_ColorARGB color = active ? ATOM_ColorARGB(0.f, 0.f, 0.f, 1.f) : ATOM_ColorARGB(0.3f, 0.3f, 0.3f, 1.f);
		const ATOM_Point2Di start[2] = { ATOM_Point2Di(margin, margin), ATOM_Point2Di(rc.w - margin - 1, margin) };
		const ATOM_Point2Di end[2] = { ATOM_Point2Di(rc.w - margin, rc.h - margin), ATOM_Point2Di(margin - 1, rc.h - margin) };
		const ATOM_ColorARGB colors[2] = { color, color };
		event->canvas->drawLineList (2, start, end, colors);
  	}

	ATOM_DECLARE_EVENT_MAP(WidgetCloseButton, ATOM_Button)
};

ATOM_BEGIN_EVENT_MAP(WidgetCloseButton, ATOM_Button)
	ATOM_EVENT_HANDLER(WidgetCloseButton, ATOM_WidgetDrawClientEvent, onPaint)
ATOM_END_EVENT_MAP

ATOM_WidgetBase::~ATOM_WidgetBase (void)
{
}

bool ATOM_WidgetBase::preHandleEvent (ATOM_Event *event)
{
	return true;
}

void ATOM_WidgetBase::postHandleEvent (ATOM_Event *event)
{
}

ATOM_HASHSET<ATOM_Widget*> ATOM_Widget::_widgetSet;

#if defined(USE_WIDGET_DEBUG_INFO)
ATOM_HASHMAP<void*, ATOM_Widget::WidgetDebugInfo> _widgetDebugInfos;
#endif

ATOM_BEGIN_EVENT_MAP_NOPARENT_NOFILTER(ATOM_WidgetBase)
ATOM_END_EVENT_MAP

ATOM_BEGIN_EVENT_MAP(ATOM_Widget, ATOM_WidgetBase)
	ATOM_EVENT_HANDLER(ATOM_Widget, ATOM_WidgetHitTestEvent, onHitTest)
	ATOM_EVENT_HANDLER(ATOM_Widget, ATOM_WidgetResizeEvent, onResize)
	ATOM_EVENT_HANDLER(ATOM_Widget, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(ATOM_Widget, ATOM_WidgetDrawFrameEvent, onPaintFrame)
	ATOM_EVENT_HANDLER(ATOM_Widget, ATOM_ScrollEvent, onScroll)
	ATOM_EVENT_HANDLER(ATOM_Widget, ATOM_WidgetMouseWheelEvent, onMouseWheel)
	ATOM_EVENT_HANDLER(ATOM_Widget, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	ATOM_EVENT_HANDLER(ATOM_Widget, ATOM_WidgetLButtonUpEvent, onLButtonUp)
ATOM_END_EVENT_MAP




//--- wangjian added ---//
// 异步加载相关
ATOM_WeakPtrT<ATOM_GUIImage>	ATOM_Widget::_imageWaiting = 0;
int								ATOM_Widget::_imageWaitingId = 0;
//----------------------//




int ATOM_Widget::fontSize = 12;
ATOM_Widget::ATOM_Widget(void)
{
	ATOM_STACK_TRACE(ATOM_Widget::ATOM_Widget);

	widgetCount++;

	_guiRenderer = 0;
	_widgetId = ATOM_Widget::AnyId;
	_widgetRect = ATOM_Rect2Di(0,0,0,0);
	_clientRect = ATOM_Rect2Di(0,0,0,0);
	_titleBarRect = ATOM_Rect2Di(0,0,0,0);
	_layout = ATOM_WidgetLayout::createLayout (ATOM_WidgetLayout::Basic);
	_layoutUnit = 0;
	_parent = 0;
	_next = 0;
	_prev = 0;
	_firstChild = 0;
	_lastChild = 0;
	_firstControl = 0;
	_lastControl = 0;
	_hierichyRoot = 0;
	_popupRelation = 0;
	_showState = ATOM_Widget::Hide;
	_stateFlags = 0;
	_creationStyle = 0;
	_borderMode = ATOM_Widget::Raise;
	_dragMode = ATOM_Widget::TitleBarDrag;
	_clientImageId = ATOM_IMAGEID_CONTROL_BKGROUND;
	_lastClientImageState = WST_NORMAL;
	_dynamicCounter = 0;
	_closeButton = 0;
	_menuBar = 0;
	_verticalScrollBar = 0;
	_horizontalScrollBar = 0;
	_imagelist = 0;
	_mouseHover = false;
	_handleIdleEvent = -1;
	_scrollBarWidth = 16;
	_canvasSize.w = 0;
	_canvasSize.h = 0;
	_scrollOffset.x = 0;
	_scrollOffset.y = 0;
	_pickable = true;
	_enableDrag = false;
	_enableDrop = false;
	_enableWheelDeliver = false;
	_widgetFont = ATOM_GUIFont::invalid_handle;
	_widgetFontColor = ATOM_ColorARGB(1.f, 1.f, 1.f, 1.f);
	_widgetFontOutLineColor = ATOM_ColorARGB(0.f, 0.f, 0.f, 0.f);
	_userData = 0;
	_attributes = ATOM_NEW(ATOM_UserAttributes);
	_clientImage = NULL;
	_isExClientImage = false;
	_widgetSet.insert (this);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif

	//--- wangjian added ---//
	setAsyncLoad(ATOM_LoadPriority_IMMEDIATE);	// 默认为非异步加载
	_bLoadRequestSubmitted = 0;
	_bCheckWaiting = 1;							// 默认为检查等待
	//----------------------//
}

#if defined(USE_WIDGET_DEBUG_INFO)
void ATOM_Widget::registerWidgetDebugInfo (ATOM_Widget *widget)
{
	_widgetDebugInfos[widget].type = widget->getType();
	_widgetDebugInfos[widget].parent = widget->getParent();
	ATOM_LOGGER::log ("Register widget debug info: 0x%08X(%s)\n", widget, ATOM_Widget::getWidgetTypeString (widget->getType()));
}
const ATOM_Widget::WidgetDebugInfo *ATOM_Widget::getWidgetDebugInfo (ATOM_Widget *widget)
{
	ATOM_HASHMAP<void*, ATOM_Widget::WidgetDebugInfo>::const_iterator it =  _widgetDebugInfos.find (widget);
	return it == _widgetDebugInfos.end () ? 0 : &it->second;
}
const char *ATOM_Widget::getWidgetTypeString (ATOM_WidgetType type)
{
	switch (type)
	{
	case WT_WIDGET:
		return "ATOM_Widget";
	case WT_TOPWINDOW:
		return "ATOM_TopWindow";
	case WT_DIALOG:
		return "ATOM_Dialog";
	case WT_DESKTOP:
		return "ATOM_Desktop";
	case WT_POPUPMENU:
		return "ATOM_PopupMenu";
	case WT_EDIT:
		return "ATOM_Edit";
	case WT_BUTTON:
		return "ATOM_Button";
	case WT_LABEL:
		return "ATOM_Label";
	case WT_PROGRESSBAR:
		return "ATOM_ProgressBar";
	case WT_RICHEDIT:
		return "ATOM_RichEdit";
	case WT_CELLDATA:
		return "ATOM_CellData";
	case WT_CELL:
		return "ATOM_Cell";
	case WT_REALTIMECTRL:
		return "ATOM_RealtimeCtrl";
	case WT_LISTBOX:
		return "ATOM_ListBox";
	case WT_SCROLLBAR:
		return "ATOM_ScrollBar";
	case WT_TREEITEM:
		return "ATOM_TreeItem";
	case WT_TREECTRL:
		return "ATOM_TreeCtrl";
	case WT_CHECKBOX:
		return "ATOM_CheckBox";
	case WT_CIRCLEPROGRESS:
		return "ATOM_CircleProgress";
	case WT_SCROLLMAP:
		return "ATOM_ScrollMap";
	case WT_LISTVIEW:
		return "ATOM_ListView";
	case WT_SLIDER:
		return "ATOM_Slider";
	case WT_PROPERTYLIST:
		return "ATOM_PropertyList";
	case WT_MARQUEE:
		return "ATOM_Marquee";
	case WT_HYPERLINK:
		return "ATOM_Hyperlink";
	case WT_MULTIEDIT:
		return "ATOM_MultiEdit";
	case WT_COMBOBOX:
		return "ATOM_ComboBox";
	case WT_CURVEEDITOR:
		return "ATOM_CurveEditor";
	default:
		return "Unknown widget";
	}
}
#endif

ATOM_Widget::ATOM_Widget (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
{
	ATOM_STACK_TRACE(ATOM_Widget::ATOM_Widget);

	widgetCount++;

	_guiRenderer = parent ? parent->getRenderer () : 0;
	_layout = ATOM_WidgetLayout::createLayout (ATOM_WidgetLayout::Basic);
	_layoutUnit = 0;
	_widgetId = id;
	_widgetRect = rect;
	_clientRect = ATOM_Rect2Di(0,0,0,0);
	_titleBarRect = ATOM_Rect2Di(0,0,0,0);
	_parent = 0;
	_next = 0;
	_prev = 0;
	_firstChild = 0;
	_lastChild = 0;
	_firstControl = 0;
	_lastControl = 0;
	_hierichyRoot = 0;
	_popupRelation = 0;
	_showState = ATOM_Widget::Hide;
	_stateFlags = 0;
	_creationStyle = style;
	_borderMode = ATOM_Widget::Raise;
	_dragMode = ATOM_Widget::TitleBarDrag;
	_clientImageId = ATOM_IMAGEID_CONTROL_BKGROUND;
	_lastClientImageState = WST_NORMAL;
	_dynamicCounter = ((style & ATOM_Widget::DynamicContent) != 0) ? 1 : 0;
	_closeButton = 0;
	_menuBar = 0;
	_verticalScrollBar = 0;
	_horizontalScrollBar = 0;
	_imagelist = 0;
	_mouseHover = false;
	_handleIdleEvent = -1;
	_scrollBarWidth = 16;
	_canvasSize.w = 0;
	_canvasSize.h = 0;
	_scrollOffset.x = 0;
	_scrollOffset.y = 0;
	_pickable = true;
	_enableDrag = false;
	_enableDrop = false;
	_enableWheelDeliver = false;
	_widgetFont = ATOM_GUIFont::invalid_handle;
	_widgetFontColor = ATOM_ColorARGB(0.f, 0.f, 0.f, 1.f);
	_widgetFontOutLineColor = ATOM_ColorARGB(0.f, 0.f, 0.f, 0.f);
	_userData = 0;
	_attributes = ATOM_NEW(ATOM_UserAttributes);
	_clientImage = NULL;
	_isExClientImage = false;
	_widgetSet.insert (this);

	//--- wangjian added ---//
	setAsyncLoad(ATOM_LoadPriority_IMMEDIATE);	// 默认为非异步加载
	_bLoadRequestSubmitted = 0;
	_bCheckWaiting = 1;							// 默认为检查等待
	//----------------------//

	show (showState);

	if (parent)
	{
		ATOM_ASSERT(ATOM_Widget::isValidWidget (parent));
		parent->attach (this);

		parent->invalidateLayout ();
		parent->updateLayout ();
	}

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_Widget::~ATOM_Widget (void)
{
	ATOM_STACK_TRACE(ATOM_Widget::~ATOM_Widget);

	widgetCount--;

	if (_guiRenderer)
	{
		_guiRenderer->notifyWidgetDeleted (this);
	}

	notifyDynamicCounter (-_dynamicCounter);

	if (_menuBar)
	{
		setMenuBar (0);
	}

	if (_parent)
	{
		_parent->detach (this);
	}

	while (_firstChild)
	{
		deleteChild (_firstChild);
	}

	while (_firstControl)
	{
		deleteChild (_firstControl);
	}

	ATOM_DELETE(_attributes);

	ATOM_WidgetLayout::deleteLayout (_layout);

	ATOM_HASHSET<ATOM_Widget*>::iterator it = _widgetSet.find (this);
	ATOM_ASSERT(it != _widgetSet.end ());
	_widgetSet.erase (it);
}

bool ATOM_Widget::isValidWidget (ATOM_Widget *widget)
{
	return widget && _widgetSet.find (widget) != _widgetSet.end ();
}

bool ATOM_Widget::validateWidget (ATOM_Widget *widget)
{
	if (!isValidWidget (widget))
	{
#if defined(USE_WIDGET_DEBUG_INFO)
		if (widget)
		{
			const ATOM_Widget::WidgetDebugInfo *debugInfo = ATOM_Widget::getWidgetDebugInfo (widget);
			static char error_buffer[2048];
			strcpy (error_buffer, "Invalid widget:\n");
			while (debugInfo)
			{
				strcat (error_buffer, ATOM_Widget::getWidgetTypeString(debugInfo->type));
				strcat (error_buffer, ":");
				debugInfo = debugInfo->parent ? ATOM_Widget::getWidgetDebugInfo (debugInfo->parent) : 0;
			}
			::MessageBoxA (ATOM_APP->getMainWindow(), error_buffer, "Fatal Error", MB_OK|MB_ICONHAND);
		}
#endif
		return false;
	}
	return true;
}

ATOM_WidgetType ATOM_Widget::getType (void) const
{
	return WT_WIDGET;
}

ATOM_GUIRenderer *ATOM_Widget::getRenderer (void) const
{
	ATOM_STACK_TRACE(ATOM_Widget::getRenderer);

	return _guiRenderer;
}

ATOM_Widget *ATOM_Widget::getParent (void) const
{
	ATOM_STACK_TRACE(ATOM_Widget::getParent);

	return _parent;
}

int ATOM_Widget::getId (void) const
{
	return _widgetId;
}

void ATOM_Widget::setId (int id)
{
	_widgetId = id;
}

ATOM_Widget *ATOM_Widget::getFirstChild (void) const
{
	return _firstChild;
}

ATOM_Widget *ATOM_Widget::getLastChild (void) const
{
	return _lastChild;
}

ATOM_Widget *ATOM_Widget::getFirstControl (void) const
{
	return _firstControl;
}

ATOM_Widget *ATOM_Widget::getLastControl (void) const
{
	return _lastControl;
}

ATOM_Widget *ATOM_Widget::getNextSibling (void) const
{
	return _next;
}

ATOM_Widget *ATOM_Widget::getPrevSibling (void) const
{
	return _prev;
}

ATOM_Widget *ATOM_Widget::getChildById (int id) const
{
	if (id == ATOM_Widget::AnyId)
	{
		return 0;
	}

	for (ATOM_Widget *c = getFirstControl(); c; c = c->getNextSibling())
	{
		if (c->getId () == id)
		{
			return c;
		}
	}
	for (ATOM_Widget *c = getFirstChild(); c; c = c->getNextSibling())
	{
		if (c->getId () == id)
		{
			return c;
		}
	}
	return 0;
}

ATOM_Widget *ATOM_Widget::getChildByIdRecursive (int id, bool hierichyOnly) const
{
	ATOM_DEQUE<ATOM_Widget*> queue;

	for (ATOM_Widget *c = getFirstControl(); c; c = c->getNextSibling())
	{
		if (c->getId() == id && (!hierichyOnly || c->_hierichyRoot == this))
		{
			return c;
		}
		else
		{
			for (ATOM_Widget *c2 = c->getFirstControl(); c2; c2 = c2->getNextSibling())
			{
				queue.push_back (c2);
			}
			for (ATOM_Widget *c2 = c->getFirstChild(); c2; c2 = c2->getNextSibling())
			{
				queue.push_back (c2);
			}
		}
	}
	for (ATOM_Widget *c = getFirstChild(); c; c = c->getNextSibling())
	{
		if (c->getId() == id && (!hierichyOnly || c->_hierichyRoot == this))
		{
			return c;
		}
		else
		{
			for (ATOM_Widget *c2 = c->getFirstControl(); c2; c2 = c2->getNextSibling())
			{
				queue.push_back (c2);
			}
			for (ATOM_Widget *c2 = c->getFirstChild(); c2; c2 = c2->getNextSibling())
			{
				queue.push_back (c2);
			}
		}
	}

	while (!queue.empty ())
	{
		ATOM_Widget *w = queue.front ();
		if (w->getId() == id && (!hierichyOnly || w->_hierichyRoot == this))
		{
			return w;
		}

		queue.pop_front ();

		for (ATOM_Widget *c = w->getFirstControl(); c; c = c->getNextSibling())
		{
			queue.push_back (c);
		}
		for (ATOM_Widget *c = w->getFirstChild(); c; c = c->getNextSibling())
		{
			queue.push_back (c);
		}
	}

	return 0;
}

ATOM_Widget *ATOM_Widget::getChildByName (const char *name) const
{
	if (!name)
	{
		return 0;
	}

	for (ATOM_Widget *c = getFirstControl(); c; c = c->getNextSibling())
	{
		if (!strcmp(c->getWidgetName(), name))
		{
			return c;
		}
	}
	for (ATOM_Widget *c = getFirstChild(); c; c = c->getNextSibling())
	{
		if (!strcmp(c->getWidgetName(), name))
		{
			return c;
		}
	}
	return 0;
}

ATOM_Widget *ATOM_Widget::getChildByNameRecursive (const char *name, bool hierichyOnly) const
{
	ATOM_DEQUE<ATOM_Widget*> queue;

	if (!name)
	{
		return 0;
	}

	for (ATOM_Widget *c = getFirstControl(); c; c = c->getNextSibling())
	{
		if (!strcmp(c->getWidgetName(), name) && (!hierichyOnly || c->_hierichyRoot == this))
		{
			return c;
		}
		else
		{
			for (ATOM_Widget *c2 = c->getFirstControl(); c2; c2 = c2->getNextSibling())
			{
				queue.push_back (c2);
			}
			for (ATOM_Widget *c2 = c->getFirstChild(); c2; c2 = c2->getNextSibling())
			{
				queue.push_back (c2);
			}
		}
	}
	for (ATOM_Widget *c = getFirstChild(); c; c = c->getNextSibling())
	{
		if (!strcmp(c->getWidgetName(), name) && (!hierichyOnly || c->_hierichyRoot == this))
		{
			return c;
		}
		else
		{
			for (ATOM_Widget *c2 = c->getFirstControl(); c2; c2 = c2->getNextSibling())
			{
				queue.push_back (c2);
			}
			for (ATOM_Widget *c2 = c->getFirstChild(); c2; c2 = c2->getNextSibling())
			{
				queue.push_back (c2);
			}
		}
	}

	while (!queue.empty ())
	{
		ATOM_Widget *w = queue.front ();
		if (!strcmp(w->getWidgetName(), name) && (!hierichyOnly || w->_hierichyRoot == this))
		{
			return w;
		}

		queue.pop_front ();

		for (ATOM_Widget *c = w->getFirstControl(); c; c = c->getNextSibling())
		{
			queue.push_back (c);
		}
		for (ATOM_Widget *c = w->getFirstChild(); c; c = c->getNextSibling())
		{
			queue.push_back (c);
		}
	}

	return 0;
}

void ATOM_Widget::deleteChild (ATOM_Widget *widget)
{
	ATOM_STACK_TRACE(ATOM_Widget::deleteChild);

	if (widget)
	{
		ATOM_ASSERT(ATOM_Widget::isValidWidget (widget));
		ATOM_ASSERT(widget->getParent() == this);
		detach (widget);
		ATOM_DELETE(widget);
	}
}

void ATOM_Widget::clearChildren()
{
	ATOM_STACK_TRACE(ATOM_Widget::clearChildren);

	while (_firstChild)
	{
		deleteChild (_firstChild);
	}

	while (_firstControl)
	{
		deleteChild (_firstControl);
	}
}

ATOM_Point2Di *ATOM_Widget::clientToScreen (ATOM_Point2Di *point) const
{
	ATOM_STACK_TRACE(ATOM_Widget::clientToScreen);

	if (point)
	{
		point->x += _clientRect.point.x;
		point->y += _clientRect.point.y;

		for (const ATOM_Widget *w = this; w ; w = w->_parent)
		{
			point->x += w->_widgetRect.point.x;
			point->y += w->_widgetRect.point.y;
		}
	}

	return point;
}

ATOM_Point2Di *ATOM_Widget::screenToClient (ATOM_Point2Di *point) const
{
	ATOM_STACK_TRACE(ATOM_Widget::screenToClient);

	if (point)
	{
		ATOM_Point2Di p(0, 0);
		clientToScreen (&p);
		point->x -= p.x;
		point->y -= p.y;
	}
	return point;
}

ATOM_Point2Di *ATOM_Widget::clientToViewport (ATOM_Point2Di *point) const
{
	ATOM_STACK_TRACE(ATOM_Widget::clientToViewport);

#if 1
	if (point)
	{
		ATOM_TopWindow *wRT = _guiRenderer->getRenderTargetWindow();
		point->x += _clientRect.point.x;
		point->y += _clientRect.point.y;

		for (const ATOM_Widget *w = this; w != wRT; w = w->_parent)
		{
			point->x += w->_widgetRect.point.x;
			point->y += w->_widgetRect.point.y;
		}

		if (wRT == 0)
		{
			*point += _guiRenderer->getViewport().point;
		}
	}

	return point;
#else
	if (point)
	{
		clientToScreen (point);
		point->x -= _guiRenderer->getDrawRegion().x;
		point->y -= _guiRenderer->getDrawRegion().y;
	}
	return point;
#endif
}

ATOM_Point2Di *ATOM_Widget::viewportToClient (ATOM_Point2Di *point) const
{
 	ATOM_STACK_TRACE(ATOM_Widget::viewportToClient);

	if (point)
	{
		ATOM_Point2Di p(0, 0);
		clientToViewport (&p);
		point->x -= p.x;
		point->y -= p.y;
	}
	return point;
}

ATOM_Point2Di *ATOM_Widget::clientToGUI (ATOM_Point2Di *point) const
{
	ATOM_STACK_TRACE(ATOM_Widget::clientToGUI);

	if (point)
	{
		clientToScreen (point);
		point->x += _guiRenderer->getViewport().point.x;
		point->y += _guiRenderer->getViewport().point.y;
	}
	return point;
}

ATOM_Point2Di *ATOM_Widget::GUIToClient (ATOM_Point2Di *point) const
{
	if (point)
	{
		ATOM_Point2Di p(0, 0);
		clientToGUI (&p);
		point->x -= p.x;
		point->y -= p.y;
	}
	return point;
}

void ATOM_Widget::setFlags (unsigned state)
{
	_stateFlags = state;
}

unsigned ATOM_Widget::getFlags (void) const
{
	return _stateFlags;
}

void ATOM_Widget::setStyle (unsigned style)
{
	if (style != _creationStyle)
	{
		int ctl1 = (style & ATOM_Widget::Control) ? 1 : 0;
		int ctl2 = (_creationStyle & ATOM_Widget::Control) ? 1 : 0;
		int layoutable1 = (style & ATOM_Widget::NonLayoutable) ? 0 : 1;
		int layoutable2 = (_creationStyle & ATOM_Widget::NonLayoutable) ? 0 : 1;
		if (ctl1 != ctl2 && _parent)
		{
			ATOM_WidgetLayout::Unit *savedUnit = _parent->getLayout()->allocUnit ();
			*savedUnit = *getLayoutUnit ();
			ATOM_Widget *parent = _parent;
			parent->detach (this);

			_creationStyle = style;

			parent->attach (this);
			*getLayoutUnit() = *savedUnit;
			_parent->getLayout()->freeUnit (savedUnit);
		}
		else
		{
			_creationStyle = style;
		}

		calcLayout ();

		if (layoutable1 != layoutable2)
		{
			_parent->invalidateLayout ();
			_parent->updateLayout ();
		}
	}
}

unsigned ATOM_Widget::getStyle (void) const
{
	return _creationStyle;
}

bool ATOM_Widget::isControl (void) const
{
	return (_creationStyle & ATOM_Widget::Control) != 0;
}

bool ATOM_Widget::isPrivate (void) const
{
	return (_creationStyle & ATOM_Widget::Private) != 0;
}

bool ATOM_Widget::isPopup (void) const
{
	return (_creationStyle & ATOM_Widget::Popup) != 0;
}

bool ATOM_Widget::isMenu (void) const
{
	return false;
}

bool ATOM_Widget::isTopWindow (void) const
{
	return false;
}

void ATOM_Widget::hitTest (int x, int y, ATOM_GUIHitTestResult *result, bool fullTest)
{
	ATOM_STACK_TRACE(ATOM_Widget::hitTest);

	//--- wangjian modified ---//
	// 异步加载相关
	if (_showState == ATOM_Widget::Hide /*|| !isEnabled() */|| _bWaiting)
	{
		result->hitPoint = HitNone;
		result->child = 0;
		result->lug = false;
		return;
	}

	if (result)
	{
		for (ATOM_Widget *widget = _lastChild; widget; widget = widget->_prev)
		{
			widget->hitTest (x - widget->_widgetRect.point.x, y - widget->_widgetRect.point.y, result, fullTest);
			if (result->hitPoint != HitNone)
			{
				result->hitPoint = HitChild;
				result->child = widget;
				result->lug = false;
				return;
			}
		}

		if (_showState != ATOM_Widget::Hide)
		{
			bool insideWidget = (x >= 0 && x < _widgetRect.size.w && y >= 0 && y < _widgetRect.size.h);
			bool insideClient = (x >= _clientRect.point.x && x < _clientRect.size.w + _clientRect.point.x && y >= _clientRect.point.y && y < _clientRect.size.h + _clientRect.point.y);

			for (ATOM_Widget *widget = _lastControl; widget; widget = widget->_prev)
			{
				if ((widget->_creationStyle & ATOM_Widget::NoClip) == 0)
				{
					if (!fullTest && !insideWidget)
					{
						continue;
					}
					else if (!fullTest && (_creationStyle & ATOM_Widget::ClipChildren) != 0 && !insideClient)
					{
						continue;
					}
				}

				widget->hitTest (x - widget->_widgetRect.point.x, y - widget->_widgetRect.point.y, result, fullTest);
				if (result->hitPoint != HitNone)
				{
					result->hitPoint = HitChild;
					result->child = widget;
					result->lug = false;
					return;
				}
			}

			if (insideWidget && isPickable())
			{
				ATOM_WidgetHitTestEvent e(x, y);
				handleEvent (&e);
				if (!e.wasHandled ())
				{
					result->hitPoint = HitNone;
					result->child = 0;
					result->lug = false;
				}
				else
				{
					result->hitPoint = e.hitTestResult.hitPoint;
					result->child = e.hitTestResult.child;
					result->lug = e.hitTestResult.lug;
				}
			}
			else
			{
				result->hitPoint = HitNone;
				result->child = 0;
				result->lug = false;
			}
		}
		else
		{
			result->hitPoint = HitNone;
			result->child = 0;
			result->lug = false;
		}
	}
}

void ATOM_Widget::bringToFront (void)
{
	ATOM_STACK_TRACE(ATOM_Widget::bringToFront);

	ATOM_Widget *w = this;
	while (w && w->isControl ())
	{
		w = w->_parent;
	}

	ATOM_Widget *parent = w->_parent;

	if (parent)
	{
		parent->bringChildToFront (w);
		parent->bringToFront ();
	}
}

bool ATOM_Widget::attach (ATOM_Widget *widget)
{
	ATOM_STACK_TRACE(ATOM_Widget::attach);

	if (widget)
	{
		ATOM_ASSERT(ATOM_Widget::isValidWidget (widget));

		bool asControl = widget->isControl ();

		if (widget->_parent != this)
		{
			if (asControl)
			{
				attachControl (widget);
				notifyDynamicCounter (widget->_dynamicCounter);
			}
			else
			{
				attachNonControl (widget);
			}

			ATOM_WidgetLayout::Unit *unit = _layout->allocUnit ();
			unit->desired_x = widget->_widgetRect.point.x;
			unit->desired_y = widget->_widgetRect.point.y;
			unit->desired_w = widget->_widgetRect.size.w;
			unit->desired_h = widget->_widgetRect.size.h;
			unit->widget = widget;
			widget->_layoutUnit = unit;

			widget->_guiRenderer = _guiRenderer;
		}

		return true;
	}
	return false;
}

void ATOM_Widget::detach (ATOM_Widget *widget)
{
	ATOM_STACK_TRACE(ATOM_Widget::detach);

	if (widget)
	{
		ATOM_ASSERT(ATOM_Widget::isValidWidget (widget));

		if (widget->_parent == this)
		{
			if (widget->isControl ())
			{
				detachControl (widget);
				notifyDynamicCounter (-widget->_dynamicCounter);
			}
			else
			{
				detachNonControl (widget);
			}

			_layout->freeUnit (widget->_layoutUnit);
			widget->_layoutUnit = 0;
		}

		releaseCapture ();
	}
}

void ATOM_Widget::onHitTest (ATOM_WidgetHitTestEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Widget::onHitTest);

	int borderSize = _clientRect.point.x;
	int titleBarHeight = _clientRect.point.y - _titleBarRect.point.y;
	int x = event->x;
	int y = event->y;
	ATOM_GUIHitTestResult *result = &event->hitTestResult;

	if ((x < borderSize || x > _widgetRect.size.w - borderSize - 1) && (y < borderSize || y > _widgetRect.size.h - borderSize - 1))
	{
		result->hitPoint = HitFrame;
		result->child = 0;
		result->lug = false;
	}
	else if (y < titleBarHeight)
	{
		result->hitPoint = HitFrame;
		result->child = 0;
		result->lug = true;
	}
	else if (x < _clientRect.size.w + _clientRect.point.x && y < _clientRect.size.h + _clientRect.point.y)
	{
		result->hitPoint = HitClient;
		result->child = 0;
		result->lug = false;
	}
	else
	{
		result->hitPoint = HitFrame;
		result->child = 0;
		result->lug = false;
	}
}

void ATOM_Widget::moveTo (int x, int y)
{
	ATOM_STACK_TRACE(ATOM_Widget::moveTo);

	if (_parent && isLayoutable ())
	{
		_layoutUnit->desired_x = x;
		_layoutUnit->desired_y = y;

		_parent->invalidateLayout ();
		_parent->updateLayout ();
	}
	else if (x != _widgetRect.point.x || y != _widgetRect.point.y)
	{
		_widgetRect.point.x = x;
		_widgetRect.point.y = y;
	}

	if (isControl ())
	{
		invalidate ();
	}
}

void ATOM_Widget::moveBy (int x, int y)
{
	ATOM_Rect2Di layoutRect = getLayoutRect ();
	moveTo (layoutRect.point.x + x, layoutRect.point.y + y);
}

bool ATOM_Widget::enableIME (void) const
{
	ATOM_STACK_TRACE(ATOM_Widget::enableIME);

	return _showState != ATOM_Widget::Hide && isEnabled () && supportIME ();
}

void ATOM_Widget::draw (void)
{
	ATOM_STACK_TRACE(ATOM_Widget::draw);

	if (_showState != ATOM_Widget::Hide)
	{
		//--- wangjian added ---//
		// 异步加载相关：如果没有加载完成
		bool bAsyncload = ( ATOM_GUIRenderer::isMultiThreadingEnabled() ) && ( _loadPriority != ATOM_LoadPriority_IMMEDIATE );
		if( bAsyncload && _bWaiting != WIDGET_LOAD_NOWAITING )
		{
			// 如果还未提交加载请求，提交
			submitLoadRequest();

			// 检查是否需要等待
			if( !checkWaiting() )
			{
				// 显示等待UI
				if( WAITING_MODE_SHOWWAITINGUI == _WaitingMode )
					drawWaitingImage();

				return;
			}
		}
		//----------------------//

		drawSelf ();
		drawChildren ();
		if (_creationStyle & ATOM_Widget::Overlay)
		{
			drawOverlay ();
		}
	}
}

void ATOM_Widget::drawOverlay (void)
{
	ATOM_GUICanvas canvas(this);

	ATOM_Rect2Di rcClient = _clientRect;
	const ATOM_Rect2Di rc(ATOM_Point2Di(0,0), rcClient.size);

	rcClient.point.x = 0;
	rcClient.point.y = 0;
	clientToViewport (&rcClient.point);
	canvas.resize (rcClient.size);
	canvas.setOrigin (rcClient.point);
	canvas.setTargetRect (rcClient);

	ATOM_WidgetDrawOverlayEvent drawOverlayEvent(&canvas);
	handleEvent (&drawOverlayEvent);
}

ATOM_GUIImage *ATOM_Widget::getValidImage (int imageId)
{
	for (ATOM_Widget *w = this; w; w = w->getParent())
	{
		ATOM_GUIImageList *imagelist = w->getImageList();
		if (!imagelist)
		{
			continue;
		}

		ATOM_GUIImage *image = imagelist->getImage (imageId);
		if (image)
		{
			return image;
		}
	}

	return ATOM_GUIImageList::getDefaultImageList().getImage(ATOM_IMAGEID_WINDOW_BKGROUND);
}

const ATOM_GUIImageList *ATOM_Widget::getValidImageList (void)
{
	ATOM_Widget *w = this;
	ATOM_GUIImageList *imagelist = _imagelist.get();
	while (!imagelist)
	{
		w = w->getParent();
		if (!w)
		{
			break;
		}
		imagelist = w->_imagelist.get();
	}
	return imagelist ? imagelist : &ATOM_GUIImageList::getDefaultImageList();
}

void ATOM_Widget::drawClient (ATOM_GUICanvas *canvas, ATOM_WidgetState state)
{
	ATOM_STACK_TRACE(ATOM_Widget::drawClient);

	if (_clientImageId != ATOM_INVALID_IMAGEID)
	{
#if 1
		
		if(!_isExClientImage || !_clientImage)
		{
			_clientImage = getValidImage (_clientImageId);
			_isExClientImage = false;
		}
		if (_clientImage)
		{
			_clientImage->draw (state, canvas, ATOM_Rect2Di(0, 0, canvas->getSize().w, canvas->getSize().h));
		}
#else
		const ATOM_GUIImageList *imagelist = getValidImageList ();
		if (imagelist)
		{
			ATOM_GUIImage *image = imagelist->getImage (_clientImageId);
			if (image)
			{
				image->draw (state, canvas, ATOM_Rect2Di(0, 0, canvas->getSize().w, canvas->getSize().h));
			}
		}
#endif
	}
}

void ATOM_Widget::drawFrame (ATOM_GUICanvas *canvas, ATOM_WidgetState state)
{
	ATOM_STACK_TRACE(ATOM_Widget::drawFrame);

	unsigned style = getStyle ();

	if ((style & ATOM_Widget::TitleBar) != 0)
	{
		ATOM_Rect2Di rc(_titleBarRect.point, _titleBarRect.size);
		canvas->fillRect (rc, ATOM_ColorARGB(1.f, 0.f, 1.f, 1.f));
	}

	if ((style & ATOM_Widget::Border) != 0)
	{
		ATOM_Widget::BorderMode borderMode = getBorderMode ();
		if (borderMode == ATOM_Widget::Flat)
		{
			ATOM_Rect2Di rc;
			rc.point.x = 0;
			rc.point.y = 0;
			rc.size = _widgetRect.size - ATOM_Size2Di(1, 1);
			canvas->drawRect (rc, ATOM_ColorARGB(0.f, 0.f, 0.f, 1.f), 1);
		}
		else
		{
			int l = 0;
			int t = 0;
			int r = _widgetRect.size.w - 1;
			int b = _widgetRect.size.h - 1;

			static const ATOM_ColorARGB lcolor(0.7f, 0.7f, 0.7f, 1.f);
			static const ATOM_ColorARGB dcolor(0.3f, 0.3f, 0.3f, 1.f);

			ATOM_ColorARGB c1, c2;
			if (borderMode == ATOM_Widget::Raise && state != WST_HOLD)
			{
			  c1 = lcolor;
			  c2 = dcolor;
			}
			else
			{
			  c1 = dcolor;
			  c2 = lcolor;
			}

			ATOM_Point2Di start[4] = { ATOM_Point2Di(l, t), ATOM_Point2Di(l, t), ATOM_Point2Di(r, t), ATOM_Point2Di(l, b) };
			ATOM_Point2Di end[4] = { ATOM_Point2Di(r, t), ATOM_Point2Di(l, b), ATOM_Point2Di(r, b), ATOM_Point2Di(r, b) };
			ATOM_ColorARGB colors[4] = { c1, c1, c2, c2 };

			canvas->drawLineList (4, start, end, colors);
			//canvas->drawLine (ATOM_Point2Di(l, t), ATOM_Point2Di(r, t), c1);
			//canvas->drawLine (ATOM_Point2Di(l, t), ATOM_Point2Di(l, b), c1);
			//canvas->drawLine (ATOM_Point2Di(r, t), ATOM_Point2Di(r, b), c2);
			//canvas->drawLine (ATOM_Point2Di(l, b), ATOM_Point2Di(r, b), c2);
		}
	}
}

void ATOM_Widget::drawSelf (void)
{
	ATOM_STACK_TRACE(ATOM_Widget::drawSelf);

	ATOM_Rect2Di rcWidget = _widgetRect;
	rcWidget.point.x = -_clientRect.point.x;
	rcWidget.point.y = -_clientRect.point.y;
	clientToViewport (&rcWidget.point);

	ATOM_GUICanvas canvas(this);
	canvas.resize (rcWidget.size);
	canvas.setOrigin (rcWidget.point);

	ATOM_WidgetDrawFrameEvent drawFrameEvent(&canvas, WST_NORMAL);
	handleEvent (&drawFrameEvent);

	ATOM_Rect2Di rcClient = _clientRect;
	const ATOM_Rect2Di rc(ATOM_Point2Di(0,0), rcClient.size);

	rcClient.point.x = 0;
	rcClient.point.y = 0;
	clientToViewport (&rcClient.point);

	int cw = (_canvasSize.w < rcClient.size.w) ? rcClient.size.w : _canvasSize.w;
	int ch = (_canvasSize.h < rcClient.size.h) ? rcClient.size.h : _canvasSize.h;
	canvas.resize (ATOM_Size2Di(cw, ch));

	canvas.setOrigin (rcClient.point - _scrollOffset);
	ATOM_Size2Di targetSize(rcClient.size.w, rcClient.size.h);
	canvas.setTargetRect (ATOM_Rect2Di(_scrollOffset, targetSize));

	ATOM_RenderDevice *device = ATOM_GetRenderDevice();
	ATOM_ASSERT(device);
	int vx, vy, vw, vh;
	device->getViewport (device->getCurrentView(), &vx, &vy, &vw, &vh);

	ATOM_Point2Di pt(0,0);
	clientToViewport (&pt);
	ATOM_Rect2Di rcScissor(pt, _clientRect.size);

	if (_guiRenderer->pushScissor (rcScissor, true))
	{
		ATOM_WidgetDrawClientEvent drawClientEvent(&canvas, _lastClientImageState);
		handleEvent (&drawClientEvent);
		_guiRenderer->popScissor ();
	}

	bool clipChildren = (_creationStyle & ATOM_Widget::ClipChildren) != 0;
	if (clipChildren)
	{
		if (_guiRenderer->pushScissor (rcScissor, true))
		{
			drawControls ();
			_guiRenderer->popScissor ();
		}
	}
	else
	{
		drawControls ();
	}
}

void ATOM_Widget::drawControls (void)
{
	ATOM_STACK_TRACE(ATOM_Widget::drawControls);

	for (ATOM_Widget *w = getFirstControl(); w; w = w->getNextSibling())
	{
		if ((w->_creationStyle & ATOM_Widget::NoClip) == 0 && w->_dynamicCounter == 0)
		{
			ATOM_Rect2Di rc = w->getWidgetRect();
			rc.point.x += _widgetRect.point.x;
			rc.point.y += _widgetRect.point.y;
			if (rc.intersectedWith (_widgetRect))
			{
				w->draw ();
			}
		}
	}
}

void ATOM_Widget::drawChildren (void)
{
	ATOM_STACK_TRACE(ATOM_Widget::drawChildren);

	for (ATOM_Widget *w = getFirstControl(); w; w = w->getNextSibling())
	{
		if ((w->_creationStyle & ATOM_Widget::NoClip) != 0 || w->_dynamicCounter != 0)
		{
			w->draw ();
		}
	}

	for (ATOM_Widget *w = getFirstChild(); w; w = w->getNextSibling())
	{
		w->draw ();
	}

}

void ATOM_Widget::resize (const ATOM_Rect2Di &rect)
{
	ATOM_Rect2Di oldClientRect = _clientRect;

	if (_parent && isLayoutable ())
	{
		_layoutUnit->desired_x = rect.point.x;
		_layoutUnit->desired_y = rect.point.y;
		_layoutUnit->desired_w = rect.size.w;
		_layoutUnit->desired_h = rect.size.h;

		_parent->invalidateLayout ();
		_parent->updateLayout ();
	}
	else
	{
		ATOM_WidgetResizeEvent e(getId(), rect);
		handleEvent (&e);
	}

	/*
	ATOM_Rect2Di newWidgetRect (_layoutUnit->actual_x, _layoutUnit->actual_y, _layoutUnit->actual_w, _layoutUnit->actual_h);

	ATOM_WidgetResizeEvent e(getId(), rect);
	handleEvent (&e);
	*/

	if (oldClientRect != _clientRect)
	{
		if (_menuBar)
		{
			ATOM_MenuBar *mb = _menuBar;
			setMenuBar (0);
			setMenuBar (mb);
		}
		_layout->setRect (_clientRect);
	}

}

void ATOM_Widget::invalidate (void)
{
	if (_showState != ATOM_Widget::Hide)
	{
		ATOM_Widget *w = this;
		while (w)
		{
			if (!w->isControl ())
			{
				w->invalidateDisplay ();
				return;
			}
			w = w->_parent;
		}
	}
}

void ATOM_Widget::invalidateDisplay (void)
{
}

void ATOM_Widget::show (ATOM_Widget::ShowState showState)
{
	ATOM_STACK_TRACE(ATOM_Widget::show);

//	// wangjian 
//#if 1
//	if( ShowNormal == showState )
//		ATOM_LOGGER::log(" %s is show now! \n", _name.c_str());
//	else if( Hide == showState )
//		ATOM_LOGGER::log(" %s is hide now! \n", _name.c_str());
//#endif

	if (showState != _showState)
	{
		if (showState == ATOM_Widget::Hide)
		{
			releaseCapture ();

			ATOM_Widget *focusWidget = _guiRenderer->getFocus();
			while (focusWidget && focusWidget != this)
			{
				focusWidget = focusWidget->getParent();
			}

			if (focusWidget == this)
			{
				_guiRenderer->setFocus (0);
			}
		}

		_showState = showState;

		if (_guiRenderer)
		{
			_guiRenderer->invalidate ();
		}

		if (_parent)
		{
			_parent->invalidateLayout ();
		}
	}
}

void ATOM_Widget::attachNonControl (ATOM_Widget *widget)
{
	ATOM_STACK_TRACE(ATOM_Widget::attachNonControl);

	ATOM_ASSERT(ATOM_Widget::isValidWidget (widget));
	widget->_guiRenderer = _guiRenderer;
	widget->_parent = this;

	ATOM_Widget *last = _lastChild;
	if ((widget->_stateFlags & ATOM_Widget::TopMost) == 0)
	{
		while (last && (last->_stateFlags & ATOM_Widget::TopMost) != 0)
		{
			last = last->_prev;
		}
	}

	if (!last)
	{
		widget->_prev = 0;
		widget->_next = _firstChild;
		if (_firstChild)
		{
			_firstChild->_prev = widget;
		}
		_firstChild = widget;
	}
	else
	{
		widget->_prev = last;
		widget->_next = last->_next;
		if (widget->_next)
		{
			widget->_next->_prev = widget;
		}
		last->_next = widget;
	}

	if (_lastChild == last || _lastChild == 0)
	{
		_lastChild = widget;
	}
}

void ATOM_Widget::attachControl (ATOM_Widget *widget)
{
	ATOM_STACK_TRACE(ATOM_Widget::attachControl);

	ATOM_ASSERT(ATOM_Widget::isValidWidget (widget));
	widget->_guiRenderer = _guiRenderer;
	widget->_parent = this;
	_dynamicCounter += widget->_dynamicCounter;

	ATOM_Widget *last = _lastControl;

	if (!last)
	{
		widget->_prev = 0;
		widget->_next = 0;
		_firstControl = widget;
		_lastControl = widget;
	}
	else
	{
		last->_next = widget;
		widget->_prev = last;
		widget->_next = 0;
		_lastControl = widget;
	}

	if (widget->getShowState() != ATOM_Widget::Hide)
	{
		invalidate ();
	}
}

void ATOM_Widget::detachNonControl (ATOM_Widget *widget)
{
	ATOM_STACK_TRACE(ATOM_Widget::detachNonControl);

	ATOM_ASSERT(ATOM_Widget::isValidWidget (widget));
	ATOM_Widget *next = widget->_next;
	ATOM_Widget *prev = widget->_prev;
	widget->_parent = 0;

	if (!prev)
	{
		_firstChild = next;
	}
	else
	{
		prev->_next = next;
	}

	if (!next)
	{
		_lastChild = prev;
	}
	else
	{
		next->_prev = prev;
	}
}

void ATOM_Widget::bringChildToFront (ATOM_Widget *widget)
{
	bool asControl = widget->isControl ();

	if (widget->isControl())
	{
		detachControl (widget);
		attachControl (widget);
	}
	else
	{
		detachNonControl (widget);
		attachNonControl (widget);
	}
}

void ATOM_Widget::detachControl (ATOM_Widget *widget)
{
	ATOM_STACK_TRACE(ATOM_Widget::detachControl);

	ATOM_ASSERT(ATOM_Widget::isValidWidget(widget));
	ATOM_Widget *next = widget->_next;
	ATOM_Widget *prev = widget->_prev;
	widget->_parent = 0;

	if (!prev)
	{
		_firstControl = next;
	}
	else
	{
		prev->_next = next;
	}

	if (!next)
	{
		_lastControl = prev;
	}
	else
	{
		next->_prev = prev;
	}

	invalidate ();
}

void ATOM_Widget::notifyDynamicCounter (int change)
{
	ATOM_STACK_TRACE(ATOM_Widget::notifyDynamicCounter);

	_dynamicCounter += change;

	if (isControl () && _parent)
	{
		_parent->notifyDynamicCounter (change);
	}
}

void ATOM_Widget::trackPopupMenu (int x, int y, ATOM_PopupMenu *popupMenu)
{
	ATOM_STACK_TRACE(ATOM_Widget::trackPopupMenu);

	if (popupMenu)
	{
		if (popupMenu->getParent())
		{
			popupMenu->collapse();
		}

		ATOM_Point2Di pt(0, 0);
		clientToScreen (&pt);
		_guiRenderer->getDesktop()->attach (popupMenu);

		popupMenu->resize (ATOM_Rect2Di(x + pt.x, y + pt.y, 100, 100));
		popupMenu->setSelected (-1);
		popupMenu->show (ATOM_Widget::ShowNormal);
		popupMenu->bringToFront ();
		popupMenu->setTrackWidget (this);
		getRenderer()->setFocus (popupMenu);
	}
}

void ATOM_Widget::setCapture (void)
{
	ATOM_STACK_TRACE(ATOM_Widget::setCapture);

	if (_guiRenderer)
	{
		_guiRenderer->setCapture (this);
	}
}

void ATOM_Widget::releaseCapture (void)
{
	ATOM_STACK_TRACE(ATOM_Widget::releaseCapture);

	if (isCaptured ())
	{
		_guiRenderer->setCapture (0);
	}
}

bool ATOM_Widget::isCaptured (void) const
{
	ATOM_STACK_TRACE(ATOM_Widget::isCaptured);

	return _guiRenderer && _guiRenderer->getCapture () == this;
}

ATOM_Widget::ShowState ATOM_Widget::getShowState (void) const
{
	return _showState;
}

void ATOM_Widget::setBorderMode (ATOM_Widget::BorderMode mode)
{
	ATOM_STACK_TRACE(ATOM_Widget::setBorderMode);

	if (mode != _borderMode)
	{
		_borderMode = mode;

		if (_showState != ATOM_Widget::Hide)
		{
			invalidate ();
		}
	}
}

ATOM_Widget::BorderMode ATOM_Widget::getBorderMode (void) const
{
	ATOM_STACK_TRACE(ATOM_Widget::getBorderMode);

	return _borderMode;
}

void ATOM_Widget::enable (bool enable)
{
	ATOM_STACK_TRACE(ATOM_Widget::enable);

	bool enabledOld = isEnabled ();

	if (enable)
	{
		_stateFlags &= ~ATOM_Widget::Disabled;
	}
	else
	{
		_stateFlags |= ATOM_Widget::Disabled;
	}

	bool enabledNew = isEnabled ();

	if (enabledOld && !enabledNew)
	{
		temporalDisable ();

		invalidate ();
	}
	else if (!enabledOld && enabledNew)
	{
		temporalEnable ();

		invalidate ();
	}
}

bool ATOM_Widget::isEnabled (void) const
{
	return (_stateFlags & ATOM_Widget::Disabled) == 0 && (!isControl() || !_parent || _parent->isEnabled());
}

void ATOM_Widget::setPickable(bool pickable)
{
	ATOM_STACK_TRACE(ATOM_Widget::setPickable);

	_pickable = pickable;
}

bool ATOM_Widget::isPickable() const
{
	ATOM_STACK_TRACE(ATOM_Widget::isPickable);

	return _pickable;
}

void ATOM_Widget::temporalEnable (void)
{
	ATOM_STACK_TRACE(ATOM_Widget::temporalEnable);

	for (ATOM_Widget *w = getFirstControl(); w; w = w->getNextSibling())
	{
		w->_stateFlags &= ~ATOM_Widget::TemporalDisabled;

		if (w->isEnabled ())
		{
			w->temporalEnable ();
		}
	}
}

void ATOM_Widget::temporalDisable (void)
{
	ATOM_STACK_TRACE(ATOM_Widget::temporalDisable);

	for (ATOM_Widget *w = getFirstControl(); w; w = w->getNextSibling())
	{
		bool enabled = w->isEnabled ();

		w->_stateFlags |= ATOM_Widget::TemporalDisabled;

		if (enabled)
		{
			w->temporalDisable ();
		}
	}
}

void ATOM_Widget::setImageList (ATOM_GUIImageList *imagelist)
{
	if (imagelist != _imagelist.get())
	{
		_imagelist = imagelist;
		invalidate ();
	}
}

ATOM_GUIImageList *ATOM_Widget::getImageList (void) const
{
	return _imagelist.get();
}

void ATOM_Widget::updateMouseState (int x, int y, bool _leftDown, bool _middleDown, bool _rightDown)
{
	ATOM_WidgetState state;

	if (!isEnabled ())
	{
		state = WST_DISABLED;
	}
	else
	{
		if (_mouseHover)
		{
			if (_leftDown)
			{
				state = WST_HOLD;
			}
			else
			{
				state = WST_HOVER;
			}
		}
		else
		{
			state = WST_NORMAL;
		}
	}

	if (state != _lastClientImageState)
	{
		if (_clientImageId != ATOM_INVALID_IMAGEID)
		{
#if 1
			ATOM_GUIImage *image = getValidImage (_clientImageId);
#else
			const ATOM_GUIImageList *imagelist = getValidImageList ();
			ATOM_GUIImage *image = imagelist->getImage (_clientImageId);
#endif
			if (!image)
			{
				image = ATOM_GUIImageList::getDefaultImageList().getImage (ATOM_IMAGEID_WINDOW_BKGROUND);
				ATOM_ASSERT(image);
			}
			if (_lastClientImageState == WST_UNKNOWN)
			{
				invalidate ();
			}
		}

		_lastClientImageState = state;
	}
}

void ATOM_Widget::playSound(int id)
{
	ATOM_STACK_TRACE(ATOM_Widget::playSound);

	if(INVALID_AUDIOID != id)
	{
		ATOM_AudioDeviceWrapper *device = getRenderer()->getAudioDevice();
		if(device)
		{
			device->playSound(id);
		}
	}
}

void ATOM_Widget::setClientImageId (int id)
{
	ATOM_STACK_TRACE(ATOM_Widget::setClientImageId);

	if (id != _clientImageId)
	{
		//--- wangjian added ---//
		// 如果不是第一次设置
		if( _clientImageId != ATOM_IMAGEID_CONTROL_BKGROUND && 
			_clientImageId != ATOM_IMAGEID_WINDOW_BKGROUND )
			reWaiting();
		//----------------------//

		_clientImageId = id;

		invalidate ();
	}
}

int ATOM_Widget::getClientImageId (void) const
{
	ATOM_STACK_TRACE(ATOM_Widget::getClientImageId);

	return _clientImageId;
}

void ATOM_Widget::setscrollBarWidth (int w)
{
	ATOM_STACK_TRACE(ATOM_Widget::setscrollBarWidth);

	if (w != _scrollBarWidth)
	{
		_scrollBarWidth = w;
		calcLayout();
		invalidate ();
	}
}

int ATOM_Widget::getscrollBarWidth (void) const
{
	return _scrollBarWidth;
}

bool ATOM_Widget::isMouseHover (void) const
{
	return _mouseHover;
}

void ATOM_Widget::setMouseHover (bool hover)
{
	_mouseHover = hover;
}

ATOM_WidgetState ATOM_Widget::getWidgetState (void) const
{
	return _lastClientImageState;
}

void ATOM_Widget::calcLayout (void)
{
	const ATOM_Rect2Di *rcWidget = &_widgetRect;
	ATOM_Rect2Di *rcClient = &_clientRect;
	ATOM_Rect2Di *rcTitleBar = &_titleBarRect;

	ATOM_ASSERT(rcWidget);
	ATOM_ASSERT(rcClient);
	ATOM_ASSERT(rcTitleBar);

	bool hasBorder = (_creationStyle & ATOM_Widget::Border) != 0;
	bool hasTitleBar = (_creationStyle & ATOM_Widget::TitleBar) != 0;
	bool hasVScroll = (_creationStyle & ATOM_Widget::VScroll) != 0;
	bool hasHScroll = (_creationStyle & ATOM_Widget::HScroll) != 0;

	int borderW = hasBorder ? 1 : 0;
	int borderH = hasBorder ? 1 : 0;
	int titleBarH = hasTitleBar ? 18 : 0;

	rcTitleBar->point.x = borderW;
	rcTitleBar->point.y = borderH;
	rcTitleBar->size.w = rcWidget->size.w - 2 * borderW;
	rcTitleBar->size.h = titleBarH;

	rcClient->point.x = rcTitleBar->point.x;
	rcClient->point.y = rcTitleBar->point.y + titleBarH;
	rcClient->size.w = rcTitleBar->size.w;
	rcClient->size.h = rcWidget->size.h - borderH * 2 - titleBarH;

	if (hasTitleBar)
	{
		if ((_creationStyle & ATOM_Widget::CloseButton) != 0)
		{
			if (!_closeButton)
			{
				_closeButton = ATOM_NEW(WidgetCloseButton, this, ATOM_Rect2Di(rcTitleBar->point.x + rcTitleBar->size.w - 3 - 13, rcTitleBar->point.y + 3, 13, 13), int(ATOM_Widget::Border|ATOM_Widget::Control), int(ATOM_Widget::CloseButtonId), ATOM_Widget::ShowNormal);
			}
			else
			{
				_closeButton->moveTo (rcTitleBar->point.x + rcTitleBar->size.w - 3 - 13, rcTitleBar->point.y + 3);
			}
		}
		else
		{
			if (_closeButton)
			{
				ATOM_DELETE(_closeButton);
				_closeButton = 0;
			}
		}
	}
	else if (_closeButton)
	{
		ATOM_DELETE(_closeButton);
		_closeButton = 0;
	}

	if (_menuBar)
	{
		_menuBar->resize (ATOM_Rect2Di(_menuBar->getWidgetRect().point, rcClient->size));

		rcClient->point.y += _menuBar->getWidgetRect().size.h;
		rcClient->size.h -= _menuBar->getWidgetRect().size.h;
	}

	ATOM_Rect2Di vscrollRect;
	ATOM_Rect2Di hscrollRect;

	if (hasVScroll)
	{
		if(_creationStyle & ATOM_Widget::LeftScroll)
		{
			vscrollRect.point.x = rcClient->point.x;
			vscrollRect.point.y = rcClient->point.y;
			vscrollRect.size.w = _scrollBarWidth;
			vscrollRect.size.h = rcClient->size.h - (hasHScroll ? _scrollBarWidth : 0);

			rcClient->point.x += _scrollBarWidth;
			rcClient->size.w -= _scrollBarWidth;
		}
		else
		{
			vscrollRect.point.x = rcClient->point.x + rcClient->size.w - _scrollBarWidth;
			vscrollRect.point.y = rcClient->point.y;
			vscrollRect.size.w = _scrollBarWidth;
			vscrollRect.size.h = rcClient->size.h - (hasHScroll ? _scrollBarWidth : 0);

			rcClient->size.w -= _scrollBarWidth;
		}
	}

	if (hasHScroll)
	{
		if(_creationStyle & ATOM_Widget::TopScroll)
		{
			hscrollRect.point.x = rcClient->point.x;
			hscrollRect.point.y = rcClient->point.y;
			hscrollRect.size.w = rcClient->size.w;
			hscrollRect.size.h = _scrollBarWidth;

			rcClient->point.y += _scrollBarWidth;
			rcClient->size.h -= _scrollBarWidth;
		}
		else
		{
			hscrollRect.point.x = rcClient->point.x;
			hscrollRect.point.y = rcClient->point.y + rcClient->size.h - _scrollBarWidth;
			hscrollRect.size.w = rcClient->size.w;
			hscrollRect.size.h = _scrollBarWidth;

			rcClient->size.h -= _scrollBarWidth;
		}
	}

	ATOM_Point2Di scrollOffset = _scrollOffset;
	if (scrollOffset.x > _canvasSize.w - _clientRect.size.w)
		scrollOffset.x = _canvasSize.w - _clientRect.size.w;
	if (scrollOffset.x < 0)
		scrollOffset.x = 0;

	if (scrollOffset.y > _canvasSize.h - _clientRect.size.h)
		scrollOffset.y = _canvasSize.h - _clientRect.size.h;
	if (scrollOffset.y < 0)
		scrollOffset.y = 0;
	setScrollValue (scrollOffset);

	if (hasVScroll)
	{
		if (!_verticalScrollBar)
		{
			_verticalScrollBar = ATOM_NEW(ATOM_ScrollBar, this, vscrollRect, int(ATOM_Widget::NoFocus|ATOM_Widget::NoClip|ATOM_Widget::NonScrollable|ATOM_Widget::NonLayoutable|ATOM_Widget::Private), int(VScrollBarId), ATOM_Widget::ShowNormal);
			_verticalScrollBar->setPlacement(WP_VERTICAL);
			_verticalScrollBar->setHandleWidth(_scrollBarWidth);
			_verticalScrollBar->setButtonWidth(_scrollBarWidth);

			_verticalScrollBar->setStep (1);
		}
		else
		{
			_verticalScrollBar->resize (vscrollRect);
		}

		_verticalScrollBar->setRange (0, (_canvasSize.h > _clientRect.size.h) ? _canvasSize.h - _clientRect.size.h : 0);
		_verticalScrollBar->setPosition (_scrollOffset.y);
		scrollChildren (0, _scrollOffset.y);
		_scrollOffset.y = _verticalScrollBar->getPosition();
		scrollChildren(0, -_scrollOffset.y);
	}
	else if (_verticalScrollBar)
	{
		ATOM_DELETE(_verticalScrollBar);
		_verticalScrollBar = 0;
	}

	if (hasHScroll)
	{
		if (!_horizontalScrollBar)
		{
			_horizontalScrollBar = ATOM_NEW(ATOM_ScrollBar, this, hscrollRect, int(ATOM_Widget::NoFocus|ATOM_Widget::NoClip|ATOM_Widget::NonScrollable|ATOM_Widget::NonLayoutable|ATOM_Widget::Private), int(HScrollBarId), ATOM_Widget::ShowNormal);
			_horizontalScrollBar->setPlacement(WP_HORIZONTAL);
			_horizontalScrollBar->setHandleWidth(_scrollBarWidth);
			_horizontalScrollBar->setButtonWidth(_scrollBarWidth);
			_horizontalScrollBar->setStep (1);
		}
		else
		{
			_horizontalScrollBar->resize (hscrollRect);
		}

		_horizontalScrollBar->setRange (0, (_canvasSize.w > _clientRect.size.w) ? _canvasSize.w - _clientRect.size.w : 0);
		_horizontalScrollBar->setPosition (_scrollOffset.x);
		scrollChildren (_scrollOffset.x, 0);
		_scrollOffset.x = _horizontalScrollBar->getPosition();
		scrollChildren (-_scrollOffset.x, 0);
	}
	else if (_horizontalScrollBar)
	{
		ATOM_DELETE(_horizontalScrollBar);
		_horizontalScrollBar = 0;
	}

	_layout->setRect (_clientRect);
	updateLayout ();
}

ATOM_Rect2Di ATOM_Widget::getLayoutRect (void) const
{
	return isLayoutable() ? ATOM_Rect2Di(_layoutUnit->desired_x, _layoutUnit->desired_y, _layoutUnit->desired_w, _layoutUnit->desired_h) : _widgetRect;
}

const ATOM_Rect2Di &ATOM_Widget::getWidgetRect (void) const
{
	ATOM_STACK_TRACE(ATOM_Widget::getWidgetRect);

	return _widgetRect;
}

const ATOM_Rect2Di &ATOM_Widget::getClientRect (void) const
{
	ATOM_STACK_TRACE(ATOM_Widget::getClientRect);

	return _clientRect;
}

void ATOM_Widget::onResize (ATOM_WidgetResizeEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Widget::onResize);

	_widgetRect = event->widgetRect;

	calcLayout ();
	invalidate ();
}

void ATOM_Widget::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Widget::onPaint);

//	//-- wangjian added ---//
//#if 1
//	ATOM_LOGGER::log("%s is onPaint", _name.c_str());
//#endif

	if (_clientImageId != ATOM_INVALID_IMAGEID)
	{
#if 1
		if(!_isExClientImage || !_clientImage)
		{
			_clientImage = getValidImage (_clientImageId);
			_isExClientImage = false;
		}
#else
		const ATOM_GUIImageList *imagelist = getValidImageList ();
		ATOM_GUIImage *image = imagelist->getImage (_clientImageId);
#endif
		if (!_clientImage)
		{
			_clientImage = ATOM_GUIImageList::getDefaultImageList().getImage (ATOM_IMAGEID_WINDOW_BKGROUND);
			ATOM_ASSERT(_clientImage);
		}
		_clientImage->draw (event->state, event->canvas, ATOM_Rect2Di(ATOM_Point2Di(0, 0), event->canvas->getSize()));
	}
}

void ATOM_Widget::onPaintFrame (ATOM_WidgetDrawFrameEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Widget::onPaintFrame);

	unsigned style = getStyle ();

	if ((style & ATOM_Widget::TitleBar) != 0)
	{
		bool activateTitleBar = false;
		ATOM_Widget *focus = _guiRenderer->getFocus ();
		if (isTopWindow() && ATOM_Widget::isValidWidget(focus) && _guiRenderer->belongsTo (focus, this))
		{
			activateTitleBar = true;
		}
		ATOM_Rect2Di rc(_titleBarRect.point, _titleBarRect.size);
		event->canvas->fillRect (rc, activateTitleBar ? ATOM_ColorARGB(0.1f, 0.25f, 1.f, 1.f) : ATOM_ColorARGB(0.3f, 0.3f, 0.3f, 1.f));
	}

	if ((style & ATOM_Widget::VScroll) != 0 && (style & ATOM_Widget::HScroll) != 0)
	{
		event->canvas->fillRect (ATOM_Rect2Di(_widgetRect.size.w - _scrollBarWidth - 1, _widgetRect.size.h - _scrollBarWidth - 1, _scrollBarWidth, _scrollBarWidth), ATOM_ColorARGB(1.f, 1.f, 1.f, 1.f));
	}

	if ((style & ATOM_Widget::Border) != 0)
	{
		ATOM_Widget::BorderMode borderMode = getBorderMode ();
		if (borderMode == ATOM_Widget::Flat)
		{
			ATOM_Rect2Di rc;
			rc.point.x = 0;
			rc.point.y = 0;
			rc.size = _widgetRect.size - ATOM_Size2Di(1, 1);
			event->canvas->drawRect (rc, ATOM_ColorARGB(0.f, 0.f, 0.f, 1.f), 1);
		}
		else
		{
			int l = 0;
			int t = 0;
			int r = _widgetRect.size.w - 1;
			int b = _widgetRect.size.h - 1;

			static const ATOM_ColorARGB lcolor(0.7f, 0.7f, 0.7f, 1.f);
			static const ATOM_ColorARGB dcolor(0.3f, 0.3f, 0.3f, 1.f);

			ATOM_ColorARGB c1, c2;
			if (borderMode == ATOM_Widget::Raise && event->state != WST_HOLD)
			{
			  c1 = lcolor;
			  c2 = dcolor;
			}
			else
			{
			  c1 = dcolor;
			  c2 = lcolor;
			}

			const ATOM_Point2Di start[4] = {
				ATOM_Point2Di(l, t),
				ATOM_Point2Di(l, t),
				ATOM_Point2Di(r, t),
				ATOM_Point2Di(l, b)
			};

			const ATOM_Point2Di end[4] = {
				ATOM_Point2Di(r, t),
				ATOM_Point2Di(l, b),
				ATOM_Point2Di(r, b),
				ATOM_Point2Di(r, b)
			};

			const ATOM_ColorARGB colors[4] = {
				c1,
				c1,
				c2,
				c2
			};

			event->canvas->drawLineList (4, start, end, colors);
		}
	}
}

void ATOM_Widget::setMenuBar (ATOM_MenuBar *menubar)
{
	ATOM_STACK_TRACE(ATOM_Widget::setMenuBar);

	if (menubar != _menuBar)
	{
		if (_menuBar)
		{
			detach (_menuBar);
			calcLayout ();
		}

		if (menubar)
		{
			attach (menubar);
			menubar->setSelected (-1);
			menubar->show (ATOM_Widget::ShowNormal);
			menubar->resize (_clientRect);
		}

		int oldMenuBarHeight = _menuBar ? _menuBar->getWidgetRect().size.h : 0;
		int newMenuBarHeight = menubar ? menubar->getWidgetRect().size.h : 0;

		_menuBar = menubar;

		calcLayout ();

		for (ATOM_Widget *w = _firstControl; w; w = w->_next)
		{
			if (w != _closeButton && w != menubar && w != _verticalScrollBar && w != _horizontalScrollBar)
			{
				w->moveBy (0, newMenuBarHeight - oldMenuBarHeight);
			}
		}
	}
}

ATOM_MenuBar *ATOM_Widget::getMenuBar (void) const
{
	ATOM_STACK_TRACE(ATOM_Widget::getMenuBar);

	return _menuBar;
}

void ATOM_Widget::setCanvasSize (const ATOM_Size2Di &size)
{
	ATOM_STACK_TRACE(ATOM_Widget::setCanvasSize);

	if (size != _canvasSize)
	{
		_canvasSize = size;

		ATOM_WidgetResizeEvent e(getId(), _widgetRect);
		handleEvent (&e);
	}
}

const ATOM_Size2Di &ATOM_Widget::getCanvasSize (void) const
{
	ATOM_STACK_TRACE(ATOM_Widget::getCanvasSize);

	return _canvasSize;
}

ATOM_ScrollBar *ATOM_Widget::getVerticalScrollBar (void) const
{
	ATOM_STACK_TRACE(ATOM_Widget::getVerticalScrollBar);

	return _verticalScrollBar;
}

ATOM_ScrollBar *ATOM_Widget::getHorizontalScrollBar (void) const
{
	ATOM_STACK_TRACE(ATOM_Widget::getHorizontalScrollBar);

	return _horizontalScrollBar;
}

void ATOM_Widget::scrollChildren (int x, int y)
{
	ATOM_STACK_TRACE(ATOM_Widget::scrollChildren);

	for (ATOM_Widget *w = _firstControl; w; w = w->_next)
	{
		if ((w->getStyle() & ATOM_Widget::NonScrollable) == 0)
		{
			w->moveBy (x, y);
		}
	}

	invalidate ();
}

void ATOM_Widget::onScroll (ATOM_ScrollEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Widget::onScroll);

	if (_canvasSize.w != 0 || _canvasSize.h != 0 && event->newPos != event->oldPos)
	{
		int controlOffsetX = 0;
		int controlOffsetY = 0;

		switch (event->id)
		{
		case VScrollBarId:
			_scrollOffset.y = event->newPos;
			controlOffsetY = event->oldPos - event->newPos;
			break;
		case HScrollBarId:
			_scrollOffset.x = event->newPos;
			controlOffsetX = event->oldPos - event->newPos;
			break;
		}

		scrollChildren (controlOffsetX, controlOffsetY);
	}
}

void ATOM_Widget::setScrollValue (const ATOM_Point2Di &val)
{
	ATOM_STACK_TRACE(ATOM_Widget::setScrollValue);

	ATOM_Point2Di p = val;

	if (p != _scrollOffset)
	{
		//if (_canvasSize.h > _clientRect.size.h)
		{
			int range = _canvasSize.h - _clientRect.size.h;
			if (p.y < 0)
			{
				p.y = 0;
			}
			else if (p.y > range)
			{
				p.y = range;
			}

			if (_verticalScrollBar)
			{
				_verticalScrollBar->setPosition (p.y, false);
				p.y = _verticalScrollBar->getPosition ();

			}
			int controlOffsetY = _scrollOffset.y - p.y;
			_scrollOffset.y = p.y;
			scrollChildren (0, controlOffsetY);
		}

		//if (_canvasSize.w > _clientRect.size.w)
		{
			int range = _canvasSize.w - _clientRect.size.w;
			if (p.x < 0)
			{
				p.x = 0;
			}
			else if (p.x > range)
			{
				p.x = range;
			}

			if (_horizontalScrollBar)
			{
				_horizontalScrollBar->setPosition (p.x, false);
				p.x = _horizontalScrollBar->getPosition ();
			}
			int controlOffsetX = _scrollOffset.x - p.x;
			_scrollOffset.x = p.x;
			scrollChildren (controlOffsetX, 0);
		}
	}
}

const ATOM_Point2Di &ATOM_Widget::getScrollValue (void) const
{
	ATOM_STACK_TRACE(ATOM_Widget::getScrollValue);

	return _scrollOffset;
}

void ATOM_Widget::onMouseWheel (ATOM_WidgetMouseWheelEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Widget::onMouseWheel);

	if (_canvasSize.w != 0 || _canvasSize.h != 0 && _canvasSize.h > _clientRect.size.h)
	{
		ATOM_Point2Di p = _scrollOffset;
		p.y -= event->delta * 20;
		int controlOffsetX = 0;
		int controlOffsetY = 0;
		if (p != _scrollOffset)
		{
			if (_verticalScrollBar)
			{
				int range = _canvasSize.h - _clientRect.size.h;
				if (p.y < 0)
				{
					p.y = 0;
				}
				else if (p.y > range)
				{
					p.y = range;
				}
				_verticalScrollBar->setPosition (p.y, false);
				p.y = _verticalScrollBar->getPosition ();
				controlOffsetY = _scrollOffset.y - p.y;
				_scrollOffset.y = p.y;
			}
			if(_horizontalScrollBar)
			{
				int range = _canvasSize.w - _clientRect.size.w;
				if (p.x < 0)
				{
					p.x = 0;
				}
				else if (p.x > range)
				{
					p.x = range;
				}
				_horizontalScrollBar->setPosition (p.x, false);
				p.x = _horizontalScrollBar->getPosition ();
				controlOffsetX = _scrollOffset.x - p.x;
				_scrollOffset.x = p.x;
			}
			scrollChildren (controlOffsetX, controlOffsetY);
		}
	}

	if(_enableWheelDeliver && _parent)
	{
		_parent->queueEvent (ATOM_NEW(ATOM_WidgetMouseWheelEvent, _widgetId, event->x,event->y, event->delta,event->shiftState), ATOM_APP);
	}
	//callParentHandler (event);
}

bool ATOM_Widget::supportIME (void) const
{
	return false;
}

void ATOM_Widget::setEventTrigger (ATOM_EventTrigger *trigger)
{
	ATOM_WidgetBase::setEventTrigger (trigger);
	_handleIdleEvent = -1;
}

bool ATOM_Widget::handleIdleEvent (void)
{
	if (_handleIdleEvent < 0)
	{
		_handleIdleEvent = supportEvent (ATOM_WidgetIdleEvent::eventTypeId()) ? 1 : 0;
	}

	return _handleIdleEvent != 0;
}

void ATOM_Widget::resetLayout (ATOM_Widget *parent, ATOM_WidgetLayout *newLayout)
{
	for (ATOM_Widget *c = parent->getFirstControl(); c; c = c->getNextSibling ())
	{
		ATOM_WidgetLayout::Unit *unit = 0;

		if (newLayout)
		{
			unit = newLayout->allocUnit ();
			unit->desired_x = c->_layoutUnit->desired_x;
			unit->desired_y = c->_layoutUnit->desired_y;
			unit->desired_w = c->_layoutUnit->desired_w;
			unit->desired_h = c->_layoutUnit->desired_h;
			unit->widget = c;
		}

		_layout->freeUnit (c->_layoutUnit);
		c->_layoutUnit = unit;
	}

	for (ATOM_Widget *c = parent->getFirstChild(); c; c = c->getNextSibling ())
	{
		ATOM_WidgetLayout::Unit *unit = 0;
		
		if (newLayout)
		{
			unit = newLayout->allocUnit ();
			unit->desired_x = c->_layoutUnit->desired_x;
			unit->desired_y = c->_layoutUnit->desired_y;
			unit->desired_w = c->_layoutUnit->desired_w;
			unit->desired_h = c->_layoutUnit->desired_h;
			unit->widget = c;
		}

		_layout->freeUnit (c->_layoutUnit);
		c->_layoutUnit = unit;
	}
}

void ATOM_Widget::setLayoutType (ATOM_WidgetLayout::Type type)
{
	if (type != _layout->getType ())
	{
		ATOM_WidgetLayout *newLayout = ATOM_WidgetLayout::createLayout (type);
		if (newLayout)
		{
			resetLayout (this, newLayout);
			ATOM_WidgetLayout::deleteLayout (_layout);
			_layout = newLayout;
			_layout->setRect (_clientRect);
			updateLayout ();
		}
	}
}

ATOM_WidgetLayout::Type ATOM_Widget::getLayoutType (void) const
{
	return _layout->getType ();
}

ATOM_WidgetLayout *ATOM_Widget::getLayout (void) const
{
	return _layout;
}

void ATOM_Widget::invalidateLayout (void)
{
	_layout->invalidateLayout ();
}

void ATOM_Widget::refreshLayout (void)
{
	_layout->calcLayout ();

	for (ATOM_Widget *c = getFirstControl(); c; c = c->getNextSibling ())
	{
		if (c->isLayoutable ())
		{
			ATOM_WidgetLayout::Unit *unit = c->_layoutUnit;
			ATOM_WidgetResizeEvent e(c->getId(), ATOM_Rect2Di(unit->actual_x, unit->actual_y, unit->actual_w, unit->actual_h));
			c->handleEvent (&e);
			c->_layout->setRect (c->getClientRect());
			c->updateLayout ();
		}
	}

	for (ATOM_Widget *c = getFirstChild(); c; c = c->getNextSibling ())
	{
		if (c->isLayoutable ())
		{
			ATOM_WidgetLayout::Unit *unit = c->_layoutUnit;
			ATOM_WidgetResizeEvent e(c->getId(), ATOM_Rect2Di(unit->actual_x, unit->actual_y, unit->actual_w, unit->actual_h));
			c->handleEvent (&e);
			c->_layout->setRect (c->getClientRect());
			c->updateLayout ();
		}
	}
}

ATOM_WidgetLayout::Unit *ATOM_Widget::getLayoutUnit (void) const
{
	return _layoutUnit;
}

bool ATOM_Widget::moveLayoutUnitUp (void)
{
	return _parent ? _parent->getLayout()->moveUnitUp (_layoutUnit) : false;
}

bool ATOM_Widget::moveLayoutUnitDown (void)
{
	return _parent ? _parent->getLayout()->moveUnitDown (_layoutUnit) : false;
}

void ATOM_Widget::updateLayout (void)
{
	if (_layout->isDirty ())
	{
		refreshLayout ();
	}
}

void ATOM_Widget::setLayoutable (bool layoutable)
{
	setStyle (layoutable ? (getStyle() & ~ATOM_Widget::NonLayoutable) : (getStyle()|ATOM_Widget::NonLayoutable));
}

bool ATOM_Widget::isLayoutable (void) const
{
	return (getStyle() & ATOM_Widget::NonLayoutable) == 0;
}

void ATOM_Widget::setFont (ATOM_GUIFont::handle font)
{
	if (_widgetFont != font)
	{
		_widgetFont = font;

		if (getShowState () != ATOM_Widget::Hide)
		{
			invalidate ();
		}
	}
}

ATOM_GUIFont::handle ATOM_Widget::getFont (void) const
{
	if (_widgetFont != ATOM_GUIFont::invalid_handle)
	{
		return _widgetFont;
	}
	else if (_parent)
	{
		return _parent->getFont ();
	}
	else
	{
		return ATOM_GUIFont::getDefaultFont (fontSize, 0);
	}
}

ATOM_GUIFont::handle ATOM_Widget::getExactFont (void) const
{
	return _widgetFont;
}

void ATOM_Widget::setFontColor (const ATOM_ColorARGB& clr)
{
	_widgetFontColor = clr;
}

ATOM_ColorARGB ATOM_Widget::getFontColor() const
{
	return _widgetFontColor;
}

void ATOM_Widget::enableDrag (bool enable)
{
	_enableDrag = enable;
}

bool ATOM_Widget::isDragEnabled (void) const
{
	return _enableDrag;
}

void ATOM_Widget::enableDrop (bool enable)
{
	_enableDrop = enable;
}

bool ATOM_Widget::isDropEnabled (void) const
{
	return _enableDrop;
}

void ATOM_Widget::setFocus (void)
{
	_guiRenderer->setFocus (this);
}

bool ATOM_Widget::reparent (ATOM_Widget *newParent)
{
	ATOM_Widget *oldParent = _parent;
	if (oldParent == newParent)
	{
		return true;
	}

	ATOM_Rect2Di oldRect;
	if (oldParent)
	{
		oldRect.point.x = getLayoutUnit ()->desired_x;
		oldRect.point.y = getLayoutUnit ()->desired_y;
		oldRect.size.w = getLayoutUnit ()->desired_w;
		oldRect.size.h = getLayoutUnit ()->desired_h;
		oldParent->detach (this);
	}

	if (newParent)
	{
		newParent->attach (this);
		resize (oldParent ? oldRect : getWidgetRect());
	}

	return true;
}

ATOM_Widget *ATOM_Widget::loadHierarchyR (ATOM_Widget *widget, ATOM_Widget *root, ATOM_TiXmlElement *xml, int loadPriority)
{
	ATOM_Widget *ret = 0;
	
	for (ATOM_TiXmlElement *child = xml->FirstChildElement("component"); child; child = child->NextSiblingElement("component"))
	{
		const char *type = child->Attribute ("type");
		if (!type)
		{
			continue;
		}

		ATOM_Widget *childWidget = 0;

		if (!stricmp(type, "panel"))
		{
			ATOM_PanelParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "spacer"))
		{
			ATOM_SpacerParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "edit"))
		{
			ATOM_EditParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "label"))
		{
			ATOM_LabelParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "window"))
		{
			ATOM_WindowParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "dialog"))
		{
			ATOM_DialogParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "multiedit"))
		{
			ATOM_MultieditParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "button"))
		{
			ATOM_ButtonParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "progressbar"))
		{
			ATOM_ProgressBarParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "richedit"))
		{
			ATOM_RichEditParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "cell"))
		{
			ATOM_CellParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "listbox"))
		{
			ATOM_ListBoxParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "checkbox"))
		{
			ATOM_CheckBoxParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "listview"))
		{
			ATOM_ListViewParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "scrollbar"))
		{
			ATOM_ScrollBarParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "viewstack"))
		{
			ATOM_ViewStackParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "treectrl"))
		{
			ATOM_TreeCtrlParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "component"))
		{
			ATOM_ComponentParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "flash"))
		{
			ATOM_FlashParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "scrollmap"))
		{
			ATOM_ScrollMapParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "combobox")) 
		{
			ATOM_ComboBoxParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "realtimectrl"))
		{
			ATOM_RealtimeCtrlParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "marquee"))
		{
			ATOM_MarqueeParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget,loadPriority);
		}
		else if (!stricmp(type, "curveeditor"))
		{
			ATOM_CurveEditorParameters params;
			params.resetParameters ();
			params.loadFromXML (child);
			childWidget = params.createWidget (widget, loadPriority);
		}
		else
		{
			continue;
		}

		if (childWidget)
		{
			childWidget->_hierichyRoot = root;

			loadHierarchyR (childWidget, root?root:childWidget, child, loadPriority);

			if (!ret)
			{
				ret = childWidget;
			}
		}
	}

	return ret;
}

ATOM_Size2Di ATOM_Widget::calcWidgetSize (void) const
{
	return ATOM_Size2Di(100, 100);
}

ATOM_Widget *ATOM_Widget::loadHierarchy (const char *filename)
{
#if 1
	ATOM_WidgetPreloadedParameters params;
	if (params.load (filename))
	{
		ATOM_Widget *w = loadHierarchyFromPreloadedParameters (&params);
		//if (w)
		//{
		//	//w->enableCheckWaiting(1);
		//	_WaitingMode = w->initWaiting();
		//}
		return w;
	}
	return 0;
#else
	ATOM_AutoFile f(filename, ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
	{
		return 0;
	}

	unsigned size = f->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = f->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument doc;
	doc.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (doc.Error ())
	{
		return 0;
	}

	ATOM_TiXmlElement *root = doc.RootElement ();
	if (!root)
	{
		return 0;
	}

	if (strcmp (root->Value(), "UI"))
	{
		return 0;
	}

	const char *fontSetFile = root->Attribute ("fonts");
	if (fontSetFile)
	{
		ATOM_GUIFont::loadFontConfig (fontSetFile);
	}

	//--- wangjian modified ---//
	// 读取UI的异步加载标记
	int asyncload = WIDGET_LOAD_PRIORITY_BASE;
	root->Attribute("asyncload", &asyncload);
	// 如果没有设置 默认开启异步加载 优先级为1000
	//setAsyncLoad( root->Attribute("asyncload", &asyncload) ? asyncload : WIDGET_LOAD_PRIORITY_BASE );
	//this->enableCheckWaiting(1);
	ATOM_Widget * w = loadHierarchyR ( this, NULL, root, asyncload );
	
	//-- wangjian added for test ---//
#if 0
	ATOM_LOGGER::log( "%s is loading now! \n", filename);
	if( w )
		w->setWidgetName(filename);
#endif
	//------------------------------//

	if( w )
	{
		//w->enableCheckWaiting(1);
		_WaitingMode = w->initWaiting();
	}

	return w/*loadHierarchyR ( this, NULL, root, getAsyncLoad() )*/;
	//----------------------//
#endif
}

ATOM_Widget *ATOM_Widget::loadHierarchyFromPreloadedParameters (ATOM_WidgetPreloadedParameters *params)
{
	ATOM_Widget * w = loadHierarchyFromPreloadedParametersR (this, 0, params, params->getLoadPriority());
	if( w )
		_WaitingMode = w->initWaiting();
	return w;
}

ATOM_Widget *ATOM_Widget::loadHierarchyFromPreloadedParametersR (ATOM_Widget *widget, ATOM_Widget *root, ATOM_WidgetPreloadedParameters *params, int loadPriority)
{
	ATOM_Widget *ret = 0;

	for (int i = 0; i < params->getNumChildParameters(); ++i)
	{
		ATOM_WidgetPreloadedParameters *childParams = params->getChildParameters(i);
		if (!childParams)
		{
			continue;
		}
		ATOM_WidgetParameters *p = childParams->getParameters();
		if (!p)
		{
			continue;
		}
		ATOM_Widget *childWidget = p->createWidget(widget, loadPriority);
		if (childWidget)
		{
			childWidget->_hierichyRoot = root;

			loadHierarchyFromPreloadedParametersR (childWidget, root?root:childWidget, childParams, loadPriority);

			if (!ret)
			{
				ret = childWidget;
			}
		}
	}

	return ret;
}

ATOM_UserAttributes *ATOM_Widget::getAttributes (void) const
{
	return _attributes;
}

unsigned ATOM_Widget::getNumChildren (void) const
{
	return _layout->getNumUnits ();
}

ATOM_Widget *ATOM_Widget::getChild (unsigned index) const
{
	return _layout->getUnit (index)->widget;
}

void ATOM_Widget::onLButtonDown (ATOM_WidgetLButtonDownEvent *event)
{
	if ((_dragMode & ATOM_Widget::ClientDrag) != 0)
	{
		ATOM_Point2Di pt(event->x, event->y);
		clientToScreen (&pt);

		getRenderer()->startDragging (this, pt.x, pt.y);
	}
}

void ATOM_Widget::onLButtonUp (ATOM_WidgetLButtonUpEvent *event)
{
 	if (getRenderer()->getDraggingWidget() == this)
	{
		getRenderer()->stopDragging ();
	}
}

void ATOM_Widget::setDragMode (unsigned mode)
{
	_dragMode = mode;
}

unsigned ATOM_Widget::getDragMode (void) const
{
	return _dragMode;
}

ATOM_GUICanvas *ATOM_Widget::getWidgetCanvas (void)
{
	ATOM_GUICanvas *canvas = ATOM_NEW(ATOM_GUICanvas, this);

	ATOM_Rect2Di rcWidget = _widgetRect;
	rcWidget.point.x = -_clientRect.point.x;
	rcWidget.point.y = -_clientRect.point.y;
	clientToViewport (&rcWidget.point);

	canvas->resize (rcWidget.size);
	canvas->setOrigin (rcWidget.point);
	canvas->setTargetRect (ATOM_Rect2Di(0, 0, rcWidget.size.w, rcWidget.size.h));

	return canvas;
}

ATOM_GUICanvas *ATOM_Widget::getClientCanvas (void)
{
	ATOM_GUICanvas *canvas = ATOM_NEW(ATOM_GUICanvas, this);

	ATOM_Rect2Di rcClient = _clientRect;
	rcClient.point.x = 0;
	rcClient.point.y = 0;
	clientToViewport (&rcClient.point);

	canvas->resize (rcClient.size);
	canvas->setOrigin (rcClient.point - _scrollOffset);
	canvas->setTargetRect (ATOM_Rect2Di(0, 0, rcClient.size.w, rcClient.size.h));

	return canvas;
}

void ATOM_Widget::freeCanvas (ATOM_GUICanvas *canvas)
{
	ATOM_DELETE(canvas);
}

void ATOM_Widget::setUserData(unsigned long long data)
{
	_userData = data;
}

unsigned long long ATOM_Widget::getUserData() const
{
	return _userData;
}

void ATOM_Widget::setWidgetName (const char *name)
{
	_name = name ? name : "";
}

const char *ATOM_Widget::getWidgetName (void) const
{
	return _name.c_str();
}

void ATOM_Widget::setFontOutLineColor( const ATOM_ColorARGB& clr )
{
	_widgetFontOutLineColor = clr;
}

ATOM_ColorARGB ATOM_Widget::getFontOutLineColor()
{
	if (_widgetFontOutLineColor.getByteA() != 0)
	{
		return _widgetFontOutLineColor;
	}
	else if (_parent)
	{
		return _parent->getFontOutLineColor ();
	}
	else
	{
		return ATOM_ColorARGB(0.f, 0.f, 0.f, 0.f);
	}
}


void ATOM_Widget::setExClientImage( ATOM_GUIImage * image )
{
	_clientImage = image;
	_isExClientImage = true;
}

void ATOM_Widget::setVerticalSCrollBarImageId( int backImageId, int sliderImageId, int upButtonImageId, int downButtonImageId )
{
	if(_verticalScrollBar)
		_verticalScrollBar->setVerticalImageId(backImageId,sliderImageId,upButtonImageId,downButtonImageId);
}


void ATOM_Widget::setScrollBarSliderLength( int len )
{
	if(_verticalScrollBar)
		_verticalScrollBar->setHandleWidth(len);
}

void ATOM_Widget::setScrollBarButtonLength( int len )
{
	if(_verticalScrollBar)
		_verticalScrollBar->setButtonWidth(len);
}

void ATOM_Widget::compactLayout (void)
{

}

void ATOM_Widget::compactLayoutR (ATOM_Widget *widget)
{

}

bool ATOM_Widget::isUserExClientImage()
{
	return _isExClientImage;
}

void ATOM_Widget::setWheelDeliver( bool wheelDeliver )
{
	_enableWheelDeliver = wheelDeliver;
}

void ATOM_Widget::setPopupRelation (ATOM_Widget *widget)
{
	_popupRelation = widget;
}

ATOM_Widget *ATOM_Widget::getPopupRelation (void) const
{
	return _popupRelation;
}



//--- wangjian added ---//
// 异步加载相关
void ATOM_Widget::setAsyncLoad(int loadPriority/*=1000*/)
{
	// 如果异步加载没有开启 不等待
	if( ( loadPriority != ATOM_LoadPriority_IMMEDIATE ) && ( !ATOM_AsyncLoader::IsRun() || !ATOM_GUIRenderer::isMultiThreadingEnabled() ) )
	{
		_loadPriority = ATOM_LoadPriority_IMMEDIATE;
		_bWaiting = WIDGET_LOAD_NOWAITING;
		_WaitingMode = WAITING_MODE_NOWAITING;
		return;
	}
	_loadPriority = loadPriority;
	setWaiting(_loadPriority != ATOM_LoadPriority_IMMEDIATE);
}
int	ATOM_Widget::getAsyncLoad(void) const
{
	return _loadPriority;
}
bool ATOM_Widget::isAsyncLoad() const
{
	return ATOM_GUIRenderer::isMultiThreadingEnabled() && (_loadPriority != ATOM_LoadPriority_IMMEDIATE);
}
void ATOM_Widget::setWaiting(bool bWaiting)
{
	// 如果异步加载没有开启 不等待
	if( bWaiting && ( !ATOM_AsyncLoader::IsRun() || !ATOM_GUIRenderer::isMultiThreadingEnabled() ) )
	{
		_bWaiting = WIDGET_LOAD_NOWAITING;
		_WaitingMode = WAITING_MODE_NOWAITING;

		return;
	}

	_bWaiting = bWaiting ? WIDGET_LOAD_WAITING : WIDGET_LOAD_NOWAITING;
	_WaitingMode = bWaiting ? WAITING_MODE_NOWAITING : WAITING_MODE_NOWAITING;
}
bool ATOM_Widget::isWaiting()
{
	if( _loadPriority == ATOM_LoadPriority_IMMEDIATE )
		return false;

	return _bWaiting;
}
UINT8 ATOM_Widget::initWaiting()
{
	if( _loadPriority == ATOM_LoadPriority_IMMEDIATE )
	{
		_bWaiting = WIDGET_LOAD_NOWAITING;
		_WaitingMode = WAITING_MODE_NOWAITING;
		return WAITING_MODE_NOWAITING;
	}

	if( _bWaiting )
	{
		ATOM_GUIImage *image = _clientImage;

		// 如果IMAGE不存在 或者 等待标志为2(重新等待） 
		if( !image && ( _clientImageId != ATOM_INVALID_IMAGEID ) )
		{
			image = getValidImage (_clientImageId);
			if(image && image != _clientImage)
				_clientImage = image;
		}
		if(image)
		{
			for( int i = 0; i < WST_HOVER; ++i )
			{
				const ATOM_STRING & texFileName = image->getTexFileName((ATOM_WidgetState)i);
				if( !texFileName.empty() )
				{
					char native[ATOM_VFS::max_filename_length] = {0};
					ATOM_GetNativePathName(texFileName.c_str(), native);
					if( ATOM_PhysicVFS().doesFileExists (native) )
					{
						_WaitingMode = WAITING_MODE_SHOWNONE;
					}
					else
					{
						_WaitingMode = WAITING_MODE_SHOWWAITINGUI;
						return _WaitingMode;
					}
				}
			}
		}
		else
		{
			_WaitingMode = WAITING_MODE_NOWAITING;
		}

		for (ATOM_Widget *w = getFirstControl(); w; w = w->getNextSibling())
		{
			UINT8 mode = w->initWaiting ();
			if( _WaitingMode < mode )
				_WaitingMode = mode;
		}

		for (ATOM_Widget *w = getFirstChild(); w; w = w->getNextSibling())
		{
			UINT8 mode = w->initWaiting ();
			if( _WaitingMode < mode )
				_WaitingMode = mode;
		}

		return _WaitingMode;
	}

	return WAITING_MODE_NOWAITING;
}

void ATOM_Widget::enableCheckWaiting(UINT8 enable )
{
	_bCheckWaiting = enable;
}
UINT8 ATOM_Widget::isCheckWaitingEnabled(void) const
{
	return _bCheckWaiting;
}


void ATOM_Widget::submitLoadRequest()
{
	if( !_bLoadRequestSubmitted )
	{
		_bLoadRequestSubmitted = 1;

		// 如果非异步加载 
		if( _loadPriority == ATOM_LoadPriority_IMMEDIATE )
			return;

		if( _bWaiting == WIDGET_LOAD_NOWAITING )
			return;

		/*if( _WaitingMode == WAITING_MODE_NOWAITING )
			return true;*/

		ATOM_GUIImage *image = _clientImage;

		// 如果IMAGE不存在 或者 等待标志为2(重新等待） 
		if( !image && _clientImageId != ATOM_INVALID_IMAGEID )
		{
			image = getValidImage (_clientImageId);
			if(image && image != _clientImage)
				_clientImage = image;
		}

		if( image )
		{
			for( int i = 0; i < WST_COUNT; ++i )
			{
				ATOM_AUTOREF(ATOM_Texture) texture = image->getTexture((ATOM_WidgetState)i);
			}
		}
	
		if( _bWaiting < WIDGET_LOAD_REWAITING )
		{
			for (ATOM_Widget *w = getFirstControl(); w; w = w->getNextSibling())
			{
				w->submitLoadRequest();
			}

			for (ATOM_Widget *w = getFirstChild(); w; w = w->getNextSibling())
			{
				w->submitLoadRequest();
			}
		}
	}
}
bool ATOM_Widget::checkWaiting()
{
	//---------------------------------------------//
	// 如果不需要检查等待 退出 跳过检查其以及其子WIDGET的资源是否加载完成
	if( !_bCheckWaiting )
	{
		_WaitingMode	= WAITING_MODE_NOWAITING;
		_bWaiting		= WIDGET_LOAD_NOWAITING;
		return true;
	}
	//---------------------------------------------//

	// 如果非异步加载 
	if( _loadPriority == ATOM_LoadPriority_IMMEDIATE )
		return true;

	if( _bWaiting == WIDGET_LOAD_NOWAITING )
		return true;

	/*if( _WaitingMode == WAITING_MODE_NOWAITING )
		return true;*/

	ATOM_GUIImage *image = _clientImage;

	// 如果IMAGE不存在 或者 等待标志为2(重新等待） 
	if( ( !image || _bWaiting == WIDGET_LOAD_REWAITING ) && ( _clientImageId != ATOM_INVALID_IMAGEID ) )
	{
		image = getValidImage (_clientImageId);
		if(image && image != _clientImage)
			_clientImage = image;
		if( _bWaiting == WIDGET_LOAD_REWAITING )
			_bWaiting = WIDGET_LOAD_TOTAL;
	}

	if( image )
	{
		for( int i = 0; i < WST_HOVER; ++i )
		{
			ATOM_AUTOREF(ATOM_Texture) texture = image->getTexture((ATOM_WidgetState)i);
			if( texture.get() )
			{
				if( !texture->getAsyncLoader()->IsLoadAllFinished() )
				{
					return false;	
				}
				else
				{
					if( image->getTextureWidth((ATOM_WidgetState)i)==0 || image->getTextureHeight((ATOM_WidgetState)i)==0 )
						image->setImage((ATOM_WidgetState)i,texture.get());
				}
			}
		}
	}
	
	if( _bWaiting < WIDGET_LOAD_REWAITING )
	{
		for (ATOM_Widget *w = getFirstControl(); w; w = w->getNextSibling())
		{
			if ( !w->checkWaiting () )
			{
				return false;
			}
		}

		for (ATOM_Widget *w = getFirstChild(); w; w = w->getNextSibling())
		{
			if ( !w->checkWaiting () )
			{
				return false;
			}
		}
	}

	_WaitingMode = WAITING_MODE_NOWAITING;

	_bWaiting = WIDGET_LOAD_NOWAITING;

	return true;
}
void ATOM_Widget::reWaiting()
{
	if( _loadPriority == ATOM_LoadPriority_IMMEDIATE )
		return;

	if( _bWaiting != WIDGET_LOAD_WAITING)
		_bWaiting = WIDGET_LOAD_REWAITING;
}
void ATOM_Widget::setWaitingImage( int imageId )
{
	if( _imageWaitingId != imageId )
	{
		_imageWaitingId = imageId;
		_imageWaiting = 0;
	}
}
void ATOM_Widget::drawWaitingImage()
{
	if( !_imageWaiting )
	{
		_imageWaiting = getValidImage(_imageWaitingId);
	}
	if(_imageWaiting)
	{
		ATOM_GUICanvas canvas(this);

		ATOM_Rect2Di rcClient = _clientRect;
		const ATOM_Rect2Di rc(ATOM_Point2Di(0,0), rcClient.size);

		rcClient.point.x = 0;
		rcClient.point.y = 0;
		clientToViewport (&rcClient.point);

		ATOM_Rect2Di img_region = _imageWaiting->getRegion(WST_NORMAL);
		int cw = 128/*img_region.size.w*/;//img->getTextureWidth(WST_NORMAL);//(_canvasSize.w < rcClient.size.w) ? rcClient.size.w : _canvasSize.w;
		int ch = 128/*img_region.size.h*/;//img->getTextureWidth(WST_NORMAL);//(_canvasSize.h < rcClient.size.h) ? rcClient.size.h : _canvasSize.h;
		canvas.resize (ATOM_Size2Di(cw, ch));

		canvas.setOrigin (rcClient.point/* - _scrollOffset*/);
		//ATOM_Size2Di targetSize(rcClient.size.w, rcClient.size.h);
		ATOM_Size2Di targetSize(cw, ch);
		canvas.setTargetRect (ATOM_Rect2Di(_scrollOffset, targetSize));

		_imageWaiting->draw (WST_NORMAL, &canvas, ATOM_Rect2Di(ATOM_Point2Di(0, 0), canvas.getSize()));
	}
}

