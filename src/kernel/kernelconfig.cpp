#include "StdAfx.h"
#include "kernelconfig.h"

static bool _isRecordFileName = false;

static void IteratorConfig(const ATOM_TiXmlNode *pXmlNode, const char *password)
{
	const ATOM_TiXmlNode *tp = pXmlNode;
	while (tp)
	{
		const ATOM_TiXmlElement *p_xmlelement= dynamic_cast<const ATOM_TiXmlElement*>(tp);
		if( p_xmlelement )
		{
			if( !_stricmp("mount", p_xmlelement->Value()) )			//auto mount
			{
				const char *s_path = p_xmlelement->Attribute("osfs");
				if( s_path)
				{
					ATOM_MemVFS *memVfs = ATOM_NEW(ATOM_MemVFS);
					if (!strnicmp (s_path, "http://", 7) || !strnicmp (s_path, "ftp://", 6))
					{
						char buff[MAX_PATH];
						const char *s_cache = p_xmlelement->Attribute("local");
						if (!s_cache)
						{
							::GetTempPathA (MAX_PATH, buff);
							s_cache = buff;
						}
						int compressed = 0;
						p_xmlelement->QueryIntAttribute ("compressed", &compressed);
						ATOM_HttpVFS *httpVFS = ATOM_NEW(ATOM_HttpVFS, s_path, s_cache);
						httpVFS->enableCompressMode (compressed != 0);
						memVfs->mount (httpVFS, "/", "/", true);
						ATOM_PhysicVFS *physicVfs = ATOM_NEW(ATOM_PhysicVFS);
						httpVFS->mount (physicVfs, s_cache, "/", false);
						ATOM_AddVFS (memVfs, false);
					}
					else
					{
						char ident[MAX_PATH];
						::GetFullPathNameA (s_path, MAX_PATH, ident, NULL);

						if (ATOM_SysIsDirectory (ident))
						{
							ATOM_PhysicVFS *physicVfs = ATOM_NEW(ATOM_PhysicVFS);
							memVfs->mount (physicVfs, ident, "/", false);
							ATOM_AddVFS (memVfs, true);
						}
						else
						{
							ATOM_VFS *vfs = ATOM_LoadVFS (ident, password);
							if (vfs)
							{
								ATOM_STRING root;
								root += vfs->getSeperator();
								memVfs->mount (vfs, root.c_str(), "/", true);
								ATOM_AddVFS (memVfs, false);
							}
							else
							{
								ATOM_DELETE(memVfs);
								ATOM_DELETE(vfs);
							}
							/*
							ATOM_ZipVFS *zipVfs = ATOM_NEW(ATOM_ZipVFS);
							if (zipVfs->load (s_path, password))
							{
								memVfs->mount (zipVfs, "/", "/", true);
								ATOM_AddVFS (memVfs, false);
							}
							else
							{
								ATOM_DELETE(memVfs);
								ATOM_DELETE(zipVfs);
							}
							*/
						}
					}
				}
			}
		}

		IteratorConfig(tp->FirstChild(), password);

		tp = tp->NextSiblingElement();
	}
}

ATOM_PhysicVFS *ATOM_KernelConfig::initializeWithPath (const char *dir)
{
	if (!dir)
	{
		return NULL;
	}

	DWORD attributes = ::GetFileAttributesA(dir);
	if ( attributes == 0xFFFFFFFF)
		return NULL;
	if ((attributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		return NULL;

	ATOM_MemVFS *memVfs = ATOM_NEW(ATOM_MemVFS);
	ATOM_PhysicVFS *physicVfs = ATOM_NEW(ATOM_PhysicVFS);
	memVfs->mount (physicVfs, dir, "/", false);
	ATOM_AddVFS (memVfs, true);

	ATOM_VECTOR<ATOM_STRING> args;
	ATOM_GetCommandLine (args);
	for (unsigned i = 0; i < args.size(); ++i)
	{
		if (args[i] == "--record-filename")
		{
			_isRecordFileName = true;
		}
	}

	return physicVfs;
}

ATOM_HttpVFS *ATOM_KernelConfig::initializeWithHTTP (const char *url, const char *url2, const char *cache, bool compressed, const char *param, int (*funcCheckFile)(const char*, int))
{
	if (!url)
	{ 
		return NULL;
	}

	ATOM_HttpVFS *httpVfs = ATOM_NEW(ATOM_HttpVFS, url, url2, cache, param, funcCheckFile);
	httpVfs->enableCompressMode (compressed);
#if 1
	ATOM_AddVFS (httpVfs, true);
#else
	ATOM_MemVFS *memVfs = ATOM_NEW(ATOM_MemVFS);
	memVfs->mount (httpVfs, "/", "/", false);
	ATOM_AddVFS (memVfs, true);
#endif

	ATOM_VECTOR<ATOM_STRING> args;
	ATOM_GetCommandLine (args);
	for (unsigned i = 0; i < args.size(); ++i)
	{
		if (args[i] == "--record-filename")
		{
			_isRecordFileName = true;
		}
	}

	return httpVfs;
}

ATOM_ZipVFS *ATOM_KernelConfig::initializeWithZIP (const char *zipFileName, const char *password)
{
	if (!zipFileName)
	{
		return NULL;
	}

	ATOM_VFS *vfs = ATOM_LoadVFS (zipFileName, password);
	if (!vfs)
	{
		return NULL;
	}

	ATOM_MemVFS *memVfs = ATOM_NEW(ATOM_MemVFS);
	memVfs->mount (vfs, "/", "/", true);
	ATOM_AddVFS (memVfs, false);

	ATOM_VECTOR<ATOM_STRING> args;
	ATOM_GetCommandLine (args);
	for (unsigned i = 0; i < args.size(); ++i)
	{
		if (args[i] == "--record-filename")
		{
			_isRecordFileName = true;
		}
	}

	return (ATOM_ZipVFS*)vfs;
}

bool ATOM_KernelConfig::initializeEx (const char *cfgFileName, const char *password)
{
	ATOM_STACK_TRACE(ATOM_KernelConfig::initializeEx);

	char cwd[MAX_PATH];
	::GetCurrentDirectoryA (MAX_PATH, cwd);

	const char *vfsroot = 0;
	ATOM_VECTOR<ATOM_STRING> args;
	ATOM_GetCommandLine (args);
	for (unsigned i = 0; i < args.size(); ++i)
	{
		if (args[i] == "--vfsroot" && i < args.size()-1)
		{
			vfsroot = args[i+1].c_str();
			break;
		}
	}
	for (unsigned i = 0; i < args.size(); ++i)
	{
		if (args[i] == "--record-filename")
		{
			_isRecordFileName = true;
		}
	}

	if (vfsroot)
	{
		DWORD attributes = ::GetFileAttributesA(vfsroot);
		if ( attributes == 0xFFFFFFFF)
			vfsroot = 0;
	}

	if (!vfsroot)
	{
		if (!cfgFileName)
		{
			cfgFileName = "vfs.cfg";
		}

		char ident[MAX_PATH];
		if (!::GetFullPathNameA (cfgFileName, MAX_PATH, ident, NULL))
		{
			return 0;
		}

		ATOM_TiXmlDocument doc(cfgFileName);
		if(!doc.LoadFile() )
		{
			if (!vfsroot)
			{
				vfsroot = cwd;
			}

			ATOM_MemVFS *memVfs = ATOM_NEW(ATOM_MemVFS);
			ATOM_PhysicVFS *physicVfs = ATOM_NEW(ATOM_PhysicVFS);
			memVfs->mount (physicVfs, vfsroot, "/", false);
			ATOM_AddVFS (memVfs, true);
		}
		else
		{
			*strrchr (ident, '\\') = '\0';
			::SetCurrentDirectoryA (ident);
			IteratorConfig(doc.RootElement(), password);
		}
	}
	else
	{
		DWORD attributes = ::GetFileAttributesA(vfsroot);
		if ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			ATOM_MemVFS *memVfs = ATOM_NEW(ATOM_MemVFS);
			ATOM_PhysicVFS *physicVfs = ATOM_NEW(ATOM_PhysicVFS);
			memVfs->mount (physicVfs, vfsroot, "/", false);
			ATOM_AddVFS (memVfs, true);
		}
		else
		{
			ATOM_VFS *vfs = ATOM_LoadVFS (vfsroot, password);
			if (vfs)
			{
				ATOM_MemVFS *memVfs = ATOM_NEW(ATOM_MemVFS);
				memVfs->mount (vfs, "/", "/", true);
				ATOM_AddVFS (memVfs, false);
			}
		}
	}

	::SetCurrentDirectoryA (cwd);
	return true;
}

bool ATOM_KernelConfig::initialize (const char *password)
{
	ATOM_STACK_TRACE(ATOM_KernelConfig::initialize);

	return initializeEx ("vfs.cfg", password);

	/*
	ATOM_TiXmlDocument doc("VFS.CFG");
	if(!doc.LoadFile() )
	{
		char s_path[MAX_PATH];
		const char *vfsroot = 0;

		ATOM_VECTOR<ATOM_STRING> args;
		ATOM_GetCommandLine (args);
		for (unsigned i = 0; i < args.size(); ++i)
		{
			if (args[i] == "--vfsroot" && i < args.size()-1)
			{
				vfsroot = args[i+1].c_str();
				break;
			}
		}

		if (!vfsroot || !ATOM_IsDirectory(vfsroot))
		{
			::GetCurrentDirectoryA (MAX_PATH, s_path);
			vfsroot = s_path;
		}

		ATOM_MemVFS *memVfs = ATOM_NEW(ATOM_MemVFS);
		ATOM_PhysicVFS *physicVfs = ATOM_NEW(ATOM_PhysicVFS);
		memVfs->mount (physicVfs, vfsroot, "/", false);
		ATOM_AddVFS (memVfs, true);
	}
	else
	{
		IteratorConfig(doc.RootElement(), password);
	}

	return true;
	*/
}

bool ATOM_KernelConfig::isRecordFileName (void)
{
	return _isRecordFileName;
}

void ATOM_KernelConfig::setRecordFileName(bool record)
{
	_isRecordFileName = record;
}

