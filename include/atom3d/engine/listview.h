/**	\file listview.h
 *	�����.
 *
 *	\author �״�ʹ
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

//! ���
class ATOM_ENGINE_API ATOM_ListView: public ATOM_Widget
{
public:
	ATOM_ListView();
	ATOM_ListView (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);
	virtual ~ATOM_ListView (void);

public:
	virtual ATOM_WidgetType getType (void) const;

	//! ��������
	virtual void setColumnCount(int num);

	//! ȡ������
	virtual int getColumnCount();

	//! ����ָ���еĿ��
	virtual void setColumnWidth(int index, int width);

	//! ȡ��ָ���еĿ��
	virtual int getColumnWidth(int index);

	//! �������
	void clear();

	//! ���һ��
	int addRow();

	//! ��������
	void setRowCount(int num);

	//! ȡ������
	int getRowCount() const;

	//! ���ø�������
	void setWidget(int x, int y, ATOM_Widget* widget);

	//! ��Ŀ���������Ϊ��ǩ
	ATOM_Label* setLabel(int x, int y, const char* str, int id=ATOM_Widget::AnyId, unsigned style=0);

	//! ��Ŀ���������Ϊ�༭��
	ATOM_Edit* setEdit(int x, int y, const char* str, int id=ATOM_Widget::AnyId, unsigned style=0);

	//! ��Ŀ���������Ϊ���ı�
	ATOM_RichEdit* setRichEdit(int x, int y, const char* str, int id=ATOM_Widget::AnyId, unsigned style=0);

	//! ��Ŀ���������Ϊ��ť
	ATOM_Button* setButton(int x, int y, const char* str, int id=ATOM_Widget::AnyId, unsigned style=0);

	//! ��Ŀ���������Ϊ���������
	ATOM_Slider* setHSlider(int x, int y, int min, int max, int pos=0, int id=ATOM_Widget::AnyId, unsigned style=0);

	//! ��Ŀ���������Ϊ���������
	ATOM_ScrollBar* setHScrollBar(int x, int y, float min, float max, float pos=0.f, float step=1.f, int id=ATOM_Widget::AnyId, unsigned style=0);

	//! ��Ŀ���������Ϊ����ѡ��
	ATOM_Checkbox* setCheckbox(int x, int y, bool checked, int id=ATOM_Widget::AnyId, unsigned style=0);

	//! ��Ŀ���������Ϊ������Ͽ�
	ATOM_ComboBox* setComboBox(int x, int y, int buttonWidth, int id=ATOM_Widget::AnyId, unsigned style=0);


	//! ȡ��Ŀ������
	ATOM_Widget* getWidget(int x, int y);	

	//! ������ĸ߶�
	void setItemHeight(int height);

	//! ȡ����߶�
	int getItemHeight() const;

	//! ����ѡ���ͼƬ
	void setSelectImage(int imageId);

	//! ȡ��ѡ���ͼƬ
	int getSelectImage() const;

	//! �����ƶ����ʱ�ĸ���ͼƬ
	void setHoverImage(int imageId);
	
	//! ȡ���ƶ����ʱ�ĸ���ͼƬ
	int	getHoverImage()const;
	//! ѡ��
	void select(int index, bool postEvent = true);

	//! ȡ��ѡ�����
	int getSelect() const;

	//! ���õ��б���ͼƬ
	void setSingleRowBackGroundImageId(int imageId);

	//! ��ȡ���б���ͼƬ
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
	int _singleRowBackGroundImage; // ���б���ͼƬ
};



#endif // __ATOM3D_ENGINE_LISTVIEW_H
/*! @} */
