#include "StdAfx.h"
#include "treebatch.h"
#include "treeinstance.h"

static bool _materialCreated = false;
static ATOM_AUTOREF(ATOM_VertexArray) _instanceDataVertexArray;

ATOM_TreeTrunkBatch::ATOM_TreeTrunkBatch (void)
{
	ATOM_STACK_TRACE(ATOM_TreeTrunkBatch::ATOM_TreeTrunkBatch);

	_core = 0;
	_vertexDecl = 0;
	_vertexDeclInstancing = 0;
	_vertexDeclShadowmap = 0;
	_core = 0;
	_numInstances = 0;
}

ATOM_TreeTrunkBatch::~ATOM_TreeTrunkBatch (void)
{
	ATOM_STACK_TRACE(ATOM_TreeTrunkBatch::~ATOM_TreeTrunkBatch);
}

void ATOM_TreeTrunkBatch::setCore (ATOM_TreeInstance *instance)
{
	_core = instance;
}

ATOM_TreeInstance *ATOM_TreeTrunkBatch::getCore (void) const
{
	return _core;
}

bool ATOM_TreeTrunkBatch::begin (ATOM_RenderDevice *device, bool gbuffer, bool hwInstancing, bool shadowmap)
{
	ATOM_STACK_TRACE(ATOM_TreeTrunkBatch::begin);

	if (!_core || !_core->hasTrunk ())
	{
		return false;
	}

	if (hwInstancing && !_instanceDataVertexArray)
	{
		_instanceDataVertexArray = device->allocVertexArray(ATOM_VERTEX_ATTRIB_TEX2_1, ATOM_USAGE_STATIC, 50, true);
		if (!_instanceDataVertexArray)
		{
			return false;
		}
		float *p = (float*)_instanceDataVertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		if (!p)
		{
			_instanceDataVertexArray = 0;
			return false;
		}
		for (unsigned i = 0; i < _instanceDataVertexArray->getNumVertices(); ++i)
		{
			p[i] = float(i);
		}
		_instanceDataVertexArray->unlock ();
	}

	_drawGBuffer = gbuffer;
	_drawInstancing = (gbuffer || _shadowMap) && hwInstancing;
	_shadowMap = shadowmap;

	if (!hwInstancing)
	{
		if (!_material)
		{
			_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/treetrunk.mat");
			if (!_material)
			{
				return false;
			}
			_material->getParameterTable()->setInt ("createBillboard", 0);
		}

		if (!_vertexDecl)
		{
			_vertexDecl = device->createInterleavedVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_NORMAL|ATOM_VERTEX_ATTRIB_TEX1_2);
			if (!_vertexDecl)
			{
				return false;
			}
		}

		if (!_shadowMap)
		{
			ATOM_Texture *texture = _core->getTrunkTexture();
			_material->getParameterTable()->setTexture ("diffuseMap", texture ? texture : ATOM_GetColorTexture(0xFFFFFFFF));
		}

		if (!device->setVertexDecl (_vertexDecl))
		{
			return false;
		}

		if (!device->setStreamSource (0, _core->getTrunkVertexArray (), 0))
		{
			return false;
		}
	}
	else
	{
		if (!_materialInstancing)
		{
			_materialInstancing = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/treetrunk_instancing.mat");
			if (!_materialInstancing)
			{
				return false;
			}
		}

		if (!_vertexDeclInstancing)
		{
			_vertexDeclInstancing = device->createInterleavedVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_NORMAL|ATOM_VERTEX_ATTRIB_TEX1_2, ATOM_VERTEX_ATTRIB_TEX2_1);
			if (!_vertexDeclInstancing)
			{
				return false;
			}
		}

		_numInstances = 0;
	}

	return true;
}

void ATOM_TreeTrunkBatch::drawInstance (ATOM_RenderDevice *device, const ATOM_Matrix4x4f &worldMatrix, const ATOM_Vector3f &worldScale)
{
	ATOM_STACK_TRACE(ATOM_TreeTrunkBatch::drawInstance);

	if (!_drawInstancing)
	{
		device->setTransform (ATOM_MATRIXMODE_WORLD, worldMatrix);
		unsigned numPasses = _material->begin (device);
		if (numPasses)
		{
			for (unsigned pass = 0; pass < numPasses; ++pass)
			{
				if (_material->beginPass (device, pass))
				{
					device->renderStreamsIndexed (_core->getTrunkIndexArray (), ATOM_PRIMITIVE_TRIANGLES, _core->getTrunkIndexArray()->getNumIndices()/3, 0);
					_material->endPass (device, pass);
				}
			}
		}
		_material->end (device);
	}
	else
	{
		_constants.worldMatrix[_numInstances * 3 + 0] = worldMatrix.getCol(0);
		_constants.worldMatrix[_numInstances * 3 + 1] = worldMatrix.getCol(1);
		_constants.worldMatrix[_numInstances * 3 + 2] = worldMatrix.getCol(2);
		_numInstances++;
	}
}

void ATOM_TreeTrunkBatch::end (ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_TreeTrunkBatch::end);

	if (_drawInstancing)
	{
		if (!_shadowMap)
		{
			ATOM_Texture *texture = _core->getTrunkTexture();
			_material->getParameterTable()->setTexture ("diffuseMap", texture ? texture : ATOM_GetColorTexture(0xFFFFFFFF));
		}

		device->setVertexDecl (_vertexDecl);

		ATOM_Matrix4x4f m;
		device->getTransform (ATOM_MATRIXMODE_VIEWPROJ, m);
		_constants.vpMatrix[0] = m.getCol(0);
		_constants.vpMatrix[1] = m.getCol(1);
		_constants.vpMatrix[2] = m.getCol(2);
		_constants.vpMatrix[3] = m.getCol(3);
		device->getTransform (ATOM_MATRIXMODE_INV_VIEW, m);
		_constants.ivMatrix[0] = m.getCol(0);
		_constants.ivMatrix[1] = m.getCol(1);
		_constants.ivMatrix[2] = m.getCol(2);

		_materialInstancing->getParameterTable()->setVector ("vpmatrix0", _constants.vpMatrix[0]);
		_materialInstancing->getParameterTable()->setVector ("vpmatrix1", _constants.vpMatrix[1]);
		_materialInstancing->getParameterTable()->setVector ("vpmatrix2", _constants.vpMatrix[2]);
		_materialInstancing->getParameterTable()->setVector ("vpmatrix3", _constants.vpMatrix[3]);
		_materialInstancing->getParameterTable()->setVector ("viewmatrix0", _constants.ivMatrix[0]);
		_materialInstancing->getParameterTable()->setVector ("viewmatrix1", _constants.ivMatrix[1]);
		_materialInstancing->getParameterTable()->setVector ("viewmatrix2", _constants.ivMatrix[2]);
		_materialInstancing->getParameterTable()->setVectorArray ("worldmatrix", _constants.worldMatrix, _numInstances * 3);

		unsigned numPasses = _materialInstancing->begin (device);
		if (numPasses)
		{
			for (unsigned pass = 0; pass < numPasses; ++pass)
			{
				if (_materialInstancing->beginPass (device, pass))
				{
					device->setStreamSource	(0, _core->getTrunkVertexArray());
					device->setStreamSourceFreq (0, ATOM_STREAMSOURCE_INDEXEDDATA|_numInstances);
					device->setStreamSource (1, _instanceDataVertexArray.get());
					device->setStreamSourceFreq (1, ATOM_STREAMSOURCE_INSTANCEDATA|1);
					device->renderStreamsIndexed (_core->getTrunkIndexArray(), ATOM_PRIMITIVE_TRIANGLES, _core->getTrunkIndexArray()->getNumIndices()/3, 0);
					device->setStreamSourceFreq (0, 1);
					device->setStreamSourceFreq (1, 1);

					_materialInstancing->endPass (device, pass);
				}
			}
		}
		_materialInstancing->end (device);
	}
}

ATOM_TreeLeavesBatch::ATOM_TreeLeavesBatch (void)
{
	ATOM_STACK_TRACE(ATOM_TreeLeavesBatch::ATOM_TreeLeavesBatch);

	_core = 0;
	_vertexDecl = 0;
	_vertexDeclInstancing = 0;
	_vertexDeclShadowmap = 0;
	_core = 0;
	_numInstances = 0;

	_constants.vertexUV[0].set (0.f, 0.f, 0.f, 0.f);
	_constants.vertexUV[1].set (1.f, 0.f, 0.f, 0.f);
	_constants.vertexUV[2].set (1.f, 1.f, 0.f, 0.f);
	_constants.vertexUV[3].set (0.f, 1.f, 0.f, 0.f);
	_constants.normals[0].set (-1.f, 1.f, -1.5f, 0.f);
	_constants.normals[1].set (1.f, 1.f, -1.5f, 0.f);
	_constants.normals[2].set (1.f, -1.f, -1.5f, 0.f);
	_constants.normals[3].set (-1.f, -1.f, -1.5f, 0.f);
	_constants.x[0].set (-1.f, 0.f, 0.f, 0.f);
	_constants.x[1].set (1.f, 0.f, 0.f, 0.f);
	_constants.x[2].set (1.f, 0.f, 0.f, 0.f);
	_constants.x[3].set (-1.f, 0.f, 0.f, 0.f);
	_constants.y[0].set (0.f, 1.f, 0.f, 0.f);
	_constants.y[1].set (0.f, 1.f, 0.f, 0.f);
	_constants.y[2].set (0.f, -1.f, 0.f, 0.f);
	_constants.y[3].set (0.f, -1.f, 0.f, 0.f);
}

ATOM_TreeLeavesBatch::~ATOM_TreeLeavesBatch (void)
{
	ATOM_STACK_TRACE(ATOM_TreeLeavesBatch::~ATOM_TreeLeavesBatch);
}

void ATOM_TreeLeavesBatch::setCore (ATOM_TreeInstance *instance)
{
	_core = instance;
}

ATOM_TreeInstance *ATOM_TreeLeavesBatch::getCore (void) const
{
	return _core;
}

bool ATOM_TreeLeavesBatch::begin (ATOM_RenderDevice *device, bool gbuffer, bool hwInstancing, bool shadowmap)
{
	ATOM_STACK_TRACE(ATOM_TreeLeavesBatch::begin);

	if (!_core || !_core->hasLeaves ())
	{
		return false;
	}

	if (hwInstancing && !_instanceDataVertexArray)
	{
		_instanceDataVertexArray = device->allocVertexArray(ATOM_VERTEX_ATTRIB_TEX2_1, ATOM_USAGE_STATIC, 50, true);
		if (!_instanceDataVertexArray)
		{
			return false;
		}
		float *p = (float*)_instanceDataVertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		if (!p)
		{
			_instanceDataVertexArray = 0;
			return false;
		}
		for (unsigned i = 0; i < _instanceDataVertexArray->getNumVertices(); ++i)
		{
			p[i] = float(i);
		}
		_instanceDataVertexArray->unlock ();
	}

	_drawInstancing = hwInstancing;
	_shadowMap = shadowmap;

	if (!hwInstancing)
	{
		if (!_material)
		{
			_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/treeleaf.mat");
			if (!_material)
			{
				return false;
			}
			_material->getParameterTable()->setInt ("createBillboard", 0);
		}

		if (!_vertexDecl)
		{
			_vertexDecl = device->createInterleavedVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD_XYZW|ATOM_VERTEX_ATTRIB_TEX1_4);
			if (!_vertexDecl)
			{
				return false;
			}
		}

		ATOM_Texture *texture = _core->getLeafTexture();
		_material->getParameterTable()->setTexture ("diffuseMap", texture ? texture : ATOM_GetColorTexture(0xFFFFFFFF));

		if (!device->setVertexDecl (_vertexDecl))
		{
			return false;
		}

		if (!device->setStreamSource (0, _core->getLeafCardVertexArray (), 0))
		{
			return false;
		}
	}
	else
	{
		if (!_materialInstancing)
		{
			_materialInstancing = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/treeleaf_instancing.mat");
			if (!_materialInstancing)
			{
				return false;
			}
		}

		if (!_vertexDeclInstancing)
		{
			_vertexDeclInstancing = device->createInterleavedVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD_XYZW|ATOM_VERTEX_ATTRIB_TEX1_4, ATOM_VERTEX_ATTRIB_TEX2_1);
			if (!_vertexDeclInstancing)
			{
				return false;
			}
		}

		ATOM_Texture *texture = _core->getLeafTexture();
		_material->getParameterTable()->setTexture ("diffuseMap", texture ? texture : ATOM_GetColorTexture(0xFFFFFFFF));
		device->setVertexDecl (_vertexDecl);

		ATOM_Matrix4x4f m;
		device->getTransform (ATOM_MATRIXMODE_PROJECTION, m);
		_constants.projMatrix[0] = m.getCol(0);
		_constants.projMatrix[1] = m.getCol(1);
		_constants.projMatrix[2] = m.getCol(2);
		_constants.projMatrix[3] = m.getCol(3);
		device->getTransform (ATOM_MATRIXMODE_INV_VIEW, m);
		_constants.ivMatrix[0] = m.getCol(0);
		_constants.ivMatrix[1] = m.getCol(1);
		_constants.ivMatrix[2] = m.getCol(2);

		_numInstances = 0;
	}

	return true;
}

void ATOM_TreeLeavesBatch::drawInstance (ATOM_RenderDevice *device, const ATOM_Matrix4x4f &worldMatrix, const ATOM_Vector3f &worldScale)
{
	ATOM_STACK_TRACE(ATOM_TreeLeavesBatch::drawInstance);

	if (!_drawInstancing)
	{
		device->setTransform (ATOM_MATRIXMODE_WORLD, worldMatrix);
		_material->getParameterTable()->setVector("scale", worldScale);
		unsigned numPasses = _material->begin (device);
		if (numPasses)
		{
			for (unsigned pass = 0; pass < numPasses; ++pass)
			{
				if (_material->beginPass (device, pass))
				{
					device->renderStreamsIndexed (_core->getLeafCardIndexArray(), ATOM_PRIMITIVE_TRIANGLES, _core->getLeafCardIndexArray()->getNumIndices()/3, 0);
					_material->endPass (device, pass);
				}
			}
		}
		_material->end (device);
	}
	else
	{
		_constants.instanceDatas[_numInstances * 4 + 0] = worldScale;
		_constants.instanceDatas[_numInstances * 4 + 1] = worldMatrix.getCol(0);
		_constants.instanceDatas[_numInstances * 4 + 2] = worldMatrix.getCol(1);
		_constants.instanceDatas[_numInstances * 4 + 3] = worldMatrix.getCol(2);
		_numInstances++;
	}
}

void ATOM_TreeLeavesBatch::end (ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_TreeLeavesBatch::end);

	if (_drawInstancing)
	{
		_materialInstancing->getParameterTable()->setVector ("projmatrix0", _constants.projMatrix[0]);
		_materialInstancing->getParameterTable()->setVector ("projmatrix1", _constants.projMatrix[1]);
		_materialInstancing->getParameterTable()->setVector ("projmatrix2", _constants.projMatrix[2]);
		_materialInstancing->getParameterTable()->setVector ("projmatrix3", _constants.projMatrix[3]);
		_materialInstancing->getParameterTable()->setVector ("viewmatrix0", _constants.ivMatrix[0]);
		_materialInstancing->getParameterTable()->setVector ("viewmatrix1", _constants.ivMatrix[1]);
		_materialInstancing->getParameterTable()->setVector ("viewmatrix2", _constants.ivMatrix[2]);
		_materialInstancing->getParameterTable()->setVectorArray ("vertexUV", _constants.vertexUV, 4);
		_materialInstancing->getParameterTable()->setVectorArray ("normals", _constants.normals, 4);
		_materialInstancing->getParameterTable()->setVectorArray ("x", _constants.x, 4);
		_materialInstancing->getParameterTable()->setVectorArray ("y", _constants.y, 4);
		_materialInstancing->getParameterTable()->setVectorArray ("instanceDatas", _constants.instanceDatas, _numInstances * 4);

		unsigned numPasses = _materialInstancing->begin (device);
		if (numPasses)
		{
			for (unsigned pass = 0; pass < numPasses; ++pass)
			{
				if (_materialInstancing->beginPass (device, pass))
				{
					device->setStreamSource	(0, _core->getLeafCardVertexArray());
					device->setStreamSourceFreq (0, ATOM_STREAMSOURCE_INDEXEDDATA|_numInstances);
					device->setStreamSource (1, _instanceDataVertexArray.get());
					device->setStreamSourceFreq (1, ATOM_STREAMSOURCE_INSTANCEDATA|1);
					device->renderStreamsIndexed (_core->getLeafCardIndexArray(), ATOM_PRIMITIVE_TRIANGLES, _core->getLeafCardIndexArray()->getNumIndices()/3, 0);
					device->setStreamSourceFreq (0, 1);
					device->setStreamSourceFreq (1, 1);

					_materialInstancing->endPass (device, pass);
				}
			}
		}
		_materialInstancing->end (device);
	}
}

ATOM_TreeBatch::ATOM_TreeBatch (ATOM_TreeInstance *instance)
{
	ATOM_STACK_TRACE(ATOM_TreeBatch::ATOM_TreeBatch);

	_trunkBatch = ATOM_NEW(ATOM_TreeTrunkBatch);
	_trunkBatch->setCore (instance);
	_leavesBatch = ATOM_NEW(ATOM_TreeLeavesBatch);
	_leavesBatch->setCore (instance);
	_instancing = false;
	_gbuffer = false;
	_queued = false;
}

ATOM_TreeBatch::~ATOM_TreeBatch (void)
{
	ATOM_STACK_TRACE(ATOM_TreeBatch::~ATOM_TreeBatch);

	ATOM_DELETE(_trunkBatch);
}

void ATOM_TreeBatch::addInstance (int orientation, const ATOM_Matrix4x4f *worldMatrix, const ATOM_Vector3f *worldScale)
{
	ATOM_STACK_TRACE(ATOM_TreeBatch::addInstance);

	if (orientation < 0)
	{
		_transformInfos.resize (_transformInfos.size() + 1);
		_transformInfos.back().worldMatrix = worldMatrix;
		_transformInfos.back().worldScale = worldScale;
	}
	else
	{
		_billboardTransformInfos[orientation].resize (_billboardTransformInfos[orientation].size() + 1);
		_billboardTransformInfos[orientation].back().worldMatrix = worldMatrix;
		_billboardTransformInfos[orientation].back().worldScale = worldScale;
	}
}

void ATOM_TreeBatch::clear (void)
{
	ATOM_STACK_TRACE(ATOM_TreeBatch::clear);

	_transformInfos.resize (0);
	_billboardTransformInfos[0].resize (0);
	_billboardTransformInfos[1].resize (0);
	_billboardTransformInfos[2].resize (0);
	_billboardTransformInfos[3].resize (0);
}

void ATOM_TreeBatch::enableInstancing (bool enable)
{
	_instancing = enable;
}

void ATOM_TreeBatch::enableGBuffer (bool enable)
{
	_gbuffer = enable;
}

void ATOM_TreeBatch::enableShadowMap (bool enable)
{
	_shadowmap = enable;
}

void ATOM_TreeBatch::render (ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_TreeBatch::draw);

	unsigned numInstances = _transformInfos.size();
	unsigned numDrawn = 0;
	while (numInstances != 0)
	{
		unsigned numToBeDrawn = numInstances > 50 ? 50 : numInstances;
		numInstances -= numToBeDrawn;
		const InstanceTransformInfo *info = &_transformInfos[numDrawn];
		numDrawn += numToBeDrawn;

		if (_trunkBatch->begin (device, true, false, false))
		{
			for (unsigned i = 0; i < numToBeDrawn; ++i)
			{
				_trunkBatch->drawInstance (device, *info[i].worldMatrix, *info[i].worldScale);
			}
			_trunkBatch->end (device);
		}
	}

	numInstances = _transformInfos.size();
	numDrawn = 0;
	while (numInstances != 0)
	{
		unsigned numToBeDrawn = numInstances > 50 ? 50 : numInstances;
		numInstances -= numToBeDrawn;
		const InstanceTransformInfo *info = &_transformInfos[numDrawn];
		numDrawn += numToBeDrawn;

		if (_leavesBatch->begin (device, true, false, false))
		{
			for (unsigned i = 0; i < numToBeDrawn; ++i)
			{
				_leavesBatch->drawInstance (device, *info[i].worldMatrix, *info[i].worldScale);
			}
			_leavesBatch->end (device);
		}
	}
}

bool ATOM_TreeBatch::draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{
	render (device);

	clear ();
	_queued = false;

	return true;
}

void ATOM_TreeBatch::setQueued (bool b)
{
	_queued = b;
}

bool ATOM_TreeBatch::queued (void) const
{
	return _queued;
}

