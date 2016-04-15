/**	\file checkbox.h
 *	复选框类.
 *
 *	\author 白大使
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_CHECKBOX_H
#define __ATOM3D_ENGINE_CHECKBOX_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"

#include "gui_font.h"
#include "label.h"



//! 复选框
class ATOM_ENGINE_API ATOM_Checkbox: public ATOM_Label
{
public:
	ATOM_Checkbox();
	ATOM_Checkbox (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::ShowNormal);
	virtual ~ATOM_Checkbox (void);

public:
	virtual ATOM_WidgetType getType (void) const;

public:
	//! 设置选中图片
	void setCheckedImageId(int imageId);

	// 取得选中图片
	int getCheckedImageId() const;

	//! 选中
	void setChecked(bool checked);

	//! 是否被选中
	bool getChecked(void) const;

	//! 设置点击音效
	void setClickSound(int sound);

	//! 取得点击音效
	int getClickSound() const;

	//! 设置进过音效
	void setHoverSound(int sound);

	//! 取得进过音效
	int getHoverSound() const;

protected:
	virtual void onClicked (void);

public:
	void onLButtonDown (ATOM_WidgetLButtonDownEvent *event);
	void onLButtonUp (ATOM_WidgetLButtonUpEvent *event);
	void onMouseEnter (ATOM_WidgetMouseEnterEvent *event);
	void onMouseLeave (ATOM_WidgetMouseLeaveEvent *event);
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onPaintFrame (ATOM_WidgetDrawFrameEvent *event);
	void onIdle(ATOM_WidgetIdleEvent *event);

	ATOM_DECLARE_EVENT_MAP(ATOM_Checkbox, ATOM_Widget)

protected:
	bool _mouseOn;
	int _checkedImageId;
	bool _checked;

	int _clickSound;
	int _hoverSound;
};



#endif // __ATOM3D_ENGINE_CHECKBOX_H
/*! @} */
