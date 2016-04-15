#include <stdlib.h>
#include <assert.h>
#include "memvfs.h"

unsigned ATOM_MemVFS::_calcNameHash (const char *name, unsigned size) const
{
  unsigned hash = 0; 
  for (unsigned i = 0 ; i < size; ++i)
    hash = 5 * hash + name[i];
  return hash;
}

static inline bool _isValidName (const char *name, unsigned size)
{
  static const char forbit_chars[] = "\\/:|<>*?&";

  assert (name);

  if (!strcmp (name, ".") || !strcmp (name, ".."))
  {
    return false;
  }

  for (unsigned i = 0; i < size; ++i)
  {
    if (strchr (forbit_chars, name[i]))
    {
      return false;
    }
  }

  return true;
}

static inline void _appendPath (char *dest, const char *source, char sep)
{
  assert (dest);
  assert (source);
  unsigned size = strlen(dest);
  if (size == 0 || dest[size-1] != sep)
  {
    dest[size] = sep;
    dest[size+1] = '\0';
  }
  strcat (dest, source);
}

void ATOM_MemVFS::_appendChild (ATOM_MemVFS::node_t *node, ATOM_MemVFS::node_t *child)
{
  assert (node && node->type == ATOM_MemVFS::t_dir);
  assert (child);

  child->parent = node;
  ATOM_MemVFS::node_t *firstchild = node->dir.child;

  if (firstchild)
  {
    firstchild->prev->next = child;
    child->prev = firstchild->prev;
    firstchild->prev = child;
    child->next = firstchild;
  }
  else
  {
    node->dir.child = child;
    child->prev = child;
    child->next = child;
  }
}

void ATOM_MemVFS::_removeChild (ATOM_MemVFS::node_t *node, ATOM_MemVFS::node_t *child)
{
  assert (node);
  assert (node->type == ATOM_MemVFS::t_dir);
  assert (child);
  assert (child->parent == node);

  if (child->prev == child)
  {
    assert (node->dir.child == child);
    node->dir.child = 0;
  }
  else
  {
    if (node->dir.child == child)
    {
      node->dir.child = child->next;
    }

    child->prev->next = child->next;
    child->next->prev = child->prev;
  }
}

ATOM_MemVFS::node_t *ATOM_MemVFS::_createNode (const char *name, int type, ATOM_MemVFS::node_t *parent, bool validate)
{
  assert (name);

  unsigned len = strlen(name);

  if (validate && !_isValidName (name, len))
  {
    return false;
  }

  node_t *node = ATOM_NEW(ATOM_MemVFS::node_t);
  memset (node, 0, sizeof(*node));

  strcpy (node->name, name);
  if (!isCaseSensitive ())
  {
    // not case sensitive, so use lower case internally
    _strlwr (node->name);
  }

  node->parent = parent;

  const char sep[2] = { getSeperator (), '\0' };

  if (parent)
  {
    unsigned parent_len = strlen(parent->fullname);
    assert (parent_len > 0);
    if (parent_len + len + 1 >= ATOM_VFS::max_filename_length)
    {
      return false;
    }

    strcpy (node->fullname, parent->fullname);
    if (parent->fullname[parent_len-1] != sep[0])
    {
      strcat (node->fullname, sep);
    }
    strcat (node->fullname, node->name);
  }
  else
  {
    strcpy (node->fullname, sep);
  }

  node->hash = _calcNameHash (node->fullname, strlen(node->fullname));
  _M_node_dict.insert(std::pair<unsigned, ATOM_MemVFS::node_t*>(node->hash, node));

  return node;
}

void ATOM_MemVFS::_deleteNode (ATOM_MemVFS::node_t *node)
{
  assert (node);

  if (node->type == ATOM_MemVFS::t_file && node->file.contents)
  {
    ATOM_DELETE_ARRAY(node->file.contents);
  }

  std::pair<nodedict_t::iterator, nodedict_t::iterator> range = _M_node_dict.equal_range (node->hash);
  for (nodedict_t::iterator it = range.first; it != range.second; ++it)
  {
    if (it->second == node)
    {
      _M_node_dict.erase (it);
      ATOM_DELETE(node);
      return;
    }
  }

  assert (0);
}

void ATOM_MemVFS::_deleteNode_r (ATOM_MemVFS::node_t *node)
{
  if (node->type == ATOM_MemVFS::t_dir)
  {
    ATOM_MemVFS::node_t *child = node->dir.child;

    if (child)
    {
      ATOM_MemVFS::node_t *sibling = child->next;
      while (sibling != child)
      {
        ATOM_MemVFS::node_t *p = sibling->next;
        _deleteNode_r (sibling);
        sibling = p;
      }
      _deleteNode_r (child);
    }
  }

  _deleteNode (node);
}

void ATOM_MemVFS::_lockNode (ATOM_MemVFS::node_t *node)
{
  assert (node);
  ++node->lockcount;

  ATOM_MemVFS::node_t *parent = node->parent;
  for (; parent; parent = parent->parent)
  {
    _lockNode (parent);
  }
}

void ATOM_MemVFS::_unlockNode (ATOM_MemVFS::node_t *node)
{
  assert (node);
  --node->lockcount;

  ATOM_MemVFS::node_t *parent = node->parent;
  for (; parent; parent = parent->parent)
  {
    _unlockNode (parent);
  }
}

ATOM_MemVFS::node_t *ATOM_MemVFS::_getNode (const char *dir, unsigned size) const
{
  assert (dir);
  size = size ? size : strlen(dir);
  assert (size < ATOM_VFS::max_filename_length);

  unsigned hash = _calcNameHash (dir, size);
  std::pair<nodedict_t::const_iterator, nodedict_t::const_iterator> range = _M_node_dict.equal_range (hash);
  for (nodedict_t::const_iterator it = range.first; it != range.second; ++it)
  {
    if (!strncmp(it->second->fullname, dir, size))
    {
      return it->second;
    }
  }
  return 0;
}

ATOM_MemVFS::node_t *ATOM_MemVFS::_getNearNode (const char *dir, const char **tail) const
{
  assert (dir);
  unsigned size = strlen(dir);
  assert (size < ATOM_VFS::max_filename_length);
  const char *dir_end = dir + size;
  const char sep = getSeperator ();

  for (;;)
  {
    ATOM_MemVFS::node_t *node = _getNode (dir, dir_end - dir);

    if (node)
    {
      if (tail)
      {
        *tail = dir + strlen(node->fullname);
        if (*tail)
        {
          if (**tail == sep)
          {
            ++*tail;
          }
          else
          {
            assert (**tail == '\0');
          }
        }
      }

      return node;
    }

    --dir_end;

    while (dir_end > dir && *dir_end != sep)
    {
      --dir_end;
    }

    if (dir == dir_end)
    {
      if (tail)
      {
        *tail = dir+1;
      }

      return _M_root_node;
    }
  }
}

ATOM_MemVFS::node_t *ATOM_MemVFS::_resolvePath (const char *dir, ATOM_VFS **owner, char *realpath) const
{
  assert (dir);
  unsigned size = strlen(dir);
  assert (size < ATOM_VFS::max_filename_length);
  const char *tail;

  ATOM_MemVFS::node_t *nearNode = _getNearNode (dir, &tail);
  ATOM_ASSERT(nearNode);

  if (*tail)
  {
    if (nearNode->type != ATOM_MemVFS::t_dir)
    {
      return 0;
    }

    if (owner)
    {
      *owner = nearNode->dir.mount;
    }

    if (realpath)
    {
      if (*owner)
      {
        char sep[2] = { (*owner)->getSeperator(), '\0' };
        const char sep_ = getSeperator();

        strcpy (realpath, nearNode->dir.sourcedir);
        unsigned len = strlen(realpath);
        assert (len);
        if (realpath[len-1] != sep[0])
        {
          strcat (realpath, sep);
        }

        char *p = realpath + strlen(realpath);
        strcat (realpath, tail);
        if (sep[0] != sep_)
        {
          for (; *p; ++p)
          {
            if (*p == sep_)
            {
              *p = sep[0];
            }
          }
        }
      }
      else
      {
        strcpy(realpath, tail);
      }
    }
  }
  else
  {
    if (nearNode->type == t_dir)
    {
      if (owner)
      {
        *owner = nearNode->dir.mount;
      }

      if (realpath)
      {
        strcpy (realpath, *owner ? nearNode->dir.sourcedir : "");
      }
    }
    else
    {
      if (owner)
      {
        *owner = 0;
      }

      if (realpath)
      {
        *realpath = '\0';
      }
    }
  }

  return nearNode;
}

ATOM_MemVFS::ATOM_MemVFS (void)
{
  _initialize ();
};

ATOM_MemVFS::~ATOM_MemVFS (void)
{
  _finalize ();
}

void ATOM_MemVFS::_initialize (void)
{
  _M_root_node = _createNode ("/", t_dir, 0, false);
  _M_root_node->prev = _M_root_node;
  _M_root_node->next = _M_root_node;
  _lockNode (_M_root_node);
  strcpy (_M_cwd, "/");
}

void ATOM_MemVFS::_finalize (void)
{
  _deleteNode_r (_M_root_node);
  _purgeFindDatas ();
}

bool ATOM_MemVFS::isCaseSensitive (void) const
{
  return false;
}

char ATOM_MemVFS::getSeperator (void) const
{
  return '/';
}

ATOM_VFS::finddata_t *ATOM_MemVFS::_newFindData (void)
{
  return ATOM_NEW(ATOM_MemVFS::finddata_mem_t);
}

void ATOM_MemVFS::_deleteFindData (ATOM_VFS::finddata_t *fd)
{
  ATOM_DELETE((ATOM_MemVFS::finddata_mem_t*)fd);
}

ATOM_VFS::filedesc_t *ATOM_MemVFS::_newFileDesc (void)
{
  return ATOM_NEW(ATOM_MemVFS::filedesc_mem_t);
}

void ATOM_MemVFS::_deleteFileDesc (ATOM_VFS::filedesc_t *fd)
{
  ATOM_DELETE((ATOM_MemVFS::filedesc_mem_t*)fd);
}

ATOM_VFS::finddata_t *ATOM_MemVFS::findFirst (const char *dir) 
{
  ATOM_RWMutex::ScopeMutexRead lock(_M_mutex);

  ATOM_VFS *owner;
  char ident[ATOM_VFS::max_filename_length];
  char realpath[ATOM_VFS::max_filename_length];

  if (!identifyPath (dir, ident))
  {
    return 0;
  }

  ATOM_MemVFS::node_t *node = _resolvePath (ident, &owner, realpath);
  if (!node)
  {
    return 0;
  }

  if (owner)
  {
    ATOM_VFS::finddata_t *finddata = owner->findFirst (realpath);

    if (finddata)
    {
      ATOM_MemVFS::finddata_mem_t *fd = (ATOM_MemVFS::finddata_mem_t*)_allocFindData();
      const char *p = strrchr (finddata->filename, owner->getSeperator());
      assert (p);

      strcpy (fd->filename, ident);
      _appendPath (fd->filename, p+1, getSeperator());
      fd->filenameNoPath = finddata->filenameNoPath;
      fd->type = finddata->type;
      fd->reserved1 = finddata;
      fd->reserved2 = owner;
      fd->reserved3 = strlen(fd->filename) - strlen(p+1);

      return fd;
    }

    return 0;
  }

  if (node->type != ATOM_MemVFS::t_dir || *realpath)
  {
    return 0;
  }

  if (!node->dir.child)
  {
    return 0;
  }

  ATOM_MemVFS::finddata_mem_t *fd = (ATOM_MemVFS::finddata_mem_t*)_allocFindData ();
  fd->reserved0 = node->dir.child;
  fd->reserved1 = node->dir.child;
  fd->reserved2 = this;
  strcpy (fd->filename, node->dir.child->fullname);
  fd->filenameNoPath = strrchr(fd->filename, getSeperator()) + 1;
  fd->type = (node->dir.child->type == ATOM_MemVFS::t_dir) ? ATOM_VFS::directory : ATOM_VFS::file;

  return fd;
}

bool ATOM_MemVFS::findNext (ATOM_VFS::finddata_t *handle)
{
  if (handle)
  {
    ATOM_RWMutex::ScopeMutexRead lock(_M_mutex);

    ATOM_MemVFS::finddata_mem_t *fd = (ATOM_MemVFS::finddata_mem_t*)handle;

    ATOM_VFS *vfs = (ATOM_VFS*)fd->reserved2;

    if (vfs != this)
    {
      ATOM_VFS::finddata_t *finddata = (ATOM_VFS::finddata_t*)fd->reserved1;
      assert (finddata);
      bool b = vfs->findNext (finddata);
      if (b)
      {
        const char *p = strrchr (finddata->filename, vfs->getSeperator());
        assert (p);
        strcpy (fd->filename + fd->reserved3, p+1);
        fd->filenameNoPath = finddata->filenameNoPath;
        fd->type = finddata->type;
        return true;
      }
      return false;
    }
    else
    {
      assert (fd->reserved0);
      assert (fd->reserved1);
      ATOM_MemVFS::node_t *firstchild = (ATOM_MemVFS::node_t*)fd->reserved0;
      ATOM_MemVFS::node_t *current = (ATOM_MemVFS::node_t*)fd->reserved1;
      current = current->next;
      if (current == firstchild)
      {
        return false;
      }
      fd->reserved1 = current;
      strcpy (fd->filename, current->fullname);
      fd->filenameNoPath = strrchr (fd->filename, getSeperator()) + 1;
      return true;
    }
  }

  return false;
}

void ATOM_MemVFS::findClose (finddata_t *handle)
{
  if (handle)
  {
    ATOM_RWMutex::ScopeMutexRead lock(_M_mutex);

    ATOM_MemVFS::finddata_mem_t *fd = (ATOM_MemVFS::finddata_mem_t*)handle;

    ATOM_VFS *vfs = (ATOM_VFS*)fd->reserved2;

    if (vfs != this)
    {
      vfs->findClose ((ATOM_VFS::finddata_t*)fd->reserved1);
    }

    _freeFindData (fd);
  }
}

bool ATOM_MemVFS::doesFileExists (const char *file)
{
  ATOM_RWMutex::ScopeMutexRead lock(_M_mutex);

  ATOM_VFS *owner;
  char ident[ATOM_VFS::max_filename_length];
  char realpath[ATOM_VFS::max_filename_length];

  if (!identifyPath (file, ident))
  {
    return false;
  }

  ATOM_MemVFS::node_t *node = _resolvePath (ident, &owner, realpath);
  if (node)
  {
    if (owner)
    {
      return owner->doesFileExists (realpath);
    }
    else if (!*realpath)
    {
      return true;
    }
  }

  return false;
}

bool ATOM_MemVFS::isDir (const char *file)
{
  ATOM_RWMutex::ScopeMutexRead lock(_M_mutex);

  ATOM_VFS *owner;
  char ident[ATOM_VFS::max_filename_length];
  char realpath[ATOM_VFS::max_filename_length];

  if (!identifyPath (file, ident))
  {
    return false;
  }

  ATOM_MemVFS::node_t *node = _resolvePath (ident, &owner, realpath);
  if (node)
  {
    if (owner)
    {
      return owner->isDir (realpath);
    }
    else if (!*realpath)
    {
      return node->type == ATOM_MemVFS::t_dir;
    }
  }

  return false;
}

bool ATOM_MemVFS::mkDir (const char *dir)
{
  ATOM_RWMutex::ScopeMutexWrite lock(_M_mutex);

  ATOM_VFS *owner;
  char ident[ATOM_VFS::max_filename_length];
  char realpath[ATOM_VFS::max_filename_length];

  if (!identifyPath (dir, ident))
  {
    return false;
  }

  ATOM_MemVFS::node_t *node = _resolvePath (ident, &owner, realpath);
  char *dir_to_make;

  if (node)
  {
    if (owner)
    {
      return owner->mkDir (realpath);
    }
    else if (!*realpath)
    {
      return false;
    }
    else
    {
      dir_to_make = realpath;    
    }
  }
  else
  {
    dir_to_make = ident + 1;
    node = _M_root_node;
  }

  const char sep = getSeperator ();

  for (;;)
  {
    char *p = strchr (dir_to_make, sep);
    if (p)
    {
      *p = '\0';
    }

    ATOM_MemVFS::node_t *newnode = _createNode (dir_to_make, ATOM_MemVFS::t_dir, node);
    _appendChild (node, newnode);

    if (!p)
    {
      break;
    }
    else
    {
      dir_to_make = p + 1;
      node = newnode;
    }
  }

  return true;
}

bool ATOM_MemVFS::removeFile (const char *file, bool force)
{
  ATOM_RWMutex::ScopeMutexWrite lock(_M_mutex);

  ATOM_VFS *owner;
  char ident[ATOM_VFS::max_filename_length];
  char realpath[ATOM_VFS::max_filename_length];

  if (!identifyPath (file, ident))
  {
    return false;
  }

  ATOM_MemVFS::node_t *node = _resolvePath (ident, &owner, realpath);
  if (!node)
  {
    return false;
  }

  if (owner)
  {
    if (!owner->_strcmp (realpath, node->dir.sourcedir))
    {
      // attempt to remove mounted path
      return false;
    }

    return owner->removeFile (realpath, force);
  }

  if (*realpath)
  {
    return false;
  }

  if (node->type != ATOM_MemVFS::t_dir)
  {
    return false;
  }

  if (node->lockcount)
  {
    return false;
  }

  ATOM_MemVFS::node_t *child = node->dir.child;

  if (child)
  {
    if (force)
    {
      _removeChild (node, child);
      _deleteNode_r (child);
      return true;
    }
    else
    {
      return false;
    }
  }

  ATOM_MemVFS::node_t *parent = node->parent;

  _removeChild (parent, node);
  _deleteNode (node);

  return true;
}

void ATOM_MemVFS::getCWD (char *buffer) const
{
  ATOM_RWMutex::ScopeMutexRead lock(_M_mutex);

  if (buffer)
  {
    strcpy (buffer, _M_cwd);
  }
}

bool ATOM_MemVFS::chDir (const char *dir)
{
  ATOM_RWMutex::ScopeMutexWrite lock(_M_mutex);

  ATOM_VFS *owner;
  char ident[ATOM_VFS::max_filename_length];
  char realpath[ATOM_VFS::max_filename_length];

  if (!identifyPath (dir, ident))
  {
    return false;
  }

  ATOM_MemVFS::node_t *node = _resolvePath (ident, &owner, realpath);
  if (!node)
  {
    return false;
  }

  if (owner && !owner->isDir (realpath))
  {
    return false;
  }
  else if (!owner)
  {
    if (*realpath)
    {
      return false;
    }

    if (node->type != ATOM_MemVFS::t_dir)
    {
      return false;
    }
  }

  strcpy (_M_cwd, ident);

  return true;
}

char *ATOM_MemVFS::identifyPath (const char *dir, char *buffer) const
{
  if (!dir || !buffer)
  {
    return 0;
  }

  *buffer = '\0';

  // skip leading spaces
  while (*dir == ' ' || *dir == '\t')
  {
    ++dir;
  }

  // empty string
  if (!*dir)
  {
    return 0;
  }

  struct tok
  {
    const char *s;
    unsigned size;
  };

  const char sep [2] = { getSeperator(), '\0' };
  tok tokens[1024];
  tok *token_top = tokens;

  assert (_M_cwd[0] == sep[0]);
  bool do_cwd = *dir != sep[0];
  const char *fullpath = do_cwd ? _M_cwd+1 : dir+1;

  for (;;)
  {
    token_top->s = fullpath;
    const char *next = strchr(fullpath, sep[0]);
    if (next == fullpath)
    {
      return 0;
    }

    unsigned size = next ? next-fullpath : strlen(fullpath);

    if (!do_cwd)
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
          return 0;
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
        assert (!do_cwd);
        break;
      }
    }
    else
    {
      if (do_cwd)
      {
        do_cwd = false;
        fullpath = dir;
      }
      else
      {
        break;
      }
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

  if (!isCaseSensitive ())
  {
    _strlwr (buffer);
  }

  return buffer;
}

ATOM_VFS::handle ATOM_MemVFS::vfopen (const char *filename, int mode)
{
  ATOM_VFS *owner;
  char ident[ATOM_VFS::max_filename_length];
  char realpath[ATOM_VFS::max_filename_length];

  ATOM_RWMutex::ScopeMutexRead lock(_M_mutex);

  if (!identifyPath (filename, ident))
  {
    return ATOM_VFS::invalid_handle;
  }

  ATOM_MemVFS::node_t *node = _resolvePath (ident, &owner, realpath);
  if (!node)
  {
    return ATOM_VFS::invalid_handle;
  }

  if (owner)
  {
    ATOM_MemVFS::filedesc_mem_t *fd = _openMountedFile (node, owner, realpath, mode);

    if (fd)
    {
      strcpy (fd->filename, filename);
    }

    return (ATOM_VFS::handle)fd;
  }

  return ATOM_VFS::invalid_handle;
}

ATOM_MemVFS::filedesc_mem_t *ATOM_MemVFS::_openMountedFile (ATOM_MemVFS::node_t *node, ATOM_VFS *owner, const char *realpath, int mode)
{
  assert (node);

  if (node->dir.mount_readonly && 
      ((mode & ATOM_VFS::write) != 0 ||
      (mode & ATOM_VFS::trunc) != 0 ||
      (mode & ATOM_VFS::append) != 0))
  {
    // could not write file if mounted read only
    return 0;
  }

  ATOM_VFS::handle f = owner->vfopen (realpath, mode);
  if (f == ATOM_VFS::invalid_handle)
  {
    return 0;
  }

  ATOM_MemVFS::filedesc_mem_t *fd = (ATOM_MemVFS::filedesc_mem_t*)_allocFileDesc ();
  fd->reserved0 = owner;
  fd->reserved1 = (void*)f;
  fd->owner = this;
  fd->vfs = this;

  return fd;
}

unsigned ATOM_MemVFS::vfread (ATOM_VFS::handle f, void *buffer, unsigned size)
{
  if (f != ATOM_VFS::invalid_handle)
  {
    ATOM_MemVFS::filedesc_mem_t *fd = (ATOM_MemVFS::filedesc_mem_t*)f;
    ATOM_VFS *vfs = (ATOM_VFS*)fd->reserved0;
    if (vfs)
    {
      return vfs->vfread ((ATOM_VFS::handle)fd->reserved1, buffer, size);
    }
  }
  return 0;
}

unsigned ATOM_MemVFS::vfwrite (ATOM_VFS::handle f, const void *buffer, unsigned size)
{
  if (f != ATOM_VFS::invalid_handle)
  {
    ATOM_MemVFS::filedesc_mem_t *fd = (ATOM_MemVFS::filedesc_mem_t*)f;
    ATOM_VFS *vfs = (ATOM_VFS*)fd->reserved0;
    if (vfs)
    {
      return vfs->vfwrite ((ATOM_VFS::handle)fd->reserved1, buffer, size);
    }
  }
  return 0;
}

long ATOM_MemVFS::vfseek (ATOM_VFS::handle f, long offset, int origin)
{
  if (f != ATOM_VFS::invalid_handle)
  {
    ATOM_MemVFS::filedesc_mem_t *fd = (ATOM_MemVFS::filedesc_mem_t*)f;
    ATOM_VFS *vfs = (ATOM_VFS*)fd->reserved0;
    if (vfs)
    {
      return vfs->vfseek ((ATOM_VFS::handle)fd->reserved1, offset, origin);
    }
  }
  return -1L;
}

long ATOM_MemVFS::vftell (ATOM_VFS::handle f)
{
  if (f != ATOM_VFS::invalid_handle)
  {
    ATOM_MemVFS::filedesc_mem_t *fd = (ATOM_MemVFS::filedesc_mem_t*)f;
    ATOM_VFS *vfs = (ATOM_VFS*)fd->reserved0;
    if (vfs)
    {
      return vfs->vftell ((ATOM_VFS::handle)fd->reserved1);
    }
  }
  return -1L;
}

long ATOM_MemVFS::vfsize (ATOM_VFS::handle f)
{
  if (f != ATOM_VFS::invalid_handle)
  {
    ATOM_MemVFS::filedesc_mem_t *fd = (ATOM_MemVFS::filedesc_mem_t*)f;
    ATOM_VFS *vfs = (ATOM_VFS*)fd->reserved0;
    if (vfs)
    {
      return vfs->vfsize ((ATOM_VFS::handle)fd->reserved1);
    }
  }
  return -1L;
}

void ATOM_MemVFS::vfclose (ATOM_VFS::handle fd)
{
  if (fd != ATOM_VFS::invalid_handle)
  {
    ATOM_MemVFS::filedesc_mem_t *f = (ATOM_MemVFS::filedesc_mem_t*)fd;
    ATOM_VFS *owner = (ATOM_VFS*)f->reserved0;
    if (owner)
    {
      owner->vfclose ((ATOM_VFS::handle)f->reserved1);
    }
    _freeFileDesc (f);
  }
}

bool ATOM_MemVFS::vfeof (ATOM_VFS::handle f)
{
  if (f != ATOM_VFS::invalid_handle)
  {
    ATOM_MemVFS::filedesc_mem_t *fd = (ATOM_MemVFS::filedesc_mem_t*)f;
    ATOM_VFS *owner = (ATOM_VFS*)fd->reserved0;
    bool ret = false;
    if (owner)
    {
      ret = owner->vfeof ((ATOM_VFS::handle)fd->reserved1);
    }
    _freeFileDesc (fd);
    return ret;
  }
  return false;
}

bool ATOM_MemVFS::mount (ATOM_VFS *vfs_from, const char *from, const char *to, bool readonly)
{
  if (!vfs_from || !from || !to)
  {
    return false;
  }

  ATOM_RWMutex::ScopeMutexWrite lock(_M_mutex);

  ATOM_VFS *owner;
  char ident_to[ATOM_VFS::max_filename_length];
  char ident_from[ATOM_VFS::max_filename_length];
  char realpath[ATOM_VFS::max_filename_length];

  if (!identifyPath (to, ident_to))
  {
    return false;
  }

  ATOM_MemVFS::node_t *node = _resolvePath (ident_to, &owner, realpath);
  if (!node || owner || *realpath || node->type != ATOM_MemVFS::t_dir)
  {
    return false;
  }

  //if (node == _M_root_node)
  //{
  //  // can not mount root node
  //  return false;
  //}

  if (node->dir.child)
  {
    return false;
  }

  if (!vfs_from->identifyPath (from, ident_from))
  {
    return false;
  }

  node->dir.mount = vfs_from;
  node->dir.mount_readonly = readonly;
  node->dir.sourcedir = _strdup (ident_from);
  _lockNode (node);

  return true;
}

bool ATOM_MemVFS::unmount (const char *dir)
{
  if (!dir)
  {
    return false;
  }

  ATOM_RWMutex::ScopeMutexWrite lock(_M_mutex);

  ATOM_VFS *owner;
  char ident[ATOM_VFS::max_filename_length];
  char realpath[ATOM_VFS::max_filename_length];

  if (!identifyPath (dir, ident))
  {
    return false;
  }

  ATOM_MemVFS::node_t *node = _resolvePath (ident, &owner, realpath);
  if (node && owner && !owner->_strcmp (realpath, node->dir.sourcedir))
  {
    node->dir.mount = 0;
    free (node->dir.sourcedir);
    node->dir.sourcedir = 0;
    _unlockNode (node);
    return true;
  }

  return false;
}

ATOM_VFS *ATOM_MemVFS::getNativePath (const char *mount_path, char *buffer) const
{
  if (mount_path && buffer)
  {
    ATOM_RWMutex::ScopeMutexRead lock(_M_mutex);

    ATOM_VFS *owner;
    char ident[ATOM_VFS::max_filename_length];
    if (!identifyPath (mount_path, ident))
    {
      return 0;
    }

    ATOM_MemVFS::node_t *node = _resolvePath (ident, &owner, buffer);
    if (node && owner)
    {
      return owner;
    }
  }

  return 0;
}

