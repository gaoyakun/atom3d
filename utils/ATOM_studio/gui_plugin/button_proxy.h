#ifndef __ATOM3D_STUDIO_GUIEDITOR_BUTTON_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_BUTTON_PROXY_H

#include "label_proxy.h"

class ButtonParameters: public LabelParameters
{
public:
	enum
	{
		PARAMID_BUTTON_CLICKSOUND = LabelParameters::PARAMID_LABEL_LAST,
		PARAMID_BUTTON_HOVERSOUND,
		PARAMID_BUTTON_LAST
	};

public:
	ButtonParameters (ControlProxy *proxy);
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
};

class ButtonProxy: public LabelProxy
{
public:
	ButtonProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(ButtonProxy, button, ATOM_Button)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_BUTTON_PROXY_H
