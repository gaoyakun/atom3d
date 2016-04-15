#include "stdafx.h"
#include <ATOM_vfs.h>

ATOM_RWopsVFS::ATOM_RWopsVFS (ATOM_File* file) 
{
	_M_file = file;
}

ATOM_RWopsVFS::~ATOM_RWopsVFS (void) 
{
	_M_file = 0;
}

ATOM_RWops::RWOPS_TYPE ATOM_RWopsVFS::getType (void) const 
{
	return ATOM_RWops::TYPE_VFS;
}

int ATOM_RWopsVFS::tell (void) const 
{
	if ( _M_file)
		return int(_M_file->getPosition());
	return -1;
}

unsigned long ATOM_RWopsVFS::size (void)
{
	return _M_file->size();
}

int ATOM_RWopsVFS::seek(int offset, int whence) 
{
	if ( _M_file)
	{
		int seekmode;
		switch ( whence)
		{
		case SEEK_SET:
			seekmode = ATOM_VFS::begin;
			break;
		case SEEK_CUR:
			seekmode = ATOM_VFS::current;
			break;
		case SEEK_END:
			seekmode = ATOM_VFS::end;
			break;
		default:
			return -1;
		}

		_M_file->seek(offset, seekmode);
		return int(_M_file->getPosition());
	}
	return -1;
}

unsigned ATOM_RWopsVFS::read (void* buffer, unsigned size, unsigned count) 
{
	if ( size == 0)
		return 0;

	if ( _M_file)
		return _M_file->read(buffer, size * count) / size;

	return 0;
}

unsigned ATOM_RWopsVFS::write (const void* buffer, unsigned size, unsigned count) 
{
	if ( size == 0)
		return 0;

	if ( _M_file)
		return _M_file->write(buffer, size * count) / size;

	return 0;
}

bool ATOM_RWopsVFS::eof (void) const 
{
	return _M_file && _M_file->eof();
}

char ATOM_RWopsVFS::getC (void) 
{
	char ret;
	read(&ret, 1, 1);
	return ret;
}


