#include "stdafx.h"
#include "grass.h"
#include "heightfield.h"

#define NUM_GRASSOBJECTS_PER_DRAWCALL 1024

GrassCluster::GrassCluster (ATOM_Terrain *terrain)
{
	ATOM_STACK_TRACE(GrassCluster::GrassCluster);

	_terrain = terrain;
	_cells = 0;
	_cellWidth = 0.f;
	_cellHeight = 0.f;
	_cellDimX = 0;
	_cellDimZ = 0;
	_visibleCellStartX = 0;
	_visibleCellStartZ = 0;
	_visibleCellCountX = 0;
	_visibleCellCountZ = 0;
	_geometry = ATOM_NEW(ATOM_InterleavedStreamGeometry);
	_material = ATOM_MaterialManager::createMaterialFromCore(ATOM_GetRenderDevice(), (ATOM_Terrain::isEditorModeEnabled()?"/editor/materials/grass.mat":"/materials/builtin/grass.mat"));
}

GrassCluster::~GrassCluster (void)
{
	ATOM_STACK_TRACE(GrassCluster::~GrassCluster);

	ATOM_DELETE_ARRAY(_cells);
	ATOM_DELETE(_geometry);
}

void GrassCluster::updateSize (float width, float height)
{
	float vMin = _terrain->getGrassSizeVarianceMin();
	float vMax = _terrain->getGrassSizeVarianceMax();

	for (unsigned i = 0; i < _cellDimX * _cellDimZ; ++i)
	{
		_cells[i].w = width;
		_cells[i].h = height;
		for (unsigned n = 0; n < _cells[i].grassObjects.size(); ++n)
		{
			float r = vMin + (float(rand())/float(RAND_MAX)) * (vMax - vMin);
			float halfWidth = r * width * 0.5f;
			float halfHeight = r * height * 0.5f;

			GrassCell::GrassObject &o = _cells[i].grassObjects[n];
			o.n = o.norm * halfHeight;
			o.d[0].normalize();
			o.d[0] *= halfWidth;
			o.d[1].normalize();
			o.d[1] *= halfWidth;
			o.d[2].normalize();
			o.d[2] *= halfWidth;
		}
	}
}

void GrassCluster::updateScale (float vMin, float vMax)
{
	if (vMin > vMax)
	{
		std::swap (vMin, vMax);
	}

	for (unsigned i = 0; i < _cellDimX * _cellDimZ; ++i)
	{
		for (unsigned n = 0; n < _cells[i].grassObjects.size(); ++n)
		{
			float r = vMin + (float(rand())/float(RAND_MAX)) * (vMax - vMin);
			float halfWidth = r * _cells[i].w * 0.5f;
			float halfHeight = r * _cells[i].h * 0.5f;

			GrassCell::GrassObject &o = _cells[i].grassObjects[n];
			o.n = o.norm * halfHeight;
			o.d[0].normalize();
			o.d[0] *= halfWidth;
			o.d[1].normalize();
			o.d[1] *= halfWidth;
			o.d[2].normalize();
			o.d[2] *= halfWidth;
		}
	}
}

void GrassCluster::addLayer (const ATOM_Vector2f &tcmin, const ATOM_Vector2f &tcmax, float width, float height, unsigned num, const ATOM_Vector3f *positions, const ATOM_Vector3f *normals, float vMin, float vMax)
{
	ATOM_STACK_TRACE(GrassCluster::addLayer);

	if (vMin > vMax)
	{
		std::swap (vMin, vMax);
	}

	for (unsigned i = 0; i < num; ++i)
	{
#if 1
		addGrassObject (positions[i], width, height, vMin, vMax, tcmin, tcmax, normals[i]);
#else
		unsigned x = ATOM_ftol(ATOM_floor(positions[i].x / _cellWidth));
		unsigned z = ATOM_ftol(ATOM_floor(positions[i].z / _cellHeight));

		float r = vMin + (float(rand())/float(RAND_MAX)) * (vMax - vMin);
		float halfWidth = r * width * 0.5f;
		float halfHeight = r * height * 0.5f;

		if (x < _cellDimX && z < _cellDimZ)
		{
			ATOM_Vector3f vectorX(1, 0, 0);
			ATOM_Vector3f vectorZ = crossProduct(vectorX, normals[i]);
			vectorX = crossProduct(vectorZ, normals[i]);
			ATOM_Matrix4x4f matrixRot = ATOM_Matrix4x4f::getRotateMatrixAngleAxis (ATOM_Pi/3.f, normals[i]);

			float initialAngle = ATOM_Pi * (float(rand()) / float(RAND_MAX));
			ATOM_Vector3f x0 = ATOM_Matrix4x4f::getRotateMatrixAngleAxis (initialAngle, normals[i]) >> vectorX;
			ATOM_Vector3f x1 = matrixRot >> x0;
			ATOM_Vector3f x2 = matrixRot >> x1;

			GrassCell &cell = _cells[x+z*_cellDimX];
			cell.w = width;
			cell.h = height;
			GrassCell::GrassObject o;
			o.position = positions[i];
			o.n = normals[i] * halfHeight;
			o.norm = normals[i];
			o.d[0] = x0 * halfWidth;
			o.d[1] = x1 * halfWidth;
			o.d[2] = x2 * halfWidth;
			o.distance = 0.f;
			o.t[0] = tcmin;
			o.t[1] = tcmax;
			cell.grassObjects.push_back (o);
			cell.bbox.extend (o.position);
		}
#endif
	}
}

bool GrassCluster::initVertexArrays (void)
{
	ATOM_STACK_TRACE(GrassCluster::initVertexArrays);

	ATOM_RenderDevice *device = ATOM_GetRenderDevice ();
	ATOM_ASSERT(device);

    _vertex_array = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_NORMAL|ATOM_VERTEX_ATTRIB_TEX1_2, ATOM_USAGE_DYNAMIC, NUM_GRASSOBJECTS_PER_DRAWCALL * 12, false);
	if (!_vertex_array)
	{
		ATOM_LOGGER::error ("ERR: Initialize grass vertex array failed\n");
		return false;
	}
	_geometry->setStream (_vertex_array.get());

	_index_array = device->allocIndexArray (ATOM_USAGE_STATIC, NUM_GRASSOBJECTS_PER_DRAWCALL * 18, false, true);
	if (!_index_array)
	{
		_vertex_array = 0;
		_geometry->setStream (0);
		ATOM_LOGGER::error ("ERR: Initialize grass index array failed\n");
		return false;
	}
	_geometry->setIndices (_index_array.get());

	unsigned short *indices = (unsigned short*)_index_array->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	if (!indices)
	{
		_vertex_array = 0;
		_index_array = 0;
		ATOM_LOGGER::error ("ERR: lock grass index array failed\n");
		return false;
	}

	for (unsigned short i = 0; i < NUM_GRASSOBJECTS_PER_DRAWCALL; ++i)
	{
		unsigned short base = i * 12;

		*indices++ = base;
		*indices++ = base + 1;
		*indices++ = base + 2;
		*indices++ = base;
		*indices++ = base + 2;
		*indices++ = base + 3;

		*indices++ = base + 4;
		*indices++ = base + 5;
		*indices++ = base + 6;
		*indices++ = base + 4;
		*indices++ = base + 6;
		*indices++ = base + 7;

		*indices++ = base + 8;
		*indices++ = base + 9;
		*indices++ = base + 10;
		*indices++ = base + 8;
		*indices++ = base + 10;
		*indices++ = base + 11;
	}

	_index_array->unlock ();
	_geometry->createVertexDecl ();

	return true;
}

void GrassCluster::initialize (float cellW, float cellH, unsigned cellDimX, unsigned cellDimZ, ATOM_Texture *texture)
{
	ATOM_STACK_TRACE(GrassCluster::initialize);

	_cellDimX = cellDimX;
	_cellDimZ = cellDimZ;
	_cellWidth = cellW;
	_cellHeight = cellH;
	_cells = ATOM_NEW_ARRAY(GrassCell, _cellDimX * _cellDimZ);

	for (unsigned i = 0; i < _cellDimX * _cellDimZ; ++i)
	{
		unsigned z = i / cellDimX;
		unsigned x = i - cellDimX * z;
		_cells[i].x = cellW * (x + 0.5f);
		_cells[i].z = cellH * (z + 0.5f);
		_cells[i].bbox.beginExtend ();
	}

	_texture = texture;

	//ATOM_Vector2f *texcoords = (ATOM_Vector2f*)_texcoord_array->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	//for (unsigned i = 0; i < _vertexArraySizeTotal; ++i)
	//{
	//	texcoords++->set(1.f, 1.f);
	//	texcoords++->set(1.f, 0.f);
	//	texcoords++->set(0.f, 0.f);
	//	texcoords++->set(0.f, 1.f);

	//	texcoords++->set(1.f, 1.f);
	//	texcoords++->set(1.f, 0.f);
	//	texcoords++->set(0.f, 0.f);
	//	texcoords++->set(0.f, 1.f);

	//	texcoords++->set(1.f, 1.f);
	//	texcoords++->set(1.f, 0.f);
	//	texcoords++->set(0.f, 0.f);
	//	texcoords++->set(0.f, 1.f);
	//}
	//_texcoord_array->unlock ();
}

void GrassCluster::setVPMatrix (const ATOM_Matrix4x4f &m, bool shadowPass/*=false*/)
{
	if( shadowPass )
		_vpMatrix_shadow = m;
	else
		_vpMatrix = m;
}

bool GrassCluster::draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{
	//--- wangjian added ---//
	const char* renderSchemeName = ATOM_RenderScheme::getCurrentRenderScheme() ? ATOM_RenderScheme::getCurrentRenderScheme()->getName() : 0;
	if( !renderSchemeName )
		return false;
	bool bShadowGenPass = false;
	if( !stricmp(renderSchemeName,"shadowmap") )
		bShadowGenPass = true;
	//---------------------//

	if (ATOM_Terrain::isEditorModeEnabled ())
	{
		material->getParameterTable()->setTexture ("heightTexture", _terrain->getHeightmap());
	}

	const ATOM_Matrix4x4f &matView = camera->getViewMatrix ();
	ATOM_Vector3f cameraPos(matView.m30, matView.m31, matView.m32);
	ATOM_Vector3f pos = _terrain->getInvWorldMatrix().transformPoint (cameraPos);
	determineVisibleRegion (pos, 1024.f);

	const ATOM_Matrix4x4f &matInvView = camera->getInvViewMatrix ();
	ATOM_Vector3f viewSpaceNormal = matInvView.transformVector (ATOM_Vector3f(0.f, 1.f, 0.f));
	material->getParameterTable()->setVector("viewSpaceNormal", viewSpaceNormal);

	//device->ClearStreamSource ();
	//device->SetStreamSource (_vertex_array.get(), VERTEX_ATTRIB_COORD);
	//device->SetStreamSource (_texcoord_array.get(), VERTEX_ATTRIB_TEX1_2);

	GrassVertex *p = 0;
	static ATOM_VECTOR<GrassCell*> renderCells;
	renderCells.resize (0);
	unsigned totalSize = 0;

	for (unsigned i = 0; i < _cellDimX * _cellDimZ; ++i)
	{
		unsigned indexZ = i / _cellDimX;
		unsigned indexX = i - _cellDimX * indexZ;
		if (indexX < _visibleCellStartX || indexX >= _visibleCellStartX + _visibleCellCountX ||
			indexZ < _visibleCellStartZ || indexZ >= _visibleCellStartZ + _visibleCellCountZ)
		{
			continue;
		}

		GrassCell &cell = _cells[i];
		//--- wangjian modified ---//
		ATOM_BBox::ClipState cs = 
			ATOM_Terrain::isEditorModeEnabled() 
			? cell.bbox.getClipStateMask ( bShadowGenPass ? _vpMatrix_shadow : _vpMatrix, ATOM_BBox::ClipLeft|ATOM_BBox::ClipRight|ATOM_BBox::ClipFar|ATOM_BBox::ClipNear) 
			: cell.bbox.getClipState (bShadowGenPass ? _vpMatrix_shadow : _vpMatrix);
		//-------------------------//
		if (cs == ATOM_BBox::ClipOutside)
		{
			continue;
		}

		renderCells.push_back (&cell);

		//drawCell (device, &cell, p1, p2, cell.sorter.getIndices());
	}

	for (unsigned i = 0; i < renderCells.size(); ++i)
	{
		drawCell (device, material, renderCells[i], p, totalSize);
	}

	if (p)
	{
		flush (device, material, totalSize);
	}

	return true;
}

void GrassCluster::determineVisibleRegion (const ATOM_Vector3f &pos, float maxDistance)
{
	int maxX = ATOM_ftol(ATOM_ceil((pos.x + maxDistance) / _cellWidth));
	int maxZ = ATOM_ftol(ATOM_ceil((pos.z + maxDistance) / _cellHeight));
	int minX = ATOM_ftol(ATOM_floor((pos.x - maxDistance) / _cellWidth));
	int minZ = ATOM_ftol(ATOM_floor((pos.z - maxDistance) / _cellHeight));

	if (minX < 0) minX = 0;
	if (minZ < 0) minZ = 0;
	if (maxX >= _cellDimX) maxX = _cellDimX - 1;
	if (maxZ >= _cellDimZ) maxZ = _cellDimZ - 1;

	_visibleCellStartX = (unsigned)minX;
	_visibleCellStartZ = (unsigned)minZ;
	_visibleCellCountX = (minX > maxX) ? 0 : maxX - minX + 1;
	_visibleCellCountZ = (minZ > maxZ) ? 0 : maxZ - minZ + 1;
}

void GrassCluster::flush (ATOM_RenderDevice *device, ATOM_Material *material, unsigned numVerts)
{
	ATOM_STACK_TRACE(GrassCluster::flush);

	_vertex_array->unlock ();

	if (numVerts > 0)
	{
		unsigned numPasses = material->begin (device);
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (material->beginPass (device, pass))
			{
				_geometry->draw (device, ATOM_PRIMITIVE_TRIANGLES, numVerts/2, 0);
				material->endPass (device, pass);
			}
		}
		material->end (device);
	}
	//device->RenderStreamsIndexed (_index_array.get(), ATOM_PRIMITIVE_TRIANGLES, (_vertexArraySizeTotal - _vertexArraySize) * 6, 0);
}

void GrassCluster::drawCell (ATOM_RenderDevice *device, ATOM_Material *material, const GrassCell *cell, GrassVertex *&p, unsigned &totalSize)
{
	ATOM_STACK_TRACE(GrassCluster::drawCell);

	if (!p)
	{
		p = (GrassVertex*)_vertex_array->lock (ATOM_LOCK_WRITEONLY, 0, _vertex_array->getNumVertices() * sizeof(GrassVertex), true);
		if (!p)
		{
			return;
		}
		totalSize = 0;
	}

	GrassVertex *verts = p;

	for (unsigned i = 0; i < cell->grassObjects.size(); ++i)
	{
		if (_vertex_array->getNumVertices() < 12 + totalSize)
		{
			flush (device, material, totalSize);
			p = (GrassVertex*)_vertex_array->lock (ATOM_LOCK_WRITEONLY, 0, _vertex_array->getNumVertices() * sizeof(GrassVertex), true);
			if (!p)
			{
				return;
			}
			verts = p;
			totalSize = 0;
		}

		const GrassCell::GrassObject &o = cell->grassObjects[i];
		float x0 = o.t[0].x;
		float x1 = o.t[1].x;
		float y0 = o.t[0].y;
		float y1 = o.t[1].y;

		verts->pos = o.position + o.d[0];
		verts->normal = o.norm;
		verts->texcoord.set(x1, y1);
		verts++;

		verts->pos = o.position + o.d[0] + o.n;
		verts->normal = o.norm;
		verts->texcoord.set(x1, y0);
		verts++;

		verts->pos = o.position - o.d[0] + o.n;
		verts->normal = o.norm;
		verts->texcoord.set(x0, y0);
		verts++;

		verts->pos = o.position - o.d[0];
		verts->normal = o.norm;
		verts->texcoord.set(x0, y1);
		verts++;

		verts->pos = o.position + o.d[1];
		verts->normal = o.norm;
		verts->texcoord.set(x1, y1);
		verts++;

		verts->pos = o.position + o.d[1] + o.n;
		verts->normal = o.norm;
		verts->texcoord.set(x1, y0);
		verts++;

		verts->pos = o.position - o.d[1] + o.n;
		verts->normal = o.norm;
		verts->texcoord.set(x0, y0);
		verts++;

		verts->pos = o.position - o.d[1];
		verts->normal = o.norm;
		verts->texcoord.set(x0, y1);
		verts++;

		verts->pos = o.position + o.d[2];
		verts->normal = o.norm;
		verts->texcoord.set(x1, y1);
		verts++;

		verts->pos = o.position + o.d[2] + o.n;
		verts->normal = o.norm;
		verts->texcoord.set(x1, y0);
		verts++;

		verts->pos = o.position - o.d[2] + o.n;
		verts->normal = o.norm;
		verts->texcoord.set(x0, y0);
		verts++;

		verts->pos = o.position - o.d[2];
		verts->normal = o.norm;
		verts->texcoord.set(x0, y1);
		verts++;

		totalSize += 12;
	}
	p = verts;
}

ATOM_Material *GrassCluster::getMaterial (void) const
{
	return _material.get();
}

void GrassCluster::removeRandomGrassObject (const ATOM_Vector3f &pos, float radiusX, float radiusZ, int count)
{
	static ATOM_VECTOR<int> vCells;

	vCells.resize (0);

	float xMin = pos.x - radiusX;
	float xMax = pos.x + radiusX;
	float zMin = pos.z - radiusZ;
	float zMax = pos.z + radiusZ;

	for (int i = 0; i < _cellDimX; ++i)
		for (int j = 0; j < _cellDimZ; ++j)
		{
			GrassCell &cell = _cells[i+j*_cellDimX];
			float xMinCell = cell.bbox.getMin().x;
			float xMaxCell = cell.bbox.getMax().x;
			float zMinCell = cell.bbox.getMin().z;
			float zMaxCell = cell.bbox.getMax().z;

			if (xMin >= xMaxCell || xMax <= xMinCell || zMin >= zMaxCell || zMax <= zMinCell)
				continue;

			vCells.push_back (i+j*_cellDimX);
		}

	if (vCells.empty ())
	{
		return;
	}

	for (int i = 0; i < count; ++i)
	{
		bool got = false;
		for (int j = 0; j < vCells.size(); ++j)
		{
			GrassCell &cell = _cells[vCells[j]];
			for (int k = 0; k < cell.grassObjects.size(); ++k)
			{
				const GrassCell::GrassObject &o = cell.grassObjects[k];
				if (o.position.x >= xMin && o.position.x <= xMax && o.position.z >= zMin && o.position.z <= zMax)
				{
					cell.grassObjects.erase (cell.grassObjects.begin() + k);
					got = true;
					break;
				}
			}
		}
		if (!got)
		{
			break;
		}
	}
}

void GrassCluster::addGrassObject (const ATOM_Vector3f &pos, float width, float height, float vMin, float vMax, const ATOM_Vector2f &tcMin, const ATOM_Vector2f &tcMax, const ATOM_Vector3f &normal)
{
	unsigned x = ATOM_ftol(ATOM_floor(pos.x / _cellWidth));
	unsigned z = ATOM_ftol(ATOM_floor(pos.z / _cellHeight));

	float r = vMin + (float(rand())/float(RAND_MAX)) * (vMax - vMin);
	float halfWidth = r * width * 0.5f;
	float halfHeight = r * height * 0.5f;

	if (x < _cellDimX && z < _cellDimZ)
	{
		ATOM_Vector3f vectorX(1, 0, 0);
		ATOM_Vector3f vectorZ = crossProduct(vectorX, normal);
		vectorX = crossProduct(vectorZ, normal);
		ATOM_Matrix4x4f matrixRot = ATOM_Matrix4x4f::getRotateMatrixAngleAxis (ATOM_Pi/3.f, normal);

		float initialAngle = ATOM_Pi * (float(rand()) / float(RAND_MAX));
		ATOM_Vector3f x0 = ATOM_Matrix4x4f::getRotateMatrixAngleAxis (initialAngle, normal) >> vectorX;
		ATOM_Vector3f x1 = matrixRot >> x0;
		ATOM_Vector3f x2 = matrixRot >> x1;

		GrassCell &cell = _cells[x+z*_cellDimX];
		cell.w = width;
		cell.h = height;
		GrassCell::GrassObject o;
		o.position = pos;
		o.n = normal * halfHeight;
		o.norm = normal;
		o.d[0] = x0 * halfWidth;
		o.d[1] = x1 * halfWidth;
		o.d[2] = x2 * halfWidth;
		o.distance = 0.f;
		o.t[0] = tcMin;
		o.t[1] = tcMax;
		cell.grassObjects.push_back (o);
		cell.bbox.extend (o.position);
	}
}

void GrassCluster::getGrassPositions (ATOM_VECTOR<ATOM_Vector3f> &v) const
{
	v.resize (0);
	for (unsigned i = 0; i < _cellDimX * _cellDimZ; ++i)
	{
		const GrassCell &cell = _cells[i];
		for (unsigned n = 0; n < cell.grassObjects.size(); ++n)
		{
			ATOM_Vector3f p = cell.grassObjects[n].position;
			p.y = _terrain->getHeightField()->getRealHeight (p.x, p.z);
			v.push_back (p);
		}
	}
}

