#ifndef __NM_TRANSLATOR_H
#define __NM_TRANSLATOR_H

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
#include <maya/MIntArray.h>

#include "mesh.h"
#include "vertexanimation.h"
#include "skeleton.h"
#include "exportoptions.h"
#include "skeletonviewer.h"
#include "materialviewer.h"
#include "meshviewer.h"
#include "propedit.h"

class CMaterial 
{ 
public: 
	CMaterial(): transparency(1.f) 
	{} 

	std::string filenameColor;
	std::string filenameTrans;
	std::string filenameBump;
	MObject uvCoordColor;
	MObject uvFileColor;
	float transparency;
	bool alphaTest;
}; 
  
 class CDagData 
 { 
 public: 
         CDagData( 
                 int index, 
                 const MDagPath &mDagPath, 
                 const CDagData *pDagParent, 
                 const bool bBindCurrent); 
  
         int Index() const 
         { 
                 return m_index; 
         } 
  
         MString Name() const 
         { 
                 return MFnDagNode( m_mDagPath ).name(); 
         } 
  
         bool HasParent() const 
         { 
                 return( m_pDagParent != NULL ); 
         } 
  
         int ParentIndex() const 
         { 
                 return m_pDagParent ? m_pDagParent->Index() : -1; 
         } 
  
         MMatrix LocalMatrix() const 
         { 
                 return MFnDagNode( m_mDagPath ).transformationMatrix(); 
         } 
  
         MMatrix InclusiveMatrix() const 
         { 
                 return m_mDagPath.inclusiveMatrix(); 
         } 
  
         const MMatrix &BindMatrix() const 
         { 
                 return m_bindMatrix; 
         } 
  
         const MMatrix &WorldBindMatrix() const 
         { 
                 return m_worldBindMatrix; 
         } 

 protected: 
         const int m_index;                                      // SMD Index 
         const MDagPath m_mDagPath;                      // Maya Dag path to node 
         const CDagData *m_pDagParent;           // Pointer to parent node 
  
         const MMatrix m_bindMatrix;                     // Local space bind matrix 
         const MMatrix m_worldBindMatrix;        // World space bind matrix 
         MDagPathArray m_bboxes;                         // Bounding boxes of things to stay near the centre of 
 }; 
  
  
 class CMeshData 
 { 
 public: 
         CMeshData( 
                 const MDagPath &mDagPath, 
                 const MDagPath &baseDagPath, 
                 const MObject &skinObj, 
                 const MObject &blendObj, 
                 const bool bBindCurrent ); 
  
         const MDagPath &DagPath() const 
         { 
                 return m_mDagPath; 
         } 
  
         const MDagPath &BaseDagPath() const 
         { 
                 return m_baseDagPath; 
         } 
  
         const MMatrix &WorldBindMatrix() const 
         { 
                 return m_worldBindMatrix; 
         } 
  
         const MObject &SkinObj() const 
         { 
                 return m_skinObj; 
         } 
  
		 ATOM_Skeleton *SkeletonObj() const
		 { 
			 return m_skeleton; 
		 }

		 void setSkeletonObj(ATOM_Skeleton *skeleton)
		 { 
			 m_skeleton = skeleton; 
		 }

		 MFnSkinCluster *SkinCluster() const
		 {
			 return m_skinCluster;
		 }

		 void setSkinCluster (MFnSkinCluster *skinCluster)
		 {
			 m_skinCluster = skinCluster;
		 }

		 const MObjectArray &getShaders (void) const
		 {
			 return m_shaders;
		 }

		 const MIntArray &getShaderIndices (void) const
		 {
			 return m_indices;
		 }

		 const char *getName (void) const
		 {
			 return m_name.c_str();
		 }

		 PropertyEditor *getProp (void)
		 {
			 return &m_prop;
		 }

		 std::vector<MFloatArray> &getWeights (void)
		 {
			 return m_weights;
		 }

		 std::vector<MIntArray> &getWeightJoints (void)
		 {
			 return m_weightjoints;
		 }

protected: 
         const MDagPath m_mDagPath;                      // Maya Dag path to node 
         const MDagPath m_baseDagPath;           // The mesh before skin/blend deformers 
  
         const MMatrix m_worldBindMatrix;        // World space bind matrix 
  
         const MObject m_skinObj;                        // Skin deformer 
         const MObject m_blendObj;                       // Blend shape deformer 
         const MObject m_latticeObj;                     // Lattice deformer 
		 std::string m_name;
		 MObjectArray m_shaders;
		 MIntArray m_indices;
		 ATOM_Skeleton *m_skeleton;
		 MFnSkinCluster *m_skinCluster;

		 std::vector<MFloatArray> m_weights;
		 std::vector<MIntArray> m_weightjoints;

		 PropertyEditor m_prop;
}; 
 
class NM_Translator: public MPxFileTranslator
{
public:
	NM_Translator (void);
	virtual ~NM_Translator (void);

public:
	static void * creator (void);
	virtual bool haveReferenceMethod (void) const { return true; }
	virtual bool haveReadMethod (void) const { return false; }
	virtual bool haveWriteMethod (void) const { return true; }
	virtual bool haveNamespaceSupport (void) const { return true; }
	virtual MString defaultExtension (void) const { return MString("nm"); }
	virtual MString filter (void) const { return MString("*.nm"); }
	virtual bool canBeOpened (void) const { return true; }

	virtual MPxFileTranslator::MFileKind identifyFile (const MFileObject &fileObject, const char *buffer, short size) const;
	virtual MStatus writer (const MFileObject &file, const MString &optionsString, MPxFileTranslator::FileAccessMode mode);

public:
	void setOptions (unsigned options) { _options = options; }
	unsigned getOptions (void) { return _options; }
	const VertexAnimationInfo &getVertexAnimationInfo (void) const { return _vertexAnimationInfo; }
	void setVertexAnimationInfo (const VertexAnimationInfo& info) { _vertexAnimationInfo = info; }
	bool saveAnimationInfo (const VertexAnimationInfo &info, const char *filename);
	bool loadAnimationInfo (VertexAnimationInfo &info, const char *filename);

public:
//	MTime frameToTime (unsigned frame) const;
	bool doExport (void);
	void setShareDiffuseMap (bool b);
	void setShareNormalMap (bool b);

private:
	void clear (void);
	void createExportLists (const MSelectionList &masterExportLists);
	unsigned preprocessDag (const MDagPath &dagPath, bool force);
	void preprocessMesh (const MDagPath &dagPath);
	void preprocessSkinCluster (const MObject &skinObj);
	bool isNodeVisible( const MDagPath &mDagPath, bool bTemplateAsInvisible = true ) const;
	unsigned addPreprocessedMesh (const MDagPath &dagPath, const MObject &skinObj, const MObject &blendObj, const MObject &latticeObj);
	bool optBindCurrent (void) const;
	int getParentIndex (const MDagPath &dagPath) const;
	const CDagData *getParent (const MDagPath &dagPath) const;
	bool outputMesh (const MFnMesh &meshFn, const MDagPath &dagPath, std::vector<MyMesh> &meshes, std::vector<std::vector<unsigned> > &verticesIndices, std::vector<std::vector<unsigned> > &uvIndices, const MObjectArray &shaders, const MIntArray &indices);
	bool outputMeshW (const MFnMesh &meshFn, const MDagPath &dagPath, std::vector<MyMesh> &meshes, std::vector<std::vector<unsigned> > &verticesIndices, std::vector<std::vector<unsigned> > &uvIndices, const std::vector<MFloatArray> &weights, const std::vector<MIntArray> &weightjoints, const MObjectArray &shaders, const MIntArray &indices);
	void getFaceSets (const MDagPath &dagPath, const MFnMesh &meshFn, const MObjectArray &shaders, const MIntArray &indices, std::vector<MFnSingleIndexedComponent*> &faceSetFns);
	bool getUVCoords (const MObject &shadingGroupObj, CMaterial &m);
	bool getMaterial (const MObject &shadingGroupObj, material *m);
	MObject findInputNode (const MObject &dstObj, const MString &dstPlugName);
	MObject findInputNodeOfType (const MObject &dstObj, const MString &dstPlugName, const MString &typeName);
	bool outputFaceSet (MyMesh &mesh, 
		const MFnMesh &meshFn, 
		const CMaterial &material, 
		MObject &faceSetObj, 
		std::vector<unsigned> &verticesIndices, 
		std::vector<unsigned> &uvIndices,
		const MFloatPointArray &meshVertices,
		const MFloatVectorArray &meshNormals,
		const MFloatVectorArray &meshTangents,
		const MFloatArray &uIndices,
		const MFloatArray &vIndices
		);
	bool outputFaceSetW (MyMesh &mesh, 
		const MFnMesh &meshFn, 
		const CMaterial &material, 
		MObject &faceSetObj, 
		std::vector<unsigned> &verticesIndices, 
		std::vector<unsigned> &uvIndices,
		const MFloatPointArray &meshVertices,
		const MFloatVectorArray &meshNormals,
		const MFloatVectorArray &meshTangents,
		const std::vector<MFloatArray> &weights,
		const std::vector<MIntArray> &weightjoints,
		const MFloatArray &uIndices,
		const MFloatArray &vIndices
		);
	bool getVertexJointWeights (const MDagPath &mesh, MFnSkinCluster *skinCluster, std::vector<MFloatArray> &weights, std::vector<MIntArray> &weightjoints, ATOM_Skeleton *skeleton);
	void getNormal (const MFnMesh &meshFn, MItMeshVertex &vIt, int meshVertexIndex, MItMeshPolygon &pIt, int faceVertexIndex, MVector &n) const;
	bool translateFileName (const std::string &fileName, const std::string &savepath, std::string &vfsFileName, std::string &physicFileName, bool share) const;
	void outputDagInDependentOrder (const MDagPath &dagPath, MSelectionList  &done);
	void outputExportData (void);
	bool loadSkeleton (const MDagPath &meshDagPath, ATOM_Skeleton *skeleton);
	bool loadSkeletonNames (const MDagPath &meshDagPath, ATOM_Skeleton *skeleton);

	static void getHistory (const MDagPath &dagPath, MSelectionList &historyList);

public:
	static INT_PTR CALLBACK ATOM_mayaexporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

private:
	MSelectionList _nodeList;
	MSelectionList _meshList;

	typedef std::vector<const CDagData *> DagDataList;
	DagDataList _orderedDagList;

	typedef std::map<std::string, CDagData *> DagMap;
	DagMap _dagMap;

	typedef std::list<CMeshData*> MeshList;
	MeshList _meshDataList;

	std::map<std::string, CMaterial> _materialList;

	unsigned _maxNameLen;
	unsigned _type;
	unsigned _options;
	MMatrix _optRotMat;

	bool _shareDiffuseMap;
	bool _shareNormalMap;

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
};

#endif // __NM_TRANSLATOR_H
