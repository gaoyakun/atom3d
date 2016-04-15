#ifndef __ATOM3D_ENGINE_SPATIALCELL_H
#define __ATOM3D_ENGINE_SPATIALCELL_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"

class ATOM_Node;
class ATOM_SpatialCell
  {
  public:
    struct NodeInfo
    {
      ATOM_SpatialCell *cell;
      ATOM_Node *node;
      unsigned cacheIndex;
      unsigned lastUpdateStamp;
      NodeInfo *prev;
      NodeInfo *next;
    };

    ATOM_SpatialCell (void);
    virtual ~ATOM_SpatialCell (void);

  private:
    ATOM_SpatialCell (const ATOM_SpatialCell &);
    ATOM_SpatialCell & operator = (const ATOM_SpatialCell &);

  public:
    NodeInfo *getHead (void) const;

  public:
    void detachNode (ATOM_Node *leaf);
    void attachNode (ATOM_Node *leaf, unsigned updateStamp);
    void detachAllNodes (void);

  private:
    NodeInfo *_M_nodes;
  };

inline ATOM_SpatialCell::NodeInfo *ATOM_SpatialCell::getHead (void) const
{
  return _M_nodes;
}

#endif // __ATOM3D_ENGINE_SPATIALCELL_H

