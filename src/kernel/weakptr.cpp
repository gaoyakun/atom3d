#include "StdAfx.h"
#include "weakptr.h"

///////////////////////////////// class ATOM_WeakPtrBase ////////////////////////////

ATOM_WeakPtrBase::ATOM_WeakPtrBase (void)
: _pointer(0)
, _prev(0)
, _next(0)
{
}

ATOM_WeakPtrBase::ATOM_WeakPtrBase (void *pointer)
: _pointer(pointer)
, _prev(0)
, _next(0)
{
}

//////////////////////////////// class ATOM_WeakPtrList //////////////////////////////

ATOM_WeakPtrList::ATOM_WeakPtrList (void)
: _head (0)
{
}

ATOM_WeakPtrList::~ATOM_WeakPtrList (void)
{
	ATOM_WeakPtrBase *p = _head;
	while (p)
	{
		ATOM_WeakPtrBase *next = p->_next;
		p->_pointer = 0;
		p->_prev = 0;
		p->_next = 0;
		p = next;
	}
}

void ATOM_WeakPtrList::insert (ATOM_WeakPtrBase *p)
{
	if (_head)
	{
		_head->_prev = p;
		p->_prev = 0;
		p->_next = _head;
	}
	else
	{
		p->_prev = 0;
		p->_next = 0;
	}
	_head = p;
}

void ATOM_WeakPtrList::remove (ATOM_WeakPtrBase *p)
{
	if (p->_prev)
	{
		p->_prev->_next = p->_next;
	}
	if (p->_next)
	{
		p->_next->_prev = p->_prev;
	}

	if (p == _head)
	{
		_head = p->_next;
	}
}

