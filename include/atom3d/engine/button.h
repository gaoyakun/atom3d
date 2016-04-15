/**	\file button.h
 *	按钮类.
 *
 *	\author 高雅昆
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_BUTTON_H
#define __ATOM3D_ENGINE_BUTTON_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"

#include "gui_font.h"
#include "label.h"
#include "WidgetAnimator.h"



//! \class ATOM_Button
//! 按钮类.
class ATOM_ENGINE_API ATOM_Button: public ATOM_Label
{
public:
	ATOM_Button();
	ATOM_Button (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::ShowNormal);
	virtual ~ATOM_Button (void);

public:
	virtual ATOM_WidgetType getType (void) const;

public:
	//! 设置点击音效
	void setClickSound(int sound);

	//! 取得点击音效
	int getClickSound() const;

	//! 设置进过音效
	void setHoverSound(int sound);

	//! 取得进过音效
	int getHoverSound() const;

	void setLua(const char* pParam);

	const char* getLua(void);

	void loadMouseEnterAnimator(ATOM_TiXmlElement* elem);
	void loadMouseLeaveAnimator(ATOM_TiXmlElement* elem);

	//! 设置点击间隔
	static void setClickInterval(unsigned interval);

	//! 取得点击间隔
	static unsigned getClickInterval();

protected:
	virtual void onClicked (unsigned keymod);
	virtual void onRClicked (unsigned keymod);

protected:
	bool _mouseOn;
	ATOM_WidgetAnimator* _mouseEnterAnimator;
	ATOM_WidgetAnimator* _mouseLeaveAnimator;
	ATOM_WidgetAnimator* _currentAnimator;

	int _clickSound;
	int _hoverSound;
	ATOM_STRING _strParam;
	static unsigned _clickTick;
	static unsigned _clickInterval;

public:
	void onLButtonDown (ATOM_WidgetLButtonDownEvent *event);
	void onLButtonUp (ATOM_WidgetLButtonUpEvent *event);
	void onLButtonDblClick (ATOM_WidgetLButtonDblClickEvent *event);
	void onRButtonDown (ATOM_WidgetRButtonDownEvent *event);
	void onRButtonUp (ATOM_WidgetRButtonUpEvent *event);
	void onMouseEnter (ATOM_WidgetMouseEnterEvent *event);
	void onMouseLeave (ATOM_WidgetMouseLeaveEvent *event);
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onPaintFrame (ATOM_WidgetDrawFrameEvent *event);
	void onIdle(ATOM_WidgetIdleEvent *event);

	ATOM_DECLARE_EVENT_MAP(ATOM_Button, ATOM_Widget)
};



#endif // __ATOM3D_ENGINE_BUTTON_H
/*! @} */
