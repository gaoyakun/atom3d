/**	\file slider.h
 *	滚动条类.
 *
 *	\author 高雅坤
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_SLIDER_H
#define __ATOM3D_ENGINE_SLIDER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"
#include "widget.h"



class ATOM_ENGINE_API ATOM_Slider: public ATOM_Widget
{
public:
	ATOM_Slider();
	ATOM_Slider (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);
	virtual ~ATOM_Slider (void);
	virtual void setFont (ATOM_GUIFont::handle font);

public:
	virtual ATOM_WidgetType getType (void) const;

public:
	void setVerticalHandleImageId (int imageId);
	int getVerticalHandleImageId (void) const;
	void setHorizontalHandleImageId (int imageId);
	int getHorizontalHandleImageId (void) const;
	void setVerticalSliderImageId (int imageId);
	int getVerticalSliderImageId (void) const;
	void setHorizontalSliderImageId (int imageId);
	int getHorizontalSliderImageId (void) const;
	void setRange (int rangeMin, int rangeMax);
	int getRangeMin (void) const;
	int getRangeMax (void) const;
	void setPosition (int position, bool postEvent = true);
	int getPosition (void) const;
	void setPlacement (ATOM_WidgetPlacement placement);
	ATOM_WidgetPlacement getPlacement (void) const;
	void setHandleWidth (int w);
	int getHandleWidth (void) const;
	void showText(bool b);
	void setHandleImage(ATOM_GUIImage * image);

private:
	void updatePosition (int x, int y);

public:
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onLButtonDown (ATOM_WidgetLButtonDownEvent *event);
	void onLButtonUp (ATOM_WidgetLButtonUpEvent *event);
	void onMouseMove (ATOM_WidgetMouseMoveEvent *event);
protected:
	int _hHandleImageId;
	int _vHandleImageId;
	int _hClientImageId;
	int _vClientImageId;
	int _rangeMin;
	int _rangeMax;
	int _rangePos;
	int _handleWidth;
	ATOM_WidgetPlacement _placement;
	ATOM_WidgetState _handleState;
	ATOM_WeakPtrT<ATOM_GUIImage> _handle;
	ATOM_AUTOREF(ATOM_Text) _text;
	bool _showText;
	enum
	{
		ID_HANDLE = 1
	};
	ATOM_DECLARE_EVENT_MAP(ATOM_Slider, ATOM_Widget)
};


#endif // __ATOM3D_ENGINE_SLIDER_H
/*! @} */
