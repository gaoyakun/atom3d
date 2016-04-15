#include "StdAfx.h"
#include "panel_proxy.h"
#include "widgetparameters.h"

void PanelParameters::resetParameters (void)
{
	WidgetParameters::resetParameters ();

	borderMode() = ATOM_Widget::Raise;
	style() = style()|ATOM_Widget::Border;
	rect().size.w = 200;
	rect().size.h = 160;

	_eventTransition = false;
}

WidgetParameters *PanelParameters::clone (void) const
{
	return ATOM_NEW(PanelParameters, *this);
}

void PanelParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	WidgetParameters::setupTweakBar (propBar);

	propBar->addBoolVar ("ÊÂ¼þ´©Í¸", PARAMID_PANEL_EVENTTRANSITION, _eventTransition, false, "Ãæ°å", 0);
}

void PanelParameters::apply (ATOM_Widget *widget)
{
	WidgetParameters::apply (widget);

	ATOM_Panel *panel = (ATOM_Panel*)widget;
	//panel->enableEventTransition (_eventTransition);
}

void PanelParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("eventtransition", _eventTransition ? 1 : 0);
}

void PanelParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		int i = 0;
		xml->QueryIntAttribute ("eventtransition", &i);
		_eventTransition = i != 0;
	}
}

bool PanelParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_PANEL_EVENTTRANSITION:
		_eventTransition = event->newValue.getB();
		return true;
	default:
		return WidgetParameters::handleBarValueChangedEvent (event);
	}
}

bool PanelParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return WidgetParameters::handleBarCommandEvent (event);
}

PanelProxy::PanelProxy (PluginGUI *plugin)
	: ControlProxy (plugin)
{
}

ATOM_Widget *PanelProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_Panel *panel = ATOM_NEW(ATOM_Panel, parent, parameters->rect(), parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (panel);
	panel->enableEventTransition (false);
	return panel;
}

void PanelProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *PanelProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(PanelParameters, proxy);
}

