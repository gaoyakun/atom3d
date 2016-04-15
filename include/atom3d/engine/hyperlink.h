/**	\file hyperlink.h
 *	��������.
 *
 *	\author �״�ʹ
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



/// ������.
/// �¼�:���ATOM_WidgetCommandEvent
class ATOM_ENGINE_API ATOM_HyperLink: public ATOM_Label
{
public:
	ATOM_HyperLink();
	ATOM_HyperLink (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::ShowNormal);
	virtual ~ATOM_HyperLink (void);

public:
	virtual ATOM_WidgetType getType (void) const;

public:
	//! ��������
	//! \param str ������ַ����������Ϊ����suitWidth��������
	//! \param suitWidth �Ƿ������ȿؼ����Ժ��ʸ��ִ�
	void setText (const char *str, bool suitWidth = false);

	//! ������꾭��������ɫ
	void setHoverFontColor (const ATOM_ColorARGB& clr);

	//! ȡ����꾭��������ɫ
	const ATOM_ColorARGB& getHoverFontColor() const;

	//! ���õ����Ч
	void setClickSound(int sound);

	//! ȡ�õ����Ч
	int getClickSound() const;

	//! ���ý�����Ч
	void setHoverSound(int sound);

	//! ȡ�ý�����Ч
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
