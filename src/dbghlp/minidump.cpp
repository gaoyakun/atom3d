#include "StdAfx.h"
#include <stdexcept>
#include "minidump.h"
#include "debugutils.h"
#include "dbghelpapi.h"

#define MAX_USER_STREAM_COUNT 8

static bool MiniDumpWrite (const char *filename, ATOM_MiniDumpUserStreams *userstreams, EXCEPTION_POINTERS *pExp, unsigned type)
{
  HMODULE hDbgHlp = ATOM_LoadDebugHelpDll ();
  if (!hDbgHlp)
    return false;

  typedef BOOL (WINAPI *MiniDumpWriteDumpFunc) (HANDLE,DWORD,HANDLE,MINIDUMP_TYPE,PMINIDUMP_EXCEPTION_INFORMATION,PMINIDUMP_USER_STREAM_INFORMATION,PMINIDUMP_CALLBACK_INFORMATION);
  MiniDumpWriteDumpFunc mdwd = (MiniDumpWriteDumpFunc)::GetProcAddress(hDbgHlp, "MiniDumpWriteDump");
  if (!mdwd)
  {
    ::FreeLibrary (hDbgHlp);
    return false;
  }

  HANDLE hFile = ::CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    ::FreeLibrary (hDbgHlp);
    return false;
  }

  MINIDUMP_EXCEPTION_INFORMATION ExInfo;
  ExInfo.ThreadId = ::GetCurrentThreadId ();
  ExInfo.ExceptionPointers = pExp;
  ExInfo.ClientPointers = FALSE;
  MINIDUMP_USER_STREAM_INFORMATION UserStreamInformation;
  MINIDUMP_USER_STREAM_INFORMATION *pUserStreamInformation = 0;
  MINIDUMP_USER_STREAM streamArray[MAX_USER_STREAM_COUNT];
  UserStreamInformation.UserStreamCount = userstreams ? userstreams->GetNumStreams() : 0;

  if (UserStreamInformation.UserStreamCount)
  {
	if (UserStreamInformation.UserStreamCount > MAX_USER_STREAM_COUNT)
	{
	  UserStreamInformation.UserStreamCount = MAX_USER_STREAM_COUNT;
	}
	pUserStreamInformation = &UserStreamInformation;
    UserStreamInformation.UserStreamArray = streamArray;
    for (unsigned i = 0; i < UserStreamInformation.UserStreamCount; ++i)
    {
      ATOM_MiniDumpUserStreams::UserStreamInfo usi;
      userstreams->GetStream (i, &usi);
      UserStreamInformation.UserStreamArray[i].Type = LastReservedStream + usi.type;
      UserStreamInformation.UserStreamArray[i].BufferSize = usi.buffersize;
      UserStreamInformation.UserStreamArray[i].Buffer = usi.buffer;
    }
  }

  BOOL ok = mdwd(::GetCurrentProcess(),::GetCurrentProcessId(),hFile,(MINIDUMP_TYPE)type,&ExInfo,pUserStreamInformation,NULL);

  if (!ok)
  {
	char errMsg[256];
	char outputMsg[512];

	DWORD err = ::GetLastError ();
	::FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM, NULL, 0, 0, errMsg, sizeof(errMsg), NULL);
	sprintf (outputMsg, "保存转储文件失败: %s", errMsg);
	::MessageBox (::GetActiveWindow(), outputMsg, "错误", MB_OK|MB_ICONHAND);
  }

  ::CloseHandle (hFile);
  ::FreeLibrary (hDbgHlp);
  return ok != FALSE;
}

static bool MiniDumpWrite (const char *filename, int processId, ATOM_MiniDumpUserStreams *userstreams, unsigned type)
{
  HANDLE hProcess = ::OpenProcess (PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, processId);
  if (hProcess == NULL)
  {
	  return false;
  }

  HMODULE hDbgHlp = ATOM_LoadDebugHelpDll ();
  if (!hDbgHlp)
  {
	::CloseHandle (hProcess);
	return false;
  }

  typedef BOOL (WINAPI *MiniDumpWriteDumpFunc) (HANDLE,DWORD,HANDLE,MINIDUMP_TYPE,PMINIDUMP_EXCEPTION_INFORMATION,PMINIDUMP_USER_STREAM_INFORMATION,PMINIDUMP_CALLBACK_INFORMATION);
  MiniDumpWriteDumpFunc mdwd = (MiniDumpWriteDumpFunc)::GetProcAddress(hDbgHlp, "MiniDumpWriteDump");
  if (!mdwd)
  {
    ::FreeLibrary (hDbgHlp);
	::CloseHandle (hProcess);
    return false;
  }

  HANDLE hFile = ::CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    ::FreeLibrary (hDbgHlp);
	::CloseHandle (hProcess);
    return false;
  }

  MINIDUMP_USER_STREAM_INFORMATION UserStreamInformation;
  MINIDUMP_USER_STREAM_INFORMATION *pUserStreamInformation = 0;
  MINIDUMP_USER_STREAM streamArray[MAX_USER_STREAM_COUNT];
  UserStreamInformation.UserStreamCount = userstreams ? userstreams->GetNumStreams() : 0;

  if (UserStreamInformation.UserStreamCount)
  {
	if (UserStreamInformation.UserStreamCount > MAX_USER_STREAM_COUNT)
	{
	  UserStreamInformation.UserStreamCount = MAX_USER_STREAM_COUNT;
	}
	pUserStreamInformation = &UserStreamInformation;
    UserStreamInformation.UserStreamArray = streamArray;
    for (unsigned i = 0; i < UserStreamInformation.UserStreamCount; ++i)
    {
      ATOM_MiniDumpUserStreams::UserStreamInfo usi;
      userstreams->GetStream (i, &usi);
      UserStreamInformation.UserStreamArray[i].Type = LastReservedStream + usi.type;
      UserStreamInformation.UserStreamArray[i].BufferSize = usi.buffersize;
      UserStreamInformation.UserStreamArray[i].Buffer = usi.buffer;
    }
  }

  BOOL ok = mdwd(hProcess, processId, hFile, (MINIDUMP_TYPE)type, 0, pUserStreamInformation, NULL);

  ::CloseHandle (hProcess);
  ::CloseHandle (hFile);
  ::FreeLibrary (hDbgHlp);
  return ok != FALSE;
}

//static LONG MiniDumpExpHandler (const char *filename, EXCEPTION_POINTERS *pExp, unsigned type)
//{
//  MiniDumpWrite (filename, pExp, type);
//  return EXCEPTION_EXECUTE_HANDLER;
//}

ATOM_DBGHLP_API bool ATOM_WriteMiniDump (const char *filename, ATOM_MiniDumpUserStreams *userstreams, EXCEPTION_POINTERS *pExp, unsigned type)
{
  if (pExp)
  {
    return MiniDumpWrite (filename, userstreams, pExp, type);
  }
  else
  {
#if defined(__MINGW32__)

    return false;

#else

    bool ok = false;

    __try
    {
      ::RaiseException (1, 0, 0, NULL);
    }
    __except((ok = MiniDumpWrite(filename, userstreams, GetExceptionInformation(), type), EXCEPTION_EXECUTE_HANDLER))
    {
    }

    return ok;
    
#endif
  }
}

static int GetProcessIdByName (const char *name)
{
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot (TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (!::Process32First (hProcessSnap, &pe32))
	{
		::CloseHandle (hProcessSnap);
		return 0;
	}
	for (;;)
	{
		if (!_stricmp (pe32.szExeFile, name))
		{
			::CloseHandle (hProcessSnap);
			return pe32.th32ProcessID;
		}
		if (!::Process32Next (hProcessSnap, &pe32))
		{
			::CloseHandle (hProcessSnap);
			return 0;
		}
	}
}

ATOM_DBGHLP_API bool ATOM_WriteMiniDump (const char *filename, int processId, ATOM_MiniDumpUserStreams *userstreams, unsigned type)
{
	if (processId == 0)
	{
		return ATOM_WriteMiniDump (filename, userstreams, 0, type);
	}
	else
	{
		return MiniDumpWrite (filename, processId, userstreams, type);
	}
}

ATOM_DBGHLP_API bool ATOM_WriteMiniDump (const char *filename, const char *processName, ATOM_MiniDumpUserStreams *userstreams, unsigned type)
{
	int processId = processName ? GetProcessIdByName (processName) : 0;
	return ATOM_WriteMiniDump (filename, processId, userstreams, type);
}

ATOM_DBGHLP_API bool ATOM_ReadMiniDumpUserStream (const char *filename, ATOM_MiniDumpUserStreams::UserStreamInfo *info)
{
  if (!info || !info->buffersize || !info->buffer)
  {
    return false;
  }

  HMODULE hDbgHlp = ATOM_LoadDebugHelpDll ();
  if (!hDbgHlp)
    return false;

  typedef BOOL (WINAPI *MiniDumpReadDumpStreamFunc) (PVOID,ULONG,PMINIDUMP_DIRECTORY*,PVOID*,ULONG*);
  MiniDumpReadDumpStreamFunc mdrds = (MiniDumpReadDumpStreamFunc)::GetProcAddress(hDbgHlp, "MiniDumpReadDumpStream");
  if (!mdrds)
  {
    ::FreeLibrary (hDbgHlp);
    return false;
  }

  HANDLE hFile = ::CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    ::FreeLibrary (hDbgHlp);
    return false;
  }

  HANDLE hFileMapping = ::CreateFileMapping (hFile, NULL, PAGE_READONLY, 0, 0, NULL);
  if (hFileMapping == NULL || ::GetLastError () == ERROR_ALREADY_EXISTS)
  {
    ::CloseHandle (hFile);
    ::FreeLibrary (hDbgHlp);
    return false;
  }

  void *imagebase = ::MapViewOfFile (hFileMapping, FILE_MAP_READ, 0, 0, 0);
  if (!imagebase)
  {
    ::CloseHandle (hFileMapping);
    ::CloseHandle (hFile);
    ::FreeLibrary (hDbgHlp);
    return false;
  }

  PMINIDUMP_DIRECTORY dir = 0;
  PVOID streamBuffer = 0;
  ULONG streamSize = 0;

  BOOL ok = mdrds (imagebase, LastReservedStream - info->type, &dir, &streamBuffer, &streamSize);

  if (!ok || info->buffersize < streamSize)
  {
    ::UnmapViewOfFile (imagebase);
    ::CloseHandle (hFileMapping);
    ::CloseHandle (hFile);
    ::FreeLibrary (hDbgHlp);
    return false;
  }

  memcpy (info->buffer, streamBuffer, streamSize);
  info->buffersize = streamSize;

  ::UnmapViewOfFile (imagebase);
  ::CloseHandle (hFileMapping);
  ::CloseHandle (hFile);
  ::FreeLibrary (hDbgHlp);
  return true;
}

class MyMiniDumpUserStreams: public ATOM_MiniDumpUserStreams
{
	int _version;
	const std::vector<std::pair<int, std::string> > *_comments;

public:
	MyMiniDumpUserStreams (int version, const std::vector<std::pair<int, std::string> > *comments = 0): _version (version), _comments(comments)
	{
	}

	virtual unsigned GetNumStreams (void) const 
	{ 
		return 1 + (_comments ? _comments->size() : 0); 
	}

	virtual void GetStream (unsigned index, UserStreamInfo *streaminfo) 
	{
		if (index == 0 && streaminfo)
		{
			streaminfo->type = 0x1;
			streaminfo->buffersize = 4;
			streaminfo->buffer = &_version;
		}
		else if (_comments && _comments->size() >= index)
		{
			streaminfo->type = (*_comments)[index-1].first;
			streaminfo->buffersize = 1 + (*_comments)[index-1].second.length();
			streaminfo->buffer = (void*)(*_comments)[index-1].second.c_str();
		}
	}
};
static ATOM_MiniDumpWriter *dumpWriterInstance = 0;

ATOM_MiniDumpWriter::ATOM_MiniDumpWriter (const char *filename, bool fulldump)
{
	if (dumpWriterInstance)
	{
		throw std::runtime_error ("Only one mini dump writer instance is allowed.");
	}
	dumpWriterInstance = this;

	setFileName (filename);
	_version = 0;
	_writeVersion = false;
	_fulldump = fulldump;

	::SetUnhandledExceptionFilter (&ATOM_MiniDumpWriter::dumpHandler);
}

ATOM_MiniDumpWriter::ATOM_MiniDumpWriter (int versionNumber, bool writeVersionNumber, const char *filename, bool fulldump)
{
	if (dumpWriterInstance)
	{
		throw std::runtime_error ("Only one mini dump writer instance is allowed.");
	}
	dumpWriterInstance = this;

	setFileName (filename);
	_version = versionNumber;
	_writeVersion = writeVersionNumber;
	_fulldump = fulldump;

	::SetUnhandledExceptionFilter (&ATOM_MiniDumpWriter::dumpHandler);
}

ATOM_MiniDumpWriter::~ATOM_MiniDumpWriter (void)
{
	::SetUnhandledExceptionFilter (NULL);

	dumpWriterInstance = 0;
}

static char myStack[2048 * 1024];
static char *__myesp = myStack + sizeof(myStack);
static unsigned long __esp;

LONG WINAPI ATOM_MiniDumpWriter::dumpHandler (_EXCEPTION_POINTERS *exceptionInfo)
{
	_EXCEPTION_POINTERS * volatile e = exceptionInfo;

	if (dumpWriterInstance->beginWriteDump ())
	{
		bool succ;

		unsigned type = dumpWriterInstance->getFullDump() ? MDT_WITHFULLMEMORY : MDT_WITHINDIRECTLYREFERENCEDMEMORY;

		if (dumpWriterInstance->getWriteVersion())
		{
			MyMiniDumpUserStreams versionStream(dumpWriterInstance->getVersionNumber(), dumpWriterInstance->getComments());
			succ = ATOM_WriteMiniDump (dumpWriterInstance->getFileName(), &versionStream, e, type);
		}
		else
		{
			succ = ATOM_WriteMiniDump (dumpWriterInstance->getFileName(), 0, e, type);
		}

		dumpWriterInstance->endWriteDump (succ);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void ATOM_MiniDumpWriter::setVersionNumber (int version)
{
	_version = version;
}

int ATOM_MiniDumpWriter::getVersionNumber (void) const
{
	return _version;
}

void ATOM_MiniDumpWriter::setWriteVersion (bool writeVersion)
{
	_writeVersion = writeVersion;
}

bool ATOM_MiniDumpWriter::getWriteVersion (void) const
{
	return _writeVersion;
}

void ATOM_MiniDumpWriter::setFileName (const char *filename)
{
	if (filename && filename[0] && strlen(filename) < 512)
	{
		strcpy (_filename, filename);
	}
	else
	{
		strcpy (_filename, "crash.dmp");
	}
}

const char *ATOM_MiniDumpWriter::getFileName (void) const
{
	return _filename;
}

void ATOM_MiniDumpWriter::addComment (int id, const char *comment)
{
	_comments.resize (_comments.size() + 1);
	_comments.back().first = id;
	_comments.back().second = comment;
}

const std::vector<std::pair<int, std::string> > *ATOM_MiniDumpWriter::getComments (void) const
{
	return &_comments;
}

bool ATOM_MiniDumpWriter::snapshot (void)
{
	if (beginSnapShot ())
	{
		bool succ;

		unsigned type = getFullDump() ? MDT_WITHFULLMEMORY : MDT_WITHINDIRECTLYREFERENCEDMEMORY;

		if (getWriteVersion())
		{
			MyMiniDumpUserStreams versionStream(getVersionNumber(), getComments());
			succ = ATOM_WriteMiniDump (getFileName(), &versionStream, 0, type);
		}
		else
		{
			succ = ATOM_WriteMiniDump (getFileName(), 0, 0, type);
		}

		endSnapShot (succ);

		return succ;
	}

	return false;
}

bool ATOM_MiniDumpWriter::beginWriteDump (void)
{
	return true;
}

void ATOM_MiniDumpWriter::endWriteDump (bool succeeded)
{
}

bool ATOM_MiniDumpWriter::beginSnapShot (void)
{
	return true;
}

void ATOM_MiniDumpWriter::endSnapShot (bool succeeded)
{
}

void ATOM_MiniDumpWriter::setFullDump (bool b)
{
	_fulldump = b;
}

bool ATOM_MiniDumpWriter::getFullDump (void) const
{
	return _fulldump;
}
