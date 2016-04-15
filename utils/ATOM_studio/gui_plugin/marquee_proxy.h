#ifndef __ATOM3D_STUDIO_GUIEDITOR_MARQUEE_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_MARQUEE_PROXY_H

#include "proxy.h"
#include "widgetparameters.h"

class MarqueeParameters: public WidgetParameters
{
public:
	enum
	{
		PARAMID_MARQUEE_DIRECTION = WidgetParameters::PARAMID_LAST,
		PARAMID_MARQUEE_SPEED,
		PARAMID_MARQUEE_POSITION,
		PARAMID_MARQUEE_OFFSET,
		PARAMID_MARQUEE_TEXT,
		PARAMID_PANEL_LAST
	};

public:
	MarqueeParameters (ControlProxy *proxy): WidgetParameters(proxy)
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
	MARQUEEDIR _dir;
	float _speed;
	float _offset;
	float _position;
	ATOM_STRING _text;
};

class MarqueeProxy: public ControlProxy
{
public:
	MarqueeProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(MarqueeProxy, marquee, ATOM_Marquee)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_MARQUEE_PROXY_H
