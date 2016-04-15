/** \file atom3dx/basedefs.h
 *	辅助库基本头文件.
 *
 *	\defgroup atom3dx ATOM辅助库
 *	\author 高雅昆
 */

#ifndef __ATOMX_BASEDEFS_H
#define __ATOMX_BASEDEFS_H

#if _MSC_VER > 1000
# pragma once
#endif

#if ATOM3D_SHARED_LIBS
#	ifdef BUILD_ATOMX
#		define ATOMX_API __declspec(dllexport)
#	else
#		define ATOMX_API __declspec(dllimport)
#	endif
#else
#	define ATOMX_API
#endif

# define ATOMX_CALL   __cdecl

#endif // __ATOMX_BASEDEFS_H
