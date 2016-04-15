#include "stdafx.h"
#include "treectrl.h"
#include "gui_canvas.h"

ATOM_BEGIN_EVENT_MAP(ATOM_TreeItem, ATOM_Widget)
	ATOM_EVENT_HANDLER(ATOM_TreeItem, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	ATOM_EVENT_HANDLER(ATOM_TreeItem, ATOM_WidgetLButtonUpEvent, onLButtonUp)
	ATOM_EVENT_HANDLER(ATOM_TreeItem, ATOM_WidgetRButtonDownEvent, onRButtonDown)
	ATOM_EVENT_HANDLER(ATOM_TreeItem, ATOM_WidgetRButtonUpEvent, onRButtonUp)
	ATOM_EVENT_HANDLER(ATOM_TreeItem, ATOM_WidgetMButtonDownEvent, onMButtonDown)
	ATOM_EVENT_HANDLER(ATOM_TreeItem, ATOM_WidgetMButtonUpEvent, onMButtonUp)
	ATOM_EVENT_HANDLER(ATOM_TreeItem, ATOM_WidgetMouseWheelEvent, onMouseWheel)
	ATOM_EVENT_HANDLER(ATOM_TreeItem, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(ATOM_TreeItem, ATOM_WidgetResizeEvent, onResize)
	ATOM_EVENT_HANDLER(ATOM_TreeItem, ATOM_WidgetDragStartEvent, onDragStart)
	ATOM_EVENT_HANDLER(ATOM_TreeItem, ATOM_WidgetDragOverEvent, onDragOver)
	ATOM_EVENT_HANDLER(ATOM_TreeItem, ATOM_WidgetDragDropEvent, onDragDrop)
	ATOM_EVENT_HANDLER(ATOM_TreeItem, ATOM_WidgetLButtonDblClickEvent, onLeftDblClick)
	ATOM_UNHANDLED_EVENT_HANDLER(ATOM_TreeItem, onUnhandled)
ATOM_END_EVENT_MAP

ATOM_TreeItem::ATOM_TreeItem()
{
	ATOM_STACK_TRACE(ATOM_TreeItem::ATOM_TreeItem);

	_clientImageId = ATOM_IMAGEID_TREEITEM_HILIGHT;
	_text = NULL;
	_textDirty = false;
	_textPosition.x = 0;
	_textPosition.y = 0;

	_iconSize = 0;
	_root = NULL;
	_expand = true;
	_select = false;
	_index = 0;
	_isAlwaysShowClientImage = false;
	//_outlineColor.setRaw(0);
	setLayoutable (false);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_TreeItem::ATOM_TreeItem (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_TreeItem* root, ATOM_Widget::ShowState showState)
:
ATOM_Widget (parent, rect, (style & ~ATOM_Widget::TitleBar)|ATOM_Widget::Control|ATOM_Widget::NonLayoutable, id, showState)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::ATOM_TreeItem);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

	_clientImageId = ATOM_IMAGEID_TREEITEM_HILIGHT;

	_text = ATOM_HARDREF(ATOM_Text) ();
	_text->setFont (ATOM_GUIFont::getFontHandle(getFont()));
	_text->setCharMargin (ATOM_GUIFont::getFontCharMargin(getFont()));
	_text->setZValue (1.f);
	_text->setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
	_textDirty = false;
	_textPosition.x = 0;
	_textPosition.y = 0;
	_offset = 0;
	_iconSize = 0;
	_root = root;
	_expand = true;
	_select = false;
	_isAlwaysShowClientImage = false;
	_index = (parent->getType() == WT_TREECTRL) ? ((ATOM_TreeCtrl*)parent)->getNextIndex() : 0;

	//_outlineColor.setRaw(0);

	setBorderMode (ATOM_Widget::Raise);

	resize (rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_TreeItem::~ATOM_TreeItem (void)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::~ATOM_TreeItem);
}

void ATOM_TreeItem::setOffset (int offset)
{
	_offset = offset;
}

int ATOM_TreeItem::getOffset (void) const
{
	return _offset;
}

int ATOM_TreeItem::getIndex (void) const
{
	return _index;
}

void ATOM_TreeItem::changeRoot (ATOM_TreeItem *newRoot)
{
	if (_root != newRoot)
	{
		if (_root)
		{
			_root->remove (this);
		}

		if (newRoot)
		{
			newRoot->append (this);
		}

		ATOM_TreeCtrl *treectrl = (ATOM_TreeCtrl*)getParent();
		treectrl->calcLayout ();
	}
}

void ATOM_TreeItem::show (ATOM_Widget::ShowState showState)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::show);

	ATOM_Widget::show(showState);
	if(showState == ATOM_Widget::ShowNormal)
	{
		for(ATOM_LIST<ATOM_TreeItem*>::iterator iter = _children.begin(); iter != _children.end(); ++iter)
		{
			(*iter)->show(_expand ? ATOM_Widget::ShowNormal : ATOM_Widget::Hide);
		}
	}
	else
	{
		for(ATOM_LIST<ATOM_TreeItem*>::iterator iter = _children.begin(); iter != _children.end(); ++iter)
		{
			(*iter)->show(showState);
		}
	}
}

void ATOM_TreeItem::select(bool sel)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::select);

	_select = sel;
}

void ATOM_TreeItem::calcTextLayout (void)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::calcTextLayout);

	ATOM_FontHandle font = ATOM_GUIFont::getFontHandle (getFont());
	int charmargin = ATOM_GUIFont::getFontCharMargin (getFont());
	ATOM_SetCharMargin (charmargin);
	int l, t, w, h;
	unsigned clip = ATOM_ClipString (font, _caption.c_str(), _clientRect.size.w, &l, &t, &w, &h);
	_textPosition.x = _iconSize ? _clientRect.size.h : 0;//(_clientRect.size.w - w) / 2;
	_textPosition.y = (_clientRect.size.h - h) / 2 - t;
	_text->setString (_caption.substr(0, clip).c_str());
}

void ATOM_TreeItem::setText (const char *str)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::setText);

	_caption = str ? str : "";
	_textDirty = true;
	invalidate ();
}

ATOM_Text *ATOM_TreeItem::getText (void) const
{
	ATOM_STACK_TRACE(ATOM_TreeItem::getText);

	return _text.get();
}

void ATOM_TreeItem::setFont (ATOM_GUIFont::handle font)
{
	ATOM_Widget::setFont (font);

	_text->setFont (ATOM_GUIFont::getFontHandle (getFont()));
	_text->setCharMargin (ATOM_GUIFont::getFontCharMargin (getFont()));
	_textDirty = true;
}

void ATOM_TreeItem::setOutlineColor(ATOM_ColorARGB color)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::setOutlineColor);

	_widgetFontOutLineColor = color;
}

ATOM_ColorARGB ATOM_TreeItem::getOutlineColor() const
{
	ATOM_STACK_TRACE(ATOM_TreeItem::getOutlineColor);

	return _widgetFontOutLineColor;
}

void ATOM_TreeItem::setIconSize(int size)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::setIconSize);

	_iconSize = size;
	_textDirty = true;
}

int ATOM_TreeItem::getIconSize() const
{
	ATOM_STACK_TRACE(ATOM_TreeItem::getIconSize);

	return _iconSize;
}

void ATOM_TreeItem::setExpandImageId(int imageId)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::setExpandImageId);

	_expandImageId = imageId;
	_textDirty = true;
}

int ATOM_TreeItem::getExpandImageId() const
{
	ATOM_STACK_TRACE(ATOM_TreeItem::getExpandImageId);

	return _expandImageId;
}

void ATOM_TreeItem::setUnexpandImageId(int imageId)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::setUnexpandImageId);

	_unexpandImageId = imageId;
	_textDirty = true;
}

int ATOM_TreeItem::getUnexpandImageId() const
{
	ATOM_STACK_TRACE(ATOM_TreeItem::getUnexpandImageId);

	return _unexpandImageId;
}

void ATOM_TreeItem::append(ATOM_TreeItem* item)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::append);

	item->_iter = _children.insert (_children.end(), item);
	item->_root = this;
}

void ATOM_TreeItem::remove(ATOM_TreeItem* item)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::remove);

	if (item && item->_root == this)
	{
		item->_root = 0;
		_children.erase (item->_iter);
	}
}

void ATOM_TreeItem::expand(bool val, bool postEvent)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::expand);

	if(_expand != val)
	{
		_expand = val;
		for(ATOM_LIST<ATOM_TreeItem*>::iterator iter = _children.begin(); iter != _children.end(); ++iter)
		{
			(*iter)->show(_expand ? ATOM_Widget::ShowNormal : ATOM_Widget::Hide);
		}
	}

	if(postEvent)
	{
		ATOM_TreeCtrl *parent = (ATOM_TreeCtrl*)getParent ();
		if (parent)
		{
			ATOM_TreeCtrlClickEvent e(parent->getId(), getId());
			parent->handleEvent (&e);
		}
	}
}

bool ATOM_TreeItem::getExpand() const
{
	ATOM_STACK_TRACE(ATOM_TreeItem::getExpand);

	return _expand;
}

ATOM_TreeItem* ATOM_TreeItem::getRoot()
{
	ATOM_STACK_TRACE(ATOM_TreeItem::getRoot);

	return _root;
}

// 取得子节点
ATOM_LIST<ATOM_TreeItem*>& ATOM_TreeItem::getChildren()
{
	ATOM_STACK_TRACE(ATOM_TreeItem::getChildren);

	return _children;
}

ATOM_WidgetType ATOM_TreeItem::getType (void) const
{
	return WT_TREEITEM;
}

void ATOM_TreeItem::onRButtonDown (ATOM_WidgetRButtonDownEvent *event)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::onRButtonDown);

	setBorderMode (ATOM_Widget::Drop);
	setCapture ();

	if (isMouseHover ())
	{
		ATOM_Rect2Di rc(_offset + (_clientRect.size.h - _iconSize)/2, (_clientRect.size.h - _iconSize)/2, _iconSize, _iconSize);
		if (!rc.isPointIn (event->x, event->y))
		{
			onRClicked ();
		}
	}
}

void ATOM_TreeItem::onRButtonUp (ATOM_WidgetRButtonUpEvent *event)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::onRButtonUp);

	if (getRenderer()->getCapture() == this)
	{
		setBorderMode (ATOM_Widget::Raise);
		releaseCapture ();

		if (isMouseHover ())
		{
			getParent()->getParent()->queueEvent (ATOM_NEW(ATOM_WidgetContextMenuEvent, getParent()->getId(), getIndex(), event->x, event->y), ATOM_APP);
		}
	}
}

void ATOM_TreeItem::onMButtonDown (ATOM_WidgetMButtonDownEvent *event)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::onMButtonDown);

	setBorderMode (ATOM_Widget::Drop);
	setCapture ();
}

void ATOM_TreeItem::onMButtonUp (ATOM_WidgetMButtonUpEvent *event)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::onMButtonUp);

	if (getRenderer()->getCapture() == this)
	{
		setBorderMode (ATOM_Widget::Raise);
		releaseCapture ();

		if (isMouseHover ())
		{
			ATOM_Rect2Di rc(_offset + (_clientRect.size.h - _iconSize)/2, (_clientRect.size.h - _iconSize)/2, _iconSize, _iconSize);
			if (!rc.isPointIn (event->x, event->y))
			{
				onMClicked ();
			}
		}
	}
}

void ATOM_TreeItem::onLButtonDown (ATOM_WidgetLButtonDownEvent *event)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::onLButtonDown);

	setBorderMode (ATOM_Widget::Drop);
	setCapture ();

	if (isMouseHover ())
	{
		ATOM_Rect2Di rc(_offset + (_clientRect.size.h - _iconSize)/2, (_clientRect.size.h - _iconSize)/2, _iconSize, _iconSize);
		if (rc.isPointIn (event->x, event->y))
		{
			expand (!getExpand (), false);
			ATOM_TreeCtrl *treectrl = (ATOM_TreeCtrl*)getParent();
			treectrl->calcLayout ();
		}
		else
		{
			onClicked ();
		}
	}
}

void ATOM_TreeItem::onLeftDblClick (ATOM_WidgetLButtonDblClickEvent *event)
{
	if (!_children.empty ())
	{
		expand (!getExpand (), false);
		ATOM_TreeCtrl *treectrl = (ATOM_TreeCtrl*)getParent();
		treectrl->calcLayout ();
	}

	ATOM_TreeCtrl *parent = (ATOM_TreeCtrl*)getParent ();
	if (parent)
	{
		ATOM_TreeCtrlDblClickEvent e(parent->getId(), getIndex());
		parent->handleEvent (&e);
	}
}

void ATOM_TreeItem::onLButtonUp (ATOM_WidgetLButtonUpEvent *event)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::onLButtonUp);

	if (getRenderer()->getCapture() == this)
	{
		setBorderMode (ATOM_Widget::Raise);
		releaseCapture ();
	}
}

void ATOM_TreeItem::onDragStart (ATOM_WidgetDragStartEvent *event)
{
	ATOM_TreeCtrl *treeCtrl = (ATOM_TreeCtrl*)_parent;
	if (treeCtrl && treeCtrl->isItemDragDropAllowed())
	{
		ATOM_TreeItemDragStartEvent e(treeCtrl->getId(), event->dragSource.get(), getIndex(), true);
		treeCtrl->handleEvent (&e);

		event->allow = e.allow;
	}
	else
	{
		event->allow = false;
	}
}

void ATOM_TreeItem::onDragOver (ATOM_WidgetDragOverEvent *event)
{
	ATOM_TreeCtrl *treeCtrl = (ATOM_TreeCtrl*)_parent;
	if (treeCtrl && treeCtrl->isItemDragDropAllowed())
	{
		ATOM_TreeItemDragOverEvent e(treeCtrl->getId(), event->dragSource.get(), getIndex(), true);
		treeCtrl->handleEvent (&e);
		event->accept = e.accept;
	}
}

void ATOM_TreeItem::onDragDrop (ATOM_WidgetDragDropEvent *event)
{
	ATOM_TreeCtrl *treeCtrl = (ATOM_TreeCtrl*)_parent;
	if (treeCtrl && treeCtrl->isItemDragDropAllowed())
	{
		ATOM_TreeItemDragDropEvent e(treeCtrl->getId(), event->dragSource.get(), getIndex(), event->keymod);
		treeCtrl->handleEvent (&e);
	}
}

void ATOM_TreeItem::onUnhandled (ATOM_Event *event)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::onUnhandled);

	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->handleEvent (event);
	}
}

void ATOM_TreeItem::onMouseWheel (ATOM_WidgetMouseWheelEvent *event)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::onMouseWheel);

	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		event->x += getWidgetRect().point.x;
		event->y += getWidgetRect().point.y;
		parent->handleEvent (event);
	}
}

void ATOM_TreeItem::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::onPaint);

	// 选中了才画背景
	if(_select || _isAlwaysShowClientImage)
	{
		callParentHandler (event);
	}

	int imageId = _expand ? _expandImageId : _unexpandImageId;
	if (imageId != ATOM_INVALID_IMAGEID && !_children.empty ())
	{
#if 1
		ATOM_GUIImage *image = getValidImage (imageId);
#else
		const ATOM_GUIImageList *imagelist = getValidImageList ();
		ATOM_GUIImage *image = imagelist->getImage (imageId);
#endif
		if (!image)
		{
			image = ATOM_GUIImageList::getDefaultImageList().getImage (ATOM_IMAGEID_WINDOW_BKGROUND);
			ATOM_ASSERT(image);
		}
		ATOM_Rect2Di rc(_offset + (_clientRect.size.h - _iconSize)/2, (_clientRect.size.h - _iconSize)/2, _iconSize, _iconSize);
		image->draw (event->state, event->canvas, rc);
	}

	if (!_caption.empty())
	{
		if (_textDirty)
		{
			calcTextLayout ();
			_textDirty = false;
		}

		if(_widgetFontOutLineColor.getByteA())
		{
			event->canvas->drawTextOutline (getText(), _textPosition.x + _offset, _textPosition.y, getFontColor(), _widgetFontOutLineColor);
		}
		else
		{
			event->canvas->drawText (getText(), _textPosition.x + _offset, _textPosition.y, getFontColor());
		}
	}
}

void ATOM_TreeItem::onClicked (void)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::onClicked);

	ATOM_Rect2Di rc((_clientRect.size.h - _iconSize)/2, (_clientRect.size.h - _iconSize)/2, _iconSize, _iconSize);

	ATOM_TreeCtrl *parent = (ATOM_TreeCtrl*)getParent ();
	if (parent)
	{
		parent->selectItem (this, true);
	}
}

void ATOM_TreeItem::onRClicked (void)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::onRClicked);

	ATOM_TreeCtrl *parent = (ATOM_TreeCtrl*)getParent ();
	if (parent)
	{
		parent->selectItem (this, true);
		parent->queueEvent (ATOM_NEW(ATOM_TreeCtrlRClickEvent, parent->getId(), getIndex()), ATOM_APP);
	}
}

void ATOM_TreeItem::onMClicked (void)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::onMClicked);

	ATOM_TreeCtrl *parent = (ATOM_TreeCtrl*)getParent ();
	if (parent)
	{
		parent->queueEvent (ATOM_NEW(ATOM_TreeCtrlMClickEvent, parent->getId(), getIndex()), ATOM_APP);
	}
}

void ATOM_TreeItem::onResize (ATOM_WidgetResizeEvent *event)
{
	ATOM_STACK_TRACE(ATOM_TreeItem::onResize);

	callParentHandler (event);

	_textDirty = true;
}

void ATOM_TreeItem::setIsAlawysShowClientImage( bool isAlawysShowClientImage )
{
	_isAlwaysShowClientImage = isAlawysShowClientImage;
}


//////////////////////////////////////////////////////////////////////////

ATOM_BEGIN_EVENT_MAP(ATOM_TreeCtrl, ATOM_Widget)
	ATOM_EVENT_HANDLER(ATOM_TreeCtrl, ATOM_TreeCtrlClickEvent, onClick)
	ATOM_EVENT_HANDLER(ATOM_TreeCtrl, ATOM_WidgetKeyDownEvent, onKeyDown)
	ATOM_EVENT_HANDLER(ATOM_TreeCtrl, ATOM_TreeCtrlRClickEvent, onRClick)
	ATOM_EVENT_HANDLER(ATOM_TreeCtrl, ATOM_TreeCtrlMClickEvent, onMClick)
	ATOM_EVENT_HANDLER(ATOM_TreeCtrl, ATOM_TreeCtrlDblClickEvent, onDblClick)
	ATOM_EVENT_HANDLER(ATOM_TreeCtrl, ATOM_WidgetCommandEvent, onCommand)
	ATOM_EVENT_HANDLER(ATOM_TreeCtrl, ATOM_TreeItemDragStartEvent, onItemDragStart)
	ATOM_EVENT_HANDLER(ATOM_TreeCtrl, ATOM_TreeItemDragOverEvent, onItemDragOver)
	ATOM_EVENT_HANDLER(ATOM_TreeCtrl, ATOM_TreeItemDragDropEvent, onItemDragDrop)
ATOM_END_EVENT_MAP

ATOM_TreeCtrl::ATOM_TreeCtrl()
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::ATOM_TreeCtrl);

	_textImageId = ATOM_IMAGEID_TREEITEM_HILIGHT;
	_imageSize = 0;
	_lineHeight = 0;
	_indent = 0;
	_nextIndex = 0;

	_selected = NULL;
	_allowItemDragDrop = false;

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_TreeCtrl::ATOM_TreeCtrl (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, int imageSize, int lineHeight, int indent, ATOM_Widget::ShowState showState)
:
ATOM_Widget (parent, rect, style|ATOM_Widget::ClipChildren, id, showState)
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::ATOM_TreeCtrl);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

	_textImageId = ATOM_IMAGEID_TREEITEM_HILIGHT;
	_imageSize = imageSize;
	_lineHeight = lineHeight;
	_indent = indent;

	//_outlineColor.setRaw(0);

	_selected = NULL;
	_nextIndex = 0;

	resize (rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_TreeCtrl::~ATOM_TreeCtrl (void)
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::~ATOM_TreeCtrl);
}

ATOM_WidgetType ATOM_TreeCtrl::getType (void) const
{
	return WT_TREECTRL;
}

// 设置文字背景
void ATOM_TreeCtrl::setTextImageId(int imageId)
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::setTextImageId);

	_textImageId = imageId;
}

// 取得文字背景
int ATOM_TreeCtrl::getTextImageId(void) const
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::getTextImageId);

	return _textImageId;
}

void ATOM_TreeCtrl::setOutlineColor(ATOM_ColorARGB color)
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::setOutlineColor);

	_widgetFontOutLineColor = color;
}

ATOM_ColorARGB ATOM_TreeCtrl::getOutlineColor() const
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::getOutlineColor);

	return _widgetFontOutLineColor;
}

ATOM_TreeItem* ATOM_TreeCtrl::newItem(const char* str, int expandImageId, int unexpandImageId, ATOM_TreeItem* root, int id, bool expend,bool isAlwaysShowClientImage)
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::newItem);

	ATOM_TreeItem* item = NULL;
	if(root)
	{
		if(root->getParent() != this)
		{
			return NULL;
		}
		item = ATOM_NEW(ATOM_TreeItem, this, ATOM_Rect2Di(0,0,0,0), 0, id, root, ShowNormal);
		root->append(item);
	}
	else
	{
		item = ATOM_NEW(ATOM_TreeItem, this, ATOM_Rect2Di(0,0,0,0), 0, id, (ATOM_TreeItem*)NULL, ShowNormal);
		append(item);
	}
	item->enableDrag (true);
	item->enableDrop (true);
	item->setClientImageId(_textImageId);
	item->setIconSize(_imageSize);
	item->setExpandImageId(expandImageId);
	item->setUnexpandImageId(unexpandImageId);
	item->setFont(getFont());
	item->setFontColor(getFontColor());
	item->setText(str);
	item->setOutlineColor(_widgetFontOutLineColor);
	item->setIsAlawysShowClientImage(isAlwaysShowClientImage);
	if(false == expend)
	{
		item->expand(expend);
	}
	calcLayout();

	return item;
}

void ATOM_TreeCtrl::deleteItem(ATOM_TreeItem* item)
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::deleteItem);

	doDeleteItem(item);
	calcLayout();
}

void ATOM_TreeCtrl::clear()
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::clear);

	while(!_children.empty())
	{
		doDeleteItem(_children.front());
	}
	calcLayout();
}

void ATOM_TreeCtrl::selectItem(int index, bool postEvent)
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::selectItem);

	if(index < 0 || index >= _children.size())
	{
		return;
	}

	ATOM_LIST<ATOM_TreeItem*>::iterator iter = _children.begin();
	std::advance(iter, index);
	selectItem(*iter, postEvent);
}

void ATOM_TreeCtrl::selectItem(ATOM_TreeItem* item, bool postEvent)
{
#if 1
	ATOM_STACK_TRACE(ATOM_TreeCtrl::selectItem);

	if(item != _selected)
	{
		if(_selected)
		{
			_selected->select(false);
		}

		if(item)
		{
			item->select(true);
		}

		_selected = item;
	}
	ATOM_TreeItem *selected = item;
	ATOM_TreeItem *deselected = _selected;

#if 0
	if(_selected && !_selected->getChildren().empty())
	{
		calcLayout();
	}
#endif

	ATOM_Widget *parent = getParent ();
	if (postEvent && parent && item)
	{
		parent->queueEvent (ATOM_NEW(ATOM_TreeCtrlSelChangedEvent, getId(), (selected ? selected->getIndex() : -1), (deselected ? deselected->getIndex() : -1)), ATOM_APP);
	}
#else
	if(item != _selected)
	{
		if(_selected)
		{
			_selected->select(false);
		}

		if(item)
		{
			item->select(true);
		}

		_selected = item;
		ATOM_TreeItem *selected = item;
		ATOM_TreeItem *deselected = _selected;

		if(_selected && !_selected->getChildren().empty())
		{
			calcLayout();
		}

		ATOM_Widget *parent = getParent ();
		if (postEvent && parent && item)
		{
			parent->queueEvent (ATOM_NEW(ATOM_TreeCtrlSelChangedEvent, getId(), (selected ? selected->getId() : -1), (deselected ? deselected->getId() : -1)), ATOM_APP);
		}
	}
#endif
}

ATOM_TreeItem* ATOM_TreeCtrl::getSelectedItem()
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::getSelectedItem);

	return _selected;
}

void ATOM_TreeCtrl::doDeleteItem(ATOM_TreeItem* item)
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::doDeleteItem);

	//
	if(item->getParent() != this)
	{
		return;
	}

	// 先删除子控件
	while(!item->getChildren().empty() )
	{
		doDeleteItem(item->getChildren().front());
	}

	if(_selected == item)
	{
		_selected = NULL;
	}

	// 移除节点
	if( item->getRoot() )
	{
		item->getRoot()->remove(item);
	}
	else
	{
		remove(item);
	}

	// 删除
	ATOM_DELETE(item);
}

void ATOM_TreeCtrl::append(ATOM_TreeItem* item)
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::append);

	item->_iter = _children.insert (_children.end(), item);
	item->_root = 0;
}

void ATOM_TreeCtrl::remove(ATOM_TreeItem* item)
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::remove);

	if (!item->_root)
	{
		_children.erase (item->_iter);
	}
}

void ATOM_TreeCtrl::onCommand (ATOM_WidgetCommandEvent *event)
{
	if (getParent())
	{
		getParent()->queueEvent (ATOM_NEW(ATOM_WidgetCommandEvent, *event), ATOM_APP);
	}
}

void ATOM_TreeCtrl::onClick(ATOM_TreeCtrlClickEvent *event)
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::onCommand);

	selectItem(getItemByIndex (event->itemIndex));
	//if(event->widget != _selected)
	//{
	//	ATOM_TreeItem* item = dynamic_cast<ATOM_TreeItem*>(event->widget);
	//	if(item)
	//	{
	//		if(_selected)
	//		{
	//			_selected->select(false);
	//		}
	//		item->select(true);
	//		_selected = item;
	//	}
	//}

	//calcLayout();

	//ATOM_Widget *parent = getParent ();
	//if (parent)
	//{
	//	parent->queueEvent (ATOM_NEW(ATOM_WidgetCommandEvent, event->widget->getId()), ATOM_APP);
	//}
}

void ATOM_TreeCtrl::onKeyDown (ATOM_WidgetKeyDownEvent *event)
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::onKeyDown);

	if (!_selected)
	{
		return;
	}

	if (event->key == KEY_UP)
	{
		ATOM_TreeItem *itemPrev = getPrevSiblingItem (_selected);
		if (!itemPrev)
		{
			itemPrev = _selected->_root;
		}
		else
		{
			while (itemPrev->getExpand() && !itemPrev->_children.empty ())
			{
				itemPrev = itemPrev->_children.back();
			}
		}

		if (itemPrev)
		{
			selectItem (itemPrev);
		}
	}
	else if (event->key == KEY_DOWN)
	{
		ATOM_TreeItem *itemNext = 0;
		if (_selected->getExpand() && !_selected->_children.empty ())
		{
			itemNext = *(_selected->_children.begin ());
		}
		else
		{
			ATOM_TreeItem *p = _selected;

			for (;;)
			{
				itemNext = getNextSiblingItem (p);

				if (itemNext || !p)
				{
					break;
				}

				p = p->_root;
			}
		}

		if (itemNext)
		{
			selectItem (itemNext);
		}
	}
}

void ATOM_TreeCtrl::onRClick(ATOM_TreeCtrlRClickEvent *event)
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::onRClick);

	ATOM_Widget *w = getParent ();
	if (w)
	{
		w->queueEvent (ATOM_NEW(ATOM_TreeCtrlRClickEvent, *event), ATOM_APP);
	}
}

void ATOM_TreeCtrl::onMClick(ATOM_TreeCtrlMClickEvent *event)
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::onMClick);

	ATOM_Widget *w = getParent ();
	if (w)
	{
		w->queueEvent (ATOM_NEW(ATOM_TreeCtrlMClickEvent, *event), ATOM_APP);
	}
}

void ATOM_TreeCtrl::onDblClick(ATOM_TreeCtrlDblClickEvent *event)
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::onDblClick);

	ATOM_Widget *w = getParent ();
	if (w)
	{
		w->queueEvent (ATOM_NEW(ATOM_TreeCtrlDblClickEvent, *event), ATOM_APP);
	}
}

void ATOM_TreeCtrl::onItemDragStart(ATOM_TreeItemDragStartEvent *event)
{
	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		event->id = getId();
		parent->handleEvent (event);
	}
}

void ATOM_TreeCtrl::onItemDragOver(ATOM_TreeItemDragOverEvent *event)
{
	if (_allowItemDragDrop)
	{
		ATOM_Widget *parent = getParent ();
		if (parent)
		{
			event->id = getId();
			parent->handleEvent (event);
		}
	}
}

void ATOM_TreeCtrl::onItemDragDrop(ATOM_TreeItemDragDropEvent *event)
{
	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->queueEvent (ATOM_NEW(ATOM_TreeItemDragDropEvent, getId(), event->dragSource.get(), event->index, event->keymod), ATOM_APP);
	}
}

void ATOM_TreeCtrl::calcLayout()
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::calcLayout);

	int line = 0;

	for(ATOM_LIST<ATOM_TreeItem*>::iterator iter = _children.begin(); iter != _children.end(); ++iter)
	{
		calcItemLayout(*iter, line);
	}

	//setScrollValue(ATOM_Point2Di(0,0));

	// 重新计算canvas大小
	ATOM_Size2Di canvas(0, 0);
	canvas.w = _widgetRect.size.w;
	canvas.h = ATOM_max2(_widgetRect.size.h, line*_lineHeight);
	setCanvasSize (canvas);

	ATOM_Point2Di scrollValue = getScrollValue ();
	if (scrollValue.y > canvas.h - _widgetRect.size.h)
	{
		scrollValue.y = canvas.h - _widgetRect.size.h;
		setScrollValue (scrollValue);
	}
}

void ATOM_TreeCtrl::calcItemLayout(ATOM_TreeItem* item, int& line)
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::calcItemLayout);

	int offsetX = 0;
	if( item->getRoot() )
	{
		offsetX = item->getRoot()->getOffset() + _indent;
	}
	else
	{
		offsetX = 0;
	}

	int offsetY = line*_lineHeight - getScrollValue().y;

	ATOM_Rect2Di rc(0, offsetY, getWidgetRect().size.w, _lineHeight);
	item->resize(rc);
	item->setOffset (offsetX);
	++line;

	if(item->getExpand())
	{
		for(ATOM_LIST<ATOM_TreeItem*>::iterator iter = item->getChildren().begin(); iter != item->getChildren().end(); ++iter)
		{
			calcItemLayout(*iter, line);
		}
	}
}

ATOM_TreeItem* ATOM_TreeCtrl::getFirstChildItem (ATOM_TreeItem *item) const
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::getFirstChildItem);

	if (item == 0)
	{
		return _children.empty () ? 0 : *_children.begin();
	}
	else
	{
		return item->_children.empty () ? 0 : *(item->_children.begin());
	}
}

ATOM_TreeItem* ATOM_TreeCtrl::getPrevSiblingItem (ATOM_TreeItem *item) const
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::getPrevSiblingItem);

	if (item)
	{
		ATOM_LIST<ATOM_TreeItem*>::iterator it = item->_iter;
		ATOM_LIST<ATOM_TreeItem*>::const_iterator itBegin = item->_root ? item->_root->_children.begin() : _children.begin();

		if (it != itBegin)
		{
			it--;
			return *it;
		}
	}

	return 0;
}

ATOM_TreeItem* ATOM_TreeCtrl::getNextSiblingItem (ATOM_TreeItem *item) const
{
	ATOM_STACK_TRACE(ATOM_TreeCtrl::getNextSiblingItem);

	if (item)
	{
		ATOM_LIST<ATOM_TreeItem*>::iterator it = item->_iter;
		ATOM_LIST<ATOM_TreeItem*>::const_iterator itEnd = item->_root ? item->_root->_children.end() : _children.end();

		it++;
		if (it != itEnd)
		{
			return *it;
		}
	}

	return 0;
}

int ATOM_TreeCtrl::getImageSize(void) const
{
	return _imageSize;
}

int ATOM_TreeCtrl::getLineHeight(void) const
{
	return _lineHeight;
}

int ATOM_TreeCtrl::getIndent(void) const
{
	return _indent;
}

int ATOM_TreeCtrl::getNextIndex (void)
{
	return _nextIndex++;
}

ATOM_TreeItem *ATOM_TreeCtrl::getItemByIndex (int index) const
{
	if (index < 0)
	{
		return 0;
	}

	return getItemByIndexR (index, NULL);
}

ATOM_TreeItem *ATOM_TreeCtrl::getItemByUserData (unsigned long long userData) const
{
	return getItemByUserDataR (userData, NULL);
}

ATOM_TreeItem *ATOM_TreeCtrl::getItemByUserDataR (unsigned long long userData, ATOM_TreeItem *root) const
{
	for (ATOM_TreeItem *item = getFirstChildItem(root); item; item = getNextSiblingItem (item))
	{
		if (item->getUserData() == userData)
		{
			return item;
		}

		ATOM_TreeItem *ret = getItemByUserDataR (userData, item);
		if (ret)
		{
			return ret;
		}
	}
	return 0;
}

ATOM_TreeItem *ATOM_TreeCtrl::getItemByIndexR (int index, ATOM_TreeItem *root) const
{
	for (ATOM_TreeItem *item = getFirstChildItem(root); item; item = getNextSiblingItem (item))
	{
		if (item->getIndex() == index)
		{
			return item;
		}

		ATOM_TreeItem *ret = getItemByIndexR (index, item);
		if (ret)
		{
			return ret;
		}
	}
	return 0;
}

void ATOM_TreeCtrl::allowItemDragDrop (bool allow)
{
	_allowItemDragDrop = allow;
}

bool ATOM_TreeCtrl::isItemDragDropAllowed (void) const
{
	return _allowItemDragDrop;
}



