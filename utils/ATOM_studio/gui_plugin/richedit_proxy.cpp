#include "StdAfx.h"
#include "richedit_proxy.h"

RichEditParameters::RichEditParameters (ControlProxy *proxy): WidgetParameters(proxy)
{
	_maxHeight = 0;
	_autoSize = 0;
	_lineHeight = 20;

	imageId() = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarBackgroundImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarDownButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarSliderImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarUpButtonImageId =  ATOM_AUTOMATIC_IMAGEID;

	_buttonLength = 16;
	_sliderWidth = 16;
	_sliderLength = 16;
}

void RichEditParameters::resetParameters (void)
{
	WidgetParameters::resetParameters ();

	rect().size.w = 200;
	rect().size.h = 160;
	borderMode() = ATOM_Widget::Drop;
	style() |= ATOM_Widget::Border;
	imageId() = ATOM_IMAGEID_WINDOW_BKGROUND;

	_maxHeight = 0;
	_autoSize = 0;
	_lineHeight = 20;

	_scrollBarBackgroundImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarDownButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarSliderImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarUpButtonImageId = ATOM_AUTOMATIC_IMAGEID;

	_buttonLength = 16;
	_sliderWidth = 16;
	_sliderLength = 16;
}

WidgetParameters *RichEditParameters::clone (void) const
{
	return ATOM_NEW(RichEditParameters, *this);
}

void RichEditParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	WidgetParameters::setupTweakBar (propBar);

	propBar->addIntVar ("�߶�����", PARAMID_RICHEDIT_MAXHEIGHT, _maxHeight, false, "���ı�", 0);
	propBar->addIntVar ("�и�", PARAMID_RICHEDIT_LINE_HEIGHT, _lineHeight, false, "���ı�", 0);
	propBar->addBoolVar ("�Զ���Ӧ", PARAMID_RICHEDIT_AUTOSIZE, _autoSize!=0, false, "���ı�", 0);
//	propBar->addRGBAVar ("�����ɫ", PARAMID_RICHEDIT_OUTLINE_COLOR, _outlineColor.getFloatR(), _outlineColor.getFloatG(), _outlineColor.getFloatB(), _outlineColor.getFloatA(), false, "���ı�", 0);
	
	propBar->addIntVar ("����������ͼƬ", PARAMID_RICHEDIT_SCROLLBAR_BACKGROUND_IMAGE, _scrollBarBackgroundImageId, false, "������", 0);
	propBar->addIntVar ("�������ϰ�ťͼƬ", PARAMID_RICHEDIT_SCROLLBAR_UPBUTTON_IMAGE, _scrollBarUpButtonImageId, false, "������", 0);
	propBar->addIntVar ("�������°�ťͼƬ", PARAMID_RICHEDIT_SCROLLBAR_DOWNBUTTON_IMAGE, _scrollBarDownButtonImageId, false, "������", 0);
	propBar->addIntVar ("��������ͼƬ", PARAMID_RICHEDIT_SCROLLBAR_SLIDER_IMAGE, _scrollBarSliderImageId, false, "������", 0);
	propBar->addIntVar ("��ť����", PARAMID_RICHEDIT_SCROLLBAR_BUTTON_LENGTH, _buttonLength, false, "������", 0);
	propBar->addIntVar ("���������", PARAMID_RICHEDIT_SCROLLBAR_SLIDER_WIDTH, _sliderWidth, false, "������", 0);
	propBar->addIntVar ("�����鳤��", PARAMID_RICHEDIT_SCROLLBAR_SLIDER_LENGTH, _sliderLength, false, "������", 0);
}

void RichEditParameters::apply (ATOM_Widget *widget)
{
	WidgetParameters::apply (widget);

	ATOM_RichEdit *richedit = (ATOM_RichEdit*)widget;

	//!����༭����������ʾ���˹��������������������ʾ�����������ڱ༭
	ATOM_RichEdit::LineList * lineList = richedit->getLineList();
	for(int i = 0; i < 30; ++i)
		lineList->push_back(RichEditLine(i*20,20));
	//
	richedit->setMaxHeight (_maxHeight);
	richedit->setAutoSize (_autoSize!=0);
	richedit->setVerticalSCrollBarImageId(_scrollBarBackgroundImageId,_scrollBarSliderImageId,
		_scrollBarUpButtonImageId,_scrollBarDownButtonImageId);

	richedit->setscrollBarWidth(_sliderWidth);
	richedit->setScrollBarSliderLength(_sliderLength);
	richedit->setScrollBarButtonLength(_buttonLength);
	
}

void RichEditParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::saveToXML (xml);

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

void RichEditParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::loadFromXML (xml);

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

bool RichEditParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_RICHEDIT_MAXHEIGHT:
		_maxHeight = event->newValue.getI();
		return true;
	case PARAMID_RICHEDIT_AUTOSIZE:
		_autoSize = event->newValue.getB()?1:0;
		return true;
	case PARAMID_RICHEDIT_LINE_HEIGHT:
		_lineHeight = event->newValue.getI();
		return true;
	case PARAMID_RICHEDIT_SCROLLBAR_BACKGROUND_IMAGE:
		_scrollBarBackgroundImageId = event->newValue.getI();
		return true;
	case PARAMID_RICHEDIT_SCROLLBAR_DOWNBUTTON_IMAGE:
		_scrollBarDownButtonImageId = event->newValue.getI();
		return true;
	case PARAMID_RICHEDIT_SCROLLBAR_SLIDER_IMAGE:
		_scrollBarSliderImageId = event->newValue.getI();
		return true;
	case PARAMID_RICHEDIT_SCROLLBAR_UPBUTTON_IMAGE:
		_scrollBarUpButtonImageId = event->newValue.getI();
		return true;
	case PARAMID_RICHEDIT_SCROLLBAR_BUTTON_LENGTH:
		_buttonLength = event->newValue.getI();
		return true;
	case PARAMID_RICHEDIT_SCROLLBAR_SLIDER_WIDTH:
		_sliderWidth = event->newValue.getI();
		return true;
	case PARAMID_RICHEDIT_SCROLLBAR_SLIDER_LENGTH:
		_sliderLength= event->newValue.getI();
		return true;
	default:
		return WidgetParameters::handleBarValueChangedEvent (event);
	}
}

bool RichEditParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return WidgetParameters::handleBarCommandEvent (event);
}

//////////////////////////////////////////////////////////////////////////////////////

RichEditProxy::RichEditProxy (PluginGUI *plugin)
	: ControlProxy (plugin)
{
}

ATOM_Widget *RichEditProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_RichEdit *richedit = ATOM_NEW(ATOM_RichEdit, parent, parameters->rect(), parameters->style(), parameters->getId(), 20, ATOM_Widget::ShowNormal);
	parameters->apply (richedit);
	return richedit;
}

void RichEditProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *RichEditProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(RichEditParameters, proxy);
}

