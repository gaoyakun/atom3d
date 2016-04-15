#ifndef __ATOM_FONT_BASEDEFS_H
#define __ATOM_FONT_BASEDEFS_H

#include "../ATOM_basedefs.h"

#if ATOM3D_SHARED_LIBS
#	ifdef BUILD_ATOM_FONT
#		define ATOM_FONT_API ATOM_EXPORT
#	else
#		define ATOM_FONT_API ATOM_IMPORT
#	endif
#else
#	define ATOM_FONT_API
#endif

#endif // __ATOM_FONT_BASEDEFS_H
