#include <malloc.h>
#include <stdexcept>
#include <ATOM_dbghlp.h>

#include "dynamicfunc.h"
#include "scriptinterface.h"
#include "scriptmngr.h"

#if ATOM3D_PLATFORM_MINGW
# define _aligned_malloc __mingw_aligned_malloc
# define _aligned_free __mingw_aligned_free
#endif

class CStructure
{
public:
  static const int TYPE_INT = 0;
  static const int TYPE_UINT = 1;
  static const int TYPE_SHORT = 2;
  static const int TYPE_USHORT = 3;
  static const int TYPE_BYTE = 4;
  static const int TYPE_UBYTE = 5;
  static const int TYPE_FLOAT = 6;
  static const int TYPE_DOUBLE = 7;
  static const int TYPE_POINTER = 8;
  static const int TYPE_STRINGP = 9;
  static const int TYPE_STRINGA = 10;
public:
  CStructure (void);
  ~CStructure (void);
private:
  struct MemberInfo
  {
    unsigned offset;
    unsigned size;
    int type;
    ATOM_STRING name;
  };
  void createStructure (void) const;
public:
  void setPack (unsigned pack);
  void addMember (const char *name, int type, unsigned size);
  void *getStructure (void) const;
  void setMember (const char *name, const ATOM_ScriptVariant &v);
  ATOM_ScriptVariant getMember (const char *name) const;
private:
  unsigned _M_pack;
  mutable char *_M_structure;
  ATOM_VECTOR<MemberInfo> _M_members;
};

static inline void RemoveHeadSpaces(ATOM_STRING& str) {
  int n = 0;
  while ( n < int(str.length()) && (str[n] == ' ' || str[n] == '\t'))
    ++n;
  str.erase(0, n);
}

static inline ATOM_STRING GetToken (ATOM_STRING &s)
{
  RemoveHeadSpaces (s);
  ATOM_STRING ret;

  if (s.size() > 0 && s[0] == ':')
  {
    ret = s[0];
    s.erase (0, 1);
    return ret;
  }

  for (unsigned i = 0; i < s.length(); ++i)
  {
    char ch = s[i];

    if (ch == ':' || ch == ' ' || ch == '\n' || ch == '\t')
    {
      ret = s.substr (0, i);
      s.erase (0, i);
      return ret;
    }
  }

  ret = s;
  s.clear ();
  return ret;
}

CStructure *ConstructCStructure (unsigned pack, const ATOM_ScriptVariant &var)
{
  if (var.getType() != ATOM_ScriptVariant::TYPE_ARRAY || var.getArraySize() == 0)
  {
    throw std::runtime_error ("Invalid parameter type");
    return 0;
  }

  ATOM_ScriptVariant *v = var.asArray();
  for (int i = 0; i < var.getArraySize(); ++i)
  {
    if (v[i].getType() != ATOM_ScriptVariant::TYPE_STRING)
    {
      throw std::runtime_error ("Invalid parameter type");
    }
  }

  CStructure *p = ATOM_NEW(CStructure);
  p->setPack (pack);

  for (int i = 0; i < var.getArraySize(); ++i)
  {
    ATOM_STRING s = v[i].asString();
    int type;
    unsigned size = 0;

    ATOM_STRING name = GetToken (s);
    ATOM_STRING sep = GetToken (s);
    if (sep != ":")
    {
      throw std::runtime_error("Invalid member description");
    }
    ATOM_STRING t = GetToken (s);

    if (t ==  "int")
      type = CStructure::TYPE_INT;
    else if (t == "uint")
      type = CStructure::TYPE_UINT;
    else if (t == "short")
      type = CStructure::TYPE_SHORT;
    else if (t == "ushort")
      type = CStructure::TYPE_USHORT;
    else if (t == "char")
      type = CStructure::TYPE_BYTE;
    else if (t == "uchar")
      type = CStructure::TYPE_UBYTE;
    else if (t == "float")
      type = CStructure::TYPE_FLOAT;
    else if (t == "double")
      type = CStructure::TYPE_DOUBLE;
    else if (t == "pointer")
      type = CStructure::TYPE_POINTER;
    else if (t == "string")
      type = CStructure::TYPE_STRINGP;
    else
    {
      unsigned l = t.length();
      if (l > 6 && !strncmp(t.c_str(), "char[", 5) && t[l-1] == ']')
      {
        t = t.substr (5, l-6);
        type = CStructure::TYPE_STRINGA;
        size = atoi (t.c_str());
      }
      else
      {
        throw std::runtime_error("Invalid member description");
      }
    }

    p->addMember (name.c_str(), type, size);
  }

  return p;
}

CStructure::CStructure (void)
{
  _M_pack = 4;
  _M_structure = 0;
}

CStructure::~CStructure (void)
{
  if (_M_structure)
  {
    for (unsigned i = 0; i < _M_members.size(); ++i)
    {
      MemberInfo &mi = _M_members[i];
      char *p = (char*)getStructure ();
      p += mi.offset;

      if (mi.type == TYPE_STRINGP)
      {
        char **val = (char**)p;
        if (*val)
        {
          free (*val);
        }
      }
    }
    _aligned_free (_M_structure);
  }
}

void CStructure::setPack (unsigned pack)
{
  _M_pack = pack;
}

void CStructure::addMember (const char *name, int type, unsigned size)
{
  MemberInfo mi;
  mi.type = type;
  mi.name = name;
  switch (type)
  {
  case TYPE_INT:
  case TYPE_UINT:
    mi.size = sizeof(int);
    break;
  case TYPE_SHORT:
  case TYPE_USHORT:
    mi.size = sizeof(short);
    break;
  case TYPE_BYTE:
  case TYPE_UBYTE:
    mi.size = sizeof(char);
    break;
  case TYPE_FLOAT:
    mi.size = sizeof(float);
    break;
  case TYPE_DOUBLE:
    mi.size = sizeof(double);
    break;
  case TYPE_POINTER:
    mi.size = sizeof(void*);
    break;
  case TYPE_STRINGP:
    mi.size = sizeof(char*);
    break;
  case TYPE_STRINGA:
    mi.size = size;
    break;
  default:
    throw std::runtime_error ("Invalid type");
    break;
  }

  if (_M_members.size() == 0)
  {
    mi.offset = 0;
  }
  else
  {
    unsigned cap = _M_members.back().offset + _M_members.back().size;
    unsigned align = (mi.size < cap) ? mi.size : _M_pack;
    mi.offset = (0 == cap % align) ? cap : (cap + align - 1) & ~(align - 1);
  }

  _M_members.push_back (mi);
};

void CStructure::createStructure (void) const
{
  if (!_M_members.empty())
  {
    unsigned size = _M_members.back().offset + _M_members.back().size;
    size = (size + _M_pack - 1) & ~(_M_pack - 1);
    _M_structure = (char*)_aligned_malloc (size, _M_pack);
    memset (_M_structure, 0, size);
  }
}

void *CStructure::getStructure (void) const
{
  if (!_M_structure)
  {
    createStructure ();
  }
  return _M_structure;
}

ATOM_ScriptVariant CStructure::getMember (const char *name) const
{
  for (unsigned i = 0; i < _M_members.size(); ++i)
  {
    const MemberInfo &mi = _M_members[i];
    if (mi.name == name)
    {
      char *p = (char*)getStructure ();
      p += mi.offset;

      switch (mi.type)
      {
      case TYPE_INT:
        {
          return (float)*((int*)p);
        }
      case TYPE_UINT:
        {
          return (float)*((unsigned*)p);
        }
      case TYPE_SHORT:
        {
          return (float)*((short*)p);
        }
      case TYPE_USHORT:
        {
          return (float)*((unsigned short*)p);
        }
      case TYPE_BYTE:
        {
          return (float)*((char*)p);
        }
      case TYPE_UBYTE:
        {
          return (float)*((unsigned char*)p);
        }
      case TYPE_FLOAT:
        {
          return *((float*)p);
        }
      case TYPE_DOUBLE:
        {
          return (float)(*((double*)p));
        }
      case TYPE_POINTER:
        {
          return *((void**)p);
        }
      case TYPE_STRINGP:
        {
          return (const char*)(*((char**)p));
        }
      case TYPE_STRINGA:
        {
          return (const char*)p;
        }
      default:
        throw std::runtime_error ("Invalid type");
        break;
      }

      break;
    }
  }

  return ATOM_ScriptVariant();
}

void CStructure::setMember (const char *name, const ATOM_ScriptVariant &v)
{
  for (unsigned i = 0; i < _M_members.size(); ++i)
  {
    MemberInfo &mi = _M_members[i];
    if (mi.name == name)
    {
      char *p = (char*)getStructure ();
      p += mi.offset;

      switch (mi.type)
      {
      case TYPE_INT:
        {
          int *val = (int*)p;
          if (v.getType() == ATOM_ScriptVariant::TYPE_INT)
            *val = v.asInteger();
          else if (v.getType() == ATOM_ScriptVariant::TYPE_FLOAT)
            *val = v.asFloat();
          else
            throw std::runtime_error ("Invalid value");
          break;
        }
      case TYPE_UINT:
        {
          unsigned *val = (unsigned*)p;
          if (v.getType() == ATOM_ScriptVariant::TYPE_INT)
            *val = v.asInteger();
          else if (v.getType() == ATOM_ScriptVariant::TYPE_FLOAT)
            *val = v.asFloat();
          else
            throw std::runtime_error ("Invalid value");
          break;
        }
      case TYPE_SHORT:
        {
          short *val = (short*)p;
          if (v.getType() == ATOM_ScriptVariant::TYPE_INT)
            *val = v.asInteger();
          else if (v.getType() == ATOM_ScriptVariant::TYPE_FLOAT)
            *val = v.asFloat();
          else
            throw std::runtime_error ("Invalid value");
          break;
        }
      case TYPE_USHORT:
        {
          unsigned short *val = (unsigned short*)p;
          if (v.getType() == ATOM_ScriptVariant::TYPE_INT)
            *val = v.asInteger();
          else if (v.getType() == ATOM_ScriptVariant::TYPE_FLOAT)
            *val = v.asFloat();
          else
            throw std::runtime_error ("Invalid value");
          break;
        }
      case TYPE_BYTE:
        {
          char *val = (char*)p;
          if (v.getType() == ATOM_ScriptVariant::TYPE_INT)
            *val = v.asInteger();
          else if (v.getType() == ATOM_ScriptVariant::TYPE_FLOAT)
            *val = v.asFloat();
          else
            throw std::runtime_error ("Invalid value");
          break;
        }
      case TYPE_UBYTE:
        {
          unsigned char *val = (unsigned char*)p;
          if (v.getType() == ATOM_ScriptVariant::TYPE_INT)
            *val = v.asInteger();
          else if (v.getType() == ATOM_ScriptVariant::TYPE_FLOAT)
            *val = v.asFloat();
          else
            throw std::runtime_error ("Invalid value");
          break;
        }
      case TYPE_FLOAT:
        {
          float *val = (float*)p;
          if (v.getType() == ATOM_ScriptVariant::TYPE_INT)
            *val = v.asInteger();
          else if (v.getType() == ATOM_ScriptVariant::TYPE_FLOAT)
            *val = v.asFloat();
          else
            throw std::runtime_error ("Invalid value");
          break;
        }
      case TYPE_DOUBLE:
        {
          double *val = (double*)p;
          if (v.getType() == ATOM_ScriptVariant::TYPE_INT)
            *val = v.asInteger();
          else if (v.getType() == ATOM_ScriptVariant::TYPE_FLOAT)
            *val = v.asFloat();
          else
            throw std::runtime_error ("Invalid value");
          break;
        }
      case TYPE_POINTER:
        {
          void **val = (void**)p;
          if (v.getType() == ATOM_ScriptVariant::TYPE_POINTER)
            *val = v.asPointer();
          else if (v.getType() == ATOM_ScriptVariant::TYPE_NONE)
            *val = 0;
          else
            throw std::runtime_error ("Invalid value");
          break;
        }
      case TYPE_STRINGP:
        {
          char **val = (char**)p;
          if (*val)
          {
            free (*val);
            *val = 0;
          }
          if (v.getType() == ATOM_ScriptVariant::TYPE_STRING)
            *val = strdup (v.asString());
          else if (v.getType() == ATOM_ScriptVariant::TYPE_NONE)
            *val = 0;
          else if (v.getType() == ATOM_ScriptVariant::TYPE_POINTER && v.asPointer() == 0)
            *val = 0;
          else
            throw std::runtime_error ("Invalid value");
          break;
        }
      case TYPE_STRINGA:
        {
          char *val = (char*)p;
          if (v.getType() == ATOM_ScriptVariant::TYPE_STRING)
            strcpy (val, v.asString());
          else if (v.getType() == ATOM_ScriptVariant::TYPE_NONE)
            *val = '\0';
          else
            throw std::runtime_error ("Invalid value");
          break;
        }
      default:
        throw std::runtime_error ("Invalid type");
        break;
      }

      break;
    }
  }
}

/*
void *_loadfunction (const char *dll, const char *funcname)
{
  HMODULE hDll = ::LoadLibrary (dll);
  if (hDll)
  {
    void *func = (void*)::GetProcAddress (hDll, funcname);
    if (!func)
    {
      ::FreeLibrary (hDll);
    }
    return func;
  }
  return 0;
}

ATOM_DynamicUserFunction *loadfunction (const char *dll, const char *funcname, const char *protocol)
{
  HMODULE hDll = ::LoadLibrary (dll);
  if (hDll)
  {
    void *func = (void*)::GetProcAddress (hDll, funcname);
    if (!func)
    {
      ::FreeLibrary (hDll);
      return 0;
    }
      
    return ATOM_NEW(ATOM_DynamicUserFunction, ATOM_DynamicUserFunction::create (func, protocol));
  }
  return 0;
}
*/

void sleepFunc (unsigned time)
{
	ATOM_Coroutine::yieldTo (NULL, time);
}

//ATOM_SCRIPT_DECLARE_TYPE_TRAITS(atom.script.ATOM_DynamicFunction, ATOM_DynamicUserFunction, false)
//ATOM_SCRIPT_DECLARE_TYPE_TRAITS(atom.script.CStructure, CStructure, true)

ATOM_SCRIPT_BEGIN_FUNCTION_TABLE(scriptFuncs)
  //ATOM_DECLARE_FUNCTION_DEREF(script.loadfunction, loadfunction, ATOM_DeleteDerefFunction<ATOM_DynamicUserFunction>)
  ATOM_DECLARE_FUNCTION(script.sleep, sleepFunc)
ATOM_SCRIPT_END_FUNCTION_TABLE

/*
ATOM_SCRIPT_BEGIN_TYPE_TABLE(scriptTypes)
  ATOM_SCRIPT_BEGIN_CLASS_NOPARENT(atom.script.ATOM_DynamicFunction, ATOM_DynamicUserFunction)
    ATOM_DECLARE_CONSTRUCTOR(ATOM_DynamicUserFunction::create)
    ATOM_DECLARE_CALL_OP(ATOM_DynamicUserFunction::execute)
    ATOM_DECLARE_STATIC_INT_CONSTANT(CDECL, ATOM_SCRIPT_DF_CALLCONV_CDECL)
    ATOM_DECLARE_STATIC_INT_CONSTANT(STDCALL, ATOM_SCRIPT_DF_CALLCONV_STDCALL)
    ATOM_DECLARE_STATIC_INT_CONSTANT(INT, ATOM_ScriptVariant::TYPE_INT)
    ATOM_DECLARE_STATIC_INT_CONSTANT(FLOAT, ATOM_ScriptVariant::TYPE_FLOAT)
    ATOM_DECLARE_STATIC_INT_CONSTANT(STRING, ATOM_ScriptVariant::TYPE_STRING)
    ATOM_DECLARE_STATIC_INT_CONSTANT(BOOL, ATOM_ScriptVariant::TYPE_BOOL)
    ATOM_DECLARE_STATIC_INT_CONSTANT(POINTER, ATOM_ScriptVariant::TYPE_POINTER)
    ATOM_DECLARE_STATIC_INT_CONSTANT(VOID, ATOM_ScriptVariant::TYPE_NONE)
  ATOM_SCRIPT_END_CLASS()
  ATOM_SCRIPT_BEGIN_NONCOPYABLE_CLASS_NOPARENT(atom.script.CStructure, CStructure)
    ATOM_DECLARE_CONSTRUCTOR(ConstructCStructure)
    ATOM_DECLARE_METHOD(set, CStructure::setMember)
    ATOM_DECLARE_METHOD(get, CStructure::getMember)
    ATOM_DECLARE_METHOD(getStructure, CStructure::getStructure)
  ATOM_SCRIPT_END_CLASS()
ATOM_SCRIPT_END_FUNCTION_TABLE
*/

void ATOM_BindScript (ATOM_Script *script)
{
  ATOM_SCRIPT_REGISTER_FUNCTION_TABLE(script, scriptFuncs);
  //ATOM_SCRIPT_REGISTER_TYPE_TABLE(script, scriptTypes);
}


