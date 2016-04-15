#include "stdafx.h"

// inline member functions
ATOM_SpatialCell::ATOM_SpatialCell (void) 
{
  _M_nodes = 0;
}

ATOM_SpatialCell::~ATOM_SpatialCell (void)
{
  while (_M_nodes)
  {
    NodeInfo *p = _M_nodes->next;
    ATOM_DELETE(_M_nodes);
    _M_nodes = p;
  }
}

void ATOM_SpatialCell::detachNode (ATOM_Node *node) 
{
  if (node)
  {
    NodeInfo *info = node->getCellNodeInfo ();
    ATOM_ASSERT(info);

    if (info->prev)
    {
      ATOM_ASSERT(info != _M_nodes);
      info->prev->next = info->next;
      if (info->next)
      {
        info->next->prev = info->prev;
      }
    }
    else
    {
      ATOM_ASSERT(info == _M_nodes);
      _M_nodes = info->next;

      if (_M_nodes)
      {
        _M_nodes->prev = 0;
      }
    }

    ATOM_DELETE(info);
    node->setCellNodeInfo (0);
  }
}

void ATOM_SpatialCell::attachNode (ATOM_Node *node, unsigned updateStamp) 
{
  if (node)
  {
    ATOM_SpatialCell::NodeInfo *nodeInfo = node->getCellNodeInfo ();
    ATOM_ASSERT(!nodeInfo || nodeInfo->node == node);

    if (nodeInfo && nodeInfo->cell == this)
    {
      nodeInfo->lastUpdateStamp = updateStamp;
    }
    else
    {
      if (nodeInfo)
      {
        nodeInfo->cell->detachNode (node);
      }

      nodeInfo = ATOM_NEW(NodeInfo);
      nodeInfo->prev = 0;
      nodeInfo->next = _M_nodes;
      if (_M_nodes)
      {
        _M_nodes->prev = nodeInfo;
      }
      _M_nodes = nodeInfo;

      nodeInfo->cell = this;
      nodeInfo->node = node;
      nodeInfo->lastUpdateStamp = updateStamp;
      node->setCellNodeInfo (nodeInfo);
    }
  }
}

void ATOM_SpatialCell::detachAllNodes () 
{
  while (_M_nodes)
  {
    NodeInfo *p = _M_nodes->next;
    detachNode (_M_nodes->node);
    _M_nodes = p;
  }
}

