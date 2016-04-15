#include "stdafx.h"
#include "kernelserver.h"
#include "lzmaenc.h"
#include "lzmadec.h"

#define MAX_VFS_COUNT 1024
static ATOM_VFS *_VFSs[MAX_VFS_COUNT];
static long _numVFSs = 0;
static int _physicalVFS = -1;
static VFSLoadCallback _VFSLoadCallback = 0;
static void *_VFSLoadCallbackUserData = 0;
static ATOM_Mutex fileRecordLock;
static ATOM_MAP<ATOM_STRING, ATOM_STRING> recordedFileName;
static ATOM_STRING fileId = "";
static ATOM_PhysicVFS physicVFS;

ATOM_KERNEL_API unsigned ATOM_CALL ATOM_GetVersion (void)
{
	return ATOM3D_VERSION;
}

ATOM_KERNEL_API void ATOM_CALL ATOM_DumpObjects(const char *filename) {
  if (ATOM_KernelServer::getInstance())
  {
    ATOM_KernelServer::getInstance()->GetFactory()->DumpObjects (filename);
  }
}

ATOM_KERNEL_API void * ATOM_CALL ATOM_BeginFindObject (void)
{
  if (ATOM_KernelServer::getInstance())
  {
    return ATOM_KernelServer::getInstance()->GetFactory()->beginFindObject ();
  }
  return 0;
}

ATOM_KERNEL_API bool ATOM_CALL ATOM_FindNextObject (void *handle)
{
  if (ATOM_KernelServer::getInstance())
  {
    return ATOM_KernelServer::getInstance()->GetFactory()->findNextObject (handle);
  }
  return false;
}

ATOM_KERNEL_API void ATOM_CALL ATOM_EndFindObject (void *handle)
{
  if (ATOM_KernelServer::getInstance())
  {
    ATOM_KernelServer::getInstance()->GetFactory()->endFindObject (handle);
  }
}

ATOM_KERNEL_API ATOM_Object *ATOM_CALL ATOM_GetObjectFound (void *handle)
{
  if (ATOM_KernelServer::getInstance())
  {
    return ATOM_KernelServer::getInstance()->GetFactory()->getObjectFound (handle);
  }
  return 0;
}

ATOM_KERNEL_API unsigned ATOM_CALL ATOM_GetObjectCount (const char *classname) {
  ATOM_STACK_TRACE(ATOM_GetObjectCount);
  if (ATOM_KernelServer::getInstance())
  {
    return ATOM_KernelServer::getInstance()->GetFactory()->GetObjectCount (classname);
  }
  return 0;
}

ATOM_KERNEL_API void ATOM_CALL ATOM_RegisterType(const char* classname,
                                                  ATOM_Object *(*cf) (void),
                                                  void (*df) (ATOM_Object*),
                                                  void (*pf) (ATOM_Object*),
                                                  ATOM_ScriptInterfaceBase *si,
                                                  bool cache) {
  ATOM_STACK_TRACE(ATOM_RegisterType);
  if (ATOM_KernelServer::getInstance())
  {
    ATOM_KernelServer::getInstance()->GetFactory()->RegisterObjectType(classname, cf, df, pf, si, cache);
  }
}

ATOM_KERNEL_API void ATOM_CALL ATOM_UnregisterType(const char* classname) {
  ATOM_STACK_TRACE(ATOM_UnregisterType);
  if (ATOM_KernelServer::getInstance())
  {
    ATOM_KernelServer::getInstance()->GetFactory()->UnregisterObjectType(classname);
  }
}

ATOM_KERNEL_API bool ATOM_CALL ATOM_IsObjectType (const char *classname) {
  ATOM_STACK_TRACE(ATOM_IsObjectType);
  if (ATOM_KernelServer::getInstance())
  {
    return ATOM_KernelServer::getInstance()->GetFactory()->ObjectTypeRegistered (classname);
  }
  return false;
}

ATOM_KERNEL_API ATOM_AUTOREF(ATOM_Object) ATOM_CALL ATOM_CreateObject(const char* classname, const char* objectname) {
  ATOM_STACK_TRACE(ATOM_CreateObject);
  if (ATOM_KernelServer::getInstance())
  {
    return ATOM_KernelServer::getInstance()->GetFactory()->CreateObject(classname, objectname);
  }
  return ATOM_AUTOREF(ATOM_Object)();
}

ATOM_KERNEL_API ATOM_AUTOREF(ATOM_Object) ATOM_CALL ATOM_LookupOrCreateObject(const char *classname, const char* objectname, bool *createNew) {
  ATOM_STACK_TRACE(ATOM_LookupOrCreateObject);
  if (ATOM_KernelServer::getInstance())
  {
    return ATOM_KernelServer::getInstance()->GetFactory()->LookupOrCreateObject(classname, objectname, createNew);
  }
  return ATOM_AUTOREF(ATOM_Object)();
}

ATOM_KERNEL_API ATOM_AUTOREF(ATOM_Object) ATOM_CALL ATOM_LookupObject(const char *classname, const char* objectname) {
  ATOM_STACK_TRACE(ATOM_LookupObject);
  if (ATOM_KernelServer::getInstance())
  {
    return ATOM_KernelServer::getInstance()->GetFactory()->LookupObject(classname, objectname);
  }
  return ATOM_AUTOREF(ATOM_Object)();
}

ATOM_KERNEL_API void ATOM_CALL ATOM_ReleaseObject(ATOM_Object* object) 
{
  ATOM_STACK_TRACE(ATOM_ReleaseObject);
  if (ATOM_KernelServer::getInstance())
  {
    ATOM_KernelServer::getInstance()->GetFactory()->ReleaseObject(object);
  }
}   

ATOM_KERNEL_API ATOM_AUTOREF(ATOM_Object) ATOM_CALL ATOM_LoadResource(const char* classname, const char* path, bool binary, void *userdata) 
{
  ATOM_STACK_TRACE(ATOM_LoadResource);
  if (!classname || !path)
  {
    return 0;
  }

  char identifyPath[ATOM_VFS::max_filename_length];
  if (!ATOM_CompletePath (path, identifyPath))
  {
    return 0;
  }
  
  bool createNew;

  ATOM_AUTOREF(ATOM_Resource) obj = ATOM_LookupOrCreateObject(classname, identifyPath, &createNew);

  if (!obj || !createNew)
  {
    return obj;
  }

  ATOM_File *file = ATOM_OpenFile (identifyPath, binary ? ATOM_VFS::read : (ATOM_VFS::read|ATOM_VFS::text));
  if (!file)
  {
    return 0;
  }

  if (!obj->loadFromFile(file, userdata))
  {
    obj = 0;
  }

  ATOM_CloseFile (file);

  return obj;
}

bool ATOM_AddVFS (ATOM_VFS *vfs, bool isPhysical)
{
	if (_numVFSs==MAX_VFS_COUNT)
	{
		return false;
	}

  ATOM_STACK_TRACE(ATOM_AddVFS);
  if (vfs)
  {
    if (isPhysical && _physicalVFS >= 0)
	{
		ATOM_LOGGER::error ("_addVFS(): Only one physical ATOM_VFS can be mounted.\n");
		return false;
	}

	_VFSs[_numVFSs] = vfs;

	if (isPhysical)
	{
		_physicalVFS = _numVFSs;
	}

	::InterlockedIncrement(&_numVFSs);

	return true;
  }

  return false;
}

ATOM_KERNEL_API ATOM_VFS *ATOM_CALL ATOM_GetVFSPathName(const char *path, char *vfsPath)
{
	ATOM_STACK_TRACE(ATOM_GetVFSPathName);
	char buffer[ATOM_VFS::max_filename_length];

	ATOM_VFS *vfs = ATOM_GetPhysicalPathName("/", buffer);
	if (!vfs)
	{
		return false;
	}

	char buffer1[ATOM_VFS::max_filename_length];
	char buffer2[ATOM_VFS::max_filename_length];

	if (0  == ::GetFullPathNameA (buffer, ATOM_VFS::max_filename_length, buffer1, 0))
	{
		return 0;
	}

	if (0 == ::GetFullPathNameA (path, ATOM_VFS::max_filename_length, buffer2, 0))
	{
		return 0;
	}

	strlwr (buffer1);
	strlwr (buffer2);

	unsigned len = strlen(buffer1);
	if (!strncmp (buffer1, buffer2, len))
	{
		char *p = buffer2 + len;
		if (*p != '\\')
		{
			return 0;
		}

		p++;
		*vfsPath++ = '/';

		while (*p)
		{
			if ((*p) == '\\')
			{
				*p = '/';
			}
			*vfsPath++ = *p++;
		}

		*vfsPath = '\0';
		return vfs;
	}

	return 0;
}

ATOM_KERNEL_API ATOM_VFS * ATOM_CALL ATOM_GetNativePathName (const char *path, char *nativePath)
{
  ATOM_STACK_TRACE(ATOM_GetNativePathName);

	  if (_numVFSs == 0 || !path || !path[0] || !nativePath)
	  {
		  return 0;
	  }

	  for (unsigned i = 0; i < _numVFSs; ++i)
	  {
		  ATOM_VFS *vfs = _VFSs[i]->getNativePath (path, nativePath);
		  if (vfs)
		  {
			  return vfs;
		  }
	  }

  return 0;
}

ATOM_KERNEL_API ATOM_VFS * ATOM_CALL ATOM_GetPhysicalPathName(const char *path, char *physicalPath)
{
	ATOM_STACK_TRACE(ATOM_GetPhysicalPathName);

	if (!path || !path[0] || !physicalPath)
	{
		return 0;
	}

	{
		if (_physicalVFS < 0 || _physicalVFS >= _numVFSs)
		{
			return 0;
		}

		ATOM_VFS *vfs = _VFSs[_physicalVFS]->getNativePath (path, physicalPath);
		if (vfs)
		{
			return vfs;
		}
	}

	return 0;
}

ATOM_KERNEL_API ATOM_VFS::finddata_t * ATOM_CALL ATOM_FindFirst(const char* parent_dir)
{
  ATOM_STACK_TRACE(ATOM_FindFirst);
  if (_numVFSs == 0)
  {
    return 0;
  }
  return _VFSs[0]->findFirst (parent_dir);
}

//-----------------------------------------------------
// Function name   : char* ATOM_CALL ATOM_FindNext
// Description     : 
// Return type     : ATOM_KERNEL_API const 
// Argument        : int handle
//-----------------------------------------------------
ATOM_KERNEL_API bool ATOM_CALL ATOM_FindNext(ATOM_VFS::finddata_t *handle) 
{
  ATOM_STACK_TRACE(ATOM_FindNext);

  if (!handle || _numVFSs == 0)
  {
    return false;
  }
  return _VFSs[0]->findNext (handle);
}

//-----------------------------------------------------
// Function name   : ATOM_CALL ATOM_CloseFind
// Description     : 
// Return type     : ATOM_KERNEL_API void 
// Argument        : int handle
//-----------------------------------------------------
ATOM_KERNEL_API void ATOM_CALL ATOM_CloseFind(ATOM_VFS::finddata_t *handle) 
{
  ATOM_STACK_TRACE(ATOM_CloseFind);

  if (handle && !_numVFSs == 0)
  {
    _VFSs[0]->findClose (handle);
  }
}

//-----------------------------------------------------
// Function name   : ATOM_CALL ATOM_FileExists
// Description     : 
// Return type     : ATOM_KERNEL_API bool 
// Argument        : const char* path
//-----------------------------------------------------
ATOM_KERNEL_API bool ATOM_CALL ATOM_FileExists(const char* path) 
{ 
  ATOM_STACK_TRACE(ATOM_FileExists);

  for (unsigned i = 0; i < _numVFSs; ++i)
  {
    if (_VFSs[i]->doesFileExists (path))
    {
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------
// Function name   : ATOM_CALL ATOM_IsDirectory
// Description     : 
// Return type     : ATOM_KERNEL_API bool 
// Argument        : const char* path
//-----------------------------------------------------
ATOM_KERNEL_API bool ATOM_CALL ATOM_IsDirectory(const char* path) 
{ 
  ATOM_STACK_TRACE(ATOM_IsDirectory);
  if (path)
  {
    for (unsigned i = 0; i < _numVFSs; ++i)
    {
      if (_VFSs[i]->doesFileExists (path))
      {
        return _VFSs[i]->isDir(path);
      }
    }
  }
  return false;
}

//-----------------------------------------------------
// Function name   : ATOM_CALL ATOM_CreateDirectory
// Description     : 
// Return type     : ATOM_KERNEL_API bool 
// Argument        : const char* path
//-----------------------------------------------------
ATOM_KERNEL_API bool ATOM_CALL ATOM_CreateDirectory(const char* path)
{ 
  ATOM_STACK_TRACE(ATOM_CreateDirectory);

  if (!path || _numVFSs == 0)
  {
    return false;
  }

  return _VFSs[0]->mkDir (path);
}

//-----------------------------------------------------
// Function name   : ATOM_CALL ATOM_Remove
// Description     : 
// Return type     : ATOM_KERNEL_API bool 
// Argument        : const char* path
//-----------------------------------------------------
ATOM_KERNEL_API bool ATOM_CALL ATOM_Remove(const char* path) 
{
  ATOM_STACK_TRACE(ATOM_Remove);

  if (!path || _numVFSs == 0)
  {
    return false;
  }

  return _VFSs[0]->removeFile (path, false);
}

//-----------------------------------------------------
// Function name   : ATOM_CALL ATOM_GetCWD
// Description     : 传进来的buffer大小必须是MAX_PATH
// Return type     : ATOM_KERNEL_API unsigned 
// Argument        : char* buffer
// Argument        : unsigned size
//-----------------------------------------------------
ATOM_KERNEL_API void ATOM_CALL ATOM_GetCWD(char* buffer)
{
  ATOM_STACK_TRACE(ATOM_GetCWD);

  if (buffer)
  {
    buffer[0] = '\0';

    if (_numVFSs != 0)
    {
      _VFSs[0]->getCWD (buffer);
    }
  }
}

//-----------------------------------------------------
// Function name   : ATOM_CALL ATOM_SetCWD
// Description     : 
// Return type     : ATOM_KERNEL_API bool 
// Argument        : const char* path
//-----------------------------------------------------
ATOM_KERNEL_API bool ATOM_CALL ATOM_SetCWD(const char* path)
{
  ATOM_STACK_TRACE(ATOM_SetCWD);

  if (path && _numVFSs != 0)
  {
    return _VFSs[0]->chDir (path);
  }

  return false;
}

//-----------------------------------------------------
// Function name   : ATOM_CALL ATOM_CompletePath
// Description     : 
// Return type     : ATOM_KERNEL_API unsigned 
// Argument        : const char* path
// Argument        : char* buffer 必须MAX_PATH，没得商量
// Argument        : unsigned size
//-----------------------------------------------------
ATOM_KERNEL_API bool ATOM_CALL ATOM_CompletePath(const char* path, char* completed) 
{
  ATOM_STACK_TRACE(ATOM_CompletePath);

  if (path && completed && _numVFSs != 0)
  {
    return _VFSs[0]->identifyPath (path, completed) != 0;
  }

  return false;
}

ATOM_KERNEL_API void ATOM_CALL ATOM_SaveOpenedFileList(const char* path)
{
	ATOM_Mutex::ScopeMutex lock(fileRecordLock);

	ATOM_MAP<ATOM_STRING, ATOM_VECTOR<ATOM_STRING> > groupedFileNames;
	for (ATOM_MAP<ATOM_STRING, ATOM_STRING>::const_iterator it = recordedFileName.begin(); it != recordedFileName.end(); ++it)
	{
		groupedFileNames[it->second].push_back (it->first);
	}

	FILE *fp = fopen (path, "wt");
	if (fp)
	{
		fprintf (fp, "%d files opened\n", recordedFileName.size());
		for (ATOM_MAP<ATOM_STRING, ATOM_VECTOR<ATOM_STRING> >::const_iterator it = groupedFileNames.begin(); it != groupedFileNames.end(); ++it)
		{
			const ATOM_VECTOR<ATOM_STRING> &v = it->second;
			fprintf (fp, "===================%s(%d)=====================\n", it->first.c_str(), v.size());
			for (int i = 0; i < v.size(); ++i)
			{
				const ATOM_STRING &filename = v[i];
				fprintf (fp, "%s\n", filename.c_str());
			}
		}
		fclose (fp);
	}
}

ATOM_KERNEL_API void ATOM_CALL ATOM_SetFileRecordID(const char *id)
{
	ATOM_Mutex::ScopeMutex lock(fileRecordLock);
	fileId = id;
}

ATOM_KERNEL_API void ATOM_CALL ATOM_ClearOpenedFileList(const char* path)
{
	ATOM_Mutex::ScopeMutex lock(fileRecordLock);
	recordedFileName.clear ();
}

static bool copySingleFile (const char *filename, const char *path, const char *dir)
{
	if (filename[0] != '/')
	{
		return false;
	}

	ATOM_STRING destFileName = path;
	if (destFileName.back() != '\\' && destFileName.back() != '/')
	{
		destFileName += '/';
	}
	if (dir)
	{
		destFileName += dir;
		if (destFileName.back() != '\\' && destFileName.back() != '/')
		{
			destFileName += '/';
		}
	}
	destFileName += filename+1;

	char buffer[ATOM_VFS::max_filename_length];
	if (!physicVFS.identifyPath (destFileName.c_str(), buffer))
	{
		return false;
	}

	char *lastSep = strrchr (buffer, '\\');
	*lastSep = '\0';
	if (!physicVFS.isDir(buffer) && !physicVFS.mkDir (buffer))
	{
		return false;
	}
	*lastSep = '\\';

	char srcFileName[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (filename, srcFileName);

	if (!::CopyFileA (srcFileName, buffer, FALSE))
	{
		return false;
	}

	return true;
}

ATOM_KERNEL_API int ATOM_CALL ATOM_CopyOpenedFileList(const char *path, const char *id, bool createIdDirectory)
{
	ATOM_MAP<ATOM_STRING, ATOM_VECTOR<ATOM_STRING> > groupedFileNames;
	for (ATOM_MAP<ATOM_STRING, ATOM_STRING>::const_iterator it = recordedFileName.begin(); it != recordedFileName.end(); ++it)
	{
		groupedFileNames[it->second].push_back (it->first);
	}

	int filesCopied = 0;
	if (id)
	{
		ATOM_MAP<ATOM_STRING, ATOM_VECTOR<ATOM_STRING> >::const_iterator it = groupedFileNames.find (id);
		if (it != groupedFileNames.end ())
		{
			const ATOM_VECTOR<ATOM_STRING> &v = it->second;
			for (int i = 0; i < v.size(); ++i)
			{
				if (copySingleFile (v[i].c_str(), path, createIdDirectory?id:NULL))
				{
					filesCopied++;
				}
			}
		}
	}
	else
	{
		for (ATOM_MAP<ATOM_STRING, ATOM_VECTOR<ATOM_STRING> >::const_iterator it = groupedFileNames.begin(); it != groupedFileNames.end(); ++it)
		{
			const char *id = it->first.c_str();
			filesCopied += ATOM_CopyOpenedFileList (path, id, createIdDirectory);
		}
	}

	return filesCopied;
}


//-----------------------------------------------------
// Function name   : *ATOM_CALL ATOM_Open
// Description     : 
// Return type     : ATOM_KERNEL_API ATOM_File 
// Argument        : const char* path
// Argument        : int mode
//-----------------------------------------------------
ATOM_KERNEL_API ATOM_File *ATOM_CALL ATOM_OpenFile (const char* path, int mode)
{
  ATOM_STACK_TRACE(ATOM_OpenFile);
  if (!path)
  {
	  return 0;
  }
 
  if (ATOM_KernelConfig::isRecordFileName () && ATOM_Application::isAppThread ())
  {
	  char buffer[ATOM_VFS::max_filename_length];
	  ATOM_CompletePath (path, buffer);
	  strlwr (buffer);

	  {
		ATOM_Mutex::ScopeMutex lock(fileRecordLock);
		if (recordedFileName.find (buffer) == recordedFileName.end ())
		{
			  recordedFileName[buffer] = fileId;
		}
	  }
  }

  {
	  if (_numVFSs != 0)
	  {
		  for (int i = _numVFSs-1; i >= 0; --i)
		  {
			ATOM_VFS::handle handle = ATOM_VFS::vfsOpen (_VFSs[i], path, mode);
			if (handle != ATOM_VFS::invalid_handle)
			{
			  return ATOM_NEW (ATOM_File, handle);
			}
		  }
	  }
  }

  if (path && path[0] == '/')
  {
	  ++path;
  }

  ATOM_VFS::handle handle = physicVFS.vfopen (path, mode);
  if (handle != ATOM_VFS::invalid_handle)
  {
	  return ATOM_NEW(ATOM_File, handle);
  }

  return 0;
}

ATOM_KERNEL_API bool ATOM_CALL ATOM_CheckFileExistence(const char *path)
{
	ATOM_STACK_TRACE(ATOM_CheckFileExistence);
	if (!path)
	{
		return false;
	}

	{
		if (_numVFSs != 0)
		{
			for (int i = _numVFSs-1; i >= 0; --i)
			{
				if (_VFSs[i]->doesFileExists(path))
				{
					return true;
				}
			}
		}
	}

	if (path && path[0] == '/')
	{
		++path;
	}

	return physicVFS.doesFileExists (path);
}

ATOM_KERNEL_API void ATOM_CALL ATOM_CloseFile (ATOM_File* file)
{
  ATOM_STACK_TRACE(ATOM_CloseFile);
  if (file)
  {
    ATOM_VFS::vfsClose (file->getHandle ());
    ATOM_DELETE(file);
  }
}

ATOM_KERNEL_API void ATOM_CALL ATOM_PutEnv(const char* var, const char* value) {
  ATOM_STACK_TRACE(ATOM_PutEnv);
  if ( var == 0)
  {
    ATOM_LOGGER::error("[ATOM_PutEnv] Invalid variable name: %s.\n", var);
    return;
  }

#ifdef WIN32
  ::SetEnvironmentVariableA(var, value);
#elif defined(POSIX)
  unsigned len = strlen(var) + 1;
  if ( value)
    len += strlen(value);
  char* buf = ATOM_NEW_ARRAY(char, len + 1);
  if ( value)
    sprintf(buf, "%s=%s", var, value);
  else
    sprintf(buf, "%s=", var);
  putenv(buf);
  ATOM_DELETE(buf);
#else
# error Unsupport platform.
#endif
}

ATOM_KERNEL_API unsigned ATOM_CALL ATOM_GetEnv(const char* var, char* buffer, unsigned size) {
  ATOM_STACK_TRACE(ATOM_GetEnv);
  if ( var == 0)
  {
    ATOM_LOGGER::error ("[ATOM_GetEnv] Invalid variable name: %s.\n", var);
    return 0;
  }

#ifdef WIN32
  return ::GetEnvironmentVariable(var, buffer, size);
#elif defined(POSIX)
  char* ret = getenv(var);
  if ( ret)
    if ( buffer == 0)
      return strlen(ret) + 1;
    else
    {
      if ( size > strlen(ret))
        strcpy(buffer, ret);
      else if ( size > 0)
      {
        MEMCPY(buffer, ret, size - 1);
        buffer[size - 1] = '\0';
      }
      return 0;
    }
    else if ( buffer == 0)
      return 1;
    else
    {
      if ( size > 0)
        buffer[0] = '\0';
      return 0;
    }
#else
# error Unsupport platform
#endif

}

/*
ATOM_KERNEL_API void ATOM_CALL ATOM_Suspend (unsigned duration) {
  if (ATOM_KernelServer::getInstance())
  {
    ATOM_SYNC_ACCESSOR(ATOM_KernelServer, KS, ATOM_KernelServer::getInstance());
    KS.get()->Suspend (duration);
  }
}

ATOM_KERNEL_API void ATOM_CALL ATOM_RunTasks () {
  if (ATOM_KernelServer::getInstance())
  {
    ATOM_SYNC_ACCESSOR(ATOM_KernelServer, KS, ATOM_KernelServer::getInstance());
    KS.get()->Suspend (0);
  }
}
*/

/*
ATOM_KERNEL_API bool ATOM_CALL ATOM_RegisterRuntimeFunction (ATOM_FunctionCallerBase *desc) {
  return RegisterRuntimeFunction (desc);
}

ATOM_KERNEL_API void ATOM_CALL ATOM_UnregisterRuntimeFunction (const char *name) {
  UnregisterRuntimeFunction (name);
}

ATOM_KERNEL_API bool ATOM_CALL ATOM_RegisterRuntimeConstant (ATOM_VariableAccessorBase *desc) {
  return RegisterRuntimeConstant (desc);
}

ATOM_KERNEL_API void ATOM_CALL ATOM_UnregisterRuntimeConstant (const char *name) {
  UnregisterRuntimeConstant (name);
}
*/
ATOM_KERNEL_API ATOM_AttribAccessorBase* ATOM_CALL ATOM_FindAttrib(const char *classname, const char *attrib) {
  ATOM_STACK_TRACE(ATOM_FindAttrib);
  if (ATOM_KernelServer::getInstance())
  {
    return ATOM_KernelServer::getInstance()->GetFactory()->FindAttrib (classname, attrib);
  }
  return 0;
}

ATOM_KERNEL_API unsigned ATOM_CALL ATOM_GetAttribCount (const char *classname) {
  ATOM_STACK_TRACE(ATOM_GetAttribCount);
  if (ATOM_KernelServer::getInstance())
  {
    return ATOM_KernelServer::getInstance()->GetFactory()->GetAttribCount (classname);
  }
  return 0;
}

ATOM_KERNEL_API int ATOM_CALL ATOM_GetAttribType (const char *classname, const char *attrib) {
  ATOM_STACK_TRACE(ATOM_GetAttribType);
  if (ATOM_KernelServer::getInstance())
  {
    ATOM_AttribAccessorBase *a = ATOM_KernelServer::getInstance()->GetFactory()->FindAttrib (classname, attrib);

    if (a)
    {
      return a->attrib.type;
    }
  }
  return -1;
}

ATOM_KERNEL_API const char * ATOM_CALL ATOM_GetAttribTypeDesc (const char *classname, const char *attrib) {
  ATOM_STACK_TRACE(ATOM_GetAttribTypeDesc);
  if (ATOM_KernelServer::getInstance())
  {
    ATOM_AttribAccessorBase *a = ATOM_KernelServer::getInstance()->GetFactory()->FindAttrib (classname, attrib);

    if (a)
    {
      return a->attrib.typedesc;
    }
  }
  return 0;
}

ATOM_KERNEL_API const char *ATOM_CALL ATOM_GetAttribComment (const char *classname, const char *attrib) {
  ATOM_STACK_TRACE(ATOM_GetAttribComment);
  if (ATOM_KernelServer::getInstance())
  {
    ATOM_AttribAccessorBase *a = ATOM_KernelServer::getInstance()->GetFactory()->FindAttrib (classname, attrib);
    if (a)
    {
      return a->attrib.comment;
    }
  }
  return 0;
}

ATOM_KERNEL_API const char *ATOM_CALL ATOM_GetAttribName (const char *classname, unsigned index) {
  ATOM_STACK_TRACE(ATOM_GetAttribName);
  if (ATOM_KernelServer::getInstance())
  {
    ATOM_AttribAccessorBase *attrib = ATOM_KernelServer::getInstance()->GetFactory()->GetAttrib (classname, index);
    if (attrib)
      return attrib->attrib.name;
  }
  return 0;
}

ATOM_KERNEL_API bool ATOM_CALL ATOM_IsAttribReadonly (const char *classname, const char *attrib) {
  ATOM_STACK_TRACE(ATOM_IsAttribReadonly);
  if (ATOM_KernelServer::getInstance())
  {
    ATOM_AttribAccessorBase *a = ATOM_KernelServer::getInstance()->GetFactory()->FindAttrib (classname, attrib);
    if (a)
    {
      return a->IsReadonly ();
    }
  }
  return false;
}

ATOM_KERNEL_API bool ATOM_CALL ATOM_IsAttribPersistent (const char *classname, const char *attrib) {
  ATOM_STACK_TRACE(ATOM_IsAttribPersistent);
  if (ATOM_KernelServer::getInstance())
  {
    ATOM_AttribAccessorBase *a = ATOM_KernelServer::getInstance()->GetFactory()->FindAttrib (classname, attrib);
    if (a)
    {
      return a->IsPersistent ();
    }
  }
  return false;
}

ATOM_KERNEL_API bool ATOM_CALL ATOM_IsAttribHasDefaultValue (const char *classname, const char *attrib) {
  ATOM_STACK_TRACE(ATOM_IsAttribHasDefaultValue);
  if (ATOM_KernelServer::getInstance())
  {
    ATOM_AttribAccessorBase *a = ATOM_KernelServer::getInstance()->GetFactory()->FindAttrib (classname, attrib);
    if (a)
    {
      return a->HasDefaultValue ();
    }
  }
  return false;
}

ATOM_KERNEL_API bool ATOM_CALL ATOM_GetDefaultAttribValue (const char *classname, const char *attrib, ATOM_ScriptVar &value) {
  ATOM_STACK_TRACE(ATOM_GetDefaultAttribValue);
  if (ATOM_KernelServer::getInstance())
  {
    ATOM_AttribAccessorBase *a = ATOM_KernelServer::getInstance()->GetFactory()->FindAttrib (classname, attrib);

    if (!a)
      return false;

    value.setType (a->attrib.type);
    bool result = false;

    switch (a->attrib.type)
    {
    case TYPE_INT:
      result = a->GetDefaultValue (&value.asInteger());
      break;
    case TYPE_INT_ARRAY:
      result = a->GetDefaultValue (&value.asIntegerArray());
      break;
    case TYPE_FLOAT:
      result = a->GetDefaultValue (&value.asFloat());
      break;
    case TYPE_FLOAT_ARRAY:
      result = a->GetDefaultValue (&value.asFloatArray());
      break;
    case TYPE_STRING:
      result = a->GetDefaultValue (&value.asString());
      break;
    case TYPE_STRING_ARRAY:
      result = a->GetDefaultValue (&value.asStringArray());
      break;
	case TYPE_VEC2:
	  result = a->GetDefaultValue (&value.asVector2());
	  break;
    case TYPE_VEC3:
      result = a->GetDefaultValue (&value.asVector3());
      break;
    case TYPE_VEC2_ARRAY:
      result = a->GetDefaultValue (&value.asVector2Array());
      break;
    case TYPE_VEC3_ARRAY:
      result = a->GetDefaultValue (&value.asVector3Array());
      break;
    case TYPE_VEC4:
      result = a->GetDefaultValue (&value.asVector4());
      break;
    case TYPE_VEC4_ARRAY:
      result = a->GetDefaultValue (&value.asVector4Array());
      break;
    case TYPE_MAT33:
      result = a->GetDefaultValue (&value.asMatrix33());
      break;
    case TYPE_MAT33_ARRAY:
      result = a->GetDefaultValue (&value.asMatrix33Array());
      break;
    case TYPE_MAT44:
      result = a->GetDefaultValue (&value.asMatrix44());
      break;
    case TYPE_MAT44_ARRAY:
      result = a->GetDefaultValue (&value.asMatrix44Array());
      break;
    case TYPE_OBJECT:
      result = a->GetDefaultValue (&value.asObject());
      break;
    case TYPE_OBJECT_ARRAY:
      result = a->GetDefaultValue (&value.asObjectArray());
      break;
    default:
      return false;
    }

    return result;
  }

  return false;
}

ATOM_KERNEL_API bool ATOM_CALL ATOM_SetAttribValue (ATOM_Object *object, const char *attrib, const ATOM_ScriptVar &value) {
  ATOM_STACK_TRACE(ATOM_SetAttribValue);
  if (object && ATOM_KernelServer::getInstance())
  {
    ATOM_AttribAccessorBase *a = ATOM_KernelServer::getInstance()->GetFactory()->FindAttrib (object->getClassName(), attrib);

    if (!a)
      return false;

    if (a->attrib.type != value.getType())
    {
      switch (a->attrib.type)
      {
      case TYPE_INT:
        {
          int i = value.getInteger();
          a->Set(object, &i);
          break;
        }
      case TYPE_INT_ARRAY:
        {
          ATOM_VECTOR<int> vi = value.getIntegerArray();
          a->Set(object, &vi);
          break;
        }
      case TYPE_FLOAT:
        {
          float f = value.getFloat();
          a->Set(object, &f);
          break;
        }
      case TYPE_FLOAT_ARRAY:
        {
          ATOM_VECTOR<float> vf = value.getFloatArray();
          a->Set(object, &vf);
          break;
        }
      case TYPE_STRING:
        {
          ATOM_STRING s = value.getString();
          a->Set(object, &s);
          break;
        }
      case TYPE_STRING_ARRAY:
        {
          ATOM_VECTOR<ATOM_STRING> vs = value.getStringArray();
          a->Set(object, &vs);
          break;
        }
	  case TYPE_VEC2:
		{
		  ATOM_Vector2f v2 = value.getVector2();
		  a->Set(object, &v2);
		  break;
		}
      case TYPE_VEC3:
        {
          ATOM_Vector3f v3 = value.getVector3();
          a->Set(object, &v3);
          break;
        }
      case TYPE_VEC2_ARRAY:
        {
          ATOM_VECTOR<ATOM_Vector2f> vv2 = value.getVector2Array();
          a->Set(object, &vv2);
          break;
        }
      case TYPE_VEC3_ARRAY:
        {
          ATOM_VECTOR<ATOM_Vector3f> vv3 = value.getVector3Array();
          a->Set(object, &vv3);
          break;
        }
      case TYPE_VEC4:
        {
          ATOM_Vector4f v4 = value.getVector4();
          a->Set(object, &v4);
          break;
        }
      case TYPE_VEC4_ARRAY:
        {
          ATOM_VECTOR<ATOM_Vector4f> vv4 = value.getVector4Array();
          a->Set(object, &vv4);
          break;
        }
      case TYPE_MAT33:
        {
          ATOM_Matrix3x3f m33 = value.getMatrix33();
          a->Set(object, &m33);
          break;
        }
      case TYPE_MAT33_ARRAY:
        {
          ATOM_VECTOR<ATOM_Matrix3x3f> vm33 = value.getMatrix33Array();
          a->Set(object, &vm33);
          break;
        }
      case TYPE_MAT44:
        {
          ATOM_Matrix4x4f m44 = value.getMatrix44();
          a->Set(object, &m44);
          break;
        }
      case TYPE_MAT44_ARRAY:
        {
          ATOM_VECTOR<ATOM_Matrix4x4f> vm44 = value.getMatrix44Array();
          a->Set(object, &vm44);
          break;
        }
      case TYPE_OBJECT:
        {
          ATOM_AUTOREF(ATOM_Object) o = value.getObject();
          a->Set(object, &o);
          break;
        }
      case TYPE_OBJECT_ARRAY:
        {
          ATOM_VECTOR< ATOM_AUTOREF(ATOM_Object) > vo = value.getObjectArray();
          a->Set(object, &vo);
          break;
        }
      default:
        return false;
      }
    }
    else
    {
      switch (a->attrib.type)
      {
      case TYPE_INT:
        a->Set(object, &value.asInteger());
        break;
      case TYPE_INT_ARRAY:
        a->Set(object, &value.asIntegerArray());
        break;
      case TYPE_FLOAT:
        a->Set(object, &value.asFloat());
        break;
      case TYPE_FLOAT_ARRAY:
        a->Set(object, &value.asFloatArray());
        break;
      case TYPE_STRING:
        a->Set(object, &value.asString());
        break;
      case TYPE_STRING_ARRAY:
        a->Set(object, &value.asStringArray());
        break;
	  case TYPE_VEC2:
	    a->Set(object, &value.asVector2());
		break;
      case TYPE_VEC3:
        a->Set(object, &value.asVector3());
        break;
      case TYPE_VEC2_ARRAY:
        a->Set(object, &value.asVector2Array());
        break;
      case TYPE_VEC3_ARRAY:
        a->Set(object, &value.asVector3Array());
        break;
      case TYPE_VEC4:
        a->Set(object, &value.asVector4());
        break;
      case TYPE_VEC4_ARRAY:
        a->Set(object, &value.asVector4Array());
        break;
      case TYPE_MAT33:
        a->Set(object, &value.asMatrix33());
        break;
      case TYPE_MAT33_ARRAY:
        a->Set(object, &value.asMatrix33Array());
        break;
      case TYPE_MAT44:
        a->Set(object, &value.asMatrix44());
        break;
      case TYPE_MAT44_ARRAY:
        a->Set(object, &value.asMatrix44Array());
        break;
      case TYPE_OBJECT:
        a->Set(object, &value.asObject());
        break;
      case TYPE_OBJECT_ARRAY:
        a->Set(object, &value.asObjectArray());
        break;
      default:
        return false;
      }
    }

    return true;
  }

  return false;
}

ATOM_KERNEL_API bool ATOM_CALL ATOM_GetAttribValue (ATOM_Object *object, const char *attrib, ATOM_ScriptVar &value) {
  ATOM_STACK_TRACE(ATOM_GetAttribValue);
  if (object && ATOM_KernelServer::getInstance())
  {
    ATOM_AttribAccessorBase *a = ATOM_KernelServer::getInstance()->GetFactory()->FindAttrib (object->getClassName(), attrib);

    if (!a)
      return false;

    value.setType (a->attrib.type);

    switch (a->attrib.type)
    {
    case TYPE_INT:
      a->Get(object, &value.asInteger());
      break;
    case TYPE_INT_ARRAY:
      a->Get(object, &value.asIntegerArray());
      break;
    case TYPE_FLOAT:
      a->Get(object, &value.asFloat());
      break;
    case TYPE_FLOAT_ARRAY:
      a->Get(object, &value.asFloatArray());
      break;
    case TYPE_STRING:
      a->Get(object, &value.asString());
      break;
    case TYPE_STRING_ARRAY:
      a->Get(object, &value.asStringArray());
      break;
	case TYPE_VEC2:
	  a->Get(object, &value.asVector2());
	  break;
    case TYPE_VEC3:
      a->Get(object, &value.asVector3());
      break;
    case TYPE_VEC2_ARRAY:
      a->Get(object, &value.asVector2Array());
      break;
    case TYPE_VEC3_ARRAY:
      a->Get(object, &value.asVector3Array());
      break;
    case TYPE_VEC4:
      a->Get(object, &value.asVector4());
      break;
    case TYPE_VEC4_ARRAY:
      a->Get(object, &value.asVector4Array());
      break;
    case TYPE_MAT33:
      a->Get(object, &value.asMatrix33());
      break;
    case TYPE_MAT33_ARRAY:
      a->Get(object, &value.asMatrix33Array());
      break;
    case TYPE_MAT44:
      a->Get(object, &value.asMatrix44());
      break;
    case TYPE_MAT44_ARRAY:
      a->Get(object, &value.asMatrix44Array());
      break;
    case TYPE_OBJECT:
      a->Get(object, &value.asObject());
      break;
    case TYPE_OBJECT_ARRAY:
      a->Get(object, &value.asObjectArray());
      break;
    default:
      return false;
    }

    return true;
  }

  return false;
}

ATOM_KERNEL_API unsigned ATOM_CALL ATOM_CompressBuffer(const void *input, unsigned len, void *output, unsigned level)
{
  ATOM_STACK_TRACE(ATOM_CompressBuffer);
#if 1
  return 0;
#else
  if (!input || !len)
    return 0;

  if (level < 1)
    level = 1;
  else if (level > 10)
    level = 10;

  unsigned outlen;
  int r = ucl_nrv2b_99_compress ((const ucl_bytep)input, len, (ucl_bytep)output, &outlen, 0, level, 0, 0);
  if (r == UCL_E_OK)
  {
    return outlen;
  }
  if (r == UCL_E_OUT_OF_MEMORY)
  {
    ATOM_LOGGER::error ("(ATOM_CompressBuffer): Out of memory.\n");
  }
  return 0;
#endif
}

ATOM_KERNEL_API unsigned ATOM_CALL ATOM_DecompressBuffer(const void *input, unsigned len, void *output)
{
  ATOM_STACK_TRACE(ATOM_DecompressBuffer);
#if 1
  return 0;
#else
  if (!input || !len)
    return 0;

  unsigned newlen = len;
  int r = ucl_nrv2b_decompress_8((const ucl_bytep)input, len, (ucl_bytep)output, &newlen, 0);
  if (r == UCL_E_OK)
    return newlen;

  ATOM_LOGGER::error ("(ATOM_DecompressBuffer): Decompress failed.\n");
  return 0;
#endif
}

ATOM_KERNEL_API unsigned ATOM_CALL ATOM_CreateFactorySnapshot(void)
{
  ATOM_STACK_TRACE(ATOM_CreateFactorySnapshot);
  if (ATOM_KernelServer::getInstance())
  {
    return ATOM_KernelServer::getInstance()->GetFactory()->CreateSnapshot ();
  }
  return unsigned(-1);
}

ATOM_KERNEL_API void ATOM_CALL ATOM_DumpSnapshotDifference(unsigned snapshot1, unsigned snapshot2)
{
  ATOM_STACK_TRACE(ATOM_DumpSnapshotDifference);
  if (ATOM_KernelServer::getInstance())
  {
    ATOM_KernelServer::getInstance()->GetFactory()->CompareSnapshot (snapshot1, snapshot2);
  }
}

ATOM_KERNEL_API unsigned ATOM_CALL ATOM_GetTick (void)
{
	return unsigned(::GetTickCount());
}

ATOM_KERNEL_API void ATOM_SetVFSLoadCallback (VFSLoadCallback callback, void *userdata)
{
	_VFSLoadCallback = callback;
	_VFSLoadCallbackUserData = userdata;
}

ATOM_KERNEL_API ATOM_VFS * ATOM_CALL ATOM_LoadVFS (const char *filename, const char *password)
{
	ATOM_STACK_TRACE(ATOM_LoadVFS);
	if (!filename)
	{
		return 0;
	}

	ATOM_VFS *result = 0;

	if (_VFSLoadCallback)
	{
		result = _VFSLoadCallback (filename, password, _VFSLoadCallbackUserData);
	}

	if (!result)
	{
		ATOM_ZipVFS *zipVfs = ATOM_NEW(ATOM_ZipVFS);
		if (zipVfs->load (filename, password))
		{
			result = zipVfs;
		}
		else
		{
			ATOM_DELETE(zipVfs);
		}
	}

	return result;
}

ATOM_KERNEL_API bool ATOM_SaveObject(ATOM_Object *object, const char *filename)
{
	ATOM_STACK_TRACE(ATOM_SaveObject);
	if (!object)
	{
		return false;
	}

	if (!filename)
	{
		return false;
	}

	char buffer[ATOM_VFS::max_filename_length];
	if (!ATOM_GetNativePathName (filename, buffer))
	{
		return false;
	}
	ATOM_TiXmlDocument doc(buffer);

	ATOM_TiXmlDeclaration eDecl("1.0", "gb2312", "");
	doc.InsertEndChild (eDecl);

	ATOM_TiXmlElement eRoot("Object");
	if (!object->writeAttribute (&eRoot))
	{
		return false;
	}

	doc.InsertEndChild (eRoot);

	return doc.SaveFile ();
}

ATOM_KERNEL_API ATOM_AUTOREF(ATOM_Object) ATOM_CALL ATOM_LoadObject(const char *filename, const char* objectname)
{
	ATOM_STACK_TRACE(ATOM_LoadObject);
	ATOM_AutoFile f(filename, ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
	{
		return 0;
	}
	unsigned size = f->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = f->read (p, size);
	p[size] = '\0';

	ATOM_TiXmlDocument doc;
	doc.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (doc.Error())
	{
		return 0;
	}

	ATOM_TiXmlElement *root = doc.RootElement ();
	if (!root)
	{
		return 0;
	}

	const char *classname = root->Attribute ("class");
	if (!classname)
	{
		return 0;
	}

	ATOM_AUTOREF(ATOM_Object) obj = ATOM_CreateObject (classname, objectname);

	if (obj && obj->loadAttribute (root))
	{
		return obj;
	}

	return 0;
}

ATOM_KERNEL_API void ATOM_CALL ATOM_GetCommandLine (ATOM_VECTOR<ATOM_STRING> &args)
{
	ATOM_STACK_TRACE(ATOM_GetCommandLine);
    PCHAR* argv;
    PCHAR  _argv;
    ULONG   len;
    ULONG   argc;
    CHAR   a;
    ULONG   i, j;

    BOOLEAN  in_QM;
    BOOLEAN  in_TEXT;
    BOOLEAN  in_SPACE;

	const char *CmdLine = ::GetCommandLineA ();

    len = strlen(CmdLine);
    i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);

    argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
        i + (len+2)*sizeof(CHAR));

    _argv = (PCHAR)(((PUCHAR)argv)+i);

    argc = 0;
    argv[argc] = _argv;
    in_QM = FALSE;
    in_TEXT = FALSE;
    in_SPACE = TRUE;
    i = 0;
    j = 0;

    while( a = CmdLine[i] ) {
        if(in_QM) {
            if(a == '\"') {
                in_QM = FALSE;
            } else {
                _argv[j] = a;
                j++;
            }
        } else {
            switch(a) {
            case '\"':
                in_QM = TRUE;
                in_TEXT = TRUE;
                if(in_SPACE) {
                    argv[argc] = _argv+j;
                    argc++;
                }
                in_SPACE = FALSE;
                break;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                if(in_TEXT) {
                    _argv[j] = '\0';
                    j++;
                }
                in_TEXT = FALSE;
                in_SPACE = TRUE;
                break;
            default:
                in_TEXT = TRUE;
                if(in_SPACE) {
                    argv[argc] = _argv+j;
                    argc++;
                }
                _argv[j] = a;
                j++;
                in_SPACE = FALSE;
                break;
            }
        }
        i++;
    }
    _argv[j] = '\0';
    argv[argc] = NULL;

	args.resize (argc);
	for (unsigned i = 0; i < argc; ++i)
	{
		args[i] = argv[i];
	}

	::GlobalFree (argv);
}

typedef struct
{
  void *(*Alloc)(void *p, size_t size);
  void (*Free)(void *p, void *address); /* address can be 0 */
} ISzAlloc;

static void *AllocFunc (void *p, size_t size)
{
	return ATOM_MALLOC(size);
}

static void FreeFunc (void *p, void *address)
{
	ATOM_FREE(address);
}

static crnlib::Byte cprops[LZMA_PROPS_SIZE];
static crnlib::SizeT sizeProps = LZMA_PROPS_SIZE;

struct CompressHeader
{
	unsigned compressedSize;
	unsigned uncompressedSize;
	crnlib::Byte props[LZMA_PROPS_SIZE];
};

ATOM_KERNEL_API unsigned ATOM_CALL ATOM_LZMAGetMaxCompressedSize (unsigned srcLen)
{
	return srcLen + ATOM_max2(128, srcLen >> 8) + sizeof(CompressHeader);
}

ATOM_KERNEL_API unsigned ATOM_CALL ATOM_LZMACompress (const void *src, unsigned srcLen, void *dest, unsigned destLen)
{
	CompressHeader *header = (CompressHeader*)dest;
	crnlib::Byte *dst = ((crnlib::Byte*)dest)+sizeof(CompressHeader);
	crnlib::SizeT dstLen = destLen - sizeof(CompressHeader);

	crnlib::ISzAlloc alloc = { &AllocFunc, &FreeFunc };
	crnlib::CLzmaEncProps props;
	crnlib::LzmaEncProps_Init (&props);
	props.level = 9; // 0 <= level <= 9 default 5
	crnlib::SizeT propSize = LZMA_PROPS_SIZE;

	if (SZ_OK != crnlib::LzmaEncode (dst, &dstLen, (const crnlib::Byte*)src, srcLen, &props, header->props, &propSize, 0, 0, &alloc, &alloc))
	{
		return 0;
	}

	header->compressedSize = dstLen;
	header->uncompressedSize = srcLen;

	return header->compressedSize + sizeof(CompressHeader);
}

ATOM_KERNEL_API unsigned ATOM_CALL ATOM_LZMADecompress (const void *src, unsigned srcLen, void *dest, unsigned destLen)
{
	crnlib::ISzAlloc alloc = { &AllocFunc, &FreeFunc };
	CompressHeader *header = (CompressHeader*)src;
	const crnlib::Byte *srcData = ((const crnlib::Byte*)src)+sizeof(CompressHeader);
	crnlib::SizeT srcDataLen = srcLen - sizeof(CompressHeader);

	crnlib::SizeT dstLen = destLen;
	crnlib::SizeT sourceLen = srcLen;
	crnlib::ELzmaStatus status;
	LzmaDecode((crnlib::Byte*)dest, &dstLen, srcData, &srcDataLen, header->props, LZMA_PROPS_SIZE, crnlib::LZMA_FINISH_ANY, &status, &alloc);

	return dstLen;
}

ATOM_KERNEL_API ATOM_WSTRING ATOM_CALL ATOM_Ansi2Wide (const char *str, size_t len)
{
	int sz = (len+1)*sizeof(wchar_t);
	wchar_t *p = (wchar_t*)ATOM_STACK_ALLOC(sz);
	memset(p, 0, sz);
	int n = MultiByteToWideChar(CP_ACP, 0, str, len, p, len+1);
	ATOM_WSTRING ws(p);
	ATOM_STACK_FREE(p, sz);

	return ws;
}

ATOM_KERNEL_API ATOM_STRING ATOM_CALL ATOM_Wide2Ansi (const wchar_t *str, size_t len)
{
	int sz = (wcslen(str)+1)*sizeof(wchar_t);
	char *p = (char*)ATOM_STACK_ALLOC(sz);
	memset(p, 0, sz);

	WideCharToMultiByte(CP_ACP,0, str, len, p, sz, NULL, NULL );
	ATOM_STRING s(p);
	ATOM_STACK_FREE(p, sz);

	return s;
}

static unsigned int crc_32_tab[256];
static bool bInit = false;

static void InitCrc32MakeTable(void)
{
	unsigned int h = 1;
	memset(crc_32_tab,0,sizeof(crc_32_tab));

	for ( int i = 128; i; i >>= 1)
	{
		h = (h >> 1) ^ ((h & 1) ? 0xedb88320L : 0);

		for ( int j = 0; j < 256; j += 2 *i)
		{
			crc_32_tab[i+j] = crc_32_tab[j] ^ h;
		}
	}
}

ATOM_KERNEL_API int ATOM_CALL ATOM_CalcCRC32 (int lastCRC32, const void *data, unsigned size)
{
	if (!bInit)
	{
		InitCrc32MakeTable ();
		bInit = true;
	}

	if ( !data || size==0)
	{
		return lastCRC32;
	}

	const char *buf = (const char*)data;
	unsigned crc = (unsigned)lastCRC32;
	crc ^= 0xffffffffL;

	while ( size--)
	{
		crc = (crc >> 8) ^ crc_32_tab[(crc ^ *buf++) & 0xff];
	}

	return crc ^ 0xffffffffL;
}

