#ifndef __ATOM_DEBUGHELP_UTILS_H
#define __ATOM_DEBUGHELP_UTILS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"
#include "stl.h"

ATOM_DBGHLP_API ATOM_STRING ATOM_DumpMemoryAsByte (const void *p, unsigned totalBytes);
ATOM_DBGHLP_API ATOM_STRING ATOM_DumpMemoryAsWord (const void *p, unsigned totalBytes);
ATOM_DBGHLP_API ATOM_STRING ATOM_DumpMemoryAsDword (const void *p, unsigned totalBytes);
ATOM_DBGHLP_API ATOM_STRING ATOM_DisassembleMemory (const void *p, unsigned size);
ATOM_DBGHLP_API void ATOM_DumpMemoryAsByte (const void *p, unsigned totalBytes, char *buffer);
ATOM_DBGHLP_API void ATOM_DumpMemoryAsWord (const void *p, unsigned totalBytes, char *buffer);
ATOM_DBGHLP_API void ATOM_DumpMemoryAsDword (const void *p, unsigned totalBytes, char *buffer);

#endif // __ATOM_DEBUGHELP_STL_H
