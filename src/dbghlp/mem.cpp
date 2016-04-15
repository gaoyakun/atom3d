#include "StdAfx.h"

#include "basedefs.h"
#include "utils.h"
#include "mem.h"
#include "memlog.h"
#include "callstack.h"
#include "mem_config.h"
#include "mempool.h"

#if ATOM3D_PLATFORM_MINGW
# define VOLATILE
#else
# define VOLATILE volatile
#endif

void * (ATOM_CALL *MallocFunc) (size_t) = 0;
void * (ATOM_CALL *ReallocFunc) (void *, size_t) = 0;
void (ATOM_CALL *FreeFunc) (void *) = 0;
VOLATILE long DebugMalloc = 0;
VOLATILE long BreakOnMemError = 1;
VOLATILE long UseSymServer = 0;
VOLATILE long Determined = 0;
VOLATILE long AllocCounterAlert = 1000;

typedef USHORT (WINAPI *CaptureStackBackTraceFunc)(ULONG,ULONG,PVOID*,PULONG);
static HMODULE hNtDll = ::GetModuleHandle ("NTDLL.DLL");
static CaptureStackBackTraceFunc pfnCaptureStackBackTrace = (CaptureStackBackTraceFunc)::GetProcAddress(hNtDll, "RtlCaptureStackBackTrace");

static ATOM_MemLog * _memlog (void)
{
  static ATOM_MemLog *memlog = new ATOM_MemLog;
  return memlog;
}

static ATOM_MemoryPool *mempool = 0;

ATOM_MemoryPool *_GetMemoryPool (void)
{
	if (!mempool)
	{
		mempool = new ATOM_MemoryPool(5 * 1024 * 1024);
	}
	return mempool;
}

static const char *prefixbuffer = 
"prefix = 0x%08X\n"
"prefix.mark = 0x%08X\n"
"prefix.entry = 0x%08X\n"
"prefix.blockentry.block = 0x%08X\n"
"prefix.blockentry.size = 0x%08X\n"
"prefix.blockentry.backtrace = {0x%08X,0x%08X,0x%08X,0x%08X,0x%08X,0x%08X,0x%08X,0x%08X}\n"
"prefix.blockentry.num_frames = 0x%08X\n"
"prefix.blockentry.next = 0x%08X\n"
"prefix.blockentry.prev = 0x%08X\n"
"prefix.guard = 0x%08X\n"
"prefix.prefix = 0x%08X\n";

static void getGuard (void *p, char *buffer)
{
  try
  {
    ATOM_BlockPrefix *b = (ATOM_BlockPrefix*)((char*)p - sizeof(ATOM_BlockPrefix));
    ATOM_BlockGuard *guard = (ATOM_BlockGuard*)((char*)p + b->blockentry.size - sizeof(ATOM_BlockPrefix) - sizeof(ATOM_BlockGuard));
    ATOM_DumpMemoryAsByte (guard, sizeof(ATOM_BlockGuard), buffer);
  }
  catch (...)
  {
    strcpy (buffer, "(not valid)\n");
  }
}

static void getPrefix (void *p, char *buffer)
{
  ATOM_BlockPrefix *b = (ATOM_BlockPrefix*)((char*)p - sizeof(ATOM_BlockPrefix));
  try
  {
    sprintf (buffer, prefixbuffer,
      b,
      b->mark,
      b->entry,
      b->blockentry.block,
      b->blockentry.size,
      b->blockentry.backtrace[0],
      b->blockentry.backtrace[1],
      b->blockentry.backtrace[2],
      b->blockentry.backtrace[3],
      b->blockentry.backtrace[4],
      b->blockentry.backtrace[5],
      b->blockentry.backtrace[6],
      b->blockentry.backtrace[7],
      b->blockentry.num_frames,
      b->blockentry.next,
      b->blockentry.prev,
      b->guard,
      b->prefix);
  }
  catch(...)
  {
    strcpy (buffer, "(not valid)");
  }
}

static void getFileName (void *p, char *filename)
{
  ATOM_BlockPrefix *b = (ATOM_BlockPrefix*)((char*)p - sizeof(ATOM_BlockPrefix));
  try
  {
    char *fn = filename;
    for (; *b->entry->filename && fn < filename+255;)
    {
      *fn++ = *b->entry->filename++;
    }
    *fn = '\0';
  }
  catch(...)
  {
    strcpy (filename, "(not valid)");
  }
}

static void getLineNo (void *p, char *lineno)
{
  ATOM_BlockPrefix *b = (ATOM_BlockPrefix*)((char*)p - sizeof(ATOM_BlockPrefix));
  try
  {
    _itoa (b->entry->lineno, lineno, 10);
  }
  catch(...)
  {
    strcpy (lineno, "(not valid)");
  }
}

void initGuard (ATOM_BlockGuard *guard)
{
  unsigned i;
  for (i = 0; i < ATOM_GUARD_SIZE; ++i)
  {
    guard->guard[i] = ATOM_BLOCK_GUARD;
  }
}

int testGuard (ATOM_BlockGuard *guard)
{
  try
  {
    unsigned i;
    for (i = 0; i < ATOM_GUARD_SIZE; ++i)
    {
      if (guard->guard[i] != ATOM_BLOCK_GUARD)
      {
        return 0;
      }
    }
    return 1;
  }
  catch(...)
  {
    return 0;
  }
}

int TestBlock (void *p, char *error)
{
  ATOM_BlockPrefix *block = *(((ATOM_BlockPrefix**)p)-1);
  int ret = 0;
  char buffer[2048];

  try
  {
    if (((char*)p - (char*)block) != sizeof(ATOM_BlockPrefix) || block->mark != ATOM_BLOCK_MARK)
    {
      ret = 1;

      if (error)
      {
        sprintf(error, "invalid block 0x%08X:\r\n", p);

        // Get prefix content
        getPrefix (p, buffer);
        strcat(error, buffer);
        strcat(error, "filename: ");

        // Get filename and line number
        //ATOM_BlockPrefix *b = (ATOM_BlockPrefix*)((char*)p - sizeof(ATOM_BlockPrefix));
        getFileName (p, buffer);
        strcat (error, buffer);
        strcat (error, "\r\nline: ");
        getLineNo (p, buffer);
        strcat (error, buffer);
      }
    }
    else
    {
      ATOM_BlockGuard *guard = (ATOM_BlockGuard*)((char*)block->blockentry.block + block->blockentry.size - sizeof(ATOM_BlockPrefix) - sizeof(ATOM_BlockGuard));
      if (!testGuard (guard))
      {
        ret = 2;

        // Bounds check failed
        if (error)
        {
          sprintf (error, "block 0x%08X bounds check failed\r\n", p);

          // Get guard content
          getGuard (p, buffer);
          strcat (error, buffer);
          strcat (error, "filename");

          // Get filename and line number
          getFileName (p, buffer);
          strcat (error, buffer);
          strcat (error, "\r\nline: ");
          getLineNo (p, buffer);
          strcat (error, buffer);
        }
      }
    }
  }
  catch(...)
  {
    if (error)
    {
      strcat (error, "content not valid");
    }
    ret = 3;
  }

  return ret;
}

void (ATOM_CALL *ErrorHandler)(const char*, const char*, int, int fatal) = 0;

static void Error (const char *msg, const char *filename, int lineno, int fatal)
{
  ATOM_GetErrorHandler() (msg, filename, lineno, fatal);
}

static void * __MallocD(size_t size, const char *filename, int lineno)
{
#if 0
  return ::malloc (size);
#else
  size_t size2 = size + sizeof (ATOM_BlockPrefix) + sizeof(ATOM_BlockGuard);
  ATOM_BlockPrefix *p = (ATOM_BlockPrefix*)MallocFunc(size2);
  if (p)
  {
    ATOM_BlockGuard *guard = (ATOM_BlockGuard*)(((char*)p) + sizeof(ATOM_BlockPrefix) + size);
    initGuard (guard);

    p->mark = ATOM_BLOCK_MARK;
    p->blockentry.block = p+1;
    p->blockentry.size = size2;
    p->blockentry.num_frames = pfnCaptureStackBackTrace ? pfnCaptureStackBackTrace(0, ATOM_MAX_BACKTRACE_DEPTH, p->blockentry.backtrace, 0) : 0;
    p->entry = _memlog()->acquire_entry (&p->blockentry, filename, lineno);
	if (p->entry->counter == 1)
	{
		p->entry->counterAlert = AllocCounterAlert;
	}
    p->prefix = p;
    p->guard = guard;
    p++;
  }

  return p;
#endif
}

static void * __ReallocD(void *p, size_t size, const char *filename, int lineno)
{
#if 0
  return ::realloc (p, size);
#else
  void *newblock;

  if (!p)
  {
    newblock = __MallocD (size, filename, lineno);
  }
  else
  {
    char errmsg[1024];
    if (TestBlock (p, errmsg))
    {
      Error(errmsg, filename, lineno, true);
      return 0;
    }
    else
    {
      ATOM_BlockPrefix *block = *(((ATOM_BlockPrefix**)p)-1);
      _memlog()->release_entry (&block->blockentry, block->entry);

      size_t size2 = size + sizeof(ATOM_BlockPrefix) + sizeof(ATOM_BlockGuard);
      block = (ATOM_BlockPrefix*)ReallocFunc(block, size2);
      if (block)
      {
        ATOM_BlockGuard *guard = (ATOM_BlockGuard*)(((char*)block) + sizeof(ATOM_BlockPrefix) + size);
        initGuard (guard);

        block->mark = ATOM_BLOCK_MARK;
        block->blockentry.block = block+1;
        block->blockentry.size = size2;
        block->blockentry.num_frames = pfnCaptureStackBackTrace ? pfnCaptureStackBackTrace(0, ATOM_MAX_BACKTRACE_DEPTH, block->blockentry.backtrace, 0) : 0;
        block->entry = _memlog()->acquire_entry (&block->blockentry, filename, lineno);
        block->prefix = block;
        block->guard = guard;
        block++;
      }
      newblock = block;
    }
  }

  return newblock;
#endif
}

static void  __FreeD (void *p, const char *filename, int lineno)
{
#if 0
  free (p);
#else
  if (p)
  {
    char errmsg[1024];

    if (TestBlock (p, errmsg))
    {
      Error(errmsg, filename, lineno, false);
      return;
    }

    ATOM_BlockPrefix *block = *(((ATOM_BlockPrefix**)p)-1);
    _memlog()->release_entry (&block->blockentry, block->entry);

    FreeFunc(block);
  }
#endif
}

ATOM_DBGHLP_API int ATOM_CALL ATOM_DumpMemoryState (const char *filename, int fulldump)
{
  return (fulldump ? _memlog()->full_dump(filename) : _memlog()->dump (filename)) ? 1 : 0;
}

ATOM_DBGHLP_API int  ATOM_CALL ATOM_DumpMemoryBlock (const char *filename, void *block, bool userblock)
{
	return _memlog()->dump_block(filename, block, userblock) ? 1 : 0;
}

ATOM_DBGHLP_API int  ATOM_CALL ATOM_DumpMemoryBlockToMemory (char *buffer, void *block, bool userblock)
{
	return _memlog()->dump_block_to_memory(buffer, block, userblock) ? 1 : 0;
}

ATOM_DBGHLP_API int ATOM_CALL ATOM_DumpMemoryDifference (const char *filename1, const char *filename2, const char *output_filename)
{
  return _memlog()->dump_difference (filename1, filename2, output_filename) ? 1 : 0;
}

struct BlockPrefixOpt
{
  size_t mark;
  ATOM_BlockPrefix *prefix;
};

ATOM_DBGHLP_API void * ATOM_CALL ATOM_Malloc(size_t size, const char *filename, int lineno)
{
#if 0
  return ::malloc (size);
#else
  if (!Determined)
  {
    ATOM_DetermineMallocAlgorithm ();
    InterlockedExchange ((LONG*)&Determined, 1);
  }

  if (DebugMalloc)
  {
    return __MallocD (size, filename, lineno);
  }
  else
  {
    BlockPrefixOpt *p = (BlockPrefixOpt*)MallocFunc(size + sizeof(BlockPrefixOpt));
    if (p)
    {
      p->mark = ATOM_BLOCK_MARK;
      p->prefix = ATOM_NON_PREFIX_MARK;
      p++;
    }
    return p;
  }
#endif
}

ATOM_DBGHLP_API void * ATOM_CALL ATOM_Realloc(void *p, size_t size, const char *filename, int lineno)
{
#if 0
  return ::realloc (p, size);
#else
  if (p)
  {
    if (!Determined)
    {
      ATOM_DetermineMallocAlgorithm ();
      InterlockedExchange ((LONG*)&Determined, 1);
    }

    if (DebugMalloc)
    {
      return __ReallocD (p, size, filename, lineno);
    }
    else
    {
      BlockPrefixOpt *prefix = ((BlockPrefixOpt*)p)-1;
      if (prefix->mark != ATOM_BLOCK_MARK)
      {
        // invalid or corrupt block, crash it or break into the debugger
        char buffer[256];
        sprintf (buffer, "Invalid block mark: %d\n", prefix->mark);
        Error (buffer, "(no filename)", 0, true);
      }

      prefix = (BlockPrefixOpt*)ReallocFunc(prefix, size+sizeof(BlockPrefixOpt));

      // the mark should has been set because of realloc
      return prefix+1;
    }
  }
  else
  {
    return ATOM_Malloc (size, filename, lineno);
  }
#endif
}

ATOM_DBGHLP_API void * ATOM_CALL ATOM_Calloc(size_t num, size_t size, const char *filename, int lineno)
{
#if 0
  return ::calloc (num, size);
#else
  void *p = ATOM_Malloc (num * size, filename, lineno);

  if (p)
  {
    memset (p, 0, num * size);
  }

  return p;
#endif
}

ATOM_DBGHLP_API void  ATOM_CALL ATOM_Free(void *p, const char *filename, int lineno)
{
#if 0
  free (p);
#else
  if (p)
  {
    if (DebugMalloc)
    {
      __FreeD (p, filename, lineno);
    }
    else
    {
      BlockPrefixOpt *prefix = ((BlockPrefixOpt*)p)-1;
      if (prefix->prefix != ATOM_NON_PREFIX_MARK || prefix->mark != ATOM_BLOCK_MARK)
      {
        // The block should be allocated by __MallocD or __ReallocD
        char buffer[256];
        sprintf (buffer, "Invalid block mark or prefix: %d 0x%08X\n", prefix->mark, prefix->prefix);
        Error (buffer, "no filename", 0, true);
      }
      else
      {
        FreeFunc(prefix);
      }
    }
  }
#endif
}

static inline bool _IsPowerOfTwo (size_t size)
{
  return 0 == (size & (size-1));
}

static inline size_t _GetNextAlignment (size_t size, size_t alignment)
{
  return (size + alignment - 1) & ~(alignment-1);
}

static void * __AlignedMallocD(size_t size, size_t alignment, const char *filename, int lineno)
{
  if (alignment && _IsPowerOfTwo (alignment))
  {
    size_t newsize = alignment - 1 + sizeof(void*) + size;
    char *p = (char*)__MallocD (newsize, filename, lineno);
    if (p)
    {
      char *aligned = (char*)_GetNextAlignment((size_t)(p+sizeof(void*)), alignment);
      void **origin = (void**)(aligned - sizeof(void*));
      *origin = p;
      return aligned;
    }
  }

  return 0;
}

ATOM_DBGHLP_API void * ATOM_CALL ATOM_AlignedMalloc(size_t size, size_t alignment, const char *filename, int lineno)
{
#if 0
  return ::_aligned_malloc (size, alignment);
#else
  if (alignment && _IsPowerOfTwo (alignment))
  {
    if (!Determined)
    {
      ATOM_DetermineMallocAlgorithm ();
      InterlockedExchange ((LONG*)&Determined, 1);
    }

    if (DebugMalloc)
    {
      return __AlignedMallocD (size, alignment, filename, lineno);
    }
    else
    {
      size_t newsize = alignment - 1 + sizeof(void*) + size;
      char *p = (char*)ATOM_Malloc (newsize, filename, lineno);
      if (p)
      {
        char *aligned = (char*)_GetNextAlignment((size_t)(p+sizeof(void*)), alignment);
        void **origin = (void**)(aligned - sizeof(void*));
        *origin = p;
        return aligned;
      }
    }
  }

  return 0;
#endif
}

ATOM_DBGHLP_API void  ATOM_CALL ATOM_AlignedFree(void *p, const char *filename, int lineno)
{
#if 0
  ::_aligned_free (p);
#else
  if (p)
  {
    char *ptr = (char*)p;
    void **origin_ptr = (void**)(ptr - sizeof(void*));
    ATOM_Free (*origin_ptr, filename, lineno);
  }
#endif
}

ATOM_DBGHLP_API void ATOM_CALL ATOM_SetErrorHandler (ATOM_ErrorHandler handler)
{
  ErrorHandler = handler;
}

ATOM_DBGHLP_API void ATOM_CALL ATOM_DefaultErrorHandler (const char *errmsg, const char *filename, int lineno, int fatal)
{
  ATOM_RecordError (filename, lineno, errmsg, fatal);

  if (fatal && BreakOnMemError)
  {
    // Crash or break into the debugger
    ::DebugBreak ();
  }
}

ATOM_DBGHLP_API void ATOM_CALL ATOM_SetAllocCounterAlert (long count)
{
	::InterlockedExchange (&AllocCounterAlert, count);
}

ATOM_DBGHLP_API long ATOM_CALL ATOM_GetAllocCounterAlert (void)
{
	return AllocCounterAlert;
}

char lastMemErrorFileName[512] = { '\0' };

const char * ATOM_CALL ATOM_GetLastMemoryErrorFileName (void)
{
	return lastMemErrorFileName;
}

ATOM_DBGHLP_API ATOM_ErrorHandler ATOM_CALL ATOM_GetErrorHandler(void)
{
  return ErrorHandler ? ErrorHandler : &ATOM_DefaultErrorHandler;
}

ATOM_DBGHLP_API void ATOM_CALL ATOM_RecordError (const char *filename, int lineno, const char *errmsg, int fatal)
{
  int num_frames = 0;
  void *frames[ATOM_MAX_BACKTRACE_DEPTH];
  if (pfnCaptureStackBackTrace)
  {
	num_frames = pfnCaptureStackBackTrace(0, ATOM_MAX_BACKTRACE_DEPTH, frames, 0);
  }

  _memlog()->record_error (filename, lineno, errmsg, fatal!=0, num_frames, frames);
}

ATOM_DBGHLP_API int ATOM_CALL ATOM_BoundsCheck (void (ATOM_CALL *callback)(void*, ATOM_BlockEntry*, const char*, int), void *userdata)
{
  return _memlog()->check_bounds (callback, userdata) ? 1 : 0;
}
