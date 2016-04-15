#include "StdAfx.h"
#include "scrollmap_proxy.h"
#include "widgetparameters.h"

ScrollMapParameters::ScrollMapParameters (ControlProxy *proxy)
	:WidgetParameters(proxy)
{
}

WidgetParameters *ScrollMapParameters::clone (void) const
{
	return ATOM_NEW(ScrollMapParameters, *this);
}

ScrollMapProxy::ScrollMapProxy (PluginGUI *plugin)
	:ControlProxy(plugin)
{
}

ATOM_Widget *ScrollMapProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_ScrollMap *scrollmap = ATOM_NEW(ATOM_ScrollMap, parent, parameters->rect(), parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (scrollmap);
	return scrollmap;
}

void ScrollMapProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *ScrollMapProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(ScrollMapParameters, proxy);
}

