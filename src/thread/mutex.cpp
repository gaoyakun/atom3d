#include <windows.h>
#include <intrin.h>
#include "mutex.h"
#include "cas.h"

struct MutexImpl
{
  CRITICAL_SECTION l;

  MutexImpl (unsigned spincount) 
  { 
    if (spincount == 0)
    {
      spincount = 4000;
    }

    ::InitializeCriticalSectionAndSpinCount (&l, spincount);
  };

  ~MutexImpl (void) 
  { 
    ::DeleteCriticalSection (&l); 
  }

  void lock (void) 
  { 
    ::EnterCriticalSection (&l); 
  }

  bool trylock (void) 
  { 
    return ::TryEnterCriticalSection (&l) == TRUE; 
  }

  void unlock (void) 
  { 
    ::LeaveCriticalSection (&l); 
  }
};

ATOM_Mutex::ATOM_Mutex (unsigned spincount)
{
  impl = new MutexImpl(spincount);
}

ATOM_Mutex::~ATOM_Mutex (void)
{
  delete impl;
}

void ATOM_Mutex::lock (void)
{
  impl->lock ();
}

bool ATOM_Mutex::trylock (void)
{
  return impl->trylock ();
}

void ATOM_Mutex::unlock (void)
{
  impl->unlock ();
}

static void nsec_sleep(int log_nsec) 
{
  if (log_nsec <= 20) 
  {
    ::SwitchToThread();
  } 
  else 
  {
    ::Sleep(1 << (log_nsec - 20));
  }
}

ATOM_FastMutex::ATOM_FastMutex (unsigned spincount)
{
  m_lockcount = 0;
  m_lockflag = 0;
  m_spincount = spincount ? spincount : 1000;
}

void ATOM_FastMutex::lock (void)
{
  long volatile *pLockFlag = &m_lockflag;
  long tid = (long)::GetCurrentThreadId ();

  if (ATOM_CAS(pLockFlag, tid, tid))
  {
    ++m_lockcount;
    return;
  }

  if (ATOM_CAS(pLockFlag, 0, tid))
  {
    ++m_lockcount;
    return;
  }

  volatile unsigned dummy = 17;

  int i;

  for (i = 0; i < m_spincount; ++i)
  {
    if (*pLockFlag)
    {
      dummy *= dummy;
      dummy *= dummy;
      dummy *= dummy;
      dummy *= dummy;
      continue;
    }
  }

  if (ATOM_CAS(pLockFlag, 0, tid))
  {
    ++m_lockcount;
    return;
  }

  for (i = 0; ; ++i)
  {
    int log_nsec = i + 6;
    if (log_nsec > 27)
    {
      log_nsec = 27;
    }

    if (ATOM_CAS(pLockFlag, 0, tid))
    {
      ++m_lockcount;
      return;
    }
    else
    {
      nsec_sleep (log_nsec);
    }
  }
}

bool ATOM_FastMutex::trylock (void)
{
  volatile long *pLockFlag = &m_lockflag;
  long tid = (long)::GetCurrentThreadId ();
  if (ATOM_CAS(pLockFlag, tid, tid))
  {
    ++m_lockcount;
    return true;
  }

  if (ATOM_CAS(pLockFlag, 0, tid))
  {
    ++m_lockcount;
    return true;
  }

  return false;
}

void ATOM_FastMutex::unlock (void)
{
  volatile long* pLockFlag = &m_lockflag;
  if (!--m_lockcount)
  {
    *pLockFlag = 0;
  }
}

ATOM_RWMutex::ATOM_RWMutex (void) 
{ 
	Main = 0; 
}

ATOM_RWMutex::~ATOM_RWMutex (void)                
{ 
}

void ATOM_RWMutex::readLock (void)
{
	for ( unsigned tick = 0 ;; tick++ )
	{
		unsigned oldVal = Main;
		if ( (oldVal & WriteDesireBit) == 0 )
		{
			if ( InterlockedCompareExchange( (LONG*) &Main, oldVal + 1, oldVal ) == oldVal )
				break;
		}
		Noop(tick);
	}
}

void ATOM_RWMutex::writeLock (void)
{
	for ( unsigned tick = 0 ;; tick++ )
	{
		if ( (tick & 0xfff) == 0 )
			_InterlockedOr( (LONG*) &Main, WriteDesireBit );

		unsigned oldVal = Main;
		if ( oldVal == WriteDesireBit )
		{
			if ( InterlockedCompareExchange( (LONG*) &Main, -1, WriteDesireBit ) == WriteDesireBit )
				break;
		}
		Noop(tick);
	}
}

void ATOM_RWMutex::readUnlock (void)
{
	InterlockedDecrement( (LONG*) &Main );
}

void ATOM_RWMutex::writeUnlock (void)
{
	InterlockedIncrement( (LONG*) &Main );
}
