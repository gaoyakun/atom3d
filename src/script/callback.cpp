#include <ATOM_dbghlp.h>

#include "callback.h"

ATOM_Mutex ATOM_ScriptCallbackBridge::lock;
ATOM_Script *ATOM_ScriptCallbackBridge::script = 0;
const char *ATOM_ScriptCallbackBridge::function = 0;

void ATOM_ScriptCallbackBridge::SetCallback (ATOM_Script *scp, const char *func)
{
	lock.lock ();
	script = scp;
	function = func;
}

void ATOM_ScriptCallbackBridge::GetCallback (ATOM_Script **scp, const char **func)
{
  *scp = script;
  *func = function;
  lock.unlock ();
}

void ATOM_DynamicFunction::create (const void *i0, const void *i1, void *callfunc, void *dst)
{
  pushaop0 = 0x66;
  pushaop1 = 0x60;
  pushop1 = 0x68;
  imm0 = i0;
  pushop2 = 0x68;
  imm1 = i1;
  callop = 0xE8;
  funcoffset = (char*)callfunc - ((char*)this + 17);
  addesp8_0 = 0x83;
  addesp8_1 = 0xC4;
  addesp8_2 = 0x08;
  popaop0 = 0x66;
  popaop1 = 0x61;
  jmpop = 0xE9;
  jmpoffset = (char*)dst - ((char*)this + 27);
}

