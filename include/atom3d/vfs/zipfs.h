#ifndef __ATOM_VFS_ZIPVFS_H
#define __ATOM_VFS_ZIPVFS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "memvfs.h"

class ATOM_VFS_API ATOM_ZipVFS: public ATOM_MemVFS
{
protected:
  struct filedesc_zip_t: public ATOM_MemVFS::filedesc_mem_t
  {
    int mode;
    int position;
    void *data;
    int size;
  };

public:
  ATOM_ZipVFS (void);
  ATOM_ZipVFS (bool bCaseSensitive);
  virtual ~ATOM_ZipVFS (void);
  virtual bool load (const char *filename, const char *password = 0);
  virtual void unload (void);
  virtual bool isCaseSensitive (void) const;
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

protected:
  virtual ATOM_VFS::filedesc_t *_newFileDesc (void);
  virtual void _deleteFileDesc (ATOM_VFS::filedesc_t *fd);

private:
  bool _rmdir_r (const char *dir);

private:
  struct ZipImpl *_zip_impl;
  bool _case_sensitive;
  char *_password;
};
#endif // __ATOM_VFS_ZIPVFS_H

