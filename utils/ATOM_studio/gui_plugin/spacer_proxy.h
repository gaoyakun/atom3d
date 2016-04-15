#ifndef __ATOM3D_STUDIO_GUIEDITOR_SPACER_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_SPACER_PROXY_H

#include "proxy.h"
#include "widgetparameters.h"

class SpacerParameters: public WidgetParameters
{
public:
	SpacerParameters (ControlProxy *proxy): WidgetParameters(proxy)
	{
	}

	virtual bool interactive (void);
	virtual void resetParameters (void);
	virtual WidgetParameters *clone (void) const;
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
};

class SpacerProxy: public ControlProxy
{
public:
	SpacerProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(SpacerProxy, spacer, ATOM_Panel)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_SPACER_PROXY_H
