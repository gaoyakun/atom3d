#ifndef __ATOM_DEBUGHELP_CALLSTACK_H__
#define __ATOM_DEBUGHELP_CALLSTACK_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <windows.h>
#include <vector>
#include <string>
#include "basedefs.h"

class ATOM_CallStackImpl;

#pragma warning(push)
#pragma warning(disable:4251)

class ATOM_DBGHLP_API ATOM_CallStack
{
public:
  enum
  {
    GET_SYMBOL = (1<<0),
    GET_LINE = (1<<1),
    GET_MODULEINFO = (1<<2),
    GET_FILEVERSION = (1<<3),
    GET_VERBOSE = (GET_SYMBOL|GET_LINE|GET_MODULEINFO|GET_FILEVERSION),
    SYM_BUILDPATH = (1<<4),
    SYM_USESYMSRV = (1<<5),
    SYM_ALL = (SYM_BUILDPATH|SYM_USESYMSRV),
    OPT_ALL = (GET_VERBOSE|SYM_ALL)
  };

  enum
  {
    MAX_NAME_LENGTH = 512
  };

  typedef struct _StackEntry
  {
    void *offset; // 0 if not a valid entry
    char name[MAX_NAME_LENGTH];
    char und_name[MAX_NAME_LENGTH];
    char und_fullname[MAX_NAME_LENGTH];
    ATOM_ULongLong offset_from_symbol;
    unsigned offset_from_line;
    unsigned line_number;
    char line_filename[MAX_NAME_LENGTH];
    unsigned symbol_type;
    const char *symbol_type_string;
    char module_name[MAX_NAME_LENGTH];
    ATOM_ULongLong base_of_image;
    char loaded_image_name[MAX_NAME_LENGTH];
  } StackEntry;

public:
  ATOM_CallStack (const CONTEXT *context = 0, unsigned options = OPT_ALL, const char *sympath = 0);

  ~ATOM_CallStack (void);

  bool showCallStack (HANDLE hThread = GetCurrentThread(), const CONTEXT *context = 0);
  size_t showCallStack (StackEntry *entries, size_t size, HANDLE hThread = GetCurrentThread(), const CONTEXT *context = 0);
  size_t getNumEntries (void) const { return _M_stack_entries.size(); }
  void setNumEntries (size_t size) { _M_stack_entries.resize(size); }
  void addEntry (const StackEntry &e) { _M_stack_entries.push_back (e); }
  const StackEntry &getEntry (size_t index) const { return _M_stack_entries[index]; }
  StackEntry &getEntry (size_t index) { return _M_stack_entries[index]; }
  std::string getDescription (size_t skip) const;
  void resolveSymbols (_StackEntry &e);

private:
  ATOM_CallStackImpl *_M_callstack_impl;
  std::vector<StackEntry> _M_stack_entries;
};

#pragma warning(pop)

#endif // __ATOM_DEBUGHELP_CALLSTACK_H__
