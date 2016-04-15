#include "stdafx.h"
#include "listview.h"
#include "gui_canvas.h"


ATOM_LVColumn::ATOM_LVColumn()
{
	ATOM_STACK_TRACE(ATOM_LVColumn::ATOM_LVColumn);

	_width = 50;
}

ATOM_LVColumn::~ATOM_LVColumn()
{
	ATOM_STACK_TRACE(ATOM_LVColumn::~ATOM_LVColumn);

	clear();
}

void ATOM_LVColumn::addWidget(ATOM_Widget* widget)
{
	ATOM_STACK_TRACE(ATOM_LVColumn::addWidget);
	if(widget)
		widget->setWheelDeliver(true);
	_widgetList.push_back(widget);
}

void ATOM_LVColumn::setWidget(int index, ATOM_Widget* widget)
{
	ATOM_STACK_TRACE(ATOM_LVColumn::setWidget);

	if(index < 0 || index >= _widgetList.size())
	{
		return;
	}
	if(widget)
		widget->setWheelDeliver(true);
	WidgetList::iterator iter = _widgetList.begin();
	std::advance(iter, index);
	if(*iter)
	{
		ATOM_DELETE(*iter);
	}
	(*iter) = widget;
}

ATOM_Widget* ATOM_LVColumn::getWidget(int index)
{
	ATOM_STACK_TRACE(ATOM_LVColumn::getWidget);

	if(index < 0 || index >= _widgetList.size())
	{
		return NULL;
	}
	WidgetList::iterator iter = _widgetList.begin();
	std::advance(iter, index);
	return *iter;
}

int ATOM_LVColumn::getWidgetCount() const
{
	ATOM_STACK_TRACE(ATOM_LVColumn::getWidgetCount);

	return _widgetList.size();
}

void ATOM_LVColumn::clear()
{
	ATOM_STACK_TRACE(ATOM_LVColumn::clear);

	for(WidgetList::iterator iter = _widgetList.begin(); iter != _widgetList.end(); ++iter)
	{
		ATOM_DELETE(*iter);
	}
	_widgetList.clear();
}

void ATOM_LVColumn::setWidth(int width)
{
	ATOM_STACK_TRACE(ATOM_LVColumn::setWidth);

	_width = width;
}

int ATOM_LVColumn::getWidth() const
{
	ATOM_STACK_TRACE(ATOM_LVColumn::getWidth);

	return _width;
}

//////////////////////////////////////////////////////////////////////////


ATOM_BEGIN_EVENT_MAP(ATOM_ListView, ATOM_Widget)
ATOM_EVENT_HANDLER(ATOM_ListView, ATOM_WidgetDrawClientEvent, onPaint)
ATOM_EVENT_HANDLER(ATOM_ListView, ATOM_WidgetLButtonDownEvent, onLButtonDown)
ATOM_EVENT_HANDLER(ATOM_ListView,ATOM_WidgetMouseMoveEvent,onMouseMove)
ATOM_EVENT_HANDLER(ATOM_ListView,ATOM_WidgetMouseLeaveEvent,onMouseLeave)
ATOM_END_EVENT_MAP

ATOM_ListView::ATOM_ListView()
{
	ATOM_STACK_TRACE(ATOM_ListView::ATOM_ListView);

	_xSize = 0;
	_ySize = 0;
	_itemHeight = 50;
	_creationStyle |= ATOM_Widget::Overlay;
	_select = -1;
	_selectImage = ATOM_INVALID_IMAGEID;
	_hoverRowIndex = -1;
	_hoverRowImage = ATOM_INVALID_IMAGEID;

	_singleRowBackGroundImage = ATOM_INVALID_IMAGEID;
#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_ListView::ATOM_ListView (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
: ATOM_Widget (parent, rect, style|ATOM_Widget::Overlay|ATOM_Widget::Control|ATOM_Widget::ClipChildren, id, showState)
{
	ATOM_STACK_TRACE(ATOM_ListView::ATOM_ListView);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

	_xSize = 0;
	_ySize = 0;
	_itemHeight = 50;
	_select = -1;
	_selectImage = ATOM_INVALID_IMAGEID;
	_hoverRowIndex = -1;
	_hoverRowImage = ATOM_INVALID_IMAGEID;
	_singleRowBackGroundImage = ATOM_INVALID_IMAGEID;
	setBorderMode (ATOM_Widget::Raise);
	resize (rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_ListView::~ATOM_ListView (void)
{
	ATOM_STACK_TRACE(ATOM_ListView::~ATOM_ListView);
}

void ATOM_ListView::setColumnCount(int num)
{
	ATOM_STACK_TRACE(ATOM_ListView::setColumnCount);

	_columnList.clear();
	_columnList.resize(num);
}

int ATOM_ListView::getColumnCount()
{
	ATOM_STACK_TRACE(ATOM_ListView::getColumnCount);

	return _columnList.size();
}

void ATOM_ListView::setColumnWidth(int index, int width)
{
	ATOM_STACK_TRACE(ATOM_ListView::setColumnWidth);

	if(index < 0 || index >= _columnList.size())
	{
		return;
	}

	_columnList[index].setWidth(width);
}

int ATOM_ListView::getColumnWidth(int index)
{
	ATOM_STACK_TRACE(ATOM_ListView::getColumnWidth);

	if(index < 0 || index >= _columnList.size())
	{
		return 0;
	}

	return _columnList[index].getWidth();
}

void ATOM_ListView::clear()
{
	ATOM_STACK_TRACE(ATOM_ListView::clear);

	int size = getColumnCount();
	for(int i=0; i<size; ++i)
	{
		_columnList[i].clear();
	}
	setScrollValue (ATOM_Point2Di(0,0));
	setCanvasSize (getClientRect().size);
}

int ATOM_ListView::addRow()
{
	ATOM_STACK_TRACE(ATOM_ListView::addRow);

	int size = _columnList.size();
	if(0 == size)
	{
		return -1;
	}

	for(int i=0; i<size; ++i)
	{
		_columnList[i].addWidget(NULL);
	}

	ATOM_Size2Di canvas = _clientRect.size;
	if(getRowCount()*getItemHeight() > canvas.h)
	{
		canvas.h = getRowCount()*getItemHeight();
	}
	setCanvasSize (canvas);
	return _columnList[0].getWidgetCount()-1;
}

void ATOM_ListView::setRowCount(int num)
{
	int size = _columnList.size();
	if(0 == size)
	{
		return;
	}

	clear();

	for(int row=0; row<num; ++row)
	{
		for(int i=0; i<size; ++i)
		{
			_columnList[i].addWidget(NULL);
		}
	}

	ATOM_Size2Di canvas = _clientRect.size;
	if(getRowCount()*getItemHeight() > canvas.h)
	{
		canvas.h = getRowCount()*getItemHeight();
	}
	setCanvasSize (canvas);
}

int ATOM_ListView::getRowCount() const
{
	ATOM_STACK_TRACE(ATOM_ListView::getRowCount);

	if(_columnList.empty())
	{
		return 0;
	}

	return _columnList[0].getWidgetCount();
}

void ATOM_ListView::setWidget(int x, int y, ATOM_Widget* widget)
{
	ATOM_STACK_TRACE(ATOM_ListView::setWidget);

	if(x < 0 || x >= _columnList.size())
	{
		return;
	}

	ATOM_LVColumn* col = &_columnList[x];
	col->setWidget(y, widget);

	ATOM_Rect2Di rc(0, y*_itemHeight, col->getWidth(), _itemHeight);
	for(int i=0; i<x; ++i)
	{
		rc.point.x += _columnList[i].getWidth();
	}
	widget->resize(rc);
}

ATOM_Label* ATOM_ListView::setLabel(int x, int y, const char* str, int id, unsigned style)
{
	ATOM_STACK_TRACE(ATOM_ListView::setLabel);

	ATOM_Label* label = ATOM_NEW(ATOM_Label, this, ATOM_Rect2Di(), style, id, ATOM_Widget::ShowNormal);
	label->setText(str);
	label->setFont(getFont());
	label->setFontColor(getFontColor());
	setWidget(x, y, label);
	return label;
}

ATOM_Edit* ATOM_ListView::setEdit(int x, int y, const char* str, int id, unsigned style)
{
	ATOM_STACK_TRACE(ATOM_ListView::setEdit);

	ATOM_Edit* edit = ATOM_NEW(ATOM_Edit, this, ATOM_Rect2Di(), style, id, ATOM_Widget::ShowNormal);
	edit->setString(str);
	edit->setFont(getFont());
	edit->setFontColor(getFontColor());
	setWidget(x, y, edit);
	return edit;
}

ATOM_Button* ATOM_ListView::setButton(int x, int y, const char* str, int id, unsigned style)
{
	ATOM_STACK_TRACE(ATOM_ListView::setButton);

	ATOM_Button* button = ATOM_NEW(ATOM_Button, this, ATOM_Rect2Di(), style, id, ATOM_Widget::ShowNormal);
	button->setText(str);
	button->setFont(getFont());
	button->setFontColor(getFontColor());
	setWidget(x, y, button);
	return button;
}

ATOM_Slider* ATOM_ListView::setHSlider(int x, int y, int min, int max, int pos, int id, unsigned style)
{
	ATOM_STACK_TRACE(ATOM_ListView::setHSlider);

	ATOM_Slider* slider = ATOM_NEW(ATOM_Slider, this, ATOM_Rect2Di(), style, id, ATOM_Widget::ShowNormal);
	slider->setRange(min, max);
	slider->setPosition(pos);
	setWidget(x, y, slider);
	return slider;
}

ATOM_ScrollBar* ATOM_ListView::setHScrollBar(int x, int y, float min, float max, float pos, float step, int id, unsigned style)
{
	ATOM_STACK_TRACE(ATOM_ListView::setHScrollBar);

	ATOM_ScrollBar* bar = ATOM_NEW(ATOM_ScrollBar, this, ATOM_Rect2Di(), style, id, ATOM_Widget::ShowNormal);
	bar->setRange(min, max);
	bar->setPosition(pos);
	bar->setStep(step);
	setWidget(x, y, bar);
	return bar;
}

ATOM_Checkbox* ATOM_ListView::setCheckbox(int x, int y, bool checked, int id, unsigned style)
{
	ATOM_STACK_TRACE(ATOM_ListView::setCheckbox);

	ATOM_Checkbox* checkbox = ATOM_NEW(ATOM_Checkbox, this, ATOM_Rect2Di(), style, id, ATOM_Widget::ShowNormal);
	checkbox->setChecked(checked);
	checkbox->setFont(getFont());
	checkbox->setFontColor(getFontColor());
	setWidget(x, y, checkbox);
	return checkbox;
}

ATOM_ComboBox* ATOM_ListView::setComboBox(int x, int y, int buttonWidth, int id, unsigned style)
{
	ATOM_STACK_TRACE(ATOM_ListView::setComboBox);

	ATOM_ComboBox* comboBox = ATOM_NEW(ATOM_ComboBox, this, ATOM_Rect2Di(), buttonWidth, style, id, ATOM_Widget::ShowNormal);
	setWidget(x, y, comboBox);
	return comboBox;
}

ATOM_Widget* ATOM_ListView::getWidget(int x, int y)
{
	ATOM_STACK_TRACE(ATOM_ListView::getWidget);

	if(x < 0 || x >= _columnList.size())
	{
		return NULL;
	}

	ATOM_LVColumn* col = &_columnList[x];
	return col->getWidget(y);
}

void ATOM_ListView::setItemHeight(int height)
{
	ATOM_STACK_TRACE(ATOM_ListView::setItemHeight);

	_itemHeight = height;
}

int ATOM_ListView::getItemHeight() const
{
	ATOM_STACK_TRACE(ATOM_ListView::getItemHeight);

	return _itemHeight;
}

// 设置选择的图片
void ATOM_ListView::setSelectImage(int imageId)
{
	ATOM_STACK_TRACE(ATOM_ListView::setSelectImage);

	_selectImage = imageId;
}

// 取得选择的图片
int ATOM_ListView::getSelectImage() const
{
	ATOM_STACK_TRACE(ATOM_ListView::getSelectImage);

	return _selectImage;
}

// 选择
void ATOM_ListView::select(int index, bool postEvent)
{
	ATOM_STACK_TRACE(ATOM_ListView::select);

	int sel = (!_columnList.empty() && index >= 0 && index < _columnList[0].getWidgetCount()) ? index : -1;
	if(sel != _select)
	{
		_select = sel;
		getParent()->queueEvent (ATOM_NEW(ATOM_ListViewSelectEvent, getId(), _select), ATOM_APP);
	}
}

// 取得选择的项
int ATOM_ListView::getSelect() const
{
	ATOM_STACK_TRACE(ATOM_ListView::getSelect);

	return _select;
}

ATOM_WidgetType ATOM_ListView::getType (void) const
{
	ATOM_STACK_TRACE(ATOM_ListView::getType);

	return WT_LISTVIEW;
}

void ATOM_ListView::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_ListView::onPaint);

	callParentHandler(event);
	if(ATOM_INVALID_IMAGEID != _singleRowBackGroundImage &&  !_columnList.empty())
	{
		for(int i = 0; i < _columnList[0].getWidgetCount();++i)
		{
			ATOM_GUIImage *image = getValidImage (_singleRowBackGroundImage);
			if (image)
				image->draw (event->state, event->canvas, ATOM_Rect2Di(0, i*_itemHeight, _widgetRect.size.w, _itemHeight));
		}
	}
	if(ATOM_INVALID_IMAGEID != _selectImage && !_columnList.empty() && _select >= 0 && _select < _columnList[0].getWidgetCount())
	{
#if 1
		ATOM_GUIImage *image = getValidImage (_selectImage);
#else
		ATOM_GUIImage *image = getValidImageList()->getImage (_selectImage);
#endif
		if (image)
		{
			image->draw (event->state, event->canvas, ATOM_Rect2Di(0, _select*_itemHeight, _widgetRect.size.w, _itemHeight));
		}

	}
	if((_creationStyle&ATOM_Widget::VScroll) && _verticalScrollBar)
	{
		if(_columnList.empty()||_widgetRect.size.h > _columnList[0].getWidgetCount()*_itemHeight)
			_verticalScrollBar->show(ATOM_Widget::Hide);
		else
			_verticalScrollBar->show(ATOM_Widget::ShowNormal);
	}
	if(ATOM_INVALID_IMAGEID != _hoverRowImage && !_columnList.empty() && _hoverRowIndex >= 0 && _hoverRowIndex < _columnList[0].getWidgetCount())
	{
		ATOM_GUIImage *image = getValidImage (_hoverRowImage);

		if (image)
		{
			image->draw (WST_NORMAL, event->canvas, ATOM_Rect2Di(0, _hoverRowIndex*_itemHeight, _widgetRect.size.w, _itemHeight));
		}
	}
}

void ATOM_ListView::onLButtonDown (ATOM_WidgetLButtonDownEvent *event)
{
	ATOM_STACK_TRACE(ATOM_ListView::onLButtonDown);

	select((getScrollValue().y+event->y)/_itemHeight);
}

void ATOM_ListView::onMouseMove( ATOM_WidgetMouseMoveEvent * event )
{
	ATOM_STACK_TRACE( ATOM_ListView::onMouseMove);

	int hoverRowIndex = (getScrollValue().y+event->y)/_itemHeight;
	hoverRowIndex = (!_columnList.empty() && hoverRowIndex >= 0 && hoverRowIndex < _columnList[0].getWidgetCount()) ? hoverRowIndex : -1;
	if(hoverRowIndex != _hoverRowIndex)
	{
		getParent()->queueEvent(ATOM_NEW(ATOM_ListViewMouseEnterRowEvent,getId(),
			_hoverRowIndex,hoverRowIndex),ATOM_APP);
		_hoverRowIndex = hoverRowIndex;
	}
}

void ATOM_ListView::setHoverImage( int imageId )
{
	ATOM_STACK_TRACE(ATOM_ListView::setHoverImage);
	_hoverRowImage = imageId;
}

int ATOM_ListView::getHoverImage() const
{
	return _hoverRowImage;
}

void ATOM_ListView::setSingleRowBackGroundImageId(int imageId)
{
	_singleRowBackGroundImage = imageId;
}

int ATOM_ListView::getSingleRowBackGroundImageId() const
{
	return _singleRowBackGroundImage;
}

void ATOM_ListView::onMouseLeave( ATOM_WidgetMouseLeaveEvent *event )
{
	_hoverRowIndex = -1;
	getParent()->queueEvent(ATOM_NEW(ATOM_ListViewMouseEnterRowEvent,getId(),
		-1,-1),ATOM_APP);
}


ATOM_RichEdit* ATOM_ListView::setRichEdit( int x, int y, const char* str, int id/*=ATOM_Widget::AnyId*/, unsigned style/*=0*/ )
{
	ATOM_STACK_TRACE(ATOM_ListView::setRichEdit);
	ATOM_RichEdit* ricthedit = ATOM_NEW(ATOM_RichEdit, this, ATOM_Rect2Di(0,0,getColumnWidth(x),getItemHeight()), style, id,getItemHeight(), ATOM_Widget::ShowNormal);
	ricthedit->addXml(str);
	ricthedit->setFont(getFont());
	ricthedit->setFontColor(getFontColor());
	setWidget(x, y, ricthedit);
	return ricthedit;
}

