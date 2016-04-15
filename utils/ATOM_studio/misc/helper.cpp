#include "StdAfx.h"
#include "helper.h"

bool IsProjectDirectory (const char *path)
{
	ATOM_PhysicVFS vfs;
	char buffer[ATOM_VFS::max_filename_length];
	vfs.identifyPath (path, buffer);
	if (!vfs.isDir (buffer))
	{
		return false;
	}

	if (buffer[strlen(buffer)-1] != '\\')
		strcat (buffer, "\\");
	strcat (buffer, ".astudio");
	return vfs.isDir (buffer);
};

ATOM_STRING findProjectDirectory (const char *file)
{
	ATOM_PhysicVFS vfs;
	char buffer[ATOM_VFS::max_filename_length];
	vfs.identifyPath (file, buffer);

	for(;;)
	{
		if (IsProjectDirectory (buffer))
		{
			return buffer;
		}
		char *p = strrchr (buffer, '\\');
		if (!p)
		{
			return "";
		}
		*p = '\0';
	}
}

bool MakeProjectDirectory (const char *path, bool forceCreate)
{
	ATOM_PhysicVFS vfs;

	if (vfs.doesFileExists(path) && !vfs.isDir(path))
	{
		return false;
	}

	if (!vfs.doesFileExists(path))
	{
		if (forceCreate)
		{
			vfs.mkDir(path);
			if (!vfs.isDir (path))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	char buffer[ATOM_VFS::max_filename_length];
	if (!vfs.identifyPath(path, buffer))
	{
		return false;
	}

	if (buffer[strlen(buffer)-1] != '\\')
	{
		strcat (buffer, "\\");
	}
	strcat (buffer, ".astudio");

	if (!vfs.isDir (buffer))
	{
		vfs.mkDir (buffer);
		if (!vfs.isDir (buffer))
		{
			return false;
		}
	}

	return true;
}

void GetDocumentForOpen (int argc, char **argv, ATOM_STRING &project, ATOM_STRING &file)
{
	project = "";
	file = "";

	for (int i = 0; i < argc; ++i)
	{
		if (!strcmp (argv[i], "-o"))
		{
			if (i < argc-1)
			{
				ATOM_STRING doc = argv[i+1];
				ATOM_STRING proj = findProjectDirectory (doc.c_str());
				if (!doc.empty() && !proj.empty())
				{
					file = doc;
					project = proj;
					return;
				}
			}
		}
	}
}

ATOM_STRING findEarthSculptorPluginDir (void)
{
	ATOM_STRING path;
	HKEY hKey = NULL;
	LONG ret = ::RegOpenKeyExA (HKEY_LOCAL_MACHINE, "software\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0, KEY_READ|KEY_WOW64_32KEY , &hKey);
	if (ret == ERROR_SUCCESS)
	{
		char nameBuffer[256];
		DWORD index = 0;
		for (;;)
		{
			if (::RegEnumKeyA (hKey, index, nameBuffer, 256) != ERROR_SUCCESS)
			{
				break;
			}
			if (StrStrIA (nameBuffer, "EarthSculptor"))
			{
				HKEY subKey;
				ret = ::RegOpenKeyExA (hKey, nameBuffer, 0, KEY_READ|KEY_WOW64_32KEY, &subKey);
				if (ret == ERROR_SUCCESS)
				{
					char valueBuffer[512];
					DWORD size = 512;
					ret = ::RegQueryValueExA (subKey, "InstallLocation", NULL, NULL, (LPBYTE)valueBuffer, &size);
					::RegCloseKey (subKey);

					if (ret == ERROR_SUCCESS)
					{
						path = valueBuffer;
						break;
					}
				}
			}
			index++;
		}
		::RegCloseKey (hKey);
	}
	return path;
}

static char gRootDir[MAX_PATH];

static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT msg, LPARAM lp, LPARAM pData)   
{   
	if (msg == BFFM_INITIALIZED && gRootDir[0])   
	{   
		::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)gRootDir);   
	}   
	return   0;   
}  

bool ChooseDir (HWND hWndParent, const char *title, char *initialDir)
{
	BROWSEINFOA bi;
	char folderName[MAX_PATH];
	memset(&bi, 0, sizeof(bi));
	memset(folderName, 0, sizeof(folderName));
	bi.hwndOwner = hWndParent;
	bi.pszDisplayName = 0;
	bi.lpszTitle = title;
	bi.ulFlags = BIF_STATUSTEXT|BIF_RETURNONLYFSDIRS;
	bi.lpfn = &BrowseCallbackProc;

	strcpy (gRootDir, initialDir);
	LPITEMIDLIST itemId = SHBrowseForFolderA (&bi);
	if (itemId)
	{
		SHGetPathFromIDListA (itemId, folderName);
		GlobalFreePtr (itemId);
		strcpy (initialDir, folderName);
		return true;
	}
	return false;
}

Config *Config::getInstance (void)
{
	static Config config;
	return &config;
}

static ATOM_STRING getApplicationDataDirectory (void)
{
	char path[MAX_PATH];

	if (S_OK == ::SHGetFolderPathA (ATOM_APP->getMainWindow(), CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, path))
	{
		return path;
	}

	return "";
}

static ATOM_STRING getConfigFileName (void)
{
	ATOM_STRING path = getApplicationDataDirectory ();
	if (path.empty())
	{
		return path;
	}
	if (path.back() != '\\')
		path += '\\';

	path += "Toy Brick Studio\\";

	ATOM_PhysicVFS vfs;
	if (!vfs.isDir (path.c_str()))
	{
		vfs.mkDir (path.c_str());
		if (!vfs.isDir (path.c_str()))
		{
			return "";
		}
	}

	path += "astudio.xml";

	return path;
}

Config::Config (void)
{
	_updateServer = "http://192.168.18.115/ATOM3D/update/";
	_version = 0;
	_configFileName = getConfigFileName ();

	load ();
}

Config::~Config (void)
{
	save ();
}

void Config::setLastProject (const char *proj)
{
	_lastProject = proj ? proj : "";
}

const char *Config::getLastProject (void) const
{
	return _lastProject.c_str();
}

unsigned Config::getNumRecentProjects (void) const
{
	return _recentProjects.size();
}

const char *Config::getRecentProject (unsigned index) const
{
	return _recentProjects[index].c_str();
}

void Config::clearRecentProjects (void)
{
	_recentProjects.resize (0);
}

void Config::addRecentProject (const char *proj)
{
	if (proj)
	{
		char ident[ATOM_VFS::max_filename_length];
		if (ATOM_PhysicVFS().identifyPath (proj, ident))
		{
			for (unsigned i = 0; i < _recentProjects.size(); ++i)
			{
				if (!stricmp (_recentProjects[i].c_str(), ident))
				{
					return;
				}
			}
			_recentProjects.push_back (ident);
		}
	}
}

bool Config::load (void)
{
	if (_configFileName.empty ())
	{
		return false;
	}

	ATOM_TiXmlDocument doc(_configFileName.c_str());
	if (!doc.LoadFile ())
	{
		return false;
	}

	if (!doc.RootElement ())
	{
		return false;
	}

	int version;
	if (doc.RootElement()->QueryIntAttribute ("Version", &version) == ATOM_TIXML_SUCCESS)
	{
		_version = version;
	}

	ATOM_TiXmlElement *lastProject = doc.RootElement()->FirstChildElement ("LastProject");
	if (lastProject)
	{
		const char *lastProjectStr = lastProject->Attribute ("Name");
		if (lastProjectStr)
		{
			setLastProject (lastProjectStr);
		}
	}

	ATOM_TiXmlElement *recentProjects = doc.RootElement()->FirstChildElement ("RecentProjects");
	if (recentProjects)
	{
		for (ATOM_TiXmlElement *p = recentProjects->FirstChildElement("Project"); p; p = p->NextSiblingElement("Project"))
		{
			const char *proj = p->Attribute ("Name");
			if (proj)
			{
				addRecentProject (proj);
			}
		}
	}

	ATOM_TiXmlElement *updateServer = doc.RootElement()->FirstChildElement ("UpdateServer");
	if (updateServer)
	{
		const char *s = updateServer->Attribute ("Name");
		if (s)
		{
			_updateServer = s;
		}
	}

	return true;
}

void Config::save (void) const
{
	if (_configFileName.empty ())
	{
		return;
	}

	ATOM_TiXmlDocument doc(_configFileName.c_str());
	ATOM_TiXmlDeclaration eDecl("1.0", "gb2312", "");
	doc.InsertEndChild (eDecl);

	ATOM_TiXmlElement root("Config");

	root.SetAttribute ("Version", _version);

	if (!_lastProject.empty())
	{
		ATOM_TiXmlElement lastProj("LastProject");
		lastProj.SetAttribute ("Name", _lastProject.c_str());
		root.InsertEndChild (lastProj);
	}

	if (!_recentProjects.empty ())
	{
		ATOM_TiXmlElement recentProjects("RecentProjects");
		for (unsigned i = 0; i < _recentProjects.size(); ++i)
		{
			ATOM_TiXmlElement p("Project");
			p.SetAttribute ("Name", _recentProjects[i].c_str());
			recentProjects.InsertEndChild (p);
		}
		root.InsertEndChild (recentProjects);
	}

	if (!_updateServer.empty())
	{
		ATOM_TiXmlElement updateServer("UpdateServer");
		updateServer.SetAttribute ("Name", _updateServer.c_str());
		root.InsertEndChild (updateServer);
	}

	doc.InsertEndChild (root);

	doc.SaveFile ();
}

const char *Config::getUpdateServer (void) const
{
	return _updateServer.c_str();
}

unsigned Config::getVersion (void) const
{
	return _version;
}

