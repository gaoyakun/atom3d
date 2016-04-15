#ifndef __ATOM3D_STUDIO_ASSET_PARTICLESYS_H
#define __ATOM3D_STUDIO_ASSET_PARTICLESYS_H

#if _MSC_VER > 1000
# pragma once
#endif

class AssetParticleSys: public AS_Asset
{
public:
	AssetParticleSys (void);

public:
	virtual bool loadFromFile (const char *filename);
	virtual void deleteMe (void);
	virtual ATOM_Node *getNode (void);
	virtual bool isPickable (void) const;
	virtual bool supportBrushing (void) const;

private:
	ATOM_AUTOREF(ATOM_ParticleSystem) _particleSystemNode;
};

class ParticleSysAssetManager: public AS_AssetManager
{
	virtual AS_Asset *createAsset (void);
	virtual unsigned getNumFileExtensions (void) const;
	virtual const char * getFileExtension (unsigned) const;
	virtual const char * getFileDesc (unsigned) const;
	virtual void setEditor (AS_Editor *editor);

private:
	AS_Editor *_editor;
};

#endif // __ATOM3D_STUDIO_ASSET_PARTICLESYS_H
