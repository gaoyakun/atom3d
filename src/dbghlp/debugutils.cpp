#include "StdAfx.h"
#include "debugutils.h"

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

HMODULE ATOM_LoadDebugHelpDll (void)
{
  HMODULE hDbgHlp = NULL;

  // Dynamically load the Entry-Points for dbghelp.dll:
  // First try to load the newsest one from
  TCHAR szTemp[4096];
  // But before wqe do this, we first check if the ".local" file exists
  if (GetModuleFileName(NULL, szTemp, 4096) > 0)
  {
    TCHAR *slash = _tcsrchr (szTemp, _T('\\'));
    if (slash)
    {
      slash[1] = _T('\0');
      _tcscat (szTemp, _T("dbghelp.dll.local"));

      if (GetFileAttributes(szTemp) != INVALID_FILE_ATTRIBUTES)
      {
        hDbgHlp = LoadLibrary(szTemp);
      }
    }
  }

  if (hDbgHlp == NULL)  // if not already loaded, try to load a default-one
  {
    hDbgHlp = LoadLibrary( _T("dbghelp.dll") );
  }

  return hDbgHlp;
}

