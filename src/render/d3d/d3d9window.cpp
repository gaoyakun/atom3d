#include "stdafx.h"
#include "InitializeD3D9.h"

#include "events.h"
#include "d3d9window.h"
#include "d3d9device.h"
#include "keymapping.h"
#include "renderdevice.h"

extern ATOM_RenderDeviceCaps device_caps;
static KeyMapping _S_key_mapper;
static bool _S_is_main_window = true;

ATOM_D3D9Window::ATOM_D3D9Window() 
{
  ATOM_STACK_TRACE(ATOM_D3D9Window::ATOM_D3D9Window);

  if (_S_is_main_window)
  {
	  _M_main_window = true;
	  _M_is_primary = false;
	  _S_is_main_window = false;
  }
  else
  {
	  _M_main_window = false;
  }
  _M_swapchain = 0;
  _M_depthstencil = 0;
  _M_window_info = 0;
}

ATOM_D3D9Window::~ATOM_D3D9Window() {
	ATOM_STACK_TRACE(ATOM_D3D9Window::~ATOM_D3D9Window);

	if (_M_device)
	{
		_M_device->removeView (this);
		detachDevice ();
	}
	close();
}

bool ATOM_D3D9Window::initDevice (ATOM_D3D9Device *device, int multisample) 
{
  ATOM_STACK_TRACE(ATOM_D3D9Window::initDevice);

  IDirect3D9 *d3d9 = device->getD3D();
  IDirect3DDevice9 *d3d9device = device->getD3DDevice();

  if ( !CreateD3D9Context(d3d9, d3d9device, (ATOM_D3D9WindowInfo*)_M_window_info, multisample))
  {
    MessageBox ((HWND)getWindowId(), "Initialize D3D9 failed.", "Error", MB_OK|MB_ICONHAND);
    return false;
  }

  RECT rc;
  GetClientRect(_M_window_info->handle, &rc);
  _M_window_hints.accel = _M_window_info->accel;
  _M_window_hints.bounds.first = rc.right - rc.left;
  _M_window_hints.bounds.second = rc.bottom - rc.top;
  _M_window_hints.bpp = _M_window_info->bpp;

  //ATOM_LOGGER::Log("ATOM_D3D9Window: mode=%dx%dx%d\n",
  //            _M_window_hints.bounds.first,
  //            _M_window_hints.bounds.second,
  //            _M_window_hints.bpp);
  //ATOM_LOGGER::Log("ATOM_D3D9Window: bpp=%d\n", _M_window_hints.bpp);
  //ATOM_LOGGER::Log("ATOM_D3D9Window: depth=%d\n", _M_window_info->depth);
  //ATOM_LOGGER::Log("ATOM_D3D9Window: stencil=%d\n", _M_window_info->stencil);

	ATOM_SetDialogParentWindow (_M_window_info->handle);

	return true;
}

bool ATOM_D3D9Window::open(ATOM_RenderDevice *device, const char* title, const ATOM_RenderWindow::Hints* hints, ATOM_RenderWindow::ShowMode showMode, void* windowid, void *parentid, void *iconid, void *instance)
{
  ATOM_STACK_TRACE(ATOM_D3D9Window::open);

  if ( hints->resizable && hints->naked)
  {
    ATOM_LOGGER::error ("[ATOM_D3D9Window::open] Window couldn't be both naked and resizable.\n");
    return false;
  }

  // Close display before open
  close();

  _M_window_info = CreateD3D9Window(hints->bounds.first,
                                  hints->bounds.second,
                                  hints->bpp,
                                  hints->resizable,
                                  hints->naked,
								  hints->fullscreen,
                                  (HWND)windowid,
								  (HWND)parentid,
								  (HICON)iconid,
                                  title,
                                  getObjectName(),
								  instance,
                                  this);
  if ( _M_window_info == 0)
    return false;

  switch (showMode)
  {
  case ATOM_RenderWindow::SHOWMODE_SHOW:
	  ShowWindow(_M_window_info->handle, SW_SHOW);
	  break;
  case ATOM_RenderWindow::SHOWMODE_SHOWMAXIZED:
	  ShowWindow(_M_window_info->handle, SW_SHOWMAXIMIZED);
	  break;
  case ATOM_RenderWindow::SHOWMODE_SHOWMINIMIZED:
	  ShowWindow(_M_window_info->handle, SW_SHOWMINIMIZED);
	  break;
  }

  _M_window_hints = *hints;
  ::GetWindowPlacement (_M_window_info->handle, &_wpm);

  WINDOWINFO wi;
  bool b = ::GetWindowInfo(_M_window_info->handle, &wi);
  if (b)
  {
    _M_saved_winpos[0] = wi.rcClient.left;
    _M_saved_winpos[1] = wi.rcClient.top;
  }

  if (!initDevice ((ATOM_D3D9Device*)device, hints->samples))
  {
	  //::DestroyWindow (_M_window_info->handle);
	  _M_window_info->handle = 0;
	  return false;
  }

  return true;
} 

void ATOM_D3D9Window::close() {
  ATOM_STACK_TRACE(ATOM_D3D9Window::close);

  if (_M_window_info)
  {
    destroy ();
    ATOM_SetDialogParentWindow (0);
  }
}

void ATOM_D3D9Window::swapBuffers() {
//      ::SwapBuffers(_M_window_info->gldc);
}

void ATOM_D3D9Window::destroy (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9Window::destroy);

  detachDevice ();

  if (_M_window_info)
  {
    if ( IsWindow(_M_window_info->handle))
    {
      ::SendMessage(_M_window_info->handle, WMU_RENDERWINDOW_DETACH, (WPARAM) this, 0);

      ATOM_D3D9Device *d3ddevice = (ATOM_D3D9Device*)getDevice();
      if (d3ddevice)
      {
        ((ATOM_D3D9WindowInfo*)_M_window_info)->device = 0;
        ((ATOM_D3D9WindowInfo*)_M_window_info)->d3d9 = 0;
      }

      if ( !_M_window_info->hooked)
      {
		  DestroyD3D9Window ((ATOM_D3D9WindowInfo*)_M_window_info, this);
      }
      else
      {
        SetWindowLongPtr(_M_window_info->handle, GWLP_WNDPROC, (LONG) _M_window_info->oldproc);
      }
    }

    if ( _M_window_info->name)
      free(_M_window_info->name);

    ATOM_DELETE(_M_window_info);
    _M_window_info = 0;
  }
}

void ATOM_D3D9Window::resizeDevice (ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_D3D9Window::resizeDevice);

	if (device->getWindow () == this)
	{
		RECT rc;
		::GetClientRect (_M_window_info->handle, &rc);
		device->resizeDevice (rc.right - rc.left, rc.bottom - rc.top);
	}
	else
	{
		detachDevice ();
	  if (attachDevice (device) && device->getCurrentView() == this)
	  {
		setCurrent ();
	  }
	}
}

bool ATOM_D3D9Window::attachDevice (ATOM_RenderDevice *device)
{
  ATOM_STACK_TRACE(ATOM_D3D9Window::attachDevice);

  if (!device)
  {
    return false;
  }

  ATOM_D3D9WindowInfo *info = (ATOM_D3D9WindowInfo*)getWindowInfo ();
  if (!info || !IsWindow(info->handle))
  {
    return false;
  }

  ATOM_D3D9Device *d3d9Device = (ATOM_D3D9Device*)device;
  IDirect3DDevice9 *d3ddevice = d3d9Device->getD3DDevice ();
  if (!d3ddevice)
  {
    return false;
  }

  if (!_M_is_primary)
  {
	  unsigned client_w, client_h;
	  GetWindowClientRect (info->handle, client_w, client_h);
	  if (client_w == 0 || client_h == 0)
	  {
		return false;
	  }

	  int multisample = _M_window_hints.samples;
	  if (multisample && (!d3d9Device->isMultisampleSupported() || !d3d9Device->isSampleSupported (multisample)))
	  {
		  multisample = 0;
	  }

	  detachDevice ();

	  D3DPRESENT_PARAMETERS d3dpp;
	  memset (&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
	  const D3DPRESENT_PARAMETERS *d3dpp_device = d3d9Device->getD3DPresentParams ();

	  d3dpp.BackBufferWidth = client_w;
	  d3dpp.BackBufferHeight = client_h;
	  d3dpp.BackBufferFormat = d3dpp_device->BackBufferFormat;
	  d3dpp.BackBufferCount = d3dpp_device->BackBufferCount;
	  d3dpp.Windowed = TRUE;
	  d3dpp.MultiSampleType = (D3DMULTISAMPLE_TYPE)multisample;
	  d3dpp.MultiSampleQuality = d3dpp_device->MultiSampleQuality;
	  d3dpp.SwapEffect = d3dpp_device->SwapEffect;
	  d3dpp.hDeviceWindow = getWindowInfo()->handle;
	  d3dpp.EnableAutoDepthStencil = d3dpp_device->EnableAutoDepthStencil;
	  d3dpp.AutoDepthStencilFormat = d3dpp_device->AutoDepthStencilFormat;
	  d3dpp.FullScreen_RefreshRateInHz = d3dpp_device->FullScreen_RefreshRateInHz;
	  d3dpp.PresentationInterval = d3dpp_device->PresentationInterval;

	  HRESULT hr = d3ddevice->CreateAdditionalSwapChain (&d3dpp, &_M_swapchain);
	  if (FAILED(hr))
	  {
		return false;
	  }

	  hr = d3ddevice->CreateDepthStencilSurface(client_w, client_h, d3dpp.AutoDepthStencilFormat, d3dpp.MultiSampleType, 0, TRUE, &_M_depthstencil, NULL);
	  if (FAILED(hr))
	  {
		return false;
	  }

	  ATOM_D3D9WindowInfo* d3dwindowinfo = d3d9Device->getD3DWindowInfo();

	  info->accel = d3dwindowinfo->accel;
	  info->adapter = d3dwindowinfo->adapter;
	  info->d3d9 = d3dwindowinfo->d3d9;
	  info->devtype = d3dwindowinfo->devtype;
	  info->device = d3dwindowinfo->device;
	  info->depth = d3dwindowinfo->depth;
	  info->stencil = d3dwindowinfo->stencil;
	  info->presentParams = d3dpp;
	  memcpy (info->multisample, d3dwindowinfo->multisample, sizeof(info->multisample));
	  info->samples = multisample;
  }

  _M_device = device;

  ::SendMessage (info->handle, WMU_RENDERWINDOW_DEVICE_ATTACHED, (WPARAM)d3ddevice, (LPARAM)&info->presentParams);

  return true;
}

IDirect3DSurface9 *ATOM_D3D9Window::getBackBuffer (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9Window::getBackBuffer);

  if (_M_is_primary)
  {
	  ATOM_D3D9Device *d3d9Device = (ATOM_D3D9Device*)_M_device;
	  IDirect3DDevice9 *d3ddevice = d3d9Device->getD3DDevice ();
	  IDirect3DSurface9 *rt = 0;
	  if (FAILED(d3ddevice->GetRenderTarget (0, &rt)))
	  {
		  return 0;
	  }
	  return rt;
  }
  else if (_M_swapchain)
  {
    IDirect3DSurface9 *surface;
    HRESULT hr = _M_swapchain->GetBackBuffer (0, D3DBACKBUFFER_TYPE_MONO, &surface);
    if (SUCCEEDED(hr))
    {
      return surface;
    }
  }
  return 0;
}

IDirect3DSurface9 *ATOM_D3D9Window::getDepthStencilBuffer (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9Window::getDepthStencilBuffer);

  if (_M_is_primary)
  {
	  ATOM_D3D9Device *d3d9Device = (ATOM_D3D9Device*)_M_device;
	  IDirect3DDevice9 *d3ddevice = d3d9Device->getD3DDevice ();
	  IDirect3DSurface9 *ds = 0;
	  if (FAILED(d3ddevice->GetDepthStencilSurface (&ds)))
	  {
		  return 0;
	  }
	  return ds;
  }
  else if (_M_depthstencil)
  {
    _M_depthstencil->AddRef();
	return _M_depthstencil;
  }
  return 0;
}

void ATOM_D3D9Window::detachDevice (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9Window::detachDevice);

  if (_M_swapchain)
  {
    _M_swapchain->Release ();
    _M_swapchain = 0;
  }

  if (_M_depthstencil)
  {
    _M_depthstencil->Release ();
    _M_depthstencil = 0;
  }

  ATOM_D3D9WindowInfo* wi = (ATOM_D3D9WindowInfo*)_M_window_info;
  if (wi)
  {
    wi->d3d9 = 0;
    wi->device = 0;
  }

  _M_device = 0;

  if (wi)
  {
	  ::SendMessage (wi->handle, WMU_RENDERWINDOW_DEVICE_DETACHED, 0, 0);
  }
}

bool ATOM_D3D9Window::setCurrent (void)
{
  ATOM_STACK_TRACE(ATOM_D3D9Window::setCurrent);

  if (_M_swapchain && _M_device && _M_depthstencil)
  {
    ATOM_D3D9Device *device = (ATOM_D3D9Device*)_M_device;
    IDirect3DDevice9 *d3ddevice = device ? device->getD3DDevice() : 0;
    if (d3ddevice)
    {
      IDirect3DSurface9 *surface;
      HRESULT hr = _M_swapchain->GetBackBuffer (0, D3DBACKBUFFER_TYPE_MONO, &surface);
      if (FAILED(hr))
      {
        return false;
      }
      hr = d3ddevice->SetRenderTarget (0, surface);
      surface->Release ();
      if (FAILED(hr))
      {
        return false;
      }
      hr = d3ddevice->SetDepthStencilSurface (_M_depthstencil);
      if (FAILED(hr))
      {
        return false;
      }
      return true;
    }
  }

  return false;
}

void ATOM_D3D9Window::presentImpl ()
{
	ATOM_STACK_TRACE(ATOM_D3D9Window::presentImpl);

	if (_M_is_primary)
	{
		ATOM_D3D9Device *device = (ATOM_D3D9Device*)_M_device;
		IDirect3DDevice9 *d3ddevice = device ? device->getD3DDevice() : 0;
		if (d3ddevice)
		{
			d3ddevice->Present (NULL, NULL, NULL, NULL);
		}
		else if (_M_swapchain)
		{
			_M_swapchain->Present (NULL, NULL, NULL, NULL, 0);
		}
	}
}

bool ATOM_D3D9Window::isPrimary (void) const
{
	return _M_is_primary;
}

bool ATOM_D3D9Window::changeMode (int width , int height, bool resizable, bool naked, bool fullscreen)
{
	if (!_M_window_info || !_M_window_info->handle)
	{
		return false;
	}

	if (fullscreen)
	{
		if (!changeDisplayMode (width, height))
		{
			return false;
		}
		::ShowWindow (_M_window_info->handle, SW_HIDE);
		::GetWindowPlacement (_M_window_info->handle, &_wpm);
		::SetWindowLongA (_M_window_info->handle, GWL_STYLE, WS_POPUP|WS_SYSMENU);
		::SetWindowPos (_M_window_info->handle, 0, 0, 0, width, height, SWP_SHOWWINDOW);
		::ShowWindow (_M_window_info->handle, SW_SHOW);
		_M_window_info->fullscreen = true;
		_M_window_hints.bounds.first = width;
		_M_window_hints.bounds.second = height;
		_M_window_hints.fullscreen = true;
	}
	else
	{
		if (_M_window_info->fullscreen)
		{
			int WindowLeft, WindowTop, WindowWidth, WindowHeight, WindowStyle, WindowExStyle;

			::ShowWindow (_M_window_info->handle, SW_HIDE);
			if (!changeDisplayMode (0, 0))
			{
				::ShowWindow (_M_window_info->handle, SW_SHOW);
				return false;
			}


			WindowExStyle = 0;
			WindowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			if (naked)
			{
				WindowStyle |= WS_POPUP;
				WindowStyle |= WS_SYSMENU;
			}
			else
			{
				WindowStyle |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX; 
				if (resizable)
				{
					WindowStyle |= WS_THICKFRAME | WS_MAXIMIZEBOX;
				}
			}
			::SetWindowLongA (_M_window_info->handle, GWL_STYLE, WindowStyle);
			::SetWindowPlacement (_M_window_info->handle, &_wpm);
			_M_window_info->fullscreen = false;
			_M_window_hints.fullscreen = false;

			unsigned w, h;
			::GetWindowClientRect (_M_window_info->handle, w, h);
			_M_window_hints.bounds.first = w;
			_M_window_hints.bounds.second = h;

			if (w != width || h != height || resizable != _M_window_hints.resizable || naked != _M_window_hints.naked)
			{
				RECT rc;
				rc.left = 0;
				rc.top = 0;
				rc.right = width;
				rc.bottom = height;

				AdjustWindowRect(&rc, WindowStyle, FALSE);
				WindowWidth = rc.right - rc.left;
				WindowHeight = rc.bottom - rc.top;

				DEVMODE desktopMode;
				EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &desktopMode);
				if ( WindowWidth > desktopMode.dmPelsWidth || WindowHeight > desktopMode.dmPelsHeight)
				{
					::ShowWindow (_M_window_info->handle, SW_SHOW);
					return false;
				}

				WindowLeft = (desktopMode.dmPelsWidth - WindowWidth) / 2;
				WindowTop = (desktopMode.dmPelsHeight - WindowHeight) / 2;
				::SetWindowPos (_M_window_info->handle, 0, WindowLeft, WindowTop, WindowWidth, WindowHeight, SWP_NOZORDER);
				::GetWindowPlacement (_M_window_info->handle, &_wpm);
				_M_window_hints.bounds.first = WindowWidth;
				_M_window_hints.bounds.second = WindowHeight;
			}

			::ShowWindow (_M_window_info->handle, SW_SHOW);
			_M_window_hints.naked = naked;
			_M_window_hints.resizable = resizable;
		}
	}

	return true;
}

bool ATOM_D3D9Window::toggleFullScreen (void)
{
	RECT rc;
	::GetClientRect (_M_window_info->handle, &rc);
	return changeMode (rc.right-rc.left, rc.bottom-rc.top, _M_window_hints.resizable, _M_window_hints.naked, !_M_window_hints.fullscreen);
}

bool ATOM_D3D9Window::changeDisplayMode (int width, int height)
{
	if (width == 0 && height == 0)
	{
		return ::ChangeDisplaySettingsA (NULL, NULL) == DISP_CHANGE_SUCCESSFUL;
	}
	else
	{
		DEVMODE desktopMode;
		::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &desktopMode);

		if (desktopMode.dmPelsWidth != width || desktopMode.dmPelsHeight != height || desktopMode.dmBitsPerPel < 24)
		{
			DEVMODE mode;
			DEVMODE sel;
			int i = 0;
			memset(&sel, 0, sizeof(sel));

			while ( ::EnumDisplaySettings(0, i++, &mode))
			{
				if ( mode.dmPelsWidth == width && mode.dmPelsHeight == height)
				{
					if (mode.dmBitsPerPel < 24)
					{
						continue;
					}

					if (mode.dmDisplayFrequency >= sel.dmDisplayFrequency)
					{
						sel = mode;
					}

					if (mode.dmBitsPerPel > sel.dmBitsPerPel)
					{
						sel = mode;
					}
				}
			}

			if (sel.dmPelsWidth == 0)
			{
				return false;
			}

			sel.dmSize = sizeof(sel);
			sel.dmDriverExtra = 0;
			sel.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

			if ( DISP_CHANGE_SUCCESSFUL != ChangeDisplaySettings(&sel, CDS_FULLSCREEN | CDS_SET_PRIMARY))
			{
				sel.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

				if ( DISP_CHANGE_SUCCESSFUL != ChangeDisplaySettingsA(&mode, CDS_FULLSCREEN | CDS_SET_PRIMARY))
				{
					return false;
				}
			}
		}

		return true;
	}
}

