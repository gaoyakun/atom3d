#ifndef __MTL_TRANSLATOR_H
#define __MTL_TRANSLATOR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <vector>
#include <map>
#include <list>
#include <string>

#include <maya/MPxFileTranslator.h>
#include <maya/MFStream.h>
#include <maya/MMatrix.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnDagNode.h>
#include <maya/MSelectionList.h>
#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MFnMesh.h>
#include <maya/MObjectArray.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItMeshPolygon.h>

#include "mesh.h"

struct ExporterConfig;

class MTL_Translator: public MPxFileTranslator
{
public:
	struct MayaMaterial
	{
		std::string name;
		material mat;
		MObject mayaObj;
		std::vector<std::pair<std::string, std::string> > fileToCopy;
	};

	enum ColorMode
	{
		MaterialColor,
		VertexColor,
		TextureColor,
		InterpolateColor
	};

public:
	MTL_Translator (void);
	virtual ~MTL_Translator (void);

public:
	static void * creator (void);
	virtual bool haveReferenceMethod (void) const { return true; }
	virtual bool haveReadMethod (void) const { return false; }
	virtual bool haveWriteMethod (void) const { return true; }
	virtual bool haveNamespaceSupport (void) const { return true; }
	virtual MString defaultExtension (void) const { return MString("nmtl"); }
	virtual MString filter (void) const { return MString("*.nmtl"); }
	virtual bool canBeOpened (void) const { return true; }

	virtual MPxFileTranslator::MFileKind identifyFile (const MFileObject &fileObject, const char *buffer, short size) const;
	virtual MStatus writer (const MFileObject &file, const MString &optionsString, MPxFileTranslator::FileAccessMode mode);

public:
	bool getOptDoubleSide (void) const;
	bool getOptShareTexture (void) const;
	bool getOptLighting (void) const;
	ColorMode getColorMode (void) const;
	void setOptDoubleSide (bool b);
	void setOptShareTexture (bool b);
	void setOptLighting (bool b);
	void setColorMode (ColorMode colormode);
	void setExportIndex (int index);
	const std::vector<MayaMaterial> &getMaterialList(void) const;

private:
	int getMaterialId (const MObject &shaderObj);
	bool getMaterial (const MObject &shaderObj, material *m, std::vector<std::pair<std::string, std::string> > &fileToCopy);
	bool translateFileName (const std::string &fileName, const std::string &savepath, std::string &vfsFileName, std::string &physicFileName, bool share) const;
	bool outputMaterialFile (const MayaMaterial &m) const;

private:
	std::vector<MayaMaterial> _materials;
	bool _shareTexture;
	bool _doubleside;
	bool _lighting;
	ColorMode _colormode;
	int _index;
	std::string _outFileName;
};

#endif // __MTL_TRANSLATOR_H
