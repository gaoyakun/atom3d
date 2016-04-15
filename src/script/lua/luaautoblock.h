#ifndef __ATOM_SCRIPT_LUAAUTOBLOCK_H
#define __ATOM_SCRIPT_LUAAUTOBLOCK_H

extern"C" {
#include "lua.h"
}

class ATOM_LuaAutoBlock
{
public:
	ATOM_LuaAutoBlock (lua_State *L): _M_state(L), _M_top(lua_gettop(L)) 
	{
	}

	~ATOM_LuaAutoBlock () 
	{
		lua_settop (_M_state, _M_top);
	}
private:
	ATOM_LuaAutoBlock (const ATOM_LuaAutoBlock &);
	void operator = (const ATOM_LuaAutoBlock &);
	lua_State *_M_state;
	int _M_top;
};

#endif // __ATOM_SCRIPT_LUAAUTOBLOCK_H
