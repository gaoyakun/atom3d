#include "StdAfx.h"
#include "errorhandlers.h"

#include <new.h>
#include <signal.h>

static ATOM_AppErrorHandler *_globalErrorHandler = nullptr;
static ATOM_AppErrorHandler _defaultErrorHandler;

void _getExceptionPointers(DWORD dwExceptionCode, EXCEPTION_POINTERS** ppExceptionPointers)
{
	// The following code was taken from VC++ 8.0 CRT (invarg.c: line 104)

	EXCEPTION_RECORD ExceptionRecord;
	CONTEXT ContextRecord;
	memset(&ContextRecord, 0, sizeof(CONTEXT));

#ifdef _X86_

	__asm {
		mov dword ptr [ContextRecord.Eax], eax
			mov dword ptr [ContextRecord.Ecx], ecx
			mov dword ptr [ContextRecord.Edx], edx
			mov dword ptr [ContextRecord.Ebx], ebx
			mov dword ptr [ContextRecord.Esi], esi
			mov dword ptr [ContextRecord.Edi], edi
			mov word ptr [ContextRecord.SegSs], ss
			mov word ptr [ContextRecord.SegCs], cs
			mov word ptr [ContextRecord.SegDs], ds
			mov word ptr [ContextRecord.SegEs], es
			mov word ptr [ContextRecord.SegFs], fs
			mov word ptr [ContextRecord.SegGs], gs
			pushfd
			pop [ContextRecord.EFlags]
	}

	ContextRecord.ContextFlags = CONTEXT_CONTROL;
#pragma warning(push)
#pragma warning(disable:4311)
	ContextRecord.Eip = (ULONG)_ReturnAddress();
	ContextRecord.Esp = (ULONG)_AddressOfReturnAddress();
#pragma warning(pop)
	ContextRecord.Ebp = *((ULONG *)_AddressOfReturnAddress()-1);


#elif defined (_IA64_) || defined (_AMD64_)

	/* Need to fill up the Context in IA64 and AMD64. */
	RtlCaptureContext(&ContextRecord);

#else  /* defined (_IA64_) || defined (_AMD64_) */

	ZeroMemory(&ContextRecord, sizeof(ContextRecord));

#endif  /* defined (_IA64_) || defined (_AMD64_) */

	ZeroMemory(&ExceptionRecord, sizeof(EXCEPTION_RECORD));

	ExceptionRecord.ExceptionCode = dwExceptionCode;
	ExceptionRecord.ExceptionAddress = _ReturnAddress();

	///

	EXCEPTION_RECORD* pExceptionRecord = new EXCEPTION_RECORD;
	memcpy(pExceptionRecord, &ExceptionRecord, sizeof(EXCEPTION_RECORD));
	CONTEXT* pContextRecord = new CONTEXT;
	memcpy(pContextRecord, &ContextRecord, sizeof(CONTEXT));

	*ppExceptionPointers = new EXCEPTION_POINTERS;
	(*ppExceptionPointers)->ExceptionRecord = pExceptionRecord;
	(*ppExceptionPointers)->ContextRecord = pContextRecord;  
}


static void _handleErrorWithoutExceptionPointers (ATOM_AppErrorHandler::ErrorType errorType)
{
	EXCEPTION_POINTERS* pExceptionPtrs = NULL;
	_getExceptionPointers(0, &pExceptionPtrs);
	ATOM_GetGlobalErrorHandler()->reportError (pExceptionPtrs, errorType);
}

ATOM_AppErrorHandler::ATOM_AppErrorHandler (void)
{
}

ATOM_AppErrorHandler::~ATOM_AppErrorHandler (void)
{
}

void ATOM_AppErrorHandler::setProcessErrorHandlers (void)
{
	_set_purecall_handler (_pureCallHandler);
	_set_new_handler (_newHandler);
	_set_invalid_parameter_handler (_invalidParamHandler);
	_set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);
	signal (SIGABRT, _sigabrt);
	signal (SIGINT, _sigint);
	signal (SIGTERM, _sigterm);
}

void ATOM_AppErrorHandler::setThreadErrorHandlers (void)
{
	set_terminate (_terminateHandler);
	set_unexpected (_unexpectedHandler);
	signal (SIGFPE, _sigfpe);
	signal (SIGILL, _sigill);
	signal (SIGSEGV, _sigsegv);
}

void ATOM_AppErrorHandler::reportError (PEXCEPTION_POINTERS p, ATOM_AppErrorHandler::ErrorType errorType)
{
	if (!p)
	{
		_getExceptionPointers (0, &p);
	}
	_reportError (p, errorType);
}

void ATOM_AppErrorHandler::_reportError (PEXCEPTION_POINTERS p, ATOM_AppErrorHandler::ErrorType errorType)
{
}

void __cdecl ATOM_AppErrorHandler::_terminateHandler (void)
{
	_handleErrorWithoutExceptionPointers (ET_TERMINATION);
}

void __cdecl ATOM_AppErrorHandler::_unexpectedHandler (void)
{
	_handleErrorWithoutExceptionPointers (ET_UNEXPECTED);
}

void __cdecl ATOM_AppErrorHandler::_pureCallHandler (void)
{
	_handleErrorWithoutExceptionPointers (ET_PURECALL);
}

void __cdecl ATOM_AppErrorHandler::_invalidParamHandler (const wchar_t *exp, const wchar_t *func, const wchar_t *file, unsigned line, uintptr_t pReserved)
{
	_handleErrorWithoutExceptionPointers (ET_INVALIDPARAM);
}

int __cdecl ATOM_AppErrorHandler::_newHandler (size_t)
{
	_handleErrorWithoutExceptionPointers (ET_NEW);
	return 0;
}

void ATOM_AppErrorHandler::_sigabrt (int)
{
	_handleErrorWithoutExceptionPointers (ET_SIGABRT);
}

void ATOM_AppErrorHandler::_sigfpe (int)
{
	_handleErrorWithoutExceptionPointers (ET_SIGFPE);
}

void ATOM_AppErrorHandler::_sigint (int)
{
	_handleErrorWithoutExceptionPointers (ET_SIGINT);
}

void ATOM_AppErrorHandler::_sigill (int)
{
	_handleErrorWithoutExceptionPointers (ET_SIGILL);
}

void ATOM_AppErrorHandler::_sigsegv (int)
{
	_handleErrorWithoutExceptionPointers (ET_SIGSEGV);
}

void ATOM_AppErrorHandler::_sigterm (int)
{
	_handleErrorWithoutExceptionPointers (ET_SIGTERM);
}

ATOM_DBGHLP_API ATOM_AppErrorHandler * ATOM_CALL ATOM_GetGlobalErrorHandler (void)
{
	return _globalErrorHandler ? _globalErrorHandler : &_defaultErrorHandler;
}

ATOM_DBGHLP_API void ATOM_CALL ATOM_SetGlobalErrorHandler (ATOM_AppErrorHandler *handler)
{
	_globalErrorHandler = handler;
}

