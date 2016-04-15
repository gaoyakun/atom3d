#ifndef __MODELIO_H
#define __MODELIO_H

#include <ATOM_engine.h>

class ModelIO
{
public:
	ModelIO (void);
	~ModelIO (void);

public:
	bool load (const char *filename);
	unsigned getNumMeshes (void) const;

	// material attributes
	ATOM_Vector4f getSpecularColor (unsigned mesh, bool modified) const;
	void setSpecularColor (unsigned mesh, const ATOM_Vector4f &color, bool modified);
	ATOM_Vector4f getDiffuseColor (unsigned mesh, bool modified) const;
	void setDiffuseColor (unsigned mesh, const ATOM_Vector4f &color, bool modified);
	float getShininess (unsigned mesh, bool modified) const;
	void setShininess (unsigned mesh, float shininess, bool modified);
	bool isAlphaTestEnabled (unsigned mesh, bool modified) const;
	void enableAlphaTest (unsigned mesh, bool enable, bool modified);
	float getAlphaRef (unsigned mesh, bool modified) const;
	void setAlphaRef (unsigned mesh, float alpharef, bool modified) const;
	float getTransparency (unsigned mesh, bool modified) const;
	void setTransparency (unsigned mesh, float transparency, bool modified);
	bool isDoubleSided (unsigned mesh, bool modified) const;
	void setDoubleSided (unsigned mesh, bool doubleSided, bool modified);
	float getDiffuseGain (unsigned mesh, bool modified) const;
	void setDiffuseGain (unsigned mesh, float diffuseGain, bool modified);
	float getSpecularGain (unsigned mesh, bool modified);
	void setSpecularGain (unsigned mesh, float specularGain, bool modified);
	const char *getDiffuseMapFileName (unsigned mesh, bool modified) const;
	void setDiffuseMapFileName (unsigned mesh, const char *filename, bool modified);
	const char *getTransparencyMapFileName (unsigned mesh, bool modified) const;
	void setTransparencyMapFileName (unsigned mesh, const char *filename, bool modified);
	const char *getNormalMapFileName (unsigned mesh, bool modified) const;
	void setNormalMapFileName (unsigned mesh, const char *filename, bool modified);

	ATOM_VECTOR<unsigned> &getMeshChunkOffsets (void);
	ATOM_VECTOR<unsigned> &getMaterialChunkOffsets (void);

public:
	bool showEditDialog (HWND parentWnd);
	bool prepareModel (char *content, unsigned size);
	void syncModelProperties (HWND dlg, unsigned mesh);
	void applyModelProperties (HWND dlg, unsigned mesh);
	bool modified (void);
	ATOM_STRING chooseVFSFile (HWND dlg, const char *filter);
	bool chooseColor (HWND dlg, ATOM_ColorARGB &color);
	const char *getFileName (void) const;
	void clear (void);
	void restoreInitialValues (void);
	bool save (const char *filename);
	void fixKdKs (void);

private:
	ATOM_STRING _filename;
	char *_fileContent;
	char *_modifyCopy;
	unsigned _size;
	bool _supportKdKs;
	bool _KdKsChanged;
	ATOM_VECTOR<unsigned> _meshChunkOffsets;
	ATOM_VECTOR<unsigned> _materialChunkOffsets;
};

#endif // __MODELIO_H
