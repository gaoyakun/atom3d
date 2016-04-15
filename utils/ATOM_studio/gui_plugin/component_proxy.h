#ifndef __ATOM3D_STUDIO_GUIEDITOR_COMPONENT_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_COMPONENT_PROXY_H

#include "proxy.h"
#include "widgetparameters.h"

class ComponentParameters: public WidgetParameters
{
public:
	enum
	{
		PARAMID_COMPONENT_LAST = WidgetParameters::PARAMID_LAST
	};

	struct ChildInfo
	{
		ATOM_STRING name;
		int id;
		ATOM_WidgetType type;
		ATOM_VECTOR<ChildInfo> children;
	};

public:
	ComponentParameters (ControlProxy *proxy);
	virtual void resetParameters (void);
	virtual WidgetParameters *clone (void) const;
	virtual void setupTweakBar (ATOMX_TweakBar *propBar);
	virtual void apply (ATOM_Widget *widget);
	virtual void saveToXML (ATOM_TiXmlElement *xml);
	virtual void loadFromXML (ATOM_TiXmlElement *xml);
	virtual bool handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event);
	virtual bool handleBarCommandEvent (ATOMX_TWCommandEvent *event);

public:
	void setFileName (const char *filename);
	const char *getFileName (void) const;
	const ChildInfo &getChildInfo (void) const;

private:
	void loadChildInfo (ATOM_TiXmlElement *element, ChildInfo *info, const char *parentName);

private:
	ATOM_STRING _filename;
	ChildInfo _childInfo;
};

class ComponentProxy: public ControlProxy
{
public:
	ComponentProxy (PluginGUI *plugin);

protected:
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters);
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged);
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const;

	DECLARE_WIDGET_PROXY(ComponentProxy, component, ATOM_Widget)
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_COMPONENT_PROXY_H
