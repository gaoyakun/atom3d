#ifndef __ATOM_SCRIPT_SCRIPTMNGR_H
#define __ATOM_SCRIPT_SCRIPTMNGR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_kernel.h"
#include "../ATOM_utils.h"
#include "basedefs.h"
#include "var.h"
#include "scriptfactory.h"

class ATOM_Script;

class ATOM_SCRIPT_API ATOM_ScriptManager
{
public:
	typedef void (ATOM_CALL *ScriptExecuteCallback) (void *);
	struct ScriptTaskDesc
	{
		const char *scriptFileName;
		const char *scriptFuncName;
		const ATOM_ScriptArgs *scriptArgs;
		ATOM_ScriptInitCallback scriptInitCallback;
		ScriptExecuteCallback preExecuteCallback;
		ScriptExecuteCallback postExecuteCallback;
		void *executeCallbackContext;
	};

private:
	typedef ATOM_HASHMAP<ATOM_STRING, ATOM_Script*> TaskMap;
	typedef TaskMap::iterator TaskMapIter;
	typedef TaskMap::const_iterator TaskMapConstIter;

public:
	ATOM_ScriptManager (void);
	~ATOM_ScriptManager (void);
	ATOM_ScriptFactory *getFactory (void) const;
	ATOM_Script *createScript (ATOM_ScriptInitCallback callback);
	void destroyScript (ATOM_Script *script);
	void enableCacheScript (bool enable);
	bool isCacheScriptEnabled (void) const;
	ATOM_Coroutine::Coroutine *newScriptTask (const ScriptTaskDesc &desc);
private:
	ATOM_ScriptFactory *_factory;
	TaskMap _taskMap;
	bool _enableCacheScript;
};
#endif // __ATOM_SCRIPT_SCRIPTMANAGER_H
