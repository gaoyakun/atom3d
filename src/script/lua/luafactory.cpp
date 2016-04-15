#include "basedefs.h"

#if defined(WIN32)
# include <windows.h>
#endif

#include <ATOM_dbghlp.h>

#include "luafactory.h"
#include "luascript.h"
#include "luaautoblock.h"
#include "funcbind.h"
#include "scriptinterface.h"

ATOM_LuaScriptFactory::ATOM_LuaScriptFactory (void) 
{
}

ATOM_LuaScriptFactory::~ATOM_LuaScriptFactory (void) 
{
}

ATOM_Script *ATOM_LuaScriptFactory::_createScriptImpl (void) 
{
	ATOM_LuaScript *scp = ATOM_NEW(ATOM_LuaScript);
	return scp;
}

void ATOM_LuaScriptFactory::_deleteScriptImpl (ATOM_Script *scp) 
{
	ATOM_DELETE(scp);
}

bool ATOM_LuaScriptFactory::registerUserDataType (ATOM_Script *scp, ATOM_ClassStruct *ud) 
{
	if (scp && ud)
	{
		ATOM_LuaScript *script = static_cast<ATOM_LuaScript*>(scp);
		script->registerUserType (ud);
		return true;
	}
	return false;
}

bool ATOM_LuaScriptFactory::registerUserFunction (ATOM_Script *scp, ATOM_FunctionStruct *uf) 
{
	if (scp && uf)
	{
		ATOM_LuaScript *script = static_cast<ATOM_LuaScript*>(scp);
		script->registerUserFunction (uf);
		return true;
	}
	return false;
}

bool ATOM_LuaScriptFactory::registerUserConstant (ATOM_Script *scp, ATOM_FunctionStruct *uc) 
{
	if (scp && uc)
	{
		ATOM_LuaScript *script = static_cast<ATOM_LuaScript*>(scp);
		script->registerUserConstant (uc);
		return true;
	}
	return false;
}

static int lua_fatalerror (lua_State *L) 
{
	const char *err = lua_tostring (L, 1);
#if defined(WIN32)
	OutputDebugString (err);
#else // !WIN32
	puts (err);
#endif // WIN32
	return -1;
}

static void *myAlloc (void *ud, void *ptr, size_t osize, size_t nsize) 
{
	(void)ud;
	(void)osize;

	if (nsize == 0)
	{
		free (ptr);
		//ATOM_FREE(ptr);
		return 0;
	}
	else
	{
		return ptr ? realloc (ptr, nsize) : malloc(nsize);
		//return ATOM_REALLOC(ptr, nsize);
	}
}

lua_State *ATOM_LuaScriptFactory::openState (void) 
{
	lua_State *L = lua_open ();

	if (L)
	{
		//ATOM_LuaAutoBlock autoblock (L);
		luaL_openlibs (L);
		lua_atpanic (L, lua_fatalerror);
	}

	return L;
}

void ATOM_LuaScriptFactory::closeState (lua_State *L) 
{
	lua_close (L);
}
