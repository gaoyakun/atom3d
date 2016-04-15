#ifndef ATOM_COMMONDLG_PUBLIC_H_
#define ATOM_COMMONDLG_PUBLIC_H_

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_basedefs.h"

#if ATOM3D_SHARED_LIBS
#	ifdef BUILD_ATOM_COMMONDLG
#		define ATOM_COMMONDLG_API ATOM_EXPORT
#	else
#		define ATOM_COMMONDLG_API ATOM_IMPORT
#	endif
#else
#	define ATOM_COMMONDLG_API
#endif

#endif//ATOM_COMMONDLG_PUBLIC_H_
