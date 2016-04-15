#include "StdAfx.h"
#include "ATOM3DX.h"
#include "trackballnode.h"

static const char circleMaterialSrc[] =
	"<?xml version=\"1.0\"?>											\n"
	"<material auto=\"1\">										\n"
	"	<param name=\"bf\" type=\"float4\" default=\"0.2,0.2,0.2,1.0\" />						\n"
	"	<code><![CDATA[																	\n"
	"		struct v2p											\n"
	"		{							\n"
	"			float4 position : POSITION0;	\n"
	"			float4 diffuse : COLOR0;	\n"
	"		};	\n"
	"		float4 ps (in v2p IN) : COLOR0	\n"
	"		{	\n"
	"			return IN.diffuse;	\n"
	"		}	\n"
	"	]]></code>	\n"
	"	<effect name=\"default\" scheme=\"deferred\" schemelayer=\"forward\" >										\n"
	"		<pass>														\n"
	"			<streamdefine>											\n"
	"				<stream semantec=\"position\" />					\n"
	"				<stream semantec=\"color0\" />						\n"
	"			</streamdefine>											\n"
	"			<rasterizer_state>										\n"
	"				<cullmode value=\"none\" />							\n"
	"			</rasterizer_state>										\n"
	"			<pixelshader entry=\"ps\" target=\"ps_2_0\" />			\n"
	"		</pass>														\n"
	"		<pass>														\n"
	"			<streamdefine>											\n"
	"				<stream semantec=\"position\" />					\n"
	"				<stream semantec=\"color0\" />						\n"
	"			</streamdefine>											\n"
	"			<alphablend_state>										\n"
	"				<enable value=\"true\" />							\n"
	"				<srcblend value=\"constant\" />						\n"
	"				<destblend value=\"invconstant\" />					\n"
	"				<blendcolor value=\"$bf\" />						\n"
	"			</alphablend_state>										\n"
	"			<depth_state>											\n"
	"				<zfunc value=\"always\" />							\n"
	"			</depth_state>											\n"
	"			<pixelshader entry=\"ps\" target=\"ps_2_0\" />			\n"
	"		</pass>														\n"
	"	</effect>														\n"
	"</material>														\n";

static const char sphereMaterialSrc[] =
	"<?xml version=\"1.0\"?>											\n"
	"<material auto=\"1\">										\n"
	"	<param name=\"bf\" type=\"float4\" default=\"0.4,0.4,0.4,1.0\" />						\n"
	"	<param name=\"color\" type=\"float4\" default=\"0.1,0.1,0.1,1.0\" />					\n"
	"	<code><![CDATA[																	\n"
	"		struct v2p											\n"
	"		{							\n"
	"			float4 position : POSITION0;	\n"
	"			float4 diffuse : COLOR0;	\n"
	"		};	\n"
	"		float4 ps (in v2p IN) : COLOR0	\n"
	"		{	\n"
	"			return color;	\n"
	"		}	\n"
	"		float4 ps_diffuse (in v2p IN) : COLOR0	\n"
	"		{	\n"
	"			return IN.diffuse;	\n"
	"		}	\n"
	"	]]></code>	\n"
	"	<effect name=\"default\" scheme=\"deferred\" schemelayer=\"forward\" >										\n"
	"		<pass>														\n"
	"			<streamdefine>											\n"
	"				<stream semantec=\"position\" />					\n"
	"			</streamdefine>											\n"
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
	"			<pixelshader entry=\"ps_diffuse\" target=\"ps_2_0\" />	\n"
	"		</pass>														\n"
	"	</effect>														\n"
	"	<effect name=\"default\" scheme=\"deferred\" schemelayer=\"forward\">										\n"
	"		<pass>														\n"
	"			<streamdefine>											\n"
	"				<stream semantec=\"position\" />					\n"
	"				<stream semantec=\"color0\" />						\n"
	"			</streamdefine>											\n"
	"			<rasterizer_state>										\n"
	"				<cullmode value=\"none\" />							\n"
	"			</rasterizer_state>										\n"
	"			<pixelshader entry=\"ps_diffuse\" target=\"ps_2_0\" />	\n"
	"		</pass>														\n"
	"		<pass>														\n"
	"			<streamdefine>											\n"
	"				<stream semantec=\"position\" />					\n"
	"				<stream semantec=\"color0\" />						\n"
	"			</streamdefine>											\n"
	"			<alphablend_state>										\n"
	"				<enable value=\"true\" />							\n"
	"				<srcblend value=\"constant\" />						\n"
	"				<destblend value=\"invconstant\" />					\n"
	"				<blendcolor value=\"$bf\" />						\n"
	"			</alphablend_state>										\n"
	"			<depth_state>											\n"
	"				<zfunc value=\"always\" />							\n"
	"			</depth_state>											\n"
	"			<pixelshader entry=\"ps_diffuse\" target=\"ps_2_0\" />	\n"
	"		</pass>														\n"
	"	</effect>														\n"
	"</material>														\n";

static const int smoothLevel = 36;
static const int numRings = 16;
static const int numSegs = 12;

class TrackBallRenderDatas: public ATOM_Drawable
{
public:
	TrackBallRenderDatas (void);
	virtual ~TrackBallRenderDatas (void);

public:
	virtual bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);

public:
	bool create (ATOM_RenderDevice *device, float radius);
	float getRadius (void) const;
	ATOM_BBox getBoundingbox (void) const;
	ATOM_Material *getCircleMaterial (void) const;
	ATOM_Material *getSphereMaterial (void) const;

private:
	float _radius;
	ATOM_AUTOREF(ATOM_VertexArray) _vertexArray;
	ATOM_AUTOREF(ATOM_VertexArray) _colorArray;
	ATOM_AUTOREF(ATOM_IndexArray) _indexArray;
	ATOM_AUTOPTR(ATOM_Material) _material;
	ATOM_VertexDecl _vertexDecl;

	ATOM_AUTOREF(ATOM_VertexArray) _vertexArraySphere;
	ATOM_AUTOREF(ATOM_IndexArray) _indexArraySphere;
	ATOM_AUTOPTR(ATOM_Material) _materialSphere;
	ATOM_VertexDecl _vertexDeclSphere;
};

TrackBallRenderDatas::TrackBallRenderDatas (void)
{
	_radius = 0.f;
	_vertexDecl = 0;
	_vertexDeclSphere = 0;
}

TrackBallRenderDatas::~TrackBallRenderDatas (void)
{
}

ATOM_Material *TrackBallRenderDatas::getCircleMaterial (void) const
{
	return _material.get();
}

ATOM_Material *TrackBallRenderDatas::getSphereMaterial (void) const
{
	return _materialSphere.get();
}

bool TrackBallRenderDatas::draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{
	if (!material)
	{
		return false;
	}

	if (material == _material && _vertexArray && _colorArray && _indexArray && _vertexDecl)
	{
		device->setVertexDecl (_vertexDecl);
		device->setStreamSource (0, _vertexArray.get());
		device->setStreamSource (1, _colorArray.get());

		unsigned numPasses = material->begin (device);
		for (unsigned i = 0; i < numPasses; ++i)
		{
			if (material->beginPass (device, i))
			{
				device->renderStreamsIndexed (_indexArray.get(), ATOM_PRIMITIVE_LINES, _indexArray->getNumIndices() / 2);
				material->endPass (device, i);
			}
		}
		material->end (device);
	}
	else if (material == _materialSphere && _vertexArraySphere && _indexArraySphere && _vertexDeclSphere)
	{
		device->setVertexDecl (_vertexDeclSphere);
		device->setStreamSource (0, _vertexArraySphere.get());

		unsigned numPasses = material->begin (device);
		{
			for (unsigned i = 0; i < numPasses; ++i)
			{
				if (material->beginPass (device, i))
				{
					device->renderStreamsIndexed (_indexArraySphere.get(), ATOM_PRIMITIVE_TRIANGLE_STRIP, _indexArraySphere->getNumIndices() - 2);
					material->endPass (device, i);
				}
			}
			material->end (device);
		}
	}
	return true;
}

bool TrackBallRenderDatas::create (ATOM_RenderDevice *device, float radius)
{
	unsigned numVertices, numIndices;
	const ATOM_Vector3f center(0.f, 0.f, 0.f);
	const int numCircles = 3;
	const ATOM_Vector3f axises[numCircles] = {
		ATOM_Vector3f(1.f, 0.f, 0.f),
		ATOM_Vector3f(0.f, 1.f, 0.f),
		ATOM_Vector3f(0.f, 0.f, 1.f),
	};
	const ATOM_ColorARGB colors[numCircles] = {
		ATOM_ColorARGB(1.f, 0.f, 0.f, 1.f),
		ATOM_ColorARGB(0.f, 1.f, 0.f, 1.f),
		ATOM_ColorARGB(0.f, 0.f, 1.f, 1.f),
	};

	// Calculate the vertex count and index count
	ATOMX_CreateCircle (center, smoothLevel, radius, axises[0], 0, 0, &numVertices, &numIndices, 0);

	// Generate vertices and indices
	ATOM_AUTOREF(ATOM_VertexArray) vertexArray = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD, ATOM_USAGE_STATIC, numVertices * numCircles);
	if (!vertexArray)
	{
		return false;
	}
	ATOM_AUTOREF(ATOM_IndexArray) indexArray = device->allocIndexArray (ATOM_USAGE_STATIC, numIndices * numCircles, false, true);
	if (!indexArray)
	{
		return false;
	}
	ATOM_Vector3f *v = (ATOM_Vector3f*)vertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	unsigned short *ind = (unsigned short*)indexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	for (unsigned i = 0; i < numCircles; ++i)
	{
		ATOMX_CreateCircle (center, smoothLevel, radius, axises[i], v + numVertices * i, ind + numIndices * i, 0, 0, numVertices * i);
	}
	vertexArray->unlock ();
	indexArray->unlock ();

	// Generate colors
	ATOM_AUTOREF(ATOM_VertexArray) colorArray = device->allocVertexArray (ATOM_VERTEX_ATTRIB_PRIMARY_COLOR, ATOM_USAGE_STATIC, numVertices * numCircles);
	if (!colorArray)
	{
		return false;
	}
	ATOM_ColorARGB *c = (ATOM_ColorARGB*)colorArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	for (unsigned i = 0; i < numCircles; ++i)
	{
		for (unsigned j = 0; j < numVertices; ++j)
		{
			c[i * numVertices + j] = colors[i];
		}
	}
	colorArray->unlock ();

	// Create material
	ATOM_AUTOPTR(ATOM_Material) material = ATOM_MaterialManager::createMaterialFromCoreString (device, circleMaterialSrc);

	if (!_vertexDecl)
	{
		_vertexDecl = device->createMultiStreamVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_PRIMARY_COLOR);
	}

	unsigned numSphereVerts, numSphereIndices;
	ATOMX_CreateSphere (ATOM_Vector3f(0.f, 0.f, 0.f), 20, 20, radius, 0, 0, &numSphereVerts, &numSphereIndices, 0);
	ATOM_AUTOREF(ATOM_VertexArray) vertexArraySP = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD, ATOM_USAGE_STATIC, numSphereVerts);
	if (!vertexArraySP)
	{
		return false;
	}
	ATOM_AUTOREF(ATOM_IndexArray) indexArraySP = device->allocIndexArray (ATOM_USAGE_STATIC, numSphereIndices, false, true);
	if (!indexArraySP)
	{
		return false;
	}
	ATOM_Vector3f *vSP = (ATOM_Vector3f*)vertexArraySP->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	unsigned short *iSP = (unsigned short*)indexArraySP->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	ATOMX_CreateSphere (ATOM_Vector3f(0.f, 0.f, 0.f), 20, 20, radius, vSP, iSP, 0, 0, 0);
	vertexArraySP->unlock ();
	indexArraySP->unlock ();

	if (!_vertexDeclSphere)
	{
		_vertexDeclSphere = device->createMultiStreamVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD);
	}

	// Create material
	ATOM_AUTOPTR(ATOM_Material) materialSphere = ATOM_MaterialManager::createMaterialFromCoreString (device, sphereMaterialSrc);

	_vertexArray = vertexArray;
	_colorArray = colorArray;
	_indexArray = indexArray;
	_material = material;
	_materialSphere = materialSphere;

	_vertexArraySphere = vertexArraySP;
	_indexArraySphere = indexArraySP;

	_radius = radius;

	return true;
}

float TrackBallRenderDatas::getRadius (void) const
{
	return _radius;
}

ATOM_BBox TrackBallRenderDatas::getBoundingbox (void) const
{
	return ATOM_BBox(ATOM_Vector3f(-_radius, -_radius, -_radius), ATOM_Vector3f(_radius, _radius, _radius));
}

ATOMX_TrackBallNode::ATOMX_TrackBallNode (void)
{
	_renderDatas = 0;
	_radius = 0;
	_hitAxis = ATOMX_AXIS_NONE;
}

ATOMX_TrackBallNode::~ATOMX_TrackBallNode (void)
{
	ATOM_DELETE(_renderDatas);
}

void ATOMX_TrackBallNode::buildBoundingbox (void) const
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

void ATOMX_TrackBallNode::setupRenderQueue (ATOM_CullVisitor *visitor)
{
	const ATOM_Matrix4x4f &worldMatrix = getWorldMatrix();
	visitor->addDrawable (_renderDatas, worldMatrix, _renderDatas->getCircleMaterial());
	visitor->addDrawable (_renderDatas, worldMatrix, _renderDatas->getSphereMaterial());
}

static bool raySphereIntersection (const ATOM_Vector3f &p1, const ATOM_Vector3f &p2, ATOM_Vector3f &sc, float r, float *d, float *d1, float *d2)
{
   ATOM_Vector3f dp = p2 - p1;

   float a = dp.getSquaredLength ();
   float b = 2.f * dotProduct (dp, p1 - sc);
   float c = sc.getSquaredLength ();
   c += p1.getSquaredLength ();
   c -= 2.f * dotProduct(sc, p1);
   c -= r * r;
   float bb4ac = b * b - 4 * a * c;
   if (ATOM_abs(a) < 0.0001f || bb4ac < 0.f)
   {
      return false;
   }

   *d1 = (-b + ATOM_sqrt(bb4ac)) / (2.f * a);
   *d2 = (-b - ATOM_sqrt(bb4ac)) / (2.f * a);
   if (*d1 < 0.f)
   {
	   if (*d2 < 0.f)
	   {
		   return false;
	   }
	   else
	   {
		   *d = *d2;
		   return true;
	   }
   }
   else
   {
	   if (*d2 < 0.f)
	   {
		   *d = *d1;
		   return true;
	   }
	   else
	   {
		   *d = ATOM_min2(*d1, *d2);
		   return true;
	   }
   }
}

static bool raySphereIntersection2 (const ATOM_Vector3f &p1, const ATOM_Vector3f &p2, float r, float *d, float *d1, float *d2)
{
   ATOM_Vector3f dp = p2 - p1;

   double a = dp.getSquaredLength ();
   double b = 2.f * dotProduct (dp, p1);
   double c = p1.getSquaredLength () - r * r;
   double bb4ac = b * b - 4 * a * c;
   if (ATOM_abs(a) < 0.0001f || bb4ac < 0.f)
   {
      return false;
   }

   *d1 = (-b + sqrt(bb4ac)) / (2.0 * a);
   *d2 = (-b - sqrt(bb4ac)) / (2.0 * a);
   if (*d1 < 0.f)
   {
	   if (*d2 < 0.f)
	   {
		   return false;
	   }
	   else
	   {
		   *d = *d2;
		   return true;
	   }
   }
   else
   {
	   if (*d2 < 0.f)
	   {
		   *d = *d1;
		   return true;
	   }
	   else
	   {
		   *d = ATOM_min2(*d1, *d2);
		   return true;
	   }
   }
}

bool ATOMX_TrackBallNode::worldRayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray) const
{
#if 0
	ATOM_AABBTree *aabbTree = _renderDatas->getAABBTree ();
	ATOM_Vector3f v0 = ray.getOrigin ();
	ATOM_Vector3f v1 = v0 + ray.getDirection ();
	const ATOM_Matrix4x4f &worldmatrix = getWorldMatrix ();
	ATOM_Matrix4x4f invWorldMatrix;
	invWorldMatrix.invertAffineFrom (worldmatrix);
	v0 = invWorldMatrix.transformPoint (v0);
	v1 = invWorldMatrix.transformPoint (v1);
	ATOM_Vector3f dir = v1 - v0;

	ATOM_Ray rayObjSpc(v0, dir);
	float distance;
	int triIndex = aabbTree->rayIntersectEx (rayObjSpc, distance, 0);
	_renderDatas->setTriIndex (triIndex);
	if (triIndex >= 0)
	{
		ATOM_Vector3f hitPoint = rayObjSpc.getOrigin() + rayObjSpc.getDirection() * distance;
		ATOM_LOGGER::log ("(%f, %f, %f)\n", hitPoint.x, hitPoint.y, hitPoint.z);
		return true;
	}
	else
	{
		ATOM_LOGGER::log ("Not hit\n");
		return false;
	}
#else
	const float screenDistanceValue = 5.f;
	const ATOM_Matrix4x4f &worldmatrix = getWorldMatrix ();
	const ATOM_Matrix4x4f &invWorldMatrix = getInvWorldMatrix ();
	ATOM_Vector3f originObj = invWorldMatrix.transformPointAffine(ray.getOrigin());
	ATOM_Vector3f endObj = invWorldMatrix.transformPointAffine(ray.getOrigin() + ray.getDirection() * 1000.f);
	ATOM_Ray rayObj(originObj, endObj - originObj);

	ATOM_Vector3f position, scale;
	ATOM_Matrix4x4f rotation;
	worldmatrix.decompose (position, rotation, scale);

	float distance;
	_hitAxis = ATOMX_AXIS_NONE;
	float distances[2];
	float dist[2];
	int axis[2];
	ATOM_Vector3f collidPointW1[2];
	ATOM_Vector3f collidPointW2[2];
	ATOM_Vector3f t(0.f, 0.f, 0.f);
	if (raySphereIntersection (originObj, endObj, t, _radius, &distance, &distances[0], &distances[1]))
	{
		ATOM_Vector3f dir = endObj - originObj;
		//dir.normalize ();

		for (unsigned i = 0; i < 2; ++i)
		{
			ATOM_Vector3f hitPoint = originObj + dir * distances[i];
			collidPointW1[i] = hitPoint;

			float distanceX = ATOM_abs (hitPoint.x);
			float distanceY = ATOM_abs (hitPoint.y);
			float distanceZ = ATOM_abs (hitPoint.z);
			dist[i] = distanceX;
			axis[i] = ATOMX_AXIS_X;
			if (distanceY < dist[i])
			{
				axis[i] = ATOMX_AXIS_Y;
				dist[i] = distanceY;
			}
			if (distanceZ < dist[i])
			{
				axis[i] = ATOMX_AXIS_Z;
				dist[i] = distanceZ;
			}

			switch (axis[i])
			{
			case ATOMX_AXIS_X:
				collidPointW2[i] = hitPoint + ATOM_Vector3f(distanceX, 0.f, 0.f);
				break;
			case ATOMX_AXIS_Y:
				collidPointW2[i] = hitPoint + ATOM_Vector3f(0.f, distanceY, 0.f);
				break;
			case ATOMX_AXIS_Z:
				collidPointW2[i] = hitPoint + ATOM_Vector3f(0.f, 0.f, distanceZ);
				break;
			}
		}

		_hitPointNearest = (distances[0] < distances[1]) ? collidPointW1[0] : collidPointW1[1];
		_hitPointFar = (distances[0] < distances[1]) ? collidPointW1[1] : collidPointW1[0];

#if 0
		_hitAxis = ATOMX_AXIS_XYZ;
		_hitLength = ATOM_min2(distances[0], distances[1]);
#else
		if (axis[0] == ATOMX_AXIS_XYZ)
		{
			if (axis[1] == ATOMX_AXIS_XYZ)
			{
				_hitAxis = ATOMX_AXIS_XYZ;
				_hitPoint = _hitPointNearest;
			}
			else
			{
				float screenDistance = getScreenDistance (camera, collidPointW1[1], collidPointW2[1]);
				if (screenDistance > screenDistanceValue)
				{
					_hitAxis = ATOMX_AXIS_XYZ;
					_hitPoint = _hitPointNearest;
				}
				else
				{
					_hitAxis = axis[1];
					_hitPoint = collidPointW1[1];
				}
			}
		}
		else
		{
			if (axis[1] == ATOMX_AXIS_XYZ)
			{
				float screenDistance = getScreenDistance (camera, collidPointW1[0], collidPointW2[0]);
				if (screenDistance > screenDistanceValue)
				{
					_hitAxis = ATOMX_AXIS_XYZ;
					_hitPoint = _hitPointNearest;
				}
				else
				{
					_hitAxis = axis[0];
					_hitPoint = collidPointW1[0];
				}
			}
			else
			{
				float screenDistance0 = getScreenDistance (camera, collidPointW1[0], collidPointW2[0]);
				float screenDistance1 = getScreenDistance (camera, collidPointW1[1], collidPointW2[1]);
				if (screenDistance0 > screenDistanceValue && screenDistance1 > screenDistanceValue)
				{
					_hitAxis = ATOMX_AXIS_XYZ;
					_hitPoint = _hitPointNearest;
				}
				else if (screenDistance0 < screenDistance1)
				{
					_hitAxis = axis[0];
					_hitPoint = collidPointW1[0];
				}
				else
				{
					_hitAxis = axis[1];
					_hitPoint = collidPointW1[1];
				}
			}
		}
#endif

		_hitPoint = worldmatrix.transformPoint (_hitPoint);
		_hitPointNearest = worldmatrix.transformPoint (_hitPointNearest);
		_hitPointFar = worldmatrix.transformPoint (_hitPointFar);

		return true;
	}
	else
	{
		return false;
	}
#endif
}

bool ATOMX_TrackBallNode::rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const
{
#if 0
	ATOM_AABBTree *aabbTree = _renderDatas->getAABBTree ();
	float distance;
	int triIndex = aabbTree->rayIntersectEx (ray, distance, 0);
	_renderDatas->setTriIndex (triIndex);
	if (triIndex >= 0)
	{
		if (len)
		{
			*len = distance;
		}
		ATOM_Vector3f dir = getWorldMatrix().transformVectorAffine (distance * ray.getDirection());
		ATOM_Vector3f hitPoint = ray.getOrigin() + ray.getDirection() * dir.getLength();
		ATOM_LOGGER::log ("(%f, %f, %f)\n", hitPoint.x, hitPoint.y, hitPoint.z);
		return true;
	}
	else
	{
		ATOM_LOGGER::log ("Not hit\n");
		return false;
	}
#else
	float distance, d1, d2;
	if (raySphereIntersection2 (ray.getOrigin(), ray.getOrigin() + ray.getDirection(), _radius, &distance, &d1, &d2))
	{
		if (len)
		{
			*len = distance;
		}
		return true;
	}
	return false;
#endif
}

bool ATOMX_TrackBallNode::setRadius (float radius)
{
	if (_radius != radius)
	{
		if (_renderDatas)
		{
			if (!_renderDatas->create (ATOM_GetRenderDevice(), radius))
			{
				return false;
			}
		}
		_radius = radius;
	}

	return true;
}

float ATOMX_TrackBallNode::getRadius (void) const
{
	return _radius;
}

bool ATOMX_TrackBallNode::onLoad(ATOM_RenderDevice *device)
{
	if (_renderDatas)
	{
		return true;
	}

	if (_radius == 0.f)
	{
		return false;
	}

	_renderDatas = ATOM_NEW(TrackBallRenderDatas);

	if (!_renderDatas->create (device, _radius))
	{
		ATOM_DELETE(_renderDatas);
		_renderDatas = 0;
		return false;
	}

	return true;
}

float ATOMX_TrackBallNode::getScreenDistance (ATOM_Camera *camera, const ATOM_Vector3f &worldPos1, const ATOM_Vector3f &worldPos2) const
{
	ATOM_Matrix4x4f matrixView = camera->getViewMatrix();
	ATOM_Matrix4x4f matrixProj = camera->getProjectionMatrix();
	ATOM_Matrix4x4f matrixWorld = getWorldMatrix ();
	matrixView.invertAffine ();

	ATOM_Matrix4x4f matrixMVP = matrixProj >> matrixView >> matrixWorld;
	ATOM_Vector4f v0(worldPos1.x, worldPos1.y, worldPos1.z, 1.f);
	ATOM_Vector4f v1(worldPos2.x, worldPos2.y, worldPos2.z, 1.f);
	v0 = matrixMVP.transformPoint (v0);
	v0 /= v0.w;
	v1 = matrixMVP.transformPoint (v1);
	v1 /= v1.w;
	float viewportW = camera->getViewport().size.w;
	float viewportH = camera->getViewport().size.h;
	float deltaX = 0.5f * viewportW * (v0.x - v1.x);
	float deltaY = 0.5f * viewportH * (v0.y - v1.y);
	return ATOM_sqrt(deltaX * deltaX + deltaY * deltaY);
}

float ATOMX_TrackBallNode::getScreenDistance (ATOM_Camera *camera, const ATOM_Vector3f &worldPos, float distance) const
{
	return ATOMX_MeasureScreenDistance (camera, worldPos, distance);
}

int ATOMX_TrackBallNode::getHitAxis (void) const
{
	return _hitAxis;
}

const ATOM_Vector3f &ATOMX_TrackBallNode::getHitPoint (void) const
{
	return _hitPoint;
}

const ATOM_Vector3f &ATOMX_TrackBallNode::getHitPointNearest (void) const
{
	return _hitPointNearest;
}

const ATOM_Vector3f &ATOMX_TrackBallNode::getHitPointFar (void) const
{
	return _hitPointFar;
}

