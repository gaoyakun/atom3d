#include <ATOM_dbghlp.h>
#include <stdexcept>
#include "dynamicfunc.h"

#if !defined(_M_IA64) && !defined(_M_AMD64)

#if ATOM3D_COMPILER_MSVC

typedef long long (*CDECL_Function) (const unsigned*, unsigned, void*);
typedef long long (*STDCALL_Function) (const unsigned*, unsigned, void*);

void CallDynamicUserFunction_CDECL (const unsigned *args, unsigned paramSize, void *func)
{
    // Copy the data to the real stack. If we fail to do
    // this we may run into trouble in case of exceptions.
    __asm
    {
	    // We must save registers that are used
      push ecx

	    // Copy arguments from script
	    // stack to application stack
    mov  ecx, paramSize
	    mov  eax, args
	    add  eax, ecx
	    cmp  ecx, 0
	    je   endcopy
copyloop:
	    sub  eax, 4
	    push dword ptr [eax]
	    sub  ecx, 4
	    jne  copyloop
endcopy:

	    // Call function
	    call [func]

	    // Pop arguments from stack
	    add  esp, paramSize

	    // Restore registers
	    pop  ecx
    }
}

void CallDynamicUserFunction_STDCALL (const unsigned *args, int paramSize, size_t func)
{
    // Copy the data to the real stack. If we fail to do
    // this we may run into trouble in case of exceptions.
    __asm
    {
	    // We must save registers that are used
    push ecx

	    // Copy arguments from script
	    // stack to application stack
    mov  ecx, paramSize
	    mov  eax, args
	    add  eax, ecx
	    cmp  ecx, 0
	    je   endcopy
copyloop:
	    sub  eax, 4
	    push dword ptr [eax]
	    sub  ecx, 4
	    jne  copyloop
endcopy:

	    // Call function
	    call [func]

	    // The callee already removed parameters from the stack

	    // Restore registers
	    pop  ecx

	    // return value in EAX or EAX:EDX
    }
}

float GetReturnedFloat (void)
{
  unsigned f;

    // Get the float value from ST0
  __asm fstp dword ptr [f]

    return (float&)f;
}

static CDECL_Function _CallerCDECL = (CDECL_Function)CallDynamicUserFunction_CDECL;
static STDCALL_Function _CallerSTDCALL = (STDCALL_Function)CallDynamicUserFunction_STDCALL;

#endif

ATOM_DynamicUserFunction::ATOM_DynamicUserFunction (void)
{
  _M_func = 0;
  _M_return_type = ATOM_ScriptVariant::TYPE_NONE;
  _M_callconv = ATOM_SCRIPT_DF_CALLCONV_UNKNOWN;
  _M_fixed_params_count = -1;
}

ATOM_DynamicUserFunction::ATOM_DynamicUserFunction (void *func, int callconv, int returntype, int fixedparams)
{
  _M_func = func;
  _M_return_type = returntype;
  _M_callconv = callconv;
  _M_fixed_params_count = fixedparams;
}

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

  if (s.size() > 0 && (s[0] == '(' || s[0] == ')' || s[0] == ','))
  {
    ret = s[0];
    s.erase (0, 1);
    return ret;
  }

  for (unsigned i = 0; i < s.length(); ++i)
  {
    char ch = s[i];

    if (ch == ',' || ch == ' ' || ch == '\n' || ch == '\t' || ch == '(' || ch == ')')
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

static int testToken (const ATOM_STRING &token)
{
  if (token == "int")
    return ATOM_ScriptVariant::TYPE_INT;

  if (token == "float")
    return ATOM_ScriptVariant::TYPE_FLOAT;
  
  if (token == "bool")
    return ATOM_ScriptVariant::TYPE_BOOL;

  if (token == "pointer")
    return ATOM_ScriptVariant::TYPE_POINTER;

  if (token == "string")
    return ATOM_ScriptVariant::TYPE_STRING;

  return ATOM_ScriptVariant::TYPE_NONE;
}

bool ATOM_DynamicUserFunction::parseProtocol (const char *protocol)
{
  if (protocol)
  {
    ATOM_STRING s = protocol;
    ATOM_STRING tok = GetToken (s);
    _M_return_type = testToken (tok);
    if (_M_return_type == ATOM_ScriptVariant::TYPE_NONE)
      return false;

    tok = GetToken (s);
    if (tok != "(")
      return false;

    tok = GetToken (s);
    if (tok == "cdecl")
      _M_callconv = ATOM_SCRIPT_DF_CALLCONV_CDECL;
    else if (tok == "stdcall")
      _M_callconv = ATOM_SCRIPT_DF_CALLCONV_STDCALL;
    else
      return false;

    tok = GetToken (s);
    if (tok != ")")
      return false;

    tok = GetToken (s);
    if (tok != "(")
      return false;

    while (true)
    {
      tok = GetToken (s);
      if (tok == ")")
        break;

      if (tok == "...")
      {
        _M_fixed_params_count = _M_param_types.size();
        tok = GetToken (s);
        if (tok != ")")
          return false;
        tok = GetToken (s);
        return tok.empty();
      }
      else
      {
        int t = testToken (tok);
        if (t == ATOM_ScriptVariant::TYPE_NONE)
          return false;
        _M_param_types.push_back (t);
      }

      tok = GetToken (s);
      if (tok == ")")
        break;

      if (tok != ",")
        return false;
    }

    _M_fixed_params_count = -1;

    tok = GetToken (s);
    return tok.empty();
  }

  return false;
}

ATOM_DynamicUserFunction ATOM_DynamicUserFunction::create(const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2)
{
  if (a1.getType() == ATOM_ScriptVariant::TYPE_POINTER && a2.getType() == ATOM_ScriptVariant::TYPE_STRING)
  {
    ATOM_DynamicUserFunction f;
    if (f.parseProtocol (a2.asString()))
    {
      f._M_func = a1.asPointer();
      return f;
    }
    else
    {
      throw std::runtime_error ("Invalid function protocol");
    }
  }
  throw std::runtime_error("Invalid parameter type or count");
}

ATOM_ScriptVariant ATOM_DynamicUserFunction::execute (const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5,
                                      const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7, const ATOM_ScriptVariant &a8, const ATOM_ScriptVariant &a9, const ATOM_ScriptVariant &a10,
                                      const ATOM_ScriptVariant &a11, const ATOM_ScriptVariant &a12, const ATOM_ScriptVariant &a13, const ATOM_ScriptVariant &a14, const ATOM_ScriptVariant &a15)
{
  if (!_M_func)
  {
    throw std::runtime_error ("NULL function call");
  }

  if (_M_callconv != ATOM_SCRIPT_DF_CALLCONV_CDECL && _M_callconv != ATOM_SCRIPT_DF_CALLCONV_STDCALL)
  {
    throw std::runtime_error ("Invalid calling conversion");
  }

  if (a1.getType() == ATOM_ScriptVariant::TYPE_NONE)
  {
    return execute0 ();
  }

  if (a2.getType() == ATOM_ScriptVariant::TYPE_NONE)
  {
    return execute1 (a1);
  }

  if (a3.getType() == ATOM_ScriptVariant::TYPE_NONE)
  {
    return execute2 (a1, a2);
  }

  if (a4.getType() == ATOM_ScriptVariant::TYPE_NONE)
  {
    return execute3 (a1, a2, a3);
  }

  if (a5.getType() == ATOM_ScriptVariant::TYPE_NONE)
  {
    return execute4 (a1, a2, a3, a4);
  }

  if (a6.getType() == ATOM_ScriptVariant::TYPE_NONE)
  {
    return execute5 (a1, a2, a3, a4, a5);
  }

  if (a7.getType() == ATOM_ScriptVariant::TYPE_NONE)
  {
    return execute6 (a1, a2, a3, a4, a5, a6);
  }

  if (a8.getType() == ATOM_ScriptVariant::TYPE_NONE)
  {
    return execute7 (a1, a2, a3, a4, a5, a6, a7);
  }

  if (a9.getType() == ATOM_ScriptVariant::TYPE_NONE)
  {
    return execute8 (a1, a2, a3, a4, a5, a6, a7, a8);
  }

  if (a10.getType() == ATOM_ScriptVariant::TYPE_NONE)
  {
    return execute9 (a1, a2, a3, a4, a5, a6, a7, a8, a9);
  }

  if (a11.getType() == ATOM_ScriptVariant::TYPE_NONE)
  {
    return execute10 (a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
  }

  if (a12.getType() == ATOM_ScriptVariant::TYPE_NONE)
  {
    return execute11 (a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
  }

  if (a13.getType() == ATOM_ScriptVariant::TYPE_NONE)
  {
    return execute12 (a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);
  }

  if (a14.getType() == ATOM_ScriptVariant::TYPE_NONE)
  {
    return execute13 (a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
  }

  if (a15.getType() == ATOM_ScriptVariant::TYPE_NONE)
  {
    return execute14 (a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14);
  }

  return execute15 (a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15);
}

static int makeParameter (const ATOM_ScriptVariant &v, unsigned *buffer, bool extendFloat, int originType)
{
  switch (v.getType())
  {
  case ATOM_ScriptVariant::TYPE_BOOL:
    if (originType == ATOM_ScriptVariant::TYPE_NONE || originType == ATOM_ScriptVariant::TYPE_INT)
    {
      *buffer = v.asBool() ? 1 : 0;
    }
    else
    {
      throw std::runtime_error ("Wrong parameter type");
    }
    return 1;
  case ATOM_ScriptVariant::TYPE_FLOAT:
    {
      if (!extendFloat)
      {
        if (originType == ATOM_ScriptVariant::TYPE_NONE || originType == ATOM_ScriptVariant::TYPE_FLOAT)
        {
          float f = v.asFloat();
          *buffer = (unsigned&)f;
        }
        else if (originType == ATOM_ScriptVariant::TYPE_INT)
        {
          float f = v.asFloat();
          *buffer = f;
        }
        else if (originType == ATOM_ScriptVariant::TYPE_BOOL)
        {
          *buffer = v.asFloat() != 0;
        }
        else
        {
          throw std::runtime_error ("Wrong parameter type");
        }
        return 1;
      }
      else
      {
        double d = v.asFloat();
        unsigned *p = (unsigned*)&d;
        buffer[0] = p[0];
        buffer[1] = p[1];
        return 2;
      }
    }
  case ATOM_ScriptVariant::TYPE_INT:
    if (originType == ATOM_ScriptVariant::TYPE_NONE || originType == ATOM_ScriptVariant::TYPE_INT)
    {
      *buffer = (unsigned)v.asInteger();
    }
    else if (originType == ATOM_ScriptVariant::TYPE_FLOAT)
    {
      float f = v.asInteger();
      *buffer = (unsigned&)f;
    }
    else if (originType == ATOM_ScriptVariant::TYPE_BOOL)
    {
      *buffer = v.asInteger() != 0;
    }
    else
    {
      throw std::runtime_error ("Wrong parameter type");
    }
    return 1;
  case ATOM_ScriptVariant::TYPE_POINTER:
    if (originType == ATOM_ScriptVariant::TYPE_NONE || originType == ATOM_ScriptVariant::TYPE_POINTER || originType == ATOM_ScriptVariant::TYPE_INT || originType == ATOM_ScriptVariant::TYPE_STRING)
      *buffer = (unsigned)v.asPointer();
    else
    {
      throw std::runtime_error ("Wrong parameter type");
    }
    return 1;
  case ATOM_ScriptVariant::TYPE_STRING:
    if (originType == ATOM_ScriptVariant::TYPE_NONE || originType == ATOM_ScriptVariant::TYPE_STRING || originType == ATOM_ScriptVariant::TYPE_POINTER)
      *buffer = (unsigned)v.asString();
    else
    {
      throw std::runtime_error ("Wrong parameter type");
    }
    return 1;
  default:
    throw std::runtime_error("Wrong parameter type");
  }
}

static ATOM_ScriptVariant makeVariant (int type, long long value)
{
#if ATOM3D_COMPILER_MSVC
  switch (type)
  {
  case ATOM_ScriptVariant::TYPE_NONE:
    return ATOM_ScriptVariant();
  case ATOM_ScriptVariant::TYPE_BOOL:
    return value != 0;
  case ATOM_ScriptVariant::TYPE_FLOAT:
    return GetReturnedFloat();
  case ATOM_ScriptVariant::TYPE_INT:
    return value;
  case ATOM_ScriptVariant::TYPE_POINTER:
    return (void*)value;
  case ATOM_ScriptVariant::TYPE_STRING:
    return (const char*)value;
  default:
    return ATOM_ScriptVariant();
  }
#else
    return ATOM_ScriptVariant();
#endif
}

static long long callFunction (void *func, const unsigned *params, unsigned paramsize, int callconv)
{
#if ATOM3D_COMPILER_MSVC
  if (callconv == ATOM_SCRIPT_DF_CALLCONV_CDECL)
    return _CallerCDECL(params, paramsize, func);
  else
    return _CallerSTDCALL(params, paramsize, func);
#else
	return 0;
#endif
}

int ATOM_DynamicUserFunction::getOriginParamType (int index) const
{
  return (index < int(_M_param_types.size())) ? _M_param_types[index] : ATOM_ScriptVariant::TYPE_NONE;
}

bool ATOM_DynamicUserFunction::extendFloat (int index) const
{
  return index >= int(_M_param_types.size());
}

ATOM_ScriptVariant ATOM_DynamicUserFunction::execute0 (void)
{
  if ((_M_fixed_params_count == -1 && _M_param_types.empty()) || _M_fixed_params_count == 0)
    return makeVariant (_M_return_type, callFunction (_M_func, 0, 0, _M_callconv));
  else
    throw std::runtime_error ("Invalid parameter count");
}

ATOM_ScriptVariant ATOM_DynamicUserFunction::execute1 (const ATOM_ScriptVariant &a1)
{
  if ((_M_fixed_params_count == -1 && _M_param_types.size() == 1) || _M_fixed_params_count <= 1)
  {
    unsigned params[2];
    unsigned numParams = makeParameter (a1, params, extendFloat(0), getOriginParamType(0));
    long long ret = callFunction (_M_func, params, numParams<<2, _M_callconv);
    return makeVariant (_M_return_type, ret);
  }
  else
  {
    throw std::runtime_error ("Invalid parameter count");
  }
}

ATOM_ScriptVariant ATOM_DynamicUserFunction::execute2 (const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2)
{
  if ((_M_fixed_params_count == -1 && _M_param_types.size() == 2) || _M_fixed_params_count <= 2)
  {
    unsigned params[4];
    unsigned numParams = 0;
    numParams += makeParameter (a1, params+numParams, extendFloat(0), getOriginParamType(0));
    numParams += makeParameter (a2, params+numParams, extendFloat(1), getOriginParamType(1));
    long long ret = callFunction (_M_func, params, numParams<<2, _M_callconv);
    return makeVariant (_M_return_type, ret);
  }
  else
  {
    throw std::runtime_error ("Invalid parameter count");
  }
}

ATOM_ScriptVariant ATOM_DynamicUserFunction::execute3 (const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3)
{
  if ((_M_fixed_params_count == -1 && _M_param_types.size() == 3) || _M_fixed_params_count <= 3)
  {
    unsigned params[6];
    unsigned numParams = 0;
    numParams += makeParameter (a1, params+numParams, extendFloat(0), getOriginParamType(0));
    numParams += makeParameter (a2, params+numParams, extendFloat(1), getOriginParamType(1));
    numParams += makeParameter (a3, params+numParams, extendFloat(2), getOriginParamType(2));
    long long ret = callFunction (_M_func, params, numParams<<2, _M_callconv);
    return makeVariant (_M_return_type, ret);
  }
  else
  {
    throw std::runtime_error ("Invalid parameter count");
  }
}

ATOM_ScriptVariant ATOM_DynamicUserFunction::execute4 (const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4)
{
  if ((_M_fixed_params_count == -1 && _M_param_types.size() == 4) || _M_fixed_params_count <= 4)
  {
    unsigned params[8];
    unsigned numParams = 0;
    numParams += makeParameter (a1, params+numParams, extendFloat(0), getOriginParamType(0));
    numParams += makeParameter (a2, params+numParams, extendFloat(1), getOriginParamType(1));
    numParams += makeParameter (a3, params+numParams, extendFloat(2), getOriginParamType(2));
    numParams += makeParameter (a4, params+numParams, extendFloat(3), getOriginParamType(3));
    long long ret = callFunction (_M_func, params, numParams<<2, _M_callconv);
    return makeVariant (_M_return_type, ret);
  }
  else
  {
    throw std::runtime_error ("Invalid parameter count");
  }
}

ATOM_ScriptVariant ATOM_DynamicUserFunction::execute5 (const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5)
{
  if ((_M_fixed_params_count == -1 && _M_param_types.size() == 5) || _M_fixed_params_count <= 5)
  {
    unsigned params[10];
    unsigned numParams = 0;
    numParams += makeParameter (a1, params+numParams, extendFloat(0), getOriginParamType(0));
    numParams += makeParameter (a2, params+numParams, extendFloat(1), getOriginParamType(1));
    numParams += makeParameter (a3, params+numParams, extendFloat(2), getOriginParamType(2));
    numParams += makeParameter (a4, params+numParams, extendFloat(3), getOriginParamType(3));
    numParams += makeParameter (a5, params+numParams, extendFloat(4), getOriginParamType(4));
    long long ret = callFunction (_M_func, params, numParams<<2, _M_callconv);
    return makeVariant (_M_return_type, ret);
  }
  else
  {
    throw std::runtime_error ("Invalid parameter count");
  }
}

ATOM_ScriptVariant ATOM_DynamicUserFunction::execute6 ( const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                                        const ATOM_ScriptVariant &a6)
{
  if ((_M_fixed_params_count == -1 && _M_param_types.size() == 6) || _M_fixed_params_count <= 6)
  {
    unsigned params[12];
    unsigned numParams = 0;
    numParams += makeParameter (a1, params+numParams, extendFloat(0), getOriginParamType(0));
    numParams += makeParameter (a2, params+numParams, extendFloat(1), getOriginParamType(1));
    numParams += makeParameter (a3, params+numParams, extendFloat(2), getOriginParamType(2));
    numParams += makeParameter (a4, params+numParams, extendFloat(3), getOriginParamType(3));
    numParams += makeParameter (a5, params+numParams, extendFloat(4), getOriginParamType(4));
    numParams += makeParameter (a6, params+numParams, extendFloat(5), getOriginParamType(5));
    long long ret = callFunction (_M_func, params, numParams<<2, _M_callconv);
    return makeVariant (_M_return_type, ret);
  }
  else
  {
    throw std::runtime_error ("Invalid parameter count");
  }
}

ATOM_ScriptVariant ATOM_DynamicUserFunction::execute7 ( const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                                        const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7)
{
  if ((_M_fixed_params_count == -1 && _M_param_types.size() == 7) || _M_fixed_params_count <= 7)
  {
    unsigned params[14];
    unsigned numParams = 0;
    numParams += makeParameter (a1, params+numParams, extendFloat(0), getOriginParamType(0));
    numParams += makeParameter (a2, params+numParams, extendFloat(1), getOriginParamType(1));
    numParams += makeParameter (a3, params+numParams, extendFloat(2), getOriginParamType(2));
    numParams += makeParameter (a4, params+numParams, extendFloat(3), getOriginParamType(3));
    numParams += makeParameter (a5, params+numParams, extendFloat(4), getOriginParamType(4));
    numParams += makeParameter (a6, params+numParams, extendFloat(5), getOriginParamType(5));
    numParams += makeParameter (a7, params+numParams, extendFloat(6), getOriginParamType(6));
    long long ret = callFunction (_M_func, params, numParams<<2, _M_callconv);
    return makeVariant (_M_return_type, ret);
  }
  else
  {
    throw std::runtime_error ("Invalid parameter count");
  }
}

ATOM_ScriptVariant ATOM_DynamicUserFunction::execute8 ( const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                                        const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7, const ATOM_ScriptVariant &a8)
{
  if ((_M_fixed_params_count == -1 && _M_param_types.size() == 8) || _M_fixed_params_count <= 8)
  {
    unsigned params[16];
    unsigned numParams = 0;
    numParams += makeParameter (a1, params+numParams, extendFloat(0), getOriginParamType(0));
    numParams += makeParameter (a2, params+numParams, extendFloat(1), getOriginParamType(1));
    numParams += makeParameter (a3, params+numParams, extendFloat(2), getOriginParamType(2));
    numParams += makeParameter (a4, params+numParams, extendFloat(3), getOriginParamType(3));
    numParams += makeParameter (a5, params+numParams, extendFloat(4), getOriginParamType(4));
    numParams += makeParameter (a6, params+numParams, extendFloat(5), getOriginParamType(5));
    numParams += makeParameter (a7, params+numParams, extendFloat(6), getOriginParamType(6));
    numParams += makeParameter (a8, params+numParams, extendFloat(7), getOriginParamType(7));
    long long ret = callFunction (_M_func, params, numParams<<2, _M_callconv);
    return makeVariant (_M_return_type, ret);
  }
  else
  {
    throw std::runtime_error ("Invalid parameter count");
  }
}

ATOM_ScriptVariant ATOM_DynamicUserFunction::execute9 ( const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                                        const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7, const ATOM_ScriptVariant &a8, const ATOM_ScriptVariant &a9)
{
  if ((_M_fixed_params_count == -1 && _M_param_types.size() == 9) || _M_fixed_params_count <= 9)
  {
    unsigned params[18];
    unsigned numParams = 0;
    numParams += makeParameter (a1, params+numParams, extendFloat(0), getOriginParamType(0));
    numParams += makeParameter (a2, params+numParams, extendFloat(1), getOriginParamType(1));
    numParams += makeParameter (a3, params+numParams, extendFloat(2), getOriginParamType(2));
    numParams += makeParameter (a4, params+numParams, extendFloat(3), getOriginParamType(3));
    numParams += makeParameter (a5, params+numParams, extendFloat(4), getOriginParamType(4));
    numParams += makeParameter (a6, params+numParams, extendFloat(5), getOriginParamType(5));
    numParams += makeParameter (a7, params+numParams, extendFloat(6), getOriginParamType(6));
    numParams += makeParameter (a8, params+numParams, extendFloat(7), getOriginParamType(7));
    numParams += makeParameter (a9, params+numParams, extendFloat(8), getOriginParamType(8));
    long long ret = callFunction (_M_func, params, numParams<<2, _M_callconv);
    return makeVariant (_M_return_type, ret);
  }
  else
  {
    throw std::runtime_error ("Invalid parameter count");
  }
}

ATOM_ScriptVariant ATOM_DynamicUserFunction::execute10( const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                                        const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7, const ATOM_ScriptVariant &a8, const ATOM_ScriptVariant &a9, const ATOM_ScriptVariant &a10)
{
  if ((_M_fixed_params_count == -1 && _M_param_types.size() == 10) || _M_fixed_params_count <= 10)
  {
    unsigned params[20];
    unsigned numParams = 0;
    numParams += makeParameter (a1, params+numParams, extendFloat(0), getOriginParamType(0));
    numParams += makeParameter (a2, params+numParams, extendFloat(1), getOriginParamType(1));
    numParams += makeParameter (a3, params+numParams, extendFloat(2), getOriginParamType(2));
    numParams += makeParameter (a4, params+numParams, extendFloat(3), getOriginParamType(3));
    numParams += makeParameter (a5, params+numParams, extendFloat(4), getOriginParamType(4));
    numParams += makeParameter (a6, params+numParams, extendFloat(5), getOriginParamType(5));
    numParams += makeParameter (a7, params+numParams, extendFloat(6), getOriginParamType(6));
    numParams += makeParameter (a8, params+numParams, extendFloat(7), getOriginParamType(7));
    numParams += makeParameter (a9, params+numParams, extendFloat(8), getOriginParamType(8));
    numParams += makeParameter (a10, params+numParams, extendFloat(9), getOriginParamType(9));
    long long ret = callFunction (_M_func, params, numParams<<2, _M_callconv);
    return makeVariant (_M_return_type, ret);
  }
  else
  {
    throw std::runtime_error ("Invalid parameter count");
  }
}

ATOM_ScriptVariant ATOM_DynamicUserFunction::execute11( const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                                        const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7, const ATOM_ScriptVariant &a8, const ATOM_ScriptVariant &a9, const ATOM_ScriptVariant &a10,
                                        const ATOM_ScriptVariant &a11)
{
  if ((_M_fixed_params_count == -1 && _M_param_types.size() == 11) || _M_fixed_params_count <= 11)
  {
    unsigned params[22];
    unsigned numParams = 0;
    numParams += makeParameter (a1, params+numParams, extendFloat(0), getOriginParamType(0));
    numParams += makeParameter (a2, params+numParams, extendFloat(1), getOriginParamType(1));
    numParams += makeParameter (a3, params+numParams, extendFloat(2), getOriginParamType(2));
    numParams += makeParameter (a4, params+numParams, extendFloat(3), getOriginParamType(3));
    numParams += makeParameter (a5, params+numParams, extendFloat(4), getOriginParamType(4));
    numParams += makeParameter (a6, params+numParams, extendFloat(5), getOriginParamType(5));
    numParams += makeParameter (a7, params+numParams, extendFloat(6), getOriginParamType(6));
    numParams += makeParameter (a8, params+numParams, extendFloat(7), getOriginParamType(7));
    numParams += makeParameter (a9, params+numParams, extendFloat(8), getOriginParamType(8));
    numParams += makeParameter (a10, params+numParams, extendFloat(9), getOriginParamType(9));
    numParams += makeParameter (a11, params+numParams, extendFloat(10), getOriginParamType(10));
    long long ret = callFunction (_M_func, params, numParams<<2, _M_callconv);
    return makeVariant (_M_return_type, ret);
  }
  else
  {
    throw std::runtime_error ("Invalid parameter count");
  }
}

ATOM_ScriptVariant ATOM_DynamicUserFunction::execute12( const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                                        const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7, const ATOM_ScriptVariant &a8, const ATOM_ScriptVariant &a9, const ATOM_ScriptVariant &a10,
                                        const ATOM_ScriptVariant &a11, const ATOM_ScriptVariant &a12)
{
  if ((_M_fixed_params_count == -1 && _M_param_types.size() == 12) || _M_fixed_params_count <= 12)
  {
    unsigned params[24];
    unsigned numParams = 0;
    numParams += makeParameter (a1, params+numParams, extendFloat(0), getOriginParamType(0));
    numParams += makeParameter (a2, params+numParams, extendFloat(1), getOriginParamType(1));
    numParams += makeParameter (a3, params+numParams, extendFloat(2), getOriginParamType(2));
    numParams += makeParameter (a4, params+numParams, extendFloat(3), getOriginParamType(3));
    numParams += makeParameter (a5, params+numParams, extendFloat(4), getOriginParamType(4));
    numParams += makeParameter (a6, params+numParams, extendFloat(5), getOriginParamType(5));
    numParams += makeParameter (a7, params+numParams, extendFloat(6), getOriginParamType(6));
    numParams += makeParameter (a8, params+numParams, extendFloat(7), getOriginParamType(7));
    numParams += makeParameter (a9, params+numParams, extendFloat(8), getOriginParamType(8));
    numParams += makeParameter (a10, params+numParams, extendFloat(9), getOriginParamType(9));
    numParams += makeParameter (a11, params+numParams, extendFloat(10), getOriginParamType(10));
    numParams += makeParameter (a12, params+numParams, extendFloat(11), getOriginParamType(11));
    long long ret = callFunction (_M_func, params, numParams<<2, _M_callconv);
    return makeVariant (_M_return_type, ret);
  }
  else
  {
    throw std::runtime_error ("Invalid parameter count");
  }
}

ATOM_ScriptVariant ATOM_DynamicUserFunction::execute13( const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                                        const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7, const ATOM_ScriptVariant &a8, const ATOM_ScriptVariant &a9, const ATOM_ScriptVariant &a10,
                                        const ATOM_ScriptVariant &a11, const ATOM_ScriptVariant &a12, const ATOM_ScriptVariant &a13)
{
  if ((_M_fixed_params_count == -1 && _M_param_types.size() == 13) || _M_fixed_params_count <= 13)
  {
    unsigned params[26];
    unsigned numParams = 0;
    numParams += makeParameter (a1, params+numParams, extendFloat(0), getOriginParamType(0));
    numParams += makeParameter (a2, params+numParams, extendFloat(1), getOriginParamType(1));
    numParams += makeParameter (a3, params+numParams, extendFloat(2), getOriginParamType(2));
    numParams += makeParameter (a4, params+numParams, extendFloat(3), getOriginParamType(3));
    numParams += makeParameter (a5, params+numParams, extendFloat(4), getOriginParamType(4));
    numParams += makeParameter (a6, params+numParams, extendFloat(5), getOriginParamType(5));
    numParams += makeParameter (a7, params+numParams, extendFloat(6), getOriginParamType(6));
    numParams += makeParameter (a8, params+numParams, extendFloat(7), getOriginParamType(7));
    numParams += makeParameter (a9, params+numParams, extendFloat(8), getOriginParamType(8));
    numParams += makeParameter (a10, params+numParams, extendFloat(9), getOriginParamType(9));
    numParams += makeParameter (a11, params+numParams, extendFloat(10), getOriginParamType(10));
    numParams += makeParameter (a12, params+numParams, extendFloat(11), getOriginParamType(11));
    numParams += makeParameter (a13, params+numParams, extendFloat(12), getOriginParamType(12));
    long long ret = callFunction (_M_func, params, numParams<<2, _M_callconv);
    return makeVariant (_M_return_type, ret);
  }
  else
  {
    throw std::runtime_error ("Invalid parameter count");
  }
}

ATOM_ScriptVariant ATOM_DynamicUserFunction::execute14( const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                                        const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7, const ATOM_ScriptVariant &a8, const ATOM_ScriptVariant &a9, const ATOM_ScriptVariant &a10,
                                        const ATOM_ScriptVariant &a11, const ATOM_ScriptVariant &a12, const ATOM_ScriptVariant &a13, const ATOM_ScriptVariant &a14)
{
  if ((_M_fixed_params_count == -1 && _M_param_types.size() == 14) || _M_fixed_params_count <= 14)
  {
    unsigned params[28];
    unsigned numParams = 0;
    numParams += makeParameter (a1, params+numParams, extendFloat(0), getOriginParamType(0));
    numParams += makeParameter (a2, params+numParams, extendFloat(1), getOriginParamType(1));
    numParams += makeParameter (a3, params+numParams, extendFloat(2), getOriginParamType(2));
    numParams += makeParameter (a4, params+numParams, extendFloat(3), getOriginParamType(3));
    numParams += makeParameter (a5, params+numParams, extendFloat(4), getOriginParamType(4));
    numParams += makeParameter (a6, params+numParams, extendFloat(5), getOriginParamType(5));
    numParams += makeParameter (a7, params+numParams, extendFloat(6), getOriginParamType(6));
    numParams += makeParameter (a8, params+numParams, extendFloat(7), getOriginParamType(7));
    numParams += makeParameter (a9, params+numParams, extendFloat(8), getOriginParamType(8));
    numParams += makeParameter (a10, params+numParams, extendFloat(9), getOriginParamType(9));
    numParams += makeParameter (a11, params+numParams, extendFloat(10), getOriginParamType(10));
    numParams += makeParameter (a12, params+numParams, extendFloat(11), getOriginParamType(11));
    numParams += makeParameter (a13, params+numParams, extendFloat(12), getOriginParamType(12));
    numParams += makeParameter (a14, params+numParams, extendFloat(13), getOriginParamType(13));
    long long ret = callFunction (_M_func, params, numParams<<2, _M_callconv);
    return makeVariant (_M_return_type, ret);
  }
  else
  {
    throw std::runtime_error ("Invalid parameter count");
  }
}

ATOM_ScriptVariant ATOM_DynamicUserFunction::execute15( const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                                        const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7, const ATOM_ScriptVariant &a8, const ATOM_ScriptVariant &a9, const ATOM_ScriptVariant &a10,
                                        const ATOM_ScriptVariant &a11, const ATOM_ScriptVariant &a12, const ATOM_ScriptVariant &a13, const ATOM_ScriptVariant &a14, const ATOM_ScriptVariant &a15)
{
  if ((_M_fixed_params_count == -1 && _M_param_types.size() == 15) || _M_fixed_params_count <= 15)
  {
    unsigned params[30];
    unsigned numParams = 0;
    numParams += makeParameter (a1, params+numParams, extendFloat(0), getOriginParamType(0));
    numParams += makeParameter (a2, params+numParams, extendFloat(1), getOriginParamType(1));
    numParams += makeParameter (a3, params+numParams, extendFloat(2), getOriginParamType(2));
    numParams += makeParameter (a4, params+numParams, extendFloat(3), getOriginParamType(3));
    numParams += makeParameter (a5, params+numParams, extendFloat(4), getOriginParamType(4));
    numParams += makeParameter (a6, params+numParams, extendFloat(5), getOriginParamType(5));
    numParams += makeParameter (a7, params+numParams, extendFloat(6), getOriginParamType(6));
    numParams += makeParameter (a8, params+numParams, extendFloat(7), getOriginParamType(7));
    numParams += makeParameter (a9, params+numParams, extendFloat(8), getOriginParamType(8));
    numParams += makeParameter (a10, params+numParams, extendFloat(9), getOriginParamType(9));
    numParams += makeParameter (a11, params+numParams, extendFloat(10), getOriginParamType(10));
    numParams += makeParameter (a12, params+numParams, extendFloat(11), getOriginParamType(11));
    numParams += makeParameter (a13, params+numParams, extendFloat(12), getOriginParamType(12));
    numParams += makeParameter (a14, params+numParams, extendFloat(13), getOriginParamType(13));
    numParams += makeParameter (a15, params+numParams, extendFloat(14), getOriginParamType(14));
    long long ret = callFunction (_M_func, params, numParams<<2, _M_callconv);
    return makeVariant (_M_return_type, ret);
  }
  else
  {
    throw std::runtime_error ("Invalid parameter count");
  }
}
#endif
