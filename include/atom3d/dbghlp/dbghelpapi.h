#ifndef __DEBUGHELP_API_H__
#define __DEBUGHELP_API_H__

#if _MSC_VER >= 1400
# include <dbghelp.h>
#else

#define UNDNAME_COMPLETE                 (0x0000)  // Enable full undecoration
#define UNDNAME_NO_LEADING_UNDERSCORES   (0x0001)  // Remove leading underscores from MS extended keywords
#define UNDNAME_NO_MS_KEYWORDS           (0x0002)  // Disable expansion of MS extended keywords
#define UNDNAME_NO_FUNCTION_RETURNS      (0x0004)  // Disable expansion of return type for primary declaration
#define UNDNAME_NO_ALLOCATION_MODEL      (0x0008)  // Disable expansion of the declaration model
#define UNDNAME_NO_ALLOCATION_LANGUAGE   (0x0010)  // Disable expansion of the declaration language specifier
#define UNDNAME_NO_MS_THISTYPE           (0x0020)  // NYI Disable expansion of MS keywords on the 'this' type for primary declaration
#define UNDNAME_NO_CV_THISTYPE           (0x0040)  // NYI Disable expansion of CV modifiers on the 'this' type for primary declaration
#define UNDNAME_NO_THISTYPE              (0x0060)  // Disable all modifiers on the 'this' type
#define UNDNAME_NO_ACCESS_SPECIFIERS     (0x0080)  // Disable expansion of access specifiers for members
#define UNDNAME_NO_THROW_SIGNATURES      (0x0100)  // Disable expansion of 'throw-signatures' for functions and pointers to functions
#define UNDNAME_NO_MEMBER_TYPE           (0x0200)  // Disable expansion of 'static' or 'virtual'ness of members
#define UNDNAME_NO_RETURN_UDT_MODEL      (0x0400)  // Disable expansion of MS model for UDT returns
#define UNDNAME_32_BIT_DECODE            (0x0800)  // Undecorate 32-bit decorated names
#define UNDNAME_NAME_ONLY                (0x1000)  // Crack only the name for primary declaration;
#define UNDNAME_NO_ARGUMENTS             (0x2000)  // Don't undecorate arguments to function
#define UNDNAME_NO_SPECIAL_SYMS          (0x4000)  // Don't undecorate special names (v-table, vcall, vector xxx, metatype, etc)

enum ADDRESS_MODE
{
  AddrMode1616,
  AddrMode1632,
  AddrModeReal,
  AddrModeFlat
};

struct ADDRESS64 
{
  DWORD64 Offset;
  WORD Segment;
  ADDRESS_MODE Mode;
};

typedef ADDRESS64* LPADDRESS64;

struct KDHELP64 
{  
  DWORD64 Thread;  
  DWORD ThCallbackStack;  
  DWORD ThCallbackBStore;  
  DWORD NextCallback;  
  DWORD FramePointer;  
  DWORD64 KiCallUserMode;  
  DWORD64 KeUserCallbackDispatcher;  
  DWORD64 SystemRangeStart;  
  DWORD64 Reserved[8];
};
typedef KDHELP64* PKDHELP64;

struct STACKFRAME64 
{  
  ADDRESS64 AddrPC;  
  ADDRESS64 AddrReturn;  
  ADDRESS64 AddrFrame;  
  ADDRESS64 AddrStack;  
  ADDRESS64 AddrBStore;  
  PVOID FuncTableEntry;  
  DWORD64 Params[4];  
  BOOL Far;  
  BOOL Virtual;  
  DWORD64 Reserved[3];  
  KDHELP64 KdHelp;
};
typedef STACKFRAME64* LPSTACKFRAME64;

typedef BOOL (WINAPI* PREAD_PROCESS_MEMORY_ROUTINE64) (HANDLE hProcess,
  DWORD64 qwBaseAddress, PVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesRead);
typedef PVOID (WINAPI* PFUNCTION_TABLE_ACCESS_ROUTINE64) (HANDLE hProcess,
  DWORD64 AddrBase);
typedef DWORD64 (WINAPI* PGET_MODULE_BASE_ROUTINE64) (HANDLE  hProcess, 
  DWORD64 Address);
typedef DWORD64 (WINAPI* PTRANSLATE_ADDRESS_ROUTINE64) (HANDLE hProcess,
  HANDLE hThread, LPADDRESS64 lpaddr);

struct SYMBOL_INFO 
{  
  ULONG SizeOfStruct;  
  ULONG TypeIndex;  
  DWORD64 Reserved[2];  
  ULONG Reserved2;  
  ULONG Size;  
  DWORD64 ModBase;  
  ULONG Flags;  
  DWORD64 Value;  
  DWORD64 Address;  
  ULONG Register;  
  ULONG Scope;  
  ULONG Tag;  
  ULONG NameLen;  
  ULONG MaxNameLen;  
  CHAR Name[1];
};
typedef SYMBOL_INFO* PSYMBOL_INFO;

struct SYMBOL_INFOW
{  
  ULONG SizeOfStruct;  
  ULONG TypeIndex;  
  DWORD64 Reserved[2];  
  ULONG Reserved2;  
  ULONG Size;  
  DWORD64 ModBase;  
  ULONG Flags;  
  DWORD64 Value;  
  DWORD64 Address;  
  ULONG Register;  
  ULONG Scope;  
  ULONG Tag;  
  ULONG NameLen;  
  ULONG MaxNameLen;  
  WCHAR Name[1];
};
typedef SYMBOL_INFOW* PSYMBOL_INFOW;

#define SYMFLAG_PARAMETER        0x00000040

#define SYMOPT_UNDNAME                  0x00000002
#define SYMOPT_DEFERRED_LOADS           0x00000004
#define SYMOPT_LOAD_LINES               0x00000010
#define SYMOPT_FAIL_CRITICAL_ERRORS     0x00000200

enum SYM_TYPE
{
  SymNone = 0,
  SymCoff,
  SymCv,
  SymPdb,
  SymExport,
  SymDeferred,
  SymSym,
  SymDia,
  SymVirtual,
  NumSymTypes
};

struct IMAGEHLP_MODULE64 
{  
  DWORD SizeOfStruct;  
  DWORD64 BaseOfImage;  
  DWORD ImageSize;  
  DWORD TimeDateStamp;  
  DWORD CheckSum;  
  DWORD NumSyms;  
  SYM_TYPE SymType;  
  CHAR ModuleName[32];  
  CHAR ImageName[256];
  CHAR LoadedImageName[256];  
/*
  The following fields are only supported on newer versions of dbghelp.dll,
  but the versions shipped with W2k resp. WXP lack them.
*/
  CHAR LoadedPdbName[256];  
  DWORD CVSig;  
  CHAR CVData[MAX_PATH*3];  
  DWORD PdbSig;  
  GUID PdbSig70;  
  DWORD PdbAge;  
  BOOL PdbUnmatched;  
  BOOL DbgUnmatched;  
  BOOL LineNumbers;  
  BOOL GlobalSymbols;  
  BOOL TypeInfo;
};
typedef IMAGEHLP_MODULE64* PIMAGEHLP_MODULE64;

struct IMAGEHLP_MODULEW64
{  
  DWORD SizeOfStruct;  
  DWORD64 BaseOfImage;  
  DWORD ImageSize;  
  DWORD TimeDateStamp;  
  DWORD CheckSum;  
  DWORD NumSyms;  
  SYM_TYPE SymType;  
  WCHAR ModuleName[32];  
  WCHAR ImageName[256];  
  WCHAR LoadedImageName[256];
  WCHAR LoadedPdbName[256];  
  DWORD CVSig;  
  WCHAR CVData[MAX_PATH*3];  
  DWORD PdbSig;  
  GUID PdbSig70;  
  DWORD PdbAge;  
  BOOL PdbUnmatched;  
  BOOL DbgUnmatched;  
  BOOL LineNumbers;  
  BOOL GlobalSymbols;  
  BOOL TypeInfo;
};
typedef IMAGEHLP_MODULEW64* PIMAGEHLP_MODULEW64;

struct IMAGEHLP_LINE64
{  
  DWORD SizeOfStruct;  
  PVOID Key;  
  DWORD LineNumber;  
  PCHAR FileName;  
  DWORD64 Address;
};
typedef IMAGEHLP_LINE64* PIMAGEHLP_LINE64;

struct IMAGEHLP_LINEW64
{  
  DWORD SizeOfStruct;  
  PVOID Key;  
  DWORD LineNumber;  
  PWCHAR FileName;  
  DWORD64 Address;
};
typedef IMAGEHLP_LINEW64* PIMAGEHLP_LINEW64;

typedef BOOL (CALLBACK* PSYM_ENUMERATESYMBOLS_CALLBACK) (PSYMBOL_INFO pSymInfo,
  ULONG SymbolSize, PVOID UserContext);
typedef BOOL (CALLBACK* PSYM_ENUMERATESYMBOLS_CALLBACKW) (PSYMBOL_INFOW pSymInfo,
  ULONG SymbolSize, PVOID UserContext);
typedef BOOL (CALLBACK* PSYM_ENUMMODULES_CALLBACK64) (PSTR ModuleName,
  DWORD64 BaseOfDll, PVOID UserContext);
typedef BOOL (CALLBACK* PSYM_ENUMMODULES_CALLBACKW64) (PWSTR ModuleName,
  DWORD64 BaseOfDll, PVOID UserContext);

struct IMAGEHLP_STACK_FRAME 
{  
  DWORD64 InstructionOffset;  
  DWORD64 ReturnOffset;  
  DWORD64 FrameOffset;  
  DWORD64 StackOffset;  
  DWORD64 BackingStoreOffset;  
  DWORD64 FuncTableEntry;  
  DWORD64 Params[4];  
  DWORD64 Reserved[5];  
  BOOL Virtual;  
  ULONG Reserved2;
};
typedef IMAGEHLP_STACK_FRAME* PIMAGEHLP_STACK_FRAME;
typedef void* PIMAGEHLP_CONTEXT;

struct MINIDUMP_EXCEPTION_INFORMATION 
{  
  DWORD ThreadId;  
  PEXCEPTION_POINTERS ExceptionPointers;  
  BOOL ClientPointers;
};
typedef MINIDUMP_EXCEPTION_INFORMATION* PMINIDUMP_EXCEPTION_INFORMATION;

struct MINIDUMP_USER_STREAM 
{  
ULONG32 Type;  
ULONG BufferSize;  
PVOID Buffer;
};
typedef MINIDUMP_USER_STREAM* PMINIDUMP_USER_STREAM;

struct MINIDUMP_USER_STREAM_INFORMATION 
{  
ULONG UserStreamCount;  
PMINIDUMP_USER_STREAM UserStreamArray;
};
typedef MINIDUMP_USER_STREAM_INFORMATION* PMINIDUMP_USER_STREAM_INFORMATION;

enum MINIDUMP_CALLBACK_TYPE
{
ModuleCallback, 
ThreadCallback, 
ThreadExCallback, 
IncludeThreadCallback, 
IncludeModuleCallback
};

struct MINIDUMP_THREAD_CALLBACK 
{  
ULONG ThreadId;  
HANDLE ThreadHandle;  
CONTEXT Context;  
ULONG SizeOfContext;  
ULONG64 StackBase;  
ULONG64 StackEnd;
};
typedef MINIDUMP_THREAD_CALLBACK* PMINIDUMP_THREAD_CALLBACK;

struct MINIDUMP_THREAD_EX_CALLBACK 
{  
ULONG ThreadId;  
HANDLE ThreadHandle;  
CONTEXT Context;  
ULONG SizeOfContext;  
ULONG64 StackBase;  
ULONG64 StackEnd;  
ULONG64 BackingStoreBase;  
ULONG64 BackingStoreEnd;
};
typedef MINIDUMP_THREAD_EX_CALLBACK* PMINIDUMP_THREAD_EX_CALLBACK;

#include <winver.h>

struct MINIDUMP_MODULE_CALLBACK 
{  
PWCHAR FullPath;  
ULONG64 BaseOfImage;  
ULONG SizeOfImage;  
ULONG CheckSum;  
ULONG TimeDateStamp;  
VS_FIXEDFILEINFO VersionInfo;  
PVOID CvRecord;  
ULONG SizeOfCvRecord;  
PVOID MiscRecord;  
ULONG SizeOfMiscRecord;
};
typedef MINIDUMP_MODULE_CALLBACK* PMINIDUMP_MODULE_CALLBACK;

struct MINIDUMP_INCLUDE_THREAD_CALLBACK 
{  
ULONG ThreadId;
};
typedef MINIDUMP_INCLUDE_THREAD_CALLBACK* PMINIDUMP_INCLUDE_THREAD_CALLBACK;

struct MINIDUMP_INCLUDE_MODULE_CALLBACK 
{  
ULONG64 BaseOfImage;
};
typedef MINIDUMP_INCLUDE_MODULE_CALLBACK* PMINIDUMP_INCLUDE_MODULE_CALLBACK;


struct MINIDUMP_CALLBACK_INPUT       
{  
  ULONG ProcessId;  
  HANDLE ProcessHandle;  
  ULONG CallbackType;  
  union 
  {    
    MINIDUMP_THREAD_CALLBACK Thread;    
    MINIDUMP_THREAD_EX_CALLBACK ThreadEx;    
    MINIDUMP_MODULE_CALLBACK Module;    
    MINIDUMP_INCLUDE_THREAD_CALLBACK IncludeThread;    
    MINIDUMP_INCLUDE_MODULE_CALLBACK IncludeModule;  
  };
};

typedef MINIDUMP_CALLBACK_INPUT* PMINIDUMP_CALLBACK_INPUT;

enum MODULE_WRITE_FLAGS
{
  ModuleWriteModule                     = 0x0001, 
  ModuleWriteDataSeg                    = 0x0002, 
  ModuleWriteMiscRecord                 = 0x0004, 
  ModuleWriteCvRecord                   = 0x0008, 
  ModuleReferencedByMemory              = 0x0010
  };

enum THREAD_WRITE_FLAGS
{
  ThreadWriteThread                     = 0x0001, 
  ThreadWriteStack                      = 0x0002, 
  ThreadWriteContext                    = 0x0004, 
  ThreadWriteBackingStore               = 0x0008, 
  ThreadWriteInstructionWindow          = 0x0010, 
  ThreadWriteThreadData                 = 0x0020
};
 
struct MINIDUMP_CALLBACK_OUTPUT 
{  
  union 
  {    
    ULONG ModuleWriteFlags;    
    ULONG ThreadWriteFlags;  
  };
}; 
typedef MINIDUMP_CALLBACK_OUTPUT* PMINIDUMP_CALLBACK_OUTPUT;
 
typedef BOOL (CALLBACK* MINIDUMP_CALLBACK_ROUTINE) (PVOID CallbackParam, 
   const PMINIDUMP_CALLBACK_INPUT CallbackInput, 
   PMINIDUMP_CALLBACK_OUTPUT CallbackOutput);
 
struct MINIDUMP_CALLBACK_INFORMATION 
{  
  MINIDUMP_CALLBACK_ROUTINE CallbackRoutine;  
  PVOID CallbackParam;
};
typedef MINIDUMP_CALLBACK_INFORMATION* PMINIDUMP_CALLBACK_INFORMATION;
 
enum MINIDUMP_TYPE
{
  MiniDumpNormal                          = 0x0000, 
  MiniDumpWithDataSegs                    = 0x0001, 
  MiniDumpWithFullMemory                  = 0x0002, 
  MiniDumpWithHandleData                  = 0x0004, 
  MiniDumpFilterMemory                    = 0x0008, 
  MiniDumpScanMemory                      = 0x0010, 
  MiniDumpWithUnloadedModules             = 0x0020, 
  MiniDumpWithIndirectlyReferencedMemory  = 0x0040, 
  MiniDumpFilterModulePaths               = 0x0080, 
  MiniDumpWithProcessThreadData           = 0x0100, 
  MiniDumpWithPrivateReadWriteMemory      = 0x0200
};
 
enum MINIDUMP_STREAM_TYPE
{
  UnusedStream             = 0, 
  ReservedStream0          = 1, 
  ReservedStream1          = 2, 
  ThreadListStream         = 3, 
  ModuleListStream         = 4, 
  MemoryListStream         = 5, 
  ExceptionStream          = 6, 
  SystemInfoStream         = 7, 
  ThreadExListStream       = 8, 
  Memory64ListStream       = 9, 
  CommentStreamA           = 10, 
  CommentStreamW           = 11, 
  HandleDataStream         = 12, 
  FunctionTableStream      = 13, 
  UnloadedModuleListStream = 14, 
  MiscInfoStream           = 15, 
  LastReservedStream = 0xffff
};
 
struct MINIDUMP_LOCATION_DESCRIPTOR 
{  
  DWORD DataSize;  
  DWORD Rva;
};

struct MINIDUMP_MEMORY_DESCRIPTOR 
{  
  DWORD64 StartOfMemoryRange;  
  MINIDUMP_LOCATION_DESCRIPTOR Memory;
};
typedef MINIDUMP_MEMORY_DESCRIPTOR* PMINIDUMP_MEMORY_DESCRIPTOR;

struct MINIDUMP_MEMORY_LIST 
{  
  ULONG32 NumberOfMemoryRanges;  
  /*MINIDUMP_MEMORY_DESCRIPTOR MemoryRanges[];*/
};
typedef MINIDUMP_MEMORY_LIST* PMINIDUMP_MEMORY_LIST;

struct MINIDUMP_HEADER 
{
  ULONG32 Signature;
  ULONG32 Version;
  ULONG32 NumberOfStreams;
  DWORD StreamDirectoryRva;
  ULONG32 CheckSum;
  union 
  {
    ULONG32 Reserved;
    ULONG32 TimeDateStamp;
  };
  DWORD64 Flags;
};
typedef MINIDUMP_HEADER* PMINIDUMP_HEADER;
 
struct MINIDUMP_DIRECTORY 
{
  ULONG32 StreamType;
  MINIDUMP_LOCATION_DESCRIPTOR Location;
};
typedef MINIDUMP_DIRECTORY* PMINIDUMP_DIRECTORY;

typedef struct _IMAGEHLP_SYMBOL64 {
    DWORD                       SizeOfStruct;           // set to sizeof(IMAGEHLP_SYMBOL64)
    DWORD64                     Address;                // virtual address including dll base address
    DWORD                       Size;                   // estimated size of symbol, can be zero
    DWORD                       Flags;                  // info about the symbols, see the SYMF defines
    DWORD                       MaxNameLength;          // maximum size of symbol name in 'Name'
    CHAR                        Name[1];                // symbol name (null terminated string)
} IMAGEHLP_SYMBOL64, *PIMAGEHLP_SYMBOL64;

#endif

#endif //__DEBUGHELP_API_H__
