#ifndef __ATOM3D_THREAD_CONDITION_H
#define __ATOM3D_THREAD_CONDITION_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

class ATOM_THREAD_API ATOM_Mutex;

class ATOM_THREAD_API ATOM_Condition
{
public:
	ATOM_Condition (void);
	~ATOM_Condition (void);

private:
	ATOM_Condition (const ATOM_Condition&);
	void operator = (const ATOM_Condition&);

public:
	void signal (void);
	void broadcast (void);
	bool wait (ATOM_Mutex &mutex);
	bool waitTimeout (ATOM_Mutex &mutex, unsigned timeout);

private:
	struct ConditionImpl *_impl;
};

#endif // __ATOM3D_THREAD_CONDITION_H
