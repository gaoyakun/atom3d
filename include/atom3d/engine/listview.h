/**	\file listview.h
 *	表格类.
 *
 *	\author 白大使
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_LISTVIEW_H
#define __ATOM3D_ENGINE_LISTVIEW_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_render.h"

#include "gui_font.h"
#include "widget.h"
#include "label.h"
#include "edit.h"
#include "checkbox.h"
#include "button.h"
#include "combobox.h"




//////////////////////////////////////////////////////////////////////////

class ATOM_LVColumn
{
public:
	ATOM_LVColumn();
	~ATOM_LVColumn();
		
	void addWidget(ATOM_Widget* widget);
	void setWidget(int index, ATOM_Widget* widget);	
	ATOM_Widget* getWidget(int index);
	int getWidgetCount() const;
	void clear();

	void setWidth(int width);
	int getWidth() const;	


private:
	typedef ATOM_LIST<ATOM_Widget*> WidgetList;
	WidgetList _widgetList;
	int _width;
};

//////////////////////////////////////////////////////////////////////////

//! 表格
class ATOM_ENGINE_API ATOM_ListView: public ATOM_Widget
{
public:
	ATOM_ListView();
	ATOM_ListView (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);
	virtual ~ATOM_ListView (void);

public:
	virtual ATOM_WidgetType getType (void) const;

	//! 设置列数
	virtual void setColumnCount(int num);

	//! 取得列数
	virtual int getColumnCount();

	//! 设置指定列的宽度
	virtual void setColumnWidth(int index, int width);

	//! 取得指定列的宽度
	virtual int getColumnWidth(int index);

	//! 清空数据
	void clear();

	//! 添加一行
	int addRow();

	//! 设置行数
	void setRowCount(int num);

	//! 取得行数
	int getRowCount() const;

	//! 设置格子数据
	void setWidget(int x, int y, ATOM_Widget* widget);

	//! 将目标格子设置为标签
	ATOM_Label* setLabel(int x, int y, const char* str, int id=ATOM_Widget::AnyId, unsigned style=0);

	//! 将目标格子设置为编辑框
	ATOM_Edit* setEdit(int x, int y, const char* str, int id=ATOM_Widget::AnyId, unsigned style=0);

	//! 将目标格子设置为富文本
	ATOM_RichEdit* setRichEdit(int x, int y, const char* str, int id=ATOM_Widget::AnyId, unsigned style=0);

	//! 将目标格子设置为按钮
	ATOM_Button* setButton(int x, int y, const char* str, int id=ATOM_Widget::AnyId, unsigned style=0);

	//! 将目标格子设置为横向滚动条
	ATOM_Slider* setHSlider(int x, int y, int min, int max, int pos=0, int id=ATOM_Widget::AnyId, unsigned style=0);

	//! 将目标格子设置为横向滚动条
	ATOM_ScrollBar* setHScrollBar(int x, int y, float min, float max, float pos=0.f, float step=1.f, int id=ATOM_Widget::AnyId, unsigned style=0);

	//! 将目标格子设置为横向复选框
	ATOM_Checkbox* setCheckbox(int x, int y, bool checked, int id=ATOM_Widget::AnyId, unsigned style=0);

	//! 将目标格子设置为横向组合框
	ATOM_ComboBox* setComboBox(int x, int y, int buttonWidth, int id=ATOM_Widget::AnyId, unsigned style=0);


	//! 取得目标数据
	ATOM_Widget* getWidget(int x, int y);	

	//! 设置项的高度
	void setItemHeight(int height);

	//! 取得项高度
	int getItemHeight() const;

	//! 设置选择的图片
	void setSelectImage(int imageId);

	//! 取得选择的图片
	int getSelectImage() const;

	//! 设置移动鼠标时的高亮图片
	void setHoverImage(int imageId);
	
	//! 取得移动鼠标时的高亮图片
	int	getHoverImage()const;
	//! 选择
	void select(int index, bool postEvent = true);

	//! 取得选择的项
	int getSelect() const;

	//! 设置单行背景图片
	void setSingleRowBackGroundImageId(int imageId);

	//! 获取单行背景图片
	int getSingleRowBackGroundImageId() const;
	
public:
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onLButtonDown (ATOM_WidgetLButtonDownEvent *event);
	void onMouseMove(ATOM_WidgetMouseMoveEvent * event);
	void onMouseLeave (ATOM_WidgetMouseLeaveEvent *event);
	ATOM_DECLARE_EVENT_MAP(ATOM_ListView, ATOM_Widget)

protected:
	typedef ATOM_VECTOR<ATOM_LVColumn> ColumnList;
	int _xSize, _ySize;
	int _itemHeight;
	ColumnList _columnList;
	int _select;
	int _selectImage;
	int _hoverRowIndex;
	int _hoverRowImage;
	int _singleRowBackGroundImage; // 单行背景图片
};



#endif // __ATOM3D_ENGINE_LISTVIEW_H
/*! @} */
