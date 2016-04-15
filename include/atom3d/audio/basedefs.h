#ifndef __ATOM_AUDIO_BASEDEFS_H
#define __ATOM_AUDIO_BASEDEFS_H

#include "../ATOM_basedefs.h"

#if ATOM3D_SHARED_LIBS
#	ifdef BUILD_ATOM_AUDIO
#		define ATOM_AUDIO_API ATOM_EXPORT
#	else
#		define ATOM_AUDIO_API ATOM_IMPORT
#	endif
#else
#	define ATOM_AUDIO_API
#endif

#endif // __ATOM_AUDIO_BASEDEFS_H
