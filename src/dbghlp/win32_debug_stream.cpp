#include "StdAfx.h"
#include "win32_debug_stream.h"
#include "logger.h"

struct DebugStringBuffer
{
	DWORD pid;
	char msg[1];
};

static HANDLE dbwin_buffer_ready = NULL;
static HANDLE dbwin_data_ready = NULL;
static HANDLE dbwin_buffer = NULL;
static DebugStringBuffer *shared_file = NULL;
static HANDLE thread_quit = NULL;
static DWORD pid = ::GetCurrentProcessId ();

#if ATOM3D_PLATFORM_MINGW
static long monitering = 0;
#else
static volatile long monitering = 0;
#endif

static DWORD WINAPI Monitor(LPVOID param)
{
#if ATOM3D_COMPILER_MSVC
	__try
	{
#endif
		while (monitering)
		{
			BOOL result = ::SetEvent (dbwin_buffer_ready);
			
			if (::WaitForSingleObject (dbwin_data_ready, 100) == WAIT_OBJECT_0)
			{
				if (shared_file->pid == pid)
				{
					ATOM_LOGGER::log2 (ATOM_LOGGER::LVL_NONE, shared_file->msg);
				}
			}
		}
#if ATOM3D_COMPILER_MSVC
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
#endif

	::SetEvent (thread_quit);

	return 0;
}

static void deinitialize (void)
{
	if (dbwin_buffer_ready)
	{
		::CloseHandle (dbwin_buffer_ready);
		dbwin_buffer_ready = NULL;
	}

	if (dbwin_data_ready)
	{
		::CloseHandle (dbwin_data_ready);
		dbwin_data_ready = NULL;
	}

	if (shared_file)
	{
		::UnmapViewOfFile (shared_file);
		shared_file = NULL;
	}

	if (dbwin_buffer)
	{
		::CloseHandle (dbwin_buffer);
		dbwin_buffer = NULL;
	}
}

static bool initialize (void)
{
	dbwin_buffer_ready = ::CreateEventA (NULL, FALSE, FALSE, "DBWIN_BUFFER_READY");
	if (!dbwin_buffer_ready)
	{
		deinitialize ();
		return false;
	}

	dbwin_data_ready = ::CreateEventA (NULL, FALSE, FALSE, "DBWIN_DATA_READY");
	if (!dbwin_data_ready)
	{
		deinitialize ();
		return false;
	}

	dbwin_buffer = ::CreateFileMappingA ((HANDLE)-1, NULL, PAGE_READWRITE, 0, 4096, "DBWIN_BUFFER");
	if (!dbwin_buffer)
	{
		deinitialize ();
		return false;
	}

	shared_file = (DebugStringBuffer*)::MapViewOfFile (dbwin_buffer, FILE_MAP_READ, 0, 0, 1024);
	if (!shared_file)
	{
		deinitialize ();
		return false;
	}

	thread_quit = ::CreateEventA (NULL, FALSE, FALSE, NULL);
	if (!thread_quit)
	{
		deinitialize ();
		return false;
	}

	return true;
}

void ATOM_Win32DebugStream::startMonitering (void)
{
	if (::IsDebuggerPresent ())
	{
		return;
	}

	HINSTANCE hInst=LoadLibrary("Kernel32.DLL");
	typedef BOOL(WINAPI * PFNQueueUserWorkItem)(LPTHREAD_START_ROUTINE, PVOID,ULONG);
	PFNQueueUserWorkItem pfnQueueUserWorkItem=NULL;

	//取得QueueUserWorkItem函数指针
	pfnQueueUserWorkItem=(PFNQueueUserWorkItem)::GetProcAddress(hInst,"QueueUserWorkItem");
	if(!pfnQueueUserWorkItem)
	{
		return;
	}

	if (!monitering && initialize ())
	{
		monitering = 1;
		pfnQueueUserWorkItem (&Monitor, 0, 0);
	}
}

void ATOM_Win32DebugStream::stopMonitering (void)
{
	if (monitering)
	{
		::InterlockedExchange (&monitering, 0);
		::WaitForSingleObject (thread_quit, INFINITE);
		deinitialize ();
	}
}

bool ATOM_Win32DebugStream::isMonitoring (void)
{
	return monitering != 0;
}

