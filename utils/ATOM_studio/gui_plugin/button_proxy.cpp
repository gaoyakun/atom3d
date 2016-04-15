#include "StdAfx.h"
#include "button_proxy.h"

ButtonParameters::ButtonParameters (ControlProxy *proxy): LabelParameters(proxy)
{
	style() |= ATOM_Widget::Border;
	rect().size.w = 60;
	rect().size.h = 24;
	_alignmentX = 1;
	_clickSound = INVALID_AUDIOID;
	_hoverSound = INVALID_AUDIOID;
	_text = "确定";
}

void ButtonParameters::resetParameters (void)
{
	LabelParameters::resetParameters ();

	style() |= ATOM_Widget::Border;
	rect().size.w = 60;
	rect().size.h = 24;
	_alignmentX = 1;
	_clickSound = INVALID_AUDIOID;
	_hoverSound = INVALID_AUDIOID;
	_text = "确定";
}

WidgetParameters *ButtonParameters::clone (void) const
{
	return ATOM_NEW(ButtonParameters, *this);
}

void ButtonParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	LabelParameters::setupTweakBar (propBar);

	propBar->addIntVar ("点击声音", PARAMID_BUTTON_CLICKSOUND, _clickSound, false, "按钮", 0);
	propBar->addIntVar ("高亮声音", PARAMID_BUTTON_HOVERSOUND, _hoverSound, false, "按钮", 0);
}

void ButtonParameters::apply (ATOM_Widget *widget)
{
	LabelParameters::apply (widget);

	ATOM_Button *btn = (ATOM_Button*)widget;
	btn->setClickSound (_clickSound);
	btn->setHoverSound (_hoverSound);
}

void ButtonParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	LabelParameters::saveToXML (xml);

	if (_clickSound != INVALID_AUDIOID)
		xml->SetAttribute ("clicksound", _clickSound);

	if (_hoverSound != INVALID_AUDIOID)
		xml->SetAttribute ("hoversound", _hoverSound);
}

void ButtonParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	LabelParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("clicksound", &_clickSound);
		xml->QueryIntAttribute ("hoversound", &_hoverSound);
	}
}

bool ButtonParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_BUTTON_CLICKSOUND:
		_clickSound = event->newValue.getI();
		return true;
	case PARAMID_BUTTON_HOVERSOUND:
		_hoverSound = event->newValue.getI();
		return true;
	default:
		return LabelParameters::handleBarValueChangedEvent (event);
	}
}

bool ButtonParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return LabelParameters::handleBarCommandEvent (event);
}

//////////////////////////////////////////////////////////////////////////////////////

ButtonProxy::ButtonProxy (PluginGUI *plugin)
	: LabelProxy (plugin)
{
}

ATOM_Widget *ButtonProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_Button *button = ATOM_NEW(ATOM_Button, parent, parameters->rect(), parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (button);
	return button;
}

void ButtonProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *ButtonProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(ButtonParameters, proxy);
}

