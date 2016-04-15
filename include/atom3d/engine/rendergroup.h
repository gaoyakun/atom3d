#ifndef __ATOM3D_ENGINE_RENDERGROUP_H
#define __ATOM3D_ENGINE_RENDERGROUP_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_utils.h"
#include "../ATOM_math.h"
#include "../ATOM_kernel.h"
#include "../ATOM_render.h"

#include "node.h"
#include "materiallib.h"

class ATOM_BaseRenderDatas;
class ATOM_Camera;

class ATOM_RenderGroup: public ATOM_ReferenceObj
{
public:
  struct Unit
  {
    ATOM_MaterialEffect *effect;
    ATOM_Node *node;
    float distanceToEyeSq;
    ATOM_Matrix4x4f worldMatrix;
    ATOM_BaseRenderDatas *renderDatas;
    unsigned userData;
  };

  enum SortMode
  {
    SM_NONE,
    SM_MATERIAL,
    SM_FRONT_TO_BACK,
    SM_BACK_TO_FRONT
  };

public:
  ATOM_RenderGroup (void);
  ~ATOM_RenderGroup (void);

public:
  void add (ATOM_Node *node, ATOM_BaseRenderDatas *renderDatas, const ATOM_Matrix4x4f &worldMatrix, ATOM_MaterialEffect *effect, unsigned userData);
  int getNumUnits (void) const;
  void clear (void);
  void setSortMode (SortMode mode);
  void render (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_MaterialEffect *forceEffect);

private:
  void sortByMaterial (void);
  void sortByDistance (void);

protected:
  ATOM_VECTOR<Unit> _units;
  SortMode _sortMode;
  bool _updateDistance;
  bool _needSort;
  float _lastViewPosition[3];
  ATOM_RadixSort _radixSort;
};

#endif // __ATOM3D_ENGINE_RENDERGROUP_H
