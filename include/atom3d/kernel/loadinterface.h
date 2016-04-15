#ifndef __ATOM3D_KERNEL_LOADINTERFACE_H
#define __ATOM3D_KERNEL_LOADINTERFACE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"
#include "refobj.h"
#include "contentstream.h"

class ATOM_BaseResourceLoader;

class ATOM_KERNEL_API ATOM_LoadInterface: public ATOM_ReferenceObj
{
public:
	enum LoadingState
	{
		LS_NOTLOADED,
		LS_LOADING,
		LS_LOADFAILED,
		LS_LOADED
	};

	enum
	{
		LF_ORDERED = (1<<0),
		LF_HIGH_PRIORITY = (1<<1)
	};

public:
	ATOM_LoadInterface (void);
	virtual ~ATOM_LoadInterface (void);

public:
	virtual LoadingState getLoadingState (void) const;
	virtual void setLoadingState (LoadingState state);
	virtual const char *getFileName (void) const;
	virtual void setFileName (const char *filename);
	virtual LoadingState load (void) = 0;
	virtual LoadingState mtload (unsigned flags = 0, long group = 0, ATOM_LoadingRequestCallback callback = 0, void *userData = 0) = 0;
	virtual void unload (void) = 0;
	virtual void finishLoad (void) = 0;
	virtual void insureLoadDone (void);
	virtual ATOM_BaseResourceLoader *getLoader (void) = 0;

protected:
	LoadingState _state;
	ATOM_STRING _filename;
};

#endif // __ATOM3D_KERNEL_LOADINTERFACE_H
