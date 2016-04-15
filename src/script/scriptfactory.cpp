#include <ATOM_dbghlp.h>
#include "scriptfactory.h"

ATOM_ScriptFactory::~ATOM_ScriptFactory ()
{
}

ATOM_Script *ATOM_ScriptFactory::createScript (ATOM_ScriptInitCallback callback)
{
	ATOM_Script *scp = _createScriptImpl ();
	if (scp && callback)
	{
		callback (scp);
	}
	return scp;
}

void ATOM_ScriptFactory::deleteScript (ATOM_Script *scp)
{
	_deleteScriptImpl(scp);
}

