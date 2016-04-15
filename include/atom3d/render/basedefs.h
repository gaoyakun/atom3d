/** \file render/basedefs.h
 *	��Ⱦ�ײ����ͷ�ļ�.
 *
 *	\defgroup render ATOM��Ⱦ�ײ��
 *	\author ������
 */

#ifndef __ATOM_RENDER_BASEDEFS_H
#define __ATOM_RENDER_BASEDEFS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_basedefs.h"

#if ATOM3D_SHARED_LIBS
#	ifdef BUILD_ATOM_RENDER
#		define ATOM_RENDER_API ATOM_EXPORT
#	else
#		define ATOM_RENDER_API ATOM_IMPORT
#	endif
#else
#	define ATOM_RENDER_API
#endif

ATOM_RENDER_API ATOM_DECLARE_PLUGINS(Render)

#endif // __ATOM_RENDER_BASEDEFS_H
