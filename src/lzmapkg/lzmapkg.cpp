#include <stdlib.h>
#include <vector>
#include <Windows.h>
#include "lzmadec.h"
#include "lzmaenc.h"
#include "lzmapkg.h"

std::string identifyPath (const char *dir)
{
	char ident[MAX_PATH];
	if (!::GetFullPathNameA (dir, MAX_PATH, ident, NULL))
	{
		return "";
	}
	return ident;
}

static bool mkDir (const char *dir)
{
	if (dir)
	{
		char ident[MAX_PATH];
		std::string identStr = identifyPath (dir);
		strcpy (ident, identStr.c_str());
		DWORD attrib = ::GetFileAttributesA (ident);
		if (attrib != 0xFFFFFFFF)
		{
			return (attrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
		}
		char *p = strchr (ident, '\\');
		while (p)
		{
			++p;
			char tmp = *p;
			*p = '\0';

			attrib = ::GetFileAttributesA (ident);
			if (attrib == 0xFFFFFFFF)
			{
				if (!::CreateDirectoryA (ident, NULL))
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

		if (!::CreateDirectoryA (ident, NULL))
		{
			return false;
		}

		return true;
	}

	return false;
}

typedef struct
{
  void *(*Alloc)(void *p, size_t size);
  void (*Free)(void *p, void *address); /* address can be 0 */
} ISzAlloc;

static void *AllocFunc (void *p, size_t size)
{
	return malloc(size);
}

static void FreeFunc (void *p, void *address)
{
	free(address);
}

static crnlib::Byte cprops[LZMA_PROPS_SIZE];
static crnlib::SizeT sizeProps = LZMA_PROPS_SIZE;

struct CompressHeader
{
	unsigned compressedSize;
	unsigned uncompressedSize;
	crnlib::Byte props[LZMA_PROPS_SIZE];
};

#undef max
#define max(a, b) ((a)>(b)?(a):(b))
unsigned LZMAGetMaxCompressedSize (unsigned srcLen)
{
	return srcLen + max(128, srcLen >> 8) + sizeof(CompressHeader);
}

unsigned LZMACompress (const void *src, unsigned srcLen, void *dest, unsigned destLen)
{
	CompressHeader *header = (CompressHeader*)dest;
	crnlib::Byte *dst = ((crnlib::Byte*)dest)+sizeof(CompressHeader);
	crnlib::SizeT dstLen = destLen - sizeof(CompressHeader);

	crnlib::ISzAlloc alloc = { &AllocFunc, &FreeFunc };
	crnlib::CLzmaEncProps props;
	crnlib::LzmaEncProps_Init (&props);
	props.level = 9; // 0 <= level <= 9 default 5
	crnlib::SizeT propSize = LZMA_PROPS_SIZE;

	if (SZ_OK != crnlib::LzmaEncode (dst, &dstLen, (const crnlib::Byte*)src, srcLen, &props, header->props, &propSize, 0, 0, &alloc, &alloc))
	{
		return 0;
	}

	header->compressedSize = dstLen;
	header->uncompressedSize = srcLen;

	return header->compressedSize + sizeof(CompressHeader);
}

unsigned LZMADecompress (const void *src, unsigned srcLen, void *dest, unsigned destLen)
{
	crnlib::ISzAlloc alloc = { &AllocFunc, &FreeFunc };
	CompressHeader *header = (CompressHeader*)src;
	const crnlib::Byte *srcData = ((const crnlib::Byte*)src)+sizeof(CompressHeader);
	crnlib::SizeT srcDataLen = srcLen - sizeof(CompressHeader);

	crnlib::SizeT dstLen = destLen;
	crnlib::SizeT sourceLen = srcLen;
	crnlib::ELzmaStatus status;
	LzmaDecode((crnlib::Byte*)dest, &dstLen, srcData, &srcDataLen, header->props, LZMA_PROPS_SIZE, crnlib::LZMA_FINISH_ANY, &status, &alloc);

	return dstLen;
}

unsigned LZMAGetUncompressedSize (const void *compressedData)
{
	CompressHeader *header = (CompressHeader*)compressedData;
	return header->uncompressedSize;
}

ATOM_LzmaFilePackage::ATOM_LzmaFilePackage (void)
{
}

ATOM_LzmaFilePackage::~ATOM_LzmaFilePackage (void)
{
}

void ATOM_LzmaFilePackage::addFolder (const char *folderPath)
{
	std::string path = identifyPath (folderPath);
	if (path.empty())
	{
		return;
	}
	iterateFolderR (path.c_str(), path.c_str());
}

bool ATOM_LzmaFilePackage::addFile (const char *filename, const char *inPkgName)
{
	std::string fn = identifyPath (filename);
	if (_fileNameMap.find (fn) != _fileNameMap.end())
	{
		return false;
	}
	_fileNameMap[fn] = inPkgName;
	return true;
}

static unsigned getFileSize (const char *filename)
{
	FILE *fp = fopen (filename, "rb");
	if (!fp)
	{
		return 0;
	}
	fseek (fp, 0, SEEK_END);
	unsigned size = ftell (fp);
	fclose (fp);

	return size;
}

struct FileHeader
{
	unsigned sig;
	unsigned ver;
	unsigned num;
	unsigned padding;
};

struct FileEntry
{
	unsigned entrySize;
	unsigned dataOffset;
	unsigned dataSize;
	unsigned nameLength;
	char name[1];
};

class FileEntryList: public std::vector<FileEntry*> 
{
public:
	FileEntryList (void) {}
	~FileEntryList (void) 
	{
		for (iterator it = begin(); it != end(); ++it)
		{
			free (*it);
		}
		clear ();
	}
};

bool ATOM_LzmaFilePackage::compress (const char *destFileName)
{
	FileHeader header;
	header.sig = 0xAAAABBBB;
	header.ver = 0x1;
	header.num = _fileNameMap.size();
	header.padding = 0;

	FileEntryList entries;

	unsigned entryTableSize = 0;
	for (std::map<std::string, std::string>::const_iterator it = _fileNameMap.begin(); it != _fileNameMap.end(); ++it)
	{
		unsigned nameLength = (it->second.length() + 1 + 15) & ~15; // make 16 bytes alignment
		unsigned entrySize = 4*sizeof(unsigned)+nameLength;
		entries.push_back ((FileEntry*)malloc(entrySize));
		entries.back()->entrySize = entrySize;
		entries.back()->dataOffset = 0;
		entries.back()->dataSize = 0;
		entries.back()->nameLength = nameLength;
		strcpy (entries.back()->name, it->second.c_str());

		entryTableSize += entrySize;
	}

	char tmpFileName[MAX_PATH];
	::GetTempFileNameA (".", "_~", 0, tmpFileName);
	FILE *fpTmp = fopen (tmpFileName, "wb");
	if (!fpTmp)
	{
		// TODO: Memory leak
		return false;
	}

	unsigned index = 0;
	unsigned offset = sizeof(FileHeader) + entryTableSize;
	for (std::map<std::string, std::string>::const_iterator it = _fileNameMap.begin(); it != _fileNameMap.end(); ++it, ++index)
	{
		const char *filename = it->first.c_str();
		FILE *fp = fopen (filename, "rb");
		if (!fp)
		{
			// TODO: Memory leak
			return false;
		}
		fseek (fp, 0, SEEK_END);
		unsigned fileSize = ftell (fp);
		fseek (fp, 0, SEEK_SET);
		unsigned maxBufferSize = (LZMAGetMaxCompressedSize (fileSize) + 15) & ~15;
		char *buffer = new char[maxBufferSize];
		char *src = new char[fileSize];
		if (fread (src, 1, fileSize, fp) != fileSize)
		{
			fclose (fp);
			// TODO: Memory leak
			return false;
		}
		fclose (fp);

		entries[index]->dataOffset = offset;
		unsigned dataSize = LZMACompress (src, fileSize, buffer, maxBufferSize);
		if (dataSize == 0)
		{
			dataSize = fileSize;
			entries[index]->dataSize = dataSize|0x80000000;
			if (fwrite (src, 1, fileSize, fpTmp) != fileSize)
			{
				return false;
			}
			offset += fileSize;
		}
		else
		{
			entries[index]->dataSize = dataSize;
			if (fwrite (buffer, 1, dataSize, fpTmp) != dataSize)
			{
				return false;
			}
			offset += dataSize;
		}

		delete [] buffer;
		delete [] src;
	}
	fclose (fpTmp);

	FILE *destFp = fopen (destFileName, "wb");
	if (!destFp)
	{
		// Memory leak
		return false;
	}

	// write header
	if (fwrite (&header, 1, sizeof(FileHeader), destFp) != sizeof(FileHeader))
	{
		// Memory leak
		fclose (destFp);
		return false;
	}

	// write entries
	for (unsigned i = 0; i < entries.size(); ++i)
	{
		if (fwrite (entries[i], 1, entries[i]->entrySize, destFp) != entries[i]->entrySize)
		{
			// Memory leak
			fclose (destFp);
			return false;
		}
	}

	// write file contents
	fpTmp = fopen (tmpFileName, "rb");
	if (!fpTmp)
	{
		// Memory leak;
		fclose (destFp);
		return false;
	}

	fseek (fpTmp, 0, SEEK_END);
	unsigned contentSize = ftell (fpTmp);
	fseek (fpTmp, 0, SEEK_SET);

	const unsigned bufferSize = 16 * 1024 * 1024;
	unsigned bytesWritten = 0;
	char *dataBuffer = (char*)malloc(bufferSize);

	while (bytesWritten != contentSize)
	{
		unsigned bytesToRead = contentSize - bytesWritten;
		if (bytesToRead > bufferSize)
		{
			bytesToRead = bufferSize;
		}
		if (bytesToRead != fread (dataBuffer, 1, bytesToRead, fpTmp))
		{
			// Memory leak
			fclose (fpTmp);
			fclose (destFp);
			free(dataBuffer);
			return false;
		}
		if (bytesToRead != fwrite (dataBuffer, 1, bytesToRead, destFp))
		{
			// Memoryleak
			fclose (fpTmp);
			fclose (destFp);
			free(dataBuffer);
			return false;
		}
		bytesWritten += bytesToRead;
	}

	fclose (fpTmp);
	fclose (destFp);
	free(dataBuffer);
	::DeleteFileA (tmpFileName);

	return true;
}

void ATOM_LzmaFilePackage::getCompressedFileInfoList (const char *pkgFileName, std::vector<CompressedFileInfo> &infos)
{
	infos.resize (0);

	FILE *fp = fopen (pkgFileName, "rb");
	if (!fp)
	{
		return;
	}

	FileHeader header;
	if (fread (&header, 1, sizeof(FileHeader), fp) != sizeof(FileHeader))
	{
		fclose (fp);
		return;
	}

	std::vector<FileEntry*> entries;

	for (unsigned i = 0; i < header.num; ++i)
	{
		unsigned entrySize;
		if (fread (&entrySize, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			fclose (fp);
			return;
		}
		entries.push_back ((FileEntry*)malloc(entrySize));
		entries.back()->entrySize = entrySize;
		if (fread (&entries.back()->dataOffset, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			fclose (fp);
			return;
		}
		if (fread (&entries.back()->dataSize, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			fclose (fp);
			return;
		}
		if (fread (&entries.back()->nameLength, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			fclose (fp);
			return;
		}
		if (fread (entries.back()->name, 1, entries.back()->nameLength, fp) != entries.back()->nameLength)
		{
			fclose (fp);
			return;
		}
	}

	char buffer[256];
	for (int i = 0; i < (int)entries.size(); ++i)
	{
		CompressedFileInfo info;
		info.compressed = true;
		info.size = entries[i]->dataSize;
		if ((info.size & 0x80000000) != 0)
		{
			info.compressed = false;
			info.size &= 0x7FFFFFFF;
		}
		sprintf (buffer, "/%s", entries[i]->name);
		for (int j = 0;j < (int)strlen(buffer); ++j)
		{
			if (buffer[j] == '\\') 
				buffer[j] = '/';
		}
		info.filename = buffer;
		info.pos = entries[i]->dataOffset;
		infos.push_back (info);

		free (entries[i]);
	}

	fclose (fp);
}

bool ATOM_LzmaFilePackage::decompress (const char *pkgFileName, const char *destFolder)
{
	std::string dir = destFolder;
	if (dir.empty ())
	{
		return false;
	}
	if (dir.at(dir.length()-1) != '\\')
	{
		dir += '\\';
	}

	FILE *fp = fopen (pkgFileName, "rb");
	if (!fp)
	{
		return false;
	}

	FileHeader header;
	if (fread (&header, 1, sizeof(FileHeader), fp) != sizeof(FileHeader))
	{
		fclose (fp);
		return false;
	}

	std::vector<FileEntry*> entries;

	for (unsigned i = 0; i < header.num; ++i)
	{
		unsigned entrySize;
		if (fread (&entrySize, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			fclose (fp);
			return false;
		}
		entries.push_back ((FileEntry*)malloc(entrySize));
		entries.back()->entrySize = entrySize;
		if (fread (&entries.back()->dataOffset, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			fclose (fp);
			return false;
		}
		if (fread (&entries.back()->dataSize, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			fclose (fp);
			return false;
		}
		if (fread (&entries.back()->nameLength, 1, sizeof(unsigned), fp) != sizeof(unsigned))
		{
			fclose (fp);
			return false;
		}
		if (fread (entries.back()->name, 1, entries.back()->nameLength, fp) != entries.back()->nameLength)
		{
			fclose (fp);
			return false;
		}
	}

	for (unsigned i = 0; i < entries.size(); ++i)
	{
		if (fseek (fp, entries[i]->dataOffset, SEEK_SET))
		{
			fclose (fp);
			return false;
		}
		bool compressed = true;
		if ((entries[i]->dataSize & 0x80000000) != 0)
		{
			compressed = false;
			entries[i]->dataSize &= 0x7FFFFFFF;
		}
		char *compressedBuffer = (char*)malloc(entries[i]->dataSize);
		if (fread (compressedBuffer, 1, entries[i]->dataSize, fp) != entries[i]->dataSize)
		{
			fclose (fp);
			return false;
		}
		char *uncompressedBuffer;
		unsigned uncompressedSize;
		if (compressed)
		{
			uncompressedSize = LZMAGetUncompressedSize (compressedBuffer);
			uncompressedBuffer = (char*)malloc(uncompressedSize);
			if (!uncompressedBuffer)
			{
				fclose (fp);
				return false;
			}
			if (LZMADecompress (compressedBuffer, entries[i]->dataSize, uncompressedBuffer, uncompressedSize) != uncompressedSize)
			{
				fclose (fp);
				return false;
			}
		}
		else
		{
			uncompressedSize = entries[i]->dataSize;
			uncompressedBuffer = compressedBuffer;
		}

		char destFileName[MAX_PATH];
		char destFilePath[MAX_PATH];
		strcpy (destFileName, dir.c_str());
		strcat (destFileName, entries[i]->name);
		strcpy (destFilePath, destFileName);
		char *p = strrchr (destFilePath, '\\');
		if (!p)
		{
			fclose (fp);
			return false;
		}
		*p = '\0';

		if (!mkDir (destFilePath))
		{
			fclose (fp);
			return false;
		}

		FILE *destFp = fopen (destFileName, "wb");
		if (!destFp)
		{
			fclose (fp);
			return false;
		}

		if (fwrite (uncompressedBuffer, 1, uncompressedSize, destFp) != uncompressedSize)
		{
			fclose (destFp);
			fclose (fp);
			return false;
		}

		fclose (destFp);

		free (compressedBuffer);
		if (uncompressedBuffer != compressedBuffer)
		{
			free (uncompressedBuffer);
		}
	}

	fclose (fp);
	return true;
}

void ATOM_LzmaFilePackage::iterateFolderR (const char *folderPath, const char *rootPath)
{
	std::string root = rootPath;
	if (root.empty())
	{
		return;
	}
	if (root.at(root.length()-1) != '\\')
	{
		root += '\\';
	}

	std::string s = folderPath;
	if (s.empty ())
	{
		return;
	}
	if (s.at(s.length()-1) != '\\')
	{
		s += '\\';
	}

	std::string sFilter = s + '*';

	WIN32_FIND_DATAA findData;
	HANDLE hFind = ::FindFirstFileA (sFilter.c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}

	for (;;)
	{
		// skip '.' or '..' folder
		if (!strcmp (findData.cFileName, ".") || !strcmp (findData.cFileName, ".."))
		{
			if (!::FindNextFileA (hFind, &findData))
			{
				::FindClose (hFind);
				return;
			}
		}
		else
		{
			break;
		}
	}

	for (;;)
	{
		bool isDir = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
		std::string fileName = identifyPath ((s + findData.cFileName).c_str());

		if (isDir)
		{
			iterateFolderR (fileName.c_str(), rootPath);
		}
		else
		{
			const char *inPkgFileName = fileName.c_str() + root.length();	
			addFile (fileName.c_str(), inPkgFileName);
		}

		if (!::FindNextFileA (hFind, &findData))
		{
			::FindClose (hFind);
			return;
		}
	}
}

