#ifndef __ATOM3D_STUDIO_GUIEDITOR_PROXY_H
#define __ATOM3D_STUDIO_GUIEDITOR_PROXY_H

#include "editor.h"

class PluginGUI;
class WidgetParameters;

#define DECLARE_WIDGET_PROXY(proxy_classname, name, classname) \
	public: \
	virtual const char *getTagName (void) const { return #name; } \
	virtual const char *getWidgetClassName (void) const { return #classname; } \
	static const char *tagName (void) { return #name; } \
	static const char *widgetClassName (void) { return #classname; } \
	protected: \
	virtual ControlProxy *dup (void) const { return ATOM_NEW(proxy_classname, getPlugin()); }

#define WT_SPACER		WT_LAST
#define WT_COMPONENT	((ATOM_WidgetType)(WT_LAST+1))
class ControlProxy
{
public:
	enum
	{
		ID_Id = 0,
		ID_ImageId,
		ID_BorderStyle,
		ID_TitleBar,
		ID_CloseButton,
		ID_Control,
		ID_NoClip,
		ID_NoFocus,
		ID_VScroll,
		ID_HScroll,
		ID_NonScrollable,
		ID_ClipChildren,
		ID_LeftScroll,
		ID_TopScroll,
		ID_RectX,
		ID_RectY,
		ID_RectW,
		ID_RectH,
		ID_Layout,
		ID_LayoutSpaceLeft,
		ID_LayoutSpaceTop,
		ID_LayoutSpaceRight,
		ID_LayoutSpaceBottom,
		ID_LayoutGap,
		ID_Font,
		ID_FontColor,
		ID_Last
	};

	struct PropertyInfo
	{
		ATOM_Variant value;
		AS_PropertyType type;
	};

public:
	ControlProxy (PluginGUI *plugin);
	virtual ~ControlProxy (void);

public:
	ATOM_Widget *getWidget (void) const;
	bool loadFromXML (ATOM_TiXmlElement *xml, ATOM_Widget *parent);
	bool saveToXML (ATOM_TiXmlElement *xml);
	void setupTweakBar (ATOMX_TweakBar *bar, ATOMX_TweakBar *cbar);
	void setName (const char *name);
	const char *getName (void) const;
	void changeFont (const char *fontName);
	ATOM_GUIFont::handle getFontHandle (const char *fontName) const;
	void resize (const ATOM_Rect2Di &rc);
	void handleTWCommandEvent (ATOMX_TWCommandEvent *event);
	void handleTWValueChangedEvent (ATOMX_TWValueChangedEvent *event);
	PluginGUI *getPlugin (void) const;
	WidgetParameters *getWidgetParams (void) const;
	void setTreeItem (ATOM_TreeItem *treeItem);
	ATOM_TreeItem *getTreeItem (void) const;
	ATOM_HASHMAP<ATOM_STRING, PropertyInfo> & getProperties (void);
	const ATOM_HASHMAP<ATOM_STRING, PropertyInfo> & getProperties (void) const;
	int newProperty (ATOM_STRING &name);
	void setupPropertyBar (ATOMX_TweakBar *bar);
	void setupProperty (ATOMX_TweakBar *bar, const char *name, const ControlProxy::PropertyInfo &info);
	void setupIntProperty (ATOMX_TweakBar *bar, const char *name, const ATOM_Variant &value);
	void setupFloatProperty (ATOMX_TweakBar *bar, const char *name, const ATOM_Variant &value);
	void setupStringProperty (ATOMX_TweakBar *bar, const char *name, const ATOM_Variant &value);
	void setupVectorProperty (ATOMX_TweakBar *bar, const char *name, const ATOM_Variant &value);
	void setupColorProperty (ATOMX_TweakBar *bar, const char *name, const ATOM_Variant &value);
	void setupSwitchProperty (ATOMX_TweakBar *bar, const char *name, const ATOM_Variant &value);
	ATOM_STRING generateName (void) const;

public:
	virtual const char *getTagName (void) const = 0;
	virtual const char *getWidgetClassName (void) const = 0;
	virtual ControlProxy *clone (ControlProxy *parent, bool deepClone) const;

protected:
	virtual void changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged) = 0;
	virtual ATOM_Widget *createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters) = 0;
	virtual WidgetParameters *createParametersImpl (ControlProxy *proxy) const = 0;
	virtual void deleteParametersImpl (WidgetParameters *parameters);
	virtual ControlProxy *dup (void) const = 0;

public:
	static ControlProxy *createProxyByType (ATOM_WidgetType type, PluginGUI *plugin);
	static ControlProxy *createProxy (PluginGUI *plugin, ATOM_Widget *parent, int widgetType);
	static ControlProxy *createProxyFromXML (PluginGUI *plugin, ATOM_Widget *parent, ATOM_TiXmlElement *xml);
	static void deleteProxy (ControlProxy *proxy);
	static ControlProxy *getWidgetProxy (ATOM_Widget *widget);
	static void clearProxyMap (void);
	static ATOM_WidgetType getTypeFromTypeString (const char *typeStr);
	static ATOM_STRING getClassNameFromType (ATOM_WidgetType type);

protected:
	PluginGUI *_plugin;
	ATOM_Widget *_widget;
	WidgetParameters *_widgetParams;
	ATOM_STRING _name;
	ATOM_TreeItem *_treeItem;
	ATOM_HASHMAP<ATOM_STRING, PropertyInfo> _properties;

	static ATOM_HashMap<ATOM_Widget*, ControlProxy*> _widgetProxyMap;
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_PROXY_H
