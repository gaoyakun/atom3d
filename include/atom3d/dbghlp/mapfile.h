#ifndef __ATOM_DEBUGHELP_MAPFILE_H__
#define __ATOM_DEBUGHELP_MAPFILE_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <string>
#include "basedefs.h"

#pragma warning(push)
#pragma warning(disable:4251)

struct ATOM_FunctionTraceInfo
{
  ATOM_ULongLong offset;
  ATOM_ULongLong base_of_image;
  char funcname[512];
  char filename[512];
  int lineno;
};

extern"C" ATOM_DBGHLP_API bool ATOM_GetFunctionTraceInfoFromMapFile (const char *mapfile, ATOM_FunctionTraceInfo *info);
extern"C" ATOM_DBGHLP_API bool ATOM_GetFunctionTraceInfoFromMapFileInMemory (const char *content, ATOM_FunctionTraceInfo *info);
extern"C" ATOM_DBGHLP_API bool ATOM_UndecorateFunctionName (const char *in, char *out, size_t size);

#pragma warning(pop)

#endif // __ATOM_DEBUGHELP_MAPFILE_H__
