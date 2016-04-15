#include "stdafx.h"
#include <ATOM_kernel.h>
#include <ATOM_net.h>
#include <windowsx.h>
#include "initializeD3D9.h"
#include "renderdevice.h"
#include "keymapping.h"
#include "events.h"
#include "d3d9window.h"
#define ATOM_WCLASS L"atom3d_WCLASS"

#ifdef _MSC_VER
# pragma warning(disable:4312)
# pragma warning(disable:4311)
#endif // _MSC_VER

#define ENABLE_NVPERFHUD
#undef ENABLE_NVPERFHUD

//#define DEBUG_D3D_SHADER

void RegisterWindowClass(HICON icon, HINSTANCE hInstance)
{
  ATOM_STACK_TRACE(RegisterWindowClass);

  WNDCLASSW wc;
  wc.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
  wc.lpfnWndProc = (WNDPROC) ATOM_RenderWindow::windowProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;
  wc.hIcon = icon ? icon : ::LoadIcon(0, IDI_WINLOGO);
  wc.hCursor = LoadCursor(0, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName = 0;
  wc.lpszClassName = ATOM_WCLASS;

  if (0 == RegisterClassW(&wc))
  {
	  DWORD error = ::GetLastError();
	  error = error;
  }
}

void UnregisterWindowClass(HINSTANCE hInstance)
{
	::UnregisterClassW (ATOM_WCLASS, hInstance);
}

void DestroyD3D9Window(ATOM_D3D9WindowInfo* info, ATOM_D3D9Window* win)
{
	if (::IsWindow (info->handle))
	{
		::SetWindowLongPtrA (info->handle, GWLP_WNDPROC, (LONG)::DefWindowProc);
		//::DestroyWindow (info->handle);
	}
	UnregisterWindowClass (info->hInstance);
}

ATOM_D3D9WindowInfo* CreateD3D9Window(int w,
                               int h,
                               int bpp,
                               bool sizable,
                               bool naked,
							   bool fullscreen,
                               HWND id,
							   HWND parentid,
							   HICON icon,
                               const char* title,
                               const char* name,
							   void *instance,
                               ATOM_D3D9Window* win)
{
  ATOM_STACK_TRACE(CreateD3D9Window);

  ATOM_ASSERT (!id || IsWindow(id));
  //ATOM_LOGGER::Log ("[CreateD3DWindow] Trying create %dx%dx D3D window...\n", w, h);

  if (!instance)
  {
	  instance = ::GetModuleHandle (NULL);
  }

  if (fullscreen || parentid)
  {
	  naked = true;
	  sizable = false;
  }

  RegisterWindowClass(icon, (HINSTANCE)instance);

  // Query the screen caps
  HDC ScreenDC = GetDC(0);
  if (!ScreenDC)
  {
    ATOM_LOGGER::error ("[CreateD3DWindow] ERR: Couldn't get screen HDC.\n");
    return 0;
  }

  DEVMODE desktopMode;
  EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &desktopMode);
  int ScreenLeft = 0;
  int ScreenTop = 0;
  int ScreenWidth = desktopMode.dmPelsWidth;
  int ScreenHeight = desktopMode.dmPelsHeight;
  int ScreenBPP = desktopMode.dmBitsPerPel;
  int ScreenFreq = desktopMode.dmDisplayFrequency;
  //ATOM_LOGGER::Log ("[CreateD3DWindow] ATOM_Desktop display mode is %dx%dx%d@%d.\n", ScreenWidth, ScreenHeight, ScreenBPP, ScreenFreq);
  ReleaseDC(0, ScreenDC);

  if (parentid)
  {
	  RECT rc;
	  ::GetClientRect (parentid, &rc);
	  ScreenWidth = rc.right - rc.left;
	  ScreenHeight = rc.bottom - rc.top;
  }
  else if (!fullscreen)
  {
	  RECT rc;
	  if (::SystemParametersInfoA(SPI_GETWORKAREA, 0, &rc, 0))
	  {
		  ScreenLeft = rc.left;
		  ScreenTop = rc.top;
		  ScreenWidth = rc.right - rc.left;
		  ScreenHeight = rc.bottom - rc.top;
	  }
  }

  int WindowLeft, WindowTop, WindowWidth, WindowHeight, WindowStyle, WindowExStyle;
  HWND WindowHandle = id;
  ATOM_D3D9WindowInfo* pWindowInfo = ATOM_NEW(ATOM_D3D9WindowInfo);
  memset(pWindowInfo, 0, sizeof(*pWindowInfo));

  if ( name)
    pWindowInfo->name = strdup(name);

  if ( WindowHandle)
    pWindowInfo->oldproc = ::IsWindowUnicode(WindowHandle) ? (WNDPROC) GetWindowLongPtrW(WindowHandle, GWLP_WNDPROC) : (WNDPROC)GetWindowLongPtrA(WindowHandle, GWLP_WNDPROC);

  if ( ScreenBPP <= 8)
  {
    ATOM_LOGGER::error ("[CreateD3DWindow] Invalid display color depth: %d.\n", ScreenBPP);
    MessageBox (WindowHandle, "Invalid display mode, please set your display mode to 16 or 32.", "", MB_OK|MB_ICONINFORMATION);
    ATOM_DELETE(pWindowInfo);
    return 0;
  }

  // We will create a new window

  if (!WindowHandle)
  {
    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = w;
    rc.bottom = h;

    WindowExStyle = 0;
    WindowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    if (naked)
    {
		WindowStyle |= (parentid ? WS_CHILD : WS_POPUP);
    }
    else
    {
		WindowStyle |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX; 

		if (sizable)
		{
			WindowStyle |= WS_THICKFRAME | WS_MAXIMIZEBOX;
		}
    }

    AdjustWindowRect(&rc, WindowStyle, FALSE);
    WindowWidth = rc.right - rc.left;
    WindowHeight = rc.bottom - rc.top;
    WindowLeft = ScreenLeft + (ScreenWidth - WindowWidth) / 2;
    WindowTop = ScreenTop + (ScreenHeight - WindowHeight) / 2;

    if ( WindowWidth > ScreenWidth || WindowHeight > ScreenHeight)
    {
      ATOM_LOGGER::error ("[CreateD3DWindow] Invalid display mode: %dx%dx%d.\n", ScreenWidth, ScreenHeight, ScreenBPP);
      MessageBox (WindowHandle, "Screen size too small for creating window, please set higher resolution.", "", MB_OK|MB_ICONHAND);
      ATOM_DELETE(pWindowInfo);
      return 0;
    }

	wchar_t titleBuffer[1024];
	wchar_t *uTitle = L"ATOM3D render window";
	if (title)
	{
		memset (titleBuffer, 0, sizeof(titleBuffer));
		::MultiByteToWideChar (CP_ACP, 0, title, -1, titleBuffer, 1024);
		uTitle = titleBuffer;
	}

    WindowHandle = CreateWindowExW(WindowExStyle,
                                  ATOM_WCLASS,
                                  uTitle,
                                  WindowStyle,
                                  WindowLeft,
                                  WindowTop,
                                  WindowWidth,
                                  WindowHeight,
                                  parentid,
                                  NULL,
                                  (HINSTANCE)instance,
                                  0);


    if ( WindowHandle == 0)
    {
      ATOM_LOGGER::error ("[CreateD3DWindow] Couldn't create window.\n");
      MessageBox (WindowHandle, "Creating window failed", "", MB_OK|MB_ICONHAND);
      ATOM_DELETE(pWindowInfo);
      return 0;
    }
  }
  else
  {
	  if (::IsWindowUnicode (WindowHandle))
	  {
		SetWindowLongPtrW(WindowHandle, GWLP_WNDPROC, (LONG)ATOM_RenderWindow::windowProc);
	  }
	  else
	  {
		SetWindowLongPtrA(WindowHandle, GWLP_WNDPROC, (LONG)ATOM_RenderWindow::windowProc);
	  }
	  SetWindowPos(WindowHandle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW);
  }

  pWindowInfo->handle = WindowHandle;
  pWindowInfo->hooked = (id != 0);
  pWindowInfo->bpp = ScreenBPP;
  pWindowInfo->fullscreen = fullscreen;
  pWindowInfo->hInstance = (HINSTANCE)instance;

  SendMessage(WindowHandle, WMU_RENDERWINDOW_ATTACH, (WPARAM) win, (LPARAM) pWindowInfo);

  return pWindowInfo;
}

bool CheckDepthFormat (IDirect3D9 *d3d9, UINT adapter, D3DDEVTYPE devtype, D3DFORMAT dispFormat, D3DFORMAT backFormat, D3DFORMAT zbufFormat)
{
  ATOM_STACK_TRACE(ATOM_D3D9IndexArray::CheckDepthFormat);

  HRESULT hr = d3d9->CheckDeviceFormat(adapter, devtype, dispFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, zbufFormat);

  if (FAILED(hr))
  {
    return false;
  }

  hr = d3d9->CheckDepthStencilMatch(adapter, devtype, dispFormat, backFormat, zbufFormat);

  return SUCCEEDED(hr);
}

void FindBufferFormats (IDirect3D9 *d3d9, UINT adapter, D3DDEVTYPE devtype, D3DFORMAT &dispFormat, D3DFORMAT &backFormat, D3DFORMAT &zbufFormat, int color, int *depth, int *stencil)
{
  ATOM_STACK_TRACE(ATOM_D3D9IndexArray::FindBufferFormats);

  HRESULT hr;

  static const D3DFORMAT formatTable[][2] = {
    { D3DFMT_X8R8G8B8, D3DFMT_D24S8 },
    { D3DFMT_X8R8G8B8, D3DFMT_D24X4S4 },
    { D3DFMT_X8R8G8B8, D3DFMT_D24X8 },
    { D3DFMT_X8R8G8B8, D3DFMT_D16 },
    { D3DFMT_R5G6B5,   D3DFMT_D16 },
    { D3DFMT_UNKNOWN,  D3DFMT_UNKNOWN }
  };

  static const int colorBits[] = {
    32,
    32,
    32,
    32,
    16,
    0
  };

  static const int depthBits[] = {
    24,
    24,
    24,
    16,
    16,
    0
  };

  static const int stencilBits[] = {
    8,
    4,
    0,
    0,
    0,
    0
  };

  D3DDISPLAYMODE desktopMode;
  hr = d3d9->GetAdapterDisplayMode(adapter, &desktopMode);
  dispFormat = desktopMode.Format;
  switch (dispFormat)
  {
  case D3DFMT_R8G8B8:
  case D3DFMT_A8R8G8B8:
  case D3DFMT_X8R8G8B8:
    backFormat = D3DFMT_X8R8G8B8;
    break;
  default:
    backFormat = dispFormat;
    break;
  }

  if (FAILED(d3d9->CheckDeviceType(adapter, devtype, dispFormat, backFormat, FALSE)))
  {
    backFormat = D3DFMT_UNKNOWN;
    zbufFormat = D3DFMT_UNKNOWN;
    return;
  }

  int i = 0;
  for (; formatTable[i][0] != D3DFMT_UNKNOWN; ++i)
  {
    zbufFormat = formatTable[i][1];

    if (CheckDepthFormat (d3d9, adapter, devtype, dispFormat, backFormat, zbufFormat))
    {
      if (depth)
      {
        *depth = depthBits[i];
      }

      if (stencil)
      {
        *stencil = stencilBits[i];
      }

      break;
    }
  }

  if (formatTable[i][0] == D3DFMT_UNKNOWN)
  {
    zbufFormat = D3DFMT_UNKNOWN;
  }
}

void GetWindowClientRect (HWND hWnd, unsigned &w, unsigned &h)
{
  ATOM_STACK_TRACE(ATOM_D3D9IndexArray::GetWindowClientRect);

  if (IsIconic (hWnd))
  {
    WINDOWPLACEMENT wp;
    memset (&wp, 0, sizeof(wp));
    wp.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement (hWnd, &wp);

    if ((wp.flags & WPF_RESTORETOMAXIMIZED) != 0 && wp.showCmd == SW_SHOWMINIMIZED)
    {
      ShowWindow (hWnd, SW_RESTORE);
      RECT rc;
      GetClientRect (hWnd, &rc);
      w = rc.right - rc.left;
      h = rc.bottom - rc.top;
      ShowWindow (hWnd, SW_MINIMIZE);
    }
    else
    {
      RECT rcFrame = { 0 };
      LONG style = ::IsWindowUnicode(hWnd) ? GetWindowLongW (hWnd, GWL_STYLE) : GetWindowLongA (hWnd, GWL_STYLE);
      AdjustWindowRect (&rcFrame, style, GetMenu(hWnd) != NULL);
      LONG frameWidth = rcFrame.right - rcFrame.left;
      LONG frameHeight = rcFrame.bottom - rcFrame.top;
      w = wp.rcNormalPosition.right - wp.rcNormalPosition.left - frameWidth;
      h = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top - frameHeight;
    }
  }
  else
  {
    RECT rc;
    GetClientRect (hWnd, &rc);
    w = rc.right - rc.left;
    h = rc.bottom - rc.top;
  }
}

bool CreateD3D9Context(IDirect3D9 *d3d, IDirect3DDevice9 *device, ATOM_D3D9WindowInfo* info, int multisample)
{
  ATOM_STACK_TRACE(ATOM_D3D9IndexArray::CreateD3D9Context);

  ATOM_VECTOR<ATOM_STRING> args;
  ATOM_GetCommandLine (args);
  const char *d3ddllName = "d3d9.dll";
  const char *d3ddllNameDbg = "d3d9d.dll";
  bool debugD3D = false;
  bool VSync = true;
  char buf[MAX_PATH] = "";
  for (unsigned i = 0; i < args.size(); ++i)
  {
	  if (args[i] == "--debugd3d")
	  {
		  debugD3D = true;
		  break;
	  }
	  else if (args[i] == "--no-vsync")
	  {
		  VSync = false;
	  }
  }
  if (!debugD3D)
  {
	  ATOM_GetEnv ("ATOM3D_ENABLE_D3D_DEBUG", buf, MAX_PATH);
	  if (!stricmp (buf, "yes"))
	  {
		  debugD3D = true;
	  }
  }

  HMODULE hD3DDll = NULL;

  if (debugD3D)
  {
    hD3DDll = ::LoadLibraryA (d3ddllNameDbg);
	if (!hD3DDll)
	{
		int downloadResult;

		buf[0] = '\0';
		unsigned size = ATOM_GetEnv ("ATOM3D_D3D_DEBUG_RUNTIME_URL", buf, MAX_PATH);
		if (size)
		{
			unsigned len = strlen(buf);
			if (len > 0)
			{
				if (buf[len-1] != '/')
				{
					strcat (buf, "/");
				}
			}
			char *p = buf + strlen(buf);

			strcpy (p, d3ddllNameDbg);

			ATOM_DownloadContext *context = ATOM_NewDownload (buf);
			if (context)
			{
				if (ATOM_StartDownloading (context))
				{
					downloadResult = ATOM_WaitForDownloading (context, INFINITE);
				}
				if (downloadResult == ATOM_NET_DOWNLOAD_OK)
				{
					::CopyFileA (ATOM_GetDownloadedFileName (context), d3ddllNameDbg, FALSE);
				}
				ATOM_DestroyDownload (context);
			}

			if (downloadResult == ATOM_NET_DOWNLOAD_OK)
			{
				strcpy (p, "d3dx9d_43.dll");
				context = ATOM_NewDownload (buf);
				if (context)
				{
					if (ATOM_StartDownloading (context))
					{
						downloadResult = ATOM_WaitForDownloading (context, INFINITE);
					}
					if (downloadResult == ATOM_NET_DOWNLOAD_OK)
					{
						::CopyFileA (ATOM_GetDownloadedFileName (context), "d3dx9d_43.dll", FALSE);
					}
					ATOM_DestroyDownload (context);
				}
			}

			hD3DDll = ::LoadLibraryA (d3ddllNameDbg);
		}
	}

	if (hD3DDll)
	{
		ATOM_LOGGER::log ("*****************************************\n");
		ATOM_LOGGER::log ("*    Direct3D Debug runtime selected    *\n");
		ATOM_LOGGER::log ("*****************************************\n");
	}
	else
	{
		ATOM_LOGGER::error ("**** Direct3D Debug runtime load failed! ****\n");
		hD3DDll = ::LoadLibraryA (d3ddllName);
	}
  }
  else
  {
	hD3DDll = ::LoadLibraryA (d3ddllName);
  }

  if (!hD3DDll)
  {
    ATOM_LOGGER::error ("[CreateD3D9Context] d3d9.dll could not be loaded.\n");
    return false;
  }

  typedef IDirect3D9 * (WINAPI *FuncDirect3DCreate9) (UINT);
  FuncDirect3DCreate9 fnDirect3DCreate9 = (FuncDirect3DCreate9)::GetProcAddress(hD3DDll, "Direct3DCreate9");
  if (!fnDirect3DCreate9)
  {
    ATOM_LOGGER::error ("[CreateD3D9Context] not found Direct3DCreate9 function entry.\n");
    return false;
  }

  info->d3d9 = d3d ? d3d : fnDirect3DCreate9 (D3D_SDK_VERSION);

  if (!info->d3d9)
  {
    ATOM_LOGGER::error ("[CreateD3D9Context] Direct3D 9 not installed.\n");
    MessageBox (info->handle, "DirectX 9.0 must be installed before running the game in D3D mode.", "", MB_OK|MB_ICONHAND);
    return false;
  }

  D3DFORMAT dispFormat;
  D3DFORMAT backFormat;
  D3DFORMAT zbufFormat;
  
  UINT adapter = D3DADAPTER_DEFAULT;
  D3DDEVTYPE devtype = D3DDEVTYPE_HAL;

#if defined(ENABLE_NVPERFHUD)// && !defined(_DEBUG)
  for (UINT adpt = 0; adpt < info->d3d9->GetAdapterCount(); adpt++)
  {
    D3DADAPTER_IDENTIFIER9 Identifier;
    HRESULT hr;
    hr = info->d3d9->GetAdapterIdentifier(adpt, 0, &Identifier);
    if (SUCCEEDED(hr) && strstr(Identifier.Description, "PerfHUD"))
    {
      adapter = adpt;
      devtype = D3DDEVTYPE_REF;
	  printf ("Using NVIDIA PerfHUD device\n");
      break;
    }
  }
#endif

  FindBufferFormats (info->d3d9, adapter, devtype, dispFormat, backFormat, zbufFormat, info->bpp, &info->depth, &info->stencil);

  if (backFormat == D3DFMT_UNKNOWN || zbufFormat == D3DFMT_UNKNOWN)
  {
    ATOM_LOGGER::error ("[CreateD3D9Context] Could not found buffer formats.\n");
    MessageBox (info->handle, "Couldn't find pixel format, D3D9 initialization failed.", "", MB_OK|MB_ICONHAND);
    return false;
  }

  unsigned deviceBehaviorFlags = 0;//D3DCREATE_MULTITHREADED;

#if defined(DEBUG_D3D_SHADER)
  deviceBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
  info->presentParams.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
  devtype = D3DDEVTYPE_REF;
# elif defined(ENABLE_NVPERFHUD)
  deviceBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
  info->presentParams.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
# else
  D3DCAPS9 caps;
  info->d3d9->GetDeviceCaps (adapter, devtype, &caps);
  if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
  {
    if (caps.VertexShaderVersion >= D3DVS_VERSION(2, 0))
    {
      deviceBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
      ATOM_LOGGER::information ("[CreateD3D9Context] Using hardware vertex processing.\n");

      if (caps.DevCaps & D3DDEVCAPS_PUREDEVICE)
      {
        deviceBehaviorFlags |= D3DCREATE_PUREDEVICE;
      }
    }
    else
    {
      deviceBehaviorFlags |= D3DCREATE_MIXED_VERTEXPROCESSING;
      ATOM_LOGGER::information ("[CreateD3D9Context] Using mixed vertex processing.\n");
    }
  }
  else
  {
    deviceBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    ATOM_LOGGER::information ("[CreateD3D9Context] Using software vertex processing.\n");
  }
  info->presentParams.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
#endif

  info->presentParams.BackBufferCount = 1;
  info->presentParams.Windowed = TRUE;
  info->presentParams.PresentationInterval = VSync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

  unsigned clientWidth, clientHeight;
  GetWindowClientRect (info->handle, clientWidth, clientHeight);

  D3DMULTISAMPLE_TYPE multisampleType = (D3DMULTISAMPLE_TYPE)multisample;
  if (multisampleType > D3DMULTISAMPLE_16_SAMPLES)
  {
    if (device)
    {
      ATOM_LOGGER::error ("Invalid multisample type: %d\n", multisampleType);
      return false;
    }
    else
    {
      multisampleType = D3DMULTISAMPLE_NONE;
    }
  }

  //multisampleType = D3DMULTISAMPLE_NONE;

  info->multisample[0] = false;
  info->multisample[1] = false;

  for (unsigned samp = 2; samp < 16; ++samp)
  {
    if (SUCCEEDED(info->d3d9->CheckDeviceMultiSampleType(adapter, devtype, backFormat, TRUE, (D3DMULTISAMPLE_TYPE)samp, NULL)) &&
        SUCCEEDED(info->d3d9->CheckDeviceMultiSampleType(adapter, devtype, zbufFormat, TRUE, (D3DMULTISAMPLE_TYPE)samp, NULL)))
    {
      info->multisample[samp] = true;
    }
    else
    {
      info->multisample[samp] = false;
    }
  }

  if (multisampleType != D3DMULTISAMPLE_NONE && !info->multisample[multisampleType])
  {
	ATOM_LOGGER::information ("%dXFSAA not supported\n", multisampleType);
	multisampleType = D3DMULTISAMPLE_NONE;
  }

  info->presentParams.BackBufferWidth = clientWidth;
  info->presentParams.BackBufferHeight = clientHeight;
  info->presentParams.BackBufferFormat = backFormat;
  info->presentParams.MultiSampleType = multisampleType;
  info->presentParams.MultiSampleQuality = 0;
  info->presentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
  info->presentParams.hDeviceWindow = info->handle;
  info->presentParams.EnableAutoDepthStencil = TRUE;
  info->presentParams.AutoDepthStencilFormat = zbufFormat;
  info->presentParams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

  HRESULT hr;
  if (!device)
  {
	  for (UINT Adapter=0;Adapter<info->d3d9->GetAdapterCount();Adapter++)  
	  {
		  D3DADAPTER_IDENTIFIER9 Identifier;
		  HRESULT res;
		  res = info->d3d9->GetAdapterIdentifier (Adapter, 0, &Identifier);
		  if (strstr (Identifier.Description, "PerfHUD") != 0)
		  {
			  MessageBox (info->handle, "PerfHUD device found", "", MB_OK);
			  adapter = Adapter;
			  devtype = D3DDEVTYPE_REF;
			  break;
		  }
	  }
	  hr = info->d3d9->CreateDevice(adapter, devtype, info->handle, deviceBehaviorFlags, &info->presentParams, &info->device);
  }
  else
  {
	hr = device->Reset (&info->presentParams);
  }

  if (FAILED(hr))
  {
    ATOM_LOGGER::error ("[CreateD3D9Context] Could not create D3D9 device. %x \n", hr);
    MessageBox (info->handle, "Creating D3D9 device failed.", "", MB_OK|MB_ICONHAND);
    return false;
  }

  info->accel = true;
  info->adapter = adapter;
  info->devtype = devtype;
  info->samples = multisampleType;

  switch (backFormat)
  {
  case D3DFMT_R5G6B5:
    info->bpp = 16;
    break;
  case D3DFMT_R8G8B8:
    info->bpp = 24;
    break;
  case D3DFMT_A8R8G8B8:
  case D3DFMT_X8R8G8B8:
    info->bpp = 32;
    break;
  default:
    info->bpp = 0;
    break;
  }

  switch (zbufFormat)
  {
  case D3DFMT_D24S8:
    info->depth = 24;
    info->stencil = 8;
    break;
  case D3DFMT_D24X4S4:
    info->depth = 24;
    info->stencil = 4;
    break;
  case D3DFMT_D24X8:
    info->depth = 24;
    info->stencil = 0;
    break;
  case D3DFMT_D16:
    info->depth = 16;
    info->stencil = 0;
    break;
  default:
    info->depth = 0;
    info->stencil = 0;
    break;
  }

  return true;
}


