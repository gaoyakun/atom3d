#include <cassert>
#include "vfs.h"

ATOM_VFS::ATOM_VFS (void)
{
  _M_num_finddatas = 0;
  _M_num_filedescs = 0;
}

ATOM_VFS::~ATOM_VFS (void)
{
}

ATOM_VFS::finddata_t *ATOM_VFS::_newFindData (void)
{
  return ATOM_NEW(ATOM_VFS::finddata_t);
}

void ATOM_VFS::_deleteFindData (ATOM_VFS::finddata_t *fd)
{
  ATOM_DELETE(fd);
}

ATOM_VFS::finddata_t *ATOM_VFS::_allocFindData (void)
{
  if (_M_num_finddatas == _M_finddatas.size())
  {
    _M_finddatas.push_back (_newFindData());
  }

  ATOM_VFS::finddata_t *ret = _M_finddatas[_M_num_finddatas];
  assert (ret);
  ret->reserved = _M_num_finddatas++;
  return ret;
}

void ATOM_VFS::_freeFindData (ATOM_VFS::finddata_t *fd)
{
  assert (fd);
  assert (_M_num_finddatas > 0);
  unsigned n = fd->reserved;
  _M_finddatas[n] = _M_finddatas[--_M_num_finddatas];
  _M_finddatas[n]->reserved = n;
}

void ATOM_VFS::_purgeFindDatas (void)
{
  for (unsigned i = 0; i < _M_num_finddatas; ++i)
  {
    _deleteFindData (_M_finddatas[i]);
  }
}

ATOM_VFS::filedesc_t *ATOM_VFS::_newFileDesc (void)
{
  return ATOM_NEW(ATOM_VFS::filedesc_t);
}

void ATOM_VFS::_deleteFileDesc (ATOM_VFS::filedesc_t *fd)
{
  ATOM_DELETE(fd);
}

ATOM_VFS::filedesc_t *ATOM_VFS::_allocFileDesc (void)
{
		return _newFileDesc ();
}

void ATOM_VFS::_freeFileDesc (ATOM_VFS::filedesc_t *fd)
{
		_deleteFileDesc (fd);
}

void ATOM_VFS::_purgeFileDescs (void)
{
  for (unsigned i = 0; i < _M_num_filedescs; ++i)
  {
    _deleteFileDesc (_M_filedescs[i]);
  }
}

int ATOM_VFS::_strcmp (const char *s1, const char *s2) const
{
  return isCaseSensitive () ? strcmp (s1, s2) : _stricmp (s1, s2);
}

int ATOM_VFS::_strncmp (const char *s1, const char *s2, unsigned n) const
{
  return isCaseSensitive () ? strncmp (s1, s2, n) : _strnicmp (s1, s2, n);
}

ATOM_VFS::handle ATOM_VFS::vfsOpen (ATOM_VFS *vfs, const char *filename, int mode)
{
  return vfs ? vfs->vfopen (filename, mode) : 0;
}

unsigned ATOM_VFS::vfsRead (ATOM_VFS::handle f, void *buffer, unsigned size)
{
  if (f != ATOM_VFS::invalid_handle)
  {
    ATOM_VFS::filedesc_t *fd = (ATOM_VFS::filedesc_t*)f;
    return fd->owner ? fd->owner->vfread (f, buffer, size) : 0;
  }
  return 0;
}

unsigned ATOM_VFS::vfsWrite (ATOM_VFS::handle f, const void *buffer, unsigned size)
{
  if (f != ATOM_VFS::invalid_handle)
  {
    ATOM_VFS::filedesc_t *fd = (ATOM_VFS::filedesc_t*)f;
    return fd->owner ? fd->owner->vfwrite (f, buffer, size) : 0;
  }
  return 0;
}

bool ATOM_VFS::vfsEof (ATOM_VFS::handle f)
{
  if (f != ATOM_VFS::invalid_handle)
  {
    ATOM_VFS::filedesc_t *fd = (ATOM_VFS::filedesc_t*)f;
    return fd->owner ? fd->owner->vfeof (f) : false;
  }
  return false;
}

long ATOM_VFS::vfsSeek (ATOM_VFS::handle f, long offset, int origin)
{
  if (f != ATOM_VFS::invalid_handle)
  {
    ATOM_VFS::filedesc_t *fd = (ATOM_VFS::filedesc_t*)f;
    return fd->owner ? fd->owner->vfseek (f, offset, origin) : -1;
  }
  return -1;
}

long ATOM_VFS::vfsTell (ATOM_VFS::handle f)
{
  if (f != ATOM_VFS::invalid_handle)
  {
    ATOM_VFS::filedesc_t *fd = (ATOM_VFS::filedesc_t*)f;
    return fd->owner ? fd->owner->vftell (f) : -1;
  }
  return -1;
}

long ATOM_VFS::vfsSize (ATOM_VFS::handle f)
{
  if (f != ATOM_VFS::invalid_handle)
  {
    ATOM_VFS::filedesc_t *fd = (ATOM_VFS::filedesc_t*)f;
    return fd->owner ? fd->owner->vfsize (f) : -1;
  }
  return -1;
}

void ATOM_VFS::vfsClose (ATOM_VFS::handle f)
{
  if (f != ATOM_VFS::invalid_handle)
  {
    ATOM_VFS::filedesc_t *fd = (ATOM_VFS::filedesc_t*)f;

    if (fd->vfs)
    {
      fd->vfs->vfclose (f);
    }
  }
}

static unsigned int crc_32_tab[256];
static bool bInit = false;

static void InitCrc32MakeTable(void)
{
	unsigned int h = 1;
	memset(crc_32_tab,0,sizeof(crc_32_tab));

	for ( int i = 128; i; i >>= 1)
	{
		h = (h >> 1) ^ ((h & 1) ? 0xedb88320L : 0);

		for ( int j = 0; j < 256; j += 2 *i)
		{
			crc_32_tab[i+j] = crc_32_tab[j] ^ h;
		}
	}
}

int calcCRC32 (int lastCRC32, const char *data, unsigned size)
{
	if (!bInit)
	{
		InitCrc32MakeTable ();
		bInit = true;
	}

	if ( !data || size==0)
	{
		return lastCRC32;
	}

	unsigned crc = (unsigned)lastCRC32;
	crc ^= 0xffffffffL;

	while ( size--)
	{
		crc = (crc >> 8) ^ crc_32_tab[(crc ^ *data++) & 0xff];
	}

	return crc ^ 0xffffffffL;
}


int ATOM_VFS::vfsCRC32 (ATOM_VFS::handle f)
{
	int crc = 0;

	if (f != ATOM_VFS::invalid_handle)
	{
		long pos = vfsTell (f);
		long size = vfsSize (f);
		if (size)
		{
			char *buffer = (char*)ATOM_MALLOC(size);
			unsigned sizeRead = vfsRead (f, buffer, size);
			if (sizeRead == size)
			{
				crc = calcCRC32 (0, buffer, size);
			}
			ATOM_FREE(buffer);
		}
		vfsSeek (f, pos, ATOM_VFS::begin);
	}

	return crc;
}

const char* ATOM_VFS::vfsGetFileName (ATOM_VFS::handle f)
{
  if (f != ATOM_VFS::invalid_handle)
  {
    ATOM_VFS::filedesc_t *fd = (ATOM_VFS::filedesc_t*)f;

    return fd->filename;
  }

  return 0;
}

ATOM_VFS* ATOM_VFS::vfsGetVFS (ATOM_VFS::handle f)
{
  if (f != ATOM_VFS::invalid_handle)
  {
    ATOM_VFS::filedesc_t *fd = (ATOM_VFS::filedesc_t*)f;

    return fd->vfs;
  }

  return 0;
}
