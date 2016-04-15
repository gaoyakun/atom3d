#include "StdAfx.h"
#include "loader.h"
#include "contentstream.h"

ATOM_BaseResourceLoader::ATOM_BaseResourceLoader (void)
{
	_fileContent = 0;
	_fileSize = 0;
}

ATOM_BaseResourceLoader::ATOM_BaseResourceLoader (const char *filename)
{
	_result = LOADERR_OK;
	_filename = filename;
	_fileContent = 0;
	_fileSize = 0;
}

void ATOM_BaseResourceLoader::setLoadingRequest (ATOM_LoadingRequest *request)
{
	_request = request;
}

ATOM_LoadingRequest *ATOM_BaseResourceLoader::getLoadingRequest (void) const
{
	return _request;
}

void ATOM_BaseResourceLoader::setFileName (const char *filename)
{
	_filename = filename ? filename : "";
}

ATOM_BaseResourceLoader::~ATOM_BaseResourceLoader (void)
{
	ATOM_FREE(_fileContent);
}

ATOM_BaseResourceLoader::LOADRESULT ATOM_BaseResourceLoader::getResult (void) const
{
	return _result;
}

ATOM_BaseResourceLoader::LOADRESULT ATOM_BaseResourceLoader::loadFromDisk (void)
{
	if (!_filename.empty ())
	{
		ATOM_AutoFile f(_filename.c_str(), ATOM_VFS::read);
		if (!f)
		{
			return LOADERR_FAILED;
		}

		_fileSize = f->size ();
		_fileContent = (unsigned char*)ATOM_MALLOC (_fileSize);
		if (!_fileContent)
		{
			return LOADERR_FAILED;
		}

		if (_fileSize != f->read (_fileContent, _fileSize))
		{
			return LOADERR_FAILED;
		}
	}

	return LOADERR_OK;
}

void ATOM_BaseResourceLoader::unload (void)
{
	ATOM_FREE (_fileContent);
}

unsigned char *ATOM_BaseResourceLoader::getData (void) const
{
	return _fileContent;
}

unsigned ATOM_BaseResourceLoader::getDataSize (void) const
{
	return _fileSize;
}
