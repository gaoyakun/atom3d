#ifndef __ATOM3DX_TW_WRAPPER_H
#define __ATOM3DX_TW_WRAPPER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <AntTweakBar.h>
#include "basedefs.h"

ATOMX_API bool ATOM_CALL ATOMX_LoadTweakBarLib (void);
ATOMX_API void ATOM_CALL ATOMX_UnloadTweakBarLib (void);

#endif // __ATOM3DX_TW_WRAPPER_H
