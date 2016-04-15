#ifndef __ATOM3D_STUDIO_GUIEDITOR_LISTBOX_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_LISTBOX_PROXY_H

#include "proxy.h"
#include "widgetparameters.h"

class ListBoxParameters: public WidgetParameters
{
public:
	enum
	{
		PARAMID_LISTBOX_LINEHEIGHT = WidgetParameters::PARAMID_LAST,
		PARAMID_LISTBOX_SELECTED_IMAGE,
		PARAMID_LISTBOX_HOVER_IMAGE,
		PARAMID_LISTBOX_LAST
	};

public:
	ListBoxParameters (ControlProxy *proxy);
	virtual void resetParameters (void);
	virtual WidgetParameters *clone (void) const;
	virtual void setupTweakBar (ATOMX_TweakBar *propBar);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual bool handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event);
	virtual bool handleBarCommandEvent (ATOMX_TWCommandEvent *event);

protected:
	int _lineHeight;
	int _selectedImageId;
	int _hoverImageId;
};

class ListBoxProxy: public ControlProxy
{
public:
	ListBoxProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(ListBoxProxy, listbox, ATOM_ListBox)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_LISTBOX_PROXY_H
