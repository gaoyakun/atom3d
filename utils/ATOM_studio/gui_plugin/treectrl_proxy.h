#ifndef __ATOM3D_STUDIO_GUIEDITOR_TREECTRL_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_TREECTRL_PROXY_H

#include "proxy.h"
#include "widgetparameters.h"

class TreeCtrlProxy;

class TreeCtrlParameters: public WidgetParameters
{
	friend class TreeCtrlProxy;

public:
	enum
	{
		PARAMID_TREECTRL_IMAGESIZE = WidgetParameters::PARAMID_LAST,
		PARAMID_TREECTRL_LINEHEIGHT,
		PARAMID_TREECTRL_INDENT,
		PARAMID_TREECTRL_TEXT_IMAGE,
		PARAMID_TREECTRL_LAST
	};

public:
	TreeCtrlParameters (ControlProxy *proxy);
	virtual void resetParameters (void);
	virtual WidgetParameters *clone (void) const;
	virtual void setupTweakBar (ATOMX_TweakBar *propBar);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual bool handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event);
	virtual bool handleBarCommandEvent (ATOMX_TWCommandEvent *event);

private:
	int _imageSize;
	int _lineHeight;
	int _indent;
	int _textImageId;
	
};

class TreeCtrlProxy: public ControlProxy
{
public:
	TreeCtrlProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(TreeCtrlProxy, treectrl, ATOM_TreeCtrl)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_TREECTRL_PROXY_H
