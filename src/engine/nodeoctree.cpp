#include "stdafx.h"
#include "nodeoctree.h"
#include "visitor.h"

#if 1
    const float ATOM_NodeOctree::OCTREE_SIZE = 16384.f;
    const float ATOM_NodeOctree::OCTREE_LEAF_SIZE = 128.f;

    ATOM_NodeOctree::ATOM_NodeOctree (void)
    {
      _octree = ATOM_NEW(ATOM_Octree);
      _octree->initialize (OCTREE_SIZE, OCTREE_LEAF_SIZE);
    }

    ATOM_NodeOctree::~ATOM_NodeOctree (void)
    {
      ATOM_DELETE(_octree);
    }

    void ATOM_NodeOctree::accept (ATOM_Visitor &visitor)
    {
      visitor.visit (*this);
    }

    ATOM_Octree *ATOM_NodeOctree::getOctree (void) const
    {
      return _octree;
    }

	ATOM_AUTOREF(ATOM_Node) ATOM_NodeOctree::clone (void) const
	{
		return 0;
	}

#else

    ATOM_NodeOctree::~ATOM_NodeOctree (void)
    {
      reset ();
    }

    void ATOM_NodeOctree::attachNodeTree_r (ATOM_Node *node, const Matrix44f &worldmatrix)
    {
      if (!node->getStatic ())
      {
        ATOM_OctreeNode *octnode = m_octree->getChunk(0)->getOrCreateNode(0);
        octnode->attachNode (node, worldmatrix, Aabbf());
        m_nodes.push_back (node);
        return;
      }

      if (!strcmp(node->getClassName(), "Npolygonsoup") || 
          !strcmp(node->getClassName(), "Nkeyframe") || 
          !strcmp(node->getClassName(), "Character"))
      {
        Aabbf aabb = node->getBoundingbox();
        aabb.transform (worldmatrix);
        ATOM_OctreeNode *octnode = m_octree->locateNodeChain (worldmatrix(0,3)-m_offset.x(), worldmatrix(1,3)-m_offset.y(), worldmatrix(2,3)-m_offset.z(), aabb.getHalfWidth().Length());
        if (!octnode)
          octnode = m_octree->getChunk(0)->getOrCreateNode (0);
        octnode->attachNode (node, worldmatrix, aabb);
        m_nodes.push_back (node);
      }

      if (node->getNumChildren() > 0)
      {
        Matrix44f matWorld = worldmatrix;
        MATRIX_MUL_4X4(matWorld, node->getO2T(), matWorld);
        for(unsigned i = 0; i < node->getNumChildren(); ++i)
	      {
          attachNodeTree_r (node->getChild(i), matWorld);
	      }
        node->clearChildren ();
      }
      else
      {
        ATOM_OctreeNode *octnode = m_octree->getChunk(0)->getOrCreateNode(0);
        Aabbf bbox = node->getBoundingbox();
        bbox.transform (worldmatrix);
        octnode->attachNode (node, worldmatrix, bbox);
        m_nodes.push_back (node);
      }
    }

    void ATOM_NodeOctree::attachNodeTree (ATOM_Node *rootnode)
    {
      //if (rootnode)
      //{
      //  reset ();

      //  if (rootnode)
      //  {
      //    const Aabbf &bbox = rootnode->getBoundingbox ();
      //    float f = bbox.getHalfWidthX ();
      //    if (f < bbox.getHalfWidthY())
      //      f = bbox.getHalfWidthY();
      //    if (f < bbox.getHalfWidthZ())
      //      f = bbox.getHalfWidthZ();
      //    f *= 4.f;

      //    if (f < 2048.f)
      //      f = 2048.f;

      //    m_octree = ATOM_NEW(ATOM_Octree);
      //    m_octree->initialize (f, 8.f);
      //    //m_offset = bbox.getCenter();
      //    m_offset.Set(0,0,0);
      //    attachNodeTree_r (rootnode, rootnode->getWorldMatrix());
      //  }
      //}
    }

    void ATOM_NodeOctree::reset (void)
    {
      ATOM_DELETE(m_octree);
      m_octree = 0;
      m_nodes.clear ();
    }

    void ATOM_NodeOctree::update(unsigned nTickNow, float fElapsed)
    {
      for (unsigned i = 0; i < m_nodes.size(); ++i)
      {
        if (m_nodes[i])
          m_nodes[i]->topmostUpdate (nTickNow, fElapsed);
      }
    }

	  void ATOM_NodeOctree::docull(ATOM_RenderDevice *pDevice, CollectorCull &Collector)
    {
      if (m_octree && _visible)
      {
        ATOM_Camera *oldCameraObjectSpace = Collector.getCameraObjectSpace();
        Matrix44f oldWorldMatrix;
        MATRIX_ASSIGN_4X4(Collector.worldmatrix (), oldWorldMatrix);
        unsigned flags = Collector.getCullFlags ();

        cull_r (m_octree->getChunk(0)->getNode (0), pDevice, Collector, false);

        Collector.setCullFlags (flags);
        MATRIX_ASSIGN_4X4(oldWorldMatrix, Collector.worldmatrix());
        Collector.setCameraObjectSpace ((oldCameraObjectSpace == Collector.getCameraWorldSpace()) ? 0 : oldCameraObjectSpace);
      }
    }

    void ATOM_NodeOctree::pick(CollectorPick &Collector)
    {
    }

    void ATOM_NodeOctree::buildBoundingbox () const
    {
      ATOM_OctreeNode *n = m_octree ? m_octree->getChunk(0)->getOrCreateNode(0) : 0;
      if (n)
      {
        _boundingBox.setCenter ((n->getMinPointLoosed() + n->getMaxPointLoosed()) * 0.5f);
        _boundingBox.setHalfWidth ((n->getMaxPointLoosed() - n->getMinPointLoosed()) * 0.5f);
      }
      else
      {
        _boundingBox.setCenter (ATOM_Vector3f(0,0,0));
        _boundingBox.setHalfWidth (ATOM_Vector3f(0,0,0));
      }
    }

    void ATOM_NodeOctree::cull_r (ATOM_OctreeNode *octnode, ATOM_RenderDevice *device, CollectorCull &Collector, bool insideFrustum)
    {
      if (octnode)
      {
        ATOM_Camera *cameraObject = Collector.getCameraObjectSpace() ;

        bool inside = insideFrustum;
        if (!inside)
        {
          ATOM_Vector3f box_min = octnode->getMinPointLoosed ();
          ATOM_Vector3f box_max = octnode->getMaxPointLoosed ();
          int result = intersect_test_AABB_Frustum3d ((box_min + box_max) * 0.5f, (box_max - box_min) * 0.5f, cameraObject->getFrustum());
          if (result == IRESULT_OUTSIDE)
            return;

          if (result == IRESULT_INSIDE)
          {
            inside = true;
            Collector.setCullFlags (Collector.getCullFlags() | CollectorCull::CULL_IN_FRUSTUM);
          }
          else
          {
            Collector.setCullFlags (Collector.getCullFlags() & ~CollectorCull::CULL_IN_FRUSTUM);
          }
        }
        else
        {
          Collector.setCullFlags (Collector.getCullFlags() | CollectorCull::CULL_IN_FRUSTUM);
        }

        if (!octnode->empty ())
        {
          ATOM_Camera cameraObjectSpace (*cameraObject);
          Collector.setCameraObjectSpace (&cameraObjectSpace);

          Matrix44f mView, mWorld;
          MATRIX_ASSIGN_4X4(Collector.worldmatrix(), mWorld);

          for (ATOM_SpatialCell::node_iterator it = octnode->nodeBegin(); it != octnode->nodeEnd(); ++it)
          {
            if (!inside)
            {
              if (!it->bbox.isZero())
              {
                int result = intersect_test_AABB_Frustum3d (it->bbox.getCenter(), it->bbox.getHalfWidth(), cameraObject->getFrustum());
                if (result == IRESULT_OUTSIDE)
                  continue;
              }

              Collector.setCullFlags (Collector.getCullFlags() | CollectorCull::CULL_IN_FRUSTUM);
            }

            MATRIX_MUL_4X4(it->inv_worldmatrix, cameraObject->getViewMatrix(), mView);
            cameraObjectSpace.setViewMatrix (mView.Elements());
            MATRIX_MUL_4X4(mWorld, it->worldmatrix, Collector.worldmatrix());
            it->node->topmostCull (device, Collector);

            if (!inside)
            {
              Collector.setCullFlags (Collector.getCullFlags() & ~CollectorCull::CULL_IN_FRUSTUM);
            }
          }

          MATRIX_ASSIGN_4X4(mWorld, Collector.worldmatrix());

          Collector.setCameraObjectSpace (cameraObject);
        }

        for (int i = ATOM_OctreeNode::PLACEMENTBEGIN; i != ATOM_OctreeNode::PLACEMENTEND; ++i)
        {
          ATOM_OctreeNode *child = octnode->getChild (i);

          if (child)
          {
            cull_r (child, device, Collector, inside);
          }
        }
      }
    }

    void ATOM_NodeOctree::accept (ATOM_Visitor &visitor)
    {
      visitor.visit (*this);
    }

#endif
