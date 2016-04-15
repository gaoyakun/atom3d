#ifndef __ATOM3D_SCRIPTEVENTTRIGGER_H
#define __ATOM3D_SCRIPTEVENTTRIGGER_H

#include "../ATOM_kernel.h"
#include "../ATOM_script.h"
#include "basedefs.h"
#include "gui_events.h"

class ATOM_ENGINE_API ATOM_ScriptEventTrigger: public ATOM_EventTrigger
{
public:
	ATOM_ScriptEventTrigger (ATOM_Script *script);
	virtual ~ATOM_ScriptEventTrigger (void);

public:
	void onWindowMouseMove (ATOM_MouseMoveEvent *event);
	void onWindowMouseWheel (ATOM_MouseWheelEvent *event);
	void onWindowMouseButtonDown (ATOM_MouseButtonDownEvent *event);
	void onWindowMouseButtonUp (ATOM_MouseButtonUpEvent *event);
	void onWindowKeyDown (ATOM_KeyDownEvent *event);
	void onWindowKeyUp (ATOM_KeyUpEvent *event);
	void onWindowChar (ATOM_CharEvent *event);
	void onWindowResize (ATOM_WindowResizeEvent *event);
	void onWindowActive (ATOM_WindowActiveEvent *event);
	void onWindowMove (ATOM_WindowMoveEvent *event);
	void onWindowExpose (ATOM_WindowExposeEvent *event);
	void onWindowClose (ATOM_WindowCloseEvent *event);
	void onDeviceLost (ATOM_DeviceLostEvent *event);
	void onDeviceReset (ATOM_DeviceResetEvent *event);
	void onAppIdle (ATOM_AppIdleEvent *event);
	void onAppInit (ATOM_AppInitEvent *event);
	void onAppExit (ATOM_AppExitEvent *event);
	void onAppQuit (ATOM_AppQuitEvent *event);
	void onAppActive (ATOM_AppActiveEvent *event);
	void onIdle (ATOM_WidgetIdleEvent *event);
	void onCommand (ATOM_WidgetCommandEvent *event);
	void onMenuPopup (ATOM_MenuPopupEvent *event);
	void onResize (ATOM_WidgetResizeEvent *event);
	void onSetFocus (ATOM_WidgetSetFocusEvent *event);
	void onKillFocus (ATOM_WidgetLostFocusEvent *event);
	void onHitTest (ATOM_WidgetHitTestEvent *event);
	void onLButtonDown (ATOM_WidgetLButtonDownEvent *event);
	void onLButtonUp (ATOM_WidgetLButtonUpEvent *event);
	void onMButtonDown (ATOM_WidgetMButtonDownEvent *event);
	void onMButtonUp (ATOM_WidgetMButtonUpEvent *event);
	void onRButtonDown (ATOM_WidgetRButtonDownEvent *event);
	void onRButtonUp (ATOM_WidgetRButtonUpEvent *event);
	void onMouseMove (ATOM_WidgetMouseMoveEvent *event);
	void onMouseWheel (ATOM_WidgetMouseWheelEvent *event);
	void onMouseEnter (ATOM_WidgetMouseEnterEvent *event);
	void onMouseLeave (ATOM_WidgetMouseLeaveEvent *event);
	void onKeyDown (ATOM_WidgetKeyDownEvent *event);
	void onKeyUp (ATOM_WidgetKeyUpEvent *event);
	void onChar (ATOM_WidgetCharEvent *event);
	void onClose (ATOM_TopWindowCloseEvent *event);
	void onCellLButtonDown (ATOM_CellLButtonDownEvent *event);
	void onCellLButtonUp (ATOM_CellLButtonUpEvent *event);
	void onCellRButtonDown (ATOM_CellRButtonDownEvent *event);
	void onCellRButtonUp (ATOM_CellRButtonUpEvent *event);
	void onCellClick (ATOM_CellLeftClickEvent *event);
	void onCellRClick (ATOM_CellRightClickEvent *event);
	void onCellMouseEnter (ATOM_CellMouseEnterEvent *event);
	void onCellMouseLeave (ATOM_CellMouseLeaveEvent *event);
	void onListBoxClick (ATOM_ListBoxClickEvent *event);
	void onScroll (ATOM_ScrollEvent *event);
	void onEditSysKey (ATOM_EditSysKeyEvent *event);
	void onEditTextChanged (ATOM_EditTextChangedEvent *event);
	void onInitDialog (ATOM_DialogInitEvent *event);
	void onFlashCall (ATOM_FlashCallEvent *event);
	void onFlashFSCommand (ATOM_FlashFSCommandEvent *event);
	void onUnhandled (ATOM_Event *event);

public:
	void setWindowMouseMoveFunc (const char *funcName);
	const char *getWindowMouseMoveFunc (void) const;
	void setWindowMouseWheelFunc (const char *funcName);
	const char *getWindowMouseWheelFunc (void) const;
	void setWindowMouseButtonDownFunc (const char *funcName);
	const char *getWindowMouseButtonDownFunc (void) const;
	void setWindowMouseButtonUpFunc (const char *funcName);
	const char *getWindowMouseButtonUpFunc (void) const;
	void setWindowKeyDownFunc (const char *funcName);
	const char *getWindowKeyDownFunc (void) const;
	void setWindowKeyUpFunc (const char *funcName);
	const char *getWindowKeyUpFunc (void) const;
	void setWindowCharFunc (const char *funcName);
	const char *getWindowCharFunc (void) const;
	void setWindowResizeFunc (const char *funcName);
	const char *getWindowResizeFunc (void) const;
	void setWindowActiveFunc (const char *funcName);
	const char *getWindowActiveFunc (void) const;
	void setWindowMoveFunc (const char *funcName);
	const char *getWindowMoveFunc (void) const;
	void setWindowExposeFunc (const char *funcName);
	const char *getWindowExposeFunc (void) const;
	void setWindowCloseFunc (const char *funcName);
	const char *getWindowCloseFunc (void) const;
	void setDeviceLostFunc (const char *funcName);
	const char *getDeviceLostFunc (void) const;
	void setDeviceResetFunc (const char *funcName);
	const char *getDeviceResetFunc (void) const;
	void setUnhandledFunc (const char *funcName);
	const char *getUnhandledFunc (void) const;
	void setAppIdleFunc (const char *funcName);
	const char *getAppIdleFunc (void) const;
	void setAppInitFunc (const char *funcName);
	const char *getAppInitFunc (void) const;
	void setAppExitFunc (const char *funcName);
	const char *getAppExitFunc (void) const;
	void setAppQuitFunc (const char *funcName);
	const char *getAppQuitFunc (void) const;
	void setAppActiveFunc (const char *funcName);
	const char *getAppActiveFunc (void) const;
	void setIdleFunc (const char *funcName);
	const char *getIdleFunc (void) const;
	void setCommandFunc (const char *funcName);
	const char *getCommandFunc (void) const;
	void setMenuPopupFunc (const char *funcName);
	const char *getMenuPopupFunc (void) const;
	void setResizeFunc (const char *funcName);
	const char *getResizeFunc (void) const;
	void setFocusFunc (const char *funcName);
	const char *getFocusFunc (void) const;
	void setKillFocusFunc (const char *funcName);
	const char *getKillFocusFunc (void) const;
	void setHitTestFunc (const char *funcName);
	const char *getHitTestFunc (void) const;
	void setLButtonDownFunc (const char *funcName);
	const char *getLButtonDownFunc (void) const;
	void setLButtonUpFunc (const char *funcName);
	const char *getLButtonUpFunc (void) const;
	void setMButtonDownFunc (const char *funcName);
	const char *getMButtonDownFunc (void) const;
	void setMButtonUpFunc (const char *funcName);
	const char *getMButtonUpFunc (void) const;
	void setRButtonDownFunc (const char *funcName);
	const char *getRButtonDownFunc (void) const;
	void setRButtonUpFunc (const char *funcName);
	const char *getRButtonUpFunc (void) const;
	void setMouseMoveFunc (const char *funcName);
	const char *getMouseMoveFunc (void) const;
	void setMouseWheelFunc (const char *funcName);
	const char *getMouseWheelFunc (void) const;
	void setMouseEnterFunc (const char *funcName);
	const char *getMouseEnterFunc (void) const;
	void setMouseLeaveFunc (const char *funcName);
	const char *getMouseLeaveFunc (void) const;
	void setKeyDownFunc (const char *funcName);
	const char *getKeyDownFunc (void) const;
	void setKeyUpFunc (const char *funcName);
	const char *getKeyUpFunc (void) const;
	void setCharFunc (const char *funcName);
	const char *getCharFunc (void) const;
	void setCloseFunc (const char *funcName);
	const char *getCloseFunc (void) const;
	void setCellLButtonDownFunc (const char *funcName);
	const char *getCellLButtonDownFunc (void) const;
	void setCellLButtonUpFunc (const char *funcName);
	const char *getCellLButtonUpFunc (void) const;
	void setCellRButtonDownFunc (const char *funcName);
	const char *getCellRButtonDownFunc (void) const;
	void setCellRButtonUpFunc (const char *funcName);
	const char *getCellRButtonUpFunc (void) const;
	void setCellClickFunc (const char *funcName);
	const char *getCellClickFunc (void) const;
	void setCellRClickFunc (const char *funcName);
	const char *getCellRClickFunc (void) const;
	void setCellMouseEnterFunc (const char *funcName);
	const char *getCellMouseEnterFunc (void) const;
	void setCellMouseLeaveFunc (const char *funcName);
	const char *getCellMouseLeaveFunc (void) const;
	void setListBoxClickFunc (const char *funcName);
	const char *getListBoxClickFunc (void) const;
	void setScrollFunc (const char *funcName);
	const char *getScrollFunc (void) const;
	void setEditSysKeyFunc (const char *funcName);
	const char *getEditSysKeyFunc (void) const;
	void setEditTextChangedFunc (const char *funcName);
	const char *getEditTextChangedFunc (void) const;
	void setInitDialogFunc (const char *funcName);
	const char *getInitDialogFunc (void) const;
	void setFlashCallFunc (const char *funcName);
	const char *getFlashCallFunc (void) const;
	void setFlashFSCommandFunc (const char *funcName);
	const char *getFlashFSCommandFunc (void) const;

public:
	static ATOM_ScriptEventTrigger *getCurrent (void);

private:
	void callHandlerFunc (const char *func, ATOM_Event *event);

private:
	ATOM_STRING _unhandledFunc;
	ATOM_STRING _windowMouseMoveFunc;
	ATOM_STRING _windowMouseWheelFunc;
	ATOM_STRING _windowMouseButtonDownFunc;
	ATOM_STRING _windowMouseButtonUpFunc;
	ATOM_STRING _windowKeyDownFunc;
	ATOM_STRING _windowKeyUpFunc;
	ATOM_STRING _windowCharFunc;
	ATOM_STRING _windowResizeFunc;
	ATOM_STRING _windowActiveFunc;
	ATOM_STRING _windowMoveFunc;
	ATOM_STRING _windowExposeFunc;
	ATOM_STRING _windowCloseFunc;
	ATOM_STRING _deviceLostFunc;
	ATOM_STRING _deviceResetFunc;
	ATOM_STRING _appIdleFunc;
	ATOM_STRING _appInitFunc;
	ATOM_STRING _appExitFunc;
	ATOM_STRING _appQuitFunc;
	ATOM_STRING _appActiveFunc;
	ATOM_STRING _idleFunc;
	ATOM_STRING _commandFunc;
	ATOM_STRING _menuPopupFunc;
	ATOM_STRING _resizeFunc;
	ATOM_STRING _setFocusFunc;
	ATOM_STRING _killFocusFunc;
	ATOM_STRING _hitTestFunc;
	ATOM_STRING _lbuttonDownFunc;
	ATOM_STRING _lbuttonUpFunc;
	ATOM_STRING _mbuttonDownFunc;
	ATOM_STRING _mbuttonUpFunc;
	ATOM_STRING _rbuttonDownFunc;
	ATOM_STRING _rbuttonUpFunc;
	ATOM_STRING _mouseMoveFunc;
	ATOM_STRING _mouseWheelFunc;
	ATOM_STRING _mouseEnterFunc;
	ATOM_STRING _mouseLeaveFunc;
	ATOM_STRING _keyDownFunc;
	ATOM_STRING _keyUpFunc;
	ATOM_STRING _charFunc;
	ATOM_STRING _closeFunc;
	ATOM_STRING _cellLButtonDownFunc;
	ATOM_STRING _cellLButtonUpFunc;
	ATOM_STRING _cellRButtonDownFunc;
	ATOM_STRING _cellRButtonUpFunc;
	ATOM_STRING _cellClickFunc;
	ATOM_STRING _cellRClickFunc;
	ATOM_STRING _cellMouseEnterFunc;
	ATOM_STRING _cellMouseLeaveFunc;
	ATOM_STRING _listboxClickFunc;
	ATOM_STRING _scrollFunc;
	ATOM_STRING _editSysKeyFunc;
	ATOM_STRING _editTextChangedFunc;
	ATOM_STRING _initDialogFunc;
	ATOM_STRING _flashCallFunc;
	ATOM_STRING _flashFSCommandFunc;

	ATOM_Script *_script;
	static ATOM_VECTOR<ATOM_ScriptEventTrigger*> _currentStack;

	ATOM_DECLARE_EVENT_MAP(ATOM_ScriptEventTrigger, ATOM_EventTrigger)
};

#endif // __ATOM3D_SCRIPTEVENTTRIGGER_H
