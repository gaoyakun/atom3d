/** \file dbghlp/basedefs.h
 *	����֧�ֿ����ͷ�ļ�.
 *
 *	\defgroup dbghlp ATOM����֧�ֿ�
 *	\author ������
 */

#ifndef __ATOM_DEBUGHELP_BASEDEFS_H__
#define __ATOM_DEBUGHELP_BASEDEFS_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_basedefs.h"

#if ATOM3D_SHARED_LIBS
#	ifdef BUILD_ATOM_DEBUGHELP
#		define ATOM_DBGHLP_API ATOM_EXPORT
#	else
#		define ATOM_DBGHLP_API ATOM_IMPORT
#	endif
#else
#	define ATOM_DBGHLP_API
#endif

#endif//__ATOM_DEBUGHELP_BASEDEFS_H__

