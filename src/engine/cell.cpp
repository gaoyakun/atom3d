#include "stdafx.h"
#include "cell.h"
#include "gui_canvas.h"
#include "polygondrawer.h"

ATOM_BEGIN_EVENT_MAP(ATOM_Cell, ATOM_Widget)
	ATOM_EVENT_HANDLER(ATOM_Cell, ATOM_CellLeftClickEvent, onCellLClick)
	ATOM_EVENT_HANDLER(ATOM_Cell, ATOM_CellRightClickEvent, onCellRClick)
	ATOM_EVENT_HANDLER(ATOM_Cell, ATOM_CellMouseEnterEvent, onCellMouseEnter)
	ATOM_EVENT_HANDLER(ATOM_Cell, ATOM_CellMouseLeaveEvent, onCellMouseLeave)
	ATOM_EVENT_HANDLER(ATOM_Cell, ATOM_CellLButtonDownEvent, onCellLButtonDown)
	ATOM_EVENT_HANDLER(ATOM_Cell, ATOM_CellLButtonUpEvent, onCellLButtonUp)
	ATOM_EVENT_HANDLER(ATOM_Cell, ATOM_CellDblClickEvent, onCellDblClick)
	ATOM_EVENT_HANDLER(ATOM_Cell, ATOM_CellDragStartEvent, onCellDragStart)
	ATOM_EVENT_HANDLER(ATOM_Cell, ATOM_CellDragOverEvent, onCellDragOver)
	ATOM_EVENT_HANDLER(ATOM_Cell, ATOM_CellDragDropEvent, onCellDragDrop)
	ATOM_EVENT_HANDLER(ATOM_Cell, ATOM_WidgetDrawClientEvent, onPaint)
ATOM_END_EVENT_MAP

ATOM_Cell::ATOM_Cell()
{
	ATOM_STACK_TRACE(ATOM_Cell::ATOM_Cell);

	_xSize = 0;
	_ySize = 0;
	_xCellWidth = 0;
	_yCellWidth = 0;
	_cooldownImageId = ATOM_INVALID_IMAGEID;
	_selectImageId = ATOM_INVALID_IMAGEID;
	_align = AlignX_Middle|AlignY_Middle;
	_offset.x = _offset.y = 0;
	_growMode = GM_DOWN;

	_xSpace = 0;
	_ySpace = 0;
	_selectId = -1;
	_enableSelect = false;
	_enableDragDrop =false;
	_updateCellDataMap = true;

	_frameImageId = ATOM_INVALID_IMAGEID;
	_frameOffset = 0;
	//_outlineColor.setRaw(0);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_Cell::ATOM_Cell (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState)
	: ATOM_Widget (parent, rect, style|ATOM_Widget::ClipChildren, id, showState)
{
	ATOM_STACK_TRACE(ATOM_Cell::ATOM_Cell);

	ATOM_ASSERT(parent);
	ATOM_ASSERT(getRenderer());

	_xSize = 0;
	_ySize = 0;
	_xCellWidth = 0;
	_yCellWidth = 0;
	_cooldownImageId = ATOM_INVALID_IMAGEID;
	_selectImageId = ATOM_INVALID_IMAGEID;
	_align = AlignX_Middle|AlignY_Middle;
	_offset.x = _offset.y = 0;
	_growMode = GM_DOWN;

	_xSpace = 0;
	_ySpace = 0;
	_selectId = -1;
	_enableSelect = false;
	_enableDragDrop = false;
	_updateCellDataMap = true;

	_frameImageId = ATOM_INVALID_IMAGEID;
	_frameOffset = 0;
	//_outlineColor.setRaw(0);
	_subscriptImageId = ATOM_INVALID_IMAGEID;
	_subscriptRect = ATOM_Rect2Di(0,0,0,0);
	_subscriptTextPosition = ATOM_Point2Di(0,0);
	setBorderMode (ATOM_Widget::Raise);

	resize (rect);

#if defined(USE_WIDGET_DEBUG_INFO)
	ATOM_Widget::registerWidgetDebugInfo (this);
#endif
}

ATOM_Cell::~ATOM_Cell (void)
{
}

void ATOM_Cell::appendRows (unsigned numRows)
{
	if (numRows > 0)
	{
		for(int i=_ySize; i<_ySize+numRows; ++i)
		{
			for(int j=0; j<_xSize; ++j)
			{
				ATOM_Rect2Di rc(0,0,1,1);
				int id = _xSize*i+j;
				ATOM_CellData* pData = ATOM_NEW(ATOM_CellData, this, rc, int(ATOM_Widget::NoFocus), id, ATOM_Widget::ShowNormal);
				pData->setFont(getFont());
				pData->enableDrag (true);
				pData->enableDrop (true);
				pData->setFontColor(getFontColor());
				pData->setAlign(_align);
				pData->setCooldownImageId(_cooldownImageId);
				pData->setSelectImageId(ATOM_INVALID_IMAGEID);
				pData->setClientImageId(_clientImageId);
				pData->setOffset(_offset);
				pData->setFrameImageId(_frameImageId);
				pData->setSubscriptTextPosition(_subscriptTextPosition);
				pData->setSubscriptImageId(_subscriptImageId);
				pData->setSubscriptImageRect(_subscriptRect);
				pData->setFrameOffset(_frameOffset);
				pData->setOutlineColor(_widgetFontOutLineColor);
			}
		}
		_ySize+=numRows;

		alignCells ();
	}
}

void ATOM_Cell::truncateRows (unsigned numRows)
{
	unselect ();

	if (numRows > 0 && numRows <= _ySize)
	{
		for(int i=_ySize-numRows; i<_ySize; ++i)
		{
			for(int j=0; j<_xSize; ++j)
			{
				ATOM_CellData *pData = getCell (j, i);
				ATOM_ASSERT(pData);
				deleteChild (pData);
			}
		}
		_ySize-=numRows;

		alignCells ();
	}
}

void ATOM_Cell::setCellSize(int x, int y)
{
	ATOM_STACK_TRACE(ATOM_Cell::setCellSize);

	if(x < 0 || y < 0)
	{
		return;
	}

	truncateRows (_ySize);
	unselect ();

	_xSize = x;
	_ySize = y;

	for(int i=0; i<y; ++i)
	{
		for(int j=0; j<x; ++j)
		{
			ATOM_Rect2Di rc;
			if (_growMode == GM_DOWN)
			{
				rc = ATOM_Rect2Di(j*(_xCellWidth+_xSpace), i*(_yCellWidth+_ySpace), _xCellWidth, _yCellWidth);
			}
			else
			{
				rc = ATOM_Rect2Di(j*(_xCellWidth+_xSpace), (y-i-1)*(_yCellWidth+_ySpace), _xCellWidth, _yCellWidth);
			}
			int index = x*i+j;
			ATOM_CellData* pData = ATOM_NEW(ATOM_CellData, this, rc, int(ATOM_Widget::NoFocus), index, ATOM_Widget::ShowNormal);
			pData->setFont(getFont());
			pData->enableDrag (true);
			pData->enableDrop (true);
			pData->setFontColor(getFontColor());
			pData->setAlign(_align);
			pData->setCooldownImageId(_cooldownImageId);
			pData->setSelectImageId(ATOM_INVALID_IMAGEID);
			pData->setClientImageId(_clientImageId);
			pData->setOffset(_offset);
			pData->setFrameImageId(_frameImageId);
			pData->setFrameOffset(_frameOffset);
			pData->setSubscriptImageId(_subscriptImageId);
			pData->setSubscriptImageRect(_subscriptRect);
			pData->setSubscriptTextPosition(_subscriptTextPosition);
			pData->setOutlineColor(_widgetFontOutLineColor);
		}
	}
}

void ATOM_Cell::setCellWidth(int x, int y)
{
	ATOM_STACK_TRACE(ATOM_Cell::setCellWidth);

	_xCellWidth = x;
	_yCellWidth = y;
}

ATOM_CellData* ATOM_Cell::setCellData(int x, int y, int imageId, const char* str, unsigned cooldownMax, unsigned cooldown)
{
	ATOM_STACK_TRACE(ATOM_Cell::setCellData);

	if(x < 0 || x >= _xSize || y < 0 || y>= _ySize)
	{
		return NULL;
	}

	int id = y * _xSize + x;
	ATOM_CellData* c = (ATOM_CellData*)getChildById(id);
	if(NULL == c)
	{
		return NULL;
	}

	c->setText(str);
	c->setCooldownMax(cooldownMax);
	c->setCooldown(cooldown);
	c->setClientImageId((ATOM_INVALID_IMAGEID == imageId) ? _clientImageId : imageId);

	if (getShowState () != ATOM_Widget::Hide)
	{
		invalidate ();
	}

	return c;
}

void ATOM_Cell::getCellSize(int& x, int& y) const
{
	x = _xSize;
	y = _ySize;
}

void ATOM_Cell::getCellWidth(int& x, int& y) const
{
	x = _xCellWidth;
	y = _yCellWidth;
}

ATOM_Size2Di ATOM_Cell::getCellWidth (void) const
{
	return ATOM_Size2Di (_xCellWidth, _yCellWidth);
}

ATOM_CellData* ATOM_Cell::getCell(int x, int y) const
{
	if(x < 0 || x >= _xSize || y < 0 || y>= _ySize)
	{
		return NULL;
	}

	return (ATOM_CellData*)getChildById(y * _xSize + x);
}

void ATOM_Cell::setAlign(int align)
{
	ATOM_STACK_TRACE(ATOM_Cell::setAlign);

	_align = align;
}

int ATOM_Cell::getAlign() const
{
	ATOM_STACK_TRACE(ATOM_Cell::getAlign);

	return _align;
}

// 设置文字偏移
void ATOM_Cell::setOffset(const ATOM_Point2Di& offset)
{
	ATOM_STACK_TRACE(ATOM_Cell::setOffset);

	_offset = offset;
}

// 取得文字偏移
const ATOM_Point2Di& ATOM_Cell::getOffset() const
{
	ATOM_STACK_TRACE(ATOM_Cell::getOffset);

	return _offset;
}

void ATOM_Cell::setCooldownImageId(int id)
{
	ATOM_STACK_TRACE(ATOM_Cell::setCooldownImageId);

	_cooldownImageId = id;
}

int ATOM_Cell::getCooldownImageId() const
{
	ATOM_STACK_TRACE(ATOM_Cell::getCooldownImageId);

	return _cooldownImageId;
}

void ATOM_Cell::setSelectImageId(int id)
{
	ATOM_STACK_TRACE(ATOM_Cell::setSelectImageId);

	_selectImageId = id;
}

int ATOM_Cell::getSelectImageId() const
{
	ATOM_STACK_TRACE(ATOM_Cell::getSelectImageId);

	return _selectImageId;
}

void ATOM_Cell::setSpaceX(int val)
{
	ATOM_STACK_TRACE(ATOM_Cell::setSpaceX);

	_xSpace = val;
}

int ATOM_Cell::getSpaceX() const
{
	ATOM_STACK_TRACE(ATOM_Cell::getSpaceX);

	return _xSpace;
}

void ATOM_Cell::setSpaceY(int val)
{
	ATOM_STACK_TRACE(ATOM_Cell::setSpaceY);

	_ySpace = val;
}

int ATOM_Cell::getSpaceY() const
{
	ATOM_STACK_TRACE(ATOM_Cell::getSpaceY);

	return _ySpace;
}

// 设置边框图片
void ATOM_Cell::setFrameImageId (int id, bool reset)
{
	ATOM_STACK_TRACE(ATOM_Cell::setFrameImageId);

	_frameImageId = id;

	if(reset)
	{
		for(int y=0; y<_ySize; ++y)
		{
			for(int x=0; x<_xSize; ++x)
			{
				ATOM_CellData* c = getCell(x, y);
				if(NULL == c)
				{
					continue;
				}
				c->setFrameImageId(id);
			}
		}
	}
}

// 取得边框图片
int ATOM_Cell::getFrameImageId (void) const
{
	ATOM_STACK_TRACE(ATOM_Cell::getFrameImageId);

	return _frameImageId;
}

// 设置边框偏移
void ATOM_Cell::setFrameOffset(int offset, bool reset)
{
	ATOM_STACK_TRACE(ATOM_Cell::setFrameOffset);

	_frameOffset = offset;

	if(reset)
	{
		for(int y=0; y<_ySize; ++y)
		{
			for(int x=0; x<_xSize; ++x)
			{
				ATOM_CellData* c = getCell(x, y);
				if(NULL == c)
				{
					continue;
				}
				c->setFrameOffset(offset);
			}
		}
	}
}

// 取得边框偏移
int ATOM_Cell::getFrameOffset (void) const
{
	ATOM_STACK_TRACE(ATOM_Cell::getFrameOffset);

	return _frameOffset;
}

void ATOM_Cell::unselect (void)
{
	if(_selectId >= 0)
	{
		ATOM_CellData* c = (ATOM_CellData*)getChildById(_selectId);
		if(c)
		{
			c->setSelectImageId(ATOM_INVALID_IMAGEID);
		}
		_selectId = -1;
	}
}

void ATOM_Cell::select(int x, int y)
{
	if(false == _enableSelect)
	{
		return;
	}

	unselect ();

	ATOM_CellData* c = getCell(x, y);
	if(c)
	{
		c->setSelectImageId(_selectImageId);
		_selectId = c->getId();
		invalidate();
	}
}

void ATOM_Cell::enableSelect(bool enable)
{
	ATOM_STACK_TRACE(ATOM_Cell::enableSelect);

	unselect ();

	_enableSelect = enable;
	invalidate();
}

void ATOM_Cell::setOutlineColor(ATOM_ColorARGB color)
{
	ATOM_STACK_TRACE(ATOM_Cell::setOutlineColor);

	_widgetFontOutLineColor = color;
}

ATOM_ColorARGB ATOM_Cell::getOutlineColor() const
{
	ATOM_STACK_TRACE(ATOM_Cell::getOutlineColor);

	return _widgetFontOutLineColor;
}

ATOM_WidgetType ATOM_Cell::getType (void) const
{
	ATOM_STACK_TRACE(ATOM_Cell::getType);

	return WT_CELL;
}

void ATOM_Cell::onCellLClick (ATOM_CellLeftClickEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Cell::onCellLClick);

	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->queueEvent (ATOM_NEW(ATOM_CellLeftClickEvent, getId(), event->id%_xSize, event->id/_xSize), ATOM_APP);
	}
	select(event->id%_xSize, event->id/_xSize);
	invalidate();
}

void ATOM_Cell::onCellLButtonDown(ATOM_CellLButtonDownEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Cell::onCellLButtonDown);

	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		ATOM_CellLButtonDownEvent e(getId(), event->id%_xSize, event->id/_xSize);
		parent->handleEvent (&e);
	}
}

void ATOM_Cell::onCellLButtonUp(ATOM_CellLButtonUpEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Cell::onCellLButtonUp);

	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		if(event->from && event->from->getParent() && event->from->getParent()->getType() == WT_CELL)
		{
			int fromId = event->from->getId();
			int xSize, ySize;
			static_cast<ATOM_Cell*>(event->from->getParent())->getCellSize(xSize, ySize);
			ATOM_CellLButtonUpEvent e(getId(), event->id%_xSize, event->id/_xSize, event->from->getParent(), fromId%xSize, fromId/xSize);
			parent->handleEvent (&e);
		}
		else
		{
			ATOM_CellLButtonUpEvent e(getId(), event->id%_xSize, event->id/_xSize, NULL, 0, 0);
			parent->handleEvent (&e);
		}
	}
}

void ATOM_Cell::onCellRButtonDown(ATOM_CellRButtonDownEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Cell::onCellRButtonDown);

	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		ATOM_CellRButtonDownEvent e(getId(), event->id%_xSize, event->id/_xSize);
		parent->handleEvent (&e);
	}
}

void ATOM_Cell::onCellRButtonUp(ATOM_CellRButtonUpEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Cell::onCellRButtonUp);

	//ATOM_Widget *parent = getParent ();
	//if (parent)
	//{
	//	if(event->from && event->from->getParent() && event->from->getParent()->getType() == WT_CELL)
	//	{
	//		int fromId = event->from->getId();
	//		int xSize, ySize;
	//		static_cast<ATOM_Cell*>(event->from->getParent())->getCellSize(xSize, ySize);
	//		ATOM_CellLButtonUpEvent e(getId(), event->id%_xSize, event->id/_xSize, event->from->getParent(), fromId%xSize, fromId/xSize);
	//		parent->handleEvent (&e);
	//	}
	//	else
	//	{
	//		ATOM_CellLButtonUpEvent e(getId(), event->id%_xSize, event->id/_xSize, NULL, 0, 0);
	//		parent->handleEvent (&e);
	//	}
	//}
}

void ATOM_Cell::onCellRClick (ATOM_CellRightClickEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Cell::onCellRClick);

	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->queueEvent (ATOM_NEW(ATOM_CellRightClickEvent, getId(), event->id%_xSize, event->id/_xSize), ATOM_APP);
	}
}

void ATOM_Cell::onCellDblClick (ATOM_CellDblClickEvent *event)
{
	if (getParent ())
	{
		getParent ()->queueEvent (ATOM_NEW(ATOM_CellDblClickEvent, getId(), event->id%_xSize, event->id/_xSize), ATOM_APP);
	}
}

void ATOM_Cell::onCellMouseEnter (ATOM_CellMouseEnterEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Cell::onCellMouseEnter);

	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->queueEvent (ATOM_NEW(ATOM_CellMouseEnterEvent, getId(), event->id%_xSize, event->id/_xSize), ATOM_APP);
	}
}

void ATOM_Cell::onCellMouseLeave (ATOM_CellMouseLeaveEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Cell::onCellMouseLeave);

	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->queueEvent (ATOM_NEW(ATOM_CellMouseLeaveEvent, getId(), event->id%_xSize, event->id/_xSize), ATOM_APP);
	}
}


void ATOM_Cell::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	//callParentHandler (event);
}

void ATOM_Cell::onCellDragStart (ATOM_CellDragStartEvent *event)
{
	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		event->id = getId();
		parent->handleEvent (event);
	}
}

void ATOM_Cell::onCellDragOver (ATOM_CellDragOverEvent *event)
{
	if (_enableDragDrop)
	{
		ATOM_Widget *parent = getParent ();
		if (parent)
		{
			event->id = getId();
			parent->handleEvent (event);
		}
	}
}

void ATOM_Cell::onCellDragDrop (ATOM_CellDragDropEvent *event)
{
	ATOM_Widget *parent = getParent ();
	if (parent)
	{
		parent->queueEvent (ATOM_NEW(ATOM_CellDragDropEvent, getId(), event->dragSource.get(), event->x, event->y), ATOM_APP);
	}
}

void ATOM_Cell::allowCellDragDrop (bool allow)
{
	_enableDragDrop = allow;
}

bool ATOM_Cell::isCellDragDropAllowed (void) const
{
	return _enableDragDrop;
}

void ATOM_Cell::setGrowMode (GrowMode growMode)
{
	if (_growMode != growMode)
	{
		_growMode = growMode;
		alignCells ();
	}
}

//! 获取行增长模式
ATOM_Cell::GrowMode ATOM_Cell::getGrowMode (void) const
{
	return _growMode;
}

void ATOM_Cell::alignCells (void)
{
	for(int i=0; i<_ySize; ++i)
	{
		for(int j=0; j<_xSize; ++j)
		{
			ATOM_Rect2Di rc;
			if (_growMode == GM_DOWN)
			{
				rc = ATOM_Rect2Di(j*(_xCellWidth+_xSpace), i*(_yCellWidth+_ySpace), _xCellWidth, _yCellWidth);
			}
			else
			{
				rc = ATOM_Rect2Di(j*(_xCellWidth+_xSpace), (_ySize-i-1)*(_yCellWidth+_ySpace), _xCellWidth, _yCellWidth);
			}

			ATOM_CellData* pData = getCell (j, i);
			ATOM_ASSERT(pData);
			pData->resize (rc);
		}
	}
}

ATOM_Size2Di ATOM_Cell::calcClientSize (void) const
{
	return ATOM_Size2Di (_xSize * (_xCellWidth + _xSpace), _ySize * (_yCellWidth + _ySpace));
}

void ATOM_Cell::setCellFrameImageId( int x,int y,int imageId )
{
	ATOM_CellData *pData = getCell(x,y);
	if(pData)
	{
		pData->setFrameImageId(imageId);
	}
}

int ATOM_Cell::getCellFrameImageId( int x,int y ) const
{
	ATOM_CellData *pData = getCell(x,y);
	if(pData)
	{
		return pData->getFrameImageId();
	}
	return ATOM_INVALID_IMAGEID;
}

void ATOM_Cell::setSubscriptImageId( int id )
{
	_subscriptImageId = id;
}

void ATOM_Cell::setSubscriptImageId( int x,int y,int id )
{
	ATOM_CellData *pData = getCell(x,y);
	if(pData)
	{
		pData->setSubscriptImageId(id);
	}
}

int ATOM_Cell::getSubscriptImageId() const
{
	return _subscriptImageId;
}

int ATOM_Cell::getSubscriptImageId( int x,int y ) const
{
	ATOM_CellData *pData = getCell(x,y);
	if(pData)
	{
		return pData->getSubscriptImageId();
	}
	return ATOM_INVALID_IMAGEID;
}

void ATOM_Cell::setSubscriptImageRect( ATOM_Rect2Di& rect )
{
	_subscriptRect = rect;
}

ATOM_Rect2Di ATOM_Cell::getSubscriptImageRect() const
{
	return _subscriptRect;
}


void ATOM_Cell::setSubscriptTextPosition( ATOM_Point2Di & point )
{
	_subscriptTextPosition = point;
}

ATOM_Point2Di ATOM_Cell::getSubscriptTextPosition() const
{
	return _subscriptTextPosition;
}

void ATOM_Cell::setSubscriptText( int x,int y,const char *str )
{
	ATOM_CellData *pData = getCell(x,y);
	if(pData)
	{
		pData->setSubscriptText(str);
	}
}

const char * ATOM_Cell::getSubscriptText( int x,int y ) const
{
	ATOM_CellData *pData = getCell(x,y);
	if(pData)
	{
		return pData->getSubscriptText();
	}
	return "";
}



