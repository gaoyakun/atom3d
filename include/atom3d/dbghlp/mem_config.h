#ifndef __DEBUGHELP_MEM_CONFIG_H
#define __DEBUGHELP_MEM_CONFIG_H

#if _MSC_VER > 1000
#pragma once
#endif

void ATOM_DetermineMallocAlgorithm (void);
ATOM_DBGHLP_API unsigned ATOM_CALL ATOM_GetTotalMallocSize (void);

#endif // __DEBUGHELP_MEM_CONFIG_H
