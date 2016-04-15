/**	\file scrollbar.h
 *	滚动条类.
 *
 *	\author 高雅坤
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_SCROLLBAR_H
#define __ATOM3D_ENGINE_SCROLLBAR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"

#include "gui_font.h"
#include "widget.h"



class ATOM_Button;
class ATOM_Slider;

class ATOM_ENGINE_API ATOM_ScrollBar: public ATOM_Widget
{
public:
	enum
	{
		ID_BUTTONUP = 1,
		ID_BUTTONDOWN = 2,
		ID_SLIDER = 3
	};

public:
	ATOM_ScrollBar();
	ATOM_ScrollBar (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);
	virtual ~ATOM_ScrollBar (void);

public:
	virtual ATOM_WidgetType getType (void) const;

public:
	void setVerticalImageId(int backImageId, int sliderImageId, int upButtonImageId, int downButtonImageId);
	void setHorizontalImageId(int backImageId, int sliderImageId, int upButtonImageId, int downButtonImageId);
	void setPlacement (ATOM_WidgetPlacement placement);
	ATOM_WidgetPlacement getPlacement (void) const;
	void setButtonWidth (int width);
	int getButtonWidth (void) const;
	void setHandleWidth (int width);
	int getHandleWidth (void) const;
	void setRange (float rangeMin, float rangeMax);
	float getRangeMin (void) const;
	float getRangeMax (void) const;
	void setStep (float step);
	float getStep (void) const;
	void setPosition (float position, bool postEvent = true);
	float getPosition (void) const;
	ATOM_Slider* getSlider();
	ATOM_Button* getUpButton();
	ATOM_Button* getDownButton();
public:
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onResize (ATOM_WidgetResizeEvent *event);
	void onScroll (ATOM_ScrollEvent *event);
	void onCommand (ATOM_WidgetCommandEvent *event);
	void onMouseWheel(ATOM_WidgetMouseWheelEvent *event);
protected:
	ATOM_Button *_upButton;
	ATOM_Button *_downButton;
	ATOM_Slider *_slider;
	int _buttonWidth;

	int _hUpButtonImageId;
	int _vUpButtonImageId;
	int _hDownButtonImageId;
	int _vDownButtonImageId;
	int _hSliderImageId;
	int _vSliderImageId;
	int _hSliderHandleImageId;
	int _vSliderHandleImageId;

	float _rangeStep;

	ATOM_DECLARE_EVENT_MAP(ATOM_ScrollBar, ATOM_Widget)
};



#endif // __ATOM3D_ENGINE_SCROLLBAR_H
/*! @} */
