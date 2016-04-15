/**	\file syncaccessor.h
 *	一些辅助函数的声明.
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_SYNCACCESSOR_H
#define __ATOM_KERNEL_SYNCACCESSOR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "basedefs.h"

#define ATOM_SYNC_ACCESSOR(classname, accessor, object) ::ATOM_SyncAccessor<classname> accessor(true, object)
#define ATOM_TRYSYNC_ACCESSOR(classname, accessor, object) ::ATOM_SyncAccessor<classname> accessor(false, object)

template <class T>
class ATOM_SyncAccessor
{
	static ATOM_Mutex L;

	T *object;

public:
	ATOM_SyncAccessor (bool block, T *p)
	{ 
		if (block)
		{
			L.lock (); 
			object = p;
		}
		else
		{
			object = L.trylock () ? p : 0;
		}
	}

	~ATOM_SyncAccessor (void) 
	{ 
		if (object)
		{
			L.unlock ();
			object = 0;
		}
	}

	T *get (void) const 
	{ 
		return object; 
	}
};

#endif // __ATOM_KERNEL_SYNCACCESSOR_H
/*! @} */
