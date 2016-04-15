#ifndef __ATOM_GLRENDER_ID3D9WINDOW_H
#define __ATOM_GLRENDER_ID3D9WINDOW_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "InitializeD3D9.h"
#include "basedefs.h"
#include "renderwindow.h"

class ATOM_D3D9Device;
class ATOM_D3D9Window : public ATOM_RenderWindow
{
	ATOM_CLASS(render, ATOM_D3D9Window, ATOM_D3D9Window);
	friend class ATOM_D3D9Device;

public:
	ATOM_D3D9Window();
	virtual ~ATOM_D3D9Window();
	virtual bool open(ATOM_RenderDevice *device, const char* title, const Hints* hints, ATOM_RenderWindow::ShowMode showMode, void* windowid, void *parentid, void *iconid, void *instance);
	virtual bool changeMode (int width , int height, bool resizable, bool naked, bool fullscreen);
	virtual bool toggleFullScreen (void);
	virtual bool attachDevice (ATOM_RenderDevice *device);
	virtual void detachDevice (void);
	virtual void close();
	virtual void swapBuffers();
	virtual void resizeDevice(ATOM_RenderDevice *device);
	virtual bool setCurrent (void);
	virtual bool isPrimary (void) const;

public:
	void destroy (void);

protected:
	virtual void presentImpl (void);

protected:
	bool initDevice (ATOM_D3D9Device *device, int multisample);
	IDirect3DSurface9 *getBackBuffer (void);
	IDirect3DSurface9 *getDepthStencilBuffer (void);
	bool changeDisplayMode (int width, int height);

private:
	IDirect3DSwapChain9 *_M_swapchain;
	IDirect3DSurface9 *_M_depthstencil;
	bool _M_main_window;
	bool _M_is_primary;
	WINDOWPLACEMENT _wpm;
};

#endif // __ATOM_GLRENDER_IGLWINDOW_H
