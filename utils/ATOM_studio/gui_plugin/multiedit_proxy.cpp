#include "StdAfx.h"
#include "multiedit_proxy.h"

MultieditParameters::MultieditParameters (ControlProxy *proxy): WidgetParameters(proxy)
{
	_readonly = 0;
	_lineHeight = 20;
	_text = "";
	_cursorImageId = -1;
	_cursorWidth = 2;
	_cursorOffset = 0;
	_maxChar = 0;

	_scrollBarBackgroundImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarDownButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarSliderImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarUpButtonImageId = ATOM_AUTOMATIC_IMAGEID;

	_buttonLength = 16;
	_sliderWidth = 16;
	_sliderLength = 16;
	_frontimage = 1;
}

void MultieditParameters::resetParameters (void)
{
	WidgetParameters::resetParameters ();

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
	_frontimage = 1;
	_scrollBarBackgroundImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarDownButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarSliderImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarUpButtonImageId = ATOM_AUTOMATIC_IMAGEID;

	_buttonLength = 16;
	_sliderWidth = 16;
	_sliderLength = 16;
}

WidgetParameters *MultieditParameters::clone (void) const
{
	return ATOM_NEW(MultieditParameters, *this);
}

void MultieditParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	WidgetParameters::setupTweakBar (propBar);

	propBar->addBoolVar ("只读", PARAMID_MULTIEDIT_READONLY, _readonly!=0, false, "多行文本框", 0);
	propBar->addIntVar ("行高", PARAMID_MULTIEDIT_LINEHEIGHT, _lineHeight, false, "多行文本框", 0);
	propBar->addStringVar ("内容", PARAMID_MULTIEDIT_CONTENT, _text.c_str(), false, "多行文本框", 0);
	propBar->addIntVar ("光标图片", PARAMID_MULTIEDIT_CURSOR_IMAGEID, _cursorImageId, false, "多行文本框", 0);
	propBar->addIntVar ("光标宽度", PARAMID_MULTIEDIT_CURSOR_WIDTH, _cursorWidth, false, "多行文本框", 0);
	propBar->addIntVar ("光标偏移", PARAMID_MULTIEDIT_CURSOR_OFFSET, _cursorOffset, false, "多行文本框", 0);
	propBar->addUIntVar ("最大字符数", PARAMID_MULTIEDIT_MAX_CHAR, _maxChar, false, "多行文本框", 0);
	propBar->addUIntVar ("选中图片", PARAMID_MULTIEDIT_FRONTIMG, _frontimage, false, "多行文本框", 0);
	propBar->addIntVar ("滚动条背景图片", PARAMID_MULTIEDIT_SCROLLBAR_BACKGROUND_IMAGE, _scrollBarBackgroundImageId, false, "滚动条", 0);
	propBar->addIntVar ("滚动条上按钮图片", PARAMID_MULTIEDIT_SCROLLBAR_UPBUTTON_IMAGE, _scrollBarUpButtonImageId, false, "滚动条", 0);
	propBar->addIntVar ("滚动条下按钮图片", PARAMID_MULTIEDIT_SCROLLBAR_DOWNBUTTON_IMAGE, _scrollBarDownButtonImageId, false, "滚动条", 0);
	propBar->addIntVar ("滚动条块图片", PARAMID_MULTIEDIT_SCROLLBAR_SLIDER_IMAGE, _scrollBarSliderImageId, false, "滚动条", 0);
	propBar->addIntVar ("按钮长度", PARAMID_MULTIEDIT_SCROLLBAR_BUTTON_LENGTH, _buttonLength, false, "滚动条", 0);
	propBar->addIntVar ("滚动条宽度", PARAMID_MULTIEDIT_SCROLLBAR_SLIDER_WIDTH, _sliderWidth, false, "滚动条", 0);
	propBar->addIntVar ("滑动块长度", PARAMID_MULTIEDIT_SCROLLBAR_SLIDER_LENGTH, _sliderLength, false, "滚动条", 0);
}

void MultieditParameters::apply (ATOM_Widget *widget)
{
	WidgetParameters::apply (widget);

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

void MultieditParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::saveToXML (xml);

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

void MultieditParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::loadFromXML (xml);

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

bool MultieditParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_MULTIEDIT_READONLY:
		_readonly = event->newValue.getB()?1:0;
		return true;
	case PARAMID_MULTIEDIT_CONTENT:
		_text = event->newValue.getS();
		return true;
	case PARAMID_MULTIEDIT_LINEHEIGHT:
		_lineHeight = event->newValue.getI();
		return true;
	case PARAMID_MULTIEDIT_CURSOR_IMAGEID:
		_cursorImageId = event->newValue.getI();
		return true;
	case PARAMID_MULTIEDIT_CURSOR_WIDTH:
		_cursorWidth = event->newValue.getI();
		return true;
	case PARAMID_MULTIEDIT_CURSOR_OFFSET:
		_cursorOffset = event->newValue.getI();
		return true;
	case PARAMID_MULTIEDIT_MAX_CHAR:
		_maxChar = event->newValue.getI();
		return true;
	case PARAMID_MULTIEDIT_SCROLLBAR_BACKGROUND_IMAGE:
		_scrollBarBackgroundImageId = event->newValue.getI();
		return true;
	case PARAMID_MULTIEDIT_SCROLLBAR_DOWNBUTTON_IMAGE:
		_scrollBarDownButtonImageId = event->newValue.getI();
		return true;
	case PARAMID_MULTIEDIT_SCROLLBAR_SLIDER_IMAGE:
		_scrollBarSliderImageId = event->newValue.getI();
		return true;
	case PARAMID_MULTIEDIT_SCROLLBAR_UPBUTTON_IMAGE:
		_scrollBarUpButtonImageId = event->newValue.getI();
		return true;
	case PARAMID_MULTIEDIT_SCROLLBAR_BUTTON_LENGTH:
		_buttonLength = event->newValue.getI();
		return true;
	case PARAMID_MULTIEDIT_SCROLLBAR_SLIDER_WIDTH:
		_sliderWidth = event->newValue.getI();
		return true;
	case PARAMID_MULTIEDIT_SCROLLBAR_SLIDER_LENGTH:
		_sliderLength= event->newValue.getI();
		return true;
	case PARAMID_MULTIEDIT_FRONTIMG:
		_frontimage= event->newValue.getI();
		return true;
	default:
		return WidgetParameters::handleBarValueChangedEvent (event);
	}
}

bool MultieditParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return WidgetParameters::handleBarCommandEvent (event);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MultieditProxy::MultieditProxy (PluginGUI *plugin): ControlProxy(plugin)
{
}

ATOM_Widget *MultieditProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_MultiEdit *edit = ATOM_NEW(ATOM_MultiEdit, parent, parameters->rect(), parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (edit);
	return edit;
}

void MultieditProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *MultieditProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(MultieditParameters, proxy);
}

