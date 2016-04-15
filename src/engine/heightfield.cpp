#include "stdafx.h"
#include "heightfield.h"

#if 1

HeightFieldBBoxTree::HeightFieldBBoxTree (int res_x, int res_y, const ATOM_Vector4f *vertices)
{
	_rootNode = 0;
	_heights = 0;
	
	create (res_x, res_y, vertices);
}

HeightFieldBBoxTree::~HeightFieldBBoxTree (void)
{
	ATOM_DELETE_ARRAY(_heights);

	for (int i = 0; i < _nodeCache.size(); ++i)
	{
		ATOM_DELETE_ARRAY(_nodeCache[i].nodeCache);
	}
	_nodeCache.clear();
}

HeightFieldBBoxTreeNode *HeightFieldBBoxTree::allocNode (void)
{
	if (_nodeCache.empty() || _nodeCache.back().nextNodeIndex==_nodeCache.back().cacheSize)
	{
		NodeCacheEntry entry;
		entry.cacheSize = 10000;
		entry.nodeCache = ATOM_NEW_ARRAY(HeightFieldBBoxTreeNode, entry.cacheSize);
		entry.nextNodeIndex = 0;
		_nodeCache.push_back (entry);
	}
	return &_nodeCache.back().nodeCache[_nodeCache.back().nextNodeIndex++];
}

float HeightFieldBBoxTree::getHeight (int x, int y) const
{
	return _heights[(_resY-1-y)*_resX+x];
}

float HeightFieldBBoxTree::getRealHeight (float x, float y) const
{
	x -= _rootNode->bbox.getMin().x;
	y -= _rootNode->bbox.getMin().z;
	float tileSizeX = (_rootNode->bbox.getMax().x - _rootNode->bbox.getMin().x) / (_resX - 1);
	float tileSizeY = (_rootNode->bbox.getMax().z - _rootNode->bbox.getMin().z) / (_resY - 1);
	float x_unscale = x / tileSizeX;
	float y_unscale = y / tileSizeY;
	int l = ATOM_ftol(x_unscale);
	int t = ATOM_ftol(y_unscale);
	int r = l + 1;
	int b = t + 1;

	if (l < 0) l = 0;
	if (t < 0) t = 0;
	if (r >= _resX) r = _resX-1;
	if (b >= _resY) b = _resY-1;

	if (l == r)
	{
		if (t == b)
		{
			return getHeight (l, t);
		}
		else
		{
			float ht = getHeight (l, t);
			float hb = getHeight (l, b);
			return ht + (hb - ht) * (y_unscale - t);
		}
	}
	else
	{
		float hlt = getHeight (l, t);
		float hrt = getHeight (r, t);
		float ht = hlt + (hrt - hlt) * (x_unscale - l);
		if (t == b)
		{
			return ht;
		}
		else
		{
			float hlb = getHeight (l, b);
			float hrb = getHeight (r, b);
			float hb = hlb + (hrb - hlb) * (x_unscale - l);
			return ht + (hb - ht) * (y_unscale - t);
		}
	}
}

static inline bool intersectRayTriangle (const ATOM_Vector3f &start, const ATOM_Vector3f &normal, const ATOM_Vector3f &v1, 
	const ATOM_Vector3f &v2, const ATOM_Vector3f &v3, bool cull, float *d)
{
	ATOM_Vector3f edge1 = v2 - v1;
	ATOM_Vector3f edge2 = v3 - v1;
	ATOM_Vector3f pvec = crossProduct (normal, edge2);
	float det = dotProduct (edge1, pvec);

	if (!cull)
	{
		if (ATOM_equal(det, 0.f))
		{
			return false;
		}

		float inv_det = 1.0f / det;
		ATOM_Vector3f tvec = start - v1;
		float u = inv_det * dotProduct (tvec, pvec);

		if (u < 0.f || u > 1.f)
		{
			return false;
		}

		ATOM_Vector3f qvec = crossProduct(tvec, edge1);
		float v = inv_det * dotProduct (normal, qvec);

		if (v < 0.f || u + v > 1.f)
		{
			return false;
		}

		if (d)
		{
			*d = dotProduct (edge2, qvec) * inv_det;
		}
		return true;
	}
	else
	{
		if (det < 0.f)
		{
			return false;
		}

		ATOM_Vector3f tvec = start - v1;
		float u = dotProduct (tvec, pvec);
		if (u < 0.f || u > det) 
		{
			return false;
		}

		ATOM_Vector3f qvec = crossProduct (tvec, edge1);
		float v = dotProduct (normal, qvec);
		if (v < 0.f || u + v > det)
		{
			return false;
		}

		if (d)
		{
			*d = dotProduct (edge2, qvec) / det;
		}

		return true;
	}
}

bool HeightFieldBBoxTree::pick (const ATOM_Ray &ray, float &d)
{
	if (!_rootNode)
	{
		return false;
	}

	float dist;

	HeightFieldBBoxTreeNode *node = rayIntersectionTestR (_rootNode, ray, &dist);
	if (node)
	{
		/*
		float minX = _rootNode->bbox.getMin().x;
		float minY = _rootNode->bbox.getMin().z;
		float stepX = (_rootNode->bbox.getMax().x - _rootNode->bbox.getMin().x) / (_resX-1);
		float stepY = (_rootNode->bbox.getMax().z - _rootNode->bbox.getMin().z) / (_resY-1);
		int x = node->v[0] % _resX;
		int y = node->v[0] / _resX;

		ATOM_Vector3f v00 (node->bbox.getMin().x, _heights[node->v[0]], node->bbox.getMax().z);
		ATOM_Vector3f v01 (node->bbox.getMax().x, _heights[node->v[1]], node->bbox.getMax().z);
		ATOM_Vector3f v11 (node->bbox.getMax().x, _heights[node->v[2]], node->bbox.getMin().z);
		ATOM_Vector3f v10 (node->bbox.getMin().x, _heights[node->v[3]], node->bbox.getMin().z);

		float dist = FLT_MAX;
		if (intersectRayTriangle(ray.getOrigin(), ray.getDirection(), v00, v01, v10, false, &d) && d>0.f)
		{
			dist = d;
		}

		if (intersectRayTriangle(ray.getOrigin(), ray.getDirection(), v10, v01, v11, false, &d) && d>0.f && d<dist)
		{
			dist = d;
		}
		*/

		d = dist;

		return true;
	}

	return false;
}

bool HeightFieldBBoxTree::create (int res_x, int res_y, const ATOM_Vector4f *vertices)
{
	_resX = res_x;
	_resY = res_y;
	_rootNode = allocNode();

	ATOM_DELETE_ARRAY(_heights);
	_heights = ATOM_NEW_ARRAY(float, res_x * res_y);
	for (unsigned i = 0; i < res_x * res_y; ++i)
	{
		_heights[i] = vertices[i].y;
	}

	createChildNode (_rootNode, 0, 0, res_x, res_y, vertices);

	return true;
}

HeightFieldBBoxTreeNode *HeightFieldBBoxTree::rayIntersectionTestR (HeightFieldBBoxTreeNode *node, const ATOM_Ray &ray, float *d) const
{
	HeightFieldBBoxTreeNode *result = 0;

	if (!ray.intersectionTestEx (node->bbox, *d))
	{
		return result;
	}

	result = node;

	if (node->left && node->right)
	{
		float dl, dr;
		HeightFieldBBoxTreeNode *nodeLeft = rayIntersectionTestR (node->left, ray, &dl);
		HeightFieldBBoxTreeNode *nodeRight = rayIntersectionTestR (node->right, ray, &dr);

		if (dl < 0.f)
			nodeLeft = 0;

		if (dr < 0.f)
			nodeRight = 0;

		if (nodeLeft && nodeRight)
		{
			result = dl < dr ? nodeLeft : nodeRight;
			*d = dl < dr ? dl : dr;
		}
		else
		{
			result = nodeLeft ? nodeLeft : nodeRight;
			*d = nodeLeft ? dl : dr;
		}
	}
	else
	{
		ATOM_Vector3f v00 (node->bbox.getMin().x, _heights[node->v[0]], node->bbox.getMax().z);
		ATOM_Vector3f v01 (node->bbox.getMax().x, _heights[node->v[1]], node->bbox.getMax().z);
		ATOM_Vector3f v11 (node->bbox.getMax().x, _heights[node->v[2]], node->bbox.getMin().z);
		ATOM_Vector3f v10 (node->bbox.getMin().x, _heights[node->v[3]], node->bbox.getMin().z);

		bool intersected = false;
		float dist1 = FLT_MAX, dist2 = FLT_MAX;
		if (intersectRayTriangle(ray.getOrigin(), ray.getDirection(), v00, v01, v10, false, &dist1) && dist1>0.f)
		{
			intersected = true;
		}

		if (intersectRayTriangle(ray.getOrigin(), ray.getDirection(), v10, v01, v11, false, &dist2) && dist2>0.f)
		{
			intersected = true;
		}

		if (!intersected)
		{
			result = 0;
		}
		else
		{
			*d = ATOM_min2(dist1, dist2);
		}
	}

	return result;
}

HeightFieldBBoxTreeNode *HeightFieldBBoxTree::getRootNode (void) const
{
	return _rootNode;
}

void HeightFieldBBoxTree::updateChildNode (HeightFieldBBoxTreeNode *node, int x, int y, float height)
{
	if (x < node->rc.point.x || x >= node->rc.point.x+node->rc.size.w || y < node->rc.point.y || y >= node->rc.point.y+node->rc.size.h)
	{
		return;
	}

	if (node->rc.size.w == 2 && node->rc.size.h == 2)
	{
		_heights[x+y*_resX] = height;

		ATOM_Vector3f vMin = node->bbox.getMin();
		ATOM_Vector3f vMax = node->bbox.getMax();
		float heightMax = -FLT_MAX;
		float heightMin = FLT_MAX;
		for (int i = 0; i < 4; ++i)
		{
			float h = _heights[node->v[i]];

			if (heightMax < h) 
			{
				heightMax = h;
			}

			if (heightMin > h) 
			{
				heightMin = h;
			}
		}

		vMin.y = heightMin;
		vMax.y = heightMax;
		node->bbox.setMin (vMin);
		node->bbox.setMax (vMax);
	}
	else
	{
		updateChildNode (node->left, x, y, height);
		updateChildNode (node->right, x, y, height);

		node->bbox.beginExtend ();
		node->bbox.extend (node->left->bbox.getMin());
		node->bbox.extend (node->left->bbox.getMax());
		node->bbox.extend (node->right->bbox.getMin());
		node->bbox.extend (node->right->bbox.getMax());
	}
}

bool HeightFieldBBoxTree::createChildNode (HeightFieldBBoxTreeNode *node, int x, int y, int w, int h, const ATOM_Vector4f *vertices)
{
	node->rc.point.x = x;
	node->rc.point.y = y;
	node->rc.size.w = w;
	node->rc.size.h = h;

	if (w == 2 && h == 2)
	{
		node->left = 0;
		node->right = 0;
		node->v[0] = x+y*_resX;
		node->v[1] = x+y*_resX+1;
		node->v[2] = x+(y+1)*_resX+1;
		node->v[3] = x+(y+1)*_resX;

		node->bbox.beginExtend ();
		node->bbox.extend (vertices[x+y*_resX].getVector3());
		node->bbox.extend (vertices[x+1+y*_resX].getVector3());
		node->bbox.extend (vertices[x+(y+1)*_resX].getVector3());
		node->bbox.extend (vertices[x+1+(y+1)*_resX].getVector3());
	}
	else
	{
		if (w >= h)
		{
			int w1 = (w+1) / 2;
			int w2 = w - w1 + 1;

			node->left = allocNode();
			createChildNode (node->left, x, y, w1, h, vertices);
		
			node->right = allocNode();
			createChildNode (node->right, x+w1-1, y, w2, h, vertices);
		}
		else
		{
			int h1 = (h+1) / 2;
			int h2 = h - h1 + 1;

			node->left = allocNode();
			createChildNode (node->left, x, y, w, h1, vertices);

			node->right = allocNode();
			createChildNode (node->right, x, y+h1-1, w, h2, vertices);
		}

		node->bbox.beginExtend ();
		node->bbox.extend (node->left->bbox.getMin());
		node->bbox.extend (node->left->bbox.getMax());
		node->bbox.extend (node->right->bbox.getMin());
		node->bbox.extend (node->right->bbox.getMax());
	}

	return true;
}

float *HeightFieldBBoxTree::getHeights (void) const
{
	return _heights;
}

void HeightFieldBBoxTree::updateHeightValue (int x, int z, float height)
{
	updateChildNode (_rootNode, x, _rootNode->rc.size.h - 1 - z, height);
}

//////////////////////////////////////////////////////////////////////////////////

ATOM_HeightField::ATOM_HeightField (void)
{
	m_v4Range.set(0.f, 0.f, 0.f, 0.f);
	m_bboxTree = NULL;
	m_scale.set (1.f, 1.f, 1.f);
	m_sizeX = 0;
	m_sizeZ = 0;
}

ATOM_HeightField::~ATOM_HeightField (void)
{
	clear ();
}

bool ATOM_HeightField::init (int sizeX, int sizeZ, const ATOM_Vector4f *vertices)
{
	ATOM_DELETE(m_bboxTree);
	m_bboxTree = ATOM_NEW(HeightFieldBBoxTree, sizeX, sizeZ, vertices);
	m_scale.set (1.f, 1.f, 1.f);
	m_sizeX = sizeX;
	m_sizeZ = sizeZ;
	m_v4Range.set (m_bboxTree->getRootNode()->bbox.getMin().x,
		m_bboxTree->getRootNode()->bbox.getMin().z,
		m_bboxTree->getRootNode()->bbox.getExtents().x * 2.f,
		m_bboxTree->getRootNode()->bbox.getExtents().z * 2.f);
	return true;
}

bool ATOM_HeightField::resetHeights (const float *heights)
{
	return init (m_sizeX, m_sizeZ, m_v4Range.x, m_v4Range.y, m_scale.x, m_scale.z, m_scale.y, heights);
}

bool ATOM_HeightField::init (int sizeX, int sizeZ, float offset_x, float offset_z, float spacing_x, float spacing_z, float vscale, const float *heights) 
{
	ATOM_Vector4f *v = ATOM_NEW_ARRAY(ATOM_Vector4f, sizeX * sizeZ);

	for (unsigned i = 0; i < sizeZ; ++i)
	{
		const float *h = heights + i * sizeX;

		ATOM_Vector4f *dst = v + (sizeZ - i - 1) * sizeX;

		for (unsigned j = 0; j < sizeX; ++j)
		{
			dst->x = offset_x + j * spacing_x;
			dst->y = h[j] * vscale;
			dst->z = offset_z + i * spacing_z;
			dst->w = 1.f;
			dst++;
		}
	}

	ATOM_DELETE(m_bboxTree);
	m_bboxTree = ATOM_NEW(HeightFieldBBoxTree, sizeX, sizeZ, v);
	ATOM_DELETE_ARRAY(v);

	m_v4Range.set (m_bboxTree->getRootNode()->bbox.getMin().x,
		m_bboxTree->getRootNode()->bbox.getMin().z,
		m_bboxTree->getRootNode()->bbox.getExtents().x * 2.f,
		m_bboxTree->getRootNode()->bbox.getExtents().z * 2.f);

	m_scale.set(spacing_x, vscale, spacing_z);
	m_sizeX = sizeX;
	m_sizeZ = sizeZ;

	return true;
}

void ATOM_HeightField::clear () 
{
	ATOM_DELETE(m_bboxTree);
	m_bboxTree = NULL;
	m_v4Range.set(0.f, 0.f, 0.f, 0.f);
	m_scale.set(1.f, 1.f, 1.f);
	m_sizeX = 0;
	m_sizeZ = 0;
}

bool ATOM_HeightField::rayIntersect (const ATOM_Vector3f &origin, const ATOM_Vector3f &direction, float &t, bool cull) const
{
	return m_bboxTree ? m_bboxTree->pick (ATOM_Ray(origin, direction), t) : false;
}

bool ATOM_HeightField::rayIntersectionTest (const ATOM_Vector3f &origin, const ATOM_Vector3f &direction, bool cull) const
{
	float t;
	return rayIntersect (origin, direction, t, false);
}

float ATOM_HeightField::getRealHeight (float x, float z) const
{
	return m_bboxTree ? m_bboxTree->getRealHeight (x, z) : 0.f;
}

void ATOM_HeightField::computeNormals (ATOM_Vector3f *normals) const
{
#define H(x,y) heights[(x)+(y)*m_sizeX]
#if 1
	float scaleX = m_scale.x;
	float scaleZ = m_scale.z;
	const float *heights = getHeights();
	for (unsigned y=0; y<m_sizeZ;++y)
		for (unsigned x=0; x<m_sizeX;++x)
		{
			float sx = H((x>0?x-1:x),y) - H((x<m_sizeX-1?x+1:x),y);
			if (x == 0 || x ==m_sizeX-1)
			{
				sx *= 2.f;
			}

			float sy = H(x,(y>0?y-1:y)) - H(x,(y<m_sizeZ-1?y+1:y));
			if (y == 0 || y ==m_sizeZ-1)
			{
				sy *= 2.f;
			}
			normals[x+(m_sizeZ-y-1)*m_sizeX].set(sx*scaleZ, 2.f*scaleX*scaleZ, -sy*scaleX);
			normals[x+(m_sizeZ-y-1)*m_sizeX].normalize();
		}
#else
	float scaleX = m_scale.x;
	float scaleZ = m_scale.z;
	const float *heights = getHeights();

	ATOM_Vector3f v[8] = {
		ATOM_Vector3f(-scaleX, 0, -scaleZ),
		ATOM_Vector3f(	   0, 0, -scaleZ),
		ATOM_Vector3f( scaleX, 0, -scaleZ),
		ATOM_Vector3f( scaleX, 0,       0),
		ATOM_Vector3f( scaleX, 0,  scaleZ),
		ATOM_Vector3f(	   0, 0,  scaleZ),
		ATOM_Vector3f(-scaleX, 0,  scaleZ),
		ATOM_Vector3f(-scaleX, 0,       0)
	};

	int vd[8][2] = {
		{-1, -1},
		{ 0, -1},
		{ 1, -1},
		{ 1,  0},
		{ 1,  1},
		{ 0,  1},
		{-1,  1},
		{-1,  0}
	};

	ATOM_Vector3f normal;
	int x, z, s, t;

	for (int i = 0; i < m_sizeZ; ++i)
		for (int j = 0; j < m_sizeX; ++j)
		{
			for (int n = 0; n < 8; ++n)
			{
				x = (j + vd[n][0] + m_sizeX) % m_sizeX;
				z = (i + vd[n][1] + m_sizeZ) % m_sizeZ;
				v[n].y = (heights[x + z * m_sizeX] - heights[j + i * m_sizeX]);
			}

			normal.x = 0;
			normal.y = 0;
			normal.z = 0;

			for (s = 0, t = 1; s < 8; ++s, ++t)
			{
				if (t >= 8)
				{
					t = 0;
				}

				ATOM_Vector3f n = crossProduct (v[s], v[t]);
				if (n.y < 0)
				{
					n = -n;
				}
				normal += n;
			}

			normal.normalize();
			normals[(m_sizeZ-1-i)*m_sizeX+j] = normal;
		}
#endif
}

float ATOM_HeightField::getSpacingX (void) const
{
	return m_scale.x;
}

float ATOM_HeightField::getSpacingZ (void) const
{
	return m_scale.z;
}

float ATOM_HeightField::getVerticalScale (void) const
{
	return m_scale.y;
}

int ATOM_HeightField::getSizeX (void) const
{
	return m_sizeX;
}

int ATOM_HeightField::getSizeZ (void) const
{
	return m_sizeZ;
}

float ATOM_HeightField::getOffsetX (void) const
{
	return m_v4Range.x;
}

float ATOM_HeightField::getOffsetZ (void) const
{
	return m_v4Range.y;
}

float ATOM_HeightField::getHeight (unsigned x, unsigned z) const
{
	return m_bboxTree ? m_bboxTree->getHeight (x, z) : 0.f;
}

ATOM_BBox ATOM_HeightField::getBoundingbox (void) const
{
	return m_bboxTree ? m_bboxTree->getRootNode()->bbox : ATOM_BBox();
}

float *ATOM_HeightField::getHeights (void) const
{
	return m_bboxTree ? m_bboxTree->getHeights() : 0;
}

void ATOM_HeightField::updateHeightValue (int x, int z, float height)
{
	if (m_bboxTree)
	{
		m_bboxTree->updateHeightValue (x, z, height);
	}
}

HeightFieldBBoxTree *ATOM_HeightField::getBBoxTree (void) const
{
	return m_bboxTree;
}

#else

ATOM_HeightField::ATOM_HeightField (void) 
{
	ATOM_STACK_TRACE(ATOM_HeightField::ATOM_HeightField);

	_blocksize = 0;
	_size = 0;
	_spacing_x = 0;
	_spacing_z = 0;
	_vertical_scale = 0;
	_offset_x = 0;
	_offset_z = 0;
	_num_levels = 0;
	_minmax = 0;
	_heights = 0;
}

ATOM_HeightField::~ATOM_HeightField ()
{
	ATOM_STACK_TRACE(ATOM_HeightField::~ATOM_HeightField);

	clear ();
}

bool ATOM_HeightField::init (int size, float offset_x, float offset_z, float spacing_x, float spacing_z, float vscale, const float *heights, int blocksize) 
{
	ATOM_STACK_TRACE(ATOM_HeightField::init);

	if (!size || !heights)
	{
		return false;
	}

	clear ();

	_size = size;
	_spacing_x = spacing_x;
	_spacing_z = spacing_z;
	_vertical_scale = vscale;
	_offset_x = offset_x;
	_offset_z = offset_z;
	_heights = ATOM_NEW_ARRAY(float, _size * _size);

	bool ret = true;
	memcpy (_heights, heights, _size * _size * sizeof(float));
	if (!computeMinMax (blocksize))
	{
		clear ();
		return false;
	}

	return ret;
}

void ATOM_HeightField::clear () 
{
	ATOM_STACK_TRACE(ATOM_HeightField::clear);

	_blocksize = 0;
	_size = 0;
	_spacing_x = 0;
	_spacing_z = 0;
	_vertical_scale = 0;
	_num_levels = 0;
	ATOM_DELETE_ARRAY(_minmax);
	_minmax = 0;
	ATOM_DELETE_ARRAY(_heights);
	_heights = 0;
}

bool ATOM_HeightField::computeMinMax (int blocksize) 
{
	ATOM_STACK_TRACE(ATOM_HeightField::computeMinMax);

	if (blocksize <= 0)
	{
		return false;
	}

	_blocksize = blocksize;
	_num_levels = 0;
	int size = _size;
	while (_blocksize <= size)
	{
		++_num_levels;
		size >>= 1;
	}

	if (_num_levels < 2)
	{
		return false;
	}

    int count = nodeCount(_num_levels);
    _minmax = ATOM_NEW_ARRAY(bound, count);
    ATOM_ASSERT (_minmax);

    int tilesize = (_size-1) / _blocksize;

    for (int i = 0; i < tilesize; ++i)
    {
	    for (int j = 0; j < tilesize; ++j)
	    {
		    int index = nodeIndex(_num_levels-1, i, j);
		    _minmax[index].y_max = -FLT_MAX;
		    _minmax[index].y_min = FLT_MAX;

		    for (int mx = i * _blocksize; mx <= (i+1) * _blocksize; ++mx)
			    for (int mz = j * _blocksize; mz <= (j+1) * _blocksize; ++mz)
			    {
				    float h = getHeight (mx, mz);

				    if (h > _minmax[index].y_max)
				    {
					    _minmax[index].y_max = h;
				    }

				    if (h < _minmax[index].y_min)
				    {
					    _minmax[index].y_min = h;
				    }
			    }
	    }
    }

    for (int k = _num_levels-2; k >= 0; --k)
    {
	    tilesize >>= 1;

	    for (int i = 0; i < tilesize; ++i)
	    {
		    for (int j = 0; j < tilesize; ++j)
		    {
			    int index = nodeIndex(k, i, j);
			    _minmax[index].y_max = -FLT_MAX;
			    _minmax[index].y_min = FLT_MAX;
			    int index0 = nodeIndex(k+1, i*2, j*2);
			    int index1 = nodeIndex(k+1, i*2+1, j*2);
			    int index2 = nodeIndex(k+1, i*2+1, j*2+1);
			    int index3 = nodeIndex(k+1, i*2, j*2+1);

			    if (_minmax[index0].y_max > _minmax[index].y_max)
				    _minmax[index].y_max = _minmax[index0].y_max;
			    if (_minmax[index1].y_max > _minmax[index].y_max)
				    _minmax[index].y_max = _minmax[index1].y_max;
			    if (_minmax[index2].y_max > _minmax[index].y_max)
				    _minmax[index].y_max = _minmax[index2].y_max;
			    if (_minmax[index3].y_max > _minmax[index].y_max)
				    _minmax[index].y_max = _minmax[index3].y_max;

			    if (_minmax[index0].y_min < _minmax[index].y_min)
				    _minmax[index].y_min = _minmax[index0].y_min;
			    if (_minmax[index1].y_min < _minmax[index].y_min)
				    _minmax[index].y_min = _minmax[index1].y_min;
			    if (_minmax[index2].y_min < _minmax[index].y_min)
				    _minmax[index].y_min = _minmax[index2].y_min;
			    if (_minmax[index3].y_min < _minmax[index].y_min)
				    _minmax[index].y_min = _minmax[index3].y_min;
		    }
	    }
    }

	return true;
}

bool ATOM_HeightField::lssIntersect (const ATOM_Vector3f &origin, const ATOM_Vector3f &direction, float radius, float &t) const
{
	ATOM_STACK_TRACE(ATOM_HeightField::lssIntersect);

	ATOM_Vector3f dir(direction);
	dir.normalize ();
	ATOM_Ray ray(origin, dir);
	t = FLT_MAX;
	lock ();
	bool b = lssIntersect_R (ray, radius, 0, 0, 0, t);
	unlock ();

	return b;
}

bool ATOM_HeightField::rayIntersect (const ATOM_Vector3f &origin, const ATOM_Vector3f &direction, float &t, bool cull) const 
{
	ATOM_STACK_TRACE(ATOM_HeightField::rayIntersect);

	ATOM_Vector3f dir(direction);
	dir.normalize();
	ATOM_Ray ray(origin, dir);
	t = FLT_MAX;
	lock ();
	bool b = rayIntersect_R (ray, 0, 0, 0, t, cull, false);
	unlock ();

	return b;
}

bool ATOM_HeightField::rayIntersectionTest (const ATOM_Vector3f &origin, const ATOM_Vector3f &direction, bool cull) const
{
	ATOM_STACK_TRACE(ATOM_HeightField::rayIntersectionTest);

	ATOM_Vector3f dir(direction);
	dir.normalize();
	ATOM_Ray ray(origin, dir);
	float t = FLT_MAX;
	lock ();
	bool b = rayIntersect_R (ray, 0, 0, 0, t, cull, true);
	unlock ();
	return b;
}

static inline bool intersectRayTriangle (const ATOM_Vector3f &start, const ATOM_Vector3f &normal, const ATOM_Vector3f &v1, 
									                         const ATOM_Vector3f &v2, const ATOM_Vector3f &v3, bool cull, float *d)
{
    ATOM_Vector3f edge1 = v2 - v1;
    ATOM_Vector3f edge2 = v3 - v1;
    ATOM_Vector3f pvec = crossProduct (normal, edge2);
    float det = dotProduct (edge1, pvec);

    if (!cull)
    {
	    if (ATOM_equal(det, 0.f))
	    {
		    return false;
	    }

	    float inv_det = 1.0f / det;
	    ATOM_Vector3f tvec = start - v1;
	    float u = inv_det * dotProduct (tvec, pvec);

	    if (u < 0.f || u > 1.f)
	    {
		    return false;
	    }

	    ATOM_Vector3f qvec = crossProduct(tvec, edge1);
	    float v = inv_det * dotProduct (normal, qvec);

	    if (v < 0.f || u + v > 1.f)
	    {
		    return false;
	    }

	    if (d)
	    {
		    *d = dotProduct (edge2, qvec) * inv_det;
	    }
	    return true;
    }
    else
    {
	    if (det < 0.f)
	    {
		    return false;
	    }

	    ATOM_Vector3f tvec = start - v1;
	    float u = dotProduct (tvec, pvec);
	    if (u < 0.f || u > det) 
	    {
		    return false;
	    }

	    ATOM_Vector3f qvec = crossProduct (tvec, edge1);
	    float v = dotProduct (normal, qvec);
	    if (v < 0.f || u + v > det)
	    {
		    return false;
	    }

	    if (d)
	    {
		    *d = dotProduct (edge2, qvec) / det;
	    }

	    return true;
    }
}

bool ATOM_HeightField::lssIntersect_R (const ATOM_Ray &ray, float radius, int level, int col, int row, float &dist) const
{
	ATOM_BBox bbox = getBoundingbox (level, col, row);
	ATOM_Vector3f maxPoint = bbox.getMax() + ATOM_Vector3f(radius, radius, radius);
	ATOM_Vector3f minPoint = bbox.getMin() - ATOM_Vector3f(radius, radius, radius);
	bbox.setMin (minPoint);
	bbox.setMax (maxPoint);

	float d;
	if (!ray.intersectionTestEx (bbox, d))
	{
		return false;
	}

	if (level == _num_levels - 1)
	{
		int levelFactor = _num_levels - 1 - level;
		int posx = (col << levelFactor) * _blocksize;
		int posz = (row << levelFactor) * _blocksize;

	    // This is the leaf level, do triangle intersection test.

		ATOM_LSSCollisionContext context;
		context.radius = radius;
		context.velocityR3 = ray.getDirection() * 99999.f;
		context.positionR3 = ray.getOrigin ();
		context.velocity = context.velocityR3 / radius;
		context.normalizedVelocity = ray.getDirection ();
		context.basePoint = ray.getOrigin() / radius;
		context.foundCollision = false;

	    for (int i = 0; i < _blocksize; ++i)
	    {
		    for (int j = 0; j < _blocksize; ++j)
		    {
			    float h00 = getHeight (posx + i, posz + j);
			    float h10 = getHeight (posx + i + 1, posz + j);
			    float h01 = getHeight (posx + i, posz + j + 1);
			    float h11 = getHeight (posx + i + 1, posz + j + 1);
			    ATOM_Vector3f v00 ((posx + i) * _spacing_x + _offset_x, h00, (posz + j) * _spacing_z + _offset_z);
			    ATOM_Vector3f v10 ((posx + i + 1) * _spacing_x + _offset_x, h10, (posz + j) * _spacing_z + _offset_z);
			    ATOM_Vector3f v01 ((posx + i) * _spacing_x + _offset_x, h01, (posz + j + 1) * _spacing_z + _offset_z);
			    ATOM_Vector3f v11 ((posx + i + 1) * _spacing_x + _offset_x, h11, (posz + j + 1) * _spacing_z + _offset_z);
				v00 /= radius;
				v10 /= radius;
				v01 /= radius;
				v11 /= radius;

				LSS_Triangle_IntersectionTest (&context, v00, v01, v11);
				LSS_Triangle_IntersectionTest (&context, v00, v11, v10);
		    }
	    }

		if (context.foundCollision)
		{
			float d = context.nearestDistance * radius;
			if (d < dist)
			{
				dist = d;
				return true;
			}
		}

		return false;
	}
	else
	{
		bool intersected = false;
		if (lssIntersect_R(ray, radius, level+1, col*2, row*2, dist))
		{
			intersected = true;
		}

		if (lssIntersect_R(ray, radius, level+1, col*2, row*2+1, dist))
		{
			intersected = true;
		}

		if (lssIntersect_R(ray, radius, level+1, col*2+1, row*2+1, dist))
		{
			intersected = true;
		}

		if (lssIntersect_R(ray, radius, level+1, col*2+1, row*2, dist))
		{
			intersected = true;
		}

		return intersected;
	}
}

bool ATOM_HeightField::rayIntersect_R (const ATOM_Ray &ray, int level, int col, int row, float &dist, bool cull, bool testOnly) const
{
	float d;
	if (!ray.intersectionTestEx (getBoundingbox (level, col, row), d))
	{
		return false;
	}

    if (level == _num_levels - 1)
    {
		int levelFactor = _num_levels - 1 - level;
		int posx = (col << levelFactor) * _blocksize;
		int posz = (row << levelFactor) * _blocksize;

	    // This is the leaf level, do triangle intersection test.
	    bool intersected = false;

	    for (int i = 0; i < _blocksize; ++i)
	    {
		    for (int j = 0; j < _blocksize; ++j)
		    {
			    float h00 = getHeight (posx + i, posz + j);
			    float h10 = getHeight (posx + i + 1, posz + j);
			    float h01 = getHeight (posx + i, posz + j + 1);
			    float h11 = getHeight (posx + i + 1, posz + j + 1);
			    float d;
			    ATOM_Vector3f v00 ((posx + i) * _spacing_x + _offset_x, h00, (posz + j) * _spacing_z + _offset_z);
			    ATOM_Vector3f v10 ((posx + i + 1) * _spacing_x + _offset_x, h10, (posz + j) * _spacing_z + _offset_z);
			    ATOM_Vector3f v01 ((posx + i) * _spacing_x + _offset_x, h01, (posz + j + 1) * _spacing_z + _offset_z);
			    ATOM_Vector3f v11 ((posx + i + 1) * _spacing_x + _offset_x, h11, (posz + j + 1) * _spacing_z + _offset_z);

				if (intersectRayTriangle(ray.getOrigin(), ray.getDirection(), v00, v01, v11, cull, &d) && d>0.f)
			    {
					if (testOnly)
					{
						dist = d;
						return true;
					}

					intersected = true;
					if (d >= 0.f && d < dist)
					{
					    dist = d;
					}
			    }

				if (intersectRayTriangle(ray.getOrigin(), ray.getDirection(), v00, v11, v10, cull, &d) && d>0.f)
			    {
					if (testOnly)
					{
						dist = d;
						return true;
					}

				    intersected = true;
					if (d >= 0.f && d < dist)
					{
						dist = d;
					}
			    }
		    }
	    }

	    return intersected;
    }
    else
    {
		bool intersected = false;
		if (rayIntersect_R(ray, level+1, col*2, row*2, dist, cull, testOnly))
		{
			if (testOnly)
			{
				return true;
			}
			intersected = true;
		}

		if (rayIntersect_R(ray, level+1, col*2, row*2+1, dist, cull, testOnly))
		{
			if (testOnly)
			{
				return true;
			}
			intersected = true;
		}

		if (rayIntersect_R(ray, level+1, col*2+1, row*2+1, dist, cull, testOnly))
		{
			if (testOnly)
			{
				return true;
			}
			intersected = true;
		}

		if (rayIntersect_R(ray, level+1, col*2+1, row*2, dist, cull, testOnly))
		{
			if (testOnly)
			{
				return true;
			}
			intersected = true;
		}

		return intersected;
	}
}

float ATOM_HeightField::getRealHeight (float x, float z) const 
{
	x -= _offset_x;
	z -= _offset_z;

    float x_unscale = x / _spacing_x;
    float z_unscale = z / _spacing_z;
    int l = ATOM_ftol(x_unscale);
    int t = ATOM_ftol(z_unscale);
    int r = l + 1;
    int b = t + 1;

    if (l < 0)
	{
	    l = 0;
	}

    if (t < 0)
	{
	    t = 0;
	}

    if (r >= _size)
	{
	    r = _size - 1;
	}

    if (b >= _size)
	{
	    b = _size - 1;
	}

    if (l == r)
	{
	    x_unscale = l;
	}

    if (b == t)
	{
	    z_unscale = t;
	}

    float factor_z = z_unscale - t;
    float factor_x = x_unscale - l;
    float lt_h = getHeight (l, t);
	float rb_h = getHeight (r, b);
    if (factor_x < factor_z)
    {
		float lb_h = getHeight (l, b);
	    float hl1 = lt_h + factor_z * (lb_h - lt_h);
		float hl2 = lt_h + factor_z * (rb_h - lt_h);
		return hl1 + (factor_x / factor_z) * (hl2 - hl1);
    }
    else if (factor_x > factor_z)
    {
		float rt_h = getHeight (r, t);
		float hl1 = lt_h + factor_x * (rt_h - lt_h);
		float hl2 = lt_h + factor_x * (rb_h - lt_h);
		return hl1 + (factor_z / factor_x) * (hl2 - hl1);
    }
	else
	{
		return lt_h + factor_x * (rb_h - lt_h);
	}
}

void ATOM_HeightField::computeNormals (ATOM_Vector3f *normals) const
{
	ATOM_STACK_TRACE(ATOM_HeightField::computeNormals);

	lock ();

	float scaleX = _spacing_x;
	float scaleZ = _spacing_z;

	ATOM_Vector3f v[8] = {
		ATOM_Vector3f(-scaleX, 0, -scaleZ),
		ATOM_Vector3f(	   0, 0, -scaleZ),
		ATOM_Vector3f( scaleX, 0, -scaleZ),
		ATOM_Vector3f( scaleX, 0,       0),
		ATOM_Vector3f( scaleX, 0,  scaleZ),
		ATOM_Vector3f(	   0, 0,  scaleZ),
		ATOM_Vector3f(-scaleX, 0,  scaleZ),
		ATOM_Vector3f(-scaleX, 0,       0)
	};

	int vd[8][2] = {
		{-1, -1},
		{ 0, -1},
		{ 1, -1},
		{ 1,  0},
		{ 1,  1},
		{ 0,  1},
		{-1,  1},
		{-1,  0}
	};

	ATOM_Vector3f normal;
	int x, z, s, t;

	for (int i = 0; i < _size; ++i)
		for (int j = 0; j < _size; ++j)
		{
			for (int n = 0; n < 8; ++n)
			{
				x = (j + vd[n][0] + _size) % _size;
				z = (i + vd[n][1] + _size) % _size;
				v[n].y = (_heights[x + z * _size] - _heights[j + i * _size]) * _vertical_scale;
			}

			normal.x = 0;
			normal.y = 0;
			normal.z = 0;

			for (s = 0, t = 1; s < 8; ++s, ++t)
			{
				if (t >= 8)
				{
				  t = 0;
				}

				ATOM_Vector3f n = crossProduct (v[s], v[t]);
				if (n.y < 0)
				{
				  n = -n;
				}
				normal += n;
			}

			normal.normalize();
			normals[i*_size+j] = normal;
		}

	unlock ();
}

void ATOM_HeightField::lock (void) const
{
}

void ATOM_HeightField::unlock (void) const
{
}

#endif
