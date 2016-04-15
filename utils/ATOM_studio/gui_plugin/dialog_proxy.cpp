#include "StdAfx.h"
#include "dialog_proxy.h"
#include "widgetparameters.h"

DialogParameters::DialogParameters (ControlProxy *proxy): WidgetParameters(proxy)
{
	borderMode() = ATOM_Widget::Raise;
	rect().size.w = 400;
	rect().size.h = 300;
	style() |= (ATOM_Widget::Border|ATOM_Widget::TitleBar);
	style() &= ~ATOM_Widget::Control;
}

void DialogParameters::resetParameters (void)
{
	WidgetParameters::resetParameters ();

	borderMode() = ATOM_Widget::Raise;
	rect().size.w = 400;
	rect().size.h = 300;
	style() |= (ATOM_Widget::Border|ATOM_Widget::TitleBar);
	style() &= ~ATOM_Widget::Control;
}

WidgetParameters *DialogParameters::clone (void) const
{
	return ATOM_NEW(DialogParameters, *this);
}

DialogProxy::DialogProxy (PluginGUI *plugin)
	: ControlProxy (plugin)
{
}

ATOM_Widget *DialogProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_Dialog *dialog = ATOM_NEW(ATOM_Dialog, parent, parameters->rect(), parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (dialog);
	return dialog;
}

void DialogProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *DialogProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(DialogParameters, proxy);
}

