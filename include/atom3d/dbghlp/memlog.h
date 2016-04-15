#ifndef __ATOM_DEBUGHELP_MEMLOG_H__
#define __ATOM_DEBUGHELP_MEMLOG_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_thread.h"
#include "mem_internal.h"

//struct ATOM_MemBreakPoint
//{
//  const char *filename;
//  size_t lineno;
//};

class ATOM_MemLog
{
public:
  typedef std::vector<ATOM_AllocEntry*> EntryArray;
  typedef EntryArray::iterator EntryArrayIter;
  typedef EntryArray::const_iterator EntryArrayConstIter;

public:
  ATOM_MemLog (void);
  virtual ~ATOM_MemLog (void);

public:
  ATOM_AllocEntry *acquire_entry (ATOM_BlockEntry *block, const char *filename, int lineno);
  void release_entry (ATOM_BlockEntry *block, ATOM_AllocEntry *entry);
  void record_error (const char *filename, int lineno, const char *errmsg, bool fatal, int numframes, void **frames);
  bool dump (const char *filename) const;
  bool full_dump (const char *filename) const;
  bool dump_block (const char *filename, void *entry, bool userblock) const;
  bool dump_block_to_memory (char *buffer, void *entry, bool userblock) const;
  bool dump_difference (const char *filename1, const char *filename2, const char *output_filename) const;
  bool check_bounds (void (ATOM_CALL *on_check_failed)(void*, ATOM_BlockEntry*, const char*, int), void *userdata) const;

private:
  //bool extractBacktraces (std::vector<BackTraceEntry> &backtraces, ATOM_BlockEntry *b) const;

private:
  EntryArray *m_buckets;
  std::multimap<const char*, int> m_errors;
  FILE *m_fperr;
  mutable ATOM_FastMutex m_lock;
  mutable ATOM_FastMutex m_errlock;
};

#endif // __ATOM_DEBUGHELP_MEM_H__
