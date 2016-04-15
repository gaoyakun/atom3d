#ifndef __ATOM_VFS_BASEDEFS_H
#define __ATOM_VFS_BASEDEFS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_basedefs.h"

#if ATOM3D_SHARED_LIBS
#	ifdef BUILD_ATOM_VFS
#		define ATOM_VFS_API ATOM_EXPORT
#	else
#		define ATOM_VFS_API ATOM_IMPORT
#	endif
#else
#	define ATOM_VFS_API
#endif

#endif//__ATOM_VFS_BASEDEFS_H
