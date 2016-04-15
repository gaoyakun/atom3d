#include <windows.h>
#include <process.h>
#include "thread.h"
#include "semaphore.h"
#include "mutex.h"

struct ThreadImpl
{
	ATOM_ThreadFunc::ThreadId threadId;
	HANDLE handle;
	int status;
	void *data;
};

struct ThreadArgs
{
	ATOM_ThreadFunc::ThreadFunc func;
	void *data;
	ThreadImpl *info;
	ATOM_Semaphore *wait;
};

static int maxthreads = 0;
static int numthreads = 0;
static ThreadImpl **threadArray = NULL;
static ATOM_Mutex thread_lock;

static void addThread (ThreadImpl *thread)
{
	ATOM_Mutex::ScopeMutex lock(thread_lock);

	if (numthreads == maxthreads)
	{
		ThreadImpl **threads = (ThreadImpl**)realloc (threadArray, (maxthreads+32)*(sizeof(*threads)));
		maxthreads += 32;
		threadArray = threads;
	}
	threadArray[numthreads++] = thread;
}

static void deleteThread (ThreadImpl *thread)
{
	ATOM_Mutex::ScopeMutex lock(thread_lock);

	int i;
	for (i = 0; i < numthreads; ++i)
	{
		if (thread == threadArray[i])
		{
			break;
		}
	}
	if (i < numthreads)
	{
		if (--numthreads > 0)
		{
			while (i < numthreads)
			{
				threadArray[i] = threadArray[i+1];
				++i;
			}
		}
		else
		{
			maxthreads = 0;
			free (threadArray);
			threadArray = 0;
		}
	}
}

static void runThread (void *data)
{
	ThreadArgs *impl = (ThreadArgs*)data;
	impl->info->threadId = (ATOM_ThreadFunc::ThreadId)::GetCurrentThreadId ();

	ATOM_ThreadFunc::ThreadFunc userfunc = impl->func;
	void *userdata = impl->data;
	int *statusloc = &impl->info->status;

	impl->wait->post ();
	*statusloc = userfunc (userdata);
}

static unsigned __stdcall ThreadProc(void *data)
{
  // Call the thread function!
  runThread (data);

  _endthreadex (0);

  return 0;
}

static int createThread (ThreadImpl *thread, void *args)
{
	unsigned threadid;
	thread->handle = (HANDLE)_beginthreadex (NULL, 0, &ThreadProc, args, 0, &threadid);
	return (thread->handle == NULL) ? -1 : 0;
}

ATOM_ThreadFunc::ATOM_ThreadFunc (ThreadFunc threadFunc, void *userdata)
{
	_impl = new ThreadImpl;

	ThreadArgs *args = new ThreadArgs;
	args->func = threadFunc;
	args->data = userdata;
	args->wait = new ATOM_Semaphore(0);
	args->info = _impl;
	args->info->threadId = 0;
	args->info->handle = 0;
	args->info->status = -1;
	args->info->data = 0;

	addThread (_impl);

	if (createThread (_impl, args) >= 0)
	{
		args->wait->wait ();
	}
	else
	{
		deleteThread (_impl);
		delete _impl;
		_impl = 0;
	}

	delete args->wait;
	delete args;
}

ATOM_ThreadFunc::~ATOM_ThreadFunc (void)
{
	kill ();
}

ATOM_ThreadFunc::ThreadId ATOM_ThreadFunc::getThreadId (void) const
{
	return _impl ? _impl->threadId : 0;
}

void ATOM_ThreadFunc::wait (int *state)
{
	if (_impl)
	{
		::WaitForSingleObject(_impl->handle, INFINITE);
		::CloseHandle(_impl->handle);
		if (state)
		{
			*state = _impl->status;
		}
		deleteThread (_impl);
		delete _impl;
		_impl = 0;
	}
}

void ATOM_ThreadFunc::kill (void)
{
	if (_impl)
	{
		::TerminateThread(_impl->handle, FALSE);
		wait (0);
	}
}

ATOM_ThreadFunc::ThreadId ATOM_ThreadFunc::getCurrentThreadId (void)
{
	return (ATOM_ThreadFunc::ThreadId)::GetCurrentThreadId ();
}

