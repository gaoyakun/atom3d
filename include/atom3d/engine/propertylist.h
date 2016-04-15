/**	\file propertylist.h
 *	�����б���.
 *
 *	\author �״�ʹ
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_PROPERTYLIST_H
#define __ATOM3D_ENGINE_PROPERTYLIST_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_render.h"

#include "gui_font.h"
#include "listview.h"




//////////////////////////////////////////////////////////////////////////

//! �����б�.
//! �����б���һ�����е�ATOM_ListView,�������к���ֵ�����.
class ATOM_ENGINE_API ATOM_PropertyList: public ATOM_ListView
{
public:
	ATOM_PropertyList();
	ATOM_PropertyList (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);
	virtual ~ATOM_PropertyList (void);

public:
	virtual ATOM_WidgetType getType (void) const;

	/// ����ָ���еĿ��
	virtual void setColumnWidth(int nameWidth, int valueWidth);

	/// ��Ŀ���������Ϊ��ǩ
	ATOM_Label* setLabel(int raw, const char* name, const char* str, int id=ATOM_Widget::AnyId, unsigned style=0, int customButtonId=ATOM_Widget::AnyId);

	/// ��Ŀ���������Ϊ�༭��
	ATOM_Edit* setEdit(int raw, const char* name, const char* str, int id=ATOM_Widget::AnyId, unsigned style=0, int customButtonId=ATOM_Widget::AnyId);

	/// ��Ŀ���������Ϊ��ť
	ATOM_Button* setButton(int raw, const char* name, const char* str, int id=ATOM_Widget::AnyId, unsigned style=0, int customButtonId=ATOM_Widget::AnyId);

	/// ��Ŀ���������Ϊ���������
	ATOM_Slider* setHSlider(int raw, const char* name, int min, int max, int pos=0, int id=ATOM_Widget::AnyId, unsigned style=0, int customButtonId=ATOM_Widget::AnyId);

	/// ��Ŀ���������Ϊ���������
	ATOM_ScrollBar* setHScrollBar(int raw, const char* name, float min, float max, float pos=0.f, float step=1.f, int id=ATOM_Widget::AnyId, unsigned style=0, int customButtonId=ATOM_Widget::AnyId);

	/// ��Ŀ���������Ϊ����ѡ��
	ATOM_Checkbox* setCheckbox(int raw, const char* name, bool checked, int id=ATOM_Widget::AnyId, unsigned style=0, int customButtonId=ATOM_Widget::AnyId);

	//! ȡ�������еĿؼ�
	ATOM_Widget* getNameWidget(int raw);

	//! ȡ�������еĿؼ�
	ATOM_Widget* getValueWidget(int raw);
};



#endif // __ATOM3D_ENGINE_PROPERTYLIST_H
/*! @} */
