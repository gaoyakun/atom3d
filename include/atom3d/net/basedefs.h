#ifndef __ATOM_NET_BASEDEFS_H
#define __ATOM_NET_BASEDEFS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_basedefs.h"

#if ATOM3D_SHARED_LIBS
#	ifdef BUILD_ATOM_NET
#		define ATOM_NET_API ATOM_EXPORT
#	else
#		define ATOM_NET_API ATOM_IMPORT
#	endif
#else
#	define ATOM_NET_API
#endif

#define ATOM_NET_DOWNLOAD_FAILED			-1
#define ATOM_NET_DOWNLOAD_CANCELED			-2
#define ATOM_NET_DOWNLOAD_NOTSTART			-3
#define ATOM_NET_DOWNLOAD_OK				0
#define ATOM_NET_DOWNLOAD_INPROGRESS		1

#endif//__ATOM_NET_BASEDEFS_H
