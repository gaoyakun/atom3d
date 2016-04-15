#include "StdAfx.h"
#include <list>
#include "linetrail.h"

#if 0
static const char materialSrcNoBlend[] = 
  "material { \n"
  "  effect { \n"
  "   source  \" \n"
  "     texture DiffuseTexture;\n"
  "     technique t0 {\n"
  "       pass P0 {\n"
  "         Texture[0] = DiffuseTexture;\n"
  "	        MagFilter[0] = Point;\n"
  "         MinFilter[0] = Point;\n"
  "         MipFilter[0] = None;\n"
  "			AddressU[0] = Clamp;\n"
  "			AddressV[0] = Clamp;\n"
  "         CullMode = None;\n"
  "         ColorOp[0] = Modulate;\n"
  "         ColorArg1[0] = Diffuse;\n"
  "         ColorArg2[0] = Texture;\n"
  "			ColorOp[1] = Disable;\n"
  "         VertexShader = null;\n"
  "         PixelShader = null;\n"
  "       }\n"
  "     }\";\n"
  "  }\n"
  "}";

static const char materialSrc[] = 
  "material { \n"
  "  effect { \n"
  "   source  \" \n"
  "     texture DiffuseTexture;\n"
  "     technique t0 {\n"
  "       pass P0 {\n"
  "         Texture[0] = DiffuseTexture;\n"
  "	        MagFilter[0] = Point;\n"
  "         MinFilter[0] = Point;\n"
  "         MipFilter[0] = None;\n"
  "			AddressU[0] = Clamp;\n"
  "			AddressV[0] = Clamp;\n"
  "         CullMode = None;\n"
  "         ZWriteEnable = False;\n"
  "         ColorOp[0] = Modulate;\n"
  "         ColorArg1[0] = Diffuse;\n"
  "         ColorArg2[0] = Texture;\n"
  "         AlphaOp[0] = Modulate;\n"
  "         AlphaArg1[0] = Diffuse;\n"
  "         AlphaArg2[0] = Texture;\n"
  "			ColorOp[1] = Disable;\n"
  "         AlphaBlendEnable = True;\n"
  "         SrcBlend = SrcAlpha;\n"
  "         DestBlend = InvSrcAlpha;\n"
  "         VertexShader = null;\n"
  "         PixelShader = null;\n"
  "       }\n"
  "     }\";\n"
  "  }\n"
  "}";

static const char materialSrcAdd[] = 
  "material { \n"
  "  effect { \n"
  "   source  \" \n"
  "     texture DiffuseTexture;\n"
  "     technique t0 {\n"
  "       pass P0 {\n"
  "         Texture[0] = DiffuseTexture;\n"
  "	        MagFilter[0] = Point;\n"
  "         MinFilter[0] = Point;\n"
  "         MipFilter[0] = None;\n"
  "			AddressU[0] = Clamp;\n"
  "			AddressV[0] = Clamp;\n"
  "         CullMode = None;\n"
  "         ZWriteEnable = False;\n"
  "         ColorOp[0] = Modulate;\n"
  "         ColorArg1[0] = Diffuse;\n"
  "         ColorArg2[0] = Texture;\n"
  "         AlphaOp[0] = Modulate;\n"
  "         AlphaArg1[0] = Diffuse;\n"
  "         AlphaArg2[0] = Texture;\n"
  "			ColorOp[1] = Disable;\n"
  "         AlphaBlendEnable = True;\n"
  "         SrcBlend = SrcAlpha;\n"
  "         DestBlend = One;\n"
  "         VertexShader = null;\n"
  "         PixelShader = null;\n"
  "       }\n"
  "     }\";\n"
  "  }\n"
  "}";
#endif

///////////////////////////// class ATOM_LineTrailRenderDatas ///////////////////////////////

ATOM_VertexDecl ATOM_LineTrailRenderDatas::_vertexDecl = 0;

const unsigned ATOM_LineTrail::LTF_WORLDSPACE = (1<<0);
const int ATOM_LineTrail::BlendMode_Default = 0;
const int ATOM_LineTrail::BlendMode_AlphaAdditive = 1;
const int ATOM_LineTrail::BlendMode_Additive = 2;

ATOM_LineTrailRenderDatas::ATOM_LineTrailRenderDatas (ATOM_LineTrail *lineTrail)
{
	ATOM_STACK_TRACE(ATOM_LineTrailRenderDatas::ATOM_LineTrailRenderDatas);

	if (!_material)
	{
		_material = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/linetrail.mat");
	}

	if (!_vertexDecl)
	{
		_vertexDecl = ATOM_GetRenderDevice()->createInterleavedVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_PRIMARY_COLOR|ATOM_VERTEX_ATTRIB_TEX1_2);
	}

	_lineTrail = lineTrail;
}

ATOM_LineTrailRenderDatas::~ATOM_LineTrailRenderDatas (void)
{
	ATOM_STACK_TRACE(ATOM_LineTrailRenderDatas::~ATOM_LineTrailRenderDatas);
}

bool ATOM_LineTrailRenderDatas::draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{
	ATOM_STACK_TRACE(ATOM_LineTrailRenderDatas::render);

	//--- wangjian added ---//
#if 1
	bool inWorldSpace = (_lineTrail->getFlags() & ATOM_LineTrail::LTF_WORLDSPACE) != 0;
	device->setTransform (ATOM_MATRIXMODE_WORLD, inWorldSpace ? ATOM_Matrix4x4f::getIdentityMatrix() : _lineTrail->getWorldMatrix() );
#endif
	//----------------------//

	updateVertices (device, camera);

	device->setVertexDecl (_vertexDecl);
	device->setStreamSource (0, _vertexArray.get());

	switch (_lineTrail->getBlendMode ())
	{
	case ATOM_LineTrail::BlendMode_Additive:
		material->getParameterTable()->setInt ("srcBlend", ATOM_RenderAttributes::BlendFunc_One);
		material->getParameterTable()->setInt ("destBlend", ATOM_RenderAttributes::BlendFunc_One);
		break;
	case ATOM_LineTrail::BlendMode_AlphaAdditive:
		material->getParameterTable()->setInt ("srcBlend", ATOM_RenderAttributes::BlendFunc_SrcAlpha);
		material->getParameterTable()->setInt ("destBlend", ATOM_RenderAttributes::BlendFunc_One);
		break;
	case ATOM_LineTrail::BlendMode_Default:
		material->getParameterTable()->setInt ("srcBlend", ATOM_RenderAttributes::BlendFunc_SrcAlpha);
		material->getParameterTable()->setInt ("destBlend", ATOM_RenderAttributes::BlendFunc_InvSrcAlpha);
		break;
	}

	material->getParameterTable()->setTexture ("texture", _lineTrail->getTexture());

	unsigned numPasses = material->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (material->beginPass (device, pass))
		{
			device->renderStreams (ATOM_PRIMITIVE_TRIANGLE_STRIP, _lineTrail->getNumInflectionPoints() * 2 - 2);
			material->endPass (device, pass);
		}
	}
	material->end (device);

	return true;
}

ATOM_Material *ATOM_LineTrailRenderDatas::getMaterial (void) const
{
	return _material.get();
}

void ATOM_LineTrailRenderDatas::updateVertices (ATOM_RenderDevice *device, ATOM_Camera *camera)
{
	ATOM_STACK_TRACE(ATOM_LineTrailRenderDatas::updateVertices);

	unsigned numInflectionPoints = _lineTrail->getNumInflectionPoints();
	if (numInflectionPoints < 2)
	{
		return;
	}

	unsigned numVertices = numInflectionPoints * 2;

	if (!_vertexArray || _vertexArray->getNumVertices() < numVertices)
	{
		_vertexArray = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2|ATOM_VERTEX_ATTRIB_PRIMARY_COLOR, ATOM_USAGE_DYNAMIC, numVertices);
		if (!_vertexArray)
		{
			ATOM_LOGGER::error ("%s: Allocate vertex array (size=%d) failed.\n", __FUNCTION__, numVertices);
			return;
		}
	}

	bool inWorldSpace = (_lineTrail->getFlags() & ATOM_LineTrail::LTF_WORLDSPACE) != 0;
	const ATOM_Matrix4x4f &invWorldMatrix = inWorldSpace ? ATOM_Matrix4x4f::getIdentityMatrix () : _lineTrail->getInvWorldMatrix ();
	const ATOM_Matrix4x4f &viewMatrix = camera->getViewMatrix ();
	ATOM_Matrix4x4f invModelViewMatrix = invWorldMatrix >> viewMatrix;
	ATOM_Vector3f z = invModelViewMatrix.transformVectorAffine (ATOM_Vector3f(0.f, 0.f, 1.f));

	struct LTVertex
	{
		ATOM_Vector3f position;
		ATOM_ColorARGB color;
		ATOM_Vector2f uv;
	};

	LTVertex *vertices = (LTVertex*)_vertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	if (!vertices)
	{
		ATOM_LOGGER::error ("%s: Lock vertex array failed.\n", __FUNCTION__);
		return;
	}

	ATOM_Vector3f y;
	for (unsigned i = 0; i < numInflectionPoints-1; ++i)
	{
		const ATOM_LineTrail::InflectionPoint &p0 = _lineTrail->inflectionPoint (i);
		const ATOM_LineTrail::InflectionPoint &p1 = _lineTrail->inflectionPoint (i+1);

		ATOM_Vector3f x = p1.position - p0.position;
		x.normalize ();
		y.normalizeFrom (crossProduct(z, x));

		float halfWidth = p0.width * 0.5f;
		vertices->position = p0.position - y * halfWidth;
		vertices->color = p0.color;
		vertices->uv.set (0.f, 0.f);
		vertices++;

		vertices->position = p0.position + y * halfWidth;
		vertices->color = p0.color;
		vertices->uv.set (1.f, 0.f);
		vertices++;
	}

	const ATOM_LineTrail::InflectionPoint &pLast = _lineTrail->inflectionPoint (numInflectionPoints - 1);
	float halfWidth = pLast.width * 0.5f;
	vertices->position = pLast.position - y * halfWidth;
	vertices->color = pLast.color;
	vertices->uv.set (0.f, 0.f);
	vertices++;

	vertices->position = pLast.position + y * halfWidth;
	vertices->color = pLast.color;
	vertices->uv.set (1.f, 0.f);
	vertices++;

	_vertexArray->unlock ();
}

////////////////////////////////////// class ATOM_LineTrail ////////////////////////////////////////////

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_LineTrail)
	ATOM_ATTRIBUTES_BEGIN(ATOM_LineTrail)
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_LineTrail, "TextureFileName", getTextureFileName, setTextureFileName, "", "group=ATOM_LineTrail;type=vfilename;desc='贴图文件名'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_LineTrail, "BlendMode", getBlendMode, setBlendMode, ATOM_LineTrail::BlendMode_Default, "group=ATOM_LineTrail;type=int32;enum='透明融合 0 透明增亮 1 增亮 2';desc='融合方式'")
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_LineTrail, ATOM_VisualNode)

ATOM_LineTrail::ATOM_LineTrail (void)
{
	ATOM_STACK_TRACE(ATOM_LineTrail::ATOM_LineTrail);

	_renderDatas = 0;
	_flags = LTF_WORLDSPACE;

	//--- wangjian added ---//
	_load_priority = ATOM_LoadPriority_IMMEDIATE;
	_load_flag = LOAD_ALLFINISHED;
	//----------------------//
}

ATOM_LineTrail::~ATOM_LineTrail (void)
{
	ATOM_STACK_TRACE(ATOM_LineTrail::~ATOM_LineTrail);

	ATOM_DELETE(_renderDatas);
}

void ATOM_LineTrail::buildBoundingbox (void) const
{
	if (getNumInflectionPoints() == 0)
	{
		_boundingBox.setMin (ATOM_Vector3f(0.f));
		_boundingBox.setMax (ATOM_Vector3f(0.f));
	}
	else
	{
		float widthMax = 0.f;
		_boundingBox.beginExtend ();
		for (unsigned i = 0; i < getNumInflectionPoints(); ++i)
		{
			const InflectionPoint &p = inflectionPoint(i);
			_boundingBox.extend (p.position);
			if (p.width > widthMax)
			{
				widthMax = p.width;
			}
		}
		_boundingBox.setMin (_boundingBox.getMin() - ATOM_Vector3f(widthMax));
		_boundingBox.setMax (_boundingBox.getMax() + ATOM_Vector3f(widthMax));
	}

	if ((getFlags() & ATOM_LineTrail::LTF_WORLDSPACE) != 0)
	{
		const ATOM_Matrix4x4f &invWorldMatrix = getInvWorldMatrix ();
		_boundingBox.transform (invWorldMatrix);
	}
}

void ATOM_LineTrail::buildWorldBoundingbox (void) const
{
	if (getNumInflectionPoints() == 0)
	{
		_boundingBoxW.setMin (ATOM_Vector3f(0.f));
		_boundingBoxW.setMax (ATOM_Vector3f(0.f));
	}
	else
	{
		float widthMax = 0.f;
		_boundingBoxW.beginExtend ();
		for (unsigned i = 0; i < getNumInflectionPoints(); ++i)
		{
			const InflectionPoint &p = inflectionPoint(i);
			_boundingBoxW.extend (p.position);
			if (p.width > widthMax)
			{
				widthMax = p.width;
			}
		}
		_boundingBox.setMin (_boundingBox.getMin() - ATOM_Vector3f(widthMax));
		_boundingBox.setMax (_boundingBox.getMax() + ATOM_Vector3f(widthMax));
	}

	if ((getFlags() & ATOM_LineTrail::LTF_WORLDSPACE) == 0)
	{
		const ATOM_Matrix4x4f &worldMatrix = getWorldMatrix ();
		_boundingBoxW.transform (worldMatrix);
	}
}

void ATOM_LineTrail::accept (ATOM_Visitor &visitor)
{
	ATOM_STACK_TRACE(ATOM_LineTrail::accept);

	visitor.visit (*this);
}

void ATOM_LineTrail::setupRenderQueue (ATOM_CullVisitor *visitor)
{
	if (getNumInflectionPoints() > 1)
	{
		//--- wangjian modified ---//
		// 将世界变换矩阵放到draw方法中去设置
#if 0
		bool inWorldSpace = (getFlags() & LTF_WORLDSPACE) != 0;
		visitor->addDrawable (_renderDatas, inWorldSpace ? ATOM_Matrix4x4f::getIdentityMatrix() : getWorldMatrix(), _renderDatas->getMaterial());
#else
		visitor->addDrawable (_renderDatas, ATOM_Matrix4x4f::getIdentityMatrix(), _renderDatas->getMaterial());
#endif
		//-------------------------//
	}
}

bool ATOM_LineTrail::rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const
{
	float d = FLT_MAX;
	return ray.intersectionTestEx (getBoundingbox(), d) && d >= 0.f;
}

bool ATOM_LineTrail::supportFixedFunction (void) const
{
	return true;
}

bool ATOM_LineTrail::onLoad(ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_LineTrail::onLoad);

	if (!_textureFileName.empty ())
	{
		_texture = ATOM_CreateTextureResource (_textureFileName.c_str());
	}
	if (!_texture)
	{
		_texture = ATOM_GetColorTexture (ATOM_ColorARGB(1.f, 1.f, 1.f, 1.f));
	}

	if (!_renderDatas)
	{
		_renderDatas = ATOM_NEW(ATOM_LineTrailRenderDatas, this);
	}

	getLoadInterface ()->setLoadingState (ATOM_LoadInterface::LS_LOADED);
	invalidateBoundingbox ();
	return true;
}

void ATOM_LineTrail::assign(ATOM_Node *other) const
{
	ATOM_VisualNode::assign (other);
	ATOM_LineTrail *otherTrail = dynamic_cast<ATOM_LineTrail*>(other);
	if (otherTrail)
	{
		otherTrail->_inflectionPoints = _inflectionPoints;
		otherTrail->_flags = _flags;
		otherTrail->_skipClipTest = _skipClipTest;
		otherTrail->invalidateBoundingbox ();
	}
}

void ATOM_LineTrail::appendInflectionPoint (const InflectionPoint &p)
{
	_inflectionPoints.push_back (p);
	invalidLength (_inflectionPoints.size() - 1);
	invalidateBoundingbox ();
}

void ATOM_LineTrail::insertInflectionPoint (unsigned pos, const InflectionPoint &p)
{
	_inflectionPoints.insert (_inflectionPoints.begin()+pos, p);
	invalidLength (pos);
	invalidateBoundingbox ();
}

void ATOM_LineTrail::removeInflectionPoint (unsigned pos)
{
	invalidLength (pos);
	_inflectionPoints.erase (_inflectionPoints.begin()+pos);
	invalidateBoundingbox ();
}

void ATOM_LineTrail::clearInflectionPoints (void)
{
	_inflectionPoints.resize (0);
	invalidateBoundingbox ();
}

void ATOM_LineTrail::setNumInflectionPoints (unsigned count)
{
	_inflectionPoints.resize (count);
	invalidateBoundingbox ();
}

unsigned ATOM_LineTrail::getNumInflectionPoints (void) const
{
	return _inflectionPoints.size ();
}

const ATOM_LineTrail::InflectionPoint &ATOM_LineTrail::inflectionPoint (unsigned pos) const
{
	return _inflectionPoints[pos];
}

void ATOM_LineTrail::invalidLength (unsigned pos)
{
	_inflectionPoints[pos].lengthDirty = true;
	if (pos > 0)
	{
		_inflectionPoints[pos-1].lengthDirty = true;
	}
	if (pos < _inflectionPoints.size()-1)
	{
		_inflectionPoints[pos+1].lengthDirty = true;
	}
}

ATOM_LineTrail::InflectionPoint &ATOM_LineTrail::inflectionPoint (unsigned pos)
{
	invalidLength (pos);
	invalidateBoundingbox ();
	return _inflectionPoints[pos];
}

void ATOM_LineTrail::setFlags (unsigned flags)
{
	if (flags != _flags)
	{
		_flags = flags;
		invalidateBoundingbox ();
	}
}

unsigned ATOM_LineTrail::getFlags (void) const
{
	return _flags;
}

void ATOM_LineTrail::setTexture (ATOM_Texture *texture)
{
	if (texture != _texture)
	{
		_texture = texture;
		_textureFileName = "";
	}
}

ATOM_Texture *ATOM_LineTrail::getTexture (void) const
{
	return _texture.get();
}

void ATOM_LineTrail::setTextureFileName (const ATOM_STRING &fileName)
{
	_textureFileName = fileName;
	_texture = ATOM_CreateTextureResource (fileName.c_str());
}

const ATOM_STRING &ATOM_LineTrail::getTextureFileName (void) const
{
	return _textureFileName;
}

float ATOM_LineTrail::getSegmentLength (unsigned pos) const
{
	const InflectionPoint &p = _inflectionPoints[pos];
	if (p.lengthDirty)
	{
		p.lengthDirty = false;
		if (pos == _inflectionPoints.size() - 1)
		{
			p.length = 0.f;
		}
		else
		{
			p.length = (p.position - _inflectionPoints[pos+1].position).getLength ();
		}
	}
	return p.length;
}

float ATOM_LineTrail::getTrailLength (void) const
{
	float length = 0.f;

	for (unsigned i = 0; i < _inflectionPoints.size(); ++i)
	{
		length += getSegmentLength (i);
	}

	return length;
}

void ATOM_LineTrail::setBlendMode (int blendMode)
{
	_blendMode = blendMode;
}

int ATOM_LineTrail::getBlendMode (void) const
{
	return _blendMode;
}


