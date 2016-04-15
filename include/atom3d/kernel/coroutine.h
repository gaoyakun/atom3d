#ifndef __ATOM3D_KERNEL_COROUTINE_H
#define __ATOM3D_KERNEL_COROUTINE_H

#include "basedefs.h"

class ATOM_KERNEL_API ATOM_Coroutine
{
public:
	struct Coroutine;
	typedef void (ATOM_CALL *CoroutineFunc) (void *param);

public:
	static void init (unsigned coroutineStackSize);
	static void done (void);
	static Coroutine *createCoroutine (CoroutineFunc func, void *param);
	static Coroutine *getCurrentCoroutine (void);
	static void deleteCoroutine (Coroutine *coroutine);
	static bool isCoroutine (Coroutine *coroutine);
	static void yieldTo (Coroutine *target, unsigned sleepTimeInMs);
	static unsigned getNumCoroutines (void);
	static bool isEmpty (void);
	static void dispatch (void);
};

#endif // __ATOM3D_KERNEL_COROUTINE_H
