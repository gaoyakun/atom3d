#include "StdAfx.h"
#include <ATOM_thread.h>
#include "logger.h"
#include "win32_debug_stream.h"

static char logFileName[256] = { '\0' };
static char logPipeName[256] = { '\0' };

static ATOM_LOGGER::LogFunc logFunc = 0;
static void *userData = 0;

static unsigned logMethods = ATOM_LOGGER::FILE;

static HANDLE pipe = INVALID_HANDLE_VALUE;
static HANDLE stdoutHandle = INVALID_HANDLE_VALUE;
static FILE *logFile = 0;

static ATOM_LOGGER::Level filterConsole = ATOM_LOGGER::LVL_INFORMATION;
static ATOM_LOGGER::Level filterDebugger = ATOM_LOGGER::LVL_INFORMATION;
static ATOM_LOGGER::Level filterFile = ATOM_LOGGER::LVL_INFORMATION;
static ATOM_LOGGER::Level filterPipe = ATOM_LOGGER::LVL_WARNING;
static ATOM_LOGGER::Level filterCustom = ATOM_LOGGER::LVL_INFORMATION;

static bool initialized = false;
extern PCHAR* cmdline2argv(PCHAR CmdLine, int* _argc);

static ATOM_Mutex &getLoggerMutex (void)
{
	static ATOM_Mutex *m = new ATOM_Mutex;
	return *m;
}

static bool needLog (ATOM_LOGGER::Level level)
{
	if (0 == (logMethods & (ATOM_LOGGER::CONSOLE|ATOM_LOGGER::DEBUGGER|ATOM_LOGGER::FILE|ATOM_LOGGER::PIPE|ATOM_LOGGER::CUSTOM)))
	{
		return false;
	}

	if (level == ATOM_LOGGER::LVL_NONE)
	{
		return true;
	}

	if (level == ATOM_LOGGER::LVL_DEBUG)
	{
#if !defined(NDEBUG)
		return true;
#else
		return false;
#endif
	}

	if ((logMethods & ATOM_LOGGER::CONSOLE) != 0 && level >= filterConsole)
	{
		return true;
	}

	if ((logMethods & ATOM_LOGGER::DEBUGGER) != 0 && level >= filterDebugger)
	{
		return true;
	}

	if ((logMethods & ATOM_LOGGER::FILE) != 0 && level >= filterFile)
	{
		return true;
	}

	if ((logMethods & ATOM_LOGGER::PIPE) != 0 && level >= filterPipe)
	{
		return true;
	}

	if ((logMethods & ATOM_LOGGER::CUSTOM) != 0 && logFunc && level >= filterCustom)
	{
		return true;
	}

	return false;
}

static void formatString(const char* format, char *buffer, unsigned bufferSize, va_list args) 
{
  vsnprintf(buffer, bufferSize, format, args);
}

void ATOM_LOGGER::log_ (Level level, bool skipDebuggerOutput, const char *format, va_list args)
{
	if (!initialized)
	{
		initialize ();
		initialized = true;
	}

	char buffer[16384];
	formatString(format, buffer, 16384, args);
	echo (level, buffer, skipDebuggerOutput);

	if (level >= LVL_WARNING)
	{
		flush ();
	}
}

void ATOM_LOGGER::log2(Level level, const char *msg)
{
	echo (level, msg, true);
}

void ATOM_LOGGER::log (const char *format, ...)
{
	va_list args;
	va_start(args, format);
	log_ (ATOM_LOGGER::LVL_NONE, false, format, args);
	va_end(args);
}

void ATOM_LOGGER::debug (const char *format, ...)
{
	va_list args;
	va_start(args, format);
	log_ (ATOM_LOGGER::LVL_DEBUG, false, format, args);
	va_end(args);
}


void ATOM_LOGGER::information (const char *format, ...)
{
	va_list args;
	va_start(args, format);
	log_ (ATOM_LOGGER::LVL_INFORMATION, false, format, args);
	va_end(args);
}

void ATOM_LOGGER::warning (const char *format, ...)
{
	va_list args;
	va_start(args, format);
	log_ (ATOM_LOGGER::LVL_WARNING, false, format, args);
	va_end(args);
}

void ATOM_LOGGER::error (const char *format, ...)
{
	va_list args;
	va_start(args, format);
	log_ (ATOM_LOGGER::LVL_ERROR, false, format, args);
	va_end(args);
}

void ATOM_LOGGER::fatal (const char *format, ...)
{
	va_list args;
	va_start(args, format);
	log_ (ATOM_LOGGER::LVL_FATAL, false, format, args);
	va_end(args);
}

void ATOM_LOGGER::setFilter (ATOM_LOGGER::Level level, unsigned methods)
{
	if (!initialized)
	{
		initialize ();
		initialized = true;
	}

	if (methods & ATOM_LOGGER::CONSOLE)
	{
		filterConsole = level;
	}

	if (methods & ATOM_LOGGER::DEBUGGER)
	{
		filterDebugger = level;
	}

	if (methods & ATOM_LOGGER::FILE)
	{
		filterFile = level;
	}

	if (methods & ATOM_LOGGER::PIPE)
	{
		filterPipe = level;
	}

	if (methods & ATOM_LOGGER::CUSTOM)
	{
		filterCustom = level;
	}
}

ATOM_LOGGER::Level ATOM_LOGGER::getFilter (unsigned method)
{
	if (!initialized)
	{
		initialize ();
		initialized = true;
	}

	switch (method)
	{
	case ATOM_LOGGER::CONSOLE:
		return filterConsole;
	case ATOM_LOGGER::DEBUGGER:
		return filterDebugger;
	case ATOM_LOGGER::FILE:
		return filterFile;
	case ATOM_LOGGER::PIPE:
		return filterPipe;
	case ATOM_LOGGER::CUSTOM:
		return filterCustom;
	default:
		return ATOM_LOGGER::LVL_NONE;
	}
}

void ATOM_LOGGER::setPipeName (const char *pipeName)
{
	if (!initialized)
	{
		initialize ();
		initialized = true;
	}

	if (pipeName)
	{
		closePipe ();

		strcpy (logPipeName, pipeName);

		openPipe (logPipeName);
	}
}

const char *ATOM_LOGGER::getPipeName (void)
{
	if (!initialized)
	{
		initialize ();
		initialized = true;
	}

	return logPipeName;
}

void ATOM_LOGGER::setCustomLogFunction (ATOM_LOGGER::LogFunc func, void *userdata)
{
	if (!initialized)
	{
		initialize ();
		initialized = true;
	}

	logFunc = func;
	userData = userdata;
}

ATOM_LOGGER::LogFunc ATOM_LOGGER::getCustomLogFunction (void)
{
	if (!initialized)
	{
		initialize ();
		initialized = true;
	}

	return logFunc;
}

void * ATOM_LOGGER::getCustomLogUserData (void)
{
	if (!initialized)
	{
		initialize ();
		initialized = true;
	}

	return userData;
}

void ATOM_LOGGER::setLogFileName (const char *filename)
{
	if (!initialized)
	{
		initialize ();
		initialized = true;
	}

	if (filename)
	{
		closeLogFile ();

		strcpy (logFileName, filename);

		openLogFile (logFileName);
	}
}

const char *ATOM_LOGGER::getLogFileName (void)
{
	if (!initialized)
	{
		initialize ();
		initialized = true;
	}

	return logFileName;
}

void ATOM_LOGGER::setLogMethods (unsigned mask)
{
	if (!initialized)
	{
		initialize ();
		initialized = true;
	}

	logMethods = mask;

	if (logMethods & ATOM_LOGGER::CONSOLE)
	{
		::AllocConsole ();
		stdoutHandle = ::GetStdHandle (STD_OUTPUT_HANDLE);
	}
	else
	{
		::FreeConsole ();
	}

	if ((logMethods & ATOM_LOGGER::FILE) != 0 && logFileName[0] && !logFile)
	{
		openLogFile (logFileName);
	}
	else if ((logMethods & ATOM_LOGGER::FILE) == 0)
	{
		closeLogFile ();
	}

	if ((logMethods & ATOM_LOGGER::PIPE) != 0 && logPipeName[0] && pipe == INVALID_HANDLE_VALUE)
	{
		openPipe (logPipeName);
	}
	else if ((logMethods & ATOM_LOGGER::PIPE) == 0)
	{
		closePipe ();
	}
}

unsigned ATOM_LOGGER::getLogMethods (void)
{
	if (!initialized)
	{
		initialize ();
		initialized = true;
	}

	return logMethods;
}

void ATOM_LOGGER::initialize (void)
{
	int argc;
	char **argv = cmdline2argv(::GetCommandLineA (), &argc);

	for (int i = 0; i < argc; ++i)
	{
		if (!strcmp (argv[i], "--logfile") && i < argc - 1)
		{
			char *filename = argv[i+1];
			strcpy (logFileName, filename);
			i++;
		}
		else if (!strcmp (argv[i], "--pipename") && i < argc - 1)
		{
			char *pipename = argv[i+1];
			strcpy (logPipeName, pipename);
			i++;
		}
		else if (!strcmp (argv[i], "--consoleout") || !strcmp (argv[i], "--log-console+"))
		{
			AllocConsole ();
			stdoutHandle = ::GetStdHandle (STD_OUTPUT_HANDLE);
			if (stdoutHandle != INVALID_HANDLE_VALUE)
			{
				logMethods |= ATOM_LOGGER::CONSOLE;
			}
		}
		else if (!strcmp (argv[i], "--log-console-"))
		{
			logMethods &= ~ATOM_LOGGER::CONSOLE;
		}
		else if (!strcmp (argv[i], "--log-file+"))
		{
			logMethods |= ATOM_LOGGER::FILE;
		}
		else if (!strcmp (argv[i], "--log-file-"))
		{
			logMethods &= ~ATOM_LOGGER::FILE;
		}
		else if (!strcmp (argv[i], "--log-debugger+"))
		{
			logMethods |= ATOM_LOGGER::DEBUGGER;
		}
		else if (!strcmp (argv[i], "--log-debugger-"))
		{
			logMethods &= ~ATOM_LOGGER::DEBUGGER;
		}
		else if (!strcmp (argv[i], "--log-pipe+"))
		{
			logMethods |= ATOM_LOGGER::PIPE;
		}
		else if (!strcmp (argv[i], "--log-pipe-"))
		{
			logMethods &= ~ATOM_LOGGER::PIPE;
		}
	}

	if ((logMethods & ATOM_LOGGER::PIPE) != 0)
	{
		if (!logPipeName[0])
		{
			strcpy (logPipeName, "\\\\10.2.1.59\\pipe\\atompipe");
		}

		openPipe (logPipeName);
	}

	if ((logMethods & ATOM_LOGGER::FILE) != 0)
	{
		if (!logFileName[0])
		{
			time_t nTime;
			time( &nTime );
			tm* tmTime = localtime( &nTime );
			::CreateDirectoryA ("log", NULL);
			sprintf(logFileName, "log/n3%d-%d-%d-%d-%d-%d.log", tmTime->tm_year+1900, tmTime->tm_mon+1, tmTime->tm_mday, tmTime->tm_hour, tmTime->tm_min, tmTime->tm_sec);
		}
		openLogFile (logFileName);
	}

	if (stdoutHandle == INVALID_HANDLE_VALUE)
	{
		char buf[32];
		::GetEnvironmentVariable("ATOM3D_ENABLE_CONSOLE_OUT", buf, 32);
		if (!stricmp (buf, "yes"))
		{
			AllocConsole ();
			stdoutHandle = ::GetStdHandle (STD_OUTPUT_HANDLE);
			if (stdoutHandle != INVALID_HANDLE_VALUE)
			{
				logMethods |= ATOM_LOGGER::CONSOLE;
			}
		}
	}
}

void ATOM_LOGGER::echo (Level level, const char *msg, bool skipDebuggerOutput)
{
	ATOM_Mutex::ScopeMutex loggerLock(getLoggerMutex());

	if (!msg)
	{
		return;
	}

	char tm[32];
	_strtime (tm);

	const char *prefix;
	switch (level)
	{
	case ATOM_LOGGER::LVL_NONE:
		prefix = "";
		break;
	case ATOM_LOGGER::LVL_INFORMATION:
		prefix = "INFO -";
		break;
	case ATOM_LOGGER::LVL_WARNING:
		prefix = "WARNNING -";
		break;
	case ATOM_LOGGER::LVL_ERROR:
		prefix = "ERROR -";
		break;
	case ATOM_LOGGER::LVL_FATAL:
		prefix = "FATAL -";
		break;
	case ATOM_LOGGER::LVL_DEBUG:
		prefix = "DEBUG -";
		break;
	default:
		return;
	}

	DWORD numWritten;

	char buffer[16384];
	_snprintf (buffer, 16384, "%s %s %s", tm, prefix, msg);
	unsigned msgLen = strlen(buffer);

	if (!skipDebuggerOutput && (logMethods & ATOM_LOGGER::DEBUGGER) != 0 && (level == LVL_NONE || filterDebugger <= level || level == LVL_DEBUG))
	{
		::OutputDebugString (buffer);
		if (ATOM_Win32DebugStream::isMonitoring())
		{
			// Skip because ATOM_Win32DebugStream will dispatch the message here again
			return;
		}
	}

	if ((logMethods & ATOM_LOGGER::FILE) != 0 && logFile && (level == LVL_NONE || filterFile <= level || level == LVL_DEBUG))
	{
		fwrite (buffer, 1, msgLen, logFile);
	}

	if ((logMethods & ATOM_LOGGER::CONSOLE) != 0 && stdoutHandle != INVALID_HANDLE_VALUE && (level == LVL_NONE || level == LVL_DEBUG || filterConsole <= level))
	{
		switch (level)
		{
		case ATOM_LOGGER::LVL_NONE:
		case ATOM_LOGGER::LVL_INFORMATION:
			break;
		case ATOM_LOGGER::LVL_WARNING:
			::SetConsoleTextAttribute (stdoutHandle, FOREGROUND_GREEN|FOREGROUND_RED);
			break;
		case ATOM_LOGGER::LVL_ERROR:
			::SetConsoleTextAttribute (stdoutHandle, FOREGROUND_RED);
			break;
		case ATOM_LOGGER::LVL_FATAL:
			::SetConsoleTextAttribute (stdoutHandle, FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY);
			break;
		case ATOM_LOGGER::LVL_DEBUG:
			::SetConsoleTextAttribute (stdoutHandle, FOREGROUND_BLUE|FOREGROUND_GREEN);
		}
		::WriteConsole (stdoutHandle, buffer, msgLen, &numWritten, 0);
		::SetConsoleTextAttribute (stdoutHandle, FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED);
	}

	if ((logMethods & ATOM_LOGGER::PIPE) != 0 && pipe != INVALID_HANDLE_VALUE && (level == LVL_NONE || filterPipe <= level))
	{
		::WriteFile (pipe, buffer, (DWORD)(msgLen + 1), &numWritten, NULL);
	}

	if ((logMethods & ATOM_LOGGER::CUSTOM) != 0 && logFunc && (level == LVL_NONE || filterCustom <= level))
	{
		logFunc (userData, buffer, msgLen, msg, level);
	}
}

bool ATOM_LOGGER::openPipe (const char *pipename)
{
	pipe = ::CreateFileA (logPipeName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (pipe == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD dwMode = PIPE_READMODE_MESSAGE;
	if (!::SetNamedPipeHandleState (pipe, &dwMode, NULL, NULL))
	{
		::CloseHandle (pipe);
		pipe = INVALID_HANDLE_VALUE;
		return false;
	}

	return true;
}

void ATOM_LOGGER::closePipe (void)
{
	if (pipe != INVALID_HANDLE_VALUE)
	{
		::CloseHandle (pipe);
		pipe = INVALID_HANDLE_VALUE;
	}
}

bool ATOM_LOGGER::openLogFile (const char *filename)
{
	logFile = fopen (filename, "wt");

	if (!logFile)
	{
		return false;
	}

	return true;
}

void ATOM_LOGGER::closeLogFile (void)
{
	if (logFile)
	{
		fclose (logFile);
		logFile = 0;
	}
}

void ATOM_LOGGER::finalize (void)
{
	closePipe ();
	closeLogFile ();
	::FreeConsole ();
}

void ATOM_LOGGER::flush (void)
{
	if (logFile)
	{
		fflush (logFile);
	}
}
