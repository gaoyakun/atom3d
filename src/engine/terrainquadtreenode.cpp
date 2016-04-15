#include "stdafx.h"
#include "terrainquadtreenode.h"

ATOM_TerrainQuadtreeNode::ATOM_TerrainQuadtreeNode (void)
{
	ATOM_STACK_TRACE(ATOM_TerrainQuadtreeNode::ATOM_TerrainQuadtreeNode);

	_patch = 0;
	_parent = 0;
	_children[0] = 0;
	_children[1] = 0;
	_children[2] = 0;
	_children[3] = 0;
}

ATOM_TerrainQuadtreeNode::~ATOM_TerrainQuadtreeNode (void)
{
	ATOM_STACK_TRACE(ATOM_TerrainQuadtreeNode::~ATOM_TerrainQuadtreeNode);

	if (_patch) ATOM_DELETE(_patch);
	if (_children[0]) ATOM_DELETE(_children[0]);
	if (_children[1]) ATOM_DELETE(_children[1]);
	if (_children[2]) ATOM_DELETE(_children[2]);
	if (_children[3]) ATOM_DELETE(_children[3]);
}

bool ATOM_TerrainQuadtreeNode::initialize (ATOM_TerrainQuadtree *quadtree, ATOM_TerrainQuadtreeNode *parent, ATOM_TerrainPatch::PatchPosition position, ATOM_VertexArray *baseVertices)
{
  ATOM_STACK_TRACE(ATOM_TerrainQuadtreeNode::initialize);

  ATOM_ASSERT(!_parent);
  ATOM_ASSERT(!_patch);

  _parent = parent;
  _patch = ATOM_NEW(ATOM_TerrainPatch);

  if (!_patch->initialize (quadtree, _parent ? _parent->_patch : 0, position, baseVertices))
  {
    return false;
  }

  if (_patch->getStep () > 1)
  {
    for (unsigned i = 0; i < 4; ++i)
    {
      _children[i] = ATOM_NEW(ATOM_TerrainQuadtreeNode);
      if (!_children[i]->initialize (quadtree, this, (ATOM_TerrainPatch::PatchPosition)i, baseVertices))
      {
        return false;
      }
    }
  }

  return true;
}

void ATOM_TerrainQuadtreeNode::setupCamera (int viewportH, float tanHalfFovy, unsigned maxPixelError)
{
  if (_patch)
  {
    _patch->setupCamera (viewportH, tanHalfFovy, maxPixelError);
  }

  for (unsigned i = 0; i < 4; ++i)
  {
    if (_children[i])
    {
      _children[i]->setupCamera (viewportH, tanHalfFovy, maxPixelError);
    }
  }
}

const ATOM_BBox &ATOM_TerrainQuadtreeNode::getBoundingbox (void) const
{
  ATOM_ASSERT(_patch);

  return _patch->getBoundingbox ();
}

ATOM_TerrainPatch *ATOM_TerrainQuadtreeNode::getPatch (void) const
{
  return _patch;
}

ATOM_TerrainQuadtreeNode *ATOM_TerrainQuadtreeNode::getParent (void) const
{
  return _parent;
}

ATOM_TerrainQuadtreeNode *ATOM_TerrainQuadtreeNode::getChild (int index) const
{
  return _children[index];
}



