/**	\file circleprogress.h
 *	环形进度条类.
 *
 *	\author 白大使
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_CIRCLEPROGRESS_H
#define __ATOM3D_ENGINE_CIRCLEPROGRESS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"

#include "gui_font.h"
#include "widget.h"


//! 环形进度条
class ATOM_ENGINE_API ATOM_CircleProgress: public ATOM_Widget
{
public:
	ATOM_CircleProgress();
	//! 构造函数
	ATOM_CircleProgress (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);

	//! 析构函数
	virtual ~ATOM_CircleProgress (void);

public:
	virtual ATOM_WidgetType getType (void) const;

public:
	//! 设置前景图片
	void setFrontImageId(int id);

	//! 取得前景图片
	int getFrontImageId() const;

	//! 设置当前位置 
	void setCurrent(int val);

	//! 取得当前值
	int getCurrent() const;

	//! 设置最大值
	void setMax(int maxValue);
	
	//! 取得最大值
	int getMax() const;

	//! 设置为反向转动
	void setInvert(bool invert);

	//! 是否为反向转动
	bool isInvert() const;

public:
	void onPaint (ATOM_WidgetDrawClientEvent *event);

	ATOM_DECLARE_EVENT_MAP(ATOM_CircleProgress, ATOM_Widget)

protected:
	int _currentValue;
	int _maxValue;
	int _frontImageId;
	bool _invert;
};



#endif // __ATOM3D_ENGINE_CIRCLEPROGRESS_H
/*! @} */
