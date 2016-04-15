// ATOM_FileDlg.cpp : Defines the entry point for the DLL application.
//

#if ATOM3D_SHARED_LIBS

#include <windows.h>

HINSTANCE g_hModule = 0;

BOOL APIENTRY DllMain( HINSTANCE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
  switch ( ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
	g_hModule = hModule;
	break;
  }
  return TRUE;
}

#endif
