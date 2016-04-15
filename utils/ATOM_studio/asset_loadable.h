#ifndef __ATOM3D_STUDIO_ASSET_LOADABLE_H
#define __ATOM3D_STUDIO_ASSET_LOADABLE_H

#if _MSC_VER > 1000
# pragma once
#endif

class LoadableAsset: public AS_Asset
{
public:
	virtual ~LoadableAsset (void);

	virtual bool loadFromFile (const char *filename);

	virtual void deleteMe (void);

	virtual ATOM_Node *getNode (void);

	virtual bool isPickable (void) const;

	virtual bool supportBrushing (void) const;

private:
	ATOM_AUTOREF(ATOM_Node) _node;
};

class LoadableAssetManager: public AS_AssetManager
{
public:
	LoadableAssetManager (void);

public:
	virtual AS_Asset *createAsset (void);
	virtual unsigned getNumFileExtensions (void) const;
	virtual const char * getFileExtension (unsigned index) const;
	virtual const char * getFileDesc (unsigned index) const;
	virtual void setEditor (AS_Editor *editor);

private:
	AS_Editor *_editor;
	ATOM_VECTOR<ATOM_STRING> _extensions;
	ATOM_VECTOR<ATOM_STRING> _desc;
};

#endif // __ATOM3D_STUDIO_ASSET_LOADABLE_H
