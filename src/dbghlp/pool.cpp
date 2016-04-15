#include "StdAfx.h"
#include "pool.h"

#pragma pack(1)

class p_allocator;
class p_allocator_base;

typedef std::map<unsigned, p_allocator_base*> AllocatorMap;
static AllocatorMap _allocatorMap;

struct _al_instance
{
	_al_instance *next;
	destructFunc destructor;
	p_allocator_base *alloc;
};

ATOM_DBGHLP_API void ATOM_CALL ATOM_PoolGarbage (void *p)
{
	p_allocator_base *al = *(p_allocator_base**)(((char*)p) - sizeof(p_allocator*));
	al->garbage (p);
}

ATOM_DBGHLP_API void ATOM_CALL ATOM_PoolDelete (void *p)
{
	p_allocator_base *al = *(p_allocator_base**)(((char*)p) - sizeof(p_allocator*));
	al->deallocate (p);
}

ATOM_DBGHLP_API void ATOM_PoolPurge (void)
{
	for (AllocatorMap::const_iterator it = _allocatorMap.begin(); it != _allocatorMap.end(); ++it)
	{
		it->second->purge ();
	}
}

ATOM_DBGHLP_API p_allocator_base *getBaseAllocator (unsigned size)
{
	if (size == 0)
	{
		return 0;
	}

	AllocatorMap::const_iterator it = _allocatorMap.find (size);
	if (it == _allocatorMap.end ())
	{
		p_allocator_base *al = ATOM_NEW(p_allocator_base, size);
		_allocatorMap[size] = al;
		return al;
	}

	return it->second;
}


p_allocator_base::p_allocator_base (unsigned size)
{
	_freelist = 0;
	_size = size;
}

p_allocator_base::~p_allocator_base (void)
{
	purge ();
}

void *p_allocator_base::allocate (destructFunc d)
{
	_al_instance *i = 0;

	ATOM_FastMutex::ScopeMutex l(_lock);
	i = _freelist;
	if (i)
	{
		i->destructor = d;
		_freelist = i->next;
	}
	else
	{
		i = (_al_instance*)ATOM_MALLOC(sizeof(_al_instance)+_size);
		if (i)
		{
			i->destructor = d;
			i->next = 0;
			i->alloc = this;
		}
	}
	return i ? i+1 : 0;
}

void p_allocator_base::garbage (void *p)
{
	if (p)
	{
		_al_instance *i = (_al_instance*)(((char*)p) - sizeof(_al_instance));
		i->destructor (p);
		ATOM_FastMutex::ScopeMutex l(_lock);
		i->next = _freelist;
		_freelist = i;
	}
}

void p_allocator_base::deallocate (void *p)
{
	if (p)
	{
	  _al_instance *i = (_al_instance*)(((char*)p) - sizeof(_al_instance));
	  i->destructor (p);
	  ATOM_FREE(i);
	}
}

void p_allocator_base::purge (void)
{
	ATOM_FastMutex::ScopeMutex l(_lock);
	while (_freelist)
	{
	  _al_instance *tmp = _freelist->next;
	  _freelist->destructor ((char*)_freelist+sizeof(_al_instance));
	  ATOM_FREE(_freelist);
	  _freelist = tmp;
	}
}


