#define WIN32_MEAN_AND_LEAN
#include <windows.h>

HINSTANCE hModelHandle = NULL;

BOOL WINAPI DllMain (HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		hModelHandle = hinstDll;
		break;
	default:
		break;
	}
	return TRUE;
}
