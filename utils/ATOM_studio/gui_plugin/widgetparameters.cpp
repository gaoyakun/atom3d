#include "StdAfx.h"
#include "widgetparameters.h"
#include "proxy.h"
#include "plugin_gui.h"
#include "idmanager.h"

WidgetParameters::WidgetParameters (ControlProxy *p)
{
	_proxy = p;
	_id = -1;
}

WidgetParameters::WidgetParameters (const WidgetParameters &other)
{
	_proxy = other._proxy;
	_rect = other._rect;
	_id = -1;
	_imageId = other._imageId;
	_style = other._style;
	_borderMode = other._borderMode;
	_dragMode = other._dragMode;
	_enableDrop = other._enableDrop;
	_enableDrag = other._enableDrag;
	_disabled = false;
	_layoutType = other._layoutType;
	_layoutSpaces[0] = other._layoutSpaces[0];
	_layoutSpaces[1] = other._layoutSpaces[1];
	_layoutSpaces[2] = other._layoutSpaces[2];
	_layoutSpaces[3] = other._layoutSpaces[3];
	_layoutGap = other._layoutGap;
	_desc = other._desc;
	_font = other._font;
	_fontColor = other._fontColor;
	_fontOutLineColor = other._fontOutLineColor;
	_imagelist = other._imagelist;

	_waitingCheck = other._waitingCheck;	// wangjian added
}

WidgetParameters::~WidgetParameters (void)
{
	if (_id != -1)
	{
		ATOM_ASSERT(IdManager::findId (_id));
		IdManager::removeId (_id);
	}
}

bool WidgetParameters::interactive (void)
{
	return true;
}

void WidgetParameters::resetParameters (void)
{
	_font = "";
	_desc = "";
	_fontColor.setFloats(1.f, 1.f, 1.f, 1.f);
	_fontOutLineColor.setFloats(0.f, 0.f, 0.f, 0.785f);// RGBA 0 0 0 200
	_id = interactive() ? IdManager::aquireId (100) : -1;
	_imageId = ATOM_IMAGEID_CONTROL_BKGROUND;
	_borderMode = -1;
	_dragMode = ATOM_Widget::TitleBarDrag;
	_enableDrop = 0;
	_enableDrag = 0;
	_disabled = false;
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
	_imagelist.clear ();

	_waitingCheck = 1;	// wangjian added 默认为需要等待
}

int WidgetParameters::selectImage (void)
{
	const ATOM_Rect2Di &rc = _proxy->getPlugin()->getEditor()->getRealtimeCtrl()->getClientRect ();
	const int dialogWidth = 400;
	const int dialogHeight = 320;
	const int x = (rc.size.w - dialogWidth) / 2;
	const int y = (rc.size.h - dialogHeight) / 2;
	ATOM_Dialog *dialog = ATOM_NEW(ATOM_Dialog, _proxy->getPlugin()->getEditor()->getRealtimeCtrl(), ATOM_Rect2Di(x, y, dialogWidth, dialogHeight));
	dialog->setLayoutType (ATOM_WidgetLayout::Vertical);
	dialog->addEndId (PARAMID_SELECTIMAGE_OK);
	dialog->addEndId (PARAMID_SELECTIMAGE_CANCEL);
	dialog->getLayout()->setGap (6);
	dialog->getLayout()->setInnerSpaceLeft(4);
	dialog->getLayout()->setInnerSpaceTop(4);
	dialog->getLayout()->setInnerSpaceRight(4);
	dialog->getLayout()->setInnerSpaceBottom(4);

	ATOM_Panel *imagePanel = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Border|ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	imagePanel->setBorderMode (ATOM_Widget::Flat);
	imagePanel->enableEventTransition (true);
	imagePanel->setLayoutType (ATOM_WidgetLayout::Horizontal);

	ATOM_Panel *buttonPanel  = ATOM_NEW(ATOM_Panel, dialog, ATOM_Rect2Di(0, 0, -100, 20), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	buttonPanel->enableEventTransition (true);
	buttonPanel->setClientImageId (-1);
	buttonPanel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	ATOM_Button *buttonNew = ATOM_NEW(ATOM_Button, buttonPanel, ATOM_Rect2Di(0, 0, 30, -100), ATOM_Widget::Border, PARAMID_SELECTIMAGE_NEW_IMAGE, ATOM_Widget::ShowNormal);
	buttonNew->setText ("新建..");
	ATOM_Button *buttonDelete = ATOM_NEW(ATOM_Button, buttonPanel, ATOM_Rect2Di(0, 0, 30, -100), ATOM_Widget::Border, PARAMID_SELECTIMAGE_DELETE_IMAGE, ATOM_Widget::ShowNormal);
	buttonDelete->setText ("删除");
	ATOM_Panel *spacer = ATOM_NEW(ATOM_Panel, buttonPanel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	spacer->setClientImageId (-1);
	ATOM_Button *buttonOk = ATOM_NEW(ATOM_Button, buttonPanel, ATOM_Rect2Di(0, 0, 30, -100), ATOM_Widget::Border, PARAMID_SELECTIMAGE_OK, ATOM_Widget::ShowNormal);
	buttonOk->setText ("确定");
	ATOM_Button *buttonCancel = ATOM_NEW(ATOM_Button, buttonPanel, ATOM_Rect2Di(0, 0, 30, -100), ATOM_Widget::Border, PARAMID_SELECTIMAGE_CANCEL, ATOM_Widget::ShowNormal);
	buttonCancel->setText ("取消");

	ATOM_ListBox *imageList = ATOM_NEW(ATOM_ListBox, imagePanel, ATOM_Rect2Di(0, 0, -100, -100), 16, ATOM_Widget::Control|ATOM_Widget::Border, PARAMID_SELECTIMAGE_IMAGELIST);
	imageList->setBorderMode (ATOM_Widget::Drop);

	ATOM_Panel *labelPanel = ATOM_NEW(ATOM_Panel, imagePanel, ATOM_Rect2Di(0, 0, 80, -100), ATOM_Widget::Control, ATOM_Widget::AnyId, ATOM_Widget::ShowNormal);
	labelPanel->setClientImageId (-1);
	labelPanel->setLayoutType (ATOM_WidgetLayout::Vertical);
	labelPanel->getLayout()->setGap (4);

	ATOM_Label *labelNormal = ATOM_NEW(ATOM_Label, labelPanel, ATOM_Rect2Di(0, 0, -100, -25), ATOM_Widget::Control|ATOM_Widget::Border, PARAMID_SELECTIMAGE_LABEL_NORMAL, ATOM_Widget::ShowNormal);
	labelNormal->setBorderMode (ATOM_Widget::Drop);
	ATOM_Label *labelHover = ATOM_NEW(ATOM_Label, labelPanel, ATOM_Rect2Di(0, 0, -100, -25), ATOM_Widget::Control|ATOM_Widget::Border, PARAMID_SELECTIMAGE_LABEL_HOVER, ATOM_Widget::ShowNormal);
	labelHover->setBorderMode (ATOM_Widget::Drop);
	ATOM_Label *labelHold = ATOM_NEW(ATOM_Label, labelPanel, ATOM_Rect2Di(0, 0, -100, -25), ATOM_Widget::Control|ATOM_Widget::Border, PARAMID_SELECTIMAGE_LABEL_HOLD, ATOM_Widget::ShowNormal);
	labelHold->setBorderMode (ATOM_Widget::Drop);
	ATOM_Label *labelDisabled = ATOM_NEW(ATOM_Label, labelPanel, ATOM_Rect2Di(0, 0, -100, -25), ATOM_Widget::Control|ATOM_Widget::Border, PARAMID_SELECTIMAGE_LABEL_DISABLED, ATOM_Widget::ShowNormal);
	labelDisabled->setBorderMode (ATOM_Widget::Drop);

	dialog->showModal ();

	return 0;
}


void WidgetParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	const char *generalProp = "通用";
	const char *layoutProp = "布局";
	const char *appearanceProp = "外观";

	propBar->addIntVar ("Id", PARAMID_WIDGET_ID, _id, false, generalProp, 0);
	propBar->addStringVar ("名称", PARAMID_WIDGET_NAME, _proxy->getName(), false, generalProp, 0);
	propBar->addStringVar ("描述", PARAMID_WIDGET_DESC, _desc.c_str(), false, generalProp, 0);
	propBar->addIntVar ("背景", PARAMID_IMAGE_ID, _imageId, false, generalProp, 0);

	propBar->addIntVar ("是否等待加载", PARAMID_WAITING_CHECK, _waitingCheck, false, generalProp, 0);			// wangjian added

	ATOMX_TBEnum borderStyleEnum;
	borderStyleEnum.addEnum ("无", -1);
	borderStyleEnum.addEnum ("平坦", ATOM_Widget::Flat);
	borderStyleEnum.addEnum ("凸起", ATOM_Widget::Raise);
	borderStyleEnum.addEnum ("陷下", ATOM_Widget::Drop);
	propBar->addEnum ("边框", PARAMID_BORDERSTYLE, _borderMode, borderStyleEnum, false, generalProp, 0);

	propBar->addBoolVar ("可拖动", PARAMID_CLIENTDRAG, (_dragMode & ATOM_Widget::ClientDrag) != 0, false, generalProp, 0);
	propBar->addBoolVar ("标题栏", PARAMID_TITLEBAR, (_style & ATOM_Widget::TitleBar) != 0, false, generalProp, 0);
	propBar->addBoolVar ("关闭按钮", PARAMID_CLOSEBUTTON, (_style & ATOM_Widget::CloseButton) != 0, false, generalProp, 0);
	propBar->addBoolVar ("控件模式", PARAMID_CONTROL, (_style & ATOM_Widget::Control) != 0, false, generalProp, 0);
	propBar->addBoolVar ("禁用", PARAMID_DISABLED, _disabled, false, generalProp, 0);
	propBar->addBoolVar ("自动布局", PARAMID_ENABLELAYOUT, (_style & ATOM_Widget::NonLayoutable) == 0, false, generalProp, 0);
	propBar->addBoolVar ("不剪裁", PARAMID_NOCLIP, (_style & ATOM_Widget::NoClip) != 0, false, generalProp, 0);
	propBar->addBoolVar ("无焦点", PARAMID_NOFOCUS, (_style & ATOM_Widget::NoFocus) != 0, false, generalProp, 0);
	propBar->addBoolVar ("竖直滚动条", PARAMID_VSCROLL, (_style & ATOM_Widget::VScroll) != 0, false, generalProp, 0);
	propBar->addBoolVar ("水平滚动条", PARAMID_HSCROLL, (_style & ATOM_Widget::HScroll) != 0, false, generalProp, 0);
	propBar->addBoolVar ("固定位置", PARAMID_NONSCROLLABLE, (_style & ATOM_Widget::NonScrollable) != 0, false, generalProp, 0);
	propBar->addBoolVar ("裁切子窗口", PARAMID_CLIPCHILDREN, (_style & ATOM_Widget::ClipChildren) != 0, false, generalProp, 0);
	propBar->addBoolVar ("左滚动条", PARAMID_LEFTSCROLL, (_style & ATOM_Widget::LeftScroll) != 0, false, generalProp, 0);
	propBar->addBoolVar ("上滚动条", PARAMID_TOPSCROLL, (_style & ATOM_Widget::TopScroll) != 0, false, generalProp, 0);
	propBar->addBoolVar ("拖放目标", PARAMID_ENABLEDROP, _enableDrop != 0, false, generalProp, 0);
	propBar->addBoolVar ("拖放源", PARAMID_ENABLEDRAG, _enableDrag != 0, false, generalProp, 0);

	ATOMX_TBEnum layoutTypeEnum;
	layoutTypeEnum.addEnum ("基本", ATOM_WidgetLayout::Basic);
	layoutTypeEnum.addEnum ("横向", ATOM_WidgetLayout::Horizontal);
	layoutTypeEnum.addEnum ("纵向", ATOM_WidgetLayout::Vertical);
	propBar->addEnum ("类型", PARAMID_LAYOUTTYPE, _layoutType, layoutTypeEnum, false, layoutProp, 0);
	propBar->addIntVar ("左间距", PARAMID_LAYOUTSPACELEFT, _layoutSpaces[0], false, layoutProp, 0);
	propBar->addIntVar ("上间距", PARAMID_LAYOUTSPACETOP, _layoutSpaces[1], false, layoutProp, 0);
	propBar->addIntVar ("右间距", PARAMID_LAYOUTSPACERIGHT, _layoutSpaces[2], false, layoutProp, 0);
	propBar->addIntVar ("下间距", PARAMID_LAYOUTSPACEBOTTOM, _layoutSpaces[3], false, layoutProp, 0);
	propBar->addIntVar ("内间距", PARAMID_LAYOUTGAP, _layoutGap, false, layoutProp, 0);
	propBar->addIntVar ("左", PARAMID_RECT_X, _rect.point.x, false, appearanceProp, 0);
	propBar->addIntVar ("上", PARAMID_RECT_Y, _rect.point.y, false, appearanceProp, 0);

	char buff[32];
	if (_rect.size.w < 0)
		sprintf (buff, "%d%%", -_rect.size.w);
	else
		sprintf (buff, "%d", _rect.size.w);
	propBar->addStringVar ("宽", PARAMID_RECT_W, buff, false, appearanceProp, 0);

	if (_rect.size.h < 0)
		sprintf (buff, "%d%%", -_rect.size.h);
	else
		sprintf (buff, "%d", _rect.size.h);
	propBar->addStringVar ("高", PARAMID_RECT_H, buff, false, appearanceProp, 0);

	propBar->addStringVar ("字体", PARAMID_FONT, _font.c_str(), false, appearanceProp, 0);
	propBar->addRGBVar ("字体颜色", PARAMID_FONT_COLOR, _fontColor.getFloatR(), _fontColor.getFloatG(), _fontColor.getFloatB(), false, appearanceProp, 0);
	propBar->addRGBAVar ("字体描边颜色", PARAMID_FONT_OUTLINE_COLOR, _fontOutLineColor.getFloatR(), _fontOutLineColor.getFloatG(), _fontOutLineColor.getFloatB(), _fontOutLineColor.getFloatA(),  false, appearanceProp, 0);
}

void WidgetParameters::apply (ATOM_Widget *widget)
{
	widget->setId (_id);
	widget->setClientImageId (_imageId);
	widget->setStyle (_style);
	if (_borderMode != -1)
	{
		widget->setBorderMode ((ATOM_Widget::BorderMode)_borderMode);
	}
	widget->setDragMode (_dragMode);
	widget->enableDrop (_enableDrop != 0);
	widget->enableDrag (_enableDrag != 0);
	widget->enable (!_disabled);
	widget->setLayoutType (_layoutType);
	widget->getLayout()->setInnerSpaceLeft (_layoutSpaces[0]);
	widget->getLayout()->setInnerSpaceTop (_layoutSpaces[1]);
	widget->getLayout()->setInnerSpaceRight (_layoutSpaces[2]);
	widget->getLayout()->setInnerSpaceBottom (_layoutSpaces[3]);
	widget->getLayout()->setGap (_layoutGap);
	widget->setFont (_proxy->getFontHandle (_font.c_str()));
	widget->setFontColor (_fontColor);
	widget->setFontOutLineColor(_fontOutLineColor);
	widget->resize (_rect);

	ATOM_AUTOPTR(ATOM_GUIImageList) il = widget->getImageList ();
	if (_imagelist.empty ())
	{
		if (il)
		{
			widget->setImageList (0);
		}
	}
	else
	{
		if (!il)
		{
			il = ATOM_NEW(ATOM_GUIImageList);
			widget->setImageList (il.get());
		}
		else
		{
			il->clear ();
		}

		for (ATOM_MAP<int,ImageInfo>::const_iterator it = _imagelist.begin(); it != _imagelist.end(); ++it)
		{
			int iid = it->first;
			const ImageInfo &info = it->second;
			switch (info.imageType)
			{
			case ATOM_GUIImage::IT_COLOR:
				il->newColorImage (iid, info.states[WST_NORMAL].color, info.states[WST_HOVER].color, info.states[WST_HOLD].color, info.states[WST_DISABLED].color);
				break;
			case ATOM_GUIImage::IT_IMAGE:
				il->newTextureImage (iid, info.states[WST_NORMAL].filename.c_str(), info.states[WST_HOVER].filename.c_str(), info.states[WST_HOLD].filename.c_str(), info.states[WST_DISABLED].filename.c_str(), info.states[WST_NORMAL].region);
				break;
			case ATOM_GUIImage::IT_IMAGE9:
				il->newTextureImage9 (iid, info.states[WST_NORMAL].filename.c_str(), info.states[WST_HOVER].filename.c_str(), info.states[WST_HOLD].filename.c_str(), info.states[WST_DISABLED].filename.c_str(), info.states[WST_NORMAL].region);
				break;
			case ATOM_GUIImage::IT_MATERIAL:
				il->newMaterialImage (iid, info.states[WST_NORMAL].filename.c_str(), info.states[WST_HOVER].filename.c_str(), info.states[WST_HOLD].filename.c_str(), info.states[WST_DISABLED].filename.c_str()
					, info.states[WST_NORMAL].material_filename.c_str(), info.states[WST_HOVER].material_filename.c_str(), info.states[WST_HOLD].filename.c_str(), info.states[WST_DISABLED].material_filename.c_str(), info.states[WST_NORMAL].region);
				break;
			}
		}
	}

	// wangjian added
	widget->enableCheckWaiting(_waitingCheck);
}

void WidgetParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	char buffer[512];

	sprintf (buffer, "%d,%d,%d,%d", _rect.point.x, _rect.point.y, _rect.size.w, _rect.size.h);
	xml->SetAttribute ("rect", buffer);

	if (_id != -1)
	{
		xml->SetAttribute ("id", _id);
	}
	xml->SetAttribute ("imageid", _imageId);
	xml->SetAttribute ("style", _style);
	xml->SetAttribute ("border", _borderMode);
	xml->SetAttribute ("dragmode", _dragMode);
	xml->SetAttribute ("dropable", _enableDrop);
	xml->SetAttribute ("dragable", _enableDrag);
	xml->SetAttribute ("disabled", _disabled);

	if (!_desc.empty ())
	{
		xml->SetAttribute ("desc", _desc.c_str());
	}

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
	if(_fontOutLineColor.getRaw() != 0)
	{
		xml->SetAttribute("outlinecolor",int(_fontOutLineColor));
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
			imagelist.InsertEndChild (image);
		}
		xml->InsertEndChild (imagelist);
	}
}

void WidgetParameters::loadFromXML (ATOM_TiXmlElement *xml)
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
					texHover = eImage->Attribute("hover");
					texHold = eImage->Attribute("hold");
					texDisabled = eImage->Attribute("disabled");
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

		// resetParameters会设置id,载入前清除掉
		int newId = -1;
		xml->QueryIntAttribute ("id", &newId);
		if (!setId (newId))
		{
			::MessageBoxA (ATOM_APP->getMainWindow(), "控件ID重复,将会被设置为-1", "ATOM3D编辑器", MB_OK|MB_ICONWARNING);
			setId (-1);
		}

		xml->QueryIntAttribute ("imageid", &_imageId);
		xml->QueryIntAttribute ("style", &_style);
		xml->QueryIntAttribute ("border", &_borderMode);
		xml->QueryIntAttribute ("dragmode", &_dragMode);
		xml->QueryIntAttribute ("dropable", &_enableDrop);
		xml->QueryIntAttribute ("dragable", &_enableDrag);
		xml->QueryIntAttribute ("disabled", &_disabled);
		if (_borderMode == -1)
			_style &= ~ATOM_Widget::Border;
		else
			_style |= ATOM_Widget::Border;

		const char *desc = xml->Attribute("desc");
		if (desc)
		{
			_desc = desc;
		}

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

		int color = 1;
		xml->QueryIntAttribute ("fontcolor", &color);
		_fontColor = color;
		color = 0;
		xml->QueryIntAttribute("outlinecolor",&color);
		_fontOutLineColor = color;

		// wangjian added
		int waitingCheck = 1;
		xml->QueryIntAttribute ("waitingCheck", &waitingCheck);
		_waitingCheck = waitingCheck;
	}
}

static unsigned toggleFlag (unsigned flag, unsigned bit, bool on)
{
	return on ? (flag | bit) : (flag & ~bit);
}

static ATOM_STRING remove_head_tail_spaces(const ATOM_STRING &s) {
	ATOM_STRING str = s;

	int n = 0;
	while ( n < int(str.length()) && (str[n] == ' ' || str[n] == '\t'))
		++n;
	str.erase(0, n);

	if ( str.length() > 0)
	{
		n = int(str.length() - 1);
		while ( n >= 0 && (str[n] == ' ' || str[n] == '\t'))
			--n;
		str.erase(n + 1);
	}

	return str;
}

static bool getValidWidthOrHeight (const char *s, int *value)
{
	ATOM_STRING s2 = remove_head_tail_spaces (s);
	if (s2.empty ())
	{
		return false;
	}

	bool percent = (s2.back() == '%');
	ATOM_STRING numPart = percent ? remove_head_tail_spaces(s2.substr(0, s2.length()-1)) : s2;
	for (int i = 0; i < numPart.length(); ++i)
	{
		if (numPart[i] < '0' || numPart[i] > '9')
		{
			return false;
		}
	}
	int n = atoi (numPart.c_str());
	*value = percent ? -n : n;
	return true;
}

bool WidgetParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_WIDGET_ID:
		{
			if (!setId (event->newValue.getI()))
			{
				event->bar->setI (event->name.c_str(), event->oldValue.getI());
				return false;
			}
			return true;
		}
	case PARAMID_WIDGET_NAME:
		if (!_proxy->getPlugin()->changeProxyName (_proxy, event->newValue.getS()))
		{
			event->bar->setS (event->name.c_str(), event->oldValue.getS());
		}
		else
		{
			_proxy->getPlugin()->updateWidgetTree (true);
		}
		return false;
	case PARAMID_WIDGET_DESC:
		_desc = event->newValue.getS();
		return true;
	case PARAMID_IMAGE_ID:
		_imageId = event->newValue.getI ();
		return true;
	case PARAMID_BORDERSTYLE:
		_borderMode = event->newValue.getI();
		_style = toggleFlag (_style, ATOM_Widget::Border, _borderMode != -1);
		return true;
	case PARAMID_CLIENTDRAG:
		_dragMode = event->newValue.getB() ? ATOM_Widget::ClientDrag : 0;
		_dragMode |= ATOM_Widget::TitleBarDrag;
		return true;
	case PARAMID_TITLEBAR:
		_style = toggleFlag (_style, ATOM_Widget::TitleBar, event->newValue.getB());
		return true;
	case PARAMID_CLOSEBUTTON:
		_style = toggleFlag (_style, ATOM_Widget::CloseButton, event->newValue.getB());
		return true;
	case PARAMID_CONTROL:
		_style = toggleFlag (_style, ATOM_Widget::Control, event->newValue.getB());
		return true;
	case PARAMID_NOCLIP:
		_style = toggleFlag (_style, ATOM_Widget::NoClip, event->newValue.getB());
		return true;
	case PARAMID_ENABLELAYOUT:
		_style = toggleFlag (_style, ATOM_Widget::NonLayoutable, !event->newValue.getB());
		return true;
	case PARAMID_DISABLED:
		_disabled = event->newValue.getB();
		return true;
	case PARAMID_NOFOCUS:
		_style = toggleFlag (_style, ATOM_Widget::NoFocus, event->newValue.getB());
		return true;
	case PARAMID_VSCROLL:
		_style = toggleFlag (_style, ATOM_Widget::VScroll, event->newValue.getB());
		return true;
	case PARAMID_HSCROLL:
		_style = toggleFlag (_style, ATOM_Widget::HScroll, event->newValue.getB());
		return true;
	case PARAMID_NONSCROLLABLE:
		_style = toggleFlag (_style, ATOM_Widget::NonScrollable, event->newValue.getB());
		return true;
	case PARAMID_CLIPCHILDREN:
		_style = toggleFlag (_style, ATOM_Widget::ClipChildren, event->newValue.getB());
		return true;
	case PARAMID_LEFTSCROLL:
		_style = toggleFlag (_style, ATOM_Widget::LeftScroll, event->newValue.getB());
		return true;
	case PARAMID_TOPSCROLL:
		_style = toggleFlag (_style, ATOM_Widget::TopScroll, event->newValue.getB());
		return true;
	case PARAMID_ENABLEDROP:
		_enableDrop = event->newValue.getB() ? 1 : 0;
		return true;
	case PARAMID_ENABLEDRAG:
		_enableDrag = event->newValue.getB() ? 1 : 0;
		return true;
	case PARAMID_LAYOUTTYPE:
		_layoutType = (ATOM_WidgetLayout::Type)event->newValue.getI();
		return true;
	case PARAMID_LAYOUTSPACELEFT:
		_layoutSpaces[0] = event->newValue.getI();
		return true;
	case PARAMID_LAYOUTSPACETOP:
		_layoutSpaces[1] = event->newValue.getI();
		return true;
	case PARAMID_LAYOUTSPACERIGHT:
		_layoutSpaces[2] = event->newValue.getI();
		return true;
	case PARAMID_LAYOUTSPACEBOTTOM:
		_layoutSpaces[3] = event->newValue.getI();
		return true;
	case PARAMID_LAYOUTGAP:
		_layoutGap = event->newValue.getI();
		return true;
	case PARAMID_RECT_X:
		if (event->newValue.getI() != event->oldValue.getI())
		{
			_rect.point.x = event->newValue.getI();
			return true;
		}
		else
		{
			return false;
		}
	case PARAMID_RECT_Y:
		if (event->newValue.getI() != event->oldValue.getI())
		{
			_rect.point.y = event->newValue.getI();
			return true;
		}
		else
		{
			return false;
		}
	case PARAMID_RECT_W:
		{
			if (strcmp(event->newValue.getS(), event->oldValue.getS()))
			{
				ATOM_STRING s = event->newValue.getS();
				int value;
				if (!getValidWidthOrHeight (s.c_str(), &value))
				{
					event->bar->setS (event->name.c_str(), event->oldValue.getS());
				}
				else
				{
					_rect.size.w = value;
				}
				return true;
			}
			else
			{
				return false;
			}
		}
	case PARAMID_RECT_H:
		{
			if (strcmp(event->newValue.getS(), event->oldValue.getS()))
			{
				ATOM_STRING s = remove_head_tail_spaces (event->newValue.getS());
				int value;
				if (!getValidWidthOrHeight (s.c_str(), &value))
				{
					event->bar->setS (event->name.c_str(), event->oldValue.getS());
				}
				else
				{
					_rect.size.h = value;
				}
				return true;
			}
			else
			{
				return false;
			}
		}
	case PARAMID_FONT:
		_font = event->newValue.getS();
		return true;
	case PARAMID_FONT_COLOR:
		_fontColor.setFloats (event->newValue.get3F()[0], event->newValue.get3F()[1], event->newValue.get3F()[2], 1.f);
		return true;
	case PARAMID_FONT_OUTLINE_COLOR:
		_fontOutLineColor.setFloats(event->newValue.get3F()[0], event->newValue.get3F()[1], event->newValue.get3F()[2],event->newValue.get3F()[3] );
		return true;

	// wangjian added
	case PARAMID_WAITING_CHECK:
		_waitingCheck = event->newValue.getI ();
		return true;

	default:
		return false;
	}
}

bool WidgetParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return false;
}

const ATOM_Rect2Di &WidgetParameters::rect (void) const
{
	return _rect;
}

ATOM_Rect2Di &WidgetParameters::rect (void)
{
	return _rect;
}

int WidgetParameters::getId (void) const
{
	return _id;
}

bool WidgetParameters::setId (int id)
{
	if (id != _id)
	{
		if (id != -1 && IdManager::findId (id))
		{
			return false;
		}

		if (_id != -1)
		{
			ATOM_ASSERT(IdManager::findId (_id));
			IdManager::removeId (_id);
		}

		_id = id;

		IdManager::addId (_id);
	}

	return true;
}

int WidgetParameters::imageId (void) const
{
	return _imageId;
}

int &WidgetParameters::imageId (void)
{
	return _imageId;
}

int WidgetParameters::borderMode (void) const
{
	return _borderMode;
}

int &WidgetParameters::borderMode (void)
{
	return _borderMode;
}

int WidgetParameters::dragMode (void) const
{
	return _dragMode;
}

int &WidgetParameters::dragMode (void)
{
	return _dragMode;
}

int WidgetParameters::enableDrop (void) const
{
	return _enableDrop;
}

int &WidgetParameters::enableDrop (void)
{
	return _enableDrop;
}

int WidgetParameters::enableDrag (void) const
{
	return _enableDrag;
}

int &WidgetParameters::enableDrag (void)
{
	return _enableDrag;
}

int WidgetParameters::disabled (void) const
{
	return _disabled;
}

int &WidgetParameters::disabled (void)
{
	return _disabled;
}

int WidgetParameters::style (void) const
{
	return _style;
}

int &WidgetParameters::style (void)
{
	return _style;
}

ATOM_WidgetLayout::Type WidgetParameters::layoutType (void) const
{
	return _layoutType;
}

ATOM_WidgetLayout::Type &WidgetParameters::layoutType (void)
{
	return _layoutType;
}

int WidgetParameters::layoutInnerSpaceLeft (void) const
{
	return _layoutSpaces[0];
}

int &WidgetParameters::layoutInnerSpaceLeft (void)
{
	return _layoutSpaces[1];
}

int WidgetParameters::layoutInnerSpaceTop (void) const
{
	return _layoutSpaces[1];
}

int &WidgetParameters::layoutInnerSpaceTop (void)
{
	return _layoutSpaces[1];
}

int WidgetParameters::layoutInnerSpaceRight (void) const
{
	return _layoutSpaces[2];
}

int &WidgetParameters::layoutInnerSpaceRight (void)
{
	return _layoutSpaces[2];
}

int WidgetParameters::layoutInnerSpaceBottom (void) const
{
	return _layoutSpaces[3];
}

int &WidgetParameters::layoutInnerSpaceBottom (void)
{
	return _layoutSpaces[3];
}

int WidgetParameters::layoutGap (void) const
{
	return _layoutGap;
}

int &WidgetParameters::layoutGap (void)
{
	return _layoutGap;
}

ATOM_STRING WidgetParameters::font (void) const
{
	return _font;
}

ATOM_STRING &WidgetParameters::font (void)
{
	return _font;
}

ATOM_ColorARGB WidgetParameters::fontColor (void) const
{
	return _fontColor;
}

ATOM_ColorARGB &WidgetParameters::fontColor (void)
{
	return _fontColor;
}

const ATOM_MAP<int,WidgetParameters::ImageInfo> &WidgetParameters::imageList (void) const
{
	return _imagelist;
}

ATOM_MAP<int,WidgetParameters::ImageInfo> &WidgetParameters::imageList (void)
{
	return _imagelist;
}

ControlProxy *WidgetParameters::getProxy (void) const
{
	return _proxy;
}

void WidgetParameters::setProxy (ControlProxy *proxy)
{
	_proxy = proxy;
}

ATOM_ColorARGB WidgetParameters::fontOutLineColor( void ) const
{
	return _fontOutLineColor;
}

ATOM_ColorARGB & WidgetParameters::fontOutLineColor( void )
{
	return _fontOutLineColor;
}


// wangjian added
int WidgetParameters::waitingCheck (void) const
{
	return _waitingCheck;
}
int & WidgetParameters::waitingCheck (void)
{
	return _waitingCheck;
}
