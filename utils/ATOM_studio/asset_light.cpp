#include "StdAfx.h"
#include "lightgeom.h"
#include "asset.h"
#include "assetmanager.h"
#include "asset_light.h"

AssetLight::AssetLight (void)
{
	create (ATOM_Light::Point);
}

void AssetLight::create (ATOM_Light::LightType type)
{
	_lightNode = ATOM_HARDREF(ATOM_LightNode)();
	_lightNode->setPickable (0);
	_lightNode->setDrawBoundingbox (0);
	_lightNode->setLightType (type);
	_lightNode->setScale (ATOM_Vector3f(50.f, 50.f, 50.f));

	ATOM_HARDREF(LightGeode) lightGeode;
	lightGeode->setLightType (type);
	lightGeode->load (ATOM_GetRenderDevice ());
	lightGeode->setPickable (1);

	_lightNode->appendChild (lightGeode.get());
}

bool AssetLight::loadFromFile (const char *filename)
{
	return true;
}

void AssetLight::deleteMe (void)
{
	ATOM_DELETE(this);
}

ATOM_Node *AssetLight::getNode (void)
{
	return _lightNode.get();
}

bool AssetLight::isPickable (void) const
{
	return false;
}

bool AssetLight::supportBrushing (void) const
{
	return true;
}

AS_Asset *LightAssetManager::createAsset (void)
{
	return ATOM_NEW(AssetLight);
}

unsigned LightAssetManager::getNumFileExtensions (void) const
{
	return 1;
}

const char * LightAssetManager::getFileExtension (unsigned) const
{
	return 0;
}

const char * LightAssetManager::getFileDesc (unsigned) const
{
	return "Light";
}

void LightAssetManager::setEditor (AS_Editor *editor)
{
	_editor = editor;
}

