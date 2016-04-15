#ifndef __ATOM_VFS_MEMVFS_H
#define __ATOM_VFS_MEMVFS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"

#include "vfs.h"

#if defined(_MSC_VER)
# pragma warning(push)
# pragma warning(disable:4251)
#endif

class ATOM_VFS_API ATOM_MemVFS: public ATOM_VFS
{
protected:
  enum
  {
	t_dir = 0,
	t_file = 1
  };

  struct node_t;

  struct dir_t
  {
    ATOM_VFS *mount;
    char *sourcedir;
    node_t *child;
    bool mount_readonly;
  };

  struct file_t
  {
    char *contents;
    int size;
  };

  struct node_t
  {
    int type;
    char name[ATOM_VFS::max_filename_length];
    char fullname[ATOM_VFS::max_filename_length];
    unsigned hash;
    node_t *parent;
    node_t *prev;
    node_t *next;
    void *userdata;
    ATOM_VFS *owner;
    int lockcount;

    union
    {
      dir_t dir;
      file_t file;
    };
  };

  struct finddata_mem_t: public ATOM_VFS::finddata_t
  {
    void *reserved0;
    void *reserved1;
    void *reserved2;
    unsigned reserved3;
  };

  struct filedesc_mem_t: public ATOM_VFS::filedesc_t
  {
    void *reserved0;
    void *reserved1;
  };

public:
  ATOM_MemVFS (void);
  virtual ~ATOM_MemVFS (void);
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
  virtual bool mount (ATOM_VFS *vfs_from, const char *from, const char *to, bool readonly);
  virtual bool unmount (const char *dir);

protected:
  node_t *_createNode (const char *name, int type, node_t *parent, bool validate = true);
  void _deleteNode (node_t *node);
  void _deleteNode_r (node_t *node);
  node_t *_getNode (const char *dir, unsigned size) const;
  node_t *_getNearNode (const char *dir, const char **tail) const;
  node_t *_resolvePath (const char *dir, ATOM_VFS **owner, char *realpath) const;
  void _appendChild (node_t *node, node_t *child);
  void _removeChild (node_t *node, node_t *child);
  void _lockNode (node_t *node);
  void _unlockNode (node_t *node);
  void _initialize (void);
  void _finalize (void);
  ATOM_MemVFS::filedesc_mem_t *_openMountedFile (ATOM_MemVFS::node_t *node, ATOM_VFS *owner, const char *realpath, int mode);

protected:
  virtual ATOM_VFS::finddata_t *_newFindData (void);
  virtual void _deleteFindData (ATOM_VFS::finddata_t *fd);
  virtual ATOM_VFS::filedesc_t *_newFileDesc (void);
  virtual void _deleteFileDesc (ATOM_VFS::filedesc_t *fd);
  virtual unsigned _calcNameHash (const char *name, unsigned size) const;

protected:
  node_t *_M_root_node;
  char _M_cwd[ATOM_VFS::max_filename_length];

  typedef ATOM_MULTIMAP<unsigned, node_t*> nodedict_t;
  nodedict_t _M_node_dict;

  mutable ATOM_RWMutex _M_mutex;
};

#if defined(_MSC_VER)
# pragma warning(pop)
#endif

#endif // __ATOM_VFS_MEMVFS_H
