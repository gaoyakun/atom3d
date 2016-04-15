#include "StdAfx.h"
#include "basedefs.h"
#include "rtc.h"
#include "callstack.h"
#include "assertion.h"
#include "logger.h"

extern HWND _ParentWindow;

ATOM_RTCErrorHandler _DefaultRTCErrorHandler;
ATOM_RTCErrorHandler *_GlobalRTCErrorHandler = 0;

ATOM_CRTErrorHandler _DefaultCRTErrorHandler;
ATOM_CRTErrorHandler *_GlobalCRTErrorHandler = 0;

#if ATOM3D_COMPILER_MSVC

bool GetVSJITDebuggerName (char *buffer)
{
  HKEY jitkey;
  LONG result = ::RegOpenKeyEx (HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\AeDebug\\", 0, KEY_QUERY_VALUE, &jitkey);
  if (result == ERROR_SUCCESS)
  {
    char path[260] = { '\0' };
    DWORD lpT = REG_SZ;
    DWORD lpS = 260;
    result = RegQueryValueEx (jitkey, "Debugger", NULL, &lpT, (LPBYTE)&path[0], &lpS);
    if (result == ERROR_SUCCESS)
    {
      _strlwr (path);
      if (strstr(path, "vsjitdebugger") || strstr(path, "vs7jit"))
      {
        strcpy (buffer, path);
        ::RegCloseKey (jitkey);
        return true;
      }
    }
  }

  ::RegCloseKey (jitkey);
  return false;
}

int ATOM_RTCErrorHandler::report (int /* err */, const char *filename, int lineno, const char * /* modulename */, const char *desc)
{
	ATOM_CallStack callstack;
	return ATOM_ShowCallstackDialog (_ParentWindow ? _ParentWindow : ::GetActiveWindow(), "运行时检查失败!!", filename, lineno, &callstack, desc, 6);
}

#if _MSC_VER > 1200

ATOM_DBGHLP_API int ATOM_MyRTCErrorFunc (int err, const char *filename, int lineno, const char *modulename, const char *format, ...)
{
  static const size_t BufferSize = 1024;
  va_list args;
  va_start (args, format);
  char buffer[BufferSize];
  _vsnprintf (buffer, BufferSize, format, args);
  va_end (args);
  buffer[BufferSize-1] = '\0';

  if (!_GlobalRTCErrorHandler)
    _GlobalRTCErrorHandler = &_DefaultRTCErrorHandler;

  switch (_GlobalRTCErrorHandler->report (err, filename, lineno, modulename, buffer))
  {
  case ATOM_ASSERT_CONTINUE:
    return 0;
    break;
  case ATOM_ASSERT_BREAK:
    {
      char vsjit[260];
      if (!IsDebuggerPresent() && GetVSJITDebuggerName (vsjit))
      {
        HANDLE event = ::CreateEvent (NULL, FALSE, FALSE, NULL);
        char cmdline[2048];
        sprintf (cmdline, vsjit, ::GetCurrentProcessId(), event);

        STARTUPINFO startupinfo;
        memset (&startupinfo, 0, sizeof(startupinfo));
        startupinfo.cb = sizeof(startupinfo);
        startupinfo.dwFlags = STARTF_USESHOWWINDOW;
        startupinfo.wShowWindow = SW_SHOW;

        PROCESS_INFORMATION processinfo;
        if (::CreateProcess (NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &startupinfo, &processinfo))
        {
          for (;;)
          {
            DWORD n = ::WaitForSingleObject (processinfo.hProcess, 0);
            if (n == WAIT_TIMEOUT)
            {
              if (::IsDebuggerPresent ())
              {
                ::DebugBreak ();
              }
              continue;
            }
            else if (n == WAIT_OBJECT_0)
            {
              CloseHandle (processinfo.hThread);
              CloseHandle (processinfo.hProcess);
              return 1;
            }
          }
        }
      }
      else
      {
        ::DebugBreak ();
      }
      break;
    }
  case ATOM_ASSERT_TERMINATE:
  default:
    ::ExitProcess(1);
    break;
  }
  return 0;
}

#else

ATOM_DBGHLP_API int ATOM_MyRTCErrorFunc (int err, const char *filename, int lineno, const char *modulename, const char *format, ...)
{
    return 0;
}

#endif

ATOM_DBGHLP_API ATOM_RTCErrorHandler *ATOM_SetRTCErrorHandler (ATOM_RTCErrorHandler *handler)
{
  ATOM_RTCErrorHandler *oldHandler = _GlobalRTCErrorHandler;
  _GlobalRTCErrorHandler = handler;
  return oldHandler ? oldHandler : &_DefaultRTCErrorHandler;
}

int ATOM_CRTErrorHandler::report (int err, const char *msg)
{
	ATOM_CallStack callstack;
	return ATOM_ShowCallstackDialog (_ParentWindow ? _ParentWindow : ::GetActiveWindow(), "运行时检查失败!!", NULL, 0, &callstack, msg, 6);
}

ATOM_DBGHLP_API int ATOM_MyCRTErrorFunc (int err, char *msg, int *ret)
{
  if (!_GlobalCRTErrorHandler)
    _GlobalCRTErrorHandler = &_DefaultCRTErrorHandler;

  switch (_GlobalCRTErrorHandler->report (err, msg))
  {
  case ATOM_ASSERT_CONTINUE:
	return 0;
    break;
  case ATOM_ASSERT_BREAK:
    {
	  ::DebugBreak();
      break;
    }
  case ATOM_ASSERT_TERMINATE:
  default:
    ::ExitProcess(1);
    break;
  }
  return 0;
}

#else

ATOM_DBGHLP_API int ATOM_MyRTCErrorFunc (int err, const char *filename, int lineno, const char *modulename, const char *format, ...)
{
    return 0;
}

ATOM_DBGHLP_API int ATOM_MyCRTErrorFunc (int err, char *msg, int *ret)
{
  return 0;
}

int ATOM_RTCErrorHandler::report (int err, const char *filename, int line, const char *modulename, const char *desc)
{
	ATOM_LOGGER::error ("RTC error: %s\n", desc);
	return 0;
}

int ATOM_CRTErrorHandler::report (int err, const char *msg)
{
	ATOM_LOGGER::error ("CRT error: %s\n", msg);
	return 0;
}

#endif

ATOM_DBGHLP_API ATOM_CRTErrorHandler *ATOM_SetCRTErrorHandler (ATOM_CRTErrorHandler *handler)
{
  ATOM_CRTErrorHandler *oldHandler = _GlobalCRTErrorHandler;
  _GlobalCRTErrorHandler = handler;
  return oldHandler ? oldHandler : &_DefaultCRTErrorHandler;
}

