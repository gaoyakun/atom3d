#include "StdAfx.h"
#include "ATOM3DX.h"
#include "axisnode.h"


static const char axisMaterialSrc[] = 
	"<?xml version=\"1.0\"?>																		\n"
	"<material>																	\n"
	"	<param name=\"lightDir\" type=\"float4\" default=\"-1.0,-1.0,-1.0,1.0\" />					\n"
	"	<param name=\"bf\" type=\"float4\" default=\"0.3,0.3,0.3,1.0\" />							\n"
	"	<param name=\"MVP\" type=\"float4x4\" default=\"$MVP_Matrix\" />					\n"
	"	<code><![CDATA[																				\n"
	"		struct v2p	{																			\n"
	"			float4 position: POSITION0;															\n"
	"			float4 diffuse: COLOR0;																\n"
	"		};																						\n"
	"		v2p vs(float4 pos : POSITION, float3 normal: NORMAL, float4 color: COLOR0)				\n"
	"		{																						\n"
	"			v2p OUT;																			\n"
	"			OUT.position = mul(pos, MVP);														\n"
	"			OUT.diffuse = color * (saturate(dot(normal, normalize(lightDir))) + 0.4);			\n"
	"			return OUT;																			\n"
	"		}																						\n"
	"		float4 ps(in v2p IN): COLOR0															\n"
	"		{																						\n"
	"			return IN.diffuse;																	\n"
	"		}																						\n"
	"	]]></code>																					\n"		
	"	<effect name=\"default\" scheme=\"deferred\" schemelayer=\"forward\">		\n"
	"		<pass>														\n"
	"			<streamdefine>											\n"
	"				<stream semantec=\"position\" />					\n"
	"				<stream semantec=\"color0\" />						\n"
	"			</streamdefine>											\n"
	"			<vertexshader entry=\"vs\" target=\"vs_2_0\" />			\n"
	"			<pixelshader entry=\"ps\" target=\"ps_2_0\" />			\n"
	"		</pass>														\n"
	"		<pass>														\n"
	"			<alphablend_state>										\n"
	"				<enable value=\"true\" />							\n"
	"				<srcblend value=\"constant\" />						\n"
	"				<destblend value=\"invconstant\" />					\n"
	"				<blendcolor value=\"$bf\" />						\n"
	"			</alphablend_state>										\n"
	"			<depth_state>											\n"
	"				<zfunc value=\"always\" />							\n"
	"				<zwrite value=\"false\" />							\n"
	"			</depth_state>											\n"
	"		</pass>														\n"
	"	</effect>														\n"
	"</material>														\n";

class AxisRenderDatas: public ATOM_Drawable
{
public:
	AxisRenderDatas (void);
	virtual ~AxisRenderDatas (void);

public:
	virtual bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);

public:
	bool create (ATOM_RenderDevice *device, float width, float height);
	float getWidth (void) const;
	float getHeight (void) const;
	ATOM_BBox getBoundingbox (void) const;
	ATOM_AABBTree *getAABBTree (void) const;
	int whichAxis (int index) const;
	ATOM_Material *getMaterial (void) const;

private:
	float _width;
	float _height;
	ATOM_AUTOREF(ATOM_VertexArray) _vertexArray;
	ATOM_AUTOREF(ATOM_VertexArray) _colorArray;
	ATOM_AUTOREF(ATOM_VertexArray) _normalArray;
	ATOM_AUTOREF(ATOM_IndexArray) _indexArray;
	ATOM_AUTOPTR(ATOM_Material) _material;
	ATOM_VECTOR<int> _axisIndices;
	ATOM_VertexDecl _vertexDecl;
	ATOM_AABBTree *_aabbTree;
};


AxisRenderDatas::AxisRenderDatas (void)
{
	_width = 2.f;
	_height = 20.f;
	_vertexDecl = 0;
	_aabbTree = 0;
}

AxisRenderDatas::~AxisRenderDatas (void)
{
	ATOM_DELETE(_aabbTree);
}

bool computeNormals (ATOM_VECTOR<ATOM_Vector3f> &vert, ATOM_VECTOR<ATOM_Vector3f> &norm, ATOM_VECTOR<ATOM_ColorARGB> &col, ATOM_VECTOR<int> &axisIndices, ATOM_VECTOR<unsigned short> &index, float smoothAngle)
{
	std::vector<unsigned> indices(index.size());
	std::vector<unsigned> remap;
	std::vector<ATOM_MeshMender::Vertex> verts;

	for (unsigned i = 0; i < index.size(); ++i)
	{
		indices[i] = index[i];
	}

	ATOM_MeshMender::Vertex inv;
	bool hascolor = true;

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
	const float weightNormalsByArea = 0.5f;

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

	ATOM_VECTOR<ATOM_ColorARGB> newColors(verts.size());
	ATOM_VECTOR<int> newAxisIndices(verts.size());
	norm.resize (0);
	vert.resize (verts.size());

	for (unsigned i = 0; i < verts.size(); ++i)
	{
		vert[i] = verts[i].pos;
		norm.push_back (verts[i].normal);
		newColors[i] = col[remap[i]];
		newAxisIndices[i] = axisIndices[remap[i]];
	}

	col = newColors;
	axisIndices = newAxisIndices;
	return true;
}

bool AxisRenderDatas::draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{
	if (_material && _vertexArray && _vertexDecl)
	{
		device->setVertexDecl (_vertexDecl);
		device->setStreamSource (0, _vertexArray.get());
		device->setStreamSource (1, _normalArray.get());
		device->setStreamSource (2, _colorArray.get());

		unsigned numPasses = material->begin (device);
		for (unsigned i = 0; i < numPasses; ++i)
		{
			if (material->beginPass (device, i))
			{
				device->renderStreamsIndexed (_indexArray.get(), ATOM_PRIMITIVE_TRIANGLES, _indexArray->getNumIndices() / 3);
				material->endPass (device, i);
			}
		}
		material->end (device);
	}
	return true;
}

bool AxisRenderDatas::create (ATOM_RenderDevice *device, float width, float height)
{
	const ATOM_Vector3f center(0.f, 0.f, 0.f);
	const ATOM_Vector3f axis[3] = {
		ATOM_Vector3f(1.f, 0.f, 0.f),
		ATOM_Vector3f(0.f, 1.f, 0.f),
		ATOM_Vector3f(0.f, 0.f, 1.f)
	};
	const ATOM_ColorARGB col[3] = {
		ATOM_ColorARGB (1.f, 0.f, 0.f, 1.f),
		ATOM_ColorARGB (0.f, 1.f, 0.f, 1.f),
		ATOM_ColorARGB (0.f, 0.f, 1.f, 1.f)
	};
	const int smoothness = 8;

	unsigned numVertsCylinder;
	unsigned numIndicesCylinder;
	ATOMX_CreateCylinder (center, true, smoothness, axis[0], width, height, 0, 0, &numVertsCylinder, &numIndicesCylinder);

	unsigned numVertsCone;
	unsigned numIndicesCone;
	ATOMX_CreateCone (center, true, smoothness, axis[0], width * 2, width * 4, 0, 0, &numVertsCone, &numIndicesCone);

	ATOM_VECTOR<ATOM_Vector3f> vertices((numVertsCylinder + numVertsCone) * 3);
	ATOM_VECTOR<ATOM_Vector3f> normals;
	ATOM_VECTOR<ATOM_ColorARGB> colors((numVertsCylinder + numVertsCone) * 3);
	ATOM_VECTOR<unsigned short> indices((numIndicesCylinder + numIndicesCone) * 3);
	_axisIndices.resize (vertices.size());

	unsigned vertexCreated = 0;
	unsigned indexCreated = 0;
	for (unsigned i = 0; i < 3; ++i)
	{
		for (unsigned j = 0; j < numVertsCylinder + numVertsCone; ++j)
		{
			_axisIndices[vertexCreated + j] = i;
			colors[vertexCreated + j] = col[i];
		}

		ATOMX_CreateCylinder (center, true, smoothness, axis[i], width, height, &vertices[vertexCreated], &indices[indexCreated], 0, 0, vertexCreated);
		vertexCreated += numVertsCylinder;
		indexCreated += numIndicesCylinder;

		ATOMX_CreateCone (center + height * axis[i], true, smoothness, axis[i], width * 2, width * 4, &vertices[vertexCreated], &indices[indexCreated], 0, 0, vertexCreated);
		vertexCreated += numVertsCone;
		indexCreated += numIndicesCone;
	}

	if (!computeNormals (vertices, normals, colors, _axisIndices, indices, ATOM_HalfPi))
	{
		return false;
	}

#if 0
	if (_aabbTree)
	{
		ATOM_DELETE(_aabbTree);
		_aabbTree = 0;
	}

	_aabbTree = ATOM_NEW(ATOM_AABBTree);
	ATOM_AABBTreeBuilder builder;
	if (!builder.buildFromPrimitives (_aabbTree, ATOM_AABBTree::PT_TRIANGLES, &vertices[0], vertices.size(), &indices[0], indices.size()/3, 4))
	{
		ATOM_DELETE(_aabbTree);
		_aabbTree = 0;
		return false;
	}
#endif

	ATOM_AUTOREF(ATOM_VertexArray) vertexArray = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD, ATOM_USAGE_STATIC, vertices.size());
	if (!vertexArray)
	{
		return false;
	}
	ATOM_Vector3f *v = (ATOM_Vector3f*)vertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	memcpy (v, &vertices[0], vertices.size() * sizeof(ATOM_Vector3f));
	vertexArray->unlock ();

	ATOM_AUTOREF(ATOM_VertexArray) normalArray = device->allocVertexArray (ATOM_VERTEX_ATTRIB_NORMAL, ATOM_USAGE_STATIC, normals.size());
	if (!normalArray)
	{
		return false;
	}
	ATOM_Vector3f *n = (ATOM_Vector3f*)normalArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	memcpy (n, &normals[0], normals.size() * sizeof(ATOM_Vector3f));
	normalArray->unlock ();

	ATOM_AUTOREF(ATOM_VertexArray) colorArray = device->allocVertexArray (ATOM_VERTEX_ATTRIB_PRIMARY_COLOR, ATOM_USAGE_STATIC, colors.size());
	if (!colorArray)
	{
		return false;
	}
	ATOM_ColorARGB *c = (ATOM_ColorARGB*)colorArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	memcpy (c, &colors[0], colors.size() * sizeof(ATOM_ColorARGB));
	colorArray->unlock ();

	ATOM_AUTOREF(ATOM_IndexArray) indexArray = device->allocIndexArray (ATOM_USAGE_STATIC, indices.size(), false, true);
	unsigned short *ind = (unsigned short*)indexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	if (!ind)
	{
		return false;
	}
	memcpy (ind, &indices[0], indices.size() * sizeof(unsigned short));
	indexArray->unlock ();

	ATOM_AUTOPTR(ATOM_Material) material = ATOM_MaterialManager::createMaterialFromCoreString (device, axisMaterialSrc);
	if (!material)
	{
		return false;
	}

	if (!_vertexDecl)
	{
		_vertexDecl = device->createMultiStreamVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_PRIMARY_COLOR|ATOM_VERTEX_ATTRIB_NORMAL);
	}

	_vertexArray = vertexArray;
	_colorArray = colorArray;
	_normalArray = normalArray;
	_indexArray = indexArray;
	_material = material;

	_width = width;
	_height = height;

	return true;
}

ATOM_Material *AxisRenderDatas::getMaterial (void) const
{
	return _material.get();
}

ATOM_BBox AxisRenderDatas::getBoundingbox (void) const
{
	ATOM_BBox bbox;
	bbox.setMin (ATOM_Vector3f(0.f, 0.f, 0.f));
	bbox.setMax (ATOM_Vector3f(_height, _height, _height));
	return bbox;
}

ATOM_AABBTree *AxisRenderDatas::getAABBTree (void) const
{
	return _aabbTree;
}

int AxisRenderDatas::whichAxis (int index) const
{
	if (index < 0 || index >= _axisIndices.size())
	{
		return -1;
	}
	return _axisIndices[index];
}

ATOMX_AxisNode::ATOMX_AxisNode (void)
{
	_renderDatas = 0;
	_width = 0;
	_height = 0;
	_contributeCulling = false;
	_hitAxis = ATOMX_AXIS_NONE;
	_load_priority = ATOM_LoadPriority_IMMEDIATE;
	_load_flag = LOAD_ALLFINISHED;
}

ATOMX_AxisNode::~ATOMX_AxisNode (void)
{
	ATOM_DELETE(_renderDatas);
}

void ATOMX_AxisNode::setupRenderQueue (ATOM_CullVisitor *visitor)
{
	if (_renderDatas)
	{
		visitor->addDrawable (_renderDatas, getWorldMatrix(), _renderDatas->getMaterial());
	}
}

void ATOMX_AxisNode::buildBoundingbox (void) const
{
	if (_renderDatas)
	{
		_boundingBox = _renderDatas->getBoundingbox ();
	}
	else
	{
		_boundingBox.setMin (ATOM_Vector3f(0.f, 0.f, 0.f));
		_boundingBox.setMax (ATOM_Vector3f(0.f, 0.f, 0.f));
	}
}

bool ATOMX_AxisNode::onLoad(ATOM_RenderDevice *device)
{
	if (_renderDatas)
	{
		return true;
	}

	if (_width == 0 || _height == 0)
	{
		return false;
	}

	_renderDatas = ATOM_NEW(AxisRenderDatas);

	if (!_renderDatas->create (device, _width, _height))
	{
		ATOM_DELETE(_renderDatas);
		_renderDatas = 0;
		return false;
	}

	return true;
}

bool ATOMX_AxisNode::setSize (float width, float height)
{
	if (width != _width || height != _height)
	{
		if (_renderDatas)
		{
			if (!_renderDatas->create (ATOM_GetRenderDevice(), width, height))
			{
				return false;
			}
		}

		_width = width;
		_height = height;
	}

	return true;
}

float ATOMX_AxisNode::getWidth (void) const
{
	return _width;
}

float ATOMX_AxisNode::getHeight (void) const
{
	return _height;
}

bool ATOMX_AxisNode::rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const
{
	float width = _width;
	const ATOM_Matrix4x4f &worldmatrix = getWorldMatrix ();
	ATOM_Vector3f cameraPos = camera->getPosition();
	ATOM_Vector3f nodePos(worldmatrix.m30, worldmatrix.m31, worldmatrix.m32);
	float length = (cameraPos - nodePos).getLength ();

	// adjust width for easy pick
	float tanHalfFovy = 1.f / camera->getInvTanHalfFovy ();
	const float screenSize = 20.f;
	float h = length * tanHalfFovy;
	float screenH = camera->getViewport().size.h * 0.5f;
	float w = h * screenSize / screenH;
	width = w * 0.5f;
	width /= getScale ().x;

	if (width < _width)
	{
		width = _width;
	}

	float height = _height + 4 * _width;
	float t = FLT_MAX;
	float d;
	_hitAxis = ATOMX_AXIS_NONE;

	ATOM_BBox box;
	box.setMin (ATOM_Vector3f(0.f, -width, -width));
	box.setMax (ATOM_Vector3f(height, width, width));
	if (ray.intersectionTestEx (box, d) && d >= 0)
	{
		t = d;
		_hitAxis = ATOMX_AXIS_X;
	}
	box.setMin (ATOM_Vector3f(-width, 0.f, -width));
	box.setMax (ATOM_Vector3f(width, height, width));
	if (ray.intersectionTestEx (box, d) && d >= 0.f && d < t)
	{
		t = d;
		_hitAxis = ATOMX_AXIS_Y;
	}
	box.setMin (ATOM_Vector3f(-width, -width, 0));
	box.setMax (ATOM_Vector3f(width, width, height));
	if (ray.intersectionTestEx (box, d) && d >= 0.f && d < t)
	{
		t = d;
		_hitAxis = ATOMX_AXIS_Z;
	}

	if (_hitAxis != ATOMX_AXIS_NONE)
	{
		if (len)
		{
			*len = t;
		}
		return true;
	}

	return false;
}

int ATOMX_AxisNode::getHitAxis (void) const
{
	return _hitAxis;
}

ATOM_Vector3f ATOMX_AxisNode::getRayProjectPlane (const ATOM_Vector3f &start, const ATOM_Vector3f &direction, int axisType) const
{
	ATOM_Vector3f d = direction;
	d.normalize ();

	ATOM_Vector3f planeNormals[2];
	switch (axisType)
	{
	case ATOMX_AXIS_X:
		planeNormals[0].set(0.f, 1.f, 0.f);
		planeNormals[1].set(0.f, 0.f, 1.f);
		break;
	case ATOMX_AXIS_Y:
		planeNormals[0].set(1.f, 0.f, 0.f);
		planeNormals[1].set(0.f, 0.f, 1.f);
		break;
	case ATOMX_AXIS_Z:
		planeNormals[0].set(1.f, 0.f, 0.f);
		planeNormals[1].set(0.f, 1.f, 0.f);
		break;
	default:
		return ATOM_Vector3f(0.f, 0.f, 0.f);
	}

	int projectPlane = 0;
	if (ATOM_abs(dotProduct(d, planeNormals[1])) > ATOM_abs(dotProduct(d, planeNormals[0])))
	{
		projectPlane = 1;
	}

	if  (ATOM_abs(dotProduct(start, planeNormals[projectPlane])) < 0.01f)
	{
		projectPlane = 1 - projectPlane;
	}

	return planeNormals[projectPlane];
}

ATOM_Vector3f ATOMX_AxisNode::getRayProjectPoint (const ATOM_Vector3f &start, const ATOM_Vector3f &direction, int axisType, const ATOM_Vector3f &plane) const
{
	ATOM_Vector3f d = direction;
	d.normalize ();

	float len = -dotProduct(start, plane)/dotProduct(d, plane);
	ATOM_Vector3f v = start + d * len;

	switch (axisType)
	{
	case ATOMX_AXIS_X:
		v *= ATOM_Vector3f(1.f, 0.f, 0.f);
		break;
	case ATOMX_AXIS_Y:
		v *= ATOM_Vector3f(0.f, 1.f, 0.f);
		break;
	case ATOMX_AXIS_Z:
		v *= ATOM_Vector3f(0.f, 0.f, 1.f);
		break;
	}

	return v;
}


