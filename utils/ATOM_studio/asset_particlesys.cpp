#include "StdAfx.h"
#include "asset.h"
#include "assetmanager.h"
#include "asset_particlesys.h"

AssetParticleSys::AssetParticleSys (void)
{
	_particleSystemNode = ATOM_HARDREF(ATOM_ParticleSystem)();
	_particleSystemNode->loadAttribute (NULL);
	_particleSystemNode->load (ATOM_GetRenderDevice());
	_particleSystemNode->setO2T (ATOM_Matrix4x4f::getIdentityMatrix());
}

bool AssetParticleSys::loadFromFile (const char *filename)
{
	return true;
}

void AssetParticleSys::deleteMe (void)
{
	ATOM_DELETE(this);
}

ATOM_Node *AssetParticleSys::getNode (void)
{
	return _particleSystemNode.get();
}

bool AssetParticleSys::isPickable (void) const
{
	return true;
}

bool AssetParticleSys::supportBrushing (void) const
{
	return true;
}

AS_Asset *ParticleSysAssetManager::createAsset (void)
{
	return ATOM_NEW(AssetParticleSys);
}

unsigned ParticleSysAssetManager::getNumFileExtensions (void) const
{
	return 1;
}

const char * ParticleSysAssetManager::getFileExtension (unsigned) const
{
	return 0;
}

const char * ParticleSysAssetManager::getFileDesc (unsigned) const
{
	return "ParticleSystem";
}

void ParticleSysAssetManager::setEditor (AS_Editor *editor)
{
	_editor = editor;
}

