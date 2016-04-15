/**	\file hyperlink.h
 *	超链接类.
 *
 *	\author 白大使
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_HYPERLINK_H
#define __ATOM3D_ENGINE_HYPERLINK_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"

#include "gui_font.h"
#include "label.h"



/// 超链接.
/// 事件:点击ATOM_WidgetCommandEvent
class ATOM_ENGINE_API ATOM_HyperLink: public ATOM_Label
{
public:
	ATOM_HyperLink();
	ATOM_HyperLink (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::ShowNormal);
	virtual ~ATOM_HyperLink (void);

public:
	virtual ATOM_WidgetType getType (void) const;

public:
	//! 设置文字
	//! \param str 输入的字符串，如果过为空则suitWidth不起作用
	//! \param suitWidth 是否调整宽度控件，以合适该字串
	void setText (const char *str, bool suitWidth = false);

	//! 设置鼠标经过字体颜色
	void setHoverFontColor (const ATOM_ColorARGB& clr);

	//! 取得鼠标经过字体颜色
	const ATOM_ColorARGB& getHoverFontColor() const;

	//! 设置点击音效
	void setClickSound(int sound);

	//! 取得点击音效
	int getClickSound() const;

	//! 设置进过音效
	void setHoverSound(int sound);

	//! 取得进过音效
	int getHoverSound() const;

	void SetLua(const char* pParam);

	const char* GetLua(void);

	void SetParam(int);

	int GetParam();

protected:
	virtual void onClicked (void);

protected:
	ATOM_ColorARGB _normalColor;
	ATOM_ColorARGB _hoverColor;
	int _clickSound;
	int _hoverSound;
	ATOM_STRING m_strParam;
	int			m_iParam;	

public:
	void onLButtonDown (ATOM_WidgetLButtonDownEvent *event);
	void onLButtonUp (ATOM_WidgetLButtonUpEvent *event);
	void onMouseEnter (ATOM_WidgetMouseEnterEvent *event);
	void onMouseLeave (ATOM_WidgetMouseLeaveEvent *event);
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onPaintFrame (ATOM_WidgetDrawFrameEvent *event);
	void onIdle(ATOM_WidgetIdleEvent *event);
	void onResize (ATOM_WidgetResizeEvent *event);

	ATOM_DECLARE_EVENT_MAP(ATOM_HyperLink, ATOM_Widget)
};



#endif // __ATOM3D_ENGINE_HYPERLINK_H
/*! @} */
