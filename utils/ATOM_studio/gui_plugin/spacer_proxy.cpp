#include "StdAfx.h"
#include "spacer_proxy.h"
#include "widgetparameters.h"
#include "idmanager.h"

bool SpacerParameters::interactive (void)
{
	return false;
}

void SpacerParameters::resetParameters (void)
{
	WidgetParameters::resetParameters ();

	rect().size.w = 200;
	rect().size.h = 160;
}

WidgetParameters *SpacerParameters::clone (void) const
{
	return ATOM_NEW(SpacerParameters, *this);
}

void SpacerParameters::apply (ATOM_Widget *widget)
{
	WidgetParameters::apply (widget);

	style() = style()&~ATOM_Widget::Border;
	style() = style()&~ATOM_Widget::TitleBar;
	style() = style()&~ATOM_Widget::CloseButton;
	widget->setStyle (style());
	widget->setClientImageId (-1);
}

void SpacerParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	style() = style()&~ATOM_Widget::Border;
	style() = style()&~ATOM_Widget::TitleBar;
	style() = style()&~ATOM_Widget::CloseButton;

	imageId() = -1;
	WidgetParameters::saveToXML (xml);
}

SpacerProxy::SpacerProxy (PluginGUI *plugin)
	: ControlProxy (plugin)
{
}

ATOM_Widget *SpacerProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_Panel *panel = ATOM_NEW(ATOM_Panel, parent, parameters->rect(), parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	panel->enableEventTransition (false);
	parameters->apply (panel);
	return panel;
}

void SpacerProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *SpacerProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(SpacerParameters, proxy);
}

