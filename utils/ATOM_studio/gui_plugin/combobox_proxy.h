#ifndef __ATOM3D_STUDIO_GUIEDITOR_COMBOBOX_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_COMBOBOX_PROXY_H

#include "proxy.h"
#include "widgetparameters.h"

class ComboBoxParameters: public WidgetParameters
{
	friend class ComboBoxProxy;

public:
	enum
	{
		PARAMID_COMBOBOX_BUTTON_WIDTH = WidgetParameters::PARAMID_LAST,
		PARAMID_COMBOBOX_DROPDOWN_HEIGHT,
		PARAMID_COMBOBOX_BUTTON_IMAGEID,
		PARAMID_COMBOBOX_BUTTON_BORDER,
		PARAMID_COMBOBOX_DROPDOWN_IMAGEID,
		PARAMID_COMBOBOX_DROPDOWN_ITEM_HEIGHT,
		PARAMID_COMBOBOX_DROPDOWN_HILIGHT_IMAGEID,
		PARAMID_COMBOBOX_DROPDOWN_BORDER,
		PARAMID_COMBOBOX_EDIT_IMAGEID,
		PARAMID_COMBOBOX_EDIT_BORDER,
		PARAMID_COMBOBOX_OFFSETX,


		PARAMID_SCROLLBAR_SLIDER_IMAGE,
		PARAMID_SCROLLBAR_UPBUTTON_IMAGE,
		PARAMID_SCROLLBAR_DOWNBUTTON_IMAGE,
		PARAMID_SCROLLBAR_BACKGROUND_IMAGE,
		PARAMID_SCROLLBAR_SLIDER_WIDTH,
		PARAMID_SCROLLBAR_SLIDER_LENGTH,
		PARAMID_SCROLLBAR_BUTTON_LENGTH,
		PARAMID_COMBOBOX_LAST
	};

public:
	ComboBoxParameters (ControlProxy *proxy);
	virtual void resetParameters (void);
	virtual WidgetParameters *clone (void) const;
	virtual void setupTweakBar (ATOMX_TweakBar *propBar);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual bool handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event);
	virtual bool handleBarCommandEvent (ATOMX_TWCommandEvent *event);

protected:
	int _buttonWidth;
	int _dropdownHeight;
	int _buttonBorderMode;
	int _buttonImageId;
	int _dropdownImageId;
	int _dropdownItemHeight;
	int _dropDownHilightImageId;
	int _dropDownBorderMode;
	int _editImageId;
	int _editBorderMode;
	int _offsetX;


	//! 滚动条的属性
	int _scrollBarSliderImageId;
	int _scrollBarBackgroundImageId;
	int _scrollBarUpButtonImageId;
	int _scrollBarDownButtonImageId;

	int _buttonLength; //! 滚动条按钮长度
	int _sliderLength; //! 滚动块的长度
	int _sliderWidth;  //! 滚动条的宽度

};

class ComboBoxProxy: public ControlProxy
{
public:
	ComboBoxProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(ComboBoxProxy, combobox, ATOM_ComboBox)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_COMBOBOX_PROXY_H
