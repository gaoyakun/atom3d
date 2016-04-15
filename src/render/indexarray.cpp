#include "stdafx.h"

ATOM_IndexArray::ATOM_IndexArray(void) 
{
    _M_num_indices = 0;
    _M_usage = ATOM_USAGE_STATIC;
	_M_32bit = false;
	_M_content_lost = false;
}

unsigned ATOM_IndexArray::getNumIndices (void) const 
{
	return _M_num_indices;
}

bool ATOM_IndexArray::isContentLost (void) const
{
	return _M_content_lost;
}

void ATOM_IndexArray::setContentLost (bool b)
{
	_M_content_lost = b;
}

