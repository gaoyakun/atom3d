#include "StdAfx.h"
#include "lightgeom.h"

static const char lightgeomMaterialSrc[] = 
	"<?xml version=\"1.0\"?>											\n"
	"<material auto=\"1\">										\n"
	"	<param name=\"color\" type=\"float4\" default=\"1.0,1.0,1.0,1.0\" />						\n"
	"	<code><![CDATA[																	\n"
	"		struct v2p											\n"
	"		{							\n"
	"			float4 position : POSITION0;	\n"
	"		};	\n"
	"		float4 ps (in v2p IN) : COLOR0	\n"
	"		{	\n"
	"			return color;	\n"
	"		}	\n"
	"	]]></code>	\n"
	"	<effect name=\"default\" scheme=\"deferred\" schemelayer=\"forward\" >										\n"
	"		<pass>														\n"
	"			<streamdefine>											\n"
	"				<stream semantec=\"position\" />					\n"
	"			</streamdefine>											\n"
	"			<stencil_state>											\n"
	"				<enable value=\"true\" />							\n"
	"				<func_front value=\"always\" />						\n"
	"				<pass_front value=\"replace\" />					\n"
	"				<writemask value=\"127\" />							\n"
	"				<ref value=\"0\" />									\n"
	"			</stencil_state>										\n"
	"			<pixelshader entry=\"ps\" target=\"ps_2_0\" />			\n"
	"		</pass>														\n"
	"	</effect>														\n"
	"</material>														\n";


#define LIGHTGEOM_SIZE 10.f

LightGeomRenderDatas::LightGeomRenderDatas (void)
{
	_vertexDecl = 0;
	_color.set(1.f, 1.f, 0.f, 1.f);
}

void LightGeomRenderDatas::setColor (const ATOM_Vector4f &color)
{
	_color = color;
}

ATOM_Material *LightGeomRenderDatas::getMaterial (void) const
{
	return _material.get();
}

bool LightGeomRenderDatas::draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{
	if (_vertexArray && _vertexDecl)
	{
		device->setVertexDecl (_vertexDecl);
		device->setStreamSource (0, _vertexArray.get());

		material->getParameterTable()->setVector ("color", _color);

		unsigned numPasses = material->begin (device);
		for (unsigned i = 0; i < numPasses; ++i)
		{
			if (material->beginPass (device, i))
			{
				device->renderStreams (ATOM_PRIMITIVE_TRIANGLES, 8);
				material->endPass (device, i);
			}
		}
		material->end (device);
	}
	return true;
}

bool LightGeomRenderDatas::create (ATOM_RenderDevice *device, int type)
{
	ATOM_AUTOREF(ATOM_VertexArray) vertexArray = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD, ATOM_USAGE_STATIC, 24);
	if (!vertexArray)
	{
		return false;
	}

	const ATOM_Vector3f v[24] = {
		ATOM_Vector3f(0.f, 1.f, 0.f),
		ATOM_Vector3f(1.f, 0.f, 0.f),
		ATOM_Vector3f(0.f, 0.f, -1.f),
		ATOM_Vector3f(0.f, 1.f, 0.f),
		ATOM_Vector3f(0.f, 0.f, 1.f),
		ATOM_Vector3f(1.f, 0.f, 0.f),
		ATOM_Vector3f(0.f, 1.f, 0.f),
		ATOM_Vector3f(-1.f, 0.f, 0.f),
		ATOM_Vector3f(0.f, 0.f, 1.f),
		ATOM_Vector3f(0.f, 1.f, 0.f),
		ATOM_Vector3f(0.f, 0.f, -1.f),
		ATOM_Vector3f(-1.f, 0.f, 0.f),
		ATOM_Vector3f(0.f, -1.f, 0.f),
		ATOM_Vector3f(0.f, 0.f, -1.f),
		ATOM_Vector3f(1.f, 0.f, 0.f),
		ATOM_Vector3f(0.f, -1.f, 0.f),
		ATOM_Vector3f(1.f, 0.f, 0.f),
		ATOM_Vector3f(0.f, 0.f, 1.f),
		ATOM_Vector3f(0.f, -1.f, 0.f),
		ATOM_Vector3f(0.f, 0.f, 1.f),
		ATOM_Vector3f(-1.f, 0.f, 0.f),
		ATOM_Vector3f(0.f, -1.f, 0.f),
		ATOM_Vector3f(-1.f, 0.f, 0.f),
		ATOM_Vector3f(0.f, 0.f, -1.f)
	};

	ATOM_Vector3f *vertices = (ATOM_Vector3f*)vertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	if (!vertices)
	{
		return false;
	}

	for (unsigned i = 0; i < 24; ++i)
	{
		vertices[i] = v[i] * LIGHTGEOM_SIZE;
	}
	vertexArray->unlock ();

	ATOM_AUTOPTR(ATOM_Material) material = ATOM_MaterialManager::createMaterialFromCoreString (device, lightgeomMaterialSrc);
	if (!material)
	{
		return false;
	}

	if (!_vertexDecl)
	{
		_vertexDecl = device->createInterleavedVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD);
	}

	_vertexArray = vertexArray;
	_material = material;

	return true;
}

ATOM_BBox LightGeomRenderDatas::getBoundingbox (void) const
{
	ATOM_BBox bbox;
	bbox.setMin (ATOM_Vector3f(-1.f, -1.f, -1.f) * LIGHTGEOM_SIZE * 4.f);
	bbox.setMax (ATOM_Vector3f(1.f, 1.f, 1.f) * LIGHTGEOM_SIZE * 4.f);
	return bbox;
}

static bool displayLightGeode = true;

void LightGeode::setDisplay (bool b)
{
	displayLightGeode = b;
}

bool LightGeode::getDisplay (void)
{
	return displayLightGeode;
}

LightGeode::LightGeode (void)
{
	_renderDatas = 0;
	_contributeCulling = false;
	_lightType = ATOM_Light::Point;


	//--- wangjian added ---//
	_load_priority = ATOM_LoadPriority_IMMEDIATE;
	_load_flag = LOAD_ALLFINISHED;
	//----------------------//
}

LightGeode::~LightGeode (void)
{
	ATOM_DELETE(_renderDatas);
}

void LightGeode::setupRenderQueue (ATOM_CullVisitor *visitor)
{
	if (displayLightGeode)
	{
		ATOM_Matrix4x4f matWorld = getWorldMatrix();
		visitor->addDrawable (_renderDatas, matWorld, _renderDatas->getMaterial());
	}
}

void LightGeode::buildWorldMatrix (void) const
{
	ATOM_VisualNode::buildWorldMatrix ();
	_worldMatrix.setUpper3 (ATOM_Matrix3x3f::getIdentityMatrix());
}

void LightGeode::buildBoundingbox (void) const
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

bool LightGeode::onLoad(ATOM_RenderDevice *device)
{
	if (_renderDatas)
	{
		return true;
	}

	_renderDatas = ATOM_NEW(LightGeomRenderDatas);

	if (!_renderDatas->create (device, _lightType))
	{
		ATOM_DELETE(_renderDatas);
		_renderDatas = 0;
		return false;
	}

	return true;
}

void LightGeode::setLightType (int type)
{
	if (_lightType != type)
	{
		_lightType = type;

		if (_renderDatas)
		{
			_renderDatas->create (ATOM_GetRenderDevice(), _lightType);
		}
	}
}

int LightGeode::getLightType (void) const
{
	return _lightType;
}

bool LightGeode::rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const
{
	if (displayLightGeode)
	{
		float t;
		bool ret = ray.intersectionTestEx (getBoundingbox (), t);
		if (ret)
		{
			if (len)
			{
				*len = t;
			}
			return true;
		}
	}

	return false;
}


