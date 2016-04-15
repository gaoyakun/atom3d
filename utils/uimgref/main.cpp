#include <stdio.h>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <ATOM.h>
#include <ATOM3DX.h>

void iterat_dir (const char *dir, std::vector<std::string> &filelist, const std::vector<std::string> &exts)
{
	std::deque<std::string> dirs;
	std::string s = dir;
	if (s.length() == 0)
	{
		return;
	}

	if (s[s.length()-1] != '\\' && s[s.length()-1] != '/')
	{
		s += "\\*";
	}
	else
	{
		s += '*';
	}
	dirs.push_back (s);

	while (!dirs.empty ())
	{
		WIN32_FIND_DATA wfd;
		HANDLE hFind = ::FindFirstFileA (dirs.front().c_str(), &wfd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!strcmp (wfd.cFileName, ".") || !strcmp (wfd.cFileName, ".."))
				{
					continue;
				}
				if (wfd.dwFileAttributes != 0xFFFFFFFF)
				{
					std::string src = dirs.front();
					src.erase (src.length()-1, 1);
					src += wfd.cFileName;

					if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
					{
						const char *ext = strrchr (src.c_str(), '.');
						for (int i = 0; i < exts.size(); ++i)
						{
							if (!stricmp (ext, exts[i].c_str()))
							{
								filelist.push_back (src);
							}
						}
					}
					else
					{
						src += "\\*";
						dirs.push_back (src);
					}
				}
			} while (::FindNextFileA (hFind, &wfd));
			::FindClose (hFind);
		}

		dirs.pop_front();
	}
}

static bool matchExt (const char *filename, const char *ext)
{
	const char *s = strrchr (filename, '.');
	return s && !stricmp (ext, s);
}

int main (int argc, char *argv [])
{
	std::vector<std::string> directories;
	std::vector<std::string> exts;
	std::vector<std::string> files;
	std::string copydir;
	std::string vfsroot;
	std::string inputfilelist;
	bool chk_mt_flag = false;

	for (int i = 0; i < argc; ++i)
	{
		if (!strcmp (argv[i], "--vfsroot") && i < argc-1)
		{
			vfsroot = argv[i+1];
			i++;
		}
		if (!strcmp (argv[i], "-d") && i < argc-1)
		{
			directories.push_back (argv[i+1]);
			i++;
		}
		else if (!strcmp (argv[i], "-f") && i < argc-1)
		{
			files.push_back (argv[i+1]);
			i++;
		}
		else if (!strcmp (argv[i], "-i") && i < argc-1)
		{
			inputfilelist = argv[i+1];
			i++;
		}
		else if (!strcmp (argv[i], "-c") && i < argc-1)
		{
			copydir = argv[i+1];
			i++;
		}
		else if (!strcmp (argv[i], "-mc"))
		{
			chk_mt_flag = true;
		}
		else if (!strcmp (argv[i], "-e") && i < argc-1)
		{
			exts.push_back (argv[i+1]);
			i++;
		}
	}

	if (vfsroot.empty ())
	{
		fprintf (stderr, "No VFS root specified.\n");
		return 1;
	}

	ATOM_KernelConfig::initializeWithPath (vfsroot.c_str());
	registerPluginsImage ();
	registerPluginsRender ();
	registerPluginsEngine ();

	std::vector<std::string> filelist;
	for (int i = 0; i < directories.size(); ++i)
	{
		iterat_dir (directories[i].c_str(), filelist, exts);
	}

	for (int i = 0; i < files.size(); ++i)
	{
		filelist.push_back (files[i]);
	}

	if (!inputfilelist.empty ())
	{
		std::ifstream infile(inputfilelist.c_str());
		if (!infile.is_open())
		{
			fprintf (stderr, "Input list file cannot be opened.\n");
		}
		else
		{
			std::string line;
			while (std::getline (infile, line))
			{
				filelist.push_back (line);
			}
		}
	}

	if (filelist.empty())
	{
		fprintf (stderr, "No input file.\n");
		return 1;
	}

	ATOM_SET<ATOM_STRING> filenameMap;
	for (int i = 0; i < filelist.size(); ++i)
	{
		printf ("Processing file %s...\n", filelist[i].c_str());
		if (matchExt (filelist[i].c_str(), ".ui"))
		{
			ATOMX_GetReferencedFileListOfUI (filelist[i].c_str(), filenameMap);
		}
		else if (matchExt (filelist[i].c_str(), ".nm2"))
		{
			ATOMX_GetReferencedFileListOfNM2 (filelist[i].c_str(), filenameMap);
		}
		else if (matchExt (filelist[i].c_str(), ".nm"))
		{
			ATOMX_GetReferencedFileListOfNM (filelist[i].c_str(), filenameMap);
		}
		else if (matchExt (filelist[i].c_str(), ".3sg"))
		{
			ATOMX_GetReferencedFileListOf3SG (filelist[i].c_str(), true, filenameMap);
		}
		else if (matchExt (filelist[i].c_str(), ".csp"))
		{
			ATOMX_GetReferencedFileListOfCSP (filelist[i].c_str(), filenameMap);
		}
		else if (matchExt (filelist[i].c_str(), ".cps"))
		{
			ATOMX_GetReferencedFileListOfCPS (filelist[i].c_str(), filenameMap);
		}
		else if (matchExt (filelist[i].c_str(), ".cp"))
		{
			ATOMX_GetReferencedFileListOfCP (filelist[i].c_str(), filenameMap);
		}
		else if (matchExt (filelist[i].c_str(), ".ccp"))
		{
			ATOMX_GetReferencedFileListOfCCP (filelist[i].c_str(), filenameMap);
		}
		else if (matchExt (filelist[i].c_str(), ".mat"))
		{
			ATOMX_GetReferencedFileListOfMat (filelist[i].c_str(), filenameMap);
		}
	}
	ATOMX_GetPrerequiredFileList (filenameMap);
	ATOMX_GetFileListInDirectory ("/editor", true, true, false, filenameMap);

	for (auto it = filenameMap.begin(); it != filenameMap.end(); ++it)
	{
		printf ("%s\n", it->c_str());

		if (!copydir.empty ())
		{
			if (!ATOMX_CopyFile (it->c_str(), copydir.c_str(), true))
			{
				fprintf (stderr, "Copy file failed.\n");
			}
		}
	}

	return 0;
}
