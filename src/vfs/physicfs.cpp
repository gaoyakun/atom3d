#if defined (WIN32)

#include <stdlib.h>
#include <assert.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <windows.h>
#include <ATOM_dbghlp.h>

#include "physicfs.h"

ATOM_PhysicVFS::ATOM_PhysicVFS (void)
{
}

ATOM_PhysicVFS::~ATOM_PhysicVFS (void)
{
  _purgeFindDatas ();
}

bool ATOM_PhysicVFS::isCaseSensitive (void) const
{
  return false;
}

char ATOM_PhysicVFS::getSeperator (void) const
{
  return '\\';
}

ATOM_VFS::finddata_t *ATOM_PhysicVFS::_newFindData (void)
{
  return ATOM_NEW(ATOM_PhysicVFS::finddata_physic_t);
}

void ATOM_PhysicVFS::_deleteFindData (ATOM_VFS::finddata_t *fd)
{
  ATOM_DELETE((ATOM_PhysicVFS::finddata_physic_t*)fd);
}

ATOM_VFS::filedesc_t *ATOM_PhysicVFS::_newFileDesc (void)
{
  return ATOM_NEW(ATOM_PhysicVFS::filedesc_physic_t);
}

void ATOM_PhysicVFS::_deleteFileDesc (ATOM_VFS::filedesc_t *fd)
{
  ATOM_DELETE((ATOM_PhysicVFS::filedesc_physic_t*)fd);
}

ATOM_VFS::finddata_t *ATOM_PhysicVFS::findFirst (const char *dir)
{
  if (dir)
  {
    char ident[ATOM_VFS::max_filename_length];
    if (!identifyPath (dir, ident))
    {
      return 0;
    }
    unsigned len = strlen(ident);
    assert (len > 0);
    if (ident[len-1] != '\\')
    {
      ident[len++] = '\\';
    }
    ident[len] = '*';
    ident[len+1] = '\0';

    WIN32_FIND_DATA *pFindData = ATOM_NEW(WIN32_FIND_DATA);
    HANDLE hFind = ::FindFirstFileA (ident, pFindData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
      ATOM_DELETE(pFindData);
      return 0;
    }

    // skip '.'
    if (!::FindNextFile (hFind, pFindData))
    {
      ::FindClose (hFind);
      ATOM_DELETE(pFindData);
      return 0;
    }

    // skip '..'
    if (!::FindNextFile (hFind, pFindData))
    {
      ::FindClose (hFind);
      ATOM_DELETE(pFindData);
      return 0;
    }

    ident[len] = '\0';

    ATOM_PhysicVFS::finddata_physic_t *fd = ATOM_NEW(ATOM_PhysicVFS::finddata_physic_t);
    fd->finddata = pFindData;
    fd->findhandle = (void*)hFind;
    strcpy (fd->filename, ident);
    strcat (fd->filename, pFindData->cFileName);
    fd->pathlen = len;
    fd->filenameNoPath = fd->filename + fd->pathlen;
    fd->type = (pFindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? ATOM_VFS::directory : ATOM_VFS::file;

    return fd;
  }

  return 0;
}

bool ATOM_PhysicVFS::findNext (finddata_t *handle)
{
  if (handle)
  {
    ATOM_PhysicVFS::finddata_physic_t *fd = (ATOM_PhysicVFS::finddata_physic_t*)handle;
    WIN32_FIND_DATA *pFindData = (WIN32_FIND_DATA*)fd->finddata;
    if (pFindData)
    {
      HANDLE hFind = (HANDLE)fd->findhandle;
      if (::FindNextFileA (hFind, pFindData))
      {
        strcpy (fd->filename+fd->pathlen, pFindData->cFileName);
        fd->filenameNoPath = fd->filename + fd->pathlen;
        handle->type = (pFindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? ATOM_VFS::directory : ATOM_VFS::file;
        return true;
      }
    }
  }
  return false;
}

void ATOM_PhysicVFS::findClose (finddata_t *handle)
{
  if (handle)
  {
    ATOM_PhysicVFS::finddata_physic_t *fd = (ATOM_PhysicVFS::finddata_physic_t*)handle;
    ::FindClose ((HANDLE)fd->findhandle);
    ATOM_DELETE(((WIN32_FIND_DATA*)fd->finddata));
    ATOM_DELETE(handle);
  }
}

bool ATOM_PhysicVFS::doesFileExists (const char *file)
{
  if (file)
  {
    char ident[ATOM_VFS::max_filename_length];
    if (!identifyPath (file, ident))
    {
      return false;
    }

    return ::GetFileAttributesA (ident) != 0xFFFFFFFF;
  }
  return false;
}

bool ATOM_PhysicVFS::isDir (const char *file)
{
  if (file)
  {
    char ident[ATOM_VFS::max_filename_length];
    if (!identifyPath (file, ident))
    {
      return false;
    }

    DWORD attrib = ::GetFileAttributesA (ident);
    return attrib != 0xFFFFFFFF && (attrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
  }

  return false;
}

bool ATOM_PhysicVFS::mkDir (const char *dir)
{
  if (dir)
  {
    char ident[ATOM_VFS::max_filename_length];
    if (!identifyPath (dir, ident))
    {
      return false;
    }

    DWORD attrib = ::GetFileAttributesA (ident);
    if (attrib != 0xFFFFFFFF)
    {
      return false;
    }

    char *p = strchr (ident, '\\');
    assert (p);
    while (p)
    {
      ++p;
      char tmp = *p;
      *p = '\0';

      attrib = ::GetFileAttributesA (ident);
      if (attrib == 0xFFFFFFFF)
      {
        if (!::CreateDirectory (ident, NULL))
        {
          return false;
        }
        else
        {
          *p = tmp;
          p = strchr(p, '\\');
        }
      }
      else if (!(attrib & FILE_ATTRIBUTE_DIRECTORY))
      {
        return false;
      }
      else
      {
        *p = tmp;
        p = strchr(p, '\\');
      }
    }

	attrib = ::GetFileAttributesA (ident);
	if (attrib != 0xFFFFFFFF && (attrib & FILE_ATTRIBUTE_DIRECTORY) != 0)
	{
		return true;
	}

    if (!::CreateDirectory (ident, NULL))
    {
      return false;
    }

    return true;
  }

  return false;
}

bool ATOM_PhysicVFS::removeFile (const char *file, bool force)
{
  if (file)
  {
    char ident[ATOM_VFS::max_filename_length];
    if (!identifyPath (file, ident))
    {
      return false;
    }

    DWORD attrib = ::GetFileAttributesA (ident);
    if (attrib == 0xFFFFFFFF)
    {
      return false;
    }
    else if ((attrib & FILE_ATTRIBUTE_DIRECTORY) != 0)
    {
      if (::RemoveDirectoryA (ident))
      {
        return true;
      }

      if (!force)
      {
        return false;
      }

      return _rmdir_r (ident);
    }
    else
    {
      return ::DeleteFileA (ident) == TRUE;
    }
  }

  return false;
}

bool ATOM_PhysicVFS::_rmdir_r (const char *dir)
{
  char ident[MAX_PATH];
  if (!identifyPath (dir, ident))
  {
    return false;
  }

  char *tmp = ident+strlen(ident);
  strcat (ident, "\\*");
  WIN32_FIND_DATA finddata;
  HANDLE hFind = ::FindFirstFileA (ident, &finddata);
  if (hFind == INVALID_HANDLE_VALUE)
  {
    return false;
  }
  *tmp = '\0';

  do
  {
    DWORD attrib = ::GetFileAttributesA (finddata.cFileName);
    if ((attrib & FILE_ATTRIBUTE_DIRECTORY) != 0)
    {
      if (strcmp(finddata.cFileName, ".") && strcmp(finddata.cFileName, ".."))
      {
        strcat (ident, "\\");
        strcat (ident, finddata.cFileName);
        if (!_rmdir_r (ident))
        {
          ::FindClose (hFind);
          return false;
        }
        *tmp = '\0';
      }
    }
    else
    {
      strcat (ident, "\\");
      strcat (ident, finddata.cFileName);
      if (!::DeleteFileA (ident))
      {
        ::FindClose (hFind);
        return false;
      }
      *tmp = '\0';
    }
  }
  while (::FindNextFileA (hFind, &finddata));

  ::FindClose (hFind);

  return ::RemoveDirectoryA (ident) == TRUE;
}

void ATOM_PhysicVFS::getCWD (char *buffer) const
{
  ::GetCurrentDirectoryA (ATOM_VFS::max_filename_length, buffer);
}

bool ATOM_PhysicVFS::chDir (const char *dir)
{
  return ::SetCurrentDirectory (dir) == TRUE;
}

char *ATOM_PhysicVFS::identifyPath (const char *dir, char *buffer) const
{
  if (!::GetFullPathNameA (dir, ATOM_VFS::max_filename_length, buffer, NULL))
  {
    return 0;
  }

  return buffer;
}

ATOM_VFS *ATOM_PhysicVFS::getNativePath (const char *mount_path, char *buffer) const
{
  return 0;
}

ATOM_VFS::handle ATOM_PhysicVFS::vfopen (const char *filename, int mode)
{
  if (filename)
  {
    int oflag = 0;
	int pmode = 0;

    if ((mode & ATOM_VFS::read) != 0 && (mode & ATOM_VFS::write) != 0)
    {
      oflag |= _O_RDWR;
	  pmode = _S_IREAD|_S_IWRITE;
    }
    else if ((mode & ATOM_VFS::read) != 0)
    {
      oflag |= _O_RDONLY;
	  pmode = _S_IREAD;
    }
    else if ((mode & ATOM_VFS::write) != 0)
    {
      oflag |= _O_WRONLY;
      oflag |= _O_CREAT;
	  pmode = _S_IWRITE;
    }
    else
    {
      // no openmode
      return ATOM_VFS::invalid_handle;
    }

    if ((mode & ATOM_VFS::text) != 0)
    {
      oflag |= _O_TEXT;
    }
    else
    {
      oflag |= _O_BINARY;
    }

    if ((mode & ATOM_VFS::append) != 0)
    {
      if ((mode & ATOM_VFS::trunc) != 0)
      {
        return ATOM_VFS::invalid_handle;
      }

      oflag |= _O_APPEND;
    }
    else if ((mode & ATOM_VFS::write) != 0)
    {
      oflag |= _O_TRUNC;
    }

    char identPath[ATOM_VFS::max_filename_length];

    if (!identifyPath (filename, identPath))
    {
      return ATOM_VFS::invalid_handle;
    }

    int fd = _open (identPath, oflag, pmode);
    if (fd == -1)
    {
      return ATOM_VFS::invalid_handle;
    }

    ATOM_PhysicVFS::filedesc_physic_t *f = (ATOM_PhysicVFS::filedesc_physic_t*)_allocFileDesc ();
    f->fd = fd;
    f->owner = this;
    f->vfs = this;
    strcpy (f->filename, identPath);

    return (ATOM_VFS::handle)f;
  }

  return ATOM_VFS::invalid_handle;
}

unsigned ATOM_PhysicVFS::vfread (ATOM_VFS::handle fd, void *buffer, unsigned size)
{
  if (fd != ATOM_VFS::invalid_handle && buffer && size)
  {
    ATOM_PhysicVFS::filedesc_physic_t *f = (ATOM_PhysicVFS::filedesc_physic_t*)fd;
    return _read (f->fd, buffer, size);
  }
  return 0;
}

unsigned ATOM_PhysicVFS::vfwrite (ATOM_VFS::handle fd, const void *buffer, unsigned size)
{
  if (fd != ATOM_VFS::invalid_handle && buffer && size)
  {
    ATOM_PhysicVFS::filedesc_physic_t *f = (ATOM_PhysicVFS::filedesc_physic_t*)fd;
    return _write (f->fd, buffer, size);
  }
  return 0;
}

long ATOM_PhysicVFS::vfseek (ATOM_VFS::handle fd, long offset, int origin)
{
  if (fd != ATOM_VFS::invalid_handle)
  {
    ATOM_PhysicVFS::filedesc_physic_t *f = (ATOM_PhysicVFS::filedesc_physic_t*)fd;
    int o;
    switch (origin)
    {
    case ATOM_VFS::begin: 
      o = SEEK_SET;
      break;
    case ATOM_VFS::current:
      o = SEEK_CUR;
      break;
    case ATOM_VFS::end:
      o = SEEK_END;
      break;
    default:
      return -1L;
    }

    return _lseek (f->fd, offset, o);
  }

  return -1L;
}

long ATOM_PhysicVFS::vftell (ATOM_VFS::handle fd)
{
  if (fd != ATOM_VFS::invalid_handle)
  {
    ATOM_PhysicVFS::filedesc_physic_t *f = (ATOM_PhysicVFS::filedesc_physic_t*)fd;
    return _tell (f->fd);
  }
  return -1L;
}

long ATOM_PhysicVFS::vfsize (ATOM_VFS::handle fd)
{
  if (fd != ATOM_VFS::invalid_handle)
  {
    ATOM_PhysicVFS::filedesc_physic_t *f = (ATOM_PhysicVFS::filedesc_physic_t*)fd;
    return _filelength (f->fd);
  }
  return -1L;
}

void ATOM_PhysicVFS::vfclose (ATOM_VFS::handle fd)
{
  if (fd != ATOM_VFS::invalid_handle)
  {
    ATOM_PhysicVFS::filedesc_physic_t *f = (ATOM_PhysicVFS::filedesc_physic_t*)fd;
    _close (f->fd);
    _freeFileDesc (f);
  }
}

bool ATOM_PhysicVFS::vfeof (ATOM_VFS::handle f)
{
  if (f != ATOM_VFS::invalid_handle)
  {
    ATOM_PhysicVFS::filedesc_physic_t *fd = (ATOM_PhysicVFS::filedesc_physic_t*)f;
    bool ret = _eof (fd->fd) != 0;
    _freeFileDesc (fd);
    return ret;
  }
  return false;
}

#else

#error not implemented

#endif
