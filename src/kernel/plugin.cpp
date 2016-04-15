#include "stdafx.h"
#include "plugin.h"
#include "kernelserver.h"

#define EXP_FCN_GETPLUGINVERSION    "ATOM_GetPluginVersion"
#define EXP_FCN_GETPLUGINDESC       "ATOM_GetPluginDescription"
#define EXP_FCN_GETPLUGINCOUNT      "ATOM_GetPluginCount"
#define EXP_FCN_GETPLUGINCLASSINFO  "ATOM_GetPluginClassInfo"
#define EXP_FCN_INIT				"ATOM_InitPlugin"
#define EXP_FCN_FINI				"ATOM_FiniPlugin"

static inline unsigned char MAJOR_VERSION(unsigned n) 
{
	return (unsigned char) ((n & 0xFF000000) >> 24);
}

static inline unsigned char MINOR_VERSION(unsigned n) 
{
	return (unsigned char) ((n & 0x00FF0000) >> 16);
}

static inline unsigned short MICRO_VERSION(unsigned n) 
{
	return (unsigned short) (n & 0x0000FFFF);
}

ATOM_Plugin::ATOM_Plugin(ATOM_KernelServer* ks)
: ATOM_DynamicLib()
, _M_kernel_server(ks)
, _M_version(0) 
{
	_M_fcn_getclassinfo = 0;
	_M_fcn_getclasscount = 0;
	_M_fcn_init = 0;
	_M_fcn_fini = 0;
}

void ATOM_Plugin::getVersionNumber(int* major, int* minor, int* micro) 
{
	if ( major)
	{
		*major = MAJOR_VERSION(_M_version);
	}
	if ( minor)
	{
		*minor = MINOR_VERSION(_M_version);
	}
	if ( micro)
	{
		*micro = MICRO_VERSION(_M_version);
	}
}

bool ATOM_Plugin::hasClass(const char* name) const 
{
	return name ? _M_plugin_classes.find(name) != _M_plugin_classes.end() : false;
}

unsigned ATOM_Plugin::getClassCount (void) const 
{
  return unsigned(_M_plugin_classes.size());
}

const ATOM_STRING& ATOM_Plugin::getClassNameString(int index) const 
{
	ATOM_ASSERT(index >= 0 && index < int(_M_plugin_classes.size()));
	ATOM_HASHSET<ATOM_STRING>::const_iterator it = _M_plugin_classes.begin();

#if (_MSC_VER == 1200)
	while ( index--)
		it++;
#else
	std::advance(it, index);
#endif

	return *it;
}

const ATOM_STRING& ATOM_Plugin::getDescription (void) const 
{
	return _M_description;
}

ATOM_Plugin::~ATOM_Plugin (void) 
{
	close();
}

void ATOM_Plugin::registerObjectTypes (void) 
{
	if ( !isOpened())
	{
		return;
	}

	try
	{
		for ( unsigned i = 0; i < _M_fcn_getclasscount(); ++i)
		{
			ClassInfo* ci = _M_fcn_getclassinfo(i);

			if ( ci == 0)
			{
				ATOM_LOGGER::error ("[ATOM_Plugin::RegisterObjectTypes] Class information pointer is NULL.\n");
				continue;
			}

			if ( ci->creation_func == 0 || ci->destroy_func == 0 || ci->purge_func == 0)
			{
				ATOM_LOGGER::error ("[ATOM_Plugin::RegisterObjectTypes] Invalid class creation/deletion functions.\n");
				continue;
			}

			if ( _M_kernel_server->ObjectTypeRegistered(ci->name))
			{
				ATOM_LOGGER::error ("[ATOM_Plugin::RegisterObjectTypes] Warning: class %s already registered.\n", ci->name);
				continue;
			}

			_M_kernel_server->GetFactory()->RegisterObjectType(ci->name,
															 (CreationFunc) ci->creation_func,
															 (DeletionFunc) ci->destroy_func,
															 (PurgeFunc) ci->purge_func,
															 (ATOM_ScriptInterfaceBase*) ci->script_interface,
															 ci->cache != 0);
			_M_plugin_classes.insert(ci->name);
		}
	}
	catch ( ...)
	{
		ATOM_LOGGER::fatal ("[ATOM_Plugin::RegisterObjectTypes] An exception occured.\n");
	}
}

bool ATOM_Plugin::open (const char* filename) 
{
	if ( !ATOM_DynamicLib::open(filename))
	{
		return false;
	}

	// Get plugin initfunc
	_M_fcn_init = (FcnInit)querySymbol(EXP_FCN_INIT);
	if (_M_fcn_init)
	{
		if  (!_M_fcn_init ())
		{
			close ();
			return false;
		}
	}

	// Get plugin finifunc
	_M_fcn_fini = (FcnFini)querySymbol (EXP_FCN_FINI);

	// Get plugin version
	FcnGetVersion fcnGetVersion = (FcnGetVersion) querySymbol(EXP_FCN_GETPLUGINVERSION);
	if ( fcnGetVersion == 0)
	{
		close ();
		return false;
	}

	_M_version = fcnGetVersion();

	// Get plugin description
	FcnGetDescription fcnGetDesc = (FcnGetDescription) querySymbol(EXP_FCN_GETPLUGINDESC);
	if ( fcnGetDesc == 0)
	{
		close ();
		return false;
	}

	_M_description = fcnGetDesc();

	// Store class manipulation function pointers
	_M_fcn_getclasscount = (FcnGetCount) querySymbol(EXP_FCN_GETPLUGINCOUNT);
	if ( _M_fcn_getclasscount == 0)
	{
		close ();
		return false;
	}

	_M_fcn_getclassinfo = (FcnGetClassInfo) querySymbol(EXP_FCN_GETPLUGINCLASSINFO);
	if ( _M_fcn_getclassinfo == 0)
	{
		close ();
		return false;
	}

	return true;
}

void ATOM_Plugin::unregisterObjectTypes (void) 
{
	if ( isOpened())
	{
		for ( ATOM_HASHSET<ATOM_STRING>::iterator it = _M_plugin_classes.begin(); it != _M_plugin_classes.end(); ++it)
		{
			_M_kernel_server->GetFactory()->UnregisterObjectType(it->c_str());
		}
	}
	_M_plugin_classes.clear();
}

void ATOM_Plugin::close (void) 
{
	if ( !isOpened())
		return;

	_M_version = 0;
	_M_description = "";
	_M_fcn_getclasscount = 0;
	_M_fcn_getclassinfo = 0;
	_M_fcn_init = 0;
	_M_fcn_fini = 0;

	ATOM_DynamicLib::close();
}

void ATOM_Plugin::finalize (void)
{
	if (_M_fcn_fini)
	{
		_M_fcn_fini ();
	}
}
