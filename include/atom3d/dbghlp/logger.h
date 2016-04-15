#ifndef __ATOM_DBGHLP_ATOM_LOGGER_H
#define __ATOM_DBGHLP_ATOM_LOGGER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <stdarg.h>
#include "basedefs.h"

class ATOM_DBGHLP_API ATOM_LOGGER
{
public:
	enum Level
	{
		LVL_NONE = 0,
		LVL_INFORMATION = 1,
		LVL_WARNING = 2,
		LVL_ERROR = 3,
		LVL_FATAL = 4,
		LVL_DEBUG = 5
	};

	enum
	{
		CONSOLE = (1<<0),
		DEBUGGER = (1<<1),
		FILE = (1<<2),
		PIPE = (1<<3),
		CUSTOM = (1<<4)
	};

	typedef void (ATOM_CALL *LogFunc) (void *userData, const char *msg, unsigned msgLen, const char *msgOrigin, ATOM_LOGGER::Level level);

public:
	static void log (const char *format, ...);
	static void debug (const char *format, ...);
	static void information (const char *format, ...);
	static void warning (const char *format, ...);
	static void error (const char *format, ...);
	static void fatal (const char *format, ...);
	static void log2(Level level, const char *msg);
	static void setFilter (ATOM_LOGGER::Level level, unsigned methods = 0xFFFFFFFF);
	static ATOM_LOGGER::Level getFilter (unsigned method);
	static void setPipeName (const char *pipeName);
	static const char *getPipeName (void);
	static void setCustomLogFunction (ATOM_LOGGER::LogFunc func, void *userData);
	static ATOM_LOGGER::LogFunc getCustomLogFunction (void);
	static void * getCustomLogUserData (void);
	static void setLogFileName (const char *filename);
	static const char *getLogFileName (void);
	static void setLogMethods (unsigned mask);
	static unsigned getLogMethods (void);
	static void flush (void);
	static void finalize (void);

private:
	static void initialize (void);
	static bool openPipe (const char *pipename);
	static void closePipe (void);
	static bool openLogFile (const char *filename);
	static void closeLogFile (void);
	static void echo (Level level, const char *msg, bool skipDebuggerOutput);
	static void log_ (Level level, bool skipDebuggerOutput, const char *format, va_list args);
};

#endif // __ATOM_DBGHLP_ATOM_LOGGER_H
