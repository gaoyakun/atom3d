#include "stdafx.h"
#include "combobox.h"
#include "gui_canvas.h"

ATOM_BEGIN_EVENT_MAP(ATOM_ComboBox, ATOM_Widget)
	ATOM_EVENT_HANDLER(ATOM_ComboBox, ATOM_WidgetCommandEvent, onCommand)
	ATOM_EVENT_HANDLER(ATOM_ComboBox, ATOM_WidgetResizeEvent, onResize)
	ATOM_EVENT_HANDLER(ATOM_ComboBox, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(ATOM_ComboBox, ATOM_ListBoxHoverItemChangedEvent, onListBoxHoverChanged)
ATOM_END_EVENT_MAP

#define LISTBOX_ID	50000

class DropDownTrigger: public ATOM_EventTrigger
{
public:
	DropDownTrigger (ATOM_ComboBox *combobox): _combobox (combobox)
	{
	}

public:
	void onLostFocus (ATOM_WidgetLostFocusEvent *event)
	{
		_combobox->getDropDownList()->show (ATOM_Widget::Hide);
	}

	void onLButtonDown (ATOM_WidgetLButtonDownEvent *event)
	{
		//callHost (event);
		//int index = _combobox->getDropDownList()->getSelectIndex();
		//if (index >= 0)
		//{
		//	_combobox->select (index);
		//	_combobox->getDropDownList()->show (ATOM_Widget::Hide);
		//}
		//setAutoCallHost (false);
	}
	void onLButtonUp (ATOM_WidgetLButtonUpEvent *event)
	{
		callHost (event);
		int index = _combobox->getDropDownList()->getSelectIndex();
		if (index >= 0)
		{
			_combobox->select (index);
			_combobox->getDropDownList()->show (ATOM_Widget::Hide);
		}
		setAutoCallHost (false);
	}
private:
	ATOM_ListBox *_listbox;
	ATOM_ComboBox *_combobox;

	ATOM_DECLARE_EVENT_MAP(DropDownTrigger, ATOM_EventTrigger)
};

ATOM_BEGIN_EVENT_MAP(DropDownTrigger, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(DropDownTrigger, ATOM_WidgetLostFocusEvent, onLostFocus)
	ATOM_EVENT_HANDLER(DropDownTrigger, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	ATOM_EVENT_HANDLER(DropDownTrigger, ATOM_WidgetLButtonUpEvent, onLButtonUp)
ATOM_END_EVENT_MAP

ATOM_ComboBox::ATOM_ComboBox()
{
	ATOM_STACK_TRACE(ATOM_ComboBox::ATOM_ComboBox);

	_buttonWidth = 12;
	_listboxHeight = 100;
	_listboxItemHeight = 12;
	_offSetX = 0;
#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif

	setInputEditImageId (1);
	setButtonImageId (1);
	setDropDownListImageId (1);
	setDropDownListHilightImageId (0);
	_buttonLength = 16;
	_sliderLength = 16;
	_sliderWidth = 16;
	_vUpButtonImage = NULL;
	_vDownButtonImage = NULL;
	_vSliderImage = NULL;
	_vSliderHandleImage =  NULL;
}

ATOM_ComboBox::ATOM_ComboBox (ATOM_Widget *parent, const ATOM_Rect2Di &rect, int buttonWidth, unsigned style, int id, ATOM_Widget::ShowState showState)
	: ATOM_Widget (parent, rect, (style & ~(ATOM_Widget::TitleBar))|ATOM_Widget::Control, id, showState)
	, _inputEdit(NULL)
	, _dropDownButton(NULL)
	, _poplist(NULL)
{
	ATOM_STACK_TRACE(ATOM_ComboBox::ATOM_ComboBox);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

	_buttonWidth = 12;
	_listboxHeight = 100;
	_listboxItemHeight = 12;
	_selectIndex = -1;
	_offSetX  = 0;
	_buttonLength = 16;
	_sliderLength = 16;
	_sliderWidth = 16;
	_vUpButtonImage = NULL;
	_vDownButtonImage = NULL;
	_vSliderImage = NULL;
	_vSliderHandleImage =  NULL;
	_isUseCbFontColor = true;
	setLayoutType (ATOM_WidgetLayout::Horizontal);
	resize (rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif

	setInputEditImageId (1);
	setButtonImageId (1);
	setDropDownListImageId (1);
	setDropDownListHilightImageId (0);
}

ATOM_ComboBox::~ATOM_ComboBox (void)
{
	ATOM_STACK_TRACE(ATOM_ComboBox::~ATOM_ComboBox);

	if(ATOM_Widget::isValidWidget(_poplist))
	{
		ATOM_EventTrigger *trigger = _poplist->getEventTrigger();
		ATOM_DELETE(trigger);
		_poplist->setEventTrigger (0);

		ATOM_DELETE(_poplist);
		_poplist = NULL;
	}
}

void ATOM_ComboBox::setButtonWidth(int w)
{
	ATOM_STACK_TRACE(ATOM_ComboBox::setButtonWidth);

	_buttonWidth = w;

	if (_dropDownButton)
	{
		_dropDownButton->resize (ATOM_Rect2Di(0, 0, _buttonWidth, -100));
	}
}

int ATOM_ComboBox::getButtonWidth() const
{
	ATOM_STACK_TRACE(ATOM_ComboBox::getButtonWidth);

	return _buttonWidth;
}

void ATOM_ComboBox::setDropDownHeight (int h)
{
	_listboxHeight = h;
}

int ATOM_ComboBox::getDropDownHeight (void) const
{
	return _listboxHeight;
}

// 取得输入的编辑框
ATOM_Edit* ATOM_ComboBox::getInputEdit()
{
	ATOM_STACK_TRACE(ATOM_ComboBox::getInputEdit);

	if(NULL == _inputEdit)
	{
		unsigned style = (getStyle() & ATOM_Widget::Border) ? ATOM_Widget::Border : 0;
		ATOM_Rect2Di rc(0, 0, -100, -100);
		_inputEdit = ATOM_NEW(ATOM_Edit, this, rc, ATOM_Widget::Control, int(ID_INPUTEDIT), ATOM_Widget::ShowNormal);
		_inputEdit->setBorderMode (ATOM_Widget::Drop);
		_inputEdit->setClientImageId (0);
		_inputEdit->setEditType (EDITTYPE_READONLY);
	}
	_inputEdit->setFontColor(_widgetFontColor);
	_inputEdit->setFontOutLineColor(_widgetFontOutLineColor);
	_inputEdit->setOffsetX(_offSetX);
	return _inputEdit;
}

// 取得下拉按钮
ATOM_Button* ATOM_ComboBox::getDropDownButton()
{
	ATOM_STACK_TRACE(ATOM_ComboBox::getDropDownButton);

	if(NULL == _dropDownButton)
	{
		unsigned style = (getStyle() & ATOM_Widget::Border) ? ATOM_Widget::Border : 0;
		_dropDownButton = ATOM_NEW(ATOM_Button, this, ATOM_Rect2Di(0, 0, _buttonWidth, -100), ATOM_Widget::Control|ATOM_Widget::NoFocus, int(ID_DROPDOWNBUTTON), ATOM_Widget::ShowNormal);
	}
	return _dropDownButton;
}

// 取得下拉列表
ATOM_ListBox* ATOM_ComboBox::getDropDownList()
{
	ATOM_STACK_TRACE(ATOM_ComboBox::getDropDownList);

	if(NULL == _poplist)
	{
		_poplist = ATOM_NEW(ATOM_ListBox, getRenderer()->getDesktop(), ATOM_Rect2Di(0, 0, 100, 100), _listboxItemHeight, ATOM_Widget::NoClip|ATOM_Widget::NonScrollable|ATOM_Widget::Popup, LISTBOX_ID, ATOM_Widget::Hide);
		_poplist->setFlags (ATOM_Widget::TopMost);
		_poplist->setClientImageId (0);
		_poplist->setEventTrigger (ATOM_NEW(DropDownTrigger, this));
		_poplist->setFontColor(_widgetFontColor);
		_poplist->setPopupRelation(this);
		_poplist->setSelectImageId(ATOM_INVALID_IMAGEID);
	}


	return _poplist;
}

void ATOM_ComboBox::show (ATOM_Widget::ShowState showState)
{
	ATOM_STACK_TRACE(ATOM_ComboBox::show);

	ATOM_Widget::show(showState);
	if(_poplist && ATOM_Widget::Hide == showState)
	{
		//_poplist->show(ATOM_Widget::Hide);
	}
}

ATOM_WidgetType ATOM_ComboBox::getType (void) const
{
	ATOM_STACK_TRACE(ATOM_ComboBox::getType);

	return WT_COMBOBOX;
}

void ATOM_ComboBox::showDropDownList (void)
{
	ATOM_ListBox *listbox = getDropDownList ();

	ATOM_Point2Di pt(0, getWidgetRect().size.h - getClientRect().point.y);
	clientToScreen (&pt);

	int heightMax = listbox->getItemHeight() * listbox->getItemCount();
	int height = heightMax ? ATOM_min2(heightMax, _listboxHeight) : _listboxHeight;
	int left = getLayout()->getInnerSpaceLeft();
	int right = getLayout()->getInnerSpaceRight();
	listbox->resize (ATOM_Rect2Di(pt.x + left, pt.y, getWidgetRect().size.w - left - right, height));
	listbox->bringToFront ();
	listbox->setFocus ();
	if (heightMax > height)
	{
		listbox->setStyle (listbox->getStyle()|ATOM_Widget::VScroll);
		listbox->setExtendedScrollImage(_vSliderImage,_vSliderHandleImage,_vUpButtonImage,_vDownButtonImage);
		listbox->setscrollBarWidth(_sliderWidth);
		listbox->setScrollBarSliderLength(_sliderLength);
		listbox->setScrollBarButtonLength(_buttonLength);
	}
	else
	{
		listbox->setStyle (listbox->getStyle()&~ATOM_Widget::VScroll);
	}
	if(_isUseCbFontColor)
	{
		for(int i  = 0; i <  _poplist->getItemCount(); ++i)
		{
			listbox->setItemColor(i,_widgetFontColor);

		}
	}
	listbox->setFontOutLineColor(_widgetFontOutLineColor);
	listbox->selectItem (-1);
	listbox->setOffset(_offSetX);
	listbox->show (ATOM_Widget::ShowNormal);
	listbox->setExtendedHoverImage(getValidImage(_listboxHilightImageId));
	listbox->setExtendedClientImage(getValidImage(_listboxImageId));
	getParent()->queueEvent (ATOM_NEW(ATOM_ComboBoxListShowEvent, getId()), ATOM_APP);
}

void ATOM_ComboBox::onCommand(ATOM_WidgetCommandEvent *event)
{
	ATOM_STACK_TRACE(ATOM_ComboBox::onCommand);

	if(ATOM_Widget::AnyId == event->id)
	{
		return;
	}

	if(ID_DROPDOWNBUTTON == event->id)
	{
		ATOM_ListBox *listbox = getDropDownList();
		if(listbox->getShowState() == ATOM_Widget::Hide)
		{
			showDropDownList ();
		}
		else
		{
			listbox->show(ATOM_Widget::Hide);
		}
	}
}

void ATOM_ComboBox::onListBoxHoverChanged (ATOM_ListBoxHoverItemChangedEvent *event)
{
	if (event->id == LISTBOX_ID)
	{
		getDropDownList()->selectItem (event->newIndex);
	}
}

int ATOM_ComboBox::getSelected (void) const
{
	return _selectIndex;
}

void ATOM_ComboBox::select (int index, bool sendEvent)
{
	if (index != _selectIndex)
	{
		_selectIndex = index;
		if (index >= 0 && index < getDropDownList()->getItemCount())
		{
			getInputEdit()->setString(getDropDownList()->getItemText (index)->getString());
			if(!_isUseCbFontColor)
				getInputEdit()->setFontColor(getDropDownList()->getItemColor(index));
		}
		else
		{
			getInputEdit()->setString("");
		}

		if (sendEvent)
		{
			getParent()->queueEvent (ATOM_NEW(ATOM_ComboBoxSelectionChangedEvent, getId(), index), ATOM_APP);
		}
	}
}

void ATOM_ComboBox::clear (void)
{
	getDropDownList ()->clearItem ();
	select (-1);
}

void ATOM_ComboBox::selectString (const char *str, bool caseSensitive)
{
	select (getDropDownList()->findString (str, caseSensitive));
}

void ATOM_ComboBox::onResize (ATOM_WidgetResizeEvent *event)
{
	ATOM_STACK_TRACE(ATOM_ComboBox::onResize);

	callParentHandler (event);

	getInputEdit()->resize(ATOM_Rect2Di(0, 0, -100, -100));
	getDropDownButton()->resize(ATOM_Rect2Di(0, 0, _buttonWidth, -100));
}

void ATOM_ComboBox::setButtonImageId(int id)
{
	_buttonImageId = id;
	getDropDownButton()->setClientImageId (id);
}

int ATOM_ComboBox::getButtonImageId (void) const
{
	return _buttonImageId;
}

void ATOM_ComboBox::setDropDownListImageId (int id)
{
	_listboxImageId = id;
	getDropDownList()->setClientImageId (id);
}

int ATOM_ComboBox::getDropDownListImageId (void) const
{
	return _listboxImageId;
}

void ATOM_ComboBox::setDropDownListHilightImageId (int id)
{
	_listboxHilightImageId = id;
	getDropDownList()->setHoverImageId (id);
}

int ATOM_ComboBox::getDropDownListHilightImageId (void) const
{
	return _listboxHilightImageId;
}

void ATOM_ComboBox::setInputEditImageId (int id)
{
	_editImageId = id;
	getInputEdit()->setClientImageId (id);
}

int ATOM_ComboBox::getInputEditImageId (void) const
{
	return _editImageId;
}

void ATOM_ComboBox::setDropDownListItemHeight (int height)
{
	_listboxItemHeight = height;

	if (_poplist)
	{
		_poplist->setItemHeight (_listboxItemHeight);
	}
}

int ATOM_ComboBox::getDropDownListItemHeight (void) const
{
	return _listboxItemHeight;
}

void ATOM_ComboBox::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_Widget::onPaint (event);
}

void ATOM_ComboBox::setOffsetX( int x )
{
	_offSetX = x;
}

int ATOM_ComboBox::getOffsetX() const
{
	return _offSetX;
}

void ATOM_ComboBox::setscrollBarWidth( int w )
{
	_sliderWidth = w;

}

void ATOM_ComboBox::setVerticalSCrollBarImageId( int backImageId, int sliderImageId, int upButtonImageId, int downButtonImageId )
{
	_vUpButtonImage = getValidImage(upButtonImageId);
	_vDownButtonImage = getValidImage(downButtonImageId);
	_vSliderImage = getValidImage(backImageId);
	_vSliderHandleImage =  getValidImage(sliderImageId);
}

void ATOM_ComboBox::setScrollBarSliderLength( int len )
{
	_sliderLength = len;
}

void ATOM_ComboBox::setScrollBarButtonLength( int len )
{

	_buttonLength = len;
}

int ATOM_ComboBox::insertItem( int index, const char* str, unsigned long long userData /*= 0*/ )
{
	ATOM_ListBox * listbox = getDropDownList();
	if(listbox)
		listbox->insertItem(index,str,_widgetFontColor,userData);
	return index;
}

void ATOM_ComboBox::setItemText(int index, const char *str)
{
	if (!str)
	{
		str = "";
	}

	ATOM_ListBox * listbox = getDropDownList();
	if(listbox)
	{
		listbox->setItemText(index, str);
		if (_selectIndex == index)
		{
			getInputEdit()->setString (str);
		}
	}
}

int ATOM_ComboBox::insertItem( int index, const char* str, ATOM_ColorARGB _color, unsigned long long userData /*= 0*/ )
{
	ATOM_ListBox * listbox = getDropDownList();
	if(listbox)
		listbox->insertItem(index,str,_color,userData);
	return index;
}

void ATOM_ComboBox::setIsUseComboxFontColor( bool bUse )
{
	_isUseCbFontColor = bUse;
}

