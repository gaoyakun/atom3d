#ifndef __ATOM3D_THREAD_SEMAPHORE_H
#define __ATOM3D_THREAD_SEMAPHORE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

class ATOM_THREAD_API ATOM_Semaphore
{
public:
	ATOM_Semaphore (void);
	ATOM_Semaphore (long value);
	~ATOM_Semaphore (void);

private:
	ATOM_Semaphore (const ATOM_Semaphore&);
	void operator = (const ATOM_Semaphore&);

public:
	bool wait (void);
	bool tryWait (void);
	bool waitTimeout (unsigned timeout);
	bool post (void);
	unsigned getValue (void) const;

private:
	struct SemaphoreImpl *_impl;
};

#endif // __ATOM3D_THREAD_SEMAPHORE_H
