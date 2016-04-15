#include "StdAfx.h"
#include "scriptbind_traits.h"

static char PathBuffer[MAX_PATH];

static const char *getCWD (void)
{
	PathBuffer[0] = '\0';
	::GetCurrentDirectoryA (MAX_PATH, PathBuffer);
	return PathBuffer;
}

static bool setCWD (const char *dir)
{
	return ::SetCurrentDirectoryA (dir) == TRUE;
}

static const char *getModuleFileName (void)
{
	PathBuffer[0] = '\0';
	::GetModuleFileNameA (NULL, PathBuffer, MAX_PATH);
	return PathBuffer;
}

static const char *getModuleFilePath (void)
{
	PathBuffer[0] = '\0';
	::GetModuleFileNameA (NULL, PathBuffer, MAX_PATH);
	char *p = strrchr (PathBuffer, '\\');
	if (p)
	{
		p[1] = '\0';
	}
	return PathBuffer;
}

class MemoryCounters: public PROCESS_MEMORY_COUNTERS
{
	static PROCESS_MEMORY_COUNTERS _counters;

public:
	static void update (void)
	{
		DWORD nId = GetCurrentProcessId();
		HANDLE hProcess = ::OpenProcess (PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, nId);	
		memset (&_counters, 0, sizeof(PROCESS_MEMORY_COUNTERS));
		_counters.cb = sizeof(PROCESS_MEMORY_COUNTERS);
		::GetProcessMemoryInfo (hProcess, &_counters, sizeof(PROCESS_MEMORY_COUNTERS));
		::CloseHandle (hProcess);
	}

	static unsigned getPageFaultCount (void) { return _counters.PageFaultCount; }
	static unsigned getPeakWorkingSetSize (void) { return _counters.PeakWorkingSetSize; }
	static unsigned getWorkingSetSize (void) { return _counters.WorkingSetSize; }
	static unsigned getQuotaPeakPagedPoolUsage (void) { return _counters.QuotaPeakPagedPoolUsage; }
	static unsigned getQuotaPagedPoolUsage (void) { return _counters.QuotaPagedPoolUsage; }
	static unsigned getQuotaPeakNonPagedPoolUsage (void) { return _counters.QuotaPeakNonPagedPoolUsage; }
	static unsigned getQuotaNonPagedPoolUsage (void) { return _counters.QuotaNonPagedPoolUsage; }
	static unsigned getPagefileUsage (void) { return _counters.PagefileUsage; }
	static unsigned getPeakPagefileUsage (void) { return _counters.PeakPagefileUsage; }
};

PROCESS_MEMORY_COUNTERS MemoryCounters::_counters;

ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(ATOM_MemoryCounters, MemoryCounters, true)

ATOM_SCRIPT_BEGIN_TYPE_TABLE(SystemTypes)

	ATOM_SCRIPT_BEGIN_NONCOPYABLE_CLASS_NOPARENT(ATOM_MemoryCounters, MemoryCounters)
		ATOM_DECLARE_STATIC_METHOD(update, MemoryCounters::update)
		ATOM_DECLARE_STATIC_METHOD(getPageFaultCount, MemoryCounters::getPageFaultCount)
		ATOM_DECLARE_STATIC_METHOD(getPeakWorkingSetSize, MemoryCounters::getPeakWorkingSetSize)
		ATOM_DECLARE_STATIC_METHOD(getWorkingSetSize, MemoryCounters::getWorkingSetSize)
		ATOM_DECLARE_STATIC_METHOD(getQuotaPeakPagedPoolUsage, MemoryCounters::getQuotaPeakPagedPoolUsage)
		ATOM_DECLARE_STATIC_METHOD(getQuotaPagedPoolUsage, MemoryCounters::getQuotaPagedPoolUsage)
		ATOM_DECLARE_STATIC_METHOD(getQuotaPeakNonPagedPoolUsage, MemoryCounters::getQuotaPeakNonPagedPoolUsage)
		ATOM_DECLARE_STATIC_METHOD(getQuotaNonPagedPoolUsage, MemoryCounters::getQuotaNonPagedPoolUsage)
		ATOM_DECLARE_STATIC_METHOD(getPagefileUsage, MemoryCounters::getPagefileUsage)
		ATOM_DECLARE_STATIC_METHOD(getPeakPagefileUsage, MemoryCounters::getPeakPagefileUsage)
	ATOM_SCRIPT_END_CLASS ()

ATOM_SCRIPT_END_TYPE_TABLE

ATOM_SCRIPT_BEGIN_FUNCTION_TABLE(SystemFunctions)
	ATOM_DECLARE_FUNCTION(ATOM_GetCurrentDirectory, getCWD)
	ATOM_DECLARE_FUNCTION(ATOM_SetCurrentDirectory, setCWD)
	ATOM_DECLARE_FUNCTION(ATOM_GetModuleFileName, getModuleFileName)
	ATOM_DECLARE_FUNCTION(ATOM_GetModuleFileDirectory, getModuleFilePath)
ATOM_SCRIPT_END_FUNCTION_TABLE

void __system_bind_script (ATOM_Script *script)
{
	ATOM_SCRIPT_REGISTER_TYPE_TABLE(script, SystemTypes);
	ATOM_SCRIPT_REGISTER_FUNCTION_TABLE(script, SystemFunctions);
}
