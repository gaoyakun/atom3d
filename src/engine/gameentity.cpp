#include "StdAfx.h"
#include "gameentity.h"



ATOM_GameEntity::ATOM_GameEntity (void)
{
}

//ATOM_GameEntity::ATOM_GameEntity (const char *name, ATOM_Node *node)
////: _name(name ? name : "")
//{
//	if (node)
//	{
//		attachNode (node);
//	}
//}

ATOM_GameEntity::~ATOM_GameEntity (void)
{
//	detachNode ();
}

//void ATOM_GameEntity::setName (const char *name)
//{
//	_name = name ? name : "";
//}
//
//const char *ATOM_GameEntity::getName (void) const
//{
//	return _name.c_str();
//}
//
//ATOM_Node *ATOM_GameEntity::getAttachedNode (void) const
//{
//	return _node.get();
//}

void ATOM_GameEntity::attachNode (ATOM_Node *node)
{
	ATOM_ASSERT(node);

	class AttachVisitor: public ATOM_Visitor
	{
	public:
		AttachVisitor (ATOM_GameEntity *entity): _entity (entity) {}
		ATOM_GameEntity *_entity;

	public:
		virtual void visit (ATOM_Node &node)
		{
			//ATOM_ASSERT(!node.getGameEntity());
			node.setGameEntity (_entity);
		}
	};

//	_node = node;

	AttachVisitor v(this);
	v.traverse (*node);
}

void ATOM_GameEntity::detachNode (ATOM_Node *node)
{
	class DetachVisitor: public ATOM_Visitor
	{
	public:
		DetachVisitor (ATOM_GameEntity *entity): _entity (entity) {}
		ATOM_GameEntity *_entity;

	public:
		virtual void visit (ATOM_Node &node)
		{
			//ATOM_ASSERT(node.getGameEntity() == _entity);
			node.setGameEntity (0);
		}
	};

	if (node)
	{
		DetachVisitor v(this);
		v.traverse (*node);
		node = 0;
	}
}


