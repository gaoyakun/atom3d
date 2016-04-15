#include "StdAfx.h"
#include "viewstack_proxy.h"

void ViewStackParameters::resetParameters (void)
{
	PanelParameters::resetParameters ();

	_activePage = -1;
}

WidgetParameters *ViewStackParameters::clone (void) const
{
	return ATOM_NEW(ViewStackParameters, *this);
}

void ViewStackParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	PanelParameters::setupTweakBar (propBar);

	propBar->addIntVar ("当前页面", PARAMID_VIEWSTACK_ACTIVEPAGE, _activePage, false, "页面集合", 0);
}

void ViewStackParameters::apply (ATOM_Widget *widget)
{
	PanelParameters::apply (widget);

	ATOM_ViewStack *viewstack = (ATOM_ViewStack*)widget;
	viewstack->setActivePage (_activePage);
}

void ViewStackParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	PanelParameters::saveToXML (xml);

	xml->SetAttribute ("activepage", _activePage);
}

void ViewStackParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	PanelParameters::loadFromXML (xml);

	if (xml)
	{
		int i = -1;
		xml->QueryIntAttribute ("activepage", &i);
		_activePage = i;
	}
}

bool ViewStackParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_VIEWSTACK_ACTIVEPAGE:
		_activePage = event->newValue.getI();
		return true;
	default:
		return PanelParameters::handleBarValueChangedEvent (event);
	}
}

bool ViewStackParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return PanelParameters::handleBarCommandEvent (event);
}

ViewStackProxy::ViewStackProxy (PluginGUI *plugin)
	: PanelProxy (plugin)
{
}

ATOM_Widget *ViewStackProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_ViewStack *vs = ATOM_NEW(ATOM_ViewStack, parent, parameters->rect(), parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (vs);
	vs->enableEventTransition (false);
	return vs;
}

void ViewStackProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *ViewStackProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(ViewStackParameters, proxy);
}

