/**	\file desktop.h
 *	×ÀÃæÀà.
 *
 *	\author ¸ßÑÅÀ¤
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_DESKTOP_H
#define __ATOM3D_ENGINE_DESKTOP_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "topwindow.h"
#include "imagelist.h"

class ATOM_ENGINE_API ATOM_Desktop: public ATOM_TopWindow
{
public:
	ATOM_Desktop();
	ATOM_Desktop (ATOM_GUIRenderer *renderer);
	virtual ~ATOM_Desktop (void);

public:
	bool clearBackground (void) const;
	void setClearBackground (bool b);

public:
	virtual ATOM_WidgetType getType (void) const;
	virtual void enableWindowTexture (bool enable);

private:
	ATOM_AUTOPTR(ATOM_GUIImageList) _defaultImageList;
	bool _clearBackground;

public:
	void onPaint (ATOM_WidgetDrawClientEvent *event);

	ATOM_DECLARE_EVENT_MAP(ATOM_Desktop, ATOM_TopWindow)
};

#endif // __ATOM3D_ENGINE_DESKTOP_H
/*! @} */
