#ifndef __ATOM3D_GFXDRIVER_WIN32_WIN_UTILS_H
#define __ATOM3D_GFXDRIVER_WIN32_WIN_UTILS_H

#include <windows.h>

typedef struct Win32_WindowInfo *WindowHandle;

struct Win32_DisplayMode
{
	int width;
	int height;
	int bpp;
	int freq;
};

struct WindowCreationParams
{
	int width;
	int height;
	int left;
	int top;
	bool sizable;
	bool naked;
	const char *title;
	HICON hIcon;
	HMENU hMenu;
	Win32_DisplayMode *fullScreenMode;
};

bool Win32_Initialize (void);
bool Win32_ChangeDisplayMode (const Win32_DisplayMode *displayMode);
WindowHandle Win32_CreateWindow (const WindowCreationParams *params);
WindowHandle Win32_AttachWindow (HWND existWindow);
void Win32_DetachWindow (WindowHandle handle);
bool Win32_SetWindowFullscreen (WindowHandle handle, const Win32_DisplayMode *displayMode);
bool Win32_SetWindowWindowed (WindowHandle handle);
void Win32_DestroyWindow (WindowHandle handle);
void Win32_Finalize (void);

#endif // __ATOM3D_GFXDRIVER_WIN32_WIN_UTILS_H
