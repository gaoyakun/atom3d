#include "stdafx.h"
#include "terrainquadtree.h"
#include "terrainquadtreenode.h"
#include "renderqueue.h"
#include "terrain.h"
#include "heightfield.h"
#include "terrainrenderdatas.h"

ATOM_TerrainQuadtree::ATOM_TerrainQuadtree (ATOM_Terrain *terrain)
{
	ATOM_STACK_TRACE(ATOM_TerrainQuadtree::ATOM_TerrainQuadtree);

	_terrain = terrain;
	_scaleX = 1.f;
	_scaleZ = 1.f;
	_patchSize = 0;
	_rootSize = 0;
	_heightfield = 0;
	_rootNode = 0;
	_primitiveCount = 0;
	_primitiveType = ATOM_PRIMITIVE_TRIANGLE_STRIP;
}

ATOM_TerrainQuadtree::~ATOM_TerrainQuadtree (void)
{
	ATOM_STACK_TRACE(ATOM_TerrainQuadtree::~ATOM_TerrainQuadtree);

	ATOM_DELETE(_rootNode);
	_rootNode = 0;

	ATOM_DELETE(_heightfield);
	_heightfield = 0;

	_terrain = 0;
}

bool ATOM_TerrainQuadtree::build (unsigned patchSize, unsigned rootSize, float *elevations, float scaleX, float scaleZ, unsigned vertexCacheSize)
{
  ATOM_STACK_TRACE(ATOM_TerrainQuadtree::build);

  if (!ATOM_ispo2 (patchSize-1) || !ATOM_ispo2 (rootSize-1) || patchSize > rootSize || !elevations)
  {
    return false;
  }

  ATOM_RenderDevice *device = ATOM_GetRenderDevice ();
  if (!device)
  {
    return false;
  }

  _heightfield = ATOM_NEW(ATOM_HeightField);
  if (!_heightfield->init (rootSize, rootSize, 0.f, 0.f, scaleX, scaleZ, 1.f, elevations))
  {
	  ATOM_DELETE(_heightfield);
	  return false;
  }

  _patchSize = patchSize;
  _rootSize = rootSize;
  _scaleX = scaleX;
  _scaleZ = scaleZ;

  // Create base vertex buffer
  unsigned dimension = patchSize + 2; // with "skirts"

  _baseVertices = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD, ATOM_USAGE_STATIC, dimension * dimension, true);
  ATOM_ASSERT(_baseVertices);
  ATOM_Vector3f *vertices = (ATOM_Vector3f*)_baseVertices->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
  ATOM_ASSERT(vertices);

  // top skirt
  vertices[0].x = 0;
  vertices[0].y = 0;
  vertices[0].z = 0;
  for (unsigned i = 1; i < dimension-1; ++i)
  {
    vertices[i].x = i - 1;
    vertices[i].y = 0;
    vertices[i].z = 0;
  }
  vertices[dimension-1].x = dimension - 3;
  vertices[dimension-1].y = 0;
  vertices[dimension-1].z = 0;
  vertices += dimension;

  for (unsigned i = 1; i < dimension - 1; ++i, vertices += dimension)
  {
    // left skirt
    vertices[0].x = 0;
    vertices[0].y = 0;
    vertices[0].z = i - 1;

    // height
    for (unsigned j = 1; j < dimension - 1; ++j)
    {
      vertices[j].x = j - 1;
      vertices[j].y = 0;
      vertices[j].z = i - 1;
    }

    // right skirt
    vertices[dimension-1].x = dimension - 3;
    vertices[dimension-1].y = 0;
    vertices[dimension-1].z = i - 1;
  }

  // bottom skirt
  vertices[0].x = 0;
  vertices[0].y = 0;
  vertices[0].z = dimension - 3;
  for (unsigned i = 1; i < dimension-1; ++i)
  {
    vertices[i].x = i - 1;
    vertices[i].y = 0;
    vertices[i].z = dimension - 3;
  }
  vertices[dimension-1].x = dimension - 3;
  vertices[dimension-1].y = 0;
  vertices[dimension-1].z = dimension - 3;

  _baseVertices->unlock ();

  // Create base index buffer
  ATOM_VECTOR<unsigned short> indices;
  strip (indices, vertexCacheSize);
  _indices = device->allocIndexArray (ATOM_USAGE_STATIC, indices.size(), false, true);
  unsigned short *p = (unsigned short*)_indices->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
  ATOM_ASSERT(p);
  memcpy (p, &indices[0], indices.size() * sizeof(unsigned short));
  _indices->unlock ();

  _primitiveCount = indices.size() - 2;
  _primitiveType = ATOM_PRIMITIVE_TRIANGLE_STRIP;

  _rootNode = ATOM_NEW(ATOM_TerrainQuadtreeNode);

  bool ret = _rootNode->initialize (this, 0, ATOM_TerrainPatch::LeftTop, _baseVertices.get());

  return ret;
}

void ATOM_TerrainQuadtree::strip (ATOM_VECTOR<unsigned short> &indices, unsigned vertexCacheSize)
{
  ATOM_STACK_TRACE(ATOM_TerrainQuadtree::strip);

  unsigned dimension = _patchSize + 2;
  unsigned step = vertexCacheSize / 2 - 1;

  for (unsigned i = 0; i < dimension - 1; i += step)
  {
    unsigned start = i;
    unsigned end = (i + step > dimension - 1) ? dimension - 1 : i + step;
    for (unsigned j = 0; j < dimension - 1; ++j)
    {
      for (unsigned k = start; k <= end; ++k)
      {
        indices.push_back ((dimension - 1 - k) * dimension + j);
        indices.push_back ((dimension - 1 - k) * dimension + j + 1);
      }
      indices.push_back ((dimension - 1 - end) * dimension + j + 1);
      indices.push_back ((j == dimension - 2) ? (dimension - 1 - end) * dimension : (dimension - 1 - start) * dimension + j + 1);
    }
  }

  indices.resize (indices.size() - 2);
}

unsigned ATOM_TerrainQuadtree::getPatchSize (void) const
{
  return _patchSize;
}

unsigned ATOM_TerrainQuadtree::getRootSize (void) const
{
  return _rootSize;
}

const float *ATOM_TerrainQuadtree::getElevations (void) const
{
	return _heightfield ? _heightfield->getHeights() : 0;
}

float ATOM_TerrainQuadtree::getScaleX (void) const
{
  return _scaleX;
}

float ATOM_TerrainQuadtree::getScaleZ (void) const
{
  return _scaleZ;
}

ATOM_IndexArray *ATOM_TerrainQuadtree::getIndices (void) const
{
  return _indices.get();
}

unsigned ATOM_TerrainQuadtree::getPrimitiveCount (void) const
{
  return _primitiveCount;
}

int ATOM_TerrainQuadtree::getPrimitiveType (void) const
{
  return _primitiveType;
}

void ATOM_TerrainQuadtree::setupCamera (int viewportH, float tanHalfFovy, unsigned maxPixelError)
{
  if (_rootNode)
  {
    _rootNode->setupCamera (viewportH, tanHalfFovy, maxPixelError);
  }
}

void ATOM_TerrainQuadtree::getBoundingbox (ATOM_BBox &bbox) const
{
	if (_heightfield)
	{
		bbox = _heightfield->getBoundingbox ();
	}
	else
	{
		bbox.setMin (ATOM_Vector3f(0, 0, 0));
		bbox.setMax (ATOM_Vector3f(0, 0, 0));
	}
}

void ATOM_TerrainQuadtree::cull (ATOM_Camera *camera, const ATOM_Vector3f &viewPoint, const ATOM_Matrix4x4f &worldMatrix, ATOM_TerrainRenderDatas *rd, bool drawBasemap)
{
	ATOM_STACK_TRACE(ATOM_TerrainQuadtree::cull);

	if (_rootNode && _terrain)
	{
		ATOM_Matrix4x4f mvp = camera->getViewProjectionMatrix() >> worldMatrix;
		cull_r (camera, _rootNode, viewPoint, mvp, worldMatrix, true, rd, drawBasemap);
	}
}

void ATOM_TerrainQuadtree::cull_r (ATOM_Camera *camera, ATOM_TerrainQuadtreeNode *node, const ATOM_Vector3f &viewPoint, const ATOM_Matrix4x4f &mvp, const ATOM_Matrix4x4f &worldMatrix, bool cliptest, ATOM_TerrainRenderDatas *rd, bool drawBasemap)
{
	const ATOM_BBox &bbox = node->getBoundingbox ();

	if (cliptest)
	{
		//ATOM_Matrix4x4f mvp = camera->getViewProjectionMatrix () >> (node->getPatch ()->getPatchTransform ());
		ATOM_BBox::ClipState clipState = ATOM_Terrain::isEditorModeEnabled() 
			? bbox.getClipStateMask (mvp, ATOM_BBox::ClipLeft|ATOM_BBox::ClipRight|ATOM_BBox::ClipFar|ATOM_BBox::ClipNear) 
			: bbox.getClipState (mvp);
		if (clipState == ATOM_BBox::ClipOutside)
		{
			return;
		}
		else if (clipState == ATOM_BBox::ClipInside)
		{
			cliptest = false;
		}
	}

	ATOM_Vector3f trunkCenter = worldMatrix >> node->getBoundingbox ().getCenter ();
	float eyeDistanceSq = (viewPoint - trunkCenter).getSquaredLength ();
	float ld = node->getPatch()->getLodDistance ();
	float lodDistance = (ld >= 0.f) ? ATOM_sqr (ld) : FLT_MAX;

	int lodLevel = _terrain->getDetailLodLevel();
	if (eyeDistanceSq < lodDistance && node->getChild(0) && (lodLevel == 0 || node->getPatch()->getMipLevel() < _terrain->getDetailLodLevel()))
	{
		// TODO: sort front to back
		for (unsigned i = 0; i < 4; ++i)
		{
			cull_r (camera, node->getChild(i), viewPoint, mvp, worldMatrix, cliptest, rd, drawBasemap);
		}
	}
	else
	{
		// select this detail level
#if 1
		bool drawDetail = true;

		// wangjian added
		// 是否绘制细节纹理 由 RenderSettings决定
		drawDetail = ATOM_RenderSettings::detailedTerrainEnabled();
#else
		bool drawDetail =	!drawBasemap 
							|| (_terrain->getDrawDetail() && ATOM_RenderSettings::detailedTerrainEnabled() && (node->getPatch()->insideDetailRegion (viewPoint, _terrain->getDetailDistance())));
#endif
		rd->addPatch (node->getPatch(), drawDetail);
		//renderQueue->Add (ATOM_RenderQueue::ORDER_DEFAULT, node->getPatch(), node->getPatch()->getPatchTransform(), _terrain, material);
	}
}

bool ATOM_TerrainQuadtree::rayIntersect (const ATOM_Vector3f &origin, const ATOM_Vector3f &direction, float &t) const
{
	return _heightfield ? _heightfield->rayIntersect (origin, direction, t, true) : false;
}

ATOM_HeightField *ATOM_TerrainQuadtree::getHeightField (void) const
{
	return _heightfield;
}

ATOM_Terrain *ATOM_TerrainQuadtree::getTerrain (void) const
{
	return _terrain;
}


