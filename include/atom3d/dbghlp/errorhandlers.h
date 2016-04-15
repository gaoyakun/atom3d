#ifndef __ATOM_DEBUGHELP_ERRORHANDLERS_H__
#define __ATOM_DEBUGHELP_ERRORHANDLERS_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <windows.h>
#include "basedefs.h"

#define ATOM_ERRHANDLER ATOM_GetGlobalErrorHandler()

class ATOM_DBGHLP_API ATOM_AppErrorHandler
{
public:
	enum ErrorType
	{
		ET_UNKNOWN = 0,
		ET_WIN32_EXCEPTION,
		ET_CPP_EXCEPTION,
		ET_TERMINATION,
		ET_UNEXPECTED,
		ET_PURECALL,
		ET_INVALIDPARAM,
		ET_NEW,
		ET_SIGABRT,
		ET_SIGFPE,
		ET_SIGINT,
		ET_SIGILL,
		ET_SIGSEGV,
		ET_SIGTERM
	};

public:
	ATOM_AppErrorHandler (void);
	virtual ~ATOM_AppErrorHandler (void);

public:
	void setProcessErrorHandlers (void);
	void setThreadErrorHandlers (void);
	void reportError (PEXCEPTION_POINTERS p, ATOM_AppErrorHandler::ErrorType errorType);

protected:
	virtual void _reportError (PEXCEPTION_POINTERS p, ATOM_AppErrorHandler::ErrorType errorType);

protected:
	static void __cdecl _terminateHandler (void);
	static void __cdecl _unexpectedHandler (void);
	static void __cdecl _pureCallHandler (void);
	static void __cdecl _invalidParamHandler (const wchar_t *exp, const wchar_t *func, const wchar_t *file, unsigned line, uintptr_t pReserved);
	static int __cdecl _newHandler (size_t);
	static void _sigabrt (int);
	static void _sigfpe (int);
	static void _sigint (int);
	static void _sigill (int);
	static void _sigsegv (int);
	static void _sigterm (int);
};

ATOM_DBGHLP_API ATOM_AppErrorHandler * ATOM_CALL ATOM_GetGlobalErrorHandler (void);
ATOM_DBGHLP_API void ATOM_CALL ATOM_SetGlobalErrorHandler (ATOM_AppErrorHandler *handler);

#endif // __ATOM_DEBUGHELP_ERRORHANDLERS_H__
