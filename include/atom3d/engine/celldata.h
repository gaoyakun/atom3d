/**	\file celldata.h
 *	格子类.
 *
 *	\author 白大使
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_CELLDATA_H
#define __ATOM3D_ENGINE_CELLDATA_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"

#include "gui_font.h"
#include "label.h"


//! \class ATOM_CellData
//! 格子类.
class ATOM_ENGINE_API ATOM_CellData: public ATOM_Label
{
public:
	ATOM_CellData(void);
	ATOM_CellData (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::ShowNormal);
	virtual ~ATOM_CellData (void);

public:
	virtual ATOM_WidgetType getType (void) const;

public:
	//! 设置CD时间
	void setCooldown(unsigned cooldown);

	//! 取得CD时间
	unsigned getCooldown() const;

	//! 设置CD最大值
	void setCooldownMax(unsigned cooldownMax);

	//! 取得CD最大值
	unsigned getCooldownMax() const;

	//! 设置CD图片
	void setCooldownImageId (int id);

	//! 取得CD图片
	int getCooldownImageId (void) const;

	//! 设置选中图片
	void setSelectImageId (int id);

	//! 取得选中图片
	int getSelectImageId (void) const;

	//! 设置边框图片
	void setFrameImageId (int id);

	//! 取得边框图片
	int getFrameImageId (void) const;

	//! 设置边框偏移
	void setFrameOffset(int offset);

	//! 取得边框偏移
	int getFrameOffset (void) const;

	//! 设置下标图片
	void setSubscriptImageId (int id);

	//! 取得下标图片
	int getSubscriptImageId()const;

	//! 设置下标图片位置
	void setSubscriptImageRect(ATOM_Rect2Di& rect);

	//! 取得下标图片位置
	ATOM_Rect2Di getSubscriptImageRect()const;

	//! 设置文字
	void setSubscriptText (const char *str);

	//! 取得文字
	const char *getSubscriptText (void) const;

	//! 设置下标字体位置
	void setSubscriptTextPosition(ATOM_Point2Di & point);

	//! 获取下标字体位置
	ATOM_Point2Di getSubscriptTextPosition() const;

	void allowClientDragging (bool allow);
	bool isClientDraggingAllowed (void) const;
protected:
	void calcSubscriptTextLayout (void);
protected:
	int						_cooldownImageId;
	int						_selectImageId;
	int						_frameImageId;
	int						_frameOffset;
	unsigned				_cooldown;
	unsigned				_cooldownMax;
	ATOM_Point2Di			_oldPosition;
	bool					_clientDragging;

	int						_subscriptImageId;		//!< 下标图片
	ATOM_Rect2Di			_subscriptRect;			//!< 下标图片位置
	ATOM_Point2Di			_subscriptTextPosition;	//!< 下标字体位置
	ATOM_AUTOREF(ATOM_Text) _subscriptText;			//!< 下标字体
	ATOM_STRING				_subscriptStr;			//!< 下标文本
	bool					_subscirptTextDirty;

public:
	void onLButtonDown (ATOM_WidgetLButtonDownEvent *event);
	void onLButtonUp (ATOM_WidgetLButtonUpEvent *event);
	void onRButtonDown (ATOM_WidgetRButtonDownEvent *event);
	void onRButtonUp (ATOM_WidgetRButtonUpEvent *event);
	void onDblClick (ATOM_WidgetLButtonDblClickEvent *event);
	void onMouseEnter (ATOM_WidgetMouseEnterEvent *event);
	void onMouseLeave (ATOM_WidgetMouseLeaveEvent *event);
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onIdle(ATOM_WidgetIdleEvent *event);
	void onHitTest (ATOM_WidgetHitTestEvent *event);
	void onDragStart (ATOM_WidgetDragStartEvent *event);
	void onDragOver (ATOM_WidgetDragOverEvent *event);
	void onDragDrop (ATOM_WidgetDragDropEvent *event);

	ATOM_DECLARE_EVENT_MAP(ATOM_CellData, ATOM_Label)
};



#endif // __ATOM3D_ENGINE_CELLDATA_H
/*! @} */
