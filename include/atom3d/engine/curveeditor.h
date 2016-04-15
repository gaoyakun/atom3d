#ifndef __ATOM_ENGINE_CURVEEDITOR_H
#define __ATOM_ENGINE_CURVEEDITOR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"
#include "gui_font.h"

class ATOM_ENGINE_API ATOM_CurveEditor: public ATOM_Widget
{
public:
	ATOM_CurveEditor (void);
	ATOM_CurveEditor (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::ShowNormal);
	virtual ~ATOM_CurveEditor (void);

public:
	void setTimeAxisUnitLength (unsigned len);
	unsigned getTimeAxisUnitLength (void) const;
	void setCurveResolution (unsigned val);
	unsigned getCurveResolution (void) const;
	void setTimeAxisUnitResolution (float pixelsPerSecond);
	float getTimeAxisUnitResolution (void) const;
	void setValueAxisUnitLength (unsigned len);
	unsigned getValueAxisUnitLength (void) const;
	void setValueAxisUnitResolution (float val);
	float getValueAxisUnitResolution (void) const;
	void setOriginTimeAxis (float val);
	float getOriginTimeAxis (void) const;
	void setOriginValueAxis (float val);
	float getOriginValueAxis (void) const;
	const ATOM_FloatAnimationCurve &getCurve (void) const;
	int controlPointHitTest (int x, int y) const;
	void setCurveColor (ATOM_ColorARGB color);
	ATOM_ColorARGB getCurveColor (void) const;
	void setMaxValue (float val);
	float getMaxValue (void) const;
	void setMinValue (float val);
	float getMinValue (void) const;
	void setMaxTimeRange (long val);
	long getMaxTimeRange (void) const;
	void setMinTimeRange (long val);
	long getMinTimeRange (void) const;
	void enableScroll (bool enable);
	bool isScrollEnabled (void) const;

public:
	ATOM_FloatAnimationCurve &getCurve (void)
	{
		return _animationCurve;
	}

public:
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onMouseWheel (ATOM_WidgetMouseWheelEvent *event);
	void onLButtonDown (ATOM_WidgetLButtonDownEvent *event);
	void onLButtonUp (ATOM_WidgetLButtonUpEvent *event);
	void onMButtonDown (ATOM_WidgetMButtonDownEvent *event);
	void onMButtonUp (ATOM_WidgetMButtonUpEvent *event);
	void onRButtonUp (ATOM_WidgetRButtonUpEvent *event);
	void onMouseMove (ATOM_WidgetMouseMoveEvent *event);
	void onKeyDown (ATOM_WidgetKeyDownEvent *event);
	void onCommand (ATOM_WidgetCommandEvent *event);

private:
	int getSplitControlPoint (unsigned width, unsigned x0, unsigned x1) const;
	float coordToKeyValue (int coord) const;
	float coordToTimeInSecond (int coord) const;
	int keyValueToCoord (float value) const;
	int timeValueToCoord (float timeInSecond) const;
	void notifyChange (void);

public:
	ATOM_FloatAnimationCurve _animationCurve;
	unsigned _timeAxisUnitLength;
	unsigned _valueAxisUnitLength;
	unsigned _curveResolution;
	float _timeAxisUnitResolution;
	float _valueAxisUnitResolution;
	float _originTimeAxis;
	float _originValueAxis;
	int _selectedControlPoint;
	long _minTimeValue;
	long _maxTimeValue;
	long _minTimeRange;
	long _maxTimeRange;
	static const int DRAG_NONE = 0;
	static const int DRAG_POINT = 1;
	static const int DRAG_CLIENT = 2;
	int _dragTarget;
	ATOM_ColorARGB _curveColor;
	float _maxValue;
	float _minValue;
	bool _enableScroll;
	ATOM_VECTOR<ATOM_Point2Di> _curveLinePoints[2];
	ATOM_VECTOR<ATOM_ColorARGB> _curveLineColors;
	ATOM_MenuBar *_menuBar;

	ATOM_DECLARE_EVENT_MAP(ATOM_CurveEditor, ATOM_Widget)
};

#endif // __ATOM_ENGINE_CURVEEDITOR_H
