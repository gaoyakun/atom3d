#include <string>
#include <map>
#include <stdio.h>
#include "basedefs.h"
#include "zip_wrapper.h"
#include "zip.h"
#include "unzip.h"
#include "iowin32.h"

struct UnzipFileInfo
{
	unz_file_info info;
	unz_file_pos pos;
};

struct ZipInfo
{
	zipFile zip;
	std::string comment;
};

struct UnzipInfo
{
	unzFile zip;
	char *currentFileName;
	char *currentFileComment;
	unsigned sizeUncompressed;
	unsigned sizeCompressed;
	unsigned long crc;
	unsigned year;
	unsigned month;
	unsigned day;
	unsigned hour;
	unsigned minute;
	unsigned second;
};

static bool _writeZip (ZipInfo *info, const void *buffer, unsigned size)
{
	unsigned long size_read = 0;
	const unsigned size_buf = 16384;
	int err;

	if (buffer == 0)
	{
		return false;
	}

	if (size == 0)
	{
		return true;
	}

    do
    {
		size_read = (size < size_buf) ? size : size_buf;
        if (size_read>0)
		{
			err = zipWriteInFileInZip (info->zip, buffer, size_read);
			if (err != ZIP_OK)
			{
				return false;
			}
			size -= size_read;
			buffer = (const char*)buffer + size_read;
		}
    }
	while (size_read > 0);

	return true;
}

bool ATOM_Zip::Writer::writeZip (ATOM_Zip *zip, const void *buffer, unsigned size)
{
	return _writeZip (zip->_info, buffer, size);
}

ATOM_Zip::FileWriter::FileWriter (const char *filename)
{
	_filename = filename ? _strdup(filename) : 0;
}

unsigned ATOM_Zip::FileWriter::getUncompressedSize (void) const
{
	FILE *fp = fopen (_filename, "rb");
	if (!fp)
	{
		return 0;
	}

	fseek (fp, 0, SEEK_END);
	unsigned size = ftell (fp);
	fclose (fp);

	return size;
}

bool ATOM_Zip::FileWriter::getCRC (unsigned long *crc) const
{
	unsigned long calculate_crc=0;
	unsigned long size_read = 0;

	if (!_filename || !crc)
	{
		return false;
	}

	FILE *fp = fopen (_filename, "rb");
	if (!fp)
	{
		return false;
	}

	if (0 != fseek (fp, 0, SEEK_END))
	{
		fclose (fp);
		return false;
	}

	long filesize = ftell (fp);
	fseek (fp, 0, SEEK_SET);
	static const unsigned size_buf = 32768;
	char buffer[size_buf];

    while (filesize > 0)
    {
		size_read = (filesize < size_buf) ? filesize : size_buf;
		if (size_read != fread (buffer, 1, size_read, fp))
		{
			fclose (fp);
			return false;
		}

        if (size_read>0)
		{
            calculate_crc = crc32(calculate_crc, (const Bytef *)buffer, size_read);
		}
		filesize -= size_read;
    }

	fclose (fp);
    *crc = calculate_crc;
    return true;
}

bool ATOM_Zip::FileWriter::write (ATOM_Zip *zip)
{
	if (!_filename)
	{
		return false;
	}

	FILE *fp = fopen (_filename, "rb");
	if (!fp)
	{
		return false;
	}

	static const unsigned size_buf = 32768;
	char buffer[size_buf];

	if (0 != fseek (fp, 0, SEEK_END))
	{
		fclose (fp);
		return false;
	}

	long filesize = ftell (fp);
	fseek (fp, 0, SEEK_SET);
	if (filesize < 0)
	{
		fclose (fp);
		return false;
	}

	while (filesize)
	{
		unsigned size_read = (size_buf < filesize) ? size_buf : filesize;
		if (size_read != fread (buffer, 1, size_read, fp))
		{
			fclose (fp);
			return false;
		}

		if (!writeZip (zip, buffer, size_read))
		{
			fclose (fp);
			return false;
		}
		filesize -= size_read;
	}

	fclose (fp);
	return true;
}

ATOM_Zip::MemWriter::MemWriter (const void *buffer, unsigned size)
{
	_buffer = buffer;
	_size = size;
}

unsigned ATOM_Zip::MemWriter::getUncompressedSize (void) const
{
	return _size;
}

bool ATOM_Zip::MemWriter::getCRC (unsigned long *crc) const
{
	if (!crc || !_buffer)
	{
		return false;
	}

	unsigned long calculate_crc=0;
	unsigned long size_read = 0;
	unsigned size = _size;
	const char *p = (const char *)_buffer;
	const unsigned size_buf = 16384;

	while (size)
    {
		size_read = (size < size_buf) ? size : size_buf;
        if (size_read>0)
		{
            calculate_crc = crc32(calculate_crc, (const Bytef *)p, size_read);
		}
		size -= size_read;
		p += size_read;
    }

    *crc = calculate_crc;
    return true;
}

bool ATOM_Zip::MemWriter::write (ATOM_Zip *zip)
{
	if (!_buffer)
	{
		return false;
	}

	unsigned size = _size;
	const char *p = (const char*)_buffer;
	const unsigned size_buf = 16384;

	while (size)
	{
		unsigned size_read = (size_buf < size) ? size_buf : size;

		if (!writeZip (zip, p, size_read))
		{
			return false;
		}
		size -= size_read;
		p += size_read;
	}

	return true;
}

ATOM_Zip::ATOM_Zip (void)
{
	_info = 0;
}

ATOM_Zip::ATOM_Zip (const char *zipname, bool append, const char *comment)
{
	_info = 0;

	open (zipname, append, comment);
}

ATOM_Zip::~ATOM_Zip (void)
{
	close ();
}

bool ATOM_Zip::open (const char *zipname, bool append, const char *comment)
{
	if (opened ())
	{
		return false;
	}

	if (!zipname)
	{
		return false;
	}

    zlib_filefunc_def ffunc;
    fill_win32_filefunc(&ffunc);

	zipFile zip = zipOpen2 (zipname, append ? APPEND_STATUS_ADDINZIP : APPEND_STATUS_CREATE, 0, &ffunc);
	if (zip)
	{
		_info = new ZipInfo;
		_info->zip = zip;
		_info->comment = comment ? comment : "";
		return true;
	}

	return false;
}

void ATOM_Zip::close (void)
{
	if (_info)
	{
		zipClose (_info->zip, _info->comment.empty() ? 0 : _info->comment.c_str());
		delete _info;
		_info = 0;
	}
}

bool ATOM_Zip::opened (void) const
{
	return _info != 0;
}

static bool filetime(const char *f, tm_zip* tmzip, unsigned long* dt)
{
	WIN32_FIND_DATA ff32;
	HANDLE handle = ::FindFirstFile(f, &ff32);
	bool bOK = (handle != INVALID_HANDLE_VALUE);
	if (bOK)
	{
		FILETIME ftLocal;
		::FileTimeToLocalFileTime(&(ff32.ftLastWriteTime),&ftLocal);
		::FileTimeToDosDateTime(&ftLocal,((LPWORD)dt)+1,((LPWORD)dt)+0);
		::FindClose(handle);
	}
	return bOK;
}

bool ATOM_Zip::newFile (const char *path, const void *buffer, unsigned size, CompressMethod method, const char *comment, bool raw, const char *passwd)
{
	MemWriter writer (buffer, size);

	return newFile (path, &writer, method, comment, raw, passwd);
}

bool ATOM_Zip::newFile (const char *path, const char *filenameSrc, CompressMethod method, const char *comment, bool raw, const char *passwd)
{
	FileWriter writer (filenameSrc);

	return newFile (path, &writer, method, comment, raw, passwd);
}

bool ATOM_Zip::newFile (const char *path, Writer *writer, CompressMethod method, const char *comment, bool raw, const char *passwd)
{
	if (!writer)
	{
		return false;
	}

	if (!opened ())
	{
		return false;
	}

	if (!path)
	{
		return false;
	}

	int level = Z_DEFAULT_COMPRESSION;
	int cm = Z_DEFLATED;

	switch (method)
	{
	case CM_STORE:
		level = Z_NO_COMPRESSION;
		cm = 0;
		break;
	case CM_BEST_COMPRESSION:
		level = Z_BEST_COMPRESSION;
		break;
	case CM_BEST_SPEED:
		level = Z_BEST_SPEED;
		break;
	}

	zip_fileinfo info;
	memset (&info, 0, sizeof(zip_fileinfo));
	SYSTEMTIME time;
	::GetLocalTime (&time);
	info.tmz_date.tm_hour = time.wHour;
	info.tmz_date.tm_mday = time.wDay;
	info.tmz_date.tm_min = time.wMinute;
	info.tmz_date.tm_mon = time.wMonth;
	info.tmz_date.tm_sec = time.wSecond;
	info.tmz_date.tm_year = time.wYear;
	FILETIME ftime;
	::SystemTimeToFileTime (&time, &ftime);
	unsigned long *dt = &info.dosDate;
	::FileTimeToDosDateTime(&ftime,((LPWORD)dt)+1,((LPWORD)dt)+0);

	unsigned long crc32 = 0;
	unsigned size_buf = 16384;

	if ((raw || passwd) && !writer->getCRC (&crc32))
	{
		return false;
	}

	int err = zipOpenNewFileInZip3 (_info->zip, path, &info, 0, 0, 0, 0, comment, cm, level, raw ? 1 : 0, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, passwd, crc32);
	if (err != ZIP_OK)
	{
		return false;
	}

	if (!writer->write (this))
	{
		return false;
	}

	err = raw ? zipCloseFileInZipRaw (_info->zip, writer->getUncompressedSize(), crc32) : zipCloseFileInZip (_info->zip);
	if (ZIP_OK != err)
	{
		return false;
	}

	return true;
}

bool ATOM_Zip::newDir (const char *path)
{
	if (!opened ())
	{
		return false;
	}

	if (!path)
	{
		return false;
	}

	zip_fileinfo info;
	memset (&info, 0, sizeof(zip_fileinfo));
	SYSTEMTIME time;
	::GetLocalTime (&time);
	info.tmz_date.tm_hour = time.wHour;
	info.tmz_date.tm_mday = time.wDay;
	info.tmz_date.tm_min = time.wMinute;
	info.tmz_date.tm_mon = time.wMonth;
	info.tmz_date.tm_sec = time.wSecond;
	info.tmz_date.tm_year = time.wYear;
	FILETIME ftime;
	::SystemTimeToFileTime (&time, &ftime);
	unsigned long *dt = &info.dosDate;
	::FileTimeToDosDateTime(&ftime,((LPWORD)dt)+1,((LPWORD)dt)+0);

	int err = zipOpenNewFileInZip3 (_info->zip, path, &info, 0, 0, 0, 0, 0, CM_STORE, Z_NO_COMPRESSION, 0, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, 0, 0);
	if (err != ZIP_OK)
	{
		return false;
	}

	if (ZIP_OK != zipCloseFileInZip (_info->zip))
	{
		return false;
	}

	return true;
}

ATOM_UnZip::ATOM_UnZip (void)
{
	_info = 0;
}

ATOM_UnZip::ATOM_UnZip (const char *zipname)
{
	_info = 0;

	open (zipname);
}

ATOM_UnZip::~ATOM_UnZip (void)
{
	close ();
}

bool ATOM_UnZip::open (const char *zipname)
{
	if (zipname)
	{
		zlib_filefunc_def ffunc;
		fill_win32_filefunc(&ffunc);

		unzFile zip = unzOpen2 (zipname, &ffunc);
		if (!zip)
		{
			return false;
		}

		_info = new UnzipInfo;
		_info->zip = zip;
		_info->currentFileComment = 0;
		_info->currentFileName = 0;
		_info->sizeUncompressed = 0;
		_info->sizeCompressed = 0;
		_info->crc = 0;

		return true;
	}

	return false;
}

void ATOM_UnZip::close (void)
{
	if (opened ())
	{
		closeInZip ();

		unzClose (_info->zip);
		delete _info;
		_info = 0;
	}
}

bool ATOM_UnZip::opened (void) const
{
	return _info != 0;
}

static bool retrieveFileInfo (UnzipInfo *info)
{
	unz_file_info fileInfo;
	int err;

	err = unzGetCurrentFileInfo (info->zip, &fileInfo, 0, 0, 0, 0, 0, 0);
	if (err != UNZ_OK)
	{
		return false;
	}

	info->currentFileName = (char*)::realloc (info->currentFileName, fileInfo.size_filename + 1);
	if (fileInfo.size_file_comment > 0)
	{
		info->currentFileComment = (char*)::realloc (info->currentFileComment, fileInfo.size_file_comment + 1);
	}
	else if (info->currentFileComment)
	{
		::free (info->currentFileComment);
		info->currentFileComment = 0;
	}

	err = unzGetCurrentFileInfo (info->zip, &fileInfo, info->currentFileName, fileInfo.size_filename + 1, 0, 0, info->currentFileComment, fileInfo.size_file_comment);
	if (err != UNZ_OK)
	{
		if (info->currentFileComment)
		{
			::free (info->currentFileComment);
			info->currentFileComment = 0;
		}

		if (info->currentFileName)
		{
			::free (info->currentFileName);
			info->currentFileName = 0;
		}
		return false;
	}

	info->sizeCompressed = fileInfo.compressed_size;
	info->sizeUncompressed = fileInfo.uncompressed_size;
	info->crc = fileInfo.crc;
	info->year = fileInfo.tmu_date.tm_year;
	info->month = fileInfo.tmu_date.tm_mon;
	info->day = fileInfo.tmu_date.tm_mday;
	info->hour = fileInfo.tmu_date.tm_hour;
	info->minute = fileInfo.tmu_date.tm_min;
	info->second = fileInfo.tmu_date.tm_sec;

	return true;
}

bool ATOM_UnZip::openFirstInZip (bool raw, const char *passwd)
{
	if (opened ())
	{
		int err = unzGoToFirstFile (_info->zip);
		if (err != UNZ_OK)
		{
			return false;
		}
		int method;
		int level;
		err = unzOpenCurrentFile3 (_info->zip, &method, &level, raw ? 1 : 0, passwd);

		if (err != UNZ_OK)
		{
			return false;
		}

		if (!retrieveFileInfo (_info))
		{
			unzCloseCurrentFile (_info->zip);
			return false;
		}

		return true;
	}

	return false;
}

bool ATOM_UnZip::openNextInZip (bool raw, const char *passwd)
{
	if (opened ())
	{
		int err = unzGoToNextFile (_info->zip);
		if (err != UNZ_OK)
		{
			return false;
		}
		int method;
		int level;
		err = unzOpenCurrentFile3 (_info->zip, &method, &level, raw ? 1 : 0, passwd);

		if (err != UNZ_OK)
		{
			return false;
		}

		if (!retrieveFileInfo (_info))
		{
			unzCloseCurrentFile (_info->zip);
			return false;
		}

		return true;
	}

	return false;
}

bool ATOM_UnZip::openInZip (const char *path, bool raw, const char *passwd)
{
	if (opened ())
	{
		int err = unzLocateFile (_info->zip, path, 2);
		if (err != UNZ_OK)
		{
			return false;
		}

		int method;
		int level;
		err = unzOpenCurrentFile3 (_info->zip, &method, &level, raw ? 1 : 0, passwd);

		if (err != UNZ_OK)
		{
			return false;
		}

		if (!retrieveFileInfo (_info))
		{
			unzCloseCurrentFile (_info->zip);
			return false;
		}

		return true;
	}

	return false;
}

const char *ATOM_UnZip::getCurrentFileName (void) const
{
	return opened () ? _info->currentFileName : 0;
}

const char *ATOM_UnZip::getCurrentFileComment (void) const
{
	return opened () ? _info->currentFileComment : 0;
}

unsigned ATOM_UnZip::getCurrentFileUncompressedSize (void) const
{
	return opened () ? _info->sizeUncompressed : 0;
}

unsigned ATOM_UnZip::getCurrentFileCompressedSize (void) const
{
	return opened () ? _info->sizeCompressed : 0;
}

unsigned long ATOM_UnZip::getCurrentFileCRC (void) const
{
	return opened () ? _info->crc : 0;
}

bool ATOM_UnZip::getCurrentFileTime (unsigned *year, unsigned *month, unsigned *day, unsigned *hour, unsigned *minute, unsigned *second) const
{
	if (opened ())
	{
		if (year) *year = _info->year;
		if (month) *month = _info->month;
		if (day) *day = _info->day;
		if (hour) *hour = _info->hour;
		if (minute) *minute = _info->minute;
		if (second) *second = _info->second;
		return true;
	}
	else
	{
		return false;
	}
}

void ATOM_UnZip::closeInZip (void)
{
	if (opened ())
	{
		unzCloseCurrentFile (_info->zip);

		if (_info->currentFileName)
		{
			::free (_info->currentFileName);
			_info->currentFileName = 0;
		}

		if (_info->currentFileComment)
		{
			::free (_info->currentFileComment);
			_info->currentFileComment = 0;
		}

		_info->sizeUncompressed = 0;
		_info->sizeCompressed = 0;
	}
}

unsigned ATOM_UnZip::readInZip (void *buffer, unsigned size)
{
	if (opened ())
	{
		int err = unzReadCurrentFile (_info->zip, buffer, size);

		return (err > 0) ? err : 0;
	}

	return 0;
}

