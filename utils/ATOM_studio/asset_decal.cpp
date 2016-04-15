#include "StdAfx.h"
#include "asset.h"
#include "assetmanager.h"
#include "asset_decal.h"

AssetDecal::AssetDecal (void)
{
	create ();
}

void AssetDecal::create (void)
{
	_decalNode = ATOM_HARDREF(ATOM_Decal)();
	_decalNode->loadAttribute (NULL);
	_decalNode->load (ATOM_GetRenderDevice());
	_decalNode->setTexture (ATOM_GetColorTexture (0xFFFFFFFF));
}

bool AssetDecal::loadFromFile (const char *filename)
{
	return true;
}

void AssetDecal::deleteMe (void)
{
	ATOM_DELETE(this);
}

ATOM_Node *AssetDecal::getNode (void)
{
	return _decalNode.get();
}

bool AssetDecal::isPickable (void) const
{
	return true;
}

bool AssetDecal::supportBrushing (void) const
{
	return true;
}

AS_Asset *DecalAssetManager::createAsset (void)
{
	return ATOM_NEW(AssetDecal);
}

unsigned DecalAssetManager::getNumFileExtensions (void) const
{
	return 1;
}

const char * DecalAssetManager::getFileExtension (unsigned) const
{
	return 0;
}

const char * DecalAssetManager::getFileDesc (unsigned) const
{
	return "Decal";
}

void DecalAssetManager::setEditor (AS_Editor *editor)
{
	_editor = editor;
}

