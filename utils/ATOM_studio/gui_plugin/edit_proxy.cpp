#include "StdAfx.h"
#include "edit_proxy.h"

EditParameters::EditParameters (ControlProxy *proxy): WidgetParameters(proxy)
{
	rect().size.w = 100;
	rect().size.h = 24;
	_readonly = 0;
	_numberType = 0;
	_password = 0;
	_text = "";
	_cursorImageId = -1;
	_cursorWidth = 2;
	_cursorOffset = 0;
	_maxChar = 0;
	_offsetX = 0;
	_frontImage = 1;
}

void EditParameters::resetParameters (void)
{
	WidgetParameters::resetParameters ();

	borderMode() = ATOM_Widget::Drop;
	style() |= ATOM_Widget::Border;
	imageId() = ATOM_IMAGEID_WINDOW_BKGROUND;
	rect().size.w = 100;
	rect().size.h = 24;

	_readonly = 0;
	_numberType = 0;
	_password = 0;
	_text = "";
	_cursorImageId = -1;
	_cursorWidth = 2;
	_cursorOffset = 0;
	_maxChar = 0;
	_offsetX = 0;
	_frontImage = 1;
}

WidgetParameters *EditParameters::clone (void) const
{
	return ATOM_NEW(EditParameters, *this);
}

void EditParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	WidgetParameters::setupTweakBar (propBar);

	propBar->addBoolVar ("只读", PARAMID_EDIT_READONLY, _readonly!=0, false, "文本框", 0);
	ATOMX_TBEnum enumNumberType;
	enumNumberType.addEnum ("无", 0);
	enumNumberType.addEnum ("整数", 1);
	enumNumberType.addEnum ("浮点", 2);
	propBar->addEnum ("数字类型", PARAMID_EDIT_NUMBERTYPE, _numberType, enumNumberType, false, "文本框", 0);
	propBar->addBoolVar ("密码", PARAMID_EDIT_PASSWORD, _password!=0, false, "文本框", 0);
	propBar->addStringVar ("内容", PARAMID_EDIT_CONTENT, _text.c_str(), false, "文本框", 0);
	propBar->addIntVar ("光标图片", PARAMID_EDIT_CURSOR_IMAGEID, _cursorImageId, false, "文本框", 0);
	propBar->addIntVar ("光标宽度", PARAMID_EDIT_CURSOR_WIDTH, _cursorWidth, false, "文本框", 0);
	propBar->addIntVar ("光标偏移", PARAMID_EDIT_CURSOR_OFFSET, _cursorOffset, false, "文本框", 0);
	propBar->addIntVar ("文字偏移", PARAMID_EDIT_OFFSET_X, _offsetX, false, "文本框", 0);
	propBar->addUIntVar ("最大字符数", PARAMID_EDIT_MAX_CHAR, _maxChar, false, "文本框", 0);
	propBar->addUIntVar ("选中图片", PARAMID_EDIT_FRONTIMG, _frontImage, false, "文本框", 0);
}

void EditParameters::apply (ATOM_Widget *widget)
{
	WidgetParameters::apply (widget);

	ATOM_Edit *edit = (ATOM_Edit*)widget;
	int editType = 0;
	if (_readonly)
		editType |= EDITTYPE_READONLY;

	if (_numberType == 1)
		editType |= EDITTYPE_INTEGER;
	else if (_numberType == 2)
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

void EditParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::saveToXML (xml);

	xml->SetAttribute ("readonly", _readonly);
	xml->SetAttribute ("password", _password);
	xml->SetAttribute ("number", _numberType==1 ? 1 : 0);
	xml->SetAttribute ("fnum", _numberType==2 ? 1 : 0);
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

void EditParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::loadFromXML (xml);

	if (xml)
	{
		xml->QueryIntAttribute ("readonly", &_readonly);
		xml->QueryIntAttribute ("password", &_password);

		int iNumber=0, fNumber=0;
		xml->QueryIntAttribute ("number", &iNumber);
		xml->QueryIntAttribute ("fnum", &fNumber);
		if (iNumber)
			_numberType = 1;
		else if (fNumber)
			_numberType = 2;
		else
			_numberType = 0;

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

bool EditParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_EDIT_READONLY:
		_readonly = event->newValue.getB()?1:0;
		return true;
	case PARAMID_EDIT_CONTENT:
		_text = event->newValue.getS();
		return true;
	case PARAMID_EDIT_NUMBERTYPE:
		_numberType = event->newValue.getI();
		return true;
	case PARAMID_EDIT_PASSWORD:
		_password = event->newValue.getB()?1:0;
		return true;
	case PARAMID_EDIT_CURSOR_IMAGEID:
		_cursorImageId = event->newValue.getI();
		return true;
	case PARAMID_EDIT_CURSOR_WIDTH:
		_cursorWidth = event->newValue.getI();
		return true;
	case PARAMID_EDIT_CURSOR_OFFSET:
		_cursorOffset = event->newValue.getI();
		return true;
	case PARAMID_EDIT_OFFSET_X:
		_offsetX = event->newValue.getI();
		return true;
	case PARAMID_EDIT_MAX_CHAR:
		_maxChar = event->newValue.getI();
		return true;
	case PARAMID_EDIT_FRONTIMG:
		_frontImage = event->newValue.getI();
		return true;
	default:
		return WidgetParameters::handleBarValueChangedEvent (event);
	}
}

bool EditParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return WidgetParameters::handleBarCommandEvent (event);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EditProxy::EditProxy (PluginGUI *plugin): ControlProxy(plugin)
{
}

ATOM_Widget *EditProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_Edit *edit = ATOM_NEW(ATOM_Edit, parent, parameters->rect(), parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (edit);
	return edit;
}

void EditProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *EditProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(EditParameters, proxy);
}

