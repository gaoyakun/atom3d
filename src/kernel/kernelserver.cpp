#include "stdafx.h"
#include "kernelserver.h"

ATOM_KernelServer* ATOM_KernelServer::getInstance(void)
{
	static ATOM_KernelServer *inst = ATOM_NEW(ATOM_KernelServer);
	return inst;
}

ATOM_KernelServer::ATOM_KernelServer()
  : _M_initialized(false)
  , _M_filesystem(0)
  , _M_factory(0) 
{
	Init (0);
}


ATOM_KernelServer::~ATOM_KernelServer() {
  Fini();
}

bool ATOM_KernelServer::Init(int reserved) {
  if ( !_M_initialized)
  {
    _M_factory = ATOM_NEW(ATOM_ObjectFactory);
#ifndef WIN32
    _M_filesystem = ATOM_NEW(ATOM_FileSystem);
#else
	_M_filesystem = 0;
#endif

#ifndef WIN32
    _M_filesystem->CreateDir("/var");
    _M_filesystem->CreateDir("/var/pool");
    _M_filesystem->CreateDir("/var/pool/objects");
    _M_filesystem->CreateDir("/var/log");
    _M_filesystem->CreateDir("/tmp");
	
	// win32的临时目录的建立在TsFilePath。cpp中
#endif
    _M_initialized = true;
    return true;
  }
  return false;
}

void ATOM_KernelServer::Fini() {
  if ( _M_initialized)
  {
#ifndef WIN32
    ATOM_DELETE(_M_filesystem);
    _M_filesystem = 0;
#endif
    ATOM_DELETE(_M_factory);
    _M_factory = 0;
  }
}	

bool ATOM_KernelServer::ObjectTypeRegistered(const char* classname) const {
  return _M_initialized && _M_factory->ObjectTypeRegistered(classname);
}

void ATOM_KernelServer::RegisterObjectType(const char* classname, CreationFunc cf, DeletionFunc df, PurgeFunc pf, ATOM_ScriptInterfaceBase *si, bool cache) {
  if ( _M_initialized)
    _M_factory->RegisterObjectType(classname, cf, df, pf, si, cache);
}

void ATOM_KernelServer::UnregisterObjectType(const char* classname) {
  if ( _M_initialized)
    _M_factory->UnregisterObjectType(classname);
}

ATOM_AUTOREF(ATOM_Object) ATOM_KernelServer::CreateObject(const char* classname, const char* objname) {
  if ( !_M_initialized)
    return 0;
  return _M_factory->CreateObject(classname, objname);
}

ATOM_AUTOREF(ATOM_Object) ATOM_KernelServer::LookupObject(const char *classname, const char* objname) {
  if ( !_M_initialized)
    return 0;
  return _M_factory->LookupObject(classname, objname);
}

void ATOM_KernelServer::ReleaseObject(ATOM_Object* object) {
  if ( _M_initialized)
    _M_factory->ReleaseObject(object);
}

void ATOM_KernelServer::DestoryObject(ATOM_Object *object)
{
	if(_M_initialized)
		_M_factory->DestroyObject(object);
}

//-----------------------------------------------------
// Function name   : ATOM_KernelServer::PushCWD
// Description     : 
// Return type     : void 
//-----------------------------------------------------
void ATOM_KernelServer::PushCWD() 
{
// TODO
}

//-----------------------------------------------------
// Function name   : ATOM_KernelServer::PopCWD
// Description     : 
// Return type     : void 
//-----------------------------------------------------
void ATOM_KernelServer::PopCWD() 
{
// TODO
}

unsigned ATOM_KernelServer::GetAttribCount (const char *classname) {
  return this->GetFactory()->GetAttribCount (classname);
}

ATOM_AttribAccessorBase* ATOM_KernelServer::GetAttrib (const char *classname, unsigned index) {
  return this->GetFactory()->GetAttrib (classname, index);
}

ATOM_AttribAccessorBase *ATOM_KernelServer::FindAttrib (const char *classname, const char *attrib) {
  return this->GetFactory()->FindAttrib (classname, attrib);
}

