#include <windows.h>
#include <direct.h>
#include <string>
#include <deque>
#include <vector>
#include <algorithm>
#include <functional>
#include <stdio.h>
#include "archive.h"

static bool identifyPathWIN32 (const char *dir, char *buffer)
{
  if (!::GetFullPathNameA (dir, MAX_PATH, buffer, NULL))
  {
    return 0;
  }

  return true;
}

static bool identifyPathZIP (const char *dir, char *buffer)
{
	if (!dir || !buffer)
	{
		return false;
	}

	// skip leading spaces
	while (*dir == ' ' || *dir == '\t')
	{
		++dir;
	}

	// empty string
	if (!*dir)
	{
		return false;
	}

	struct tok
	{
		const char *s;
		unsigned size;
	};

	const char sep [2] = { '/', '\0' };
	tok tokens[1024];
	tok *token_top = tokens;

	const char *fullpath = dir+1;

	for (;;)
	{
		token_top->s = fullpath;
		const char *next = strchr(fullpath, sep[0]);
		if (next == fullpath)
		{
			return false;
		}

		unsigned size = next ? next-fullpath : strlen(fullpath);

		if (1)
		{
			// we need not do dot check for cwd because it should be always identified
			if (size == 1 && fullpath[0] == '.')
			{
				// meet '.', skip it
				if (next)
				{
					fullpath = next + 1;
					continue;
				}
				else
				{
					break;
				}
			}
			else if (size == 2 && fullpath[0] == '.' && fullpath[1] == '.')
			{
				// meet "..", discard previous token
				if (token_top == tokens)
				{
					// failed because too many ".."
					*buffer = '\0';
					return false;
				}
				else
				{
					--token_top;

					if (next)
					{
						fullpath = next + 1;
						continue;
					}
					else
					{
						break;
					}
				}
			}
		}

		// valid token
		token_top->size = size;
		++token_top;

		if (next)
		{
			if (next[1])
			{
				fullpath = next+1;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	*buffer = '\0';

	for (tok *p = tokens; p < token_top; ++p)
	{
		if (p->size)
		{
			strcat (buffer, sep);
			strncat (buffer, p->s, p->size);
		}
	}

	if (!*buffer)
	{
		strcpy (buffer, sep);
	}
	else
	{
		// remove pending spaces
		char *buffer_end = buffer + strlen(buffer);
		while (*(buffer_end-1) == ' ' || *(buffer_end-1) == '\t')
		{
			--buffer_end;
		}

		if (*(buffer_end-1) == sep[0])
		{
			--buffer_end;
		}

		*buffer_end = '\0';
	}

    _strlwr (buffer);

	return true;
}

static bool isDirectoryWIN32 (const char *dir)
{
	char ident[MAX_PATH];

	if (dir)
	{
		if (!identifyPathWIN32 (dir, ident))
		{
			return false;
		}

        DWORD attrib = ::GetFileAttributesA (ident);
        return attrib != 0xFFFFFFFF && (attrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}
	return false;
}

ATOM_Archive::ATOM_Archive (void)
{
	_filename = 0;
	_passwd = 0;
}

ATOM_Archive::ATOM_Archive (const char *filename, const char *passwd)
{
	_filename = 0;
	_passwd = 0;

	open (filename, passwd);
}

ATOM_Archive::~ATOM_Archive (void)
{
	close ();
}

bool ATOM_Archive::open (const char *filename, const char *passwd)
{
	close ();

	if (!filename)
	{
		return false;
	}

	ATOM_UnZip unzip(filename);
	if (!unzip.opened())
	{
		{
			ATOM_Zip zip(filename, false);
			if (!zip.opened())
			{
				return false;
			}
		}

		if (!unzip.open (filename))
		{
			return false;
		}
	}

	_filename = _strdup (filename);

	if (passwd)
	{
		_passwd = _strdup (passwd);
	}

	return true;
}

void ATOM_Archive::close (void)
{
	if (_filename)
	{
		free (_filename);
		_filename = 0;
	}

	if (_passwd)
	{
		free (_passwd);
		_passwd = 0;
	}
}

bool ATOM_Archive::opened (void) const
{
	return _filename != 0;
}

bool ATOM_Archive::fileExists (const char *file) const
{
	if (!file)
	{
		return false;
	}

	if (!opened ())
	{
		return false;
	}

	ATOM_UnZip unzip(_filename);
	if (!unzip.opened ())
	{
		return false;
	}

	if (unzip.openInZip (file, false, _passwd))
	{
		unzip.closeInZip ();
		return true;
	}

	return false;
}

bool ATOM_Archive::addFile (const char *filenameDst, const char *filenameSrc)
{
	if (!filenameDst || !filenameSrc)
	{
		return false;
	}

	if (!opened ())
	{
		return false;
	}

	if (fileExists (filenameDst))
	{
		return false;
	}

	ATOM_Zip zip(_filename, true);
	if (!zip.opened ())
	{
		return false;
	}

	char ident[MAX_PATH];
	if (!identifyPathWIN32 (filenameSrc, ident))
	{
		return false;
	}
	ATOM_Zip::FileWriter writer(ident);

	if (!zip.newFile (filenameDst, &writer, ATOM_Zip::CM_DEFAULT, 0, false, _passwd))
	{
		return false;
	}

	return true;
}

bool ATOM_Archive::addFile (const char *filenameDst, const void *mem, unsigned size)
{
	if (!mem)
	{
		return false;
	}

	if (!opened ())
	{
		return false;
	}

	if (fileExists (filenameDst))
	{
		return false;
	}

	ATOM_Zip zip(_filename, true);
	if (!zip.opened ())
	{
		return false;
	}

	ATOM_Zip::MemWriter writer(mem, size);

	if (!zip.newFile (filenameDst, (ATOM_Zip::Writer*)&writer, ATOM_Zip::CM_DEFAULT, 0, false, _passwd))
	{
		return false;
	}

	return true;
}

static void adjustDirName (std::string &s)
{
	// trim
	s.erase (0, s.find_first_not_of(" \t\r\n"));
	s.erase (s.find_last_not_of(" \t\r\n") + 1);

	// replace '/' with '\\'
	std::replace_if (s.begin(), s.end(), std::bind2nd(std::equal_to<char>(), '/'), '\\');

	// insure '\\' is at the end of the string
	unsigned len = s.length();
	if (len && s[len-1] != '\\')
	{
		s += '\\';
	}

	// add parttern
	s += '*';
}

static void adjustZipDirName (std::string &s)
{
	// trim
	s.erase (0, s.find_first_not_of(" \t\r\n"));
	s.erase (s.find_last_not_of(" \t\r\n") + 1);

	// replace '\\' with '/'
	std::replace_if (s.begin(), s.end(), std::bind2nd(std::equal_to<char>(), '\\'), '/');

	// insure '/' is at the end of the string
	unsigned len = s.length();
	if (len && s[len-1] != '/')
	{
		s += '/';
	}
}

bool ATOM_Archive::addDirectory (const char *dirDst, const char *dirSrc)
{
	if (!dirDst)
	{
		return false;
	}

	if (!dirSrc)
	{
		// add empty directory
		if (fileExists (dirDst))
		{
			return false;
		}

		ATOM_Zip zip(_filename, true);
		if (!zip.opened ())
		{
			return false;
		}

		zip.newDir (dirDst);

		return true;
	}

	if (!dirSrc[0])
	{
		return false;
	}

	char ident[MAX_PATH];
	if (!identifyPathWIN32 (dirSrc, ident))
	{
		return false;
	}

	if (!isDirectoryWIN32 (ident))
	{
		return false;
	}

	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	std::deque<std::string> dirs;
	std::deque<std::string> dirdest;

	dirs.push_back (ident);
	adjustDirName (dirs.back());
	dirdest.push_back (dirDst);
	adjustZipDirName (dirdest.back());

	while (!dirs.empty())
	{
		int numFilesAdded = 0;

		hFind = ::FindFirstFile (dirs.front().c_str(), &wfd);

		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!strcmp(wfd.cFileName, ".") || !strcmp(wfd.cFileName, ".."))
				{
					continue;
				}

				if (wfd.dwFileAttributes != 0xFFFFFFFF)
				{
					// create the full path
					std::string src = dirs.front();
					src.erase (src.length() - 1, 1);
					src += wfd.cFileName;

					std::string dest = dirdest.front();
					dest += wfd.cFileName;

					if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
					{
						// is a file, add it to zip
						printf ("\nAdding file %s", src.c_str());

						if (!addFile (dest.c_str(), src.c_str()))
						{
							return false;
						}

						numFilesAdded++;
					}
					else
					{
						// is a directory, queue it for further processing
						src += "\\*";
						dirs.push_back (src);

						dest += '/';
						dirdest.push_back (dest);
					}
				}
			}
			while (::FindNextFileA (hFind, &wfd));
			::FindClose (hFind);
		}

		if (!numFilesAdded && !addDirectory (dirdest.front().c_str(), 0))
		{
			return false;
		}

		dirs.pop_front ();
		dirdest.pop_front ();
	}

	return true;
}

class RawWriter: public ATOM_Zip::Writer
{
	ATOM_UnZip *_unzip;

public:
	RawWriter (ATOM_UnZip *unzip): _unzip(unzip) {}

public:
	virtual bool write (ATOM_Zip *zip)
	{
		const unsigned size_buf = 16384;
		char buffer[size_buf];
		unsigned total_read = 0;
		const unsigned total_size = _unzip->getCurrentFileCompressedSize();

		for (;;)
		{
			unsigned numRead = _unzip->readInZip (buffer, size_buf);
			if (numRead == 0)
			{
				break;
			}
			if (!this->writeZip (zip, buffer, numRead))
			{
				return false;
			}
			total_read += numRead;
		}

		if(total_read != total_size)
		{
			return false;
		}

		return true;
	}

	virtual bool getCRC (unsigned long *crc) const
	{
		*crc = _unzip->getCurrentFileCRC ();
		return true;
	}

	virtual unsigned getUncompressedSize (void) const
	{
		return _unzip->getCurrentFileUncompressedSize();
	}
};

bool ATOM_Archive::removeFile (const char *filenameDst)
{
	if (!filenameDst)
	{
		return false;
	}

	if (!opened ())
	{
		return false;
	}

	ATOM_UnZip unzip (_filename);
	if (!unzip.opened())
	{
		return false;
	}

	if (!unzip.openInZip (filenameDst, false, _passwd))
	{
		return false;
	}

	char filenameInZip[256];
	const char *fn = unzip.getCurrentFileName();
	if (!fn)
	{
		return false;
	}
	strcpy (filenameInZip, fn);

	unzip.closeInZip ();

	char tmpFileName[260];
	if (!tmpnam (tmpFileName))
	{
		return false;
	}

	ATOM_Zip zip (tmpFileName, false);
	if (!zip.opened ())
	{
		return false;
	}

	const unsigned size_buf = 16384;

	if (!unzip.openFirstInZip (true, _passwd))
	{
		return false;
	}

	for (;;)
	{
		if (strcmp (filenameInZip, unzip.getCurrentFileName()))
		{
			RawWriter writer(&unzip);
			if (!zip.newFile (unzip.getCurrentFileName(), &writer, ATOM_Zip::CM_DEFAULT, 0, true, _passwd))
			{
				return false;
			}
		}
		unzip.closeInZip ();
		if (!unzip.openNextInZip (true, _passwd))
		{
			break;
		}
	}

	unzip.close ();
	zip.close ();

	if (0 != _unlink (_filename))
	{
		return false;
	}

	if (!::CopyFile (tmpFileName, _filename, TRUE))
	{
		_unlink (tmpFileName);
		return false;
	}

	_unlink (tmpFileName);

	return true;
}

static void genTmpFileName (const char *filename, char *buffer)
{
	int n = 0;
	for (;;)
	{
		sprintf (buffer, "%s.%d", filename, n++);
		HANDLE h = ::CreateFile (buffer, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (h == INVALID_HANDLE_VALUE)
		{
			break;
		}
		::CloseHandle (h);
	}
}

bool ATOM_Archive::removeFiles (const char * const *filenames, unsigned size)
{
	if (!opened ())
	{
		return false;
	}

	if (size == 0)
	{
		return true;
	}

	if (!filenames)
	{
		return false;
	}

	std::vector<std::string> filenamesToBeRemoved;

	ATOM_UnZip unzip (_filename);
	if (!unzip.opened())
	{
		return false;
	}

	for (unsigned i = 0; i < size; ++i)
	{
		const char *fn = filenames[i];
		if (fn && unzip.openInZip (fn, false, _passwd))
		{
			filenamesToBeRemoved.push_back (unzip.getCurrentFileName());
			unzip.closeInZip ();
		}
	}

	//if (filenamesToBeRemoved.size() != size)
	//{
	//	return false;
	//}

	char tmpFileName[260];
	genTmpFileName (_filename, tmpFileName);

	ATOM_Zip zip (tmpFileName, false);
	if (!zip.opened ())
	{
		return false;
	}

	const unsigned size_buf = 16384;

	if (!unzip.openFirstInZip (true, _passwd))
	{
		return false;
	}

	for (;;)
	{
		bool skipped = false;
		for (unsigned i = 0; i < filenamesToBeRemoved.size(); ++i)
		{
			if (!strcmp (filenamesToBeRemoved[i].c_str(), unzip.getCurrentFileName()))
			{
				skipped = true;
				break;
			}
		}
		if (!skipped)
		{
			RawWriter writer(&unzip);
			if (!zip.newFile (unzip.getCurrentFileName(), &writer, ATOM_Zip::CM_DEFAULT, 0, true, _passwd))
			{
				return false;
			}
		}
		unzip.closeInZip ();
		if (!unzip.openNextInZip (true, _passwd))
		{
			break;
		}
	}

	unzip.close ();
	zip.close ();

	char tmpFileName2[512];
	genTmpFileName (_filename, tmpFileName2);

	// Backup old archive to tmpFileName2
	if (!::MoveFileA (_filename, tmpFileName2))
	{
		return false;
	}

	// Rename new archive
	if (!::MoveFileA (tmpFileName, _filename))
	{
		::DeleteFileA (tmpFileName);
		::MoveFileA (tmpFileName2, _filename);
		return false;
	}

	::DeleteFileA (tmpFileName2);

	return true;
}

static bool createDirWIN32 (const char *dir)
{
	if (dir)
	{
		char ident[MAX_PATH];
		if (!identifyPathWIN32 (dir, ident))
		{
			return false;
		}

		DWORD attrib = ::GetFileAttributesA (ident);
		if (attrib != 0xFFFFFFFF)
		{
			return false;
		}

		char *p = strchr (ident, '\\');
		if (!p)
		{
			return false;
		}

		while (p)
		{
			++p;
			char tmp = *p;
			*p = '\0';

			attrib = ::GetFileAttributesA (ident);
			if (attrib == 0xFFFFFFFF)
			{
				if (!::CreateDirectory (ident, NULL))
				{
					return false;
				}
				else
				{
					*p = tmp;
					p = strchr(p, '\\');
				}
			}
			else if (!(attrib & FILE_ATTRIBUTE_DIRECTORY))
			{
				return false;
			}
			else
			{
				*p = tmp;
				p = strchr(p, '\\');
			}
		}

		attrib = ::GetFileAttributesA (ident);
		if (attrib != 0xFFFFFFFF && (attrib & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			return true;
		}

		if (!::CreateDirectory (ident, NULL))
		{
			return false;
		}

		return true;
	}

	return false;
}

bool ATOM_Archive::extractFile (const char *filenameDst, const char *filenameSrc)
{
	if (!filenameSrc)
	{
		return false;
	}

	if (!opened ())
	{
		return false;
	}

	ATOM_UnZip unzip (_filename);
	if (!unzip.opened())
	{
		return false;
	}

	if (!unzip.openInZip (filenameSrc, false, _passwd))
	{
		return false;
	}
	unsigned fileSize = unzip.getCurrentFileUncompressedSize ();

	const unsigned size_buf = 16384;
	char buffer[size_buf];

	const char *filenameDst2;
	if (!filenameDst)
	{
		const char *p1 = strrchr (filenameSrc, '/');
		const char *p2 = strrchr (filenameSrc, '\\');
		if (!p1 && !p2)
		{
			filenameDst2 = filenameSrc;
		}
		else if (p1 && !p2)
		{
			filenameDst2 = p1+1;
		}
		else if (!p1 && p2)
		{
			filenameDst2 = p2+1;
		}
		else
		{
			filenameDst2 = (p1 > p2) ? p1+1 : p2+1;
		}
	}
	else
	{
		filenameDst2 = filenameDst;
	}

	if (!identifyPathWIN32 (filenameDst2, buffer))
	{
		return false;
	}
	char *p = strrchr (buffer, '\\');
	if (!p)
	{
		return false;
	}
	*p = '\0';
	if (!isDirectoryWIN32 (buffer) && !createDirWIN32 (buffer))
	{
		return false;
	}
	if (!isDirectoryWIN32 (buffer))
	{
		return false;
	}
	*p = '\\';

	FILE *fp = fopen (buffer, "wb");
	if (!fp)
	{
		return false;
	}

	while (fileSize > 0)
	{
		unsigned readSize = (fileSize < size_buf) ? fileSize : size_buf;
		if (readSize != unzip.readInZip (buffer, readSize))
		{
			fclose (fp);
			return false;
		}
		if (readSize != fwrite (buffer, 1, readSize, fp))
		{
			fclose (fp);
			return false;
		}
		fileSize -= readSize;
	}

	fclose (fp);

	return true;
}

bool ATOM_Archive::extractDirectory (const char *dirDst, const char *dirSrc)
{
	if (!opened ())
	{
		return false;
	}

	char cwd[MAX_PATH];
	char ident[MAX_PATH];
	char ident2[MAX_PATH];

	if (!_getcwd (cwd, MAX_PATH))
	{
		return false;
	}

	if (dirDst)
	{
		if (!identifyPathWIN32 (dirDst, ident))
		{
			return false;
		}
		if (!isDirectoryWIN32 (ident) && !createDirWIN32 (ident))
		{
			return false;
		}
		if (!isDirectoryWIN32 (ident))
		{
			return false;
		}
		if (0 != _chdir (ident))
		{
			return false;
		}
	}

	if (dirSrc)
	{
		ident2[0] = '\0';
		if (dirSrc[0] != '/')
		{
			strcat (ident2, "/");
		}
		strcat (ident2, dirSrc);

		if (!identifyPathZIP (ident2, ident))
		{
			_chdir (cwd);
			return false;
		}

		if (!ident[0])
		{
			_chdir (cwd);
		}

		if (ident[strlen(ident)-1] != '/')
		{
			strcat (ident, "/");
		}
	}
	else
	{
		ident[0] = '/';
		ident[1] = '\0';
	}
	unsigned size_prefix = strlen(ident+1);

	ATOM_UnZip unzip (_filename);
	if (!unzip.opened ())
	{
		_chdir (cwd);
		return false;
	}

	if (unzip.openFirstInZip (false, _passwd))
	{
		do
		{
			if (ident[1] && _strnicmp(unzip.getCurrentFileName(), ident+1, size_prefix))
			{
				unzip.closeInZip ();
				continue;
			}

			const char *fn = unzip.getCurrentFileName();
			if (fn && fn[0] && fn[strlen(fn)-1] != '/')
			{
				printf ("\nExtracting file %s", fn);

				if (!extractFile (fn + size_prefix, fn))
				{
					unzip.closeInZip ();
					_chdir (cwd);
					return false;
				}
			}
			else
			{
				if (!isDirectoryWIN32(fn) && !createDirWIN32 (fn))
				{
					unzip.closeInZip ();
					_chdir (cwd);
					return false;
				}
			}
			unzip.closeInZip ();
		}
		while (unzip.openNextInZip (false, _passwd));
	}

	_chdir (cwd);
	return true;
}

bool ATOM_Archive::iterate (FileIterator *iterator) const
{
	if (!iterator)
	{
		return false;
	}

	if (!opened ())
	{
		return false;
	}

	ATOM_UnZip unzip (_filename);
	if (!unzip.opened ())
	{
		return false;
	}

	if (iterator->begin ())
	{
		if (unzip.openFirstInZip (iterator->rawIterate (), _passwd))
		{
			do
			{
				FileInfo fileInfo;
				fileInfo.fileName = unzip.getCurrentFileName();
				fileInfo.fileComment = unzip.getCurrentFileComment();
				fileInfo.compressedSize = unzip.getCurrentFileCompressedSize();
				fileInfo.uncompressedSize = unzip.getCurrentFileUncompressedSize();
				fileInfo.crc32 = unzip.getCurrentFileCRC();
				unzip.getCurrentFileTime (&fileInfo.time.year, &fileInfo.time.month, &fileInfo.time.day, &fileInfo.time.hour, &fileInfo.time.minute, &fileInfo.time.second);

				bool finished = iterator->process (&unzip, &fileInfo);
				unzip.closeInZip ();
				if (finished)
				{
					break;
				}
			}
			while (unzip.openNextInZip (iterator->rawIterate (), _passwd));
		}

		iterator->end ();
	}

	return true;
}

