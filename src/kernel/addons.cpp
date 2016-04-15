#include "StdAfx.h"
#include <io.h>
#include <fcntl.h>
#include "addons.h"
#include "thread.h"



static HANDLE _StdoutPipeRead = 0;
static HANDLE _StdoutPipeWrite = 0;
static HANDLE _StderrPipeRead = 0;
static HANDLE _StderrPipeWrite = 0;
static HANDLE _StdoutThreadHandle = 0;
static HANDLE _StderrThreadHandle = 0;
static DWORD _StdoutThreadId = 0;
static DWORD _StderrThreadId = 0;

static FILE _OldStdout = *stdout;
static FILE _OldStderr = *stderr;
static FILE *_StdoutPipeFp = 0;
static FILE *_StderrPipeFp = 0;
static ATOM_Mutex _StdoutCallbackLock(200);
static ATOM_Mutex _StderrCallbackLock(200);
static ATOM_Mutex _StdoutStringLock(200);
static ATOM_Mutex _StderrStringLock(200);
static ATOM_STRING _StdoutString;
static ATOM_STRING _StderrString;

static ATOM_VECTOR<ATOM_StdOutputCallback *> _Stdout_Callbacks;
static ATOM_VECTOR<ATOM_StdOutputCallback *> _Stderr_Callbacks;

static bool _PipeInitialized = false;

static DWORD ReadThread (HANDLE hRead, ATOM_STRING &str, ATOM_Mutex &lock)
{
	char buffer[2];

	DWORD bytesRead;

	for (;;)
	{
		if (!::ReadFile (hRead, buffer, 1, &bytesRead, 0) || !bytesRead)
		{
			break;
		}

		if (bytesRead > 0)
		{
			buffer[1] = '\0';

			{
				ATOM_Mutex::ScopeMutex sm(lock);
				str += buffer;
			}
		}
	}

	return 0;
}

static DWORD WINAPI StdoutReadThread (LPVOID)
{
	return ReadThread (_StdoutPipeRead, _StdoutString, _StdoutStringLock);
}

static DWORD WINAPI StderrReadThread (LPVOID)
{
	return ReadThread (_StderrPipeRead, _StderrString, _StderrStringLock);
}

static bool InitPipes (void)
{
	if (!::CreatePipe (&_StdoutPipeRead, &_StdoutPipeWrite, NULL, 0))
	{
		ATOM_LOGGER::error ("Create stdout pipe failed.\n");
		return false;
	}

	if (!::CreatePipe (&_StderrPipeRead, &_StderrPipeWrite, NULL, 0))
	{
		::CloseHandle (_StdoutPipeRead);
		::CloseHandle (_StdoutPipeWrite);
		_StdoutPipeRead = 0;
		_StdoutPipeWrite = 0;
		ATOM_LOGGER::error ("Create stderr pipe failed.\n");
		return false;
	}

	int hCrt = _open_osfhandle ((long)_StdoutPipeWrite, _O_TEXT);
	_StdoutPipeFp = _fdopen (hCrt, "w");
	ATOM_ASSERT(_StdoutPipeFp);

	hCrt = _open_osfhandle ((long)_StderrPipeWrite, _O_TEXT);
	_StderrPipeFp = _fdopen (hCrt, "w");
	ATOM_ASSERT(_StderrPipeFp);

	_StdoutThreadHandle = ::CreateThread (NULL, 0, &StdoutReadThread, NULL, CREATE_SUSPENDED, &_StdoutThreadId);
	if (!_StdoutThreadHandle)
	{
		ATOM_LOGGER::error ("Create stdout read thread failed.\n");
		::CloseHandle (_StdoutPipeRead);
		::CloseHandle (_StdoutPipeWrite);
		::CloseHandle (_StderrPipeRead);
		::CloseHandle (_StderrPipeWrite);
		_StdoutPipeRead = 0;
		_StdoutPipeWrite = 0;
		_StderrPipeRead = 0;
		_StderrPipeWrite = 0;
		_StdoutPipeFp = 0;
		_StderrPipeFp = 0;
		return false;
	}

	_StderrThreadHandle = ::CreateThread (NULL, 0, &StderrReadThread, NULL, CREATE_SUSPENDED, &_StdoutThreadId);
	if (!_StderrThreadHandle)
	{
		ATOM_LOGGER::error ("Create stderr read thread failed.\n");

		::TerminateThread (_StdoutThreadHandle, 0);
		::CloseHandle (_StdoutThreadHandle);
		_StdoutThreadHandle = 0;

		::CloseHandle (_StdoutPipeRead);
		::CloseHandle (_StdoutPipeWrite);
		::CloseHandle (_StderrPipeRead);
		::CloseHandle (_StderrPipeWrite);
		_StdoutPipeRead = 0;
		_StdoutPipeWrite = 0;
		_StderrPipeRead = 0;
		_StderrPipeWrite = 0;
		_StdoutPipeFp = 0;
		_StderrPipeFp = 0;
		return false;
	}

	_PipeInitialized = true;

	return true;
}

ATOM_KERNEL_API void ATOM_CALL ATOM_AddStandardOutputCallback (ATOM_StdOutputCallback *stdoutCallback, ATOM_StdOutputCallback *stderrCallback)
{
	if (!_PipeInitialized && !InitPipes ())
	{
		ATOM_LOGGER::error ("Init output pipes failed.\n");
		return;
	}

	{
		ATOM_Mutex::ScopeMutex sm(_StdoutCallbackLock);
		if (stdoutCallback && std::find(_Stdout_Callbacks.begin(), _Stdout_Callbacks.end(), stdoutCallback) == _Stdout_Callbacks.end())
		{
			_Stdout_Callbacks.push_back (stdoutCallback);

			if (_Stdout_Callbacks.size() == 1)
			{
				*stdout = *_StdoutPipeFp;
				setvbuf (stdout, 0, _IONBF, 0);
				::ResumeThread (_StdoutThreadHandle);
			}
		}
	}

	{
		ATOM_Mutex::ScopeMutex sm(_StderrCallbackLock);

		if (stderrCallback && std::find(_Stderr_Callbacks.begin(), _Stderr_Callbacks.end(), stderrCallback) == _Stderr_Callbacks.end())
		{
			_Stderr_Callbacks.push_back (stderrCallback);

			if (!_Stderr_Callbacks.size() == 1)
			{
				*stderr = *_StderrPipeFp;
				setvbuf (stderr, 0, _IONBF, 0);
				::ResumeThread (_StderrThreadHandle);
			}
		}
	}
}

ATOM_KERNEL_API void ATOM_CALL ATOM_RemoveStandardOutputCallback (ATOM_StdOutputCallback *stdoutCallback, ATOM_StdOutputCallback *stderrCallback)
{
	if (!_PipeInitialized && !InitPipes ())
	{
		ATOM_LOGGER::error ("Init output pipes failed.\n");
		return;
	}

	{
		ATOM_Mutex::ScopeMutex sm(_StdoutCallbackLock);

		ATOM_VECTOR<ATOM_StdOutputCallback *>::iterator it = std::find(_Stdout_Callbacks.begin(), _Stdout_Callbacks.end(), stdoutCallback);
		if (it != _Stdout_Callbacks.end ())
		{
			_Stdout_Callbacks.erase (it);

			if (_Stdout_Callbacks.empty ())
			{
				::SuspendThread (_StdoutThreadHandle);
				*stdout = _OldStdout;
			}
		}
	}

	{
		ATOM_Mutex::ScopeMutex sm(_StderrCallbackLock);

		ATOM_VECTOR<ATOM_StdOutputCallback *>::iterator it = std::find(_Stderr_Callbacks.begin(), _Stderr_Callbacks.end(), stderrCallback);
		if (it != _Stderr_Callbacks.end ())
		{
			_Stderr_Callbacks.erase (it);

			if (_Stderr_Callbacks.empty ())
			{
				::SuspendThread (_StderrThreadHandle);
				*stderr = _OldStderr;
			}
		}
	}
}

ATOM_KERNEL_API void ATOM_CALL ATOM_FlushStdOutputs (void)
{
	{
		ATOM_STRING outputString;

		{
			ATOM_Mutex::ScopeMutex sm(_StdoutStringLock);

			outputString = _StdoutString;
			_StdoutString.clear();
		}

		if (outputString.length() > 0)
		{
			ATOM_Mutex::ScopeMutex sm(_StdoutCallbackLock);

			for (unsigned i = 0; i < _Stdout_Callbacks.size(); ++i)
			{
				_Stdout_Callbacks[i]->OutputString (outputString.c_str());
			}
		}

		{
			ATOM_Mutex::ScopeMutex sm(_StderrStringLock);

			outputString = _StderrString;
			_StderrString.clear();
		}

		if (outputString.length() > 0)
		{
			ATOM_Mutex::ScopeMutex sm(_StderrCallbackLock);

			for (unsigned i = 0; i < _Stderr_Callbacks.size(); ++i)
			{
				_Stderr_Callbacks[i]->OutputString (outputString.c_str());
			}
		}
	}
}


