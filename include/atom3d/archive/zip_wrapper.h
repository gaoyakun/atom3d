#ifndef __ATOM3D_ARCHIVE_ZIP_WRAPPER_H
#define __ATOM3D_ARCHIVE_ZIP_WRAPPER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

class ATOM_Zip
{
	friend class Writer;

public:
	ATOM_Zip (void);
	ATOM_Zip (const char *zipname, bool append, const char *comment = 0);
	virtual ~ATOM_Zip (void);

	enum CompressMethod
	{
		CM_STORE,
		CM_BEST_SPEED,
		CM_BEST_COMPRESSION,
		CM_DEFAULT
	};

	class Writer
	{
	public:
		virtual bool write (ATOM_Zip *zip) = 0;
		virtual bool getCRC (unsigned long *crc) const = 0;
		virtual unsigned getUncompressedSize (void) const = 0;

	protected:
		bool writeZip (ATOM_Zip *zip, const void *buffer, unsigned size);
	};

	class FileWriter: public Writer
	{
	public:
		FileWriter (const char *filename);

	private:
		FileWriter (const FileWriter&);
		FileWriter & operator = (const FileWriter&);

	public:
		virtual bool write (ATOM_Zip *zip);
		virtual bool getCRC (unsigned long *crc) const;
		virtual unsigned getUncompressedSize (void) const;

	private:
		char *_filename;
	};

	class MemWriter: public Writer
	{
	public:
		MemWriter (const void *buffer, unsigned size);
		virtual bool getCRC (unsigned long *crc) const;
		virtual unsigned getUncompressedSize (void) const;

	public:
		virtual bool write (ATOM_Zip *zip);

	private:
		const void *_buffer;
		unsigned _size;
	};

private:
	ATOM_Zip (const ATOM_Zip &);
	ATOM_Zip & operator = (const ATOM_Zip &);

public:
	bool open (const char *zipname, bool append, const char *comment = 0);
	void close (void);
	bool opened (void) const;
	bool newFile (const char *path, const char *filenameSrc, CompressMethod method = CM_DEFAULT, const char *comment = 0, bool raw = false, const char *passwd = 0);
	bool newFile (const char *path, const void *buffer, unsigned size, CompressMethod method = CM_DEFAULT, const char *comment = 0, bool raw = false, const char *passwd = 0);
	bool newFile (const char *path, Writer *writer, CompressMethod method = CM_DEFAULT, const char *comment = 0, bool raw = false, const char *passwd = 0);
	bool newDir (const char *path);

private:
	struct ZipInfo *_info;
};

class ATOM_UnZip
{
public:
	ATOM_UnZip (void);
	ATOM_UnZip (const char *zipname);
	virtual ~ATOM_UnZip (void);

private:
	ATOM_UnZip (const ATOM_UnZip &);
	ATOM_UnZip & operator = (const ATOM_UnZip &);

public:
	bool open (const char *zipname);
	void close (void);
	bool opened (void) const;

	bool openFirstInZip (bool raw = false, const char *passwd = 0);
	bool openNextInZip (bool raw = false, const char *passwd = 0);
	bool openInZip (const char *filename, bool raw = false, const char *passwd = 0);
	void closeInZip (void);
	const char *getCurrentFileName (void) const;
	const char *getCurrentFileComment (void) const;
	unsigned getCurrentFileUncompressedSize (void) const;
	unsigned getCurrentFileCompressedSize (void) const;
	unsigned long getCurrentFileCRC (void) const;
	bool getCurrentFileTime (unsigned *year, unsigned *month, unsigned *day, unsigned *hour, unsigned *minute, unsigned *second) const;
	unsigned readInZip (void *buffer, unsigned size);

private:
	struct UnzipInfo *_info;
};

#endif // __ATOM3D_ARCHIVE_ZIP_WRAPPER_H
