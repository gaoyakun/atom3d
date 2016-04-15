#ifndef ATOM_AUDIO_PUBLIC_H_
#define ATOM_AUDIO_PUBLIC_H_

#if _MSC_VER > 1000
# pragma once
#endif

//==========================================================================

#include<m_public.h>

//==========================================================================
#ifdef AUDIO_EXPORTS
#	define ATOM_LIB_AUDIO	M_DYNAMICLIB_EXPORT
#else
#	define ATOM_LIB_AUDIO	M_DYNAMICLIB_IMPORT
#endif

//==========================================================================
#endif//ATOM_AUDIO_PUBLIC_H_
