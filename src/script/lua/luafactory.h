#ifndef __ATOM_SCRIPT_LUAFACTORY_H
#define __ATOM_SCRIPT_LUAFACTORY_H

#include <lua.hpp>
#include "scriptfactory.h"

class ATOM_LuaScriptFactory: public ATOM_ScriptFactory
{
public:
	ATOM_LuaScriptFactory (void);
	virtual ~ATOM_LuaScriptFactory (void);

protected:
	virtual ATOM_Script *_createScriptImpl (void);
	virtual void _deleteScriptImpl (ATOM_Script *scp);

public:
	virtual bool registerUserDataType (ATOM_Script *scp, ATOM_ClassStruct *ud);
	virtual bool registerUserFunction (ATOM_Script *scp, ATOM_FunctionStruct *uf);
	virtual bool registerUserConstant (ATOM_Script *scp, ATOM_FunctionStruct *uc);

public:
	static lua_State *openState (void);
	static void closeState (lua_State *L);
};

#endif // __ATOM_SCRIPT_LUAFACTORY_H
