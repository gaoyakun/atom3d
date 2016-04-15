#include "StdAfx.h"
#include "widget_io.h"

ATOM_WidgetParameters::ATOM_WidgetParameters (void)
{
}

ATOM_WidgetParameters::~ATOM_WidgetParameters (void)
{
}

void ATOM_WidgetParameters::resetParameters (void)
{
	_font = "";
	_fontColor.setFloats(0.f, 0.f, 0.f, 1.f);
	_fontOutLineColor.setFloats(0.f, 0.f, 0.f, 0.f);
	_id = ATOM_Widget::AnyId;
	_imageId = ATOM_IMAGEID_CONTROL_BKGROUND;
	_borderMode = -1;
	_dragMode = ATOM_Widget::TitleBarDrag;
	_layoutGap = 4;
	_layoutSpaces[0] = 4;
	_layoutSpaces[1] = 4;
	_layoutSpaces[2] = 4;
	_layoutSpaces[3] = 4;
	_layoutType = ATOM_WidgetLayout::Basic;
	_rect.point.x = 0;
	_rect.point.y = 0;
	_rect.size.w = 100;
	_rect.size.h = 100;
	_style = ATOM_Widget::Control;
	_dragable = false;
	_dropable = false;
	_disabled = false;
	_imagelist.clear ();

	_scrollBarBackgroundImageId = ATOM_INVALID_IMAGEID;
	_scrollBarDownButtonImageId = ATOM_INVALID_IMAGEID;
	_scrollBarSliderImageId  = ATOM_INVALID_IMAGEID;
	_scrollBarUpButtonImageId = ATOM_INVALID_IMAGEID;

	// wangjian added
	_waitingCheck = 1;
}

void ATOM_WidgetParameters::apply (ATOM_Widget *widget)
{
	widget->setId (_id);
	widget->setWidgetName (_name.c_str());
	widget->setClientImageId (_imageId);
	widget->setStyle (_style|widget->getStyle());
	if (_borderMode != -1)
	{
		widget->setBorderMode ((ATOM_Widget::BorderMode)_borderMode);
	}
	widget->setDragMode (_dragMode);
	widget->setLayoutType (_layoutType);
	widget->getLayout()->setInnerSpaceLeft (_layoutSpaces[0]);
	widget->getLayout()->setInnerSpaceTop (_layoutSpaces[1]);
	widget->getLayout()->setInnerSpaceRight (_layoutSpaces[2]);
	widget->getLayout()->setInnerSpaceBottom (_layoutSpaces[3]);
	widget->getLayout()->setGap (_layoutGap);
	widget->setFont (ATOM_GUIFont::lookUpFont (_font.c_str()));
	widget->setFontColor (_fontColor);
	widget->setFontOutLineColor (_fontOutLineColor);
	widget->enableDrag (_dragable);
	widget->enableDrop (_dropable);
	widget->enable (!_disabled);
	widget->resize (_rect);

	//--------------------------------------------------------------------------------//
	// wangjian added
	int checkWaiting = _waitingCheck;
	// 如果父WIDGET是不需要等待加载的，则当前WIDGET也不需要等待
	if(widget->getParent() && false == widget->getParent()->isCheckWaitingEnabled() )
	{
		checkWaiting = false;
	}
	widget->enableCheckWaiting(_waitingCheck);
	//--------------------------------------------------------------------------------//

	if (!_properties.empty ())
	{
		for (ATOM_HASHMAP<ATOM_STRING, ATOM_Variant>::const_iterator it = _properties.begin(); it != _properties.end(); ++it)
		{
			widget->getAttributes()->setAttribute (it->first.c_str(), it->second);
		}
	}

	ATOM_AUTOPTR(ATOM_GUIImageList) il = widget->getImageList ();
	if (!il && !_imagelist.empty())
	{
		il = ATOM_NEW(ATOM_GUIImageList);
		widget->setImageList (il.get());

		for (ATOM_MAP<int,ImageInfo>::const_iterator it = _imagelist.begin(); it != _imagelist.end(); ++it)
		{
			int iid = it->first;
			const ImageInfo &info = it->second;
			ATOM_GUIImage * image = 0;
			switch (info.imageType)
			{
			case ATOM_GUIImage::IT_COLOR:
				image = il->newColorImage (iid, info.states[WST_NORMAL].color, info.states[WST_HOVER].color, info.states[WST_HOLD].color, info.states[WST_DISABLED].color);
				break;
			case ATOM_GUIImage::IT_IMAGE:
				image = il->newTextureImage (iid, info.states[WST_NORMAL].filename.c_str(), info.states[WST_HOVER].filename.c_str(), info.states[WST_HOLD].filename.c_str(), info.states[WST_DISABLED].filename.c_str(), info.states[WST_NORMAL].region);
				break;
			case ATOM_GUIImage::IT_IMAGE9:
				image = il->newTextureImage9 (iid, info.states[WST_NORMAL].filename.c_str(), info.states[WST_HOVER].filename.c_str(), info.states[WST_HOLD].filename.c_str(), info.states[WST_DISABLED].filename.c_str(), info.states[WST_NORMAL].region);
				break;
			case ATOM_GUIImage::IT_MATERIAL:
				image = il->newMaterialImage (iid, info.states[WST_NORMAL].filename.c_str(), info.states[WST_HOVER].filename.c_str(), info.states[WST_HOLD].filename.c_str(), info.states[WST_DISABLED].filename.c_str()
					, info.states[WST_NORMAL].material_filename.c_str(), info.states[WST_HOVER].material_filename.c_str(), info.states[WST_HOLD].filename.c_str(), info.states[WST_DISABLED].material_filename.c_str(), info.states[WST_NORMAL].region);
				break;
			}

			//--- wangjian added ---//
			// 设置图片异步加载优先级
			if( image )
			{
				int asyncloadFlag = info.asycnLoadFlag;
				// 检查一下标记 如果还未设置 则设置为WIDGET的优先级标记
				if( asyncloadFlag == ImageInfo::LOAD_NOT_SPECIFIED )
					asyncloadFlag = widget->getAsyncLoad();
				image->setAsyncLoad(asyncloadFlag);
			}
			//----------------------//
		}
	}
}

static void getPropertyString (const ATOM_Variant &propValue, ATOM_STRING &type, ATOM_STRING &value)
{
	char buffer[1024];

	switch (propValue.getType())
	{
	case ATOM_Variant::INT:
		type = "int";
		sprintf (buffer, "%d", propValue.getI());
		break;
	case ATOM_Variant::FLOAT:
		type = "float";
		sprintf (buffer, "%f", propValue.getF());
		break;
	case ATOM_Variant::STRING:
		type = "string";
		strcpy (buffer, propValue.getS());
		break;
	case ATOM_Variant::VECTOR4:
		type = "vec";
		sprintf (buffer, "%f,%f,%f,%f", propValue.getV()[0], propValue.getV()[1], propValue.getV()[2], propValue.getV()[3]);
		break;
	default:
		return;
	}

	value = buffer;
}

void ATOM_WidgetParameters::saveToXML (ATOM_TiXmlElement *xml) const
{
	char buffer[512];

	sprintf (buffer, "%d,%d,%d,%d", _rect.point.x, _rect.point.y, _rect.size.w, _rect.size.h);
	xml->SetAttribute ("rect", buffer);

	xml->SetAttribute ("id", _id);
	xml->SetAttribute ("imageid", _imageId);
	xml->SetAttribute ("style", _style);
	xml->SetAttribute ("border", _borderMode);
	xml->SetAttribute ("dragmode", _dragMode);
	xml->SetAttribute ("name", _name.c_str());
	xml->SetAttribute ("dragable", _dragable);
	xml->SetAttribute ("dropable", _dropable);
	xml->SetAttribute ("disabled", _disabled);

	switch (_layoutType)
	{
	case ATOM_WidgetLayout::Vertical:
		xml->SetAttribute ("layout", "vertical");
		xml->SetAttribute ("layout_gap", _layoutGap);
		break;
	case ATOM_WidgetLayout::Horizontal:
		xml->SetAttribute ("layout", "horizontal");
		xml->SetAttribute ("layout_gap", _layoutGap);
		break;
	default:
		xml->SetAttribute ("layout", "basic");
		break;
	}

	sprintf (buffer, "%d,%d,%d,%d", _layoutSpaces[0], _layoutSpaces[1], _layoutSpaces[2], _layoutSpaces[3]);
	xml->SetAttribute ("layout_spc", buffer);

	if (!_font.empty ())
	{
		xml->SetAttribute ("font", _font.c_str());
	}

	if (_fontColor.getRaw() != 0)
	{
		xml->SetAttribute ("fontcolor", int(_fontColor));
	}

	if (_fontOutLineColor.getRaw() != 0)
	{
		xml->SetAttribute ("outlinecolor", int(_fontOutLineColor));
	}

	// wangjian added:只有不需等待 才保存[默认为需要等待]
	if( _waitingCheck != 1 )
	{
		xml->SetAttribute ("waitingCheck", _waitingCheck);
	}

	if (!_imagelist.empty ())
	{
		ATOM_TiXmlElement imagelist("imagelist");
		for (ATOM_MAP<int,ImageInfo>::const_iterator it = _imagelist.begin(); it != _imagelist.end(); ++it)
		{
			ATOM_TiXmlElement image("image");
			image.SetAttribute ("id", it->first);

			const ImageInfo &info = it->second;
			int type = info.imageType;
			image.SetAttribute ("type", type);

			switch (type)
			{
			case ATOM_GUIImage::IT_COLOR:
				image.SetAttribute ("normal", int(info.states[WST_NORMAL].color));
				image.SetAttribute ("hover", int(info.states[WST_HOVER].color));
				image.SetAttribute ("hold", int(info.states[WST_HOLD].color));
				image.SetAttribute ("disabled", int(info.states[WST_DISABLED].color));
				break;
			case ATOM_GUIImage::IT_IMAGE:
			case ATOM_GUIImage::IT_IMAGE9:
				image.SetAttribute ("normal", info.states[WST_NORMAL].filename.c_str());
				image.SetAttribute ("hover", info.states[WST_HOVER].filename.c_str());
				image.SetAttribute ("hold", info.states[WST_HOLD].filename.c_str());
				image.SetAttribute ("disabled", info.states[WST_DISABLED].filename.c_str());
				sprintf (buffer, "%d,%d,%d,%d", info.states[WST_NORMAL].region.point.x, info.states[WST_NORMAL].region.point.y, info.states[WST_NORMAL].region.size.w, info.states[WST_NORMAL].region.size.h);
				image.SetAttribute ("region", buffer);
				break;
			case ATOM_GUIImage::IT_MATERIAL:
				image.SetAttribute ("normal", info.states[WST_NORMAL].filename.c_str());
				image.SetAttribute ("hover", info.states[WST_HOVER].filename.c_str());
				image.SetAttribute ("hold", info.states[WST_HOLD].filename.c_str());
				image.SetAttribute ("disabled", info.states[WST_DISABLED].filename.c_str());
				image.SetAttribute ("mat_normal", info.states[WST_NORMAL].material_filename.c_str());
				image.SetAttribute ("mat_hover", info.states[WST_HOVER].material_filename.c_str());
				image.SetAttribute ("mat_hold", info.states[WST_HOLD].material_filename.c_str());
				image.SetAttribute ("mat_disabled", info.states[WST_DISABLED].material_filename.c_str());
				sprintf (buffer, "%d,%d,%d,%d", info.states[WST_NORMAL].region.point.x, info.states[WST_NORMAL].region.point.y, info.states[WST_NORMAL].region.size.w, info.states[WST_NORMAL].region.size.h);
				image.SetAttribute ("region", buffer);
				break;
			}

			//--- wangjian added ---//
			// 异步加载 保存UI时保存异步加载的标记
			int loadpriority = info.asycnLoadFlag;
			if( loadpriority != ImageInfo::LOAD_NOT_SPECIFIED )	// 如果给定了优先级
				image.SetAttribute ("asyncload", loadpriority);
			//----------------------//

			imagelist.InsertEndChild (image);
		}
		xml->InsertEndChild (imagelist);
	}

	if (!_properties.empty ())
	{
		ATOM_TiXmlElement props("properties");
		for (ATOM_HASHMAP<ATOM_STRING, ATOM_Variant>::const_iterator it = _properties.begin(); it != _properties.end(); ++it)
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
}

static void getPropertyInfo (ATOM_TiXmlElement *xml, ATOM_Variant &var, const ATOM_STRING &type, const ATOM_STRING &value)
{
	if (type == "int")
	{
		int n = 0;
		if (!value.empty ())
		{
			n = atoi (value.c_str());
		}
		var.setI (n);
	}
	else if (type == "float")
	{
		float f = 0;
		if (!value.empty ())
		{
			f = atof (value.c_str());
		}
		var.setF (f);
	}
	else if (type == "string")
	{
		var.setS (value.c_str());
	}
	else if (type == "vec" || type == "color")
	{
		ATOM_Vector4f v(0.f, 0.f, 0.f, 0.f);
		sscanf (value.c_str(), "%f,%f,%f,%f", &v.x, &v.y, &v.z, &v.w);
		var.setV (v);
	}
	else if (type == "bool")
	{
		var.setI (value=="true" ? 1 : 0);
	}
}

void ATOM_WidgetParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	resetParameters ();

	if (xml)
	{
		for (ATOM_TiXmlElement *eImageList = xml->FirstChildElement ("imagelist"); eImageList; eImageList = eImageList->NextSiblingElement("imagelist"))
		{
			for (ATOM_TiXmlElement *eImage = eImageList->FirstChildElement ("image"); eImage; eImage = eImage->NextSiblingElement("image"))
			{
				ImageInfo imageInfo;

				int id = 0;
				if (eImage->QueryIntAttribute ("id", &id) != ATOM_TIXML_SUCCESS)
				{
					continue;
				}
				int type = 0;
				if (eImage->QueryIntAttribute ("type", &type) != ATOM_TIXML_SUCCESS)
				{
					continue;
				}

				int color = 0;
				const char *texNormal = 0;
				const char *texHover = 0;
				const char *texHold = 0;
				const char *texDisabled = 0;
				const char *regionStr = 0;
				const char *matNormal = 0;
				const char *matHover = 0;
				const char *matHold = 0;
				const char *matDisabled = 0;
				ATOM_Rect2Di region (0,0,0,0);

				switch (type)
				{
				case ATOM_GUIImage::IT_COLOR:
					imageInfo.states[WST_NORMAL].color = eImage->Attribute("normal", &color) ? (unsigned)color : 0;
					imageInfo.states[WST_HOVER].color = eImage->Attribute("hover", &color) ? (unsigned)color : 0;
					imageInfo.states[WST_HOLD].color = eImage->Attribute("hold", &color) ? (unsigned)color : 0;
					imageInfo.states[WST_DISABLED].color = eImage->Attribute("disabled", &color) ? (unsigned)color : 0;
					imageInfo.imageType = ATOM_GUIImage::IT_COLOR;
					_imagelist[id] = imageInfo;
					break;
				case ATOM_GUIImage::IT_MATERIAL:
					regionStr = eImage->Attribute("region");
					if (regionStr)
					{
						sscanf (regionStr, "%d,%d,%d,%d", &region.point.x, &region.point.y, &region.size.w, &region.size.h);
					}
					imageInfo.states[WST_NORMAL].region = region;
					imageInfo.states[WST_HOVER].region = region;
					imageInfo.states[WST_HOLD].region = region;
					imageInfo.states[WST_DISABLED].region = region;
					texNormal = eImage->Attribute("normal");
					texHover = eImage->Attribute("hover");
					texHold = eImage->Attribute("hold");
					texDisabled = eImage->Attribute("disabled");
					matNormal = eImage->Attribute("mat_normal");
					matHover = eImage->Attribute("mat_hover");
					matHold = eImage->Attribute("mat_hold");
					matDisabled = eImage->Attribute("mat_disabled");
					imageInfo.imageType = (ATOM_GUIImage::Type)type;
					imageInfo.states[WST_NORMAL].filename = texNormal;
					imageInfo.states[WST_HOVER].filename = texHover;
					imageInfo.states[WST_HOLD].filename = texHold;
					imageInfo.states[WST_DISABLED].filename = texDisabled;
					imageInfo.states[WST_NORMAL].material_filename = matNormal;
					imageInfo.states[WST_HOVER].material_filename = matHover;
					imageInfo.states[WST_HOLD].material_filename = matHold;
					imageInfo.states[WST_DISABLED].material_filename = matDisabled;
					imageInfo.states[WST_NORMAL].region = region;
					imageInfo.states[WST_HOVER].region = region;
					imageInfo.states[WST_HOLD].region = region;
					imageInfo.states[WST_DISABLED].region = region;
					_imagelist[id] = imageInfo;
					break;
				case ATOM_GUIImage::IT_IMAGE:
				case ATOM_GUIImage::IT_IMAGE9:
					texNormal = eImage->Attribute("normal");
					texNormal = texNormal ? texNormal : "";
					texHover = eImage->Attribute("hover");
					texHover = texHover ? texHover : texNormal;
					texHold = eImage->Attribute("hold");
					texHold = texHold ? texHold : texHover;
					texDisabled = eImage->Attribute("disabled");
					texDisabled = texDisabled ? texDisabled : texNormal;
					regionStr = eImage->Attribute("region");
					if (regionStr)
					{
						sscanf (regionStr, "%d,%d,%d,%d", &region.point.x, &region.point.y, &region.size.w, &region.size.h);
					}
					imageInfo.imageType = (ATOM_GUIImage::Type)type;
					imageInfo.states[WST_NORMAL].filename = texNormal;
					imageInfo.states[WST_NORMAL].region = region;
					imageInfo.states[WST_HOVER].filename = texHover;
					imageInfo.states[WST_HOVER].region = region;
					imageInfo.states[WST_HOLD].filename = texHold;
					imageInfo.states[WST_HOLD].region = region;
					imageInfo.states[WST_DISABLED].filename = texDisabled;
					imageInfo.states[WST_DISABLED].region = region;
					_imagelist[id] = imageInfo;
					break;
				default:
					break;
				}

				//--- wangjian added ---//
				// 读取图片的异步加载标记
				int asyncload;
				_imagelist[id].asycnLoadFlag = eImage->Attribute("asyncload", &asyncload) ? asyncload : ImageInfo::LOAD_NOT_SPECIFIED;	// 如果没有设置 则设置为-1
				//----------------------//
			}
		}

		const char *rc = xml->Attribute ("rect");
		if (rc)
		{
			int x, y, w, h;
			if (4 == sscanf (rc, "%d,%d,%d,%d", &x, &y, &w, &h))
			{
				_rect.point.x = x;
				_rect.point.y = y;
				_rect.size.w = w;
				_rect.size.h = h;
			}
		}

		xml->QueryIntAttribute ("id", &_id);
		xml->QueryIntAttribute ("imageid", &_imageId);
		xml->QueryIntAttribute ("style", &_style);
		xml->QueryIntAttribute ("border", &_borderMode);
		xml->QueryIntAttribute ("dragmode", &_dragMode);
		xml->QueryIntAttribute ("dragable", &_dragable);
		xml->QueryIntAttribute ("dropable", &_dropable);
		xml->QueryIntAttribute ("disabled", &_disabled);

		const char *name = xml->Attribute ("name");
		_name = name ? name : "";

		const char *layoutType = xml->Attribute ("layout");
		if (layoutType)
		{
			if (!stricmp (layoutType, "basic"))
			{
				_layoutType = ATOM_WidgetLayout::Basic;
			}
			else if (!stricmp (layoutType, "vertical"))
			{
				_layoutType = ATOM_WidgetLayout::Vertical;
			}
			else if (!stricmp (layoutType, "horizontal"))
			{
				_layoutType = ATOM_WidgetLayout::Horizontal;
			}
			else
			{
				_layoutType = ATOM_WidgetLayout::Basic;
			}
		}

		const char *layoutSpaces = xml->Attribute ("layout_spc");
		if (layoutSpaces)
		{
			int l, t, r, b;
			if (4 == sscanf (layoutSpaces, "%d,%d,%d,%d", &l, &t, &r, &b))
			{
				_layoutSpaces[0] = l;
				_layoutSpaces[1] = t;
				_layoutSpaces[2] = r;
				_layoutSpaces[3] = b;
			}
		}

		if (_layoutType == ATOM_WidgetLayout::Vertical || _layoutType == ATOM_WidgetLayout::Horizontal)
		{
			xml->QueryIntAttribute ("layout_gap", &_layoutGap);
		}

		const char *fontName = xml->Attribute ("font");
		if (fontName)
		{
			_font = fontName;
		}

		int color = 0;
		xml->QueryIntAttribute ("fontcolor", &color);
		_fontColor = color;

		int outlinecolor = 0;
		xml->QueryIntAttribute ("outlinecolor", &outlinecolor);
		_fontOutLineColor = outlinecolor;

		// wangjian added
		int waitingCheck = 1;
		xml->QueryIntAttribute ("waitingCheck", &waitingCheck);
		_waitingCheck = waitingCheck;

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
	}
}

const ATOM_Rect2Di &ATOM_WidgetParameters::rect (void) const
{
	return _rect;
}

ATOM_Rect2Di &ATOM_WidgetParameters::rect (void)
{
	return _rect;
}

int ATOM_WidgetParameters::dragable (void) const
{
	return _dragable;
}

int &ATOM_WidgetParameters::dragable (void)
{
	return _dragable;
}

int ATOM_WidgetParameters::dropable (void) const
{
	return _dropable;
}

int &ATOM_WidgetParameters::dropable (void)
{
	return _dropable;
}

int ATOM_WidgetParameters::disabled (void) const
{
	return _disabled;
}

int &ATOM_WidgetParameters::disabled (void)
{
	return _disabled;
}

int ATOM_WidgetParameters::id (void) const
{
	return _id;
}

int &ATOM_WidgetParameters::id (void)
{
	return _id;
}

int ATOM_WidgetParameters::imageId (void) const
{
	return _imageId;
}

int &ATOM_WidgetParameters::imageId (void)
{
	return _imageId;
}

int ATOM_WidgetParameters::borderMode (void) const
{
	return _borderMode;
}

int &ATOM_WidgetParameters::borderMode (void)
{
	return _borderMode;
}

int ATOM_WidgetParameters::style (void) const
{
	return _style;
}

int &ATOM_WidgetParameters::style (void)
{
	return _style;
}

ATOM_WidgetLayout::Type ATOM_WidgetParameters::layoutType (void) const
{
	return _layoutType;
}

ATOM_WidgetLayout::Type &ATOM_WidgetParameters::layoutType (void)
{
	return _layoutType;
}

int ATOM_WidgetParameters::layoutInnerSpaceLeft (void) const
{
	return _layoutSpaces[0];
}

int &ATOM_WidgetParameters::layoutInnerSpaceLeft (void)
{
	return _layoutSpaces[1];
}

int ATOM_WidgetParameters::layoutInnerSpaceTop (void) const
{
	return _layoutSpaces[1];
}

int &ATOM_WidgetParameters::layoutInnerSpaceTop (void)
{
	return _layoutSpaces[1];
}

int ATOM_WidgetParameters::layoutInnerSpaceRight (void) const
{
	return _layoutSpaces[2];
}

int &ATOM_WidgetParameters::layoutInnerSpaceRight (void)
{
	return _layoutSpaces[2];
}

int ATOM_WidgetParameters::layoutInnerSpaceBottom (void) const
{
	return _layoutSpaces[3];
}

int &ATOM_WidgetParameters::layoutInnerSpaceBottom (void)
{
	return _layoutSpaces[3];
}

int ATOM_WidgetParameters::layoutGap (void) const
{
	return _layoutGap;
}

int &ATOM_WidgetParameters::layoutGap (void)
{
	return _layoutGap;
}

ATOM_STRING ATOM_WidgetParameters::font (void) const
{
	return _font;
}

ATOM_STRING &ATOM_WidgetParameters::font (void)
{
	return _font;
}

ATOM_ColorARGB ATOM_WidgetParameters::fontColor (void) const
{
	return _fontColor;
}

ATOM_ColorARGB &ATOM_WidgetParameters::fontColor (void)
{
	return _fontColor;
}

ATOM_ColorARGB ATOM_WidgetParameters::fontOutLineColor (void) const 
{
	return _fontOutLineColor;
}

ATOM_ColorARGB &ATOM_WidgetParameters::fontOutLineColor (void)
{
	return _fontOutLineColor; 
}

const ATOM_MAP<int,ATOM_WidgetParameters::ImageInfo> &ATOM_WidgetParameters::imageList (void) const
{
	return _imagelist;
}

ATOM_MAP<int,ATOM_WidgetParameters::ImageInfo> &ATOM_WidgetParameters::imageList (void)
{
	return _imagelist;
}

// wangjian added
int ATOM_WidgetParameters::waitingCheck (void) const
{
	return _waitingCheck;
}
int & ATOM_WidgetParameters::waitingCheck (void)
{
	return _waitingCheck;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_Widget *ATOM_ScrollMapParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_ScrollMap *scrollmap = ATOM_NEW(ATOM_ScrollMap, parent, rect(), style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	scrollmap->setAsyncLoad(loadPriority);
	//---------------------//
	apply (scrollmap);
	return scrollmap;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_LabelParameters::ATOM_LabelParameters (void)
{
}

void ATOM_LabelParameters::resetParameters (void)
{
	ATOM_PanelParameters::resetParameters ();

	rect().size.w = 100;
	rect().size.h = 24;

	_alignmentX = 0;
	_alignmentY = 1;
	_textOffsetX = 0;
	_textOffsetY = 0;
	_color = 0;
	_underlineColor = 0;
	_rotation = 0.f;
	_text = "标签";
}

void ATOM_LabelParameters::apply (ATOM_Widget *widget)
{
	ATOM_PanelParameters::apply (widget);

	ATOM_Label *label = (ATOM_Label*)widget;
	int alignment = 0;
	switch (_alignmentX)
	{
	case 0: alignment |= ATOM_Widget::AlignX_Left; break;
	case 1: alignment |= ATOM_Widget::AlignX_Middle; break;
	case 2: alignment |= ATOM_Widget::AlignX_Right; break;
	}
	switch (_alignmentY)
	{
	case 0: alignment |= ATOM_Widget::AlignY_Top; break;
	case 1: alignment |= ATOM_Widget::AlignY_Middle; break;
	case 2: alignment |= ATOM_Widget::AlignY_Bottom; break;
	}
	label->setText (_text.c_str());
	label->setAlign (alignment);
	label->setOffset (ATOM_Point2Di(_textOffsetX, _textOffsetY));
	label->setColor (_color);
	label->setUnderlineColor (_underlineColor);
	label->setRotation (_rotation);
}

void ATOM_LabelParameters::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_PanelParameters::saveToXML (xml);

	if (!_text.empty ())
	{
		xml->SetAttribute ("text", _text.c_str());
	}
	xml->SetAttribute ("alignx", _alignmentX);
	xml->SetAttribute ("aligny", _alignmentY);
	xml->SetAttribute ("offsetx", _textOffsetX);
	xml->SetAttribute ("offsetY", _textOffsetY);
	xml->SetAttribute ("color", int(_color));
	xml->SetAttribute ("underlinecolor", int(_underlineColor));
	xml->SetDoubleAttribute ("rotation", _rotation);
}

void ATOM_LabelParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_PanelParameters::loadFromXML (xml);

	if (xml)
	{
		const char *text = xml->Attribute ("text");
		_text = text ? text : "";
		xml->QueryIntAttribute ("alignx", &_alignmentX);
		xml->QueryIntAttribute ("aligny", &_alignmentY);
		xml->QueryIntAttribute ("offsetx", &_textOffsetX);
		xml->QueryIntAttribute ("offsetY", &_textOffsetY);
		xml->QueryIntAttribute ("color", (int*)&_color);
		xml->QueryIntAttribute ("underlinecolor", (int*)&_underlineColor);
		xml->QueryFloatAttribute ("rotation", &_rotation);
	}
}

ATOM_Widget *ATOM_LabelParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_Label *label = ATOM_NEW(ATOM_Label, parent, rect(), style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	label->setAsyncLoad(loadPriority);
	//---------------------//
	apply (label);
	return label;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_ButtonParameters::ATOM_ButtonParameters (void)
{
}

void ATOM_ButtonParameters::resetParameters (void)
{
	ATOM_LabelParameters::resetParameters ();

	style() |= ATOM_Widget::Border;
	rect().size.w = 60;
	rect().size.h = 24;
	_alignmentX = 1;
	_clickSound = INVALID_AUDIOID;
	_hoverSound = INVALID_AUDIOID;
	_text = "确定";
}

void ATOM_ButtonParameters::apply (ATOM_Widget *widget)
{
	ATOM_LabelParameters::apply (widget);

	ATOM_Button *btn = (ATOM_Button*)widget;
	btn->setClickSound (_clickSound);
	btn->setHoverSound (_hoverSound);
}

void ATOM_ButtonParameters::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_LabelParameters::saveToXML (xml);

	if (_clickSound != INVALID_AUDIOID)
		xml->SetAttribute ("clicksound", _clickSound);

	if (_hoverSound != INVALID_AUDIOID)
		xml->SetAttribute ("hoversound", _hoverSound);
}

void ATOM_ButtonParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_LabelParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("clicksound", &_clickSound);
		xml->QueryIntAttribute ("hoversound", &_hoverSound);
	}
}

ATOM_Widget *ATOM_ButtonParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_Button *button = ATOM_NEW(ATOM_Button, parent, rect(), style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	button->setAsyncLoad(loadPriority);
	//---------------------//
	apply (button);
	return button;
}

///////////////////////////////////////////////////////////////////////////////////////////

ATOM_CellParameters::ATOM_CellParameters (void)
{
}

void ATOM_CellParameters::resetParameters (void)
{
	ATOM_WidgetParameters::resetParameters ();

	rect().size.w = 200;
	rect().size.h = 160;
	borderMode() = ATOM_Widget::Drop;
	style() |= ATOM_Widget::Border;
	imageId() = ATOM_IMAGEID_CONTROL_BKGROUND;

	_sizeX = 0;
	_sizeY = 0;
	_cellWidth = 0;
	_cellHeight = 0;
	_alignmentX = 1;
	_alignmentY = 1;
	_textOffsetX = 0;
	_textOffsetY = 0;
	_cooldownImage = -1;
	_selectedImage = -1;
	_frameImage = -1;
	_frameOffset = 0;
	_spaceX = 0;
	_spaceY = 0;
	_dragdrop = 0;
	_subscriptImageId = ATOM_INVALID_IMAGEID;
	_subscriptImageRect = ATOM_Rect2Di(0,0,0,0);
	_subscriptTextPosition = ATOM_Point2Di(0,0);
}

void ATOM_CellParameters::apply (ATOM_Widget *widget)
{
	ATOM_WidgetParameters::apply (widget);

	ATOM_Cell *cell = (ATOM_Cell*)widget;
	cell->setCellWidth (_cellWidth, _cellHeight);

	int alignment = 0;
	switch (_alignmentX)
	{
	case 0: alignment |= ATOM_Widget::AlignX_Left; break;
	case 1: alignment |= ATOM_Widget::AlignX_Middle; break;
	case 2: alignment |= ATOM_Widget::AlignX_Right; break;
	}
	switch (_alignmentY)
	{
	case 0: alignment |= ATOM_Widget::AlignY_Top; break;
	case 1: alignment |= ATOM_Widget::AlignY_Middle; break;
	case 2: alignment |= ATOM_Widget::AlignY_Bottom; break;
	}
	cell->setAlign (alignment);
	cell->setOffset (ATOM_Point2Di(_textOffsetX, _textOffsetY));
	cell->setCooldownImageId (_cooldownImage);
	cell->setSelectImageId (_selectedImage);
	cell->setFrameImageId (_frameImage);
	cell->setFrameOffset (_frameOffset);
	cell->setSpaceX (_spaceX);
	cell->setSpaceY (_spaceY);
	cell->setSubscriptTextPosition(_subscriptTextPosition);
	cell->setSubscriptImageRect(_subscriptImageRect);
	cell->setSubscriptImageId(_subscriptImageId);
	cell->setCellSize (_sizeX, _sizeY);
	cell->allowCellDragDrop (_dragdrop != 0);

}

void ATOM_CellParameters::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("sizex", _sizeX);
	xml->SetAttribute ("sizey", _sizeY);
	xml->SetAttribute ("cellw", _cellWidth);
	xml->SetAttribute ("cellh", _cellHeight);
	xml->SetAttribute ("alignx", _alignmentX);
	xml->SetAttribute ("aligny", _alignmentY);
	xml->SetAttribute ("offsetx", _textOffsetX);
	xml->SetAttribute ("offsetY", _textOffsetY);
	xml->SetAttribute ("cdimage", _cooldownImage);
	xml->SetAttribute ("selimage", _selectedImage);
	xml->SetAttribute ("frameimage", _frameImage);
	xml->SetAttribute ("frameoffset", _frameOffset);
	xml->SetAttribute ("spacex", _spaceX);
	xml->SetAttribute ("spacey", _spaceY);
	xml->SetAttribute ("celldragdrop", _dragdrop);
	xml->SetAttribute("subtxtOffsetx",_subscriptTextPosition.x);
	xml->SetAttribute("subtxtOffsety",_subscriptTextPosition.y);
	xml->SetAttribute("subimgoffsetx",_subscriptImageRect.point.x);
	xml->SetAttribute("subimgoffsety",_subscriptImageRect.point.y);
	xml->SetAttribute("subimgw",_subscriptImageRect.size.w);
	xml->SetAttribute("subimgh",_subscriptImageRect.size.h);
	xml->SetAttribute("subimg",_subscriptImageId);
}

void ATOM_CellParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("sizex", &_sizeX);
		xml->QueryIntAttribute ("sizey", &_sizeY);
		xml->QueryIntAttribute ("cellw", &_cellWidth);
		xml->QueryIntAttribute ("cellh", &_cellHeight);
		xml->QueryIntAttribute ("alignx", &_alignmentX);
		xml->QueryIntAttribute ("aligny", &_alignmentY);
		xml->QueryIntAttribute ("offsetx", &_textOffsetX);
		xml->QueryIntAttribute ("offsetY", &_textOffsetY);
		xml->QueryIntAttribute ("cdimage", &_cooldownImage);
		xml->QueryIntAttribute ("selimage", &_selectedImage);
		xml->QueryIntAttribute ("frameimage", &_frameImage);
		xml->QueryIntAttribute ("frameoffset", &_frameOffset);
		xml->QueryIntAttribute ("spacex", &_spaceX);
		xml->QueryIntAttribute ("spacey", &_spaceY);
		xml->QueryIntAttribute ("celldragdrop", &_dragdrop);
		xml->QueryIntAttribute("subtxtOffsetx", &_subscriptTextPosition.x);
		xml->QueryIntAttribute("subtxtOffsety", &_subscriptTextPosition.y);
		xml->QueryIntAttribute("subimgoffsetx", &_subscriptImageRect.point.x);
		xml->QueryIntAttribute("subimgoffsety", &_subscriptImageRect.point.y);
		xml->QueryIntAttribute("subimgw", &_subscriptImageRect.size.w);
		xml->QueryIntAttribute("subimgh", &_subscriptImageRect.size.h);
		xml->QueryIntAttribute("subimg", &_subscriptImageId);
	}
}

ATOM_Widget *ATOM_CellParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_Cell *cell = ATOM_NEW(ATOM_Cell, parent, rect(), style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	cell->setAsyncLoad(loadPriority);
	//---------------------//
	apply (cell);
	return cell;
}


////////////////////////////////////////////////////////////////////////////////////////

ATOM_CheckBoxParameters::ATOM_CheckBoxParameters (void)
{
}

void ATOM_CheckBoxParameters::resetParameters (void)
{
	ATOM_LabelParameters::resetParameters ();

	rect().size.w = 60;
	rect().size.h = 24;
	imageId() = ATOM_IMAGEID_CHECKBOX_UNCHECKED;

	_alignmentX = 1;
	_clickSound = INVALID_AUDIOID;
	_hoverSound = INVALID_AUDIOID;
	_text = "选项";
	_checked = 0;
	_checkedImageId = ATOM_IMAGEID_CHECKBOX_CHECKED;
}

void ATOM_CheckBoxParameters::apply (ATOM_Widget *widget)
{
	ATOM_LabelParameters::apply (widget);

	ATOM_Checkbox *cb = (ATOM_Checkbox*)widget;
	cb->setClickSound (_clickSound);
	cb->setHoverSound (_hoverSound);
	cb->setCheckedImageId (_checkedImageId);
	cb->setChecked (_checked != 0);
}

void ATOM_CheckBoxParameters::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_LabelParameters::saveToXML (xml);

	if (_clickSound != INVALID_AUDIOID)
		xml->SetAttribute ("clicksound", _clickSound);

	if (_hoverSound != INVALID_AUDIOID)
		xml->SetAttribute ("hoversound", _hoverSound);

	if (_checkedImageId != ATOM_IMAGEID_CHECKBOX_CHECKED)
		xml->SetAttribute ("checkedimage", _checkedImageId);

	xml->SetAttribute ("checked", _checked);

}

void ATOM_CheckBoxParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_LabelParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("clicksound", &_clickSound);
		xml->QueryIntAttribute ("hoversound", &_hoverSound);
		xml->QueryIntAttribute ("checkedimage", &_checkedImageId);
		xml->QueryIntAttribute ("checked", &_checked);
	}
}

ATOM_Widget *ATOM_CheckBoxParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_Checkbox *cb = ATOM_NEW(ATOM_Checkbox, parent, rect(), style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	cb->setAsyncLoad(loadPriority);
	//---------------------//
	apply (cb);
	return cb;
}

///////////////////////////////////////////////////////////////////////////////////////////

ATOM_DialogParameters::ATOM_DialogParameters (void)
{
}

void ATOM_DialogParameters::resetParameters (void)
{
	ATOM_WidgetParameters::resetParameters ();

	borderMode() = ATOM_Widget::Raise;
	rect().size.w = 400;
	rect().size.h = 300;
	style() |= (ATOM_Widget::Border|ATOM_Widget::TitleBar);
	style() &= ~ATOM_Widget::Control;
}

ATOM_Widget *ATOM_DialogParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_Dialog *dialog = ATOM_NEW(ATOM_Dialog, parent, rect(), style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	dialog->setAsyncLoad(loadPriority);
	//---------------------//
	apply (dialog);
	return dialog;
}

////////////////////////////////////////////////////////////////////////////////////////////

ATOM_EditParameters::ATOM_EditParameters (void)
{
}

void ATOM_EditParameters::resetParameters (void)
{
	ATOM_WidgetParameters::resetParameters ();

	borderMode() = ATOM_Widget::Drop;
	style() |= ATOM_Widget::Border;
	imageId() = ATOM_IMAGEID_WINDOW_BKGROUND;
	rect().size.w = 100;
	rect().size.h = 24;

	_readonly = 0;
	_iNumber = 0;
	_fNumber = 0;
	_password = 0;
	_text = "";
	_cursorImageId = -1;
	_cursorWidth = 2;
	_cursorOffset = 0;
	_maxChar = 0;
	_offsetX = 0;
	_frontImage = 1;
}

void ATOM_EditParameters::apply (ATOM_Widget *widget)
{
	ATOM_WidgetParameters::apply (widget);

	ATOM_Edit *edit = (ATOM_Edit*)widget;
	int editType = 0;
	if (_readonly)
		editType |= EDITTYPE_READONLY;
	if (_iNumber)
		editType |= EDITTYPE_INTEGER;
	if (_fNumber)
		editType |= EDITTYPE_NUMBER;
	if (_password)
		editType |= EDITTYPE_PASSWORD;

	edit->setEditType (editType);
	edit->setString (_text.c_str());
	edit->setCursorImageId (_cursorImageId);
	edit->setCursorWidth (_cursorWidth);
	edit->setCursorOffset (_cursorOffset);
	edit->setMaxLength (_maxChar);
	edit->setOffsetX (_offsetX);
	edit->setFrontImageId(_frontImage);
}

void ATOM_EditParameters::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("readonly", _readonly);
	xml->SetAttribute ("password", _password);
	xml->SetAttribute ("number", _iNumber);
	xml->SetAttribute ("fnum", _fNumber);
	xml->SetAttribute ("cursor", _cursorImageId);
	xml->SetAttribute ("cursorw", _cursorWidth);
	xml->SetAttribute ("cursoroffset", _cursorOffset);
	xml->SetAttribute ("maxchar", _maxChar);
	xml->SetAttribute ("offsetx", _offsetX);
	xml->SetAttribute ("frontimg", _frontImage);
	if (!_text.empty ())
	{
		xml->SetAttribute ("content", _text.c_str());
	}
}

void ATOM_EditParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("readonly", &_readonly);
		xml->QueryIntAttribute ("password", &_password);
		xml->QueryIntAttribute ("number", &_iNumber);
		xml->QueryIntAttribute ("fnum", &_fNumber);
		xml->QueryIntAttribute ("cursor", &_cursorImageId);
		xml->QueryIntAttribute ("cursorw", &_cursorWidth);
		xml->QueryIntAttribute ("cursoroffset", &_cursorOffset);
		xml->QueryIntAttribute ("maxchar", &_maxChar);
		xml->QueryIntAttribute ("offsetx", &_offsetX);
		xml->QueryIntAttribute ("frontimg", &_frontImage);
		const char *content = xml->Attribute ("content");
		if (content)
		{
			_text = content;
		}
	}
}

ATOM_Widget *ATOM_EditParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_Edit *edit = ATOM_NEW(ATOM_Edit, parent, rect(), style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	edit->setAsyncLoad(loadPriority);
	//---------------------//
	apply (edit);
	return edit;
}

//////////////////////////////////////////////////////////////////////////////////////

ATOM_ComboBoxParameters::ATOM_ComboBoxParameters (void)
{
}

void ATOM_ComboBoxParameters::resetParameters (void)
{
	ATOM_WidgetParameters::resetParameters ();

	imageId() = ATOM_IMAGEID_CONTROL_BKGROUND;
	rect().size.w = 100;
	rect().size.h = 30;
	layoutType() = ATOM_WidgetLayout::Horizontal;

	_buttonWidth = 18;
	_buttonImageId = ATOM_IMAGEID_SCROLLBAR_VBUTTONDOWN;
	_buttonBorderMode = -1;
	_dropdownHeight = 100;
	_dropdownImageId = 1;
	_dropdownItemHeight = 20;
	_dropDownHilightImageId = 0;
	_dropDownBorderMode = -1;
	_editImageId = 0;
	_editBorderMode = -1;
	_offsetX = 0;
	_buttonLength = 16;
	_sliderWidth = 16;
	_sliderLength = 16;

}

void ATOM_ComboBoxParameters::apply (ATOM_Widget *widget)
{
	ATOM_WidgetParameters::apply (widget);

	ATOM_ComboBox *combobox = (ATOM_ComboBox*)widget;
	combobox->setButtonWidth (_buttonWidth);
	if (_buttonBorderMode != -1)
	{
		combobox->getDropDownButton ()->setStyle (combobox->getDropDownButton()->getStyle()|ATOM_Widget::Border);
	}
	else
	{
		combobox->getDropDownButton ()->setStyle (combobox->getDropDownButton()->getStyle()&~ATOM_Widget::Border);
	}

	combobox->getDropDownButton()->setClientImageId (_buttonImageId);
	combobox->setDropDownHeight (_dropdownHeight);
	combobox->setDropDownListImageId(_dropdownImageId);
	combobox->setDropDownListItemHeight (_dropdownItemHeight);
	combobox->setDropDownListHilightImageId (_dropDownHilightImageId);
	if (_dropDownBorderMode!= -1)
	{
		combobox->getDropDownList()->setStyle (combobox->getDropDownList()->getStyle()|ATOM_Widget::Border);
		combobox->getDropDownList()->setBorderMode ((ATOM_Widget::BorderMode)_dropDownBorderMode);
	}
	else
	{
		combobox->getDropDownList()->setStyle (combobox->getDropDownList()->getStyle()&~ATOM_Widget::Border);
	}

	combobox->getInputEdit()->setClientImageId (_editImageId);
	if (_editBorderMode!= -1)
	{
		combobox->getInputEdit()->setStyle (combobox->getInputEdit()->getStyle()|ATOM_Widget::Border);
		combobox->getInputEdit()->setBorderMode ((ATOM_Widget::BorderMode)_editBorderMode);
	}
	else
	{
		combobox->getInputEdit()->setStyle (combobox->getInputEdit()->getStyle()&~ATOM_Widget::Border);
	}
	if(_offsetX)
		combobox->setOffsetX(_offsetX);

	combobox->setVerticalSCrollBarImageId(_scrollBarBackgroundImageId,_scrollBarSliderImageId,
		_scrollBarUpButtonImageId,_scrollBarDownButtonImageId);

	combobox->setscrollBarWidth(_sliderWidth);
	combobox->setScrollBarSliderLength(_sliderLength);
	combobox->setScrollBarButtonLength(_buttonLength);
}

void ATOM_ComboBoxParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	ATOM_WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("buttonwidth", _buttonWidth);
	xml->SetAttribute ("buttonimage", _buttonImageId);
	xml->SetAttribute ("buttonborder", _buttonBorderMode);
	xml->SetAttribute ("dropdownheight", _dropdownHeight);
	xml->SetAttribute ("dropdownimage", _dropdownImageId);
	xml->SetAttribute ("dropdownitemheight", _dropdownItemHeight);
	xml->SetAttribute ("dropdownhilightimage", _dropDownHilightImageId);
	xml->SetAttribute ("dropdownborder", _dropDownBorderMode);
	xml->SetAttribute ("editimage", _editImageId);
	xml->SetAttribute ("editborder", _editBorderMode);
	if(_offsetX)
		xml->SetAttribute ("offsetx", _offsetX);

	xml->SetAttribute ("handleimage", _scrollBarSliderImageId);
	xml->SetAttribute ("ubimage", _scrollBarUpButtonImageId);
	xml->SetAttribute ("dbimage", _scrollBarDownButtonImageId);
	xml->SetAttribute ("scrollBarBkImage", _scrollBarBackgroundImageId);


	xml->SetAttribute ("buttonLength", _buttonLength);
	xml->SetAttribute ("sliderWidth", _sliderWidth);
	xml->SetAttribute ("sliderLength", _sliderLength);
}

void ATOM_ComboBoxParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("buttonwidth", &_buttonWidth);
		xml->QueryIntAttribute ("buttonimage", &_buttonImageId);
		xml->QueryIntAttribute ("buttonborder", &_buttonBorderMode);
		xml->QueryIntAttribute ("dropdownheight", &_dropdownHeight);
		xml->QueryIntAttribute ("dropdownimage", &_dropdownImageId);
		xml->QueryIntAttribute ("dropdownitemheight", &_dropdownItemHeight);
		xml->QueryIntAttribute ("dropdownhilightimage", &_dropDownHilightImageId);
		xml->QueryIntAttribute ("dropdownborder", &_dropDownBorderMode);
		xml->QueryIntAttribute ("editimage", &_editImageId);
		xml->QueryIntAttribute ("editborder", &_editBorderMode);
		xml->QueryIntAttribute ("offsetx", &_offsetX);

		xml->QueryIntAttribute ("handleimage", &_scrollBarSliderImageId);
		xml->QueryIntAttribute ("ubimage", &_scrollBarUpButtonImageId);
		xml->QueryIntAttribute ("dbimage", &_scrollBarDownButtonImageId);
		xml->QueryIntAttribute ("scrollBarBkImage", &_scrollBarBackgroundImageId);

		xml->QueryIntAttribute ("buttonLength", &_buttonLength);
		xml->QueryIntAttribute ("sliderWidth", &_sliderWidth);
		xml->QueryIntAttribute ("sliderLength", &_sliderLength);
	}
}

ATOM_Widget *ATOM_ComboBoxParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_ComboBox *combobox = ATOM_NEW(ATOM_ComboBox, parent, rect(), _buttonWidth, style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	combobox->setAsyncLoad(loadPriority);
	//---------------------//
	apply (combobox);
	return combobox;
}

//////////////////////////////////////////////////////////////////////////////////////

ATOM_RealtimeCtrlParameters::ATOM_RealtimeCtrlParameters (void)
{
}

void ATOM_RealtimeCtrlParameters::resetParameters (void)
{
	ATOM_WidgetParameters::resetParameters ();

	imageId() = -1;
	rect().size.w = 400;
	rect().size.h = 300;

	_transparent = 0;
}

void ATOM_RealtimeCtrlParameters::apply (ATOM_Widget *widget)
{
	ATOM_WidgetParameters::apply (widget);

	ATOM_RealtimeCtrl *rt = (ATOM_RealtimeCtrl*)widget;
	rt->transparent (_transparent != 0);
}

void ATOM_RealtimeCtrlParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	ATOM_WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("transparent", _transparent);
}

void ATOM_RealtimeCtrlParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		_transparent = 0;

		xml->QueryIntAttribute ("transparent", &_transparent);
	}
}

ATOM_Widget *ATOM_RealtimeCtrlParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_RealtimeCtrl *rt = ATOM_NEW(ATOM_RealtimeCtrl, parent, rect(), style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	rt->setAsyncLoad(loadPriority);
	//---------------------//
	apply (rt);
	return rt;
}

//////////////////////////////////////////////////////////////////////////////////////

ATOM_ListBoxParameters::ATOM_ListBoxParameters (void)
{
}

void ATOM_ListBoxParameters::resetParameters (void)
{
	ATOM_WidgetParameters::resetParameters ();

	rect().size.w = 60;
	rect().size.h = 100;
	borderMode() = ATOM_Widget::Drop;
	style() |= ATOM_Widget::Border;
	imageId() = ATOM_IMAGEID_WINDOW_BKGROUND;

	_lineHeight = 24;
	_selectedImageId = ATOM_IMAGEID_LISTITEM_HILIGHT;
	_hoverImageId = ATOM_INVALID_IMAGEID;
}

void ATOM_ListBoxParameters::apply (ATOM_Widget *widget)
{
	ATOM_WidgetParameters::apply (widget);

	ATOM_ListBox *listbox = (ATOM_ListBox*)widget;
	listbox->setItemHeight (_lineHeight);
	listbox->setSelectImageId (_selectedImageId);
	listbox->setHoverImageId (_hoverImageId);
}

void ATOM_ListBoxParameters::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("lineheight", _lineHeight);
	xml->SetAttribute ("selimage", _selectedImageId);
	xml->SetAttribute ("hoverimage", _hoverImageId);
}

void ATOM_ListBoxParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("lineheight", &_lineHeight);
		xml->QueryIntAttribute ("selimage", &_selectedImageId);
		xml->QueryIntAttribute ("hoverimage", &_hoverImageId);
	}
}

ATOM_Widget *ATOM_ListBoxParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_ListBox *listbox = ATOM_NEW(ATOM_ListBox, parent, rect(), _lineHeight, style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	listbox->setAsyncLoad(loadPriority);
	//---------------------//
	apply (listbox);
	return listbox;
}

///////////////////////////////////////////////////////////////////////////////

ATOM_TreeCtrlParameters::ATOM_TreeCtrlParameters (void)
{
}

void ATOM_TreeCtrlParameters::resetParameters (void)
{
	ATOM_WidgetParameters::resetParameters ();

	rect().size.w = 60;
	rect().size.h = 100;
	imageId() = ATOM_IMAGEID_WINDOW_BKGROUND;
	style() = style()|ATOM_Widget::Border;
	borderMode() = ATOM_Widget::Drop;

	_imageSize = 0;
	_lineHeight = 0;
	_indent = 0;
	_textImageId = -1;
}

void ATOM_TreeCtrlParameters::apply (ATOM_Widget *widget)
{
	ATOM_WidgetParameters::apply (widget);

	ATOM_TreeCtrl *tc = (ATOM_TreeCtrl*)widget;
	tc->setTextImageId (_textImageId);
}

void ATOM_TreeCtrlParameters::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("imagesize", _imageSize);
	xml->SetAttribute ("lineheight", _lineHeight);
	xml->SetAttribute ("indent", _indent);
	xml->SetAttribute ("textimage", _textImageId);
}

void ATOM_TreeCtrlParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("imagesize", &_imageSize);
		xml->QueryIntAttribute ("lineheight", &_lineHeight);
		xml->QueryIntAttribute ("indent", &_indent);
		xml->QueryIntAttribute ("textimage", &_textImageId);
	}
}

ATOM_Widget *ATOM_TreeCtrlParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_TreeCtrl *tc = ATOM_NEW(ATOM_TreeCtrl, parent, rect(), style(), id(), _imageSize, _lineHeight, _indent, ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	tc->setAsyncLoad(loadPriority);
	//---------------------//
	apply (tc);
	return tc;
}

///////////////////////////////////////////////////////////////////////////////

ATOM_ListViewParameters::ATOM_ListViewParameters (void)
{
}

void ATOM_ListViewParameters::resetParameters (void)
{
	ATOM_WidgetParameters::resetParameters ();

	rect().size.w = 200;
	rect().size.h = 160;
	borderMode() = ATOM_Widget::Drop;
	style() |= ATOM_Widget::Border;
	imageId() = ATOM_IMAGEID_CONTROL_BKGROUND;

	_columnCount = 0;
	_rowCount = 0;
	_columnWidth = 50;
	_itemHeight = 50;
	_selectedImage = -1;
	_hoverImage = -1;
	_singleBackGroundImage = -1;

	_buttonLength = 16;
	_sliderLength = 16;
	_sliderWidth = 16;


}

void ATOM_ListViewParameters::apply (ATOM_Widget *widget)
{
	ATOM_WidgetParameters::apply (widget);

	ATOM_ListView *lv = (ATOM_ListView*)widget;
	lv->setColumnCount (_columnCount);
	for(unsigned i = 0; i < _columnCount; ++i)
	{
		lv->setColumnWidth (i, _columnWidth);
	}
	lv->setRowCount (_rowCount);
	lv->setItemHeight (_itemHeight);
	lv->setSelectImage (_selectedImage);
	lv->setVerticalSCrollBarImageId(_scrollBarBackgroundImageId,_scrollBarSliderImageId,
		_scrollBarUpButtonImageId,_scrollBarDownButtonImageId);
	lv->setSingleRowBackGroundImageId(_singleBackGroundImage);
	lv->setHoverImage(_hoverImage);

	lv->setscrollBarWidth(_sliderWidth);
	lv->setScrollBarSliderLength(_sliderLength);
	lv->setScrollBarButtonLength(_buttonLength);
}

void ATOM_ListViewParameters::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("columnc", _columnCount);
	xml->SetAttribute ("columnw", _columnWidth);
	xml->SetAttribute ("rowc", _rowCount);
	xml->SetAttribute ("rowh", _itemHeight);
	xml->SetAttribute ("selimage", _selectedImage);

	xml->SetAttribute ("handleimage", _scrollBarSliderImageId);
	xml->SetAttribute ("ubimage", _scrollBarUpButtonImageId);
	xml->SetAttribute ("dbimage", _scrollBarDownButtonImageId);
	xml->SetAttribute ("scrollBarBkImage", _scrollBarBackgroundImageId);

	xml->SetAttribute("bkImage",_singleBackGroundImage);
	xml->SetAttribute("hoverImage",_hoverImage);


	xml->SetAttribute ("buttonLength", _buttonLength);
	xml->SetAttribute ("sliderWidth", _sliderWidth);
	xml->SetAttribute ("sliderLength", _sliderLength);

}

void ATOM_ListViewParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("columnc", &_columnCount);
		xml->QueryIntAttribute ("columnw", &_columnWidth);
		xml->QueryIntAttribute ("rowc", &_rowCount);
		xml->QueryIntAttribute ("rowh", &_itemHeight);
		xml->QueryIntAttribute ("selimage", &_selectedImage);

		xml->QueryIntAttribute ("handleimage", &_scrollBarSliderImageId);
		xml->QueryIntAttribute ("ubimage", &_scrollBarUpButtonImageId);
		xml->QueryIntAttribute ("dbimage", &_scrollBarDownButtonImageId);
		xml->QueryIntAttribute ("scrollBarBkImage", &_scrollBarBackgroundImageId);


		xml->QueryIntAttribute("bkImage",&_singleBackGroundImage);
		xml->QueryIntAttribute("hoverImage",&_hoverImage);

		xml->QueryIntAttribute ("buttonLength", &_buttonLength);
		xml->QueryIntAttribute ("sliderWidth", &_sliderWidth);
		xml->QueryIntAttribute ("sliderLength", &_sliderLength);
	}
}

ATOM_Widget *ATOM_ListViewParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_ListView *lv = ATOM_NEW(ATOM_ListView, parent, rect(), style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	lv->setAsyncLoad(loadPriority);
	//---------------------//
	apply (lv);
	return lv;
}

//////////////////////////////////////////////////////////////////////////////////

ATOM_MultieditParameters::ATOM_MultieditParameters (void)
{
}

void ATOM_MultieditParameters::resetParameters (void)
{
	ATOM_WidgetParameters::resetParameters ();

	borderMode() = ATOM_Widget::Drop;
	style() |= ATOM_Widget::Border;
	imageId() = ATOM_IMAGEID_WINDOW_BKGROUND;
	rect().size.w = 300;
	rect().size.h = 100;

	_readonly = 0;
	_lineHeight = 20;
	_text = "";
	_cursorImageId = -1;
	_cursorWidth = 2;
	_cursorOffset = 0;
	_maxChar = 0;
	_buttonLength = 16;
	_sliderLength = 16;
	_sliderWidth = 16;
	_frontimage = 1;
}

void ATOM_MultieditParameters::apply (ATOM_Widget *widget)
{
	ATOM_WidgetParameters::apply (widget);

	ATOM_MultiEdit *edit = (ATOM_MultiEdit*)widget;
	edit->setEditType (_readonly ? EDITTYPE_READONLY : 0);
	edit->setLineHeight (_lineHeight);
	edit->setString (_text.c_str());
	edit->setCursorImageId (_cursorImageId);
	edit->setCursorWidth (_cursorWidth);
	edit->setCursorOffset (_cursorOffset);
	edit->setMaxLength (_maxChar);
	edit->setVerticalSCrollBarImageId(_scrollBarBackgroundImageId,_scrollBarSliderImageId,
		_scrollBarUpButtonImageId,_scrollBarDownButtonImageId);
	edit->setscrollBarWidth(_sliderWidth);
	edit->setScrollBarSliderLength(_sliderLength);
	edit->setScrollBarButtonLength(_buttonLength);
	edit->setFrontImageId(_frontimage);
}

void ATOM_MultieditParameters::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("readonly", _readonly);
	xml->SetAttribute ("lineheight", _lineHeight);
	xml->SetAttribute ("cursor", _cursorImageId);
	xml->SetAttribute ("cursorw", _cursorWidth);
	xml->SetAttribute ("cursoroffset", _cursorOffset);
	xml->SetAttribute ("maxchar", _maxChar);
	if (!_text.empty ())
	{
		xml->SetAttribute ("content", _text.c_str());
	}

	xml->SetAttribute ("handleimage", _scrollBarSliderImageId);
	xml->SetAttribute ("ubimage", _scrollBarUpButtonImageId);
	xml->SetAttribute ("dbimage", _scrollBarDownButtonImageId);
	xml->SetAttribute ("scrollBarBkImage", _scrollBarBackgroundImageId);

	xml->SetAttribute ("buttonLength", _buttonLength);
	xml->SetAttribute ("sliderWidth", _sliderWidth);
	xml->SetAttribute ("sliderLength", _sliderLength);
	xml->SetAttribute("frontimg",_frontimage);
}

void ATOM_MultieditParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("readonly", &_readonly);
		xml->QueryIntAttribute ("lineheight", &_lineHeight);
		xml->QueryIntAttribute ("cursor", &_cursorImageId);
		xml->QueryIntAttribute ("cursorw", &_cursorWidth);
		xml->QueryIntAttribute ("maxchar", &_maxChar);
		xml->QueryIntAttribute ("cursoroffset", &_cursorOffset);

		const char *content = xml->Attribute ("content");
		if (content)
		{
			_text = content;
		}
		xml->QueryIntAttribute ("handleimage", &_scrollBarSliderImageId);
		xml->QueryIntAttribute ("ubimage", &_scrollBarUpButtonImageId);
		xml->QueryIntAttribute ("dbimage", &_scrollBarDownButtonImageId);
		xml->QueryIntAttribute ("scrollBarBkImage", &_scrollBarBackgroundImageId);

		xml->QueryIntAttribute ("buttonLength", &_buttonLength);
		xml->QueryIntAttribute ("sliderWidth", &_sliderWidth);
		xml->QueryIntAttribute ("sliderLength", &_sliderLength);
		xml->QueryIntAttribute("frontimg",&_frontimage);
	}
}

ATOM_Widget *ATOM_MultieditParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_MultiEdit *edit = ATOM_NEW(ATOM_MultiEdit, parent, rect(), style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	edit->setAsyncLoad(loadPriority);
	//---------------------//
	apply (edit);
	return edit;
}

///////////////////////////////////////////////////////////////////////////////////

ATOM_PanelParameters::ATOM_PanelParameters (void)
{
}

void ATOM_PanelParameters::resetParameters (void)
{
	ATOM_WidgetParameters::resetParameters ();

	borderMode() = ATOM_Widget::Raise;
	style() = style()|ATOM_Widget::Border;
	rect().size.w = 200;
	rect().size.h = 160;

	_eventTransition = false;
}

void ATOM_PanelParameters::apply (ATOM_Widget *widget)
{
	ATOM_WidgetParameters::apply (widget);

	ATOM_Panel *panel = (ATOM_Panel*)widget;
	panel->enableEventTransition (_eventTransition);
}

void ATOM_PanelParameters::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("eventtransition", _eventTransition ? 1 : 0);
}

void ATOM_PanelParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		int i = 0;
		xml->QueryIntAttribute ("eventtransition", &i);
		_eventTransition = i != 0;
	}
}

ATOM_Widget *ATOM_PanelParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_Panel *panel = ATOM_NEW(ATOM_Panel, parent, rect(), style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	panel->setAsyncLoad(loadPriority);
	//---------------------//
	apply (panel);
	return panel;
}

///////////////////////////////////////////////////////////////////////////////////

ATOM_MarqueeParameters::ATOM_MarqueeParameters (void)
{
}

void ATOM_MarqueeParameters::resetParameters (void)
{
	ATOM_WidgetParameters::resetParameters ();

	rect().size.w = 100;
	rect().size.h = 24;

	_dir = MARQUEEDIR_RIGHT;
	_speed = 0.f;
	_offset = 0.f;
	_position = 0.f;
}

void ATOM_MarqueeParameters::apply (ATOM_Widget *widget)
{
	ATOM_WidgetParameters::apply (widget);

	ATOM_Marquee *marquee = (ATOM_Marquee*)widget;
	marquee->setDir (_dir);
	marquee->setPosition (_position);
	marquee->setOffset (_offset);
	marquee->setSpeed (_speed);
	marquee->setText (_text.c_str());
}

void ATOM_MarqueeParameters::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("dir", (int)_dir);
	xml->SetDoubleAttribute ("position", _position);
	xml->SetDoubleAttribute ("offset", _offset);
	xml->SetDoubleAttribute ("speed", _speed);
	xml->SetAttribute ("text", _text.c_str());
}

void ATOM_MarqueeParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		int dir = MARQUEEDIR_RIGHT;
		double pos = 0;
		double speed = 0;
		double offset = 0;

		xml->QueryIntAttribute("dir", &dir);
		xml->QueryDoubleAttribute("position", &pos);
		xml->QueryDoubleAttribute ("offset", &offset);
		xml->QueryDoubleAttribute ("speed", &speed);
		_dir = (MARQUEEDIR)dir;
		_position = pos;
		_offset = offset;
		_speed = speed;

		const char *s = xml->Attribute ("text");
		_text = s ? s : "";
	}
}

ATOM_Widget *ATOM_MarqueeParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_Marquee *marquee = ATOM_NEW(ATOM_Marquee, parent, rect(), style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	marquee->setAsyncLoad(loadPriority);
	//---------------------//
	apply (marquee);
	return marquee;
}

///////////////////////////////////////////////////////////////////////////////////

ATOM_SpacerParameters::ATOM_SpacerParameters (void)
{
}

void ATOM_SpacerParameters::resetParameters (void)
{
	ATOM_WidgetParameters::resetParameters ();

	borderMode() = ATOM_Widget::Raise;
	style() = style()|ATOM_Widget::Border;
	rect().size.w = 200;
	rect().size.h = 160;
	imageId() = -1;
}

void ATOM_SpacerParameters::apply (ATOM_Widget *widget)
{
	ATOM_WidgetParameters::apply (widget);

	ATOM_Panel *panel = (ATOM_Panel*)widget;
	panel->enableEventTransition (true);
}

void ATOM_SpacerParameters::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_WidgetParameters::saveToXML (xml);
}

void ATOM_SpacerParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_WidgetParameters::loadFromXML (xml);
}

ATOM_Widget *ATOM_SpacerParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_Panel *panel = ATOM_NEW(ATOM_Panel, parent, rect(), style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	panel->setAsyncLoad(loadPriority);
	//---------------------//
	apply (panel);
	return panel;
}

///////////////////////////////////////////////////////////////////////////////////

ATOM_ProgressBarParameters::ATOM_ProgressBarParameters (void)
{
}

void ATOM_ProgressBarParameters::resetParameters (void)
{
	ATOM_LabelParameters::resetParameters ();

	style() |= ATOM_Widget::Border;
	borderMode() = ATOM_Widget::Drop;
	rect().size.w = 100;
	rect().size.h = 24;
	imageId() = ATOM_IMAGEID_WINDOW_BKGROUND;

	_alignmentX = 1;
	_frontImageId = ATOM_IMAGEID_PROGRESSBAR;
	_minValue = 0;
	_maxValue = 100;
	_currentValue = 50;
	_drawText = true;
	_isHorizontal = true;
}

void ATOM_ProgressBarParameters::apply (ATOM_Widget *widget)
{
	ATOM_LabelParameters::apply (widget);

	ATOM_ProgressBar *progressBar = (ATOM_ProgressBar*)widget;
	progressBar->setFrontImageId (_frontImageId);
	progressBar->setMinMax (_minValue, _maxValue);
	progressBar->setCurrent (_currentValue);
	progressBar->enableDrawText (_drawText);
	progressBar->setIsHorizontal(_isHorizontal);
}

void ATOM_ProgressBarParameters::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_LabelParameters::saveToXML (xml);

	xml->SetAttribute ("frontimage", _frontImageId);
	xml->SetAttribute ("min", _minValue);
	xml->SetAttribute ("max", _maxValue);
	xml->SetAttribute ("current", _currentValue);
	xml->SetAttribute ("drawtext", _drawText?1:0);
	xml->SetAttribute("horizontal",_isHorizontal?1:0);
}

void ATOM_ProgressBarParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_LabelParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("frontimage", &_frontImageId);
		xml->QueryIntAttribute ("min", &_minValue);
		xml->QueryIntAttribute ("max", &_maxValue);
		xml->QueryIntAttribute ("current", &_currentValue);
		int bDrawText = 1;
		xml->QueryIntAttribute ("drawtext", &bDrawText);
		_drawText = (bDrawText!= 0);
		int bIsHorizontal = 1;
		xml->QueryIntAttribute ("horizontal", &bIsHorizontal);
		_isHorizontal = (bIsHorizontal != 0);
	}
}

ATOM_Widget *ATOM_ProgressBarParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_ProgressBar *progressBar = ATOM_NEW(ATOM_ProgressBar, parent, rect(), style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	progressBar->setAsyncLoad(loadPriority);
	//---------------------//
	apply (progressBar);
	return progressBar;
}

/////////////////////////////////////////////////////////////////////////////////////////

ATOM_RichEditParameters::ATOM_RichEditParameters (void)
{
}

void ATOM_RichEditParameters::resetParameters (void)
{
	ATOM_WidgetParameters::resetParameters ();

	rect().size.w = 200;
	rect().size.h = 160;
	borderMode() = ATOM_Widget::Drop;
	style() |= ATOM_Widget::Border;
	imageId() = ATOM_IMAGEID_WINDOW_BKGROUND;

	_maxHeight = 0;
	_autoSize = 0;
	_lineHeight = 20;
	

	_buttonLength = 16;
	_sliderWidth = 16;
	_sliderLength = 16;
}

void ATOM_RichEditParameters::apply (ATOM_Widget *widget)
{
	ATOM_WidgetParameters::apply (widget);

	ATOM_RichEdit *richedit = (ATOM_RichEdit*)widget;
	richedit->setMaxHeight (_maxHeight);
	richedit->setAutoSize (_autoSize!=0);

	richedit->setVerticalSCrollBarImageId(_scrollBarBackgroundImageId,_scrollBarSliderImageId,
		_scrollBarUpButtonImageId,_scrollBarDownButtonImageId);

	richedit->setscrollBarWidth(_sliderWidth);
	richedit->setScrollBarSliderLength(_sliderLength);
	richedit->setScrollBarButtonLength(_buttonLength);
}

void ATOM_RichEditParameters::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("maxheight", _maxHeight);
	xml->SetAttribute ("autosize", _autoSize);
	xml->SetAttribute ("lineheight", _lineHeight);

	xml->SetAttribute ("handleimage", _scrollBarSliderImageId);
	xml->SetAttribute ("ubimage", _scrollBarUpButtonImageId);
	xml->SetAttribute ("dbimage", _scrollBarDownButtonImageId);
	xml->SetAttribute ("scrollBarBkImage", _scrollBarBackgroundImageId);


	xml->SetAttribute ("buttonLength", _buttonLength);
	xml->SetAttribute ("sliderWidth", _sliderWidth);
	xml->SetAttribute ("sliderLength", _sliderLength);

}

void ATOM_RichEditParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("maxheight", &_maxHeight);
		xml->QueryIntAttribute ("autosize", &_autoSize);
		xml->QueryIntAttribute ("lineheight", &_lineHeight);

		xml->QueryIntAttribute ("handleimage", &_scrollBarSliderImageId);
		xml->QueryIntAttribute ("ubimage", &_scrollBarUpButtonImageId);
		xml->QueryIntAttribute ("dbimage", &_scrollBarDownButtonImageId);
		xml->QueryIntAttribute ("scrollBarBkImage", &_scrollBarBackgroundImageId);

		xml->QueryIntAttribute ("buttonLength", &_buttonLength);
		xml->QueryIntAttribute ("sliderWidth", &_sliderWidth);
		xml->QueryIntAttribute ("sliderLength", &_sliderLength);
	}
}

ATOM_Widget *ATOM_RichEditParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_RichEdit *richedit = ATOM_NEW(ATOM_RichEdit, parent, rect(), style(), id(), _lineHeight, ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	richedit->setAsyncLoad(loadPriority);
	//---------------------//
	apply (richedit);
	return richedit;
}

/////////////////////////////////////////////////////////////////////////////

ATOM_ScrollBarParameters::ATOM_ScrollBarParameters (void)
{
}

void ATOM_ScrollBarParameters::resetParameters (void)
{
	ATOM_WidgetParameters::resetParameters ();
	rect().size.w = 16;
	rect().size.h = 80;

	imageId() = ATOM_AUTOMATIC_IMAGEID;
	_SliderImageId = ATOM_AUTOMATIC_IMAGEID;
	_UpButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_DownButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_placement = WP_VERTICAL;
	_buttonWidth = 10;
	_handleWidth = 6;
	_minValue = 0;
	_maxValue = 100;
	_stepValue = 1;
}

void ATOM_ScrollBarParameters::apply (ATOM_Widget *widget)
{
	ATOM_WidgetParameters::apply (widget);

	ATOM_ScrollBar *sb = (ATOM_ScrollBar*)widget;
	sb->setPlacement ((ATOM_WidgetPlacement)_placement);
	if (_placement == WP_VERTICAL)
	{
		sb->setVerticalImageId (imageId(), _SliderImageId, _UpButtonImageId, _DownButtonImageId);
	}
	else
	{
		sb->setHorizontalImageId (imageId(), _SliderImageId, _UpButtonImageId, _DownButtonImageId);
	}
	sb->setButtonWidth (_buttonWidth);
	sb->setHandleWidth (_handleWidth);
	sb->setRange (_minValue, _maxValue);
	sb->setStep (_stepValue);
}

void ATOM_ScrollBarParameters::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("placement", _placement);
	xml->SetAttribute ("handleimage", _SliderImageId);
	xml->SetAttribute ("ubimage", _UpButtonImageId);
	xml->SetAttribute ("dbimage", _DownButtonImageId);
	xml->SetAttribute ("buttonw", _buttonWidth);
	xml->SetAttribute ("handlew", _handleWidth);
	xml->SetAttribute ("min", _minValue);
	xml->SetAttribute ("max", _maxValue);
	xml->SetAttribute ("step", _stepValue);
}

void ATOM_ScrollBarParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("placement", &_placement);
		xml->QueryIntAttribute ("handleimage", &_SliderImageId);
		xml->QueryIntAttribute ("ubimage", &_UpButtonImageId);
		xml->QueryIntAttribute ("dbimage", &_DownButtonImageId);
		xml->QueryIntAttribute ("buttonw", &_buttonWidth);
		xml->QueryIntAttribute ("handlew", &_handleWidth);
		xml->QueryIntAttribute ("min", &_minValue);
		xml->QueryIntAttribute ("max", &_maxValue);
		xml->QueryIntAttribute ("step", &_stepValue);
	}
}

ATOM_Widget *ATOM_ScrollBarParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_ScrollBar *sb = ATOM_NEW(ATOM_ScrollBar, parent, rect(), style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	sb->setAsyncLoad(loadPriority);
	//---------------------//
	apply (sb);
	return sb;
}

///////////////////////////////////////////////////////////////////////////////////

ATOM_WindowParameters::ATOM_WindowParameters (void)
{
}

void ATOM_WindowParameters::resetParameters (void)
{
	ATOM_WidgetParameters::resetParameters ();

	borderMode() = ATOM_Widget::Raise;
	style() |= (ATOM_Widget::Border|ATOM_Widget::TitleBar);
	style() &= ~ATOM_Widget::Control;
	rect().size.w = 400;
	rect().size.h = 300;
}

ATOM_Widget *ATOM_WindowParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_TopWindow *window = ATOM_NEW(ATOM_TopWindow, parent, rect(), style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	window->setAsyncLoad(loadPriority);
	//---------------------//
	apply (window);
	return window;
}

//////////////////////////////////////////////////////////////////////////////////////

ATOM_ViewStackParameters::ATOM_ViewStackParameters (void)
{
	_activePage = -1;
}

void ATOM_ViewStackParameters::resetParameters (void)
{
	ATOM_PanelParameters::resetParameters ();

	_activePage = -1;
}

void ATOM_ViewStackParameters::apply (ATOM_Widget *widget)
{
	ATOM_PanelParameters::apply (widget);

	ATOM_ViewStack *viewstack = (ATOM_ViewStack*)widget;
	viewstack->setActivePage (_activePage);
}

void ATOM_ViewStackParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	ATOM_PanelParameters::saveToXML (xml);

	xml->SetAttribute ("activepage", _activePage ? 1 : 0);
}

void ATOM_ViewStackParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_PanelParameters::loadFromXML (xml);

	if (xml)
	{
		int i = -1;
		xml->QueryIntAttribute ("activepage", &i);
		_activePage = i;
	}
}

ATOM_Widget *ATOM_ViewStackParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_ViewStack *vs = ATOM_NEW(ATOM_ViewStack, parent, rect(), style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	vs->setAsyncLoad(loadPriority);
	//---------------------//
	apply (vs);
	return vs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_ComponentParameters::ATOM_ComponentParameters (void)
{
}

void ATOM_ComponentParameters::resetParameters (void)
{
	ATOM_WidgetParameters::resetParameters ();

	_filename = "";
}

void ATOM_ComponentParameters::apply (ATOM_Widget *widget)
{
	ATOM_WidgetParameters::apply (widget);
}

void ATOM_ComponentParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	ATOM_WidgetParameters::saveToXML (xml);

	if (!_filename.empty ())
	{
		xml->SetAttribute ("filename", _filename.c_str());
	}
}

void ATOM_ComponentParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_WidgetParameters::loadFromXML (xml);

	const char *filename = xml->Attribute ("filename");
	_filename = filename ? filename : "";
}

ATOM_Widget *ATOM_ComponentParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_Widget *widget = parent->loadHierarchy (_filename.c_str());
	//--- wangjan added ---//
	widget->setAsyncLoad(loadPriority);
	//---------------------//
	apply (widget);
	return widget;
}

ATOM_FlashParameters::ATOM_FlashParameters (void)
{
}

void ATOM_FlashParameters::resetParameters (void)
{
	ATOM_WidgetParameters::resetParameters ();

	_filename = "";
	_initFunc = "initSwf";
	_clearFunc = "clearSwf";
	_activateFunc = "showSwf";
	_deactivateFunc = "hideSwf";
	_FPS = 24;
}

void ATOM_FlashParameters::apply (ATOM_Widget *widget)
{
	ATOM_WidgetParameters::apply (widget);

	ATOM_FlashCtrl *flashCtrl = (ATOM_FlashCtrl*)widget;

	flashCtrl->setInitCallback (_initFunc.c_str());
	flashCtrl->setClearCallback (_clearFunc.c_str());
	flashCtrl->setActivateCallback (_activateFunc.c_str());
	flashCtrl->setDeactivateCallback (_deactivateFunc.c_str());
	flashCtrl->setFPS (_FPS);

	if (_filename != flashCtrl->getSWFFileName())
	{
		flashCtrl->loadSWF (_filename.c_str());
	}
}

void ATOM_FlashParameters::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("SWF", _filename.c_str());
	xml->SetAttribute ("InitCallback", _initFunc.c_str());
	xml->SetAttribute ("ClearCallback", _clearFunc.c_str());
	xml->SetAttribute ("ActivateCallback", _activateFunc.c_str());
	xml->SetAttribute ("DeactivateCallback", _deactivateFunc.c_str());
	xml->SetAttribute ("FPS", _FPS);
}

void ATOM_FlashParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_WidgetParameters::loadFromXML (xml);

	const char *filename = xml->Attribute ("SWF");
	_filename = filename ? filename : "";
	const char *initCB = xml ? xml->Attribute ("InitCallback") : 0;
	_initFunc = initCB ? initCB : "initSwf";
	const char *clearCB = xml ? xml->Attribute ("ClearCallback") : 0;
	_clearFunc = clearCB ? clearCB : "clearSwf";
	const char *activateCB = xml ? xml->Attribute ("ActivateCallback") : 0;
	_activateFunc = activateCB ? activateCB : "showSwf";
	const char *deactivateCB = xml ? xml->Attribute ("DeactivateCallback") : 0;
	_deactivateFunc = deactivateCB ? deactivateCB : "hideSwf";

	if (xml)
	{
		xml->QueryIntAttribute ("FPS", (int*)&_FPS);
	}
}

ATOM_Widget *ATOM_FlashParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_FlashCtrl *flashCtrl = ATOM_NEW(ATOM_FlashCtrl, parent, rect(), style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	flashCtrl->setAsyncLoad(loadPriority);
	//---------------------//
	apply (flashCtrl);
	return flashCtrl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_CurveEditorParameters::ATOM_CurveEditorParameters (void)
{
}

void ATOM_CurveEditorParameters::resetParameters (void)
{
	ATOM_WidgetParameters::resetParameters ();

	_timeAxisUnitLength = 20;
	_timeAxisUnitResolution = 100.f;
	_valueAxisUnitLength = 20;
	_valueAxisUnitResolution = 100.f;
	_curveResolution = 4;
	_originTimeAxis = 0.2f;
	_originValueAxis = 0.5f;
	_enableScroll = true;
	_curveColor = 0xFFFFFFFF;
}

void ATOM_CurveEditorParameters::apply (ATOM_Widget *widget)
{
	ATOM_WidgetParameters::apply (widget);

	ATOM_CurveEditor *curveEditor = (ATOM_CurveEditor*)widget;

	curveEditor->setTimeAxisUnitLength(_timeAxisUnitLength);
	curveEditor->setTimeAxisUnitResolution(_timeAxisUnitResolution);
	curveEditor->setValueAxisUnitLength(_valueAxisUnitLength);
	curveEditor->setValueAxisUnitResolution(_valueAxisUnitResolution);
	curveEditor->setCurveResolution(_curveResolution);
	curveEditor->setOriginTimeAxis(_originTimeAxis);
	curveEditor->setOriginValueAxis(_originValueAxis);
	curveEditor->enableScroll(_enableScroll);
	curveEditor->setCurveColor(_curveColor);
}

void ATOM_CurveEditorParameters::saveToXML (ATOM_TiXmlElement *xml) const
{
	ATOM_WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("TimeAxisUnitLen", (int)_timeAxisUnitLength);
	xml->SetDoubleAttribute ("TimeAxisUnitRes", _timeAxisUnitResolution);
	xml->SetAttribute ("ValueAxisUnitLen", (int)_valueAxisUnitLength);
	xml->SetDoubleAttribute ("ValueAxisUnitRes", _valueAxisUnitResolution);
	xml->SetAttribute ("CurveRes", (int)_curveResolution);
	xml->SetDoubleAttribute ("OriginTimeAxis", _originTimeAxis);
	xml->SetDoubleAttribute ("OriginValueAxis", _originValueAxis);
	xml->SetAttribute ("EnableScroll", _enableScroll?1:0);
	xml->SetAttribute ("CurveColor", _curveColor.getRaw());
}

void ATOM_CurveEditorParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	ATOM_WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("TimeAxisUnitLen", (int*)&_timeAxisUnitLength);
		xml->QueryFloatAttribute ("TimeAxisUnitRes", &_timeAxisUnitResolution);
		xml->QueryIntAttribute ("ValueAxisUnitLen", (int*)&_valueAxisUnitLength);
		xml->QueryFloatAttribute ("ValueAxisUnitRes", &_valueAxisUnitResolution);
		xml->QueryIntAttribute ("CurveRes", (int*)&_curveResolution);
		xml->QueryFloatAttribute ("OriginTimeAxis", &_originTimeAxis);
		xml->QueryFloatAttribute ("OriginValueAxis", &_originValueAxis);
		int b = _enableScroll?1:0;
		xml->QueryIntAttribute ("EnableScroll", &b);
		_enableScroll = (b != 0);
		int c = 0xFFFFFFFF;
		xml->QueryIntAttribute ("CurveColor", &c);
		_curveColor = c;
	}
}

ATOM_Widget *ATOM_CurveEditorParameters::createWidget (ATOM_Widget *parent, int loadPriority)
{
	ATOM_CurveEditor *curveEditor = ATOM_NEW(ATOM_CurveEditor, parent, rect(), style(), id(), ATOM_Widget::ShowNormal);
	//--- wangjan added ---//
	curveEditor->setAsyncLoad(loadPriority);
	//---------------------//
	apply (curveEditor);
	return curveEditor;
}

ATOM_WidgetPreloadedParameters::ATOM_WidgetPreloadedParameters (void)
{
	_parameters = nullptr;
}

ATOM_WidgetPreloadedParameters::~ATOM_WidgetPreloadedParameters (void)
{
	ATOM_DELETE(_parameters);
	for (int i = 0; i < _childParameters.size(); ++i)
	{
		ATOM_DELETE(_childParameters[i]);
	}
	_childParameters.clear();
}

int ATOM_WidgetPreloadedParameters::getLoadPriority (void) const
{
	return asyncload;
}

ATOM_WidgetParameters *ATOM_WidgetPreloadedParameters::getParameters (void) const
{
	return _parameters;
}

int ATOM_WidgetPreloadedParameters::getNumChildParameters (void) const
{
	return _childParameters.size();
}

ATOM_WidgetPreloadedParameters *ATOM_WidgetPreloadedParameters::getChildParameters (int n) const
{
	return _childParameters[n];
}

bool ATOM_WidgetPreloadedParameters::load (const char *filename)
{
	ATOM_AutoFile f(filename, ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
	{
		return false;
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
		return false;
	}

	ATOM_TiXmlElement *root = doc.RootElement ();
	if (!root)
	{
		return false;
	}

	if (strcmp (root->Value(), "UI"))
	{
		return false;
	}

	const char *fontSetFile = root->Attribute ("fonts");
	if (fontSetFile)
	{
		ATOM_GUIFont::loadFontConfig (fontSetFile);
	}

	//--- wangjian modified ---//
	// 读取UI的异步加载标记
	asyncload = ATOM_Widget::WIDGET_LOAD_PRIORITY_BASE;
	root->Attribute("asyncload", &asyncload);
	// 如果没有设置 默认开启异步加载 优先级为1000
	//setAsyncLoad( root->Attribute("asyncload", &asyncload) ? asyncload : WIDGET_LOAD_PRIORITY_BASE );
	//this->enableCheckWaiting(1);
	return load_r ( this, root);
}

bool ATOM_WidgetPreloadedParameters::load_r (ATOM_WidgetPreloadedParameters *param, ATOM_TiXmlElement *root)
{
	for (ATOM_TiXmlElement *child = root->FirstChildElement("component"); child; child = child->NextSiblingElement("component"))
	{
		const char *type = child->Attribute ("type");
		if (!type)
		{
			continue;
		}

		ATOM_WidgetPreloadedParameters *childWidget = 0;

		if (!stricmp(type, "panel"))
		{
			ATOM_PanelParameters *params = ATOM_NEW(ATOM_PanelParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "spacer"))
		{
			ATOM_SpacerParameters *params = ATOM_NEW(ATOM_SpacerParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "edit"))
		{
			ATOM_EditParameters *params = ATOM_NEW(ATOM_EditParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "label"))
		{
			ATOM_LabelParameters *params = ATOM_NEW(ATOM_LabelParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "window"))
		{
			ATOM_WindowParameters *params = ATOM_NEW(ATOM_WindowParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters  = params;
		}
		else if (!stricmp(type, "dialog"))
		{
			ATOM_DialogParameters *params = ATOM_NEW(ATOM_DialogParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "multiedit"))
		{
			ATOM_MultieditParameters *params = ATOM_NEW(ATOM_MultieditParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "button"))
		{
			ATOM_ButtonParameters *params = ATOM_NEW(ATOM_ButtonParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "progressbar"))
		{
			ATOM_ProgressBarParameters *params = ATOM_NEW(ATOM_ProgressBarParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "richedit"))
		{
			ATOM_RichEditParameters *params = ATOM_NEW(ATOM_RichEditParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "cell"))
		{
			ATOM_CellParameters *params = ATOM_NEW(ATOM_CellParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "listbox"))
		{
			ATOM_ListBoxParameters *params = ATOM_NEW(ATOM_ListBoxParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "checkbox"))
		{
			ATOM_CheckBoxParameters *params = ATOM_NEW(ATOM_CheckBoxParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "listview"))
		{
			ATOM_ListViewParameters *params = ATOM_NEW(ATOM_ListViewParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "scrollbar"))
		{
			ATOM_ScrollBarParameters *params = ATOM_NEW(ATOM_ScrollBarParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "viewstack"))
		{
			ATOM_ViewStackParameters *params = ATOM_NEW(ATOM_ViewStackParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "treectrl"))
		{
			ATOM_TreeCtrlParameters *params = ATOM_NEW(ATOM_TreeCtrlParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "component"))
		{
			ATOM_ComponentParameters *params = ATOM_NEW(ATOM_ComponentParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "flash"))
		{
			ATOM_FlashParameters *params = ATOM_NEW(ATOM_FlashParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "scrollmap"))
		{
			ATOM_ScrollMapParameters *params = ATOM_NEW(ATOM_ScrollMapParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "combobox")) 
		{
			ATOM_ComboBoxParameters *params = ATOM_NEW(ATOM_ComboBoxParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "realtimectrl"))
		{
			ATOM_RealtimeCtrlParameters *params = ATOM_NEW(ATOM_RealtimeCtrlParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "marquee"))
		{
			ATOM_MarqueeParameters *params = ATOM_NEW(ATOM_MarqueeParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else if (!stricmp(type, "curveeditor"))
		{
			ATOM_CurveEditorParameters *params = ATOM_NEW(ATOM_CurveEditorParameters);
			params->resetParameters ();
			params->loadFromXML (child);
			childWidget = ATOM_NEW(ATOM_WidgetPreloadedParameters);
			childWidget->_parameters = params;
		}
		else
		{
			continue;
		}

		if (childWidget)
		{
			param->_childParameters.push_back (childWidget);
			childWidget->load_r (childWidget, child);
		}
	}

	return true;
}
