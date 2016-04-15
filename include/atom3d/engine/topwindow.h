/**	\file topwindow.h
 *	顶层窗口类.
 *
 *	\author 高雅坤
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_TOPWINDOW_H
#define __ATOM3D_ENGINE_TOPWINDOW_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_render.h"

#include "gui_font.h"
#include "widget.h"

class ATOM_ENGINE_API ATOM_TopWindow: public ATOM_Widget
{
public:
	ATOM_TopWindow();
	ATOM_TopWindow (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style = ATOM_Widget::Border|ATOM_Widget::TitleBar|ATOM_Widget::CloseButton, int id = ATOM_Widget::AnyId, ATOM_Widget::ShowState showState = ATOM_Widget::ShowNormal);
	virtual ~ATOM_TopWindow();

public:
	virtual bool isTopWindow (void) const;
	virtual ATOM_WidgetType getType (void) const;
	virtual void close (void);
	virtual void moveTo (int x, int y);
	virtual void setFont (ATOM_GUIFont::handle font);
	//virtual ATOM_Point2Di getViewportOffset (void);

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

public:
	void enableWindowTexture (bool enable);
	bool isWindowTextureEnabled (void) const;
	ATOM_Texture *getWindowTexture (void) const;
	void allowClientDragging (bool allow);
	bool isClientDraggingAllowed (void) const;

protected:
	virtual void drawSelf (void);
	virtual void invalidateDisplay (void);
	void calcTextLayout (void);

protected:
	ATOM_AUTOREF(ATOM_Texture) _renderTarget;
	bool _displayInvalid;
	bool _enableWindowTexture;
	bool _clientDragging;
	ATOM_STRING _caption;
	ATOM_Point2Di _textPosition;
	int _align;
	ATOM_Point2Di _offset;
	ATOM_AUTOREF(ATOM_Text) _text;
	bool _textDirty;

public:
	void onClose (ATOM_TopWindowCloseEvent *event);
	void onHitTest (ATOM_WidgetHitTestEvent *event);
	void onResize (ATOM_WidgetResizeEvent *event);
	void onPaint (ATOM_WidgetDrawClientEvent *event);

	ATOM_DECLARE_EVENT_MAP(ATOM_TopWindow, ATOM_Widget)
};

#endif // __ATOM3D_ENGINE_TOPWINDOW_H
/*! @} */
