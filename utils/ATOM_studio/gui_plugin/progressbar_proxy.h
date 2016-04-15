#ifndef __ATOM3D_STUDIO_GUIEDITOR_PROGRESSBAR_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_PROGRESSBAR_PROXY_H

#include "label_proxy.h"

class ProgressBarParameters: public LabelParameters
{
public:
	enum
	{
		PARAMID_PROGRESSBAR_FRONTIMAGE = LabelParameters::PARAMID_LABEL_LAST,
		PARAMID_PROGRESSBAR_MINVALUE,
		PARAMID_PROGRESSBAR_MAXVALUE,
		PARAMID_PROGRESSBAR_CURRENTVALUE,
		PARAMID_PROGRESSBAR_DRAWTEXT,
		PARAMID_PROGRESSBAR_HORIZONTAL,
		PARAMID_PROGRESSBAR_LAST
	};

public:
	ProgressBarParameters (ControlProxy *proxy);
	virtual void resetParameters (void);
	virtual WidgetParameters *clone (void) const;
	virtual void setupTweakBar (ATOMX_TweakBar *propBar);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual bool handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event);
	virtual bool handleBarCommandEvent (ATOMX_TWCommandEvent *event);

private:
	int _frontImageId;
	int _minValue;
	int _maxValue;
	int _currentValue;
	bool _drawText;
	bool _isHorizontal;
};

class ProgressBarProxy: public LabelProxy
{
public:
	ProgressBarProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(ProgressBarProxy, progressbar, ATOM_ProgressBar)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_PROGRESSBAR_PROXY_H
