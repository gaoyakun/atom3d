#ifndef __ATOM3D_ENGINE_LOADINTERFACE_H
#define __ATOM3D_ENGINE_LOADINTERFACE_H

#include "../ATOM_kernel.h"
#include "basedefs.h"

class ATOM_Model;
class ATOM_ModelLoader;

class ATOM_ModelLoadInterface: public ATOM_LoadInterface
{
public:
	ATOM_ModelLoadInterface (ATOM_Model *owner);
	virtual ~ATOM_ModelLoadInterface (void);

public:
	virtual ATOM_LoadInterface::LoadingState load (void);
	virtual ATOM_LoadInterface::LoadingState mtload (unsigned flags, long group, ATOM_LoadingRequestCallback callback, void *userData);
	virtual void unload (void);
	virtual void finishLoad (void);
	virtual void insureLoadDone (void);
	virtual ATOM_BaseResourceLoader *getLoader (void);

private:
	ATOM_Model *_model;
	ATOM_ModelLoader *_loader;
};

class ATOM_NodeLoadInterface: public ATOM_LoadInterface
{
public:
	ATOM_NodeLoadInterface (ATOM_Node *owner);
	virtual ~ATOM_NodeLoadInterface (void);

public:
	virtual ATOM_LoadInterface::LoadingState load (void);
	virtual ATOM_LoadInterface::LoadingState mtload (unsigned flags, long group, ATOM_LoadingRequestCallback callback, void *userData);
	virtual void unload (void);
	virtual void finishLoad (void);
	virtual ATOM_BaseResourceLoader *getLoader (void);

protected:
	ATOM_Node *_node;
};

/*
class ATOM_GeodeLoadInterface: public ATOM_LoadInterface
{
public:
	ATOM_GeodeLoadInterface (ATOM_BaseGeode *owner);
	virtual ~ATOM_GeodeLoadInterface (void);

public:
	virtual ATOM_LoadInterface::LoadingState load (void);
	virtual ATOM_LoadInterface::LoadingState mtload (unsigned flags, long group, ATOM_LoadingRequestCallback callback, void *userData);
	virtual void unload (void);
	virtual void finishLoad (void);
	virtual void insureLoadDone (void);
	virtual ATOM_BaseResourceLoader *getLoader (void);

private:
	ATOM_BaseGeode *_geode;
	ATOM_GeodeLoader *_loader;
};
*/
#endif // __ATOM3D_ENGINE_LOADINTERFACE_H
