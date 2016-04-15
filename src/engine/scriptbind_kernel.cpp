#include "StdAfx.h"
#include "scriptbind_traits.h"

const char *AppInitEvent_GetArgv (ATOM_AppInitEvent *event, int index)
{
	return event->argv[index];
}

ATOM_Application *AppGetInstance (void)
{
	return ATOM_APP;
}

ATOM_Object *CreateObject (const char *classname, const char *objectname)
{
	ATOM_AUTOREF(ATOM_Object) obj = ATOM_CreateObject(classname, objectname);
	if (obj)
	{
		obj->addRef ();
	}
	return obj.get();
}

ATOM_Object *LookupObject (const char *classname, const char *objectname)
{
	ATOM_AUTOREF(ATOM_Object) obj = ATOM_LookupObject(classname, objectname);
	if (obj)
	{
		obj->addRef ();
	}
	return obj.get();
}

ATOM_Object *LookupOrCreateObject (const char *classname, const char *objectname)
{
	ATOM_AUTOREF(ATOM_Object) obj = ATOM_LookupOrCreateObject(classname, objectname, 0);
	if (obj)
	{
		obj->addRef ();
	}
	return obj.get();
}

ATOM_Object *LoadObject (const char *filename, const char *objectname)
{
	ATOM_AUTOREF(ATOM_Object) obj = ATOM_LoadObject(filename, objectname);
	if (obj)
	{
		obj->addRef ();
	}
	return obj.get();
}

static char versionStr[64];
const char *_GetVersionString (void)
{
	sprintf (versionStr, "%d.%d.%d", ATOM3D_MAJOR_VERSION, ATOM3D_MINOR_VERSION, ATOM3D_PATCH_VERSION);
	return versionStr;
}

unsigned GetNumCoroutines (void)
{
	return ATOM_Coroutine::getNumCoroutines ();
}

bool AddLZMAVFS (const char *path)
{
	ATOM_LzmaVFS *vfs = ATOM_NEW(ATOM_LzmaVFS);
	if (!vfs->load (path))
	{
		ATOM_DELETE(vfs);
		return false;
	}
	return ATOM_AddVFS (vfs, false);
}

ATOM_SCRIPT_BEGIN_FUNCTION_TABLE(KernelFunctions)
	ATOM_DECLARE_FUNCTION_DEREF(ATOM_CreateObject, CreateObject, ATOM_OBJECT_DEREF_FUNCNAME(ATOM_Object))
	ATOM_DECLARE_FUNCTION_DEREF(ATOM_LookupObject, LookupObject, ATOM_OBJECT_DEREF_FUNCNAME(ATOM_Object))
	ATOM_DECLARE_FUNCTION_DEREF(ATOM_LookupOrCreateObject, LookupOrCreateObject, ATOM_OBJECT_DEREF_FUNCNAME(ATOM_Object))
	ATOM_DECLARE_FUNCTION_DEREF(ATOM_LoadObject, LoadObject, ATOM_OBJECT_DEREF_FUNCNAME(ATOM_Object))
	ATOM_DECLARE_FUNCTION(ATOM_SaveOpenedFileList, ATOM_SaveOpenedFileList)
	ATOM_DECLARE_FUNCTION(ATOM_CopyOpenedFileList, ATOM_CopyOpenedFileList)
	ATOM_DECLARE_FUNCTION(ATOM_GetVersion, ATOM_GetVersion)
	ATOM_DECLARE_FUNCTION(ATOM_GetVersionString, _GetVersionString)
	ATOM_DECLARE_FUNCTION(ATOM_ReleaseObject, ATOM_ReleaseObject)
	ATOM_DECLARE_FUNCTION(ATOM_IsDirectory, ATOM_IsDirectory)
	ATOM_DECLARE_FUNCTION(ATOM_GetTick, ATOM_GetTick)
	ATOM_DECLARE_FUNCTION(ATOM_SaveObject, ATOM_SaveObject)
	ATOM_DECLARE_FUNCTION(ATOM_DumpObjects, ATOM_DumpObjects)
	ATOM_DECLARE_FUNCTION(ATOM_GetObjectCount, ATOM_GetObjectCount)
	ATOM_DECLARE_FUNCTION(ATOM_GetNumRequestsPending, ATOM_ContentStream::getNumRequestsPending)
	ATOM_DECLARE_FUNCTION(ATOM_SetMaxRequestsPending, ATOM_ContentStream::setMaxRequestsPending)
	ATOM_DECLARE_FUNCTION(ATOM_GetMaxRequestsPending, ATOM_ContentStream::getMaxRequestsPending)
	ATOM_DECLARE_FUNCTION(ATOM_SetProcessBatchBusy, ATOM_ContentStream::setProcessBatchBusy)
	ATOM_DECLARE_FUNCTION(ATOM_GetProcessBatchBusy, ATOM_ContentStream::getProcessBatchBusy)
	ATOM_DECLARE_FUNCTION(ATOM_SetProcessBatch, ATOM_ContentStream::setProcessBatch)
	ATOM_DECLARE_FUNCTION(ATOM_GetProcessBatch, ATOM_ContentStream::getProcessBatch)
	ATOM_DECLARE_FUNCTION(ATOM_BeginFindObject, ATOM_BeginFindObject)
	ATOM_DECLARE_FUNCTION(ATOM_FindNextObject, ATOM_FindNextObject)
	ATOM_DECLARE_FUNCTION(ATOM_EndFindObject, ATOM_EndFindObject)
	ATOM_DECLARE_FUNCTION(ATOM_GetObjectFound, ATOM_GetObjectFound)
	ATOM_DECLARE_FUNCTION(ATOM_CheckFileExistence, ATOM_CheckFileExistence)
	ATOM_DECLARE_FUNCTION(GetNumCoroutines, GetNumCoroutines)
	ATOM_DECLARE_FUNCTION(AddLZMAVFS, AddLZMAVFS)
ATOM_SCRIPT_END_FUNCTION_TABLE

ATOM_SCRIPT_BEGIN_TYPE_TABLE(KernelTypes)

	ATOM_SCRIPT_BEGIN_NONCOPYABLE_CLASS_NOPARENT_NOCONSTRUCTOR(ATOM_KernelConfig, ATOM_KernelConfig)
		ATOM_DECLARE_STATIC_METHOD(initialize, ATOM_KernelConfig::initialize)
		ATOM_DECLARE_STATIC_METHOD(initializeEx, ATOM_KernelConfig::initializeEx)
		ATOM_DECLARE_STATIC_METHOD(initializeWithPath, ATOM_KernelConfig::initializeWithPath)
		ATOM_DECLARE_STATIC_METHOD(initializeWithZIP, ATOM_KernelConfig::initializeWithZIP)
	ATOM_SCRIPT_END_CLASS()

	ATOM_SCRIPT_BEGIN_NONCOPYABLE_CLASS_NOPARENT_NOCONSTRUCTOR(ATOM_Object, ATOM_Object)
		ATOM_DECLARE_METHOD(getObjectName, ATOM_Object::getObjectName)
		ATOM_DECLARE_METHOD(getClassName, ATOM_Object::getClassName)
		ATOM_DECLARE_METHOD(dumpAttributes, ATOM_Object::dumpAttributes)
		ATOM_DECLARE_METHOD(copyAttributesTo, ATOM_Object::copyAttributesTo)
	ATOM_SCRIPT_END_CLASS()

	ATOM_SCRIPT_BEGIN_CLASS_NOPARENT(ATOM_FrameStamp, ATOM_FrameStamp)
		ATOM_DECLARE_METHOD(reset, ATOM_FrameStamp::reset)
		ATOM_DECLARE_RAW_ATTRIBUTE(frameStamp, ATOM_FrameStamp, frameStamp, unsigned)
		ATOM_DECLARE_RAW_ATTRIBUTE(currentTick, ATOM_FrameStamp, currentTick, unsigned)
		ATOM_DECLARE_RAW_ATTRIBUTE(elapsedTick, ATOM_FrameStamp, elapsedTick, unsigned)
		ATOM_DECLARE_RAW_ATTRIBUTE(FPS, ATOM_FrameStamp, FPS, unsigned)
		ATOM_DECLARE_RAW_ATTRIBUTE(timeScale, ATOM_FrameStamp, timeScale, float)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_NONCOPYABLE_CLASS_NOPARENT(ATOM_Application, ATOM_Application)
		ATOM_DECLARE_METHOD(setFrameInterval, ATOM_Application::setFrameInterval)
		ATOM_DECLARE_METHOD(getFrameInterval, ATOM_Application::getFrameInterval)
		ATOM_DECLARE_METHOD(getFrameStamp, ATOM_Application::getFrameStamp)
		ATOM_DECLARE_METHOD(setTimeScale, ATOM_Application::setTimeScale)
		ATOM_DECLARE_METHOD(getTimeScale, ATOM_Application::getTimeScale)
		ATOM_DECLARE_METHOD(postQuitEvent, ATOM_Application::postQuitEvent)
		ATOM_DECLARE_METHOD(setMainWindow, ATOM_Application::setMainWindow)
		ATOM_DECLARE_METHOD(getMainWindow, ATOM_Application::getMainWindow)
		ATOM_DECLARE_METHOD(updateFrameStamp, ATOM_Application::updateFrameStamp)
		ATOM_DECLARE_METHOD(run, ATOM_Application::run)
		ATOM_DECLARE_METHOD(handleEvent, ATOM_Application::handleEvent)
		ATOM_DECLARE_METHOD(setEventTrigger, ATOM_Application::setEventTrigger)
		ATOM_DECLARE_STATIC_METHOD(getInstance, AppGetInstance)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_NONCOPYABLE_CLASS_NOPARENT(ATOM_EventTrigger, ATOM_EventTrigger)
		ATOM_DECLARE_METHOD(getHost, ATOM_EventTrigger::getHost)
		ATOM_DECLARE_METHOD(setAutoCallHost, ATOM_EventTrigger::setAutoCallHost)
		ATOM_DECLARE_METHOD(getAutoCallHost, ATOM_EventTrigger::getAutoCallHost)
		ATOM_DECLARE_METHOD(callHost, ATOM_EventTrigger::callHost)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_NONCOPYABLE_CLASS_NOPARENT(ATOM_Event, ATOM_Event)
		ATOM_DECLARE_METHOD(getEventTypeId, ATOM_Event::getEventTypeId)
		ATOM_DECLARE_METHOD(setCallerAlive, ATOM_Event::setCallerAlive)
		ATOM_DECLARE_METHOD(isCallerAlive, ATOM_Event::isCallerAlive)
	ATOM_SCRIPT_END_CLASS ()
		
	ATOM_SCRIPT_BEGIN_CLASS(ATOM_AppIdleEvent, ATOM_AppIdleEvent, ATOM_Event, ATOM_Event)
		ATOM_DECLARE_STATIC_METHOD(eventTypeId, ATOM_AppIdleEvent::eventTypeId)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS(ATOM_AppInitEvent, ATOM_AppInitEvent, ATOM_Event, ATOM_Event)
		ATOM_DECLARE_STATIC_METHOD(eventTypeId, ATOM_AppInitEvent::eventTypeId)
		ATOM_DECLARE_RAW_ATTRIBUTE(argc, ATOM_AppInitEvent, argc, int)
		ATOM_DECLARE_RAW_ATTRIBUTE(success, ATOM_AppInitEvent, success, bool)
		ATOM_DECLARE_RAW_ATTRIBUTE(errorcode, ATOM_AppInitEvent, errorcode, int)
		ATOM_DECLARE_METHOD(getArgv, AppInitEvent_GetArgv)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS(ATOM_AppExitEvent, ATOM_AppExitEvent, ATOM_Event, ATOM_Event)
		ATOM_DECLARE_STATIC_METHOD(eventTypeId, ATOM_AppExitEvent::eventTypeId)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS(ATOM_AppQuitEvent, ATOM_AppQuitEvent, ATOM_Event, ATOM_Event)
		ATOM_DECLARE_STATIC_METHOD(eventTypeId, ATOM_AppQuitEvent::eventTypeId)
		ATOM_DECLARE_RAW_ATTRIBUTE(returnValue, ATOM_AppQuitEvent, returnValue, int)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS(ATOM_AppActiveEvent, ATOM_AppActiveEvent, ATOM_Event, ATOM_Event)
		ATOM_DECLARE_STATIC_METHOD(eventTypeId, ATOM_AppActiveEvent::eventTypeId)
		ATOM_DECLARE_RAW_ATTRIBUTE(active, ATOM_AppActiveEvent, active, bool)
	ATOM_SCRIPT_END_CLASS ()

ATOM_SCRIPT_END_TYPE_TABLE

void __kernel_bind_script (ATOM_Script *script)
{
	ATOM_SCRIPT_REGISTER_FUNCTION_TABLE(script, KernelFunctions);
	ATOM_SCRIPT_REGISTER_TYPE_TABLE(script, KernelTypes);
}
