#ifndef __ATOM_SCRIPT_BASEDEFS_H
#define __ATOM_SCRIPT_BASEDEFS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_basedefs.h"

#if ATOM3D_SHARED_LIBS
#	ifdef BUILD_ATOM_SCRIPT
#		define ATOM_SCRIPT_API ATOM_EXPORT
#	else
#		define ATOM_SCRIPT_API ATOM_IMPORT
#	endif
#else
#	define ATOM_SCRIPT_API
#endif

typedef void (*ATOM_DereferenceFunc) (void*);

#endif // __ATOM_SCRIPT_BASEDEFS_H
