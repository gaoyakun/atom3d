#include <assert.h>
#include <direct.h>
#include <ATOM_dbghlp.h>
#include "zipfs.h"
#include "unzip.h"
#include "utils.h"

struct ZipNode
{
  unz_file_pos pos;
  long uncompressed_size;
};

struct ZipImpl
{
  char filename[ATOM_VFS::max_filename_length];
  char cwd[ATOM_VFS::max_filename_length];
  unzFile hzip;

  ATOM_VECTOR<ZipNode*> nodes;
};

ATOM_ZipVFS::ATOM_ZipVFS (void)
{
  _zip_impl = ATOM_NEW(ZipImpl);
  _zip_impl->hzip = 0;
  _case_sensitive = false;
  _password = 0;
}

ATOM_ZipVFS::ATOM_ZipVFS (bool bCaseSensitive)
{
  _zip_impl = ATOM_NEW(ZipImpl);
  _zip_impl->hzip = 0;
  _case_sensitive = bCaseSensitive;
  _password = 0;
}

bool ATOM_ZipVFS::load (const char *filename, const char *password)
{
  ATOM_RWMutex::ScopeMutexWrite lock(_M_mutex);

  unload ();

  if (!filename)
  {
    return false;
  }

  _zip_impl->hzip = unzOpen (filename);

  if (!_zip_impl->hzip)
  {
    return false;
  }

  _password = password ? _strdup (password) : 0;

  strcpy (_zip_impl->filename, filename);
  _getcwd (_zip_impl->cwd, sizeof(_zip_impl->cwd));

  const char sep = getSeperator ();
  char buffer[ATOM_VFS::max_filename_length];
  buffer[0] = sep;
  buffer[1] = '\0';

  for (int ret = unzGoToFirstFile(_zip_impl->hzip); ret == UNZ_OK; ret = unzGoToNextFile(_zip_impl->hzip))
  {
    unz_file_info info;

    if (UNZ_OK != unzGetCurrentFileInfo(_zip_impl->hzip, &info, buffer+1, sizeof(buffer)-1, 0, 0, 0, 0))
    {
      unload ();
      return false;
    }

    unsigned len = strlen(buffer);
    assert (len > 0);
	for (unsigned n = 0; n < len; ++n)
	{
		if (buffer[n] == '\\')
		{
			buffer[n] = sep;
		}
	}

    bool isdir = false;
    ZipNode *znode = 0;
    if (buffer[len-1] == sep)
    {
      // this is a directory
      assert (info.uncompressed_size == 0);
      isdir = true;
      buffer[len-1] = '\0';
    }

	if (!ATOM_MemVFS::isDir (buffer) && !ATOM_MemVFS::mkDir (buffer))
    {
      return false;
    }

    if (!isdir)
    {
      znode = ATOM_NEW(ZipNode);
      unzGetFilePos (_zip_impl->hzip, &znode->pos);
      znode->uncompressed_size = info.uncompressed_size;
      _zip_impl->nodes.push_back (znode);
    }

    if (!isCaseSensitive())
    {
      _strlwr (buffer);
    }

    ATOM_MemVFS::node_t *node = _getNode (buffer, 0);
    assert (node);
    node->userdata = isdir ? 0 : znode;
  }

  return true;
}

ATOM_VFS::finddata_t *ATOM_ZipVFS::findFirst (const char *dir)
{
  ATOM_MemVFS::finddata_mem_t *fd = (ATOM_MemVFS::finddata_mem_t*)ATOM_MemVFS::findFirst (dir);
  if (fd)
  {
    ATOM_MemVFS::node_t *node = (ATOM_MemVFS::node_t*)fd->reserved1;
    assert (node);
    fd->type = node->userdata ? ATOM_VFS::file : ATOM_VFS::directory;
    return fd;
  }
  return 0;
}

bool ATOM_ZipVFS::findNext (ATOM_VFS::finddata_t *handle)
{
  if (ATOM_MemVFS::findNext (handle))
  {
    ATOM_MemVFS::finddata_mem_t* fd = (ATOM_MemVFS::finddata_mem_t*)handle;
    ATOM_MemVFS::node_t *node = (ATOM_MemVFS::node_t*)fd->reserved1;
    assert (node);
    fd->type = node->userdata ? ATOM_VFS::file : ATOM_VFS::directory;
    return true;
  }
  return false;
}

void ATOM_ZipVFS::unload (void)
{
  ATOM_RWMutex::ScopeMutexWrite lock(_M_mutex);

  if (_zip_impl->hzip)
  {
    unzClose (_zip_impl->hzip);
    _zip_impl->hzip = 0;
  }

  for (unsigned i = 0; i < _zip_impl->nodes.size(); ++i)
  {
    ATOM_DELETE(_zip_impl->nodes[i]);
  }

  _zip_impl->nodes.clear ();

  if (_password)
  {
    free (_password);
    _password = 0;
  }

  _finalize ();
  _initialize ();
}

ATOM_ZipVFS::~ATOM_ZipVFS (void)
{
  unload ();
  ATOM_DELETE(_zip_impl);
}

bool ATOM_ZipVFS::isCaseSensitive (void) const
{
  return _case_sensitive;
}

bool ATOM_ZipVFS::mkDir (const char *dir)
{
  // not implement
  return false;
}

bool ATOM_ZipVFS::isDir (const char *dir)
{
  ATOM_RWMutex::ScopeMutexRead lock(_M_mutex);

  if (!dir)
  {
    return false;
  }

  ATOM_VFS *owner;
  char ident[ATOM_VFS::max_filename_length];
  char realpath[ATOM_VFS::max_filename_length];
  if (!identifyPath (dir, ident))
  {
    return ATOM_VFS::invalid_handle;
  }

  ATOM_MemVFS::node_t *node = _resolvePath (ident, &owner, realpath);
  if (!node)
  {
    return false;
  }

  if (node->userdata)
  {
    // this is a ZIP directory
    return false;
  }

  return true;
}

bool ATOM_ZipVFS::removeFile (const char *file, bool force)
{
  // not implementation
  return false;
}

ATOM_VFS::handle ATOM_ZipVFS::vfopen (const char *filename, int mode)
{
  ATOM_RWMutex::ScopeMutexRead lock(_M_mutex);

  if (!filename || !_zip_impl->hzip)
  {
    return ATOM_VFS::invalid_handle;
  }

  ATOM_VFS *owner;
  char ident[ATOM_VFS::max_filename_length];
  char realpath[ATOM_VFS::max_filename_length];
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
      fd->vfs = this;
      strcpy (fd->filename, ident);
    }
    return (ATOM_VFS::handle)fd;
  }

  if (!node->userdata)
  {
    // this is a ZIP directory
    return ATOM_VFS::invalid_handle;
  }

  unzFile zip = _zip_impl->hzip;

  if (!zip)
  {
    return ATOM_VFS::invalid_handle;
  }

  ZipNode *znode = (ZipNode*)node->userdata;
  if (UNZ_OK != unzGoToFilePos (zip, &znode->pos))
  {
    return ATOM_VFS::invalid_handle;
  }

  if (UNZ_OK != _password ? unzOpenCurrentFilePassword (zip, _password) : unzOpenCurrentFile (zip))
  {
    return ATOM_VFS::invalid_handle;
  }

  unsigned size = znode->uncompressed_size;
  void *data = ATOM_MALLOC(size);
  int n = unzReadCurrentFile (zip, data, size);
  if (n != size)
  {
    unzCloseCurrentFile (zip);
    ATOM_FREE(data);
    return ATOM_VFS::invalid_handle;
  }

  if ((mode & ATOM_VFS::text) != 0)
  {
    n = bin2txt ((char*)data, n);
  }

  ATOM_ZipVFS::filedesc_zip_t *fd = (ATOM_ZipVFS::filedesc_zip_t*)_allocFileDesc ();
  fd->vfs = this;
  fd->owner = this;
  fd->mode = mode;
  fd->data = data;
  fd->position = 0;
  fd->size = n;
  strcpy (fd->filename, ident);

  unzCloseCurrentFile (zip);

  return (ATOM_VFS::handle)fd;
}

unsigned ATOM_ZipVFS::vfread (ATOM_VFS::handle f, void *buffer, unsigned size)
{
  if (f == ATOM_VFS::invalid_handle)
  {
    return 0;
  }

  ATOM_MemVFS::filedesc_mem_t *fd = (ATOM_MemVFS::filedesc_mem_t*)f;
  ATOM_VFS *owner = (ATOM_VFS*)fd->owner;
  if (owner && owner != this)
  {
    return owner->vfread (f, buffer, size);
  }

  ATOM_ZipVFS::filedesc_zip_t *zfd = (ATOM_ZipVFS::filedesc_zip_t*)f;
  unsigned remained = zfd->size - zfd->position;
  unsigned numToRead = (remained < size) ? remained : size;
  memcpy (buffer, ((char*)zfd->data) + zfd->position, numToRead);
  zfd->position += numToRead;

  return numToRead;
}

unsigned ATOM_ZipVFS::vfwrite (ATOM_VFS::handle f, const void *buffer, unsigned size)
{
  // read only, not support write
  return 0;
}

long ATOM_ZipVFS::vfseek (ATOM_VFS::handle f, long offset, int origin)
{
  if (f == ATOM_VFS::invalid_handle)
  {
    return -1L;
  }

  ATOM_MemVFS::filedesc_mem_t *fd = (ATOM_MemVFS::filedesc_mem_t*)f;
  ATOM_VFS *owner = (ATOM_VFS*)fd->owner;
  if (owner && owner != this)
  {
    return owner->vfseek (f, offset, origin);
  }

  ATOM_ZipVFS::filedesc_zip_t *zfd = (ATOM_ZipVFS::filedesc_zip_t*)f;
  long pos;

  switch (origin)
  {
  case ATOM_VFS::begin:
    pos = offset;
    break;
  case ATOM_VFS::current:
    pos = zfd->position + offset;
    break;
  case ATOM_VFS::end:
    pos = zfd->size + offset;
    break;
  default:
    return -1L;
  }

  if (pos < 0 || pos > zfd->size)
  {
    return -1L;
  }

  zfd->position = pos;

  return pos;
}

long ATOM_ZipVFS::vftell (ATOM_VFS::handle f)
{
  if (f == ATOM_VFS::invalid_handle)
  {
    return -1L;
  }

  ATOM_MemVFS::filedesc_mem_t *fd = (ATOM_MemVFS::filedesc_mem_t*)f;
  ATOM_VFS *owner = (ATOM_VFS*)fd->owner;
  if (owner && owner != this)
  {
    return owner->vftell (f);
  }

  ATOM_ZipVFS::filedesc_zip_t *zfd = (ATOM_ZipVFS::filedesc_zip_t*)f;

  return zfd->position;
}

long ATOM_ZipVFS::vfsize (ATOM_VFS::handle f)
{
  if (f == ATOM_VFS::invalid_handle)
  {
    return 0;
  }

  ATOM_MemVFS::filedesc_mem_t *fd = (ATOM_MemVFS::filedesc_mem_t*)f;
  ATOM_VFS *owner = (ATOM_VFS*)fd->owner;
  if (owner && owner != this)
  {
    return owner->vfsize (f);
  }

  ATOM_ZipVFS::filedesc_zip_t *zfd = (ATOM_ZipVFS::filedesc_zip_t*)f;
  return zfd->size;
}

void ATOM_ZipVFS::vfclose (ATOM_VFS::handle f)
{
  if (f != ATOM_VFS::invalid_handle)
  {
    ATOM_MemVFS::filedesc_mem_t *fd = (ATOM_MemVFS::filedesc_mem_t*)f;
    ATOM_VFS *owner = (ATOM_VFS*)fd->owner;
    if (owner && owner != this)
    {
      owner->vfclose (f);
    }
    else
    {
      ATOM_ZipVFS::filedesc_zip_t *zfd = (ATOM_ZipVFS::filedesc_zip_t*)f;
      ATOM_FREE (zfd->data);
    }
    _freeFileDesc (fd);
  }
}

bool ATOM_ZipVFS::vfeof (ATOM_VFS::handle f)
{
  if (f != ATOM_VFS::invalid_handle)
  {
    ATOM_MemVFS::filedesc_mem_t *fd = (ATOM_MemVFS::filedesc_mem_t*)f;
    ATOM_VFS *owner = (ATOM_VFS*)fd->owner;
    if (owner && owner != this)
    {
      return owner->vfeof (f);
    }
    else
    {
      ATOM_ZipVFS::filedesc_zip_t *zfd = (ATOM_ZipVFS::filedesc_zip_t*)f;
      return zfd->position == zfd->size;
    }
  }
  return false;
}

ATOM_VFS::filedesc_t *ATOM_ZipVFS::_newFileDesc (void)
{
  return ATOM_NEW(ATOM_ZipVFS::filedesc_zip_t);
}

void ATOM_ZipVFS::_deleteFileDesc (ATOM_VFS::filedesc_t *fd)
{
  ATOM_DELETE((ATOM_ZipVFS::filedesc_zip_t*)fd);
}

