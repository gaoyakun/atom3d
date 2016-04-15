#ifndef __ATOM_UTILS_INSTANCEGUARD_H
#define __ATOM_UTILS_INSTANCEGUARD_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <n3_dbghlp.h>

template <class T>
struct InstanceGuard
{
	DH_HASHSET<T*> instanceSet;
	ATOM_Mutex mutex;

	void addInstance (T *p)
	{
		ATOM_Mutex::ScopeMutex lock(mutex);
		instanceSet.insert (p);
	}

	void removeInstance (T *p)
	{
		ATOM_Mutex::ScopeMutex lock(mutex);
		DH_HASHSET<T*>::iterator it = instanceSet.find (p);
		if (it != instanceSet.end ())
		{
			instanceSet.erase (it);
		}
	}

	bool lockInstance (T *p)
	{
		mutex.lock ();

		DH_HASHSET<T*>::iterator it = instanceSet.find (p);
		if (it == instanceSet.end ())
		{
			mutex.unlock ();
			return false;
		}

		return true;
	}

	void unlockInstance (T *p)
	{
		mutex.unlock ();
	}
};

#endif // __ATOM_UTILS_INSTANCEGUARD_H
