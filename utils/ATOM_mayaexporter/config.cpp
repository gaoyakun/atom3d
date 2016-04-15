#include <windowsx.h>
#include <shlobj.h>
#include "config.h"
#include "tinyxml.h"

extern HINSTANCE hModelHandle;
static const char *configFileName = "ATOM_mayaexp.xml";

static char gRootDir[MAX_PATH];

static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT msg, LPARAM lp, LPARAM pData)   
{   
	if (msg == BFFM_INITIALIZED && gRootDir[0])   
	{   
		::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)gRootDir);   
	}   
	return   0;   
}  

BOOL chooseDir (HWND hWndParent, const char *title, char *initialDir)
{
	BROWSEINFO bi;
	char folderName[MAX_PATH];
	memset(&bi, 0, sizeof(bi));
	memset(folderName, 0, sizeof(folderName));
	bi.hwndOwner = hWndParent;
	bi.pszDisplayName = 0;
	bi.lpszTitle = title;
	bi.ulFlags = BIF_STATUSTEXT|BIF_USENEWUI|BIF_RETURNONLYFSDIRS;
	bi.lpfn = &BrowseCallbackProc;

	strcpy (gRootDir, initialDir);
	LPITEMIDLIST itemId = SHBrowseForFolder (&bi);
	if (itemId)
	{
		SHGetPathFromIDList (itemId, folderName);
		GlobalFreePtr (itemId);
		strcpy (initialDir, folderName);
		return TRUE;
	}
	return FALSE;
}

ExporterConfig::ExporterConfig (void)
{
	load ();
}

ExporterConfig::~ExporterConfig (void)
{
	save ();
}

std::string ExporterConfig::getFileName (void) const
{
	char path[MAX_PATH];
	::GetModuleFileName (NULL, path, MAX_PATH);
	char *p = strrchr (path, '\\');
	assert(p);
	strcpy (p+1, "\\plug-ins\\");
	strcat (path, configFileName);
	return path;
}

void ExporterConfig::load (void)
{
	shareDirectory.clear ();
	rootDirectory.clear ();

	std::string configFile = getFileName ();

	ATOM_TiXmlDocument doc(configFile.c_str());

	if (doc.LoadFile ())
	{
		ATOM_TiXmlNode *rootNode = doc.RootElement ();
		if (rootNode)
		{
			const ATOM_TiXmlElement* sharedDir = rootNode->FirstChildElement("sharedDir");
			if (sharedDir)
			{
				const char *dir = sharedDir->Attribute("path");
				if (dir)
				{
					shareDirectory = dir;
				}
			}

			const ATOM_TiXmlElement* rootDir = rootNode->FirstChildElement("rootDir");
			if (sharedDir)
			{
				const char *dir = rootDir->Attribute("path");
				if (dir)
				{
					rootDirectory = dir;
				}
			}
		}
	}
}

void ExporterConfig::save (void)
{
	std::string configFile = getFileName ();
	ATOM_TiXmlDocument doc (configFile.c_str());
	ATOM_TiXmlDeclaration decl ("1.0", "gb2312", "");
	doc.InsertEndChild (decl);

	ATOM_TiXmlElement pRoot("config");

	ATOM_TiXmlElement pElementShare("sharedDir");
	pElementShare.SetAttribute ("path", shareDirectory.c_str());
	pRoot.InsertEndChild (pElementShare);

	ATOM_TiXmlElement pElementRoot("rootDir");
	pElementRoot.SetAttribute ("path", rootDirectory.c_str());
	pRoot.InsertEndChild (pElementRoot);

	doc.InsertEndChild (pRoot);
	doc.SaveFile ();
}

ExporterConfig &ExporterConfig::getInstance (void)
{
	static ExporterConfig *config = new ExporterConfig;

	if (config->rootDirectory.empty ())
	{
		char rootDir[MAX_PATH] = { 0 };
		char shareDir[MAX_PATH] = { 0 };
		if (chooseDir (::GetActiveWindow(), "选择资源根目录:", rootDir))
		{
			if (chooseDir (::GetActiveWindow(), "选择共享目录:", shareDir))
			{
				config->rootDirectory = rootDir;
				config->shareDirectory = shareDir;
				config->save ();
			}
		}
	}

	return *config;
}
