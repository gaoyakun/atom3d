#ifndef __ATOM3D_STUDIO_GUIEDITOR_WINDOW_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_WINDOW_PROXY_H

#include "proxy.h"
#include "widgetparameters.h"

class WindowParameters: public WidgetParameters
{
public:
	WindowParameters (ControlProxy *proxy);
	virtual void resetParameters (void);
	virtual WidgetParameters *clone (void) const;
};

class WindowProxy: public ControlProxy
{
public:
	WindowProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(WindowProxy, window, ATOM_TopWindow)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_WINDOW_PROXY_H
