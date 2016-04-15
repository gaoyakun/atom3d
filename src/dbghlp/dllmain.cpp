#include "StdAfx.h"
#include "basedefs.h"
#include "logger.h"

#if ATOM3D_SHARED_LIBS

HINSTANCE hInst = NULL;

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) 
{
  switch ( dwReason)
  {
  case DLL_PROCESS_ATTACH:
    {
      hInst = hInstance;
      break;
    }
  case DLL_PROCESS_DETACH:
	{
		ATOM_LOGGER::finalize ();
		break;
	}
  default:
    break;
  }

  return TRUE;
}

#endif
