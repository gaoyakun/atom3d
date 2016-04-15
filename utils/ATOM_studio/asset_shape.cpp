#include "StdAfx.h"
#include "asset.h"
#include "assetmanager.h"
#include "asset_shape.h"

AssetShape::AssetShape (void)
{
	_shapeNode = ATOM_HARDREF(ATOM_ShapeNode)();
	_shapeNode->loadAttribute (NULL);
	_shapeNode->load (ATOM_GetRenderDevice());
	_shapeNode->setO2T (ATOM_Matrix4x4f::getIdentityMatrix());
}

bool AssetShape::loadFromFile (const char *filename)
{
	return true;
}

void AssetShape::deleteMe (void)
{
	ATOM_DELETE(this);
}

ATOM_Node *AssetShape::getNode (void)
{
	return _shapeNode.get();
}

bool AssetShape::isPickable (void) const
{
	return true;
}

bool AssetShape::supportBrushing (void) const
{
	return true;
}

AS_Asset *ShapeAssetManager::createAsset (void)
{
	return ATOM_NEW(AssetShape);
}

unsigned ShapeAssetManager::getNumFileExtensions (void) const
{
	return 1;
}

const char * ShapeAssetManager::getFileExtension (unsigned) const
{
	return 0;
}

const char * ShapeAssetManager::getFileDesc (unsigned) const
{
	return "Shape";
}

void ShapeAssetManager::setEditor (AS_Editor *editor)
{
	_editor = editor;
}

