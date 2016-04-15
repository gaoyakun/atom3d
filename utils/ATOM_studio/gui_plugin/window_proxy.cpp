#include "StdAfx.h"
#include "window_proxy.h"
#include "widgetparameters.h"

WindowParameters::WindowParameters (ControlProxy *proxy): WidgetParameters(proxy)
{
	borderMode() = ATOM_Widget::Raise;
	style() |= (ATOM_Widget::Border|ATOM_Widget::TitleBar);
	style() &= ~ATOM_Widget::Control;
}

void WindowParameters::resetParameters (void)
{
	WidgetParameters::resetParameters ();

	borderMode() = ATOM_Widget::Raise;
	style() |= (ATOM_Widget::Border|ATOM_Widget::TitleBar);
	style() &= ~ATOM_Widget::Control;
	rect().size.w = 400;
	rect().size.h = 300;
}

WidgetParameters *WindowParameters::clone (void) const
{
	return ATOM_NEW(WindowParameters, *this);
}

WindowProxy::WindowProxy (PluginGUI *plugin)
	: ControlProxy (plugin)
{
}

ATOM_Widget *WindowProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_TopWindow *window = ATOM_NEW(ATOM_TopWindow, parent, parameters->rect(), parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (window);
	return window;
}

void WindowProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *WindowProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(WindowParameters, proxy);
}

