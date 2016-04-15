#include "stdafx.h"

ATOM_File::ATOM_File(ATOM_VFS::handle handle)
  : _M_handle(handle) 
{
}

ATOM_File::~ATOM_File() 
{
}

const char* ATOM_File::getName() const {
  return ATOM_VFS::vfsGetFileName (_M_handle);
}

unsigned long ATOM_File::read(void* buffer, unsigned long size) 
{
  return ATOM_VFS::vfsRead (_M_handle, buffer, size);
}

unsigned long ATOM_File::write(const void* buffer, unsigned long size) 
{
  return ATOM_VFS::vfsWrite (_M_handle, buffer, size);
}

unsigned long ATOM_File::getPosition() const 
{
  return ATOM_VFS::vfsTell (_M_handle);
}

bool ATOM_File::seek(long offset, int origin) 
{
  return ATOM_VFS::vfsSeek (_M_handle, offset, origin) != -1;
}

unsigned long ATOM_File::size() const 
{
  return ATOM_VFS::vfsSize (_M_handle);
}

bool ATOM_File::eof() const 
{
  return ATOM_VFS::vfsEof (_M_handle);
}

ATOM_VFS::handle ATOM_File::getHandle (void) const
{
  return _M_handle;
}

unsigned long ATOM_File::printf(const char *format, ...)
{
	char buff[2048];

	va_list args;
	va_start(args, format);
	vsnprintf (buff, 2048, format, args);
	va_end(args);

	return write (buff, strlen(buff));
}

