/**	\file combobox.h
 *	��ѡ����.
 *
 *	\author �״�ʹ
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_COMBOBOX_H
#define __ATOM3D_ENGINE_COMBOBOX_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"

#include "gui_font.h"
#include "widget.h"
#include "WidgetAnimator.h"
#include "edit.h"
#include "button.h"
#include "listbox.h"
#include "menu.h"

//! ��Ͽ�
class ATOM_ENGINE_API ATOM_ComboBox: public ATOM_Widget
{
public:
	enum
	{
		ID_INPUTEDIT = 1001,
		ID_DROPDOWNBUTTON = 1002,
	};

	ATOM_ComboBox();
	ATOM_ComboBox (ATOM_Widget *parent, const ATOM_Rect2Di &rect, int buttonWidth, unsigned style, int id, ATOM_Widget::ShowState showState = ATOM_Widget::ShowNormal);
	virtual ~ATOM_ComboBox (void);

public:
	virtual ATOM_WidgetType getType (void) const;

public:
	//! ���ð�ť���
	void setButtonWidth(int w);

	//! ȡ�ð�ť���
	int getButtonWidth() const;

	void setButtonImageId(int id);

	int getButtonImageId (void) const;

	void setDropDownHeight (int h);

	int getDropDownHeight (void) const;

	void setDropDownListImageId (int id);

	int getDropDownListImageId (void) const;

	void setDropDownListHilightImageId (int id);

	int getDropDownListHilightImageId (void) const;

	void setInputEditImageId (int id);

	int getInputEditImageId (void) const;

	void setDropDownListItemHeight (int height);

	int getDropDownListItemHeight (void) const;

	//! ȡ������ı༭��
	ATOM_Edit* getInputEdit();

	//! ȡ��������ť
	ATOM_Button* getDropDownButton();

	//! ȡ�������б�
	ATOM_ListBox* getDropDownList();

	void select (int index, bool sendEvent = true);

	void selectString (const char *str, bool caseSensitive);

	int getSelected (void) const;

	void clear (void);

	virtual void show (ATOM_Widget::ShowState showState);


	/// ������
	int insertItem(int index, const char* str, unsigned long long userData = 0);

	/// ������
	int insertItem(int index, const char* str, ATOM_ColorARGB _color, unsigned long long userData = 0);

	//! ��������
	void setItemText(int index, const char *str);

	//! ��������ƫ��
	void setOffsetX(int x);

	//! ��ȡ����ƫ��
	int getOffsetX()const;

	//! ���ù�����������

	virtual void setscrollBarWidth (int w);

	virtual void setVerticalSCrollBarImageId(int backImageId, int sliderImageId, int upButtonImageId, int downButtonImageId );

	//! ���ô�ֱ������ĳ���
	virtual void setScrollBarSliderLength(int len);

	//! ���ô�ֱ��������ť�ĳ���
	virtual void setScrollBarButtonLength(int len);

	void setIsUseComboxFontColor(bool bUse);
public:
	void onCommand (ATOM_WidgetCommandEvent *event);
	void onResize (ATOM_WidgetResizeEvent *event);
	void onListBoxHoverChanged (ATOM_ListBoxHoverItemChangedEvent *event);
	void onPaint (ATOM_WidgetDrawClientEvent *event);

	ATOM_DECLARE_EVENT_MAP(ATOM_ComboBox, ATOM_Widget)

private:
	void showDropDownList (void);

protected:
	ATOM_Edit* _inputEdit;
	ATOM_Button* _dropDownButton;
	ATOM_ListBox* _poplist;
	int _selectIndex;
	int _buttonWidth;
	int _buttonImageId;
	int _listboxHeight;
	int _listboxImageId;
	int _listboxHilightImageId;
	int _editImageId;
	int _listboxItemHeight;
	int _offSetX;
	int _buttonLength;
	int _sliderLength;
	int _sliderWidth;
	bool _isUseCbFontColor;
	ATOM_GUIImage * _vUpButtonImage;
	ATOM_GUIImage * _vDownButtonImage;
	ATOM_GUIImage * _vSliderImage;
	ATOM_GUIImage * _vSliderHandleImage;
};

#endif // __ATOM3D_ENGINE_COMBOBOX_H
/*! @} */
