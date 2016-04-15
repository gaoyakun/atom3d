#include "StdAfx.h"
#include "scripteventtrigger.h"

ATOM_VECTOR<ATOM_ScriptEventTrigger *> ATOM_ScriptEventTrigger::_currentStack;

ATOM_BEGIN_EVENT_MAP(ATOM_ScriptEventTrigger, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_MouseMoveEvent, onWindowMouseMove)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_MouseWheelEvent, onWindowMouseWheel)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_MouseButtonDownEvent, onWindowMouseButtonDown)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_MouseButtonUpEvent, onWindowMouseButtonUp)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_KeyDownEvent, onWindowKeyDown)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_KeyUpEvent, onWindowKeyUp)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_CharEvent, onWindowChar)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WindowResizeEvent, onWindowResize)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WindowActiveEvent, onWindowActive)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WindowMoveEvent, onWindowMove)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WindowExposeEvent, onWindowExpose)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WindowCloseEvent, onWindowClose)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_DeviceLostEvent, onDeviceLost)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_DeviceResetEvent, onDeviceReset)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_AppIdleEvent, onAppIdle)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_AppInitEvent, onAppInit)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_AppExitEvent, onAppExit)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_AppQuitEvent, onAppQuit)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_AppActiveEvent, onAppActive)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WidgetIdleEvent, onIdle)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WidgetCommandEvent, onCommand)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_MenuPopupEvent, onMenuPopup)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WidgetResizeEvent, onResize)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WidgetSetFocusEvent, onSetFocus)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WidgetLostFocusEvent, onKillFocus)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WidgetLButtonUpEvent, onLButtonUp)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WidgetMButtonDownEvent, onMButtonDown)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WidgetMButtonUpEvent, onMButtonUp)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WidgetRButtonDownEvent, onRButtonDown)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WidgetRButtonUpEvent, onRButtonUp)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WidgetMouseMoveEvent, onMouseMove)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WidgetMouseWheelEvent, onMouseWheel)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WidgetMouseEnterEvent, onMouseEnter)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WidgetMouseLeaveEvent, onMouseLeave)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WidgetKeyDownEvent, onKeyDown)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WidgetKeyUpEvent, onKeyUp)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WidgetCharEvent, onChar)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_TopWindowCloseEvent, onClose)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_CellLButtonDownEvent, onCellLButtonDown)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_CellLButtonUpEvent, onCellLButtonUp)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_CellRButtonDownEvent, onCellRButtonDown)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_CellRButtonUpEvent, onCellRButtonUp)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_CellLeftClickEvent, onCellClick)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_CellRightClickEvent, onCellRClick)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_CellMouseEnterEvent, onCellMouseEnter)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_CellMouseLeaveEvent, onCellMouseLeave)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_ListBoxClickEvent, onListBoxClick)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_ScrollEvent, onScroll)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_EditSysKeyEvent, onEditSysKey)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_EditTextChangedEvent, onEditTextChanged)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_DialogInitEvent, onInitDialog)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_FlashCallEvent, onFlashCall)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_FlashFSCommandEvent, onFlashFSCommand)
	ATOM_EVENT_HANDLER(ATOM_ScriptEventTrigger, ATOM_WidgetHitTestEvent, onHitTest)
	ATOM_UNHANDLED_EVENT_HANDLER(ATOM_ScriptEventTrigger, onUnhandled)
ATOM_END_EVENT_MAP

ATOM_ScriptEventTrigger::ATOM_ScriptEventTrigger (ATOM_Script *script)
{
	ATOM_ASSERT(dynamic_cast<ATOM_Script*>(script));
	_script = script;
}

ATOM_ScriptEventTrigger::~ATOM_ScriptEventTrigger (void)
{
}

void ATOM_ScriptEventTrigger::callHandlerFunc (const char *func, ATOM_Event *event)
{
	if (_script && func && func[0])
	{
		_currentStack.push_back (this);

		_script->setFunction (func);
		_script->setNumParameters (1);
		_script->setParameter (0, event);
		_script->setNumResults (0);
		_script->execute ();

		_currentStack.pop_back ();
	}

	setAutoCallHost (false);
	callHost (event);
}

void ATOM_ScriptEventTrigger::onWindowMouseMove (ATOM_MouseMoveEvent *event)
{
	callHandlerFunc (_windowMouseMoveFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onWindowMouseWheel (ATOM_MouseWheelEvent *event)
{
	callHandlerFunc (_windowMouseWheelFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onWindowMouseButtonDown (ATOM_MouseButtonDownEvent *event)
{
	callHandlerFunc (_windowMouseButtonDownFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onWindowMouseButtonUp (ATOM_MouseButtonUpEvent *event)
{
	callHandlerFunc (_windowMouseButtonUpFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onWindowKeyDown (ATOM_KeyDownEvent *event)
{
	callHandlerFunc (_windowKeyDownFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onWindowKeyUp (ATOM_KeyUpEvent *event)
{
	callHandlerFunc (_windowKeyUpFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onWindowChar (ATOM_CharEvent *event)
{
	callHandlerFunc (_windowCharFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onWindowResize (ATOM_WindowResizeEvent *event)
{
	callHandlerFunc (_windowResizeFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onWindowActive (ATOM_WindowActiveEvent *event)
{
	callHandlerFunc (_windowActiveFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onWindowMove (ATOM_WindowMoveEvent *event)
{
	callHandlerFunc (_windowMoveFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onWindowExpose (ATOM_WindowExposeEvent *event)
{
	callHandlerFunc (_windowExposeFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onWindowClose (ATOM_WindowCloseEvent *event)
{
	callHandlerFunc (_windowCloseFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onDeviceLost (ATOM_DeviceLostEvent *event)
{
	callHandlerFunc (_deviceLostFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onDeviceReset (ATOM_DeviceResetEvent *event)
{
	callHandlerFunc (_deviceResetFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onAppIdle (ATOM_AppIdleEvent *event)
{
	callHandlerFunc (_appIdleFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onAppInit (ATOM_AppInitEvent *event)
{
	callHandlerFunc (_appInitFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onAppExit (ATOM_AppExitEvent *event)
{
	callHandlerFunc (_appExitFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onAppQuit (ATOM_AppQuitEvent *event)
{
	callHandlerFunc (_appQuitFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onAppActive (ATOM_AppActiveEvent *event)
{
	callHandlerFunc (_appActiveFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onIdle (ATOM_WidgetIdleEvent *event)
{
	callHandlerFunc (_idleFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onCommand (ATOM_WidgetCommandEvent *event)
{
	callHandlerFunc (_commandFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onMenuPopup (ATOM_MenuPopupEvent *event)
{
	callHandlerFunc (_menuPopupFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onResize (ATOM_WidgetResizeEvent *event)
{
	callHandlerFunc (_resizeFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onSetFocus (ATOM_WidgetSetFocusEvent *event)
{
	callHandlerFunc (_setFocusFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onKillFocus (ATOM_WidgetLostFocusEvent *event)
{
	callHandlerFunc (_killFocusFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onHitTest (ATOM_WidgetHitTestEvent *event)
{
	callHandlerFunc (_hitTestFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onLButtonDown (ATOM_WidgetLButtonDownEvent *event)
{
	callHandlerFunc (_lbuttonDownFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onLButtonUp (ATOM_WidgetLButtonUpEvent *event)
{
	callHandlerFunc (_lbuttonUpFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onMButtonDown (ATOM_WidgetMButtonDownEvent *event)
{
	callHandlerFunc (_mbuttonDownFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onMButtonUp (ATOM_WidgetMButtonUpEvent *event)
{
	callHandlerFunc (_mbuttonUpFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onRButtonDown (ATOM_WidgetRButtonDownEvent *event)
{
	callHandlerFunc (_rbuttonDownFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onRButtonUp (ATOM_WidgetRButtonUpEvent *event)
{
	callHandlerFunc (_rbuttonUpFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onMouseMove (ATOM_WidgetMouseMoveEvent *event)
{
	callHandlerFunc (_mouseMoveFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onMouseWheel (ATOM_WidgetMouseWheelEvent *event)
{
	callHandlerFunc (_mouseWheelFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onMouseEnter (ATOM_WidgetMouseEnterEvent *event)
{
	callHandlerFunc (_mouseEnterFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onMouseLeave (ATOM_WidgetMouseLeaveEvent *event)
{
	callHandlerFunc (_mouseLeaveFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onKeyDown (ATOM_WidgetKeyDownEvent *event)
{
	callHandlerFunc (_keyDownFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onKeyUp (ATOM_WidgetKeyUpEvent *event)
{
	callHandlerFunc (_keyUpFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onChar (ATOM_WidgetCharEvent *event)
{
	callHandlerFunc (_charFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onClose (ATOM_TopWindowCloseEvent *event)
{
	callHandlerFunc (_closeFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onCellLButtonDown (ATOM_CellLButtonDownEvent *event)
{
	callHandlerFunc (_cellLButtonDownFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onCellLButtonUp (ATOM_CellLButtonUpEvent *event)
{
	callHandlerFunc (_cellLButtonUpFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onCellRButtonDown (ATOM_CellRButtonDownEvent *event)
{
	callHandlerFunc (_cellRButtonDownFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onCellRButtonUp (ATOM_CellRButtonUpEvent *event)
{
	callHandlerFunc (_cellRButtonUpFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onCellClick (ATOM_CellLeftClickEvent *event)
{
	callHandlerFunc (_cellClickFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onCellRClick (ATOM_CellRightClickEvent *event)
{
	callHandlerFunc (_cellRClickFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onCellMouseEnter (ATOM_CellMouseEnterEvent *event)
{
	callHandlerFunc (_cellMouseEnterFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onCellMouseLeave (ATOM_CellMouseLeaveEvent *event)
{
	callHandlerFunc (_cellMouseLeaveFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onListBoxClick (ATOM_ListBoxClickEvent *event)
{
	callHandlerFunc (_listboxClickFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onScroll (ATOM_ScrollEvent *event)
{
	callHandlerFunc (_scrollFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onEditSysKey (ATOM_EditSysKeyEvent *event)
{
	callHandlerFunc (_editSysKeyFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onEditTextChanged (ATOM_EditTextChangedEvent *event)
{
	callHandlerFunc (_editTextChangedFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onInitDialog (ATOM_DialogInitEvent *event)
{
	callHandlerFunc (_initDialogFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onFlashCall (ATOM_FlashCallEvent *event)
{
	callHandlerFunc (_flashCallFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onFlashFSCommand (ATOM_FlashFSCommandEvent *event)
{
	callHandlerFunc (_flashFSCommandFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::onUnhandled (ATOM_Event *event)
{
	callHandlerFunc (_unhandledFunc.c_str(), event);
}

void ATOM_ScriptEventTrigger::setUnhandledFunc (const char *funcName)
{
	_unhandledFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getUnhandledFunc (void) const
{
	return _unhandledFunc.c_str();
}

void ATOM_ScriptEventTrigger::setWindowMouseMoveFunc (const char *funcName)
{
	_windowMouseMoveFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getWindowMouseMoveFunc (void) const
{
	return _windowMouseMoveFunc.c_str();
}

void ATOM_ScriptEventTrigger::setWindowMouseWheelFunc (const char *funcName)
{
	_windowMouseWheelFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getWindowMouseWheelFunc (void) const
{
	return _windowMouseWheelFunc.c_str();
}

void ATOM_ScriptEventTrigger::setWindowMouseButtonDownFunc (const char *funcName)
{
	_windowMouseButtonDownFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getWindowMouseButtonDownFunc (void) const
{
	return _windowMouseButtonDownFunc.c_str();
}

void ATOM_ScriptEventTrigger::setWindowMouseButtonUpFunc (const char *funcName)
{
	_windowMouseButtonUpFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getWindowMouseButtonUpFunc (void) const
{
	return _windowMouseButtonUpFunc.c_str();
}

void ATOM_ScriptEventTrigger::setWindowKeyDownFunc (const char *funcName)
{
	_windowKeyDownFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getWindowKeyDownFunc (void) const
{
	return _windowKeyDownFunc.c_str();
}

void ATOM_ScriptEventTrigger::setWindowKeyUpFunc (const char *funcName)
{
	_windowKeyUpFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getWindowKeyUpFunc (void) const
{
	return _windowKeyUpFunc.c_str();
}

void ATOM_ScriptEventTrigger::setWindowCharFunc (const char *funcName)
{
	_windowCharFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getWindowCharFunc (void) const
{
	return _windowCharFunc.c_str();
}

void ATOM_ScriptEventTrigger::setWindowResizeFunc (const char *funcName)
{
	_windowResizeFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getWindowResizeFunc (void) const
{
	return _windowResizeFunc.c_str();
}

void ATOM_ScriptEventTrigger::setWindowActiveFunc (const char *funcName)
{
	_windowActiveFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getWindowActiveFunc (void) const
{
	return _windowActiveFunc.c_str();
}

void ATOM_ScriptEventTrigger::setWindowMoveFunc (const char *funcName)
{
	_windowMoveFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getWindowMoveFunc (void) const
{
	return _windowMoveFunc.c_str();
}

void ATOM_ScriptEventTrigger::setWindowExposeFunc (const char *funcName)
{
	_windowExposeFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getWindowExposeFunc (void) const
{
	return _windowExposeFunc.c_str();
}

void ATOM_ScriptEventTrigger::setWindowCloseFunc (const char *funcName)
{
	_windowCloseFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getWindowCloseFunc (void) const
{
	return _windowCloseFunc.c_str();
}

void ATOM_ScriptEventTrigger::setDeviceLostFunc (const char *funcName)
{
	_deviceLostFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getDeviceLostFunc (void) const
{
	return _deviceLostFunc.c_str();
}

void ATOM_ScriptEventTrigger::setDeviceResetFunc (const char *funcName)
{
	_deviceResetFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getDeviceResetFunc (void) const
{
	return _deviceResetFunc.c_str();
}

void ATOM_ScriptEventTrigger::setAppIdleFunc (const char *funcName)
{
	_appIdleFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getAppIdleFunc (void) const
{
	return _appIdleFunc.c_str();
}

void ATOM_ScriptEventTrigger::setAppInitFunc (const char *funcName)
{
	_appInitFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getAppInitFunc (void) const
{
	return _appInitFunc.c_str();
}

void ATOM_ScriptEventTrigger::setAppExitFunc (const char *funcName)
{
	_appExitFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getAppExitFunc (void) const
{
	return _appExitFunc.c_str();
}

void ATOM_ScriptEventTrigger::setAppQuitFunc (const char *funcName)
{
	_appQuitFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getAppQuitFunc (void) const
{
	return _appQuitFunc.c_str();
}

void ATOM_ScriptEventTrigger::setAppActiveFunc (const char *funcName)
{
	_appActiveFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getAppActiveFunc (void) const
{
	return _appActiveFunc.c_str();
}

void ATOM_ScriptEventTrigger::setIdleFunc (const char *funcName)
{
	_idleFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getIdleFunc (void) const
{
	return _idleFunc.c_str();
}

void ATOM_ScriptEventTrigger::setCommandFunc (const char *funcName)
{
	_commandFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getCommandFunc (void) const
{
	return _commandFunc.c_str();
}

void ATOM_ScriptEventTrigger::setMenuPopupFunc (const char *funcName)
{
	_menuPopupFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getMenuPopupFunc (void) const
{
	return _menuPopupFunc.c_str();
}

void ATOM_ScriptEventTrigger::setResizeFunc (const char *funcName)
{
	_resizeFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getResizeFunc (void) const
{
	return _menuPopupFunc.c_str();
}

void ATOM_ScriptEventTrigger::setFocusFunc (const char *funcName)
{
	_setFocusFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getFocusFunc (void) const
{
	return _setFocusFunc.c_str();
}

void ATOM_ScriptEventTrigger::setKillFocusFunc (const char *funcName)
{
	_killFocusFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getKillFocusFunc (void) const
{
	return _killFocusFunc.c_str();
}

void ATOM_ScriptEventTrigger::setLButtonDownFunc (const char *funcName)
{
	_lbuttonDownFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getLButtonDownFunc (void) const
{
	return _lbuttonDownFunc.c_str();
}

void ATOM_ScriptEventTrigger::setHitTestFunc (const char *funcName)
{
	_hitTestFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getHitTestFunc (void) const
{
	return _hitTestFunc.c_str();
}

void ATOM_ScriptEventTrigger::setLButtonUpFunc (const char *funcName)
{
	_lbuttonUpFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getLButtonUpFunc (void) const
{
	return _lbuttonUpFunc.c_str();
}

void ATOM_ScriptEventTrigger::setMButtonDownFunc (const char *funcName)
{
	_mbuttonDownFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getMButtonDownFunc (void) const
{
	return _mbuttonDownFunc.c_str();
}

void ATOM_ScriptEventTrigger::setMButtonUpFunc (const char *funcName)
{
	_mbuttonUpFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getMButtonUpFunc (void) const
{
	return _mbuttonUpFunc.c_str();
}

void ATOM_ScriptEventTrigger::setRButtonDownFunc (const char *funcName)
{
	_rbuttonDownFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getRButtonDownFunc (void) const
{
	return _rbuttonDownFunc.c_str();
}

void ATOM_ScriptEventTrigger::setRButtonUpFunc (const char *funcName)
{
	_rbuttonUpFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getRButtonUpFunc (void) const
{
	return _rbuttonUpFunc.c_str();
}

void ATOM_ScriptEventTrigger::setMouseMoveFunc (const char *funcName)
{
	_mouseMoveFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getMouseMoveFunc (void) const
{
	return _mouseMoveFunc.c_str();
}

void ATOM_ScriptEventTrigger::setMouseWheelFunc (const char *funcName)
{
	_mouseWheelFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getMouseWheelFunc (void) const
{
	return _mouseWheelFunc.c_str();
}

void ATOM_ScriptEventTrigger::setMouseEnterFunc (const char *funcName)
{
	_mouseEnterFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getMouseEnterFunc (void) const
{
	return _mouseEnterFunc.c_str();
}

void ATOM_ScriptEventTrigger::setMouseLeaveFunc (const char *funcName)
{
	_mouseLeaveFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getMouseLeaveFunc (void) const
{
	return _mouseLeaveFunc.c_str();
}

void ATOM_ScriptEventTrigger::setKeyDownFunc (const char *funcName)
{
	_keyDownFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getKeyDownFunc (void) const
{
	return _keyDownFunc.c_str();
}

void ATOM_ScriptEventTrigger::setKeyUpFunc (const char *funcName)
{
	_keyUpFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getKeyUpFunc (void) const
{
	return _keyUpFunc.c_str();
}

void ATOM_ScriptEventTrigger::setCharFunc (const char *funcName)
{
	_charFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getCharFunc (void) const
{
	return _charFunc.c_str();
}

void ATOM_ScriptEventTrigger::setCloseFunc (const char *funcName)
{
	_closeFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getCloseFunc (void) const
{
	return _closeFunc.c_str();
}

void ATOM_ScriptEventTrigger::setCellLButtonDownFunc (const char *funcName)
{
	_cellLButtonDownFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getCellLButtonDownFunc (void) const
{
	return _cellLButtonDownFunc.c_str();
}

void ATOM_ScriptEventTrigger::setCellLButtonUpFunc (const char *funcName)
{
	_cellLButtonUpFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getCellLButtonUpFunc (void) const
{
	return _cellLButtonUpFunc.c_str();
}

void ATOM_ScriptEventTrigger::setCellRButtonDownFunc (const char *funcName)
{
	_cellRButtonDownFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getCellRButtonDownFunc (void) const
{
	return _cellRButtonDownFunc.c_str();
}

void ATOM_ScriptEventTrigger::setCellRButtonUpFunc (const char *funcName)
{
	_cellRButtonUpFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getCellRButtonUpFunc (void) const
{
	return _cellRButtonUpFunc.c_str();
}

void ATOM_ScriptEventTrigger::setCellClickFunc (const char *funcName)
{
	_cellClickFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getCellClickFunc (void) const
{
	return _cellClickFunc.c_str();
}

void ATOM_ScriptEventTrigger::setCellRClickFunc (const char *funcName)
{
	_cellRClickFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getCellRClickFunc (void) const
{
	return _cellRClickFunc.c_str();
}

void ATOM_ScriptEventTrigger::setCellMouseEnterFunc (const char *funcName)
{
	_cellMouseEnterFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getCellMouseEnterFunc (void) const
{
	return _cellMouseEnterFunc.c_str();
}

void ATOM_ScriptEventTrigger::setCellMouseLeaveFunc (const char *funcName)
{
	_cellMouseLeaveFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getCellMouseLeaveFunc (void) const
{
	return _cellMouseLeaveFunc.c_str();
}

void ATOM_ScriptEventTrigger::setListBoxClickFunc (const char *funcName)
{
	_listboxClickFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getListBoxClickFunc (void) const
{
	return _listboxClickFunc.c_str();
}

void ATOM_ScriptEventTrigger::setScrollFunc (const char *funcName)
{
	_scrollFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getScrollFunc (void) const
{
	return _scrollFunc.c_str();
}

void ATOM_ScriptEventTrigger::setEditSysKeyFunc (const char *funcName)
{
	_editSysKeyFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getEditSysKeyFunc (void) const
{
	return _editSysKeyFunc.c_str();
}

void ATOM_ScriptEventTrigger::setEditTextChangedFunc (const char *funcName)
{
	_editTextChangedFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getEditTextChangedFunc (void) const
{
	return _editTextChangedFunc.c_str();
}

void ATOM_ScriptEventTrigger::setInitDialogFunc (const char *funcName)
{
	_initDialogFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getInitDialogFunc (void) const
{
	return _initDialogFunc.c_str();
}

void ATOM_ScriptEventTrigger::setFlashCallFunc (const char *funcName)
{
	_flashCallFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getFlashCallFunc (void) const
{
	return _flashCallFunc.c_str();
}

void ATOM_ScriptEventTrigger::setFlashFSCommandFunc (const char *funcName)
{
	_flashFSCommandFunc = funcName ? funcName : "";
}

const char *ATOM_ScriptEventTrigger::getFlashFSCommandFunc (void) const
{
	return _flashFSCommandFunc.c_str();
}

ATOM_ScriptEventTrigger *ATOM_ScriptEventTrigger::getCurrent (void)
{
	return _currentStack.empty() ? 0 : _currentStack.back();
}

