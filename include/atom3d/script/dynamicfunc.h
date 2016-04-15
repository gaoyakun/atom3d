#ifndef __ATOM_SCRIPT_DYNAMICFUNC_H
#define __ATOM_SCRIPT_DYNAMICFUNC_H

#if _MSC_VER > 1000
# pragma once
#endif

// Calling conversion of dynamic function
#define ATOM_SCRIPT_DF_CALLCONV_UNKNOWN  -1  
#define ATOM_SCRIPT_DF_CALLCONV_CDECL     0
#define ATOM_SCRIPT_DF_CALLCONV_STDCALL   1

#include "basedefs.h"
#include "var.h"

class ATOM_SCRIPT_API ATOM_DynamicUserFunction
{
public:
  ATOM_DynamicUserFunction(void);
  ATOM_DynamicUserFunction(void *func, int callconv, int returntype, int fixedparams);

public:
  ATOM_ScriptVariant execute (const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5,
                   const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7, const ATOM_ScriptVariant &a8, const ATOM_ScriptVariant &a9, const ATOM_ScriptVariant &a10,
                   const ATOM_ScriptVariant &a11, const ATOM_ScriptVariant &a12, const ATOM_ScriptVariant &a13, const ATOM_ScriptVariant &a14, const ATOM_ScriptVariant &a15);

public:
  static ATOM_DynamicUserFunction create(const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2);

private:
  bool parseProtocol (const char *protocol);
  int getOriginParamType (int index) const;
  bool extendFloat (int index) const;

private:
  ATOM_ScriptVariant execute0 (void);
  ATOM_ScriptVariant execute1 (const ATOM_ScriptVariant &a1);
  ATOM_ScriptVariant execute2 (const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2);
  ATOM_ScriptVariant execute3 (const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3);
  ATOM_ScriptVariant execute4 (const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4);
  ATOM_ScriptVariant execute5 (const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5);
  ATOM_ScriptVariant execute6 (const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                    const ATOM_ScriptVariant &a6);
  ATOM_ScriptVariant execute7 (const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                    const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7);
  ATOM_ScriptVariant execute8 (const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                    const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7, const ATOM_ScriptVariant &a8);
  ATOM_ScriptVariant execute9 (const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                    const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7, const ATOM_ScriptVariant &a8, const ATOM_ScriptVariant &a9);
  ATOM_ScriptVariant execute10(const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                    const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7, const ATOM_ScriptVariant &a8, const ATOM_ScriptVariant &a9, const ATOM_ScriptVariant &a10);
  ATOM_ScriptVariant execute11(const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                    const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7, const ATOM_ScriptVariant &a8, const ATOM_ScriptVariant &a9, const ATOM_ScriptVariant &a10,
                    const ATOM_ScriptVariant &a11);
  ATOM_ScriptVariant execute12(const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                    const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7, const ATOM_ScriptVariant &a8, const ATOM_ScriptVariant &a9, const ATOM_ScriptVariant &a10,
                    const ATOM_ScriptVariant &a11, const ATOM_ScriptVariant &a12);
  ATOM_ScriptVariant execute13(const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                    const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7, const ATOM_ScriptVariant &a8, const ATOM_ScriptVariant &a9, const ATOM_ScriptVariant &a10,
                    const ATOM_ScriptVariant &a11, const ATOM_ScriptVariant &a12, const ATOM_ScriptVariant &a13);
  ATOM_ScriptVariant execute14(const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                    const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7, const ATOM_ScriptVariant &a8, const ATOM_ScriptVariant &a9, const ATOM_ScriptVariant &a10,
                    const ATOM_ScriptVariant &a11, const ATOM_ScriptVariant &a12, const ATOM_ScriptVariant &a13, const ATOM_ScriptVariant &a14);
  ATOM_ScriptVariant execute15(const ATOM_ScriptVariant &a1, const ATOM_ScriptVariant &a2, const ATOM_ScriptVariant &a3, const ATOM_ScriptVariant &a4, const ATOM_ScriptVariant &a5, 
                    const ATOM_ScriptVariant &a6, const ATOM_ScriptVariant &a7, const ATOM_ScriptVariant &a8, const ATOM_ScriptVariant &a9, const ATOM_ScriptVariant &a10,
                    const ATOM_ScriptVariant &a11, const ATOM_ScriptVariant &a12, const ATOM_ScriptVariant &a13, const ATOM_ScriptVariant &a14, const ATOM_ScriptVariant &a15);

private:
  int _M_return_type;
  int _M_callconv;
  void *_M_func;
  int _M_fixed_params_count;
  ATOM_VECTOR<int> _M_param_types;
};

#endif //__ATOM_SCRIPT_DYNAMICFUNC_H

