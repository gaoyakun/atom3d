#include "stdafx.h"
#include "listbox.h"
#include "gui_canvas.h"

ATOM_BEGIN_EVENT_MAP(ATOM_ListBox, ATOM_Widget)
	ATOM_EVENT_HANDLER(ATOM_ListBox, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	ATOM_EVENT_HANDLER(ATOM_ListBox, ATOM_WidgetLButtonUpEvent, onLButtonUp)
	ATOM_EVENT_HANDLER(ATOM_ListBox, ATOM_WidgetRButtonDownEvent, onRButtonDown)
	ATOM_EVENT_HANDLER(ATOM_ListBox, ATOM_WidgetRButtonUpEvent, onRButtonUp)
	ATOM_EVENT_HANDLER(ATOM_ListBox, ATOM_WidgetLButtonDblClickEvent, onLButtonDblClick)
	ATOM_EVENT_HANDLER(ATOM_ListBox, ATOM_WidgetKeyDownEvent, onKeyDown)
	ATOM_EVENT_HANDLER(ATOM_ListBox, ATOM_WidgetMouseMoveEvent, onMouseMove)
	ATOM_EVENT_HANDLER(ATOM_ListBox, ATOM_WidgetCommandEvent, onCommand)
	ATOM_EVENT_HANDLER(ATOM_ListBox, ATOM_WidgetMouseEnterEvent, onMouseEnter)
	ATOM_EVENT_HANDLER(ATOM_ListBox, ATOM_WidgetMouseLeaveEvent, onMouseLeave)
	ATOM_EVENT_HANDLER(ATOM_ListBox, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(ATOM_ListBox, ATOM_WidgetResizeEvent, onResize)
ATOM_END_EVENT_MAP

ATOM_ListBox::ATOM_ListBox()
{
	ATOM_STACK_TRACE(ATOM_ListBox::ATOM_ListBox);

	_selectIndex = -1;
	_selectImageId = ATOM_IMAGEID_LISTITEM_HILIGHT;
	_hoverIndex = -1;
	_hoverImageId = ATOM_INVALID_IMAGEID;
	_itemHeight = 0;
	_selectIndex = -1;
	_showItemCursor = 0;
	_offsetX = 0;
	_ExtendClientImage = NULL;
	_ExtendHoverImage = NULL;
	_isExHoverImage= false;
#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_ListBox::ATOM_ListBox(ATOM_Widget *parent, const ATOM_Rect2Di &rect, int itemHeight, unsigned style, int id, ATOM_Widget::ShowState showState /* = ATOM_Widget::Hide */)
	: ATOM_Widget (parent, rect, style|ATOM_Widget::ClipChildren, id, showState)
{
	ATOM_STACK_TRACE(ATOM_ListBox::ATOM_ListBox);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

	_selectIndex = -1;
	_selectImageId = ATOM_IMAGEID_LISTITEM_HILIGHT;
	_hoverIndex = -1;
	_hoverImageId = ATOM_INVALID_IMAGEID;
	_itemHeight = itemHeight;
	_selectIndex = -1;
	_showItemCursor = 0;
	_ExtendClientImage = NULL;
	_ExtendHoverImage = NULL;
	_isExHoverImage = false;
	_offsetX = 0;
	setBorderMode (ATOM_Widget::Raise);

	resize (rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_ListBox::~ATOM_ListBox (void)
{
	_ExtendClientImage = NULL;
	_ExtendHoverImage = NULL;
	ATOM_STACK_TRACE(ATOM_ListBox::~ATOM_ListBox);
}

int ATOM_ListBox::addItem(const char* str, unsigned long long userData)
{
	ATOM_STACK_TRACE(ATOM_ListBox::addItem);

	return insertItem(_items.size(), str, userData);
}

int ATOM_ListBox::insertItem(int index, const char* str, unsigned long long userData)
{
	ATOM_STACK_TRACE(ATOM_ListBox::insertItem);

	ItemVsl::iterator iter;
	if(index <= 0)
	{
		iter = _items.begin();
		index = 0;
	}
	else if(index >= _items.size())
	{
		iter = _items.end();
		index = _items.size();
	}
	else
	{
		iter = _items.begin();
		std::advance(iter, index);
	}

	ATOM_ListItem item;
	item._userData = userData;
	item._color = getFontColor();
	item._text = ATOM_HARDREF(ATOM_Text) ();
	item._text->setFont (ATOM_GUIFont::getFontHandle(getFont()));
	item._text->setCharMargin (0);
	item._text->setZValue (1.f);
	item._text->setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
	_items.insert(iter, item);
	setItemText(index, str);
	if (getShowState () != ATOM_Widget::Hide)
	{
		invalidate ();
	}

	ATOM_Size2Di canvas(_clientRect.size.w, _clientRect.size.h);
	if(_items.size()*getItemHeight() > canvas.h)
	{
		canvas.h = _items.size()*getItemHeight();
	}
	setCanvasSize (canvas);


	return index;
}

int ATOM_ListBox::insertItem(int index, const char* str, ATOM_ColorARGB _color, unsigned long long userData)
{
	ATOM_STACK_TRACE(ATOM_ListBox::insertItem);

	ItemVsl::iterator iter;
	if(index <= 0)
	{
		iter = _items.begin();
		index = 0;
	}
	else if(index >= _items.size())
	{
		iter = _items.end();
		index = _items.size();
	}
	else
	{
		iter = _items.begin();
		std::advance(iter, index);
	}

	ATOM_ListItem item;
	item._userData = userData;
	item._color = _color;
	item._text = ATOM_HARDREF(ATOM_Text) ();
	item._text->setFont (ATOM_GUIFont::getFontHandle(getFont()));
	item._text->setCharMargin (0);
	item._text->setZValue (1.f);
	item._text->setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
	_items.insert(iter, item);
	setItemText(index, str);
	if (getShowState () != ATOM_Widget::Hide)
	{
		invalidate ();
	}

	ATOM_Size2Di canvas(_clientRect.size.w, _clientRect.size.h);
	if(_items.size()*getItemHeight() > canvas.h)
	{
		canvas.h = _items.size()*getItemHeight();
	}
	setCanvasSize (canvas);

	return index;
}

void ATOM_ListBox::removeItem(int index)
{
	ATOM_STACK_TRACE(ATOM_ListBox::removeItem);

	if(index < 0 || index >= _items.size())
	{
		return;
	}
	ItemVsl::iterator iter = _items.begin();
	std::advance(iter, index);

	_items.erase(iter);
	if (getShowState () != ATOM_Widget::Hide)
	{
		invalidate ();
	}

	ATOM_Size2Di canvas(_clientRect.size.w, _clientRect.size.h);
	if(_items.size()*getItemHeight() > canvas.h)
	{
		canvas.h = _items.size()*getItemHeight();
	}
	setCanvasSize (canvas);
}

void ATOM_ListBox::clearItem()
{
	ATOM_STACK_TRACE(ATOM_ListBox::clearItem);

	_items.clear();
	if (getShowState () != ATOM_Widget::Hide)
	{
		invalidate ();
	}

	ATOM_Size2Di canvas(_clientRect.size.w, _clientRect.size.h);
	if(_items.size()*getItemHeight() > canvas.h)
	{
		canvas.h = _items.size()*getItemHeight();
	}
	setCanvasSize (canvas);
}

void ATOM_ListBox::setSelectImageId(int id)
{
	ATOM_STACK_TRACE(ATOM_ListBox::setSelectImageId);

	if (id != _selectImageId)
	{
		_selectImageId = id;
		invalidate ();
	}
}


int ATOM_ListBox::getSelectImageId() const
{
	ATOM_STACK_TRACE(ATOM_ListBox::getSelectImageId);

	return _selectImageId;
}

void ATOM_ListBox::setHoverImageId(int id)
{
	ATOM_STACK_TRACE(ATOM_ListBox::setHoverImageId);

	if (id != _hoverImageId)
	{
		_hoverImageId = id;
		invalidate ();
	}
}

int ATOM_ListBox::getHoverImageId() const
{
	ATOM_STACK_TRACE(ATOM_ListBox::getHoverImageId);

	return _hoverImageId;
}

void ATOM_ListBox::selectItem(int index, int postEvent)
{
	ATOM_STACK_TRACE(ATOM_ListBox::selectItem);

	if(index >=0 && index < _items.size())
	{
		_selectIndex = index;
		if(LBCLICK_NULL != postEvent)
		{
			if (postEvent == LBDBLCLICK_LEFT)
			{
				getParent()->queueEvent (ATOM_NEW(ATOM_ListBoxDblClickEvent, getId(), index, postEvent), ATOM_APP);
			}
			else
			{
				getParent()->queueEvent (ATOM_NEW(ATOM_ListBoxClickEvent, getId(), index, postEvent), ATOM_APP);
			}
		}
	}
	else
	{
		_selectIndex = -1;
	}
}

void ATOM_ListBox::setItemText(int index, const char* str)
{
	ATOM_STACK_TRACE(ATOM_ListBox::setItemText);

	if(index < 0 || index >= _items.size())
	{
		return;
	}
	ItemVsl::iterator iter = _items.begin();
	std::advance(iter, index);

	if(!str)
	{
		str = "";
	}


	ATOM_ListItem& item = *iter;

	if (strcmp (item._text->getString(), str))
	{
		item._text->setString (str);
		item._text->realize ();

		if (getShowState () != ATOM_Widget::Hide)
		{
			invalidate ();
		}
	}
}

void ATOM_ListBox::setItemData(int index, unsigned long long userData)
{
	ATOM_STACK_TRACE(ATOM_ListBox::setItemData);

	if(index < 0 || index >= _items.size())
	{
		return;
	}
	ItemVsl::iterator iter = _items.begin();
	std::advance(iter, index);

	ATOM_ListItem& item = *iter;
	item._userData = userData;
}

ATOM_Text* ATOM_ListBox::getItemText (int index)
{
	ATOM_STACK_TRACE(ATOM_ListBox::getItemText);

	if(index < 0 || index >= _items.size())
	{
		return NULL;
	}
	ItemVsl::iterator iter = _items.begin();
	std::advance(iter, index);

	ATOM_ListItem& item = *iter;
	return item._text.pointer;
}

int ATOM_ListBox::findString (const char *str, bool caseSensitive) const
{
	if (!str)
	{
		return -1;
	}

	int index = 0;
	for (ItemVsl::const_iterator it = _items.begin(); it != _items.end(); ++it, ++index)
	{
		ATOM_Text *text = (*it)._text.get();
		if (!text)
		{
			continue;
		}

		const char *s = text->getString();
		int r = caseSensitive ? strcmp (str, s) : stricmp (str, s);
		if (r == 0)
		{
			return index;
		}
	}

	return -1;
}

unsigned long long ATOM_ListBox::getItemData(int index)
{
	ATOM_STACK_TRACE(ATOM_ListBox::getItemData);

	if(index < 0 || index >= _items.size())
	{
		return 0;
	}
	ItemVsl::iterator iter = _items.begin();
	std::advance(iter, index);

	ATOM_ListItem& item = *iter;
	return item._userData;
}

int ATOM_ListBox::getItemHeight() const
{
	ATOM_STACK_TRACE(ATOM_ListBox::getItemHeight);

	return _itemHeight;
}

void ATOM_ListBox::setItemHeight(int height)
{
	if (_itemHeight != height)
	{
		_itemHeight = height;

		ATOM_Size2Di canvas(_clientRect.size.w, _clientRect.size.h);
		if(_items.size()*getItemHeight() > canvas.h)
		{
			canvas.h = _items.size()*getItemHeight();
		}
		setCanvasSize (canvas);
	}
}

int ATOM_ListBox::getItemCount()
{
	ATOM_STACK_TRACE(ATOM_ListBox::getItemCount);

	return (int)_items.size();
}

int ATOM_ListBox::getSelectIndex() const
{
	ATOM_STACK_TRACE(ATOM_ListBox::getSelectIndex);

	return _selectIndex;
}

void ATOM_ListBox::setItemColor(int index, ATOM_ColorARGB clr)
{
	ATOM_STACK_TRACE(ATOM_ListBox::setItemColor);

	if(index < 0 || index >= _items.size())
	{
		return;
	}
	ItemVsl::iterator iter = _items.begin();
	std::advance(iter, index);

	ATOM_ListItem& item = *iter;
	item._color = clr;
}

ATOM_ColorARGB ATOM_ListBox::getItemColor(int index) const
{
	ATOM_STACK_TRACE(ATOM_ListBox::getItemColor);

	if(index < 0 || index >= _items.size())
	{
		return getFontColor();
	}
	ItemVsl::const_iterator iter = _items.begin();
	std::advance(iter, index);

	const ATOM_ListItem& item = *iter;
	return item._color;
}

ATOM_WidgetType ATOM_ListBox::getType (void) const
{
	ATOM_STACK_TRACE(ATOM_ListBox::getType);

	return WT_LISTBOX;
}

void ATOM_ListBox::onCommand (ATOM_WidgetCommandEvent *event)
{
	if (getParent())
	{
		getParent()->queueEvent (ATOM_NEW(ATOM_WidgetCommandEvent, *event), ATOM_APP);
	}
}

void ATOM_ListBox::onMouseEnter (ATOM_WidgetMouseEnterEvent *event)
{
}

void ATOM_ListBox::onMouseLeave (ATOM_WidgetMouseLeaveEvent *event)
{
	if (_hoverIndex >= 0)
	{
		getParent()->queueEvent (ATOM_NEW(ATOM_ListBoxHoverItemChangedEvent, getId(), _hoverIndex, -1), ATOM_APP);
		_hoverIndex = -1;
	}
}

void ATOM_ListBox::onLButtonDown (ATOM_WidgetLButtonDownEvent *event)
{
	setCapture ();
	selectItem((getScrollValue().y+event->y)/_itemHeight, LBCLICK_LEFT);
}

int ATOM_ListBox::getItemByPosition (int x, int y) const
{
	int n = (getScrollValue().y + y)/_itemHeight;
	return (n >=0 && n < _items.size()) ? n : -1;
}

void ATOM_ListBox::onLButtonUp (ATOM_WidgetLButtonUpEvent *event)
{
	ATOM_STACK_TRACE(ATOM_ListBox::onLButtonUp);

	if (getRenderer()->getCapture() == this)
	{
		releaseCapture ();
	}
}

void ATOM_ListBox::onLButtonDblClick (ATOM_WidgetLButtonDblClickEvent *event)
{
	selectItem((getScrollValue().y+event->y)/_itemHeight, LBDBLCLICK_LEFT);
}

void ATOM_ListBox::onRButtonDown (ATOM_WidgetLButtonDownEvent *event)
{
	setCapture ();
	selectItem((getScrollValue().y+event->y)/_itemHeight, LBCLICK_RIGHT);
}

void ATOM_ListBox::onRButtonUp (ATOM_WidgetLButtonUpEvent *event)
{
	ATOM_STACK_TRACE(ATOM_ListBox::onRButtonUp);

	if (getRenderer()->getCapture() == this)
	{
		releaseCapture ();
	}

	getParent()->queueEvent (ATOM_NEW(ATOM_WidgetContextMenuEvent, getId(), -1, event->x, event->y), ATOM_APP);
}

void ATOM_ListBox::onKeyDown(ATOM_WidgetKeyDownEvent *event)
{
	ATOM_STACK_TRACE(ATOM_ListBox::onKeyDown);

	switch(event->key)
	{
	case KEY_UP:
		moveShowItemCursor(-1);
		break;

	case KEY_DOWN:
		moveShowItemCursor(1);
		break;
	}
}

void ATOM_ListBox::onMouseMove(ATOM_WidgetMouseMoveEvent *event)
{
	ATOM_STACK_TRACE(ATOM_ListBox::onMouseMove);

	int hoverIndex = (getScrollValue().y+event->y)/_itemHeight;
	if (hoverIndex < 0 || hoverIndex >= _items.size())
	{
		hoverIndex = -1;
	}

	if (hoverIndex != _hoverIndex)
	{
		getParent()->queueEvent (ATOM_NEW(ATOM_ListBoxHoverItemChangedEvent, getId(), _hoverIndex, hoverIndex), ATOM_APP);
		_hoverIndex = hoverIndex;
	}
}

void ATOM_ListBox::shrink (void)
{
	ATOM_WidgetLayout::Unit *unit = getLayoutUnit ();
	if (unit && unit->desired_h > 0)
	{
		int newHeight = getItemCount() * getItemHeight();
		int oldHeight = getClientRect().size.h;
		if (newHeight < oldHeight)
		{
			int h = ATOM_max2(0, unit->desired_h - (oldHeight - newHeight));
			resize (ATOM_Rect2Di(unit->desired_x, unit->desired_y, unit->desired_w, h));
		}
	}
}

void ATOM_ListBox::onResize (ATOM_WidgetResizeEvent *event)
{
	callParentHandler (event);

	ATOM_Size2Di canvas(_clientRect.size.w, _clientRect.size.h);
	if(_items.size()*getItemHeight() > canvas.h)
	{
		canvas.h = _items.size()*getItemHeight();
	}
	setCanvasSize (canvas);
}

void ATOM_ListBox::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_ListBox::onPaint);

	ATOM_GUICanvas* canvas = event->canvas;
	ATOM_WidgetState state = event->state;

	callParentHandler (event);

	int itemHeight = getItemHeight();
	int selectIndex = getSelectIndex();
	for(int i=0; i<getItemCount(); ++i)
	{
		// 绘制选中框
		if( selectIndex == i && ATOM_INVALID_IMAGEID != _selectImageId)
		{
#if 1
			ATOM_GUIImage *image = getValidImage (_selectImageId);
#else
			const ATOM_GUIImageList *imagelist = getValidImageList ();
			ATOM_GUIImage *image = imagelist->getImage (_selectImageId);
#endif
			if (!image)
			{
				image = ATOM_GUIImageList::getDefaultImageList().getImage (ATOM_IMAGEID_LISTITEM_HILIGHT);
				ATOM_ASSERT(image);
			}
			image->draw (event->state, event->canvas, ATOM_Rect2Di(0, i*itemHeight, _clientRect.size.w, itemHeight));
		}
		else if( _hoverIndex == i && ATOM_INVALID_IMAGEID != _hoverImageId)
		{
#if 1
			if(!_isExHoverImage || !_ExtendHoverImage)
			{
				_ExtendHoverImage = getValidImage (_hoverImageId);
				_isExHoverImage = false;
			}
#else
			const ATOM_GUIImageList *imagelist = getValidImageList ();
			ATOM_GUIImage *image = imagelist->getImage (_hoverImageId);
#endif
			if (!_ExtendHoverImage)
			{
				_ExtendHoverImage = ATOM_GUIImageList::getDefaultImageList().getImage (ATOM_IMAGEID_LISTITEM_HILIGHT);
				ATOM_ASSERT(_ExtendHoverImage);
			}
			_ExtendHoverImage->draw (event->state, event->canvas, ATOM_Rect2Di(0, i*itemHeight, _clientRect.size.w, itemHeight));
		}

		// 绘制文字
		ItemVsl::iterator iter = _items.begin();
		std::advance(iter, i);

		ATOM_ListItem& item = *iter;
		ATOM_Text* text = item._text.get();
		if(text)
		{
			const char* str = text->getString();
			if (strcmp (str, ""))
			{
				ATOM_GUIFont::handle f = getFont();
				ATOM_FontHandle font = ATOM_GUIFont::getFontHandle (f);
				text->setFont (font);

				int charmargin = ATOM_GUIFont::getFontCharMargin (getFont());
				ATOM_SetCharMargin (charmargin);
				int l, t, w, h;

				ATOM_CalcStringBounds (font, str, strlen(str), &l, &t, &w, &h);
				canvas->drawTextOutline (text, _offsetX, i*itemHeight+(itemHeight-h)/2-t, item._color,_widgetFontOutLineColor);
			}
		}
	}
}

void ATOM_ListBox::moveShowItemCursor(int offset)
{
	ATOM_STACK_TRACE(ATOM_ListBox::moveShowItemCursor);

	return;
	_showItemCursor += offset;

	if(_showItemCursor >= getItemCount())
	{
		_showItemCursor = getItemCount()-1;
	}
	if(_showItemCursor < 0)
	{
		_showItemCursor = 0;
	}

	if (getShowState () != ATOM_Widget::Hide)
	{
		invalidate ();
	}
}

void ATOM_ListBox::setExtendedClientImage( ATOM_GUIImage * image )
{
	_ExtendClientImage = image;
	setExClientImage(_ExtendClientImage);
}

void ATOM_ListBox::setExtendedHoverImage( ATOM_GUIImage * image )
{
	_ExtendHoverImage = image;
	_isExHoverImage = true;
}

void ATOM_ListBox::setOffset( int offsetX )
{
	_offsetX = offsetX;
}

int ATOM_ListBox::getOffset() const
{
	return _offsetX;
}

void ATOM_ListBox::setExtendedScrollImage( ATOM_GUIImage * backImage, 
	ATOM_GUIImage * sliderImage,ATOM_GUIImage * upButtonImage,ATOM_GUIImage * downButtonImage )
{
	if(_verticalScrollBar)
	{
		_verticalScrollBar->getUpButton()->setExClientImage(upButtonImage);
		_verticalScrollBar->getDownButton()->setExClientImage(downButtonImage);
		_verticalScrollBar->getSlider()->setExClientImage(backImage);
		_verticalScrollBar->getSlider()->setHandleImage(sliderImage);
	}

}






