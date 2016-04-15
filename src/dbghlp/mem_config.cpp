#include "StdAfx.h"
#include <ATOM_thread.h>
#include "basedefs.h"
#if ATOM3D_COMPILER_MSVC
#include "rtc.h"
#endif
#include "exception.h"
#include "mempool.h"

extern void * (ATOM_CALL *MallocFunc) (size_t);
extern void * (ATOM_CALL *ReallocFunc) (void *, size_t);
extern void (ATOM_CALL *FreeFunc) (void *);
extern volatile long DebugMalloc;

#if ATOM3D_COMPILER_MSVC
# pragma warning(push)
# pragma warning(disable:4312)
# pragma warning(disable:4267)
# pragma warning(disable:4127)
# pragma warning(disable:4100)
# pragma warning(disable:4702)
# pragma warning(disable:4706)
# pragma warning(disable:4005)
# define ABORT do { ::DebugBreak(); } while (0)
# pragma warning(pop)
#endif

#if !ATOM3D_PLATFORM_MINGW
# include "nedmalloc/nedmalloc.c"
#endif

PCHAR* cmdline2argv(PCHAR CmdLine, int* _argc)
{
    PCHAR* argv;
    PCHAR  _argv;
    ULONG   len;
    ULONG   argc;
    CHAR   a;
    ULONG   i, j;

    BOOLEAN  in_QM;
    BOOLEAN  in_TEXT;
    BOOLEAN  in_SPACE;

    len = strlen(CmdLine);
    i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);

    argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
        i + (len+2)*sizeof(CHAR));

    _argv = (PCHAR)(((PUCHAR)argv)+i);

    argc = 0;
    argv[argc] = _argv;
    in_QM = FALSE;
    in_TEXT = FALSE;
    in_SPACE = TRUE;
    i = 0;
    j = 0;

    while( a = CmdLine[i] ) {
        if(in_QM) {
            if(a == '\"') {
                in_QM = FALSE;
            } else {
                _argv[j] = a;
                j++;
            }
        } else {
            switch(a) {
            case '\"':
                in_QM = TRUE;
                in_TEXT = TRUE;
                if(in_SPACE) {
                    argv[argc] = _argv+j;
                    argc++;
                }
                in_SPACE = FALSE;
                break;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                if(in_TEXT) {
                    _argv[j] = '\0';
                    j++;
                }
                in_TEXT = FALSE;
                in_SPACE = TRUE;
                break;
            default:
                in_TEXT = TRUE;
                if(in_SPACE) {
                    argv[argc] = _argv+j;
                    argc++;
                }
                _argv[j] = a;
                j++;
                in_SPACE = FALSE;
                break;
            }
        }
        i++;
    }
    _argv[j] = '\0';
    argv[argc] = NULL;

    (*_argc) = argc;
    return argv;
}

#if ATOM3D_PLATFORM_MINGW
static long totalMallocSize = 0;
#else
static volatile long totalMallocSize = 0;
#endif

static void * ATOM_CALL __HeapMalloc (size_t size)
{
	void *p = ::HeapAlloc (::GetProcessHeap(), 0, size);
	if (!p)
	{
		throw ATOM_OutOfMemory ();
	}
	::InterlockedExchangeAdd (&totalMallocSize, size);

	return p;
}

static void * ATOM_CALL __HeapReAlloc (void *ptr, size_t size)
{
	long increment = ptr ? ((long)size - ::HeapSize(::GetProcessHeap(), 0, ptr)) : size;
	void *p = ::HeapReAlloc (::GetProcessHeap(), 0, ptr, size);
	if (!p)
	{
		throw ATOM_OutOfMemory ();
	}
	::InterlockedExchangeAdd (&totalMallocSize, increment);

	return p;
}

static void ATOM_CALL __HeapFree (void *ptr)
{
  if (ptr)
  {
	  long size = ::HeapSize(::GetProcessHeap(), 0, ptr);
	  ::HeapFree (::GetProcessHeap(), 0, ptr);
	  ::InterlockedExchangeAdd (&totalMallocSize, -size);
  }
}

static void * ATOM_CALL __CrtMalloc (size_t size)
{
	long *p = (long*)::malloc (size + sizeof(long));
	if (!p)
	{
		throw ATOM_OutOfMemory ();
	}
	*p = size;
	::InterlockedExchangeAdd (&totalMallocSize, size);
	return p+1;
}

static void * ATOM_CALL __CrtReAlloc (void *ptr, size_t size)
{
	if (!ptr)
	{
		return __CrtMalloc(size);
	}

	long *p2 = ((long*)ptr)-1;
	long sizeIncrement = long(size) - *p2;
	long *p = (long*)::realloc (p2, size+sizeof(long));
	if (!p)
	{
		throw ATOM_OutOfMemory ();
	}
	*p = size;
	::InterlockedExchangeAdd (&totalMallocSize, sizeIncrement);
	return p+1;
}

static void ATOM_CALL __CrtFree (void *ptr)
{
	if (ptr)
	{
		long *p2 = ((long*)ptr)-1;
		::InterlockedExchangeAdd (&totalMallocSize, -*p2);
		::free (p2);
	}
}

#if !ATOM3D_PLATFORM_MINGW

static void * ATOM_CALL __NedMalloc (size_t size)
{
	long *p = (long*)nedalloc::nedmalloc (size+sizeof(long));
	if (!p)
	{
		throw ATOM_OutOfMemory ();
	}
	*p = size;
	::InterlockedExchangeAdd (&totalMallocSize, size);
	return p+1;
}

static void * ATOM_CALL __NedReAlloc (void *ptr, size_t size)
{
	if (!ptr)
	{
		return __NedMalloc(size);
	}

	long *p2 = ((long*)ptr)-1;
	long sizeIncrement = long(size) - *p2;
	long *p = (long*)nedalloc::nedrealloc (p2, size+sizeof(long));
	if (!p)
	{
		throw ATOM_OutOfMemory ();
	}
	*p = size;
	::InterlockedExchangeAdd (&totalMallocSize, sizeIncrement);
	return p+1;
}

static void ATOM_CALL __NedFree (void *ptr)
{
	if (ptr)
	{
		long *p2 = ((long*)ptr)-1;
		::InterlockedExchangeAdd (&totalMallocSize, -*p2);
		nedalloc::nedfree(p2);
	}
}

#endif

extern ATOM_MemoryPool *_GetMemoryPool (void);

static void * ATOM_CALL __PoolMalloc (size_t size)
{
	long *p = (long*)_GetMemoryPool()->alloc(size+sizeof(long));
	if (!p)
	{
		throw ATOM_OutOfMemory ();
	}
	*p = size;
	::InterlockedExchangeAdd (&totalMallocSize, size);
	return p+1;
}

static void * ATOM_CALL __PoolReAlloc (void *ptr, size_t size)
{
	if (!ptr)
	{
		return __PoolMalloc(size);
	}

	long *p = (long*)_GetMemoryPool()->alloc(size+sizeof(long));
	if (!p)
	{
		throw ATOM_OutOfMemory ();
	}

	long *p2 = ((long*)ptr)-1;
	long sizeIncrement = long(size) - *p2;
	memcpy (p+1, ptr, (*p2 > size) ? size : *p2);
	_GetMemoryPool()->dealloc(p2);

	*p = size;
	::InterlockedExchangeAdd (&totalMallocSize, sizeIncrement);


	return p+1;
}

static void ATOM_CALL __PoolFree (void *ptr)
{
	if (ptr)
	{
		long *p2 = ((long*)ptr)-1;
		::InterlockedExchangeAdd (&totalMallocSize, -*p2);
		_GetMemoryPool()->dealloc(p2);
	}
}

extern void * (ATOM_CALL *MallocFunc) (size_t size);
extern void * (ATOM_CALL *ReallocFunc) (void *p, size_t size);
extern void (ATOM_CALL *FreeFunc) (void *p);
extern volatile long DebugMalloc;
extern volatile long BreakOnMemError;
extern volatile long UseSymServer;

#define ALGORITHM_CONFIG_FILE "memconfig.ini"

/* config file content
  algorithm = crt|winheap|ned (default:crt)
  debug = 1|0 (default:0)
*/

static char *remove_head_tail_spaces(char *str)
{
  if (!str[0])
  {
    return str;
  }

  char *result = str;
  for (; *result == ' ' || *result == '\t'; ++result)
    ;

  char *tail = result + strlen(str) - 1;
  for (; *tail == ' ' || *tail == '\t'; --tail)
    ;

  tail[1] = '\0';

  return result;
}

static char *getline_from_stream (std::istream &in, char *buffer, unsigned size)
{
  while (!in.eof())
  {
    in.getline (buffer, size);
    buffer[size-1] = '\0';
    return remove_head_tail_spaces (buffer);
  }
  return 0;
}

const char * get_config (const char *token, char *line, char sep, bool ignorecase)
{
  int token_len = (int)strlen(token);
  int line_len = (int)strlen(line);

  if (token_len >= line_len)
  {
    return 0;
  }

  if (ignorecase)
  {
    if (_strnicmp (line, token, token_len))
    {
      return 0;
    }
  }
  else
  {
    if (strncmp (line, token, token_len))
    {
      return 0;
    }
  }

  const char *str = line + token_len;
  while (*str == ' ' || *str == '\t')
  {
    ++str;
  }

  if (*str++ != sep)
  {
    return 0;
  }

  while (*str == ' ' || *str == '\t')
  {
    ++str;
  }

  return str;
}

#define MEM_ALGO_CRT 0
#define MEM_ALGO_WINHEAP 1
#define MEM_ALGO_POOL 2
#if !ATOM3D_PLATFORM_MINGW
#define MEM_ALGO_NED 3
#endif

#define ALGORITHM_KEYWORD "algorithm"
#define DEBUG_KEYWORD "debug"

void ATOM_DetermineMallocAlgorithm (void)
{
  if (!MallocFunc || !ReallocFunc || !FreeFunc)
  {
    int algo = MEM_ALGO_NED;
	bool debug = false;
	bool nofatal = false;
	bool symsrv = false;

    char filename[MAX_PATH];
    ::GetModuleFileNameA (NULL, filename, MAX_PATH);
    char *p = strrchr (filename, '\\');
    p[1] = '\0';
    strcat (filename, ALGORITHM_CONFIG_FILE);

    std::ifstream s(filename);

    if (s.good())
    {
      char buffer[1024];
      const char *v;

      while (getline_from_stream(s, buffer, 1024) && buffer[0])
      {
        v = get_config (ALGORITHM_KEYWORD, buffer, '=', true);
        if (v)
        {
          if (!_stricmp (v, "crt"))
          {
            algo = MEM_ALGO_CRT;
          }
          else if (!strcmp (v, "winheap"))
          {
            algo = MEM_ALGO_WINHEAP;
          }
		  else if (!strcmp (v, "pool"))
		  {
		    algo = MEM_ALGO_POOL;
		  }
#if !ATOM3D_PLATFORM_MINGW
          else if (!strcmp (v, "ned"))
          {
            algo = MEM_ALGO_NED;
          }
#endif
          continue;
        }

        v = get_config (DEBUG_KEYWORD, buffer, '=', true);
        if (v)
        {
          if (!strcmp (v, "1"))
          {
            debug = true;
          }
          else if (!strcmp (v, "0"))
          {
            debug = false;
          }
        }
      }
    }
	else
	{
		int argc;
		char **argv = cmdline2argv(::GetCommandLineA (), &argc);
		for (int i = 0; i < argc; ++i)
		{
			if (!strcmp(argv[i], "--mem") && i < argc-1)
			{
				char *memalgo = argv[i+1];
				if (!strcmp (memalgo, "crt"))
				{
					algo = MEM_ALGO_CRT;
				}
				else if (!strcmp (memalgo, "winheap"))
				{
					algo = MEM_ALGO_WINHEAP;
				}
#if !ATOM3D_PLATFORM_MINGW
				else if (!strcmp (memalgo, "ned"))
				{
					algo = MEM_ALGO_NED;
				}
#endif
				i++;
			}
			else if (!strcmp (argv[i], "--debugmem"))
			{
				debug = true;
#if ATOM_COMPILER_MSVC
				HOOK_CRT_ERROR_CHECK (true);
#endif
			}
			else if (!strcmp (argv[i], "--nofatal"))
			{
				nofatal = true;
			}
			else if (!strcmp (argv[i], "--symsrv"))
			{
				symsrv = true;
			}
		}
		::GlobalFree (argv);
	}

	if (!debug)
	{
		char buf[32] = "";
		::GetEnvironmentVariable("ATOM3D_ENABLE_MEM_DEBUG", buf, 32);
		if (!stricmp (buf, "yes"))
		{
			debug = true;
#if ATOM3D_COMPILER_MSVC
			HOOK_CRT_ERROR_CHECK(true);
#endif
		}
	}

    switch (algo)
    {
    case MEM_ALGO_WINHEAP:
      MallocFunc = &__HeapMalloc;
      ReallocFunc = &__HeapReAlloc;
      FreeFunc = &__HeapFree;
      break;

	case MEM_ALGO_POOL:
      MallocFunc = &__PoolMalloc;
      ReallocFunc = &__PoolReAlloc;
      FreeFunc = &__PoolFree;
      break;

#if !ATOM3D_PLATFORM_MINGW
    case MEM_ALGO_NED:
      MallocFunc = &__NedMalloc;
      ReallocFunc = &__NedReAlloc;
      FreeFunc = &__NedFree;
      break;
#endif

    case MEM_ALGO_CRT: // fall through
    default:
		MallocFunc = __CrtMalloc;
		ReallocFunc = __CrtReAlloc;
		FreeFunc = __CrtFree;
      break;
    }

    ATOM_AtomExchange ((long volatile*)&DebugMalloc, debug ? 1L : 0L);
	ATOM_AtomExchange ((long volatile*)&BreakOnMemError, nofatal ? 0L : 1L);
	ATOM_AtomExchange ((long volatile*)&UseSymServer, symsrv ? 1L : 0L);
  }
}

ATOM_DBGHLP_API unsigned ATOM_CALL ATOM_GetTotalMallocSize (void)
{
	return totalMallocSize;
}



