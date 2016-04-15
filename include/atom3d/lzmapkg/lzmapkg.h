#ifndef __LZMAPKG_H
#define __LZMAPKG_H

#include <map>
#include <string>
#include <vector>

unsigned LZMAGetUncompressedSize (const void *compressedData);
unsigned LZMAGetMaxCompressedSize (unsigned srcLen);
unsigned LZMACompress (const void *src, unsigned srcLen, void *dest, unsigned destLen);
unsigned LZMADecompress (const void *src, unsigned srcLen, void *dest, unsigned destLen);

class ATOM_LzmaFilePackage
{
public:
	struct CompressedFileInfo
	{
		unsigned pos;
		unsigned size;
		int compressed;
		std::string filename;
	};

public:
	ATOM_LzmaFilePackage (void);
	~ATOM_LzmaFilePackage (void);

public:
	void addFolder (const char *folderPath);
	bool addFile (const char *filename, const char *inPkgName);
	bool compress (const char *destFileName);
	bool decompress (const char *pkgFileName, const char *destFolder);
	void getCompressedFileInfoList (const char *pkgFileName, std::vector<CompressedFileInfo> &infos);

private:
	void iterateFolderR (const char *folderPath, const char *rootPath);
	std::map<std::string, std::string> _fileNameMap;
};

#endif // __LZMAPKG_H
