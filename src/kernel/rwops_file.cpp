#include "stdafx.h"

ATOM_RWopsFile::ATOM_RWopsFile(const char* filename, const char* mode) 
{
	_M_fp = fopen(filename, mode);
}

ATOM_RWopsFile::~ATOM_RWopsFile (void) 
{
	if ( _M_fp)
	{
		fclose(_M_fp);
	}
}

ATOM_RWops::RWOPS_TYPE ATOM_RWopsFile::getType (void) const 
{
	return ATOM_RWops::TYPE_FILE;
}

int ATOM_RWopsFile::tell(void) const 
{
	return _M_fp ? ftell(_M_fp) : -1;
}

int ATOM_RWopsFile::seek(int offset, int whence) 
{
	if ( _M_fp == 0)
		return -1;

	fseek(_M_fp, offset, whence);
	return ftell(_M_fp);
}
unsigned long ATOM_RWopsFile::size (void)
{
	if (_M_fp)
	{
		int pos = ftell (_M_fp);
		fseek (_M_fp, 0, SEEK_END);
		unsigned long size = ftell (_M_fp);
		fseek (_M_fp, pos, SEEK_SET);
		return size;
	}
	return 0;
}
unsigned ATOM_RWopsFile::read(void* buffer, unsigned size, unsigned count) 
{
	return _M_fp ? fread(buffer, size, count, _M_fp) : 0;
}

unsigned ATOM_RWopsFile::write(const void* buffer, unsigned size, unsigned count) 
{
	return _M_fp ? fwrite(buffer, size, count, _M_fp) : 0;
}

bool ATOM_RWopsFile::eof() const 
{
	return _M_fp && feof(_M_fp);
}

char ATOM_RWopsFile::getC() 
{
	return _M_fp ? getc(_M_fp) : 0;
}

FILE *ATOM_RWopsFile::getFilePointer (void) const 
{
	return _M_fp;
}

