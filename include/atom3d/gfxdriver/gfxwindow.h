#ifndef __ATOM3D_GFXDRIVER_GFXWINDOW_H
#define __ATOM3D_GFXDRIVER_GFXWINDOW_H

#include "../ATOM_kernel.h"
#include "basedefs.h"

#if ATOM3D_PLATFORM_WIN32
#include <windows.h>
class ATOM_Win32WMEvent: public ATOM_Event
{
public:
	ATOM_Win32WMEvent(void):hWnd(0), msg(0), wParam(0), lParam(0)
	{ 
	}

	ATOM_Win32WMEvent(HWND hWnd_, UINT msg_, WPARAM wParam_, LPARAM lParam_): hWnd(hWnd_), msg(msg_), wParam(wParam_), lParam(lParam_)
	{ 
	}

	HWND hWnd;
	UINT msg;
	WPARAM wParam;
	LPARAM lParam;
	DWORD retval;

	ATOM_DECLARE_EVENT(ATOM_Win32WMEvent)
};
#endif // ATOM3D_PLATFORM_WIN32

class ATOM_GfxDriver;

class ATOM_GfxWindow
{
public:
	virtual ATOM_GfxDriver *getGfxDriver (void) const = 0;
	virtual bool beginRender (void) = 0;
	virtual bool endRender (void) = 0;
	virtual bool swapBuffers (void) = 0;
	virtual void setEventTrigger (ATOM_EventTrigger*) = 0;
	virtual ATOM_EventTrigger * getEventTrigger (void) = 0;
};

#endif // __ATOM3D_GFXDRIVER_GFXWINDOW_H
