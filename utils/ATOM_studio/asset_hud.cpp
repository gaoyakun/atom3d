#include "StdAfx.h"
#include "asset.h"
#include "assetmanager.h"
#include "asset_hud.h"

AssetHud::AssetHud (void)
{
	create ();
}

void AssetHud::create (void)
{
	_hudNode = ATOM_HARDREF(ATOM_Hud)();
	_hudNode->setTexture (ATOM_GetColorTexture (0xFFFFFFFF));
}

bool AssetHud::loadFromFile (const char *filename)
{
	return true;
}

void AssetHud::deleteMe (void)
{
	ATOM_DELETE(this);
}

ATOM_Node *AssetHud::getNode (void)
{
	return _hudNode.get();
}

bool AssetHud::isPickable (void) const
{
	return true;
}

bool AssetHud::supportBrushing (void) const
{
	return true;
}

AS_Asset *HudAssetManager::createAsset (void)
{
	return ATOM_NEW(AssetHud);
}

unsigned HudAssetManager::getNumFileExtensions (void) const
{
	return 1;
}

const char * HudAssetManager::getFileExtension (unsigned) const
{
	return 0;
}

const char * HudAssetManager::getFileDesc (unsigned) const
{
	return "Hud";
}

void HudAssetManager::setEditor (AS_Editor *editor)
{
	_editor = editor;
}

