/**	\file progressbar.h
 *	��������.
 *
 *	\author �״�ʹ
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



/// ������
class ATOM_ENGINE_API ATOM_ProgressBar: public ATOM_Label
{
public:
	ATOM_ProgressBar();
	ATOM_ProgressBar (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::Hide,bool isHorizontal = true);
	virtual ~ATOM_ProgressBar (void);

public:
	virtual ATOM_WidgetType getType (void) const;

public:
	//! ����ǰ��ͼƬ
	void setFrontImageId(int id);

	//! ȡ��ǰ��ͼƬ
	int getFrontImageId() const;

	//! ���õ�ǰֵ
	void setCurrent(int val, bool resetText = true);

	//! ȡ�õ�ǰֵ
	int getCurrent() const;

	//! ������С���ֵ
	void setMinMax(int minValue, int maxValue);
	
	//! ȡ����Сֵ
	int getMin() const;

	//! ȡ�����ֵ
	int getMax() const;
	
	void enableDrawText (bool draw);

	bool isDrawTextEnabled (void) const;

	//! �����Ƿ�Ϊ��ֱ������
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
