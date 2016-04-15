#include "StdAfx.h"
#include "visitor.h"
#include "node.h"

ATOM_Visitor::ATOM_Visitor (void)
{
  _cancelTraverse = false;
  _skipChildren = false;
}

ATOM_Visitor::~ATOM_Visitor (void)
{
}

void ATOM_Visitor::cancelTraverse (void)
{
  _cancelTraverse = true;
}

bool ATOM_Visitor::traverseCanceled (void) const
{
  return _cancelTraverse;
}

void ATOM_Visitor::skipChildren (void)
{
  _skipChildren = true;
}

bool ATOM_Visitor::childrenSkipped (void) const
{
  return _skipChildren;
}

void ATOM_Visitor::visit (ATOM_Node &node)
{
}

void ATOM_Visitor::visit (ATOM_NodeOctree &node)
{
  visit ((ATOM_Node&)node);
}

void ATOM_Visitor::visit (ATOM_Terrain &node)
{
  visit ((ATOM_Node&)node);
}

void ATOM_Visitor::visit (ATOM_VisualNode &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_Visitor::visit (ATOM_Geode &node)
{
	visit ((ATOM_VisualNode&)node);
}

void ATOM_Visitor::visit (ATOM_ParticleSystem &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_Visitor::visit (ATOM_Hud &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_Visitor::visit (ATOM_GuiHud &node)
{
	visit ((ATOM_Hud&)node);
}

void ATOM_Visitor::visit (ATOM_Water &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_Visitor::visit (ATOM_LightNode &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_Visitor::visit (ATOM_Atmosphere &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_Visitor::visit (ATOM_Atmosphere2 &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_Visitor::visit (ATOM_Actor &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_Visitor::visit (ATOM_Sky &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_Visitor::visit (ATOM_BkImage &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_Visitor::visit (ATOM_Decal &node)
{
	visit ((ATOM_Node&)node);
}

void ATOM_Visitor::visit (ClientSimpleCharacter &node)
{
	visit ((ATOM_Geode&)node);
}

void ATOM_Visitor::visit (ATOM_CompositionNode &node)
{
	visit ((ATOM_VisualNode&)node);
}

void ATOM_Visitor::visit (ATOM_ShapeNode &node)
{
	visit ((ATOM_VisualNode&)node);
}

void ATOM_Visitor::visit (ATOM_RibbonTrail &node)
{
	visit ((ATOM_VisualNode&)node);
}

void ATOM_Visitor::visit (ATOM_WeaponTrail &node)
{
	visit ((ATOM_VisualNode&)node);
}

/*
void ATOM_Visitor::visit (SwordEffect &node)
{
	visit ((ATOM_Geode&)node);
}
*/
void ATOM_Visitor::reset (void)
{
  _cancelTraverse = false;
  _skipChildren = false;
  //_nodePath.resize (0);

  onResetVisitor ();
}

void ATOM_Visitor::traverse (ATOM_Node &node)
{
  reset ();
  //_rootNode = &node;

  traverse_R (node);
}

void ATOM_Visitor::traverse_R (ATOM_Node &node)
{
	//_nodePath.push_back (&node);
	onBeginVisitNodeTree (node);

	if (!_cancelTraverse)
	{
		// 保持一个引用,以免在下面的accept方法里node被删除导致后面的代码崩溃
		ATOM_AUTOREF(ATOM_Node) refHolder(&node);

		node.accept (*this);

		if (!_cancelTraverse && !_skipChildren)
		{
			for (unsigned i = 0; i < node.getNumChildren (); ++i)
			{
				traverse_R (*node.getChild (i));

				if (_cancelTraverse)
				{
					break;
				}
			}
		}
	}

	onEndVisitNodeTree (node);
	//_nodePath.pop_back ();

	if (!_cancelTraverse)
	{
		_skipChildren = false;
	}
}

void ATOM_Visitor::onBeginVisitNodeTree (ATOM_Node &node)
{
}

void ATOM_Visitor::onEndVisitNodeTree (ATOM_Node &node)
{
}

void ATOM_Visitor::onResetVisitor (void)
{
}

/*
unsigned ATOM_Visitor::getNumNodesVisited (void) const
{
  return _nodePath.size();
}

ATOM_Node *ATOM_Visitor::getVisitedNode (unsigned index) const
{
  return _nodePath[index].get();
}

ATOM_Node *ATOM_Visitor::getRootNode (void) const
{
  return _rootNode.get();
}
*/
