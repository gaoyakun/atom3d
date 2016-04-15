#ifndef __ATOM_DEBUGHELP_MEM_H__
#define __ATOM_DEBUGHELP_MEM_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <malloc.h>
#include "basedefs.h"
#include "mem_internal.h"

# define ATOM_FOURCC(c0,c1,c2,c3) \
  ((unsigned)(unsigned char)(c0) | ((unsigned)(unsigned char)(c1) << 8) |  \
  ((unsigned)(unsigned char)(c2) << 16) | ((unsigned)(unsigned char)(c3) << 24 ))

#define ATOM_BLOCK_MARK ATOM_FOURCC('M','0','0','1')
#define ATOM_NON_PREFIX_MARK ((ATOM_BlockPrefix*)ATOM_FOURCC('d','h','p','f'))

#if defined(ATOM_NO_DEBUGHELP_MEM_MNGR)

#   define ATOM_MALLOC(size) ::malloc(size)
#   define ATOM_MALLOC_T(size, type) ::malloc(size)
#   define ATOM_REALLOC(ptr, size) ::realloc(ptr, size)
#   define ATOM_REALLOC_T(ptr, size, type) ::realloc(ptr, size)
#   define ATOM_CALLOC(num, size) ::calloc(num, size)
#   define ATOM_CALLOC_T(num, size, type) ::calloc(num, size)
#   define ATOM_TAG_MALLOC(size, tag) ::malloc(size)
#   define ATOM_TAG_MALLOC_T(size, type, tag) ::malloc(size)
#   define ATOM_TAG_REALLOC(ptr, size, tag) ::realloc(ptr, size)
#   define ATOM_TAG_REALLOC_T(ptr, size, type, tag) ::realloc(ptr, size)
#   define ATOM_TAG_CALLOC(num, size, tag) ::calloc(num, size)
#   define ATOM_TAG_CALLOC_T(num, size, type, tag) ::calloc(num, size)
#   define ATOM_ALIGNED_MALLOC(size, alignment) ::_aligned_malloc(size, alignment)
#   define ATOM_ALIGNED_MALLOC_T(size, alignement, type) ::_aligned_malloc(size, alignment)
#   define ATOM_ALIGNED_REALLOC(ptr, size, alignment) ::_aligned_realloc(ptr, size, alignment)
#   define ATOM_ALIGNED_REALLOC_T(ptr, size, alignment, type) ::_aligned_realloc(ptr, size, alignment)
#   define ATOM_TAG_ALIGNED_MALLOC(size, alignment, tag) ::_aligned_malloc(size, alignment)
#   define ATOM_TAG_ALIGNED_MALLOC_T(size, alignment, type, tag) ::_aligned_malloc(size, alignment)
#   define ATOM_TAG_ALIGNED_REALLOC(ptr, size, alignment, tag) ::_aligned_realloc(ptr, size, alignment)
#   define ATOM_TAG_ALIGNED_REALLOC_T(ptr, size, alignment, type, tag) ::_aligned_realloc(ptr, size, alignment)

#   define ATOM_TRACE_MALLOC(size) ::malloc(size)
#   define ATOM_TRACE_MALLOC_T(size, type) ::malloc(size)
#   define ATOM_TRACE_REALLOC(ptr, size) ::realloc(ptr, size)
#   define ATOM_TRACE_REALLOC_T(ptr, size, type) ::realloc(ptr, size)
#   define ATOM_TRACE_CALLOC(num, size) ::calloc(num, size)
#   define ATOM_TRACE_CALLOC_T(num, size, type) ::calloc(num, size)
#   define ATOM_TAG_TRACE_MALLOC(size, tag) ::malloc(size)
#   define ATOM_TAG_TRACE_MALLOC_T(size, type, tag) ::malloc(size)
#   define ATOM_TAG_TRACE_REALLOC(ptr, size, tag) ::realloc(ptr, size)
#   define ATOM_TAG_TRACE_REALLOC_T(ptr, size, type, tag) ::realloc(ptr, size)
#   define ATOM_TAG_TRACE_CALLOC(num, size, tag) ::calloc(num, size)
#   define ATOM_TAG_TRACE_CALLOC_T(num, size, type, tag) ::calloc(num, size)
#   define ATOM_TRACE_ALIGNED_MALLOC(size, alignment) ::_aligned_malloc(size, alignment)
#   define ATOM_TRACE_ALIGNED_MALLOC_T(size, alignment, type) ::_aligned_malloc(size, alignment)
#   define ATOM_TRACE_ALIGNED_REALLOC(ptr, size, alignment) ::_aligned_realloc(ptr, size, alignment)
#   define ATOM_TRACE_ALIGNED_REALLOC_T(ptr, size, alignment, type) ::_aligned_realloc(ptr, size, alignment)
#   define ATOM_TAG_TRACE_ALIGNED_MALLOC(size, alignment, tag) ::_aligned_malloc(size, alignment)
#   define ATOM_TAG_TRACE_ALIGNED_MALLOC_T(size, alignment, type, tag) ::_aligned_malloc(size, alignment)
#   define ATOM_TAG_TRACE_ALIGNED_REALLOC(ptr, size, alignment, tag) ::_aligned_realloc(ptr, size, alignment)
#   define ATOM_TAG_TRACE_ALIGNED_REALLOC_T(ptr, size, alignment, type, tag) ::_aligned_realloc(ptr, size, alignment)

#   define ATOM_FREE(ptr) ::free(ptr)
#   define ATOM_ALIGNED_FREE(ptr) ::_aligned_free(ptr)

#else

# define ATOM_MALLOC(size) ATOM_Malloc(size, __FILE__, __LINE__)
# define ATOM_MALLOC_T(size, type) ATOM_Malloc(size, __FILE__, __LINE__)
# define ATOM_REALLOC(ptr, size) ATOM_Realloc(ptr, size, __FILE__, __LINE__)
# define ATOM_REALLOC_T(ptr, size, type) ATOM_Realloc(ptr, size, __FILE__, __LINE__)
# define ATOM_CALLOC(num, size) ATOM_Calloc(num, size, __FILE__, __LINE__)
# define ATOM_CALLOC_T(num, size, type) ATOM_Calloc(num, size, __FILE__, __LINE__)
# define ATOM_TAG_MALLOC(size, tag) ATOM_Malloc(size, __FILE__, __LINE__)
# define ATOM_TAG_MALLOC_T(size, type, tag) ATOM_Malloc(size, __FILE__, __LINE__)
# define ATOM_TAG_REALLOC(ptr, size, tag) ATOM_Realloc(ptr, size, __FILE__, __LINE__)
# define ATOM_TAG_REALLOC_T(ptr, size, type, tag) ATOM_Realloc(ptr, size, __FILE__, __LINE__)
# define ATOM_TAG_CALLOC(num, size, tag) ATOM_Calloc(num, size, __FILE__, __LINE__)
# define ATOM_TAG_CALLOC_T(num, size, type, tag) ATOM_Calloc(num, size, __FILE__, __LINE__)
# define ATOM_ALIGNED_MALLOC(size, alignment) ATOM_AlignedMalloc(size, alignment, __FILE__, __LINE__)
# define ATOM_ALIGNED_MALLOC_T(size, alignment, type) ATOM_AlignedMalloc(size, alignment, __FILE__, __LINE__)
# define ATOM_TAG_ALIGNED_MALLOC(size, alignment, tag) ATOM_AlignedMalloc(size, alignment, __FILE__, __LINE__)
# define ATOM_TAG_ALIGNED_MALLOC_T(size, alignment, type, tag) ATOM_AlignedMalloc(size, alignment, __FILE__, __LINE__)

# define ATOM_TRACE_MALLOC                ATOM_TRACE_MALLOC
# define ATOM_TRACE_MALLOC_T              ATOM_MALLOC_T
# define ATOM_TRACE_REALLOC               ATOM_REALLOC
# define ATOM_TRACE_REALLOC_T             ATOM_REALLOC_T
# define ATOM_TRACE_CALLOC                ATOM_CALLOC
# define ATOM_TRACE_CALLOC_T              ATOM_CALLOC_T
# define ATOM_TAG_TRACE_MALLOC            ATOM_MALLOC
# define ATOM_TAG_TRACE_MALLOC_T          ATOM_MALLOC_T
# define ATOM_TAG_TRACE_REALLOC           ATOM_REALLOC
# define ATOM_TAG_TRACE_REALLOC_T         ATOM_REALLOC_T
# define ATOM_TAG_TRACE_CALLOC            ATOM_CALLOC
# define ATOM_TAG_TRACE_CALLOC_T          ATOM_CALLOC_T
# define ATOM_TRACE_ALIGNED_MALLOC        ATOM_ALIGNED_MALLOC
# define ATOM_TRACE_ALIGNED_MALLOC_T      ATOM_ALIGNED_MALLOC_T
# define ATOM_TAG_TRACE_ALIGNED_MALLOC    ATOM_ALIGNED_MALLOC
# define ATOM_TAG_TRACE_ALIGNED_MALLOC_T  ATOM_ALIGNED_MALLOC_T

# define ATOM_FREE(ptr) ATOM_Free(ptr, __FILE__, __LINE__)
# define ATOM_ALIGNED_FREE(ptr) ATOM_AlignedFree(ptr, __FILE__, __LINE__)

#endif

#define ATOM_STACK_MALLOC(size) ((void*)_alloca(size))

#define ATOM_STACK_ALLOC_SIZE (1024*16)
#define ATOM_STACK_ALLOC(size) \
  (((size) < ATOM_STACK_ALLOC_SIZE) ? ATOM_STACK_MALLOC(size) : ATOM_MALLOC(size))
  
#define ATOM_STACK_FREE(buffer, size) \
  if ((buffer) && (size) >= ATOM_STACK_ALLOC_SIZE) { ATOM_FREE(buffer); }

#if defined(__cplusplus)
extern"C" {
#endif

typedef void (ATOM_CALL *ATOM_ErrorHandler)(const char*, const char*, int, int);
ATOM_DBGHLP_API void ATOM_CALL ATOM_DefaultErrorHandler (const char *errmsg, const char *filename, int lineno, int fatal);
ATOM_DBGHLP_API const char * ATOM_CALL ATOM_GetLastMemoryErrorFileName (void);

ATOM_DBGHLP_API void * ATOM_CALL ATOM_Malloc(size_t size, const char *filename, int lineno);
ATOM_DBGHLP_API void * ATOM_CALL ATOM_Realloc(void *p, size_t size, const char *filename, int lineno);
ATOM_DBGHLP_API void * ATOM_CALL ATOM_Calloc(size_t num, size_t size, const char *filename, int lineno);
ATOM_DBGHLP_API void  ATOM_CALL ATOM_Free(void *p, const char *filename, int lineno);
ATOM_DBGHLP_API void * ATOM_CALL ATOM_AlignedMalloc(size_t size, size_t alignment, const char *filename, int lineno);
ATOM_DBGHLP_API void  ATOM_CALL ATOM_AlignedFree(void *p, const char *filename, int lineno);

ATOM_DBGHLP_API void  ATOM_CALL ATOM_SetErrorHandler (ATOM_ErrorHandler handler);
ATOM_DBGHLP_API ATOM_ErrorHandler ATOM_CALL ATOM_GetErrorHandler(void);

ATOM_DBGHLP_API int  ATOM_CALL ATOM_DumpMemoryState (const char *filename, int fulldump);
ATOM_DBGHLP_API int  ATOM_CALL ATOM_DumpMemoryBlock (const char *filename, void *block, bool userblock);
ATOM_DBGHLP_API int  ATOM_CALL ATOM_DumpMemoryBlockToMemory (char *buffer, void *block, bool userblock);
ATOM_DBGHLP_API int  ATOM_CALL ATOM_DumpMemoryDifference (const char *filename1, const char *filename2, const char *output_filename);
ATOM_DBGHLP_API void ATOM_CALL ATOM_SetAllocCounterAlert (long count);
ATOM_DBGHLP_API long ATOM_CALL ATOM_GetAllocCounterAlert (void);
ATOM_DBGHLP_API void ATOM_CALL ATOM_RecordError (const char *filename, int lineno, const char *errmsg, int fatal);
ATOM_DBGHLP_API int  ATOM_CALL ATOM_BoundsCheck (void (ATOM_CALL*)(void*, ATOM_BlockEntry*, const char*, int), void *userdata);

#if defined(__cplusplus)
}
#endif

#endif // __ATOM_DEBUGHELP_MEM_H__
