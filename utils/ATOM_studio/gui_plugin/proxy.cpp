#include "StdAfx.h"
#include "proxy.h"
#include "idmanager.h"
#include "plugin_gui.h"
#include "panel_proxy.h"
#include "edit_proxy.h"
#include "label_proxy.h"
#include "window_proxy.h"
#include "dialog_proxy.h"
#include "multiedit_proxy.h"
#include "button_proxy.h"
#include "progressbar_proxy.h"
#include "richedit_proxy.h"
#include "cell_proxy.h"
#include "listbox_proxy.h"
#include "checkbox_proxy.h"
#include "listview_proxy.h"
#include "scrollbar_proxy.h"
#include "spacer_proxy.h"
#include "treectrl_proxy.h"
#include "component_proxy.h"
#include "viewstack_proxy.h"
#include "combobox_proxy.h"
#include "flash_proxy.h"
#include "rt_proxy.h"
#include "scrollmap_proxy.h"
#include "marquee_proxy.h"
#include "curveeditor_proxy.h"
#include "widgetparameters.h"

ATOM_HashMap<ATOM_Widget*, ControlProxy*> ControlProxy::_widgetProxyMap;

#define ID_NEW_PROPERTY			(PLUGIN_ID_START + 1000)
#define ID_CLEAR_PROPERTIES		(PLUGIN_ID_START + 1001)
#define ID_PROPERTY_VALUE		(PLUGIN_ID_START + 1002)
#define ID_PROPERTY_DELETE		(PLUGIN_ID_START + 1003)

ControlProxy::ControlProxy (PluginGUI *plugin)
{
	_plugin = plugin;
	_widget = 0;
	_widgetParams = 0;
}

ControlProxy::~ControlProxy (void)
{
	if (_widget)
	{
		ATOM_HashMap<ATOM_Widget*, ControlProxy*>::iterator it = _widgetProxyMap.find (_widget);
		if (it != _widgetProxyMap.end ())
		{
			_widgetProxyMap.erase (it);
		}
		ATOM_DELETE(_widget);
		_widget = 0;

		deleteParametersImpl (_widgetParams);

		_plugin->removeProxyName (getName());
	}
}

void ControlProxy::setupTweakBar (ATOMX_TweakBar *bar, ATOMX_TweakBar *cbar)
{
	bar->clear ();
	_widgetParams->setupTweakBar (bar);

	cbar->clear ();
	setupPropertyBar (cbar);
}

ATOM_STRING ControlProxy::generateName (void) const
{
	char buffer[256];
	ATOM_STRING baseName = getTagName ();
	int id = 1;

	for (;;)
	{
		sprintf (buffer, "%s%d", baseName.c_str(), id++);
		if (_plugin->validateProxyName (buffer))
		{
			return buffer;
		}
	}
}

ControlProxy *ControlProxy::createProxy (PluginGUI *plugin, ATOM_Widget *parent, int widgetType)
{
	ControlProxy *ret = createProxyByType ((ATOM_WidgetType)widgetType, plugin);
	if (!ret)
	{
		::MessageBoxA (ATOM_APP->getMainWindow(), "此功能尚未实现", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
		return 0;
	}

	ATOM_STRING s = ret->generateName ();
	ret->setName (s.c_str());
	plugin->addProxyName (s.c_str());

	if (!ret->loadFromXML (NULL, parent))
	{
		deleteProxy (ret);
		return 0;
	}


	return ret;
}

ControlProxy *ControlProxy::createProxyByType (ATOM_WidgetType type, PluginGUI *plugin)
{
	switch (type)
	{
	case WT_PANEL:
		return ATOM_NEW(PanelProxy, plugin);

	case WT_EDIT:
		return ATOM_NEW(EditProxy, plugin);

	case WT_LABEL:
		return ATOM_NEW(LabelProxy, plugin);

	case WT_TOPWINDOW:
		return ATOM_NEW(WindowProxy, plugin);

	case WT_DIALOG:
		return ATOM_NEW(DialogProxy, plugin);

	case WT_MULTIEDIT:
		return ATOM_NEW(MultieditProxy, plugin);

	case WT_BUTTON:
		return ATOM_NEW(ButtonProxy, plugin);

	case WT_PROGRESSBAR:
		return ATOM_NEW(ProgressBarProxy, plugin);

	case WT_RICHEDIT:
		return ATOM_NEW(RichEditProxy, plugin);

	case WT_CELL:
		return ATOM_NEW(CellProxy, plugin);

	case WT_LISTBOX:
		return ATOM_NEW(ListBoxProxy, plugin);

	case WT_CHECKBOX:
		return ATOM_NEW(CheckBoxProxy, plugin);

	case WT_LISTVIEW:
		return ATOM_NEW(ListViewProxy, plugin);

	case WT_SCROLLBAR:
		return ATOM_NEW(ScrollBarProxy, plugin);

	case WT_SPACER:
		return ATOM_NEW(SpacerProxy, plugin);

	case WT_COMPONENT:
		return ATOM_NEW(ComponentProxy, plugin);

	case WT_TREECTRL:
		return ATOM_NEW(TreeCtrlProxy, plugin);

	case WT_VIEWSTACK:
		return ATOM_NEW(ViewStackProxy, plugin);

	case WT_FLASHCTRL:
		return ATOM_NEW(FlashProxy, plugin);

	case WT_SCROLLMAP:
		return ATOM_NEW(ScrollMapProxy, plugin);

	case WT_COMBOBOX:
		return ATOM_NEW(ComboBoxProxy, plugin);

	case WT_REALTIMECTRL:
		return ATOM_NEW(RealtimeCtrlProxy, plugin);

	case WT_MARQUEE:
		return ATOM_NEW(MarqueeProxy, plugin);

	case WT_CURVEEDITOR:
		return ATOM_NEW(CurveEditorProxy, plugin);

	default:
		return 0;
	}
}

ATOM_STRING ControlProxy::getClassNameFromType (ATOM_WidgetType type)
{
	switch (type)
	{
	case WT_PANEL:
		return PanelProxy::widgetClassName();

	case WT_EDIT:
		return EditProxy::widgetClassName();

	case WT_LABEL:
		return LabelProxy::widgetClassName();

	case WT_TOPWINDOW:
		return WindowProxy::widgetClassName();;

	case WT_DIALOG:
		return DialogProxy::widgetClassName();

	case WT_MULTIEDIT:
		return MultieditProxy::widgetClassName();

	case WT_BUTTON:
		return ButtonProxy::widgetClassName();

	case WT_PROGRESSBAR:
		return ProgressBarProxy::widgetClassName();

	case WT_RICHEDIT:
		return RichEditProxy::widgetClassName();

	case WT_CELL:
		return CellProxy::widgetClassName();

	case WT_LISTBOX:
		return ListBoxProxy::widgetClassName();

	case WT_CHECKBOX:
		return CheckBoxProxy::widgetClassName();

	case WT_LISTVIEW:
		return ListViewProxy::widgetClassName();

	case WT_SCROLLBAR:
		return ScrollBarProxy::widgetClassName();

	case WT_SPACER:
		return SpacerProxy::widgetClassName();

	case WT_COMPONENT:
		return ComponentProxy::widgetClassName();

	case WT_TREECTRL:
		return TreeCtrlProxy::widgetClassName();

	case WT_VIEWSTACK:
		return ViewStackProxy::widgetClassName();

	case WT_FLASHCTRL:
		return FlashProxy::widgetClassName();

	case WT_SCROLLMAP:
		return ScrollMapProxy::widgetClassName();

	case WT_COMBOBOX:
		return ComboBoxProxy::widgetClassName();

	case WT_REALTIMECTRL:
		return RealtimeCtrlProxy::widgetClassName();

	case WT_MARQUEE:
		return MarqueeProxy::widgetClassName();

	case WT_CURVEEDITOR:
		return CurveEditorProxy::widgetClassName();

	default:
		return "";
	}
}

ATOM_WidgetType ControlProxy::getTypeFromTypeString (const char *type)
{
	if (!stricmp(type, PanelProxy::tagName()))
	{
		return WT_PANEL;
	}
	else if (!stricmp(type, EditProxy::tagName()))
	{
		return WT_EDIT;
	}
	else if (!stricmp(type, LabelProxy::tagName()))
	{
		return WT_LABEL;
	}
	else if (!stricmp(type, WindowProxy::tagName()))
	{
		return WT_TOPWINDOW;
	}
	else if (!stricmp(type, DialogProxy::tagName()))
	{
		return WT_DIALOG;
	}
	else if (!stricmp(type, MultieditProxy::tagName()))
	{
		return WT_MULTIEDIT;
	}
	else if (!stricmp(type, ButtonProxy::tagName()))
	{
		return WT_BUTTON;
	}
	else if (!stricmp(type, ProgressBarProxy::tagName()))
	{
		return WT_PROGRESSBAR;
	}
	else if (!stricmp(type, RichEditProxy::tagName()))
	{
		return WT_RICHEDIT;
	}
	else if (!stricmp(type, CellProxy::tagName()))
	{
		return WT_CELL;
	}
	else if (!stricmp(type, ListBoxProxy::tagName()))
	{
		return WT_LISTBOX;
	}
	else if (!stricmp(type, CheckBoxProxy::tagName()))
	{
		return WT_CHECKBOX;
	}
	else if (!stricmp(type, ListViewProxy::tagName()))
	{
		return WT_LISTVIEW;
	}
	else if (!stricmp(type, ScrollBarProxy::tagName()))
	{
		return WT_SCROLLBAR;
	}
	else if (!stricmp(type, SpacerProxy::tagName()))
	{
		return WT_SPACER;
	}
	else if (!stricmp(type, TreeCtrlProxy::tagName()))
	{
		return WT_TREECTRL;
	}
	else if (!stricmp(type, ViewStackProxy::tagName()))
	{
		return WT_VIEWSTACK;
	}
	else if (!stricmp(type, ComponentProxy::tagName()))
	{
		return WT_COMPONENT;
	}
	else if (!stricmp(type, FlashProxy::tagName()))
	{
		return WT_FLASHCTRL;
	}
	else if (!stricmp(type, ScrollMapProxy::tagName()))
	{
		return WT_SCROLLMAP;
	}
	else if (!stricmp(type, ComboBoxProxy::tagName()))
	{
		return WT_COMBOBOX;
	}
	else if (!stricmp(type, RealtimeCtrlProxy::tagName()))
	{
		return WT_REALTIMECTRL;
	}
	else if (!stricmp(type, MarqueeProxy::tagName()))
	{
		return WT_MARQUEE;
	}
	else if (!stricmp(type, CurveEditorProxy::tagName()))
	{
		return WT_CURVEEDITOR;
	}
	else
	{
		return WT_UNKNOWN;
	}
}

ControlProxy *ControlProxy::createProxyFromXML (PluginGUI *plugin, ATOM_Widget *parent, ATOM_TiXmlElement *xml)
{
	ControlProxy *ret = 0;

	const char *type = xml->Attribute ("type");
	if (!type)
	{
		return 0;
	}

	ATOM_WidgetType widgetType = getTypeFromTypeString (type);
	if (widgetType == WT_UNKNOWN)
	{
		return 0;
	}

	ret = createProxyByType (widgetType, plugin);
	if (ret)
	{
		if (!ret->loadFromXML (xml, parent))
		{
			deleteProxy (ret);
			return 0;
		}

		for (ATOM_TiXmlElement *childElement = xml->FirstChildElement("component"); childElement; childElement = childElement->NextSiblingElement("component"))
		{
			if (!createProxyFromXML (plugin, ret->getWidget(), childElement))
			{
				deleteProxy (ret);
				return 0;
			}
		}
	}

	return ret;
}

void ControlProxy::deleteProxy (ControlProxy *proxy)
{
	if (!proxy)
	{
		return;
	}

	if (proxy->getWidget())
	{
		bool proxyDeleted = true;

		while (proxyDeleted)
		{
			proxyDeleted = false;

			for (ATOM_Widget *child = proxy->getWidget()->getFirstControl(); child; child = child->getNextSibling())
			{
				ControlProxy *proxy = ControlProxy::getWidgetProxy (child);

				if (proxy)
				{
					deleteProxy (proxy);
					proxyDeleted = true;
					break;
				}
			}
		}
	}

	ATOM_DELETE(proxy);
}

static void getPropertyInfo (ATOM_TiXmlElement *xml, ControlProxy::PropertyInfo &info, const ATOM_STRING &type, const ATOM_STRING &value)
{
	info.type = AS_PROP_TYPE_NONE;

	if (type == "int")
	{
		info.type = AS_PROP_TYPE_INT;
		int n = 0;
		if (!value.empty ())
		{
			n = atoi (value.c_str());
		}
		info.value.setI (n);
	}
	else if (type == "float")
	{
		info.type = AS_PROP_TYPE_FLOAT;
		float f = 0;
		if (!value.empty ())
		{
			f = atof (value.c_str());
		}
		info.value.setF (f);
	}
	else if (type == "string")
	{
		info.type = AS_PROP_TYPE_STRING;
		info.value.setS (value.c_str());
	}
	else if (type == "vec")
	{
		info.type = AS_PROP_TYPE_VECTOR;
		ATOM_Vector4f v(0.f, 0.f, 0.f, 0.f);
		sscanf (value.c_str(), "%f,%f,%f,%f", &v.x, &v.y, &v.z, &v.w);
		info.value.setV (v);
	}
	else if (type == "color")
	{
		info.type = AS_PROP_TYPE_COLOR4F;
		ATOM_Vector4f v(0.f, 0.f, 0.f, 0.f);
		sscanf (value.c_str(), "%f,%f,%f,%f", &v.x, &v.y, &v.z, &v.w);
		info.value.setV (v);
	}
	else if (type == "bool")
	{
		info.type = AS_PROP_TYPE_SWITCH;
		info.value.setI (value=="true" ? 1 : 0);
	}
}

bool ControlProxy::loadFromXML (ATOM_TiXmlElement *xml, ATOM_Widget *parent)
{
	char buffer[256];

	if (xml)
	{
		const char *name = xml->Attribute ("name");
		if (!name)
		{
			return false;
		}
		if (!_plugin->validateProxyName (name))
		{
			int n = 1;
			for (;;)
			{
				sprintf (buffer, "%s__%d", name, n++);
				if (_plugin->validateProxyName (buffer))
				{
					name = buffer;
					break;
				}
			}
			MessageBoxA (ATOM_APP->getMainWindow(), "发现重复控件名称,已自动修正", "ATOM3D编辑器", MB_OK|MB_ICONWARNING);
		}
		setName (name);
		_plugin->addProxyName (name);
	}

	_widgetParams = createParametersImpl (this);
	_widgetParams->loadFromXML (xml);
	_widget = createWidgetImpl (parent, _widgetParams);
	_widgetProxyMap[_widget] = this;

	ATOM_TiXmlElement *props = xml ? xml->FirstChildElement ("properties") : 0;
	if (props)
	{
		for (ATOM_TiXmlElement *prop  = props->FirstChildElement ("property"); prop; prop = prop->NextSiblingElement("property"))
		{
			const char *name = prop->Attribute ("name");
			if (name)
			{
				const char *type = prop->Attribute ("type");
				const char *value = prop->Attribute ("value");
				if (type && value)
				{
					getPropertyInfo (prop, _properties[name], type, value);
				}
			}
		}
	}

	return true;
}

static void getPropertyString (const ControlProxy::PropertyInfo &info, ATOM_STRING &type, ATOM_STRING &value)
{
	char buffer[1024];

	switch (info.type)
	{
	case AS_PROP_TYPE_INT:
		type = "int";
		sprintf (buffer, "%d", info.value.getI());
		break;
	case AS_PROP_TYPE_FLOAT:
		type = "float";
		sprintf (buffer, "%f", info.value.getF());
		break;
	case AS_PROP_TYPE_STRING:
		type = "string";
		strcpy (buffer, info.value.getS());
		break;
	case AS_PROP_TYPE_VECTOR:
		type = "vec";
		sprintf (buffer, "%f,%f,%f,%f", info.value.getV()[0], info.value.getV()[1], info.value.getV()[2], info.value.getV()[3]);
		break;
	case AS_PROP_TYPE_COLOR4F:
		type = "color";
		sprintf (buffer, "%f,%f,%f,%f", info.value.getV()[0], info.value.getV()[1], info.value.getV()[2], info.value.getV()[3]);
		break;
	case AS_PROP_TYPE_SWITCH:
		type = "bool";
		strcpy (buffer, info.value.getI() != 0 ? "true" : "false");
		break;
	default:
		return;
	}

	value = buffer;
}

bool ControlProxy::saveToXML (ATOM_TiXmlElement *xml)
{
	if (_widget)
	{
		xml->SetValue ("component");
		xml->SetAttribute ("type", getTagName());
		xml->SetAttribute ("name", getName());
		_widgetParams->saveToXML (xml);

		if (!_properties.empty ())
		{
			ATOM_TiXmlElement props("properties");
			for (ATOM_HASHMAP<ATOM_STRING, PropertyInfo>::const_iterator it = _properties.begin(); it != _properties.end(); ++it)
			{
				ATOM_TiXmlElement prop("property");
				prop.SetAttribute ("name", it->first.c_str());
				ATOM_STRING type, value;
				getPropertyString (it->second, type, value);
				prop.SetAttribute ("type", type.c_str());
				prop.SetAttribute ("value", value.c_str());
				props.InsertEndChild (prop);
			}
			xml->InsertEndChild (props);
		}
#if 1
		ATOM_WidgetLayout *layout = _widget->getLayout ();
		for (unsigned i = 0; i < layout->getNumUnits(); ++i)
		{
			ATOM_Widget *child = layout->getUnit(i)->widget;
			ControlProxy *proxy = ControlProxy::getWidgetProxy (child);
			if (proxy)
			{
				ATOM_TiXmlElement childElement("");
				if (!proxy->saveToXML (&childElement))
				{
					return false;
				}
				xml->InsertEndChild (childElement);
			}
		}
#else
		for (ATOM_Widget *child = _widget->getFirstControl(); child; child = child->getNextSibling())
		{
			ControlProxy *proxy = ControlProxy::getWidgetProxy (child);
			if (proxy)
			{
				ATOM_TiXmlElement childElement("");
				if (!proxy->saveToXML (&childElement))
				{
					return false;
				}
				xml->InsertEndChild (childElement);
			}
		}

		for (ATOM_Widget *child = _widget->getFirstChild(); child; child = child->getNextSibling())
		{
			ControlProxy *proxy = ControlProxy::getWidgetProxy (child);
			if (proxy)
			{
				ATOM_TiXmlElement childElement("");
				if (!proxy->saveToXML (&childElement))
				{
					return false;
				}
				xml->InsertEndChild (childElement);
			}
		}
#endif

		return true;
	}

	return false;
}

ATOM_Widget *ControlProxy::getWidget (void) const
{
	return _widget;
}

ControlProxy *ControlProxy::getWidgetProxy (ATOM_Widget *widget)
{
	ATOM_HashMap<ATOM_Widget*, ControlProxy*>::const_iterator it = _widgetProxyMap.find (widget);
	return it == _widgetProxyMap.end () ? 0 : it->second;
}

void ControlProxy::clearProxyMap (void)
{
	_widgetProxyMap.clear ();
}

void ControlProxy::setName (const char *name)
{
	_name = name ? name : "";

	if (_widget)
	{
		_widget->setWidgetName (_name.c_str());
	}
}

const char *ControlProxy::getName (void) const
{
	return _name.c_str ();
}

void ControlProxy::changeFont (const char *fontName)
{
	if (!fontName || _widgetParams->font() == fontName)
	{
		_widgetParams->apply (_widget);
	}

	changeFontImpl (fontName, fontName ? getFontHandle(fontName) : ATOM_GUIFont::invalid_handle);
}

ATOM_GUIFont::handle ControlProxy::getFontHandle (const char *fontName) const
{
	PluginGUI::FontInfoMap::const_iterator it = _plugin->getFontInfoMap()->find (fontName);
	if (it != _plugin->getFontInfoMap()->end ())
	{
		return it->second.handle;
	}
	return ATOM_GUIFont::invalid_handle;
}

void ControlProxy::deleteParametersImpl (WidgetParameters *parameters)
{
	ATOM_DELETE(parameters);
}

void ControlProxy::handleTWCommandEvent (ATOMX_TWCommandEvent *event)
{
	switch (event->id)
	{
	case ID_NEW_PROPERTY:
		{
			AS_PropertyInfo info;
			if (this->_plugin->getEditor()->showNewPropertyDialog (&info))
			{
				const ATOM_STRING &name = info.name;
				AS_PropertyType propertyType = info.type;
				if (_properties.find (name) != _properties.end())
				{
					::MessageBoxA (ATOM_APP->getMainWindow(), "该属性已经存在", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
				}
				else
				{
					_properties[name].type = propertyType;

					switch (propertyType)
					{
					case AS_PROP_TYPE_INT:
						_properties[name].value.setI (0);
						break;
					case AS_PROP_TYPE_FLOAT:
						_properties[name].value.setF (0.f);
						break;
					case AS_PROP_TYPE_STRING:
						_properties[name].value.setS ("");
						break;
					case AS_PROP_TYPE_VECTOR:
						_properties[name].value.setV (ATOM_Vector4f(0.f, 0.f, 0.f, 0.f));
						break;
					case AS_PROP_TYPE_COLOR4F:
						_properties[name].value.setV (ATOM_Vector4f(1.f, 1.f, 1.f, 1.f));
						break;
					case AS_PROP_TYPE_SWITCH:
						_properties[name].value.setI (1);
						break;
					}

					ATOM_HASHMAP<ATOM_STRING, PropertyInfo>::const_iterator it = _properties.find(name);
					if (it != _properties.end ())
					{	
						setupProperty (event->bar, it->first.c_str(), it->second);
					}
				}
			}
			break;
		}

	case ID_CLEAR_PROPERTIES:
		{
			_properties.clear ();
			setupPropertyBar (event->bar);
			break;
		}

	default:
		if (_widgetParams)
		{
			if (_widgetParams->handleBarCommandEvent(event))
			{
				if (_widget)
				{
					_widgetParams->apply (_widget);
				}
			}
		}
	}
}

void ControlProxy::handleTWValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case ID_PROPERTY_VALUE:
		{
			const char *name = (const char *)event->userdata;
			PropertyInfo &info = _properties[name];
			switch (info.type)
			{
			case AS_PROP_TYPE_INT:
				{
					info.value.setI (event->newValue.getI());
					break;
				}
			case AS_PROP_TYPE_FLOAT:
				{
					info.value.setF (event->newValue.getF());
					break;
				}
			case AS_PROP_TYPE_STRING:
				{
					info.value.setS (event->newValue.getS());
					break;
				}
			case AS_PROP_TYPE_VECTOR:
			case AS_PROP_TYPE_COLOR4F:
				{
					const float *v = event->newValue.get4F ();
					info.value.setV (ATOM_Vector4f(v[0], v[1], v[2], v[3]));
					break;
				}
			case AS_PROP_TYPE_SWITCH:
				{
					info.value.setI (event->newValue.getB() ? 1 : 0);
					break;
				}
			}
			break;
		}

	default:
		if (_widgetParams)
		{
			if (_widgetParams->handleBarValueChangedEvent (event))
			{
				if (_widget)
				{
					_widgetParams->apply (_widget);
				}
			}
		}
	}
}

void ControlProxy::resize (const ATOM_Rect2Di &rc)
{
	if (_widgetParams)
	{
		_widgetParams->rect() = rc;
		_widget->resize (rc);
		//_widgetParams->apply(_widget);
		_plugin->getWidgetPropertyBar ()->clear ();
		_widgetParams->setupTweakBar (_plugin->getWidgetPropertyBar ());
	}
}

PluginGUI *ControlProxy::getPlugin (void) const
{
	return _plugin;
}

WidgetParameters *ControlProxy::getWidgetParams (void) const
{
	return _widgetParams;
}

void ControlProxy::setTreeItem (ATOM_TreeItem *treeItem)
{
	_treeItem = treeItem;;
}

ATOM_TreeItem *ControlProxy::getTreeItem (void) const
{
	return _treeItem;
}

ATOM_HASHMAP<ATOM_STRING, ControlProxy::PropertyInfo> & ControlProxy::getProperties (void)
{
	return _properties;
}

const ATOM_HASHMAP<ATOM_STRING, ControlProxy::PropertyInfo> & ControlProxy::getProperties (void) const
{
	return _properties;
}

void ControlProxy::setupPropertyBar (ATOMX_TweakBar *bar)
{
	bar->clear ();
	bar->addButton ("NewProp", ID_NEW_PROPERTY, "新建..", "自定义属性");
	bar->addButton ("ClearProp", ID_CLEAR_PROPERTIES, "清除", "自定义属性");

	for (ATOM_HASHMAP<ATOM_STRING, ControlProxy::PropertyInfo>::const_iterator it = _properties.begin(); it != _properties.end(); ++it)
	{
		setupProperty (bar, it->first.c_str(), it->second);
	}
}

void ControlProxy::setupProperty (ATOMX_TweakBar *bar, const char *name, const ControlProxy::PropertyInfo &info)
{
	switch (info.type)
	{
	case AS_PROP_TYPE_INT:
		setupIntProperty (bar, name, info.value);
		break;
	case AS_PROP_TYPE_FLOAT:
		setupFloatProperty (bar, name, info.value);
		break;
	case AS_PROP_TYPE_STRING:
		setupStringProperty (bar, name, info.value);
		break;
	case AS_PROP_TYPE_VECTOR:
		setupVectorProperty (bar, name, info.value);
		break;
	case AS_PROP_TYPE_COLOR4F:
		setupColorProperty (bar, name, info.value);
		break;
	case AS_PROP_TYPE_SWITCH:
		setupSwitchProperty (bar, name, info.value);
		break;
	default:
		return;
	}
	char buttonDelete[256];
	sprintf (buttonDelete, "PropDel_%s", name);
	bar->addButton (buttonDelete, ID_PROPERTY_DELETE, "删除", name, (void*)name);
}

void ControlProxy::setupIntProperty (ATOMX_TweakBar *bar, const char *name, const ATOM_Variant &value)
{
	char ValueName[256];
	sprintf (ValueName, "PropValue_%s", name);

	ATOMX_TBValue val;
	val.setI (ATOMX_TBTYPE_INT32, value.getI());

	bar->addVariable (ValueName, ID_PROPERTY_VALUE, val, false, name, (void*)name);
	bar->setVarLabel (ValueName, "Value");
}

void ControlProxy::setupFloatProperty (ATOMX_TweakBar *bar, const char *name, const ATOM_Variant &value)
{
	char ValueName[256];
	sprintf (ValueName, "PropValue_%s", name);

	ATOMX_TBValue val;
	val.setF (ATOMX_TBTYPE_FLOAT, value.getF());

	bar->addVariable (ValueName, ID_PROPERTY_VALUE, val, false, name, (void*)name);
	bar->setVarLabel (ValueName, "Value");
	bar->setVarStep (ValueName, 0.001f);
}

void ControlProxy::setupStringProperty (ATOMX_TweakBar *bar, const char *name, const ATOM_Variant &value)
{
	char ValueName[256];
	sprintf (ValueName, "PropValue_%s", name);

	ATOMX_TBValue val;
	val.setS (ATOMX_TBTYPE_STRING, value.getS());

	bar->addVariable (ValueName, ID_PROPERTY_VALUE, val, false, name, (void*)name);
	bar->setVarLabel (ValueName, "Value");
}

void ControlProxy::setupVectorProperty (ATOMX_TweakBar *bar, const char *name, const ATOM_Variant &value)
{
	char ValueName[256];
	sprintf (ValueName, "PropValue_%s", name);

	ATOMX_TBValue val;
	const float *f = value.getV ();
	val.set4F (ATOMX_TBTYPE_VECTOR4F, f[0], f[1], f[2], f[3]);

	bar->addVariable (ValueName, ID_PROPERTY_VALUE, val, false, name, (void*)name);
	bar->setVarLabel (ValueName, "Value");
}

void ControlProxy::setupColorProperty (ATOMX_TweakBar *bar, const char *name, const ATOM_Variant &value)
{
	char ValueName[256];
	sprintf (ValueName, "PropValue_%s", name);

	ATOMX_TBValue val;
	const float *v = value.getV ();
	val.set4F (ATOMX_TBTYPE_COLOR4F, v[0], v[1], v[2], v[3]);

	bar->addVariable (ValueName, ID_PROPERTY_VALUE, val, false, name, (void*)name);
	bar->setVarLabel (ValueName, "Value");
}

void ControlProxy::setupSwitchProperty (ATOMX_TweakBar *bar, const char *name, const ATOM_Variant &value)
{
	char ValueName[256];
	sprintf (ValueName, "PropValue_%s", name);

	ATOMX_TBValue val;
	val.setB (ATOMX_TBTYPE_BOOL, value.getI() != 0);

	bar->addVariable (ValueName, ID_PROPERTY_VALUE, val, false, name, (void*)name);
	bar->setVarLabel (ValueName, "Value");
}

#define ID_SELECT_PROP_TYPE_OK		(PLUGIN_ID_START + 1200)
#define ID_SELECT_PROP_TYPE_CANCEL	(PLUGIN_ID_START + 1201)
#define ID_LIST_ALL_PROP_TYPES		(PLUGIN_ID_START + 1202)
#define ID_EDIT_PROP_NAME			(PLUGIN_ID_START + 1203)

class PropTypeSelectDialogEventTrigger2: public ATOM_EventTrigger
{
public:
	PropTypeSelectDialogEventTrigger2 (void)
	{
	}

	void onCommand (ATOM_WidgetCommandEvent *event)
	{
		ATOM_Dialog *dialog = (ATOM_Dialog*)getHost();

		switch (event->id)
		{
		case ID_SELECT_PROP_TYPE_OK:
			{
				ATOM_ListBox *listBoxType = (ATOM_ListBox*)dialog->getChildById (ID_LIST_ALL_PROP_TYPES);
				ATOM_Edit *editName = (ATOM_Edit*)dialog->getChildById (ID_EDIT_PROP_NAME);
				ATOM_STRING s;
				editName->getString (s);
				if (listBoxType->getSelectIndex() >= 0 && !s.empty())
				{
					dialog->endModal (listBoxType->getSelectIndex());
				}
				break;
			}
		case ID_SELECT_PROP_TYPE_CANCEL:
		case ATOM_Widget::CloseButtonId:
			{
				dialog->endModal (-1);
				break;
			}
		}
	}

	ATOM_DECLARE_EVENT_MAP(PropTypeSelectDialogEventTrigger2, ATOM_EventTrigger)
};

ATOM_BEGIN_EVENT_MAP(PropTypeSelectDialogEventTrigger2, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(PropTypeSelectDialogEventTrigger2, ATOM_WidgetCommandEvent, onCommand)
	ATOM_END_EVENT_MAP

int ControlProxy::newProperty (ATOM_STRING &name)
{
	ATOM_Desktop *desktop = _plugin->getEditor()->getGUIRenderer()->getDesktop();
	ATOM_Rect2Di rc = desktop->getClientRect();
	int dlgWidth = 150;
	int dlgHeight = 260;
	int x = (rc.size.w - dlgWidth) / 2;
	int y = (rc.size.h - dlgHeight) / 2;

	ATOM_Dialog *typeDialog = ATOM_NEW(ATOM_Dialog, desktop, ATOM_Rect2Di(x, y, dlgWidth, dlgHeight));

	ATOM_Label *labelPropName = ATOM_NEW(ATOM_Label, typeDialog, ATOM_Rect2Di (20, 5, 110, 20), 0, 0, ATOM_Widget::ShowNormal);
	labelPropName->setText ("属性名称:");
	labelPropName->setAlign (ATOM_Widget::AlignX_Left|ATOM_Widget::AlignY_Middle);
	labelPropName->setFont (ATOM_GUIFont::getDefaultFont(12, 0));

	ATOM_Edit *nameEdit = ATOM_NEW(ATOM_Edit, typeDialog, ATOM_Rect2Di(20, 35, 110, 20), int(ATOM_Widget::Border), int(ID_EDIT_PROP_NAME), ATOM_Widget::ShowNormal);
	nameEdit->setBorderMode (ATOM_Widget::Drop);
	nameEdit->setFont (ATOM_GUIFont::getDefaultFont(12, 0));

	ATOM_Label *labelPropType = ATOM_NEW(ATOM_Label, typeDialog, ATOM_Rect2Di (20, 65, 110, 20), 0, 0, ATOM_Widget::ShowNormal);
	labelPropType->setText ("选择属性类型:");
	labelPropType->setAlign (ATOM_Widget::AlignX_Left|ATOM_Widget::AlignY_Middle);
	labelPropType->setFont (ATOM_GUIFont::getDefaultFont(12, 0));

	ATOM_ListBox *allTypesList = ATOM_NEW(ATOM_ListBox, typeDialog, ATOM_Rect2Di(20, 95, 110, 90), 16, int(ATOM_Widget::Border|ATOM_Widget::VScroll), int(ID_LIST_ALL_PROP_TYPES), ATOM_Widget::ShowNormal);
	allTypesList->setBorderMode (ATOM_Widget::Drop);
	allTypesList->setFont (ATOM_GUIFont::getDefaultFont(12, 0));
	allTypesList->setHoverImageId (ATOM_IMAGEID_LISTITEM);
	allTypesList->addItem ("整数");
	allTypesList->addItem ("浮点数");
	allTypesList->addItem ("字符串");
	allTypesList->addItem ("矢量");
	allTypesList->addItem ("颜色");
	allTypesList->addItem ("开关");
	allTypesList->selectItem (0);

	ATOM_Button *buttonOk = ATOM_NEW(ATOM_Button, typeDialog, ATOM_Rect2Di(20, 195, 40, 20), int(ATOM_Widget::Border), int(ID_SELECT_PROP_TYPE_OK), ATOM_Widget::ShowNormal);
	buttonOk->setText ("确定");
	buttonOk->setFont (ATOM_GUIFont::getDefaultFont (12, 0));

	ATOM_Button *buttonCancel = ATOM_NEW(ATOM_Button, typeDialog, ATOM_Rect2Di(90, 195, 40, 20), int(ATOM_Widget::Border), int(ID_SELECT_PROP_TYPE_CANCEL), ATOM_Widget::ShowNormal);
	buttonCancel->setText ("取消");
	buttonCancel->setFont (ATOM_GUIFont::getDefaultFont (12, 0));

	PropTypeSelectDialogEventTrigger2 trigger;
	typeDialog->setEventTrigger (&trigger);
	int index = typeDialog->showModal ();
	nameEdit->getString (name);

	ATOM_DELETE(typeDialog);

	switch (index)
	{
	case 0:
		return AS_PROP_TYPE_INT;
	case 1:
		return AS_PROP_TYPE_FLOAT;
	case 2:
		return AS_PROP_TYPE_STRING;
	case 3:
		return AS_PROP_TYPE_VECTOR;
	case 4:
		return AS_PROP_TYPE_COLOR4F;
	case 5:
		return AS_PROP_TYPE_SWITCH;
	default:
		return AS_PROP_TYPE_NONE;
	}
}

ControlProxy *ControlProxy::clone (ControlProxy *parent, bool deepClone) const
{
	ATOM_VECTOR<ControlProxy*> children;
	if (deepClone)
	{
		for (unsigned i = 0; i < _widget->getNumChildren(); ++i)
		{
			ATOM_Widget *c = _widget->getChild (i);
			ControlProxy *p = ControlProxy::getWidgetProxy (c);
			if (p)
			{
				children.push_back (p);
			}
		}
	}

	ControlProxy *proxy = dup ();
	proxy->setName (proxy->generateName().c_str());
	_plugin->addProxyName (proxy->getName());

	proxy->_widgetParams = _widgetParams->clone ();
	proxy->_widgetParams->setId (proxy->_widgetParams->interactive() ? IdManager::peekId (100) : -1);
	proxy->_widgetParams->setProxy (proxy);
	proxy->_widget = proxy->createWidgetImpl ((parent ? parent->getWidget() : _widget->getParent()), proxy->_widgetParams);
	_widgetProxyMap[proxy->_widget] = proxy;

	for (unsigned i = 0; i < children.size(); ++i)
	{
		children[i]->clone (proxy, deepClone);
	}

	return proxy;
}

