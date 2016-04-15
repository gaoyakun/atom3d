#include "StdAfx.h"
#include "exception.h"
#include "stacktrace.h"
#include "logger.h"

static char msg[2048];

ATOM_Exception::ATOM_Exception (void)
{
	_desc = "";
	_stacktrace = ATOM_GetStackTraceString ();
}

ATOM_Exception::ATOM_Exception (const char *desc)
{
	_desc = desc ? desc : "";
	_stacktrace = ATOM_GetStackTraceString ();
}

ATOM_Exception::~ATOM_Exception (void)
{
}

const char *ATOM_Exception::getDesc (void) const
{
	return _desc;
}

const char *ATOM_Exception::getStackTrace (void) const
{
	return _stacktrace;
}

ATOM_OutOfMemory::ATOM_OutOfMemory (void)
: ATOM_Exception ("Out of memory")
{
}

ATOM_AbnormalTermination::ATOM_AbnormalTermination (void)
: ATOM_Exception ("Abnormal termination")
{
}

BOOL Unicode16ToAnsi(WCHAR *in_Src, CHAR *out_Dst, INT in_MaxLen)
{
	INT lv_Len;
	BOOL lv_UsedDefault;
	if (in_MaxLen <= 0)
		return FALSE;
	lv_Len = WideCharToMultiByte(CP_ACP,0,in_Src,-1,out_Dst,in_MaxLen,0,&lv_UsedDefault);
	if (lv_Len < 0)
		lv_Len = 0;
	if (lv_Len < in_MaxLen)
		out_Dst[lv_Len] = 0;
	else if (out_Dst[in_MaxLen-1])
		out_Dst[0] = 0;
	return !lv_UsedDefault;
}

ATOM_InvalidCRTParameter::ATOM_InvalidCRTParameter (const wchar_t *expression, const wchar_t *function, const wchar_t *file, unsigned line)
{
	char exp[512];
	char func[512];
	char f[512];

	if (!expression || !Unicode16ToAnsi((wchar_t*)expression, exp, 512))
	{
		strcpy(exp, "Unknown expression");
	}
	if (!function || !Unicode16ToAnsi((wchar_t*)function, func, 512))
	{
		strcpy(func, "Unknown function");
	}
	if (!file || !Unicode16ToAnsi((wchar_t*)file, f, 512))
	{
		strcpy(f, "Unknown file");
	}
	ATOM_LOGGER::log ("Invalid CRuntime Parameter:\n");
	ATOM_LOGGER::log ("  exp:%s\n", exp);
	ATOM_LOGGER::log ("  func:%s\n", func);
	ATOM_LOGGER::log ("  file:%s\n", f);
	ATOM_LOGGER::log ("  line:%d\n", line);

	char *s = (char*)malloc(2048);
	if (s)
	{
		sprintf (s, "Invalid CRuntime Parameter:\n  exp:%s\n  func:%s\n  file:%s\n  line:%d\n", exp, func, f, line);
	}
	_desc = s;
}

ATOM_PureVirtualFunctionCall::ATOM_PureVirtualFunctionCall (void)
{
	_desc = strdup("Pure virtual function call");
}

ATOM_Win32Exception::ATOM_Win32Exception (int code, PEXCEPTION_POINTERS exceptionPoints)
{
	::FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_FROM_HMODULE, ::LoadLibraryA("NTDLL.dll"), code, 0, msg, 2048, NULL);
	_desc = msg;
}

