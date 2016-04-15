#include "stdafx.h"
#include "desktop.h"
#include "gui_renderer.h"
#include "gui_events.h"
#include "imagelist.h"

ATOM_BEGIN_EVENT_MAP(ATOM_Desktop, ATOM_TopWindow)
	ATOM_EVENT_HANDLER(ATOM_Desktop, ATOM_WidgetDrawClientEvent, onPaint)
ATOM_END_EVENT_MAP

static ATOM_GUIImageList *defaultImageList = 0;

ATOM_Desktop::ATOM_Desktop()
{
	ATOM_STACK_TRACE(ATOM_Desktop::ATOM_Desktop);

	_enableWindowTexture = false;

	_guiRenderer = NULL;
	_enableWindowTexture = false;
	_defaultImageList = NULL;

	_widgetRect.point.x = 0;
	_widgetRect.point.y = 0;
	_widgetRect.size.w = 0;
	_widgetRect.size.h = 0;
	_clientRect = _widgetRect;
	_titleBarRect.point.x = 0;
	_titleBarRect.point.y = 0;
	_titleBarRect.size.w = 0;
	_titleBarRect.size.h = 0;

	_clearBackground = true;

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_Desktop::ATOM_Desktop (ATOM_GUIRenderer *renderer)
	:
ATOM_TopWindow (0, ATOM_Rect2Di(ATOM_Point2Di(0, 0), renderer->getViewport().size), 0, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal)
{
	ATOM_STACK_TRACE(ATOM_Desktop::ATOM_Desktop);

	_enableWindowTexture = false;

	_guiRenderer = renderer;
	_enableWindowTexture = false;

	if (!defaultImageList)
	{
		defaultImageList = ATOM_NEW(ATOM_GUIImageList);
		defaultImageList->loadDefaults ();
	}
	setImageList (defaultImageList);

	_widgetRect.point.x = 0;
	_widgetRect.point.y = 0;
	_widgetRect.size = renderer->getViewport().size;
	_clientRect = _widgetRect;
	_titleBarRect.point.x = 0;
	_titleBarRect.point.y = 0;
	_titleBarRect.size.w = 0;
	_titleBarRect.size.h = 0;

	_clearBackground = true;

	resize (_widgetRect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_Desktop::~ATOM_Desktop (void)
{

}

ATOM_WidgetType ATOM_Desktop::getType (void) const
{
	return WT_DESKTOP;
}

void ATOM_Desktop::enableWindowTexture (bool enable)
{
	ATOM_TopWindow::enableWindowTexture (enable);
}

void ATOM_Desktop::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	if (_clearBackground)
	{
		ATOM_RenderDevice *device = ATOM_GetRenderDevice ();
		if (device)
		{
			device->clear (true, true, true);
		}
	}
}

bool ATOM_Desktop::clearBackground (void) const
{
	ATOM_STACK_TRACE(ATOM_Desktop::clearBackground);

	return _clearBackground;
}

void ATOM_Desktop::setClearBackground (bool b)
{
	ATOM_STACK_TRACE(ATOM_Desktop::setClearBackground);

	_clearBackground = b;
}
