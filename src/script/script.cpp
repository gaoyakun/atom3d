#include <ATOM_dbghlp.h>

#include "script.h"
#include "callback.h"
#include "scriptinterface.h"

ATOM_Script::~ATOM_Script (void)
{
  ATOM_STACK_TRACE(ATOM_Script::~ATOM_Script);

  ScriptFunctionMapIter sfi;
  for (sfi = _M_dynamic_funcs.begin(); sfi != _M_dynamic_funcs.end(); ++sfi)
  {
    FunctionMapIter fi;
    for (fi = sfi->second.begin(); fi != sfi->second.end(); ++fi)
    {
      ATOM_DELETE(fi->second);
    }
  }
}

ATOM_DynamicFunction *ATOM_Script::allocDynamicFunction (const char *function, const ATOM_Script *scp, void *callfunc, void *dst)
{
  ATOM_STACK_TRACE(ATOM_Script::allocDynamicFunction);

  ScriptFunctionMapIter sfi = _M_dynamic_funcs.find (scp);
  if (sfi != _M_dynamic_funcs.end ())
  {
    FunctionMapIter fi = sfi->second.find (function);
    if (fi != sfi->second.end())
    {
      return fi->second;
    }
    else
    {
      ATOM_DynamicFunction *df = ATOM_NEW(ATOM_DynamicFunction);
      df->create (function, scp, callfunc, dst);
      sfi->second[function] = df;
      return df;
    }
  }
  else
  {
    ATOM_DynamicFunction *df = ATOM_NEW(ATOM_DynamicFunction);
    df->create (function, scp, callfunc, dst);
    _M_dynamic_funcs[scp][function] = df;
    return df;
  }
}

void ATOM_Script::updateClassStructDependencies (void)
{
  ATOM_STACK_TRACE(ATOM_Script::updateClassStructDependencies);

  for (unsigned i = 0; i < _M_classstructs.size(); ++i)
  {
    ATOM_ClassStruct *p = _M_classstructs[i];
    if (p->parentname && !p->parent)
    {
      for (unsigned j = 0; j < _M_classstructs.size(); ++j)
      {
        if (j != i)
        {
          ATOM_ClassStruct *q = _M_classstructs[j];
          if (!strcmp (q->name, p->parentname))
          {
            p->parent = q;
            break;
          }
        }
      }
    }
  }
}

void ATOM_Script::registerTypes (ATOM_ClassStruct *types, unsigned num)
{
  ATOM_STACK_TRACE(ATOM_Script::registerTypes);

  for (unsigned i = 0; i < num; ++i)
  {
    ATOM_ClassStruct *p = types + i;
    const char *s = strrchr (p->name, '.');
    p->purename = s ? s+1 : p->name;
    _M_classstructs.push_back (p);
  }
  updateClassStructDependencies ();

  for (unsigned i = 0; i < num; ++i)
  {
    registerUserType (types + i);
  }
}

void ATOM_Script::registerFunctions (ATOM_FunctionStruct *funcs, unsigned num)
{
  ATOM_STACK_TRACE(ATOM_Script::registerFunctions);

  for (unsigned i = 0; i < num; ++i)
  {
    registerUserFunction (funcs + i);
  }
}

void ATOM_Script::registerConstants (ATOM_FunctionStruct *constants, unsigned num)
{
  ATOM_STACK_TRACE(ATOM_Script::registerConstants);

  for (unsigned i = 0; i < num; ++i)
  {
    registerUserConstant (constants + i);
  }
}

void ATOM_Script::setGlobali (const char *name, int value)
{
	setGlobaliImp (name, value);
}

void ATOM_Script::setGlobalf (const char *name, float value)
{
	setGlobalfImp (name, value);
}

void ATOM_Script::setGlobals (const char *name, const char *value)
{
	setGlobalsImp (name, value);
}

void ATOM_Script::setGlobalp (const char *name, void *value)
{
	setGlobalpImp (name, value);
}


