#ifndef __ATOM_VFS_PHYSICVFS_H
#define __ATOM_VFS_PHYSICVFS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "vfs.h"

class ATOM_VFS_API ATOM_PhysicVFS: public ATOM_VFS
{
protected:
  struct finddata_physic_t: public ATOM_VFS::finddata_t
  {
    void *finddata;
    void *findhandle;
    unsigned pathlen;
  };

protected:
  struct filedesc_physic_t: public ATOM_VFS::filedesc_t
  {
    int fd;
  };

public:
  ATOM_PhysicVFS (void);
  virtual ~ATOM_PhysicVFS (void);
  virtual bool isCaseSensitive (void) const;
  virtual char getSeperator (void) const;
  virtual ATOM_VFS::finddata_t *findFirst (const char *dir);
  virtual bool findNext (ATOM_VFS::finddata_t *handle);
  virtual void findClose (ATOM_VFS::finddata_t *handle);
  virtual bool doesFileExists (const char *file);
  virtual bool isDir (const char *file);
  virtual bool mkDir (const char *dir);
  virtual bool removeFile (const char *file, bool force);
  virtual void getCWD (char *buffer) const;
  virtual bool chDir (const char *dir);
  virtual char *identifyPath (const char *dir, char *buffer) const;
  virtual ATOM_VFS *getNativePath (const char *mount_path, char *buffer) const;
  virtual ATOM_VFS::handle vfopen (const char *filename, int mode);
  virtual unsigned vfread (ATOM_VFS::handle f, void *buffer, unsigned size);
  virtual unsigned vfwrite (ATOM_VFS::handle f, const void *buffer, unsigned size);
  virtual long vfseek (ATOM_VFS::handle f, long offset, int origin);
  virtual long vftell (ATOM_VFS::handle f);
  virtual long vfsize (ATOM_VFS::handle f);
  virtual void vfclose (ATOM_VFS::handle f);
  virtual bool vfeof (ATOM_VFS::handle f);

protected:
  virtual ATOM_VFS::finddata_t *_newFindData (void);
  virtual void _deleteFindData (ATOM_VFS::finddata_t *fd);
  virtual ATOM_VFS::filedesc_t *_newFileDesc (void);
  virtual void _deleteFileDesc (ATOM_VFS::filedesc_t *fd);

private:
  bool _rmdir_r (const char *dir);
};

#endif // __ATOM_VFS_PHYSICVFS_H

