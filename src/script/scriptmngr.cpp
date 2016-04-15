#include <ATOM_dbghlp.h>

#include "lua/luafactory.h"
#include "scriptmngr.h"
#include "script.h"
#include "scriptfactory.h"
#include "scriptinterface.h"

extern void ATOM_BindScript (ATOM_Script *);

void RegisterRTF (ATOM_ScriptManager *);

ATOM_ScriptManager::ATOM_ScriptManager (void) 
{
	_factory = ATOM_NEW(ATOM_LuaScriptFactory);
	_enableCacheScript = true;
}

ATOM_ScriptManager::~ATOM_ScriptManager (void)
{
	ATOM_DELETE(_factory);
}

ATOM_ScriptFactory *ATOM_ScriptManager::getFactory (void) const 
{
	return _factory;
}

ATOM_Script *ATOM_ScriptManager::createScript (ATOM_ScriptInitCallback callback) 
{
	ATOM_Script *scp = _factory->createScript (callback);
	if (scp)
	{
		ATOM_BindScript (scp);
	}
	return scp;
}

void ATOM_ScriptManager::destroyScript (ATOM_Script *script) {
	if (script)
	{
		_factory->deleteScript (script);
	}
}

struct ScriptExecuteContext
{
	ATOM_Script *script;
	ATOM_ScriptManager::ScriptExecuteCallback preExecuteCallback;
	ATOM_ScriptManager::ScriptExecuteCallback postExecuteCallback;
	void *executeCallbackContext;
};

static void ATOM_CALL scriptTaskFunc (void *param)
{
	ScriptExecuteContext *context = (ScriptExecuteContext*)param;
	if (context->preExecuteCallback)
		context->preExecuteCallback (context->executeCallbackContext);
	context->script->execute ();
	if (context->postExecuteCallback)
		context->postExecuteCallback (context->executeCallbackContext);

	ATOM_DELETE(context->script);
	ATOM_DELETE(context);
}

ATOM_Coroutine::Coroutine *ATOM_ScriptManager::newScriptTask (const ScriptTaskDesc &desc)
//ATOM_Coroutine::Coroutine *ATOM_ScriptManager::newScriptTask (const char *scriptFileName, const char *functionName, const ATOM_ScriptArgs &args, ATOM_ScriptInitCallback callback)
{
	if (!desc.scriptFileName || !desc.scriptFuncName)
	{
		return 0;
	}

	char buffer[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (desc.scriptFileName, buffer);
	strlwr (buffer);

	ATOM_Script *script = 0;
	if (_enableCacheScript)
	{
		ATOM_Script *mainScript = 0;
		TaskMapIter it = _taskMap.find (buffer);
		if (it == _taskMap.end ())
		{
			ATOM_AutoFile f(desc.scriptFileName, ATOM_VFS::read|ATOM_VFS::text);
			if (!f)
			{
				return 0;
			}
			ATOM_VECTOR<char> content(f->size() + 1);
			unsigned sz = f->read (&content[0], f->size());
			content[sz] = '\0';

			mainScript = createScript (desc.scriptInitCallback);
			if (!mainScript)
			{
				return 0;
			}
			if (!mainScript->setSource (&content[0], true))
			{
				destroyScript (mainScript);
				return 0;
			}
			_taskMap[buffer] = mainScript;
		}
		else
		{
			mainScript = it->second;
		}

		script = mainScript->clone ();
	}
	else
	{
		ATOM_AutoFile f(buffer, ATOM_VFS::read|ATOM_VFS::text);
		if (!f)
		{
			return 0;
		}
		ATOM_VECTOR<char> content(f->size() + 1);
		unsigned sz = f->read (&content[0], f->size());
		content[sz] = '\0';

		script = createScript (desc.scriptInitCallback);
		if (!script)
		{
			return 0;
		}
		if (!script->setSource (&content[0], true))
		{
			destroyScript (script);
			return 0;
		}
	}

	script->setFunction (desc.scriptFuncName);
	if (desc.scriptArgs)
	{
		script->setNumParameters (desc.scriptArgs->size());
		for (unsigned i = 0; i < desc.scriptArgs->size(); ++i)
		{
			script->setParameter (i, (*desc.scriptArgs)[i]);
		}
	}

	ScriptExecuteContext *context = ATOM_NEW(ScriptExecuteContext);
	context->script = script;
	context->preExecuteCallback = desc.preExecuteCallback;
	context->postExecuteCallback = desc.postExecuteCallback;
	context->executeCallbackContext = desc.executeCallbackContext;
	return ATOM_Coroutine::createCoroutine (scriptTaskFunc, context);
}

void ATOM_ScriptManager::enableCacheScript (bool enable)
{
	_enableCacheScript = enable;
}

bool ATOM_ScriptManager::isCacheScriptEnabled (void) const
{
	return _enableCacheScript;
}

