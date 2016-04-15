/** \file kernel/basedefs.h
 *	核心基本头文件.
 *
 *	\defgroup kernel ATOM核心库
 *	\author 高雅昆
 */

#ifndef __ATOM_KERNEL_BASEDEFS_H
#define __ATOM_KERNEL_BASEDEFS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_basedefs.h"

#if ATOM3D_SHARED_LIBS
#	ifdef BUILD_ATOM_KERNEL
#		define ATOM_KERNEL_API ATOM_EXPORT
#	else
#		define ATOM_KERNEL_API ATOM_IMPORT
#	endif
#else
#	define ATOM_KERNEL_API
#endif

#endif // __ATOM_KERNEL_BASEDEFS_H
