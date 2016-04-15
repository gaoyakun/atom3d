#include "StdAfx.h"
#include "asset.h"
#include "assetmanager.h"
#include "asset_weapontrail.h"

AssetWeaponTrail::AssetWeaponTrail (void)
{
	create ();
}

void AssetWeaponTrail::create (void)
{
	_trailNode = ATOM_HARDREF(ATOM_WeaponTrail)();
	_trailNode->setPickable (0);
	_trailNode->setDrawBoundingbox (0);
	_trailNode->loadAttribute (NULL);
	_trailNode->load (ATOM_GetRenderDevice());
}

bool AssetWeaponTrail::loadFromFile (const char *filename)
{
	return true;
}

void AssetWeaponTrail::deleteMe (void)
{
	ATOM_DELETE(this);
}

ATOM_Node *AssetWeaponTrail::getNode (void)
{
	return _trailNode.get();
}

bool AssetWeaponTrail::isPickable (void) const
{
	return false;
}

bool AssetWeaponTrail::supportBrushing (void) const
{
	return true;
}

AS_Asset *WeaponTrailAssetManager::createAsset (void)
{
	return ATOM_NEW(AssetWeaponTrail);
}

unsigned WeaponTrailAssetManager::getNumFileExtensions (void) const
{
	return 1;
}

const char * WeaponTrailAssetManager::getFileExtension (unsigned) const
{
	return 0;
}

const char * WeaponTrailAssetManager::getFileDesc (unsigned) const
{
	return "Weapon Trail";
}

void WeaponTrailAssetManager::setEditor (AS_Editor *editor)
{
	_editor = editor;
}

