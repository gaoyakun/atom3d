#include "indexarray.h"

ATOM_IndexArray::ATOM_IndexArray(void) 
{
    _numIndices = 0;
    _usage = Static;
	_32bit = false;
	_contentLost = false;
}

unsigned ATOM_IndexArray::getNumIndices (void) const 
{
	return _numIndices;
}

bool ATOM_IndexArray::isContentLost (void) const
{
	return _contentLost;
}

void ATOM_IndexArray::setContentLost (bool b)
{
	_contentLost = b;
}

