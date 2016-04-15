/**	\file animatedtexturechannel.h
 *	渲染窗口类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup render
 *	@{
 */

#ifndef __ATOM_RENDER_IRENDERWINDOW_H
#define __ATOM_RENDER_IRENDERWINDOW_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <string.h>
#include <string>
#include "../ATOM_kernel.h"
#include "basedefs.h"
//#include "commontypes.h"
#include "keysymbols.h"

#define WMU_RENDERWINDOW_ATTACH				WM_APP + 400
#define WMU_RENDERWINDOW_DETACH				WM_APP + 401
#define WMU_RENDERWINDOW_DEVICE_ATTACHED	WM_APP + 402
#define WMU_RENDERWINDOW_DEVICE_DETACHED	WM_APP + 403

struct ATOM_WindowInfo
{
	HWND handle;
	WNDPROC wndproc;
	WNDPROC oldproc;
	HMENU oldmenu;
	DWORD oldstyle;
	DWORD oldstyleex;
	HINSTANCE hInstance;
	WINDOWPLACEMENT oldplacement;
	bool accel;
	bool hooked;
	bool fullscreen;
	int bpp;
	int depth;
	int stencil;
	int samples;
	char* name;
	bool multisample[16];
};

struct ATOM_RenderView;
class ATOM_RenderDevice;

class ATOM_RenderWindow : public ATOM_Object
{
	friend class ATOM_RenderDevice;

public:
	enum
	{
		MAX_TITLE_LENGTH = 256
	};

	enum ShowMode
	{
		SHOWMODE_HIDE = 0,
		SHOWMODE_SHOW,
		SHOWMODE_SHOWMAXIZED,
		SHOWMODE_SHOWMINIMIZED
	};

	struct Hints
	{
		std::pair<int, int> bounds;
		bool accel;
		bool resizable;
		bool naked;
		bool fullscreen;
		char bpp;
		int samples;
	};

public:
	ATOM_RenderWindow (void);
	virtual ~ATOM_RenderWindow (void);

public:
	virtual bool open(ATOM_RenderDevice *device, const char* title, const Hints* hints, ShowMode show, void* windowid = 0, void *parentid = 0, void *iconid = 0, void *instance = 0) = 0;
	virtual bool changeMode (int width , int height, bool resizable, bool naked, bool fullscreen) = 0;
	virtual bool toggleFullScreen (void) = 0;
	virtual bool attachDevice (ATOM_RenderDevice *device) = 0;
	virtual void detachDevice (void) = 0;
	virtual bool setCurrent (void) = 0;
	virtual void present (void);
	virtual void close() = 0;
	virtual void swapBuffers() = 0;
	virtual bool isPrimary (void) const = 0;
	virtual void getMousePosition(int* px, int* py) const;
	virtual void setMousePosition(int x, int y);
	virtual bool keyPressed(ATOM_Key key) const;
	virtual const char *getKeyDesc (ATOM_Key key) const;
	virtual void showMouseCursor(bool show);
	virtual void *getWindowId() const;
	virtual void setCapture();
	virtual void releaseCapture();
	virtual bool isActive() const;
	virtual void resizeDevice(ATOM_RenderDevice *device);
	virtual bool isMultisampleSupported(void);
	virtual bool isSampleSupported(unsigned sample);
	virtual int getMultiSampleType (void) const;
	virtual LRESULT processWindowMessage (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	virtual ATOM_WindowInfo* getWindowInfo() const;
	virtual void getWindowBounds (unsigned *w, unsigned *h, unsigned *client_w, unsigned *client_h);
	virtual unsigned getWindowWidth (void) const;
	virtual unsigned getWindowHeight (void) const;

protected:
	virtual void presentImpl (void) = 0;

public:
	static LONG WINAPI windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    virtual const Hints& getWindowHints() const;
    virtual Hints& getWindowHints (void);
    virtual ATOM_RenderDevice* getDevice (void) const;
    virtual ATOM_RenderView *getRenderView (void) const;
    virtual ATOM_Key translateKey2TKey (int from);

private:
	void checkResizeDevice (ATOM_RenderDevice *device);
	void resetKeyStates (void);

protected:
	Hints _M_window_hints;
	ATOM_RenderDevice* _M_device;
	int _M_saved_position_x;
	int _M_saved_position_y;
	int _M_saved_winpos[2];
	char _M_saved_char[3];
	int _M_key_state[KEY_COUNT];
	unsigned _M_key_mod;
	bool _M_active;
	bool _M_insizemove;
	bool _M_fullscreen;
	ATOM_WindowInfo *_M_window_info;
	ATOM_RenderView *_M_view;
};

#endif // __ATOM_GLRENDER_IRENDERWINDOW_H
/*! @} */
