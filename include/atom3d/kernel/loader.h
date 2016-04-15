#ifndef __ATOM3D_KERNEL_ASYNC_LOADER_H
#define __ATOM3D_KERNEL_ASYNC_LOADER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "basedefs.h"
#include "refobj.h"

class ATOM_BaseResourceCreator;
struct ATOM_LoadingRequest;

class ATOM_KERNEL_API ATOM_BaseResourceLoader: private ATOM_Noncopyable
{
public:
	enum LOADRESULT
	{
		LOADERR_OK,
		LOADERR_FAILED,
		LOADERR_TRYAGAIN
	};

	enum LOADSTAGE
	{
		LOADSTAGE_FAILED,
		LOADSTAGE_LOADED,
		LOADSTAGE_LOADING
	};

public:
	ATOM_BaseResourceLoader (void);
	ATOM_BaseResourceLoader (const char *filename);
	virtual ~ATOM_BaseResourceLoader (void);

public:
	LOADRESULT getResult (void) const;
	void setFileName (const char *filename);
	unsigned char *getData (void) const;
	unsigned getDataSize (void) const;
	void setLoadingRequest (ATOM_LoadingRequest *request);
	ATOM_LoadingRequest *getLoadingRequest (void) const;

public:
	virtual LOADRESULT loadFromDisk (void);
	virtual void unload (void);
	virtual bool lock (void) = 0;
	virtual int unlock (void) = 0;
	virtual bool realize (void) = 0;

protected:
	ATOM_STRING _filename;
	unsigned char*_fileContent;
	unsigned _fileSize;
	LOADRESULT _result;
	ATOM_LoadingRequest *_request;
};

#endif // __ATOM3D_KERNEL_ASYNC_LOADER_H
