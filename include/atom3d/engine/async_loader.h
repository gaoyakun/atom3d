#ifndef __ATOM3D_ENGINE_ASYNC_LOADER_H
#define __ATOM3D_ENGINE_ASYNC_LOADER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_render.h"
#include "basedefs.h"
#include "model.h"

class ATOM_ENGINE_API ATOM_ModelLoader: public ATOM_BaseResourceLoader
{
public:
	ATOM_ModelLoader (void);
	ATOM_ModelLoader (const char *filename);

public:
	void setModel (ATOM_Model *model);
	ATOM_Model *getModel (void) const;

public:
	virtual LOADRESULT loadFromDisk (void);
	virtual bool lock (void);
	virtual int unlock (void);
	virtual bool realize (void);

private:
	ATOM_AUTOREF(ATOM_Model) _model;
};

class ATOM_ENGINE_API ATOM_BaseNodeLoader: public ATOM_BaseResourceLoader
{
public:
	ATOM_BaseNodeLoader (void);
	ATOM_BaseNodeLoader (const char *filename);

public:
	void setNode (ATOM_Node *node);
	ATOM_Node *getNode (void) const;

public:
	virtual LOADRESULT loadFromDisk (void);
	virtual bool lock (void);
	virtual int unlock (void);
	virtual bool realize (void);

private:
	ATOM_AUTOREF(ATOM_Node) _node;
};

class ATOM_ENGINE_API ATOM_GeodeLoader: public ATOM_BaseNodeLoader
{
public:
	ATOM_GeodeLoader (void);

public:
	void addModelRequest (ATOM_Model *model);
	unsigned getNumModelRequests (void) const;
	ATOM_LoadInterface *getModelRequest (unsigned index) const;

private:
	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Model)> _modelRequests;
};

#endif // __ATOM3D_ENGINE_ASYNC_LOADER_H
