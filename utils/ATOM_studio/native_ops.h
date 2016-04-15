#ifndef __ATOM3D_STUDIO_NATIVE_OPS_H
#define __ATOM3D_STUDIO_NATIVE_OPS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "operation.h"

class ObjectAttribModifyOp: public AS_Operation
{
public:
	ObjectAttribModifyOp (ATOM_Object *object, const char *prop, const ATOM_ScriptVar &oldVal, const ATOM_ScriptVar &newVal);

public:
	virtual void undo (void);
	virtual void redo (void);
	virtual void deleteMe (void);

private:
	ATOM_AUTOREF(ATOM_Object) _object;
	ATOM_STRING _property;
	ATOM_ScriptVar _oldValue;
	ATOM_ScriptVar _newValue;
};

class NodeTransformOp: public AS_Operation
{
public:
	NodeTransformOp (ATOM_Node *node, const ATOM_Matrix4x4f &oldMatrix, const ATOM_Matrix4x4f &newMatrix);

public:
	virtual void undo (void);
	virtual void redo (void);
	virtual void deleteMe (void);

private:
	ATOM_AUTOREF(ATOM_Node) _node;
	ATOM_Matrix4x4f _oldMatrix;
	ATOM_Matrix4x4f _newMatrix;
};

class NodeCreateOp: public AS_Operation
{
public:
	NodeCreateOp (ATOM_Node *nodeChild, ATOM_Node *nodeParent);

public:
	virtual void undo (void);
	virtual void redo (void);
	virtual void deleteMe (void);

private:
	ATOM_AUTOREF(ATOM_Node) _child;
	ATOM_AUTOREF(ATOM_Node) _parent;
};

class NodeDeleteOp: public AS_Operation
{
public:
	NodeDeleteOp (ATOM_Node *nodeChild, ATOM_Node *nodeParent);

public:
	virtual void undo (void);
	virtual void redo (void);
	virtual void deleteMe (void);

private:
	ATOM_AUTOREF(ATOM_Node) _child;
	ATOM_AUTOREF(ATOM_Node) _parent;
};

class NodeReplaceOp: public AS_Operation
{
public:
	NodeReplaceOp (ATOM_Node *newNode, ATOM_Node *oldNode);

public:
	virtual void undo (void);
	virtual void redo (void);
	virtual void deleteMe (void);

private:
	ATOM_AUTOREF(ATOM_Node) _newNode;
	ATOM_AUTOREF(ATOM_Node) _oldNode;
};

#endif // __ATOM3D_STUDIO_NATIVE_OPS_H
