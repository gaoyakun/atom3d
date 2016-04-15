#include "stdafx.h"

#define RESCACHE_MAXCOUNT 256

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_Resource)
  ATOM_ATTRIBUTES_BEGIN(ATOM_Resource)
  ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_Resource, ATOM_Object)

ATOM_Resource::ATOM_Resource (void) 
{
}

ATOM_Resource::~ATOM_Resource(void) 
{
}

bool ATOM_Resource::loadFromFile (ATOM_File *file, void *userdata) 
{
	return readFromFile (file, userdata);
}

