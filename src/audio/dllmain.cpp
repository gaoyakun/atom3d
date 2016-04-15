
#include <windows.h>

#if ATOM3D_SHARED_LIBS

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) 
{
  switch ( dwReason)
  {
  case DLL_PROCESS_DETACH:
    break;

  default:
    break;
  }
  
  return TRUE;
}

#endif
