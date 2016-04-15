#include "stdafx.h"

// ATOM_OctreeNode

ATOM_OctreeNode *ATOM_OctreeNode::getChild (Placement placement) const 
{
  ATOM_ASSERT (_M_chunk);
  if (_M_chunk->getNext())
  {
    return _M_chunk->getNext()->getNode (_M_chunk->getChildIndex(_M_position, placement));
  }
  return 0;
}

ATOM_OctreeNode *ATOM_OctreeNode::getOrCreateChild (Placement placement) const 
{
  ATOM_ASSERT (_M_chunk);
  if (_M_chunk->getNext())
  {
    return _M_chunk->getNext()->getOrCreateNode (_M_chunk->getChildIndex(_M_position, placement));
  }
  return 0;
}

ATOM_OctreeNode *ATOM_OctreeNode::getParent (void) const 
{
  ATOM_ASSERT (_M_chunk);
  if (_M_chunk->getPrev())
  {
    return _M_chunk->getPrev()->getNode (_M_chunk->getParentIndex(_M_position));
  }
  return 0;
}

ATOM_OctreeNode *ATOM_OctreeNode::getOrCreateParent (void) const 
{
  ATOM_ASSERT (_M_chunk);
  if (_M_chunk->getPrev())
  {
    return _M_chunk->getPrev()->getOrCreateNode (_M_chunk->getParentIndex(_M_position));
  }
  return 0;
}

void ATOM_OctreeNode::createChildren (void) const 
{
  getOrCreateChild (ATOM_OctreeNode::PXPYPZ);
  getOrCreateChild (ATOM_OctreeNode::PXPYNZ);
  getOrCreateChild (ATOM_OctreeNode::PXNYPZ);
  getOrCreateChild (ATOM_OctreeNode::PXNYNZ);
  getOrCreateChild (ATOM_OctreeNode::NXPYPZ);
  getOrCreateChild (ATOM_OctreeNode::NXPYNZ);
  getOrCreateChild (ATOM_OctreeNode::NXNYPZ);
  getOrCreateChild (ATOM_OctreeNode::NXNYNZ);
}

bool ATOM_OctreeNode::tidy (void) 
{
  _M_reference = 8;
  for (int i = PLACEMENTBEGIN; i != PLACEMENTEND; ++i)
  {
    ATOM_OctreeNode *node = getChild ((Placement)i);
    if (!node || node->tidy())
    {
      --_M_reference;
    }
  }
  if (!getHead() && !_M_reference)
  {
    _M_chunk->freeNode (_M_position);
    return true;
  }
  return false;
}

// inline member functions
ATOM_OctreeNode::ATOM_OctreeNode (void) 
{
  _M_chunk = 0;
  _M_position = 0;
  _M_reference = 0;
  _M_userdata = 0;
}

void ATOM_OctreeNode::setChunk (ATOM_OctreeNodeChunk *chunk) 
{
  ATOM_ASSERT (chunk);
  _M_chunk = chunk;
}

ATOM_OctreeNodeChunk *ATOM_OctreeNode::getChunk (void) const 
{
  return _M_chunk;
}

void ATOM_OctreeNode::setPosition (NodeIndex position) 
{
  _M_position = position;
}

ATOM_OctreeNode::NodeIndex ATOM_OctreeNode::getPosition (void) const 
{
  return _M_position;
}

ATOM_Vector3f ATOM_OctreeNode::getMinPoint (void) const 
{
  ATOM_ASSERT (_M_chunk);
  unsigned d = _M_chunk->getDimension ();
  float nodesize = _M_chunk->getNodeSize ();
  float half_worldsize = _M_chunk->getWorldSize () * 0.5f;
  NodeIndex px = _M_position % d;
  NodeIndex py = (_M_position / d) % d;
  NodeIndex pz = _M_position / d / d;
  return ATOM_Vector3f (px * nodesize, py * nodesize, pz * nodesize)
         - ATOM_Vector3f (half_worldsize, half_worldsize, half_worldsize);
}

ATOM_Vector3f ATOM_OctreeNode::getMaxPoint (void) const 
{
  ATOM_ASSERT (_M_chunk);
  unsigned d = _M_chunk->getDimension ();
  float nodesize = _M_chunk->getNodeSize ();
  float half_worldsize = _M_chunk->getWorldSize () * 0.5f;
  NodeIndex px = (_M_position % d) + 1;
  NodeIndex py = ((_M_position / d) % d) + 1;
  NodeIndex pz = (_M_position / d / d) + 1;
  return ATOM_Vector3f (px * nodesize, py * nodesize, pz * nodesize)
         - ATOM_Vector3f (half_worldsize, half_worldsize, half_worldsize);
}

ATOM_Vector3f ATOM_OctreeNode::getMinPointLoosed (void) const 
{
  ATOM_ASSERT (_M_chunk);
  unsigned d = _M_chunk->getDimension ();
  float half_worldsize = _M_chunk->getWorldSize () * 0.5f;
  float nodesize = _M_chunk->getNodeSize ();
  float half_loosedsize = _M_chunk->getNodeSizeLoosed () * 0.5f;
  NodeIndex px = _M_position % d;
  NodeIndex py = (_M_position / d) % d;
  NodeIndex pz = _M_position / d / d;
  return ATOM_Vector3f (px * nodesize, py * nodesize, pz * nodesize)
         - ATOM_Vector3f (half_loosedsize, half_loosedsize, half_loosedsize)
         - ATOM_Vector3f (half_worldsize, half_worldsize, half_worldsize);
}

ATOM_Vector3f ATOM_OctreeNode::getMaxPointLoosed (void) const 
{
  ATOM_ASSERT (_M_chunk);
  unsigned d = _M_chunk->getDimension ();
  float half_worldsize = _M_chunk->getWorldSize () * 0.5f;
  float nodesize = _M_chunk->getNodeSize ();
  float half_loosedsize = _M_chunk->getNodeSizeLoosed () * 0.5f;
  NodeIndex px = (_M_position % d) + 1;
  NodeIndex py = ((_M_position / d) % d) + 1;
  NodeIndex pz = (_M_position / d / d) + 1;
  return ATOM_Vector3f (px * nodesize, py * nodesize, pz * nodesize)
         + ATOM_Vector3f (half_loosedsize, half_loosedsize, half_loosedsize)
         - ATOM_Vector3f (half_worldsize, half_worldsize, half_worldsize);
}

unsigned ATOM_OctreeNode::getReference (void) const 
{
  return _M_reference;
}

// ATOM_OctreeNodeChunk

ATOM_OctreeNodeChunk *ATOM_OctreeNodeChunk::getNext (void) const 
{
  return _M_next;
}

ATOM_OctreeNodeChunk *ATOM_OctreeNodeChunk::getPrev (void) const 
{
  return _M_prev;
}

float ATOM_OctreeNodeChunk::getNodeSize (void) const 
{
  return _M_node_size;
}

float ATOM_OctreeNodeChunk::getNodeSizeLoosed (void) const 
{
  return _M_loose_size;
}

float ATOM_OctreeNodeChunk::getWorldSize (void) const 
{
  return _M_octree->getRootSize();
}

unsigned ATOM_OctreeNodeChunk::getDimension (void) const 
{
  return _M_dimension;
}

unsigned ATOM_OctreeNodeChunk::getLevel (void) const 
{
  return _M_level;
}

ATOM_Octree *ATOM_OctreeNodeChunk::getOctree (void) const 
{
  return _M_octree;
}

bool ATOM_OctreeNodeChunk::empty (void) const 
{
  return _M_node_map.empty ();
}

void ATOM_OctreeNodeChunk::setLevel (unsigned level) 
{
  _M_level = level;
}

void ATOM_OctreeNodeChunk::setDimension (unsigned dimension) 
{
  _M_dimension = dimension;
}

void ATOM_OctreeNodeChunk::setNodeSize (float size) 
{
  _M_node_size = size;
}

void ATOM_OctreeNodeChunk::setNodeSizeLoosed (float size) 
{
  _M_loose_size = size;
}

void ATOM_OctreeNodeChunk::setPrev (ATOM_OctreeNodeChunk *chunk) 
{
  _M_prev = chunk;
}

void ATOM_OctreeNodeChunk::setNext (ATOM_OctreeNodeChunk *chunk) 
{
  _M_next = chunk;
}

ATOM_OctreeNodeChunk::ATOM_OctreeNodeChunk (ATOM_Octree *octree) 
{
  _M_level = 0;
  _M_dimension = 0;
  _M_node_size = 0;
  _M_loose_size = 0;
  _M_next = 0;
  _M_prev = 0;
  _M_octree = octree;
}

ATOM_OctreeNodeChunk::~ATOM_OctreeNodeChunk (void) 
{
  clearNodes ();
}

// ATOM_OctreeNodeChunk 

ATOM_OctreeNode *ATOM_OctreeNodeChunk::getNode (ATOM_OctreeNode::NodeIndex index) const 
{
  node_const_iterator it = _M_node_map.find (index);
  return (it == _M_node_map.end ()) ? 0 : it->second;
}

ATOM_OctreeNode *ATOM_OctreeNodeChunk::getOrCreateNode (ATOM_OctreeNode::NodeIndex index) 
{
  node_iterator it = _M_node_map.find (index);
  if (it != _M_node_map.end ())
  {
    return it->second;
  }
  else
  {
    ATOM_OctreeNode *node = ATOM_NEW(ATOM_OctreeNode);
    node->setChunk (this);
    node->setPosition (index);
    _M_node_map[index] = node;
    return node;
  }
}

ATOM_OctreeNode *ATOM_OctreeNodeChunk::getOrCreateNodeChain (ATOM_OctreeNode::NodeIndex index) 
{
  ATOM_OctreeNode *node = getOrCreateNode (index);
  if (getPrev ())
  {
    getPrev()->getOrCreateNodeChain (getParentIndex (index));
  }
  return node;
}

void ATOM_OctreeNodeChunk::freeNode (ATOM_OctreeNode *node) 
{
  if (node)
  {
    ATOM_ASSERT (node->getChunk () == this);
    freeNode (node->getPosition());
  }
}

void ATOM_OctreeNodeChunk::freeNode (ATOM_OctreeNode::NodeIndex index) 
{
  node_iterator it = _M_node_map.find (index);
  if (it != _M_node_map.end ())
  {
    ATOM_OctreeNode *node = it->second;
    node->detachAllNodes ();
    ATOM_DELETE(it->second);
    _M_node_map.erase (it);
  }
}

void ATOM_OctreeNodeChunk::clearNodes (void) 
{
  for (node_iterator it = _M_node_map.begin(); it != _M_node_map.end(); ++it)
  {
    it->second->detachAllNodes ();
    ATOM_DELETE(it->second);
  }
  _M_node_map.clear ();
}

// ATOM_Octree

void ATOM_Octree::initialize (float rootsize, float leafsize) 
{
  ATOM_ASSERT (rootsize >= leafsize && leafsize > 0);
  finalize ();

  _M_root_size = rootsize;
  _M_leaf_size = leafsize;

  unsigned n = 1;
  for (; rootsize >= leafsize * 2.f; leafsize *= 2.f, ++n)
    ;

  _M_chunks.resize (n);
  for (unsigned i = 0; i < n; ++i, rootsize *= 0.5f)
  {
    _M_chunks[i] = ATOM_NEW(ATOM_OctreeNodeChunk, this);
    _M_chunks[i]->setLevel (i);
    _M_chunks[i]->setNodeSize (rootsize);
    _M_chunks[i]->setNodeSizeLoosed (rootsize * 1.5f);
    _M_chunks[i]->setDimension (1 << i);
    if (i > 0) 
    {
      _M_chunks[i-1]->setNext (_M_chunks[i]);
      _M_chunks[i]->setPrev (_M_chunks[i-1]);
    }
  }
  _M_chunks.back ()->setNext (0);
}

void ATOM_Octree::finalize (void) 
{
  for (unsigned i = 0; i < _M_chunks.size(); ++i)
  {
    ATOM_DELETE(_M_chunks[i]);
  }

  _M_chunks.clear ();
  _M_root_size = 0.f;
  _M_leaf_size = 0.f;
}

ATOM_OctreeNode *ATOM_Octree::locateNodeChain2 (ATOM_OctreeNode *candidate, const ATOM_Vector3f &center, float radius)
{
  unsigned level = _M_chunks.size() - 1;
  while (level && _M_chunks[level]->getNodeSize() < 4 * radius)
    --level;
  unsigned dimension = _M_chunks[level]->getDimension();
  float inv_node_size = 1.f / float(_M_chunks[level]->getNodeSize ());
  ATOM_OctreeNode::NodeIndex px = ATOM_ftol((center.x + _M_root_size * 0.5f) * inv_node_size);
  ATOM_OctreeNode::NodeIndex py = ATOM_ftol((center.y + _M_root_size * 0.5f) * inv_node_size);
  ATOM_OctreeNode::NodeIndex pz = ATOM_ftol((center.z + _M_root_size * 0.5f) * inv_node_size);
  if (px >= dimension || py >= dimension || pz >= dimension)
  {
	  level = 0;
	  px = 0;
	  py = 0;
	  pz = 0;
  }
  ATOM_OctreeNode::NodeIndex index = px + py * dimension + pz * dimension * dimension;

  if (candidate && candidate->getChunk()->getLevel() == level && candidate->getPosition() == index)
  {
	  return candidate;
  }

  return _M_chunks[level]->getOrCreateNodeChain (index);
}

ATOM_OctreeNode *ATOM_Octree::locateNodeChain (const ATOM_Vector3f &center, float radius) 
{
  unsigned level = _M_chunks.size() - 1;
  while (level && _M_chunks[level]->getNodeSize() < 4 * radius)
    --level;
  unsigned dimension = _M_chunks[level]->getDimension();
  float inv_node_size = 1.f / float(_M_chunks[level]->getNodeSize ());
  ATOM_OctreeNode::NodeIndex px = ATOM_ftol((center.x + _M_root_size * 0.5f) * inv_node_size);
  ATOM_OctreeNode::NodeIndex py = ATOM_ftol((center.y + _M_root_size * 0.5f) * inv_node_size);
  ATOM_OctreeNode::NodeIndex pz = ATOM_ftol((center.z + _M_root_size * 0.5f) * inv_node_size);
  if (px >= dimension || py >= dimension || pz >= dimension)
  {
	  level = 0;
	  px = 0;
	  py = 0;
	  pz = 0;
  }
  ATOM_OctreeNode::NodeIndex index = px + py * dimension + pz * dimension * dimension;
  return _M_chunks[level]->getOrCreateNodeChain (index);
}

ATOM_Octree::ATOM_Octree (void)
{
	_M_root_node = 0;
}

ATOM_Octree::~ATOM_Octree (void) 
{
  finalize ();
}

float ATOM_Octree::getRootSize (void) const 
{
  return _M_root_size;
}

float ATOM_Octree::getLeafSize (void) const 
{
  return _M_leaf_size;
}

unsigned ATOM_Octree::getNumChunks (void) const 
{
  return _M_chunks.size();
}

ATOM_OctreeNodeChunk *ATOM_Octree::getChunk (unsigned level) const 
{
  return _M_chunks[level];
}

ATOM_OctreeNode *ATOM_Octree::getRootNode (void)
{
	if (!_M_root_node)
	{
		_M_root_node = _M_chunks[0]->getOrCreateNode (0);
	}
	return _M_root_node;
}

ATOM_OctreeNode::NodeIndex ATOM_OctreeNodeChunk::getParentIndex (ATOM_OctreeNode::NodeIndex index) const 
{
  ATOM_OctreeNode::NodeIndex px = (index % _M_dimension) / 2;
  ATOM_OctreeNode::NodeIndex py = ((index / _M_dimension) % _M_dimension) / 2;
  ATOM_OctreeNode::NodeIndex pz = (index / _M_dimension / _M_dimension) / 2;
  unsigned d = _M_dimension / 2;
  return px + py * d + pz * d * d;
}

ATOM_OctreeNode::NodeIndex ATOM_OctreeNodeChunk::getChildIndex (ATOM_OctreeNode::NodeIndex index, ATOM_OctreeNode::Placement placement) const 
{
  ATOM_OctreeNode::NodeIndex px = 2 * (index % _M_dimension);
  ATOM_OctreeNode::NodeIndex py = 2 * ((index / _M_dimension) % _M_dimension);
  ATOM_OctreeNode::NodeIndex pz = 2 * (index / _M_dimension / _M_dimension);

  switch (placement)
  {
  case ATOM_OctreeNode::PXPYPZ:
    ++px;
    ++py;
    ++pz;
    break;
  case ATOM_OctreeNode::PXPYNZ:
    ++px;
    ++py;
    break;
  case ATOM_OctreeNode::PXNYPZ:
    ++px;
    ++pz;
    break;
  case ATOM_OctreeNode::PXNYNZ:
    ++px;
    break;
  case ATOM_OctreeNode::NXPYPZ:
    ++py;
    ++pz;
    break;
  case ATOM_OctreeNode::NXPYNZ:
    ++py;
    break;
  case ATOM_OctreeNode::NXNYPZ:
    ++pz;
    break;
  case ATOM_OctreeNode::NXNYNZ:
    break;
  default:
	ATOM_LOGGER::error("ATOM_OctreeNodeChunk::getChildIndex(): Got invalid index\n");
	return 0;
  }
  unsigned dimension2 = 2 * _M_dimension;
  return pz * dimension2 * dimension2 + py * dimension2 + px;
}

