#ifndef __ATOM3D_ENGINE_WEAKPTR_H
#define __ATOM3D_ENGINE_WEAKPTR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

#define ATOM_DECLARE_WEAK_REF \
	private:\
		ATOM_WeakPtrList _M_weakptr_list;\
	public:\
		ATOM_WeakPtrList &getWeakPtrList (void) { return _M_weakptr_list; }

class ATOM_WeakPtrList;

class ATOM_KERNEL_API ATOM_WeakPtrBase
{
	friend class ATOM_WeakPtrList;

protected:
	void *_pointer;
	ATOM_WeakPtrBase *_prev;
	ATOM_WeakPtrBase *_next;

public:
	ATOM_WeakPtrBase (void);
	ATOM_WeakPtrBase (void *pointer);

private:
	ATOM_WeakPtrBase (const ATOM_WeakPtrBase &);
	void operator = (const ATOM_WeakPtrBase &);
};

class ATOM_KERNEL_API ATOM_WeakPtrList
{
public:
	ATOM_WeakPtrList (void);
	~ATOM_WeakPtrList (void);

private:
	ATOM_WeakPtrList (const ATOM_WeakPtrList &);
	void operator = (const ATOM_WeakPtrList &);

public:
	void insert (ATOM_WeakPtrBase *p);
	void remove (ATOM_WeakPtrBase *p);

private:
	ATOM_WeakPtrBase *_head;
};

template <class Type>
class ATOM_WeakPtrT: public ATOM_WeakPtrBase
{
public:
	typedef Type element_type;
	typedef element_type *pointer_type;
	typedef element_type &reference_type;
	typedef ATOM_WeakPtrT<Type> this_type;
    struct _Dummy;

public:
	ATOM_WeakPtrT (void)
	: ATOM_WeakPtrBase (0)
	{
	}

	ATOM_WeakPtrT (Type *p)
	: ATOM_WeakPtrBase (p)
	{
		if (p)
		{
			p->getWeakPtrList().insert (this);
		}
	}

	ATOM_WeakPtrT (const this_type &other)
		: ATOM_WeakPtrBase (other.get())
	{
		if (_pointer)
		{
			((pointer_type)_pointer)->getWeakPtrList().insert (this);
		}
	}

	~ATOM_WeakPtrT (void)
	{
		if (_pointer)
		{
			((pointer_type)_pointer)->getWeakPtrList().remove (this);
		}
	}

public:
	void swap (this_type &other)
	{
		if (_pointer != other._pointer)
		{
			if (_pointer)
			{
				((pointer_type)_pointer)->getWeakPtrList().remove (this);
			}

			if (other._pointer)
			{
				((pointer_type)other._pointer)->getWeakPtrList().remove (&other);
			}

			void *tmp = _pointer;
			_pointer = other._pointer;
			other._pointer = _pointer;

			if (_pointer)
			{
				((pointer_type)_pointer)->getWeakPtrList().insert (this);
			}

			if (other._pointer)
			{
				((pointer_type)other._pointer)->getWeakPtrList().insert (&other);
			}
		}
	}

	this_type &operator = (const this_type &other)
	{
		this_type tmp(other);
		swap (tmp);
		return *this;
	}

	pointer_type operator -> (void) const
	{
		return (pointer_type)_pointer;
	}

    reference_type operator *(void) const
	{
		return *((pointer_type)_pointer);
	}

    bool operator !(void) const
	{
		return _pointer == 0;
	}

    operator _Dummy * (void) const
	{
		return (_Dummy*)_pointer;
	}

	pointer_type get (void) const
	{
		return (pointer_type)_pointer;
	}
};

#endif // __ATOM3D_ENGINE_WEAKPTR_H
