#include "StdAfx.h"
#include "curveeditor.h"
#include "gui_canvas.h"

#define MENUID_ATOM_WAVE_TYPE_SPLINE		1
#define MENUID_ATOM_WAVE_TYPE_LINEAR		2
#define MENUID_ATOM_WAVE_TYPE_STEP		3
#define MENUID_ATOM_CURVE_ADDRESS_WRAP			4
#define MENUID_ATOM_CURVE_ADDRESS_CLAMP		5
#define MENUID_ATOM_CURVE_ADDRESS_CONTINUE		6
#define MENUID_EDIT_CLEAR			7

ATOM_BEGIN_EVENT_MAP(ATOM_CurveEditor, ATOM_Widget)
	ATOM_EVENT_HANDLER(ATOM_CurveEditor, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(ATOM_CurveEditor, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	ATOM_EVENT_HANDLER(ATOM_CurveEditor, ATOM_WidgetLButtonUpEvent, onLButtonUp)
	ATOM_EVENT_HANDLER(ATOM_CurveEditor, ATOM_WidgetMButtonDownEvent, onMButtonDown)
	ATOM_EVENT_HANDLER(ATOM_CurveEditor, ATOM_WidgetMButtonUpEvent, onMButtonUp)
	ATOM_EVENT_HANDLER(ATOM_CurveEditor, ATOM_WidgetRButtonUpEvent, onRButtonUp)
	ATOM_EVENT_HANDLER(ATOM_CurveEditor, ATOM_WidgetKeyDownEvent, onKeyDown)
	ATOM_EVENT_HANDLER(ATOM_CurveEditor, ATOM_WidgetMouseMoveEvent, onMouseMove)
	ATOM_EVENT_HANDLER(ATOM_CurveEditor, ATOM_WidgetMouseWheelEvent, onMouseWheel)
	ATOM_EVENT_HANDLER(ATOM_CurveEditor, ATOM_WidgetCommandEvent, onCommand)
ATOM_END_EVENT_MAP

ATOM_CurveEditor::ATOM_CurveEditor (void)
{
	_timeAxisUnitLength = 20;
	_valueAxisUnitLength = 20;
	_curveResolution = 4;
	_timeAxisUnitResolution = 20.f;
	_valueAxisUnitResolution = 20.f;
	_originTimeAxis = 0.2f;
	_originValueAxis = 0.5f;
	_selectedControlPoint = -1;
	_dragTarget = DRAG_NONE;
	_curveColor.setBytes (0, 0, 0, 255);
	_minValue = -ATOM_MaxFloatValue;
	_maxValue = ATOM_MaxFloatValue;
	_minTimeRange = ATOM_MinLongValue;
	_maxTimeRange = ATOM_MaxLongValue;
	_enableScroll = true;

	setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);

	_menuBar = ATOM_NEW (ATOM_MenuBar, getRenderer());
	_menuBar->appendMenuItem ("插值方式", 0);
	_menuBar->appendMenuItem ("寻址方式", 0);
	_menuBar->appendMenuItem ("编辑", 0);

	ATOM_PopupMenu *waveTypeMenu = _menuBar->createSubMenu (0);
	waveTypeMenu->appendMenuItem ("曲线", MENUID_ATOM_WAVE_TYPE_SPLINE);
	waveTypeMenu->appendMenuItem ("线性", MENUID_ATOM_WAVE_TYPE_LINEAR);
	waveTypeMenu->appendMenuItem ("阶梯", MENUID_ATOM_WAVE_TYPE_STEP);

	ATOM_PopupMenu *addressMenu = _menuBar->createSubMenu (1);
	addressMenu->appendMenuItem ("循环", MENUID_ATOM_CURVE_ADDRESS_WRAP);
	addressMenu->appendMenuItem ("切断", MENUID_ATOM_CURVE_ADDRESS_CLAMP);
	addressMenu->appendMenuItem ("延伸", MENUID_ATOM_CURVE_ADDRESS_CONTINUE);

	ATOM_PopupMenu *editMenu = _menuBar->createSubMenu (2);
	editMenu->appendMenuItem ("清除", MENUID_EDIT_CLEAR);

	setMenuBar (_menuBar);
}

ATOM_CurveEditor::ATOM_CurveEditor (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
: ATOM_Widget (parent, rect, style, id, showState)
{
	_timeAxisUnitLength = 20;
	_valueAxisUnitLength = 20;
	_curveResolution = 8;
	_timeAxisUnitResolution = 20.f;
	_valueAxisUnitResolution = 20.f;
	_originTimeAxis = 0.2f;
	_originValueAxis = 0.5f;
	_selectedControlPoint = -1;
	_dragTarget = DRAG_NONE;
	_curveColor.setBytes (0, 0, 0, 255);
	_minValue = -ATOM_MaxFloatValue;
	_maxValue = ATOM_MaxFloatValue;
	_minTimeRange = ATOM_MinLongValue;
	_maxTimeRange = ATOM_MaxLongValue;
	_enableScroll = true;

	setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);

	_menuBar = ATOM_NEW (ATOM_MenuBar, getRenderer());
	_menuBar->appendMenuItem ("插值方式", 0);
	_menuBar->appendMenuItem ("寻址方式", 0);
	_menuBar->appendMenuItem ("编辑", 0);

	ATOM_PopupMenu *waveTypeMenu = _menuBar->createSubMenu (0);
	waveTypeMenu->appendMenuItem ("曲线", MENUID_ATOM_WAVE_TYPE_SPLINE);
	waveTypeMenu->appendMenuItem ("线性", MENUID_ATOM_WAVE_TYPE_LINEAR);
	waveTypeMenu->appendMenuItem ("阶梯", MENUID_ATOM_WAVE_TYPE_STEP);

	ATOM_PopupMenu *addressMenu = _menuBar->createSubMenu (1);
	addressMenu->appendMenuItem ("循环", MENUID_ATOM_CURVE_ADDRESS_WRAP);
	addressMenu->appendMenuItem ("切断", MENUID_ATOM_CURVE_ADDRESS_CLAMP);
	addressMenu->appendMenuItem ("延伸", MENUID_ATOM_CURVE_ADDRESS_CONTINUE);

	ATOM_PopupMenu *editMenu = _menuBar->createSubMenu (2);
	editMenu->appendMenuItem ("清除", MENUID_EDIT_CLEAR);

	setMenuBar (_menuBar);
}

ATOM_CurveEditor::~ATOM_CurveEditor (void)
{
	ATOM_DELETE(_menuBar);
}

/*
ATOM_FloatAnimationCurve &ATOM_CurveEditor::getCurve (void)
{
	return _animationCurve;
}
*/

const ATOM_FloatAnimationCurve &ATOM_CurveEditor::getCurve (void) const
{
	return _animationCurve;
}

void ATOM_CurveEditor::setTimeAxisUnitLength (unsigned len)
{
	_timeAxisUnitLength = len;
}

unsigned ATOM_CurveEditor::getTimeAxisUnitLength (void) const
{
	return _timeAxisUnitLength;
}

void ATOM_CurveEditor::setCurveResolution (unsigned val)
{
	_curveResolution = val;
}

unsigned ATOM_CurveEditor::getCurveResolution (void) const
{
	return _curveResolution;
}

void ATOM_CurveEditor::setTimeAxisUnitResolution (float pixelsPerSecond)
{
	_timeAxisUnitResolution = pixelsPerSecond;
}

float ATOM_CurveEditor::getTimeAxisUnitResolution (void) const
{
	return _timeAxisUnitResolution;
}

void ATOM_CurveEditor::setValueAxisUnitLength (unsigned len)
{
	_valueAxisUnitLength = len;
}

unsigned ATOM_CurveEditor::getValueAxisUnitLength (void) const
{
	return _valueAxisUnitLength;
}

void ATOM_CurveEditor::setValueAxisUnitResolution (float val)
{
	_valueAxisUnitResolution = val;
}

float ATOM_CurveEditor::getValueAxisUnitResolution (void) const
{
	return _valueAxisUnitResolution;
}

int ATOM_CurveEditor::getSplitControlPoint (unsigned width, unsigned x0, unsigned x1) const
{
	for (unsigned i = 0; i < _animationCurve.getNumKeys (); ++i)
	{
		int pos = timeValueToCoord (_animationCurve[i].x * 0.001f);
		if (pos > x0 && pos < x1)
		{
			return i;
		}
	}
	return -1;
}

float ATOM_CurveEditor::coordToKeyValue (int coord) const
{
	int coordInCanvas = coord + getScrollValue().y;
	unsigned canvasSize = (getClientRect().size.h > getCanvasSize().h) ? getClientRect().size.h : getCanvasSize().h;
	float origin = canvasSize * _originValueAxis;
	return (origin - coordInCanvas) / _valueAxisUnitResolution;
}

float ATOM_CurveEditor::coordToTimeInSecond (int coord) const
{
	int coordInCanvas = coord + getScrollValue().x;
	unsigned canvasSize = (getClientRect().size.w > getCanvasSize().w) ? getClientRect().size.w : getCanvasSize().w;
	float origin = canvasSize * _originTimeAxis;
	return (coordInCanvas - origin) / _timeAxisUnitResolution;
}

int ATOM_CurveEditor::keyValueToCoord (float value) const
{
	unsigned canvasSize = (getClientRect().size.h > getCanvasSize().h) ? getClientRect().size.h : getCanvasSize().h;
	return canvasSize * _originValueAxis - value * _valueAxisUnitResolution - getScrollValue().y;
}

int ATOM_CurveEditor::timeValueToCoord (float timeInSecond) const
{
	unsigned canvasSize = (getClientRect().size.w > getCanvasSize().w) ? getClientRect().size.w : getCanvasSize().w;
	return timeInSecond * _timeAxisUnitResolution + canvasSize * _originTimeAxis - getScrollValue().x;
}

void ATOM_CurveEditor::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	// draw background
	int clientImageId = getClientImageId ();
#if 1
	ATOM_GUIImage *clientImage = getValidImage (clientImageId);
#else
	const ATOM_GUIImageList *imagelist = getValidImageList ();
	ATOM_GUIImage *clientImage = imagelist->getImage (clientImageId);
	if (!clientImage)
	{
		clientImage = imagelist->getImage (ATOM_IMAGEID_WINDOW_BKGROUND);
	}
#endif
	clientImage->draw (event->state, event->canvas, ATOM_Rect2Di(ATOM_Point2Di(0,0), event->canvas->getSize()));

	// draw axises
	int originTimeAxis = event->canvas->getSize().w * _originTimeAxis;
	int originValueAxis = event->canvas->getSize().h * _originValueAxis;

	event->canvas->drawLine (ATOM_Point2Di(0, originValueAxis), ATOM_Point2Di(event->canvas->getSize().w, originValueAxis), ATOM_ColorARGB(1.f, 0.f, 0.f, 1.f));
	float resolution = _valueAxisUnitResolution;
	int upPos = originValueAxis - resolution;
	while (upPos > 0)
	{
		if (upPos < event->canvas->getSize().h)
		{
			event->canvas->drawLine (ATOM_Point2Di(0, upPos), ATOM_Point2Di(event->canvas->getSize().w, upPos), ATOM_ColorARGB(0.5f, 0.5f, 0.5f, 1.f));
		}
		upPos -= resolution;
	}
	int downPos = originValueAxis + resolution;
	while (downPos < event->canvas->getSize().h)
	{
		if (downPos > 0)
		{
			event->canvas->drawLine (ATOM_Point2Di(0, downPos), ATOM_Point2Di(event->canvas->getSize().w, downPos), ATOM_ColorARGB(0.5f, 0.5f, 0.5f, 1.f));
		}
		downPos += resolution;
	}

	event->canvas->drawLine (ATOM_Point2Di(originTimeAxis, 0), ATOM_Point2Di(originTimeAxis, event->canvas->getSize().h), ATOM_ColorARGB(1.f, 0.f, 0.f, 1.f));
	resolution = _timeAxisUnitResolution;
	int leftPos = originTimeAxis - resolution;
	while (leftPos > 0)
	{
		if (leftPos < event->canvas->getSize().w)
		{
			event->canvas->drawLine (ATOM_Point2Di(leftPos, 0), ATOM_Point2Di(leftPos, event->canvas->getSize().h), ATOM_ColorARGB(0.5f, 0.5f, 0.5f, 1.f));
		}
		leftPos -= resolution;
	}
	int rightPos = originTimeAxis + resolution;
	while (rightPos < event->canvas->getSize().w)
	{
		if (rightPos > 0)
		{
			event->canvas->drawLine (ATOM_Point2Di(rightPos, 0), ATOM_Point2Di(rightPos, event->canvas->getSize().h), ATOM_ColorARGB(0.5f, 0.5f, 0.5f, 1.f));
		}
		rightPos += resolution;
	}

	if (_animationCurve.getNumKeys () == 0)
	{
		return;
	}

	// draw curve
	_curveLinePoints[0].resize (0);
	_curveLinePoints[1].resize (0);
	_curveLineColors.resize (0);

	unsigned currentPos = 0;
	while (currentPos < event->canvas->getSize().w)
	{
		unsigned nextPos = currentPos + _curveResolution;
		int splitControlPoint = getSplitControlPoint (event->canvas->getSize().w, currentPos, nextPos);
		if (splitControlPoint >= 0)
		{
			nextPos = timeValueToCoord (_animationCurve[splitControlPoint].x * 0.001f);
		}
		if (nextPos > event->canvas->getSize().w)
		{
			nextPos = event->canvas->getSize().w;
		}

#if 1
		long timeInMs0 = coordToTimeInSecond (currentPos) * 1000;
		long timeInMs1 = coordToTimeInSecond (nextPos) * 1000;
#else
		float timeInSecond0 = ((float)currentPos - (float)originTimeAxis)/(float)_timeAxisUnitResolution;
		long timeInMs0 = timeInSecond0 * 1000;
		float timeInSecond1 = ((float)nextPos - (float)originTimeAxis)/(float)_timeAxisUnitResolution;
		long timeInMs1 = timeInSecond1 * 1000;
#endif

		float val1 = _animationCurve.eval (timeInMs0);
		if (val1 < _minValue)
		{
			val1 = _minValue;
		}
		if (val1 > _maxValue)
		{
			val1 = _maxValue;
		}

		float val2 = _animationCurve.eval (timeInMs1);
		if (val2 < _minValue)
		{
			val2 = _minValue;
		}
		if (val2 > _maxValue)
		{
			val2 = _maxValue;
		}

#if 1
		long pos1 = keyValueToCoord (val1);
		long pos2 = keyValueToCoord (val2);
#else
		long pos1 = (long)originValueAxis - (long)(val1 * _valueAxisUnitResolution);
		long pos2 = (long)originValueAxis - (long)(val2 * _valueAxisUnitResolution);
#endif
		_curveLinePoints[0].push_back (ATOM_Point2Di(currentPos, pos1));
		_curveLinePoints[1].push_back (ATOM_Point2Di(nextPos, pos2));
		_curveLineColors.push_back (_curveColor);

		//event->canvas->drawLine (ATOM_Point2Di(currentPos, pos1), ATOM_Point2Di(nextPos, pos2), ATOM_ColorARGB(0.f, 0.f, 0.f, 1.f));

		currentPos = nextPos;
	}
	if (!_curveLinePoints[0].empty ())
	{
		event->canvas->drawLineList (_curveLinePoints[0].size(), &_curveLinePoints[0][0], &_curveLinePoints[1][0], &_curveLineColors[0]);
	}

	// draw control points
	for (unsigned i = 0; i < _animationCurve.getNumKeys (); ++i)
	{
		long timeInMs = _animationCurve[i].x;
		float val = _animationCurve[i].value();
		long x = originTimeAxis + _timeAxisUnitResolution * timeInMs * 0.001f;
		long y = (long)originValueAxis - (long)(val * _valueAxisUnitResolution);
		event->canvas->fillRect (ATOM_Rect2Di(x - 5, y - 5, 10, 10), (i == _selectedControlPoint) ? ATOM_ColorARGB(1.f, 1.f, 0.f, 1.f) : ATOM_ColorARGB(0.f, 0.f, 0.f, 1.f));
	}
}

int ATOM_CurveEditor::controlPointHitTest (int x, int y) const
{
	for (unsigned i = 0; i < _animationCurve.getNumKeys(); ++i)
	{
		long timeInMs = _animationCurve[i].x;
		float val = _animationCurve[i].value();
		int coordX = timeValueToCoord (timeInMs * 0.001f);
		int coordY = keyValueToCoord (val);
		int deltaX = x - coordX;
		int deltaY = y - coordY;

		if (deltaX >= -5 && deltaX <= 5 && deltaY >= -5 && deltaY <= 5)
		{
			return i;
		}
	}
	return -1;
}

void ATOM_CurveEditor::onMouseWheel (ATOM_WidgetMouseWheelEvent *event)
{
	if ((event->shiftState == KEYMOD_LSHIFT) || (event->shiftState == KEYMOD_RSHIFT))
	{
		_valueAxisUnitResolution += event->delta * 10;

		if (_valueAxisUnitResolution < 10)
		{
			_valueAxisUnitResolution = 10;
		}
	}
	else if (event->shiftState == 0)
	{
		_timeAxisUnitResolution += event->delta * 10;

		if (_timeAxisUnitResolution < 10)
		{
			_timeAxisUnitResolution = 10;
		}
	}
}

void ATOM_CurveEditor::onLButtonDown (ATOM_WidgetLButtonDownEvent *event)
{
	_selectedControlPoint = controlPointHitTest (event->x, event->y);

	if (_selectedControlPoint < 0)
	{
		long timeInMs = coordToTimeInSecond (event->x) * 1000;
		float value = coordToKeyValue (event->y);
		_selectedControlPoint = _animationCurve.insertKey (timeInMs, value);
	}

	if (_selectedControlPoint > 0)
	{
		_minTimeValue = _animationCurve[_selectedControlPoint-1].x;
	}
	else
	{
		_minTimeValue = _minTimeRange;;
	}

	if (_selectedControlPoint < _animationCurve.getNumKeys() - 1)
	{
		_maxTimeValue = _animationCurve[_selectedControlPoint+1].x;
	}
	else
	{
		_maxTimeValue = _maxTimeRange;
	}

	_dragTarget = DRAG_POINT;

	setCapture ();
}

void ATOM_CurveEditor::onLButtonUp (ATOM_WidgetLButtonUpEvent *event)
{
	if (_dragTarget == DRAG_POINT)
	{
		notifyChange ();
	}

	_dragTarget = DRAG_NONE;

	releaseCapture ();
}

void ATOM_CurveEditor::onMButtonDown (ATOM_WidgetMButtonDownEvent *event)
{
	if (_enableScroll)
	{
		_dragTarget = DRAG_CLIENT;
		setCapture ();
	}
}

void ATOM_CurveEditor::onMButtonUp (ATOM_WidgetMButtonUpEvent *event)
{
	if (_enableScroll)
	{
		_dragTarget = DRAG_NONE;
		releaseCapture ();
	}
}

void ATOM_CurveEditor::onRButtonUp (ATOM_WidgetRButtonUpEvent *event)
{
	int controlPoint = controlPointHitTest (event->x, event->y);
	if (controlPoint >= 0)
	{
		_animationCurve.removeKey (controlPoint);
		_selectedControlPoint = -1;
		_dragTarget = DRAG_NONE;

		notifyChange ();
	}
}

void ATOM_CurveEditor::onKeyDown (ATOM_WidgetKeyDownEvent *event)
{
	if (event->key == KEY_DELETE && _selectedControlPoint >= 0)
	{
		_animationCurve.removeKey (_selectedControlPoint);
		_selectedControlPoint = -1;
		_dragTarget = DRAG_NONE;

		notifyChange();
	}
}

void ATOM_CurveEditor::onMouseMove (ATOM_WidgetMouseMoveEvent *event)
{
	if (_dragTarget == DRAG_CLIENT)
	{
		unsigned canvasSizeX = (getClientRect().size.w > getCanvasSize().w) ? getClientRect().size.w : getCanvasSize().w;
		unsigned canvasSizeY = (getClientRect().size.h > getCanvasSize().h) ? getClientRect().size.h : getCanvasSize().h;
		int xrel = event->xrel;
		int yrel = event->yrel;
		_originTimeAxis += float(event->xrel)/float(canvasSizeX);
		_originValueAxis += float(event->yrel)/float(canvasSizeY);
	}
	else if (_dragTarget == DRAG_POINT)
	{
		long time = coordToTimeInSecond (event->x) * 1000;
		if (time <= _minTimeValue)
		{
			if (_animationCurve.findKey(_minTimeValue) != unsigned(-1))
			{
				return;
			}
			time = _minTimeValue;
		}
		else if (time >= _maxTimeValue)
		{
			if (_animationCurve.findKey(_maxTimeValue) != unsigned(-1))
			{
				return;
			}
			time = _maxTimeValue;
		}

		_animationCurve.removeKey (_selectedControlPoint);
		float value = coordToKeyValue (event->y);
		if (value < _minValue)
		{
			value = _minValue;
		}
		else if (value > _maxValue)
		{
			value = _maxValue;
		}
		_selectedControlPoint = _animationCurve.insertKey (time , value);
	}
}

void ATOM_CurveEditor::onCommand (ATOM_WidgetCommandEvent *event)
{
	switch (event->id)
	{
	case MENUID_ATOM_WAVE_TYPE_SPLINE:
		if (_animationCurve.getWaveType() != ATOM_WAVE_TYPE_SPLINE)
		{
			_animationCurve.setWaveType (ATOM_WAVE_TYPE_SPLINE);
			notifyChange ();
		}
		break;
	case MENUID_ATOM_WAVE_TYPE_LINEAR:
		if (_animationCurve.getWaveType() != ATOM_WAVE_TYPE_LINEAR)
		{
			_animationCurve.setWaveType (ATOM_WAVE_TYPE_LINEAR);
			notifyChange ();
		}
		break;
	case MENUID_ATOM_WAVE_TYPE_STEP:
		if (_animationCurve.getWaveType() != ATOM_WAVE_TYPE_STEP)
		{
			_animationCurve.setWaveType (ATOM_WAVE_TYPE_STEP);
			notifyChange ();
		}
		break;
	case MENUID_ATOM_CURVE_ADDRESS_WRAP:
		if (_animationCurve.getAddress () != ATOM_CURVE_ADDRESS_WRAP)
		{
			_animationCurve.setAddress (ATOM_CURVE_ADDRESS_WRAP);
			notifyChange ();
		}
		break;
	case MENUID_ATOM_CURVE_ADDRESS_CLAMP:
		if (_animationCurve.getAddress () != ATOM_CURVE_ADDRESS_CLAMP)
		{
			_animationCurve.setAddress (ATOM_CURVE_ADDRESS_CLAMP);
			notifyChange ();
		}
		break;
	case MENUID_ATOM_CURVE_ADDRESS_CONTINUE:
		if (_animationCurve.getAddress () != ATOM_CURVE_ADDRESS_CONTINUE)
		{
			_animationCurve.setAddress (ATOM_CURVE_ADDRESS_CONTINUE);
			notifyChange ();
		}
		break;
	case MENUID_EDIT_CLEAR:
		if (_animationCurve.getNumKeys() > 0)
		{
			_animationCurve.removeAllKeys ();
			_selectedControlPoint = -1;
			_dragTarget = DRAG_NONE;
			notifyChange ();
		}
		break;
	}
}

void ATOM_CurveEditor::setOriginTimeAxis (float val)
{
	this->_originTimeAxis = val;
}

float ATOM_CurveEditor::getOriginTimeAxis (void) const
{
	return this->_originTimeAxis;
}

void ATOM_CurveEditor::setOriginValueAxis (float val)
{
	this->_originValueAxis = val;
}

float ATOM_CurveEditor::getOriginValueAxis (void) const
{
	return this->_originValueAxis;
}

void ATOM_CurveEditor::setCurveColor (ATOM_ColorARGB color)
{
	_curveColor = color;
}

ATOM_ColorARGB ATOM_CurveEditor::getCurveColor (void) const
{
	return _curveColor;
}

void ATOM_CurveEditor::setMaxValue (float val)
{
	_maxValue = val;
}

float ATOM_CurveEditor::getMaxValue (void) const
{
	return _maxValue;
}

void ATOM_CurveEditor::setMinValue (float val)
{
	_minValue = val;
}

float ATOM_CurveEditor::getMinValue (void) const
{
	return _minValue;
}

void ATOM_CurveEditor::setMaxTimeRange (long val)
{
	_maxTimeRange = val;
}

long ATOM_CurveEditor::getMaxTimeRange (void) const
{
	return _maxTimeRange;
}

void ATOM_CurveEditor::setMinTimeRange (long val)
{
	_minTimeRange = val;
}

long ATOM_CurveEditor::getMinTimeRange (void) const
{
	return _minTimeRange;
}

void ATOM_CurveEditor::enableScroll (bool enable)
{
	_enableScroll = enable;
}

bool ATOM_CurveEditor::isScrollEnabled (void) const
{
	return _enableScroll;
}

void ATOM_CurveEditor::notifyChange (void)
{
	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->queueEvent (ATOM_NEW(ATOM_CurveChangedEvent, getId()), ATOM_APP);
	}
}

