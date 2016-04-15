/**	\file progressbar.h
 *	进度条类.
 *
 *	\author 白大使
 *	\addtogroup engine
 *	@{
 */


#ifndef __ATOM3D_ENGINE_PROGRESSBAR_H
#define __ATOM3D_ENGINE_PROGRESSBAR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"

#include "gui_font.h"
#include "widget.h"
#include "label.h"



/// 进度条
class ATOM_ENGINE_API ATOM_ProgressBar: public ATOM_Label
{
public:
	ATOM_ProgressBar();
	ATOM_ProgressBar (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::Hide,bool isHorizontal = true);
	virtual ~ATOM_ProgressBar (void);

public:
	virtual ATOM_WidgetType getType (void) const;

public:
	//! 设置前景图片
	void setFrontImageId(int id);

	//! 取得前景图片
	int getFrontImageId() const;

	//! 设置当前值
	void setCurrent(int val, bool resetText = true);

	//! 取得当前值
	int getCurrent() const;

	//! 设置最小最大值
	void setMinMax(int minValue, int maxValue);
	
	//! 取得最小值
	int getMin() const;

	//! 取得最大值
	int getMax() const;
	
	void enableDrawText (bool draw);

	bool isDrawTextEnabled (void) const;

	//! 设置是否为垂直进度条
	void setIsHorizontal(bool isHorizontal);
public:
	void onPaint (ATOM_WidgetDrawClientEvent *event);

	ATOM_DECLARE_EVENT_MAP(ATOM_ProgressBar, ATOM_Widget)

protected:
	int _currentValue;
	int _minValue;
	int _maxValue;
	int _frontImageId;
	bool _drawText;
	bool _isHorizontal;
};



#endif // __ATOM3D_ENGINE_PROGRESSBAR_H
/*! @} */
