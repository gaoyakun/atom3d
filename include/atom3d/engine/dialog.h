/**	\file dialog.h
 *	´°¿ÚÀà.
 *
 *	\author ¸ßÑÅÀ¤
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_DIALOG_H
#define __ATOM3D_ENGINE_DIALOG_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"
#include "topwindow.h"



class ATOM_ENGINE_API ATOM_Dialog: public ATOM_TopWindow
{
public:
	ATOM_Dialog();
	ATOM_Dialog (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style = ATOM_Widget::Border|ATOM_Widget::TitleBar|ATOM_Widget::CloseButton, int id = ATOM_Widget::AnyId, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);
	virtual ~ATOM_Dialog (void);

public:
	virtual ATOM_WidgetType getType (void) const;
	virtual void draw (void);

public:
	virtual int showModal (ATOM_ColorARGB darkColor = 0xFFFFFFFF);
	virtual void endModal (int retVal);

public:
	void addEndId (int id);
	void removeEndId (int id);

public:
	void onClose (ATOM_TopWindowCloseEvent *event);
	void onCommand (ATOM_WidgetCommandEvent *event);

protected:
	bool _isShowModal;
	bool _initialized;
	int _exitModalCode;
	ATOM_SET<int> _endIDs;

	struct LockScreenInfo
	{
		ATOM_ColorARGB lockColor;
		ATOM_Dialog *lockDialog;
	};
	static ATOM_VECTOR<LockScreenInfo> _lockScreenInfoStack;

	ATOM_DECLARE_EVENT_MAP(ATOM_Dialog, ATOM_TopWindow)
};


#endif // __ATOM3D_ENGINE_DIALOG_H
/*! @} */
