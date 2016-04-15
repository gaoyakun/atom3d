#include "StdAfx.h"
#include <ATOM_thread.h>
#include "stacktrace.h"

#ifndef ATOM_NO_STACK_TRACE

#define MAX_STACK_TRACE_ENTRIES 256
#define MSG_BUFFER_SIZE

static char msg_buffer[65535];

struct StackTraceEntries
{
	const char *entries[MAX_STACK_TRACE_ENTRIES];
	unsigned depth;
};

ATOM_TlsData<StackTraceEntries> stackTraceEntries;

static StackTraceEntries *_getStackTrace (void)
{
	StackTraceEntries *entries = stackTraceEntries.getValue();
	if (!entries)
	{
		entries = new StackTraceEntries;
		entries->depth = 0;
		stackTraceEntries.setValue (entries);
	}
	return entries;
}

ATOM_StackTrace::ATOM_StackTrace (const char *functionName)
{
	StackTraceEntries *entries = _getStackTrace ();

	if (entries->depth < MAX_STACK_TRACE_ENTRIES-1)
	{
		entries->entries[entries->depth++] = functionName;
		_added = true;
	}
	else
	{
		_added = false;
	}
}

ATOM_StackTrace::~ATOM_StackTrace (void)
{
	if (_added)
	{
		_getStackTrace ()->depth--;
	}
}

ATOM_StackTraceEx::ATOM_StackTraceEx (const char *functionName, ...)
{
	char buffer[2048];

	va_list args;
	va_start(args, functionName);
	vsnprintf(buffer, 2048, functionName, args);
	va_end(args);

	StackTraceEntries *entries = _getStackTrace ();

	if (entries->depth < MAX_STACK_TRACE_ENTRIES-1)
	{
		entries->entries[entries->depth++] = functionName;
		_added = true;
	}
	else
	{
		_added = false;
	}
}

ATOM_StackTraceEx::~ATOM_StackTraceEx (void)
{
	if (_added)
	{
		_getStackTrace ()->depth--;
	}
}

#endif

ATOM_DBGHLP_API unsigned ATOM_GetStackTraceDepth (void)
{
#ifndef ATOM_NO_STACK_TRACE
	StackTraceEntries *entries = _getStackTrace ();
	return entries->depth;
#else
	return 0;
#endif
}

ATOM_DBGHLP_API const char *ATOM_GetStackTraceEntry (unsigned index)
{
#ifndef ATOM_NO_STACK_TRACE
	StackTraceEntries *entries = _getStackTrace ();
	return index < entries->depth ? entries->entries[index] : 0;
#else
	return 0;
#endif
}

ATOM_DBGHLP_API const char *ATOM_GetStackTraceString (void)
{
#ifndef ATOM_NO_STACK_TRACE
	msg_buffer[0] = '\0';

	StackTraceEntries *entries = _getStackTrace ();
	if (entries->depth > 0)
	{
		strcat (msg_buffer, entries->entries[entries->depth-1]);

		for (unsigned i = 0; i < entries->depth-1; ++i)
		{
			const char *s = "<-\r\n";
			strcat (msg_buffer, s);
			strcat (msg_buffer, entries->entries[entries->depth-2-i]);
		}
	}
	return msg_buffer;
#else
	return 0;
#endif
}

