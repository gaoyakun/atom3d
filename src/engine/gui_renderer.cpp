#include "stdafx.h"
#include <kernel/addons.h>
#include "gui_renderer.h"
#include "gui_events.h"
#include "desktop.h"
#include "topwindow.h"
#include "dragdrop.h"
#include "gui_misc.h"

#define EDITID_SCRIPT 0
#define EDITID_OUTPUT 1
#define MENUID_RUNSCRIPT 1
#define MENUID_CLOSE 2
#define MENUID_CLEAROUTPUT 3
#define MENUID_CLEARSCRIPT 4

ATOM_BEGIN_EVENT_MAP_NOPARENT(ATOM_GUIRenderer)
	ATOM_EVENT_HANDLER(ATOM_GUIRenderer, ATOM_MouseMoveEvent, onMouseMove)
	ATOM_EVENT_HANDLER(ATOM_GUIRenderer, ATOM_MouseWheelEvent, onMouseWheel)
	ATOM_EVENT_HANDLER(ATOM_GUIRenderer, ATOM_MouseButtonDownEvent, onMouseButtonDown)
	ATOM_EVENT_HANDLER(ATOM_GUIRenderer, ATOM_MouseButtonUpEvent, onMouseButtonUp)
	ATOM_EVENT_HANDLER(ATOM_GUIRenderer, ATOM_MouseDblClickEvent, onMouseDblClick)
	ATOM_EVENT_HANDLER(ATOM_GUIRenderer, ATOM_CharEvent, onChar)
	ATOM_EVENT_HANDLER(ATOM_GUIRenderer, ATOM_KeyDownEvent, onKeyDown)
	ATOM_EVENT_HANDLER(ATOM_GUIRenderer, ATOM_KeyUpEvent, onKeyUp)
	ATOM_EVENT_HANDLER(ATOM_GUIRenderer, ATOM_AppIdleEvent, onIdle)
	ATOM_EVENT_HANDLER(ATOM_GUIRenderer, ATOM_AppWMEvent, onWinMessage)
	ATOM_EVENT_HANDLER(ATOM_GUIRenderer, ATOM_WindowFilesDroppedEvent, onFilesDropped)
ATOM_END_EVENT_MAP

ATOM_VECTOR<ATOM_Rect2Di> ATOM_GUIRenderer::_scissorStack;
bool ATOM_GUIRenderer::_mtEnabled = false;

unsigned GUIrendererCount = 0;

static bool isPopupWidget (ATOM_Widget *widget)
{
	return widget && widget->isPopup ();
}

void ScriptOutputFunc (const char *str, bool error)
{
	if (error)
	{
		ATOM_LOGGER::error ("%s", str);
	}
	else
	{
		ATOM_LOGGER::log ("%s", str);
	}
}

static void ATOM_CALL ScriptCallback (ATOM_Script *script)
{
	ATOM_BindToScript (script);
}

static ATOM_GUIRenderer *_mainGUI = 0;
static ATOM_GUIRenderer *getMainGUI (void)
{
	return _mainGUI;
}

ATOM_GUIRenderer::ATOM_GUIRenderer (bool asMain)
{
	if (asMain)
	{
		_mainGUI = this;
	}

	_viewport.point.x = 0;
	_viewport.point.y = 0;
	_viewport.size.w = 0;
	_viewport.size.h = 0;
	_desktop = 0;
	_draggingWidget = 0;
	_hoverWidget = 0;
	_focusWidget = 0;
	_captureWidget = 0;
	_mouseX = 0;
	_mouseY = 0;
	_leftDown = false;
	_middleDown = false;
	_rightDown = false;
	_displayDirty = true;
	_3dMode = false;
	_renderTargetWindow = 0;
	_zValue = 0.f;
	_nofog = true;
	_audioDevice = NULL;
	_draggingX = 0;
	_draggingY = 0;
	_manualChange = false;
	_keyboardLayout = NULL;
	_scriptDialog = 0;
	_script = 0;
	_scriptManager = 0;
	_scriptDialogImageList = 0;
	_scriptDialogTrigger = 0;
	_isMainGUI = asMain;
	_preDragWidget = 0;
	_cancelUpdate = false;

	if (_isMainGUI)
	{
		ATOM_AutoFile debugScript("/autoload.lua", ATOM_VFS::text|ATOM_VFS::read);
		_scriptManager = ATOM_NEW(ATOM_ScriptManager);
		_script = _scriptManager->createScript (&ScriptCallback);
		_script->setGlobalp ("MainGUIRenderer", this);
		_script->setErrorOutputCallback (&ScriptOutputFunc);

		if (debugScript)
		{
			unsigned size = debugScript->size();
			if (size > 0)
			{
				char *content = (char*)ATOM_MALLOC(size+1);
				size = debugScript->read (content, size);
				content[size] = '\0';
				_script->executeString (content);
				ATOM_FREE(content);

				if (_script->isFunction ("startup"))
				{
					_script->executeString ("startup();");
				}
			}
		}
	}
}

ATOM_GUIRenderer::~ATOM_GUIRenderer (void)
{
	closeScriptDialog ();

	if (_script)
	{
		if (_script->isFunction ("cleanup"))
		{
			_script->executeString ("cleanup()");
		}
		ATOM_DELETE(_script);
		_script = 0;
	}

	ATOM_DELETE(_scriptManager);
	_scriptManager = 0;

	ATOM_DELETE (_desktop);
}

void ATOM_GUIRenderer::invalidate (void)
{
	_displayDirty = true;
}

void ATOM_GUIRenderer::setViewport (const ATOM_Rect2Di &rect)
{
	_viewport = rect;

	if (_desktop)
	{
		ATOM_Rect2Di rc;
		rc.point.x = 0;
		rc.point.y = 0;
		rc.size.w = rect.size.w;
		rc.size.h = rect.size.h;

		_desktop->resize (rc);
	}
}

const ATOM_Rect2Di &ATOM_GUIRenderer::getViewport (void) const
{
	return _viewport;
}

void ATOM_GUIRenderer::setAudioDevice(ATOM_AudioDeviceWrapper *device)
{
	_audioDevice = device;
}

ATOM_AudioDeviceWrapper *ATOM_GUIRenderer::getAudioDevice()
{
	return _audioDevice;
}

ATOM_Desktop *ATOM_GUIRenderer::getDesktop (void)
{
	if (!_desktop)
	{
		_desktop = ATOM_NEW(ATOM_Desktop, this);
	}

	return _desktop;
}

//void ATOM_GUIRenderer::renderToTexture (ATOM_Texture *texture)
//{
//  ATOM_RenderDevice *device = ATOM_GetRenderDevice ();
//  if (device)
//  {
//    device->SetRenderTarget (texture);
//    render ();
//    device->SetRenderTarget (0);
//  }
//}

void ATOM_GUIRenderer::doRender (void)
{
	// empty event queue
	ATOM_APP->dispatchPendingEvents ();

	ATOM_RenderDevice *device = ATOM_GetRenderDevice ();
	if (device)
	{
		if (pushScissor (ATOM_Rect2Di(_viewport.point.x, _viewport.point.y, _viewport.size.w, _viewport.size.h), true))
		{
			getDesktop()->draw ();
			drawDragIndicator ();

			popScissor ();
		}
	}
}

void ATOM_GUIRenderer::render (void)
{
	if (1 || _displayDirty)
	{
		doRender ();

		_displayDirty = false;
	}
}

void ATOM_GUIRenderer::onFilesDropped (ATOM_WindowFilesDroppedEvent *event)
{
	int x = event->x;
	int y = event->y;

	ATOM_GUIHitTestResult hitResult;
	ATOM_Point2Di position;
	ATOM_Point2Di clientPosition;

	ATOM_Widget *hit = hitTest (x, y, &hitResult, &position, &clientPosition);
	if (!hit || !hit->isEnabled ())
	{
		return;
	}

	event->x = clientPosition.x;
	event->y = clientPosition.y;
	hit->handleEvent (event);
}

void ATOM_GUIRenderer::onWinMessage (ATOM_AppWMEvent *event)
{
	if (event->msg == WM_INPUTLANGCHANGE)
	{
		if (!_manualChange)
		{
			_keyboardLayout = ::GetKeyboardLayout (0);
		}
		_manualChange = false;
	}
	else if (event->msg == WM_ACTIVATEAPP)
	{
		if (!event->wParam)
		{
			setCapture (0);
		}
	}

	if (_focusWidget)
	{
		switch (event->msg)
		{
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		case WM_DEADCHAR:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_SYSDEADCHAR:
		case WM_SYSCHAR:
		case WM_IME_CHAR:
		case WM_IME_COMPOSITION:
		case WM_IME_COMPOSITIONFULL:
		case WM_IME_CONTROL:
		case WM_IME_ENDCOMPOSITION:
		case WM_IME_KEYDOWN:
		case WM_IME_KEYUP:
		case WM_IME_NOTIFY:
		case WM_IME_REQUEST:
		case WM_IME_SELECT:
		case WM_IME_SETCONTEXT:
		case WM_IME_STARTCOMPOSITION:
		case WM_INPUTLANGCHANGE:
		case WM_HELP:
		case WM_CANCELMODE: 
		default:
			_focusWidget->handleEvent (event);
			break;
		}
	}
}

void ATOM_GUIRenderer::onMouseMove (ATOM_MouseMoveEvent *event)
{
	int x = event->x;
	int y = event->y;
	int xrel = event->xrel;
	int yrel = event->yrel;
	bool leftDown = event->states[0] == KEYSTATE_PRESSED;
	bool middleDown = event->states[1] == KEYSTATE_PRESSED;
	bool rightDown = event->states[2] == KEYSTATE_PRESSED;

	_mouseX = x;
	_mouseY = y;

	if (_draggingWidget)
	{
		ATOM_WidgetDragEvent event(_draggingWidget->getId(), x - _draggingX, y - _draggingY);
		_draggingWidget->handleEvent (&event);
		_draggingWidget->moveBy (event.cx, event.cy);
		_draggingX = x;
		_draggingY = y;
		return;
	}
	else if (_preDragWidget && _preDragWidget->isDragEnabled())
	{
		ATOM_Widget *hit = _preDragWidget;
		_preDragWidget = 0;

		_dragSource = ATOM_NEW(ATOM_DragSource);
		_dragSource->setWidget(hit);
		_dragSource->setDraggingPosition (ATOM_Point2Di(event->x, event->y));
		_dragSource->setIndicatorImageId (hit->getClientImageId ());
		ATOM_Rect2Di rc = hit->getWidgetRect ();
		rc.point.x = -rc.size.w / 2;
		rc.point.y = -rc.size.h / 2;
		_dragSource->setIndicatorRect (rc);

		ATOM_WidgetDragStartEvent e(hit->getId(), _preDragPosition.x, _preDragPosition.y, true, _dragSource.get());
		hit->handleEvent (&e);

		if (!e.allow)
		{
			_dragSource = 0;
		}
	}
	else if (_dragSource)
	{
		_dragSource->setDraggingPosition (ATOM_Point2Di(_mouseX, _mouseY));
	}

	ATOM_GUIHitTestResult hitResult;
	ATOM_Point2Di position;
	ATOM_Point2Di clientPosition;

	ATOM_Widget *hit = hitTest (x, y, &hitResult, &position, &clientPosition);
	ATOM_Widget *newHoverWidget = (hitResult.hitPoint == HitClient) ? hit : 0;
	if (_dragSource && newHoverWidget && newHoverWidget->isDropEnabled ())
	{
		ATOM_Point2Di p(x, y);
		newHoverWidget->screenToClient (&p);

		ATOM_WidgetDragOverEvent dragOverEvent(newHoverWidget->getId(), p.x, p.y, true, _dragSource.get());
		newHoverWidget->handleEvent (&dragOverEvent);

		_dropTarget = dragOverEvent.accept ? newHoverWidget : 0;
	}
	else
	{
		_dropTarget = 0;
	}

	ATOM_TopWindow *currentModalDialog = getCurrentModalDialog();

	if (!_captureWidget || !_captureWidget->isEnabled())
	{
		_captureWidget = 0;

		if (newHoverWidget != _hoverWidget)
		{
			if (_hoverWidget)
			{
				_hoverWidget->setMouseHover(false);

				//if (_hoverWidget->isEnabled())
				{
					ATOM_WidgetMouseLeaveEvent mouseLeave(_hoverWidget->getId());
					_hoverWidget->handleEvent (&mouseLeave);
				}
			}

			if (newHoverWidget)
			{
				newHoverWidget->setMouseHover (true);
				//if (newHoverWidget->isEnabled ())
				{
					ATOM_WidgetMouseEnterEvent mouseEnter(newHoverWidget->getId());
					newHoverWidget->handleEvent (&mouseEnter);
				}
			}

			_hoverWidget = newHoverWidget;
		}

		if (currentModalDialog && !isPopupWidget(newHoverWidget) && !belongsTo (newHoverWidget, currentModalDialog))
		{
			return;
		}

		if (newHoverWidget && newHoverWidget->isEnabled())
		{
			ATOM_Point2Di p(x, y);
			newHoverWidget->screenToClient (&p);
			ATOM_WidgetMouseMoveEvent mouseMove(newHoverWidget->getId(), p.x, p.y, xrel, yrel, leftDown, middleDown, rightDown, 0);
			newHoverWidget->handleEvent (&mouseMove);
		}
	}
	else
	{
		if (newHoverWidget != _hoverWidget)
		{
			if (_hoverWidget == _captureWidget)
			{
				_hoverWidget->setMouseHover(false);
				ATOM_WidgetMouseLeaveEvent mouseLeave(_hoverWidget->getId());
				_hoverWidget->handleEvent (&mouseLeave);
				_hoverWidget = 0;
			}
			else if (newHoverWidget == _captureWidget)
			{
				newHoverWidget->setMouseHover (true);
				ATOM_WidgetMouseEnterEvent mouseEnter(newHoverWidget->getId());
				newHoverWidget->handleEvent (&mouseEnter);
				_hoverWidget = newHoverWidget;
			}
			else
			{
				_hoverWidget = 0;
			}
		}

		if (currentModalDialog && !isPopupWidget(_captureWidget) && !belongsTo (_captureWidget, currentModalDialog))
		{
			return;
		}

		ATOM_Point2Di p(x, y);
		_captureWidget->screenToClient (&p);
		ATOM_WidgetMouseMoveEvent mouseMove(_captureWidget->getId(), p.x, p.y, xrel, yrel, leftDown, middleDown, rightDown, 0);
		_captureWidget->handleEvent (&mouseMove);
	}
}

void ATOM_GUIRenderer::onMouseWheel (ATOM_MouseWheelEvent *event)
{
	int x = event->x;
	int y = event->y;

	if (!_captureWidget || !_captureWidget->isEnabled())
	{
		_captureWidget = 0;

		ATOM_GUIHitTestResult hitResult;
		ATOM_Point2Di position;
		ATOM_Point2Di clientPosition;

		ATOM_Widget *hit = hitTest (x, y, &hitResult, &position, &clientPosition);

		if (!hit || hitResult.hitPoint != HitClient)
		{
			return;
		}

		ATOM_TopWindow *currentModalDialog = getCurrentModalDialog();
		if (currentModalDialog && !isPopupWidget(hit) && !belongsTo (hit, currentModalDialog))
		{
			return;
		}

		if (hit->isEnabled ())
		{
			ATOM_WidgetMouseWheelEvent e(hit->getId(), clientPosition.x, clientPosition.y, event->delta, event->keymod);
			hit->handleEvent (&e);
		}
	}
	else
	{
		ATOM_Point2Di p(x, y);
		_captureWidget->screenToClient (&p);
		ATOM_WidgetMouseWheelEvent e(_captureWidget->getId(), p.x, p.y, event->delta, event->keymod);
		_captureWidget->handleEvent (&e);
	}
}

void ATOM_GUIRenderer::startDragging (ATOM_Widget *widget, int x, int y)
{
	if (widget)
	{
		_draggingWidget = widget;
		_draggingX = x;
		_draggingY = y;
		setCapture (widget);
	}
}

void ATOM_GUIRenderer::stopDragging (void)
{
	if (_draggingWidget)
	{
		_draggingWidget = 0;
		setCapture (0);
	}
}

ATOM_Widget *ATOM_GUIRenderer::getDraggingWidget (void) const
{
	return _draggingWidget;
}

void ATOM_GUIRenderer::onMouseButtonDown (ATOM_MouseButtonDownEvent *event)
{
	int x = event->x;
	int y = event->y;

	ATOM_TopWindow *currentModalDialog = getCurrentModalDialog();
	if (!_captureWidget || !_captureWidget->isEnabled())
	{
		_captureWidget = 0;

		ATOM_GUIHitTestResult hitResult;
		ATOM_Point2Di position;
		ATOM_Point2Di clientPosition;

		ATOM_Widget *hit = hitTest (x, y, &hitResult, &position, &clientPosition);
		if (!hit || !hit->isEnabled ())
		{
			return;
		}

		hit->setMouseHover (true);
		if (currentModalDialog && !isPopupWidget(hit) && !belongsTo (hit, currentModalDialog))
		{
			currentModalDialog->bringToFront ();
			setFocus (hit);
			return;
		}

		hit->bringToFront ();
		setFocus (hit);

		switch (event->button)
		{
		case BUTTON_LEFT:
			{
				_leftDown = true;
				if (hitResult.hitPoint != HitNone && hitResult.lug)
				{
					startDragging (hit, event->x, event->y);
				}

				if (hitResult.hitPoint == HitClient && hit->isEnabled ())
				{
					ATOM_WidgetLButtonDownEvent e(hit->getId(), clientPosition.x, clientPosition.y, event->keymod);
					hit->handleEvent (&e);

					if (hit->isDragEnabled () && !_dragSource)
					{
						_preDragWidget = hit;
						_preDragPosition = clientPosition;

						_dropTarget = 0;
					}
				}

				break;
			}
		case BUTTON_MIDDLE:
			{
				_middleDown = true;
				if (hitResult.hitPoint == HitClient && hit->isEnabled ())
				{
					ATOM_WidgetMButtonDownEvent e(hit->getId(), clientPosition.x, clientPosition.y, event->keymod);
					hit->handleEvent (&e);
				}
				break;
			}
		case BUTTON_RIGHT:
			{
				_rightDown = true;
				if (hitResult.hitPoint == HitClient && hit->isEnabled ())
				{
					ATOM_WidgetRButtonDownEvent e(hit->getId(), clientPosition.x, clientPosition.y, event->keymod);
					hit->handleEvent (&e);
				}
				break;
			}
		};
	}
	else
	{
		if (currentModalDialog && !isPopupWidget(_captureWidget) && !belongsTo (_captureWidget, currentModalDialog))
		{
			currentModalDialog->bringToFront ();
			setCapture (nullptr);
			setFocus (currentModalDialog);
			return;
		}

		ATOM_Point2Di p(x, y);
		_captureWidget->screenToClient (&p);

		switch (event->button)
		{
		case BUTTON_LEFT:
			{
				_leftDown = true;
				ATOM_WidgetLButtonDownEvent e(_captureWidget->getId(), p.x, p.y, event->keymod);
				_captureWidget->handleEvent (&e);
				break;
			}
		case BUTTON_MIDDLE:
			{
				_middleDown = true;
				ATOM_WidgetMButtonDownEvent e(_captureWidget->getId(), p.x, p.y, event->keymod);
				_captureWidget->handleEvent (&e);
				break;
			}
		case BUTTON_RIGHT:
			{
				_rightDown = true;
				ATOM_WidgetRButtonDownEvent e(_captureWidget->getId(), p.x, p.y, event->keymod);
				_captureWidget->handleEvent (&e);
				break;
			}
		};
	}
}

void ATOM_GUIRenderer::onMouseButtonUp (ATOM_MouseButtonUpEvent *event)
{
	int x = event->x;
	int y = event->y;

	if (event->button == BUTTON_LEFT)
	{
		stopDragging ();

		if (_dragSource)
		{
			if (_dragSource && _dropTarget && _dropTarget->isDropEnabled())
			{
				ATOM_Point2Di p(event->x, event->y);
				_dropTarget->screenToClient (&p);

				ATOM_Widget *dropTarget = _dropTarget;
				ATOM_AUTOPTR(ATOM_DragSource) dragSource = _dragSource;
				_dragSource = 0;
				_dropTarget = 0;
				setCapture (0);

				ATOM_WidgetDragDropEvent e(dropTarget->getId(), p.x, p.y, event->keymod, dragSource.get());
				dropTarget->handleEvent (&e);
			}
			else
			{
				_dragSource = 0;
				_dropTarget = 0;
			}
		}
		_preDragWidget = 0;
	}

	ATOM_TopWindow *currentModalDialog = getCurrentModalDialog();
	if (!_captureWidget || !_captureWidget->isEnabled())
	{
		_captureWidget = 0;

		ATOM_GUIHitTestResult hitResult;
		ATOM_Point2Di position;
		ATOM_Point2Di clientPosition;

		ATOM_Widget *hit = hitTest (x, y, &hitResult, &position, &clientPosition);
		if (!hit || !hit->isEnabled())
		{
			return;
		}

		hit->setMouseHover (true);
		if (currentModalDialog && !isPopupWidget(hit) && !belongsTo (hit, currentModalDialog))
		{
			currentModalDialog->bringToFront ();
			setFocus (hit);
			return;
		}

		switch (event->button)
		{
		case BUTTON_LEFT:
			{
				_leftDown = false;

				if (hitResult.hitPoint == HitClient && hit->isEnabled ())
				{
					ATOM_WidgetLButtonUpEvent e(hit->getId(), clientPosition.x, clientPosition.y, event->keymod);
					hit->handleEvent (&e);
				}
				break;
			}
		case BUTTON_MIDDLE:
			{
				_middleDown = false;
				if (hitResult.hitPoint == HitClient && hit->isEnabled ())
				{
					ATOM_WidgetMButtonUpEvent e(hit->getId(), clientPosition.x, clientPosition.y, event->keymod);
					hit->handleEvent (&e);
				}
				break;
			}
		case BUTTON_RIGHT:
			{
				_rightDown = false;
				if (hitResult.hitPoint == HitClient && hit->isEnabled ())
				{
					ATOM_WidgetRButtonUpEvent e(hit->getId(), clientPosition.x, clientPosition.y, event->keymod);
					hit->handleEvent (&e);
				}
				break;
			}
		}
	}
	else
	{
		if (currentModalDialog && !isPopupWidget(_captureWidget) && !belongsTo (_captureWidget, currentModalDialog))
		{
			currentModalDialog->bringToFront ();
			setFocus (_captureWidget);
			return;
		}

		ATOM_Point2Di p(x, y);
		_captureWidget->screenToClient (&p);

		switch (event->button)
		{
		case BUTTON_LEFT:
			{
				_leftDown = false;
				ATOM_WidgetLButtonUpEvent e(_captureWidget->getId(), p.x, p.y, event->keymod);
				_captureWidget->handleEvent (&e);
				break;
			}
		case BUTTON_MIDDLE:
			{
				_middleDown = false;
				ATOM_WidgetMButtonUpEvent e(_captureWidget->getId(), p.x, p.y, event->keymod);
				_captureWidget->handleEvent (&e);
				break;
			}
		case BUTTON_RIGHT:
			{
				_rightDown = false;
				ATOM_WidgetRButtonUpEvent e(_captureWidget->getId(), p.x, p.y, event->keymod);
				_captureWidget->handleEvent (&e);
				break;
			}
		};
	}
}

void ATOM_GUIRenderer::onMouseDblClick (ATOM_MouseDblClickEvent *event)
{
	int x = event->x;
	int y = event->y;

	ATOM_TopWindow *currentModalDialog = getCurrentModalDialog();
	if (!_captureWidget || !_captureWidget->isEnabled())
	{
		_captureWidget = 0;

		ATOM_GUIHitTestResult hitResult;
		ATOM_Point2Di position;
		ATOM_Point2Di clientPosition;

		ATOM_Widget *hit = hitTest (x, y, &hitResult, &position, &clientPosition);
		if (!hit || !hit->isEnabled ())
		{
			return;
		}

		if (currentModalDialog && !isPopupWidget(hit) && !belongsTo (hit, currentModalDialog))
		{
			currentModalDialog->bringToFront ();
			setFocus (hit);
			return;
		}

		hit->bringToFront ();
		setFocus (hit);

		switch (event->button)
		{
		case BUTTON_LEFT:
			{
				_leftDown = true;
				if (hitResult.hitPoint == HitClient && hit->isEnabled ())
				{
					ATOM_WidgetLButtonDblClickEvent e(hit->getId(), clientPosition.x, clientPosition.y, event->keymod);
					hit->handleEvent (&e);
				}
				break;
			}
		case BUTTON_MIDDLE:
			{
				_middleDown = true;
				if (hitResult.hitPoint == HitClient && hit->isEnabled ())
				{
					ATOM_WidgetMButtonDblClickEvent e(hit->getId(), clientPosition.x, clientPosition.y, event->keymod);
					hit->handleEvent (&e);
				}
				break;
			}
		case BUTTON_RIGHT:
			{
				_rightDown = true;
				if (hitResult.hitPoint == HitClient && hit->isEnabled ())
				{
					ATOM_WidgetRButtonDblClickEvent e(hit->getId(), clientPosition.x, clientPosition.y, event->keymod);
					hit->handleEvent (&e);
				}
				break;
			}
		};
	}
	else
	{
		if (currentModalDialog && !isPopupWidget(_captureWidget) && !belongsTo (_captureWidget, currentModalDialog))
		{
			currentModalDialog->bringToFront ();
			setFocus (_captureWidget);
			return;
		}

		ATOM_Point2Di p(x, y);
		_captureWidget->screenToClient (&p);

		switch (event->button)
		{
		case BUTTON_LEFT:
			{
				_leftDown = true;
				ATOM_WidgetLButtonDblClickEvent e(_captureWidget->getId(), p.x, p.y, event->keymod);
				_captureWidget->handleEvent (&e);
				break;
			}
		case BUTTON_MIDDLE:
			{
				_middleDown = true;
				ATOM_WidgetMButtonDblClickEvent e(_captureWidget->getId(), p.x, p.y, event->keymod);
				_captureWidget->handleEvent (&e);
				break;
			}
		case BUTTON_RIGHT:
			{
				_rightDown = true;
				ATOM_WidgetRButtonDblClickEvent e(_captureWidget->getId(), p.x, p.y, event->keymod);
				_captureWidget->handleEvent (&e);
				break;
			}
		};
	}
}

void ATOM_GUIRenderer::onChar(ATOM_CharEvent *event)
{
	if (_focusWidget)
	{
		ATOM_TopWindow *currentModalDialog = getCurrentModalDialog();
		if (currentModalDialog && !isPopupWidget(_focusWidget) && !belongsTo (_focusWidget, currentModalDialog))
		{
			currentModalDialog->bringToFront ();
			setFocus (_focusWidget);
			return;
		}

		ATOM_Widget *w = _focusWidget;
		while (w)
		{
			if (w->isEnabled())
			{
				ATOM_WidgetCharEvent e(w->getId(), event->mbcc, event->unicc, event->codec);
				w->handleEvent (&e);
				if (e.wasHandled() && e.eat)
				{
					return;
				}
			}
			if (!w->isControl())
			{
				return;
			}
			w = w->getParent ();
		}
	}
}

void ATOM_GUIRenderer::onKeyDown(ATOM_KeyDownEvent *event)
{
	if (_isMainGUI && event->key == KEY_BACKQUOTE && event->keymod == (KEYMOD_LCTRL|KEYMOD_LSHIFT))
	{
		showScriptDialog ("执行脚本", ATOM_Rect2Di(50, 50, 600, 400));
		return;
	}

	ATOM_TopWindow *currentModalDialog = getCurrentModalDialog();

	if (!_focusWidget)
	{
		_focusWidget = currentModalDialog;
	}

	if (_focusWidget)
	{
		if (currentModalDialog && !isPopupWidget(_focusWidget) && !belongsTo (_focusWidget, currentModalDialog))
		{
			_focusWidget = currentModalDialog;
		}

		ATOM_Widget *w = _focusWidget;
		while (w)
		{
			if (w->isEnabled())
			{
				ATOM_WidgetKeyDownEvent e(w->getId(), event->key, event->keymod);
				w->handleEvent (&e);
				if (e.wasHandled() && e.eat)
				{
					return;
				}
			}
			if (!w->isControl())
			{
				return;
			}
			w = w->getParent ();
		}
	}
}

void ATOM_GUIRenderer::onKeyUp(ATOM_KeyUpEvent *event)
{
	if (_focusWidget)
	{
		ATOM_TopWindow *currentModalDialog = getCurrentModalDialog();
		if (currentModalDialog && !isPopupWidget(_focusWidget) && !belongsTo (_focusWidget, currentModalDialog))
		{
			currentModalDialog->bringToFront ();
			setFocus (_focusWidget);
			return;
		}


		ATOM_Widget *w = _focusWidget;
		while (w)
		{
			if (w->isEnabled())
			{
				ATOM_WidgetKeyUpEvent e(w->getId(), event->key, event->keymod);
				w->handleEvent (&e);
				if (e.wasHandled() && e.eat)
				{
					return;
				}
			}
			if (!w->isControl())
			{
				return;
			}
			w = w->getParent ();
		}
	}
}

void ATOM_GUIRenderer::onIdle (ATOM_AppIdleEvent *event)
{
	if (isPopupWidget (_focusWidget))
	{
		ATOM_Widget *rel = _focusWidget->getPopupRelation ();
		bool vis = true;
		while (rel)
		{
			if (rel->getShowState() == ATOM_Widget::Hide)
			{
				vis = false;
				break;
			}
			rel = rel->getParent();
		}

		if (!vis)
		{
			_focusWidget->show (ATOM_Widget::Hide);
			setFocus (NULL);
		}
	}

	_cancelUpdate = false;
	widgetStateUpdateR (getDesktop());
	widgetFrameUpdateR (getDesktop(), event);

	if (_scriptDialog)
	{
		_scriptDialog->bringToFront ();
		ATOM_FlushStdOutputs ();
	}
}

ATOM_Widget *ATOM_GUIRenderer::hitTest (int x, int y, ATOM_GUIHitTestResult *result, ATOM_Point2Di *position, ATOM_Point2Di *clientPosition, bool fullTest) const
{
	ATOM_Widget *w = _desktop;

	if (w)
	{
		for (;;)
		{
			ATOM_GUIHitTestResult r;
			w->hitTest (x, y, &r, fullTest);
			result->hitPoint = r.hitPoint;
			result->child = r.child;
			result->lug = r.lug;

			if (result->hitPoint != HitChild)
			{
				if (position)
				{
					position->x = x;
					position->y = y;
				}

				if (clientPosition)
				{
					clientPosition->x = x - w->getClientRect().point.x;
					clientPosition->y = y - w->getClientRect().point.y;
				}

				break;
			}

			const ATOM_Rect2Di rc = result->child->getWidgetRect ();
			x -= rc.point.x;
			y -= rc.point.y;
			w = result->child;
		}

		return w;
	}
	else
	{
		result->hitPoint = HitNone;
		result->child = 0;
		result->lug = false;
		return 0;
	}
}

void ATOM_GUIRenderer::setFocus (ATOM_Widget *widget)
{
	//	ATOM_LOGGER::log (">>>0x%08X<<<\n", widget);
	if (widget && (widget->getStyle() & ATOM_Widget::NoFocus) != 0)
	{
		//setFocus (widget->getParent());
		return;
	}

	if (widget == 0 || (ATOM_Widget::isValidWidget (widget) && widget->getRenderer() == this))
	{
		if (widget != _focusWidget)
		{
			if (_focusWidget)
			{
				_focusWidget->invalidate ();
				_focusWidget->queueEvent (ATOM_NEW(ATOM_WidgetLostFocusEvent), ATOM_APP);
			}
			if (widget)
			{
				widget->invalidate ();
				widget->queueEvent (ATOM_NEW(ATOM_WidgetSetFocusEvent), ATOM_APP);
			}
			_focusWidget = widget;

#if ATOM3D_COMPILER_MSVC
			if (ATOM_RenderSettings::isAutoIMEChangeEnabled ())
			{
				if (_focusWidget && _focusWidget->enableIME ())
				{
					::ImmAssociateContextEx (ATOM_APP->getMainWindow(), NULL, IACE_DEFAULT);
				}
				else
				{
					::ImmAssociateContext (ATOM_APP->getMainWindow(), NULL);
				}
			}
#endif
		}
	}
}

ATOM_Widget *ATOM_GUIRenderer::getFocus (void) const
{
	return _focusWidget;
}

void ATOM_GUIRenderer::setCapture (ATOM_Widget *widget)
{
	if (_captureWidget != widget)
	{
		_captureWidget = widget;

		//if (!widget)
		//{
		//	ATOM_MouseMoveEvent e(0, _mouseX, _mouseY, 0, 0, 0, 0, 0);
		//	onMouseMove (&e);
		//}
	}
}

ATOM_Widget *ATOM_GUIRenderer::getCapture (void) const
{
	return _captureWidget;
}

ATOM_Widget *ATOM_GUIRenderer::getHover (void) const
{
	return _hoverWidget;
}

void ATOM_GUIRenderer::widgetStateUpdateR (ATOM_Widget *widget)
{
	if (widget->getShowState() != ATOM_Widget::Hide)
	{
		widget->updateMouseState (_mouseX, _mouseY, _leftDown, _middleDown, _rightDown);
		widget->updateLayout ();
	}

	for (ATOM_Widget *ctrl = widget->getFirstControl(); ctrl; ctrl = ctrl->getNextSibling ())
	{
		widgetStateUpdateR (ctrl);
	}

	for (ATOM_Widget *child = widget->getFirstChild(); child; child = child->getNextSibling ())
	{
		widgetStateUpdateR (child);
	}
}

void ATOM_GUIRenderer::widgetFrameUpdateR (ATOM_Widget *widget, ATOM_AppIdleEvent *event)
{
	if (widget->getShowState() != ATOM_Widget::Hide)
	{
		if (widget->handleIdleEvent ())
		{
			ATOM_WidgetIdleEvent e(widget->getId());
			widget->handleEvent (&e);

			// Check if widget had been destroyed by idle handler
			if (_cancelUpdate)
			{
				return;
			}
		}
	}

	for (ATOM_Widget *ctrl = widget->getFirstControl(); ctrl; ctrl = ctrl->getNextSibling ())
	{
		widgetFrameUpdateR (ctrl, event);

		// Check if ctrl had been destroyed by idle handler
		if (_cancelUpdate)
		{
			return;
		}
	}

	// Must check if widget had been destroyed by idle handler
	for (ATOM_Widget *child = widget->getFirstChild(); child; child = child->getNextSibling ())
	{
		widgetFrameUpdateR (child, event);

		// Must check if child had been destroyed by idle handler
		if (_cancelUpdate)
		{
			return;
		}
	}
}

ATOM_Point2Di ATOM_GUIRenderer::getMousePosition (void) const
{
	return ATOM_Point2Di(_mouseX, _mouseY);
}

bool ATOM_GUIRenderer::isLeftButtonDown (void) const
{
	return _leftDown;
}

bool ATOM_GUIRenderer::isMiddleButtonDown (void) const
{
	return _middleDown;
}

bool ATOM_GUIRenderer::isRightButtonDown (void) const
{
	return _rightDown;
}

const ATOM_Rect2Di *ATOM_GUIRenderer::getScissor (void) const
{
	return _scissorStack.empty () ? 0 : &_scissorStack.back ();
}

bool ATOM_GUIRenderer::pushScissor (const ATOM_Rect2Di &rect, bool keepLast)
{
	ATOM_RenderDevice *device = ATOM_GetRenderDevice();
	ATOM_ASSERT(device);

	if (_scissorStack.empty ())
	{
		device->enableScissorTest (device->getCurrentView(), true);
	}

	ATOM_Rect2Di scissor = (keepLast && !_scissorStack.empty ()) ? ATOM_Rect2Di::getIntersection(_scissorStack.back(), rect) : rect;
	if (scissor.size.w <= 0 || scissor.size.h <= 0)
	{
		return false;
	}

	device->setScissorRect (device->getCurrentView(), scissor.point.x, scissor.point.y, scissor.size.w, scissor.size.h);

	_scissorStack.push_back (scissor);

	return true;
}

void ATOM_GUIRenderer::popScissor (void)
{
	ATOM_ASSERT(!_scissorStack.empty ());
	_scissorStack.pop_back ();

	ATOM_RenderDevice *device = ATOM_GetRenderDevice();
	ATOM_ASSERT(device);
	if (_scissorStack.empty ())
	{
		device->enableScissorTest (device->getCurrentView(), false);
	}
	else
	{
		const ATOM_Rect2Di &scissorLast = _scissorStack.back ();
		device->setScissorRect (device->getCurrentView(), scissorLast.point.x, scissorLast.point.y, scissorLast.size.w, scissorLast.size.h);
	}
}

bool ATOM_GUIRenderer::belongsTo (ATOM_Widget *child, ATOM_Widget *parent) const
{
	ATOM_Widget *w = child;
	while (w)
	{
		if (w == parent)
		{
			return true;
		}
		w = w->getParent ();
	}
	return false;
}

void ATOM_GUIRenderer::setModalDialog (ATOM_TopWindow *dialog)
{
	if (ATOM_Widget::isValidWidget (dialog))
	{
		_modalDialogsStack.push_back (dialog);
		dialog->bringToFront ();
		setFocus (dialog);
	}
}

void ATOM_GUIRenderer::removeModalDialog (ATOM_TopWindow *dialog)
{
	for (unsigned i = 0; i < _modalDialogsStack.size(); ++i)
	{
		if (_modalDialogsStack[i] == dialog)
		{
			_modalDialogsStack.erase (_modalDialogsStack.begin() + i);
			return;
		}
	}
}

ATOM_TopWindow *ATOM_GUIRenderer::getCurrentModalDialog (void)
{
	while (!_modalDialogsStack.empty ())
	{
		ATOM_TopWindow *w = _modalDialogsStack.back ();
		if (ATOM_Widget::isValidWidget (w))
		{
			return w;
		}
		_modalDialogsStack.pop_back ();
	}
	return 0;
}

void ATOM_GUIRenderer::setRenderTargetWindow (ATOM_TopWindow *window)
{
	_renderTargetWindow = window;
}

ATOM_TopWindow *ATOM_GUIRenderer::getRenderTargetWindow (void) const
{
	return _renderTargetWindow;
}

void ATOM_GUIRenderer::set3DMode (bool b)
{
	_3dMode = b;
}

bool ATOM_GUIRenderer::is3DMode (void) const
{
	return _3dMode;
}

void ATOM_GUIRenderer::setZValue (float z)
{
	_zValue = z;
}

float ATOM_GUIRenderer::getZValue (void) const
{
	return _zValue;
}

void ATOM_GUIRenderer::ignoreFog (bool b)
{
	_nofog = b;
}

bool ATOM_GUIRenderer::isIgnoreFog (void) const
{
	return _nofog;
}

class ScriptDialogEventTrigger: public ATOM_EventTrigger
{
	ATOM_Script *_script;

public:
	ScriptDialogEventTrigger (ATOM_Script *script): _script(script)
	{
	}

public:
	void onCommand (ATOM_WidgetCommandEvent *event)
	{
		ATOM_TopWindow *window = (ATOM_TopWindow*)getHost();

		switch (event->id)
		{
		case MENUID_RUNSCRIPT:
			{
				ATOM_MultiEdit *edit = (ATOM_MultiEdit*)window->getChildById (EDITID_SCRIPT);
				if (edit)
				{
					ATOM_STRING str;
					edit->getString (str);
					_script->executeString (str.c_str());
				}
				break;
			}
		case MENUID_CLOSE:
			{
				window->getRenderer()->closeScriptDialog ();
				event->setCallerAlive(false);
				break;
			}
		case MENUID_CLEAROUTPUT:
			{
				ATOM_MultiEdit *edit = (ATOM_MultiEdit*)window->getChildById (EDITID_OUTPUT);
				if (edit)
				{
					edit->setString ("");
				}
				break;
			}
		case MENUID_CLEARSCRIPT:
			{
				ATOM_MultiEdit *edit = (ATOM_MultiEdit*)window->getChildById (EDITID_SCRIPT);
				if (edit)
				{
					edit->setString ("");
				}
				break;
			}
		default:
			break;
		}
	}

	void onKeyDown (ATOM_WidgetKeyDownEvent *event)
	{
		ATOM_Widget *w = (ATOM_Widget*)getHost();
		ATOM_MultiEdit *me = dynamic_cast<ATOM_MultiEdit*>(w);
		if (me && me == me->getParent()->getChildById (EDITID_SCRIPT) && (event->keymod & KEYMOD_CTRL) != 0 && event->key == KEY_RETURN)
		{
			ATOM_STRING str;
			me->getString (str);
			_script->executeString (str.c_str());
			setAutoCallHost (false);
		}
		else
		{
			ATOM_TopWindow *window = (ATOM_TopWindow*)getHost();

			if (event->key == KEY_r && event->keymod == KEYMOD_CTRL)
			{
				window->queueEvent (ATOM_NEW(ATOM_WidgetCommandEvent, MENUID_RUNSCRIPT), ATOM_APP);
			}
			else if (event->key == KEY_q && event->keymod == KEYMOD_CTRL)
			{
				window->queueEvent (ATOM_NEW(ATOM_WidgetCommandEvent, MENUID_CLOSE), ATOM_APP);
			}
		}
	}

private:
	ATOM_DECLARE_EVENT_MAP(ScriptDialogEventTrigger, ATOM_EventTrigger)
};

ATOM_BEGIN_EVENT_MAP(ScriptDialogEventTrigger, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(ScriptDialogEventTrigger, ATOM_WidgetCommandEvent, onCommand)
	ATOM_EVENT_HANDLER(ScriptDialogEventTrigger, ATOM_WidgetKeyDownEvent, onKeyDown)
	ATOM_END_EVENT_MAP

class ScriptStdoutCallback: public ATOM_StdOutputCallback
{
public:
	virtual ~ScriptStdoutCallback (void) {}

	virtual void OutputString (const char *str)
	{
		ATOM_LOGGER::log ("%s", str);
	}
};

class ScriptStderrCallback: public ATOM_StdOutputCallback
{
public:
	virtual ~ScriptStderrCallback (void) {}

	virtual void OutputString (const char *str)
	{
		ATOM_LOGGER::error ("%s", str);
	}
};

void ATOM_CALL LogFunc (void *userData, const char *msg, unsigned msgLen, const char *msgOrigin, ATOM_LOGGER::Level level)
{
	ATOM_MultiEdit *outputEdit = (ATOM_MultiEdit*)userData;

	static char buffer[8192];
	const char *prefix;
	switch (level)
	{
	case ATOM_LOGGER::LVL_INFORMATION:
		prefix = "INFO - ";
		break;
	case ATOM_LOGGER::LVL_WARNING:
		prefix = "WARNNING - ";
		break;
	case ATOM_LOGGER::LVL_ERROR:
		prefix = "ERROR - ";
		break;
	case ATOM_LOGGER::LVL_FATAL:
		prefix = "FATAL - ";
		break;
	case ATOM_LOGGER::LVL_DEBUG:
		prefix = "DEBUG - ";
		break;
	default:
		prefix = "";
	}
	_snprintf (buffer, 8192, "%s%s", prefix, msgOrigin);

	if (outputEdit->getNumLines() > 50)
	{
		outputEdit->setString ("");
	}

	outputEdit->moveCursorEnd ();
	outputEdit->insertText (buffer);
}

static ScriptStdoutCallback stdoutCallback;
static ScriptStderrCallback stderrCallback;

void ATOM_GUIRenderer::showScriptDialog (const char *title, const ATOM_Rect2Di &rect)
{
	closeScriptDialog ();

	ATOM_Desktop *desktop = getDesktop ();
	if (!desktop)
	{
		return;
	}

	_scriptDialogImageList = ATOM_NEW(ATOM_GUIImageList);
	_scriptDialogImageList->loadDefaults ();
	ATOM_ColorARGB c(0.f, 0.f, 0.f, 1.f);
	_scriptDialogImageList->newColorImage (ATOM_IMAGEID_USER+1, c, c, c, c);

	_scriptDialog = ATOM_NEW(ATOM_TopWindow, desktop, rect, ATOM_Widget::Border|ATOM_Widget::TitleBar|ATOM_Widget::CloseButton);
	_scriptDialog->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	_scriptDialog->setFlags (ATOM_Widget::TopMost);

	ATOM_MenuBar *menubar = ATOM_NEW(ATOM_MenuBar, this);
	menubar->appendMenuItem ("脚本", 0);
	ATOM_PopupMenu *submenu = menubar->createSubMenu (0);
	submenu->appendMenuItem ("运行脚本", MENUID_RUNSCRIPT);
	submenu->appendMenuItem ("清除输入", MENUID_CLEARSCRIPT);
	submenu->appendMenuItem ("清除输出", MENUID_CLEAROUTPUT);
	submenu->appendMenuItem ("关闭", MENUID_CLOSE);
	_scriptDialog->setMenuBar (menubar);

	ATOM_Rect2Di rcScriptEdit (ATOM_Point2Di(0, 0), _scriptDialog->getClientRect().size);
	rcScriptEdit.point.y += 5;
	rcScriptEdit.point.x += 5;
	rcScriptEdit.size.w -= 10;
	rcScriptEdit.size.h -= 10;
	rcScriptEdit.size.h /= 2;
	rcScriptEdit.size.h -= 3;
	ATOM_MultiEdit *scriptEdit = ATOM_NEW(ATOM_MultiEdit, _scriptDialog, rcScriptEdit, ATOM_Widget::Control|ATOM_Widget::Border, EDITID_SCRIPT, ATOM_Widget::ShowNormal);
	scriptEdit->setFont (ATOM_GUIFont::getDefaultFont (12, 0));
	scriptEdit->setImageList (_scriptDialogImageList.get());
	scriptEdit->setCursorImageId (ATOM_IMAGEID_USER+1);
	scriptEdit->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	scriptEdit->setCursorWidth (2);
	scriptEdit->setLineHeight (14);
	scriptEdit->setBorderMode (ATOM_Widget::Drop);
	setFocus (scriptEdit);

	ATOM_Rect2Di rcOutputEdit = _scriptDialog->getClientRect();
	rcOutputEdit.size.h = rcScriptEdit.size.h;
	rcOutputEdit.point.y = rcScriptEdit.point.y + rcScriptEdit.size.h + 6;
	rcOutputEdit.point.x = rcScriptEdit.point.x;
	rcOutputEdit.size.w = rcScriptEdit.size.w;
	ATOM_MultiEdit *outputEdit = ATOM_NEW(ATOM_MultiEdit, _scriptDialog, rcOutputEdit, ATOM_Widget::Control|ATOM_Widget::Border, EDITID_OUTPUT, ATOM_Widget::ShowNormal);
	outputEdit->setFont (ATOM_GUIFont::getDefaultFont (12, 0));
	outputEdit->setImageList (_scriptDialogImageList.get());
	outputEdit->setLineHeight (14);
	outputEdit->setBorderMode (ATOM_Widget::Drop);

	if (!_scriptManager)
	{
		_scriptManager = ATOM_NEW(ATOM_ScriptManager);
		_script = _scriptManager->createScript (&ScriptCallback);
		_script->setGlobalp ("MainGUIRenderer", this);
		_script->setErrorOutputCallback (&ScriptOutputFunc);
	}

	_scriptDialogTrigger = ATOM_NEW(ScriptDialogEventTrigger, _script);
	_scriptDialog->setEventTrigger (_scriptDialogTrigger);
	scriptEdit->setEventTrigger (_scriptDialogTrigger);

	ATOM_AddStandardOutputCallback (&stdoutCallback, &stderrCallback);

	ATOM_LOGGER::setLogMethods (ATOM_LOGGER::getLogMethods() | ATOM_LOGGER::CUSTOM);
	ATOM_LOGGER::setCustomLogFunction (&LogFunc, outputEdit);
}

void ATOM_GUIRenderer::closeScriptDialog (void)
{
	if (_scriptDialog)
	{
		ATOM_LOGGER::setLogMethods (ATOM_LOGGER::getLogMethods() & ~ATOM_LOGGER::CUSTOM);

		ATOM_DELETE(_scriptDialog);
		_scriptDialog = 0;

		_scriptDialogImageList = 0;

		ATOM_DELETE(_scriptDialogTrigger);
		_scriptDialogTrigger = 0;

		ATOM_RemoveStandardOutputCallback (&stdoutCallback, &stderrCallback);
	}
}

ATOM_Script *ATOM_GUIRenderer::getScript (void) const
{
	return _script;
}

void ATOM_GUIRenderer::notifyWidgetDeleted (ATOM_Widget *widget)
{
	if (widget == _desktop)
	{
		_desktop = 0;
	}

	if (widget == _hoverWidget)
	{
		_hoverWidget = 0;
	}

	if (widget == _draggingWidget)
	{
		_draggingWidget = 0;
	}

	if (widget == _focusWidget)
	{
		_focusWidget = 0;
	}

	if (widget == _captureWidget)
	{
		_captureWidget = 0;
	}

	if (widget == _dropTarget)
	{
		_dropTarget = 0;
	}

	if (widget == _preDragWidget)
	{
		_preDragWidget = 0;
	}

	if (_dragSource && _dragSource->getWidget() == widget)
	{
		cancelDrag ();
	}

	bool r = true;
	while (r)
	{
		r = false;
		for (unsigned i = 0; i < _modalDialogsStack.size(); ++i)
		{
			if (_modalDialogsStack[i] == widget)
			{
				_modalDialogsStack.erase (_modalDialogsStack.begin() + i);
				r = true;
				break;
			}
		}
	}

	_cancelUpdate = true;
}

void ATOM_GUIRenderer::setAsMainGUI (bool b)
{
	_isMainGUI = b;
}

bool ATOM_GUIRenderer::isModalLooping (void)
{
	return getCurrentModalDialog() != NULL;
}

void ATOM_GUIRenderer::doDrag (ATOM_DragSource *dragSource, ATOM_Widget *sourceWidget)
{
	if (dragSource && sourceWidget)
	{
		_dragSource = dragSource;
		_dragSource->setWidget (sourceWidget);
		_dragSource->setDraggingPosition (ATOM_Point2Di(_mouseX, _mouseY));
		setCapture (sourceWidget);
	}
}

void ATOM_GUIRenderer::cancelDrag (void)
{
	_dragSource = 0;
}

void ATOM_GUIRenderer::drawDragIndicator (void)
{
	if (_dragSource)
	{
		int imageId = _dragSource->getIndicatorImageId ();
		if (imageId != ATOM_INVALID_IMAGEID)
		{
			ATOM_GUIImage * image = _dragSource->getWidget()->getValidImage (imageId);
			if (image)
			{
				ATOM_Rect2Di rect = _dragSource->getIndicatorRect ();
				rect.point += _dragSource->getDraggingPosition ();

				ATOM_GUICanvas *canvas = getDesktop()->getWidgetCanvas();
				//ATOM_LOGGER::log ("Drawing drag indicator at rect: (%d, %d, %d, %d)\n", rect.point.x, rect.point.y, rect.size.w, rect.size.h);
				image->draw (WST_NORMAL, canvas, rect);
				getDesktop()->freeCanvas (canvas);
			}
		}
	}
}

void ATOM_GUIRenderer::drawDark (ATOM_ColorARGB color)
{
	if (pushScissor (ATOM_Rect2Di(_viewport.point.x, _viewport.point.y, _viewport.size.w, _viewport.size.h), false))
	{
		ATOM_GUICanvas *canvas = getDesktop()->getWidgetCanvas();
		canvas->fillRect (ATOM_Rect2Di(0, 0, _viewport.size.w, _viewport.size.h), color);
		getDesktop()->freeCanvas (canvas);

		popScissor ();
	}
}

void ATOM_GUIRenderer::enableMultiThreading (bool enable)
{
	_mtEnabled = enable;
}

bool ATOM_GUIRenderer::isMultiThreadingEnabled (void)
{
	return _mtEnabled;
}

