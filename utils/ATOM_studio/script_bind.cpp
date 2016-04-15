#include "StdAfx.h"
#include "script_bind.h"

static void *MallocWrapper (unsigned size)
{
	return ATOM_MALLOC(size);
}

static void *ReallocWrapper (void *p, unsigned size)
{
	return ATOM_REALLOC(p, size);
}

static void *CallocWrapper (unsigned size)
{
	return ATOM_CALLOC(1, size);
}

static void FreeWrapper (void *p)
{
	ATOM_FREE(p);
}

static void *AlignedMallocWrapper (unsigned size, unsigned alignment)
{
	return ATOM_ALIGNED_MALLOC(size, alignment);
}

static void AlignedFreeWrapper (void *p)
{
	ATOM_ALIGNED_FREE(p);
}

static void LogWrapper (const char *msg)
{
	ATOM_LOGGER::log (msg);
};

static void LogInfoWrapper (const char *msg)
{
	ATOM_LOGGER::information (msg);
};

static void LogWarningWrapper (const char *msg)
{
	ATOM_LOGGER::warning (msg);
};

static void LogErrorWrapper (const char *msg)
{
	ATOM_LOGGER::error (msg);
};

static void LogFatalWrapper (const char *msg)
{
	ATOM_LOGGER::fatal (msg);
};

ATOM_SCRIPT_BEGIN_FUNCTION_TABLE(DbgHlpFunctions)
	ATOM_DECLARE_FUNCTION(ATOM_MALLOC, MallocWrapper)
	ATOM_DECLARE_FUNCTION(ATOM_REALLOC, ReallocWrapper)
	ATOM_DECLARE_FUNCTION(ATOM_CALLOC, CallocWrapper)
	ATOM_DECLARE_FUNCTION(ATOM_FREE, FreeWrapper)
	ATOM_DECLARE_FUNCTION(ATOM_ALIGNEDMALLOC, AlignedMallocWrapper)
	ATOM_DECLARE_FUNCTION(ATOM_ALIGNEDFREE, AlignedFreeWrapper)
	ATOM_DECLARE_FUNCTION(ATOM_DumpMemoryState, ATOM_DumpMemoryState)
	ATOM_DECLARE_FUNCTION(ATOM_DumpMemoryDifference, ATOM_DumpMemoryDifference)
	ATOM_DECLARE_FUNCTION(ATOM_Log, LogWrapper)
	ATOM_DECLARE_FUNCTION(ATOM_Info, LogInfoWrapper)
	ATOM_DECLARE_FUNCTION(ATOM_Warning, LogWarningWrapper)
	ATOM_DECLARE_FUNCTION(ATOM_Error, LogErrorWrapper)
	ATOM_DECLARE_FUNCTION(ATOM_Fatal, LogFatalWrapper)
ATOM_SCRIPT_END_FUNCTION_TABLE

void BindDbgHlp (ATOM_Script *scp)
{
	ATOM_SCRIPT_REGISTER_FUNCTION_TABLE(scp, DbgHlpFunctions);
}

void BindScript (ATOM_Script *scp)
{
	BindDbgHlp (scp);
}

