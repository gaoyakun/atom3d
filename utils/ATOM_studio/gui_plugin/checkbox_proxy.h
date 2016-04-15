#ifndef __ATOM3D_STUDIO_GUIEDITOR_CHECKBOX_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_CHECKBOX_PROXY_H

#include "label_proxy.h"

class CheckBoxParameters: public LabelParameters
{
public:
	enum
	{
		PARAMID_CHECKBOX_CLICKSOUND = LabelParameters::PARAMID_LABEL_LAST,
		PARAMID_CHECKBOX_HOVERSOUND,
		PARAMID_CHECKBOX_CHECKED_IMAGE,
		PARAMID_CHECKBOX_CHECKED,
		PARAMID_BUTTON_LAST
	};

public:
	CheckBoxParameters (ControlProxy *proxy);
	virtual void resetParameters (void);
	virtual WidgetParameters *clone (void) const;
	virtual void setupTweakBar (ATOMX_TweakBar *propBar);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual bool handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event);
	virtual bool handleBarCommandEvent (ATOMX_TWCommandEvent *event);

private:
	int _clickSound;
	int _hoverSound;
	int _checkedImageId;
	int _checked;
};

class CheckBoxProxy: public LabelProxy
{
public:
	CheckBoxProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(CheckBoxProxy, checkbox, ATOM_Checkbox)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_CHECKBOX_PROXY_H
