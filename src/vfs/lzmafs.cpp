#include <assert.h>
#include <direct.h>
#include <ATOM_dbghlp.h>
#include <ATOM_lzmapkg.h>
#include "lzmafs.h"

ATOM_LzmaVFS::ATOM_LzmaVFS (void)
{
	_fp = NULL;
}

bool ATOM_LzmaVFS::load (const char *filename, const char *password)
{
	ATOM_Mutex::ScopeMutex lock(_M_mutex);

	unload ();

	if (!filename)
	{
		return false;
	}

	std::vector<ATOM_LzmaFilePackage::CompressedFileInfo> infos;
	ATOM_LzmaFilePackage pkg;
	pkg.getCompressedFileInfoList (filename, infos);
	if (infos.empty ())
	{
		return false;
	}

	_filename = filename;
	_fp = fopen (filename, "rb");
	if (!_fp)
	{
		return false;
	}

	for (int i = 0; i < infos.size(); ++i)
	{
		std::transform (infos[i].filename.begin(), infos[i].filename.end(), infos[i].filename.begin(), ::tolower);
		_infos[infos[i].filename.c_str()] = infos[i];
	}

	return true;
}

ATOM_VFS::finddata_t *ATOM_LzmaVFS::findFirst (const char *dir)
{
	return 0;
}

bool ATOM_LzmaVFS::findNext (ATOM_VFS::finddata_t *handle)
{
	return false;
}

void ATOM_LzmaVFS::findClose (ATOM_VFS::finddata_t *handle)
{
}

void ATOM_LzmaVFS::unload (void)
{
	ATOM_Mutex::ScopeMutex lock(_M_mutex);

	_filename = "";
	_infos.clear ();

	if (_fp)
	{
		fclose (_fp);
	}
}

ATOM_LzmaVFS::~ATOM_LzmaVFS (void)
{
	unload ();
}

bool ATOM_LzmaVFS::isCaseSensitive (void) const
{
	return false;
}

bool ATOM_LzmaVFS::mkDir (const char *dir)
{
	// not implement
	return false;
}

bool ATOM_LzmaVFS::isDir (const char *dir)
{
	return false;
}

bool ATOM_LzmaVFS::removeFile (const char *file, bool force)
{
	// not implementation
	return false;
}

ATOM_VFS::handle ATOM_LzmaVFS::vfopen (const char *filename, int mode)
{
	ATOM_Mutex::ScopeMutex lock(_M_mutex);

	if (mode & ATOM_VFS::write)
	{
		return false;
	}

	if (!_fp)
	{
		return ATOM_VFS::invalid_handle;
	}

	char buffer[ATOM_VFS::max_filename_length];
	identifyPath (filename, buffer);
	strlwr (buffer);
	ATOM_HASHMAP<ATOM_STRING, ATOM_LzmaFilePackage::CompressedFileInfo>::const_iterator it = _infos.find (buffer);
	if (it == _infos.end ())
	{
		return ATOM_VFS::invalid_handle;
	}

	const ATOM_LzmaFilePackage::CompressedFileInfo &info = it->second;

	filedesc_lzma_t *fd = (filedesc_lzma_t*)_allocFileDesc();
	fd->owner = this;
	fd->vfs = this;
	fd->position = 0;
	fd->mode = mode;

	if (!info.compressed)
	{
		fd->data = ATOM_MALLOC(info.size);
		fseek (_fp, info.pos, SEEK_SET);
		fread (fd->data, 1, info.size, _fp);
		strcpy (fd->filename, filename);
		fd->size = info.size;
	}
	else
	{
		void *p = ATOM_MALLOC(info.size);
		fseek (_fp, info.pos, SEEK_SET);
		fread (p, 1, info.size, _fp);
		unsigned uncompressedSize = LZMAGetUncompressedSize (p);
		fd->data = ATOM_MALLOC(uncompressedSize);
		LZMADecompress (p, info.size, fd->data, uncompressedSize);
		ATOM_FREE(p);
		strcpy (fd->filename, filename);
		fd->size = uncompressedSize;
	}

	return (ATOM_VFS::handle)fd;
}

unsigned ATOM_LzmaVFS::vfread (ATOM_VFS::handle f, void *buffer, unsigned size)
{
	if (f == ATOM_VFS::invalid_handle || !buffer)
	{
		return 0;
	}

	filedesc_lzma_t *fd = (filedesc_lzma_t *)f;
	ATOM_VFS *owner = (ATOM_VFS*)fd->owner;
	if (owner != this)
	{
		return 0;
	}

	unsigned remain = fd->size - fd->position;
	if (size > remain)
		size = remain;

	memcpy (buffer, (char*)fd->data+fd->position, size);
	fd->position += size;

	return size;
}

unsigned ATOM_LzmaVFS::vfwrite (ATOM_VFS::handle f, const void *buffer, unsigned size)
{
	// read only, not support write
	return 0;
}

long ATOM_LzmaVFS::vfseek (ATOM_VFS::handle f, long offset, int origin)
{
	if (f == ATOM_VFS::invalid_handle)
	{
		return -1L;
	}

	filedesc_lzma_t *fd = (filedesc_lzma_t*)f;
	ATOM_VFS *owner = (ATOM_VFS*)fd->owner;
	if (owner != this)
	{
		return -1L;
	}

	long pos;
	long sz = fd->size;

	switch (origin)
	{
	case ATOM_VFS::begin:
		pos = offset;
		break;
	case ATOM_VFS::current:
		pos = fd->position + offset;
		break;
	case ATOM_VFS::end:
		pos = sz + offset;
		break;
	default:
		return -1L;
	}

	if (pos < 0 || pos > fd->size)
	{
		return -1L;
	}

	fd->position = pos;

	return pos;
}

long ATOM_LzmaVFS::vftell (ATOM_VFS::handle f)
{
	if (f == ATOM_VFS::invalid_handle)
	{
		return -1L;
	}

	filedesc_lzma_t *fd = (filedesc_lzma_t*)f;
	ATOM_VFS *owner = (ATOM_VFS*)fd->owner;
	if (owner != this)
	{
		return -1L;
	}

	return fd->position;
}

long ATOM_LzmaVFS::vfsize (ATOM_VFS::handle f)
{
	if (f == ATOM_VFS::invalid_handle)
	{
		return 0;
	}

	filedesc_lzma_t *fd = (filedesc_lzma_t*)f;
	ATOM_VFS *owner = (ATOM_VFS*)fd->owner;
	if (owner != this)
	{
		return 0;
	}

	return fd->size;
}

void ATOM_LzmaVFS::vfclose (ATOM_VFS::handle f)
{
	if (f != ATOM_VFS::invalid_handle)
	{
		filedesc_lzma_t *fd = (filedesc_lzma_t*)f;
		ATOM_VFS *owner = (ATOM_VFS*)fd->owner;
		if (owner == this)
		{
			ATOM_FREE (fd->data);
			_freeFileDesc (fd);
		}
	}
}

bool ATOM_LzmaVFS::vfeof (ATOM_VFS::handle f)
{
	if (f != ATOM_VFS::invalid_handle)
	{
		filedesc_lzma_t *fd = (filedesc_lzma_t*)f;
		ATOM_VFS *owner = (ATOM_VFS*)fd->owner;
		if (owner != this)
		{
			return false;
		}
		else
		{
			return fd->position == fd->size;
		}
	}
	return false;
}

ATOM_VFS::filedesc_t *ATOM_LzmaVFS::_newFileDesc (void)
{
	return ATOM_NEW(ATOM_LzmaVFS::filedesc_lzma_t);
}

void ATOM_LzmaVFS::_deleteFileDesc (ATOM_VFS::filedesc_t *fd)
{
	ATOM_DELETE((ATOM_LzmaVFS::filedesc_lzma_t*)fd);
}

char ATOM_LzmaVFS::getSeperator (void) const
{
	return '/';
}

bool ATOM_LzmaVFS::doesFileExists (const char *file)
{
	if (file)
	{
		char buffer[ATOM_VFS::max_filename_length]; 
		identifyPath (file, buffer);
		strlwr (buffer);
		ATOM_HASHMAP<ATOM_STRING, ATOM_LzmaFilePackage::CompressedFileInfo>::const_iterator it = _infos.find (buffer);
		return it != _infos.end ();
	}
	return false;
}

void ATOM_LzmaVFS::getCWD (char *buffer) const
{
	if (buffer)
	{
		strcpy (buffer, "/");
	}
}

bool ATOM_LzmaVFS::chDir (const char *dir)
{
	return false;
}

char *ATOM_LzmaVFS::identifyPath (const char *dir, char *buffer) const
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

	const char *fullpath = dir+1;

	for (;;)
	{
		token_top->s = fullpath;
		const char *next = strchr(fullpath, sep[0]);
		if (next == fullpath)
		{
			return 0;
		}

		unsigned size = next ? next-fullpath : strlen(fullpath);

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
				break;
			}
		}
		else
		{
			break;
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

ATOM_VFS *ATOM_LzmaVFS::getNativePath (const char *mount_path, char *buffer) const
{
	char ident[ATOM_VFS::max_filename_length];
	if (!identifyPath (mount_path, ident))
	{
		return 0;
	}
	strcpy (buffer, ident);
	return 0;
}

