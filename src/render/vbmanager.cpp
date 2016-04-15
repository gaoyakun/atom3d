#include "stdafx.h"
#include "vbmanager.h"

ATOM_AUTOREF(ATOM_VertexArray) ATOM_VBManager::allocVertexArray(unsigned attributes, int usage, unsigned length, unsigned attribFlags/*=0*/)
{
	ATOM_AUTOREF(ATOM_VertexArray) va = _doAllocVertexArray (attributes, usage, attribFlags);
	if (va && va->updateData (0, length * va->getVertexStride()))
	{
		return va;
	}
	return 0;
}

