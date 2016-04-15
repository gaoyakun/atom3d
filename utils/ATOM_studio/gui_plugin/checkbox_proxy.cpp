#include "StdAfx.h"
#include "checkbox_proxy.h"

CheckBoxParameters::CheckBoxParameters (ControlProxy *proxy): LabelParameters(proxy)
{
	rect().size.w = 60;
	rect().size.h = 24;
	imageId() = ATOM_IMAGEID_CHECKBOX_UNCHECKED;
	_alignmentX = 0;
	_clickSound = INVALID_AUDIOID;
	_hoverSound = INVALID_AUDIOID;
	_text = "选项";
	_checked = 0;
	_checkedImageId = ATOM_IMAGEID_CHECKBOX_CHECKED;
}

void CheckBoxParameters::resetParameters (void)
{
	LabelParameters::resetParameters ();

	rect().size.w = 60;
	rect().size.h = 24;
	imageId() = ATOM_IMAGEID_CHECKBOX_UNCHECKED;

	_alignmentX = 1;
	_clickSound = INVALID_AUDIOID;
	_hoverSound = INVALID_AUDIOID;
	_text = "选项";
	_checked = 0;
	_checkedImageId = ATOM_IMAGEID_CHECKBOX_CHECKED;
}

WidgetParameters *CheckBoxParameters::clone (void) const
{
	return ATOM_NEW(CheckBoxParameters, *this);
}

void CheckBoxParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	LabelParameters::setupTweakBar (propBar);

	propBar->addIntVar ("选中图片", PARAMID_CHECKBOX_CHECKED_IMAGE, _checkedImageId, false, "复选框", 0);
	propBar->addBoolVar ("选中", PARAMID_CHECKBOX_CHECKED, _checked!= 0, false, "复选框", 0);
	propBar->addIntVar ("点击声音", PARAMID_CHECKBOX_CLICKSOUND, _clickSound, false, "复选框", 0);
	propBar->addIntVar ("高亮声音", PARAMID_CHECKBOX_HOVERSOUND, _hoverSound, false, "复选框", 0);
}

void CheckBoxParameters::apply (ATOM_Widget *widget)
{
	LabelParameters::apply (widget);

	ATOM_Checkbox *cb = (ATOM_Checkbox*)widget;
	cb->setClickSound (_clickSound);
	cb->setHoverSound (_hoverSound);
	cb->setCheckedImageId (_checkedImageId);
	cb->setChecked (_checked != 0);
}

void CheckBoxParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	LabelParameters::saveToXML (xml);

	if (_clickSound != INVALID_AUDIOID)
		xml->SetAttribute ("clicksound", _clickSound);

	if (_hoverSound != INVALID_AUDIOID)
		xml->SetAttribute ("hoversound", _hoverSound);

	if (_checkedImageId != ATOM_IMAGEID_CHECKBOX_CHECKED)
		xml->SetAttribute ("checkedimage", _checkedImageId);

	xml->SetAttribute ("checked", _checked);

}

void CheckBoxParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	LabelParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("clicksound", &_clickSound);
		xml->QueryIntAttribute ("hoversound", &_hoverSound);
		xml->QueryIntAttribute ("checkedimage", &_checkedImageId);
		xml->QueryIntAttribute ("checked", &_checked);
	}
}

bool CheckBoxParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_CHECKBOX_CLICKSOUND:
		_clickSound = event->newValue.getI();
		return true;
	case PARAMID_CHECKBOX_HOVERSOUND:
		_hoverSound = event->newValue.getI();
		return true;
	case PARAMID_CHECKBOX_CHECKED:
		_checked = event->newValue.getB() ? 1 : 0;
		return true;
	case PARAMID_CHECKBOX_CHECKED_IMAGE:
		_checkedImageId = event->newValue.getI();
		return true;
	default:
		return LabelParameters::handleBarValueChangedEvent (event);
	}
}

bool CheckBoxParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return LabelParameters::handleBarCommandEvent (event);
}

//////////////////////////////////////////////////////////////////////////////////////

CheckBoxProxy::CheckBoxProxy (PluginGUI *plugin)
	: LabelProxy (plugin)
{
}

ATOM_Widget *CheckBoxProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_Checkbox *cb = ATOM_NEW(ATOM_Checkbox, parent, parameters->rect(), parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (cb);
	return cb;
}

void CheckBoxProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *CheckBoxProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(CheckBoxParameters, proxy);
}

