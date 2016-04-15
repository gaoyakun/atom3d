#ifndef __ATOM_KERNEL_KERNELSERVER_H
#define __ATOM_KERNEL_KERNELSERVER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <vector>
#include <stack>
#include "basedefs.h"
#include "factory.h"

class ATOM_KERNEL_API ATOM_KernelServer;
class ATOM_ObjectFactory;
class ATOM_Object;
class ATOM_FileSystem;
class ATOM_KERNEL_API ATOM_KernelServer
  {
  public:
    static ATOM_KernelServer* getInstance(void);

  public:
    ATOM_KernelServer();
    virtual ~ATOM_KernelServer();

    inline bool Initialized() const;
    inline ATOM_FileSystem* GetVFS() const;
    inline ATOM_ObjectFactory* GetFactory() const;

  public:
    bool Init(int reserved);
    void Fini();

  public:
	//inline void lock(){ _M_factory->lock(); }
	//inline void unlock(){ _M_factory->unlock(); }
    // Object instance manipulation
    void RegisterObjectType(const char* classname, CreationFunc cf, DeletionFunc df, PurgeFunc pf, ATOM_ScriptInterfaceBase *si, bool cache);
    void UnregisterObjectType(const char* classname);
    bool ObjectTypeRegistered(const char* classname) const;
    ATOM_AUTOREF(ATOM_Object) CreateObject(const char* classname, const char* objname);
    ATOM_AUTOREF(ATOM_Object) LookupObject(const char *classname, const char* objname);
    void ReleaseObject(ATOM_Object* object);	//减object ref, ref==0的时候调用DestoryObject.
		void DestoryObject(ATOM_Object *object);	//不管object的ref, 删除object.
    unsigned GetAttribCount (const char *classname);
    ATOM_AttribAccessorBase* GetAttrib (const char *classname, unsigned index);
    ATOM_AttribAccessorBase* FindAttrib(const char *classname, const char *attrib);

    // Push/Pop CWD
    void PushCWD();
    void PopCWD();

  private:
    bool _M_initialized;  
    ATOM_ObjectFactory* _M_factory;
    ATOM_FileSystem* _M_filesystem;
    ATOM_STACK<ATOM_STRING> _M_cwd_stack;
  };

inline bool ATOM_KernelServer::Initialized() const {
  return _M_initialized;
}

inline ATOM_FileSystem* ATOM_KernelServer::GetVFS() const {
  return _M_filesystem;
}

inline ATOM_ObjectFactory* ATOM_KernelServer::GetFactory() const {
  return _M_factory;
}

#endif // __ATOM_KERNEL_KERNELSERVER_H
