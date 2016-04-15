#include "StdAfx.h"
#include "combobox_proxy.h"

ComboBoxParameters::ComboBoxParameters (ControlProxy *proxy): WidgetParameters(proxy)
{
	rect().size.w = 100;
	rect().size.h = 30;

	_buttonWidth = 18;
	_dropdownHeight = 100;
	_buttonImageId = 1;
	_buttonBorderMode = -1;
	_dropdownImageId = 1;
	_dropdownItemHeight = 20;
	_dropDownHilightImageId = 0;
	_dropDownBorderMode = -1;
	_editImageId = 0;
	_editBorderMode = -1;
	_offsetX = 0;
	_scrollBarBackgroundImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarDownButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarSliderImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarUpButtonImageId = ATOM_AUTOMATIC_IMAGEID;

	_buttonLength = 16;
	_sliderWidth = 16;
	_sliderLength = 16;
}

void ComboBoxParameters::resetParameters (void)
{
	WidgetParameters::resetParameters ();

	imageId() = ATOM_IMAGEID_CONTROL_BKGROUND;
	rect().size.w = 100;
	rect().size.h = 30;
	layoutType() = ATOM_WidgetLayout::Horizontal;

	_buttonWidth = 18;
	_dropdownHeight = 100;
	_buttonImageId = ATOM_IMAGEID_SCROLLBAR_VBUTTONDOWN;
	_buttonBorderMode = -1;
	_dropdownImageId = 1;
	_dropdownItemHeight = 20;
	_dropDownHilightImageId = 0;
	_dropDownBorderMode = -1;
	_editImageId = 0;
	_editBorderMode = -1;
	_offsetX = 0;
	_scrollBarBackgroundImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarDownButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarSliderImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarUpButtonImageId = ATOM_AUTOMATIC_IMAGEID;

	_buttonLength = 16;
	_sliderWidth = 16;
	_sliderLength = 16;
}

WidgetParameters *ComboBoxParameters::clone (void) const
{
	return ATOM_NEW(ComboBoxParameters, *this);
}

void ComboBoxParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	WidgetParameters::setupTweakBar (propBar);

	ATOMX_TBEnum borderStyleEnum;
	borderStyleEnum.addEnum ("无", -1);
	borderStyleEnum.addEnum ("平坦", ATOM_Widget::Flat);
	borderStyleEnum.addEnum ("凸起", ATOM_Widget::Raise);
	borderStyleEnum.addEnum ("陷下", ATOM_Widget::Drop);

	propBar->addIntVar ("按钮宽度", PARAMID_COMBOBOX_BUTTON_WIDTH, _buttonWidth, false, "下拉列表框", 0);
	propBar->addIntVar ("按钮图片", PARAMID_COMBOBOX_BUTTON_IMAGEID, _buttonImageId, false, "下拉列表框", 0);
	propBar->addEnum ("按钮边框", PARAMID_COMBOBOX_BUTTON_BORDER, _buttonBorderMode, borderStyleEnum, false, "下拉列表框", 0);
	propBar->addIntVar ("下拉长度", PARAMID_COMBOBOX_DROPDOWN_HEIGHT, _dropdownHeight, false, "下拉列表框", 0);
	propBar->addIntVar ("下拉图片", PARAMID_COMBOBOX_DROPDOWN_IMAGEID, _dropdownImageId, false, "下拉列表框", 0);
	propBar->addIntVar ("下拉项高度", PARAMID_COMBOBOX_DROPDOWN_ITEM_HEIGHT, _dropdownItemHeight, false, "下拉列表框", 0);
	propBar->addIntVar ("下拉项高亮图片", PARAMID_COMBOBOX_DROPDOWN_HILIGHT_IMAGEID, _dropDownHilightImageId, false, "下拉列表框", 0);
	propBar->addEnum ("下拉边框", PARAMID_COMBOBOX_DROPDOWN_BORDER, _dropDownBorderMode, borderStyleEnum, false, "下拉列表框", 0);
	propBar->addIntVar ("编辑框图片", PARAMID_COMBOBOX_EDIT_IMAGEID, _editImageId, false, "下拉列表框", 0);
	propBar->addEnum ("编辑框边框", PARAMID_COMBOBOX_EDIT_BORDER, _editBorderMode, borderStyleEnum, false, "下拉列表框", 0);
	propBar->addIntVar ("文字横向偏移", PARAMID_COMBOBOX_OFFSETX, _offsetX, false, "下拉列表框", 0);

	propBar->addIntVar ("滚动条背景图片", PARAMID_SCROLLBAR_BACKGROUND_IMAGE, _scrollBarBackgroundImageId, false, "滚动条", 0);
	propBar->addIntVar ("滚动条上按钮图片", PARAMID_SCROLLBAR_UPBUTTON_IMAGE, _scrollBarUpButtonImageId, false, "滚动条", 0);
	propBar->addIntVar ("滚动条下按钮图片", PARAMID_SCROLLBAR_DOWNBUTTON_IMAGE, _scrollBarDownButtonImageId, false, "滚动条", 0);
	propBar->addIntVar ("滚动条块图片", PARAMID_SCROLLBAR_SLIDER_IMAGE, _scrollBarSliderImageId, false, "滚动条", 0);
	propBar->addIntVar ("按钮长度", PARAMID_SCROLLBAR_BUTTON_LENGTH, _buttonLength, false, "滚动条", 0);
	propBar->addIntVar ("滚动条宽度", PARAMID_SCROLLBAR_SLIDER_WIDTH, _sliderWidth, false, "滚动条", 0);
	propBar->addIntVar ("滑动块长度", PARAMID_SCROLLBAR_SLIDER_LENGTH, _sliderLength, false, "滚动条", 0);

}

void ComboBoxParameters::apply (ATOM_Widget *widget)
{
	WidgetParameters::apply (widget);

	ATOM_ComboBox *combobox = (ATOM_ComboBox*)widget;
	combobox->setButtonWidth (_buttonWidth);
	if (_buttonBorderMode != -1)
	{
		combobox->getDropDownButton ()->setStyle (combobox->getDropDownButton()->getStyle()|ATOM_Widget::Border);
	}
	else
	{
		combobox->getDropDownButton ()->setStyle (combobox->getDropDownButton()->getStyle()&~ATOM_Widget::Border);
	}

	combobox->getDropDownButton()->setClientImageId (_buttonImageId);
	combobox->setDropDownHeight (_dropdownHeight);
	combobox->setDropDownListImageId(_dropdownImageId);
	combobox->setDropDownListItemHeight (_dropdownItemHeight);
	combobox->setDropDownListHilightImageId (_dropDownHilightImageId);
	if (_dropDownBorderMode!= -1)
	{
		combobox->getDropDownList()->setStyle (combobox->getDropDownList()->getStyle()|ATOM_Widget::Border);
		combobox->getDropDownList()->setBorderMode ((ATOM_Widget::BorderMode)_dropDownBorderMode);
	}
	else
	{
		combobox->getDropDownList()->setStyle (combobox->getDropDownList()->getStyle()&~ATOM_Widget::Border);
	}

	combobox->getInputEdit()->setClientImageId (_editImageId);
	if (_editBorderMode!= -1)
	{
		combobox->getInputEdit()->setStyle (combobox->getInputEdit()->getStyle()|ATOM_Widget::Border);
		combobox->getInputEdit()->setBorderMode ((ATOM_Widget::BorderMode)_editBorderMode);
	}
	else
	{
		combobox->getInputEdit()->setStyle (combobox->getInputEdit()->getStyle()&~ATOM_Widget::Border);
	}
	if(_offsetX)
		combobox->setOffsetX(_offsetX);

	combobox->setVerticalSCrollBarImageId(_scrollBarBackgroundImageId,_scrollBarSliderImageId,
		_scrollBarUpButtonImageId,_scrollBarDownButtonImageId);

	combobox->setscrollBarWidth(_sliderWidth);
	combobox->setScrollBarSliderLength(_sliderLength);
	combobox->setScrollBarButtonLength(_buttonLength);
}

void ComboBoxParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("buttonwidth", _buttonWidth);
	xml->SetAttribute ("buttonimage", _buttonImageId);
	xml->SetAttribute ("buttonborder", _buttonBorderMode);
	xml->SetAttribute ("dropdownheight", _dropdownHeight);
	xml->SetAttribute ("dropdownimage", _dropdownImageId);
	xml->SetAttribute ("dropdownitemheight", _dropdownItemHeight);
	xml->SetAttribute ("dropdownhilightimage", _dropDownHilightImageId);
	xml->SetAttribute ("dropdownborder", _dropDownBorderMode);
	xml->SetAttribute ("editimage", _editImageId);
	xml->SetAttribute ("editborder", _editBorderMode);
	if(_offsetX)
		xml->SetAttribute ("offsetx", _offsetX);

	xml->SetAttribute ("handleimage", _scrollBarSliderImageId);
	xml->SetAttribute ("ubimage", _scrollBarUpButtonImageId);
	xml->SetAttribute ("dbimage", _scrollBarDownButtonImageId);
	xml->SetAttribute ("scrollBarBkImage", _scrollBarBackgroundImageId);

	xml->SetAttribute ("buttonLength", _buttonLength);
	xml->SetAttribute ("sliderWidth", _sliderWidth);
	xml->SetAttribute ("sliderLength", _sliderLength);
}

void ComboBoxParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("buttonwidth", &_buttonWidth);
		xml->QueryIntAttribute ("buttonimage", &_buttonImageId);
		xml->QueryIntAttribute ("buttonborder", &_buttonBorderMode);
		xml->QueryIntAttribute ("dropdownheight", &_dropdownHeight);
		xml->QueryIntAttribute ("dropdownimage", &_dropdownImageId);
		xml->QueryIntAttribute ("dropdownitemheight", &_dropdownItemHeight);
		xml->QueryIntAttribute ("dropdownhilightimage", &_dropDownHilightImageId);
		xml->QueryIntAttribute ("dropdownborder", &_dropDownBorderMode);
		xml->QueryIntAttribute ("editimage", &_editImageId);
		xml->QueryIntAttribute ("editborder", &_editBorderMode);
		xml->QueryIntAttribute ("offsetx", &_offsetX);


		xml->QueryIntAttribute ("handleimage", &_scrollBarSliderImageId);
		xml->QueryIntAttribute ("ubimage", &_scrollBarUpButtonImageId);
		xml->QueryIntAttribute ("dbimage", &_scrollBarDownButtonImageId);
		xml->QueryIntAttribute ("scrollBarBkImage", &_scrollBarBackgroundImageId);

		xml->QueryIntAttribute ("buttonLength", &_buttonLength);
		xml->QueryIntAttribute ("sliderWidth", &_sliderWidth);
		xml->QueryIntAttribute ("sliderLength", &_sliderLength);
	}
}

bool ComboBoxParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_COMBOBOX_BUTTON_WIDTH:
		_buttonWidth = event->newValue.getI();
		return true;
	case PARAMID_COMBOBOX_BUTTON_BORDER:
		_buttonBorderMode = event->newValue.getI();
		return true;
	case PARAMID_COMBOBOX_DROPDOWN_HEIGHT:
		_dropdownHeight = event->newValue.getI();
		return true;
	case PARAMID_COMBOBOX_BUTTON_IMAGEID:
		_buttonImageId = event->newValue.getI();
		return true;
	case PARAMID_COMBOBOX_DROPDOWN_IMAGEID:
		_dropdownImageId = event->newValue.getI();
		return true;
	case PARAMID_COMBOBOX_DROPDOWN_ITEM_HEIGHT:
		_dropdownItemHeight = event->newValue.getI();
		return true;
	case PARAMID_COMBOBOX_DROPDOWN_HILIGHT_IMAGEID:
		_dropDownHilightImageId = event->newValue.getI();
		return true;
	case PARAMID_COMBOBOX_DROPDOWN_BORDER:
		_dropDownBorderMode = event->newValue.getI();
		return true;
	case PARAMID_COMBOBOX_EDIT_IMAGEID:
		_editImageId = event->newValue.getI();
		return true;
	case PARAMID_COMBOBOX_EDIT_BORDER:
		_editBorderMode = event->newValue.getI();
		return true;
		return true;
	case PARAMID_COMBOBOX_OFFSETX:
		_offsetX = event->newValue.getI();
		return true;
	case PARAMID_SCROLLBAR_BACKGROUND_IMAGE:
		_scrollBarBackgroundImageId = event->newValue.getI();
		return true;
	case PARAMID_SCROLLBAR_DOWNBUTTON_IMAGE:
		_scrollBarDownButtonImageId = event->newValue.getI();
		return true;
	case PARAMID_SCROLLBAR_SLIDER_IMAGE:
		_scrollBarSliderImageId = event->newValue.getI();
		return true;
	case PARAMID_SCROLLBAR_UPBUTTON_IMAGE:
		_scrollBarUpButtonImageId = event->newValue.getI();
		return true;
	case PARAMID_SCROLLBAR_BUTTON_LENGTH:
		_buttonLength = event->newValue.getI();
		return true;
	case PARAMID_SCROLLBAR_SLIDER_WIDTH:
		_sliderWidth = event->newValue.getI();
		return true;
	case PARAMID_SCROLLBAR_SLIDER_LENGTH:
		_sliderLength= event->newValue.getI();
		return true;
	default:
		return WidgetParameters::handleBarValueChangedEvent (event);
	}
}

bool ComboBoxParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return WidgetParameters::handleBarCommandEvent (event);
}

//////////////////////////////////////////////////////////////////////////////////////

ComboBoxProxy::ComboBoxProxy (PluginGUI *plugin)
	: ControlProxy (plugin)
{
}

ATOM_Widget *ComboBoxProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_ComboBox *combobox = ATOM_NEW(ATOM_ComboBox, parent, parameters->rect(), ((ComboBoxParameters*)parameters)->_buttonWidth, parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (combobox);
	return combobox;
}

void ComboBoxProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *ComboBoxProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(ComboBoxParameters, proxy);
}

