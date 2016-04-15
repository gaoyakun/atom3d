/**	\file label.h
 *	��ǩ����.
 *
 *	\author �״�ʹ
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_LABEL_H
#define __ATOM3D_ENGINE_LABEL_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"

#include "gui_font.h"
#include "panel.h"



//! ��ǩ��
class ATOM_ENGINE_API ATOM_Label: public ATOM_Panel
{
public:
	ATOM_Label(void);
	ATOM_Label (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);
	virtual ~ATOM_Label (void);

public:
	virtual ATOM_WidgetType getType (void) const;
	virtual void setFont (ATOM_GUIFont::handle font);

public:
	//! ��������
	void setText (const char *str);

	//! ȡ������
	const char *getText (void) const;

	//! ���ö���
	void setAlign(int align = AlignX_Middle|AlignY_Middle);

	//! ȡ�ö���
	int getAlign() const;

	//! ��������ƫ��
	void setOffset(const ATOM_Point2Di& offset);

	//! ȡ������ƫ��
	const ATOM_Point2Di& getOffset() const;

	//! �����Զ�����ɫ
	void setColor(ATOM_ColorARGB color);

	//! ȡ���Զ�����ɫ
	ATOM_ColorARGB getColor() const;

	//! ������ת
	void setRotation(float f);

	//! ȡ����ת
	float getRotation() const;

	//! ���������ɫ
	void setOutlineColor(ATOM_ColorARGB color);

	//! ȡ�������ɫ
	ATOM_ColorARGB getOutlineColor() const;

	//! �����»�����ɫ
	void setUnderlineColor(ATOM_ColorARGB color);

	//! ȡ���»�����ɫ
	ATOM_ColorARGB getUnderlineColor() const;

public:
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onResize (ATOM_WidgetResizeEvent *event);

	ATOM_DECLARE_EVENT_MAP(ATOM_Label, ATOM_Panel)

protected:
	virtual void calcTextLayout (void);

protected:
	ATOM_STRING _caption;
	ATOM_Point2Di _textPosition;
	int _align;
	ATOM_Point2Di _offset;
	ATOM_AUTOREF(ATOM_Text) _text;
	bool _textDirty;
	ATOM_ColorARGB _customColor;
//	ATOM_ColorARGB _outlineColor;
	ATOM_ColorARGB _underlineColor;
	float _rotation;
};



#endif // __ATOM3D_ENGINE_LABEL_H
/*! @} */
