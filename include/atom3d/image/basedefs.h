#ifndef __ATOM_IMAGE_BASEDEFS_H
#define __ATOM_IMAGE_BASEDEFS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_basedefs.h"
#include "../ATOM_kernel.h"

#if ATOM3D_SHARED_LIBS
#	ifdef BUILD_ATOM_IMAGE
#		define ATOM_IMAGE_API ATOM_EXPORT
#	else
#		define ATOM_IMAGE_API ATOM_IMPORT
#	endif
#else
#	define ATOM_IMAGE_API
#endif

ATOM_IMAGE_API ATOM_DECLARE_PLUGINS(Image)

#endif // __ATOM_IMAGE_BASEDEFS_H
