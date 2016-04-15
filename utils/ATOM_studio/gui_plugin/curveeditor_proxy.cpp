#include "StdAfx.h"
#include "curveeditor_proxy.h"
#include "plugin_gui.h"

void CurveEditorParameters::resetParameters (void)
{
	WidgetParameters::resetParameters ();

	_timeAxisUnitLength = 20;
	_timeAxisUnitResolution = 100.f;
	_valueAxisUnitLength = 20;
	_valueAxisUnitResolution = 100.f;
	_curveResolution = 4;
	_curveColor = 0xFFFFFFFF;
	_originTimeAxis = 0.2f;
	_originValueAxis = 0.5f;
	_enableScroll = true;
}

WidgetParameters *CurveEditorParameters::clone (void) const
{
	return ATOM_NEW(CurveEditorParameters, *this);
}

void CurveEditorParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	WidgetParameters::setupTweakBar (propBar);

	propBar->addUIntVar ("时间轴单位长度", PARAMID_CURVEEDITOR_TIME_AXIS_UNIT_LENGTH, _timeAxisUnitLength, false, "曲线编辑器", 0);
	propBar->addFloatVar ("时间轴精细度", PARAMID_CURVEEDITOR_TIME_AXIS_UNIT_RESOLUTION, _timeAxisUnitResolution, false, "曲线编辑器", 0);
	propBar->addUIntVar ("数值轴单位长度", PARAMID_CURVEEDITOR_VALUE_AXIS_UNIT_LENGTH, _valueAxisUnitLength, false, "曲线编辑器", 0);
	propBar->addFloatVar ("数值轴精细度", PARAMID_CURVEEDITOR_VALUE_AXIS_UNIT_RESOLUTION, _valueAxisUnitResolution, false, "曲线编辑器", 0);
	propBar->addUIntVar ("曲线精细度", PARAMID_CURVEEDITOR_CURVE_RESOLUTION, _curveResolution, false, "曲线编辑器", 0);
	propBar->addRGBVar ("曲线颜色", PARAMID_CURVEEDITOR_CURVE_COLOR, _curveColor.getFloatR(), _curveColor.getFloatG(), _curveColor.getFloatB(), false, "曲线编辑器", 0);
	propBar->addFloatVar ("时间轴原点", PARAMID_CURVEEDITOR_ORIGIN_TIME_AXIS, _originTimeAxis, false, "曲线编辑器", 0);
	propBar->addFloatVar ("数值轴原点", PARAMID_CURVEEDITOR_ORIGIN_VALUE_AXIS, _originValueAxis, false, "曲线编辑器", 0);
	propBar->addBoolVar ("允许滚动", PARAMID_CURVEEDITOR_ENABLE_SCROLL, _enableScroll, false, "曲线编辑器", 0);
}

void CurveEditorParameters::apply (ATOM_Widget *widget)
{
	WidgetParameters::apply (widget);

	ATOM_CurveEditor *curveEditor = (ATOM_CurveEditor*)widget;

	curveEditor->setTimeAxisUnitLength(_timeAxisUnitLength);
	curveEditor->setTimeAxisUnitResolution(_timeAxisUnitResolution);
	curveEditor->setValueAxisUnitLength(_valueAxisUnitLength);
	curveEditor->setValueAxisUnitResolution(_valueAxisUnitResolution);
	curveEditor->setCurveColor (_curveColor);
	curveEditor->setCurveResolution(_curveResolution);
	curveEditor->setOriginTimeAxis(_originTimeAxis);
	curveEditor->setOriginValueAxis(_originValueAxis);
	curveEditor->enableScroll(_enableScroll);
}

void CurveEditorParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("TimeAxisUnitLen", (int)_timeAxisUnitLength);
	xml->SetDoubleAttribute ("TimeAxisUnitRes", _timeAxisUnitResolution);
	xml->SetAttribute ("ValueAxisUnitLen", (int)_valueAxisUnitLength);
	xml->SetDoubleAttribute ("ValueAxisUnitRes", _valueAxisUnitResolution);
	xml->SetAttribute ("CurveRes", (int)_curveResolution);
	xml->SetDoubleAttribute ("OriginTimeAxis", _originTimeAxis);
	xml->SetDoubleAttribute ("OriginValueAxis", _originValueAxis);
	xml->SetAttribute ("EnableScroll", _enableScroll?1:0);
	xml->SetAttribute ("CurveColor", _curveColor.getRaw());
}

void CurveEditorParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("TimeAxisUnitLen", (int*)&_timeAxisUnitLength);
		xml->QueryFloatAttribute ("TimeAxisUnitRes", &_timeAxisUnitResolution);
		xml->QueryIntAttribute ("ValueAxisUnitLen", (int*)&_valueAxisUnitLength);
		xml->QueryFloatAttribute ("ValueAxisUnitRes", &_valueAxisUnitResolution);
		xml->QueryIntAttribute ("CurveRes", (int*)&_curveResolution);
		xml->QueryFloatAttribute ("OriginTimeAxis", &_originTimeAxis);
		xml->QueryFloatAttribute ("OriginValueAxis", &_originValueAxis);
		int b = _enableScroll?1:0;
		xml->QueryIntAttribute ("EnableScroll", &b);
		_enableScroll = (b != 0);
		int c = 0xFFFFFFFF;
		xml->QueryIntAttribute ("CurveColor", &c);
		_curveColor = c;
	}
}

bool CurveEditorParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_CURVEEDITOR_TIME_AXIS_UNIT_LENGTH:
		_timeAxisUnitLength = event->newValue.getI();
		return true;
	case PARAMID_CURVEEDITOR_TIME_AXIS_UNIT_RESOLUTION:
		_timeAxisUnitResolution = event->newValue.getF();
		return true;
	case PARAMID_CURVEEDITOR_VALUE_AXIS_UNIT_LENGTH:
		_valueAxisUnitLength = event->newValue.getI();
		return true;
	case PARAMID_CURVEEDITOR_VALUE_AXIS_UNIT_RESOLUTION:
		_valueAxisUnitResolution = event->newValue.getF();
		return true;
	case PARAMID_CURVEEDITOR_CURVE_RESOLUTION:
		_curveResolution = event->newValue.getI();
		return true;
	case PARAMID_CURVEEDITOR_CURVE_COLOR:
		{
			const float *rgb = event->newValue.get3F();
			_curveColor.setFloats (rgb[0], rgb[1], rgb[2], 1.f);
			return true;
		}
	case PARAMID_CURVEEDITOR_ORIGIN_TIME_AXIS:
		_originTimeAxis = event->newValue.getF();
		return true;
	case PARAMID_CURVEEDITOR_ORIGIN_VALUE_AXIS:
		_originValueAxis = event->newValue.getF();
		return true;
	case PARAMID_CURVEEDITOR_ENABLE_SCROLL:
		_enableScroll = event->newValue.getB();
		return true;
	default:
		return WidgetParameters::handleBarValueChangedEvent (event);
	}
}

bool CurveEditorParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return WidgetParameters::handleBarCommandEvent (event);
}

CurveEditorProxy::CurveEditorProxy (PluginGUI *plugin)
	: ControlProxy (plugin)
{
}

ATOM_Widget *CurveEditorProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_CurveEditor *curveEditor = ATOM_NEW(ATOM_CurveEditor, parent, parameters->rect(), parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (curveEditor);
	return curveEditor;
}

void CurveEditorProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *CurveEditorProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(CurveEditorParameters, proxy);
}

