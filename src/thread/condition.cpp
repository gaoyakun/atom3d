#include <windows.h>
#include "condition.h"
#include "mutex.h"
#include "semaphore.h"

struct ConditionImpl
{
	ATOM_Mutex lock;
	int waiting;
	int signals;
	ATOM_Semaphore wait_sem;
	ATOM_Semaphore wait_done;
};

ATOM_Condition::ATOM_Condition (void)
{
	_impl = new ConditionImpl;
	_impl->waiting = 0;
	_impl->signals = 0;
}

ATOM_Condition::~ATOM_Condition (void)
{
	delete _impl;
}

void ATOM_Condition::signal (void)
{
	_impl->lock.lock ();
	if (_impl->waiting > _impl->signals)
	{
		++_impl->signals;
		_impl->wait_sem.post ();
		_impl->lock.unlock ();
		_impl->wait_done.wait ();
	}
	else
	{
		_impl->lock.unlock ();
	}
}

void ATOM_Condition::broadcast (void)
{
	_impl->lock.lock ();
	if (_impl->waiting > _impl->signals)
	{
		int num_waiting = _impl->waiting - _impl->signals;
		_impl->signals = _impl->waiting;
		for (int i = 0; i < num_waiting; ++i)
		{
			_impl->wait_sem.post ();
		}
		_impl->lock.unlock ();
		for (int i = 0; i < num_waiting; ++i)
		{
			_impl->wait_done.wait ();
		}
	}
	else
	{
		_impl->lock.unlock ();
	}
}

bool ATOM_Condition::wait (ATOM_Mutex &mutex)
{
	return waitTimeout (mutex, ATOM_MAXWAIT);
}

bool ATOM_Condition::waitTimeout (ATOM_Mutex &mutex, unsigned timeout)
{
	bool retval;

	_impl->lock.lock ();
	++_impl->waiting;
	_impl->lock.unlock ();

	mutex.unlock ();
	if (timeout == ATOM_MAXWAIT)
	{
		retval = _impl->wait_sem.wait ();
	}
	else
	{
		retval = _impl->wait_sem.waitTimeout (timeout);
	}

	_impl->lock.lock ();
	if (_impl->signals > 0)
	{
		if (!retval)
		{
			_impl->wait_sem.wait ();
		}
		_impl->wait_done.post ();
		--_impl->signals;
	}
	--_impl->waiting;
	_impl->lock.unlock ();

	mutex.lock ();

	return retval;
}

