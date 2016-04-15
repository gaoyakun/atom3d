#ifndef __ATOM_VFS_LZMAVFS_H
#define __ATOM_VFS_LZMAVFS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_lzmapkg.h"
#include "vfs.h"

class ATOM_VFS_API ATOM_LzmaVFS: public ATOM_VFS
{
protected:
	struct filedesc_lzma_t: public ATOM_VFS::filedesc_t
	{
		int mode;
		int position;
		void *data;
		int size;
	};

public:
	ATOM_LzmaVFS (void);
	virtual ~ATOM_LzmaVFS (void);
	virtual bool load (const char *filename, const char *password = 0);
	virtual void unload (void);
	virtual bool isCaseSensitive (void) const;
	virtual ATOM_VFS::finddata_t *findFirst (const char *dir);
	virtual bool findNext (ATOM_VFS::finddata_t *handle);
	virtual void findClose (ATOM_VFS::finddata_t *handle);
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
	virtual char getSeperator (void) const;
	virtual bool doesFileExists (const char *file);
	virtual void getCWD (char *buffer) const;
	virtual bool chDir (const char *dir);
	virtual char *identifyPath (const char *dir, char *buffer) const;
	virtual ATOM_VFS *getNativePath (const char *mount_path, char *buffer) const;

protected:
	virtual ATOM_VFS::filedesc_t *_newFileDesc (void);
	virtual void _deleteFileDesc (ATOM_VFS::filedesc_t *fd);

private:
	ATOM_Mutex _M_mutex;
	ATOM_STRING _filename;
	FILE *_fp;
	ATOM_HASHMAP<ATOM_STRING, ATOM_LzmaFilePackage::CompressedFileInfo> _infos;
};
#endif // __ATOM_VFS_LZMAVFS_H

