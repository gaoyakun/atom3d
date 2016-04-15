#include "StdAfx.h"
#include "terrainpatch.h"
#include "terrainquadtree.h"

ATOM_TerrainPatch::ATOM_TerrainPatch (void)
{
	ATOM_STACK_TRACE(ATOM_TerrainPatch::ATOM_TerrainPatch);

	_mipLevel = 0;
	_offsetX = 0;
	_offsetZ = 0;
	_position = LeftTop;
	_parent = 0;
	_quadtree = 0;
	_step = 0;
	_lodDistance = 0;
	_geometry = ATOM_NEW(ATOM_MultiStreamGeometry);
}

ATOM_TerrainPatch::~ATOM_TerrainPatch (void)
{
	ATOM_STACK_TRACE(ATOM_TerrainPatch::~ATOM_TerrainPatch);

	ATOM_DELETE(_geometry);
}

bool ATOM_TerrainPatch::initialize (ATOM_TerrainQuadtree *quadtree, ATOM_TerrainPatch *parent, PatchPosition position, ATOM_VertexArray *baseVertices)
{
	ATOM_STACK_TRACE(ATOM_TerrainPatch::initialize);

	ATOM_ASSERT(quadtree);

	unsigned patchSize = quadtree->getPatchSize ();
	unsigned rootSize = quadtree->getRootSize ();
	float scaleX = quadtree->getScaleX ();
	float scaleZ = quadtree->getScaleZ ();

	_mipLevel = parent ? parent->getMipLevel() + 1 : 0;
	unsigned step = ((rootSize - 1) / (patchSize - 1)) >> _mipLevel;
	unsigned interval = (patchSize - 1) * step;
	unsigned parentOffsetX = parent ? parent->getOffsetX() : 0;
	unsigned parentOffsetZ = parent ? parent->getOffsetZ() : 0;

	switch (position)
	{
	case ATOM_TerrainPatch::LeftTop:
		_offsetX = parentOffsetX;
		_offsetZ = parentOffsetZ;
		break;
	case ATOM_TerrainPatch::RightTop:
		_offsetX = parentOffsetX + interval;
		_offsetZ = parentOffsetZ;
		break;
	case ATOM_TerrainPatch::LeftBottom:
		_offsetX = parentOffsetX;
		_offsetZ = parentOffsetZ + interval;
		break;
	case ATOM_TerrainPatch::RightBottom:
		_offsetX = parentOffsetX + interval;
		_offsetZ = parentOffsetZ + interval;
		break;
	default:
		return false;
	}

	_position = position;
	_quadtree = quadtree;
	_step = step;
	_parent = parent;
	_maxError = computeMaxError ();

	setupVertices (computeSkirtLength (), baseVertices);
	_boxRadius = computeBoundingBox (_boundingBox);

	return true;
}

void ATOM_TerrainPatch::setupCamera (int viewportH, float tanHalfFovy, unsigned maxPixelError)
{
	if (maxPixelError > 0)
	{
		_lodDistance = computeLodDistance (viewportH, tanHalfFovy, maxPixelError);
	}
	else
	{
		_lodDistance = -1.f;
	}
}

void ATOM_TerrainPatch::setupVertices (float skirtLength, ATOM_VertexArray *baseVertices)
{
	ATOM_STACK_TRACE(ATOM_TerrainPatch::setupVertices);

	//const float *elevation = _quadtree->getElevations ();
	unsigned rootSize = _quadtree->getRootSize ();
	unsigned patchSize = _quadtree->getPatchSize ();

	ATOM_RenderDevice *device = ATOM_GetRenderDevice ();
	ATOM_ASSERT(device);

	ATOM_AUTOREF(ATOM_VertexArray) texcoordArray = device->allocVertexArray (ATOM_VERTEX_ATTRIB_TEX1_1, ATOM_USAGE_STATIC, (patchSize + 2) * (patchSize + 2), true);
	float *texcoords = (float*)texcoordArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	ATOM_ASSERT(texcoords);

	unsigned x = _offsetX, z = _offsetZ;
	float scaleX = _quadtree->getScaleX();
	float scaleZ = _quadtree->getScaleZ();

	*texcoords++ = _quadtree->getHeightField()->getHeight(x, z) - skirtLength;

	for (unsigned i = 0; i < patchSize; ++i, x += _step)
	{
		*texcoords++ = _quadtree->getHeightField()->getHeight(x, z) - skirtLength;
	}

	*texcoords++ = _quadtree->getHeightField()->getHeight(x-_step, z) - skirtLength;

	z = _offsetZ;

	for (unsigned i = 0; i < patchSize; ++i, z += _step)
	{
		x = _offsetX;
		*texcoords++ = _quadtree->getHeightField()->getHeight(x, z) - skirtLength;

		for (unsigned j = 0; j < patchSize; ++j, x += _step)
		{
			*texcoords++ = _quadtree->getHeightField()->getHeight(x, z);
		}

		*texcoords++ = _quadtree->getHeightField()->getHeight(x-_step, z) - skirtLength;
	}

	x = _offsetX;
	z -= _step;

	*texcoords++ = _quadtree->getHeightField()->getHeight(x, z) - skirtLength;

	for (unsigned i = 0; i < patchSize; ++i, x += _step)
	{
		*texcoords++ = _quadtree->getHeightField()->getHeight(x, z) - skirtLength;
	}

	*texcoords++ = _quadtree->getHeightField()->getHeight(x-_step, z) - skirtLength;

	texcoordArray->unlock ();

	_geometry->addStream (baseVertices);
	_geometry->addStream (texcoordArray.get());
	_geometry->setIndices (_quadtree->getIndices());
	_geometry->createVertexDecl ();
}

unsigned ATOM_TerrainPatch::getMipLevel (void) const
{
	return _mipLevel;
}

ATOM_TerrainPatch::PatchPosition ATOM_TerrainPatch::getPosition (void) const
{
	return _position;
}

unsigned ATOM_TerrainPatch::getOffsetX (void) const
{
	return _offsetX;
}

unsigned ATOM_TerrainPatch::getOffsetZ (void) const
{
	return _offsetZ;
}

unsigned ATOM_TerrainPatch::getStep (void) const
{
	return _step;
}

void ATOM_TerrainPatch::render (ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_TerrainPatch::render);

	ATOM_ASSERT(_quadtree);
	_geometry->draw (device, _quadtree->getPrimitiveType(), _quadtree->getPrimitiveCount(), 0);
}

float ATOM_TerrainPatch::getHeight (unsigned x, unsigned z) const
{
	unsigned patchSize = _quadtree->getPatchSize ();
	unsigned dimension = (patchSize - 1) * _step;
	ATOM_ASSERT (x >= _offsetX && x <= _offsetX + dimension);
	ATOM_ASSERT (z >= _offsetZ && z <= _offsetZ + dimension);
	unsigned startX = _offsetX + _step * ((x - _offsetX) / _step);
	unsigned startZ = _offsetZ + _step * ((z - _offsetZ) / _step);
	unsigned endX = startX == x ? startX : startX + _step;
	unsigned endZ = startZ == z ? startZ : startZ + _step;
	unsigned rootSize = _quadtree->getRootSize ();
	float lt_height = _quadtree->getHeightField()->getHeight(startX, startZ);
	float rt_height = _quadtree->getHeightField()->getHeight(endX, startZ);
	float lb_height = _quadtree->getHeightField()->getHeight(startX, endZ);
	float rb_height = _quadtree->getHeightField()->getHeight(endX, endZ);;
	float t1 = float(x - startX) / float(_step);
	float t2 = float(z - startZ) / float(_step);
	float h1 = lt_height + (rt_height - lt_height) * t1;
	float h2 = lb_height + (rb_height - lb_height) * t1;
	return h1 + (h2 - h1) * t2;
}

float ATOM_TerrainPatch::computeErrorMetric (ATOM_TerrainPatch *other) const
{
	float errorMetric = 0.f;

	if (other->getMipLevel() > _mipLevel)
	{
		unsigned otherOffsetX = other->getOffsetX ();
		unsigned otherOffsetZ = other->getOffsetZ ();
		unsigned otherStep = other->getStep ();
		unsigned otherDimension = other->getStep () * _quadtree->getPatchSize();

		for (unsigned i = otherOffsetZ; i < otherOffsetZ + otherDimension; i += otherStep)
		{
			for (unsigned j = otherOffsetX; j < otherOffsetX + otherDimension; j += otherStep)
			{
				float error = ATOM_abs (getHeight (j, i) - other->getHeight (j, i));
				if (error > errorMetric)
				{
					errorMetric = error;
				}
			}
		}
	}
	else if (other->getMipLevel() < _mipLevel)
	{
		unsigned dimension = _step * (_quadtree->getPatchSize() - 1);

		for (unsigned i = _offsetZ; i <= _offsetZ + dimension; i += _step)
		{
			for (unsigned j = _offsetX; j <= _offsetX + dimension; j += _step)
			{
				float error = ATOM_abs (getHeight (j, i) - other->getHeight (j, i));
				if (error > errorMetric)
				{
					errorMetric = error;
				}
			}
		}
	}

	return errorMetric;
}

float ATOM_TerrainPatch::computeSkirtLength (void) const
{
	float skirtLength = 0.f;
	ATOM_TerrainPatch *p = _parent;
	while (p)
	{
		float f = computeErrorMetric (p);
		if (f > skirtLength)
		{
			skirtLength = f;
		}
		p = p->_parent;
	}

	return skirtLength;
}

void ATOM_TerrainPatch::computeHeightBound (float &maxHeight, float &minHeight) const
{
	maxHeight = 0.f;
	minHeight = 1.f;

	unsigned dimension = _step * (_quadtree->getPatchSize() - 1);
	unsigned rootSize = _quadtree->getRootSize();

	for (unsigned i = _offsetZ; i <= _offsetZ + dimension; ++i)
	{
		for (unsigned j = _offsetX; j <= _offsetX + dimension; ++j)
		{
			float h = _quadtree->getHeightField()->getHeight(j, i);
			if (h > maxHeight)
			{
				maxHeight = h;
			}
			if (h < minHeight)
			{
				minHeight = h;
			}
		}
	}
}

float ATOM_TerrainPatch::computeMaxError (void) const
{
	if (_step == 1)
	{
		return 0.f;
	}

	float maxError = 0.f;
	unsigned dimension = _step * (_quadtree->getPatchSize() - 1);
	unsigned rootSize = _quadtree->getRootSize();
	unsigned step = _step / 2;

#if 1
	for (unsigned i = _offsetZ; i <= _offsetZ + dimension; i += 1)
	{
		for (unsigned j = _offsetX; j <= _offsetX + dimension; j += 1)
		{
			int i00 = _offsetZ + ((i - _offsetZ) / _step) * _step;
			int j00 = _offsetX + ((j - _offsetX) / _step) * _step;
			if (i00 == rootSize - 1 || j00 == rootSize - 1)
			{
				continue;
			}
			int i11 = i00 + _step;
			int j11 = j00 + _step;
			float h00 = _quadtree->getHeightField()->getHeight(j00, i00);
			float h01 = _quadtree->getHeightField()->getHeight(j11, i00);
			float h10 = _quadtree->getHeightField()->getHeight(j00, i11);
			float h11 = _quadtree->getHeightField()->getHeight(j11, i11);
			float factorZ = float(i - i00)/float(_step);
			float factorX = float(j - j00)/float(_step);
			float h = _quadtree->getHeightField()->getHeight(j, i);
			float h0 = h00 + factorX * (h01 - h00);
			float h1 = h10 + factorX * (h11 - h10);
			float h2 = h0 + factorZ * (h1 - h0);
			float error = ATOM_abs(h - h2);
			if (error > maxError)
			{
				maxError = error;
			}
		}
	}
#else
	for (unsigned i = _offsetZ; i <= _offsetZ + dimension; i += step)
	{
		for (unsigned j = _offsetX; j <= _offsetX + dimension; j += step)
		{
			float h = elevations [i * rootSize + j];
			float error = nabs (getHeight (j, i) - h);
			if (error > maxError)
			{
				maxError = error;
			}
		}
	}
#endif
	return maxError;
}

float ATOM_TerrainPatch::computeLodDistance (int viewportH, float tanHalfFovy, unsigned maxPixelError) const
{
	ATOM_ASSERT(_quadtree);
	return 0.5f * _maxError * float(viewportH) / (maxPixelError * tanHalfFovy);
}

float ATOM_TerrainPatch::computeBoundingBox (ATOM_BBox &bbox) const
{
	ATOM_ASSERT(_quadtree);

	float maxHeight, minHeight;
	computeHeightBound (maxHeight, minHeight);

	float scaleX = _quadtree->getScaleX ();
	float scaleZ = _quadtree->getScaleZ ();
	ATOM_Vector3f minPoint(_offsetX * scaleX, minHeight, _offsetZ * scaleZ);
	ATOM_Vector3f maxPoint(_offsetX * scaleX + (_quadtree->getPatchSize() - 1) * _step * scaleX, maxHeight, _offsetZ * scaleZ + (_quadtree->getPatchSize() - 1) * _step * scaleZ);

	bbox.setMax (maxPoint);
	bbox.setMin (minPoint);

	return (maxPoint - minPoint).getLength() * 0.5f;
}

float ATOM_TerrainPatch::getLodDistance (void) const
{
	return _lodDistance;
}

const ATOM_BBox &ATOM_TerrainPatch::getBoundingbox (void) const
{
	return _boundingBox;
}

static inline float SQR (float x)
{
	return x * x;
}

bool ATOM_TerrainPatch::insideDetailRegion (const ATOM_Vector3f &eyePos, float distance) const
{
	return (eyePos - _boundingBox.getCenter()).getSquaredLength () < SQR(_boxRadius + distance);
}

