#include "StdAfx.h"
#include "gridnode.h"

static const char gridMaterialSrc[] = 
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
	"	<effect name=\"default\" scheme=\"deferred\" schemelayer=\"PostShading\">										\n"
	"		<pass>														\n"
	"			<streamdefine>											\n"
	"				<stream semantec=\"position\" />					\n"
	"				<stream semantec=\"color0\" />						\n"
	"			</streamdefine>											\n"
	"			<pixelshader entry=\"ps\" target=\"ps_2_0\" />			\n"
	"		</pass>														\n"
	"	</effect>														\n"
	"</material>														\n";

class GridRenderDatas: public ATOM_Drawable
{
public:
	GridRenderDatas (void);

public:
	virtual bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);

public:
	bool create (ATOM_RenderDevice *device, unsigned width, unsigned height);
	void setColor (const ATOM_Vector4f &color);
	unsigned getWidth (void) const;
	unsigned getHeight (void) const;
	ATOM_BBox getBoundingbox (void) const;
	ATOM_Material *getMaterial (void) const;

private:
	unsigned _width;
	unsigned _height;
	ATOM_Vector4f _color;
	ATOM_AUTOREF(ATOM_VertexArray) _vertexArray;
	ATOM_AUTOPTR(ATOM_Material) _material;
	ATOM_VertexDecl _vertexDecl;
};

GridRenderDatas::GridRenderDatas (void)
{
	_width = 0;
	_height = 0;
	_vertexDecl = 0;
	_color.set(0.f, 1.f, 1.f, 1.f);
}

void GridRenderDatas::setColor (const ATOM_Vector4f &color)
{
	_color = color;
}

ATOM_Material *GridRenderDatas::getMaterial (void) const
{
	return _material.get();
}

bool GridRenderDatas::draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{
	if (_vertexArray && _vertexDecl)
	{
		device->setVertexDecl (_vertexDecl);
		device->setStreamSource (0, _vertexArray.get());

		unsigned nLines = _width + 1 + _height + 1;
		unsigned numPasses;

		material->getParameterTable()->setVector ("color", _color);
		numPasses = material->begin (device);
		for (unsigned i = 0; i < numPasses; ++i)
		{
			if (material->beginPass (device, i))
			{
				device->renderStreams (ATOM_PRIMITIVE_LINES, nLines);
				material->endPass (device, i);
			}
		}
		material->end (device);

		device->setStreamSource (0, _vertexArray.get(), 2 * sizeof(ATOM_Vector3f) * nLines);
		material->getParameterTable()->setVector ("color", ATOM_Vector4f(0.f, 0.f, 1.f, 1.f));
		numPasses = material->begin (device);
		for (unsigned i = 0; i < numPasses; ++i)
		{
			if (material->beginPass (device, i))
			{
				device->renderStreams (ATOM_PRIMITIVE_LINES, 1);
				material->endPass (device, i);
			}
		}
		material->end (device);

		device->setStreamSource (0, _vertexArray.get(), 2 * sizeof(ATOM_Vector3f) * (nLines + 1));
		material->getParameterTable()->setVector ("color", ATOM_Vector4f(1.f, 0.f, 0.f, 1.f));
		numPasses = material->begin (device);
		for (unsigned i = 0; i < numPasses; ++i)
		{
			if (material->beginPass (device, i))
			{
				device->renderStreams (ATOM_PRIMITIVE_LINES, 1);
				material->endPass (device, i);
			}
		}
		material->end (device);
	}
	return true;
}

static inline ATOM_ColorARGB getGridLineColor (float x, float z)
{
	static const ATOM_ColorARGB blue(0.f, 0.f, 1.f, 1.f);
	static const ATOM_ColorARGB red(1.f, 0.f, 0.f, 1.f);
	static const ATOM_ColorARGB white(1.f, 1.f, 1.f, 1.f);
	return white;
	if (x == 0)
		return blue;
	else if (z == 0)
		return red;
	else
		return white;
}

bool GridRenderDatas::create (ATOM_RenderDevice *device, unsigned width, unsigned height)
{
	ATOM_AUTOREF(ATOM_VertexArray) vertexArray = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD, ATOM_USAGE_STATIC, (width + 1) * 2 + (height + 1) * 2 + 4);
	if (!vertexArray)
	{
		return false;
	}

	ATOM_Vector3f *vertices = (ATOM_Vector3f*)vertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	if (!vertices)
	{
		return false;
	}

	for (unsigned i = 0; i <= width; ++i)
	{
		float x = i - width * 0.5f;
		float z = -(height * 0.5f);
		vertices->x = x;
		vertices->y = 0.f;
		vertices->z = z;
		vertices++;

		vertices->x = x;
		vertices->y = 0.f;
		vertices->z = -z;
		vertices++;
	}

	for (unsigned i = 0; i <= height; ++i)
	{
		float x = -(width * 0.5f);
		float z = i - height * 0.5f;
		vertices->x = x;
		vertices->y = 0.f;
		vertices->z = z;
		vertices++;

		vertices->x = -x;
		vertices->y = 0.f;
		vertices->z = z;
		vertices++;
	}

	vertices->x = 0;
	vertices->y = 0;
	vertices->z = -(height * 0.5f);
	vertices++;

	vertices->x = 0;
	vertices->y = 0;
	vertices->z = height * 0.5f;
	vertices++;

	vertices->x = -(width * 0.5f);
	vertices->y = 0;
	vertices->z = 0;
	vertices++;

	vertices->x = width * 0.5f;
	vertices->y = 0;
	vertices->z = 0;
	vertices++;

	vertexArray->unlock ();

	ATOM_AUTOPTR(ATOM_Material) material = ATOM_MaterialManager::createMaterialFromCoreString (device, gridMaterialSrc);
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

	_width = width;
	_height = height;

	return true;
}

ATOM_BBox GridRenderDatas::getBoundingbox (void) const
{
	ATOM_BBox bbox;
	bbox.setMin (ATOM_Vector3f(-(_width * 0.5f), 0.f, -(_height * 0.5f)));
	bbox.setMax (ATOM_Vector3f(_width * 0.5f, 0.f, _height * 0.5f));
	return bbox;
}

GridNode::GridNode (void)
{
	_renderDatas = 0;
	_width = 0;
	_height = 0;
	_contributeCulling = false;
	_skipClipTest = false;

	//--- wangjian added ---//
	_load_priority = ATOM_LoadPriority_IMMEDIATE;
	_load_flag = LOAD_ALLFINISHED;
	//----------------------//
}

GridNode::~GridNode (void)
{
	ATOM_DELETE(_renderDatas);
}

void GridNode::setupRenderQueue (ATOM_CullVisitor *visitor)
{
	visitor->addDrawable (_renderDatas, getWorldMatrix(), _renderDatas->getMaterial());
}

void GridNode::buildBoundingbox (void) const
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

bool GridNode::onLoad(ATOM_RenderDevice *device)
{
	if (_renderDatas)
	{
		return true;
	}

	if (_width == 0 || _height == 0)
	{
		return false;
	}

	_renderDatas = ATOM_NEW(GridRenderDatas);

	if (!_renderDatas->create (device, _width, _height))
	{
		ATOM_DELETE(_renderDatas);
		_renderDatas = 0;
		return false;
	}

	return true;
}

bool GridNode::setSize (unsigned width, unsigned height)
{
	if (width == 0 || height == 0)
	{
		return false;
	}

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

unsigned GridNode::getWidth (void) const
{
	return _width;
}

unsigned GridNode::getHeight (void) const
{
	return _height;
}

