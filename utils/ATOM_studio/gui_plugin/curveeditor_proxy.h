#ifndef __ATOM3D_STUDIO_GUIEDITOR_CURVEEDITOR_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_CURVEEDITOR_PROXY_H

#include "proxy.h"
#include "widgetparameters.h"

class CurveEditorParameters: public WidgetParameters
{
public:
	enum
	{
		PARAMID_CURVEEDITOR_TIME_AXIS_UNIT_LENGTH = WidgetParameters::PARAMID_LAST,
		PARAMID_CURVEEDITOR_TIME_AXIS_UNIT_RESOLUTION,
		PARAMID_CURVEEDITOR_VALUE_AXIS_UNIT_LENGTH,
		PARAMID_CURVEEDITOR_VALUE_AXIS_UNIT_RESOLUTION,
		PARAMID_CURVEEDITOR_CURVE_RESOLUTION,
		PARAMID_CURVEEDITOR_ORIGIN_TIME_AXIS,
		PARAMID_CURVEEDITOR_ORIGIN_VALUE_AXIS,
		PARAMID_CURVEEDITOR_MAX_VALUE,
		PARAMID_CURVEEDITOR_MIN_VALUE,
		PARAMID_CURVEEDITOR_MAX_TIME_RANGE,
		PARAMID_CURVEEDITOR_MIN_TIME_RANGE,
		PARAMID_CURVEEDITOR_CURVE_COLOR,
		PARAMID_CURVEEDITOR_ENABLE_SCROLL,
		PARAMID_CURVEEDITOR_LAST
	};

public:
	CurveEditorParameters (ControlProxy *proxy): WidgetParameters(proxy)
	{
	}

	virtual void resetParameters (void);
	virtual WidgetParameters *clone (void) const;
	virtual void setupTweakBar (ATOMX_TweakBar *propBar);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual bool handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event);
	virtual bool handleBarCommandEvent (ATOMX_TWCommandEvent *event);

private:
	unsigned _timeAxisUnitLength;
	float _timeAxisUnitResolution;
	unsigned _valueAxisUnitLength;
	float _valueAxisUnitResolution;
	unsigned _curveResolution;
	ATOM_ColorARGB _curveColor;
	float _originTimeAxis;
	float _originValueAxis;
	bool _enableScroll;
};

class CurveEditorProxy: public ControlProxy
{
public:
	CurveEditorProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(CurveEditorProxy, curveeditor, ATOM_CurveEditor)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_CURVEEDITOR_PROXY_H
