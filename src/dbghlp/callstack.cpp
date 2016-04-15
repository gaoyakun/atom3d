#include "StdAfx.h"
#include <ATOM_thread.h>
#include "callstack.h"
#include "stackwalker.h"

ATOM_FastMutex *dbghlp_lock = 0;

static inline void _StrnCpy (char *dest, const char *src, size_t size)
{
  strncpy (dest, src, size);
  dest[size-1] = '\0';
}

class ATOM_CallStackImpl: public StackWalker
{
public:
  ATOM_CallStackImpl (int options, const char *sympath, DWORD pid, HANDLE hProc)
    : StackWalker (options, sympath, pid, hProc) 
	, _M_entries (0)
	, _M_size (0)
	, _M_current_entry (0)
  {
  }
public:
  void BeginShowATOM_CallStack (ATOM_CallStack::StackEntry *entries, size_t size) 
  { 
    _M_current_entry = 0; 
    _M_size = size;
    _M_entries = entries;
  }
  size_t EndShowATOM_CallStack (void)
  {
    return _M_current_entry;
  }
protected:
  virtual void OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry);
private:
  ATOM_CallStack::StackEntry *_M_entries;
  size_t _M_size;
  size_t _M_current_entry;
};

void ATOM_CallStackImpl::OnCallstackEntry (CallstackEntryType /* eType */, CallstackEntry &entry)
{
  if (_M_current_entry < _M_size)
  {
    ATOM_CallStack::StackEntry &e = _M_entries[_M_current_entry];
    ++_M_current_entry;

    e.offset = entry.offset;
    _StrnCpy (e.name, entry.name, ATOM_CallStack::MAX_NAME_LENGTH);
    _StrnCpy (e.und_name, entry.undName, ATOM_CallStack::MAX_NAME_LENGTH);
    _StrnCpy (e.und_fullname, entry.undFullName, ATOM_CallStack::MAX_NAME_LENGTH);
    e.offset_from_symbol = entry.offsetFromSmybol;
    e.offset_from_line = entry.offsetFromLine;
    e.line_number = entry.lineNumber;
    _StrnCpy (e.line_filename, entry.lineFileName, ATOM_CallStack::MAX_NAME_LENGTH);
    e.symbol_type = entry.symType;
    e.symbol_type_string = entry.symTypeString;
    _StrnCpy (e.module_name, entry.moduleName, ATOM_CallStack::MAX_NAME_LENGTH);
    e.base_of_image = entry.baseOfImage;
    _StrnCpy (e.loaded_image_name, entry.loadedImageName, ATOM_CallStack::MAX_NAME_LENGTH);
  }
}

ATOM_CallStack::ATOM_CallStack (const CONTEXT *context, unsigned options, const char *sympath)
{
  int opt = 0;

  if ((options & GET_SYMBOL) != 0)
    opt |= StackWalker::RetrieveSymbol;

  if ((options & GET_LINE) != 0)
    opt |= StackWalker::RetrieveLine;

  if ((options & GET_MODULEINFO) != 0)
    opt |= StackWalker::RetrieveModuleInfo;

  if ((options & GET_FILEVERSION) != 0)
    opt |= StackWalker::RetrieveFileVersion;

  if ((options & SYM_BUILDPATH) != 0)
    opt |= StackWalker::SymBuildPath;

  if ((options & SYM_USESYMSRV) != 0)
    opt |= StackWalker::SymUseSymSrv;

  _M_callstack_impl = new ATOM_CallStackImpl(opt, sympath, ::GetCurrentProcessId(), ::GetCurrentProcess());

  if (!dbghlp_lock)
  {
    dbghlp_lock = new ATOM_FastMutex;
  }

  {
    ATOM_FastMutex::ScopeMutex l(*dbghlp_lock);
    _M_stack_entries.resize(32);
    size_t size = showCallStack (&_M_stack_entries[0], 32, ::GetCurrentThread(), context);
    _M_stack_entries.resize(size);
  }
}

bool ATOM_CallStack::showCallStack (HANDLE hThread, const CONTEXT *context)
{
  ATOM_FastMutex::ScopeMutex l(*dbghlp_lock);
  _M_stack_entries.resize(32);
  _M_callstack_impl->BeginShowATOM_CallStack (&_M_stack_entries[0], 32);
  _M_callstack_impl->ShowCallstack (hThread, context);
  return _M_callstack_impl->EndShowATOM_CallStack () > 0;
}

size_t ATOM_CallStack::showCallStack (StackEntry *entries, size_t size, HANDLE hThread, const CONTEXT *context)
{
  ATOM_FastMutex::ScopeMutex l(*dbghlp_lock);
  _M_callstack_impl->BeginShowATOM_CallStack (entries, size);
  _M_callstack_impl->ShowCallstack (hThread, context);
  return _M_callstack_impl->EndShowATOM_CallStack ();
}

ATOM_CallStack::~ATOM_CallStack (void)
{
  delete _M_callstack_impl;
}

void ATOM_CallStack::resolveSymbols (_StackEntry &e)
{
  StackWalker::CallstackEntry entry;
  memset (&entry, 0, sizeof(e));
  entry.offset = e.offset;

  {
    ATOM_FastMutex::ScopeMutex l(*dbghlp_lock);
    _M_callstack_impl->ResolveFunctionName (entry);
  }

  e.offset = entry.offset;
  _StrnCpy (e.name, entry.name, ATOM_CallStack::MAX_NAME_LENGTH);
  _StrnCpy (e.und_name, entry.undName, ATOM_CallStack::MAX_NAME_LENGTH);
  _StrnCpy (e.und_fullname, entry.undFullName, ATOM_CallStack::MAX_NAME_LENGTH);
  e.offset_from_symbol = entry.offsetFromSmybol;
  e.offset_from_line = entry.offsetFromLine;
  e.line_number = entry.lineNumber;
  _StrnCpy (e.line_filename, entry.lineFileName, ATOM_CallStack::MAX_NAME_LENGTH);
  e.symbol_type = entry.symType;
  e.symbol_type_string = entry.symTypeString;
  _StrnCpy (e.module_name, entry.moduleName, ATOM_CallStack::MAX_NAME_LENGTH);
  e.base_of_image = entry.baseOfImage;
  _StrnCpy (e.loaded_image_name, entry.loadedImageName, ATOM_CallStack::MAX_NAME_LENGTH);
}

static std::string UL2Str (ATOM_ULongLong num)
{
  char buffer[32];
  sprintf (buffer, "0x%I64X", num);
  return buffer;
}

static std::string L2Str (unsigned long num)
{
  char buffer[32];
  sprintf (buffer, "%d", num);
  return buffer;
}

static std::string formatATOM_CallStackString (const ATOM_CallStack::StackEntry &entry)
{
  if (entry.offset == 0)
  {
    return "";
  }

  std::string offset = UL2Str((unsigned long)entry.offset);
  //std::string offsetFromSymbol = UL2Str(entry.offset_from_symbol);
  std::string baseOfImage = UL2Str (entry.base_of_image);
  //std::string offsetFromLine = L2Str(entry.offset_from_line);
  std::string lineNumber = L2Str(entry.line_number);
  std::string lineFileName = entry.line_filename[0] ? entry.line_filename : "(unknown file)";
  //std::string symTypeString = entry.symbol_type_string ? entry.symbol_type_string : "(unknown symbol type)";
  std::string name = entry.name[0] ? entry.name : "(unknown name)";
  //std::string und_name = entry.und_name[0] ? entry.und_name : "(unknown und_name)";
  //std::string und_fullname = entry.und_fullname[0] ? entry.und_fullname : "(unknown und_fullname)";
  std::string modulename = entry.module_name[0] ? entry.module_name : "(unknown module)";
  //std::string loadedImageName = entry.loaded_image_name[0] ? entry.loaded_image_name : "(unknown loaded image)";

  return modulename + '(' + baseOfImage + ')' + "!" + name + " " + lineFileName + "(" + lineNumber + ") " + offset;
}

std::string ATOM_CallStack::getDescription (size_t skip) const
{
  std::string ret;
  for (size_t i = skip; i < _M_stack_entries.size(); ++i)
  {
    ret += formatATOM_CallStackString (_M_stack_entries[i]);
    ret += "\r\n";
  }
  return ret;
}

