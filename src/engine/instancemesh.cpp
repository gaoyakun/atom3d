#include "StdAfx.h"
#include "instancemesh.h"
#include "instancetrack.h"
#include "instanceskeleton.h"

ATOM_InstanceMesh::ATOM_InstanceMesh (ATOM_Node *node, ATOM_Mesh *mesh): _node(node), _mesh(mesh)
{
	ATOM_STACK_TRACE(ATOM_InstanceMesh::ATOM_InstanceMesh);

	ATOM_ASSERT(_mesh);
	_id = 0;
	_index = -1;
	_visible = true;
	_billboardNormal = mesh->billboardNormal;
	_billboardXAxis = mesh->billboardXAxis;
	_billboardCenter = mesh->billboardCenter;
	_worldMatrix = mesh->worldMatrix;
	_jointMapOk = false;
	_currentMapping = 0;
	_glareTexture = 0;
	_glareVelocity.set(0.f, 0.f, 0.f, 0.f);
	_glareRepeat.set(1.f, 1.f, 0.f, 0.f);
	_albedoTexture = _mesh->diffuseTexture;
	_lastAnimationFrame = -1.f;
	_updateFrameStamp = 0;
	_currentTrack = 0;
	_currentTrackFrame = -1.f;
	_diffuseColor = mesh->diffuseColor;
	_alpharef = mesh->alpharef;
	_shininess = mesh->shininess;
	_animationContext = 0;
}

ATOM_InstanceMesh::~ATOM_InstanceMesh (void)
{
	ATOM_STACK_TRACE(ATOM_InstanceMesh::~ATOM_InstanceMesh);

	_node = 0;
	_currentMapping = 0;
	_currentTrack = 0;
	ATOM_DELETE(_animationContext);
}

ATOM_Mesh *ATOM_InstanceMesh::getMesh (void) const
{
	return _mesh.get ();
}

static ATOM_Vector4f saveLightColor;
static ATOM_Vector4f saveAmbientColor;
static bool cwr, cwg, cwb, cwa;

bool ATOM_InstanceMesh::setupParameters (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material, bool skinning)
{
 	if (_mesh->diffuseTexture && _mesh->diffuseTexture->getLoadInterface()->getLoadingState () != ATOM_LoadInterface::LS_LOADED)
	{
		if (GT_HWINSTANCING == _mesh->geometry->getType())
		{
			((ATOM_HWInstancingGeometry*)_mesh->geometry)->clearInstances ();
		}
		return false;
	}

	ATOM_Texture *glareTexture = getGlareMap();
	if (glareTexture && glareTexture->getLoadInterface()->getLoadingState () != ATOM_LoadInterface::LS_LOADED)
	{
		if (GT_HWINSTANCING == _mesh->geometry->getType())
		{
			((ATOM_HWInstancingGeometry*)_mesh->geometry)->clearInstances ();
		}
		return false;
	}

	ATOM_Vector4f diffuseColor = getDiffuseColor();

	material->getParameterTable()->setVector ("viewPoint", ATOM_Vector4f(camera->getPosition ()));
	material->getParameterTable()->setMatrix44 ("viewProjectionMatrix", camera->getViewProjectionMatrix());
	material->getParameterTable()->setMatrix44 ("viewProjectionMatrixGBuffer", camera->getProjectionMatrix());
	material->getParameterTable()->setVector ("diffuseColor", diffuseColor);
	material->getParameterTable()->setMatrix44 ("worldMatrixGBuffer", camera->getViewProjectionMatrix());

	ATOM_Texture *texture = getAlbedoMap ();
	material->getParameterTable()->setTexture ("diffuseTexture", texture?texture:_mesh->diffuseTexture.get());
	material->getParameterTable()->setFloat ("depthScale", 1.f/ATOM_RenderSettings::getDepthScale());

	return true;
}

bool ATOM_InstanceMesh::draw(ATOM_RenderDevice* device, ATOM_Camera *camera, ATOM_Material *material)
{
	ATOM_STACK_TRACE(ATOM_InstanceMesh::draw);

	if (_mesh && material)
	{
		setupParameters (device, camera, material, false);

		ATOM_AUTOREF(ATOM_VertexArray) oldVertexArray = _mesh->vertices;
		ATOM_AUTOREF(ATOM_VertexArray) oldNormalArray = _mesh->normals;
		ATOM_AUTOREF(ATOM_VertexArray) oldUVArray = _mesh->texcoords;

		if (GT_HWINSTANCING == _mesh->geometry->getType())
		{
			unsigned numPasses = material->begin (device);
			for (unsigned pass = 0; pass < numPasses; ++pass)
			{
				if (material->beginPass (device, pass))
				{
					_mesh->render (device);
					material->endPass (device, pass);
				}
			}
			material->end (device);
		}
		else
		{
			ATOM_MultiStreamGeometry *geo = (ATOM_MultiStreamGeometry*)_mesh->geometry;

			if (_vertexArray)
			{
				geo->addStream (_vertexArray.get());
			}

			if (_normalArray)
			{
				geo->addStream (_normalArray.get());
			}

			if (_uvArray)
			{
				geo->addStream (_uvArray.get());
			}

			unsigned numPasses = material->begin (device);
			for (unsigned pass = 0; pass < numPasses; ++pass)
			{
				if (material->beginPass (device, pass))
				{
					_mesh->render (device);
					material->endPass (device, pass);
				}
			}
			material->end (device);

			if (_vertexArray)
			{
				geo->addStream (oldVertexArray.get(), 0);
			}

			if (_normalArray)
			{
				geo->addStream (oldNormalArray.get(), 0);
			}

			if (_uvArray)
			{
				geo->addStream (oldUVArray.get(), 0);
			}
		}
	}

	return true;
}

void ATOM_InstanceMesh::setComponents (ATOM_Components components)
{
	_owner = components;
}

ATOM_Components ATOM_InstanceMesh::getComponents (void) const
{
	return _owner;
}

void ATOM_InstanceMesh::setId (int id)
{
	_id = id;
}

int ATOM_InstanceMesh::getId (void) const
{
	return _id;
}

void ATOM_InstanceMesh::setIndex (int index)
{
	_index = index;
}

int ATOM_InstanceMesh::getIndex (void) const
{
	return _index;
}

void ATOM_InstanceMesh::setUVArray (ATOM_VertexArray *array)
{
	_uvArray = array;
}

ATOM_VertexArray *ATOM_InstanceMesh::getUVArray (void) const
{
	return _uvArray.get ();
}

void ATOM_InstanceMesh::setVertexArray (ATOM_VertexArray *array)
{
	_vertexArray = array;
}

ATOM_VertexArray *ATOM_InstanceMesh::getVertexArray (void) const
{
	return _vertexArray.get ();
}

void ATOM_InstanceMesh::setNormalArray (ATOM_VertexArray *array)
{
	_normalArray = array;
}

ATOM_VertexArray *ATOM_InstanceMesh::getNormalArray (void) const
{
	return _normalArray.get();
}

void ATOM_InstanceMesh::setBillboardNormal (const ATOM_Vector3f &v)
{
	_billboardNormal = v;
}

const ATOM_Vector3f &ATOM_InstanceMesh::getBillboardNormal (void) const
{
	return _billboardNormal;
}

void ATOM_InstanceMesh::setBillboardXAxis (const ATOM_Vector3f &v)
{
	_billboardXAxis = v;
}

const ATOM_Vector3f &ATOM_InstanceMesh::getBillboardXAxis (void) const
{
	return _billboardXAxis;
}

void ATOM_InstanceMesh::setBillboardCenter (const ATOM_Vector3f &v)
{
	_billboardCenter = v;
}

const ATOM_Vector3f &ATOM_InstanceMesh::getBillboardCenter (void) const
{
	return _billboardCenter;
}

void ATOM_InstanceMesh::setWorldMatrix (const ATOM_Matrix4x4f &m)
{
	_worldMatrix = m;
}

const ATOM_Matrix4x4f &ATOM_InstanceMesh::getWorldMatrix (void) const
{
	return _worldMatrix;
}

void ATOM_InstanceMesh::setVisible (bool visible)
{
	_visible = visible;
}

bool ATOM_InstanceMesh::getVisible (void) const
{
	return _visible;
}

bool ATOM_InstanceMesh::mapJoints (ATOM_InstanceTrack *track)
{
	ATOM_STACK_TRACE(ATOM_InstanceMesh::mapJoints);

	if (_currentMapping)
	{
		return _jointMapOk;
	}

	track->getTrack()->invalidBoundingbox ();
	_currentMapping = track;
	_jointMapOk = true;

	if (_mesh && _mesh->weights.size() > 0)
	{
		const ATOM_HASHMAP<unsigned, unsigned> &jointHashes = track->getTrack()->getHashes ();
		if (_mesh->jointHashes.size() == 0)
		{
			if (jointHashes.size())
			{
				ATOM_LOGGER::error ("The mesh need joint mapping, please re-export it.\n");
				_jointMapOk = false;
				return false;
			}
		}
		else if (jointHashes.size() == 0)
		{
			ATOM_LOGGER::error ("The animation need joint mapping, please re-export it.\n");
			_jointMapOk = false;
			return false;
		}

		_jointMap.resize (_mesh->jointHashes.size());
		for (unsigned i = 0; i < _jointMap.size(); ++i)
		{
			if (i >= _mesh->jointMask.size() || !_mesh->jointMask.testBit (i))
			{
				continue;
			}

			ATOM_HASHMAP<unsigned, unsigned>::const_iterator it = jointHashes.find (_mesh->jointHashes[i]);
			if (it == jointHashes.end ())
			{
				ATOM_LOGGER::error ("Couldn't remap vertex joints: id(%u).\n", _mesh->jointHashes[i]);
				_jointMapOk = false;
				return false;
			}
			else
			{
				unsigned joint = it->second;
				_jointMap[i] = joint;
			}
		}
		
		return true;
	}

	return true;
}

void ATOM_InstanceMesh::invalidateMapJoints (void)
{
	_currentMapping = 0;
}

const ATOM_VECTOR<unsigned> &ATOM_InstanceMesh::getJointMap (void) const
{
	return _jointMap;
}

void ATOM_InstanceMesh::setDiffuseColor (const ATOM_Vector4f &color)
{
	_diffuseColor = color;
}

void ATOM_InstanceMesh::setAlphaRef (float alpharef)
{
	_alpharef = alpharef;
}

void ATOM_InstanceMesh::setShininess (float shininess)
{
	_shininess = shininess;
}

void ATOM_InstanceMesh::setAlbedoMap (ATOM_Texture *texture)
{
	_albedoTexture = texture;
}

void ATOM_InstanceMesh::setGlareMap (ATOM_Texture *texture)
{
	_glareTexture = texture;
}

void ATOM_InstanceMesh::setGlareVelocity (const ATOM_Vector4f &v)
{
	_glareVelocity = v;
}

void ATOM_InstanceMesh::setGlareRepeat (const ATOM_Vector4f &v)
{
	_glareRepeat = v;
}

const ATOM_Vector4f &ATOM_InstanceMesh::getDiffuseColor (void) const
{
	static const ATOM_Vector4f c(1.f);
	return c;
}

ATOM_Texture *ATOM_InstanceMesh::getAlbedoMap (void) const
{
	return 0;
}

ATOM_Texture *ATOM_InstanceMesh::getGlareMap (void) const
{
	return 0;
}

const ATOM_Vector4f &ATOM_InstanceMesh::getGlareVelocity (void) const
{
	return _glareVelocity;
}

const ATOM_Vector4f &ATOM_InstanceMesh::getGlareRepeat (void) const
{
	return _glareRepeat;
}

float ATOM_InstanceMesh::getAlphaRef (void) const
{
	return _alpharef;
}

float ATOM_InstanceMesh::getShininess (void) const
{
	return _shininess;
}

void ATOM_InstanceMesh::setLastAnimationFrame (float frame)
{
	_lastAnimationFrame = frame;
}

float ATOM_InstanceMesh::getLastAnimationFrame (void) const
{
	return _lastAnimationFrame;
}

void ATOM_InstanceMesh::setCurrentTrackFrame (float frame)
{
	_currentTrackFrame = frame;
}

float ATOM_InstanceMesh::getCurrentTrackFrame (void) const
{
	return _currentTrackFrame;
}

void ATOM_InstanceMesh::setUpdateFrameStamp (unsigned val)
{
	_updateFrameStamp = val;
}

unsigned ATOM_InstanceMesh::getUpdateFrameStamp (void) const
{
	return _updateFrameStamp;
}

void ATOM_InstanceMesh::setCurrentTrack (ATOM_InstanceTrack *track)
{
	_currentTrack = track;
}

ATOM_InstanceTrack *ATOM_InstanceMesh::getCurrentTrack (void) const
{
	return _currentTrack;
}

ATOM_Node *ATOM_InstanceMesh::getNode (void) const
{
	return _node;
}

bool ATOM_InstanceMesh::canBatch (void) const
{
	return true;
}

bool ATOM_InstanceMesh::isContentLost (void) const
{
	ATOM_STACK_TRACE(ATOM_InstanceMesh::isContentLost);

	if (_vertexArray && _vertexArray->isContentLost ())
	{
		return true;
	}

	if (_normalArray && _normalArray->isContentLost ())
	{
		return true;
	}

	if (_uvArray && _uvArray->isContentLost ())
	{
		return true;
	}

	return false;
}

void ATOM_InstanceMesh::setContentLost (bool b)
{
	ATOM_STACK_TRACE(ATOM_InstanceMesh::setContentLost);

	if (_vertexArray)
	{
		_vertexArray->setContentLost (b);
	}

	if (_normalArray)
	{
		_normalArray->setContentLost (b);
	}

	if (_uvArray)
	{
		_uvArray->setContentLost (b);
	}
}

/*
	ATOM_Vector4f diffuseColor;
	int meshIndex;
	volatile long finished;
	float lastAnimationFrame;
	unsigned vertexAttrib;
	unsigned cycleTick;
	ATOM_AUTOPTR(ATOM_InstanceMesh) imesh;
	ATOM_AUTOPTR(ATOM_ModelAnimationTrack) track;
	ATOM_Vector3f billboardNormal;
	ATOM_Vector3f billboardXAxis;
	ATOM_Vector3f billboardCenter;
	ATOM_Matrix4x4f worldMatrix;

	void *vertices;
	void *uvs;
	void *normals;

	ATOM_AUTOREF(ATOM_VertexArray) vertexArray;
	ATOM_AUTOREF(ATOM_VertexArray) uvArray;
	ATOM_AUTOREF(ATOM_VertexArray) normalArray;
*/

