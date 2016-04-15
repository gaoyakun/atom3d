#include "StdAfx.h"
#include "rt_proxy.h"

RealtimeCtrlParameters::RealtimeCtrlParameters (ControlProxy *proxy)
	: WidgetParameters (proxy)
{
	rect().size.w = 400;
	rect().size.h = 300;
	imageId() = -1;

	_transparent = 0;
}

void RealtimeCtrlParameters::resetParameters (void)
{
	WidgetParameters::resetParameters ();

	imageId() = -1;
	rect().size.w = 400;
	rect().size.h = 300;
	_transparent = 0;
}

WidgetParameters *RealtimeCtrlParameters::clone (void) const
{
	return ATOM_NEW(RealtimeCtrlParameters, *this);
}

void RealtimeCtrlParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	WidgetParameters::setupTweakBar (propBar);

	propBar->addBoolVar ("±³¾°Í¸Ã÷", PARAMID_REALTIMECTRL_TRANSPARENT, (_transparent != 0), false, "3DäÖÈ¾¿Ø¼þ", 0);
}

void RealtimeCtrlParameters::apply (ATOM_Widget *widget)
{
	WidgetParameters::apply (widget);

	ATOM_RealtimeCtrl *rt = (ATOM_RealtimeCtrl*)widget;
	rt->transparent (_transparent != 0);
}

void RealtimeCtrlParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("transparent", _transparent);
}

void RealtimeCtrlParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		_transparent = 0;

		xml->QueryIntAttribute ("transparent", &_transparent);
	}
}

bool RealtimeCtrlParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_REALTIMECTRL_TRANSPARENT:
		_transparent = event->newValue.getB() ? 1 : 0;
		return true;
	default:
		return WidgetParameters::handleBarValueChangedEvent (event);
	}
}

bool RealtimeCtrlParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return WidgetParameters::handleBarCommandEvent (event);
}

///////////////////////////////////////////////////////////////////////////////////////////

RealtimeCtrlProxy::RealtimeCtrlProxy (PluginGUI *plugin)
	: ControlProxy(plugin)
{
}

ATOM_Widget *RealtimeCtrlProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_RealtimeCtrl *rt = ATOM_NEW(ATOM_RealtimeCtrl, parent, parameters->rect(), parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (rt);
	return rt;
}

void RealtimeCtrlProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *RealtimeCtrlProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(RealtimeCtrlParameters, proxy);
}

