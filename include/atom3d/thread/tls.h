#ifndef __ATOM_THREAD_TLS_H
#define __ATOM_THREAD_TLS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

class ATOM_THREAD_API ATOM_TlsPtr
{
public:
	ATOM_TlsPtr (void);
	~ATOM_TlsPtr (void);

private:
	ATOM_TlsPtr (const ATOM_TlsPtr&);
	void operator = (const ATOM_TlsPtr&);

public:
	void setPtr (void *ptr);
	void *getPtr (void) const;

private:
	DWORD _tlsIndex;
};

template <class T>
class ATOM_TlsData: public ATOM_TlsPtr
{
public:
	struct Dummy;

	ATOM_TlsData (void)
	{
		setPtr (0);
	}

	ATOM_TlsData<T> &operator = (T *value)
	{
		setValue (value);
		return *this;
	}

	void setValue (T *value)
	{
		setPtr (value);
	}

	T *getValue (void) const
	{
		return (T*)getPtr ();
	}

	T * operator ->(void) const
	{
		return getValue ();
	}

	T & operator *(void) const
	{
		return *getValue ();
	}

	operator Dummy * (void) const
	{
		return (Dummy*)getValue ();
	}
};

#endif // __ATOM_THREAD_TLS_H

