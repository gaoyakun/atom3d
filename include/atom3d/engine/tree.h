#ifndef __ATOM3D_TREE_H
#define __ATOM3D_TREE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "treeinstance.h"

class ATOM_ENGINE_API ATOM_Tree: public ATOM_VisualNode
{
public:
	ATOM_Tree (void);
	virtual ~ATOM_Tree (void);

public:
	virtual void buildBoundingbox (void) const;
	virtual void setupRenderQueue (ATOM_CullVisitor *visitor);
	virtual bool rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const;
	virtual bool supportFixedFunction (void) const;

protected:
	virtual bool onLoad(ATOM_RenderDevice *device);
	virtual void assign(ATOM_Node *other) const;

public:
	void setFileName (const ATOM_STRING &fileName);
	const ATOM_STRING &getFileName (void) const;
	void setLODDistance (float distance);
	float getLODDistance (void) const;

private:
	ATOM_STRING _fileName;
	float _LODdistance;
	ATOM_AUTOREF(ATOM_TreeInstance) _instance;

	ATOM_CLASS(engine, ATOM_Tree, ATOM_Tree)
    ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_Tree)
	ATOM_DECLARE_NODE_FILE_TYPE(ATOM_Tree, ATOM_Tree, "atr", "ATOM3D tree")
};

#endif // __ATOM3D_TREE_H
