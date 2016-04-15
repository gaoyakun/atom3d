#ifndef __NM_TRANSLATOR_H
#define __NM_TRANSLATOR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "mesh.h"
#include "vertexanimation.h"
#include "skeleton.h"
#include "exportoptions.h"
#include "skeletonviewer.h"
#include "materialviewer.h"
#include "meshviewer.h"
#include "propedit.h"
#include "CrowdSimulationModel.h"

class CMaterial 
{ 
public: 
	CMaterial(): transparency(1.f) 
	{} 

	std::string filenameColor;
	std::string filenameTrans;
	std::string filenameBump;
	float transparency;
	bool alphaTest;
}; 
  
class ATOM_MaxExporter: public SceneExport
{
public:
	ATOM_MaxExporter (void);
	virtual ~ATOM_MaxExporter (void);

public:
	int				ExtCount();					// Number of extensions supported
	const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
	const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
	const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
	const TCHAR *	AuthorName();				// ASCII Author name
	const TCHAR *	CopyrightMessage();			// ASCII Copyright message
	const TCHAR *	OtherMessage1();			// Other message #1
	const TCHAR *	OtherMessage2();			// Other message #2
	unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
	void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box
	BOOL SupportsOptions(int ext, DWORD options);
	int DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

public:
	void setOptions (unsigned options) { _options = options; }
	unsigned getOptions (void) { return _options; }
	const VertexAnimationInfo &getVertexAnimationInfo (void) const { return _vertexAnimationInfo; }
	void setVertexAnimationInfo (const VertexAnimationInfo& info) { _vertexAnimationInfo = info; }
	bool saveAnimationInfo (const VertexAnimationInfo &info, const char *filename);
	bool loadAnimationInfo (VertexAnimationInfo &info, const char *filename);
	bool exportScene (const char *filename);

public:
	void setShareDiffuseMap (bool b);
	void setShareNormalMap (bool b);

private:
	void clear (void);
	bool translateFileName (const std::string &fileName, const std::string &savepath, std::string &vfsFileName, std::string &physicFileName, bool share) const;

public:
	static INT_PTR CALLBACK MaxExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

private:
	std::map<std::string, CMaterial> _materialList;
	std::string _outputFileName;

	unsigned _maxNameLen;
	unsigned _type;
	unsigned _options;

	bool _shareDiffuseMap;
	bool _shareNormalMap;
	bool _exportSelected;

	std::string _debugMaterial;
	std::string _outFileName;

	// vertex animation options
	VertexAnimationInfo _vertexAnimationInfo;

	ATOM_Skeleton *_skeleton;

public:
	bool _exportSkeleton;
	bool _exportSkeletonMesh;
	bool _exportSkeletonAnim;
	ExportOptions _exportOptions;
	SkeletonViewer _skeletonviewer;
	MaterialViewer _materialviewer;
	MeshViewer _meshviewer;
	PropertyEditor _modelprops;
	ExportManager *pEM;
};

#define ATOM_EXPORT_CLASS_ID Class_ID(0x3e190ae1, 0x321e70d8)
extern "C" HINSTANCE hModelHandle;

class ATOM_MaxExporterClassDesc : public ClassDesc2 {
public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new ATOM_MaxExporter(); }
	const TCHAR *	ClassName() { return _T("ATOM_MaxExporter"); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return ATOM_EXPORT_CLASS_ID; }
	const TCHAR* 	Category() { return _T("Export"); }

	const TCHAR*	InternalName() { return _T("ATOM_MaxExporter"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hModelHandle; }					// returns owning module handle
};

#endif // __NM_TRANSLATOR_H
