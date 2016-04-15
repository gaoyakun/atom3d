/**	\file checkbox.h
 *	��ѡ����.
 *
 *	\author �״�ʹ
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



//! ��ѡ��
class ATOM_ENGINE_API ATOM_Checkbox: public ATOM_Label
{
public:
	ATOM_Checkbox();
	ATOM_Checkbox (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::ShowNormal);
	virtual ~ATOM_Checkbox (void);

public:
	virtual ATOM_WidgetType getType (void) const;

public:
	//! ����ѡ��ͼƬ
	void setCheckedImageId(int imageId);

	// ȡ��ѡ��ͼƬ
	int getCheckedImageId() const;

	//! ѡ��
	void setChecked(bool checked);

	//! �Ƿ�ѡ��
	bool getChecked(void) const;

	//! ���õ����Ч
	void setClickSound(int sound);

	//! ȡ�õ����Ч
	int getClickSound() const;

	//! ���ý�����Ч
	void setHoverSound(int sound);

	//! ȡ�ý�����Ч
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
