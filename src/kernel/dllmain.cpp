#include "StdAfx.h"

#if ATOM3D_SHARED_LIBS

//-----------------------------------------------------
// Function name   : DllMain
// Return type     : BOOL WINAPI 
// Argument        : HINSTANCE hInstance
// Argument        : DWORD dwReason
// Argument        : LPVOID lpReserved
// Description     : 
//-----------------------------------------------------
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) 
{

  switch ( dwReason)
  {
  case DLL_PROCESS_ATTACH:
    {
#ifdef _CRTDBG_MAP_ALLOC
      int flag = _CrtSetDbgFlag(0);
      _CrtSetDbgFlag(flag | _CRTDBG_LEAK_CHECK_DF);
#endif
    }
    break;

  case DLL_PROCESS_DETACH:
    break;

  default:
    break;
  }
  return TRUE;
}

# endif

