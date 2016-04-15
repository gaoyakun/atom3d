#include "StdAfx.h"
#include "listbox_proxy.h"

ListBoxParameters::ListBoxParameters (ControlProxy *proxy): WidgetParameters(proxy)
{
	_lineHeight = 24;
	_selectedImageId = ATOM_IMAGEID_LISTITEM_HILIGHT;
	_hoverImageId = ATOM_INVALID_IMAGEID;
}

void ListBoxParameters::resetParameters (void)
{
	WidgetParameters::resetParameters ();

	rect().size.w = 60;
	rect().size.h = 100;
	borderMode() = ATOM_Widget::Drop;
	style() |= ATOM_Widget::Border;
	imageId() = ATOM_IMAGEID_WINDOW_BKGROUND;

	_lineHeight = 24;
	_selectedImageId = ATOM_IMAGEID_LISTITEM_HILIGHT;
	_hoverImageId = ATOM_INVALID_IMAGEID;
}

WidgetParameters *ListBoxParameters::clone (void) const
{
	return ATOM_NEW(ListBoxParameters, *this);
}

void ListBoxParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	WidgetParameters::setupTweakBar (propBar);

	propBar->addIntVar ("行高度", PARAMID_LISTBOX_LINEHEIGHT, _lineHeight, false, "列表框", 0);
	propBar->addIntVar ("选中图片", PARAMID_LISTBOX_SELECTED_IMAGE, _selectedImageId, false, "列表框", 0);
	propBar->addIntVar ("高亮图片", PARAMID_LISTBOX_HOVER_IMAGE, _hoverImageId, false, "列表框", 0);
}

void ListBoxParameters::apply (ATOM_Widget *widget)
{
	WidgetParameters::apply (widget);

	ATOM_ListBox *listbox = (ATOM_ListBox*)widget;
	listbox->setItemHeight (_lineHeight);
	listbox->setSelectImageId (_selectedImageId);
	listbox->setHoverImageId (_hoverImageId);
}

void ListBoxParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("lineheight", _lineHeight);
	xml->SetAttribute ("selimage", _selectedImageId);
	xml->SetAttribute ("hoverimage", _hoverImageId);
}

void ListBoxParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("lineheight", &_lineHeight);
		xml->QueryIntAttribute ("selimage", &_selectedImageId);
		xml->QueryIntAttribute ("hoverimage", &_hoverImageId);
	}
}

bool ListBoxParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_LISTBOX_LINEHEIGHT:
		_lineHeight = event->newValue.getI();
		return true;
	case PARAMID_LISTBOX_SELECTED_IMAGE:
		_selectedImageId = event->newValue.getI();
		return true;
	case PARAMID_LISTBOX_HOVER_IMAGE:
		_hoverImageId = event->newValue.getI();
		return true;
	default:
		return WidgetParameters::handleBarValueChangedEvent (event);
	}
}

bool ListBoxParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return WidgetParameters::handleBarCommandEvent (event);
}

//////////////////////////////////////////////////////////////////////////////////////

ListBoxProxy::ListBoxProxy (PluginGUI *plugin)
	: ControlProxy (plugin)
{
}

ATOM_Widget *ListBoxProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_ListBox *listbox = ATOM_NEW(ATOM_ListBox, parent, parameters->rect(), 24, parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (listbox);
	return listbox;
}

void ListBoxProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *ListBoxProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(ListBoxParameters, proxy);
}

