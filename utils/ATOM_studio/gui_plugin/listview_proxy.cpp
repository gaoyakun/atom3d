#include "StdAfx.h"
#include "listview_proxy.h"

ListViewParameters::ListViewParameters (ControlProxy *proxy): WidgetParameters(proxy)
{
	_columnCount = 0;
	_rowCount = 0;
	_columnWidth = 50;
	_itemHeight = 50;
	_selectedImage = -1;

	_scrollBarBackgroundImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarDownButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarSliderImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarUpButtonImageId =  ATOM_AUTOMATIC_IMAGEID;

	_singleBackGroundImage = -1;
	_hoverImage = -1;


	_buttonLength = 16;
	_sliderWidth = 16;
	_sliderLength = 16;
}

void ListViewParameters::resetParameters (void)
{
	WidgetParameters::resetParameters ();

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
	_singleBackGroundImage = -1;
	_scrollBarBackgroundImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarDownButtonImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarSliderImageId = ATOM_AUTOMATIC_IMAGEID;
	_scrollBarUpButtonImageId =  ATOM_AUTOMATIC_IMAGEID;

	_hoverImage = -1;


	_buttonLength = 16;
	_sliderWidth = 16;
	_sliderLength = 16;
}

WidgetParameters *ListViewParameters::clone (void) const
{
	return ATOM_NEW(ListViewParameters, *this);
}

void ListViewParameters::setupTweakBar (ATOMX_TweakBar *propBar)
{
	WidgetParameters::setupTweakBar (propBar);

	propBar->addUIntVar ("列数", PARAMID_LISTVIEW_COLUMN_COUNT, _columnCount, false, "表格", 0);
	propBar->addUIntVar ("行数", PARAMID_LISTVIEW_ROW_COUNT, _rowCount, false, "表格", 0);
	propBar->addUIntVar ("列宽", PARAMID_LISTVIEW_COLUMN_WIDTH, _columnWidth, false, "表格", 0);
	propBar->addUIntVar ("行高", PARAMID_LISTVIEW_ITEM_HEIGHT, _itemHeight, false, "表格", 0);
	propBar->addIntVar ("选择图片", PARAMID_LISTVIEW_SELECTED_IMAGE, _selectedImage, false, "表格", 0);

	propBar->addIntVar ("滚动条背景图片", PARAMID_LISTVIEW_SCROLLBAR_BACKGROUND_IMAGE, _scrollBarBackgroundImageId, false, "滚动条", 0);
	propBar->addIntVar ("滚动条上按钮图片", PARAMID_LISTVIEW_SCROLLBAR_UPBUTTON_IMAGE, _scrollBarUpButtonImageId, false, "滚动条", 0);
	propBar->addIntVar ("滚动条下按钮图片", PARAMID_LISTVIEW_SCROLLBAR_DOWNBUTTON_IMAGE, _scrollBarDownButtonImageId, false, "滚动条", 0);
	propBar->addIntVar ("滚动条块图片", PARAMID_LISTVIEW_SCROLLBAR_SLIDER_IMAGE, _scrollBarSliderImageId, false, "滚动条", 0);

	propBar->addIntVar ("按钮长度", PARAMID_LISTVIEW_SCROLLBAR_BUTTON_LENGTH, _buttonLength, false, "滚动条", 0);
	propBar->addIntVar ("滚动条宽度", PARAMID_LISTVIEW_SCROLLBAR_SLIDER_WIDTH, _sliderWidth, false, "滚动条", 0);
	propBar->addIntVar ("滑动块长度", PARAMID_LISTVIEW_SCROLLBAR_SLIDER_LENGTH, _sliderLength, false, "滚动条", 0);

	propBar->addIntVar ("单行背景图片", RARAMID_LISTVIEW_BKGROUND_IMGAE, _singleBackGroundImage, false, "表格", 0);
	propBar->addIntVar ("高亮图片", RARAMID_LISTVIEW_HOVER_IMAGE, _hoverImage, false, "表格", 0);


}

void ListViewParameters::apply (ATOM_Widget *widget)
{
	WidgetParameters::apply (widget);

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
	lv->setHoverImage(_hoverImage);
	lv->setSingleRowBackGroundImageId(_singleBackGroundImage);

	lv->setscrollBarWidth(_sliderWidth);
	lv->setScrollBarSliderLength(_sliderLength);
	lv->setScrollBarButtonLength(_buttonLength);
}

void ListViewParameters::saveToXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::saveToXML (xml);

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

void ListViewParameters::loadFromXML (ATOM_TiXmlElement *xml)
{
	WidgetParameters::loadFromXML (xml);

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

bool ListViewParameters::handleBarValueChangedEvent (ATOMX_TWValueChangedEvent *event)
{
	switch (event->id)
	{
	case PARAMID_LISTVIEW_COLUMN_COUNT:
		_columnCount = event->newValue.getI();
		return true;
	case PARAMID_LISTVIEW_COLUMN_WIDTH:
		_columnWidth = event->newValue.getI();
		return true;
	case PARAMID_LISTVIEW_ROW_COUNT:
		_rowCount = event->newValue.getI();
		return true;
	case PARAMID_LISTVIEW_ITEM_HEIGHT:
		_itemHeight = event->newValue.getI();
		return true;
	case PARAMID_LISTVIEW_SELECTED_IMAGE:
		_selectedImage = event->newValue.getI();
		return true;
	case PARAMID_LISTVIEW_SCROLLBAR_BACKGROUND_IMAGE:
		_scrollBarBackgroundImageId = event->newValue.getI();
		return true;
	case PARAMID_LISTVIEW_SCROLLBAR_DOWNBUTTON_IMAGE:
		_scrollBarDownButtonImageId = event->newValue.getI();
		return true;
	case PARAMID_LISTVIEW_SCROLLBAR_SLIDER_IMAGE:
		_scrollBarSliderImageId = event->newValue.getI();
		return true;
	case PARAMID_LISTVIEW_SCROLLBAR_UPBUTTON_IMAGE:
		_scrollBarUpButtonImageId = event->newValue.getI();
		return true;
	case RARAMID_LISTVIEW_BKGROUND_IMGAE:
		_singleBackGroundImage = event->newValue.getI();
		return true;
	case RARAMID_LISTVIEW_HOVER_IMAGE:
		_hoverImage = event->newValue.getI();
		return true;
	case PARAMID_LISTVIEW_SCROLLBAR_SLIDER_WIDTH:
		_sliderWidth = event->newValue.getI();
		return true;
	case PARAMID_LISTVIEW_SCROLLBAR_SLIDER_LENGTH:
		_sliderLength= event->newValue.getI();
		return true;
	case PARAMID_LISTVIEW_SCROLLBAR_BUTTON_LENGTH:
		_buttonLength = event->newValue.getI();
		return true;
	default:
		return WidgetParameters::handleBarValueChangedEvent (event);
	}
}

bool ListViewParameters::handleBarCommandEvent (ATOMX_TWCommandEvent *event)
{
	return WidgetParameters::handleBarCommandEvent (event);
}

//////////////////////////////////////////////////////////////////////////////////////

ListViewProxy::ListViewProxy (PluginGUI *plugin)
	: ControlProxy (plugin)
{
}

ATOM_Widget *ListViewProxy::createWidgetImpl (ATOM_Widget *parent, WidgetParameters *parameters)
{
	ATOM_ListView *lv = ATOM_NEW(ATOM_ListView, parent, parameters->rect(), parameters->style(), parameters->getId(), ATOM_Widget::ShowNormal);
	parameters->apply (lv);
	return lv;
}

void ListViewProxy::changeFontImpl (const char *fontName, ATOM_GUIFont::handle handleAfterChanged)
{
}

WidgetParameters *ListViewProxy::createParametersImpl (ControlProxy *proxy) const
{
	return ATOM_NEW(ListViewParameters, proxy);
}

