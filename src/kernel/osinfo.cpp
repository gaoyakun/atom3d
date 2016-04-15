#include "stdafx.h"

ATOM_OSInfo::ATOM_OSInfo (void)
{
  OSVERSIONINFOEX osvi;
  BOOL bIsWindows64Bit;
  BOOL bOsVersionInfoEx;

  strcpy (osname, "Unknown Windows ");
  type = OS_UNKNOWN;

  memset (&osvi, 0, sizeof(OSVERSIONINFOEX));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

  if (!(bOsVersionInfoEx = ::GetVersionEx ((OSVERSIONINFO*)&osvi)))
  {
    // GetVersionEx doesn't work, try OSVERSIONINFO
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (!::GetVersionEx ((OSVERSIONINFO*)&osvi))
    {
      type = OS_UNKNOWN;
      strcpy (osname, "n/a");
      return;
    }
  }

  switch (osvi.dwPlatformId)
  {
  case VER_PLATFORM_WIN32_NT:
	if (osvi.dwMajorVersion <= 4) 
    {
      strcpy (osname, "Microsoft Windows NT  ");
      type = OS_WINDOWS_NT;
    }
	else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) 
    {
      strcpy (osname, "Microsoft Windows 2000 ");
      type = OS_WINDOWS_2000;
    }
	else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) 
    {
      strcpy (osname, "Microsoft Windows XP ");
      type = OS_WINDOWS_XP;
    }

    if (bOsVersionInfoEx)
    {
	  if (osvi.wProductType == VER_NT_WORKSTATION) 
      {
		if (osvi.wSuiteMask & VER_SUITE_PERSONAL) 
          strcat (osname, "Personal ");
		else 
          strcat (osname, "Professional ");
	  } 
      else if (osvi.wProductType == VER_NT_SERVER) 
      {
	    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) 
          strcpy (osname, "Microsoft Windows .NET ");
				
		if (osvi.wSuiteMask & VER_SUITE_DATACENTER) 
          strcat (osname, "DataCenter Server ");
		else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE) 
          strcat (osname, "Advanced Server ");
		else 
          strcat (osname, "Server ");
	  }
    }
    else
    {
	  HKEY hKey;
	  char szProductType[80];
	  DWORD dwBufLen;

	  RegOpenKeyEx (HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\ProductOptions", 0, KEY_QUERY_VALUE, &hKey);
	  RegQueryValueEx (hKey, "ProductType", NULL, NULL, (LPBYTE) szProductType, &dwBufLen);
	  RegCloseKey (hKey);

	  if (lstrcmpi ("WINNT", szProductType) == 0) 
      {
        strcat (osname, "Professional ");
      }

	  if (lstrcmpi ("LANMANNT", szProductType) == 0)
      {
		  if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) 
			  strcat (osname, "Standard Server ");
		  else 
              strcat (osname, "Server ");
      }

	  if (lstrcmpi ("SERVERNT", szProductType) == 0)
      {
		  if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) 
			  strcat (osname, "Enterprise Server ");
		  else 
			strcat (osname, "Advanced Server ");
      }
    }

    if (osvi.dwMajorVersion <= 4)
    {
      // NT 4.0 and earlier
      sprintf (osname, "%sversion %d.%d %s (Build %d)",
        osname, osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);
    }
    else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
    {
      // Windows XP and .NET Server
      typedef BOOL (CALLBACK * LPFNPROC)(HANDLE, BOOL*);
      bIsWindows64Bit = false;
      HINSTANCE hKernelDll = ::LoadLibrary("Kernel32");
      
      if (hKernelDll)
      {
        LPFNPROC proc = (LPFNPROC)::GetProcAddress (hKernelDll, "IsWow64Process");

        if (proc)
        {
          (void) proc (GetCurrentProcess (), &bIsWindows64Bit);
        }

        FreeLibrary (hKernelDll);
      }

      if (bIsWindows64Bit)
        strcat (osname, "64-Bit ");
      else
        strcat (osname, "32-Bit ");
    }
    else
    {
			// Windows 2000 and everything else.
  			sprintf (osname, "%s%s(Build %d)", osname, osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);
    }

    break;

  case VER_PLATFORM_WIN32_WINDOWS:
		  // Test for the product.
	if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0) 
    {
	  strcpy (osname, "Microsoft Windows 95 ");
      type = OS_WINDOWS_95;

	  if (osvi.szCSDVersion[1] == 'C') 
      {
        strcat (osname, "OSR 2.5 ");
        type = OS_WINDOWS_95_OSR2;
      }
	  else if (osvi.szCSDVersion[1] == 'B') 
      {
        strcat (osname, "OSR 2 ");
        type = OS_WINDOWS_95_OSR2;
      }
	} 

	if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10) 
    {
	  strcpy (osname, "Microsoft Windows?98 ");
      type = OS_WINDOWS_98;

	  if (osvi.szCSDVersion[1] == 'A' ) 
        strcat (osname, "SE ");
	} 

	if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90) 
    {
	  strcpy (osname, "Microsoft Windows Me ");
      type = OS_WINDOWS_ME;
	} 
	break;

  case VER_PLATFORM_WIN32s:
    strcpy (osname, "Microsoft Win32s ");
    type = OS_WINDOWS_WIN32s;
    break;

  default:
    strcpy (osname, "Unknown Windows ");
    type = OS_UNKNOWN;
    break;
  }
}

ATOM_OSInfo::WINDOWS_TYPE ATOM_OSInfo::getOSType (void) const 
{
  return type;
}

const char * ATOM_OSInfo::getOSName (void) const
{
  return osname;
}

