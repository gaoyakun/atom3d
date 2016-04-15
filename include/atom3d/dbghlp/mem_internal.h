#ifndef __ATOM_DEBUGHELP_MEM_INTERNAL_H__
#define __ATOM_DEBUGHELP_MEM_INTERNAL_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

#define ATOM_MAX_BACKTRACE_DEPTH 32
#define ATOM_GUARD_SIZE 8
#define ATOM_BLOCK_GUARD 0x9F9F9F9F

#ifndef EXCEPTION_EXECUTE_HANDLER
# define EXCEPTION_EXECUTE_HANDLER 1
#endif

typedef struct _ATOM_BlockEntry
{
  void *block;
  unsigned size;
  void *backtrace[ATOM_MAX_BACKTRACE_DEPTH];
  unsigned short num_frames;
  struct _ATOM_BlockEntry *next;
  struct _ATOM_BlockEntry *prev;
} ATOM_BlockEntry;

typedef struct _ATOM_BlockGuard
{
  size_t guard[ATOM_GUARD_SIZE];
} ATOM_BlockGuard;

typedef struct _ATOM_AllocEntry
{
  const char *filename;
  size_t lineno;
  size_t total_size;
  size_t id;
  ATOM_ULongLong counter;
  ATOM_ULongLong counterAlert;
  ATOM_BlockEntry *blocks;
} ATOM_AllocEntry;

typedef struct _ATOM_BlockPrefix
{
  size_t mark;
  ATOM_AllocEntry *entry;
  ATOM_BlockEntry blockentry;
  ATOM_BlockGuard *guard;
  struct _ATOM_BlockPrefix *prefix;
} ATOM_BlockPrefix;

#endif // __ATOM_DEBUGHELP_MEM_INTERNAL_H__
