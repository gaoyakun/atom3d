#include "basedefs.h"

#if defined(WIN32)
# include <windows.h>
#endif // WIN32

#include <ATOM_dbghlp.h>

#include "luascript.h"
#include "luafactory.h"
#include "scriptinterface.h"

static ATOM_LuaScriptFactory s_Factory;

void * myAlloc (void *ud, void *ptr, size_t osize, size_t nsize)
{
	ATOM_STACK_TRACE(myAlloc);

	if (nsize == 0)
	{
		ATOM_FREE(ptr);
		return 0;
	}
	else
	{
		return ptr ? ATOM_REALLOC(ptr, nsize) : ATOM_MALLOC(nsize);
	}
}

static void lua_print_error(lua_State *L, bool error, const char* fmt, ...)
{
	char text[4096];

	va_list args;
	va_start(args, fmt);
	vsprintf(text, fmt, args);
	va_end(args);

	lua_pushstring(L, "_ALERT");
	lua_gettable(L, LUA_GLOBALSINDEX);
	if(lua_isfunction(L, -1))
	{
		lua_pushstring(L, text);
		lua_call(L, 1, 0);
	}
	else
	{
		lua_pop(L, 1);

		bool output = false;
		lua_pushlightuserdata (L, L);
		lua_gettable(L, LUA_REGISTRYINDEX);
		if(lua_islightuserdata(L, -1))
		{
			ATOM_LuaScript *script = (ATOM_LuaScript*)lua_touserdata(L, -1);
			if (script)
			{
				ATOM_Script::OutputFunc func = script->getErrorOutputCallback();
				if (func)
				{
					func (text, error);
					output = true;
				}
			}
		}
		lua_pop(L, 1);

		if (!output)
		{
			if (error)
			{
				ATOM_LOGGER::error("%s", text);
			}
			else
			{
				ATOM_LOGGER::log("%s", text);
			}
		}
	}
}

static void lua_call_stack(lua_State* L, int n)
{
    lua_Debug ar;
	if(lua_getstack(L, n, &ar) == 1)
	{
		lua_getinfo(L, "nSlu", &ar);

		const char* indent = "\t";

		if(ar.name)
			lua_print_error(L, false, "%s%s() : line %d\n", indent, ar.name, ar.currentline);
		/*
		else
			lua_print_error(L, false, "%sunknown : line %d\n", indent, ar.currentline);
		*/

		lua_call_stack(L, n+1);
	}
}

static int lua_error_handler (lua_State *L)
{
	lua_print_error(L, true, "%s", lua_tostring(L, -1));

	lua_print_error(L, false, "================= callstack =================\n");
	lua_call_stack(L, 0);
	lua_print_error(L, false, "=============================================\n");

	return 0;	
}

ATOM_LuaScript::ATOM_LuaScript (void): ATOM_Script ("lua") 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::ATOM_LuaScript);

  _M_state = ATOM_LuaScriptFactory::openState ();
  //lua_setallocf (_M_state, &myAlloc, 0); 
  lua_atpanic (_M_state, &lua_error_handler);

  lua_pushlightuserdata (_M_state, _M_state);
  lua_pushlightuserdata (_M_state, this);
  lua_settable (_M_state, LUA_REGISTRYINDEX);
  _M_const_dirty = false;
  _isThread = false;
}

ATOM_LuaScript::ATOM_LuaScript (lua_State *master): ATOM_Script ("lua")
{
	_M_state = lua_newthread (master);
	//lua_setallocf (_M_state, &myAlloc, 0); 
	lua_atpanic (_M_state, &lua_error_handler);

	lua_pushthread (_M_state);
	_threadRef = luaL_ref (_M_state, LUA_REGISTRYINDEX);
	lua_pop (master, 1);

	lua_pushlightuserdata (_M_state, _M_state);
	lua_pushlightuserdata (_M_state, this);
	lua_settable (_M_state, LUA_REGISTRYINDEX);
	int top = lua_gettop (_M_state);
	ATOM_ASSERT(top == 0);

	// new global table for coroutine
	lua_newtable (_M_state);
	lua_createtable (_M_state, 0, 1);
	lua_pushvalue (_M_state, LUA_GLOBALSINDEX);
	lua_setfield (_M_state, -2, "__index");
	lua_setmetatable (_M_state, -2);
	lua_replace (_M_state, LUA_GLOBALSINDEX);
	top = lua_gettop (_M_state);
	ATOM_ASSERT(top == 0);

	_M_const_dirty = false;
	_isThread = true;
}

ATOM_LuaScript::~ATOM_LuaScript (void) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::~ATOM_LuaScript);

  if (_isThread)
  {
	  lua_pushlightuserdata (_M_state, _M_state);
	  lua_pushnil (_M_state);
	  lua_settable (_M_state, LUA_REGISTRYINDEX);

	  luaL_unref (_M_state, LUA_REGISTRYINDEX, _threadRef);
  }
  else
  {
	ATOM_LuaScriptFactory::closeState (_M_state);
  }
}

bool ATOM_LuaScript::setSource (const char *src, bool lib) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::setSource);

  if (!src)
  {
    return false;
  }

  if (_M_const_dirty)
  {
    registerUserConstants ();
  }

  _M_status = LOADERR;
  int top = lua_gettop (_M_state);

  lua_pushcclosure(_M_state, lua_error_handler, 0);
  int errfunc = lua_gettop (_M_state);

  if (luaL_loadstring(_M_state, src))
  {
	if (lua_isstring (_M_state, -1))
	{
		if (_M_output_func)
		{
			_M_output_func (lua_tostring (_M_state, -1), true);
		}
		else
		{
			ATOM_LOGGER::error (lua_tostring (_M_state, -1));
		}
	}
  }
  else if (!lib || !lua_pcall (_M_state, 0, LUA_MULTRET, errfunc))
  {
    _M_status = LOADED;
/*
    if (lib)
    {
		lua_getstate
      for (;;)
      {
        int err = lua_resume (_M_state, 0);
        if (err != LUA_YIELD)
        {
          _M_status = err ? LOADERR : LOADED;
          break;
        }
      }
    }
*/
  }

  lua_settop (_M_state, top);

  if (_M_status != LOADED)
  {
    return false;
  }

  _M_source = src;

  return true;
}

static ATOM_LuaScript::ReferencedPtr_t *getInstance (lua_State *L, int selfLoc) 
{
  ATOM_LuaScript::UserData_t *d = static_cast<ATOM_LuaScript::UserData_t*>(lua_touserdata (L, selfLoc));
  if (!d || !d->instance)
  {
    luaL_error (L, "Invalid user type\n");
  }
  return d->instance;
}

static ATOM_Script *_getLuaScriptObject (lua_State *L) 
{
  ATOM_STACK_TRACE(_getLuaScriptObject);

  lua_pushlightuserdata (L, L);
  lua_gettable (L, LUA_REGISTRYINDEX);
  ATOM_Script *scp = (ATOM_Script*)lua_touserdata (L, -1);
  lua_pop (L, 1);
  return scp;
}

static int _outputMemberlist (lua_State *L) 
{
  ATOM_STACK_TRACE(_outputMemberlist);

  ATOM_ClassStruct *p = static_cast<ATOM_ClassStruct*>(lua_touserdata(L, lua_upvalueindex(1)));
  p->help ();
  return 0;
}

static int _callmemberFunc (lua_State *L) 
{
  ATOM_STACK_TRACE(_callmemberFunc);

  ATOM_CommandCallerBase *cmd = static_cast<ATOM_CommandCallerBase*>(lua_touserdata(L, lua_upvalueindex(1)));
  if (!cmd)
  {
    luaL_error (L, "Internal error\n");
  }

  void *object = lua_touserdata(L, lua_upvalueindex(2));
  // make sure arguments begin at index 1.
  lua_remove (L, 1);
  ATOM_Script *scp = _getLuaScriptObject (L);
  int ret = 1;
  try
  {
    ret = cmd->invoke (object, scp, L);
  }
  catch (std::exception &e)
  {
    luaL_error (L, "%s\n", e.what());
  }
  catch (...)
  {
    luaL_error (L, "exception occured while executing command %s\n", cmd->_purename);
  }

  return ret;
}

static int _newindexFunc (lua_State *L) 
{
  ATOM_STACK_TRACE(_newindexFunc);

  ATOM_ClassStruct *type = static_cast<ATOM_ClassStruct*>(lua_touserdata(L, lua_upvalueindex(1)));
  if (!type || !type->commands)
  {
    luaL_error (L, "Internal error\n");
  }

  ATOM_LuaScript::ReferencedPtr_t *instance = getInstance (L, 1);
  // get script object
  ATOM_Script *scp = _getLuaScriptObject (L);

  int indexType = lua_type(L, -2);
  if (indexType == LUA_TNUMBER)
  {
    if (type->commands->newindexcommand)
    {
      lua_remove (L, 1);
      try
      {
        return type->commands->newindexcommand->command->invoke (instance->instance, scp, L);
      }
      catch (std::exception &e)
      {
        luaL_error (L, "%s\n", e.what());
      }
      catch (...)
      {
        luaL_error (L, "exception occured while executing command\n");
      }
    }
    else
    {
      luaL_error (L, "NewIndex function not exists.\n");
    }
  }
  else if (indexType == LUA_TSTRING)
  {
    const char *name = lua_tostring (L, -2);
    // make sure arguments begin at index 1.
    lua_remove (L, 1);
    lua_remove (L, 1);
    // find the attribute.
    void *object = instance->instance;
    for (ATOM_ClassStruct *p = type; p; p=p->parent)
    {
      for (unsigned i = 0; i < p->commands->commands.size(); ++i)
      {
        ATOM_CommandCallerBase *cmd = p->commands->commands[i]->command;
        if (!strcmp(cmd->_purename, name))
        {
          if (cmd->_isAttrib)
          {
            try
            {
              return ((ATOM_AttributeAccessorBase*)cmd)->invokeWrite (object, scp, L);
            }
            catch (std::exception &e)
            {
              luaL_error (L, "%s\n", e.what());
            }
            catch (...)
            {
              luaL_error (L, "exception occured while executing command %s\n", ((ATOM_AttributeAccessorBase*)cmd)->_purename);
            }
          }
        }
      }

      if (p->parent)
      {
        object = p->castfunc(object);
      }
    }
    luaL_error (L, "Method or Attribute not exists.\n");
    return 0;
  }
  else
  {
    luaL_error (L, "Invalidate new index function\n");
  }

  return 0;
}

static int _indexFunc (lua_State *L) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::_indexFunc);

  ATOM_ClassStruct *type = static_cast<ATOM_ClassStruct*>(lua_touserdata(L, lua_upvalueindex(1)));
  ATOM_LuaScript::ReferencedPtr_t *instance = getInstance (L, 1);
  // get script object
  ATOM_Script *scp = _getLuaScriptObject (L);
  int indexType = lua_type(L, -1);

  if (indexType == LUA_TNUMBER)
  {
    if (type->commands->indexcommand)
    {
      lua_remove (L, 1);
      try
      {
        return type->commands->indexcommand->command->invoke (instance->instance, scp, L);
      }
      catch (std::exception &e)
      {
        luaL_error (L, "%s\n", e.what());
      }
      catch (...)
      {
        luaL_error (L, "exception occured while executing command\n");
      }
    }
    else
    {
      luaL_error (L, "Index function not exists.\n");
    }
  }
  else if (indexType == LUA_TSTRING)
  {
    const char *name = lua_tostring (L, -1);
    if (!strcmp (name, "_help"))
    {
      lua_pushlightuserdata (L, type);
      lua_pushcclosure (L, _outputMemberlist, 1);
      return 1;
    }

    // find the attribute.
    void *object = instance->instance;
    for (ATOM_ClassStruct *p = type; p; p=p->parent)
    {
      for (unsigned i = 0; i < p->commands->commands.size(); ++i)
      {
        ATOM_CommandCallerBase *cmd = p->commands->commands[i]->command;
        if (!strcmp(cmd->_purename, name))
        {
          if (cmd->_isAttrib)
          {
            try
            {
              return cmd->invoke (object, scp, L);
            }
            catch (std::exception &e)
            {
              luaL_error (L, "%s\n", e.what());
            }
            catch (...)
            {
              luaL_error (L, "exception occured while executing command %s\n", cmd->_purename);
            }
          }
          else
          {
            lua_pushlightuserdata (L, cmd);
            lua_pushlightuserdata (L, object);
            lua_pushcclosure (L, _callmemberFunc, 2);
            return 1;
          }
        }
      }

      if (p->parent)
      {
        try
        {
          object = p->castfunc(object);
        }
        catch (std::exception &e)
        {
          luaL_error (L, "%s\n", e.what());
        }
        catch (...)
        {
          luaL_error (L, "exception occured while casting object\n");
        }
      }
    }

    luaL_error (L, "Method or Attribute not exists.\n");
  }
  else
  {
    luaL_error (L, "Invalidate index function\n");
  }

  return 0;
}

static int _namespaceIndexFunc (lua_State *L) 
{
  ATOM_STACK_TRACE(_namespaceIndexFunc);

  ATOM_LuaScript::ConstantMap *consts = static_cast<ATOM_LuaScript::ConstantMap*>(lua_touserdata(L, lua_upvalueindex(1)));
  const char *name = lua_tostring (L, -1);
  ATOM_LuaScript::ConstantMapIter it = consts->find (name);
  if (it != consts->end ())
  {
    try
    {
      return it->second->function->invoke (_getLuaScriptObject(L), L);
    }
    catch (std::exception &e)
    {
      luaL_error (L, "%s\n", e.what());
    }
    catch (...)
    {
      luaL_error (L, "exception occured while executing command %s\n", it->second->function->_purename);
    }
  }

  luaL_error (L, "Constant not exists.\n");
  return 0;
}

static int _indexFuncMethod (lua_State *L) {
  ATOM_STACK_TRACE(_indexFuncMethod);

  ATOM_ClassStruct *type = static_cast<ATOM_ClassStruct*>(lua_touserdata(L, lua_upvalueindex(1)));
  const char *name = lua_tostring (L, -1);
  ATOM_Script *scp = _getLuaScriptObject (L);
  for (ATOM_ClassStruct *p = type; p; p=p->parent)
  {
    // A constant?
    for (unsigned i = 0; i < p->commands->constants.size(); ++i)
    {
      ATOM_FunctionCallerBase *func = p->commands->constants[i]->function;
      if (!strcmp (func->_purename, name))
      {
        try
        {
          return func->invoke (scp, L);
        }
        catch (std::exception &e)
        {
          luaL_error (L, "%s\n", e.what());
        }
        catch (...)
        {
          luaL_error (L, "exception occured while executing command %s\n", func->_purename);
        }
      }
    }
  }

  luaL_error (L, "Static constant not exists.\n");
  return 1;
}

static int _getScriptFunc (lua_State *L)
{
	int top = lua_gettop(L);
	lua_pushlightuserdata (L, L);
	lua_gettable(L, LUA_REGISTRYINDEX);
	if(lua_islightuserdata(L, -1))
	{
		ATOM_Script *script = (ATOM_Script*)lua_touserdata(L, -1);
	}
	else
	{
		lua_pop (L, 1);
		lua_pushnil (L);
	}
	int top2 = lua_gettop(L);
	return 1;
}

static int _castFunc (lua_State *L) 
{
  ATOM_STACK_TRACE(_castFunc);

  ATOM_ClassStruct *type = static_cast<ATOM_ClassStruct*>(lua_touserdata(L, lua_upvalueindex(1)));
  int nargs = lua_gettop (L);
  if (nargs == 1)
  {
    if (lua_islightuserdata (L, 1) || lua_isuserdata(L, 1))
    {
      void *p = lua_touserdata (L, 1);
      if (p == 0)
      {
        lua_pushnil (L);
        return 1;
      }

      if (lua_islightuserdata (L, 1))
      {
        ATOM_LuaScript::UserData_t *ret = static_cast<ATOM_LuaScript::UserData_t*>(lua_newuserdata(L, sizeof(ATOM_LuaScript::UserData_t)));
        ATOM_LuaScript::ReferencedPtr_t *instance = ATOM_NEW(ATOM_LuaScript::ReferencedPtr_t);
        instance->refcount = 1;
        instance->instance = p;
        instance->creator = 0;
        instance->instance_op = 0;
        instance->deref = 0;
        ret->instance = instance;
      }
      else
      {
        ATOM_LuaScript::UserData_t *ud = (ATOM_LuaScript::UserData_t *)p;
        ATOM_LuaScript::UserData_t *ret = static_cast<ATOM_LuaScript::UserData_t*>(lua_newuserdata(L, sizeof(ATOM_LuaScript::UserData_t)));
        ret->instance = ud->instance;
        ret->instance->refcount++;
      }

      luaL_getmetatable (L, type->name);
      lua_setmetatable (L, -2);
    }
    else if (lua_isnil (L, 1))
    {
      lua_pushnil (L);
    }
    else
    {
      luaL_error(L, "Invalidate cast\n");
    }
  }
  else
  {
    luaL_error(L, "_Cast method must take one parameter\n");
  }

  return 1;
}

#if 1
static int _constructFunc (lua_State *L) 
{
  ATOM_STACK_TRACE(_constructFunc);

  ATOM_ClassStruct *type = static_cast<ATOM_ClassStruct*>(lua_touserdata(L, lua_upvalueindex(1)));
  lua_remove (L, 1);
  int numParams = lua_gettop(L);
  if (type->commands->constructfunction)
  {
    ATOM_Script *scp = _getLuaScriptObject (L);

    try
    {
      return type->commands->constructfunction->function->invoke (scp, L);
    }
    catch (std::exception &e)
    {
      luaL_error (L, "%s\n", e.what());
    }
    catch (...)
    {
      luaL_error (L, "exception occured while executing command %s\n", type->commands->constructfunction->function->_purename);
    }
  }
  else if (lua_gettop (L) > 0)
  {
    luaL_error (L, "No custom constructor avaliable\n");
  }
  else
  {
    void *instance = type->creator->create();
    if (!instance)
    {
      luaL_error (L, "Constructor execution failed\n");
    }

    ATOM_LuaScript::UserData_t *d = static_cast<ATOM_LuaScript::UserData_t*>(lua_newuserdata(L, sizeof(ATOM_LuaScript::UserData_t)));
    d->instance = ATOM_NEW(ATOM_LuaScript::ReferencedPtr_t);
    d->instance->refcount = 1;
    d->instance->instance = instance;
    d->instance->creator = type->creator;
    d->instance->instance_op = 0;
    d->instance->deref = 0;
    luaL_getmetatable (L, type->name);
    lua_setmetatable (L, -2);
  }

  return 1;
}
#else
static int _constructFunc (lua_State *L) 
{
  ATOM_ClassStruct *type = static_cast<ATOM_ClassStruct*>(lua_touserdata(L, lua_upvalueindex(1)));
  lua_remove (L, 1);
  int nargs = lua_gettop (L);
  if (nargs == 1)
  {
    if (lua_isuserdata (L, 1))
    {
      ATOM_LuaScript::ReferencedPtr_t *p = getInstance(L, -1);
      ATOM_LuaScript::UserData_t *ret = static_cast<ATOM_LuaScript::UserData_t*>(lua_newuserdata(L, sizeof(ATOM_LuaScript::UserData_t)));
      ret->instance = p;
      ret->instance->refcount++;
      luaL_getmetatable (L, type->name);
      lua_setmetatable (L, -2);
      return 1;
    }
    else if (lua_isnil (L, 1))
    {
      lua_pushnil (L);
      return 1;
    }
    else
    {
      luaL_error(L, "Invalidate cast\n");
    }
  }
  else if (nargs == 0)
  {
    void *p = type->creator->create();

    if (p)
    {
      ATOM_LuaScript::UserData_t *d = static_cast<ATOM_LuaScript::UserData_t*>(lua_newuserdata(L, sizeof(ATOM_LuaScript::UserData_t)));
      d->instance = ATOM_NEW(ATOM_LuaScript::ReferencedPtr_t);
      d->instance->refcount = 1;
      d->instance->instance = p;
      d->instance->creator = type->creator;
      d->instance->instance_op = 0;
      d->instance->deref = 0;
      luaL_getmetatable (L, type->name);
      lua_setmetatable (L, -2);
    }
    else
    {
      lua_pushnil (L);
    }
    return 1;
  }
  else
  {
    luaL_error (L, "Invalid constructor or cast operation\n");
  }

  return 1;
}
#endif

static int _gcFunc (lua_State *L) 
{
  ATOM_STACK_TRACE(_gcFunc);

  try
  {
    ATOM_LuaScript::UserData_t *d = static_cast<ATOM_LuaScript::UserData_t*>(lua_touserdata (L, 1));
    if (!--d->instance->refcount)
    {
      if (d->instance->instance_op)
      {
        d->instance->instance_op->destruct (d->instance->instance);
        ATOM_FREE (d->instance->instance);
        d->instance->instance_op->destroy ();
      }
      else if (d->instance->creator)
      {
        d->instance->creator->release(d->instance->instance);
      }
      else if (d->instance->deref)
      {
        d->instance->deref (d->instance->instance);
      }
      ATOM_DELETE(d->instance);
    }
  }
  catch (std::exception &e)
  {
    luaL_error (L, "%s\n", e.what());
  }
  catch (...)
  {
    luaL_error (L, "exception occured while doing garbage collection\n");
  }

  return 0;
}

static int _toString (lua_State *L) 
{
  ATOM_STACK_TRACE(_toString);

  ATOM_ClassStruct *type = static_cast<ATOM_ClassStruct*>(lua_touserdata(L, lua_upvalueindex(1)));
  ATOM_LuaScript::UserData_t *d = static_cast<ATOM_LuaScript::UserData_t*>(lua_touserdata (L, 1));
  if (type->commands->namecommand)
  {
    try
    {
      return type->commands->namecommand->command->invoke (d->instance->instance, _getLuaScriptObject(L), L);
    }
    catch (std::exception &e)
    {
      luaL_error (L, "%s\n", e.what());
    }
    catch (...)
    {
      luaL_error (L, "exception occured while executing command %s\n", type->commands->namecommand->command->_purename);
    }
  }
  else
  {
    char buff[32];
    sprintf (buff, "%p", d->instance->instance);
    lua_pushfstring (L, "%s (%s)", type->name, buff);
    return 1;
  }

  return 0;
}

static int _metafunc (lua_State *L) 
{
  ATOM_STACK_TRACE(_metafunc);

  ATOM_CommandStruct *cmd = static_cast<ATOM_CommandStruct*>(lua_touserdata(L, lua_upvalueindex(1)));
  ATOM_LuaScript::ReferencedPtr_t *instance = getInstance (L, 1);
  // make sure arguments begin at index 1.
  lua_remove (L, 1);
  ATOM_Script *scp = _getLuaScriptObject (L);
  try
  {
    return cmd->command->invoke (instance->instance, scp, L);
  }
  catch (std::exception &e)
  {
    luaL_error (L, "%s\n", e.what());
  }
  catch (...)
  {
    luaL_error (L, "exception occured while executing command %s\n", cmd->command->_purename);
  }

  return 0;
}

static int _callFunc (lua_State *L) 
{
  ATOM_STACK_TRACE(_callFunc);

  ATOM_FunctionCallerBase *func = static_cast<ATOM_FunctionCallerBase*>(lua_touserdata(L, lua_upvalueindex(1)));
  try
  {
    return func->invoke (_getLuaScriptObject(L), L); 
  }
  catch (std::exception &e)
  {
    luaL_error (L, "%s\n", e.what());
  }
  catch (...)
  {
    luaL_error (L, "exception occured while executing command %s\n", func->_purename);
  }

  return 0;
}

static bool checkNamespaceValid (const char *ns) 
{
  ATOM_STACK_TRACE(checkNamespaceValid);

  if (ns)
  {
    if (!*ns || ((*ns) >= '0' && (*ns) <= '9'))
    {
      return false;
    }
    while (*ns)
    {
      if ((*ns) != '_' && !isalpha(*ns) && ((*ns) < '0' || (*ns) > '9'))
      {
        return false;
      }
      ++ns;
    }
    return true;
  }
  return false;
}

static const char *parseNamespace (const char *namespaces, ATOM_VECTOR<ATOM_STRING> &nslist) 
{
  ATOM_STACK_TRACE(parseNamespace);

  nslist.clear();

  if (!namespaces)
  {
    return 0;
  }

  char *pos = const_cast<char*>(strrchr (namespaces, '.'));
  if (!pos)
  {
    return namespaces;
  }

  char *ret = pos+1;
  char *p = _strdup (namespaces);
  char *t = p;

  while (true)
  {
    pos = strchr (p, '.');
    if (!pos)
    {
      break;
    }

    *pos = '\0';
    nslist.push_back (p);
    p = pos+1;
  }

  free (t);

  for (unsigned i = 0; i < nslist.size(); ++i)
  {
    if (!checkNamespaceValid (nslist[i].c_str()))
    {
      nslist.clear();
      return 0;
    }
  }

  return ret;
}

static int createNamespaceTable (lua_State *L, const ATOM_VECTOR<ATOM_STRING> &nslist) 
{
  ATOM_STACK_TRACE(createNamespaceTable);

  int parentTableIndex = LUA_GLOBALSINDEX;

  int n = lua_gettop (L);
  for (unsigned i = 0; i < nslist.size(); ++i)
  {
    lua_pushstring (L, nslist[i].c_str());
    lua_rawget (L, parentTableIndex);
    if (lua_isnil (L, -1))
    {
      lua_pop (L, 1);
      lua_pushstring (L, nslist[i].c_str());
      lua_newtable (L);
      lua_rawset (L, parentTableIndex);
      lua_pushstring (L, nslist[i].c_str());
      lua_rawget (L, parentTableIndex);
    }
    if (parentTableIndex != LUA_GLOBALSINDEX)
    {
      lua_remove (L, -2);
    }
    parentTableIndex = lua_gettop (L);
  }

  return parentTableIndex;
}

void ATOM_LuaScript::registerUserFunction (ATOM_FunctionStruct *func) 
{
	ATOM_STACK_TRACE(ATOM_LuaScript::registerUserFunction);

  ATOM_VECTOR<ATOM_STRING> nslist;
  const char *funcname = parseNamespace (func->function->_name, nslist);
  if (funcname)
  {
    // Find or create the namespace table.
    int namespaceTable = createNamespaceTable (_M_state, nslist);

    lua_pushstring (_M_state, func->function->_purename);
    lua_pushlightuserdata (_M_state, func->function);
    lua_pushcclosure (_M_state, _callFunc, 1);
    lua_settable (_M_state, namespaceTable);

    if (namespaceTable != LUA_GLOBALSINDEX)
      lua_pop (_M_state, 1);
  }
}

void ATOM_LuaScript::registerUserConstant (ATOM_FunctionStruct *constant) 
{
	ATOM_STACK_TRACE(ATOM_LuaScript::registerUserConstant);

  ATOM_STRING constantName = constant->function->_name;
  ATOM_STRING ns;
  ATOM_STRING::size_type pos = constantName.find_last_of ('.');
  if (pos != ATOM_STRING::npos)
  {
    ns = constantName.substr(0, pos+1);
  }
  constantName = constantName.substr(pos+1);
  _M_constants[ns][constantName] = constant;
  _M_const_dirty = true;
}

void ATOM_LuaScript::registerUserConstants (void) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::registerUserConstants);

  static const char *globalScope = "N3SCRIPT_GLOBALSCOPE";

  if (_M_const_dirty)
  {
    _M_const_dirty = false;

    ATOM_VECTOR<ATOM_STRING> nslist;

    for (NamespaceMapConstIter it = _M_constants.begin(); it != _M_constants.end(); ++it)
    {
      const ATOM_STRING &ns = it->first;
      const ConstantMap &cm = it->second;
      const char *funcname = parseNamespace (ns.c_str(), nslist);
      int namespaceTable = createNamespaceTable (_M_state, nslist);

      ATOM_STRING metatableName = globalScope;
      if (namespaceTable != LUA_GLOBALSINDEX)
      {
        for (unsigned i = 0; i < nslist.size(); ++i)
        {
          metatableName += ":";
          metatableName += nslist[i];
        }
      }

      bool needset = luaL_newmetatable (_M_state, metatableName.c_str()) != 0;
      if (needset)
      {
        int namespaceMetatable = lua_gettop (_M_state);
        lua_pushliteral (_M_state, "__index");
        lua_pushlightuserdata (_M_state, (void*)&cm);
        lua_pushcclosure (_M_state, _namespaceIndexFunc, 1);
        lua_settable (_M_state, namespaceMetatable);

        lua_pushliteral (_M_state, "__metatable");
        lua_pushvalue (_M_state, namespaceTable);
        lua_settable (_M_state, namespaceMetatable); // metatable.__metatable = namespacetable
        lua_setmetatable (_M_state, namespaceTable);
      }

      if (namespaceTable != LUA_GLOBALSINDEX)
      {
        lua_pop (_M_state, 1);
      }
    }
  }
}

void ATOM_LuaScript::registerUserType (ATOM_ClassStruct *type) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::registerUserType);

  int top = lua_gettop (_M_state);
  ATOM_VECTOR<ATOM_STRING> nslist;
  const char *classname = parseNamespace (type->name, nslist);
  if (!classname)
  {
    return;
  }

  // Find or create the namespace table.
  int namespaceTable = createNamespaceTable (_M_state, nslist);

  // Create a new method table.
  lua_newtable (_M_state);

  // Stack index for the method table.
  int MethodsTable = lua_gettop (_M_state);

  // Register static functions and variables
  for (ATOM_ClassStruct *p = type; p; p=p->parent)
  {
    // Static functions
    for (unsigned i = 0; i < p->commands->staticcommands.size(); ++i)
    {
      ATOM_FunctionCallerBase *func = p->commands->staticcommands[i]->function;
      lua_pushstring (_M_state, func->_purename);
      lua_pushlightuserdata (_M_state, func);
      lua_pushcclosure (_M_state, _callFunc, 1);
      lua_settable (_M_state, MethodsTable);
    }
  }

  lua_pushliteral (_M_state, "_CastFrom");
  lua_pushlightuserdata (_M_state, type);
  lua_pushcclosure (_M_state, _castFunc, 1);
  lua_settable (_M_state, MethodsTable);

  lua_pushliteral (_M_state, "_Script");
  lua_pushcfunction (_M_state, _getScriptFunc);
  lua_settable (_M_state, LUA_GLOBALSINDEX);

  lua_pushliteral (_M_state, "_help");
  lua_pushlightuserdata (_M_state, type);
  lua_pushcclosure (_M_state, _outputMemberlist, 1);
  lua_settable (_M_state, MethodsTable);

  // Create a new metatable for the new type.
  luaL_newmetatable (_M_state, type->name);
  
  // Stack index for the meta table.
  int TypeMetatable = lua_gettop (_M_state);

  // Store the method table to the global table.
  lua_pushstring (_M_state, type->purename);
  lua_pushvalue (_M_state, MethodsTable);
  lua_settable (_M_state, namespaceTable/*LUA_GLOBALSINDEX*/);

  // Hide meta table
  lua_pushliteral (_M_state, "__metatable");
  lua_pushvalue (_M_state, MethodsTable);
  lua_settable (_M_state, TypeMetatable); // metatable.__metatable = methods

  // Set type struct to metatable
  lua_pushliteral (_M_state, "classstruct");
  lua_pushlightuserdata (_M_state, type);
  lua_settable (_M_state, TypeMetatable);

  // Read access checks the methods table
  lua_pushliteral (_M_state, "__index");
  lua_pushlightuserdata (_M_state, type);
  lua_pushcclosure (_M_state, _indexFunc, 1);
  lua_settable (_M_state, TypeMetatable);

  lua_pushliteral (_M_state, "__newindex");
  lua_pushlightuserdata (_M_state, type);
  lua_pushcclosure (_M_state, _newindexFunc, 1);
  lua_settable (_M_state, TypeMetatable);

  // Custom function to display type info.
  lua_pushliteral (_M_state, "__tostring");
  lua_pushlightuserdata (_M_state, type);
  lua_pushcclosure (_M_state, _toString, 1);
  lua_settable (_M_state, TypeMetatable);

  // Custom function to add type
  if (type->commands->addfunction)
  {
    lua_pushliteral (_M_state, "__add");
    lua_pushlightuserdata (_M_state, type->commands->addfunction);
    lua_pushcclosure (_M_state, _metafunc, 1);
    lua_settable (_M_state, TypeMetatable);
  }

  // Custom function to subtract type
  if (type->commands->subfunction)
  {
    lua_pushliteral (_M_state, "__sub");
    lua_pushlightuserdata (_M_state, type->commands->subfunction);
    lua_pushcclosure (_M_state, _metafunc, 1);
    lua_settable (_M_state, TypeMetatable);
  }

  // Custom function to multiply type
  if (type->commands->mulfunction)
  {
    lua_pushliteral (_M_state, "__mul");
    lua_pushlightuserdata (_M_state, type->commands->mulfunction);
    lua_pushcclosure (_M_state, _metafunc, 1);
    lua_settable (_M_state, TypeMetatable);
  }

  // Custom function to divide type
  if (type->commands->divfunction)
  {
    lua_pushliteral (_M_state, "__div");
    lua_pushlightuserdata (_M_state, type->commands->divfunction);
    lua_pushcclosure (_M_state, _metafunc, 1);
    lua_settable (_M_state, TypeMetatable);
  }

  // Custom function to negate type
  if (type->commands->negfunction)
  {
    lua_pushliteral (_M_state, "__unm");
    lua_pushlightuserdata (_M_state, type->commands->negfunction);
    lua_pushcclosure (_M_state, _metafunc, 1);
    lua_settable (_M_state, TypeMetatable);
  }

  // Custom function to concat type
  if (type->commands->concatfunction)
  {
    lua_pushliteral (_M_state, "__concat");
    lua_pushlightuserdata (_M_state, type->commands->concatfunction);
    lua_pushcclosure (_M_state, _metafunc, 1);
    lua_settable (_M_state, TypeMetatable);
  }

  // Custom function to len
  if (type->commands->lenfunction)
  {
    lua_pushliteral (_M_state, "__len");
    lua_pushlightuserdata (_M_state, type->commands->lenfunction);
    lua_pushcclosure (_M_state, _metafunc, 1);
    lua_settable (_M_state, TypeMetatable);
  }

  // Custom function to compare type
  if (type->commands->eqfunction)
  {
    lua_pushliteral (_M_state, "__eq");
    lua_pushlightuserdata (_M_state, type->commands->eqfunction);
    lua_pushcclosure (_M_state, _metafunc, 1);
    lua_settable (_M_state, TypeMetatable);
  }

  // Custom function to compare type
  if (type->commands->ltfunction)
  {
    lua_pushliteral (_M_state, "__lt");
    lua_pushlightuserdata (_M_state, type->commands->ltfunction);
    lua_pushcclosure (_M_state, _metafunc, 1);
    lua_settable (_M_state, TypeMetatable);
  }

  // Custom function to call type
  if (type->commands->callfunction)
  {
    lua_pushliteral (_M_state, "__call");
    lua_pushlightuserdata (_M_state, type->commands->callfunction);
    lua_pushcclosure (_M_state, _metafunc, 1);
    lua_settable (_M_state, TypeMetatable);
  }

  // Garbage collection function
  lua_pushliteral (_M_state, "__gc");
  lua_pushcfunction (_M_state, _gcFunc);
  lua_settable (_M_state, TypeMetatable);

  // Need a metatable for the methods table to allow class instantiation
  lua_newtable (_M_state);
  int MethodsMetatable = lua_gettop (_M_state);

  lua_pushliteral (_M_state, "__index");
  lua_pushlightuserdata (_M_state, type);
  lua_pushcclosure (_M_state, _indexFuncMethod, 1);
  lua_settable (_M_state, MethodsMetatable);

  // "Calling" the methods table should call the constructor.
  // We will as well provide the constructor method as "new".
  lua_pushliteral (_M_state, "__call");
  lua_pushlightuserdata (_M_state, type);
  lua_pushcclosure (_M_state, _constructFunc, 1);
  lua_settable (_M_state, MethodsMetatable); // Type()

  // Methods metatable now on top of the stack.
  lua_setmetatable (_M_state, MethodsTable);
  lua_pop (_M_state, namespaceTable == LUA_GLOBALSINDEX ? 2 : 3);
  top = lua_gettop (_M_state);
}

bool ATOM_LuaScript::executeString (const char *str) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::executeString);

  if (_M_const_dirty)
  {
    registerUserConstants ();
  }

  bool ret = true;
  int top = lua_gettop (_M_state);

  lua_pushcclosure(_M_state, lua_error_handler, 0);
  int errfunc = lua_gettop (_M_state);

  if (luaL_loadstring(_M_state, str))
  {
	if (lua_isstring (_M_state, -1))
	{
		if (_M_output_func)
		{
			_M_output_func (lua_tostring (_M_state, -1), true);
		}
		else
		{
			ATOM_LOGGER::error (lua_tostring (_M_state, -1));
		}
	}
	ret = false;
  }
  else if (lua_pcall (_M_state, 0, LUA_MULTRET, errfunc))
  {
    ret = false;
  }

  lua_settop (_M_state, top);

  return ret;
}

bool ATOM_LuaScript::isFunction (const char *name)
{
	bool ret = false;

	if (name)
	{
		lua_getglobal (_M_state, name);
		ret = lua_isfunction (_M_state, -1);
		lua_pop (_M_state, 1);
	}

	return ret;
}

void ATOM_LuaScript::execute (void) 
{
	if (!_M_state)
	{
		return;
	}

	if (_M_const_dirty)
	{
		registerUserConstants ();
	}

	if (_M_function_name.empty ())
	{
		if (_M_status != LOADED)
		{
			return;
		}
		int k = lua_resume(_M_state, 0);
		_M_status = k == LUA_YIELD ? YIELD : FINISHED;
	}
	else
	{
		int top = lua_gettop (_M_state);

		lua_pushcclosure(_M_state, lua_error_handler, 0);
		int errfunc = lua_gettop (_M_state);

		lua_getglobal (_M_state, _M_function_name.c_str());
		if (lua_isfunction (_M_state, -1))
		{
			for (unsigned i = 0; i < _M_parameters.size(); ++i)
			{
				pushParameter (_M_parameters[i]);
			}

			if (lua_pcall (_M_state, _M_parameters.size(), _M_results.size(), errfunc))
			{
				for (unsigned i = 0; i < _M_results.size(); ++i)
				{
					_M_results[i].reset();
				}
			}
			else
			{
				for (unsigned i = 0; i < _M_results.size(); ++i)
				{
					getResult (_M_results[_M_results.size() - 1 - i]);
					lua_pop (_M_state, 1);
				}
			}
		}

		lua_settop (_M_state, top);
	}
}

void ATOM_LuaScript::pushParameter (const ATOM_ScriptVariant &var) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::pushParameter);

  if (!_M_state)
  {
    return;
  }

  switch (var.getType())
  {
  case ATOM_ScriptVariant::TYPE_INT:
    lua_pushnumber (_M_state, var.asInteger());
    break;
  case ATOM_ScriptVariant::TYPE_FLOAT:
    lua_pushnumber (_M_state, var.asFloat());
    break;
  case ATOM_ScriptVariant::TYPE_BOOL:
    lua_pushboolean (_M_state, var.asBool() ? 1 : 0);
    break;
  case ATOM_ScriptVariant::TYPE_STRING:
    lua_pushstring (_M_state, var.asString());
    break;
  case ATOM_ScriptVariant::TYPE_POINTER:
    lua_pushlightuserdata (_M_state, var.asPointer());
    break;
  default:
    break;
  }
}

void ATOM_LuaScript::getResult (ATOM_ScriptVariant &var) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::getResult);

  if (!_M_state)
  {
    var.reset();
    return;
  }

  //printf (lua_typename(_M_state, lua_type(_M_state, -1)));

  if (lua_isnumber (_M_state, -1))
  {
    var = float(lua_tonumber (_M_state, -1));
  }
  else if (lua_isboolean (_M_state, -1))
  {
    var = lua_toboolean (_M_state, -1) != 0;
  }
  else if (lua_isstring (_M_state, -1))
  {
    var = lua_tostring (_M_state, -1);
  }
  else if (lua_islightuserdata (_M_state, -1))
  {
    var = lua_touserdata (_M_state, -1);
  }
  else if (lua_isuserdata (_M_state, -1))
  {
    ATOM_LuaScript::UserData_t *d = static_cast<ATOM_LuaScript::UserData_t*>(lua_touserdata (_M_state, 1));
    var = d->instance->instance;
  }
  else
  {
    luaL_error (_M_state, "Unsupported return type\n");
  }
}

int ATOM_LuaScript::yeild (void)
{
	return lua_yield (_M_state, 0);
}

bool ATOM_LuaScript::resume (void) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::resume);

  if (_M_status != LOADED && _M_status != YIELD)
  {
    return false;
  }

  if (lua_resume(_M_state, 0) != LUA_YIELD)
  {
    _M_status = FINISHED;
    return true;
  }

  _M_status = YIELD;
  return false;
}

int ATOM_LuaScript::getStatus (void) const 
{
  return _M_status;
}

int ATOM_LuaScript::getParameterType (int index) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::getParameterType);

  int numParams = lua_gettop(_M_state);
  if (index >= numParams)
  {
    return TYPE_UNKNOWN;
  }

  int type = lua_type (_M_state, index+1);
  switch (type)
  {
  case LUA_TNUMBER:
    return TYPE_NUMBER;
  case LUA_TBOOLEAN:
    return TYPE_BOOL;
  case LUA_TSTRING:
    return TYPE_STRING;
  case LUA_TUSERDATA:
  case LUA_TLIGHTUSERDATA:
    return TYPE_POINTER;
  case LUA_TNIL:
    return TYPE_NIL;
  default:
    return TYPE_UNKNOWN;
  }
}

int ATOM_LuaScript::getParameteri (int index) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::getParameteri);

  int numParams = lua_gettop(_M_state);
  if (index >= numParams)
  {
    return 0;
  }

  int type = lua_type (_M_state, index+1);
  switch (type)
  {
  case LUA_TNUMBER:
    return static_cast<int>(lua_tonumber (_M_state, index+1));
  case LUA_TBOOLEAN:
    return lua_toboolean (_M_state, index+1) != 0 ? 1 : 0;
  default:
    luaL_error (_M_state, "Couldn't convert parameter to integer\n");
    return 0;
  }
}

unsigned int ATOM_LuaScript::getParameterui (int index) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::getParameterui);

  int numParams = lua_gettop(_M_state);
  if (index >= numParams)
  {
    return 0;
  }

  int type = lua_type (_M_state, index+1);
  switch (type)
  {
  case LUA_TNUMBER:
    return static_cast<unsigned>(lua_tonumber (_M_state, index+1));
  case LUA_TBOOLEAN:
    return lua_toboolean (_M_state, index+1) != 0 ? 1 : 0;
  default:
    luaL_error (_M_state, "Couldn't convert parameter to unsigned integer\n");
    return 0;
  }
}

float ATOM_LuaScript::getParameterf (int index) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::getParameterf);

  int numParams = lua_gettop(_M_state);
  if (index >= numParams)
  {
    return 0.f;
  }

  int type = lua_type (_M_state, index+1);
  switch (type)
  {
  case LUA_TNUMBER:
    return static_cast<float>(lua_tonumber (_M_state, index+1));
  case LUA_TBOOLEAN:
    return lua_toboolean (_M_state, index+1) != 0 ? 1 : 0;
  default:
    luaL_error (_M_state, "Couldn't convert parameter to float\n");
    return 0;
  }
}

bool ATOM_LuaScript::getParameterb (int index) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::getParameterb);

  int numParams = lua_gettop(_M_state);
  if (index >= numParams)
  {
    return false;
  }

  int type = lua_type (_M_state, index+1);
  switch (type)
  {
  case LUA_TNUMBER:
    return lua_tonumber (_M_state, index+1) != 0;
  case LUA_TBOOLEAN:
    return lua_toboolean (_M_state, index+1) != 0;
  default:
    luaL_error (_M_state, "Couldn't convert parameter to boolean\n");
    return false;
  }
}

const char *ATOM_LuaScript::getParameters (int index) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::getParameters);

  int numParams = lua_gettop(_M_state);
  if (index >= numParams)
  {
    return 0;
  }

  int type = lua_type (_M_state, index+1);
  switch (type)
  {
  case LUA_TSTRING:
    return lua_tostring (_M_state, index+1);
  case LUA_TLIGHTUSERDATA:
    return reinterpret_cast<const char*>(lua_touserdata(_M_state, index+1));
  case LUA_TNIL:
    return 0;
  default:
    luaL_error (_M_state, "Couldn't convert parameter to string\n");
    return 0;
  }
}

void *ATOM_LuaScript::getParameterp (int index) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::getParameterp);

  int numParams = lua_gettop(_M_state);
  if (index >= numParams)
  {
    return 0;
  }

  int type = lua_type (_M_state, index+1);
  switch (type)
  {
  case LUA_TUSERDATA:
    return getParameterc (index);
  case LUA_TLIGHTUSERDATA:
    return lua_touserdata (_M_state, index+1);
  case LUA_TSTRING:
    return const_cast<char*>(lua_tostring (_M_state, index+1));
  case LUA_TNIL:
    return 0;
  default:
    luaL_error (_M_state, "Couldn't convert parameter to pointer\n");
    return 0;
  }
}

void *ATOM_LuaScript::getParameterc (int index) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::getParameterc);

  int numParams = lua_gettop(_M_state);
  if (index >= numParams)
  {
    return 0;
  }

  int type = lua_type (_M_state, index+1);
  switch (type)
  {
  case LUA_TUSERDATA:
    {
      ATOM_LuaScript::UserData_t *d = static_cast<ATOM_LuaScript::UserData_t*>(lua_touserdata (_M_state, index+1));
      //ATOM_LuaScript::ReferencedPtr_t *instance = (ATOM_LuaScript::ReferencedPtr_t*)((unsigned)(d->instance) & ~1);
      ATOM_LuaScript::ReferencedPtr_t *instance = d->instance;
      return instance->instance;
    }
  case LUA_TLIGHTUSERDATA:
	{
	  return lua_touserdata (_M_state, index+1);
	  break;
	}
  case LUA_TNIL:
    return 0;
  default:
    luaL_error (_M_state, "Couldn't get typed object pointer\n");
    return 0;
  }
}

static ATOM_ScriptVariant _getParamv (lua_State *L, int index)
{
  ATOM_STACK_TRACE(ATOM_LuaScript::_getParamv);

  int type = lua_type (L, index);
  switch (type)
  {
  case LUA_TUSERDATA:
    {
      ATOM_LuaScript::UserData_t *d = static_cast<ATOM_LuaScript::UserData_t*>(lua_touserdata (L, index));
      ATOM_LuaScript::ReferencedPtr_t *instance = d->instance;
      return instance->instance;
    }
  case LUA_TLIGHTUSERDATA:
    {
      return lua_touserdata (L, index);
    }
  case LUA_TNIL:
    {
      return ATOM_ScriptVariant ((void*)0);
    }
  case LUA_TBOOLEAN:
    {
      return lua_toboolean (L, index) != 0;
    }
  case LUA_TNUMBER:
    {
      return float(lua_tonumber (L, index));
    }
  case LUA_TSTRING:
    {
      return lua_tostring (L, index);
    }
  case LUA_TTABLE:
    {
      ATOM_VECTOR<ATOM_ScriptVariant> v;
      int i = 0;
      while (true)
      {
        lua_pushnumber (L, ++i);
        lua_gettable (L, -2);
        int t = lua_type(L, -1);
        if (t == LUA_TNIL)
        {
          lua_pop (L, 1);
          break;
        }
        else
        {
          ATOM_ScriptVariant var = _getParamv (L, -1);
          lua_pop (L, 1);
          v.push_back (var);
        }
      }
      return v.size() ? ATOM_ScriptVariant(&v[0], v.size()) : ATOM_ScriptVariant(0, 0);
    }
  default:
    {
      return ATOM_ScriptVariant();
    }
  }
}

ATOM_ScriptVariant ATOM_LuaScript::getParameterv (int index) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::getParameterv);

  int numParams = lua_gettop(_M_state);
  if (index >= numParams)
  {
    return ATOM_ScriptVariant();
  }

  return _getParamv (_M_state, index+1);
  /*
  int type = lua_type (_M_state, index+1);
  switch (type)
  {
  case LUA_TUSERDATA:
    {
      ATOM_LuaScript::UserData_t *d = static_cast<ATOM_LuaScript::UserData_t*>(lua_touserdata (_M_state, index+1));
      //ATOM_LuaScript::ReferencedPtr_t *instance = (ATOM_LuaScript::ReferencedPtr_t*)((unsigned)(d->instance) & ~1);
      ATOM_LuaScript::ReferencedPtr_t *instance = d->instance;
      return instance->instance;
    }
  case LUA_TLIGHTUSERDATA:
    {
      return lua_touserdata (_M_state, index+1);
    }
  case LUA_TNIL:
    {
      return ATOM_ScriptVariant ((void*)0);
    }
  case LUA_TBOOLEAN:
    {
      return lua_toboolean (_M_state, index+1) != 0;
    }
  case LUA_TNUMBER:
    {
      return float(lua_tonumber (_M_state, index+1));
    }
  case LUA_TSTRING:
    {
      return lua_tostring (_M_state, index+1);
    }
  case LUA_TABLE:
    {
      ATOM_VECTOR<ATOM_ScriptVariant> v;
      int i = 1;
      while (true)
      {
        lua_pushnumber (_M_state, i);
        lua_gettable (_M_state, -1);
        int t = lua_type(_M_state, -1);
        if (t == LUA_TNIL)
          break;
      }
    }
  default:
    {
      return ATOM_ScriptVariant();
    }
  }
  */
}

int ATOM_LuaScript::setReturni (int ret) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::setReturni);

  lua_pushnumber (_M_state, ret);
  return 1;
}

int ATOM_LuaScript::setReturnui (unsigned int ret) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::setReturnui);

  lua_pushnumber (_M_state, ret);
  return 1;
}

int ATOM_LuaScript::setReturnf (float ret) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::setReturnf);

  lua_pushnumber (_M_state, ret);
  return 1;
}

int ATOM_LuaScript::setReturnb (bool ret) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::setReturnb);

  lua_pushboolean (_M_state, ret ? 1 : 0);
  return 1;
}

int ATOM_LuaScript::setReturns (const char *ret) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::setReturns);

  if (ret)
    lua_pushstring (_M_state, ret);
  else
    lua_pushnil (_M_state);

  return 1;
}

static int _gcFunc1 (lua_State *L) 
{
  ATOM_STACK_TRACE(_gcFunc1);

  try
  {
    ATOM_LuaScript::UserData_t *d = static_cast<ATOM_LuaScript::UserData_t*>(lua_touserdata (L, 1));
    if (!--d->instance->refcount)
    {
      d->instance->deref (d->instance->instance);
      ATOM_DELETE(d->instance);
    }
  }
  catch (std::exception &e)
  {
    luaL_error (L, "%s\n", e.what());
  }
  catch (...)
  {
    luaL_error (L, "exception occured while executing garbage collection function\n");
  }

  return 0;
}

int ATOM_LuaScript::setReturnp (void *ret, ATOM_DereferenceFunc deref) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::setReturnp);

  if (ret)
  {
    if (deref)
    {
      ATOM_LuaScript::UserData_t *d = static_cast<ATOM_LuaScript::UserData_t*>(lua_newuserdata(_M_state, sizeof(ATOM_LuaScript::UserData_t)));
      d->instance = ATOM_NEW(ATOM_LuaScript::ReferencedPtr_t);
      d->instance->refcount = 1;
      d->instance->instance = ret;
      d->instance->creator = 0;
      d->instance->instance_op = 0;
      d->instance->deref = deref;
      char buffer[12];
      sprintf (buffer, "_t%p", d);
      luaL_newmetatable (_M_state, buffer);
      int TypeMetatable = lua_gettop (_M_state);
      lua_pushliteral (_M_state, "__gc");
      lua_pushcfunction (_M_state, _gcFunc1);
      lua_settable (_M_state, TypeMetatable);
      lua_setmetatable (_M_state, -2);
    }
    else
    {
      lua_pushlightuserdata (_M_state, const_cast<void*>(ret));
    }
  }
  else
  {
    lua_pushnil (_M_state);
  }

  return 1;
}

int ATOM_LuaScript::setReturnc (void *ret, const char *classname, ATOM_DereferenceFunc deref) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::setReturnc);

  if (ret)
  {
    ATOM_LuaScript::UserData_t *d = static_cast<ATOM_LuaScript::UserData_t*>(lua_newuserdata(_M_state, sizeof(ATOM_LuaScript::UserData_t)));
    d->instance = ATOM_NEW(ATOM_LuaScript::ReferencedPtr_t);
    d->instance->refcount = 1;
    d->instance->instance = const_cast<void*>(ret);
    d->instance->creator = 0;
    d->instance->instance_op = 0;
    d->instance->deref = deref;
    luaL_getmetatable(_M_state, classname);
    lua_setmetatable(_M_state, -2);
  }
  else
  {
    lua_pushnil (_M_state);
  }
  return 1;
}

static int _gcFunc2 (lua_State *L) 
{
  ATOM_STACK_TRACE(_gcFunc2);

  try
  {
    ATOM_LuaScript::UserData_t *d = static_cast<ATOM_LuaScript::UserData_t*>(lua_touserdata (L, 1));
    if (!--d->instance->refcount)
    {
      d->instance->instance_op->destruct (d->instance->instance);
      ATOM_FREE (d->instance->instance);
      d->instance->instance_op->destroy ();
      ATOM_DELETE(d->instance);
    }
  }
  catch (std::exception &e)
  {
    luaL_error (L, "%s\n", e.what());
  }
  catch (...)
  {
    luaL_error (L, "exception occured while executing garbage collection function\n");
  }

  return 0;
}

int ATOM_LuaScript::setReturnpi (void *ret, ATOM_InstanceOp *op) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::setReturnpi);

  lua_State *L = _M_state;
  ATOM_LuaScript::UserData_t *d = static_cast<ATOM_LuaScript::UserData_t*>(lua_newuserdata(L, sizeof(ATOM_LuaScript::UserData_t)));
  d->instance = ATOM_NEW(ATOM_LuaScript::ReferencedPtr_t);
  d->instance->refcount = 1;
  d->instance->instance = ATOM_MALLOC (op->getSize ());
  op->copy_construct (d->instance->instance, ret);
  d->instance->instance_op = op->clone ();
  d->instance->creator = 0;
  d->instance->deref = 0;
  char buffer[12];
  sprintf (buffer, "_t%p", d);
  luaL_newmetatable (L, buffer);
  int TypeMetatable = lua_gettop (L);
  lua_pushliteral (L, "__gc");
  lua_pushcfunction (L, _gcFunc2);
  lua_settable (L, TypeMetatable);
  lua_setmetatable (L, -2);
  return 1;
}

int ATOM_LuaScript::setReturnci (void *ret, const char *classname, ATOM_InstanceOp *op) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::setReturnci);

  ATOM_LuaScript::UserData_t *d = static_cast<ATOM_LuaScript::UserData_t*>(lua_newuserdata(_M_state, sizeof(ATOM_LuaScript::UserData_t)));
  d->instance = ATOM_NEW(ATOM_LuaScript::ReferencedPtr_t);
  d->instance->refcount = 1;
  d->instance->creator = 0;
  d->instance->instance = ATOM_MALLOC (op->getSize ());
  op->copy_construct (d->instance->instance, ret);
  d->instance->instance_op = op->clone();
  d->instance->deref = 0;
  luaL_getmetatable(_M_state, classname);
  lua_setmetatable(_M_state, -2);
  return 1;
}

void _setReturnArray (lua_State *L, const ATOM_ScriptVariant &var) 
{
}

int ATOM_LuaScript::setReturnv (const ATOM_ScriptVariant &var, ATOM_DereferenceFunc deref) 
{
  ATOM_STACK_TRACE(ATOM_LuaScript::setReturnv);

  switch (var.getType())
  {
  case ATOM_ScriptVariant::TYPE_NONE:
    return 0;
    break;
  case ATOM_ScriptVariant::TYPE_BOOL:
    lua_pushboolean (_M_state, var.asBool() ? 1 : 0);
    break;
  case ATOM_ScriptVariant::TYPE_FLOAT:
    lua_pushnumber (_M_state, var.asFloat());
    break;
  case ATOM_ScriptVariant::TYPE_INT:
    lua_pushnumber (_M_state, var.asInteger());
    break;
  case ATOM_ScriptVariant::TYPE_POINTER:
    {
      void *p = var.asPointer();
      if (p)
      {
        return setReturnp (p, deref);
      }
      else
      {
        lua_pushnil (_M_state);
      }
      break;
    }
  case ATOM_ScriptVariant::TYPE_STRING:
    lua_pushstring (_M_state, var.asString());
    break;
  case ATOM_ScriptVariant::TYPE_ARRAY:
    {
      lua_newtable (_M_state);
      lua_pushnumber (_M_state, 0);
      lua_pushnumber (_M_state, var.getArraySize());
      lua_settable (_M_state, -3);

      for (int i = 0; i < var.getArraySize(); ++i)
      {
        lua_pushnumber (_M_state, i+1);

        const ATOM_ScriptVariant &v = var.asArray()[i];
        if (var.getType() == ATOM_ScriptVariant::TYPE_NONE)
        {
          lua_pushnil (_M_state);
        }
        else
        {
          setReturnv (v, deref);
        }

        lua_settable (_M_state, -3);
      }
      break;
    }
  default:
    lua_pushnil (_M_state);
    break;
  }
  return 1;
}


ATOM_Script *ATOM_LuaScript::clone (void) const
{
	return ATOM_NEW(ATOM_LuaScript, _M_state);
}

void ATOM_LuaScript::setGlobaliImp (const char *name, int value)
{
	lua_pushnumber (_M_state, value);
	lua_setglobal (_M_state, name);
}

void ATOM_LuaScript::setGlobalfImp (const char *name, float value)
{
	lua_pushnumber (_M_state, value);
	lua_setglobal (_M_state, name);
}

void ATOM_LuaScript::setGlobalsImp (const char *name, const char *value)
{
	lua_pushstring (_M_state, value);
	lua_setglobal (_M_state, name);
}

void ATOM_LuaScript::setGlobalpImp (const char *name, void *value)
{
	lua_pushlightuserdata (_M_state, value);
	lua_setglobal (_M_state, name);
}
