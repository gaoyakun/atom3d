#include "StdAfx.h"
#include "terrainrenderdatas.h"
#include "terrainpatch.h"

void ATOM_TerrainRenderDatas::setTerrain (ATOM_Terrain *terrain)
{
	ATOM_STACK_TRACE(ATOM_TerrainRenderDatas::setTerrain);

	_terrain = terrain;
}

void ATOM_TerrainRenderDatas::addPatch (ATOM_TerrainPatch *patch, bool detail)
{
	ATOM_STACK_TRACE(ATOM_TerrainRenderDatas::addPatch);

	if (detail)
	{
		_detailPatches.push_back (patch);
	}
	else
	{
		_nondetailPatches.push_back (patch);
	}
}

void ATOM_TerrainRenderDatas::clearPatches (void)
{
	ATOM_STACK_TRACE(ATOM_TerrainRenderDatas::clearPatches);

	_detailPatches.resize (0);
	_nondetailPatches.resize (0);
}

void ATOM_TerrainRenderDatas::setFogEnabled (bool fogEnabled)
{
	_fogEnabled = fogEnabled;
}

bool ATOM_TerrainRenderDatas::draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{
	ATOM_STACK_TRACE(ATOM_TerrainRenderDatas::render);

	//---------------------------------//
	// wangjian added
	_terrain->checkEnableDetailNormal();
	//---------------------------------//

	ATOM_StateCache *stateCache = device->getStateCache();
	ATOM_RenderStateSet *stateSet = stateCache->getDesiredStates ();

	float scaleX = _terrain->getScale().x;
	float scaleZ = _terrain->getScale().z;

	if (_terrain->getRenderMode() == ATOM_Terrain::RM_COLOR)
	{
		if (!_detailPatches.empty ())
		{
			material->getParameterTable()->setInt ("drawDetail", 1);
			material->autoSetActiveEffect ();

			for (unsigned n = 0; n < _detailPatches.size(); ++n)
			{
				ATOM_Vector4f scaleOffset;
				scaleOffset.x = _detailPatches[n]->getStep() * scaleX;
				scaleOffset.y = _detailPatches[n]->getOffsetX() * scaleX;
				scaleOffset.z = _detailPatches[n]->getStep() * scaleZ;
				scaleOffset.w = _detailPatches[n]->getOffsetZ() * scaleZ;
				material->getParameterTable()->setVector ("offsetScale", scaleOffset);

				unsigned numPasses = material->begin (device);
				for (unsigned pass = 0; pass < numPasses; ++pass)
				{
					if (material->beginPass (device, pass))
					{
						_detailPatches[n]->render (device);
						material->endPass (device, pass);
					}
				}
				material->end (device);
			}
		}

		if (!_nondetailPatches.empty ())
		{
			material->getParameterTable()->setInt ("drawDetail", 0);
			material->autoSetActiveEffect ();

			for (unsigned n = 0; n < _nondetailPatches.size(); ++n)
			{
				ATOM_Vector4f scaleOffset;
				scaleOffset.x = _nondetailPatches[n]->getStep() * scaleX;
				scaleOffset.y = _nondetailPatches[n]->getOffsetX() * scaleX;
				scaleOffset.z = _nondetailPatches[n]->getStep() * scaleZ;
				scaleOffset.w = _nondetailPatches[n]->getOffsetZ() * scaleZ;
				material->getParameterTable()->setVector ("offsetScale", scaleOffset);

				unsigned numPasses = material->begin (device);
				for (unsigned pass = 0; pass < numPasses; ++pass)
				{
					if (material->beginPass (device, pass))
					{
						_nondetailPatches[n]->render (device);
						material->endPass (device, pass);
					}
				}
				material->end (device);
			}
		}
	}

	return true;
}

static const unsigned TERRAIN_SIZE_FF = 128;

struct TerrainVertex
{
    ATOM_Vector3f v;
    ATOM_Vector3f n;
};

bool ATOM_TerrainRenderDatasFF::create (ATOM_RenderDevice *device, ATOM_Terrain *terrain, ATOM_HeightField *hf, const ATOM_Vector3f *normals)
{
	ATOM_STACK_TRACE(ATOM_TerrainRenderDatasFF::create);

	//TODO getSizeX()
	unsigned size = hf->getSizeX() - 1;
	unsigned dim = ATOM_min2 (TERRAIN_SIZE_FF, size);
	unsigned step = size / dim;
	unsigned dim2 = dim + 1 + 2;
	unsigned rootSize = size + 1;
	unsigned patchSize = dim + 1;
	const float *elevation = hf->getHeights ();

	TerrainVertex *v = ATOM_NEW_ARRAY(TerrainVertex, dim2 * dim2);
	TerrainVertex *vertices = v;

	unsigned x = 0, z = 0;
	float scaleX = hf->getSpacingX();
	float scaleZ = hf->getSpacingZ();

	vertices->v.x = x * scaleX;
	vertices->v.y = hf->getHeight (x, z);
	vertices->v.z = z * scaleZ;
	vertices->n = normals[z * rootSize + x];
	vertices++;

	for (unsigned i = 0; i < patchSize; ++i, x += step)
	{
		vertices->v.x = x * scaleX;
		vertices->v.y = hf->getHeight (x, z);
		vertices->v.z = z * scaleZ;
		vertices->n = normals[z * rootSize + x];
		vertices++;
	}

	vertices->v.x = (x - step) * scaleX;
	vertices->v.y = hf->getHeight (x - step, z);
	vertices->v.z = z * scaleZ;
	vertices->n = normals[z * rootSize + x - step];
	vertices++;

	z = 0;

	for (unsigned i = 0; i < patchSize; ++i, z += step)
	{
		x = 0;

		vertices->v.x = x * scaleX;
		vertices->v.y = hf->getHeight(x, z);
		vertices->v.z = z * scaleZ;
		vertices->n = normals[z * rootSize + x];
		vertices++;

		for (unsigned j = 0; j < patchSize; ++j, x += step)
		{
			vertices->v.x = x * scaleX;
			vertices->v.y = hf->getHeight (x, z);
			vertices->v.z = z * scaleZ;
			vertices->n = normals[z * rootSize + x];
			vertices++;
		}

		vertices->v.x = (x - step) * scaleX;
		vertices->v.y = hf->getHeight (x - step, z);
		vertices->v.z = z * scaleZ;
		vertices->n = normals[z * rootSize + x - step];
		vertices++;
	}

	x = 0;
	z -= step;

	vertices->v.x = x * scaleX;
	vertices->v.y = hf->getHeight (x, z);
	vertices->v.z = z * scaleZ;
	vertices->n = normals[z * rootSize + x];
	vertices++;

	for (unsigned i = 0; i < patchSize; ++i, x += step)
	{
		vertices->v.x = x * scaleX;
		vertices->v.y = hf->getHeight (x, z);
		vertices->v.z = z * scaleZ;
		vertices->n = normals[z * rootSize + x];
		vertices++;
	}

	vertices->v.x = (x - step) * scaleX;
	vertices->v.y = hf->getHeight (x - step, z);
	vertices->v.z = z * scaleZ;
	vertices->n = normals[z * rootSize + x - step];

	_vertices = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_NORMAL, ATOM_USAGE_STATIC, dim2 * dim2, true);
	void *p = _vertices->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	memcpy (p, v, sizeof(TerrainVertex) * dim2 * dim2);
	_vertices->unlock ();

	ATOM_DELETE_ARRAY(v);

	ATOM_VECTOR<unsigned short> indices;
	unsigned vertexCacheSize = 24;

	unsigned vstep = vertexCacheSize / 2 - 1;

	for (unsigned i = 0; i < dim2 - 1; i += vstep)
	{
		unsigned start = i;
		unsigned end = (i + vstep > dim2 - 1) ? dim2 - 1 : i + vstep;
		for (unsigned j = 0; j < dim2 - 1; ++j)
		{
			for (unsigned k = start; k <= end; ++k)
			{
				indices.push_back ((dim2 - 1 - k) * dim2 + j);
				indices.push_back ((dim2 - 1 - k) * dim2 + j + 1);
			}
			indices.push_back ((dim2 - 1 - end) * dim2 + j + 1);
			indices.push_back ((j == dim2 - 2) ? (dim2 - 1 - end) * dim2 : (dim2 - 1 - start) * dim2 + j + 1);
		}
	}
	indices.resize (indices.size() - 2);

	_indices = device->allocIndexArray (ATOM_USAGE_STATIC, indices.size(), false, true);
	void *idx = _indices->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	memcpy (idx, &indices[0], sizeof(unsigned short) * indices.size());
	_indices->unlock ();

	_vertexDecl = device->createInterleavedVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_NORMAL);

	float texScaleX = 1.f / ((rootSize - 1) * scaleX);
	float texScaleY = 1.f / ((rootSize - 1) * scaleZ);
	_scaleMatrix.makeScale (ATOM_Vector3f (texScaleX, texScaleY, 1.f));

	return true;
}

bool ATOM_TerrainRenderDatasFF::draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{
	ATOM_STACK_TRACE(ATOM_TerrainRenderDatasFF::render);

	ATOM_Matrix4x4f swapYZ;
	swapYZ.setRow (0, ATOM_Vector4f(1.f, 0.f, 0.f, 0.f));
	swapYZ.setRow (1, ATOM_Vector4f(0.f, 0.f, 1.f, 0.f));
	swapYZ.setRow (2, ATOM_Vector4f(0.f, 1.f, 0.f, 0.f));
	swapYZ.setRow (3, ATOM_Vector4f(0.f, 0.f, 0.f, 1.f));

	ATOM_Matrix4x4f matrixToObject;
	device->getTransform (ATOM_MATRIXMODE_INV_MODELVIEW, matrixToObject);

	ATOM_Matrix4x4f textureMatrix = _scaleMatrix >> swapYZ >> matrixToObject;
	device->setTextureTransform (0, textureMatrix);

	device->setVertexDecl (_vertexDecl);
	device->setStreamSource (0, _vertices.get());

	unsigned numPasses = material->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (material->beginPass (device, pass))
		{
			device->renderStreamsIndexed (_indices.get(), ATOM_PRIMITIVE_TRIANGLE_STRIP, _indices->getNumIndices() - 2);
			material->endPass (device, pass);
		}
	}
	material->end (device);

	return true;
}

