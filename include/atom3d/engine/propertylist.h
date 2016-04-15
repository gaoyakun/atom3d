/**	\file propertylist.h
 *	属性列表类.
 *
 *	\author 白大使
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

//! 属性列表.
//! 属性列表是一个两列的ATOM_ListView,由名字列和数值列组成.
class ATOM_ENGINE_API ATOM_PropertyList: public ATOM_ListView
{
public:
	ATOM_PropertyList();
	ATOM_PropertyList (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);
	virtual ~ATOM_PropertyList (void);

public:
	virtual ATOM_WidgetType getType (void) const;

	/// 设置指定列的宽度
	virtual void setColumnWidth(int nameWidth, int valueWidth);

	/// 将目标格子设置为标签
	ATOM_Label* setLabel(int raw, const char* name, const char* str, int id=ATOM_Widget::AnyId, unsigned style=0, int customButtonId=ATOM_Widget::AnyId);

	/// 将目标格子设置为编辑框
	ATOM_Edit* setEdit(int raw, const char* name, const char* str, int id=ATOM_Widget::AnyId, unsigned style=0, int customButtonId=ATOM_Widget::AnyId);

	/// 将目标格子设置为按钮
	ATOM_Button* setButton(int raw, const char* name, const char* str, int id=ATOM_Widget::AnyId, unsigned style=0, int customButtonId=ATOM_Widget::AnyId);

	/// 将目标格子设置为横向滚动条
	ATOM_Slider* setHSlider(int raw, const char* name, int min, int max, int pos=0, int id=ATOM_Widget::AnyId, unsigned style=0, int customButtonId=ATOM_Widget::AnyId);

	/// 将目标格子设置为横向滚动条
	ATOM_ScrollBar* setHScrollBar(int raw, const char* name, float min, float max, float pos=0.f, float step=1.f, int id=ATOM_Widget::AnyId, unsigned style=0, int customButtonId=ATOM_Widget::AnyId);

	/// 将目标格子设置为横向复选框
	ATOM_Checkbox* setCheckbox(int raw, const char* name, bool checked, int id=ATOM_Widget::AnyId, unsigned style=0, int customButtonId=ATOM_Widget::AnyId);

	//! 取得名字列的控件
	ATOM_Widget* getNameWidget(int raw);

	//! 取得数据列的控件
	ATOM_Widget* getValueWidget(int raw);
};



#endif // __ATOM3D_ENGINE_PROPERTYLIST_H
/*! @} */
