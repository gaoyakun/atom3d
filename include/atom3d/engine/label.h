/**	\file label.h
 *	标签框类.
 *
 *	\author 白大使
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



//! 标签框
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
	//! 设置文字
	void setText (const char *str);

	//! 取得文字
	const char *getText (void) const;

	//! 设置对齐
	void setAlign(int align = AlignX_Middle|AlignY_Middle);

	//! 取得对齐
	int getAlign() const;

	//! 设置文字偏移
	void setOffset(const ATOM_Point2Di& offset);

	//! 取得文字偏移
	const ATOM_Point2Di& getOffset() const;

	//! 设置自定义颜色
	void setColor(ATOM_ColorARGB color);

	//! 取得自定义颜色
	ATOM_ColorARGB getColor() const;

	//! 设置旋转
	void setRotation(float f);

	//! 取得旋转
	float getRotation() const;

	//! 设置描边颜色
	void setOutlineColor(ATOM_ColorARGB color);

	//! 取得描边颜色
	ATOM_ColorARGB getOutlineColor() const;

	//! 设置下划线颜色
	void setUnderlineColor(ATOM_ColorARGB color);

	//! 取得下划线颜色
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
