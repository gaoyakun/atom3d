#ifndef __ATOM_GLRENDER_INITIALIZED3D9_H
#define __ATOM_GLRENDER_INITIALIZED3D9_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <windows.h>
#include <d3d9.h>

class ATOM_D3D9Window;

struct ATOM_D3D9WindowInfo: public ATOM_WindowInfo
{
  IDirect3D9 *d3d9;
  IDirect3DDevice9 *device;
  UINT adapter;
  D3DDEVTYPE devtype;
  D3DPRESENT_PARAMETERS presentParams;
};

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
                               ATOM_D3D9Window* win);
void DestroyD3D9Window(ATOM_D3D9WindowInfo* info, ATOM_D3D9Window* win);
bool CreateD3D9Context(IDirect3D9 *d3d, IDirect3DDevice9 *device, ATOM_D3D9WindowInfo* info, int multisample);
void FindBufferFormats (IDirect3D9 *d3d9, UINT adapter, D3DDEVTYPE devtype, D3DFORMAT &dispFormat, D3DFORMAT &backFormat, D3DFORMAT &zbufFormat, int color, int *depth, int *stencil);
bool CheckDepthFormat (IDirect3D9 *d3d9, UINT adapter, D3DDEVTYPE devtype, D3DFORMAT dispFormat, D3DFORMAT backFormat, D3DFORMAT zbufFormat);
void RegisterWindowClass(const char* classname, HICON icon);
void GetWindowClientRect (HWND hWnd, unsigned &w, unsigned &h);

#endif // __ATOM_GLRENDER_INITIALIZED3D9_H

