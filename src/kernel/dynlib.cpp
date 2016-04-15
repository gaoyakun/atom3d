#include "stdafx.h"
#include "dynlib.h"

#ifdef WIN32

ATOM_DynamicLib::ATOM_DynamicLib(const char* ext): _M_lib_instance(0) 
{
	if ( ext)
	{
		_M_def_extension = ext;
	}
	else
	{
		_M_def_extension = ".dll";
	}
}

#else

ATOM_DynamicLib::ATOM_DynamicLib (void): _M_lib_instance(0) 
{
}

#endif

ATOM_DynamicLib::~ATOM_DynamicLib (void) 
{
	close();
}

const char* ATOM_DynamicLib::getFilename (void) const 
{
	return _M_lib_filename.c_str();
}

void ATOM_DynamicLib::close (void) 
{
	if ( _M_lib_instance)
	{
		dlclose(_M_lib_instance);
		_M_lib_instance = 0;
		_M_lib_filename = "";
	}
}

bool ATOM_DynamicLib::open (const char* filename) 
{
	if ( isOpened())
	{
		close();
	}

	if ( filename == 0)
	{
		ATOM_LOGGER::error ("[ATOM_DynamicLib::open] Invalid arguments: 0x%08X.\n", filename);
		return false;
	}

	ATOM_STRING name(filename);
#ifdef WIN32
	name += _M_def_extension;
#elif defined(POSIX)
	name = ATOM_STRING("lib") + name;
	name += ".so";
#else
# error Unsupport platform
#endif

	_M_lib_instance = dlopen(name.c_str(), RTLD_LAZY);
	//unsigned e =GetLastError();

	if ( _M_lib_instance)
	{
		_M_lib_filename = name;
		return true;
	}

	return false;
}

bool ATOM_DynamicLib::isOpened (void) const 
{
  return _M_lib_instance != 0;
}

void* ATOM_DynamicLib::querySymbol (const char* sym) 
{
	if ( sym == 0)
	{
		ATOM_LOGGER::error ("[ATOM_DynamicLib::QuerySymbol] Invalid arguments: 0x%08X.\n", sym);
		return 0;
	}

	if ( !isOpened())
	{
		ATOM_LOGGER::error ("[ATOM_DynamicLib::QuerySymbol] Dyamic library not opened.\n");
		return 0;
	}

	return (void*)dlsym(_M_lib_instance, sym);
}
