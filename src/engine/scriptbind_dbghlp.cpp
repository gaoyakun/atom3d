#include "StdAfx.h"
#include "scriptbind_traits.h"

static void Logger_log (const char *str)
{
	ATOM_LOGGER::log (str);
}

static void Logger_info (const char *str)
{
	ATOM_LOGGER::information (str);
}

static void Logger_warning (const char *str)
{
	ATOM_LOGGER::warning (str);
}

static void Logger_error (const char *str)
{
	ATOM_LOGGER::error (str);
}

static void Logger_fatal (const char *str)
{
	ATOM_LOGGER::fatal (str);
}

static void Logger_debug (const char *str)
{
	ATOM_LOGGER::debug (str);
}

static int Logger_get_filter (void)
{
	return ATOM_LOGGER::getFilter (ATOM_LOGGER::CUSTOM);
}

static void Logger_set_filter (int filter)
{
	ATOM_LOGGER::setFilter ((ATOM_LOGGER::Level)filter, ATOM_LOGGER::CUSTOM);
}


static void WriteDump (const char *filename, bool full)
{
	ATOM_WriteMiniDump (filename, 0, 0, full ? MDT_WITHFULLMEMORY|MDT_WITHINDIRECTLYREFERENCEDMEMORY|MDT_WITHFULLMEMORYINFO|MDT_WITHTHREADINFO|MDT_WITHCODESEGS : MDT_NORMAL);
}

static void *Malloc (unsigned size)
{
	return ATOM_MALLOC(size);
}

static void *Realloc (void *p, unsigned size)
{
	return ATOM_REALLOC(p, size);
}

static void *Calloc (unsigned size)
{
	return ATOM_CALLOC(1, size);
}

static void Free (void *p)
{
	return ATOM_FREE(p);
}

static void *AlignedMalloc (unsigned size, unsigned align)
{
	return ATOM_ALIGNED_MALLOC(size, align);
}

static void AlignedFree (void *p)
{
	return ATOM_ALIGNED_FREE(p);
}

void StrFreeFunc (void *p)
{
	free (p);
}

ATOM_SCRIPT_BEGIN_TYPE_TABLE(DbghlpTypes)

	ATOM_SCRIPT_BEGIN_CLASS_NOPARENT_NOCONSTRUCTOR(ATOM_LOGGER, ATOM_LOGGER)
		ATOM_DECLARE_STATIC_INT_CONSTANT(LVL_NONE, ATOM_LOGGER::LVL_NONE)
		ATOM_DECLARE_STATIC_INT_CONSTANT(LVL_INFORMATION, ATOM_LOGGER::LVL_INFORMATION)
		ATOM_DECLARE_STATIC_INT_CONSTANT(LVL_WARNING, ATOM_LOGGER::LVL_WARNING)
		ATOM_DECLARE_STATIC_INT_CONSTANT(LVL_ERROR, ATOM_LOGGER::LVL_ERROR)
		ATOM_DECLARE_STATIC_INT_CONSTANT(LVL_FATAL, ATOM_LOGGER::LVL_FATAL)
		ATOM_DECLARE_STATIC_INT_CONSTANT(LVL_DEBUG, ATOM_LOGGER::LVL_DEBUG)
		ATOM_DECLARE_STATIC_INT_CONSTANT(CONSOLE, ATOM_LOGGER::CONSOLE)
		ATOM_DECLARE_STATIC_INT_CONSTANT(DEBUGGER, ATOM_LOGGER::DEBUGGER)
		ATOM_DECLARE_STATIC_INT_CONSTANT(FILE, ATOM_LOGGER::FILE)
		ATOM_DECLARE_STATIC_INT_CONSTANT(PIPE, ATOM_LOGGER::PIPE)
		ATOM_DECLARE_STATIC_METHOD(log, Logger_log)
		ATOM_DECLARE_STATIC_METHOD(info, Logger_info)
		ATOM_DECLARE_STATIC_METHOD(warning, Logger_warning)
		ATOM_DECLARE_STATIC_METHOD(error, Logger_error)
		ATOM_DECLARE_STATIC_METHOD(fatal, Logger_fatal)
		ATOM_DECLARE_STATIC_METHOD(debug, Logger_debug)
		ATOM_DECLARE_STATIC_METHOD(setFilter, Logger_set_filter)
		ATOM_DECLARE_STATIC_METHOD(getFilter, Logger_get_filter)
		ATOM_DECLARE_STATIC_METHOD(setLogMethods, ATOM_LOGGER::setLogMethods)
		ATOM_DECLARE_STATIC_METHOD(getLogMethods, ATOM_LOGGER::getLogMethods)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS_NOPARENT_NOCONSTRUCTOR(ATOM_Win32DebugStream, ATOM_Win32DebugStream)
		ATOM_DECLARE_STATIC_METHOD(isMonitoring, ATOM_Win32DebugStream::isMonitoring)
		ATOM_DECLARE_STATIC_METHOD(startMonitering, ATOM_Win32DebugStream::startMonitering)
		ATOM_DECLARE_STATIC_METHOD(stopMonitering, ATOM_Win32DebugStream::stopMonitering)
	ATOM_SCRIPT_END_CLASS ()

ATOM_SCRIPT_END_TYPE_TABLE

ATOM_SCRIPT_BEGIN_FUNCTION_TABLE(DbghlpFunctions)
	ATOM_DECLARE_FUNCTION(ATOM_WriteMiniDump, WriteDump)
	ATOM_DECLARE_FUNCTION(ATOM_Malloc, Malloc)
	ATOM_DECLARE_FUNCTION(ATOM_Realloc, Realloc)
	ATOM_DECLARE_FUNCTION(ATOM_Calloc, Calloc)
	ATOM_DECLARE_FUNCTION(ATOM_Free, Free)
	ATOM_DECLARE_FUNCTION(ATOM_AlignedMalloc, AlignedMalloc)
	ATOM_DECLARE_FUNCTION(ATOM_AlignedFree, AlignedFree)
	ATOM_DECLARE_FUNCTION(ATOM_DumpMemoryState, ATOM_DumpMemoryState)
	ATOM_DECLARE_FUNCTION(ATOM_DumpMemoryBlock, ATOM_DumpMemoryBlock)
	ATOM_DECLARE_FUNCTION(ATOM_DumpMemoryBlockToMemory, ATOM_DumpMemoryBlockToMemory)
	ATOM_DECLARE_FUNCTION(ATOM_DumpMemoryDifference, ATOM_DumpMemoryDifference)
	ATOM_DECLARE_FUNCTION(ATOM_SetAllocCounterAlert, ATOM_SetAllocCounterAlert)
	ATOM_DECLARE_FUNCTION(ATOM_GetAllocCounterAlert, ATOM_GetAllocCounterAlert)
ATOM_SCRIPT_END_FUNCTION_TABLE

void __dbghlp_bind_script (ATOM_Script *script)
{
	ATOM_SCRIPT_REGISTER_TYPE_TABLE(script, DbghlpTypes);
	ATOM_SCRIPT_REGISTER_FUNCTION_TABLE(script, DbghlpFunctions);
}
