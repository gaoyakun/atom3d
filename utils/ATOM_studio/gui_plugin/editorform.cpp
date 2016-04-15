#include "StdAfx.h"
#include "editorform.h"
#include "proxy.h"
#include "plugin_gui.h"

ATOM_BEGIN_EVENT_MAP(EditorForm, ATOM_Widget)
	ATOM_EVENT_HANDLER(EditorForm, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	ATOM_EVENT_HANDLER(EditorForm, ATOM_WidgetLButtonUpEvent, onLButtonUp)
	ATOM_EVENT_HANDLER(EditorForm, ATOM_WidgetRButtonUpEvent, onRButtonUp)
	ATOM_EVENT_HANDLER(EditorForm, ATOM_WidgetMouseMoveEvent, onMouseMove)
	ATOM_EVENT_HANDLER(EditorForm, ATOM_WidgetKeyDownEvent, onKeyDown)
	ATOM_EVENT_HANDLER(EditorForm, ATOM_WidgetKeyUpEvent, onKeyUp)
	ATOM_EVENT_HANDLER(EditorForm, ATOM_WidgetResizeEvent, onResize)
	ATOM_EVENT_HANDLER(EditorForm, ATOM_WidgetIdleEvent, onIdle)
	ATOM_EVENT_HANDLER(EditorForm, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(EditorForm, ATOMX_TWCommandEvent, onTWCommandEvent)
	ATOM_EVENT_HANDLER(EditorForm, ATOMX_TWValueChangedEvent, onTWValueChangedEvent)
ATOM_END_EVENT_MAP

static const int blockoff = 3;
static const int blocksize = blockoff * 2;

EditorForm::EditorForm (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
	:ATOM_Widget (parent, rect, style, id, showState)
{
	_mainProxy = 0;
	_promptParentProxy = 0;
	_propBar = 0;
	_customPropBar = 0;
	_preview = false;
	_dragging = false;
	_enablePick = false;
	_promptCreate = false;
	_draggingCtlPoint = -1;
	_promptCreateWidgetType = 0;
	_promptMouseX = 0;
	_promptMouseY = 0;
	_movingVel.x = 0;
	_movingVel.y = 0;
	_plugin = 0;
}

EditorForm::~EditorForm (void)
{
}

ATOM_WidgetType EditorForm::getType (void) const
{
	return (ATOM_WidgetType)ATOM_MAKE_FOURCC('e','d','t','f');
}

void EditorForm::setWidgetPropertyBar (ATOMX_TweakBar *bar)
{
	_propBar = bar;
}

void EditorForm::setCustomPropertyBar (ATOMX_TweakBar *bar)
{
	_customPropBar = bar;
}

void EditorForm::draw (void)
{
	ATOM_Widget::draw ();

	if (!_preview)
	{
		drawController ();

		if (_promptCreate)
		{
			drawParentProxy ();
			drawIndicator ();
		}
	}
}

void EditorForm::setPlugin (PluginGUI *plugin)
{
	_plugin = plugin;
}

void EditorForm::beginCreateWidget (int widgetType, int x, int y)
{
	if (!_preview)
	{
		_promptCreate = true;
		_promptCreateWidgetType = widgetType;
		_promptMouseX = x;
		_promptMouseY = y;

		setActiveProxy (0);
		//setCapture ();
	}
}

void EditorForm::endCreateWidget (void)
{
	_promptCreate = false;
	//releaseCapture ();
}

void EditorForm::hitTest (int x, int y, ATOM_GUIHitTestResult *result, bool fullTest)
{
	// Don't hit children

	if (_preview || _enablePick)
	{
		ATOM_Widget::hitTest (x, y, result, fullTest);
	}
	else
	{
		if (x >= 0 && x < _widgetRect.size.w && y >= 0 && y < _widgetRect.size.h)
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
}

ATOM_Widget *EditorForm::getHoverWidget (void)
{
	ATOM_GUIHitTestResult hitTestResult;
	_enablePick = true;
	ATOM_Widget *widget = getRenderer()->hitTest (getRenderer()->getMousePosition().x, getRenderer()->getMousePosition().y, &hitTestResult, 0, 0, true);
	_enablePick = false;

	return widget;
}

void EditorForm::calcControllerPoints (void)
{
	ATOM_Widget *parentWidget = _mainProxy->getWidget()->getParent();
	ATOM_Rect2Di rc = _mainProxy->getWidget()->getWidgetRect();
	rc.point.x -= parentWidget->getClientRect().point.x;
	rc.point.y -= parentWidget->getClientRect().point.y;
	parentWidget->clientToGUI (&rc.point);
	GUIToClient (&rc.point);

	switch (_draggingCtlPoint)
	{
	case CTLPOINT_LEFT_TOP:
		{
			int expandLeft = ATOM_min2(_currentCtlPoint.x - _lastCtlPoint.x, rc.size.w);
			int expandTop = ATOM_min2(_currentCtlPoint.y - _lastCtlPoint.y, rc.size.h);
			rc.point.x += expandLeft;
			rc.point.y += expandTop;
			rc.size.w -= expandLeft;
			rc.size.h -= expandTop;
			break;
		}
	case CTLPOINT_MIDDLE_TOP:
		{
			int expand = ATOM_min2(_currentCtlPoint.y - _lastCtlPoint.y, rc.size.h);
			rc.point.y += expand;
			rc.size.h -= expand;
			break;
		}
	case CTLPOINT_RIGHT_TOP:
		{
			int expandRight = ATOM_min2(_lastCtlPoint.x - _currentCtlPoint.x, rc.size.w);
			int expandTop = ATOM_min2(_currentCtlPoint.y - _lastCtlPoint.y, rc.size.h);
			rc.point.y += expandTop;
			rc.size.w -= expandRight;
			rc.size.h -= expandTop;
			break;
		}
	case CTLPOINT_MIDDLE_LEFT:
		{
			int expand = ATOM_min2(_currentCtlPoint.x - _lastCtlPoint.x, rc.size.w);
			rc.point.x += expand;
			rc.size.w -= expand;
			break;
		}
	case CTLPOINT_MIDDLE_RIGHT:
		{
			int expand = ATOM_min2(_lastCtlPoint.x - _currentCtlPoint.x, rc.size.w);
			rc.size.w -= expand;
			break;
		}
	case CTLPOINT_LEFT_BOTTOM:
		{
			int expandLeft = ATOM_min2(_currentCtlPoint.x - _lastCtlPoint.x, rc.size.w);
			int expandTop = ATOM_min2(_lastCtlPoint.y - _currentCtlPoint.y, rc.size.h);
			rc.point.x += expandLeft;
			rc.size.w -= expandLeft;
			rc.size.h -= expandTop;
			break;
		}
	case CTLPOINT_MIDDLE_BOTTOM:
		{
			int expand = ATOM_min2(_lastCtlPoint.y - _currentCtlPoint.y, rc.size.h);
			rc.size.h -= expand;
			break;
		}
	case CTLPOINT_RIGHT_BOTTOM:
		{
			int expandLeft = ATOM_min2(_lastCtlPoint.x - _currentCtlPoint.x, rc.size.w);
			int expandTop = ATOM_min2(_lastCtlPoint.y - _currentCtlPoint.y, rc.size.h);
			rc.size.w -= expandLeft;
			rc.size.h -= expandTop;
			break;
		}
	}

	_ctlPoints[CTLPOINT_LEFT_TOP] = ATOM_Rect2Di(rc.point.x-blockoff, rc.point.y-blockoff, blocksize, blocksize);
	_ctlPoints[CTLPOINT_MIDDLE_LEFT] = ATOM_Rect2Di(rc.point.x-blockoff, rc.point.y+rc.size.h/2-blockoff, blocksize, blocksize);
	_ctlPoints[CTLPOINT_LEFT_BOTTOM] = ATOM_Rect2Di(rc.point.x-blockoff, rc.point.y+rc.size.h-blockoff, blocksize, blocksize);
	_ctlPoints[CTLPOINT_MIDDLE_BOTTOM] = ATOM_Rect2Di(rc.point.x+rc.size.w/2-blockoff, rc.point.y+rc.size.h-blockoff, blocksize, blocksize);
	_ctlPoints[CTLPOINT_RIGHT_BOTTOM] = ATOM_Rect2Di(rc.point.x+rc.size.w-blockoff, rc.point.y+rc.size.h-blockoff, blocksize, blocksize);
	_ctlPoints[CTLPOINT_MIDDLE_RIGHT] = ATOM_Rect2Di(rc.point.x+rc.size.w-blockoff, rc.point.y+rc.size.h/2-blockoff, blocksize, blocksize);
	_ctlPoints[CTLPOINT_RIGHT_TOP] = ATOM_Rect2Di(rc.point.x+rc.size.w-blockoff, rc.point.y-blockoff, blocksize, blocksize);
	_ctlPoints[CTLPOINT_MIDDLE_TOP] = ATOM_Rect2Di(rc.point.x+rc.size.w/2-blockoff, rc.point.y-blockoff, blocksize, blocksize);
}

void EditorForm::onLButtonDown (ATOM_WidgetLButtonDownEvent *event)
{
	setFocus ();

	if (!_preview)
	{
		if (_promptCreate)
		{
			ATOM_Widget *parentWidget = _promptParentProxy ? _promptParentProxy->getWidget() : this;

			ControlProxy *proxy = ControlProxy::createProxy (_plugin, parentWidget, _promptCreateWidgetType);
			if (proxy)
			{
				ATOM_Widget *w = proxy->getWidget();
				ATOM_Rect2Di rc = w->getLayoutRect();
				ATOM_Point2Di pt(event->x, event->y);
				clientToGUI (&pt);
				parentWidget->GUIToClient (&pt);

				proxy->resize (ATOM_Rect2Di(pt.x - w->getWidgetRect().size.w/2, pt.y - w->getWidgetRect().size.h/2, rc.size.w, rc.size.h));
				//w->moveTo (event->x - w->getWidgetRect().size.w/2, event->y - w->getWidgetRect().size.h/2);
				_plugin->updateWidgetTree (true);
			}
			setActiveProxy (proxy);

			_promptCreate = false;
			_promptParentProxy = 0;
			releaseCapture ();
		}
		else
		{
			// check for controller points
			if (_mainProxy)
			{
				calcControllerPoints ();

				for (int i = 0; i < 8; ++i)
				{
					if (_ctlPoints[i].isPointIn (event->x, event->y))
					{
						_draggingCtlPoint = i;
						_lastCtlPoint.x = event->x;
						_lastCtlPoint.y = event->y;
						_currentCtlPoint.x = event->x;
						_currentCtlPoint.y = event->y;
						setCapture ();
						break;
					}
				}
			}

			if (_draggingCtlPoint < 0)
			{
				ATOM_Widget *widget = getHoverWidget ();

				if (widget)
				{
					ControlProxy *proxy = 0;
					while (widget)
					{
						proxy = ControlProxy::getWidgetProxy (widget);
						if (proxy)
						{
							break;
						}
						widget = widget->getParent();
					}

					if (proxy)
					{
						setActiveProxy (proxy);
						_dragging = true;
						_lastDragMouseX = event->x;
						_lastDragMouseY = event->y;
						setCapture ();
						return;
					}
				}

				setActiveProxy (0);
			}
		}
	}
}

void EditorForm::onRButtonUp (ATOM_WidgetRButtonUpEvent *event)
{
	if (!_preview && _promptCreate)
	{
		_promptCreate = false;
		_promptParentProxy = 0;
		setActiveProxy (0);
		releaseCapture ();
	}
}

void EditorForm::onLButtonUp (ATOM_WidgetLButtonUpEvent *event)
{
	if (!_preview)
	{
		if (_dragging)
		{
			_dragging = false;
		}
		else if (_draggingCtlPoint >= 0)
		{
			calcControllerPoints ();
			int x = _ctlPoints[CTLPOINT_LEFT_TOP].point.x;
			int y = _ctlPoints[CTLPOINT_LEFT_TOP].point.y;
			int w = _ctlPoints[CTLPOINT_RIGHT_TOP].point.x - _ctlPoints[CTLPOINT_LEFT_TOP].point.x;
			int h = _ctlPoints[CTLPOINT_LEFT_BOTTOM].point.y - _ctlPoints[CTLPOINT_LEFT_TOP].point.y;

			_draggingCtlPoint = -1;

			calcControllerPoints ();
			int x2 = _ctlPoints[CTLPOINT_LEFT_TOP].point.x;
			int y2 = _ctlPoints[CTLPOINT_LEFT_TOP].point.y;
			int w2 = _ctlPoints[CTLPOINT_RIGHT_TOP].point.x - _ctlPoints[CTLPOINT_LEFT_TOP].point.x;
			int h2 = _ctlPoints[CTLPOINT_LEFT_BOTTOM].point.y - _ctlPoints[CTLPOINT_LEFT_TOP].point.y;

			if (_mainProxy)
			{
				ATOM_WidgetLayout::Unit *unit = _mainProxy->getWidget()->getLayoutUnit ();
				int new_x = unit->desired_x + x - x2;
				int new_y = unit->desired_y + y - y2;
				int new_w = unit->desired_w >= 0 ? unit->desired_w + w - w2 : unit->desired_w;
				int new_h = unit->desired_h >= 0 ? unit->desired_h + h - h2 : unit->desired_h;
				_mainProxy->resize (ATOM_Rect2Di(new_x, new_y, new_w, new_h));
			}
		}
		releaseCapture ();
	}
}

void EditorForm::deleteActiveProxy (void)
{
	if (!_dragging && !_promptCreate && _mainProxy)
	{
		ControlProxy::deleteProxy (_mainProxy);
		_promptParentProxy = 0;
		setActiveProxy (0);
		_plugin->updateWidgetTree (true);
	}
}

void EditorForm::cloneActiveProxy (void)
{
	if (!_dragging && !_promptCreate && _mainProxy)
	{
		ControlProxy *proxy = _mainProxy->clone (ControlProxy::getWidgetProxy(_mainProxy->getWidget()->getParent()), true);
		_promptParentProxy = 0;
		_plugin->updateWidgetTree (true);
		setActiveProxy (proxy);
	}
}

void EditorForm::onKeyDown (ATOM_WidgetKeyDownEvent *event)
{
	if (!_preview)
	{
		if (event->key == KEY_DOWN && !_dragging && !_promptCreate && _mainProxy)
		{
			if (event->keymod & KEYMOD_CTRL)
			{
				ATOM_WidgetLayout::Unit *unit = _mainProxy->getWidget()->getLayoutUnit ();
				int new_x = unit->desired_x;
				int new_y = unit->desired_y + 1;
				int new_w = unit->desired_w;
				int new_h = unit->desired_h;
				_mainProxy->resize (ATOM_Rect2Di(new_x, new_y, new_w, new_h));
			}
			else if (event->keymod & KEYMOD_ALT)
			{
				_mainProxy->getWidget()->moveLayoutUnitDown ();
			}
			else
			{
				_movingVel.y = 1;
			}
		}
		else if (event->key == KEY_UP && !_dragging && !_promptCreate && _mainProxy)
		{
			if (event->keymod & KEYMOD_CTRL)
			{
				ATOM_WidgetLayout::Unit *unit = _mainProxy->getWidget()->getLayoutUnit ();
				int new_x = unit->desired_x;
				int new_y = unit->desired_y - 1;
				int new_w = unit->desired_w;
				int new_h = unit->desired_h;
				_mainProxy->resize (ATOM_Rect2Di(new_x, new_y, new_w, new_h));
			}
			else if (event->keymod & KEYMOD_ALT)
			{
				_mainProxy->getWidget()->moveLayoutUnitUp ();
			}
			else
			{
				_movingVel.y = -1;
			}
		}
		else if (event->key == KEY_LEFT && !_dragging && !_promptCreate && _mainProxy)
		{
			if (event->keymod & KEYMOD_CTRL)
			{
				ATOM_WidgetLayout::Unit *unit = _mainProxy->getWidget()->getLayoutUnit ();
				int new_x = unit->desired_x - 1;
				int new_y = unit->desired_y;
				int new_w = unit->desired_w;
				int new_h = unit->desired_h;
				_mainProxy->resize (ATOM_Rect2Di(new_x, new_y, new_w, new_h));
			}
			else if (event->keymod & KEYMOD_ALT)
			{
				_mainProxy->getWidget()->moveLayoutUnitUp ();
			}
			else
			{
				_movingVel.x = -1;
			}
		}
		else if (event->key == KEY_RIGHT && !_dragging && !_promptCreate && _mainProxy)
		{
			if (event->keymod & KEYMOD_CTRL)
			{
				ATOM_WidgetLayout::Unit *unit = _mainProxy->getWidget()->getLayoutUnit ();
				int new_x = unit->desired_x + 1;
				int new_y = unit->desired_y;
				int new_w = unit->desired_w;
				int new_h = unit->desired_h;
				_mainProxy->resize (ATOM_Rect2Di(new_x, new_y, new_w, new_h));
			}
			else if (event->keymod & KEYMOD_ALT)
			{
				_mainProxy->getWidget()->moveLayoutUnitDown ();
			}
			else
			{
				_movingVel.x = 1;
			}
		}
	}
}

void EditorForm::onKeyUp (ATOM_WidgetKeyUpEvent *event)
{
	if (!_preview)
	{
		if (event->key == KEY_DOWN || event->key == KEY_UP)
		{
			_movingVel.y = 0;
		}
		else if (event->key == KEY_LEFT || event->key == KEY_RIGHT)
		{
			_movingVel.x = 0;
		}
	}
}

void EditorForm::onIdle (ATOM_WidgetIdleEvent *event)
{
	if (!_preview)
	{
		if (_movingVel.x != 0 || _movingVel.y != 0)
		{
			if (_mainProxy)
			{
				ATOM_WidgetLayout::Unit *unit = _mainProxy->getWidget()->getLayoutUnit ();
				int new_x = unit->desired_x + _movingVel.x;
				int new_y = unit->desired_y + _movingVel.y;
				int new_w = unit->desired_w;
				int new_h = unit->desired_h;
				_mainProxy->resize (ATOM_Rect2Di(new_x, new_y, new_w, new_h));
			}
		}
	}
}

void EditorForm::onMouseMove (ATOM_WidgetMouseMoveEvent *event)
{
	if (!_preview)
	{
		if (_promptCreate)
		{
			_promptMouseX = event->x;
			_promptMouseY = event->y;
			_promptParentProxy = 0;

			ATOM_Widget *widget = getHoverWidget ();
			if (widget)
			{
				ControlProxy *proxy = ControlProxy::getWidgetProxy (widget);
				if (proxy)
				{
					_promptParentProxy = proxy;
				}
			}
		}
		else if (_dragging)
		{
			ATOM_Rect2Di layoutRect = _mainProxy->getWidget()->getLayoutRect ();
			layoutRect.point.x += (event->x - _lastDragMouseX);
			layoutRect.point.y += (event->y - _lastDragMouseY);
			_mainProxy->resize (layoutRect);

			//_mainProxy->getWidget()->moveBy (event->x-_lastDragMouseX, event->y-_lastDragMouseY);
			_lastDragMouseX = event->x;
			_lastDragMouseY = event->y;
		}
		else if (_draggingCtlPoint >= 0)
		{
			_currentCtlPoint.x = event->x;
			_currentCtlPoint.y = event->y;
		}
	}
}

void EditorForm::setActiveProxy (ControlProxy *proxy)
{
	if (proxy != _mainProxy)
	{
		_mainProxy = proxy;

		if (_mainProxy)
		{
			_mainProxy->setupTweakBar (_propBar, _customPropBar);
		}
		else
		{
			_propBar->clear ();
			_customPropBar->clear ();
		}
		_plugin->updateImageList ();
		_plugin->updateWidgetTree (false);
	}
}

ControlProxy *EditorForm::getActiveProxy (void) const
{
	return _mainProxy;
}

void EditorForm::drawController (void)
{
	if (_mainProxy)
	{
		calcControllerPoints ();

		ATOM_GUICanvas canvas(this);
		canvas.resize (getClientRect().size);
		ATOM_Point2Di pt(getClientRect().point);
		pt.x = 0;
		pt.y = 0;
		clientToViewport (&pt);
		canvas.setOrigin (pt);

		for (int i = 0; i < 8; ++i)
		{
			canvas.fillRect (_ctlPoints[i], ATOM_ColorARGB(1.f, 0.f, 1.f, 1.f), 0.f);
		}
	}
}

void EditorForm::drawIndicator (void)
{
	ATOM_Rect2Di rc(_promptMouseX-10, _promptMouseY-10, 20, 20);

	ATOM_GUICanvas canvas(this);
	canvas.resize (getClientRect().size);
	ATOM_Point2Di pt(getClientRect().point);
	pt.x = 0;
	pt.y = 0;
	clientToViewport (&pt);
	canvas.setOrigin (pt);

	canvas.fillRect (rc, ATOM_ColorARGB(1.f, 1.f, 1.f, 1.f), 0.f);
}

void EditorForm::drawParentProxy (void)
{
	if (_promptParentProxy)
	{
		ATOM_Widget *parentWidget = _promptParentProxy->getWidget()->getParent();
		ATOM_Rect2Di rc = _promptParentProxy->getWidget()->getWidgetRect();
		rc.point.x -= parentWidget->getClientRect().point.x;
		rc.point.y -= parentWidget->getClientRect().point.y;
		parentWidget->clientToGUI (&rc.point);
		GUIToClient (&rc.point);

		ATOM_GUICanvas canvas(this);
		canvas.resize (getClientRect().size);
		ATOM_Point2Di pt(getClientRect().point);
		pt.x = 0;
		pt.y = 0;
		clientToViewport (&pt);
		canvas.setOrigin (pt);
		canvas.fillRect (rc, ATOM_ColorARGB(1.f, 0.f, 0.f, 0.3f), 0.f);
	}
}

void EditorForm::onResize (ATOM_WidgetResizeEvent *event)
{
	callParentHandler (event);
}

void EditorForm::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	callParentHandler (event);
}

void EditorForm::onTWCommandEvent (ATOMX_TWCommandEvent *event)
{
	if (_mainProxy)
	{
		_mainProxy->handleTWCommandEvent (event);
	}
}

void EditorForm::onTWValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	if (_mainProxy)
	{
		_mainProxy->handleTWValueChangedEvent (event);
	}
}

bool EditorForm::isPreviewMode (void) const
{
	return _preview;
}

void EditorForm::setPreviewMode (bool preview)
{
	if (preview)
	{
		_dragging = false;
		_promptCreate = false;
		_promptParentProxy = 0;
		releaseCapture ();
	}
	_preview = preview;
}

