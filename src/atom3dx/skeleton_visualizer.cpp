#include "StdAfx.h"
#include "ATOM3DX.h"
#include "skeleton_visualizer.h"

static const char *materialSrc =
	"<?xml version=\"1.0\"?>											\n"
	"<material auto=\"1\">												\n"
	"	<param name=\"diffuseColor\" type=\"float4\" default=\"1.0,1.0,1.0,1.0\" />		\n"
	"	<code><![CDATA[																	\n"
	"		struct v2p											\n"
	"		{							\n"
	"			float4 position : POSITION0;	\n"
	"		};	\n"
	"		float4 ps (in v2p IN) : COLOR0	\n"
	"		{	\n"
	"			return diffuseColor;	\n"
	"		}	\n"
	"	]]></code>	\n"
	"	<effect name=\"default\" scheme=\"deferred\" schemelayer=\"forward\" >										\n"
	"		<pass>														\n"
	"			<streamdefine>											\n"
	"				<stream semantec=\"position\" />					\n"
	"			</streamdefine>											\n"
	"			<pixelshader entry=\"ps\" target=\"ps_2_0\" />			\n"
	"		</pass>														\n"
	"	</effect>														\n"
	"</material>														\n";

ATOMX_SkeletonVisualizer::ATOMX_SkeletonVisualizer (void)
{
	_jointRadius = 0.1f;
	_vertexDecl = 0;
}

ATOMX_SkeletonVisualizer::~ATOMX_SkeletonVisualizer (void)
{
	if (_vertexDecl)
	{
		ATOM_RenderDevice *device = ATOM_GetRenderDevice();
		if (device)
		{
			device->destroyVertexDecl (_vertexDecl);
		}
		_vertexDecl = 0;
	}
}

void ATOMX_SkeletonVisualizer::setSource (ATOM_Geode *source)
{
	if (_source.get() != source)
	{
		_source = source;
		_vertices = 0;
		_indices = 0;
	}
}

ATOM_Material *ATOMX_SkeletonVisualizer::getMaterial (void) const
{
	return _material.get();
}

void ATOMX_SkeletonVisualizer::draw (ATOM_Material *material)
{
	if (_vertices && _indices)
	{
		ATOM_RenderDevice *device = ATOM_GetRenderDevice ();
		device->setVertexDecl (_vertexDecl);

		unsigned numPasses = material->begin (device);
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (material->beginPass (device, pass))
			{
				device->setStreamSource (0, _vertices.get());
				device->renderStreamsIndexed (_indices.get(), ATOM_PRIMITIVE_TRIANGLES, _indices->getNumIndices() / 3);
				material->endPass (device, pass);
			}
		}
		material->end (device);
	}
}

bool ATOMX_SkeletonVisualizer::prepareForRender (void)
{
	ATOM_RenderDevice *device = ATOM_GetRenderDevice();
	if (!device)
	{
		return false;
	}

	if (!_vertices || !_indices)
	{
		ATOM_Skeleton *skeleton = _source ? _source->getSkeleton() : 0;
		unsigned numJoints = skeleton ? skeleton->getNumAttachPoints () : 0;
		if (numJoints < 2)
		{
			return false;
		}

		const ATOM_VECTOR<int> parents = skeleton->getJointParents();
		if (parents.empty ())
		{
			return false;
		}
		unsigned numChildJoints = 0;
		for (unsigned i = 0; i < numJoints; ++i)
		{
			if (parents[i] >= 0)
			{
				++numChildJoints;
			}
		}

		unsigned numBoneVerts = 0;
		unsigned numBoneIndices = 0;
		ATOMX_CreateCone (ATOM_Vector3f(0.f, 0.f, 0.f), false, 4, ATOM_Vector3f(1.f, 0.f, 0.f), 1.f, 1.f, 0, 0, &numBoneVerts, &numBoneIndices, 0);

		unsigned totalVertexCountBone = numChildJoints * numBoneVerts;
		unsigned totalIndexCountBone = numChildJoints * numBoneIndices;

		_vertices = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD, ATOM_USAGE_DYNAMIC, totalVertexCountBone, true);
		_indices = device->allocIndexArray (ATOM_USAGE_STATIC, totalIndexCountBone, false, true);
	}

	if (!_material)
	{
		_material = ATOM_MaterialManager::createMaterialFromCoreString (device, materialSrc);
		if (!_material)
		{
			_vertices = 0;
			_indices = 0;
			return false;
		}
		_material->setActiveEffect ("default");
	}

	if (!_vertexDecl)
	{
		_vertexDecl = ATOM_GetRenderDevice()->createMultiStreamVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD);
	}

	return true;
}

void ATOMX_SkeletonVisualizer::updateVertices (void)
{
	if (!prepareForRender ())
	{
		return;
	}

	unsigned numJoints = _source->getSkeleton()->getNumAttachPoints ();
	const ATOM_VECTOR<int> parents = _source->getSkeleton()->getJointParents();
	static ATOM_VECTOR<ATOM_Matrix4x4f> boneMatrices;
	boneMatrices.resize (numJoints);

	ATOM_Vector3f *pVerts = (ATOM_Vector3f*)_vertices->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	unsigned short *pIndices = (unsigned short*)_indices->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	unsigned short indexOffset = 0;

	for (unsigned i = 0; i < numJoints; ++i)
	{
		_source->getActionMixer()->getBoneMatrix (i, boneMatrices[i], false);

		int parent = parents[i];
		if (parent >= 0)
		{
			ATOM_Vector3f jointPos (boneMatrices[i].m30, boneMatrices[i].m31, boneMatrices[i].m32);
			ATOM_Vector3f parentPos (boneMatrices[parent].m30, boneMatrices[parent].m31, boneMatrices[parent].m32);
			ATOM_Vector3f axis = jointPos - parentPos;

			unsigned numVerts, numIndices;
			ATOMX_CreateCone (parentPos, false, 4, axis, axis.getLength() * 0.1f/*_jointRadius*/, axis.getLength(), pVerts, pIndices, &numVerts, &numIndices, indexOffset);

			pVerts += numVerts;
			pIndices += numIndices;
			indexOffset += numVerts;
		}
	}

	_vertices->unlock ();
	_indices->unlock ();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SkeletonVisualizerRenderDatas: public ATOM_Drawable
{
public:
	SkeletonVisualizerRenderDatas (void)
	{
		_visualizer = ATOM_NEW(ATOMX_SkeletonVisualizer);
	}

	virtual ~SkeletonVisualizerRenderDatas (void)
	{
		ATOM_DELETE(_visualizer);
	}

public:
	ATOMX_SkeletonVisualizer *getVisualizer (void) const
	{
		return _visualizer;
	}

public:
	virtual bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
	{
		_visualizer->draw (material);
		return true;
	}

private:
	ATOMX_SkeletonVisualizer *_visualizer;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ATOMX_SkeletonVisualizerNode::ATOMX_SkeletonVisualizerNode (void)
{
	_renderDatas = 0;
}

ATOMX_SkeletonVisualizerNode::~ATOMX_SkeletonVisualizerNode (void)
{
	ATOM_DELETE(_renderDatas);
	_renderDatas = 0;
}

void ATOMX_SkeletonVisualizerNode::buildBoundingbox (void) const
{
}

void ATOMX_SkeletonVisualizerNode::setupRenderQueue (ATOM_CullVisitor *visitor)
{
	visitor->addDrawable (_renderDatas, getWorldMatrix(), _renderDatas->getVisualizer()->getMaterial());
}

void ATOMX_SkeletonVisualizerNode::skipClipTest (bool b)
{
}

bool ATOMX_SkeletonVisualizerNode::isSkipClipTest (void) const
{
	return true;
}

bool ATOMX_SkeletonVisualizerNode::rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const
{
	return false;
}

void ATOMX_SkeletonVisualizerNode::setSource (ATOM_Geode *geode)
{
	_source = geode;

	if (!geode)
	{
		ATOM_DELETE(_renderDatas);
		_renderDatas = 0;
	}
	else
	{
		if (!_renderDatas)
		{
			_renderDatas = ATOM_NEW(SkeletonVisualizerRenderDatas);
		}
		_renderDatas->getVisualizer()->setSource (geode);
	}
}

void ATOMX_SkeletonVisualizerNode::update (ATOM_Camera *camera)
{
	if (!_source)
	{
		ATOM_DELETE(_renderDatas);
		_renderDatas = 0;
	}
	else
	{
		_renderDatas->getVisualizer()->updateVertices ();
	}
}





