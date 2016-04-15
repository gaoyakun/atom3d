#ifndef __ATOM_VFS_HTTPVFS_H
#define __ATOM_VFS_HTTPVFS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_net.h"
#include "memvfs.h"

class ATOM_VFS_API ATOM_PhysicVFS;

class ATOM_VFS_API ATOM_HttpVFS: public ATOM_MemVFS
{
public:
	typedef int (*FuncDownloadHandler)(ATOM_DownloadContext*, void*);
	typedef int (*FuncCheckFile)(const char *filename, int checksum);

public:
	ATOM_HttpVFS (const char *domain, const char *domain2 = nullptr, const char *cachePath = nullptr, const char *version = nullptr, FuncCheckFile funcCheckFile = nullptr);
	virtual ~ATOM_HttpVFS (void);
	virtual ATOM_VFS::finddata_t *findFirst (const char *dir);
	virtual bool findNext (ATOM_VFS::finddata_t *handle);
	virtual bool mkDir (const char *dir);
	virtual bool isDir (const char *dir);
	virtual bool removeFile (const char *file, bool force);
	virtual ATOM_VFS::handle vfopen (const char *filename, int mode);
	virtual unsigned vfread (ATOM_VFS::handle f, void *buffer, unsigned size);
	virtual unsigned vfwrite (ATOM_VFS::handle f, const void *buffer, unsigned size);
	virtual long vfseek (ATOM_VFS::handle f, long offset, int origin);
	virtual long vftell (ATOM_VFS::handle f);
	virtual long vfsize (ATOM_VFS::handle f);
	virtual void vfclose (ATOM_VFS::handle f);
	virtual bool vfeof (ATOM_VFS::handle f);
	virtual ATOM_VFS *getNativePath (const char *mount_path, char *buffer) const;
	virtual bool doesFileExists (const char *file);

public:
	void setDownloadHandler (FuncDownloadHandler handler, void *userData);
	void enableCompressMode (bool enable);
	ATOM_VFS::handle _tryopen (const char *domain, const char *filename, int mode, bool force);

private:
	ATOM_STRING _domain;
	ATOM_STRING _domain2;
	ATOM_STRING _cachePath;
	ATOM_STRING _version;
	ATOM_PhysicVFS *_cachedVFS;
	ATOM_Mutex _ioLock;
	FuncDownloadHandler _downloadHandler;
	int _handlerInExecuting;
	bool _compressed;
	void *_downloaderHandlerUserData;
	FuncCheckFile _funcCheckFile;
};
#endif // __ATOM_VFS_HTTPVFS_H


