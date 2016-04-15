#ifndef __ATOM_SCRIPT_SCRIPTFACTORY_H
#define __ATOM_SCRIPT_SCRIPTFACTORY_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"
#include "basedefs.h"

class ATOM_Script;
struct ATOM_ClassStruct;
struct ATOM_FunctionStruct;
struct ATOM_InstanceOp;
typedef void (ATOM_CALL *ATOM_ScriptInitCallback)(ATOM_Script *);

class ATOM_ScriptFactory
{
public:
	virtual ~ATOM_ScriptFactory ();
	// Factory interface.

public:
	ATOM_Script *createScript (ATOM_ScriptInitCallback callback);
	void deleteScript (ATOM_Script *scp);

protected:
	virtual ATOM_Script *_createScriptImpl (void) = 0;
	virtual void _deleteScriptImpl (ATOM_Script *scp) = 0;

public:
	virtual bool registerUserDataType (ATOM_Script *scp, ATOM_ClassStruct *ud) = 0;
	virtual bool registerUserFunction (ATOM_Script *scp, ATOM_FunctionStruct *uf) = 0;
	virtual bool registerUserConstant (ATOM_Script *scp, ATOM_FunctionStruct *uc) = 0;
};

#endif // __ATOM_SCRIPT_SCRIPTFACTORY_H
