#include "StdAfx.h"
#include "tree.h"
#include "treebatch.h"
#include "shadowscene.h"

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_Tree)
  ATOM_ATTRIBUTES_BEGIN(ATOM_Tree)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Tree, "TreeFileName", getFileName, setFileName, "", "group=ATOM_Tree;desc='ÎÄ¼þÃû'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Tree, "LODDistance",	getLODDistance, setLODDistance, 1000.f, "group=ATOM_Tree;desc='LOD¾àÀë'")
  ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_Tree, ATOM_Node)

ATOM_IMPLEMENT_NODE_FILE_TYPE(ATOM_Tree)

ATOM_Tree::ATOM_Tree (void)
{
	ATOM_STACK_TRACE(ATOM_Tree::ATOM_Tree);

	_LODdistance = 2000.f;
}

ATOM_Tree::~ATOM_Tree (void)
{
	ATOM_STACK_TRACE(ATOM_Tree::~ATOM_Tree);
}

void ATOM_Tree::buildBoundingbox (void) const
{
	if (_instance)
	{
		_boundingBox = _instance->getBoundingbox ();
	}
	else
	{
		_boundingBox.setMin (ATOM_Vector3f(0.f, 0.f, 0.f));
		_boundingBox.setMax (ATOM_Vector3f(0.f, 0.f, 0.f));
	}
}

void ATOM_Tree::setupRenderQueue (ATOM_CullVisitor *visitor)
{
	ATOM_STACK_TRACE(ATOM_Tree::setupRenderQueue);

	if (_instance)
	{
		const ATOM_Matrix4x4f &viewMatrix = visitor->getCamera()->getViewMatrix();
		const ATOM_Matrix4x4f &projMatrix = visitor->getCamera()->getProjectionMatrix();
		const ATOM_Matrix4x4f &worldMatrix = getWorldMatrix();
		const ATOM_Vector3f &worldScale = getWorldScale ();
		ATOM_TreeBatch *batch = _instance->getBatch ();
		batch->addInstance (-1, &worldMatrix, &worldScale);

		if (!batch->queued ())
		{
			bool instancing = false && ATOM_RenderSettings::getRenderCaps().supportInstancing;
			batch->enableGBuffer (true);
			batch->enableInstancing (instancing);
			batch->enableShadowMap (false);
			visitor->addDrawable (batch, worldMatrix, 0, ATOM_DeferredRenderScheme::RenderScheme_GbufferConstruction);
			batch->setQueued (true);
		}
	}

}

bool ATOM_Tree::rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const
{
	if (_instance)
	{
		float d1 = FLT_MAX, d2 = FLT_MAX;
		bool trunkPick = _instance->hasTrunk() ? (ray.intersectionTestEx (_instance->getTrunkBoundingbox(), d1) && d1 >= 0.f) : false;
		bool leafPick = _instance->hasLeaves() ? (ray.intersectionTestEx (_instance->getLeafBoundingbox(), d2) && d2 >= 0.f) : false;
		if (trunkPick || leafPick)
		{
			if (len)
			{
				*len = ATOM_min2(d1, d2);
			}
			return true;
		}
	}

	return false;
}

bool ATOM_Tree::onLoad(ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_Tree::onLoad);

	if (_instance)
	{
		return true;
	}

	if (!getNodeFileName().empty ())
	{
		_fileName = getNodeFileName();
	}

	if (!_fileName.empty())
	{
		char identFileName[ATOM_VFS::max_filename_length];
		if (!ATOM_CompletePath (_fileName.c_str(), identFileName))
		{
			return false;
		}
		_strlwr (identFileName);

		_instance = ATOM_LookupObject (ATOM_TreeInstance::_classname(), identFileName);

		if (_instance)
		{
			_LODdistance = _instance->getLODDistance();
			return true;
		}

		_instance = ATOM_CreateObject (ATOM_TreeInstance::_classname(), identFileName);

		if (_instance->load (device, identFileName))
		{
			_LODdistance = _instance->getLODDistance();
			return true;
		}
	}

	return false;
}

void ATOM_Tree::setFileName (const ATOM_STRING &fileName)
{
	ATOM_STACK_TRACE(ATOM_Tree::setFileName);

	_fileName = fileName;
}

const ATOM_STRING &ATOM_Tree::getFileName (void) const
{
	return _fileName;
}

void ATOM_Tree::setLODDistance (float distance)
{
	_LODdistance = distance;
}

float ATOM_Tree::getLODDistance (void) const
{
	return _LODdistance;
}

void ATOM_Tree::assign(ATOM_Node *other) const
{
	ATOM_STACK_TRACE(ATOM_Tree::assign);

	ATOM_Node::assign (other);

	ATOM_Tree *node = dynamic_cast<ATOM_Tree*>(other);

	if (node)
	{
		node->_fileName = _fileName;
		node->_LODdistance = _LODdistance;
		node->_instance = _instance;
	}
}

bool ATOM_Tree::supportFixedFunction (void) const
{
	return true;
}

