#include "StdAfx.h"
#include "asset.h"
#include "assetmanager.h"
#include "asset_loadable.h"

LoadableAsset::~LoadableAsset (void)
{
}

bool LoadableAsset::loadFromFile (const char *filename)
{
	_node = ATOM_Node::loadNodeFromFile (filename,1);

	if (!_node)
	{
		return false;
	}

	return true;
}

void LoadableAsset::deleteMe (void)
{
	ATOM_DELETE(this);
}

ATOM_Node *LoadableAsset::getNode (void)
{
	return _node.get();
}

bool LoadableAsset::isPickable (void) const
{
	return true;
}

bool LoadableAsset::supportBrushing (void) const
{
	return dynamic_cast<ATOM_Terrain*>(_node.get()) == 0;
}

LoadableAssetManager::LoadableAssetManager (void)
{
	unsigned numFileTypes = ATOM_Node::getNumRegisteredFileTypes ();
	_extensions.resize (numFileTypes);
	_desc.resize (numFileTypes);
	for (unsigned i = 0; i < numFileTypes; ++i)
	{
		ATOM_Node::getRegisteredFileInfo (i, _extensions[i], _desc[i]);
	}
}

AS_Asset *LoadableAssetManager::createAsset (void)
{
	return ATOM_NEW(LoadableAsset);
}

unsigned LoadableAssetManager::getNumFileExtensions (void) const
{
	return _extensions.size();
}

const char * LoadableAssetManager::getFileExtension (unsigned index) const
{
	return _extensions[index].c_str();
}

const char * LoadableAssetManager::getFileDesc (unsigned index) const
{
	return _desc[index].c_str();
}

void LoadableAssetManager::setEditor (AS_Editor *editor)
{
	_editor = editor;
}

