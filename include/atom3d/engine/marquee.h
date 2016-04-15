/**	\file marquee.h
 *	跑马灯类.
 *
 *	\author 白大使
 *	\addtogroup engine
 *	@{
 */


#ifndef __ATOM3D_ENGINE_MARQUEE_H
#define __ATOM3D_ENGINE_MARQUEE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"

#include "gui_font.h"
#include "widget.h"



//! 滚动方向
enum MARQUEEDIR
{
	MARQUEEDIR_LEFT,
	MARQUEEDIR_RIGHT,
};

enum MARQUEETYPE
{
	MARQUEETYPE_FATSCROLL = (1<<0),
};

//! 跑马灯.
class ATOM_ENGINE_API ATOM_Marquee: public ATOM_Widget
{
public:
	ATOM_Marquee();
	ATOM_Marquee (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::ShowNormal);
	virtual ~ATOM_Marquee (void);

public:
	virtual ATOM_WidgetType getType (void) const;
	virtual void setFont (ATOM_GUIFont::handle font);

public:
	//! 设置滚动方向
	void setDir(MARQUEEDIR dir);

	//! 取得滚动方向
	MARQUEEDIR getDir() const;

	//! 设置速度
	void setSpeed(float speed);

	//! 取得速度
	float getSpeed() const;

	//! 设置位置
	void setPosition(float pos);

	//! 取得位置
	float getPosition() const;

	//! 设置偏移
	void setOffset(float o);

	//! 取得偏移
	float getOffset() const;

	//! 取得字串宽度
	float getTextWidth() const;
	
	//! 设置文字
	void setText (const char *str);

	//! 取得文字
	ATOM_Text *getText (void) const;

protected:
	virtual void onClicked (void);

private:
	void calcTextLayout (void);

protected:
	bool _mouseOn;
	bool _textDirty;
	ATOM_STRING _caption;
	ATOM_AUTOREF(ATOM_Text) _text;
	int _y;
	float _textPosition;
	float _textWidth;
	MARQUEEDIR _dir;
	float _speed;
	float _offset;

public:
	void onLButtonDown (ATOM_WidgetLButtonDownEvent *event);
	void onLButtonUp (ATOM_WidgetLButtonUpEvent *event);
	void onMouseEnter (ATOM_WidgetMouseEnterEvent *event);
	void onMouseLeave (ATOM_WidgetMouseLeaveEvent *event);
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onPaintFrame (ATOM_WidgetDrawFrameEvent *event);
	void onIdle(ATOM_WidgetIdleEvent *event);
	void onResize (ATOM_WidgetResizeEvent *event);

	ATOM_DECLARE_EVENT_MAP(ATOM_Marquee, ATOM_Widget)
};



#endif // __ATOM3D_ENGINE_MARQUEE_H
/*! @} */
