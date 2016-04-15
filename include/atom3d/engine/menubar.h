/**	\file menubar.h
 *	²Ëµ¥Àà.
 *
 *	\author ¸ßÑÅÀ¤
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_MENUBAR_H
#define __ATOM3D_ENGINE_MENUBAR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "menu.h"

class ATOM_ENGINE_API ATOM_MenuBar: public ATOM_PopupMenu
{
public:
	ATOM_MenuBar();
	ATOM_MenuBar (ATOM_GUIRenderer *renderer);
	virtual ~ATOM_MenuBar (void);

protected:
	virtual void calcLayout (const ATOM_Rect2Di &rc);
	virtual ATOM_Point2Di calcSubMenuPosition (unsigned item) const;
	virtual void collapse (void);

public:
	void onPaint (ATOM_WidgetDrawClientEvent *event);

protected:
    ATOM_DECLARE_EVENT_MAP(ATOM_MenuBar, ATOM_PopupMenu)
};


#endif // __ATOM3D_ENGINE_MENUBAR_H
/*! @} */
