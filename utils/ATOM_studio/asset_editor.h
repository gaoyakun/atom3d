#ifndef __ATOM3D_STUDIO_ASSET_EDITOR_H
#define __ATOM3D_STUDIO_ASSET_EDITOR_H

#if _MSC_VER > 1000
# pragma once
#endif

#define ASSETEDITOR_COMMAND_CREATE_ADDITIONAL	-500

#define ASSETEDITOR_COMMAND_CREATE	-7
#define ASSETEDITOR_COMMAND_DELETE	-6
#define ASSETEDITOR_COMMAND_LOAD	-5
#define ASSETEDITOR_COMMAND_SAVE	-4
#define ASSETEDITOR_COMMAND_SAVEAS	-3
#define ASSETEDITOR_COMMAND_NEW		-2
#define ASSETEDITOR_COMMAND_CLEAR	-1

class AS_Asset;
class AS_AssetManager;
class EditorImpl;

class AssetEditor
{
private:
	struct AssetInfo
	{
		ATOM_STRING name;
		ATOM_STRING filename;
		ATOM_STRING desc;
		float rotateX;
		float rotateY;
		float rotateZ;
		float offsetY;
	};

public:
	AssetEditor (EditorImpl *editor);
	~AssetEditor (void);

public:
	void clear (void);
	bool load (void);
	bool load (const char *filename);
	bool save (void);
	bool save (const char *filename);
	bool saveAs (void);
	void setPosition (int x, int y, int w, int h);
	unsigned getNumAssets (void) const;
	const char *getAssetName (unsigned index) const;
	const char *getAssetFileName (unsigned index) const;
	const char *getAssetDesc (unsigned index) const;
	float getAssetRotateX (unsigned index) const;
	float getAssetRotateY (unsigned index) const;
	float getAssetRotateZ (unsigned index) const;
	float getAssetOffsetY (unsigned index) const;
	void newAsset (void);
	void deleteAsset (unsigned index);
	void deleteAll (void);
	void show (bool b);
	EditorImpl *getEditor (void) const;
	const char *getAssetManagerName (int index) const;
	const char *getAssetManagerDesc (int index) const;

private:
	void createBar (void);
	void addAssetToBar (unsigned index);

private:
	ATOMX_TweakBar *_bar;
	EditorImpl *_editor;
	ATOM_STRING _currentFileName;
	ATOM_VECTOR<AssetInfo*> _assets;
	ATOM_VECTOR<std::pair<ATOM_STRING,ATOM_STRING> > _additionalAssetManagers;
};

#endif // __ATOM3D_STUDIO_ASSET_EDITOR_H
