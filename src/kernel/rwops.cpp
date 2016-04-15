#include "stdafx.h"

ATOM_RWops::~ATOM_RWops(void) 
{
}

ATOM_RWops* ATOM_RWops::createFileRWops(const char* fn, const char* mode) 
{
	ATOM_RWopsFile* p_rf = ATOM_NEW(ATOM_RWopsFile, fn, mode);

	if ( -1 == p_rf->tell())
		if ( strstr(mode, "r"))
			if ( !strstr(mode, "r+"))
			{
				ATOM_DELETE(p_rf);
				return 0;
			}
	return p_rf;
}

ATOM_RWops* ATOM_RWops::createMemRWops(void* mem, unsigned size) 
{
	return ATOM_NEW(ATOM_RWopsMem, mem, size);
}

ATOM_RWops* ATOM_RWops::createVFSRWops(ATOM_File* file) 
{
	return ATOM_NEW(ATOM_RWopsVFS, file);
}

void ATOM_RWops::destroyRWops(ATOM_RWops* rwops) 
{
	ATOM_DELETE(rwops);
}

