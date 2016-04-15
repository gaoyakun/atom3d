#ifndef __ATOM_THREAD_BASEDEFS_H
#define __ATOM_THREAD_BASEDEFS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_basedefs.h"

#if ATOM3D_SHARED_LIBS
#	ifdef BUILD_ATOM_THREAD
#		define ATOM_THREAD_API ATOM_EXPORT
#	else
#		define ATOM_THREAD_API ATOM_IMPORT
#	endif
#else
#	define ATOM_THREAD_API
#endif

#define ATOM_MAXWAIT	(~(unsigned)0)

#endif//__ATOM_THREAD_BASEDEFS_H
