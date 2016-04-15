#include <windows.h>
#include <commctrl.h>
#include <tchar.h>

HINSTANCE hModelHandle = NULL;
int controlsInit = FALSE;

BOOL WINAPI DllMain (HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		hModelHandle = hinstDll;
		if (!controlsInit) {
			controlsInit = TRUE;
			InitCommonControls();			// Initialize Win95 controls
		}
		break; 
	default:
		break;
	}
	return TRUE;
}

extern void *GetMaxExporterDesc (void);
extern int GetMaxVersion (void);

__declspec(dllexport) const TCHAR *LibDescription ()
{
	return _T("ATOM3D Max Exporter");
}

__declspec(dllexport) int LibNumberClasses()
{
	return 1;
}

__declspec(dllexport) void* LibClassDesc(int i)
{
	switch(i) {
		case 0: return GetMaxExporterDesc();
		default: return 0;
	}
}

__declspec(dllexport) ULONG LibVersion()
{
	return GetMaxVersion();
}

__declspec ( dllexport ) ULONG CanAutoDefer()
{
	return 0;
}

