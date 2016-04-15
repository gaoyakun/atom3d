#ifndef __ATOM_DEBUGHELP_DEBUGGER_H__
#define __ATOM_DEBUGHELP_DEBUGGER_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <windows.h>
#include <set>
#include "basedefs.h"

class ATOM_DBGHLP_API ATOM_Debugger
  {
  public:
    ATOM_Debugger (const char *cmdline, unsigned latency = 100); // 1/10 second
    virtual ~ATOM_Debugger (void) = 0; // pure virtual

  private:
    ATOM_Debugger (void); // impossible to call the default constructor
    ATOM_Debugger (const ATOM_Debugger &); // impossible to call the copy constructor
    void operator = (const ATOM_Debugger &); // impossible to assign

  public:
    bool debugProcess (void);
    bool breakProcess (void);
    bool readProcessMemoryEx (const void *baseAddress, void *buffer, unsigned size);
    bool getModuleHeader (const void *module, PIMAGE_NT_HEADERS hdr);
    bool getMoudleNameInProcess (HMODULE hModule, char *name, unsigned size);
    void *getPreferredLoadAddress (const void *module);
    HANDLE getDebuggeeHandle (void);
    virtual bool needToQuit (void);

    static void askToQuit (void);

    bool getExceptionDescription (unsigned exceptionCode, std::string &desc);
    bool getFullModuleName (HMODULE hModule, std::string &fullpathname);

  protected:
    virtual unsigned onCreateProcess (CREATE_PROCESS_DEBUG_INFO *info);
    virtual unsigned onExitProcess (unsigned exitCode);
    virtual unsigned onCreateThread (CREATE_THREAD_DEBUG_INFO *info);
    virtual unsigned onExitThread (unsigned exitCode);
    virtual unsigned onLoadDll (LOAD_DLL_DEBUG_INFO *info);
    virtual unsigned onUnloadDll (void *baseOfDll);
    virtual unsigned onException (EXCEPTION_RECORD *info, bool firstChance);
    virtual unsigned onOutputDebugString (OUTPUT_DEBUG_STRING_INFO *info);

  protected:
    virtual bool preloadingProcess (void);
    virtual bool postMortem (void);
    virtual void onProcessRunning (void);
    virtual void onThreadMessage (MSG *msg);
    virtual void onWindowMessage (MSG *msg);
    virtual void onIdle (void);
    virtual void onDebugEvent (DEBUG_EVENT *event);

  protected:
    unsigned _M_event_process_id;
    unsigned _M_event_thread_id;

  protected:
    std::string _M_cmd_line;
    PROCESS_INFORMATION _M_process_information;
    unsigned _M_latency;
    bool _M_first_breakpoint;

    static long _S_quit;

  private:
    bool loadTheProcess (void);
    unsigned handleDebugEvent (DEBUG_EVENT *event);
  };

#endif // __ATOM_DEBUGHELP_DEBUGGER_H__

