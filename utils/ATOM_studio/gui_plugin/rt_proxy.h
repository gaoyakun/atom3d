#ifndef __ATOM3D_STUDIO_GUIEDITOR_RT_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_RT_PROXY_H

#include "proxy.h"
#include "widgetparameters.h"

class RealtimeCtrlParameters: public WidgetParameters
{
	friend class RealtimeCtrlProxy;

public:
	enum
	{
		PARAMID_REALTIMECTRL_TRANSPARENT = WidgetParameters::PARAMID_LAST,
		PARAMID_REALTIMECTRL_LAST
	};

public:
	RealtimeCtrlParameters (ControlProxy *proxy);
	virtual void resetParameters (void);
	virtual WidgetParameters *clone (void) const;
	virtual void setupTweakBar (ATOMX_TweakBar *propBar);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual bool handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event);
	virtual bool handleBarCommandEvent (ATOMX_TWCommandEvent *event);

protected:
	int _transparent;
};

class RealtimeCtrlProxy: public ControlProxy
{
public:
	RealtimeCtrlProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(RealtimeCtrlProxy, realtimectrl, ATOM_RealtimeCtrl)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_RT_PROXY_H
