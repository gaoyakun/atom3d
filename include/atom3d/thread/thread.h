#ifndef __ATOM3D_THREAD_THREAD_H
#define __ATOM3D_THREAD_THREAD_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "basedefs.h"

class ATOM_THREAD_API ATOM_ThreadFunc
{
public:
	typedef int (ATOM_CALL * ThreadFunc)(void*);
	typedef unsigned ThreadId;

public:
	ATOM_ThreadFunc (ThreadFunc threadFunc, void *userdata);
	~ATOM_ThreadFunc (void);

private:
	ATOM_ThreadFunc (const ATOM_ThreadFunc&);
	void operator = (const ATOM_ThreadFunc&);

public:
	ThreadId getThreadId (void) const;
	void wait (int *state);
	void kill (void);

public:
	static ThreadId getCurrentThreadId (void);

private:
	struct ThreadImpl *_impl;
};

#endif // __ATOM3D_THREAD_THREAD_H
