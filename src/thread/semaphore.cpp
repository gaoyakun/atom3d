#include <windows.h>
#include "semaphore.h"

struct SemaphoreImpl
{
	HANDLE handle;
#if ATOM3D_COMPILER_GCC
	long count;
#else
	long volatile count;
#endif
};

ATOM_Semaphore::ATOM_Semaphore (void)
{
	_impl = new SemaphoreImpl;
	_impl->handle = ::CreateSemaphore (NULL, 0, 32*1024, NULL);
	_impl->count = 0;
}

ATOM_Semaphore::ATOM_Semaphore (long value)
{
	_impl = new SemaphoreImpl;
	_impl->handle = ::CreateSemaphore (NULL, value, 32*1024, NULL);
	_impl->count = value;
}

ATOM_Semaphore::~ATOM_Semaphore (void)
{
	::CloseHandle (_impl->handle);
	delete _impl;
}

bool ATOM_Semaphore::wait (void)
{
	return waitTimeout (ATOM_MAXWAIT);
}

bool ATOM_Semaphore::tryWait (void)
{
	return waitTimeout (0);
}

bool ATOM_Semaphore::waitTimeout (unsigned timeout)
{
	DWORD ms = ((timeout == ATOM_MAXWAIT) ? INFINITE : (DWORD)timeout);
	if (::WaitForSingleObject (_impl->handle, ms) == WAIT_OBJECT_0)
	{
		InterlockedDecrement (&_impl->count);
		return true;
	}
	return false;
}

bool ATOM_Semaphore::post (void)
{
	InterlockedIncrement(&_impl->count);
	if (::ReleaseSemaphore(_impl->handle, 1, NULL) == FALSE ) 
	{
		InterlockedDecrement (&_impl->count);
		return false;
	}
	return true;
}

unsigned ATOM_Semaphore::getValue (void) const
{
	return _impl->count;
}

