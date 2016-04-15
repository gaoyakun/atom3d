#include <windows.h>
#include "tls.h"

ATOM_TlsPtr::ATOM_TlsPtr (void)
{
	_tlsIndex = ::TlsAlloc ();
	::TlsSetValue (_tlsIndex, 0);
}

ATOM_TlsPtr::~ATOM_TlsPtr (void)
{
	::TlsFree (_tlsIndex);
}

void ATOM_TlsPtr::setPtr (void *ptr)
{
	::TlsSetValue (_tlsIndex, ptr);
}

void *ATOM_TlsPtr::getPtr (void) const
{
	return ::TlsGetValue (_tlsIndex);
}
