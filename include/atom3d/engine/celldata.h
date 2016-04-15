/**	\file celldata.h
 *	������.
 *
 *	\author �״�ʹ
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_CELLDATA_H
#define __ATOM3D_ENGINE_CELLDATA_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"

#include "gui_font.h"
#include "label.h"


//! \class ATOM_CellData
//! ������.
class ATOM_ENGINE_API ATOM_CellData: public ATOM_Label
{
public:
	ATOM_CellData(void);
	ATOM_CellData (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::ShowNormal);
	virtual ~ATOM_CellData (void);

public:
	virtual ATOM_WidgetType getType (void) const;

public:
	//! ����CDʱ��
	void setCooldown(unsigned cooldown);

	//! ȡ��CDʱ��
	unsigned getCooldown() const;

	//! ����CD���ֵ
	void setCooldownMax(unsigned cooldownMax);

	//! ȡ��CD���ֵ
	unsigned getCooldownMax() const;

	//! ����CDͼƬ
	void setCooldownImageId (int id);

	//! ȡ��CDͼƬ
	int getCooldownImageId (void) const;

	//! ����ѡ��ͼƬ
	void setSelectImageId (int id);

	//! ȡ��ѡ��ͼƬ
	int getSelectImageId (void) const;

	//! ���ñ߿�ͼƬ
	void setFrameImageId (int id);

	//! ȡ�ñ߿�ͼƬ
	int getFrameImageId (void) const;

	//! ���ñ߿�ƫ��
	void setFrameOffset(int offset);

	//! ȡ�ñ߿�ƫ��
	int getFrameOffset (void) const;

	//! �����±�ͼƬ
	void setSubscriptImageId (int id);

	//! ȡ���±�ͼƬ
	int getSubscriptImageId()const;

	//! �����±�ͼƬλ��
	void setSubscriptImageRect(ATOM_Rect2Di& rect);

	//! ȡ���±�ͼƬλ��
	ATOM_Rect2Di getSubscriptImageRect()const;

	//! ��������
	void setSubscriptText (const char *str);

	//! ȡ������
	const char *getSubscriptText (void) const;

	//! �����±�����λ��
	void setSubscriptTextPosition(ATOM_Point2Di & point);

	//! ��ȡ�±�����λ��
	ATOM_Point2Di getSubscriptTextPosition() const;

	void allowClientDragging (bool allow);
	bool isClientDraggingAllowed (void) const;
protected:
	void calcSubscriptTextLayout (void);
protected:
	int						_cooldownImageId;
	int						_selectImageId;
	int						_frameImageId;
	int						_frameOffset;
	unsigned				_cooldown;
	unsigned				_cooldownMax;
	ATOM_Point2Di			_oldPosition;
	bool					_clientDragging;

	int						_subscriptImageId;		//!< �±�ͼƬ
	ATOM_Rect2Di			_subscriptRect;			//!< �±�ͼƬλ��
	ATOM_Point2Di			_subscriptTextPosition;	//!< �±�����λ��
	ATOM_AUTOREF(ATOM_Text) _subscriptText;			//!< �±�����
	ATOM_STRING				_subscriptStr;			//!< �±��ı�
	bool					_subscirptTextDirty;

public:
	void onLButtonDown (ATOM_WidgetLButtonDownEvent *event);
	void onLButtonUp (ATOM_WidgetLButtonUpEvent *event);
	void onRButtonDown (ATOM_WidgetRButtonDownEvent *event);
	void onRButtonUp (ATOM_WidgetRButtonUpEvent *event);
	void onDblClick (ATOM_WidgetLButtonDblClickEvent *event);
	void onMouseEnter (ATOM_WidgetMouseEnterEvent *event);
	void onMouseLeave (ATOM_WidgetMouseLeaveEvent *event);
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onIdle(ATOM_WidgetIdleEvent *event);
	void onHitTest (ATOM_WidgetHitTestEvent *event);
	void onDragStart (ATOM_WidgetDragStartEvent *event);
	void onDragOver (ATOM_WidgetDragOverEvent *event);
	void onDragDrop (ATOM_WidgetDragDropEvent *event);

	ATOM_DECLARE_EVENT_MAP(ATOM_CellData, ATOM_Label)
};



#endif // __ATOM3D_ENGINE_CELLDATA_H
/*! @} */
