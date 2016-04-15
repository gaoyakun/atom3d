#ifndef __ATOM3D_ENGINE_FLASHCTRL_H
#define __ATOM3D_ENGINE_FLASHCTRL_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_render.h"

#include "gui_font.h"
#include "widget.h"
#include "delegate.h"
#include "flashvalue.h"

class ATOM_ENGINE_API FlashControl;
class ATOM_ENGINE_API ATOM_FlashCtrl: public ATOM_Widget
{
public:
	ATOM_FlashCtrl();
	ATOM_FlashCtrl (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState = ATOM_Widget::Hide);
	virtual ~ATOM_FlashCtrl (void);

public:
	virtual ATOM_WidgetType getType (void) const;
	
public:
	void loadSWF (const char *filename);
	const char *getSWFFileName (void) const;
	FlashControl *getFlashControl (void) const;

public:
	void onChar (ATOM_WidgetCharEvent *event);
	void onKeyDown (ATOM_WidgetKeyDownEvent *event);
	void onKeyUp (ATOM_WidgetKeyUpEvent *event);
	void onLButtonDown(ATOM_WidgetLButtonDownEvent *event);
	void onLButtonUp(ATOM_WidgetLButtonUpEvent *event);
	void onMouseMove(ATOM_WidgetMouseMoveEvent *event);
	void onIdle(ATOM_WidgetIdleEvent *event);
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onResize (ATOM_WidgetResizeEvent *event);
	void onFlashCall (ATOM_FlashCallEvent *event);
	void onWinMessage (ATOM_AppWMEvent *event);
	void onHitTest (ATOM_WidgetHitTestEvent *event);

public:
	void bind(const char *funcName, const FlashDelegate& callback);
	void unbind(const char *funcName);
	ATOM_FlashValue callFunction(const char *funcName, const ATOM_FlashArguments& args = ATOM_FlashArgs());
	void setInitCallback (const char *name);
	const char *getInitCallback (void) const;
	void setClearCallback (const char *name);
	const char *getClearCallback (void) const;
	void setActivateCallback (const char *name);
	const char *getActivateCallback (void) const;
	void setDeactivateCallback (const char *name);
	const char *getDeactivateCallback (void) const;
	void setFPS (int FPS);
	int getFPS (void) const;
	void activate (void);
	void deactivate (void);
	bool isActive (void) const;

protected:
	FlashControl *_flashControl;
	ATOM_STRING _filename;
	ATOM_STRING _initFunc;
	ATOM_STRING _clearFunc;
	ATOM_STRING _activateFunc;
	ATOM_STRING _deactivateFunc;
	bool _active;
	int _FPS;
	unsigned _tick;

	ATOM_DECLARE_EVENT_MAP(ATOM_FlashCtrl, ATOM_Widget)
};



#endif // __ATOM3D_ENGINE_FLASHCTRL_H

