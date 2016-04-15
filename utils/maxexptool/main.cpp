#include <windows.h>
#include <shlobj.h>
#include <Shlwapi.h>

#include <string>

static bool isDir (const char *filename)
{
	DWORD attrib = ::GetFileAttributesA (filename);
	return (attrib != 0xFFFFFFFF && (attrib & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

static bool fileExists (const char *filename)
{
	DWORD attrib = ::GetFileAttributesA (filename);
	return attrib != 0xFFFFFFFF;
}

static std::string getApplicationDataDirectory (void)
{
	char path[MAX_PATH];

	if (S_OK == ::SHGetFolderPathA (::GetDesktopWindow(), CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, path))
	{
		std::string s = path;
		if (s.back() != '\\')
			s += '\\';
	
		s += "Autodesk\\3dsMax\\";

		if (isDir (s.c_str()))
		{
			return s;
		}
	}

	return "";
}

static std::string searchFile_R (const char *dir, const char *targetFileName)
{
	std::string s = dir;
	if (s.back() != '\\')
		s += '\\';

	std::string filename = s + targetFileName;
	if (fileExists (filename.c_str()) && !isDir (filename.c_str()))
	{
		return filename;
	}

	std::string filter = s + '*';

	WIN32_FIND_DATAA findData;
	HANDLE sh = FindFirstFileA (filter.c_str(), &findData);
	if (sh == INVALID_HANDLE_VALUE)
	{
		return "";
	}
	do
	{
		filename = findData.cFileName;
		if (filename != "." && filename != "..")
		{
			filename = s + findData.cFileName;
			if (isDir (filename.c_str()))
			{
				std::string result = searchFile_R (filename.c_str(), targetFileName);
				if (!result.empty ())
				{
					FindClose (sh);
					return result;
				}
			}
		}
	} while (FindNextFileA (sh, &findData));

	FindClose (sh);
	return "";
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	char CurrentPath[MAX_PATH];
	::GetModuleFileNameA (NULL, CurrentPath, MAX_PATH);
	char *sep = strrchr (CurrentPath, '\\');
	sep[1] = '\0';

	std::string appDataPath = getApplicationDataDirectory ();
	if (!appDataPath.empty ())
	{
		std::string iniFileName = searchFile_R (appDataPath.c_str(), "Plugin.UserSettings.ini");
		if (!iniFileName.empty ())
		{
			::WritePrivateProfileStringA ("Directories", "ATOM3D MAX Exporter", CurrentPath, iniFileName.c_str());
			::MessageBoxA (::GetDesktopWindow (), "3DSMax≤Âº˛“—≈‰÷√", "ATOM3D", MB_OK);
			return 0;
		}
	}

	return 1;
}
