#include "StdAfx.h"
#include "component_proxy.h"
#include "plugin_gui.h"

ComponentParameters::ComponentParameters (ControlProxy *proxy)
	: WidgetParameters(proxy)
{
}

void ComponentParameters::resetParameters (void)
{
	WidgetParameters::resetParameters ();

	_filename = "";
}

WidgetParameters *ComponentParameters::clone (void) const
{
	return ATOM_NEW(ComponentParameters, *this);
}

void ComponentParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	WidgetParameters::setupTweakBar (propBar);
}

void ComponentParameters::apply (ATOM_Widget *widget)
{
	widget->setId (getId ());
	widget->resize (rect ());
}

void ComponentParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::saveToXML (xml);

	if (!_filename.empty ())
	{
		xml->SetAttribute ("filename", _filename.c_str());
	}
}

const ComponentParameters::ChildInfo &ComponentParameters::getChildInfo (void) const
{
	return _childInfo;
}

void ComponentParameters::loadChildInfo (ATOM_TiXmlElement *element, ChildInfo *info, const char *parentName)
{
	const char *type = element->Attribute("type");
	if (type)
	{
		ATOM_WidgetType widgetType = ControlProxy::getTypeFromTypeString (type);
		if (widgetType != WT_UNKNOWN)
		{
			info->type = widgetType;
			info->id = -1;
			element->QueryIntAttribute ("id", &info->id);

			const char *name = element->Attribute ("name");
			if (name)
			{
				info->name = parentName;
				info->name += "_";
				info->name += name;

				for (ATOM_TiXmlElement *e = element->FirstChildElement ("component"); e; e = e->NextSiblingElement ("component"))
				{
					ChildInfo subinfo;
					loadChildInfo (e, &subinfo, info->name.c_str());
					if (subinfo.type != WT_UNKNOWN && !subinfo.name.empty())
					{
						info->children.push_back (subinfo);
					}
				}
			}
		}
	}
}

void ComponentParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		const char *filename = xml->Attribute ("filename");
		_filename = filename ? filename : "";
	}
	else
	{
		const char *filename = (const char*)getProxy()->getPlugin()->getCreateParameter();
		_filename = filename ? filename : "";
	}

	if (!_filename.empty())
	{
		ATOM_AutoFile f(_filename.c_str(), ATOM_VFS::read|ATOM_VFS::text);
		if (!f)
		{
			return;
		}

		unsigned size = f->size ();
		char *p = ATOM_NEW_ARRAY(char, size);
		size = f->read (p, size);
		p[size] = '\0';
		ATOM_TiXmlDocument doc;
		doc.Parse (p);
		ATOM_DELETE_ARRAY(p);
		if (doc.Error ())
		{
			return;
		}

		_childInfo.name = getProxy()->getName();
		_childInfo.id = getId();
		_childInfo.type = ControlProxy::getTypeFromTypeString (getProxy()->getTagName());

		ATOM_TiXmlElement *root = doc.RootElement ();
		if (root)
		{
			for (ATOM_TiXmlElement *e = root->FirstChildElement("component"); e; e = e->NextSiblingElement ("component"))
			{
				ChildInfo info;
				loadChildInfo (e, &info, _childInfo.name.c_str());
				if (info.type != WT_UNKNOWN && !info.name.empty())
				{
					_childInfo.children.push_back (info);
				}
			}
		}
	}
}

bool ComponentParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	return WidgetParameters::handleBarValueChangedEvent (event);
}

bool ComponentParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return WidgetParameters::handleBarCommandEvent (event);
}

const char *ComponentParameters::getFileName (void) const
{
	return _filename.c_str();
}

void ComponentParameters::setFileName (const char *filename)
{
	_filename = filename ? filename : "";
}

ComponentProxy::ComponentProxy (PluginGUI *plugin)
	: ControlProxy (plugin)
{
}

ATOM_Widget *ComponentProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_Widget *w = parent->loadHierarchy (((ComponentParameters*)parameters)->getFileName());
	if (w)
	{
		//parameters->rect().point.x = w->getLayoutUnit()->desired_x;
		//parameters->rect().point.y = w->getLayoutUnit()->desired_y;
		//parameters->rect().size.w = w->getLayoutUnit()->desired_w;
		//parameters->rect().size.h = w->getLayoutUnit()->desired_h;
		parameters->imageId() = w->getClientImageId();
		parameters->style() = w->getStyle();
		parameters->borderMode() = w->getBorderMode();
		parameters->dragMode() = w->getDragMode();
		parameters->enableDrag() = w->isDragEnabled()?1:0;
		parameters->enableDrop() = w->isDropEnabled()?1:0;
		parameters->fontColor() = w->getFontColor();
		parameters->fontOutLineColor() = w->getFontOutLineColor();
		parameters->layoutType() = w->getLayoutType();
		parameters->layoutInnerSpaceLeft() = w->getLayout()->getInnerSpaceLeft();
		parameters->layoutInnerSpaceTop() = w->getLayout()->getInnerSpaceTop();
		parameters->layoutInnerSpaceRight() = w->getLayout()->getInnerSpaceRight();
		parameters->layoutInnerSpaceBottom() = w->getLayout()->getInnerSpaceBottom();
		parameters->layoutGap() = w->getLayout()->getGap();

		parameters->apply (w);
	}
	return w;
}

void ComponentProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *ComponentProxy::createParametersImpl (ControlProxy *proxy) const
{
	ComponentParameters *params = ATOM_NEW(ComponentParameters, proxy);
	return params;
}

