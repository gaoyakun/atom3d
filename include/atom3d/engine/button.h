/**	\file button.h
 *	��ť��.
 *
 *	\author ������
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
//! ��ť��.
class ATOM_ENGINE_API ATOM_Button: public ATOM_Label
{
public:
	ATOM_Button();
	ATOM_Button (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::ShowNormal);
	virtual ~ATOM_Button (void);

public:
	virtual ATOM_WidgetType getType (void) const;

public:
	//! ���õ����Ч
	void setClickSound(int sound);

	//! ȡ�õ����Ч
	int getClickSound() const;

	//! ���ý�����Ч
	void setHoverSound(int sound);

	//! ȡ�ý�����Ч
	int getHoverSound() const;

	void setLua(const char* pParam);

	const char* getLua(void);

	void loadMouseEnterAnimator(ATOM_TiXmlElement* elem);
	void loadMouseLeaveAnimator(ATOM_TiXmlElement* elem);

	//! ���õ�����
	static void setClickInterval(unsigned interval);

	//! ȡ�õ�����
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
