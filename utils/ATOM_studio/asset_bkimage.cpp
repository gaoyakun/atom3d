#include "StdAfx.h"
#include "lightgeom.h"
#include "asset.h"
#include "assetmanager.h"
#include "asset_bkimage.h"

AssetBkImage::AssetBkImage (void)
{
	create ();
}

void AssetBkImage::create (void)
{
	_bkImage = ATOM_HARDREF(ATOM_BkImage)();
	_bkImage->load (ATOM_GetRenderDevice());
	_bkImage->setDrawBoundingbox (0);
}

bool AssetBkImage::loadFromFile (const char *filename)
{
	return true;
}

void AssetBkImage::deleteMe (void)
{
	ATOM_DELETE(this);
}

ATOM_Node *AssetBkImage::getNode (void)
{
	return _bkImage.get();
}

bool AssetBkImage::isPickable (void) const
{
	return true;
}

bool AssetBkImage::supportBrushing (void) const
{
	return false;
}

AS_Asset *BkImageAssetManager::createAsset (void)
{
	return ATOM_NEW(AssetBkImage);
}

unsigned BkImageAssetManager::getNumFileExtensions (void) const
{
	return 1;
}

const char * BkImageAssetManager::getFileExtension (unsigned) const
{
	return 0;
}

const char * BkImageAssetManager::getFileDesc (unsigned) const
{
	return "Background Image";
}

void BkImageAssetManager::setEditor (AS_Editor *editor)
{
	_editor = editor;
}

