#ifndef __ATOM3D_ENGINE_TERRAINQUADTREENODE_H
#define __ATOM3D_ENGINE_TERRAINQUADTREENODE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <ATOM_math.h>
#include "basedefs.h"
#include "terrainpatch.h"

class ATOM_TerrainPatch;

class ATOM_ENGINE_API ATOM_TerrainQuadtreeNode
{
public:
  ATOM_TerrainQuadtreeNode (void);
  ~ATOM_TerrainQuadtreeNode (void);

public:
  bool initialize (ATOM_TerrainQuadtree *quadtree, ATOM_TerrainQuadtreeNode *parent, ATOM_TerrainPatch::PatchPosition position, ATOM_VertexArray *baseVertices);
  void setupCamera (int viewportH, float tanHalfFovy, unsigned maxPixelError);
  const ATOM_BBox & getBoundingbox (void) const;
  ATOM_TerrainPatch *getPatch (void) const;
  ATOM_TerrainQuadtreeNode *getParent (void) const;
  ATOM_TerrainQuadtreeNode *getChild (int index) const;

private:
  ATOM_TerrainPatch *_patch;
  ATOM_TerrainQuadtreeNode *_parent;
  ATOM_TerrainQuadtreeNode *_children[4];
};

#endif // __ATOM3D_ENGINE_TERRAINQUADTREENODE_H
