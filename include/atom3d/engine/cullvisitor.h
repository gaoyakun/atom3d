#ifndef __ATOM3D_ENGINE_CULLVISITOR_H
#define __ATOM3D_ENGINE_CULLVISITOR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"
#include "visitor.h"

class ATOM_NodeOctree;
class ATOM_Camera;
class ATOM_Octree;
class ATOM_TerrainRenderDatas;
class ATOM_BBoxRenderDatas;
class ATOM_DefaultMaterialObject;
class ATOM_Scene;
class ATOM_Atmosphere;
class ATOM_RenderQueue;

class ATOM_ENGINE_API ATOM_CullVisitor: public ATOM_Visitor
{
public:
  enum
  {
    CULL_NO_CLIP_TEST = (1<<0),
    //CULL_SHADOW_PASS = (1<<1),
	CULL_DRAW_BOUNDINGBOX = (1<<2),
	//CULL_REFLECTION_PASS = (1<<3),
	//CULL_GBUFFER = (1<<4)
  };

public:
  ATOM_CullVisitor (void);
  virtual ~ATOM_CullVisitor (void);

public:
  virtual void traverse (ATOM_Node &node);
  virtual void visit (ATOM_Node &node);
  virtual void visit (ATOM_NodeOctree &node);
  virtual void visit (ATOM_Terrain &node);
  virtual void visit (ATOM_VisualNode &node);
  virtual void visit (ATOM_Geode &node);
  virtual void visit (ATOM_ParticleSystem &node);
  virtual void visit (ATOM_Hud &node);
  virtual void visit (ATOM_GuiHud &node);
  virtual void visit (ATOM_Water &node);
  virtual void visit (ATOM_LightNode &node);
  virtual void visit (ATOM_Atmosphere &node);
  virtual void visit (ATOM_Atmosphere2 &node);
  virtual void visit (ATOM_Sky &node);
  virtual void visit (ATOM_BkImage &node);
  virtual void visit (ATOM_Actor &node);
  virtual void visit (ATOM_Decal &node);
  virtual void visit (ATOM_CompositionNode &node);
  virtual void visit (ClientSimpleCharacter &node);

protected:
  virtual void onBeginVisitNodeTree (ATOM_Node &node);
  virtual void onEndVisitNodeTree (ATOM_Node &node);
  virtual void onResetVisitor (void);

public:
  void addDrawable (ATOM_Drawable *drawable, const ATOM_Matrix4x4f &worldmatrix, ATOM_Material *material, int layer = -1, bool projectable = false);
  void setRenderQueue (unsigned i, ATOM_RenderQueue *renderQueue);
  ATOM_RenderQueue *getRenderQueue (unsigned i) const;
  void setNumRenderQueues (unsigned count);
  unsigned getNumRenderQueues (void);
  void setCamera (ATOM_Camera *camera);
  ATOM_Camera *getCamera (void) const;
  const ATOM_Matrix4x4f & getViewProjectionMatrix (void) const;
  void setFlags (unsigned flags);
  unsigned getFlags (void) const;
  void setTerrainOccluder (ATOM_Terrain *occluder);
  ATOM_Terrain *getTerrainOccluder (void) const;
  void setUpdateVisibleStamp (bool b);
  bool getUpdateVisibleStamp (void) const;
  void setDistanceOverrideValue (float value);
  float getDistanceOverrideValue (void) const;

protected:
  void CullOctree_r (ATOM_OctreeNode *node, bool clipped, bool nonclip, const ATOM_Matrix4x4f &viewMatrix, const ATOM_Matrix4x4f &projectionMatrix, const ATOM_Frustum3d &frustum);
  void queueBoundingboxRD (ATOM_Node &node, const ATOM_Matrix4x4f &worldMatrix);
  bool occludedByTerrain (ATOM_Terrain *terrain, ATOM_Node *node, float minSize) const;
  bool testOcclusion (ATOM_Node *node, float minSize) const;
  bool frustumTest (ATOM_Node *node, const ATOM_BBox &bboxW);
  bool frustumTestMask (ATOM_Node *node, const ATOM_BBox &bboxW, unsigned mask);

protected:
  ATOM_VECTOR<ATOM_RenderQueue *> _renderQueues;
  ATOM_Matrix4x4f _vpMatrix;
  ATOM_Camera *_camera;
  ATOM_RenderDevice *_device;
  unsigned _flags;
  unsigned _frameStamp;
  bool _fogEnabled;
  bool _updateVisibleStamp;
  float _contribute;
  float _distanceOverrideValue;
  ATOM_Frustum3d _frustum;

  ATOM_VECTOR<ATOM_BBoxRenderDatas*> _bboxRDs;
  ATOM_VECTOR<ATOM_BBoxRenderDatas*> _bboxRDsFree;
  ATOM_Terrain *_terrainOccluder;
  ATOM_Matrix4x4f _invTerrainWorldMatrix;
  bool _isTerrainOccluderWorldMatrixIdentity;

  //--- wangjian added ---//
  bool _enableLoading;
  //----------------------//
};

#endif // __ATOM3D_ENGINE_CULLVISITOR_H
