#include "stdafx.h"

ATOM_RWopsMem::ATOM_RWopsMem(void* mem, unsigned length) 
{
	_M_start = (char *) mem;
	_M_pos = (char *) mem;
	_M_end = _M_start + length;
	_M_eof = false;
}

ATOM_RWopsMem::~ATOM_RWopsMem (void) 
{
	_M_start = 0;
	_M_pos = 0;
	_M_end = 0;
}

ATOM_RWops::RWOPS_TYPE ATOM_RWopsMem::getType (void) const 
{
	return ATOM_RWops::TYPE_MEM;
}

int ATOM_RWopsMem::tell (void) const 
{
	return _M_pos - _M_start;
}

unsigned long ATOM_RWopsMem::size (void)
{
	return _M_end - _M_start;
}

int ATOM_RWopsMem::seek (int offset, int whence) 
{
	switch ( whence)
	{
	case SEEK_SET:
		_M_pos = _M_start + offset;
		break;
	case SEEK_CUR:
		_M_pos = _M_pos + offset;
		break;
	case SEEK_END:
		_M_pos = _M_end + offset;
		break;
	default:
		return -1;
	}

	if ( _M_pos > _M_end)
	{
		_M_eof = true;
		_M_pos = _M_end;
	}
	else
	{
		_M_eof = false;
		if ( _M_pos < _M_start)
		{
			_M_pos = _M_start;
		}
	}

	return _M_pos - _M_start;
}

unsigned ATOM_RWopsMem::read (void* buffer, unsigned size, unsigned count) 
{
	unsigned num = count;
	if ( (_M_pos + num * size) > _M_end)
	{
		_M_eof = true;
		num = (_M_end - _M_pos) / size;
	}

	memcpy(buffer, _M_pos, num * size);
	_M_pos += num * size;

	return num;
}

unsigned ATOM_RWopsMem::write(const void* buffer, unsigned size, unsigned count) 
{
	unsigned num = count;
	if ( (_M_pos + num * size) > _M_end)
	{
		_M_eof = true;
		num = (_M_end - _M_pos) / size;
	}

	memcpy(_M_pos, buffer, num * size);
	_M_pos += num * size;

	return num;
}

bool ATOM_RWopsMem::eof (void) const 
{
	return _M_eof;
}

char ATOM_RWopsMem::getC (void) 
{
	char ret;
	read(&ret, 1, 1);
	return ret;
}

