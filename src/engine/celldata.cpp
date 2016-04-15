#include "stdafx.h"
#include "celldata.h"
#include "gui_canvas.h"
#include "polygondrawer.h"
#include "cell.h"

ATOM_BEGIN_EVENT_MAP(ATOM_CellData, ATOM_Label)
	ATOM_EVENT_HANDLER(ATOM_CellData, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	ATOM_EVENT_HANDLER(ATOM_CellData, ATOM_WidgetLButtonUpEvent, onLButtonUp)
	ATOM_EVENT_HANDLER(ATOM_CellData, ATOM_WidgetRButtonDownEvent, onRButtonDown)
	ATOM_EVENT_HANDLER(ATOM_CellData, ATOM_WidgetRButtonUpEvent, onRButtonUp)
	ATOM_EVENT_HANDLER(ATOM_CellData, ATOM_WidgetLButtonDblClickEvent, onDblClick)
	ATOM_EVENT_HANDLER(ATOM_CellData, ATOM_WidgetMouseEnterEvent, onMouseEnter)
	ATOM_EVENT_HANDLER(ATOM_CellData, ATOM_WidgetMouseLeaveEvent, onMouseLeave)
	ATOM_EVENT_HANDLER(ATOM_CellData, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(ATOM_CellData, ATOM_WidgetDrawFrameEvent, onPaintFrame)
	ATOM_EVENT_HANDLER(ATOM_CellData, ATOM_WidgetIdleEvent, onIdle)
	ATOM_EVENT_HANDLER(ATOM_CellData, ATOM_WidgetHitTestEvent, onHitTest)
	ATOM_EVENT_HANDLER(ATOM_CellData, ATOM_WidgetDragStartEvent, onDragStart)
	ATOM_EVENT_HANDLER(ATOM_CellData, ATOM_WidgetDragOverEvent, onDragOver)
	ATOM_EVENT_HANDLER(ATOM_CellData, ATOM_WidgetDragDropEvent, onDragDrop)
ATOM_END_EVENT_MAP

ATOM_CellData::ATOM_CellData()
{
	ATOM_STACK_TRACE(ATOM_CellData::ATOM_CellData);

	_cooldown = 0;
	_cooldownMax = 0;
	_cooldownImageId = ATOM_INVALID_IMAGEID;
	_selectImageId = ATOM_INVALID_IMAGEID;
	_frameImageId = ATOM_INVALID_IMAGEID;
	_subscriptImageId = ATOM_INVALID_IMAGEID;
	_frameOffset = 0;
	_oldPosition.x = _oldPosition.y = 0;
	_clientDragging = false;
	_subscirptTextDirty = false;
	_subscriptText = ATOM_HARDREF(ATOM_Text)();
	_subscriptText->setFont (ATOM_GUIFont::getFontHandle(getFont()));
	_subscriptText->setCharMargin (ATOM_GUIFont::getFontCharMargin(getFont()));
	_subscriptText->setZValue (1.f);
	_subscriptText->setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
	_subscriptRect = ATOM_Rect2Di(0,0,0,0);
	_subscriptTextPosition.x = _subscriptTextPosition.y = 0;
	setLayoutable (false);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_CellData::ATOM_CellData (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
	: ATOM_Label (parent, rect, (style & ~ATOM_Widget::TitleBar) | ATOM_Widget::Control | ATOM_Widget::NonLayoutable, id, showState)
{
	ATOM_STACK_TRACE(ATOM_CellData::ATOM_CellData);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

	_cooldown = 0;
	_cooldownMax = 0;
	_cooldownImageId = ATOM_INVALID_IMAGEID;
	_selectImageId = ATOM_INVALID_IMAGEID;
	_frameImageId = ATOM_INVALID_IMAGEID;
	_subscriptImageId = ATOM_INVALID_IMAGEID;
	_frameOffset = 0;
	_oldPosition = rect.point;
	_clientDragging = false;
	_subscirptTextDirty = false;
	_subscriptText = ATOM_HARDREF(ATOM_Text)();
	_subscriptText->setFont (ATOM_GUIFont::getFontHandle(getFont()));
	_subscriptText->setCharMargin (ATOM_GUIFont::getFontCharMargin(getFont()));
	_subscriptText->setZValue (1.f);
	_subscriptText->setDisplayMode (ATOM_Text::DISPLAYMODE_2D);
	_subscriptRect = ATOM_Rect2Di(0,0,0,0);
	_subscriptTextPosition.x = _subscriptTextPosition.y = 0;
	setBorderMode (ATOM_Widget::Raise);

	resize (rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_CellData::~ATOM_CellData (void)
{
}

void ATOM_CellData::setCooldown(unsigned cooldown)
{
	ATOM_STACK_TRACE(ATOM_CellData::setCooldown);

	_cooldown = cooldown;
	if(_cooldownMax < _cooldown)
	{
		_cooldown = _cooldownMax;
	}
}

unsigned ATOM_CellData::getCooldown() const
{
	ATOM_STACK_TRACE(ATOM_CellData::getCooldown);

	return _cooldown;
}

// 设置CD最大值
void ATOM_CellData::setCooldownMax(unsigned cooldownMax)
{
	ATOM_STACK_TRACE(ATOM_CellData::setCooldownMax);

	_cooldownMax = cooldownMax;
	if(_cooldownMax < _cooldown)
	{
		_cooldown = _cooldownMax;
	}
}

// 取得CD最大值
unsigned ATOM_CellData::getCooldownMax() const
{
	ATOM_STACK_TRACE(ATOM_CellData::getCooldownMax);

	return _cooldownMax;
}

void ATOM_CellData::setCooldownImageId(int id)
{
	ATOM_STACK_TRACE(ATOM_CellData::setCooldownImageId);

	_cooldownImageId = id;
}

int ATOM_CellData::getCooldownImageId() const
{
	ATOM_STACK_TRACE(ATOM_CellData::getCooldownImageId);

	return _cooldownImageId;
}

void ATOM_CellData::setSelectImageId(int id)
{
	ATOM_STACK_TRACE(ATOM_CellData::setSelectImageId);

	_selectImageId = id;
}

int ATOM_CellData::getSelectImageId() const
{
	ATOM_STACK_TRACE(ATOM_CellData::getSelectImageId);

	return _selectImageId;
}

// 设置边框图片
void ATOM_CellData::setFrameImageId (int id)
{
	ATOM_STACK_TRACE(ATOM_CellData::setFrameImageId);

	_frameImageId = id;
}

// 取得边框图片
int ATOM_CellData::getFrameImageId (void) const
{
	ATOM_STACK_TRACE(ATOM_CellData::getFrameImageId);

	return _frameImageId;
}

// 设置边框偏移
void ATOM_CellData::setFrameOffset(int offset)
{
	ATOM_STACK_TRACE(ATOM_CellData::setFrameOffset);

	_frameOffset = offset;
}

// 取得边框偏移
int ATOM_CellData::getFrameOffset (void) const
{
	ATOM_STACK_TRACE(ATOM_CellData::getFrameOffset);

	return _frameOffset;
}

ATOM_WidgetType ATOM_CellData::getType (void) const
{
	ATOM_STACK_TRACE(ATOM_CellData::getType);

	return WT_CELLDATA;
}

void ATOM_CellData::onLButtonDown (ATOM_WidgetLButtonDownEvent *event)
{
	ATOM_STACK_TRACE(ATOM_CellData::onLButtonDown);

	setBorderMode (ATOM_Widget::Drop);
	setCapture ();
	_oldPosition = _widgetRect.point;
	ATOM_Widget *parent = getParent ();
	if(parent)
	{
		ATOM_CellLButtonDownEvent e(getId(), 0, 0);
		parent->handleEvent(&e);
	}
}

void ATOM_CellData::onLButtonUp (ATOM_WidgetLButtonUpEvent *event)
{
	ATOM_STACK_TRACE(ATOM_CellData::onLButtonUp);

	if(_clientDragging)
	{
		moveTo(_oldPosition.x, _oldPosition.y);
	}

	if (getRenderer()->getCapture() == this)
	{
		setBorderMode (ATOM_Widget::Raise);
		releaseCapture ();
		ATOM_Widget *parent = getParent ();
		if(parent)
		{
			ATOM_Widget* hover = getRenderer()->getHover();
			if(hover && hover->getType() == WT_CELLDATA && hover->getParent())
			{
				ATOM_CellLButtonUpEvent e(hover->getId(), 0, 0, this, 0, 0);
				hover->getParent()->handleEvent(&e);
			}
			else if(hover && hover->getType() == WT_REALTIMECTRL)
			{
				ATOM_Cell* c = dynamic_cast<ATOM_Cell*>(getParent());
				if(c)
				{
					int xSize, ySize;
					c->getCellSize(xSize, ySize);
					ATOM_CellLButtonUpEvent e(hover->getId(), 0, 0, this, getId()%xSize, getId()/xSize);
					hover->handleEvent(&e);
				}
				else
				{
					ATOM_CellLButtonUpEvent e(hover->getId(), 0, 0, this, 0, 0);
					hover->handleEvent(&e);
				}
			}
			else
			{
				ATOM_CellLButtonUpEvent e(getId(), 0, 0, NULL, 0, 0);
				parent->handleEvent(&e);
			}

			if (isMouseHover())
			{
				if (parent && !_clientDragging)
				{
					parent->queueEvent (ATOM_NEW(ATOM_CellLeftClickEvent, getId(), 0, 0), ATOM_APP);
				}
			}
		}
	}
}

void ATOM_CellData::onRButtonDown (ATOM_WidgetRButtonDownEvent *event)
{
	ATOM_STACK_TRACE(ATOM_CellData::onRButtonDown);

	setBorderMode (ATOM_Widget::Drop);
	setCapture ();
	ATOM_Widget *parent = getParent ();
	if(parent)
	{
		ATOM_CellRButtonDownEvent e(getId(), 0, 0);
		parent->handleEvent(&e);
	}
}


void ATOM_CellData::onRButtonUp (ATOM_WidgetRButtonUpEvent *event)
{
	ATOM_STACK_TRACE(ATOM_CellData::onRButtonUp);

	if (getRenderer()->getCapture() == this)
	{
		setBorderMode (ATOM_Widget::Raise);
		releaseCapture ();

		if (isMouseHover ())
		{
			ATOM_Widget *parent = getParent ();
			if (parent)
			{
				parent->queueEvent (ATOM_NEW(ATOM_CellRightClickEvent, getId(), 0, 0), ATOM_APP);
			}
		}
	}
}

void ATOM_CellData::onMouseEnter (ATOM_WidgetMouseEnterEvent *event)
{
	ATOM_STACK_TRACE(ATOM_CellData::onMouseEnter);

	if (getRenderer()->getCapture() == this)
	{
		setBorderMode (ATOM_Widget::Drop);
	}

	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->queueEvent (ATOM_NEW(ATOM_CellMouseEnterEvent, getId(), 0, 0), ATOM_APP);
	}
}

void ATOM_CellData::onMouseLeave (ATOM_WidgetMouseLeaveEvent *event)
{
	ATOM_STACK_TRACE(ATOM_CellData::onMouseLeave);

	if (getRenderer()->getCapture() == this)
	{
		setBorderMode (ATOM_Widget::Raise);
	}

	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->queueEvent (ATOM_NEW(ATOM_CellMouseLeaveEvent, getId(), 0, 0), ATOM_APP);
	}
}

void ATOM_CellData::onDblClick (ATOM_WidgetLButtonDblClickEvent *event)
{
	if (getParent())
	{
		getParent()->queueEvent (ATOM_NEW(ATOM_CellDblClickEvent, getId(), 0, 0), ATOM_APP);
	}
}

void ATOM_CellData::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	ATOM_STACK_TRACE(ATOM_CellData::onPaint);

	ATOM_Rect2Di outRect(ATOM_Point2Di(0, 0), event->canvas->getSize());
	ATOM_Rect2Di inRect(_frameOffset, _frameOffset, outRect.size.w-_frameOffset*2, outRect.size.h-_frameOffset*2);

	bool inCD = (ATOM_INVALID_IMAGEID != _cooldownImageId) && _cooldownMax;

	if (_frameImageId != ATOM_INVALID_IMAGEID)
	{
		ATOM_GUIImage *image = getValidImage (_frameImageId);
		image->draw (event->state, event->canvas, outRect);
	}

	if (_clientImageId != ATOM_INVALID_IMAGEID)
	{
		ATOM_GUIImage *image = getValidImage (_clientImageId);
		if (!image)
		{
			image = ATOM_GUIImageList::getDefaultImageList().getImage (ATOM_IMAGEID_WINDOW_BKGROUND);
			ATOM_ASSERT(image);
		}
		if(_customColor)
		{
			image->draw (event->state, event->canvas, inRect, &_customColor);
		}
		else
		{
			image->draw (event->state, event->canvas, inRect);
		}

	}

	if (!_caption.empty())
	{
		if (_textDirty)
		{
			calcTextLayout ();
			_textDirty = false;
		}

		ATOM_GUICanvas::DrawTextInfo info;
		info.textObject = _text.get();
		info.textString = _text->getString();
		info.font = _text->getFont();
		info.x = _textPosition.x;
		info.y = _textPosition.y;
		info.shadowOffsetX = 0;
		info.shadowOffsetY = 0;
		info.textColor = getFontColor();
		info.outlineColor = _widgetFontOutLineColor;
		info.shadowColor = 0;
		info.underlineColor = _underlineColor;
		info.flags = ATOM_GUICanvas::DRAWTEXT_TEXTOBJECT;
		if(_widgetFontOutLineColor.getByteA())
		{
			info.flags |= ATOM_GUICanvas::DRAWTEXT_OUTLINE;
		}
		if(_underlineColor.getByteA())
		{
			info.flags |= ATOM_GUICanvas::DRAWTEXT_UNDERLINE;
		}
		event->canvas->drawTextEx(&info);
		//event->canvas->drawText (_text.get(), _textPosition.x, _textPosition.y, getFontColor());
	}
	//绘制下标字体
	if (!_subscriptStr.empty())
	{
		if (_subscirptTextDirty)
		{
			calcSubscriptTextLayout();
			_subscirptTextDirty = false;
		}

		ATOM_GUICanvas::DrawTextInfo info;
		info.textObject = _subscriptText.get();
		info.textString = _subscriptText->getString();
		info.font = _subscriptText->getFont();
		info.x = _subscriptTextPosition.x;
		info.y = _subscriptTextPosition.y;
		info.shadowOffsetX = 0;
		info.shadowOffsetY = 0;
		info.textColor = getFontColor();
		info.outlineColor = _widgetFontOutLineColor;
		info.shadowColor = 0;
		info.underlineColor = _underlineColor;
		info.flags = ATOM_GUICanvas::DRAWTEXT_TEXTOBJECT;
		if(_widgetFontOutLineColor.getByteA())
		{
			info.flags |= ATOM_GUICanvas::DRAWTEXT_OUTLINE;
		}
		if(_underlineColor.getByteA())
		{
			info.flags |= ATOM_GUICanvas::DRAWTEXT_UNDERLINE;
		}
		event->canvas->drawTextEx(&info);
		//event->canvas->drawText (_text.get(), _textPosition.x, _textPosition.y, getFontColor());
	}
	//绘制下标图片
	if(_subscriptImageId != ATOM_INVALID_IMAGEID)
	{
		ATOM_GUIImage *image = getValidImage (_subscriptImageId);
		if(!image)
		{
			image = ATOM_GUIImageList::getDefaultImageList().getImage(ATOM_IMAGEID_WINDOW_BKGROUND);
			ATOM_ASSERT(image);
		}
		image->draw(event->state,event->canvas,ATOM_Rect2Di(inRect.point.x+_subscriptRect.point.x,
			inRect.point.y+_subscriptRect.point.y,_subscriptRect.size.w,_subscriptRect.size.h));
	}

	if (_selectImageId != ATOM_INVALID_IMAGEID)
	{
#if 1
		ATOM_GUIImage *image = getValidImage (_selectImageId);
#else
		const ATOM_GUIImageList *imagelist = getValidImageList ();
		ATOM_GUIImage *image = imagelist->getImage (_selectImageId);
#endif
		if (!image)
		{
			image = ATOM_GUIImageList::getDefaultImageList().getImage (ATOM_IMAGEID_WINDOW_BKGROUND);
			ATOM_ASSERT(image);
		}
		image->draw (event->state, event->canvas, outRect);
	}

	//
	//bool inCD = (ATOM_INVALID_IMAGEID != _cooldownImageId) && _cooldownMax;

	//// 画CELL背景
	//if(cell->_texture.pointer)
	//{
	//	unsigned color = inCD ? 0xFF808080 : 0xFFFFFFFF;
	//	canvas->drawTexturedRectEx(ATOM_Rect2Di(x*(xCellWidth+_xSpace), y*(yCellWidth+_ySpace), xCellWidth, yCellWidth), color, cell->_texture.pointer, cell->_region, false);
	//}
	//else if (_clientImageId != ATOM_INVALID_IMAGEID)
	//{
	//	imageDefault->draw (event->state, event->canvas, ATOM_Rect2Di(x*(xCellWidth+_xSpace), y*(yCellWidth+_ySpace), xCellWidth, yCellWidth));
	//}

	//if(cell->_text.pointer)
	//{
	//	const char* str = cell->_text->getString();
	//	if (strcmp (str, ""))
	//	{
	//		ATOM_FontHandle font = ATOM_GUIFont::getFontHandle (getFont());
	//		int charmargin = ATOM_GUIFont::getFontCharMargin (getFont());
	//		int charset = ATOM_GUIFont::getFontCharSet (getFont());
	//		ATOM_SetCharMargin (charmargin);
	//		int l, t, w, h;
	//		ATOM_CalcStringBounds (font, charset, str, strlen(str), &l, &t, &w, &h);
	//		canvas->drawText (cell->_text.pointer, x*(xCellWidth+_xSpace), y*(yCellWidth+_ySpace)-t, _fontColor);
	//	}
	//}

	// 画CD
	if(inCD)
	{
#if 1
		ATOM_GUIImage *image = getValidImage (_cooldownImageId);
#else
		ATOM_GUIImage* image = getValidImageList()->getImage (_cooldownImageId);
#endif
		if(image)
		{
			ATOM_Texture* texture = image->getTexture(WST_NORMAL);
			if(texture)
			{
				ATOM_CircleDrawer drawer(event->canvas, texture);
				drawer.DrawRemove(inRect, float(_cooldown)/float(_cooldownMax));
			}
		}
#if 0
		char buffer[256];
		sprintf (buffer, "%d", _cooldown);
		event->canvas->drawText (buffer, ATOM_GUIFont::getFontHandle(ATOM_GUIFont::getDefaultFont(10, 0)), 0, 0, 0xFFFFFFFF);
#endif
	}
}

void ATOM_CellData::onIdle(ATOM_WidgetIdleEvent *event)
{
	ATOM_STACK_TRACE(ATOM_CellData::onIdle);

	if(0 == _cooldownMax)
	{
		return;
	}

	unsigned elapsed = ATOM_APP->getFrameStamp().elapsedTick;
	_cooldown += elapsed;
	if(_cooldown >= _cooldownMax)
	{
		_cooldown = 0;
		_cooldownMax = 0;
	}

	if (getShowState () != ATOM_Widget::Hide)
	{
		invalidate ();
	}
}

void ATOM_CellData::onHitTest (ATOM_WidgetHitTestEvent *event)
{
	ATOM_STACK_TRACE(ATOM_CellData::onHitTest);

	ATOM_Label::onHitTest(event);

	if (event->hitTestResult.hitPoint == HitClient && _clientDragging)
	{
		event->hitTestResult.lug = true;
	}
}

void ATOM_CellData::allowClientDragging (bool allow)
{
	ATOM_STACK_TRACE(ATOM_CellData::allowClientDragging);

	_clientDragging = allow;
}

bool ATOM_CellData::isClientDraggingAllowed (void) const
{
	return _clientDragging;
}

void ATOM_CellData::onDragStart (ATOM_WidgetDragStartEvent *event)
{
	ATOM_Cell *cell = (ATOM_Cell*)_parent;
	if (cell && cell->isCellDragDropAllowed())
	{
		int x, y;
		cell->getCellSize (x, y);

		ATOM_CellDragStartEvent e(cell->getId(), event->dragSource.get(), getId()%x, getId()/x, true);
		cell->handleEvent (&e);

		event->allow = e.allow;
	}
	else
	{
		event->allow = false;
	}
}

void ATOM_CellData::onDragOver (ATOM_WidgetDragOverEvent *event)
{
	ATOM_Cell *cell = (ATOM_Cell*)_parent;
	if (cell && cell->isCellDragDropAllowed())
	{
		int x, y;
		cell->getCellSize (x, y);

		ATOM_CellDragOverEvent e(cell->getId(), event->dragSource.get(), getId()%x, getId()/x, true);
		cell->handleEvent (&e);
		event->accept = e.accept;
	}
}

void ATOM_CellData::onDragDrop (ATOM_WidgetDragDropEvent *event)
{
	ATOM_Cell *cell = (ATOM_Cell*)_parent;
	if (cell && cell->isCellDragDropAllowed())
	{
		int x, y;
		cell->getCellSize (x, y);

		ATOM_CellDragDropEvent e(cell->getId(), event->dragSource.get(), getId()%x, getId()/x);
		cell->handleEvent (&e);
	}
}

void ATOM_CellData::setSubscriptImageId( int id )
{
	ATOM_STACK_TRACE(ATOM_CellData::setSubscriptImageId);
	_subscriptImageId = id;
	invalidate ();
}

int ATOM_CellData::getSubscriptImageId() const
{
	ATOM_STACK_TRACE(ATOM_CellData::getSubscriptImageId);
	return _subscriptImageId;
}

void ATOM_CellData::setSubscriptImageRect( ATOM_Rect2Di& rect )
{
	ATOM_STACK_TRACE(ATOM_CellData::setSubscriptImageRect);
	_subscriptRect = rect;
	invalidate ();
}

ATOM_Rect2Di ATOM_CellData::getSubscriptImageRect() const
{
	ATOM_STACK_TRACE(ATOM_CellData::getSubscriptImageRect);
	return _subscriptRect;
}

void ATOM_CellData::setSubscriptText( const char *str )
{
	ATOM_STACK_TRACE(ATOM_CellData::setSubscriptText);
	_subscriptStr = str ? str : "";
	_subscirptTextDirty = true;
	invalidate ();
}

const char * ATOM_CellData::getSubscriptText( void ) const
{
	ATOM_STACK_TRACE(ATOM_CellData::getSubscriptText);
	return _subscriptStr.c_str();
}

void ATOM_CellData::calcSubscriptTextLayout( void )
{
	ATOM_STACK_TRACE(ATOM_CellData::calcSubscriptTextLayout);

	ATOM_FontHandle font = ATOM_GUIFont::getFontHandle (getFont());
	int charmargin = ATOM_GUIFont::getFontCharMargin (getFont());
	ATOM_SetCharMargin (charmargin);
	int l, t, w, h;
	unsigned clip = ATOM_ClipString (font, _subscriptStr.c_str(), _clientRect.size.w, &l, &t, &w, &h);
	_subscriptText->setString (_subscriptStr.substr(0, clip).c_str());
	_subscirptTextDirty = false;
}

ATOM_Point2Di ATOM_CellData::getSubscriptTextPosition() const
{
	ATOM_STACK_TRACE(ATOM_CellData::getSubscriptTextPosition);
	return _subscriptTextPosition;
}

void ATOM_CellData::setSubscriptTextPosition( ATOM_Point2Di & point )
{
	ATOM_STACK_TRACE(ATOM_CellData::setSubscriptTextPosition);
	_subscriptTextPosition = point;
	_subscirptTextDirty = true;
}





