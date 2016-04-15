#include "stdafx.h"
#include "ibmanager.h"

#define DEFAULT_INDEX_BUFFER_SIZE  3000

ATOM_AUTOREF(ATOM_IndexArray) ATOM_IBManager::allocIndexArray(int usage, unsigned length, bool use32bit) 
{
	ATOM_AUTOREF(ATOM_IndexArray) ia = _doAllocIndexArray (usage, length, use32bit);

	return ia.get();
}

