#ifndef __ATOM3D_STUDIO_GUIEDITOR_PANEL_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_PANEL_PROXY_H

#include "proxy.h"
#include "widgetparameters.h"

class PanelParameters: public WidgetParameters
{
public:
	enum
	{
		PARAMID_PANEL_EVENTTRANSITION = WidgetParameters::PARAMID_LAST,
		PARAMID_PANEL_LAST
	};

public:
	PanelParameters (ControlProxy *proxy): WidgetParameters(proxy)
	{
	}

	virtual void resetParameters (void);
	virtual WidgetParameters *clone (void) const;
	virtual void setupTweakBar (ATOMX_TweakBar *propBar);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual bool handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event);
	virtual bool handleBarCommandEvent (ATOMX_TWCommandEvent *event);

private:
	bool _eventTransition;
};

class PanelProxy: public ControlProxy
{
public:
	PanelProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(PanelProxy, panel, ATOM_Panel)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_PANEL_PROXY_H
