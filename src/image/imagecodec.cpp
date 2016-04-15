#include "StdAfx.h"
#ifdef __CODEGEARC__
# pragma hdrstop
#endif

#ifdef __CODEGEARC__
# define _stricmp stricmp
#endif

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_ImageCodec)
	ATOM_ATTRIBUTES_BEGIN(ATOM_ImageCodec)
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_ImageCodec, ATOM_Object)

ATOM_ImageCodec::~ATOM_ImageCodec (void) 
{
	ATOM_STACK_TRACE(ATOM_ImageCodec::~ATOM_ImageCodec);
}

int ATOM_ImageCodec::guessTypeByFileName (const char *fn) 
{
	ATOM_STACK_TRACE(ATOM_ImageCodec::guessTypeByFileName);
	ATOM_ASSERT (fn);

	const char *p = strrchr(fn, '.');
	return ATOM_ImageCodec::guessTypeByExtension(p ? p+1 : 0);
}

int ATOM_ImageCodec::guessTypeByExtension(const char* ext) 
{
	ATOM_STACK_TRACE(ATOM_ImageCodec::guessTypeByExtension);
	if ( ext)
	{
		if ( _stricmp(ext, "jpg") == 0 || _stricmp(ext, "jpeg") == 0 || _stricmp(ext, "jpe") == 0)
		{
			return JPEG;
		}

		if ( _stricmp(ext, "png") == 0)
		{
			return PNG;
		}

		if ( _stricmp(ext, "bmp") == 0)
		{
			return BMP;
		}

		if ( _stricmp(ext, "gif") == 0)
		{
			return GIF;
		}

		if ( _stricmp(ext, "dds") == 0)
		{
			return DDS;
		}

		if ( _stricmp(ext, "tif") == 0 || _stricmp(ext, "tiff") == 0)
		{
			return TIFF;
		}
	}

	return ANY;
}
