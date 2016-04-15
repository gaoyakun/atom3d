#ifndef __ATOM3D_STUDIO_GUIEDITOR_SCROLLMAP_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_SCROLLMAP_PROXY_H

#include "proxy.h"
#include "widgetparameters.h"

class ScrollMapParameters: public WidgetParameters
{
public:
	ScrollMapParameters (ControlProxy *proxy);
	virtual WidgetParameters *clone (void) const;
};

class ScrollMapProxy: public ControlProxy
{
public:
	ScrollMapProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(ScrollMapProxy, scrollmap, ATOM_ScrollMap)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_SCROLLMAP_PROXY_H
