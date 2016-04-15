/**	\file factory.h
 *	ATOM_ObjectFactory类的声明.
 *
 *	\author 高雅昆
 *	\ingroup kernel
 */
#ifndef __ATOM_KERNEL_FACTORY
#define __ATOM_KERNEL_FACTORY

#if _MSC_VER > 1000
# pragma once
#endif

#include <string>
#include <functional>
#include <ATOM_utils.h>
#include <ATOM_dbghlp.h>
#include "basedefs.h"
#include "object.h"
#include "kernel.h"
#include "thread.h"

namespace std
{
  template <>
  struct less<const char*>: public binary_function<const char*, const char*, bool>
  {
	  bool operator()(const char *_Left, const char *_Right) const
		  {
		    return strcmp(_Left, _Right) < 0;
		  }
  };
}

class ATOM_Object;
typedef ATOM_Object* (*CreationFunc) ();
typedef void (*DeletionFunc) (ATOM_Object*);
typedef void (*PurgeFunc) (ATOM_Object*);

struct ObjectManagementStruct
{
	typedef ATOM_HASHMAP<const char*, ATOM_Object*> ObjectInstanceDict;
	typedef ObjectInstanceDict::iterator ObjectInstanceDictIter;
	typedef ObjectInstanceDict::const_iterator ObjectInstanceDictConstIter;
	CreationFunc creation_func;
	DeletionFunc deletion_func;
	PurgeFunc purge_func;
	ATOM_ScriptInterfaceBase *script_interface;
	ObjectInstanceDict instance_dict;
	bool enable_cache;
	int num_objects;

	inline ObjectManagementStruct(CreationFunc cf = 0, DeletionFunc df = 0, PurgeFunc pf = 0, ATOM_ScriptInterfaceBase *si = 0, bool cache = false)
	: creation_func(cf), deletion_func(df), purge_func(pf), script_interface(si), enable_cache(cache), num_objects(0) {
	}
};

struct ATOM_AttribAccessorBase;

//! \class ATOM_ObjectFactory
//! 命名对象工厂类
class ATOM_KERNEL_API ATOM_ObjectFactory
{
	friend class ATOM_KernelServer;

	typedef ATOM_HASHMAP<ATOM_STRING, ObjectManagementStruct> ObjectManagementStructDict;
	typedef ObjectManagementStructDict::iterator ObjectManagementStructDictIter;
	typedef ObjectManagementStructDict::const_iterator ObjectManagementStructDictConstIter;
	typedef ATOM_HASHSET<ATOM_Object*> ObjectInstanceSet;
	typedef ObjectInstanceSet::iterator ObjectInstanceSetIter;
	typedef ObjectInstanceSet::const_iterator ObjectInstanceSetConstIter;

public:
	ATOM_ObjectFactory (void);
	~ATOM_ObjectFactory (void);

public:
	void RegisterObjectType(const char* classname, CreationFunc cf, DeletionFunc df, PurgeFunc pf, ATOM_ScriptInterfaceBase *si, bool cache);
	void UnregisterObjectType(const char* classname);
	bool ObjectTypeRegistered(const char* classname) const;
	void EnableObjectCache(const char* classname, bool enable);
	bool ObjectCacheEnabled(const char* classname) const;
	ATOM_AUTOREF(ATOM_Object) CreateObject(const char* classname, const char* objectname);
	ATOM_AUTOREF(ATOM_Object) LookupObject(const char* classname, const char* objectname) const;
	ATOM_AUTOREF(ATOM_Object) LookupOrCreateObject(const char *classname, const char* objectname, bool *createNew);
	void ReleaseObject(ATOM_Object* object);
	void DestroyObject(ATOM_Object* object);
	void DumpObjects (const char *filename);
	unsigned CreateSnapshot (void);
	void CompareSnapshot (unsigned snapshot1, unsigned snapshot2);
	unsigned GetObjectCount (const char *classname);
	unsigned GetAttribCount (const char *classname);
	ATOM_AttribAccessorBase *GetAttrib (const char *classname, unsigned index);
	ATOM_AttribAccessorBase *FindAttrib (const char *classname, const char *attrib);
	void *beginFindObject (void);
	bool findNextObject (void *handle);
	void endFindObject (void *handle);
	ATOM_Object *getObjectFound (void *handle);

private:	
	ObjectManagementStructDictIter GetOMS(const char* classname);
	ObjectManagementStructDictConstIter GetOMS(const char* classname) const;
	bool OMSExists(const char* classname) const;
	bool ObjectExists(ATOM_Object* obj) const;
	bool ObjectExists(const char *classname, const char* objname) const;
	void DumpObjectsNoSort (const char *indent, bool dumpObjectDetails);
	void DumpObjectsSortName (const char *indent, bool dumpObjectDetails);
	void DumpObjectsSortClassName (const char *indent, bool dumpObjectDetails);

	ObjectManagementStructDict _M_creator_dict;
	ObjectInstanceSet _M_instance_set;
	bool _M_enable_cache;
};

#endif // __ATOM_KERNEL_FACTORY
