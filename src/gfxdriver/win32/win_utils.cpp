#include <ATOM_dbghlp.h>
#include "win_utils.h"

struct Win32_WindowInfo
{
	HWND hWnd;
	HMENU hMenu;
	WINDOWPLACEMENT wpm;
	WNDPROC oldWndProc;
	BOOL fullScreen;
	BOOL topmostWhileWindowed;
	DWORD styleWindowed;
	short keyStates[256];
	short mouseButtonStates[5];
};

static HINSTANCE g_hInstance = (HINSTANCE)::GetModuleHandleA (NULL);
static LRESULT CALLBACK g_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static Win32_DisplayMode g_desktopDisplayMode;
static const char *ATOM3DWClass = "ATOM3D_WCLASS";

bool Win32_Initialize (void)
{
	HINSTANCE hUser32 = ::LoadLibraryW( L"user32.dll" );
    if( hUser32 )
    {
        typedef BOOL ( WINAPI* LPSetProcessDPIAware )( void );
        LPSetProcessDPIAware pSetProcessDPIAware = ( LPSetProcessDPIAware )GetProcAddress( hUser32,
                                                                                           "SetProcessDPIAware" );
        if( pSetProcessDPIAware )
        {
            pSetProcessDPIAware();
        }
		::FreeLibrary( hUser32 );
    }

    WNDCLASSA wndClass;
    wndClass.style = CS_DBLCLKS;
    wndClass.lpfnWndProc = g_WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = g_hInstance;
	wndClass.hIcon = ::LoadIconA (NULL, IDI_WINLOGO);
	wndClass.hCursor = ::LoadCursor( NULL, IDC_ARROW );
    wndClass.hbrBackground = ( HBRUSH )GetStockObject( BLACK_BRUSH );
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = ATOM3DWClass;

	if( !::RegisterClassA( &wndClass ) )
    {
		DWORD dwError = ::GetLastError();
        if( dwError != ERROR_CLASS_ALREADY_EXISTS )
            return false;
    }

	DEVMODE desktopMode;
	::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &desktopMode);
	g_desktopDisplayMode.width = desktopMode.dmPelsWidth;
	g_desktopDisplayMode.height = desktopMode.dmPelsHeight;
	g_desktopDisplayMode.bpp = desktopMode.dmBitsPerPel;
	g_desktopDisplayMode.freq = desktopMode.dmDisplayFrequency;

	return true;
}

bool Win32_ChangeDisplayMode (const Win32_DisplayMode *displayMode)
{
	if (!displayMode)
	{
		return ::ChangeDisplaySettingsA (NULL, NULL) == DISP_CHANGE_SUCCESSFUL;
	}

	if (displayMode->width && displayMode->height)
	{
		if (displayMode->width != g_desktopDisplayMode.width || displayMode->height != g_desktopDisplayMode.height || displayMode->bpp != g_desktopDisplayMode.bpp)
		{
			DEVMODE mode;
			DEVMODE sel;
			int i = 0;
			memset(&sel, 0, sizeof(sel));

			while ( ::EnumDisplaySettings(0, i++, &mode))
			{
				if ( mode.dmPelsWidth == displayMode->width && mode.dmPelsHeight == displayMode->height)
				{
					if (mode.dmBitsPerPel < displayMode->bpp)
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

	return false;
}

WindowHandle Win32_CreateWindow (const WindowCreationParams *params)
{
	if (!params)
	{
		return false;
	}

	if (g_hInstance == NULL)
	{
		g_hInstance = (HINSTANCE)::GetModuleHandle(NULL);
	}

	HICON hIcon = params->hIcon;
	if (hIcon == NULL)
	{
		wchar_t szExePath[MAX_PATH];
		::GetModuleFileNameW (NULL, szExePath, MAX_PATH);
		hIcon = ::ExtractIconW (g_hInstance, szExePath, 0);
		if (!hIcon)
		{
			hIcon = ::LoadIcon(0, IDI_WINLOGO);
		}
	}

	static const int defaultWidth = 640;
	static const int defaultHeight = 480;

	int w = params->width;
	int h = params->height;
	int x = params->left;
	int y = params->top;

	if (w == 0)
	{
		w = defaultWidth;
	}

	if (h == 0)
	{
		h = defaultHeight;
	}

	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = w;
	rc.bottom = h;

    DWORD WindowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	if (params->naked)
    {
      WindowStyle |= WS_POPUP;
    }
    else
    {
      WindowStyle |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX; 

      if (params->sizable)
      {
        WindowStyle |= WS_THICKFRAME | WS_MAXIMIZEBOX;
      }
    }
	::AdjustWindowRect (&rc, WindowStyle, (params->hMenu != NULL)?TRUE:FALSE);
	w = rc.right - rc.left;
	h = rc.bottom - rc.top;

	if (x == 0)
	{
		x = (g_desktopDisplayMode.width - w) / 2;
	}

	if (y == 0)
	{
		y = (g_desktopDisplayMode.height - h) / 2;
	}

	const char *caption = params->title ? params->title : "";

	Win32_WindowInfo *info = ATOM_NEW(Win32_WindowInfo);
	memset (info, 0, sizeof(Win32_WindowInfo));

	info->hWnd = ::CreateWindowA (ATOM3DWClass, caption, WindowStyle, x, y, w, h, 0, params->hMenu, g_hInstance, info);
	if (info->hWnd == NULL)
	{
		ATOM_DELETE(info);
		return false;
	}

	info->hMenu = params->hMenu;
	info->oldWndProc = 0;
	info->styleWindowed = WindowStyle;

	::SendMessageA (info->hWnd, WM_SETICON, TRUE, (LPARAM)hIcon);
	::SendMessageA (info->hWnd, WM_SETICON, FALSE, (LPARAM)hIcon);

	return info;
}

WindowHandle Win32_AttachWindow (HWND existWindow)
{
	if (!::IsWindow (existWindow))
	{
		return false;
	}

	Win32_WindowInfo *info = ATOM_NEW(Win32_WindowInfo);
	memset (info, 0, sizeof(Win32_WindowInfo));
	info->hMenu = ::GetMenu (existWindow);
	info->hWnd = existWindow;
	info->oldWndProc = (WNDPROC)::SetWindowLongPtrA (existWindow, GWLP_WNDPROC, (LONG_PTR)g_WndProc);
	info->styleWindowed = (DWORD)::GetWindowLongA (existWindow, GWL_STYLE);
	info->styleWindowed &= ~WS_MAXIMIZE & ~WS_MINIMIZE;
	info->topmostWhileWindowed = ((::GetWindowLongA (existWindow, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0);
	::SetWindowLongPtrA (existWindow, GWLP_USERDATA, (LONG_PTR)info);

	return info;
}

bool Win32_SetWindowFullscreen (WindowHandle handle, const Win32_DisplayMode *displayMode)
{
	Win32_WindowInfo *info = (Win32_WindowInfo*)handle;
	if (!handle || !displayMode)
	{
		return false;
	}

	if (!info->fullScreen)
	{
		memset (&info->wpm, 0, sizeof(WINDOWPLACEMENT));
		info->wpm.length = sizeof(WINDOWPLACEMENT);
		::GetWindowPlacement (info->hWnd, &info->wpm);
		info->topmostWhileWindowed = ((::GetWindowLongA (info->hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0);
		info->styleWindowed = ::GetWindowLong (info->hWnd, GWL_STYLE);
		info->styleWindowed &= ~WS_MAXIMIZE & ~WS_MINIMIZE;
		if (info->wpm.showCmd != SW_HIDE)
		{
			::ShowWindow (info->hWnd, SW_HIDE);
		}
		if (!Win32_ChangeDisplayMode (displayMode))
		{
			::SetWindowPlacement (info->hWnd, &info->wpm);
			return false;
		}
		::SetWindowLongA (info->hWnd, GWL_STYLE, WS_POPUP|WS_SYSMENU);
		::SetMenu (info->hWnd, NULL);
		::SetWindowPos (info->hWnd, HWND_TOPMOST, 0, 0, displayMode->width, displayMode->height, 0);
		if (info->wpm.showCmd != SW_HIDE)
		{
			::ShowWindow (info->hWnd, SW_SHOW);
		}
		info->fullScreen = TRUE;
	}

	return true;
}

bool Win32_SetWindowWindowed (WindowHandle handle)
{
	Win32_WindowInfo *info = (Win32_WindowInfo*)handle;
	if (!handle)
	{
		return false;
	}

	if (info->fullScreen)
	{
		::ShowWindow (info->hWnd, SW_HIDE);
		Win32_ChangeDisplayMode (0);
		::SetWindowLongA (info->hWnd, GWL_EXSTYLE, info->topmostWhileWindowed?WS_EX_TOPMOST:0);
		::SetWindowLongA (info->hWnd, GWL_STYLE, info->styleWindowed);
		::SetMenu (info->hWnd, info->hMenu);
		::SetWindowPlacement (info->hWnd, &info->wpm);
		info->fullScreen = FALSE;
	}

	return true;
}

void Win32_DestroyWindow (WindowHandle handle)
{
	if (handle)
	{
		Win32_SetWindowWindowed (handle);
		Win32_WindowInfo *info = (Win32_WindowInfo*)handle;
		::DestroyWindow (info->hWnd);
		ATOM_DELETE(info);
	}
}

void Win32_DetachWindow (WindowHandle handle)
{
	if (handle)
	{
		Win32_SetWindowWindowed (handle);
		Win32_WindowInfo *info = (Win32_WindowInfo*)handle;
		if (info->oldWndProc)
		{
			::SetWindowLongPtrA (info->hWnd, GWLP_WNDPROC, (LONG_PTR)info->oldWndProc);
		}
		ATOM_DELETE(info);
	}
}

void Win32_Finalize (void)
{
	::UnregisterClassA (ATOM3DWClass, g_hInstance);
}

LRESULT CALLBACK g_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

