/** \file engine/basedefs.h
 *	��������ͷ�ļ�.
 *
 *	\defgroup engine ATOM�����
 *	\author ������
 */

#ifndef __ATOM_ENGINE_BASEDEFS_H
#define __ATOM_ENGINE_BASEDEFS_H

#ifdef WIN32
# pragma comment(lib, "imm32.lib")
#endif

#include "../ATOM_basedefs.h"

#if ATOM3D_SHARED_LIBS
#	ifdef BUILD_ATOM_ENGINE
#		define ATOM_ENGINE_API ATOM_EXPORT
#	else
#		define ATOM_ENGINE_API ATOM_IMPORT
#	endif
#else
#	define ATOM_ENGINE_API
#endif

ATOM_ENGINE_API ATOM_DECLARE_PLUGINS(Engine)

#endif // __ATOM_ENGINE_BASEDEFS_H
