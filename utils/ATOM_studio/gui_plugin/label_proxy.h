#ifndef __ATOM3D_STUDIO_GUIEDITOR_LABEL_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_LABEL_PROXY_H

#include "panel_proxy.h"

class LabelParameters: public PanelParameters
{
public:
	enum
	{
		PARAMID_LABEL_XALIGNMENT = PanelParameters::PARAMID_PANEL_LAST,
		PARAMID_LABEL_YALIGNMENT,
		PARAMID_LABEL_TEXT,
		PARAMID_LABEL_TEXTOFFSETX,
		PARAMID_LABEL_TEXTOFFSETY,
		PARAMID_LABEL_COLOR,
		PARAMID_LABEL_UNDERLINE_COLOR,
		PARAMID_LABEL_ROTATION,
		PARAMID_LABEL_LAST
	};

public:
	LabelParameters (ControlProxy *proxy);
	virtual void resetParameters (void);
	virtual WidgetParameters *clone (void) const;
	virtual void setupTweakBar (ATOMX_TweakBar *propBar);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual bool handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event);
	virtual bool handleBarCommandEvent (ATOMX_TWCommandEvent *event);

protected:
	ATOM_STRING _text;
	int _alignmentX;
	int _alignmentY;
	int _textOffsetX;
	int _textOffsetY;
	ATOM_ColorARGB _color;
//	ATOM_ColorARGB _outlineColor;
	ATOM_ColorARGB _underlineColor;
	float _rotation;
};

class LabelProxy: public PanelProxy
{
public:
	LabelProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(LabelProxy, label, ATOM_Label)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_LABEL_PROXY_H
