#include "StdAfx.h"
#include "assertion.h"
#include "debugger.h"

long ATOM_Debugger::_S_quit = 0;

ATOM_Debugger::ATOM_Debugger (const char *cmdline, unsigned latency)
{
  ATOM_ASSERT(latency != INFINITE);
  ATOM_ASSERT(cmdline && cmdline[0]);

  _M_cmd_line = cmdline;
  _M_latency = latency;
  _M_first_breakpoint = true;
  _M_event_process_id = 0;
  _M_event_thread_id = 0;
  _M_latency = 0;
  _M_first_breakpoint = false;
  memset (&_M_process_information, 0, sizeof(_M_process_information));
}

ATOM_Debugger::~ATOM_Debugger (void)
{
  // Must emplement
}

#define MAKEPTR(cast,ptr,addValue) (cast)((DWORD)(ptr)+(DWORD)(addValue))

bool ATOM_Debugger::getModuleHeader (const void *module, PIMAGE_NT_HEADERS hdr)
{
  IMAGE_DOS_HEADER dosHdr;

  return  readProcessMemoryEx (module, &dosHdr, sizeof(dosHdr)) && 
          readProcessMemoryEx (MAKEPTR(PVOID,module,dosHdr.e_lfanew), hdr, sizeof(*hdr));
}

bool ATOM_Debugger::getMoudleNameInProcess (HMODULE hModule, char *name, unsigned size)
{
  ATOM_ASSERT (name);
  ATOM_ASSERT (size > 0);

  IMAGE_NT_HEADERS ntHdr;
  IMAGE_EXPORT_DIRECTORY exportDir;
  DWORD exportsRVA;

  *name = '\0';

  if (!getModuleHeader (hModule, &ntHdr))
  {
    return false;
  }

  exportsRVA = ntHdr.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

  if (!exportsRVA)
  {
      return false;
  }

  return readProcessMemoryEx(MAKEPTR(PVOID, hModule, exportsRVA), &exportDir, sizeof(exportDir)) && 
          readProcessMemoryEx(MAKEPTR(PVOID, hModule, exportDir.Name), name, size);
}

void *ATOM_Debugger::getPreferredLoadAddress (const void *module)
{
  IMAGE_NT_HEADERS ntHdr;

  if (!getModuleHeader (module, &ntHdr))
  {
    return false;
  }

  return (LPVOID)ntHdr.OptionalHeader.ImageBase;
}

bool ATOM_Debugger::readProcessMemoryEx (const void *baseAddress, void *buffer, unsigned size)
{
  ATOM_ASSERT (_M_process_information.hProcess != NULL);
  if (_M_process_information.hProcess == NULL)
  {
    return false;
  }

  SIZE_T bytesRead;
  if (!::ReadProcessMemory (_M_process_information.hProcess, baseAddress, buffer, size, &bytesRead))
    return false;

  return bytesRead == size;
}

typedef DWORD (WINAPI *PFNGETMODULEFILENAMEEXW)(HANDLE, HMODULE, LPWSTR, DWORD);

bool ATOM_Debugger::getFullModuleName (HMODULE hModule, std::string &fullpathname)
{
  fullpathname = "";

  HMODULE hPSAPI = ::LoadLibrary ("PSAPI.DLL");
  if (hPSAPI)
  {
    PFNGETMODULEFILENAMEEXW pfnGetModuleFileNameExW = (PFNGETMODULEFILENAMEEXW)::GetProcAddress(hPSAPI,"GetModuleFileNameExW");
    if (pfnGetModuleFileNameExW)
    {
      wchar_t wFilename[MAX_PATH];
      DWORD dwLength = pfnGetModuleFileNameExW(getDebuggeeHandle(), hModule, wFilename, MAX_PATH);
      if (dwLength > 0)
      {
        char aFilename[MAX_PATH];
        ::WideCharToMultiByte (CP_ACP, 0, wFilename, dwLength, aFilename, MAX_PATH, NULL, NULL);
        fullpathname = aFilename;
      }
    }
    ::FreeLibrary (hPSAPI);
  }

  return !fullpathname.empty();
}

bool ATOM_Debugger::getExceptionDescription (unsigned exceptionCode, std::string &desc)
{
  bool found = true;

  switch (exceptionCode)
  {
  case EXCEPTION_ACCESS_VIOLATION:
    desc = "Access Violation";
    break;
  case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
    desc = "Array bounds exceeded";
    break;
  case EXCEPTION_BREAKPOINT: 
    desc = "Breakpoint";
    break;
  case EXCEPTION_DATATYPE_MISALIGNMENT: 
    desc = "Datatype misalignment";
    break;
  case EXCEPTION_FLT_DENORMAL_OPERAND:
    desc = "Floating-Point enormal operand";
    break;
  case EXCEPTION_FLT_DIVIDE_BY_ZERO:
    desc = "Floating-Point divide by zero";
    break;
  case EXCEPTION_FLT_INEXACT_RESULT:
    desc = "Floating-Point inexact result";
    break;
  case EXCEPTION_FLT_INVALID_OPERATION:
    desc = "Floating-Point invalid operation";
    break;
  case EXCEPTION_FLT_OVERFLOW:
    desc = "Floating-Point overflow";
    break;
  case EXCEPTION_FLT_STACK_CHECK:
    desc = "Floating-Point stack check";
    break;
  case EXCEPTION_FLT_UNDERFLOW:
    desc = "Floating-Point underflow";
    break;
  case EXCEPTION_ILLEGAL_INSTRUCTION:
    desc = "Illegal instruction";
    break;
  case EXCEPTION_IN_PAGE_ERROR:
    desc = "Integer page error";
    break;
  case EXCEPTION_INT_DIVIDE_BY_ZERO:
    desc = "Integer divide by zero";
    break;
  case EXCEPTION_INT_OVERFLOW:
    desc = "Integer overflow";
    break;
  case EXCEPTION_INVALID_DISPOSITION:
    desc = "Invalid disposition";
    break;
  case EXCEPTION_NONCONTINUABLE_EXCEPTION:
    desc = "non continuable exception";
    break;
  case EXCEPTION_PRIV_INSTRUCTION:
    desc = "Priviledge instruction";
    break;
  case EXCEPTION_SINGLE_STEP: 
    desc = "Single step";
    break;
  case EXCEPTION_STACK_OVERFLOW:
    desc = "Stack overflow";
    break;
  case DBG_CONTROL_C: 
    // First chance: Pass this on to the kernel. 
    // Last chance: Display an appropriate error.  
    // Handle other exceptions.
    desc = "Ctrl+C";
    break;
// some without any documented explanation
  case EXCEPTION_GUARD_PAGE:
    desc = "Guard Page Hit";
    break;
  case EXCEPTION_INVALID_HANDLE:
    desc = "Invalid Handle";
    break;
  case CONTROL_C_EXIT:
    desc = "Control C Exit";
    break;
// from VC++ exceptions listbox
  case 0xc0000135:
    desc = "DLL Not Found";
    break;
  case 0xc0000142:
    desc = "DLL Initialization Failed";
    break;
  case 0xc06d007e:
    desc = "Module Not Found";
    break;
  case 0xc06d007f:
    desc = "Procedure Not Found";
    break;
  case 0xe06d7363:
    desc = "Microsoft C++ Exception";
    break;
  default:
    desc = "unknown... See WINNT.H/WINBASE.H for exception codes (STATUS_xxx/DBG_xxx/EXCEPTION_xxx)";
    found = false;
    break;
  }

  char buffer[512];
  sprintf (buffer, "Exception: 0x%08X -> %s", exceptionCode, desc.c_str());
  
  desc = buffer;
  return found;
}

void ATOM_Debugger::onThreadMessage (MSG * /* msg */)
{
}

void ATOM_Debugger::onWindowMessage (MSG * /* msg */)
{
}

bool ATOM_Debugger::preloadingProcess (void)
{
  return true;
}

void ATOM_Debugger::onDebugEvent (DEBUG_EVENT * /* event */)
{
}

void ATOM_Debugger::onIdle (void)
{
}

bool ATOM_Debugger::postMortem (void)
{
  return true;
}

bool ATOM_Debugger::loadTheProcess (void)
{
  STARTUPINFO startupInfo;
    
  memset(&startupInfo, 0, sizeof(startupInfo));
  startupInfo.cb = sizeof(startupInfo);

  if (::CreateProcess(
        NULL,                      // lpszImageName
        (LPSTR)_M_cmd_line.c_str(),       // lpszCommandLine
        NULL, NULL,                // lpsaProcess and lpsaThread
        FALSE,                     // fInheritHandles
        DEBUG_ONLY_THIS_PROCESS,   // fdwCreate. use DEBUG_PROCESS to get notifications for processes launched by the debuggee
        NULL, NULL,                // lpvEnvironment and lpszCurDir
        &startupInfo,              // lpsiStartupInfo
        &_M_process_information      // lppiProcInfo
        ))
  {
    return true;
  }

  return false;
}

void ATOM_Debugger::askToQuit (void)
{
  ::InterlockedIncrement (&_S_quit);
}

bool ATOM_Debugger::needToQuit (void)
{
  return _S_quit > 0;
}

HANDLE ATOM_Debugger::getDebuggeeHandle (void)
{
  return _M_process_information.hProcess;
}

bool ATOM_Debugger::breakProcess (void)
{
#if 0 // Disable because DebugBreakProcess function not supported by WIN2000
  if (_M_process_information.hProcess)
  {
    BOOL ret = ::DebugBreakProcess (_M_process_information.hProcess);
    return ret != FALSE;
  }
#endif
  return false;
}

bool ATOM_Debugger::debugProcess (void)
{
  DEBUG_EVENT event;
  unsigned continueStatus;

  bool bReturn = preloadingProcess ();
  if (!bReturn)
    return bReturn;

  _M_first_breakpoint = true;

  if (!loadTheProcess ())
  {
    return false;
  }

  for (;;)
  {
    if (::WaitForDebugEvent (&event, _M_latency))
    {
      onDebugEvent (&event);

      continueStatus = handleDebugEvent (&event);

      if (event.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
        break;
      else
        ::ContinueDebugEvent (event.dwProcessId, event.dwThreadId, continueStatus);
    }
    else
    {
      onDebugEvent (0);

      MSG msg;
      while (::PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
      {
        if (msg.hwnd == 0)
        {
          onThreadMessage (&msg);
        }
        else
        {
          onWindowMessage (&msg);
        }
      }

      onIdle ();

      if (needToQuit ())
      {
        break;
      }
    }
  }

  ::CloseHandle (_M_process_information.hProcess);
  ::CloseHandle (_M_process_information.hThread);

  return postMortem ();
}

unsigned ATOM_Debugger::onCreateProcess (CREATE_PROCESS_DEBUG_INFO * /* info */)
{
  return DBG_CONTINUE;
}

unsigned ATOM_Debugger::onExitProcess (unsigned /* exitCode */)
{
  return DBG_CONTINUE;
}

void ATOM_Debugger::onProcessRunning (void)
{
}

unsigned ATOM_Debugger::onException (EXCEPTION_RECORD * /* info */, bool /* firstChance */)
{
  return DBG_EXCEPTION_NOT_HANDLED;
}

unsigned ATOM_Debugger::onOutputDebugString (OUTPUT_DEBUG_STRING_INFO * /* info */)
{
  return DBG_CONTINUE;
}

unsigned ATOM_Debugger::onCreateThread (CREATE_THREAD_DEBUG_INFO * /* info */)
{
  return DBG_CONTINUE;
}

unsigned ATOM_Debugger::onExitThread (unsigned  /* exitCode */)
{
  return DBG_CONTINUE;
}

unsigned ATOM_Debugger::onLoadDll (LOAD_DLL_DEBUG_INFO * /* info */)
{
  return DBG_CONTINUE;
}

unsigned ATOM_Debugger::onUnloadDll (void * /* baseOfDll */)
{
  return DBG_CONTINUE;
}

unsigned ATOM_Debugger::handleDebugEvent (DEBUG_EVENT *event)
{
  _M_event_process_id = event->dwProcessId;
  _M_event_thread_id = event->dwThreadId;

  switch (event->dwDebugEventCode)
  {
  case OUTPUT_DEBUG_STRING_EVENT:
    return onOutputDebugString (&event->u.DebugString);

  case LOAD_DLL_DEBUG_EVENT:
    {
      onLoadDll (&event->u.LoadDll);
      if (event->u.LoadDll.hFile)
      {
        ::CloseHandle (event->u.LoadDll.hFile);
      }
      break;
    }

  case UNLOAD_DLL_DEBUG_EVENT:
    {
      unsigned dwReturn = onUnloadDll (&event->u.UnloadDll);
      return dwReturn;
    }

  case CREATE_THREAD_DEBUG_EVENT:
    {
      unsigned dwReturn = onCreateThread (&event->u.CreateThread);
      if (event->u.CreateThread.hThread)
      {
        ::CloseHandle (event->u.CreateThread.hThread);
      }
      return dwReturn;
    }

  case EXIT_THREAD_DEBUG_EVENT:
    {
      unsigned dwReturn = onExitThread (event->u.ExitThread.dwExitCode);
      return dwReturn;
    }

  case CREATE_PROCESS_DEBUG_EVENT:
    {
      unsigned dwReturn = onCreateProcess (&event->u.CreateProcessInfo);
      if (event->u.CreateProcessInfo.hFile)
      {
        ::CloseHandle (event->u.CreateProcessInfo.hFile);
      }
      if (event->u.CreateProcessInfo.hProcess)
      {
        ::CloseHandle (event->u.CreateProcessInfo.hProcess);
      }
      if (event->u.CreateProcessInfo.hThread)
      {
        ::CloseHandle (event->u.CreateProcessInfo.hThread);
      }
      return dwReturn;
    }

  case EXIT_PROCESS_DEBUG_EVENT:
    {
      unsigned dwReturn = onExitProcess (event->u.ExitProcess.dwExitCode);
      return dwReturn;
    }

  case EXCEPTION_DEBUG_EVENT:
    {
      if (STATUS_BREAKPOINT == event->u.Exception.ExceptionRecord.ExceptionCode)
      {
        if (_M_first_breakpoint)
        {
          _M_first_breakpoint = false;
          onProcessRunning ();
        }
        return DBG_CONTINUE;
      }
      unsigned dwReturn = onException (&event->u.Exception.ExceptionRecord, event->u.Exception.dwFirstChance != 0);
      return dwReturn;
    }

  default:
    ATOM_ASSERT(0);
    break;
  }

  return DBG_CONTINUE;
}

/*
ATOM_Debugger::ATOM_Debugger (void)
{
  _M_debuggee = 0;
}

ATOM_Debugger::~ATOM_Debugger (void)
{
  detachDebuggee ();
}

void ATOM_Debugger::_testOutputError (void)
{
  LPVOID lpMsgBuf;
  DWORD dw = ::GetLastError(); 

  FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | 
      FORMAT_MESSAGE_FROM_SYSTEM,
      NULL,
      dw,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR) &lpMsgBuf,
      0, NULL );

  onOutput ((char*)lpMsgBuf);

  LocalFree(lpMsgBuf);
}

bool ATOM_Debugger::attachProcess (unsigned pid)
{
  if (::DebugActiveProcess (pid))
  {
    return true;
  }

  _testOutputError ();
  return false;
}

bool ATOM_Debugger::launchProcess (const char *cmdline, const char *workingdir)
{
  STARTUPINFO si;
  ::GetStartupInfo (&si);      

  PROCESS_INFORMATION pi;

  if (::CreateProcess (NULL, (LPSTR)cmdline, NULL, NULL, FALSE, DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS, NULL, workingdir, &si, &pi))
  {
    return true;
  }

  _testOutputError ();
  return false;
}

bool ATOM_Debugger::detachDebuggee (void)
{
  if (_M_debuggee)
  {
    if (::DebugActiveProcessStop (_M_debuggee))
    {
      _M_debuggee = 0;
      return true;
    }
    else
    {
      _testOutputError ();
    }
  }

  return false;
}

bool ATOM_Debugger::terminateDebuggee (unsigned exitcode)
{
  if (_M_debuggee)
  {
    if (::DebugActiveProcessStop (_M_debuggee))
    {
      HANDLE hProcess = ::OpenProcess (PROCESS_TERMINATE, FALSE, _M_debuggee);
      if (hProcess == NULL)
      {
        _testOutputError ();
        return false;
      }

      if (!::TerminateProcess (hProcess, exitcode))
      {
        _testOutputError ();
        _M_debuggee = 0;
        return false;
      }

      _M_debuggee = 0;
      return true;
    }
    else
    {
      _testOutputError ();
    }
  }

  return false;
}

void ATOM_Debugger::run (void)
{
  DEBUG_EVENT event;

  for (;;)
  {
    if (!::WaitForDebugEvent (&event, INFINITE))
    {
      _testOutputError ();
      break;
    }

    if (_handleDebugEvent (event))
    {
      break;
    }
  }
}

bool ATOM_Debugger::_handleDebugEvent (const DEBUG_EVENT &event)
{
  unsigned continueState;
  switch (event.dwDebugEventCode)
  {
  case EXCEPTION_DEBUG_EVENT:
    continueState = onException (event.dwProcessId, event.dwThreadId, &event.u.Exception);
    break;
  case CREATE_THREAD_DEBUG_EVENT:
    continueState = onThreadCreated (event.dwProcessId, event.dwThreadId, &event.u.CreateThread);
    break;
  case CREATE_PROCESS_DEBUG_EVENT:
    continueState = onProcessAttached (event.dwProcessId, event.dwThreadId, &event.u.CreateProcessInfo);
    break;
  case EXIT_THREAD_DEBUG_EVENT:
    continueState = onThreadExit (event.dwProcessId, event.dwThreadId, &event.u.ExitThread);
    break;
  case EXIT_PROCESS_DEBUG_EVENT:
    continueState = onProcessExit (event.dwProcessId, event.dwThreadId, &event.u.ExitProcess);
    break;
  case LOAD_DLL_DEBUG_EVENT:
    continueState = onLibraryLoaded (event.dwProcessId, event.dwThreadId, &event.u.LoadDll);
    break;
  case UNLOAD_DLL_DEBUG_EVENT:
    continueState = onLibraryUnloaded (event.dwProcessId, event.dwThreadId, &event.u.UnloadDll);
    break;
  case OUTPUT_DEBUG_STRING_EVENT:
    continueState = onOutputDebugString (event.dwProcessId, event.dwThreadId, &event.u.DebugString);
    break;
  case RIP_EVENT:
    _testOutputError ();
    return true;
    break;
  }

  ::ContinueDebugEvent (event.dwProcessId, event.dwThreadId, continueState);

  return false;
}

void ATOM_Debugger::output (const char *str,...)
{
  static const unsigned bufferlen = 4096;
  char buffer[bufferlen];

  va_list args;
  va_start (args, str);
  _vsnprintf (buffer, 4096, str, args);
  buffer[bufferlen-1] = '\0';
  va_end (args);

  onOutput (buffer);
}

void ATOM_Debugger::onOutput (const char *str)
{
  printf (str);
}

unsigned ATOM_Debugger::onProcessAttached (unsigned pid, unsigned tid, const CREATE_PROCESS_DEBUG_INFO *info)
{
  output ("Process attached:\n");
  output ("\tprocessId = %d\n", pid);
  output ("\tthreadId = %d\n", tid);
  output ("\tbaseOfImage = 0x%08X\n", info->lpBaseOfImage);
  return DBG_CONTINUE;
}

unsigned ATOM_Debugger::onThreadCreated (unsigned pid, unsigned tid, const CREATE_THREAD_DEBUG_INFO *info)
{
  output ("ATOM_Thread created:\n");
  output ("\tprocessId = %d\n", pid);
  output ("\tthreadId = %d\n", tid);
  return DBG_CONTINUE;
}

unsigned ATOM_Debugger::onException (unsigned pid, unsigned tid, const EXCEPTION_DEBUG_INFO *info)
{
  output ("Exception occured:\n");
  output ("\tprocessId = %d\n", pid);
  output ("\tthreadId = %d\n", tid);
  output ("\tdescription = %s(%s)\n", ExceptionCode2String(info->ExceptionRecord.ExceptionCode), info->ExceptionRecord.ExceptionFlags?"NonContinuable":"Continuable");
  output ("\tfirstChance = %s\n", info->dwFirstChance ? "TRUE" : "FALSE");
  output ("\taddress = 0x%08X\n", info->ExceptionRecord.ExceptionAddress);
  if (info->ExceptionRecord.ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
  {
    output ("\t%s memory 0x%08X\n", info->ExceptionRecord.ExceptionInformation[0]?"Writing":"Reading", info->ExceptionRecord.ExceptionInformation[1]);
  }
  return info->ExceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT ? DBG_CONTINUE : DBG_EXCEPTION_NOT_HANDLED;
}

unsigned ATOM_Debugger::onProcessExit (unsigned pid, unsigned tid, const EXIT_PROCESS_DEBUG_INFO *info)
{
  output ("Process exit:\n");
  output ("\tprocessId = %d\n", pid);
  output ("\tthreadId = %d\n", tid);
  output ("\texitCode = %d\n", info->dwExitCode);
  return DBG_CONTINUE;
}

unsigned ATOM_Debugger::onThreadExit (unsigned pid, unsigned tid, const EXIT_THREAD_DEBUG_INFO *info)
{
  output ("ATOM_Thread exit:\n");
  output ("\tprocessId = %d\n", pid);
  output ("\tthreadId = %d\n", tid);
  output ("\texitCode = %d\n", info->dwExitCode);
  return DBG_CONTINUE;
}

unsigned ATOM_Debugger::onLibraryLoaded (unsigned pid, unsigned tid, const LOAD_DLL_DEBUG_INFO *info)
{
  output ("Library loaded:\n");
  output ("\tprocessId = %d\n", pid);
  output ("\tthreadId = %d\n", tid);
  output (info->
}

const char *ATOM_Debugger::ExceptionCode2String (unsigned code)
{
  switch (code)
  {
  case EXCEPTION_ACCESS_VIOLATION: return "EXCEPTION_ACCESS_VIOLATION";
  case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
  case EXCEPTION_BREAKPOINT: return "EXCEPTION_BREAKPOINT";
  case EXCEPTION_DATATYPE_MISALIGNMENT: return "EXCEPTION_DATATYPE_MISALIGNMENT";
  case EXCEPTION_FLT_DENORMAL_OPERAND: return "EXCEPTION_FLT_DENORMAL_OPERAND";
  case EXCEPTION_FLT_DIVIDE_BY_ZERO: return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
  case EXCEPTION_FLT_INEXACT_RESULT: return "EXCEPTION_FLT_INEXACT_RESULT";
  case EXCEPTION_FLT_INVALID_OPERATION: return "EXCEPTION_FLT_INVALID_OPERATION";
  case EXCEPTION_FLT_OVERFLOW: return "EXCEPTION_FLT_OVERFLOW";
  case EXCEPTION_FLT_STACK_CHECK: return "EXCEPTION_FLT_STACK_CHECK";
  case EXCEPTION_FLT_UNDERFLOW: return "EXCEPTION_FLT_UNDERFLOW";
  case EXCEPTION_ILLEGAL_INSTRUCTION: return "EXCEPTION_ILLEGAL_INSTRUCTION";
  case EXCEPTION_IN_PAGE_ERROR: return "EXCEPTION_IN_PAGE_ERROR";
  case EXCEPTION_INT_DIVIDE_BY_ZERO: return "EXCEPTION_INT_DIVIDE_BY_ZERO";
  case EXCEPTION_INT_OVERFLOW: return "EXCEPTION_INT_OVERFLOW";
  case EXCEPTION_INVALID_DISPOSITION: return "EXCEPTION_INVALID_DISPOSITION";
  case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
  case EXCEPTION_PRIV_INSTRUCTION: return "EXCEPTION_PRIV_INSTRUCTION";
  case EXCEPTION_SINGLE_STEP: return "EXCEPTION_SINGLE_STEP";
  case EXCEPTION_STACK_OVERFLOW: return "EXCEPTION_STACK_OVERFLOW";
  default: return "Uknown Exception";
  }
}
*/
