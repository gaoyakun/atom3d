#ifndef __ATOM3D_STUDIO_GUIEDITOR_SCROLLBAR_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_SCROLLBAR_PROXY_H

#include "proxy.h"
#include "widgetparameters.h"

class ScrollBarParameters: public WidgetParameters
{
public:
	enum
	{
		PARAMID_SCROLLBAR_SLIDER_IMAGE = WidgetParameters::PARAMID_LAST,
		PARAMID_SCROLLBAR_UPBUTTON_IMAGE,
		PARAMID_SCROLLBAR_DOWNBUTTON_IMAGE,
		PARAMID_SCROLLBAR_PLACEMENT,
		PARAMID_SCROLLBAR_BUTTON_WIDTH,
		PARAMID_SCROLLBAR_HANDLE_WIDTH,
		PARAMID_SCROLLBAR_MIN_VALUE,
		PARAMID_SCROLLBAR_MAX_VALUE,
		PARAMID_SCROLLBAR_STEP_VALUE,
		PARAMID_SCROLLBAR_LAST
	};

public:
	ScrollBarParameters (ControlProxy *proxy);
	virtual void resetParameters (void);
	virtual WidgetParameters *clone (void) const;
	virtual void setupTweakBar (ATOMX_TweakBar *propBar);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual bool handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event);
	virtual bool handleBarCommandEvent (ATOMX_TWCommandEvent *event);

protected:
	int _SliderImageId;
	int _UpButtonImageId;
	int _DownButtonImageId;
	int _placement;
	int _buttonWidth;
	int _handleWidth;
	int _minValue;
	int _maxValue;
	int _stepValue;
};

class ScrollBarProxy: public ControlProxy
{
public:
	ScrollBarProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(ScrollBarProxy, scrollbar, ATOM_ScrollBar)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_SCROLLBAR_PROXY_H
