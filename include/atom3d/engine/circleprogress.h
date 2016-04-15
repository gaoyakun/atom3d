/**	\file circleprogress.h
 *	���ν�������.
 *
 *	\author �״�ʹ
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


//! ���ν�����
class ATOM_ENGINE_API ATOM_CircleProgress: public ATOM_Widget
{
public:
	ATOM_CircleProgress();
	//! ���캯��
	ATOM_CircleProgress (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);

	//! ��������
	virtual ~ATOM_CircleProgress (void);

public:
	virtual ATOM_WidgetType getType (void) const;

public:
	//! ����ǰ��ͼƬ
	void setFrontImageId(int id);

	//! ȡ��ǰ��ͼƬ
	int getFrontImageId() const;

	//! ���õ�ǰλ�� 
	void setCurrent(int val);

	//! ȡ�õ�ǰֵ
	int getCurrent() const;

	//! �������ֵ
	void setMax(int maxValue);
	
	//! ȡ�����ֵ
	int getMax() const;

	//! ����Ϊ����ת��
	void setInvert(bool invert);

	//! �Ƿ�Ϊ����ת��
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
