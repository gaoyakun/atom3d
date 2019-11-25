#include "StdAfx.h"
#include "shape.h"

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_ShapeNode)
	ATOM_ATTRIBUTES_BEGIN(ATOM_ShapeNode)
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_ShapeNode, "Type", getType, setType, int(ATOM_ShapeNode::SPHERE), "type=int32;enum='平面 0 球 1 圆柱 2 圆锥 3 公告板 4 网格 6 模型 7';group=ATOM_ShapeNode;desc='类型'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_ShapeNode, "ModelFile", getModelFileName, setModelFileName, "", "group=ATOM_ShapeNode;type=vfilename;desc='模型文件名'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_ShapeNode, "Smoothness", getSmoothness, setSmoothness, 20, "type=int32;group=ATOM_ShapeNode;desc='曲面平滑度'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_ShapeNode, "DivisionX", getDivisionX, setDivisionX, 1, "type=int32;group=ATOM_ShapeNode;desc='平面镶嵌X密度'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_ShapeNode, "DivisionY", getDivisionY, setDivisionY, 1, "type=int32;group=ATOM_ShapeNode;desc='平面镶嵌Y密度'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_ShapeNode, "WithCaps", getWithCaps, setWithCaps, 1, "type=bool;group=ATOM_ShapeNode;desc='封盖'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_ShapeNode, "VisiblePart", getVisiblePart, setVisiblePart, 20, "type=int32;group=ATOM_ShapeNode;desc='可视部分'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_ShapeNode, "Transparency", getTransparency, setTransparency, 1.f, "group=ATOM_ShapeNode;desc='透明度'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_ShapeNode, "Shearing", getShearing, setShearing, 0.f, "group=ATOM_ShapeNode;desc='倾斜度'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_ShapeNode, "ShapeSize", getSize, setSize, ATOM_Vector3f(1.f, 1.f, 1.f), "group=ATOM_ShapeNode;desc='大小'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_ShapeNode, "Material", getMaterialFileName, setMaterialFileName, "/materials/builtin/shape.mat", "type=vfilename;desc='材质文件名';group=ATOM_ShapeNode")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_ShapeNode, "ShapeColor", getColor, setColor, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), "type=rgba;desc='颜色';group=ATOM_ShapeNode")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_ShapeNode, "UVRotation", getUVRotation, setUVRotation, 0.f, "desc='UV旋转角';group=ATOM_ShapeNode")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_ShapeNode, "UVScaleOffset", getUVScaleOffset, setUVScaleOffset, ATOM_Vector4f(1.f, 1.f, 0.f, 0.f), "desc='UV缩放平移';group=ATOM_ShapeNode")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_ShapeNode, "ColorMultiplier", getColorMultiplier, setColorMultiplier, 1.f, "group=ATOM_ShapeNode;desc='颜色强度因子'")	// wangjian added
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_ShapeNode, "ResolveScale", getResolveScale, setResolveScale, 0.f, "group=ATOM_ShapeNode;desc='溶解强度因子'")			// wangjian added
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_ShapeNode, ATOM_VisualNode)

ATOM_VertexDecl ATOM_ShapeNode::_vertexDecl = nullptr;

static void setupGridVertices (float w, float h, int rx, int ry, const ATOM_Vector3f &center, unsigned indexOffset, ATOM_VECTOR<ATOM_Vector3f> &vertices, ATOM_VECTOR<unsigned> &indices)
{
	vertices.resize (rx * ry);
	indices.reserve ((ry - 1) * 2 * (rx + 1));

	float step_x = w / (rx - 1);
	float step_y = h / (ry - 1);

	for (unsigned i = 0; i < rx; ++i)
	{
		for (unsigned j = 0; j < ry; ++j)
		{
			vertices[i+rx*j].set (center.x - 0.5f * w + step_x * i, center.y, center.z - 0.5f * h + step_y * j);
		}
	}

	for (unsigned i = 0; i < ry; ++i)
	{
		for (unsigned j = 0; j < rx - 1; ++j)
		{
			indices.push_back (i * rx + j + indexOffset);
			indices.push_back (i * rx + j + 1 + indexOffset);
		}
	}

	for (unsigned i = 0; i < rx; ++i)
	{
		for (unsigned j = 0; j < ry - 1; ++j)
		{
			indices.push_back (j * rx + i + indexOffset);
			indices.push_back ((j + 1) * rx + i + indexOffset);
		}
	}
}

static void setupPlaneVertices (float w, float h, int rx, int ry, const ATOM_Vector3f &center, unsigned indexOffset, ATOM_VECTOR<ATOM_Vector3f> &vertices, ATOM_VECTOR<unsigned> &indices)
{
	vertices.resize (rx * ry);
	indices.reserve ((ry - 1) * 2 * (rx + 1));

	float step_x = w / (rx - 1);
	float step_y = h / (ry - 1);

	for (unsigned i = 0; i < rx; ++i)
	{
		for (unsigned j = 0; j < ry; ++j)
		{
			vertices[i+rx*j].set (center.x - 0.5f * w + step_x * i, center.y, center.z - 0.5f * h + step_y * j);
		}
	}

#if 1
	unsigned vertexCacheSize = 24;
	unsigned vstep = vertexCacheSize / 2 - 1;

	for (unsigned i = 0; i < ry - 1; i += vstep)
	{
		unsigned start = i;
		unsigned end = (i + vstep > ry - 1) ? ry - 1 : i + vstep;
		for (unsigned j = 0; j < rx - 1; ++j)
		{
			for (unsigned k = start; k <= end; ++k)
			{
				indices.push_back ((ry - 1 - k) * rx + j);
				indices.push_back ((ry - 1 - k) * rx + j + 1);
			}
			indices.push_back ((ry - 1 - end) * rx + j + 1);
			indices.push_back ((j == rx - 2) ? (ry - 1 - end) * rx : (ry - 1 - start) * rx + j + 1);
		}
	}
	indices.resize (indices.size() - 2);
#else
		for (unsigned i = 0; i < ry-1; ++i)
		{
			if (i != 0)
			{
				indices.push_back (i * rx + indexOffset);
			}

			for (unsigned j = 0; j < rx; ++j)
			{
				indices.push_back (i * rx + j + indexOffset);
				indices.push_back ((i+1) * rx + j + indexOffset);
			}

			if (i != ry-2)
			{
				indices.push_back ((i+1) * rx + rx - 1 + indexOffset);
			}
		}
#endif
}

static void CreatePlane (const ATOM_Vector3f &center, float width, float height, int resolutionX, int resolutionY, ATOM_Vector3f *vertexbuffer, ATOM_Vector3f *normals, ATOM_Vector2f *uv, unsigned *indexbuffer, unsigned *numVertices, unsigned *numIndices, unsigned indexOffset)
{
#if 1
	ATOM_VECTOR<ATOM_Vector3f> v;
	ATOM_VECTOR<unsigned> i;
	setupPlaneVertices (width, height, resolutionX, resolutionY, center, indexOffset, v, i);

	if (numVertices)
	{
		*numVertices = v.size();
	}

	if (numIndices)
	{
		*numIndices = i.size();
	}

	if (vertexbuffer)
	{
		memcpy (vertexbuffer, &v[0], v.size() * sizeof(ATOM_Vector3f));
	}

	if (indexbuffer)
	{
		memcpy (indexbuffer, &i[0], i.size() * sizeof(unsigned));
	}

	if (uv)
	{
		float u_step = 1.f / (resolutionX - 1);
		float v_step = 1.f / (resolutionY - 1);

		for (unsigned i = 0; i < resolutionX; ++i)
			for (unsigned j = 0; j < resolutionY; ++j)
			{
				uv[j * resolutionX + i].set (i * u_step, j * v_step);
			}
	}

	if (normals)
	{
		for (unsigned i = 0; i < resolutionX * resolutionY; ++i)
		{
			normals[i].set (0.f, 1.f, 0.f);
		}
	}
#else
	*numVertices = 4;
	*numIndices = 6;
	float halfWidth = width * 0.5f;
	float halfHeight = height * 0.5f;

	if (vertexbuffer)
	{
		*vertexbuffer++ = ATOM_Vector3f(center.x - halfWidth, center.y, center.z - halfHeight);
		*vertexbuffer++ = ATOM_Vector3f(center.x - halfWidth, center.y, center.z + halfHeight);
		*vertexbuffer++ = ATOM_Vector3f(center.x + halfWidth, center.y, center.z + halfHeight);
		*vertexbuffer++ = ATOM_Vector3f(center.x + halfWidth, center.y, center.z - halfHeight);
	}

	if (normals)
	{
		*normals++ = ATOM_Vector3f(0.f, 1.f, 0.f);
		*normals++ = ATOM_Vector3f(0.f, 1.f, 0.f);
		*normals++ = ATOM_Vector3f(0.f, 1.f, 0.f);
		*normals++ = ATOM_Vector3f(0.f, 1.f, 0.f);
	}

	if (uv)
	{
		*uv++ = ATOM_Vector2f(0.f, 0.f);
		*uv++ = ATOM_Vector2f(0.f, 1.f);
		*uv++ = ATOM_Vector2f(1.f, 1.f);
		*uv++ = ATOM_Vector2f(1.f, 0.f);
	}

	if (indexbuffer)
	{
		*indexbuffer++ = indexOffset;
		*indexbuffer++ = indexOffset + 1;
		*indexbuffer++ = indexOffset + 2;
		*indexbuffer++ = indexOffset;
		*indexbuffer++ = indexOffset + 2;
		*indexbuffer++ = indexOffset + 3;
	}
#endif
}

static void CreateGrid (const ATOM_Vector3f &center, float width, float height, int resolutionX, int resolutionY, ATOM_Vector3f *vertexbuffer, ATOM_Vector3f *normals, ATOM_Vector2f *uv, unsigned *indexbuffer, unsigned *numVertices, unsigned *numIndices, unsigned indexOffset)
{
#if 1
	ATOM_VECTOR<ATOM_Vector3f> v;
	ATOM_VECTOR<unsigned> i;
	setupGridVertices (width, height, resolutionX, resolutionY, center, indexOffset, v, i);

	if (numVertices)
	{
		*numVertices = v.size();
	}

	if (numIndices)
	{
		*numIndices = i.size();
	}

	if (vertexbuffer)
	{
		memcpy (vertexbuffer, &v[0], v.size() * sizeof(ATOM_Vector3f));
	}

	if (indexbuffer)
	{
		memcpy (indexbuffer, &i[0], i.size() * sizeof(unsigned));
	}

	if (uv)
	{
		float u_step = 1.f / (resolutionX - 1);
		float v_step = 1.f / (resolutionY - 1);

		for (unsigned i = 0; i < resolutionX; ++i)
			for (unsigned j = 0; j < resolutionY; ++j)
			{
				uv[j * resolutionX + i].set (i * u_step, j * v_step);
			}
	}

	if (normals)
	{
		for (unsigned i = 0; i < resolutionX * resolutionY; ++i)
		{
			normals[i].set (0.f, 1.f, 0.f);
		}
	}
#else
	*numVertices = 4;
	*numIndices = 6;
	float halfWidth = width * 0.5f;
	float halfHeight = height * 0.5f;

	if (vertexbuffer)
	{
		*vertexbuffer++ = ATOM_Vector3f(center.x - halfWidth, center.y, center.z - halfHeight);
		*vertexbuffer++ = ATOM_Vector3f(center.x - halfWidth, center.y, center.z + halfHeight);
		*vertexbuffer++ = ATOM_Vector3f(center.x + halfWidth, center.y, center.z + halfHeight);
		*vertexbuffer++ = ATOM_Vector3f(center.x + halfWidth, center.y, center.z - halfHeight);
	}

	if (normals)
	{
		*normals++ = ATOM_Vector3f(0.f, 1.f, 0.f);
		*normals++ = ATOM_Vector3f(0.f, 1.f, 0.f);
		*normals++ = ATOM_Vector3f(0.f, 1.f, 0.f);
		*normals++ = ATOM_Vector3f(0.f, 1.f, 0.f);
	}

	if (uv)
	{
		*uv++ = ATOM_Vector2f(0.f, 0.f);
		*uv++ = ATOM_Vector2f(0.f, 1.f);
		*uv++ = ATOM_Vector2f(1.f, 1.f);
		*uv++ = ATOM_Vector2f(1.f, 0.f);
	}

	if (indexbuffer)
	{
		*indexbuffer++ = indexOffset;
		*indexbuffer++ = indexOffset + 1;
		*indexbuffer++ = indexOffset + 2;
		*indexbuffer++ = indexOffset;
		*indexbuffer++ = indexOffset + 2;
		*indexbuffer++ = indexOffset + 3;
	}
#endif
}

static void CreateSphere (const ATOM_Vector3f &center, int numRings, int numSegments, float radius, ATOM_Vector3f *vertexbuffer, ATOM_Vector3f *normals, ATOM_Vector2f *uv, unsigned *indexbuffer, unsigned *numVertices, unsigned *numIndices, unsigned indexOffset)
{
	ATOM_STACK_TRACE(createSphere);
	if (numRings < 2)
	{
		numRings = 2;
	}

	if (numSegments < 3)
	{
		numSegments = 3;
	}

	if (numVertices)
	{
		*numVertices = (numRings + 1) * (numSegments + 1);
	}

	if (numIndices)
	{
		*numIndices = 2 * numRings * (numSegments + 1);
	}

	if (!vertexbuffer && !indexbuffer)
	{
		return;
	}

	float fDeltaRingAngle = ( ATOM_Pi / numRings );
	float fDeltaSegAngle = ( ATOM_TwoPi / numSegments );

	unsigned wVerticeIndex = 0 ;

	// Generate the group of rings for the sphere
	for( int ring = 0; ring < numRings + 1 ; ring++ )
	{
		float r0, y0;
		ATOM_sincos (ring * fDeltaRingAngle, &r0, &y0);

		// Generate the group of segments for the current ring
		for( int seg = 0; seg < numSegments + 1 ; seg++ )
		{
			float x0, z0;
			ATOM_sincos (seg * fDeltaSegAngle, &x0, &z0);
			x0 *= r0;
			z0 *= r0;

			// Add one vertices to the strip which makes up the sphere
			if (vertexbuffer)
			{
				vertexbuffer->x = center.x + x0 * radius;
				vertexbuffer->y = center.y + y0 * radius;
				vertexbuffer->z = center.z + z0 * radius;
				vertexbuffer++;
			}

			if (normals)
			{
				normals->x = x0;
				normals->y = y0;
				normals->z = z0;
				normals++;
			}

			if (uv)
			{
				uv->x = float(ring)/float(numRings);
				uv->y = float(seg)/float(numSegments);
				uv++;
			}

			// add two indices except for last ring
			if (indexbuffer && ring != numRings )
			{
				*indexbuffer++ = wVerticeIndex + indexOffset;
				*indexbuffer++ = wVerticeIndex + (unsigned)(numSegments + 1) + indexOffset;
				wVerticeIndex++ ;
			}
		} // end for seg
	} // end for ring
}

static void CreateCylinder (const ATOM_Vector3f &center, bool withCaps, int smoothLevel, const ATOM_Vector3f &axis, float radius, float height, ATOM_Vector3f *vertexbuffer, ATOM_Vector2f *uv, unsigned *indexbuffer, unsigned *numVertices, unsigned *numIndices, unsigned indexOffset)
{
	ATOM_STACK_TRACE(createCylinder);
	if (smoothLevel < 3)
	{
		smoothLevel = 3;
	}
	unsigned numVerts = (smoothLevel + 1) * 2;
	unsigned numTris = smoothLevel * 2;
	if (withCaps)
	{
		numTris += 2 * (smoothLevel - 2);
	}

	if (numVertices)
	{
		*numVertices = numVerts;
	}

	if (numIndices)
	{
		*numIndices = numTris * 3;
	}

	if (!vertexbuffer && !indexbuffer)
	{
		return;
	}

	float theta = ATOM_TwoPi / float(smoothLevel);

	ATOM_Vector3f axisY = axis;
	axisY.normalize ();

	ATOM_Vector3f temp(0.f, 0.f, 0.f);
	int idx = 0;
	if (ATOM_abs(axisY.xyz[1]) < ATOM_abs(axisY.xyz[idx]))
	{
		idx = 1;
	}
	if (ATOM_abs(axisY.xyz[2]) < ATOM_abs(axisY.xyz[idx]))
	{
		idx = 2;
	}
	temp.xyz[idx] = 1.f;

	ATOM_Vector3f axisX = crossProduct (axisY, temp);

	if (vertexbuffer || uv)
	{
		for (unsigned i = 0; i <= smoothLevel; ++i)
		{
			ATOM_Matrix4x4f rotMatrix = ATOM_Matrix4x4f::getRotateMatrixAngleNormalizedAxis (-(i * theta), axisY);
			ATOM_Vector3f point = rotMatrix >> axisX;
			point *= radius;

			if (vertexbuffer)
			{
				*vertexbuffer++ = center + point;
				*vertexbuffer++ = center + point + axisY * height;
			}

			if (uv)
			{
				*uv++ = ATOM_Vector2f((float)i/(float)smoothLevel, 0.f);
				*uv++ = ATOM_Vector2f((float)i/(float)smoothLevel, 1.f);
			}
		}
	}

	if (indexbuffer)
	{
		for (unsigned i = 0; i < smoothLevel * 2; ++i)
		{
			*indexbuffer++ = (((i + 1) / 2) * 2) + indexOffset;
			*indexbuffer++ = ((i / 2) * 2) + 1 + indexOffset;
			*indexbuffer++ = (i + 2) + indexOffset;
		}

		if (withCaps)
		{
			for (unsigned i = 0; i < smoothLevel - 2; ++i)
			{
				*indexbuffer++ = 0 + indexOffset;
				*indexbuffer++ = (i + 1) * 2 + indexOffset;
				*indexbuffer++ = (i + 2) * 2 + indexOffset;
			}
			for (unsigned i = 0; i < smoothLevel - 2; ++i)
			{
				*indexbuffer++ = (i + 1) * 2 + 1 + indexOffset;
				*indexbuffer++ = 1 + indexOffset;
				*indexbuffer++ = (i + 2) * 2 + 1 + indexOffset;
			}
		}
	}
}

static void CreateCone (const ATOM_Vector3f &center, bool withCap, int smoothLevel, const ATOM_Vector3f &axis, float radius, float height, ATOM_Vector3f *vertexbuffer, ATOM_Vector2f *uv, unsigned *indexbuffer, unsigned *numVertices, unsigned *numIndices, unsigned indexOffset)
{
	ATOM_STACK_TRACE(CreateCone);
	if (smoothLevel < 3)
	{
		smoothLevel = 3;
	}

	unsigned numVerts = smoothLevel * 2;
	unsigned numTris = smoothLevel;
	if (withCap)
	{
		numTris += smoothLevel - 2;
	}

	if (numVertices)
	{
		*numVertices = numVerts;
	}
	if (numIndices)
	{
		*numIndices = 3 * numTris;
	}

	if (!vertexbuffer && !indexbuffer && !uv)
	{
		return;
	}

	float theta = ATOM_TwoPi / float(smoothLevel);

	ATOM_Vector3f axisY = axis;
	axisY.normalize ();

	ATOM_Vector3f temp(0.f, 0.f, 0.f);
	int idx = 0;
	if (ATOM_abs(axisY.xyz[1]) < ATOM_abs(axisY.xyz[idx]))
	{
		idx = 1;
	}
	if (ATOM_abs(axisY.xyz[2]) < ATOM_abs(axisY.xyz[idx]))
	{
		idx = 2;
	}
	temp.xyz[idx] = 1.f;

	ATOM_Vector3f axisX = crossProduct (axisY, temp);

	if (vertexbuffer || uv)
	{
		for (unsigned i = 0; i < smoothLevel; ++i)
		{
			ATOM_Matrix4x4f rotMatrix = ATOM_Matrix4x4f::getRotateMatrixAngleNormalizedAxis ((i * theta), axisY);
			ATOM_Vector3f point = rotMatrix >> axisX;
			point *= radius;

			if (vertexbuffer)
			{
				*vertexbuffer++ = center + point;
			}

			if (uv)
			{
				*uv++ = ATOM_Vector2f(0.f, 0.f);
			}
		}
		for (unsigned i = 0; i < smoothLevel; ++i)
		{
			if (vertexbuffer)
			{
				*vertexbuffer++ = center + axisY * height;
			}

			if (uv)
			{
				*uv++ = ATOM_Vector2f(0.f, 0.f);
			}
		}
	}

	if (indexbuffer)
	{
		for (unsigned i = 0; i < smoothLevel; ++i)
		{
			*indexbuffer++ = smoothLevel + indexOffset + i;
			*indexbuffer++ = i + indexOffset;
			*indexbuffer++ = ((i + 1) % smoothLevel) + indexOffset;
		}

		if (withCap)
		{
			for (unsigned i = 0; i < smoothLevel - 2; ++i)
			{
				*indexbuffer++ = i + 1 + indexOffset;
				*indexbuffer++ = 0 + indexOffset;
				*indexbuffer++ = ((i + 2) % smoothLevel) + indexOffset;
			}
		}
	}
}

ATOM_ShapeNode::ATOM_ShapeNode (void)
{
	ATOM_STACK_TRACE(ATOM_ShapeNode::ATOM_ShapeNode);
	_type = SPHERE;
	_typeDirty = true;
	_withCaps = 1;
	_smoothness = 20;
	_divideX = 1;
	_divideY = 1;
	_visiblePart = 20;
	_transparency = 1.f;
	_shearing = 0.f;
	_size.set(1.f, 1.f, 1.f);
	_aabbTree = 0;
	_uvRotation = 0.f;
	_uvScaleOffset.set(1.f, 1.f, 0.f, 0.f);

	_colorMultiplier = 1.0f;					// wangjian added
	_resolveFactor	= 1.0f;						// wangjian added

	_hTransparency = 0;
	_hDiffuseColor = 0;
	_hUVRotation = 0;
	_hUVScaleOffset = 0;
	_hProjectable = 0;
	_hColorMultiplier = 0;
	_hTransparency = 0;
	_hResolveFactor = 0;						// wangjian added

#if 0
	_hDiffuseTexture = 0;	// wangjian added
	_hPerturbTexture = 0;	// wangjian added
	_hPerturbMaskTex = 0;	// wangjian added
	_hDisolveTexture = 0;	// wangjian added
#endif
}

ATOM_ShapeNode::~ATOM_ShapeNode (void)
{
	ATOM_STACK_TRACE(ATOM_ShapeNode::~ATOM_ShapeNode);
	ATOM_DELETE(_aabbTree);
}

void ATOM_ShapeNode::buildBoundingbox (void) const
{
	if (_type == BILLBOARD)
	{
		float maxSize = ATOM_max3(ATOM_abs(_size.x), ATOM_abs(_size.y), ATOM_abs(_size.z));
		_boundingBox.setMin (maxSize*0.5f);
		_boundingBox.setMax (-maxSize*0.5f);
	}
	else if (_type == PLANE || _type == GRID)
	{
		_boundingBox.setMin (ATOM_Vector3f(-_size.x, 0.f, -_size.z));
		_boundingBox.setMax (ATOM_Vector3f(_size.x, 0.f, _size.z));
	}
	else if (_type == MODEL && _model)
	{
		_boundingBox = _model->getBoundingbox ();
		_boundingBox.setMin(_boundingBox.getMin() * _size);
		_boundingBox.setMax(_boundingBox.getMax() * _size);
	}
	else
	{
		_boundingBox.setMin (-_size);
		_boundingBox.setMax (_size);
	}
}

void ATOM_ShapeNode::accept (ATOM_Visitor &visitor)
{
	visitor.visit (*this);
}

//--- wangjian added ---//
void ATOM_ShapeNode::prepareForSort(ATOM_RenderScheme* renderScheme)
{
	ATOM_Drawable::prepareForSort(renderScheme);

	//-----------------------------------------------------------------------------------------------//
	/*_material->getParameterTable()->setFloat("transparency", _transparency);
	_material->getParameterTable()->setVector("diffuseColor", _color);
	_material->getParameterTable()->setFloat("uvRotation", _uvRotation * (ATOM_Pi/180.f));
	_material->getParameterTable()->setVector("uvScaleOffset", _uvScaleOffset);*/
	if (_hProjectable)
	{
		_material->getParameterTable()->setInt(_hProjectable, getProjectable()==ATOM_Node::PROJECTABLE);
	}
	//-----------------------------------------------------------------------------------------------//
}
// 设置绘制排序标记
void ATOM_ShapeNode::setupSortFlag(int queueid)
{
	////-----------------------------------------------------------------------------------------------//
	//_material->getParameterTable()->setFloat("transparency", _transparency);
	//_material->getParameterTable()->setVector("diffuseColor", _color);
	//_material->getParameterTable()->setFloat("uvRotation", _uvRotation * (ATOM_Pi/180.f));
	//_material->getParameterTable()->setVector("uvScaleOffset", _uvScaleOffset);
	//_material->getParameterTable()->setInt("projectable", getProjectable()==ATOM_Node::PROJECTABLE);
	////-----------------------------------------------------------------------------------------------//

	//_material->autoSetActiveEffect (queueid);

	unsigned sf_ac = 0;
	unsigned sf_mt = 0;
	unsigned sf_matid = 0;

#if 0
	ULONGLONG matFlag = (ULONGLONG)_material->getActiveEffect();
	ULONGLONG miscFlag = 0;
	if (_type == MODEL)
	{
		if( _model )
			miscFlag = (ULONGLONG)_model.get();
	}
	ULONGLONG sf = ( matFlag << 32 ) + miscFlag;
#else
	//unsigned matFlag = (unsigned)_material->getActiveEffect();
	/*int effectid = getEffectIdEnd(ATOM_RenderScheme::getCurrentRenderScheme());
	ATOM_ASSERT(effectid!=-1);
	unsigned matFlag = (unsigned)_material->getCoreMaterial()->getEffect(effectid);
	unsigned miscFlag = 0;
	if (_type == MODEL)
	{
		if( _model )
			miscFlag = (unsigned)_model.get();
	}
	matFlag <<= 16;
	matFlag /= 100000;
	matFlag *= 100000;
	unsigned sf = matFlag + ( miscFlag & 0x0000ffff );*/

	int effectid = getEffectIdEnd(ATOM_RenderScheme::getCurrentRenderScheme());
	ATOM_ASSERT(effectid>=0);
	sf_ac = (unsigned)_material->getCoreMaterial()->getEffect(effectid);
	if (_type == MODEL && _model)
	{
		sf_mt = (unsigned)_model.get();

#if 0
		ATOM_Texture * diffuseTex		= (ATOM_Texture *)_hDiffuseTexture->t;
		ATOM_Texture * perturbTexture	= (ATOM_Texture *)_hPerturbTexture->t;
		ATOM_Texture * perturbMaskTex	= (ATOM_Texture *)_hPerturbMaskTex->t;
		ATOM_Texture * disolveTexture	= (ATOM_Texture *)_hDisolveTexture->t;

		sf_mt |= (unsigned)diffuseTex;
		sf_mt |= (unsigned)perturbTexture;
		sf_mt |= (unsigned)perturbMaskTex;
		sf_mt |= (unsigned)disolveTexture;
#endif
	}

#endif

	//setSortFlag(sf);
	setSortFlag(sf_ac,sf_mt,sf_matid);
}

#if 0
// 对于model类型的SHAPE节点 使用实例化渲染（如果开启实例化)
bool ATOM_ShapeNode::drawBatch(	ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material, 
								bool bInstancing/* = true*/, sInstancing * instancing/* = 0*/	);
{
	if( !instancing )
		return false;

	// no instances skip
	if( instancing->getIntanceCount() <= 0 )
		return false;

	// if skinned mesh ,skip
	if( _geode->getActionMixer() )
	{
		bool bSkinning = !_sharedMesh->getJointMap().empty();
		if( ATOM_RenderSettings::isEditorModeEnabled() )
		{
			boneMatrices = &_geode->getActionMixer()->getJointMatrices();
			bSkinning &= ( boneMatrices && !boneMatrices->empty());
		}

		if( bSkinning )
		{
			return false;
		}
	}

	bool ret = false;

	// 使用几何体实例化
	if( bInstancing )
	{
		// not HW Instancing Geometry skip
		if(_sharedMesh->getGeometry()->getType()!=GT_HWINSTANCING)
			return false;

		ATOM_HWInstancingGeometry* hw_instancing_geo = (ATOM_HWInstancingGeometry*)(_sharedMesh->getGeometry());

		// enable hw instancing
		hw_instancing_geo->enableInstancing(true);

		int instance_count = instancing->getIntanceCount();

		// 如果使用静态INSTANCINGBUFFER
		bool useStaticInstancingBuffer = ATOM_RenderSettings::isUseInstancingStaticBuffer();
		if( useStaticInstancingBuffer )
		{
			hw_instancing_geo->setInstanceDataAttributes(ATOM_VERTEX_ATTRIB_TEX4_1);
			hw_instancing_geo->setInstanceCount(instance_count);

			// set the instance data ( world matrix )
			material->getParameterTable()->setVectorArray("instanceDatas",(ATOM_Vector4f*)instancing->getInstancedata(),instance_count*3);
		}
		// 如果是动态BUFFER lock/unlock
		else
		{
			void * data = hw_instancing_geo->addInstance(instance_count);
			if( !data )
				return false;
			memcpy(data,instancing->getInstancedata(),sizeof(sInstancing::sInstanceAttribute)*instance_count);
		}

		//material->getParameterTable()->setInt ("enableSkinning", (boneMatrices && !boneMatrices->empty()) ? 1 : 0);
		//material->getParameterTable()->setInt ("projectable", (_geode->getProjectable()==ATOM_Node::PROJECTABLE)?1:0);

		//--- wangjian added ---//

		int effect_id = getEffectIdFront(ATOM_RenderScheme::getCurrentRenderScheme());
		if( effect_id >= 0 )
		{
			material->setActiveEffect(effect_id);
			material->getCoreMaterial()->setAutoSelectEffect(false);
		}
		//----------------------//

		device->setTransform (ATOM_MATRIXMODE_WORLD, ATOM_Matrix4x4f::getIdentityMatrix());

		ret = _sharedMesh->draw(device,material);

		//--- wangjian added ---//
		if( effect_id >= 0 )
			material->getCoreMaterial()->setAutoSelectEffect(true);
		//----------------------//
	}
	// 不使用几何体实例化 而是进行BATCH
	else
	{
		ATOM_HWInstancingGeometry* hw_instancing_geo = (ATOM_HWInstancingGeometry*)(_sharedMesh->getGeometry());
		if( !hw_instancing_geo )
			return false;

		// enable hw instancing
		hw_instancing_geo->enableInstancing(false);

		int instance_count = instancing->getIntanceCount();

		//material->getParameterTable()->setInt ("enableSkinning", (boneMatrices && !boneMatrices->empty()) ? 1 : 0);
		//material->getParameterTable()->setInt ("projectable", (_geode->getProjectable()==ATOM_Node::PROJECTABLE)?1:0);

		//--- wangjian added ---//

		int effect_id = getEffectIdFront(ATOM_RenderScheme::getCurrentRenderScheme());
		if( effect_id >= 0 )
		{
			material->setActiveEffect(effect_id);
			material->getCoreMaterial()->setAutoSelectEffect(false);
		}
		//----------------------//

		device->setTransform (ATOM_MATRIXMODE_WORLD, ATOM_Matrix4x4f::getIdentityMatrix());

		ret = _sharedMesh->draw(device,material);

		//--- wangjian added ---//
		if( effect_id >= 0 )
			material->getCoreMaterial()->setAutoSelectEffect(true);
		//----------------------//
	}

	return ret;
}

return ret;
}
void ATOM_ShapeNode::batching(sInstancing * instanceWrapper, ATOM_Camera *camera)
{
	if( instanceWrapper )
		instanceWrapper->appendInstance( getWorldTransform(camera->getProjectionMatrix(), camera->getViewMatrix()) );
}

#endif
//---------------------//

void ATOM_ShapeNode::setupRenderQueue (ATOM_CullVisitor *visitor)
{
	ATOM_STACK_TRACE(ATOM_ShapeNode::setupRenderQueue);
	if (!_material)
	{
		return;
	}

	if (_typeDirty)
	{
		_typeDirty = false;

		if (!regenerate (ATOM_GetRenderDevice()))
		{
			return;
		}
	}

	//--- wangjian modified ---//
#if 1
	const ATOM_Matrix4x4f &matWorld = getWorldMatrix();
#endif
	//-------------------------//

	if (_type == BILLBOARD)
	{
		//--- wangjian modified ---//
		// 将世界变换矩阵放到draw中去设置
#if 0
		ATOM_Vector3f t, s;
		ATOM_Matrix4x4f r;
		matWorld.decompose (t, r, s);
		ATOM_Matrix4x4f m = ATOM_Matrix4x4f::getScaleMatrix(s);
		m.setRow3 (3, t);
		visitor->addDrawable (this, m, _material.get());
#else
		visitor->addDrawable (this, matWorld, _material.get());
#endif
		//--------------------------//
	}
	else
	{
		if (_shearing != 0.f)
		{
			//--- wangjian modified ---//
			// 将世界变换矩阵放到draw中去设置
#if 0
			ATOM_Matrix4x4f matShear;

			if (_type == PLANE || _type == GRID)
			{
				matShear.set(1.f, 0.f, 0.f, 0.f,
							0.f, 1.f, 0.f, 0.f,
							_shearing, 0.f, 1.f, 0.f,
							0.f, 0.f, 0.f, 1.f);
			}
			else
			{
				matShear.set(1.f, 0.f, 0.f, 0.f,
							_shearing, 1.f, 0.f, 0.f,
							0.f, 0.f, 1.f, 0.f,
							0.f, 0.f, 0.f, 1.f);
			}

			matShear >>= ATOM_Matrix4x4f::getScaleMatrix (_size);
			visitor->addDrawable (this, matWorld>>matShear, _material.get(), -1, getProjectable()==ATOM_Node::PROJECTABLE);
#else
			visitor->addDrawable (this, matWorld, _material.get(), -1, getProjectable()==ATOM_Node::PROJECTABLE);
#endif
			//---------------------------------//
		}
		else
		{
			//--- wangjian modified ---//
			// 将世界变换矩阵放到draw中去设置
#if 0
			visitor->addDrawable (this, matWorld>>ATOM_Matrix4x4f::getScaleMatrix(_size), _material.get(), -1, getProjectable()==ATOM_Node::PROJECTABLE);
#else
			visitor->addDrawable (this, matWorld, _material.get(), -1, getProjectable()==ATOM_Node::PROJECTABLE);
#endif
			//---------------------------------//
		}
	}
}

bool ATOM_ShapeNode::rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const
{
	if (_type == BILLBOARD || !_aabbTree)
	{
		float d;
		if (ray.intersectionTestEx (getBoundingbox(), d) && d >= 0.f)
		{
			if (len)
			{
				*len = d;
			}
			return true;
		}
		return false;
	}
	else
	{
		if (len)
		{
			return _aabbTree->rayIntersect(ray, *len) >= 0;
		}
		else
		{
			return _aabbTree->rayIntersectionTest (ray);
		}
	}
}

bool ATOM_ShapeNode::draw (ATOM_RenderDevice* device, ATOM_Camera *camera, ATOM_Material *material)
{
	if (_hTransparency)
	{
		material->getParameterTable()->setFloat(_hTransparency, _transparency);
	}

	if (_hDiffuseColor)
	{
		material->getParameterTable()->setVector(_hDiffuseColor, _color);
	}

	if (_hUVRotation)
	{
		material->getParameterTable()->setFloat(_hUVRotation, _uvRotation * (ATOM_Pi/180.f));
	}

	if (_hUVScaleOffset)
	{
		material->getParameterTable()->setVector(_hUVScaleOffset, _uvScaleOffset);
	}

	if (_hProjectable)
	{
		material->getParameterTable()->setInt(_hProjectable, getProjectable()==ATOM_Node::PROJECTABLE);
	}
	
	if (_hColorMultiplier)
	{
		material->getParameterTable()->setFloat(_hColorMultiplier, _colorMultiplier);	// wangjian added
	}

	//--- wangjian added ---//
	if (_hResolveFactor)
	{
		material->getParameterTable()->setFloat(_hResolveFactor, _resolveFactor);		// wangjian added
	}

	int effect_id = getEffectIdFront(ATOM_RenderScheme::getCurrentRenderScheme());
	if( effect_id >= 0 )
	{
		material->setActiveEffect(effect_id);
		material->getCoreMaterial()->setAutoSelectEffect(false);
	}
	//----------------------//

	//--- wangjian added ---//
#if 1
	const ATOM_Matrix4x4f &matWorld = getWorldMatrix();
#endif
	//----------------------//

	if (_type == BILLBOARD)
	{
		//--- wangjian added ---//
#if 1
		ATOM_Vector3f t, s;
		ATOM_Matrix4x4f r;
		matWorld.decompose (t, r, s);
		ATOM_Matrix4x4f m = ATOM_Matrix4x4f::getScaleMatrix(s);
		m.setRow3 (3, t);
		device->setTransform (ATOM_MATRIXMODE_WORLD, m);
#endif
		//---------------------//

		struct MyVertex
		{
			ATOM_Vector3f pos;
			ATOM_Vector3f normal;
			ATOM_Vector2f uv;
		};
		MyVertex vertices[4];

		ATOM_Vector3f eye, to, up;
		camera->getViewMatrix().decomposeLookatLH (eye, to, up);
		ATOM_Vector3f viewDir = eye - to;

		ATOM_Vector3f viewAxisY = up;
		ATOM_Vector3f viewAxisX = crossProduct (viewAxisY, viewDir);

		ATOM_Vector3f axisX = viewAxisX * _size.x;
		ATOM_Vector3f axisY = viewAxisY * _size.y;

		vertices[0].pos = (axisX+axisY) * 0.5f;
		vertices[0].normal = viewDir;
		vertices[0].uv = ATOM_Vector2f(0.f, 0.f);

		vertices[1].pos = (-axisX+axisY) * 0.5f;
		vertices[1].normal = viewDir;
		vertices[1].uv = ATOM_Vector2f(1.f, 0.f);

		vertices[2].pos = (-axisX-axisY) * 0.5f;
		vertices[2].normal = viewDir;
		vertices[2].uv = ATOM_Vector2f(1.f, 1.f);

		vertices[3].pos = (axisX-axisY) * 0.5f;
		vertices[3].normal = viewDir;
		vertices[3].uv = ATOM_Vector2f(0.f, 1.f);

		if (_shearing != 0.f)
		{
			vertices[0].pos += _shearing * dotProduct(vertices[0].pos, viewAxisY) * viewAxisX;
			vertices[1].pos += _shearing * dotProduct(vertices[1].pos, viewAxisY) * viewAxisX;
			vertices[2].pos += _shearing * dotProduct(vertices[2].pos, viewAxisY) * viewAxisX;
			vertices[3].pos += _shearing * dotProduct(vertices[3].pos, viewAxisY) * viewAxisX;
		}

		unsigned short indices[6] = { 0, 1, 2, 0, 2, 3 };

		unsigned numPasses = material->begin (device);
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (material->beginPass (device, pass))
			{
				device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLES, 6, 4, ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_NORMAL|ATOM_VERTEX_ATTRIB_TEX1_2, sizeof(ShapeVertex), vertices, indices);
				material->endPass (device, pass);
			}
		}
		material->end (device);
	}
	else if (_type == MODEL)
	{
		if (_model)
		{
			//--- wangjian added ---//
#if 1
			device->setTransform (ATOM_MATRIXMODE_WORLD, matWorld>>ATOM_Matrix4x4f::getScaleMatrix(_size));
			
#endif
			//----------------------//

			unsigned numPasses = material->begin (device);
			for (unsigned pass = 0; pass < numPasses; ++pass)
			{
				if (material->beginPass (device, pass))
				{
					for (unsigned i = 0; i < _model->getNumMeshes(); ++i)
					{
						// wangjian added
						// 当前shape中使用到的MODEL如果其网格是实例化几何体类型 则不开启实例化渲染
						if( _model->getMesh (i)->getGeometry()->getType()==GT_HWINSTANCING )
						{
							ATOM_HWInstancingGeometry* hw_instancing_geo = (ATOM_HWInstancingGeometry*)(_model->getMesh (i)->getGeometry());
							if( hw_instancing_geo )
								hw_instancing_geo->enableInstancing(false);
						}

						_model->getMesh (i)->drawWithoutMaterial (device);
					}
					material->endPass (device, pass);
				}
			}
			material->end (device);
		}
	}
	else
	{
		//--- wangjian added ---//
#if 1
		if (_shearing != 0.f)
		{
			ATOM_Matrix4x4f matShear;

			if (_type == PLANE || _type == GRID)
			{
				matShear.set(1.f, 0.f, 0.f, 0.f,
					0.f, 1.f, 0.f, 0.f,
					_shearing, 0.f, 1.f, 0.f,
					0.f, 0.f, 0.f, 1.f);
			}
			else
			{
				matShear.set(1.f, 0.f, 0.f, 0.f,
					_shearing, 1.f, 0.f, 0.f,
					0.f, 0.f, 1.f, 0.f,
					0.f, 0.f, 0.f, 1.f);
			}

			matShear >>= ATOM_Matrix4x4f::getScaleMatrix (_size);
			device->setTransform (ATOM_MATRIXMODE_WORLD, matWorld>>matShear);
			//---------------------------------//
		}
		else
		{
			device->setTransform (ATOM_MATRIXMODE_WORLD, matWorld>>ATOM_Matrix4x4f::getScaleMatrix(_size));
		}
#endif
		//----------------------//

		if (_vertexArray)
		{
			device->setStreamSource (0, _vertexArray.get());

			if (!_vertexDecl)
			{
				_vertexDecl = device->createInterleavedVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_NORMAL|ATOM_VERTEX_ATTRIB_TEX1_2);
			}
			device->setVertexDecl (_vertexDecl);
		}

		int visiblePart = ATOM_min2(_visiblePart, _smoothness);
		int primitiveType = ATOM_PRIMITIVE_TRIANGLES;
		unsigned numPrimitives;
		if (_type == SPHERE)
		{
			numPrimitives = (2 * visiblePart * (_smoothness + 1) - 2);
			primitiveType = ATOM_PRIMITIVE_TRIANGLE_STRIP;
		}
		else if (_type == CYLINDER && !_withCaps)
		{
			numPrimitives = 2 * visiblePart;
		}
		else if (_type == PLANE)
		{
			numPrimitives = _indexArray ? _indexArray->getNumIndices()-2 : _indices.size()-2;
			primitiveType = ATOM_PRIMITIVE_TRIANGLE_STRIP;
		}
		else if (_type == GRID)
		{
			numPrimitives = _indexArray ? _indexArray->getNumIndices() / 2 : _indices.size() / 2;
			primitiveType = ATOM_PRIMITIVE_LINES;
		}
		else
		{
			numPrimitives = _indexArray ? _indexArray->getNumIndices() / 3 : _indices.size() / 3;
		}

		unsigned numPasses = material->begin (device);
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (material->beginPass (device, pass))
			{
				if (!_vertexArray)
				{
					device->renderInterleavedMemoryStreamsIndexed (
						primitiveType, 
						_indices.size(), 
						_vertices.size(), 
						ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_NORMAL|ATOM_VERTEX_ATTRIB_TEX1_2, 
						sizeof(ShapeVertex), 
						&_vertices[0], 
						&_indices[0]
					);
				}
				else
				{
					device->renderStreamsIndexed (_indexArray.get(), primitiveType, numPrimitives, 0);
				}
				material->endPass (device, pass);
			}	
		}
		material->end (device);
		
	}

	//--- wangjian added ---//
	if( effect_id >= 0 )
		material->getCoreMaterial()->setAutoSelectEffect(true);
	//----------------------//

	return true;
}

bool ATOM_ShapeNode::onLoad(ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_ShapeNode::onLoad);
	_typeDirty = false;

	return regenerate (device);
}

void ATOM_ShapeNode::setType (int type)
{
	if (type != _type)
	{
		_type = type;
		_typeDirty = true;
		invalidateBoundingbox();
	}
}

int ATOM_ShapeNode::getType (void) const
{
	return _type;
}

bool computeNormals (ATOM_VECTOR<ATOM_Vector3f> &vert, ATOM_VECTOR<ATOM_Vector3f> &norm, ATOM_VECTOR<ATOM_Vector2f> &uv, ATOM_VECTOR<unsigned> &index, float smoothAngle)
{
	ATOM_STACK_TRACE(ATOM_ShapeNode::computeNormals);
	std::vector<unsigned> indices(index.size());
	std::vector<unsigned> remap;
	std::vector<ATOM_MeshMender::Vertex> verts;

	for (unsigned i = 0; i < index.size(); ++i)
	{
		indices[i] = index[i];
	}

	ATOM_MeshMender::Vertex inv;

    for (unsigned i = 0; i < vert.size(); ++i)
    {
		inv.pos = vert[i];
		inv.s = 0.f;
		inv.t = 0.f;
        verts.push_back (inv);
    }

	const float minNormalCreaseCos = cosf (smoothAngle);
	const float minTangentCreaseCos = minNormalCreaseCos;
	const float minBinormalCreaseCos = minNormalCreaseCos;
	const float weightNormalsByArea = 1.f;

	ATOM_MeshMender mender;
	mender.Mend (verts,
			  indices,
			  remap,
			  minNormalCreaseCos,
			  minTangentCreaseCos,
			  minBinormalCreaseCos,
			  weightNormalsByArea,
			  ATOM_MeshMender::CALCULATE_NORMALS,
			  ATOM_MeshMender::DONT_RESPECT_SPLITS,
			  ATOM_MeshMender::DONT_FIX_CYLINDRICAL);

	if (index.size() != indices.size())
	{
		return false;
	}

	for (unsigned i = 0; i < index.size(); ++i)
	{
		index[i] = indices[i];
	}

	ATOM_VECTOR<ATOM_Vector2f> newUV(verts.size());
	norm.resize (0);
	vert.resize (verts.size());

	for (unsigned i = 0; i < verts.size(); ++i)
	{
		vert[i] = verts[i].pos;
		norm.push_back (verts[i].normal);
		newUV[i] = uv[remap[i]];
	}

	uv = newUV;

	return true;
}

bool ATOM_ShapeNode::regenerate (ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_ShapeNode::regenerate);
	_vertexArray = 0;
	_indexArray = 0;
	_vertices.resize (0);
	_indices.resize (0);
	ATOM_DELETE(_aabbTree);
	_aabbTree = 0;

	unsigned numVertices = 0;
	unsigned numIndices = 0;
	const ATOM_Vector3f center(0.f, 0.f, 0.f);
	ATOM_VECTOR<ATOM_Vector3f> vertices;
	ATOM_VECTOR<ATOM_Vector2f> uv;
	ATOM_VECTOR<unsigned> indices;
	ATOM_VECTOR<ATOM_Vector3f> normals;

	switch (_type)
	{
	case PLANE:
		{
			CreatePlane (center, 2.f, 2.f, _divideX+1, _divideY+1, 0, 0, 0, 0, &numVertices, &numIndices, 0);
			vertices.resize (numVertices);
			normals.resize (numVertices);
			uv.resize (numVertices);
			indices.resize (numIndices);
			CreatePlane (center, 2.f, 2.f, _divideX+1, _divideY+1, &vertices[0], &normals[0], &uv[0], &indices[0], &numVertices, &numIndices, 0);
			break;
		}
	case GRID:
		{
			CreateGrid (center, 2.f, 2.f, _divideX+1, _divideY+1, 0, 0, 0, 0, &numVertices, &numIndices, 0);
			vertices.resize (numVertices);
			normals.resize (numVertices);
			uv.resize (numVertices);
			indices.resize (numIndices);
			CreateGrid (center, 2.f, 2.f, _divideX+1, _divideY+1, &vertices[0], &normals[0], &uv[0], &indices[0], &numVertices, &numIndices, 0);
			break;
		}
	case SPHERE:
		{
			CreateSphere (center, _smoothness, _smoothness, 1.f, 0, 0, 0, 0, &numVertices, &numIndices, 0);
			vertices.resize (numVertices);
			normals.resize (numVertices);
			uv.resize (numVertices);
			indices.resize (numIndices);
			CreateSphere (center, _smoothness, _smoothness, 1.f, &vertices[0], &normals[0], &uv[0], &indices[0], &numVertices, &numIndices, 0);
			break;
		}
	case CYLINDER:
		{
			CreateCylinder (ATOM_Vector3f(0.f, -1.f, 0.f), _withCaps != 0, _smoothness, ATOM_Vector3f(0.f, 1.f, 0.f), 1.f, 2.f, 0, 0, 0, &numVertices, &numIndices, 0);
			vertices.resize (numVertices);
			uv.resize (numVertices);
			indices.resize (numIndices);
			CreateCylinder (ATOM_Vector3f(0.f, -1.f, 0.f), _withCaps != 0, _smoothness, ATOM_Vector3f(0.f, 1.f, 0.f), 1.f, 2.f, &vertices[0], &uv[0], &indices[0], &numVertices, &numIndices, 0);
			break;
		}
	case CONE:
		{
			CreateCone (ATOM_Vector3f(0.f, -1.f, 0.f), _withCaps, _smoothness, ATOM_Vector3f(0.f, 1.f, 0.f), 1.f, 2.f, 0, 0, 0, &numVertices, &numIndices, 0);
			vertices.resize (numVertices);
			uv.resize (numVertices);
			indices.resize (numIndices);
			CreateCone (ATOM_Vector3f(0.f, -1.f, 0.f), _withCaps, _smoothness, ATOM_Vector3f(0.f, 1.f, 0.f), 1.f, 2.f, &vertices[0], &uv[0], &indices[0], &numVertices, &numIndices, 0);
			break;
		}
	case BILLBOARD:
		{
			//--- wangjian added ---//
			// 加载完成（因为不需要加载，直接完成）
			_load_flag = LOAD_ALLFINISHED;
			//----------------------//

			return true;
		}
	case MODEL:
		{
			_vertexArray = 0;
			_indexArray = 0;

			// 加载模型
			loadModel (_modelFileName.c_str());

			return true;
		}
	default:
		{
			return false;
		}
	}

	if (normals.empty ())
	{
		if (!computeNormals (vertices, normals, uv, indices, ATOM_HalfPi * 0.2f))
		{
			return false;
		}
	}

	numVertices = vertices.size();
	numIndices = indices.size();
	if (numVertices < 16 && numIndices < 16)
	{
		_vertices.resize (numVertices);
		for (int i = 0; i < numVertices; ++i)
		{
			_vertices[i].pos = vertices[i];
			_vertices[i].normal = normals[i];
			_vertices[i].uv = uv[i];
		}

		_indices.resize (numIndices);
		for (int i = 0; i < numIndices; ++i)
		{
			_indices[i] = indices[i];
		}
	}
	else
	{
		_vertexArray = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_NORMAL|ATOM_VERTEX_ATTRIB_TEX1_2, ATOM_USAGE_STATIC, numVertices, true);
		if( _vertexArray )
		{
			ShapeVertex *v = (ShapeVertex*)_vertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
			if( v )
			{
				for (unsigned i = 0; i < numVertices; ++i)
				{
					v[i].pos = vertices[i];
					v[i].normal = normals[i];
					v[i].uv = uv[i];
				}
				_vertexArray->unlock ();
			}
		
		}
	
		bool use32bit = numIndices > 0xFFFF;
		_indexArray = device->allocIndexArray (ATOM_USAGE_STATIC, numIndices, use32bit, true);
		if( _indexArray )
		{
			if (use32bit)
			{
				unsigned *i = (unsigned*)_indexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
				if( i )
				{
					memcpy (i, &indices[0], numIndices * sizeof(unsigned));
					_indexArray->unlock ();
				}
			
			}
			else
			{
				unsigned short *i = (unsigned short*)_indexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
				if( i )
				{
					unsigned *s = &indices[0];
					for (unsigned n = 0; n < numIndices; ++n)
					{
						*i++ = *s++;
					}
					_indexArray->unlock ();
				}
			}
		}
	
		if (0 && !use32bit)
		{
			_aabbTree = ATOM_NEW(ATOM_AABBTree);
			ATOM_AABBTreeBuilder builder;
			ATOM_AABBTree::PrimitiveType pt = (_type == SPHERE) ? ATOM_AABBTree::PT_TRIANGLESTRIP : ATOM_AABBTree::PT_TRIANGLES;
			unsigned numPrimitives = (_type == SPHERE) ? numIndices - 2 : numIndices / 3;
			ATOM_VECTOR<unsigned short> indices16(indices.size());
			for (unsigned i = 0; i < indices.size(); ++i)
			{
				indices16[i] = indices[i];
			}
			builder.buildFromPrimitives (_aabbTree, pt, &vertices[0], numVertices, &indices16[0], numPrimitives, 2);
		}
	}

	//--- wangjian added ---//
	// 加载完成（因为不需要加载，直接完成）
	_load_flag = LOAD_ALLFINISHED;
	//----------------------//

	return true;
}

void ATOM_ShapeNode::setMaterialFileName (const ATOM_STRING &materialFileName)
{
	if (materialFileName != _materialFileName)
	{
		_material = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), materialFileName.c_str());
		_materialFileName = materialFileName;

		if (_material)
		{
			_hTransparency = _material->getParameterTable()->getValueHandle ("transparency");
			_hDiffuseColor = _material->getParameterTable()->getValueHandle ("diffuseColor");
			_hUVRotation = _material->getParameterTable()->getValueHandle ("uvRotation");
			_hUVScaleOffset = _material->getParameterTable()->getValueHandle ("uvScaleOffset");
			_hProjectable = _material->getParameterTable()->getValueHandle ("projectable");
			_hColorMultiplier = _material->getParameterTable()->getValueHandle ("colorMultiplier");

			_hResolveFactor		= _material->getParameterTable()->getValueHandle ("resolveScale");		// wangjian added

#if 0
			_hDiffuseTexture	= _material->getParameterTable()->getValueHandle ("diffuseTexture");	// wangjian added
			_hPerturbTexture	= _material->getParameterTable()->getValueHandle ("perturbTexture");	// wangjian added
			_hPerturbMaskTex	= _material->getParameterTable()->getValueHandle ("perturbMaskTex");	// wangjian added
			_hDisolveTexture	= _material->getParameterTable()->getValueHandle ("resolveMaskTex");	// wangjian added
#endif

		}
		else
		{
			_hTransparency = 0;
			_hDiffuseColor = 0;
			_hUVRotation = 0;
			_hUVScaleOffset = 0;
			_hProjectable = 0;
			_hColorMultiplier = 0;

			_hResolveFactor		= 0;	// wangjian added

#if 0
			_hDiffuseTexture	= 0;	// wangjian added
			_hPerturbTexture	= 0;	// wangjian added
			_hPerturbMaskTex	= 0;	// wangjian added
			_hDisolveTexture	= 0;	// wangjian added
#endif

		}
	}
}

const ATOM_STRING &ATOM_ShapeNode::getMaterialFileName (void) const
{
	return _materialFileName;
}

void ATOM_ShapeNode::setMaterial (ATOM_Material *material)
{
	if (material != _material)
	{
		_material = material;
		_materialFileName = "";

		if (_material)
		{
			_hTransparency = _material->getParameterTable()->getValueHandle ("transparency");
			_hDiffuseColor = _material->getParameterTable()->getValueHandle ("diffuseColor");
			_hUVRotation = _material->getParameterTable()->getValueHandle ("uvRotation");
			_hUVScaleOffset = _material->getParameterTable()->getValueHandle ("uvScaleOffset");
			_hProjectable = _material->getParameterTable()->getValueHandle ("projectable");
			_hColorMultiplier = _material->getParameterTable()->getValueHandle ("colorMultiplier");
			_hResolveFactor = _material->getParameterTable()->getValueHandle ("resolveScale");	// wangjian added
		}
		else
		{
			_hTransparency = 0;
			_hDiffuseColor = 0;
			_hUVRotation = 0;
			_hUVScaleOffset = 0;
			_hProjectable = 0;
			_hColorMultiplier = 0;
			_hResolveFactor = 0;		// wangjian added
		}
	}
}

ATOM_Material *ATOM_ShapeNode::getMaterial (void) const
{
	return _material.get();
}

bool ATOM_ShapeNode::loadAttribute(const ATOM_TiXmlElement *xmlelement)
{
	//--- wangjian added ---//
	// 首先保存优先级 因为在加载属性时可能会改变该值
	int		priority			= _load_priority;
	bool	bChangedExternal	= ( priority != ATOM_LoadPriority_ASYNCBASE );
	//----------------------//

	if (ATOM_VisualNode::loadAttribute (xmlelement))
	{
		if (xmlelement && _material)
		{
			//--- wangjian modified ---//
			// 如果已经从外部改变了优先级 使用外部给定的优先级
			if( bChangedExternal )
				_load_priority = priority;

			// 纹理需要判断是否异步加载
			_material->getParameterTable()->loadFromXML ( xmlelement,_load_priority );
			//-------------------------//
		}
		return true;
	}
	return false;
}

bool ATOM_ShapeNode::writeAttribute(ATOM_TiXmlElement *xmlelement)
{
	if (ATOM_VisualNode::writeAttribute (xmlelement))
	{
		if (xmlelement && _material)
		{
			_material->getParameterTable()->saveToXML (xmlelement);
		}
		return true;
	}
	return false;
}

void ATOM_ShapeNode::setWithCaps (int b)
{
	if (b != _withCaps)
	{
		_withCaps = b;
		_typeDirty = true;
		invalidateBoundingbox();
	}
}

int ATOM_ShapeNode::getWithCaps (void) const
{
	return _withCaps;
}

void ATOM_ShapeNode::setSmoothness (int smoothness)
{
	if (_smoothness != smoothness)
	{
		_smoothness = smoothness;
		_typeDirty = true;
		invalidateBoundingbox ();
	}
}

int ATOM_ShapeNode::getSmoothness (void) const
{
	return _smoothness;
}

void ATOM_ShapeNode::setDivisionX (int division)
{
	int val = ATOM_max2(division, 1);

	if (_divideX != val)
	{
		_divideX = val;
		_typeDirty = true;
		invalidateBoundingbox ();
	}
}

int ATOM_ShapeNode::getDivisionX (void) const
{
	return _divideX;
}

void ATOM_ShapeNode::setDivisionY (int division)
{
	int val = ATOM_max2(division, 1);

	if (_divideY != val)
	{
		_divideY = val;
		_typeDirty = true;
		invalidateBoundingbox ();
	}
}

int ATOM_ShapeNode::getDivisionY (void) const
{
	return _divideY;
}

void ATOM_ShapeNode::setVisiblePart (int part)
{
	_visiblePart = part;
}

int ATOM_ShapeNode::getVisiblePart (void) const
{
	return _visiblePart;
}

void ATOM_ShapeNode::setTransparency (float val)
{
	_transparency = val;
}

float ATOM_ShapeNode::getTransparency (void) const
{
	return _transparency;
}

void ATOM_ShapeNode::setColor (const ATOM_Vector4f &color)
{
	_color = color;
}

const ATOM_Vector4f &ATOM_ShapeNode::getColor (void) const
{
	return _color;
}

void ATOM_ShapeNode::setText (const char *str)
{
	_text = str ? str : "";
}

const char *ATOM_ShapeNode::getText (void) const
{
	return _text.c_str();
}

void ATOM_ShapeNode::setModelFileName (const ATOM_STRING &fileName)
{
	if (_modelFileName != fileName)
	{
		_modelFileName = fileName;

		if (_type == MODEL)
		{
			_typeDirty = true;
			invalidateBoundingbox ();
		}
	}
}

const ATOM_STRING &ATOM_ShapeNode::getModelFileName (void) const
{
	return _modelFileName;
}

void ATOM_ShapeNode::setShearing (float shearing)
{
	_shearing = shearing;
}

float ATOM_ShapeNode::getShearing (void) const
{
	return _shearing;
}

void ATOM_ShapeNode::setSize (const ATOM_Vector3f &size)
{
	_size = size;
	invalidateBoundingbox();
}

const ATOM_Vector3f &ATOM_ShapeNode::getSize (void) const
{
	return _size;
}

void ATOM_ShapeNode::setUVRotation (float uvRotation)
{
	_uvRotation = uvRotation;
}

float ATOM_ShapeNode::getUVRotation (void) const
{
	return _uvRotation;
}

void ATOM_ShapeNode::setUVScaleOffset (const ATOM_Vector4f &v)
{
	_uvScaleOffset = v;
}

const ATOM_Vector4f &ATOM_ShapeNode::getUVScaleOffset (void) const
{
	return _uvScaleOffset;
}

bool ATOM_ShapeNode::loadModel (const char *fileName)
{
	//--- wangjian added ---//
	// 异步加载相关

	if( !fileName || fileName[0] == '\0' )
		return false;

#if 0
	if( _load_priority == ATOM_LoadPriority_IMMEDIATE )
		ATOM_LOGGER::log("======= %s : the load priority is immediate =========\n", fileName, _load_priority );
#endif

	// 如果加载优先级别是ATOM_LoadPriority_IMMEDIATE，则立即加载 不使用异步加载
	int loadPriority = _load_priority;

	bool bMT = ( ATOM_AsyncLoader::IsRun() );// && ( _load_priority != ATOM_LoadPriority_IMMEDIATE );
	if( bMT )
	{
		char buffer[ATOM_VFS::max_filename_length];
		if (ATOM_CompletePath (fileName, buffer))
		{
			_strlwr (buffer);

			// 查找该模型
			ATOM_AUTOREF(ATOM_SharedModel) model = ATOM_LookupObject ( ATOM_SharedModel::_classname(), buffer );

			// 如果该模型还不存在 且 优先级为ATOM_LoadPriority_IMMEDIATE
			if( !model && _load_priority == ATOM_LoadPriority_IMMEDIATE )
			{
				// 直接加载 设置加载完成标记
				_load_flag = LOAD_ALLFINISHED;
			}
			// 如果该模型资源不存在 或 该模型资源尚未加载完成
			else if	( !model || ( !model->getAsyncLoader()->IsLoadAllFinished() ) )
			{
				// 如果模型存在 且 优先级为ATOM_LoadPriority_IMMEDIATE 则 使用该模型的加载对象的优先级
				// 否则 使用此节点的优先级
				loadPriority = ( model && _load_priority == ATOM_LoadPriority_IMMEDIATE ) ?	model->getAsyncLoader()->priority : 
																							_load_priority;

				// 添加加载请求
				ATOM_LOADRequest_Event * event_load( ATOM_NEW(	ATOM_LOADRequest_Event, 
																buffer, 
																this, 
																loadPriority,
																model ? model->getAsyncLoader() : 0,
																ATOM_LOADRequest_Event::FLAG_SHOW	) );
			}
			// 直接设置加载完成了
			else
			{
				_load_flag = LOAD_ALLFINISHED;
			}
		}
	}
	//----------------------//

	//--- wangjian modified ---//
	//ATOM_Components components(fileName);
	// 如果加载优先级别是ATOM_LoadPriority_IMMEDIATE，则立即加载 不使用异步加载
	ATOM_Components components(fileName, loadPriority);
	//-------------------------//

	if (!components.getModel())
	{
		return false;
	}

	_model = components.getModel();

	//--- wangjian added ---//
	// 如果是非异步加载 则直接设置加载完成
	if( !bMT )
		_load_flag = LOAD_ALLFINISHED;
	//----------------------//

	return true;
}

//--- wangjian added ---//
void ATOM_ShapeNode::setColorMultiplier (float colorMultiplier)
{
	_colorMultiplier = colorMultiplier;
}

const float ATOM_ShapeNode::getColorMultiplier (void) const
{
	return _colorMultiplier;
}

//! 设置全局溶解强度因子
void ATOM_ShapeNode::setResolveScale (const float resolveFactor)
{
	_resolveFactor = resolveFactor;
}
//! 获取全局溶解强度因子
const float ATOM_ShapeNode::getResolveScale (void) const
{
	return _resolveFactor;
}

void ATOM_ShapeNode::onLoadFinished()
{
	onTransformChanged();
	getWorldBoundingbox();

	_load_flag = LOAD_ALLFINISHED;
}

void ATOM_ShapeNode::resetMaterialDirtyFlag_impl()
{
	if( _material )
		_material->getParameterTable()->resetDirtyFlag();
}
//----------------------//

