#ifndef __ATOM3D_STUDIO_GUIEDITOR_VIEWSTACK_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_VIEWSTACK_PROXY_H

#include "proxy.h"
#include "panel_proxy.h"

class ViewStackParameters: public PanelParameters
{
	enum
	{
		PARAMID_VIEWSTACK_ACTIVEPAGE = PanelParameters::PARAMID_PANEL_LAST,
		PARAMID_VIEWSTACK_LAST
	};
public:
	ViewStackParameters (ControlProxy *proxy): PanelParameters(proxy)
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
	int _activePage;
};

class ViewStackProxy: public PanelProxy
{
public:
	ViewStackProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(ViewStackProxy, viewstack, ATOM_ViewStack)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_VIEWSTACK_PROXY_H
