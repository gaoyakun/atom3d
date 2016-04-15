#include "StdAfx.h"
#include "heightgen.h"
#include "gridnode.h"
#include "lightgeom.h"
#include "blockeditor.h"
#include "plugin.h"
#include "plugin_scene.h"

class SceneSizeCalculator: public ATOM_Visitor
{
public:
	SceneSizeCalculator (void): boundingbox(ATOM_Vector3f(FLT_MAX, FLT_MAX, FLT_MAX), ATOM_Vector3f(-FLT_MAX, -FLT_MAX, -FLT_MAX)) {}

	virtual void visit (ATOM_Terrain &node)
	{
		const ATOM_BBox &bbox = node.getWorldBoundingbox ();
		boundingbox.extend (bbox.getMin ());
		boundingbox.extend (bbox.getMax ());
	}

	virtual void visit (ATOM_Geode &node)
	{
		const ATOM_BBox &bbox = node.getWorldBoundingbox ();
		boundingbox.extend (bbox.getMin ());
		boundingbox.extend (bbox.getMax ());
	}

	virtual void visit (ATOM_ShapeNode &node)
	{
		const ATOM_BBox &bbox = node.getWorldBoundingbox ();
		boundingbox.extend (bbox.getMin ());
		boundingbox.extend (bbox.getMax ());
	}

	ATOM_BBox boundingbox;
};

#if 0

//////////////////////////////////////////////////////////////////////////////////

HeightFieldBBoxTree::HeightFieldBBoxTree (int res_x, int res_y, const ATOM_Vector4f *vertices)
{
	_rootNode = 0;
	_heights = 0;
	
	create (res_x, res_y, vertices);
}

HeightFieldBBoxTree::~HeightFieldBBoxTree (void)
{
	ATOM_DELETE_ARRAY(_heights);
	deleteNode (_rootNode);
}

void HeightFieldBBoxTree::deleteNode (HeightFieldBBoxTreeNode *node)
{
	if (node)
	{
		deleteNode (node->left);
		deleteNode (node->right);
		ATOM_DELETE(node);
	}
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

bool HeightFieldBBoxTree::pick (const ATOM_Ray &ray, float &distance)
{
	if (!_rootNode)
	{
		return false;
	}

	float d;
	HeightFieldBBoxTreeNode *node = rayIntersectionTestR (_rootNode, ray, &d);
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

		distance = d;

		return true;
	}

	return false;
}

bool HeightFieldBBoxTree::create (int res_x, int res_y, const ATOM_Vector4f *vertices)
{
	_resX = res_x;
	_resY = res_y;
	_rootNode = ATOM_NEW(HeightFieldBBoxTreeNode);

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

			node->left = ATOM_NEW(HeightFieldBBoxTreeNode);
			createChildNode (node->left, x, y, w1, h, vertices);
		
			node->right = ATOM_NEW(HeightFieldBBoxTreeNode);
			createChildNode (node->right, x+w1-1, y, w2, h, vertices);
		}
		else
		{
			int h1 = (h+1) / 2;
			int h2 = h - h1 + 1;

			node->left = ATOM_NEW(HeightFieldBBoxTreeNode);
			createChildNode (node->left, x, y, w, h1, vertices);

			node->right = ATOM_NEW(HeightFieldBBoxTreeNode);
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

void HeightFieldBBoxTree::updateHeightValue (int x, int z, float height)
{
	updateChildNode (_rootNode, x, _rootNode->rc.size.h - 1 - z, height);
}

float *HeightFieldBBoxTree::getHeights (void) const
{
	return _heights;
}

#endif

//////////////////////////////////////////////////////////////////////////////////

HeightGrid::HeightGrid (ATOM_Scene *scene, DlgEditBlocks *dlgEditBlocks, PluginScene *plugin)
{
	_scene = scene;
	_dlgEditBlocks = dlgEditBlocks;
	_plugin = plugin;
	_size.w = 0;
	_size.h = 0;
#if 1
	_heightField = 0;
#else
	_bboxTree = 0;
#endif
	_cellSize = 0.f;

	_heightGridMaterial = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/editor/materials/heightgrid.mat");
	_heightGrid = ATOM_HARDREF(ATOM_ShapeNode)();
	_heightGrid->setO2T (ATOM_Matrix4x4f::getIdentityMatrix());
	_heightGrid->setType (ATOM_ShapeNode::GRID);
	_heightGrid->skipClipTest (true);
	_heightGrid->setPickable (ATOM_Node::NONPICKABLE);
	_heightGrid->setPrivate (1);
	_heightGrid->setMaterial (_heightGridMaterial.get());

	_blockGridMaterial = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/editor/materials/blockgrid.mat");
	_blockGrid = ATOM_HARDREF(ATOM_ShapeNode)();
	_blockGrid->setO2T (ATOM_Matrix4x4f::getIdentityMatrix());
	_blockGrid->setType (ATOM_ShapeNode::PLANE);
	_blockGrid->skipClipTest (true);
	_blockGrid->setPickable (ATOM_Node::NONPICKABLE);
	_blockGrid->setPrivate (1);
	_blockGrid->setMaterial (_blockGridMaterial.get());

	setGridColor (ATOM_Vector4f(1,1,1,1));
}

HeightGrid::~HeightGrid (void)
{
#if 1
	ATOM_DELETE(_heightField);
#else
	ATOM_DELETE(_bboxTree);
#endif
}

void HeightGrid::clearBlockTexture (ATOM_ColorARGB color)
{
	updateBlockTexture (0, 0, _blockTexture->getWidth(), _blockTexture->getHeight(), color);
}

void HeightGrid::clearBlocks (int type, bool set)
{
	for (unsigned i = 0; i < _blockData.size(); ++i)
	{
		if (set)
		{
			_blockData[i] |= type;
		}
		else
		{
			_blockData[i] &= ~type;
		}
	}
}

void HeightGrid::clearBlocksLessAngle (int type, int angle, bool set)
{
	float a = angle * (ATOM_HalfPi / 90.f);

	ATOM_ASSERT(_size.w - 1 == _blockTexture->getWidth());
	ATOM_ASSERT(_size.h - 1 == _blockTexture->getHeight());
	ATOM_ColorARGB *colors = (ATOM_ColorARGB*)ATOM_MALLOC((_size.w-1) * (_size.h-1) * sizeof(ATOM_ColorARGB));

	for (unsigned i = 0; i < _size.h - 1; ++i)
	{
#if 1
		const float *heights = _heightField->getHeights () + i * _size.w;
#else
		const float *heights = _bboxTree->getHeights () + i * _size.w;
#endif
		unsigned char *data = &_blockData[(_size.h-1-i-1) * (_size.w - 1)];
		ATOM_ColorARGB *cdata = colors+i*(_size.w-1);

		for (unsigned j = 0; j < _size.w - 1; ++j, ++heights, ++cdata)
		{
			float hMax = -FLT_MAX;
			float hMin = FLT_MAX;

			if (hMax < *heights) 
			{
				hMax = *heights;
			}

			if (hMax < *(heights+1)) 
			{
				hMax = *(heights+1);
			}

			if (hMax < *(heights+_size.w)) 
			{
				hMax = *(heights+_size.w);
			}

			if (hMax < *(heights+_size.w+1))
			{
				hMax = *(heights+_size.w+1);
			}

			if (hMin > *heights) 
			{
				hMin = *heights;
			}

			if (hMin > *(heights+1)) 
			{
				hMin = *(heights+1);
			}

			if (hMin > *(heights+_size.w)) 
			{
				hMin = *(heights+_size.w);
			}

			if (hMin > *(heights+_size.w+1))
			{
				hMin = *(heights+_size.w+1);
			}

			float t = ATOM_atan ((hMax - hMin) / _cellSize);

			if (t <= a)
			{
				if (set)
				{
					data[j] |= type;
				}
				else
				{
					data[j] &= ~type;
				}
			}

			bool b = (data[j] & type) == type;

			*cdata = _dlgEditBlocks->getBrushColor (b ? (BlockType)type : BT_None);
		}
	}

	_blockTexture->updateTexImage(colors, 0, 0, _size.w-1, _size.h-1, ATOM_PIXEL_FORMAT_BGRA8888);
	ATOM_FREE(colors);
}

void HeightGrid::clearBlocksGreaterAngle (int type, int angle, bool set)
{
	float a = angle * (ATOM_HalfPi / 90.f);

	ATOM_ASSERT(_size.w - 1 == _blockTexture->getWidth());
	ATOM_ASSERT(_size.h - 1 == _blockTexture->getHeight());
	ATOM_ColorARGB *colors = (ATOM_ColorARGB*)ATOM_MALLOC((_size.w-1) * (_size.h-1) * sizeof(ATOM_ColorARGB));

	for (unsigned i = 0; i < _size.h - 1; ++i)
	{
#if 1
		const float *heights = _heightField->getHeights () + i * _size.w;
#else
		const float *heights = _bboxTree->getHeights () + i * _size.w;
#endif
		unsigned char *data = &_blockData[(_size.h-1-i-1) * (_size.w - 1)];
		ATOM_ColorARGB *cdata = colors+i*(_size.w-1);

		for (unsigned j = 0; j < _size.w - 1; ++j, ++heights, ++cdata)
		{
			float hMax = -FLT_MAX;
			float hMin = FLT_MAX;

			if (hMax < *heights) 
			{
				hMax = *heights;
			}

			if (hMax < *(heights+1)) 
			{
				hMax = *(heights+1);
			}

			if (hMax < *(heights+_size.w)) 
			{
				hMax = *(heights+_size.w);
			}

			if (hMax < *(heights+_size.w+1))
			{
				hMax = *(heights+_size.w+1);
			}

			if (hMin > *heights) 
			{
				hMin = *heights;
			}

			if (hMin > *(heights+1)) 
			{
				hMin = *(heights+1);
			}

			if (hMin > *(heights+_size.w)) 
			{
				hMin = *(heights+_size.w);
			}

			if (hMin > *(heights+_size.w+1))
			{
				hMin = *(heights+_size.w+1);
			}

			float t = ATOM_atan ((hMax - hMin) / _cellSize);

			if (t > a)
			{
				if (set)
				{
					data[j] |= type;
				}
				else
				{
					data[j] &= ~type;
				}
			}

			bool b = (data[j] & type) == type;
			*cdata = _dlgEditBlocks->getBrushColor (b ? (BlockType)type : BT_None);
		}
	}

	_blockTexture->updateTexImage(colors, 0, 0, _size.w-1, _size.h-1, ATOM_PIXEL_FORMAT_BGRA8888);
	ATOM_FREE(colors);
}

DlgEditBlocks *HeightGrid::getDlgEditBlocks (void) const
{
	return _dlgEditBlocks;
}

void HeightGrid::updateHeightValue (int x, int z, float height)
{
	if (x >= 0 && x < _positionTexture->getWidth() && z >= 0 && z < _positionTexture->getHeight())
	{
#if 1
		_heightField->updateHeightValue (x, z, height);
#else
		_bboxTree->updateHeightValue (x, z, height);
#endif

		updatePositionHeight (x, z, height);
	}
}

void HeightGrid::updatePositionHeight (int x, int z, float height)
{
	if (x >= 0 && x < _positionTexture->getWidth() && z >= 0 && z < _positionTexture->getHeight())
	{
		ATOM_RenderDevice *device = ATOM_GetRenderDevice();

		if (!_updatePosMaterial)
		{
			_updatePosMaterial = ATOM_MaterialManager::createMaterialFromCore (device, "/editor/materials/updatepos.mat");
			if (!_updatePosMaterial)
			{
				return;
			}
			_updatePosMaterial->setActiveEffect ("default");
		}

#if 1
		ATOM_Vector3f offset = _heightField->getBBoxTree()->getRootNode()->bbox.getMin();
#else
		ATOM_Vector3f offset = _bboxTree->getRootNode()->bbox.getMin();
#endif
		ATOM_Vector4f v(offset.x+x*_cellSize, height, offset.z+z*_cellSize, 1.f);
		_updatePosMaterial->getParameterTable()->setVector ("pos", v);

		ATOM_AUTOREF(ATOM_Texture) oldRT = device->getRenderTarget (0);
		ATOM_Rect2Di oldVP = device->getViewport (NULL);

		device->beginFrame ();
		device->setRenderTarget (0, _positionTexture.get());
		device->setViewport (NULL, ATOM_Rect2Di(x, _positionTexture->getHeight()-1-z, 1, 1));
		
		float vertices[4 * 3] = {
			-1.f, -1.f, 0.f,
			1.f, -1.f, 0.f,
			1.f,  1.f, 0.f,
			-1.f,  1.f, 0.f
		};

		unsigned short indices[4] = {
			0, 1, 2, 3
		};

		unsigned n = _updatePosMaterial->begin (device);
		for (unsigned pass = 0; pass < n; ++pass)
		{
			if (_updatePosMaterial->beginPass (device, pass))
			{
				device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_FAN, 4, 4, ATOM_VERTEX_ATTRIB_COORD, 3 * sizeof(float), vertices, indices);
				_updatePosMaterial->endPass (device, pass);
			}
		}
		_updatePosMaterial->end (device);

		device->setRenderTarget (0, oldRT.get());
		device->setViewport (NULL, oldVP);
		device->endFrame ();
	}
}

void HeightGrid::updateBlockData (int x, int y, int w, int h, int type, bool set)
{
	int tw = _size.w - 1;
	int th = _size.h - 1;
	if (x < 0)
	{
		w += x;
		x = 0;
	}
	if (y < 0)
	{
		h += y;
		y = 0;
	}
	if (x + w > tw)
	{
		w = tw - x;
	}
	if (y + h > th)
	{
		h = th - y;
	}

	if (w > 0 && h > 0)
	{
		for (unsigned i = 0; i < h; ++i)
		{
			for (unsigned j = 0; j < w; ++j)
			{
				if (set)
				{
					_blockData[(y+i)*tw+x+j] |= type;
				}
				else
				{
					_blockData[(y+i)*tw+x+j] &= ~type;
				}
			}
		}
	}
}

void HeightGrid::updateBlockTexture (int x, int y, int w, int h, ATOM_ColorARGB color)
{
	int tw = _blockTexture->getWidth();
	int th = _blockTexture->getHeight();
	if (x < 0)
	{
		w += x;
		x = 0;
	}
	if (y < 0)
	{
		h += y;
		y = 0;
	}
	if (x + w > tw)
	{
		w = tw - x;
	}
	if (y + h > th)
	{
		h = th - y;
	}

	if (w > 0 && h > 0)
	{
		int size = w * h;
		if (size > _colorCache.size() || color != _colorCache[0])
		{
			_colorCache.resize (size);

			for (unsigned i = 0; i < size; ++i)
			{
				_colorCache[i] = color;
			}
		}
		_blockTexture->updateTexImage (&_colorCache[0], x, y, w, h, ATOM_PIXEL_FORMAT_BGRA8888);
	}
}

void HeightGrid::updateBlockTextureByType (int type)
{
	if (!_blockTexture)
	{
		return;
	}

	int tw = _blockTexture->getWidth();
	int th = _blockTexture->getHeight();
	ATOM_ColorARGB *colors = (ATOM_ColorARGB*)ATOM_MALLOC(tw * th * sizeof(ATOM_ColorARGB));
	DlgEditBlocks *dlgEditBlocks = getDlgEditBlocks();

	for (unsigned i = 0; i < th; ++i)
	{
		for (unsigned j = 0; j < tw; ++j)
		{
			int blockType = _blockData[(th-i-1)*tw+j];
			bool b = (blockType & type) == type;
			colors[i*tw+j] = dlgEditBlocks->getBrushColor (b ? (BlockType)type : BT_None);
		}
	}

	getBlockTexture()->updateTexImage(colors, 0, 0, tw, th, ATOM_PIXEL_FORMAT_BGRA8888);
	ATOM_FREE(colors);
}

HeightFieldBBoxTree *HeightGrid::getBBoxTree(void) const
{
	return _heightField->getBBoxTree();
}

bool HeightGrid::isGridShown (void) const
{
	return _heightGrid && _heightGrid->getParent();
}

void HeightGrid::showGrid (bool show)
{
	if (show)
	{
		if (!_heightGrid->getParent())
		{
			_scene->getRootNode()->appendChild (_heightGrid.get());
		}
	}
	else
	{
		if (_heightGrid->getParent())
		{
			_heightGrid->getParent()->removeChild (_heightGrid.get());
		}
	}
}

void HeightGrid::showBlock (bool show)
{
	if (show)
	{
		if (!_blockGrid->getParent())
		{
			_scene->getRootNode()->appendChild (_blockGrid.get());
		}
	}
	else
	{
		if (_blockGrid->getParent())
		{
			_blockGrid->getParent()->removeChild (_blockGrid.get());
		}
	}
}

bool HeightGrid::isBlockShown (void) const
{
	return _blockGrid && _blockGrid->getParent();
}

void HeightGrid::setVertices (const ATOM_Vector4f *vertices)
{
#if 1
	ATOM_DELETE(_heightField);
	_heightField = ATOM_NEW(ATOM_HeightField);
	_heightField->init (_size.w, _size.h, vertices);
#else
	ATOM_DELETE(_bboxTree);
	_bboxTree = ATOM_NEW(HeightFieldBBoxTree, _size.w, _size.h, vertices);
#endif
}

const ATOM_VECTOR<unsigned char> &HeightGrid::getBlockData (void) const
{
	return _blockData;
}

ATOM_VECTOR<unsigned char> &HeightGrid::getBlockData (void)
{
	return _blockData;
}

void HeightGrid::setGridColor (const ATOM_Vector4f &color)
{
	_gridColor = color;
	_heightGridMaterial->getParameterTable()->setVector("gridcolor", color);
}

void HeightGrid::setCellSize (float cellSize)
{
	_cellSize = cellSize;
}

void HeightGrid::setSize (const ATOM_Size2Di &size)
{
	if (size != _size)
	{
		_size = size;

		_positionTexture = 0;
		_blockTexture = 0;

#if 1
		ATOM_DELETE(_heightField);
		_heightField = 0;
#else
		ATOM_DELETE(_bboxTree);
		_bboxTree = 0;
#endif

		if (_size.w > 0 && _size.h > 0)
		{
			_positionTexture = ATOM_GetRenderDevice()->allocTexture (NULL, NULL, _size.w, _size.h, ATOM_PIXEL_FORMAT_RGBA32F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
			_blockTexture = ATOM_GetRenderDevice()->allocTexture (NULL, NULL, _size.w-1, _size.h-1, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOCOMPRESS|ATOM_Texture::NOMIPMAP);
			updateBlockTexture (0, 0, _size.w-1, _size.h-1, _dlgEditBlocks->getBrushColor (BT_None));

			_blockData.resize ((_size.w-1) * (_size.h-1));
			for (unsigned i = 0; i < _blockData.size(); ++i)
			{
				_blockData[i] = BT_None;
			}
		}
	}
}

const ATOM_Size2Di &HeightGrid::getSize (void) const
{
	return _size;
}

ATOM_ShapeNode *HeightGrid::getHeightGrid (void) const
{
	return _heightGrid.get();
}

ATOM_ShapeNode *HeightGrid::getBlockGrid (void) const
{
	return _blockGrid.get();
}

ATOM_Material *HeightGrid::getHeightGridMaterial (void) const
{
	return _heightGridMaterial.get();
}

ATOM_Material *HeightGrid::getBlockGridMaterial (void) const
{
	return _blockGridMaterial.get();
}

ATOM_Texture *HeightGrid::getPositionTexture (void) const
{
	return _positionTexture.get();
}

ATOM_Texture *HeightGrid::getBlockTexture (void) const
{
	return _blockTexture.get();
}

////////////////////////////////////////////////////////////////////////////////////////

ATOM_BEGIN_EVENT_MAP(DlgHeightGen, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(DlgHeightGen, ATOM_WidgetCommandEvent, onCommand)
ATOM_END_EVENT_MAP

DlgHeightGen::DlgHeightGen (ATOM_Widget *parent, ATOM_Scene *scene, DlgEditBlocks *dlgEditBlocks, PluginScene *plugin)
: _heightGrid (scene, dlgEditBlocks, plugin)
{
	_scene = scene;
	_plugin = plugin;
	_heightIndicator = nullptr;
	_region.point.x = 0;
	_region.point.y = 0;
	_region.size.w = 0;
	_region.size.h = 0;
	_dialog = (ATOM_Dialog*)parent->loadHierarchy ("/editor/ui/heightgen.ui");
	if (_dialog)
	{
		initControls (_dialog);
		_dialog->setEventTrigger (this);
		_dialog->show (ATOM_Widget::Hide);
	}
}

DlgHeightGen::~DlgHeightGen (void)
{
	ATOM_DELETE(_dialog);
}

int DlgHeightGen::showModal (void)
{
	if (_dialog)
	{
		SceneSizeCalculator bboxCalc;
		bboxCalc.traverse (*_scene->getRootNode());
		_sceneBoundingBox = bboxCalc.boundingbox;
		int w = int(ATOM_floor(bboxCalc.boundingbox.getExtents().x * 2.f));
		int h = int(ATOM_floor(bboxCalc.boundingbox.getExtents().z * 2.f));

		_cellSize = atof(m_edCellSize->getString().c_str());
		if (_cellSize <= 0.f)
		{
			return false;
		}

		int cw = bboxCalc.boundingbox.getExtents().x * 2.f / _cellSize;
		int ch = bboxCalc.boundingbox.getExtents().z * 2.f / _cellSize;

		int t0 = ATOM_nextpo2 (cw);
		int t1 = ATOM_max2(t0 / 2, 1);
		int hw = (t0 - cw > cw - t1) ? t1 : t0;
		
		int v0 = ATOM_nextpo2 (ch);
		int v1 = ATOM_max2(v0 / 2, 1);
		int hh = (v0 - ch > ch - v1) ? v1 : v0;

		char buff[256];

		m_edX->setString ("0");

		m_edY->setString ("0");

		sprintf (buff, "%d", cw);
		m_edWidth->setString (buff);

		sprintf (buff, "%d", ch);
		m_edHeight->setString (buff);

		sprintf (buff, "%d", int(ATOM_floor(bboxCalc.boundingbox.getMax().y)+2.f));
		m_edNear->setString (buff);

		sprintf (buff, "%d", int(ATOM_floor(bboxCalc.boundingbox.getExtents().y * 2.f)+2.f));
		m_edFar->setString (buff);

		_dialog->show (ATOM_Widget::ShowNormal);

		return true;
	}

	return false;
}

void DlgHeightGen::onCommand (ATOM_WidgetCommandEvent *event)
{
	if (event->id == ID_BTNOK)
	{
		_region.point.x = atoi (m_edX->getString().c_str());
		_region.point.y = atoi (m_edY->getString().c_str());
		_region.size.w = atoi (m_edWidth->getString().c_str());
		_region.size.h = atoi (m_edHeight->getString().c_str());
		_nearFar.x = atoi(m_edNear->getString().c_str());
		_nearFar.y = atoi(m_edFar->getString().c_str());
		_heightGrid.setSize (_region.size);

		if (_region.size.w <= 0 || _region.size.h <= 0 || _nearFar.x <= 0 || _nearFar.y <= 0)
		{
			::MessageBoxA (ATOM_APP->getMainWindow(), "参数错误!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
			return;
		}

		buildHeightGrid (nullptr, false);
		_plugin->showHeightGrid (true);
	}
	else if (event->id == ID_BTNSUBGEN || event->id == ID_BTNSUBGEN2)
	{
		_heightIndicator = _plugin->getCurrentHeightIndicator ();
		if (!_heightIndicator)
		{
			::MessageBoxA (ATOM_APP->getMainWindow(), "未选中高度指示面片!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
			return;
		}
		const ATOM_BBox &bbox = _heightIndicator->getWorldBoundingbox ();
		ATOM_Rect2Di subRegion;
		subRegion.point.x = ceilf (bbox.getMin().x / _cellSize);
		subRegion.point.y = ceilf (bbox.getMin().z / _cellSize);
		float x2 = floorf (bbox.getMax().x / _cellSize);
		float y2 = floorf (bbox.getMax().z / _cellSize);
		subRegion.size.w = x2 - subRegion.point.x + 1;
		subRegion.size.h = y2 - subRegion.point.y + 1;
		_nearFar.x = atoi(m_edNear->getString().c_str());
		_nearFar.y = atoi(m_edFar->getString().c_str());

		if (subRegion.size.w <= 0 || subRegion.size.h <= 0)
		{
			::MessageBoxA (ATOM_APP->getMainWindow(), "参数错误!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
			return;
		}

		buildHeightGrid (_heightIndicator, event->id==ID_BTNSUBGEN, &subRegion);
		_plugin->showHeightGrid (true);
	}
	else if (event->id == ID_BTNCANCEL)
	{
		_dialog->show (ATOM_Widget::Hide);
	}

	//_dialog->endModal (event->id);
}

static bool FilterNode (ATOM_Node *node, const char *prefix)
{
	if (!strcmp (node->getClassName(), GridNode::_classname()) 
		||!strcmp (node->getClassName(), LightGeode::_classname()))
	{
		return true;
	}

	if (!prefix || !prefix[0])
	{
		return false;
	}

	const char *desc = node->getDescribe ().c_str();
	if (strlen(desc) >= strlen(prefix) && !strncmp (desc, prefix, strlen(prefix)))
	{
		return false;
	}

	return true;
}

class TerrainFinderVisitor: public ATOM_Visitor
{
public:
	virtual void visit (ATOM_Node &node) {}
	virtual void visit (ATOM_Terrain &node) { terrain = &node; }
	ATOM_AUTOREF(ATOM_Terrain) terrain;
};

class SkyFinderVisitor: public ATOM_Visitor
{
public:
	virtual void visit (ATOM_Node &node) {}
	virtual void visit (ATOM_Sky &node) { sky = &node; }
	ATOM_AUTOREF(ATOM_Sky) sky;
};

class HeightGenCullVisitor: public ATOM_DeferredCullVisitor
{
	ATOM_STRING _prefix;
	ATOM_ShapeNode *_indicator;
	bool _exclude;

public:
	HeightGenCullVisitor (ATOM_DeferredRenderScheme *deferredRenderScheme, const char *prefix, ATOM_ShapeNode *indicator, bool exclude)
		: ATOM_DeferredCullVisitor(deferredRenderScheme)
		, _prefix(prefix)
		, _indicator(indicator)
		, _exclude(exclude)
	{
	}

public:
	virtual void visit (ATOM_LightNode &node)
	{
		return;
	}

	virtual void visit (ATOM_Atmosphere2 &node)
	{
		return;
	}

	virtual void visit (ATOM_Sky &node)
	{
		return;
	}

	virtual void visit (ATOM_VisualNode &node)
	{
		if (_indicator == &node)
		{
			if (!_exclude)
			{
				node.resetMaterialDirtyFlag();
				ATOM_CullVisitor::visit (node);
			}
		}
		else if (!FilterNode (&node, _prefix.c_str()))
		{
			node.resetMaterialDirtyFlag();
			ATOM_CullVisitor::visit (node);
		}
	}

	virtual void visit (ATOM_CompositionNode &node)
	{
		if (!FilterNode (&node, _prefix.c_str()))
		{
			node.resetMaterialDirtyFlag();
			ATOM_CullVisitor::visit (node);
		}
	}

	virtual void visit (ATOM_Geode &node)
	{
		if (!FilterNode (&node, _prefix.c_str()))
		{
			node.resetMaterialDirtyFlag();
			ATOM_CullVisitor::visit (node);
		}
	}

	virtual void visit (ATOM_Terrain &node)
	{
		node.resetMaterialDirtyFlag();
		ATOM_CullVisitor::visit (node);
	}
};

bool DlgHeightGen::buildHeightGrid (ATOM_ShapeNode *indicator, bool exclude, const ATOM_Rect2Di *subRegion)
{
	TerrainFinderVisitor terrainFinder;
	terrainFinder.traverse (*_scene->getRootNode());
	int drawGrass;
	int pixelError;
	if (terrainFinder.terrain)
	{
		drawGrass = terrainFinder.terrain->getDrawGrass ();
		pixelError = terrainFinder.terrain->getMaxPixelError ();
		terrainFinder.terrain->setDrawGrass (0);
		terrainFinder.terrain->setMaxPixelError (0);
	}

	ATOM_RenderDevice *device = ATOM_GetRenderDevice();
	if (!_heightGenMaterial)
	{
		_heightGenMaterial = ATOM_MaterialManager::createMaterialFromCore (device, "/editor/materials/heightgen.mat");
	}

	if (!_heightGenMaterial)
	{
		::MessageBoxA (ATOM_APP->getMainWindow(), "载入高度图生成材质失败!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
	}
	else
	{
		_heightGenMaterial->setActiveEffect ("default");

		unsigned cellWidth = _heightGrid.getSize().w;
		unsigned cellHeight = _heightGrid.getSize().h;

		ATOM_DeferredRenderScheme *scheme = (ATOM_DeferredRenderScheme*)ATOM_RenderScheme::createRenderScheme ("deferred");
		//-----------------------------------------------------------------------------------------------------------//
		// wangjian added 
		scheme->setSchemeFlag(ATOM_DeferredRenderScheme::DSF_NOMAINSCHEME | ATOM_DeferredRenderScheme::DSF_NOSHADOW);
		//-----------------------------------------------------------------------------------------------------------//
		scheme->setDrawGbufferOnly (true);
		scheme->resize (ATOM_GetRenderDevice(), cellWidth, cellHeight);
		HeightGenCullVisitor v(scheme, m_edIncludePattern->getString().c_str(), indicator, exclude);
		scheme->setCullVisitor (&v);

		ATOM_RenderScheme *oldScheme = _scene->getRenderScheme();
		_scene->setRenderScheme (scheme);

		//---wangjian modified---//
		// 不是引用
		const ATOM_Matrix4x4f projMatrix = _scene->getCamera()->getProjectionMatrix();
		const ATOM_Matrix4x4f viewMatrix = _scene->getCamera()->getViewMatrix();
		const ATOM_Rect2Di viewport = _scene->getCamera()->getViewport ();
		//-----------------------//
		ATOM_ColorARGB clearColor = device->getClearColor (NULL);
		ATOM_Rect2Di vpDevice = device->getViewport (NULL);

		float x = _region.point.x * _cellSize;
		float y = _region.point.y * _cellSize - _cellSize;
		float w = _region.size.w * _cellSize;
		float h = _region.size.h * _cellSize;
		float n = 1.f;
		float f = _nearFar.y+100.f;
		ATOM_Vector3f center(x + 0.5f * w, 0.f, y + 0.5f * h);
		_scene->getCamera()->setOrtho (-w * 0.5f, w * 0.5f, -h * 0.5f, h * 0.5f, n, f);
		_scene->getCamera()->lookAt (center+ATOM_Vector3f(0.f,_nearFar.x,0.f), center, ATOM_Vector3f(0.f, 0.f, 1.f));
		_scene->getCamera()->setViewport (0, 0, cellWidth, cellHeight);

		ATOM_AUTOREF(ATOM_Texture) target0 = device->allocTexture (NULL, NULL, cellWidth, cellHeight, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
		device->beginFrame ();
		device->setClearColor (NULL, 0);
		device->setRenderTarget (0, target0.get());
		device->setViewport (NULL, ATOM_Rect2Di(0, 0, target0->getWidth(), target0->getHeight()));
		_scene->render (device, true);
		device->setRenderTarget (0, 0);

		device->setRenderTarget (0, _heightGrid.getPositionTexture());
		device->setViewport (NULL, ATOM_Rect2Di(0, 0, _heightGrid.getPositionTexture()->getWidth(), _heightGrid.getPositionTexture()->getHeight()));
		_heightGenMaterial->getParameterTable()->setVector ("OrthoParam", ATOM_Vector4f(-w * 0.5f, -h * 0.5f, w * 0.5f, h * 0.5f));
		_heightGenMaterial->getParameterTable()->setVector ("NearFar", ATOM_Vector4f(n, f, 0.f, 0.f));
		_heightGenMaterial->getParameterTable()->setMatrix44("ViewToWorld", _scene->getCamera()->getViewMatrix());
		_heightGenMaterial->getParameterTable()->setTexture ("depthTexture", scheme->getDepthTexture());

		if (subRegion)
		{
			device->enableScissorTest (nullptr, true);
			ATOM_Rect2Di rc = *subRegion;
			rc.point.x -= _region.point.x;
			rc.point.y -= _region.point.y;
			rc.point.y = _heightGrid.getPositionTexture()->getHeight()-rc.point.y-rc.size.h;
			device->setScissorRect (nullptr, rc);
		}
		scheme->drawScreenQuad (device, _heightGenMaterial.get());
		if (subRegion)
		{
			device->enableScissorTest (nullptr, false);
		}

		device->setRenderTarget (0, 0);

		device->endFrame ();

		int tmp = 0;
		if (tmp)
		{
			_heightGrid.getPositionTexture()->saveToFile ("/hh.dds");
		}

		_scene->setRenderScheme (oldScheme);
		ATOM_RenderScheme::destroyRenderScheme (scheme);
		_scene->getCamera()->setProjectionMatrix (projMatrix);
		_scene->getCamera()->setViewMatrix (viewMatrix);
		_scene->getCamera()->setViewport (viewport.point.x, viewport.point.y, viewport.size.w, viewport.size.h);
		device->setViewport (NULL, vpDevice);
		device->setClearColor (NULL, clearColor);

		int pw = _heightGrid.getPositionTexture()->getWidth();
		int ph = _heightGrid.getPositionTexture()->getHeight();
		ATOM_VECTOR<ATOM_Vector4f> buffer(pw * ph);
		_heightGrid.getPositionTexture()->getTexImage (ATOM_PIXEL_FORMAT_RGBA32F, &buffer[0]);
		//ATOM_VECTOR<ATOM_Vector4f> buffer2(pw * ph);
		//for (unsigned i = 0; i < ph; ++i)
		//{
		//	memcpy (&buffer2[i*pw], &buffer[(ph-i-1)*pw], pw*sizeof(float)*4);
		//}
		_heightGrid.setCellSize (_cellSize);
		_heightGrid.setVertices (&buffer[0]);

		_heightGrid.getHeightGrid()->setDivisionX (cellWidth-1);
		_heightGrid.getHeightGrid()->setDivisionY (cellHeight-1);
		_heightGrid.getHeightGridMaterial()->getParameterTable()->setTexture ("heightTexture", _heightGrid.getPositionTexture());

		_heightGrid.getBlockGrid()->setDivisionX (cellWidth-1);
		_heightGrid.getBlockGrid()->setDivisionY (cellHeight-1);
		_heightGrid.getBlockGridMaterial()->getParameterTable()->setTexture ("heightTexture", _heightGrid.getPositionTexture());
		_heightGrid.getBlockGridMaterial()->getParameterTable()->setTexture ("diffuseTexture", _heightGrid.getBlockTexture());
	}

	if (terrainFinder.terrain)
	{
		terrainFinder.terrain->setDrawGrass (drawGrass);
		terrainFinder.terrain->setMaxPixelError (pixelError);
	}

	return true;
}

HeightGrid *DlgHeightGen::getGrid (void)
{
	return &_heightGrid;
}

float DlgHeightGen::getCellSize (void) const
{
	return _cellSize;
}

const ATOM_Rect2Di &DlgHeightGen::getRegion (void) const
{
	return _region;
}

struct HeightFieldHeader
{
	UINT32 m_nWidth;	//!< 地图X轴格子数
	UINT32 m_nHeight;	//!< 地图Y轴格子数
	FLOAT m_fSpacingX;	//!< 格子宽度
	FLOAT m_fSpacingZ;	//!< 格子高度
	FLOAT m_fOffsetX;	//!< X轴偏移
	FLOAT m_fOffsetZ;	//!< Z轴偏移
	// FLOAT m_arHeight[m_nWidth*m_nHeight];	// 这里是文件头之后的高度信息
};

bool DlgHeightGen::loadHeightData (const char *filename)
{
	ATOM_AutoFile f(filename, ATOM_VFS::binary|ATOM_VFS::read);
	if (!f)
	{
		return false;
	}

	HeightFieldHeader header;
	f->read (&header, sizeof(header));

	ATOM_Vector4f *v = ATOM_NEW_ARRAY(ATOM_Vector4f, header.m_nWidth * header.m_nHeight);
	float *h = ATOM_NEW_ARRAY(float, header.m_nWidth);

	for (unsigned i = 0; i < header.m_nHeight; ++i)
	{
		f->read (h, header.m_nWidth * sizeof(float));

		ATOM_Vector4f *dst = v + (header.m_nHeight - i - 1) * header.m_nWidth;

		for (unsigned j = 0; j < header.m_nWidth; ++j)
		{
			dst->x = header.m_fOffsetX + j * header.m_fSpacingX;
			dst->y = h[j];
			dst->z = header.m_fOffsetZ + i * header.m_fSpacingZ;
			dst->w = 1.f;
			dst++;
		}
	}

	_cellSize = header.m_fSpacingX;
	_region.point.x = header.m_fOffsetX / _cellSize;
	_region.point.y = header.m_fOffsetZ / _cellSize;
	_region.size.w = header.m_nWidth;
	_region.size.h = header.m_nHeight;

	//--- wangjian added ---//
	_nearFar.x = 1000;
	_nearFar.y = 1000;
	unsigned long num = f->read( &_nearFar,sizeof(ATOM_Point2Di) );
	if(!num)
	{
		::MessageBoxA( ATOM_APP->getMainWindow(),"该高度图文件版本需要更新，请重新生成高度图!","ATOM3D编辑器",MB_OK|MB_ICONHAND );
	}
	//----------------------//

	_heightGrid.setCellSize (_cellSize);
	_heightGrid.setSize (ATOM_Size2Di(header.m_nWidth, header.m_nHeight));
	_heightGrid.setVertices (v);
	_heightGrid.getPositionTexture()->updateTexImage (v, 0, 0, header.m_nWidth, header.m_nHeight, ATOM_PIXEL_FORMAT_RGBA32F);
	_heightGrid.getHeightGrid()->setDivisionX (header.m_nWidth-1);
	_heightGrid.getHeightGrid()->setDivisionY (header.m_nHeight-1);
	_heightGrid.getHeightGridMaterial()->getParameterTable()->setTexture ("heightTexture", _heightGrid.getPositionTexture());

	// wangjian added
	unsigned cellWidth = _heightGrid.getSize().w;
	unsigned cellHeight = _heightGrid.getSize().h;
	_heightGrid.getBlockGrid()->setDivisionX (cellWidth-1);
	_heightGrid.getBlockGrid()->setDivisionY (cellHeight-1);
	_heightGrid.getBlockGridMaterial()->getParameterTable()->setTexture ("heightTexture", _heightGrid.getPositionTexture());
	_heightGrid.getBlockGridMaterial()->getParameterTable()->setTexture ("diffuseTexture", _heightGrid.getBlockTexture());

	ATOM_DELETE_ARRAY(v);

	return true;
}

bool DlgHeightGen::saveHeightData (const char *filename)
{
	if (_heightGrid.getSize().w <= 0 || _heightGrid.getSize().h <= 0 || !_heightGrid.getBBoxTree())
	{
		return false;
	}

	ATOM_AutoFile f(filename, ATOM_VFS::binary|ATOM_VFS::write);
	if (!f)
	{
		return false;
	}

	HeightFieldHeader header;
	header.m_nWidth = _heightGrid.getSize().w;
	header.m_nHeight = _heightGrid.getSize().h;
	header.m_fSpacingX = _cellSize;
	header.m_fSpacingZ = _cellSize;
	header.m_fOffsetX = _region.point.x * _cellSize;
	header.m_fOffsetZ = _region.point.y * _cellSize;

	f->write (&header, sizeof(header));
	for (unsigned i = 0; i < header.m_nHeight; ++i)
	{
		f->write (_heightGrid.getBBoxTree()->getHeights()+(header.m_nHeight-i-1)*header.m_nWidth, header.m_nWidth * sizeof(float));
	}
	//--- wangjian added ---//
	f->write (&_nearFar, sizeof(ATOM_Point2Di));
	//----------------------//

	return true;
}

bool DlgHeightGen::exportHeightMap (const char *filename)
{
	ATOM_AutoFile f(filename, ATOM_VFS::write);
	if (!f)
	{
		return false;
	}

	float minHeight = _heightGrid.getBBoxTree()->getRootNode()->bbox.getMin().y;
	float maxHeight = _heightGrid.getBBoxTree()->getRootNode()->bbox.getMax().y;
	float heightRange = maxHeight - minHeight;

	ATOM_BaseImage img;
	img.init (_heightGrid.getSize().w, _heightGrid.getSize().h, ATOM_PIXEL_FORMAT_BGRA8888, NULL, 1);
	ATOM_ColorARGB *colors = (ATOM_ColorARGB*)img.getData();

	for (unsigned i = 0; i < img.getHeight(); ++i)
	{
		const float *heights = _heightGrid.getBBoxTree()->getHeights() + i * img.getWidth();
		ATOM_ColorARGB *c = colors + i * img.getWidth();

		for (unsigned j = 0; j < img.getWidth(); ++j)
		{
			float g = heightRange > 0.0001f ? (heights[j]-minHeight)/heightRange :0.f;
			c[j].setFloats (g, g, g, 1.f);
		}
	}

	return img.save (f, ATOM_PIXEL_FORMAT_BGRA8888);
}

struct BlockFileHeader
{
	UINT32 m_nWidth;	// 地图X轴格子数
	UINT32 m_nHeight;	// 地图Y轴格子数

	// UINT8 m_arBlock[m_nWidth*m_nHeight];	// 这里是文件头之后的障碍信息,具体参照MAPBLOCK
};

bool DlgHeightGen::loadBlockData (const char *filename)
{
	if (_heightGrid.getSize().w <= 0 || _heightGrid.getSize().h <= 0)
	{
		::MessageBoxA (ATOM_APP->getMainWindow(), "没有高度信息!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
		return false;
	}

	ATOM_AutoFile f(filename, ATOM_VFS::binary|ATOM_VFS::read);
	if (!f)
	{
		return false;
	}

	BlockFileHeader header;
	f->read (&header, sizeof(header));

	if (header.m_nWidth != _heightGrid.getSize().w - 1 || header.m_nHeight != _heightGrid.getSize().h - 1)
	{
		::MessageBoxA (ATOM_APP->getMainWindow(), "障碍尺寸和高度图尺寸不匹配!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
		return false;
	}

	_heightGrid.getBlockData().resize (header.m_nWidth * header.m_nHeight);
	unsigned char *data = &_heightGrid.getBlockData()[0];
	f->read (data, header.m_nWidth * header.m_nHeight * sizeof(unsigned char));
	/*
	ATOM_ColorARGB *colors = (ATOM_ColorARGB*)ATOM_MALLOC(header.m_nWidth * header.m_nHeight * sizeof(ATOM_ColorARGB));
	DlgEditBlocks *dlgEditBlocks = _heightGrid.getDlgEditBlocks();

	for (unsigned i = 0; i < header.m_nHeight; ++i)
	{
		for (unsigned j = 0; j < header.m_nWidth; ++j)
		{
			int blockType = data[(header.m_nHeight-i-1)*header.m_nWidth+j];
			colors[i*header.m_nWidth+j] = dlgEditBlocks->getBrushColor ((BlockType)blockType);
		}
	}

	_heightGrid.getBlockTexture()->updateTexImage(colors, 0, 0, header.m_nWidth, header.m_nHeight, ATOM_PIXEL_FORMAT_BGRA8888);
	ATOM_FREE(colors);
	*/
	_heightGrid.getBlockGrid()->setDivisionX (_heightGrid.getSize().w-1);
	_heightGrid.getBlockGrid()->setDivisionY (_heightGrid.getSize().h-1);
	_heightGrid.getBlockGridMaterial()->getParameterTable()->setTexture ("heightTexture", _heightGrid.getPositionTexture());
	_heightGrid.getBlockGridMaterial()->getParameterTable()->setTexture ("diffuseTexture", _heightGrid.getBlockTexture());

	_heightGrid.updateBlockTextureByType (BT_Block);

	return true;
}

bool DlgHeightGen::saveBlockData (const char *filename)
{
	if (_heightGrid.getSize().w <= 0 || _heightGrid.getSize().h <= 0 || !_heightGrid.getBBoxTree())
	{
		return false;
	}

	ATOM_AutoFile f(filename, ATOM_VFS::binary|ATOM_VFS::write);
	if (!f)
	{
		return false;
	}

	BlockFileHeader header;
	header.m_nWidth = _heightGrid.getSize().w - 1;
	header.m_nHeight = _heightGrid.getSize().h - 1;

	f->write (&header, sizeof(header));
	f->write (&_heightGrid.getBlockData()[0], header.m_nWidth * header.m_nHeight * sizeof(unsigned char));

	return true;
}

bool DlgHeightGen::exportBlockMap (const char *filename)
{
	ATOM_AutoFile f(filename, ATOM_VFS::write);
	if (!f)
	{
		return false;
	}

	int w = _heightGrid.getSize().w - 1;
	int h = _heightGrid.getSize().h - 1;
	ATOM_BaseImage img;
	img.init (w, h, ATOM_PIXEL_FORMAT_BGRA8888, NULL, 1);
	ATOM_ColorARGB *colors = (ATOM_ColorARGB*)img.getData();
	unsigned char *data = &_heightGrid.getBlockData()[0];

	DlgEditBlocks *dlgEditBlocks = _heightGrid.getDlgEditBlocks();

	for (unsigned i = 0; i < img.getHeight(); ++i)
	{
		for (unsigned j = 0; j < img.getWidth(); ++j)
		{
			int blockType = data[(img.getHeight()-i-1)*img.getWidth()+j];
			colors[i*img.getWidth()+j] = dlgEditBlocks->getBrushColor ((BlockType)blockType);
		}
	}

	return img.save (f, ATOM_PIXEL_FORMAT_BGRA8888);
}


bool DlgHeightGen::exportSceneMap (const char *filename)
{
	SkyFinderVisitor skyFinder;
	skyFinder.traverse (*_scene->getRootNode());
	bool drawSky;
	if (skyFinder.sky)
	{
		drawSky = skyFinder.sky->isDrawSkyEnabled();
		skyFinder.sky->enableDrawSky (false);
	}

	TerrainFinderVisitor terrainFinder;
	terrainFinder.traverse (*_scene->getRootNode());
	int drawGrass;
	int pixelError;
	if (terrainFinder.terrain)
	{
		drawGrass = terrainFinder.terrain->getDrawGrass ();
		pixelError = terrainFinder.terrain->getMaxPixelError ();
		terrainFinder.terrain->setDrawGrass (0);
		terrainFinder.terrain->setMaxPixelError (0);
	}

	bool gridShown = _heightGrid.isGridShown ();
	_heightGrid.showGrid (false);

	bool blockShown = _heightGrid.isBlockShown ();
	_heightGrid.showBlock (false);

	ATOM_RenderDevice *device = ATOM_GetRenderDevice();
	SceneSizeCalculator bboxCalc;
	bboxCalc.traverse (*_scene->getRootNode());

	unsigned cellWidth = _heightGrid.getSize().w;
	unsigned cellHeight = _heightGrid.getSize().h;
	unsigned viewWidth, viewHeight;
	if (cellWidth > cellHeight)
	{
		viewWidth = 1024;
		viewHeight = 1024 * (float(cellHeight)/float(cellWidth));
	}
	else
	{
		viewHeight = 1024;
		viewWidth = 1024 * (float(cellWidth)/float(cellHeight));
	}
	//viewHeight = cellHeight;
	//viewWidth = cellWidth;
	if (viewWidth == 0 || viewHeight == 0)
	{
		::MessageBoxA (ATOM_APP->getMainWindow(), "需要生成或载入正确的高度信息!", "ATOM3D编辑器", MB_OK|MB_ICONHAND);
		return false;
	}

	ATOM_DeferredRenderScheme *scheme = (ATOM_DeferredRenderScheme*)ATOM_RenderScheme::createRenderScheme ("deferred");
	//-----------------------------------------------------------------------------------------------------------//
	// wangjian added 
	//scheme->setSchemeFlag(ATOM_DeferredRenderScheme::DSF_NOMAINSCHEME | ATOM_DeferredRenderScheme::DSF_NOSHADOW);
	//-----------------------------------------------------------------------------------------------------------//
	scheme->resize (ATOM_GetRenderDevice(), viewWidth, viewHeight);

	ATOM_RenderScheme *oldScheme = _scene->getRenderScheme();
	_scene->setRenderScheme (scheme);

	const ATOM_Matrix4x4f &projMatrix = _scene->getCamera()->getProjectionMatrix();
	const ATOM_Matrix4x4f &viewMatrix = _scene->getCamera()->getViewMatrix();
	const ATOM_Rect2Di &viewport = _scene->getCamera()->getViewport ();
	ATOM_ColorARGB clearColor = device->getClearColor (NULL);
	ATOM_Rect2Di vpDevice = device->getViewport (NULL);
	ATOM_AUTOREF(ATOM_DepthBuffer) oldDepth = device->getDepthBuffer ();

	float x = _region.point.x * _cellSize;
	float y = _region.point.y * _cellSize - _cellSize;
	float w = _region.size.w * _cellSize;
	float h = _region.size.h * _cellSize;
	float n = 1.f;
	float f = bboxCalc.boundingbox.getExtents().y*2.f+100.f;
	ATOM_Vector3f center(x + 0.5f * w, 0.f, y + 0.5f * h);
	_scene->getCamera()->setOrtho (-w * 0.5f, w * 0.5f, -h * 0.5f, h * 0.5f, n, f);
	_scene->getCamera()->lookAt (center+ATOM_Vector3f(0.f,bboxCalc.boundingbox.getMax().y+1.f,0.f), center, ATOM_Vector3f(0.f, 0.f, 1.f));
	_scene->getCamera()->setViewport (0, 0, viewWidth, viewHeight);

	ATOM_AUTOREF(ATOM_Texture) target0 = device->allocTexture (NULL, NULL, viewWidth, viewHeight, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
	ATOM_AUTOREF(ATOM_DepthBuffer) depth0 = device->allocDepthBuffer (viewWidth, viewHeight);
	device->beginFrame ();
	device->setClearColor (NULL, 0);
	device->setRenderTarget (0, target0.get());
	device->setDepthBuffer (depth0.get());
	device->setViewport (NULL, ATOM_Rect2Di(0, 0, target0->getWidth(), target0->getHeight()));
	_scene->render (device, true);
	device->setRenderTarget (0, 0);
	device->setDepthBuffer (oldDepth.get());
	device->endFrame ();

	_scene->setRenderScheme (oldScheme);

	bool succ = scheme->getAlbedoTexture()->saveToFile (filename);

	ATOM_RenderScheme::destroyRenderScheme (scheme);
	_scene->getCamera()->setProjectionMatrix (projMatrix);
	_scene->getCamera()->setViewMatrix (viewMatrix);
	_scene->getCamera()->setViewport (viewport.point.x, viewport.point.y, viewport.size.w, viewport.size.h);
	device->setViewport (NULL, vpDevice);
	device->setClearColor (NULL, clearColor);

	_heightGrid.showGrid (gridShown);
	_heightGrid.showBlock (blockShown);
	if (skyFinder.sky)
	{
		skyFinder.sky->enableDrawSky (drawSky);
	}
	if (terrainFinder.terrain)
	{
		terrainFinder.terrain->setDrawGrass (drawGrass);
		terrainFinder.terrain->setMaxPixelError (pixelError);
	}

	return succ;
}
