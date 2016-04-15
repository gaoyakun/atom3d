#ifndef __ATOM3D_THREAD_MUTEX_H
#define __ATOM3D_THREAD_MUTEX_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

class ATOM_THREAD_API ATOM_Mutex
{
public:
	ATOM_Mutex (unsigned spincount = 0);
	~ATOM_Mutex (void);

private:
	ATOM_Mutex (const ATOM_Mutex&);
	void operator = (const ATOM_Mutex&);

public:
	void lock (void);
	bool trylock (void);
	void unlock (void);

public:
	struct ScopeMutex
	{
		ATOM_Mutex &l;
		ScopeMutex (ATOM_Mutex &mutex): l(mutex) { l.lock (); }
		~ScopeMutex (void) { l.unlock (); }
	private:
		ScopeMutex (const ScopeMutex &);
		void operator = (const ScopeMutex&);
	};

private:
	struct MutexImpl *impl;
};

class ATOM_THREAD_API ATOM_FastMutex
{
public:
	ATOM_FastMutex (unsigned spincount = 0);
	void lock (void);
	bool trylock (void);
	void unlock (void);

public:
	struct ScopeMutex
	{
		ATOM_FastMutex &l;
		ScopeMutex (ATOM_FastMutex &mutex): l(mutex) { l.lock (); }
		~ScopeMutex (void) { l.unlock (); }
	private:
		ScopeMutex (const ScopeMutex &);
		void operator = (const ScopeMutex&);
	};

private:
	long volatile m_lockflag;
	long m_spincount;
	long m_lockcount;
};

class ATOM_THREAD_API ATOM_RWMutex
{
private:
	volatile unsigned Main;
	static const unsigned WriteDesireBit = 0x80000000;

	void Noop( unsigned tick )
	{
		if ( ((tick + 1) & 0xfff) == 0 )     // Sleep after 4k cycles. Crude, but usually better than spinning indefinitely.
			Sleep(0);
	}

public:
	ATOM_RWMutex(void);
	~ATOM_RWMutex(void);

	void readLock (void);
	void writeLock (void);
	void readUnlock (void);
	void writeUnlock (void);

public:
	struct ScopeMutexRead
	{
		ATOM_RWMutex &l;
		ScopeMutexRead (ATOM_RWMutex &mutex): l(mutex) { l.readLock (); }
		~ScopeMutexRead (void) { l.readUnlock (); }
	private:
		ScopeMutexRead (const ScopeMutexRead &);
		void operator = (const ScopeMutexRead&);
	};

	struct ScopeMutexWrite
	{
		ATOM_RWMutex &l;
		ScopeMutexWrite (ATOM_RWMutex &mutex): l(mutex) { l.writeLock (); }
		~ScopeMutexWrite (void) { l.writeUnlock (); }
	private:
		ScopeMutexWrite (const ScopeMutexWrite &);
		void operator = (const ScopeMutexWrite &);
	};
};

#endif // __ATOM3D_THREAD_MUTEX_H
