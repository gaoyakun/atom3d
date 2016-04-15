#ifndef __ATOM3D_ENGINE_GAMEENTITY_H
#define __ATOM3D_ENGINE_GAMEENTITY_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_kernel.h"

#include "basedefs.h"
#include "node.h"

class ATOM_ENGINE_API ATOM_GameEntity
{
public:
	ATOM_GameEntity (void);
	//ATOM_GameEntity (const char *name, ATOM_Node *node = 0);

	virtual ~ATOM_GameEntity (void);
	//virtual int getEntityType (void) const = 0;

public:
//	void setName (const char *name);
//	const char *getName (void) const;
//	ATOM_Node *getAttachedNode (void) const;
	void attachNode (ATOM_Node *node);
	void detachNode (ATOM_Node *node);

protected:
//	ATOM_STRING _name;
//	ATOM_AUTOREF(ATOM_Node) _node;
};

#endif // __ATOM3D_ENGINE_GAMEENTITY_H
