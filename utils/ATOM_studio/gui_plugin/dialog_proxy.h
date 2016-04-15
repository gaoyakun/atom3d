#ifndef __ATOM3D_STUDIO_GUIEDITOR_DIALOG_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_DIALOG_PROXY_H

#include "proxy.h"
#include "widgetparameters.h"

class DialogParameters: public WidgetParameters
{
public:
	DialogParameters (ControlProxy *proxy);
	virtual void resetParameters (void);
	virtual WidgetParameters *clone (void) const;
};

class DialogProxy: public ControlProxy
{
public:
	DialogProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(DialogProxy, dialog, ATOM_Dialog)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_DIALOG_PROXY_H
