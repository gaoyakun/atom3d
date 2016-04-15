#include "StdAfx.h"
#include "native_ops.h"

ObjectAttribModifyOp::ObjectAttribModifyOp (ATOM_Object *object, const char *prop, const ATOM_ScriptVar &oldVal, const ATOM_ScriptVar &newVal)
: _object(object)
, _property(prop)
, _oldValue(oldVal)
, _newValue(newVal)
{
}

void ObjectAttribModifyOp::undo (void)
{
	ATOM_SetAttribValue (_object.get(), _property.c_str(), _oldValue);
}

void ObjectAttribModifyOp::redo (void)
{
	ATOM_SetAttribValue (_object.get(), _property.c_str(), _newValue);
}

void ObjectAttribModifyOp::deleteMe (void)
{
	ATOM_DELETE(this);
}

NodeTransformOp::NodeTransformOp (ATOM_Node *node, const ATOM_Matrix4x4f &oldMatrix, const ATOM_Matrix4x4f &newMatrix)
: _node(node)
, _oldMatrix(oldMatrix)
, _newMatrix(newMatrix)
{
}

void NodeTransformOp::undo (void)
{
	_node->setO2T (_oldMatrix);
}

void NodeTransformOp::redo (void)
{
	_node->setO2T (_newMatrix);
}

void NodeTransformOp::deleteMe (void)
{
	ATOM_DELETE(this);
}

NodeCreateOp::NodeCreateOp (ATOM_Node *nodeChild, ATOM_Node *nodeParent)
: _child(nodeChild)
, _parent(nodeParent)
{
}

void NodeCreateOp::undo (void)
{
	_parent->removeChild (_child.get());
}

void NodeCreateOp::redo (void)
{
	_parent->appendChild (_child.get());
}

void NodeCreateOp::deleteMe (void)
{
	ATOM_DELETE(this);
}

NodeDeleteOp::NodeDeleteOp (ATOM_Node *nodeChild, ATOM_Node *nodeParent)
: _child(nodeChild)
, _parent(nodeParent)
{
}

void NodeDeleteOp::undo (void)
{
	_parent->appendChild (_child.get());
}

void NodeDeleteOp::redo (void)
{
	_parent->removeChild (_child.get());
}

void NodeDeleteOp::deleteMe (void)
{
	ATOM_DELETE(this);
}

NodeReplaceOp::NodeReplaceOp (ATOM_Node *newNode, ATOM_Node *oldNode)
: _newNode(newNode)
, _oldNode(oldNode)
{
}

void NodeReplaceOp::undo (void)
{
	ATOM_Node *parent = _newNode->getParent();
	if (parent)
	{
		parent->removeChild (_newNode.get());
		parent->appendChild (_oldNode.get());
	}
}

void NodeReplaceOp::redo (void)
{
	ATOM_Node *parent = _oldNode->getParent();
	if (parent)
	{
		_newNode->setO2T (_oldNode->getO2T());
		parent->removeChild (_oldNode.get());
		parent->appendChild (_newNode.get());
	}
}

void NodeReplaceOp::deleteMe (void)
{
	ATOM_DELETE(this);
}


