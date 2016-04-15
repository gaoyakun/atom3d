#include "stdafx.h"
#include "kernelserver.h"

ATOM_ObjectFactory::ATOM_ObjectFactory (void)
{
	ATOM_STACK_TRACE(ATOM_ObjectFactory::ATOM_ObjectFactory);

	_M_enable_cache = false;
}

ATOM_ObjectFactory::~ATOM_ObjectFactory (void) 
{
	ATOM_STACK_TRACE(ATOM_ObjectFactory::~ATOM_ObjectFactory);
}

bool ATOM_ObjectFactory::ObjectTypeRegistered(const char* classname) const 
{
	ATOM_STACK_TRACE(ATOM_ObjectFactory::ObjectTypeRegistered);

	return classname && _M_creator_dict.find(classname) != _M_creator_dict.end();
}

void ATOM_ObjectFactory::EnableObjectCache(const char* classname, bool enable) 
{
	if (classname)
	{
		ObjectManagementStructDictIter iter = _M_creator_dict.find(classname);
		if ( iter != _M_creator_dict.end())
		{
			iter->second.enable_cache = enable;
		}
	}
}

bool ATOM_ObjectFactory::ObjectCacheEnabled(const char* classname) const 
{
	if (classname)
	{
		ObjectManagementStructDictConstIter iter = _M_creator_dict.find(classname);
		return iter != _M_creator_dict.end() ? iter->second.enable_cache : false;
	}
	return false;
}

void ATOM_ObjectFactory::RegisterObjectType(const char* classname, CreationFunc cf, DeletionFunc df, PurgeFunc pf, ATOM_ScriptInterfaceBase *si, bool cache) 
{
	ATOM_STACK_TRACE(ATOM_ObjectFactory::RegisterObjectType);

	if ( classname && cf && df && pf && si && _M_creator_dict.find(classname) == _M_creator_dict.end())
	{
		_M_creator_dict[classname] = ObjectManagementStruct(cf, df, pf, si, cache);
	}
}

void ATOM_ObjectFactory::UnregisterObjectType(const char* classname) 
{
	ATOM_STACK_TRACE(ATOM_ObjectFactory::UnregisterObjectType);

	if ( classname == 0)
		return;

	ObjectManagementStructDictIter iter = _M_creator_dict.find(classname);
	if ( iter != _M_creator_dict.end())
	{
		// Release all instances of this class
		bool finished = false;
		while ( !finished)
		{
			finished = true;

			for ( ObjectInstanceSetIter it = _M_instance_set.begin(); it != _M_instance_set.end(); ++it)
			{
				ATOM_ASSERT(*it);
				ATOM_ASSERT((*it)->getClassName());
				if ( !strcmp((*it)->getClassName(), classname))
				{
					if ((*it)->getObjectName())
					{
						// Remove it from dictionary
						ObjectManagementStruct::ObjectInstanceDictIter dict_iter = iter->second.instance_dict.find((*it)->getObjectName());
						ATOM_ASSERT(dict_iter != iter->second.instance_dict.end());
						iter->second.instance_dict.erase(dict_iter);
					}
					//iter->second.purge_func(*it);

					// Remove it from instance set
					_M_instance_set.erase(it);

					finished = false;
					break;
				}
			}
		}

		iter->second.script_interface->cleanup ();

		_M_creator_dict.erase(iter);
	}
}

ATOM_AUTOREF(ATOM_Object) ATOM_ObjectFactory::LookupOrCreateObject(const char *classname, const char *objectname, bool *createNew) {
	ATOM_STACK_TRACE(ATOM_ObjectFactory::LookupOrCreateObject);

	if ( objectname == 0 || !objectname[0])
	{
		if (createNew)
		{
			*createNew = true;
		}
		return CreateObject(classname, 0);
	}
	else
	{
		ATOM_AUTOREF(ATOM_Object) pObj;
		ObjectManagementStructDictIter iter = _M_creator_dict.find(classname);

		if ( iter != _M_creator_dict.end())
		{
			ObjectManagementStruct::ObjectInstanceDictConstIter it = iter->second.instance_dict.find(objectname);
			if( it != iter->second.instance_dict.end()) 
			{
				pObj = it->second;
			}

			if (pObj)
			{
				if (createNew)
				{
					*createNew = false;
				}
				return pObj;
			}

			if (createNew)
			{
				*createNew = true;
			}

			pObj = iter->second.creation_func();

			if (!pObj)
			{
				return 0;
			}

			// Insert into instance set
			_M_instance_set.insert(pObj.get());

			// Set object name
			pObj->setObjectName(objectname);

			// Add to dictionary for searching
			iter->second.instance_dict[pObj->getObjectName()] = pObj.get();

			// Set class name
			pObj->setClassName(iter->first.c_str());

			iter->second.num_objects++;

		}

		return pObj;
	}
}

ATOM_AUTOREF(ATOM_Object) ATOM_ObjectFactory::LookupObject(const char *classname, const char* objectname) const 
{
	ATOM_STACK_TRACE(ATOM_ObjectFactory::LookupObject);

	if ( objectname && objectname[0])
	{
		ObjectManagementStructDictConstIter iter = _M_creator_dict.find(classname);
		if ( iter != _M_creator_dict.end())
		{
			ObjectManagementStruct::ObjectInstanceDictConstIter it = iter->second.instance_dict.find(objectname);
			if (it != iter->second.instance_dict.end()) 
			{
				return it->second;
			}
		}
	}

	return 0;
}

ATOM_AUTOREF(ATOM_Object) ATOM_ObjectFactory::CreateObject(const char* classname, const char* objectname) 
{
	ATOM_STACK_TRACE(ATOM_ObjectFactory::CreateObject);

	// Invalid parameter
	if ( classname == 0)
		return 0;

	ATOM_AUTOREF(ATOM_Object) obj;

	ObjectManagementStructDictIter iter = _M_creator_dict.find(classname);
	if ( iter == _M_creator_dict.end())
	{
		return 0;
	}

	if ( objectname && objectname[0])
	{
		// If a name was given, see if it has been created already
		ObjectManagementStruct::ObjectInstanceDictConstIter it = iter->second.instance_dict.find(objectname);

		if ( it != iter->second.instance_dict.end())
		{
			// The object with this name exists, creation failed.
			return 0;
		}
	}

	obj = iter->second.creation_func();
	if ( !obj)
	{
		return obj;
	}

	// Insert into instance set
	_M_instance_set.insert(obj.get());

	// Set class name
	obj->setClassName(iter->first.c_str());

	// Set object name
	if (objectname && objectname[0])
	{
		// Named object should be stored in the dictionary for searching.
		obj->setObjectName(objectname);
		iter->second.instance_dict[obj->getObjectName()] = obj.get();
	}
	else
	{
		obj->setObjectName(0);
	}

	iter->second.num_objects++;

	return obj;
}

void ATOM_ObjectFactory::ReleaseObject(ATOM_Object* object) 
{
	ATOM_STACK_TRACE(ATOM_ObjectFactory::ReleaseObject);

	if ( object == 0)
	{
		return;
	}

#if 1
	ATOM_ASSERT(*((void**)object) != NULL);
#else
	//FIXME: check for vtable
	void *vtble = *((void**)object);
	if (!vtble)
	{
		ATOM_LOGGER::error ("%s(0x%08X) %s\n", __FUNCTION__, object, "Attempt to release object, but it has already been freed.");
		return;
	}
#endif

	int ref = object->decRef ();
	ATOM_ASSERT (ref >= 0);
	if (ref != 0)
	{
		return;
	}

	const char *className = object->getClassName();
	if (!className)
	{
		ATOM_LOGGER::error ("%s(0x%08X) %s\n", __FUNCTION__, object, "Attempt to release object with NULL class name");
		return;
	}

	ObjectManagementStructDictIter oms_iter = _M_creator_dict.find(className);
	if (oms_iter == _M_creator_dict.end())
	{
		ATOM_LOGGER::fatal ("ATOM_ObjectFactory::ReleaseObject:  Unknown class \"%s\"\n", className);
		return;
	}

	if (oms_iter->second.enable_cache)
	{
		return;
	}

	ObjectInstanceSetIter set_iter = _M_instance_set.find(object);
	if ( set_iter == _M_instance_set.end())
	{
		ATOM_LOGGER::fatal ("ATOM_ObjectFactory::ReleaseObject: Attempt to release an non-exist object: 0x%08X\n", object);
		return;
	}

	_M_instance_set.erase(set_iter);

	if (object->getObjectName())
	{
		// Remove it from dictionary
		ObjectManagementStruct::ObjectInstanceDictIter dict_iter = oms_iter->second.instance_dict.find(object->getObjectName());
		ATOM_ASSERT(dict_iter != oms_iter->second.instance_dict.end());
		oms_iter->second.instance_dict.erase(dict_iter);
	}

	oms_iter->second.deletion_func(object);

	oms_iter->second.num_objects--;
}

void ATOM_ObjectFactory::DestroyObject(ATOM_Object* object) 
{
	ATOM_STACK_TRACE(ATOM_ObjectFactory::DestroyObject);

	if ( object == 0)
		return;

	ObjectManagementStructDictIter oms_iter;

	{
		ObjectInstanceSetIter set_iter = _M_instance_set.find(object);
		if ( set_iter == _M_instance_set.end())
		{
			ATOM_LOGGER::fatal ("ATOM_ObjectFactory::DestroyObject: Attempt to destroy an non-exist object: 0x%08X\n", object);
			return;
		}

		// Remove it from instance set
		_M_instance_set.erase(set_iter);

		const char *className = object->getClassName();
		if (!className)
		{
			ATOM_LOGGER::error ("%s(0x%08X) %s\n", __FUNCTION__, object, "Attempt to destroy object with NULL class name");
			return;
		}
		
		oms_iter = _M_creator_dict.find(className);
		ATOM_ASSERT(oms_iter != _M_creator_dict.end());

		if (object->getObjectName())
		{
			// Remove it from dictionary
			ObjectManagementStruct::ObjectInstanceDictIter dict_iter = oms_iter->second.instance_dict.find(object->getObjectName());
			ATOM_ASSERT(dict_iter != oms_iter->second.instance_dict.end());
			oms_iter->second.instance_dict.erase(dict_iter);
		}
	}

	oms_iter->second.deletion_func(object);

	oms_iter->second.num_objects--;
}

ATOM_ObjectFactory::ObjectManagementStructDictIter ATOM_ObjectFactory::GetOMS(const char* classname) 
{
	return _M_creator_dict.find(classname);
}

ATOM_ObjectFactory::ObjectManagementStructDictConstIter ATOM_ObjectFactory::GetOMS(const char* classname) const 
{
	return _M_creator_dict.find(classname);
}

bool ATOM_ObjectFactory::OMSExists(const char* classname) const 
{
	return _M_creator_dict.find(classname) != _M_creator_dict.end();
}

bool ATOM_ObjectFactory::ObjectExists(ATOM_Object* obj) const 
{
	return _M_instance_set.find(obj) != _M_instance_set.end();
}

bool ATOM_ObjectFactory::ObjectExists(const char *classname, const char* objname) const 
{
	return LookupObject (classname, objname) != 0;
}

namespace 
{
	struct _ObjEntry 
	{
		ATOM_Object *obj;
		DeletionFunc delfunc;
	};
}

unsigned ATOM_ObjectFactory::GetAttribCount (const char *classname) 
{
	ATOM_ObjectFactory::ObjectManagementStructDictIter it = GetOMS (classname);
	if (it != _M_creator_dict.end())
	{
		ATOM_ASSERT (it->second.script_interface);
		return it->second.script_interface->getAttribCount ();
	}
	return 0;
}

ATOM_AttribAccessorBase *ATOM_ObjectFactory::GetAttrib (const char *classname, unsigned index) 
{
	ATOM_ObjectFactory::ObjectManagementStructDictIter it = GetOMS (classname);
	if (it != _M_creator_dict.end())
	{
		ATOM_ASSERT (it->second.script_interface);
		return it->second.script_interface->getAttrib (index);
	}
	return 0;
}

ATOM_AttribAccessorBase *ATOM_ObjectFactory::FindAttrib (const char *classname, const char *attrib) 
{
	ATOM_ObjectFactory::ObjectManagementStructDictIter it = GetOMS (classname);
	if (it != _M_creator_dict.end())
	{
		ATOM_ASSERT (it->second.script_interface);
		return it->second.script_interface->findAttrib (attrib);
	}
	return 0;
}

void ATOM_ObjectFactory::DumpObjects (const char *filename) 
{
	if (filename && filename[0])
	{
		ATOM_AutoFile f(filename, ATOM_VFS::write|ATOM_VFS::text);
		if (f)
		{
			f->printf("-- Dump kernel objects --\n", _M_instance_set.size());
			f->printf("Total number: %u\n", _M_instance_set.size());
			for (ATOM_ObjectFactory::ObjectManagementStructDictIter it = _M_creator_dict.begin(); it != _M_creator_dict.end(); ++it)
			{
				const ObjectManagementStruct &oms = it->second;
				int num = oms.num_objects;
				if (num != 0)
				{
					f->printf ("%s: %d\n", it->first.c_str(), num);
				}
			}
		}
	}
	else
	{
		ATOM_LOGGER::log("-- Dump kernel objects --\n", _M_instance_set.size());
		ATOM_LOGGER::log("Total number: %u\n", _M_instance_set.size());
		for (ATOM_ObjectFactory::ObjectManagementStructDictIter it = _M_creator_dict.begin(); it != _M_creator_dict.end(); ++it)
		{
			const ObjectManagementStruct &oms = it->second;
			int num = oms.num_objects;
			if (num != 0)
			{
				ATOM_LOGGER::log ("%s: %d\n", it->first.c_str(), num);
			}
		}
	}
}

void ATOM_ObjectFactory::DumpObjectsNoSort (const char *indent, bool dumpObjectDetails) 
{
	if (!indent)
	{
		indent = "";
	}

	ATOM_LOGGER::log ("%sThere are %u objects.\n", indent, _M_instance_set.size());
	unsigned i = 0;


	for (ObjectInstanceSetConstIter it = _M_instance_set.begin(); it != _M_instance_set.end(); ++it, ++i)
	{
		ATOM_LOGGER::log ("%s\tobject %u (%s, %s)\n", indent, i, (*it)->getClassName(), (*it)->getObjectName());

		if (dumpObjectDetails)
		{
			ATOM_STRING s = indent;
			s += "\t\t";
			(*it)->dumpAttributes (s.c_str());
		}
	}
}

int _SortName (const void *p1, const void *p2)
{
	ATOM_Object *o1 = *((ATOM_Object**)p1);
	ATOM_Object *o2 = *((ATOM_Object**)p2);
	const char *name1 = o1 ? o1->getObjectName() : 0;
	const char *name2 = o2 ? o2->getObjectName() : 0;
	if (name1 == 0) name1 = "";
	if (name2 == 0) name2 = "";
	return strcmp (name1, name2);
}

int _SortClassName (const void *p1, const void *p2)
{
	ATOM_Object *o1 = *((ATOM_Object**)p1);
	ATOM_Object *o2 = *((ATOM_Object**)p2);
	const char *name1 = o1 ? o1->getClassName() : 0;
	const char *name2 = o2 ? o2->getClassName() : 0;
	if (name1 == 0) name1 = "";
	if (name2 == 0) name2 = "";
	return strcmp (name1, name2);
}

void ATOM_ObjectFactory::DumpObjectsSortName (const char *indent, bool dumpObjectDetails) 
{
	ATOM_LOGGER::log ("%sThere are %u objects.\n", indent, _M_instance_set.size());
	unsigned i = 0;

	ATOM_VECTOR<ATOM_Object*> temp(_M_instance_set.size());
	for (ObjectInstanceSetConstIter it = _M_instance_set.begin(); it != _M_instance_set.end(); ++it, ++i)
	{
		temp[i] = *it;
	}

	qsort (&temp[0], temp.size(), sizeof(temp[0]), &_SortName);

	for (i = 0; i < temp.size(); ++i)
	{
		ATOM_LOGGER::log ("%s\tobject %u (%s, %s)\n", indent, i, temp[i]->getClassName(), temp[i]->getObjectName());

		if (dumpObjectDetails)
		{
			ATOM_STRING s = indent;
			s += "\t\t";
			temp[i]->dumpAttributes (s.c_str());
		}
	}
}

void ATOM_ObjectFactory::DumpObjectsSortClassName (const char *indent, bool dumpObjectDetails) 
{
	ATOM_LOGGER::log ("%sThere are %u objects.\n", indent, _M_instance_set.size());
	unsigned i = 0;

	ATOM_VECTOR<ATOM_Object*> temp(_M_instance_set.size());
	for (ObjectInstanceSetConstIter it = _M_instance_set.begin(); it != _M_instance_set.end(); ++it, ++i)
	{
		temp[i] = *it;
	}

	qsort (&temp[0], temp.size(), sizeof(temp[0]), &_SortClassName);

	for (i = 0; i < temp.size(); ++i)
	{
		ATOM_LOGGER::log ("%s\tobject %u (%s, %s)\n", indent, i, temp[i]->getClassName(), temp[i]->getObjectName());

		if (dumpObjectDetails)
		{
			ATOM_STRING s = indent;
			s += "\t\t";
			temp[i]->dumpAttributes (s.c_str());
		}
	}
}

unsigned ATOM_ObjectFactory::GetObjectCount (const char *classname)
{
	if (!classname || !classname[0])
	{
		return _M_instance_set.size();
	}
	else
	{
		ObjectManagementStructDictConstIter it = _M_creator_dict.find (classname);
		if (it != _M_creator_dict.end())
		{
			return it->second.num_objects;
		}
	}

	return 0;
}

struct Snapshot
{
	bool used;
	std::vector<std::pair<const char*, int> > infos;
};

static std::vector<Snapshot> snapshots;

unsigned ATOM_ObjectFactory::CreateSnapshot (void)
{
	Snapshot *ps = 0;
	unsigned ret = 0;

	for (unsigned i = 0; i < snapshots.size(); ++i)
	{
		if (!snapshots[i].used)
		{
			ps = &snapshots[i];
			ret = i;
			break;
		}
	}

	if (!ps)
	{
		snapshots.resize(snapshots.size()+1);
		ps = &snapshots.back();
		ret = snapshots.size()-1;
	}

	{
		ps->used = true;
		ps->infos.reserve (_M_creator_dict.size());

		for (ObjectManagementStructDictConstIter it = _M_creator_dict.begin(); it != _M_creator_dict.end(); ++it)
		{
			int count = it->second.instance_dict.size();
			ps->infos.push_back (std::pair<const char*, int>(it->first.c_str(), count));
		}
	}

	return ret;
}

void ATOM_ObjectFactory::CompareSnapshot (unsigned snapshot1, unsigned snapshot2)
{
	if (snapshot1 < snapshots.size() && snapshots[snapshot1].used &&
	snapshot2 < snapshots.size() && snapshots[snapshot2].used)
	{
		for (unsigned i = 0; i < snapshots[snapshot1].infos.size(); ++i)
		{
			int n1 = snapshots[snapshot1].infos[i].second;
			int n2 = snapshots[snapshot2].infos[i].second;

			if (n1 != n2)
			{
				const char *classname = snapshots[snapshot1].infos[i].first;
				ATOM_LOGGER::log ("%s %+d (%d --> %d)\n", classname, n2-n1, n1, n2);
			}
		}
	}
}

void *ATOM_ObjectFactory::beginFindObject (void)
{
	return _M_instance_set.empty () ? 0 : ATOM_NEW(ObjectInstanceSetIter, _M_instance_set.begin());
}

bool ATOM_ObjectFactory::findNextObject (void *handle)
{
	if (!handle)
	{
		return false;
	}

	ObjectInstanceSetIter *iter = (ObjectInstanceSetIter*)handle;

	(*iter)++;

	if (*iter == _M_instance_set.end ())
	{
		return false;
	}

	return true;
}

void ATOM_ObjectFactory::endFindObject (void *handle)
{
	ATOM_DELETE((ObjectInstanceSetIter*)handle);
}

ATOM_Object *ATOM_ObjectFactory::getObjectFound (void *handle)
{
	if (handle)
	{
		ObjectInstanceSetIter *iter = (ObjectInstanceSetIter*)handle;

		return *(*iter);
	}

	return 0;
}


