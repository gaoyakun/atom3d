#include "stdafx.h"

ATOM_BEGIN_EVENT_MAP(ATOM_Thread, ATOM_EventDispatcher)
  ATOM_EVENT_HANDLER(ATOM_Thread, ATOM_ThreadInitEvent, onThreadInit)
  ATOM_EVENT_HANDLER(ATOM_Thread, ATOM_ThreadExitEvent, onThreadExit)
  ATOM_EVENT_HANDLER(ATOM_Thread, ATOM_ThreadQuitEvent, onThreadQuit)
ATOM_END_EVENT_MAP

ATOM_Thread::ATOM_Thread (void)
{
	ATOM_STACK_TRACE(ATOM_Thread::ATOM_Thread);
	_returnCode = 0;
	_frameInterval = 0;
	_timerChanged = false;
	_timerHandle = NULL;
	_eventHandle = NULL;
	_threadHandle = NULL;
}

ATOM_Thread::~ATOM_Thread (void)
{
	ATOM_STACK_TRACE(ATOM_Thread::~ATOM_Thread);
	if (_threadHandle)
	{
		postQuitEvent (0);

		if (WAIT_TIMEOUT == ::WaitForSingleObject (_threadHandle, 10000))
		{
			::TerminateThread (_threadHandle, 1);
		}
		::CloseHandle (_threadHandle);
		_threadHandle = NULL;
	}

	if (_timerHandle)
	{
		::CloseHandle (_timerHandle);
		_timerHandle = 0;
	}

	if (_eventHandle)
	{
		::CloseHandle (_eventHandle);
		_eventHandle = 0;
	}
}

bool ATOM_Thread::pumpEvents (int *retVal)
{
	ATOM_STACK_TRACE(ATOM_Thread::pumpEvents);
	if (_frameInterval)
	{
		HANDLE h[2] = { _timerHandle, _eventHandle };
		::WaitForMultipleObjects (2, h, FALSE, INFINITE);

		if (!dispatchPendingEvents ())
		{
			_frameStamp.update ();
			ATOM_ThreadIdleEvent event;
			handleEvent (&event);
		}
		else
		{
			if (retVal) *retVal = _returnCode;
		}

		if (_timerChanged)
		{
			_timerChanged = false;

			if (_frameInterval)
			{
				LARGE_INTEGER li;
				li.QuadPart = -100;
				::SetWaitableTimer (_timerHandle, &li, _frameInterval, NULL, NULL, FALSE);
			}
		}
	}
	else
	{
		if (!dispatchPendingEvents ())
		{
			_frameStamp.update ();
			ATOM_ThreadIdleEvent event;
			handleEvent (&event);
		}
		else
		{
			if (retVal) *retVal = _returnCode;
		}

		if (_timerChanged)
		{
			_timerChanged = false;

			if (_frameInterval != 0)
			{
				if (!_timerHandle)
				{
					_timerHandle = ::CreateWaitableTimer (NULL, FALSE, NULL);
					ATOM_ASSERT(_timerHandle);

					LARGE_INTEGER li;
					li.QuadPart = -100;
					::SetWaitableTimer (_timerHandle, &li, _frameInterval, NULL, NULL, FALSE);
				}

				if (!_eventHandle)
				{
					_eventHandle = ::CreateEvent (NULL, FALSE, FALSE, NULL);
				}
			}
		}
	}

	return _quitRequest;
}

int ATOM_Thread::eventLoop (void)
{
	ATOM_STACK_TRACE(ATOM_Thread::eventLoop);
	_quitRequest = false;

	if (_frameInterval)
	{
		if (!_timerHandle)
		{
			_timerHandle = ::CreateWaitableTimer (NULL, FALSE, NULL);
			ATOM_ASSERT(_timerHandle);

			LARGE_INTEGER li;
			li.QuadPart = -100;
			::SetWaitableTimer (_timerHandle, &li, _frameInterval, NULL, NULL, FALSE);
		}

		if (!_eventHandle)
		{
			_eventHandle = ::CreateEvent (NULL, FALSE, FALSE, NULL);
		}

		_timerChanged = false;
	}

	{
		ATOM_ThreadInitEvent event;
		handleEvent (&event);
		if (!event.success)
		{
			return event.errorcode;
		}
	}

	try
	{
		while (!pumpEvents (0))
			;
	}
	catch (std::exception &)
	{
		if (_timerHandle)
		{
			::CloseHandle (_timerHandle);
			_timerHandle = 0;
		}

		if (_eventHandle)
		{
			::CloseHandle (_eventHandle);
			_eventHandle = 0;
		}

		ATOM_ERRHANDLER->reportError (0, ATOM_AppErrorHandler::ET_CPP_EXCEPTION);
		throw;
	}
	catch (...)
	{
		ATOM_ERRHANDLER->reportError (0, ATOM_AppErrorHandler::ET_UNKNOWN);
		throw;
	}

	{
		ATOM_ThreadExitEvent event;
		handleEvent (&event);
	}

	ATOM_LOGGER::log ("ATOM_Thread exit!\n");
	return _returnCode;
}

void ATOM_Thread::setFrameInterval (int ms)
{
  if (ms != _frameInterval)
  {
    _frameInterval = ms;
    _timerChanged = true;
  }
}

int ATOM_Thread::getFrameInterval (void)
{
  return _frameInterval;
}

void ATOM_Thread::onThreadInit (ATOM_ThreadInitEvent *event)
{
	event->success = true;
}

void ATOM_Thread::onThreadExit (ATOM_ThreadExitEvent *event)
{
}

void ATOM_Thread::onThreadQuit (ATOM_ThreadQuitEvent *event)
{
	_returnCode = event->returnValue;
	_quitRequest = true;
}

const ATOM_FrameStamp &ATOM_Thread::getFrameStamp (void)
{
	return _frameStamp;
}

void ATOM_Thread::onPushEvent (void)
{
	if (_eventHandle)
	{
		::SetEvent (_eventHandle);
	}
}

void ATOM_Thread::postQuitEvent (int ret)
{
	queueEvent (ATOM_NEW(ATOM_ThreadQuitEvent, ret), this);
}

int ATOM_Thread::run (void)
{
	ATOM_STACK_TRACE(ATOM_Thread::run);
	if (!isRunning ())
	{
		_threadHandle = (HANDLE)_beginthreadex (0, 0, &ATOM_Thread::threadMain, this, 0, 0);
	}

	return 0;
}

static int myhandler(int code, PEXCEPTION_POINTERS p)
{
	ATOM_ERRHANDLER->reportError (p, ATOM_AppErrorHandler::ET_WIN32_EXCEPTION);
	return EXCEPTION_CONTINUE_SEARCH;
}

unsigned __stdcall ATOM_Thread::threadMain (void *arg)
{
	ATOM_ERRHANDLER->setThreadErrorHandlers ();

	__try
	{
		ATOM_STACK_TRACE(ATOM_Thread::threadMain);
		ATOM_Thread* thread = (ATOM_Thread*)arg;
		return thread->eventLoop ();
	}
	__except(myhandler(_exception_code() , (PEXCEPTION_POINTERS)_exception_info()))
	{
	}
}

bool ATOM_Thread::isRunning (void) const
{
	ATOM_STACK_TRACE(ATOM_Thread::isRunning);
	if (_threadHandle)
	{
		if (WAIT_TIMEOUT == ::WaitForSingleObject (_threadHandle, 0))
		{
			return true;
		}
	}
	return false;
}

bool ATOM_Thread::wait (unsigned timeout, int *retval)
{
	ATOM_STACK_TRACE(ATOM_Thread::wait);
	if (_threadHandle && WAIT_TIMEOUT == ::WaitForSingleObject (_threadHandle, timeout))
	{
		return false;
	}
	return true;
}

void ATOM_Thread::terminate (unsigned timeout, int retval)
{
	ATOM_STACK_TRACE(ATOM_Thread::terminate);
	if (_threadHandle)
	{
		postQuitEvent (retval);

		if (WAIT_TIMEOUT == ::WaitForSingleObject (_threadHandle, timeout))
		{
			::TerminateThread (_threadHandle, retval);
			::CloseHandle (_threadHandle);
			_threadHandle = 0;
		}
	}
}


