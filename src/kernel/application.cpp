#include "stdafx.h"
#include "application.h"
#include "swappable.h"

#define SHOW_FPS_ON_WINDOW 1

static ATOM_Application * _ApplicationInstance = 0;
static bool _showFPS = false;
static char errorMsg[8192];
static char errorFiles[2048];
LPTOP_LEVEL_EXCEPTION_FILTER _prevExceptionHandler = nullptr;
bool ATOM_Application::_enableErrorPopup = true;

ATOM_KERNEL_API ATOM_Application * ATOM_CALL ATOM_GetApplication (void)
{
  return _ApplicationInstance;
}

ATOM_BEGIN_EVENT_MAP(ATOM_Application, ATOM_EventDispatcher)
  ATOM_EVENT_HANDLER(ATOM_Application, ATOM_AppInitEvent, onAppInit)
  ATOM_EVENT_HANDLER(ATOM_Application, ATOM_AppExitEvent, onAppExit)
  ATOM_EVENT_HANDLER(ATOM_Application, ATOM_AppQuitEvent, onAppQuit)
  ATOM_EVENT_HANDLER(ATOM_Application, ATOM_AppFatalEvent, onAppFatal)
ATOM_END_EVENT_MAP

static DWORD AppThreadId = 0;

static void logSystemStatus (void)
{
	{
		// System memory status
		PROCESS_MEMORY_COUNTERS pmc;
		DWORD nId = GetCurrentProcessId();
		HANDLE hProcess = ::OpenProcess (PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, nId);	
		BOOL bRet = ::GetProcessMemoryInfo (hProcess, &pmc, sizeof(pmc));
		::CloseHandle (hProcess);
		ATOM_LOGGER::log("-----------------------------------------------------------------");
		ATOM_LOGGER::log("PROCESS_MEMORY_COUNTERS:cb = %d\n", pmc.cb);
		ATOM_LOGGER::log("PROCESS_MEMORY_COUNTERS:PageFaultCount = %d\n", pmc.PageFaultCount);
		ATOM_LOGGER::log("PROCESS_MEMORY_COUNTERS:PeakWorkingSetSize = %d\n", pmc.PeakWorkingSetSize);
		ATOM_LOGGER::log("PROCESS_MEMORY_COUNTERS:WorkingSetSize = %d\n", pmc.WorkingSetSize);
		ATOM_LOGGER::log("PROCESS_MEMORY_COUNTERS:QuotaPeakPagedPoolUsage = %d\n", pmc.QuotaPeakPagedPoolUsage);
		ATOM_LOGGER::log("PROCESS_MEMORY_COUNTERS:QuotaPagedPoolUsage = %d\n", pmc.QuotaPagedPoolUsage);
		ATOM_LOGGER::log("PROCESS_MEMORY_COUNTERS:QuotaPeakNonPagedPoolUsage = %d\n", pmc.QuotaPeakNonPagedPoolUsage);
		ATOM_LOGGER::log("PROCESS_MEMORY_COUNTERS:QuotaNonPagedPoolUsage = %d\n", pmc.QuotaNonPagedPoolUsage);
		ATOM_LOGGER::log("PROCESS_MEMORY_COUNTERS:PagefileUsage = %d\n", pmc.PagefileUsage);
		ATOM_LOGGER::log("PROCESS_MEMORY_COUNTERS:PeakPagefileUsage = %d\n", pmc.PeakPagefileUsage);
		ATOM_LOGGER::log("-----------------------------------------------------------------");
	}

	{
		// System info
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
		ATOM_LOGGER::log("-----------------------------------------------------------------");
		ATOM_LOGGER::log("SYSTEM_INFO:dwPageSize = %d\n", sysInfo.dwPageSize);
		ATOM_LOGGER::log("SYSTEM_INFO:lpMinimumApplicationAddress = %d\n", sysInfo.lpMinimumApplicationAddress);
		ATOM_LOGGER::log("SYSTEM_INFO:lpMaximumApplicationAddress = %d\n", sysInfo.lpMaximumApplicationAddress);
		ATOM_LOGGER::log("SYSTEM_INFO:dwActiveProcessorMask = %d\n", sysInfo.dwActiveProcessorMask);
		ATOM_LOGGER::log("SYSTEM_INFO:dwNumberOfProcessors = %d\n", sysInfo.dwNumberOfProcessors);
		ATOM_LOGGER::log("SYSTEM_INFO:dwProcessorType = %d\n", sysInfo.dwProcessorType);
		ATOM_LOGGER::log("SYSTEM_INFO:dwAllocationGranularity = %d\n", sysInfo.dwAllocationGranularity);
		ATOM_LOGGER::log("SYSTEM_INFO:wProcessorLevel = %d\n", sysInfo.wProcessorLevel);
		ATOM_LOGGER::log("SYSTEM_INFO:wProcessorRevision = %d\n", sysInfo.wProcessorRevision);
		ATOM_LOGGER::log("-----------------------------------------------------------------");
	}

	{
		HANDLE hProcess = NULL;  
		PROCESSENTRY32 pe32 = {0};  
		hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);  
		pe32.dwSize=sizeof(PROCESSENTRY32);  
		ATOM_LOGGER::log("-----------------------------------------------------------------");
		if(Process32First(hProcess,&pe32))  
		{
			do
			{
				if(pe32.szExeFile[0])
				{
					ATOM_LOGGER::log("Process:%s\n", pe32.szExeFile);
				}
			}
			while(Process32Next(hProcess, &pe32));  
		}
		ATOM_LOGGER::log("-----------------------------------------------------------------");
	}
}

static PWCHAR*
cmdlineToArgvW(
    PWCHAR CmdLine,
    int* _argc
    )
{
    PWCHAR* argv;
    PWCHAR  _argv;
    ULONG   len;
    ULONG   argc;
    WCHAR   a;
    ULONG   i, j;

    BOOLEAN  in_QM;
    BOOLEAN  in_TEXT;
    BOOLEAN  in_SPACE;

    len = wcslen(CmdLine);
    i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);

    argv = (PWCHAR*)GlobalAlloc(GMEM_FIXED,
        i + (len+2)*sizeof(WCHAR));

    _argv = (PWCHAR)(((PUCHAR)argv)+i);

    argc = 0;
    argv[argc] = _argv;
    in_QM = FALSE;
    in_TEXT = FALSE;
    in_SPACE = TRUE;
    i = 0;
    j = 0;

    while( a = CmdLine[i] ) {
        if(in_QM) {
            if(a == '\"') {
                in_QM = FALSE;
            } else {
                _argv[j] = a;
                j++;
            }
        } else {
            switch(a) {
            case '\"':
                in_QM = TRUE;
                in_TEXT = TRUE;
                if(in_SPACE) {
                    argv[argc] = _argv+j;
                    argc++;
                }
                in_SPACE = FALSE;
                break;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                if(in_TEXT) {
                    _argv[j] = '\0';
                    j++;
                }
                in_TEXT = FALSE;
                in_SPACE = TRUE;
                break;
            default:
                in_TEXT = TRUE;
                if(in_SPACE) {
                    argv[argc] = _argv+j;
                    argc++;
                }
                _argv[j] = a;
                j++;
                in_SPACE = FALSE;
                break;
            }
        }
        i++;
    }
    _argv[j] = '\0';
    argv[argc] = NULL;

    (*_argc) = argc;
    return argv;
}

PCHAR*
cmdlineToArgvA(
    PCHAR CmdLine,
    int* _argc
    )
{
    PCHAR* argv;
    PCHAR  _argv;
    ULONG   len;
    ULONG   argc;
    CHAR   a;
    ULONG   i, j;

    BOOLEAN  in_QM;
    BOOLEAN  in_TEXT;
    BOOLEAN  in_SPACE;

    len = strlen(CmdLine);
    i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);

    argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
        i + (len+2)*sizeof(CHAR));

    _argv = (PCHAR)(((PUCHAR)argv)+i);

    argc = 0;
    argv[argc] = _argv;
    in_QM = FALSE;
    in_TEXT = FALSE;
    in_SPACE = TRUE;
    i = 0;
    j = 0;

    while( a = CmdLine[i] ) {
        if(in_QM) {
            if(a == '\"') {
                in_QM = FALSE;
            } else {
                _argv[j] = a;
                j++;
            }
        } else {
            switch(a) {
            case '\"':
                in_QM = TRUE;
                in_TEXT = TRUE;
                if(in_SPACE) {
                    argv[argc] = _argv+j;
                    argc++;
                }
                in_SPACE = FALSE;
                break;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                if(in_TEXT) {
                    _argv[j] = '\0';
                    j++;
                }
                in_TEXT = FALSE;
                in_SPACE = TRUE;
                break;
            default:
                in_TEXT = TRUE;
                if(in_SPACE) {
                    argv[argc] = _argv+j;
                    argc++;
                }
                _argv[j] = a;
                j++;
                in_SPACE = FALSE;
                break;
            }
        }
        i++;
    }
    _argv[j] = '\0';
    argv[argc] = NULL;

    (*_argc) = argc;
    return argv;
}

ATOM_Application::ATOM_Application (void)
{
	ATOM_STACK_TRACE(ATOM_Application::ATOM_Application);

	ATOM_ASSERT(_ApplicationInstance == 0);
	_ApplicationInstance = this;
	AppThreadId = 0;

	_returnCode = 0;
	_frameInterval = 1;
	_quitRequest = true;
	_timerChanged = false;
	_timerHandle = NULL;
	_eventHandle = NULL;
	_mainWindow = NULL;
	_dmpFileName = "crash.dmp";
}

ATOM_Application::~ATOM_Application (void)
{
  ATOM_STACK_TRACE(ATOM_Application::~ATOM_Application);

  ATOM_ASSERT(_ApplicationInstance == this);

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

  _ApplicationInstance = 0;
}

void ATOM_Application::enableErrorPopup (bool enable)
{
	_enableErrorPopup = enable;
}

bool ATOM_Application::isErrorPopupEnabled (void)
{
	return _enableErrorPopup;
}

bool ATOM_Application::pumpEvents (int *retVal)
{
	ATOM_STACK_TRACE(ATOM_Application::pumpEvents);
	static unsigned FPStick = 0;
	static char buffer1[256];
	static char buffer2[256];

	if (_quitRequest)
	{
		return true;
	}

	if (_frameInterval)
	{
		HANDLE h[2] = { _timerHandle, _eventHandle };
		DWORD result = ::MsgWaitForMultipleObjects (2, h, FALSE, INFINITE, QS_ALLINPUT);

		switch (result - WAIT_OBJECT_0)
		{
		case 0:
		case 1:
			{
				if (!dispatchPendingEvents ())
				{
					_frameStamp.update ();

					if (_showFPS)
					{
						HWND hWnd = getMainWindow();
						if (hWnd && _frameStamp.currentTick > FPStick + 1000)
						{
							::GetWindowTextA (hWnd, buffer1, 256);
							char *p = strstr (buffer1, " #");
							if (p)
							{
								*p = '\0';
							}
							sprintf (buffer2, "%s # FPS:%d", buffer1, _frameStamp.FPS);
							::SetWindowTextA (hWnd, buffer2);
							FPStick = _frameStamp.currentTick;
						}
					}

					ATOM_AppIdleEvent event;
					handleEvent (&event);
				}
				else
				{
					if (retVal) *retVal = _returnCode;
				}
				break;
			}
		case 2:
			{
				MSG msg;
				while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
				{
					if ( msg.message != WM_QUIT)
					{
						TranslateMessage(&msg);
						DispatchMessageW(&msg);
					}
					else
					{
						queueEvent (ATOM_NEW(ATOM_AppQuitEvent, msg.wParam), this);
						break;
					}
				}
				break;
			}
		default:
			break;
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
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if ( msg.message != WM_QUIT)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				queueEvent (ATOM_NEW(ATOM_AppQuitEvent, msg.wParam), this);
				break;
			}
		}

		if (!dispatchPendingEvents ())
		{
			updateFrameStamp ();

			if (_showFPS)
			{
				HWND hWnd = getMainWindow();
				if (hWnd && _frameStamp.currentTick > FPStick + 1000)
				{
					::GetWindowTextA (hWnd, buffer1, 256);
					char *p = strstr (buffer1, " #");
					if (p)
					{
						*p = '\0';
					}
					sprintf (buffer2, "%s # FPS:%d", buffer1, _frameStamp.FPS);
					::SetWindowTextA (hWnd, buffer2);
					FPStick = _frameStamp.currentTick;
				}
			}

			ATOM_AppIdleEvent event;
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

bool ATOM_Application::pumpWMEvents (void)
{
	MSG msg;
	while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if ( msg.message != WM_QUIT)
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else
		{
			return true;
		}
	}
	return false;
}

void ATOM_Application::updateFrameStamp (void)
{
	_frameStamp.update ();
}

BOOL WINAPI ConsoleHandler (DWORD event)
{
	if (event == CTRL_CLOSE_EVENT)
	{
#if 1
		::TerminateProcess (::GetCurrentProcess(), 2);
#else
		if (ATOM_APP)
		{
			ATOM_APP->postQuitEvent (2);

			while (!ATOM_APP->pumpEvents (0))
				;

			ATOM_AppExitEvent event;
			ATOM_APP->handleEvent (&event);

			::ExitProcess (2);
		}
#endif
	}

	return TRUE;
}

class MyMiniDumpUserStream: public ATOM_MiniDumpUserStreams
{
	UserStreamInfo _info;
public:
	MyMiniDumpUserStream (const char *msg)
	{
		_info.type = 0;
		_info.buffersize = strlen(msg) + 1;
		_info.buffer = (void*)msg;
	}
public:
	virtual unsigned GetNumStreams (void) const { return 1; }
	virtual void GetStream (unsigned index, UserStreamInfo *streaminfo)
	{
		*streaminfo = _info;
	}
};

static void createErrorFiles (const char *msg, PEXCEPTION_POINTERS p, const char *dmpFileName)
{
	char dmpFile[MAX_PATH];
	char logFile[MAX_PATH];
	::GetTempPathA (sizeof(dmpFile), dmpFile);
	strcat (dmpFile, dmpFileName);

	MyMiniDumpUserStream myStreams(msg);
	ATOM_WriteMiniDump (dmpFile, &myStreams, p);

	const char *logFileName = ATOM_LOGGER::getLogFileName();
	::GetFullPathNameA (logFileName, MAX_PATH, logFile, NULL);
	char *s = errorFiles;
	strcpy (s, "b");
	strcat (s, dmpFile);
	s += strlen(s) + 1;
	strcpy (s, "t");
	strcat (s, logFile);
	s += strlen(s) + 1;
	*s = '\0';

	ATOM_LOGGER::flush ();
	ATOM_LOGGER::finalize ();
}

void ATOM_Application::ErrorHandler::_reportError (PEXCEPTION_POINTERS p, ATOM_AppErrorHandler::ErrorType errorType)
{
	if (_ApplicationInstance)
	{
		_ApplicationInstance->onReportError (errorType);

		switch (errorType)
		{
		case ATOM_AppErrorHandler::ET_WIN32_EXCEPTION:
			{
				char msg[2048];
				BOOL ret = ::FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_HMODULE, 
					::LoadLibraryA("NTDLL.dll"), p->ExceptionRecord->ExceptionCode, 0, msg, 2048, NULL);
				sprintf (errorMsg, "WIN32 exception (0x%08X): \r\n%s\r\n%s\r\n", p->ExceptionRecord->ExceptionCode, (ret ? msg : "Unknown exception"), ATOM_GetStackTraceString());
				break;
			}
		case ATOM_AppErrorHandler::ET_CPP_EXCEPTION:
			{
				strcpy(errorMsg, "C++ exception");
				break;
			}
		case ATOM_AppErrorHandler::ET_TERMINATION:
			{
				strcpy(errorMsg, "Abnormal termination");
				break;
			}
		case ATOM_AppErrorHandler::ET_UNEXPECTED:
			{
				strcpy(errorMsg, "C++ unexpected exception");
				break;
			}
		case ATOM_AppErrorHandler::ET_PURECALL:
			{
				strcpy (errorMsg, "Pure virtual function call");
				break;
			}
		case ATOM_AppErrorHandler::ET_INVALIDPARAM:
			{
				strcpy (errorMsg, "CRT function invalid parameter error");
				break;
			}
		case ATOM_AppErrorHandler::ET_NEW:
			{
				strcpy (errorMsg, "CRT new operator failed");
				break;
			}
		case ATOM_AppErrorHandler::ET_SIGABRT:
			{
				strcpy(errorMsg, "SIGABRT");
				break;
			}
		case ATOM_AppErrorHandler::ET_SIGFPE:
			{
				strcpy(errorMsg, "SIGFPE");
				break;
			}
		case ATOM_AppErrorHandler::ET_SIGILL:
			{
				strcpy(errorMsg, "SIGILL");
				break;
			}
		case ATOM_AppErrorHandler::ET_SIGINT:
			{
				strcpy(errorMsg, "SIGINT");
				break;
			}
		case ATOM_AppErrorHandler::ET_SIGSEGV:
			{
				strcpy(errorMsg, "SIGSEGV");
				break;
			}
		case ATOM_AppErrorHandler::ET_SIGTERM:
			{
				strcpy(errorMsg, "SIGTERM");
				break;
			}
		}

		ATOM_LOGGER::fatal (errorMsg);
		logSystemStatus ();
		createErrorFiles (errorMsg, p, _ApplicationInstance->getDefaultDumpFileName());
		ATOM_SendErrorFiles (errorFiles);

		if (ATOM_Application::isErrorPopupEnabled ())
		{
			ATOM_VERIFY_EX(0, 0, ATOM_ASSERT_ALL_MASK, "³ÌÐò´íÎó!!", errorFiles)(errorMsg);
		}
	}
}

int myhandler(int code, PEXCEPTION_POINTERS p)
{
	ATOM_ERRHANDLER->reportError (p, ATOM_AppErrorHandler::ET_WIN32_EXCEPTION);
	return EXCEPTION_CONTINUE_SEARCH;
}

void ATOM_Application::setDefaultDumpFileName (const char *dmpFileName)
{
	_dmpFileName = dmpFileName ? dmpFileName : "crash.dmp";
}

const char *ATOM_Application::getDefaultDumpFileName (void) const
{
	return _dmpFileName.c_str();
}

void ATOM_Application::eventLoop (void)
{
	__try
	{
		_eventloop ();
	}
	__except(myhandler(_exception_code() , (PEXCEPTION_POINTERS)_exception_info()))
	{
	}
}

void ATOM_Application::_eventloop (void)
{
#if ATOM3D_COMPILER_MSVC
	try
	{
#endif
		while (!pumpEvents (0))
		{
			static unsigned lastCheckTime = 0;

			if (lastCheckTime == 0)
			{
				lastCheckTime = _frameStamp.currentTick;
			}
#if 0
			if (_frameStamp.currentTick > lastCheckTime + 500)
			{
				ATOM_SwappableManager::checkAndSwap (_frameStamp.currentTick);
				lastCheckTime = _frameStamp.currentTick;
			}
#endif

		}
#if ATOM3D_COMPILER_MSVC
	}
	catch (std::exception &e)
	{
		ATOM_ERRHANDLER->reportError (0, ATOM_AppErrorHandler::ET_CPP_EXCEPTION);
		throw;
	}
	catch (...)
	{
		ATOM_ERRHANDLER->reportError (0, ATOM_AppErrorHandler::ET_UNKNOWN);
		throw;
	}
#endif
}

int ATOM_Application::run (void)
{
	ATOM_STACK_TRACE(ATOM_Application::run);
	AppThreadId = ::GetCurrentThreadId ();

	_CrtSetReportMode (_CRT_ASSERT, 0);
	::SetConsoleCtrlHandler (&ConsoleHandler, TRUE);
	::SetConsoleCtrlHandler (NULL, TRUE);

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

	ATOM_ERRHANDLER->setProcessErrorHandlers ();
	ATOM_ERRHANDLER->setThreadErrorHandlers ();

	{
		ATOM_AppInitEvent event;
		event.argv = cmdlineToArgvA (::GetCommandLineA (), &event.argc);
		for (int i = 0; i < event.argc; ++i)
		{
			if (!stricmp(event.argv[i], "--show-fps"))
			{
				_showFPS = true;
			}
		}

		handleEvent (&event);
		::GlobalFree (event.argv);
		if (!event.success)
		{
			return event.errorcode;
		}
	}

	eventLoop ();
	
	{
		ATOM_AppExitEvent event;
		handleEvent (&event);
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

	return _returnCode;
}

void ATOM_Application::setTimeScale (float timescale)
{
	_frameStamp.reset (timescale);
}

float ATOM_Application::getTimeScale (void) const
{
	return _frameStamp.timeScale;
}

void ATOM_Application::setFrameInterval (int ms)
{
  if (ms != _frameInterval)
  {
    _frameInterval = ms;
    _timerChanged = true;
  }
}

int ATOM_Application::getFrameInterval (void)
{
  return _frameInterval;
}

void ATOM_Application::onAppInit (ATOM_AppInitEvent *event)
{
	event->success = true;
}

void ATOM_Application::onAppExit (ATOM_AppExitEvent *event)
{
}

void ATOM_Application::onAppQuit (ATOM_AppQuitEvent *event)
{
	_returnCode = event->returnValue;
	_quitRequest = true;
}

void ATOM_Application::onAppFatal (ATOM_AppFatalEvent *event)
{
	ATOM_STACK_TRACE(ATOM_Application::onAppFatal);

	if (!event->msg.empty ())
	{
		ATOM_LOGGER::fatal ("%s\n", event->msg.c_str());

		if (event->showmsg)
		{
			HWND hWnd = _mainWindow ? _mainWindow : ::GetActiveWindow ();
			::MessageBoxA (hWnd, event->msg.c_str(), "´íÎó", MB_OK|MB_ICONHAND);
		}
	}

	if (event->notify)
	{
		ATOM_AppExitEvent exitEvent;
		handleEvent (&exitEvent);
	}

	::ExitProcess (255);
}

const ATOM_FrameStamp &ATOM_Application::getFrameStamp (void)
{
	return _frameStamp;
}

void ATOM_Application::onPushEvent (void)
{
	if (_eventHandle)
	{
		::SetEvent (_eventHandle);
	}
}

void ATOM_Application::onReportError (ATOM_AppErrorHandler::ErrorType errorType)
{
}

void ATOM_Application::postQuitEvent (int ret)
{
	queueEvent (ATOM_NEW(ATOM_AppQuitEvent, ret), this);
}

void ATOM_Application::setMainWindow (HWND hWnd)
{
	_mainWindow = hWnd;
}

HWND ATOM_Application::getMainWindow (void) const
{
	return _mainWindow;
}

bool ATOM_Application::isAppThread (void)
{
	return ::GetCurrentThreadId() == AppThreadId;
}

ATOM_AsyncTask::ATOM_AsyncTask (void)
{
	_canceled = false;
}

void ATOM_AsyncTask::lock (void)
{
	_lock.lock ();
}

void ATOM_AsyncTask::unlock (void)
{
	_lock.unlock ();
}

void ATOM_AsyncTask::cancel (void)
{
	lock ();
	_canceled = true;
	unlock ();
}

bool ATOM_AsyncTask::isCanceled (void) const
{
	return _canceled;
}



