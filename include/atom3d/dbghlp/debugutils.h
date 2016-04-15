#ifndef __ATOM_DEBUGHELP_DEBUGUTILS_H__
#define __ATOM_DEBUGHELP_DEBUGUTILS_H__

#include "basedefs.h"

// secure-CRT_functions are only available starting with VC8
#if _MSC_VER < 1400
#define strcpy_s strcpy
#define strcat_s(dst, len, src) strcat(dst, src)
#define _snprintf_s _snprintf
#define _tcscat_s _tcscat
#endif

HMODULE ATOM_LoadDebugHelpDll (void);

#endif // __ATOM_DEBUGHELP_DEBUGUTILS_H__
