#include "StdAfx.h"
#include "asset.h"
#include "assetmanager.h"
#include "asset_node.h"

#undef TEST_HURT_NUMBER_EFFECT

#ifdef TEST_HURT_NUMBER_EFFECT
static ATOM_AUTOREF(ATOM_ShapeNode) createNumberShapeNode (int number, const ATOM_Vector4f &color, const char *textureFileName)
{
	static ATOM_AUTOREF(ATOM_Texture) texture = ATOM_CreateTextureResource(textureFileName);
	if (!texture)
	{
		return 0;
	}

	static ATOM_AUTOREF(ATOM_CoreMaterial) coreNumShapeMaterial = ATOM_MaterialManager::loadCoreMaterial (ATOM_GetRenderDevice(), "/materials/number_hud.mat");
	if (!coreNumShapeMaterial)
	{
		return 0;
	}

	ATOM_AUTOPTR(ATOM_Material) numShapeMaterial = ATOM_NEW(ATOM_Material);
	numShapeMaterial->setCoreMaterial (coreNumShapeMaterial.get());

	float digits[10];
	int numDigits = 0;

	do
	{
		digits[10-1-numDigits++] = number % 10;
		number = number / 10;
	} 
	while (number);

	numShapeMaterial->getParameterTable()->setTexture ("diffuseTexture", texture.get());
	numShapeMaterial->getParameterTable()->setFloatArray ("digits", digits+10-numDigits, numDigits);
	numShapeMaterial->getParameterTable()->setFloat ("num_digits", numDigits);

	ATOM_HARDREF(ATOM_ShapeNode) shapeNode;
	shapeNode->loadAttribute (NULL);
	shapeNode->setType (ATOM_ShapeNode::BILLBOARD);
	shapeNode->setTransparency (1.f);
	if (!shapeNode->load (ATOM_GetRenderDevice()))
	{
		return 0;
	}
	shapeNode->setMaterial (numShapeMaterial.get());
	shapeNode->setColor (color);

	float ratio = 0.1f * float(texture->getWidth())/float(texture->getHeight());
	shapeNode->setSize (ATOM_Vector3f(10.f * ratio * numDigits, 10.f, 1.f));

	return shapeNode;
}

ATOM_AUTOREF(ATOM_Node) NewHurtNumberEffect (ATOM_Node *parent, int number, const ATOM_Vector4f &color, const char *textureFileName)
{
	ATOM_AUTOREF(ATOM_ShapeNode) shapeNode = createNumberShapeNode (number, color, textureFileName);
	if (!shapeNode)
	{
		return 0;
	}

	if (parent)
	{
		parent->appendChild (shapeNode.get());
	}

	return shapeNode;
}
#endif

AssetNode::AssetNode (void)
{
#ifdef TEST_HURT_NUMBER_EFFECT
	_node = NewHurtNumberEffect (NULL, 23403, ATOM_Vector4f(1.f,1.f,1.f,1.f),"/textures/number.png");
#else
	_node = ATOM_HARDREF(ATOM_Node)();
#endif
	_node->setPickable (0);
	_node->setDrawBoundingbox (0);
}

bool AssetNode::loadFromFile (const char *filename)
{
	return true;
}

void AssetNode::deleteMe (void)
{
	ATOM_DELETE(this);
}

ATOM_Node *AssetNode::getNode (void)
{
	return _node.get();
}

bool AssetNode::isPickable (void) const
{
	return false;
}

bool AssetNode::supportBrushing (void) const
{
	return true;
}

AS_Asset *NodeAssetManager::createAsset (void)
{
	return ATOM_NEW(AssetNode);
}

unsigned NodeAssetManager::getNumFileExtensions (void) const
{
	return 1;
}

const char * NodeAssetManager::getFileExtension (unsigned) const
{
	return 0;
}

const char * NodeAssetManager::getFileDesc (unsigned) const
{
	return "Node";
}

void NodeAssetManager::setEditor (AS_Editor *editor)
{
	_editor = editor;
}

