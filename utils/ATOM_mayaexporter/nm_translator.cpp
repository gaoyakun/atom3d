#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shlobj.h>

#include "nm_translator.h"
#include "mesh.h"
#include "model.h"
#include "resource.h"
#include "config.h"
#include "skeleton.h"
#include "tinyxml.h"
#include "propedit.h"

#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MObjectArray.h>
#include <maya/MItDag.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MFnAttribute.h>
#include <maya/MFnSet.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MAnimUtil.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MFnMatrixData.h>
#include <maya/MPointArray.h>
#include <maya/MFnLambertShader.h>
#include <maya/MFnBlinnShader.h>
#include <maya/MFnPhongShader.h>
#include <maya/MFnAnisotropyShader.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MAnimControl.h>
#include <maya/MFloatPointArray.h>
#include <maya/MItGeometry.h>
#include <maya/MBoundingBox.h>

extern HINSTANCE hModelHandle;

static void CenterWindow (HWND child, HWND parent)
{
	RECT rcChild, rcParent;
	::GetWindowRect (child, &rcChild);
	::GetWindowRect (parent, &rcParent);
	int childWidth = rcChild.right - rcChild.left + 1;
	int childHeight = rcChild.bottom - rcChild.top + 1;
	int parentWidth = rcParent.right - rcParent.left + 1;
	int parentHeight = rcParent.bottom - rcParent.top + 1;
	int x = rcParent.left + (parentWidth - childWidth) / 2;
	int y = rcParent.top + (parentHeight - childHeight) / 2;
	::MoveWindow (child, x, y, childWidth, childHeight, FALSE);
}

static char gRootDir[MAX_PATH];

static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT msg, LPARAM lp, LPARAM pData)   
{   
	if (msg == BFFM_INITIALIZED && gRootDir[0])   
	{   
		::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)gRootDir);   
	}   
	return   0;   
}  

static BOOL chooseDir (HWND hWndParent, const char *title, char *initialDir)
{
	BROWSEINFO bi;
	char folderName[MAX_PATH];
	memset(&bi, 0, sizeof(bi));
	memset(folderName, 0, sizeof(folderName));
	bi.hwndOwner = hWndParent;
	bi.pszDisplayName = 0;
	bi.lpszTitle = title;
	bi.ulFlags = BIF_STATUSTEXT|BIF_USENEWUI|BIF_RETURNONLYFSDIRS;
	bi.lpfn = &BrowseCallbackProc;

	strcpy (gRootDir, initialDir);
	LPITEMIDLIST itemId = SHBrowseForFolder (&bi);
	if (itemId)
	{
		SHGetPathFromIDList (itemId, folderName);
		GlobalFreePtr (itemId);
		strcpy (initialDir, folderName);
		return TRUE;
	}
	return FALSE;
}

static char *removeSpace (char *s)
{
	char *h = s;
	while (*h == ' ' || *h == '\t' || *h == '\n')
	{
		++h;
	}

	unsigned l = strlen (h);
	if (l)
	{
		char *e = h + l - 1;
		while (*e == ' ' || *e == '\t' || *e == '\n')
		{
			*e = '\0';
			--e;
		}
	}
	return h;
}

INT_PTR CALLBACK NM_Translator::ATOM_mayaexporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static NM_Translator *translator = NULL;
	static int animchoice = 0; // 0: no animation 1: vertex animation 2: bone animation

	switch(message) {
		case WM_INITDIALOG:
			{
				translator = (NM_Translator*)lParam;
				
				::SendMessage (::GetDlgItem (hWnd, IDC_EXPORTCOLOR), BM_SETCHECK, translator->_exportOptions.exportVertexColor() ? BST_CHECKED : BST_UNCHECKED, 0);
				::SendMessage (::GetDlgItem (hWnd, IDC_EXPORTVERTEXALPHA), BM_SETCHECK, translator->_exportOptions.exportVertexTransparency() ? BST_CHECKED : BST_UNCHECKED, 0);
				::SendMessage (::GetDlgItem (hWnd, IDC_SAVEANIMATION_SETTINGS), BM_SETCHECK, translator->_exportOptions.saveAnimationSettings() ? BST_CHECKED : BST_UNCHECKED, 0);
				::SendMessage (::GetDlgItem (hWnd, IDC_EXPOPT_ACTIONS), BM_SETCHECK, translator->_exportOptions.exportActions() ? BST_CHECKED : BST_UNCHECKED, 0);
				::SendMessage (::GetDlgItem (hWnd, IDC_EXPOPT_MESHES), BM_SETCHECK, translator->_exportOptions.exportMeshes() ? BST_CHECKED : BST_UNCHECKED, 0);
				::SendMessage (::GetDlgItem (hWnd, IDC_EXPOPT_SKELETONS), BM_SETCHECK, translator->_exportOptions.exportSkeletons() ? BST_CHECKED : BST_UNCHECKED, 0);
				::SendMessage (::GetDlgItem (hWnd, IDC_DOUBLESIDE), BM_SETCHECK, translator->_exportOptions.doubleSide() ? BST_CHECKED : BST_UNCHECKED, 0);
				::SendMessage (::GetDlgItem (hWnd, IDC_SELECTBONEBYNAME), BM_SETCHECK, translator->_exportOptions.getSelectBoneByName() ? BST_CHECKED : BST_UNCHECKED, 0);
				::EnableWindow (::GetDlgItem (hWnd, IDC_VIEWSKELETONS), translator->_skeletonviewer.getSkeleton().skeleton ? TRUE : FALSE);
				::EnableWindow (::GetDlgItem (hWnd, IDC_EXPOPT_SKELETONS), translator->_skeletonviewer.getSkeleton().skeleton ? TRUE : FALSE);

				ComboBox_AddString (::GetDlgItem (hWnd, IDC_BINDPOSE), "使用第一帧");
				ComboBox_AddString (::GetDlgItem (hWnd, IDC_BINDPOSE), "使用当前帧");
				ComboBox_SetCurSel (::GetDlgItem (hWnd, IDC_BINDPOSE), translator->_exportOptions.getBindPoseSource());

				CenterWindow(hWnd,GetParent(hWnd));
				animchoice = 0;
				return TRUE;
			}

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case IDC_SELECTBONEBYNAME:
				{
					translator->_exportOptions.setSelectBoneByName(BST_CHECKED == ::SendMessage(::GetDlgItem(hWnd, IDC_SELECTBONEBYNAME), BM_GETCHECK, 0, 0));
					::MessageBoxA (hWnd, "注意!修改此选项将会导致所有手工选择的骨骼丢失!", "ATOM3D导出插件", MB_OK|MB_ICONEXCLAMATION);
					translator->_skeletonviewer.setSkeleton (translator->_skeleton, translator->_meshDataList, translator->_exportOptions.getSelectBoneByName());
					break;
				}
			case IDC_MODELPROPS:
				{
					translator->_modelprops.edit (hWnd);
					break;
				}
			case IDC_VIEWSKELETONS:
				{
					translator->_skeletonviewer.showDialog (hWnd);
					break;
				}
			case IDC_VIEWMATERIALS:
				{
					translator->_materialviewer.edit (hWnd);
					break;
				}
			case IDC_VIEWMESHES:
				{
					translator->_meshviewer.edit (hWnd);
					break;
				}
			case IDC_OPT_VERTEXANIM:
				{
					VertexAnimationInfo info = translator->getVertexAnimationInfo();
					if (editVertexAnimation (hModelHandle, hWnd, &info))
					{
						translator->setVertexAnimationInfo(info);
					}
					break;
				}
			case IDC_EXPORT:
				{
					HWND hChkShareDiffuseMap = ::GetDlgItem (hWnd, IDC_SHAREDIFFUSEMAP);
					HWND hChkShareNormalMap = ::GetDlgItem (hWnd, IDC_SHARENORMALMAP);
					translator->setShareDiffuseMap(BST_CHECKED == ::SendMessage (hChkShareDiffuseMap, BM_GETCHECK, 0, 0));
					translator->setShareNormalMap(BST_CHECKED == ::SendMessage (hChkShareNormalMap, BM_GETCHECK, 0, 0));
					ExporterConfig &exporterConfig = ExporterConfig::getInstance();

					if (exporterConfig.rootDirectory.empty () || exporterConfig.shareDirectory.empty())
					{
						MessageBox (hWnd, "先设定要保存的资源目录。", "atom exporter", MB_OK|MB_ICONHAND);
						return FALSE;
					}

					translator->_exportOptions.setBindPoseSource (ComboBox_GetCurSel (::GetDlgItem (hWnd, IDC_BINDPOSE)));

					translator->_exportOptions.setExportVertexColor(
						BST_CHECKED == ::SendMessage (::GetDlgItem(hWnd, IDC_EXPORTCOLOR), BM_GETCHECK, 0, 0));

					translator->_exportOptions.setExportVertexTransparency(
						BST_CHECKED == ::SendMessage (::GetDlgItem (hWnd, IDC_EXPORTVERTEXALPHA), BM_GETCHECK, 0, 0));

					translator->_exportOptions.setExportActions(
						BST_CHECKED == ::SendMessage (::GetDlgItem(hWnd, IDC_EXPOPT_ACTIONS), BM_GETCHECK, 0, 0));

					translator->_exportOptions.setSaveAnimationSettings(
						BST_CHECKED == ::SendMessage (::GetDlgItem (hWnd, IDC_SAVEANIMATION_SETTINGS), BM_GETCHECK, 0, 0));

					translator->_exportOptions.setExportMeshes(
						BST_CHECKED == ::SendMessage (::GetDlgItem(hWnd, IDC_EXPOPT_MESHES), BM_GETCHECK, 0, 0));

					translator->_exportOptions.setExportSkeletons(
						BST_CHECKED == ::SendMessage (::GetDlgItem(hWnd, IDC_EXPOPT_SKELETONS), BM_GETCHECK, 0, 0));

					translator->_exportOptions.setDoubleSide(
						BST_CHECKED == ::SendMessage (::GetDlgItem(hWnd, IDC_DOUBLESIDE), BM_GETCHECK, 0, 0));

					translator->_exportOptions.setSelectBoneByName(
						BST_CHECKED == ::SendMessage (::GetDlgItem(hWnd, IDC_SELECTBONEBYNAME), BM_GETCHECK, 0, 0));

					if (!translator->_exportOptions.exportActions() && !translator->_exportOptions.exportMeshes() && !translator->_exportOptions.exportSkeletons())
					{
						MessageBox (hWnd, "想导出什么？", "atom exporter", MB_OK|MB_ICONQUESTION);
						return FALSE;
					}
					
					translator->doExport ();
					EndDialog (hWnd, 0);

					break;
				}
			case IDC_SETUPDIR:
				{
					ExporterConfig &exporterConfig = ExporterConfig::getInstance();
					char oldrootDir[MAX_PATH];
					char oldshareDir[MAX_PATH];
					strcpy (oldrootDir, exporterConfig.rootDirectory.c_str());
					if (chooseDir (hWnd, "选择资源根目录:", oldrootDir))
					{
						strcpy (oldshareDir, exporterConfig.shareDirectory.c_str());
						if (chooseDir (hWnd, "选择共享目录:", oldshareDir))
						{
							exporterConfig.rootDirectory = oldrootDir;
							exporterConfig.shareDirectory = oldshareDir;
							exporterConfig.save ();
						}
					}
					break;
				}
			default:
				return FALSE;
			}
			break;
		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return 1;
	}
	return 0;
}

static std::string identifyFileName (const char *filename)
{
	char buffer[MAX_PATH];

	if (!::GetFullPathName (filename, MAX_PATH, buffer, 0))
	{
		return "";
	}

	return buffer;
}

static std::string generateVFSPath (const char *root, const char *subdir)
{
	char identRoot[MAX_PATH];
	char identSub[MAX_PATH];
	::GetFullPathName (root, MAX_PATH, identRoot, 0);
	::GetFullPathName (subdir, MAX_PATH, identSub, 0);
	int rootLen = strlen(identRoot);
	int subLen = strlen(identSub);
	if (rootLen > subLen)
	{
		return "";
	}
	if (_strnicmp (identRoot, identSub, rootLen))
	{
		return "";
	}
	char *s = identSub + rootLen;
	for (char *s2 = s; *s2; ++s2)
	{
		if (*s2 == '\\')
		{
			*s2 = '/';
		}
	}
	std::string ret;
	if (s[0] != '/')
	{
		ret += '/';
	}
	ret += s;
	return ret;
}

static bool confirmFilePath (std::string &fileName)
{
	fileName = identifyFileName (fileName.c_str());
	if (fileName.empty ())
	{
		return false;
	}

	FILE *fp = fopen (fileName.c_str(), "rb");
	if (fp)
	{
		fclose (fp);
		return true;
	}

	const char *p = strrchr (fileName.c_str(), '\\');
	if (p)
	{
		return true;
	}

	OPENFILENAME ofn;
	char fileNameBuffer[MAX_PATH];

	memset (&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = ::GetActiveWindow ();
	ofn.lpstrFile = fileNameBuffer;
	strcpy (ofn.lpstrFile, fileName.c_str());
	ofn.nMaxFile = sizeof(fileNameBuffer);

	char filterBuffer[MAX_PATH];
	strcpy (filterBuffer, "*.");
	const char *ext = strrchr (fileName.c_str(), '.');
	strcat (filterBuffer, ext ? ext+1 : "*");
	char *s2 = filterBuffer+strlen(filterBuffer)+1;
	strcpy (s2, filterBuffer);
	*(s2 + strlen(s2) + 1) = '\0';

	ofn.lpstrFilter = filterBuffer;
	ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;

	if (::GetOpenFileName (&ofn))
	{
		fileName = fileNameBuffer;
		return true;
	}

	return false;
}

bool NM_Translator::translateFileName (const std::string &fileName, const std::string &savepath, std::string &vfsFileName, std::string &physicFileName, bool share) const
{
	std::string srcPath = fileName;
	if (!confirmFilePath (srcPath))
	{
		MessageBox (::GetActiveWindow(), "导出过程中出现了一个问题,导出失败.", "atom exporter", MB_OK|MB_ICONHAND);
		return false;
	}

	std::string FilePart = strrchr (srcPath.c_str(), '\\') + 1;
	std::string SavePath = savepath.substr (0, savepath.find_last_of ('\\') + 1);
	std::string dir;

	ExporterConfig &exporterConfig = ExporterConfig::getInstance();
	if (!share)
	{
		dir = generateVFSPath (exporterConfig.rootDirectory.c_str(), SavePath.c_str());
		if (dir.empty ())
		{
			MessageBox (::GetActiveWindow(), "保存的路径有问题，请确保是保存在资源根目录之下。", "atom exporter", MB_OK|MB_ICONHAND);
			return false;
		}
	}
	else
	{
		dir = generateVFSPath (exporterConfig.rootDirectory.c_str(), exporterConfig.shareDirectory.c_str());
		if (dir[dir.length()-1] != '/')
		{
			dir += '/';
		}
	}

	vfsFileName = dir + FilePart;

	physicFileName = share ? exporterConfig.shareDirectory : SavePath;
	if (physicFileName[physicFileName.length()-1] != '\\') physicFileName += '\\';
	physicFileName += FilePart;

	return true;
}

static MMatrix getBindMatrix (const MDagPath &mDagPath, const bool bindCurrent)
{
	if (!bindCurrent)
	{
		MPlugArray mPlugArray;
		MPlug plug(MFnDagNode (mDagPath).findPlug ("bindPose"));

		if (!plug.isNull () && plug.connectedTo (mPlugArray, false, true) && mPlugArray.length())
		{
			const MPlug bindPosePlug (mPlugArray[0]);
			const MObject dagPoseObj (bindPosePlug.node ());
			const MFnDependencyNode dagPoseFn (dagPoseObj);
			const MObject xformMatrixObj (dagPoseFn.attribute ("xformMatrix"));

			MPlug xformMatrixPlug (dagPoseObj, xformMatrixObj);
			xformMatrixPlug.selectAncestorLogicalIndex (bindPosePlug.logicalIndex (), xformMatrixObj);
			MObject xformMatrixVal;

			if (xformMatrixPlug.getValue (xformMatrixVal))
			{
				return MFnMatrixData (xformMatrixVal).matrix ();
			}
		}
	}

	return MFnDagNode (mDagPath).transformationMatrix ();
}

static MMatrix getWorldBindMatrix(const MDagPath &mDagPath, const bool bindCurrent ) 
{ 
	if ( !bindCurrent ) 
	{ 
		MPlugArray mPlugArray; 
		MPlug mPlug( MFnDagNode( mDagPath ).findPlug( "bindPose" ) ); 
		if ( !mPlug.isNull() && mPlug.connectedTo( mPlugArray, false, true ) && mPlugArray.length() ) 
		{ 
			const MPlug bindPosePlug( mPlugArray[ 0 ] ); 

			const MObject dagPoseObj( bindPosePlug.node() ); 
			const MFnDependencyNode dagPoseFn( dagPoseObj ); 

			const MObject worldMatrixObj( dagPoseFn.attribute( "worldMatrix" ) ); 

			MPlug xformMatrixPlug( dagPoseObj, worldMatrixObj ); 
			xformMatrixPlug.selectAncestorLogicalIndex( bindPosePlug.logicalIndex(), worldMatrixObj ); 

			MObject worldMatrixVal; 

			if ( xformMatrixPlug.getValue( worldMatrixVal ) ) 
			{
				return MFnMatrixData( worldMatrixVal ).matrix(); 
			}
		} 
	} 
	return mDagPath.inclusiveMatrix(); 
} 

CMeshData::CMeshData (const MDagPath &dagPath, const MDagPath &baseDagPath, const MObject &skinObj, const MObject &blendObj, const bool bBindCurrent)
: m_mDagPath(dagPath)
, m_baseDagPath(bBindCurrent ? dagPath : baseDagPath)
, m_worldBindMatrix(getWorldBindMatrix (dagPath, bBindCurrent))
, m_skinObj (skinObj)
, m_blendObj (blendObj)
, m_skeleton (0)
, m_skinCluster (0)
{
	MFnMesh meshFn (m_mDagPath);
	m_name = meshFn.name().asChar();
	meshFn.getConnectedShaders (m_mDagPath.instanceNumber(), m_shaders, m_indices);
}

CDagData::CDagData (int index, const MDagPath &dagPath, const CDagData *dagParent, const bool bBindCurrent)
: m_index (index)
, m_mDagPath (dagPath)
, m_pDagParent (dagParent)
, m_bindMatrix (getBindMatrix (dagPath, bBindCurrent))
, m_worldBindMatrix (getWorldBindMatrix (dagPath, bBindCurrent))
{
}

NM_Translator::NM_Translator (void)
: _debugMaterial("debugempty")
{
	_maxNameLen = 0;
	_optRotMat = MMatrix::identity;
	_exportSkeleton = true;
	_exportSkeletonMesh = true;
	_exportSkeletonAnim = true;
	_options = 0;
	_skeleton = 0;
}

NM_Translator::~NM_Translator (void)
{
}

void *NM_Translator::creator (void)
{
	return new NM_Translator;
}

void NM_Translator::clear (void)
{
}

MPxFileTranslator::MFileKind NM_Translator::identifyFile (const MFileObject &fileObject, const char *buffer, short size) const
{
	const char *name = fileObject.name().asChar ();
	int nameLength = (int)strlen (name);
	if (nameLength > 3 && !_stricmp (name + nameLength - 3, ".nm"))
	{
		return kIsMyFileType;
	}
	return kNotMyFileType;
}

struct TimeProtector
{
	MTime savedTime;
	TimeProtector (void): savedTime(MAnimControl::currentTime())  
	{
	}

	~TimeProtector (void) 
	{ 
		MAnimControl::setCurrentTime (savedTime); 
	}
};

static void Name2Props (const char *name, PropertyEditor *props)
{
	if (!strncmp(name, "bb_", 3) || strstr(name, "_bb_"))
	{
		props->newProperty ("Billboard", "True");
	}

	if (!strncmp(name, "ta_", 3) || strstr (name, "_ta_"))
	{
		props->newProperty ("BlendMode", "Add");
	}
	else if (!strncmp(name, "tp_", 3) || strstr (name, "_tp_"))
	{
		props->newProperty ("BlendMode", "AlphaAdd");
	}
	else if (!strncmp(name, "tm_", 3) || strstr (name, "_tm_"))
	{
		props->newProperty ("BlendMode", "Modulate");
	}
	else if (!strncmp(name, "tM_", 3) || strstr (name, "_tM_"))
	{
		props->newProperty ("BlendMode", "ModulateB");
	}
	else if (!strncmp(name, "tn_", 3) || strstr (name, "_tn_"))
	{
		props->newProperty ("BlendMode", "InvModulate");
	}
	else if (!strncmp(name, "tN_", 3) || strstr (name, "_tN_"))
	{
		props->newProperty ("BlendMode", "InvModulateB");
	}
	else if (!strncmp(name, "tb_", 3) || strstr (name, "_tb_"))
	{
		props->newProperty ("BlendMode", "InvNormal");
	}

	if (!strncmp(name, "bu_", 3) || strstr (name, "_bu_"))
	{
		props->newProperty ("UVNoIpol", "True");
	}

	if (!strncmp(name, "Mjs_", 4) || strstr (name, "_Mjs_"))
	{
		props->newProperty ("MaterialStyle", "Metal");
	}
	else if (!strncmp(name, "Msl_", 4) || strstr (name, "_Msl_"))
	{
		props->newProperty ("MaterialStyle", "Plastic");
	}
	else if (!strncmp(name, "Mpf_", 4) || strstr (name, "_Mpf_"))
	{
		props->newProperty ("MaterialStyle", "Skin");
	}

	if (!strncmp(name, "t_", 2) || strstr (name, "_t_"))
	{
		props->newProperty ("ObjectHint", "Hair");
	}

	if (!strncmp(name, "MFn_", 4) || strstr (name, "_MFn_"))
	{
		props->newProperty ("MipmapMode", "None");
	}
	else if (!strncmp(name, "MFb_", 4) || strstr (name, "_MFb_"))
	{
		props->newProperty ("MipmapMode", "Bilinear");
	}
	else if (!strncmp(name, "MFt_", 4) || strstr (name, "_MFt_"))
	{
		props->newProperty ("MipmapMode", "Trilinear");
	}
}

MStatus NM_Translator::writer (const MFileObject &file, const MString &optionsString, MPxFileTranslator::FileAccessMode mode)
{
	std::cout << "atom mesh translator version 0.5.0" << std::endl;
	std::cout << "By GYK@server01" << std::endl;

	TimeProtector tp ;

	_outFileName = identifyFileName(file.fullName().asChar ());
	std::string filename = _outFileName + ".lst";
	loadAnimationInfo (_vertexAnimationInfo, filename.c_str());

	_modelprops.clear ();

	MSelectionList selectionList;
	MGlobal::getActiveSelectionList (selectionList);
	createExportLists (selectionList);

	for (MeshList::const_iterator mi = _meshDataList.begin(); mi != _meshDataList.end (); ++mi)
	{
		if (_skeleton && _skeleton->getJoints().size() > 0 && (*mi)->SkinCluster())
		{
			MFnMesh meshFn ((*mi)->DagPath());
			(*mi)->getWeights().resize (meshFn.numVertices());
			(*mi)->getWeightJoints().resize (meshFn.numVertices());

			if (!getVertexJointWeights ((*mi)->DagPath(), (*mi)->SkinCluster(), (*mi)->getWeights(), (*mi)->getWeightJoints(), _skeleton))
			{
				std::cout << "Get vertex weights failed" << std::endl;
				return MS::kFailure;
			}
		}
		_materialviewer.addMaterials ((*mi)->getShaders ());
		_meshviewer.addMesh ((*mi)->getName(), (*mi)->getProp());
		Name2Props ((*mi)->getName(), (*mi)->getProp());
	}

	for (unsigned i = 0; i < _materialviewer.getNumMaterials(); ++i)
	{
		Name2Props (_materialviewer.getMaterialName(i), _materialviewer.getProperty (i));
	}

	if (_skeleton)
	{
		_skeletonviewer.setSkeleton (_skeleton, _meshDataList, _exportOptions.getSelectBoneByName());
	}

	::DialogBoxParam(hModelHandle, MAKEINTRESOURCE(IDD_EXPORTER), GetActiveWindow(), ATOM_mayaexporterOptionsDlgProc, (LPARAM)this);

	_nodeList.clear ();
	_meshList.clear ();

	for (DagDataList::iterator it = _orderedDagList.begin(); it != _orderedDagList.end(); ++it)
	{
		delete *it;
	}
	_orderedDagList.clear ();

	_dagMap.clear ();

	for (MeshList::iterator it = _meshDataList.begin(); it != _meshDataList.end(); ++it)
	{
		delete *it;
	}
	_meshDataList.clear ();

	_skeletonviewer.clear ();
	_meshviewer.clear ();
	_materialviewer.clear ();

	delete _skeleton;
	_skeleton = 0;

	return MS::kSuccess;
}

bool NM_Translator::doExport (void)
{
	if (_exportOptions.getBindPoseSource() == 0)
	{
		MAnimControl::setCurrentTime (MTime(1, MTime::kFilm));
	}

	outputExportData ();

	return true;
}

static MFnSkinCluster *getSkinCluster (const MDagPath &meshDag)
{
	MFnMesh meshNode(meshDag);

	MFnSkinCluster *skinCluster = 0;

	MItDependencyNodes kDepNodeIt(MFn::kSkinClusterFilter);
	for (; !kDepNodeIt.isDone () && !skinCluster; kDepNodeIt.next ())
	{
		MObject obj = kDepNodeIt.item ();
		skinCluster = new MFnSkinCluster(obj);
		unsigned numGeoms = skinCluster->numOutputConnections();
		for (unsigned iGeom = 0; iGeom < numGeoms; ++iGeom)
		{
			unsigned index = skinCluster->indexForOutputConnection(iGeom);
			MObject kOutputObj = skinCluster->outputShapeAtIndex (index);
			if (kOutputObj != meshNode.object())
			{
				delete skinCluster;
				skinCluster = 0;
				break;
			}
		}
	}

	return skinCluster;
}

void NM_Translator::createExportLists (const MSelectionList &masterExportLists)
{
	_skeleton = new ATOM_Skeleton;
	
	if (!_skeleton->load ())
	{
		delete _skeleton;
		_skeleton = 0;
	}

	if (_skeleton && _skeleton->getJoints().size () > 0)
	{
		bool nameOk = true;
		for (unsigned i = 0; i < _skeleton->getJoints().size()-1; ++i)
		{
			if (!nameOk)
			{
				break;
			}

			for (unsigned j = i+1; j < _skeleton->getJoints().size(); ++j)
			{
				if (_skeleton->getJoints()[i].hashCode == _skeleton->getJoints()[j].hashCode)
				{
					char buffer[1024];
					sprintf (buffer, "骨头<%s>和<%s>具有相同的hash值，请修改一下骨头名字", _skeleton->getJoints()[i].name.asChar(), _skeleton->getJoints()[j].name.asChar());
					MessageBox (::GetActiveWindow(), buffer, "atom exporter", MB_OK|MB_ICONHAND);
					nameOk = false;
					break;
				}
			}
		}

		if (!nameOk)
		{
			delete _skeleton;
			_skeleton = 0;
		}
	}

	MDagPath dagPath;
	for (MItSelectionList sIt(masterExportLists); !sIt.isDone (); sIt.next ())
	{
		if (sIt.itemType () == MItSelectionList::kDagSelectionItem)
		{
			if (sIt.getDagPath (dagPath))
			{
				preprocessDag (dagPath, false);
			}
		}
	}

	for (MeshList::const_iterator mi = _meshDataList.begin(); mi != _meshDataList.end (); ++mi)
	{
		MFnSkinCluster *skinCluster = _skeleton ? getSkinCluster ((*mi)->DagPath ()) : 0;
		(*mi)->setSkeletonObj (skinCluster ? _skeleton : 0);
		(*mi)->setSkinCluster (skinCluster);
	}
}

static MString removeNameSpace (const MString &s)
{
	const int index = s.rindex (':');
	if (index >= 0)
	{
		const int l = s.length ();
		if (index + 1 < l)
		{
			return s.substring (index + 1, l - 1);
		}
	}
	return s;
}

unsigned NM_Translator::preprocessDag (const MDagPath &dagPath, bool force)
{
	if (_nodeList.hasItem (dagPath) || _meshList.hasItem (dagPath))
	{
		return 1;
	}

	if (!force && !isNodeVisible (dagPath))
	{
		return 0;
	}

	unsigned retVal = 0;

	if (dagPath.hasFn (MFn::kTransform))
	{
		if (dagPath.hasFn (MFn::kJoint))
		{
			retVal = 1;
			_nodeList.add (dagPath, MObject::kNullObj, true);
			const unsigned nLen = removeNameSpace (MFnDagNode (dagPath).name()).length ();
			if (nLen > _maxNameLen)
			{
				_maxNameLen = nLen;
			}
		}

		unsigned nProcess = 0;
		const unsigned nChildren = dagPath.childCount ();
		for (unsigned ci = 0; ci != nChildren; ++ci)
		{
			MDagPath cDagPath(dagPath);
			cDagPath.push (dagPath.child (ci));
			nProcess += preprocessDag (cDagPath, force);
		}

		if (retVal == 0 && nProcess)
		{
			retVal = 1;
			_nodeList.add (dagPath, MObject::kNullObj, true);
			const unsigned nLen = removeNameSpace (MFnDagNode (dagPath).name()).length ();
			if (nLen > _maxNameLen)
			{
				_maxNameLen = nLen;
			}
		}
	}
	else if (dagPath.hasFn (MFn::kMesh))
	{
		retVal = 1;
		preprocessMesh (dagPath);
	}
	else if (MFnDagNode(dagPath).typeName() == "vstAttachment")
	{
		short tV = 1;
		MFnDagNode (dagPath).findPlug ("type").getValue (tV);
		if (tV == 0 || tV == 2)
		{
			retVal = 1;
		}
		else
		{
			retVal = 0;
		}
	}

	return retVal;
}

void NM_Translator::preprocessMesh (const MDagPath &dagPath)
{
	if (!isNodeVisible (dagPath))
	{
		return;
	}

	MSelectionList historyList;
	getHistory (dagPath, historyList);

	MObject skinObj;
	MObject blendObj;
	MObject latticeObj;
	MObject dObj;
	MObjectArray inputObjs;

	for (MItSelectionList sIt(historyList); !sIt.isDone(); sIt.next ())
	{
		if (sIt.itemType () != MItSelectionList::kDNselectionItem)
		{
			continue;
		}

		if (!sIt.getDependNode (dObj))
		{
			continue;
		}

		if (!dObj.hasFn (MFn::kGeometryFilt))
		{
			continue;
		}

		if (dObj.hasFn (MFn::kSkinClusterFilter))
		{
			skinObj = dObj;
			preprocessSkinCluster (dObj);
		}
		else if (dObj.hasFn (MFn::kBlendShape))
		{
			blendObj = dObj;
		}
		else if (dObj.hasFn (MFn::kFFD))
		{
			latticeObj = dObj;
		}
	}

	addPreprocessedMesh (dagPath, skinObj, blendObj, latticeObj);
}

void NM_Translator::getHistory (const MDagPath &dagPath, MSelectionList &historyList)
{
	historyList.clear ();

	MStringArray history;

	MGlobal::executeCommand ("listHistory -il 2 -pdo 1 \"" + dagPath.fullPathName() + "\"", history);

	const unsigned nHistory = history.length ();
	for (unsigned hi = 0; hi != nHistory; ++hi)
	{
		historyList.add (history [hi]);
	}
}

void NM_Translator::preprocessSkinCluster (const MObject &skinObj)
{
	MFnSkinCluster skinFn (skinObj);
	MDagPathArray dagPathArray;

	MStatus status;
	if (skinFn.influenceObjects (dagPathArray, &status) == 0)
	{
		return;
	}

	if (!status || dagPathArray.length () == 0)
	{
		return;
	}

	const unsigned nDag = dagPathArray.length ();
	for (unsigned di = 0; di != nDag; ++di)
	{
		const MDagPath &dagPath = dagPathArray[di];

		if (dagPath.hasFn (MFn::kShape))
		{
			continue;
		}

		preprocessDag (dagPath, true);
	}
}

unsigned NM_Translator::addPreprocessedMesh (const MDagPath &dagPath, const MObject &skinObj, const MObject &blendObj, const MObject &latticeObj)
{
	assert (!_meshList.hasItem (dagPath));

	MDagPath baseDagPath (dagPath);

	if (latticeObj.isNull ())
	{
		const MObject &dObj = !blendObj.isNull () ? blendObj : skinObj;
		if (!dObj.isNull ())
		{
			MObjectArray inputObjs;
			if (MFnGeometryFilter(dObj).getInputGeometry (inputObjs) && inputObjs.length ())
			{
				MDagPath::getAPathTo (inputObjs[0], baseDagPath);
				if (!(baseDagPath.isValid () && baseDagPath.length ()))
				{
					baseDagPath = dagPath;
				}
			}
			else
			{
				MGlobal::displayWarning (MString ("Cannot find the base geometry for deforming mesh ") + dagPath.partialPathName ());
			}
		}
	}

	_meshList.add (dagPath);
	_meshDataList.push_back (new CMeshData (dagPath, baseDagPath, skinObj, blendObj, false));

	return 1;
}

bool NM_Translator::isNodeVisible( const MDagPath &dagPath, bool bTemplateAsInvisible) const
{
	const MFnDagNode dagFn (dagPath);

	if (dagFn.isIntermediateObject ())
	{
		return false;
	}

	bool visibleVal = false;
	dagFn.findPlug ("visibility").getValue (visibleVal);
	if (!visibleVal)
	{
		return false;
	}

	if (bTemplateAsInvisible)
	{
		bool templateVal = false;
		dagFn.findPlug ("template").getValue(templateVal);
		if (templateVal)
		{
			return false;
		}
	}

	bool overrideVal = false;
	dagFn.findPlug ("overrideEnabled").getValue (overrideVal);
	if (overrideVal)
	{
		dagFn.findPlug ("overrideVisibility").getValue (visibleVal);
		if (!visibleVal)
		{
			return false;
		}

		if (bTemplateAsInvisible)
		{
			int displayTypeVal = 0;
			dagFn.findPlug ("overrideDisplayType").getValue (displayTypeVal);
			if (displayTypeVal == 1)
			{
				return false;
			}
		}
	}

	return true;
}

const CDagData *NM_Translator::getParent (const MDagPath &dagPath) const
{
	MDagPath dp(dagPath);
	dp.pop ();
	const DagMap::const_iterator pi = _dagMap.find (dp.partialPathName().asChar ());
	if (pi != _dagMap.end ())
	{
		return dynamic_cast<const CDagData*>(pi->second);
	}
	return 0;
}

int NM_Translator::getParentIndex (const MDagPath &dagPath) const
{
	const CDagData *dagData = getParent (dagPath);
	if (dagData)
	{
		return dagData->Index ();
	}
	return -1;
}

void NM_Translator::getFaceSets (const MDagPath &dagPath, const MFnMesh &meshFn, const MObjectArray &shaders, const MIntArray &faceShaderIndices, std::vector<MFnSingleIndexedComponent*> &faceSetFns)
{
	//shaders.clear ();

	{
		const std::vector<MFnSingleIndexedComponent*>::const_iterator fsEnd (faceSetFns.end ());
		std::vector<MFnSingleIndexedComponent*>::const_iterator fsi;
		for (fsi = faceSetFns.begin (); fsi != fsEnd; ++fsi)
		{
			delete (*fsi);
		}
	}

	faceSetFns.clear ();

	{
		const unsigned shaderEnd = shaders.length ();
		for (unsigned shaderIndex = 0; shaderIndex < shaderEnd; ++shaderIndex)
		{
			faceSetFns.push_back (new MFnSingleIndexedComponent);
			faceSetFns.back ()->create (MFn::kMeshPolygonComponent);
		}
	}

	{
		const unsigned faceEnd = faceShaderIndices.length ();
		assert (faceEnd == static_cast<unsigned>(meshFn.numPolygons()));
		for (unsigned faceIndex = 0; faceIndex != faceEnd; ++faceIndex)
		{
			unsigned idx = faceShaderIndices[faceIndex];
			if (idx < faceSetFns.size ())
			{
				faceSetFns[faceShaderIndices[faceIndex]]->addElement (faceIndex);
			}
		}
	}
}

bool NM_Translator::outputMesh (const MFnMesh &meshFn, const MDagPath &dagPath, std::vector<MyMesh> &meshes, std::vector<std::vector<unsigned> > &verticesIndices, std::vector<std::vector<unsigned> > &uvIndices, const MObjectArray &shaders, const MIntArray &indices)
{
	bool ret = true;

	//MObjectArray shaders;
	std::vector<MFnSingleIndexedComponent*> faceSetFns;

	getFaceSets (dagPath, meshFn, shaders, indices, faceSetFns);

	std::string targaName = _debugMaterial;
	std::string targaPath;
	const size_t nFaceSets = faceSetFns.size ();
	std::vector<CMaterial> materials(nFaceSets);

	MDagPathArray dagPathArray;
	std::vector<unsigned> validInfluenceIndices;
	std::vector<int> influenceNodeIndices;

	if (0 /*!skinObj.isNull ()*/)
	{
		// TODO
	}
	else
	{
		meshes.resize (nFaceSets);
		verticesIndices.resize (nFaceSets);
		uvIndices.resize (nFaceSets);

		for (unsigned fi = 0; fi < nFaceSets; ++fi)
		{
			const MFnSingleIndexedComponent &faceSetFn = *faceSetFns[fi];
			if (faceSetFn.isEmpty ())
			{
				continue;
			}

			CMaterial &m = materials[fi];
			material &mat = meshes[fi].getMaterial ();

			if (!getMaterial (shaders[fi], &mat))
			{
				ret = false;
				break;
			}

			m.transparency = mat.transparency;
			if (!getUVCoords (shaders[fi], m))
			{
				ret = false;
				break;
			}

			mat.doubleSide = _exportOptions.doubleSide();
			mat.transparency = m.transparency;

			if (!m.filenameColor.empty () && !m.uvCoordColor.isNull() && !m.uvFileColor.isNull())
			{
				mat.diffuseChannel1 = 0;
				mat.diffuseMap1 = m.filenameColor;
				mat.diffuseColor.set(1.f, 1.f, 1.f);
			}
			else
			{
				mat.diffuseChannel1 = -1;
				mat.diffuseMap1 = "";
			}

			if (!m.filenameTrans.empty ())
			{
				mat.diffuseChannel2 = 0;
				mat.diffuseMap2 = m.filenameTrans;
				mat.diffuseColor.set(1.f, 1.f, 1.f);
			}
			else
			{
				mat.diffuseChannel2 = -1;
				mat.diffuseMap2 = "";
			}

			if (!m.filenameBump.empty ())
			{
				mat.normalmapChannel = 0;
				mat.normalMap = m.filenameBump;
				mat.diffuseColor.set(1.f, 1.f, 1.f);
			}
			else
			{
				mat.normalmapChannel = -1;
				mat.normalMap = "";
			}

			MObject faceSetObj (faceSetFn.object ());
			MFloatPointArray meshVertices;
			MFloatVectorArray meshNormals;
			MFloatVectorArray meshTangents;
			meshFn.getPoints (meshVertices, MSpace::kWorld);
			meshFn.getNormals (meshNormals, MSpace::kWorld);
			meshFn.getTangents (meshTangents, MSpace::kWorld);
			MFloatArray u, v;
			meshFn.getUVs (u, v);

			if (!outputFaceSet (meshes[fi], meshFn, m, faceSetObj, verticesIndices[fi], uvIndices[fi], meshVertices, meshNormals, meshTangents, u, v))
			{
				ret = false;
				break;
			}
		}
	}

	{
		const std::vector<MFnSingleIndexedComponent*>::const_iterator fsEnd = faceSetFns.end ();
		std::vector<MFnSingleIndexedComponent*>::const_iterator fsi;
		for (fsi = faceSetFns.begin (); fsi != fsEnd; ++fsi)
		{
			delete (*fsi);
		}
	}

	if (!ret)
	{
		std::cout << "Output mesh failed: " << dagPath.partialPathName ().asChar() << std::endl;
	}

	return ret;
}

bool NM_Translator::outputMeshW (const MFnMesh &meshFn, const MDagPath &dagPath, std::vector<MyMesh> &meshes, std::vector<std::vector<unsigned> > &verticesIndices, std::vector<std::vector<unsigned> > &uvIndices, const std::vector<MFloatArray> &weights, const std::vector<MIntArray> &weightjoints, const MObjectArray &shaders, const MIntArray &indices)
{
	bool ret = true;

	//MObjectArray shaders;
	std::vector<MFnSingleIndexedComponent*> faceSetFns;

	getFaceSets (dagPath, meshFn, shaders, indices, faceSetFns);

	std::string targaName = _debugMaterial;
	std::string targaPath;
	const size_t nFaceSets = faceSetFns.size ();
	std::vector<CMaterial> materials(nFaceSets);

	MDagPathArray dagPathArray;
	std::vector<unsigned> validInfluenceIndices;
	std::vector<int> influenceNodeIndices;

	if (0 /*!skinObj.isNull ()*/)
	{
		// TODO
	}
	else
	{
		meshes.resize (nFaceSets);
		verticesIndices.resize (nFaceSets);
		uvIndices.resize (nFaceSets);

		for (unsigned fi = 0; fi != nFaceSets; ++fi)
		{
			const MFnSingleIndexedComponent &faceSetFn = *faceSetFns[fi];
			if (faceSetFn.isEmpty ())
			{
				continue;
			}

			CMaterial &m = materials[fi];
			material &mat = meshes[fi].getMaterial ();

			if (!getUVCoords (shaders[fi], materials[fi]))
			{
				ret = false;
				break;
			}

			if (!getMaterial (shaders[fi], &mat))
			{
				ret = false;
				break;
			}

			if (mat.transparency == 1.f && m.transparency == 0.f)
			{
				mat.transparency = 0.f;
			}

			mat.doubleSide = _exportOptions.doubleSide();

			if (!m.filenameColor.empty () && !m.uvCoordColor.isNull() && !m.uvFileColor.isNull())
			{
				mat.diffuseChannel1 = 0;
				mat.diffuseMap1 = m.filenameColor;
				mat.diffuseColor.set(1.f, 1.f, 1.f);
			}
			else
			{
				mat.diffuseChannel1 = -1;
				mat.diffuseMap1 = "";
			}

			if (!m.filenameTrans.empty ())
			{
				mat.diffuseChannel2 = 0;
				mat.diffuseMap2 = m.filenameTrans;
				mat.diffuseColor.set(1.f, 1.f, 1.f);
			}
			else
			{
				mat.diffuseChannel2 = -1;
				mat.diffuseMap2 = "";
			}

			if (!m.filenameBump.empty ())
			{
				mat.normalmapChannel = 0;
				mat.normalMap = m.filenameBump;
				mat.diffuseColor.set(1.f, 1.f, 1.f);
			}
			else
			{
				mat.normalmapChannel = -1;
				mat.normalMap = "";
			}

			MObject faceSetObj (faceSetFn.object ());
			MFloatPointArray meshVertices;
			meshFn.getPoints (meshVertices, MSpace::kWorld);
			MFloatVectorArray meshNormals;
			meshFn.getNormals (meshNormals, MSpace::kWorld);
			MFloatVectorArray meshTangents;
			meshFn.getTangents (meshTangents, MSpace::kWorld);
			MFloatArray u, v;
			meshFn.getUVs (u, v);

			if (!outputFaceSetW (meshes[fi], meshFn, m, faceSetObj, verticesIndices[fi], uvIndices[fi], meshVertices, meshNormals, meshTangents, weights, weightjoints, u, v))
			{
				ret = false;
				break;
			}
		}
	}

	{
		const std::vector<MFnSingleIndexedComponent*>::const_iterator fsEnd = faceSetFns.end ();
		std::vector<MFnSingleIndexedComponent*>::const_iterator fsi;
		for (fsi = faceSetFns.begin (); fsi != fsEnd; ++fsi)
		{
			delete (*fsi);
		}
	}

	if (!ret)
	{
		std::cout << "Output mesh failed: " << dagPath.partialPathName ().asChar() << std::endl;
	}

	return ret;
}

MObject NM_Translator::findInputNode (const MObject &dstObj, const MString &dstPlugName)
{
	const MFnDependencyNode dstFn (dstObj);
	const MPlug dstPlug (dstFn.findPlug (dstPlugName));
	if (dstPlug.isNull ())
	{
		return MObject::kNullObj;
	}
	MPlugArray plugArray;
	if (!(dstPlug.connectedTo (plugArray, true, false) && plugArray.length ()))
	{
		return MObject::kNullObj;
	}

	return plugArray[0].node ();
}

MObject NM_Translator::findInputNodeOfType (const MObject &dstObj, const MString &dstPlugName, const MString &typeName)
{
	const MFnDependencyNode dstFn(dstObj);
	const MPlug dstPlug(dstFn.findPlug (dstPlugName));

	if (dstPlug.isNull ())
	{
		return MObject::kNullObj;
	}

	MPlugArray plugArray;
	if (!(dstPlug.connectedTo (plugArray, true, false) && plugArray.length ()))
	{
		return MObject::kNullObj;
	}

	MPlug srcPlug(plugArray[0]);
	MObject srcObj (srcPlug.node ());
	MFnDependencyNode srcFn (srcObj);
	if (srcFn.typeName () == typeName)
	{
		return srcObj;
	}

	return findInputNodeOfType (srcObj, dstPlugName, typeName);
}

bool NM_Translator::getMaterial (const MObject &shadingGroupObj, material *m)
{
	MPlug mShader = MFnDependencyNode (shadingGroupObj).findPlug ("surfaceShader");

	bool initialized = true;
	if (mShader.isNull ())
	{
		initialized = false;
	}
	else
	{
		MFnDependencyNode shaderNode;
		MPlugArray plugs;
		shaderNode.setObject (mShader);

		mShader.connectedTo (plugs, true, false);
		if (plugs.length() == 0)
		{
			initialized = false;
		}
		else
		{
			switch (plugs[0].node().apiType())
			{
			//case MFn::kAnisotropy:
			//	{
			//		MFnAnisotropyShader nShader(plugs[0].node());
			//		MColor diffuse = nShader.tangentUCamera();
			//		MColor trans = nShader.tangentVCamera();
			//		m->ambientColor.set (0.f.r, ambient.g, ambient.b);
			//		m->diffuseColor.set (diffuse.r, diffuse.g, diffuse.b);
			//		m->specularColor.set (0.f, 0.f, 0.f);
			//		m->transparency = 1.f - nShader.transparency ().r;
			//		m->shininess = 0;
			//		float t = nShader.translucenceCoeff();
			//		if (t > 0.f)
			//		{
			//			m->alphaTest = true;
			//			m->alpharef = t;
			//		}
			//		else
			//		{
			//			m->alphaTest = false;
			//			m->alpharef = 0.f;
			//		}
			//		break;
			//	}
			case MFn::kLambert:
				{
					MFnLambertShader nShader(plugs[0].node());
					m->name = nShader.name().asChar();
					float coeff = nShader.diffuseCoeff ();
					MColor diffuse = nShader.color() * coeff;
					MColor ambient = nShader.ambientColor ();
					MColor emissive = nShader.incandescence ();
					m->ambientColor.set (ambient.r, ambient.g, ambient.b);
					m->diffuseColor.set (diffuse.r, diffuse.g, diffuse.b);
					m->emissiveColor.set (emissive.r, emissive.g, emissive.b);
					m->transparency = 1.f - nShader.transparency ().r;
					MFnDependencyNode node(plugs[0].node());
					MPlug alphaGain = node.findPlug ("materialAlphaGain");
					if (!alphaGain.isNull ())
					{
						float f;
						alphaGain.getValue (f);
						m->transparency *= f;
					}
					m->shininess = 0;
					float t = nShader.translucenceCoeff();
					if (t > 0.f)
					{
						m->alphaTest = true;
						m->alpharef = t;
					}
					else
					{
						m->alphaTest = false;
						m->alpharef = 0.f;
					}
					break;
				}
			case MFn::kBlinn:
				{
					MFnBlinnShader nShader(plugs[0].node());
					m->name = nShader.name().asChar();
					float coeff = nShader.diffuseCoeff ();
					MColor diffuse = nShader.color() * coeff;
					MColor ambient = nShader.ambientColor ();
					MColor emissive = nShader.incandescence ();
					m->ambientColor.set (ambient.r, ambient.g, ambient.b);
					m->diffuseColor.set (diffuse.r, diffuse.g, diffuse.b);
					m->emissiveColor.set (emissive.r, emissive.g, emissive.b);
					m->transparency = 1.f - nShader.transparency ().r;
					MFnDependencyNode node(plugs[0].node());
					MPlug alphaGain = node.findPlug ("materialAlphaGain");
					if (!alphaGain.isNull ())
					{
						float f;
						alphaGain.getValue (f);
						m->transparency *= f;
					}
					m->shininess = nShader.eccentricity () < 0.03125f ? 128.f : 4.f / nShader.eccentricity ();
					float t = nShader.translucenceCoeff();
					t = nShader.translucenceCoeff ();
					if (t > 0.f)
					{
						m->alphaTest = true;
						m->alpharef = t;
					}
					else
					{
						m->alphaTest = false;
						m->alpharef = 0.f;
					}
					break;
				}
			case MFn::kPhong:
				{
					MFnPhongShader nShader(plugs[0].node ());
					m->name = nShader.name().asChar();
					float coeff = nShader.diffuseCoeff ();
					MColor diffuse = nShader.color() * coeff;
					MColor ambient = nShader.ambientColor();
					MColor emissive = nShader.incandescence();
					m->ambientColor.set (ambient.r, ambient.g, ambient.b);
					m->diffuseColor.set (diffuse.r, diffuse.g, diffuse.b);
					m->emissiveColor.set (emissive.r, emissive.g, emissive.b);
					m->transparency = 1.f - nShader.transparency ().r;
					MFnDependencyNode node(plugs[0].node());
					MPlug alphaGain = node.findPlug ("materialAlphaGain");
					if (!alphaGain.isNull ())
					{
						float f;
						alphaGain.getValue (f);
						m->transparency *= f;
					}
					m->shininess = nShader.cosPower () * 4.f;
					float t = nShader.translucenceCoeff();
					if (t > 0.f)
					{
						m->alphaTest = true;
						m->alpharef = t;
					}
					else
					{
						m->alphaTest = false;
						m->alpharef = 0.f;
					}
					break;
				}
			default:
				{
					::MessageBox (::GetActiveWindow(), "发现不支持的材质!\n\n目前只支持Lambert|Blinn|Phong三种材质!", "atom exporter", MB_OK|MB_ICONHAND);
					return false;

					//MFnDependencyNode nNode(plugs[0].node());
					//MPlug diffuseP = nNode.findPlug ("outColor");
					//if (!diffuseP.isNull() && diffuseP.numElements() >= 3)
					//{
					//	diffuseP[0].getValue (m->diffuseColor.x);
					//	diffuseP[1].getValue (m->diffuseColor.y);
					//	diffuseP[2].getValue (m->diffuseColor.z);
					//}
					//m->specularColor.set (0.f, 0.f, 0.f);

					//MPlug transparencyP = nNode.findPlug ("outTransparency");
					//if (!transparencyP.isNull() && transparencyP.numElements() >= 3)
					//{
					//	transparencyP[0].getValue (m->transparency);
					//	m->transparency = 1.f - m->transparency;
					//}
					//else
					//{
					//	m->transparency = 0.f;
					//}

					//m->ambientColor.set (0.f, 0.f, 0.f);
					//m->shininess = 0.f;
					break;
				}
			}

			//MPlug color = MFnDependencyNode (plugs[0].node()).findPlug ("color");
			//if (!color.isNull ())
			//{
			//	MItDependencyGraph it(color, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel);
			//	it.disablePruningOnFilter ();
			//	if (!it.isDone ())
			//	{
			//		MString textureFileName;
			//		MFnDependencyNode (it.thisNode()).findPlug ("fileTextureName").getValue (textureFileName);
			//		std::string fileName = identifyFileName (textureFileName.asChar ());
			//		std::string vfsFileName, physicFileName;
			//		if (!translateFileName (fileName.c_str(), _outFileName, vfsFileName, physicFileName, _share))
			//		{
			//			return false;
			//		}
			//		if (stricmp (fileName.c_str(), physicFileName.c_str()))
			//		{
			//			if (!::CopyFileA (fileName.c_str(), physicFileName.c_str(), FALSE))
			//			{
			//				return false;
			//			}
			//		}
			//		m->diffuseMap1 = vfsFileName;
			//	}

			//	MObject tmpFileObj = findInputNodeOfType (surfaceShaderObj, "color", "file");
			//	if (tmpFileObj.isNull ())
			//	{
			//		return _debugMaterial;
			//	}

			//	MObject tmpPlace2dTextureObj (findInputNodeOfType (fileObj, "uvCoord", "place2dTexture"));
			//	if (!tmpPlace2dTextureObj.isNull ())
			//	{
			//		MFnDependencyNode place2dTextureFn (tmpPlace2dTextureObj);
			//		MPlug uvCoordP (place2dTextureFn.findPlug ("uvCoord"));
			//		if (!(uvCoordP.isNull () || uvCoordP.isConnected () || uvCoordP.isLocked ()))
			//		{
			//			uvCoord_d0 = tmpPlace2dTextureObj;
			//		}
			//	}
			//}
			//else
			//{
			//	m->diffuseMap1 = "";
			//}

			//MPlug bumpmap = MFnDependencyNode (plugs[0].node()).findPlug ("normalCamera");
			//if (!bumpmap.isNull ())
			//{
			//	MItDependencyGraph it(bumpmap, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel);
			//	it.disablePruningOnFilter ();
			//	if (!it.isDone ())
			//	{
			//		MString textureFileName;
			//		MFnDependencyNode (it.thisNode()).findPlug ("fileTextureName").getValue (textureFileName);
			//		std::string fileName = identifyFileName (textureFileName.asChar());
			//		std::string vfsFileName, physicFileName;
			//		if (!translateFileName (fileName.c_str(), _outFileName, vfsFileName, physicFileName, _share))
			//		{
			//			return false;
			//		}
			//		if (stricmp (fileName.c_str(), physicFileName.c_str()))
			//		{
			//			if (!::CopyFileA (fileName.c_str(), physicFileName.c_str(), FALSE))
			//			{
			//				return false;
			//			}
			//		}
			//		m->normalMap = vfsFileName;
			//	}
			//}
			//else
			//{
			//	m->normalMap = "";
			//}
		}
	}

	if (!initialized)
	{
		m->name = "(noname)";
		m->alphaTest = false;
		m->alpharef = 0.f;
		m->ambientColor.set (0.2f, 0.2f, 0.2f);
		m->diffuseChannel1 = -1;
		m->diffuseChannel2 = -1;
		m->diffuseColor.set (1.f, 1.f, 1.f);
		m->diffuseMap1 = "";
		m->diffuseMap2 = "";
		m->normalMap = "";
		m->doubleSide = _exportOptions.doubleSide();
		m->normalmapChannel = -1;
		m->shininess = 0;
		m->emissiveColor.set (0.f, 0.f, 0.f);
		m->transparency = 1.f;
	}

	return true;
}

bool NM_Translator::getUVCoords (const MObject &shadingGroupObj, CMaterial &m)
{
	m.filenameColor = "";
	m.filenameTrans = "";
	m.filenameBump = "";
	m.uvCoordColor = MObject::kNullObj;
	m.uvFileColor = MObject::kNullObj;
	m.alphaTest = false;


	MObject surfaceShaderObj (findInputNode (shadingGroupObj, "surfaceShader"));
	if (surfaceShaderObj.isNull ())
	{
		std::cout << "Can not find surfaceShader." << std::endl;
		return false;
	}

#if 1
	const MFnDependencyNode dstFn (shadingGroupObj);
	MPlug mShader = MFnDependencyNode (shadingGroupObj).findPlug ("surfaceShader");

	if (!mShader.isNull ())
	{
		MFnDependencyNode shaderNode;
		MPlugArray plugs;
		shaderNode.setObject (mShader);

		mShader.connectedTo (plugs, true, false);
		if (plugs.length() != 0)
		{
			MPlug color = MFnDependencyNode (plugs[0].node()).findPlug ("color");
			if (!color.isNull ())
			{
				MItDependencyGraph it(color, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel);
				it.disablePruningOnFilter ();
				if (!it.isDone ())
				{
					MString textureFileName;
					MFnDependencyNode (it.thisNode()).findPlug ("fileTextureName").getValue (textureFileName);

					std::string fileName = identifyFileName (textureFileName.asChar ());
					std::string vfsFileName, physicFileName;
					if (!translateFileName (fileName.c_str(), _outFileName, vfsFileName, physicFileName, _shareDiffuseMap))
					{
						std::cout << "Translate file name failed: " << fileName << std::endl;
						return false;
					}

					if (_exportOptions.exportMeshes() && _stricmp (fileName.c_str(), physicFileName.c_str()))
					{
						if (!::CopyFileA (fileName.c_str(), physicFileName.c_str(), FALSE))
						{
							std::cout << "Copy file " << fileName << " to " << physicFileName << " failed." << std::endl;
							return false;
						}
					}
					m.filenameColor = vfsFileName;
					m.uvFileColor = it.thisNode ();
					m.uvCoordColor = findInputNodeOfType (it.thisNode(), "uvCoord", "place2dTexture");

				}
			}

			MPlug trans = MFnDependencyNode (plugs[0].node()).findPlug ("transparency");
			if (!trans.isNull ())
			{
				MItDependencyGraph it(trans, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel);
				it.disablePruningOnFilter ();
				if (!it.isDone ())
				{
					MString textureFileName;
					MFnDependencyNode (it.thisNode()).findPlug ("fileTextureName").getValue (textureFileName);
					if (m.transparency == 1.f)
					{
						m.transparency = 0.f;
					}

					if (it.thisNode() != m.uvFileColor)
					{
						std::string fileName = identifyFileName (textureFileName.asChar());
						std::string vfsFileName, physicFileName;
						if (!translateFileName (fileName.c_str(), _outFileName, vfsFileName, physicFileName, _shareDiffuseMap))
						{
							std::cout << "Translate file name failed: " << fileName << std::endl;
							return false;
						}
						if (_exportOptions.exportMeshes() && _stricmp (fileName.c_str(), physicFileName.c_str()))
						{
							if (!::CopyFileA (fileName.c_str(), physicFileName.c_str(), FALSE))
							{
								std::cout << "Copy file " << fileName << " to " << physicFileName << " failed." << std::endl;
								return false;
							}
						}
						m.filenameTrans = vfsFileName;
					}
				}
			}

			MPlug bumpmap = MFnDependencyNode (plugs[0].node()).findPlug ("normalCamera");
			if (!bumpmap.isNull ())
			{
				MItDependencyGraph it(bumpmap, MFn::kFileTexture, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel);
				it.disablePruningOnFilter ();
				if (!it.isDone ())
				{
					MString textureFileName;
					MFnDependencyNode (it.thisNode()).findPlug ("fileTextureName").getValue (textureFileName);
					std::string fileName = identifyFileName (textureFileName.asChar());
					std::string vfsFileName, physicFileName;
					if (!translateFileName (fileName.c_str(), _outFileName, vfsFileName, physicFileName, _shareNormalMap))
					{
						std::cout << "Translate file name failed: " << fileName << std::endl;
						return false;
					}
					if (_exportOptions.exportMeshes() && _stricmp (fileName.c_str(), physicFileName.c_str()))
					{
						if (!::CopyFileA (fileName.c_str(), physicFileName.c_str(), FALSE))
						{
							std::cout << "Copy file " << fileName << " to " << physicFileName << " failed." << std::endl;
							return false;
						}
					}
					m.filenameBump = vfsFileName;
				}
			}
		}
	}
#else

	MObject colorFileObj = findInputNodeOfType (surfaceShaderObj, "color", "file");
	if (!colorFileObj.isNull ())
	{
		MObject tmpPlace2dTextureObj (findInputNodeOfType (colorFileObj, "uvCoord", "place2dTexture"));
		if (!tmpPlace2dTextureObj.isNull ())
		{
			MFnDependencyNode place2dTextureFn (tmpPlace2dTextureObj);
			MPlug uvCoordP (place2dTextureFn.findPlug ("uvCoord"));
			if (!(uvCoordP.isNull () || uvCoordP.isConnected () || uvCoordP.isLocked ()))
			{
				m.uvCoordColor = tmpPlace2dTextureObj;
			}

			MString fileTexture;
			const MFnDependencyNode fileFn (colorFileObj);
			const MPlug fileP (fileFn.findPlug ("fileTextureName"));
			if (!fileP.isNull ())
			{
				fileP.getValue (fileTexture);
				m.filenameColor = fileTexture.asChar ();
			}

			m.uvFileColor = colorFileObj;
		}
	}

	MObject transFileObj (findInputNodeOfType (surfaceShaderObj, "transparency", "file"));
	if (!transFileObj.isNull ())
	{
		if (transFileObj == colorFileObj)
		{
			m.alphaTest = true;
		}
		else
		{
			MString fileTexture;
			const MFnDependencyNode fileFn (transFileObj);
			const MPlug fileP (fileFn.findPlug ("fileTextureName"));
			if (!fileP.isNull ())
			{
				fileP.getValue (fileTexture);
				m.filenameTrans = fileTexture.asChar ();
			}
			m.alphaTest = false;
		}
	}

	MObject bumpFileObj (findInputNodeOfType (surfaceShaderObj, "normalCamera", "file"));
	if (!bumpFileObj.isNull ())
	{
		MString fileTexture;
		const MFnDependencyNode fileFn (bumpFileObj);
		const MPlug fileP (fileFn.findPlug ("fileTextureName"));
		if (!fileP.isNull ())
		{
			fileP.getValue (fileTexture);
			m.filenameBump = fileTexture.asChar ();
		}
	}
#endif

	return true;
}

bool NM_Translator::outputFaceSet (MyMesh &mesh, const MFnMesh &meshFn, const CMaterial &material, MObject &faceSetObj, std::vector<unsigned> &verticesIndices, std::vector<unsigned> &uvIndices, const MFloatPointArray &meshVertices, const MFloatVectorArray &meshNormals, const MFloatVectorArray &meshTangents, const MFloatArray &u, const MFloatArray &v)
//void NM_Translator::outputFaceSet (MyMesh &mesh, const MFnMesh &meshFn, const MMatrix &xformMat, bool doubleSided, const MObject &fileObj, const MObject &place2dTextureObj, bool transparent, const CMeshData *meshData, int parentIndex, MObject &faceSetObj)
{
	MIntArray meshVertexIndices;
	MPointArray tpList;
	MIntArray tvList;

	for (MItMeshPolygon bpIt(meshFn.dagPath(), faceSetObj); !bpIt.isDone (); bpIt.next ())
	{
		if (!bpIt.hasValidTriangulation ())
		{
			continue;
		}

		bpIt.getVertices (meshVertexIndices);
		const unsigned nVerts = meshVertexIndices.length ();

		int nTri = 0;
		bpIt.numTriangles (nTri);

		std::set<unsigned> usedIndices;

		for (int ti = 0; ti < nTri; ++ti)
		{
			bpIt.getTriangle (ti, tpList, tvList);

			MIntArray triVertexIdx;
			for (unsigned iObj = 0; iObj < tvList.length(); ++iObj)
			{
				for (unsigned iPoly = 0; iPoly < meshVertexIndices.length(); ++iPoly)
				{
					if (tvList[iObj]==meshVertexIndices[iPoly])
					{
						triVertexIdx.append (iPoly);
						break;
					}
				}
			}

			int idx[3] = { 0, 2, 1 };
			for (unsigned i = 0; i < 3; ++i)
			{
				int vi = idx[i];

				MStatus state;
				unsigned vertexIdx = bpIt.vertexIndex (triVertexIdx[vi], &state);
				if (state != MS::kSuccess)
				{
					::MessageBox (::GetActiveWindow(), "获取顶点索引失败！", "atom exporter", MB_OK|MB_ICONHAND);
					return false;
				}
				unsigned normalIdx = bpIt.normalIndex (triVertexIdx[vi], &state);
				if (state != MS::kSuccess)
				{
					::MessageBox (::GetActiveWindow(), "获取法线索引失败！", "atom exporter", MB_OK|MB_ICONHAND);
					return false;
				}
				unsigned tangentIdx = bpIt.tangentIndex (triVertexIdx[vi], &state);
				if (state != MS::kSuccess)
				{
					::MessageBox (::GetActiveWindow(), "获取法线索引失败！", "atom exporter", MB_OK|MB_ICONHAND);
					return false;
				}

				MyMesh::Vertex vtx;
				MPoint pt = meshVertices[vertexIdx];
				pt.cartesianize ();
				vtx.position.set (pt.z, pt.y, pt.x);
				MVector vec = meshNormals[normalIdx];
				vtx.normal.set (vec.z, vec.y, vec.x);

				if (tangentIdx < meshTangents.length())
				{
					MVector tan = meshTangents[tangentIdx];
					vtx.tangent.set (tan.z, tan.y, tan.x);
				}
				else
				{
					vtx.tangent.set (0.f, 0.f, 0.f);
				}

				if (_exportOptions.exportVertexColor())
				{
					MColor color;

					if (bpIt.hasColor (triVertexIdx[vi]))
					{
						state = bpIt.getColor (color, triVertexIdx[vi]);
						if (state != MS::kSuccess)
						{
							color = MColor(mesh.getMaterial().diffuseColor.x, mesh.getMaterial().diffuseColor.y, mesh.getMaterial().diffuseColor.z, 1.f);
						}
						else
						{
							if (color.r > 1) color.r = 1; else if (color.r < 0) color.r = 0;
							if (color.g > 1) color.g = 1; else if (color.g < 0) color.g = 0;
							if (color.b > 1) color.b = 1; else if (color.b < 0) color.b = 0;

							if (_exportOptions.exportVertexTransparency())
							{
								if (color.a > 1) color.a = 1; else if (color.a < 0) color.a = 0;
								if (color.a < 1.f)
								{
									mesh.getMaterial().transparency = 0.f;
								}
							}
							else
							{
								color.a = 1;
							}
						}
					}
					else
					{
						color = MColor(mesh.getMaterial().diffuseColor.x, mesh.getMaterial().diffuseColor.y, mesh.getMaterial().diffuseColor.z, 1.f);
					}

					vtx.color = ATOM_ColorARGB(color.r, color.g, color.b, color.a);
				}

				int uvIdx;
				MStatus stat = bpIt.getUVIndex (triVertexIdx[vi], uvIdx);
				if (stat == MS::kSuccess)
				{
					vtx.texcoords.set (u[uvIdx], 1.f-v[uvIdx]);
				}
				else
				{
					vtx.texcoords.set (0.f, 0.f);
					//::MessageBox (::GetActiveWindow(), "获取贴图坐标索引失败！", "atom exporter", MB_OK|MB_ICONHAND);
					//return false;
				}

				mesh.pushSuperVertex (vtx);
				verticesIndices.push_back (vertexIdx);
				uvIndices.push_back (uvIdx);
			}
		}
	}

	return true;
}

bool NM_Translator::outputFaceSetW (MyMesh &mesh, 
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
		const MFloatArray &u,
		const MFloatArray &v
		)
{
	MIntArray meshVertexIndices;
	MPointArray tpList;
	MIntArray tvList;

	bool outputWeightProblem = true;
	bool outputWeightProblem2 = true;

	for (MItMeshPolygon bpIt(meshFn.dagPath(), faceSetObj); !bpIt.isDone (); bpIt.next ())
	{
		if (!bpIt.hasValidTriangulation ())
		{
			continue;
		}

		bpIt.getVertices (meshVertexIndices);
		const unsigned nVerts = meshVertexIndices.length ();

		int nTri = 0;
		bpIt.numTriangles (nTri);

		std::set<unsigned> usedIndices;

		for (int ti = 0; ti < nTri; ++ti)
		{
			bpIt.getTriangle (ti, tpList, tvList);

			MIntArray triVertexIdx;
			for (unsigned iObj = 0; iObj < tvList.length(); ++iObj)
			{
				for (unsigned iPoly = 0; iPoly < meshVertexIndices.length(); ++iPoly)
				{
					if (tvList[iObj]==meshVertexIndices[iPoly])
					{
						triVertexIdx.append (iPoly);
						break;
					}
				}
			}

			int idx[3] = { 0, 2, 1 };
			for (unsigned i = 0; i < 3; ++i)
			{
				int vi = idx[i];

				MStatus state;
				unsigned vertexIdx = bpIt.vertexIndex (triVertexIdx[vi], &state);
				if (state != MS::kSuccess)
				{
					::MessageBox (::GetActiveWindow(), "获取顶点索引失败！", "atom exporter", MB_OK|MB_ICONHAND);
					return false;
				}
				unsigned normalIdx = bpIt.normalIndex (triVertexIdx[vi], &state);
				if (state != MS::kSuccess)
				{
					::MessageBox (::GetActiveWindow(), "获取法线索引失败！", "atom exporter", MB_OK|MB_ICONHAND);
					return false;
				}
				unsigned tangentIdx = bpIt.tangentIndex (triVertexIdx[vi], &state);
				if (state != MS::kSuccess)
				{
					::MessageBox (::GetActiveWindow(), "获取切线索引失败！", "atom exporter", MB_OK|MB_ICONHAND);
					return false;
				}

				MyMesh::Vertex vtx;
				MPoint pt = meshVertices[vertexIdx];
				pt.cartesianize ();
				vtx.position.set (pt.z, pt.y, pt.x);
				MVector vec = meshNormals[normalIdx];
				vtx.normal.set (vec.z, vec.y, vec.x);

				if (tangentIdx < meshTangents.length())
				{
					MVector tan = meshTangents[tangentIdx];
					vtx.tangent.set (tan.z, tan.y, tan.x);
				}
				else
				{
					vtx.tangent.set (0.f, 0.f, 0.f);
				}

				if (_exportOptions.exportVertexColor())
				{
					MColor color;

					if (bpIt.hasColor (triVertexIdx[vi]))
					{
						state = bpIt.getColor (color, triVertexIdx[vi]);
						if (state != MS::kSuccess)
						{
							color = MColor(mesh.getMaterial().diffuseColor.x, mesh.getMaterial().diffuseColor.y, mesh.getMaterial().diffuseColor.z, 1.f);
						}
						else
						{
							if (color.r > 1) color.r = 1; else if (color.r < 0) color.r = 0;
							if (color.g > 1) color.g = 1; else if (color.g < 0) color.g = 0;
							if (color.b > 1) color.b = 1; else if (color.b < 0) color.b = 0;

							if (_exportOptions.exportVertexTransparency())
							{
								if (color.a > 1) color.a = 1; else if (color.a < 0) color.a = 0;
								if (color.a < 1.f)
								{
									mesh.getMaterial().transparency = 0.f;
								}
							}
							else
							{
								color.a = 1;
							}
						}
					}
					else
					{
						color = MColor(mesh.getMaterial().diffuseColor.x, mesh.getMaterial().diffuseColor.y, mesh.getMaterial().diffuseColor.z, 1.f);
					}

					vtx.color = ATOM_ColorARGB(color.r, color.g, color.b, color.a);
				}

				int uvIdx;
				MStatus stat = bpIt.getUVIndex (triVertexIdx[vi], uvIdx);
				if (stat == MS::kSuccess)
				{
					vtx.texcoords.set (u[uvIdx], 1.f-v[uvIdx]);
				}
				else
				{
					vtx.texcoords.set (0.f, 0.f);
					//::MessageBox (::GetActiveWindow(), "获取贴图坐标索引失败！", "atom exporter", MB_OK|MB_ICONHAND);
					//return false;
				}

				if (weightjoints.empty () || weights.empty ())
				{
					vtx.weights.set (0.f, 0.f, 0.f, 0.f);
					vtx.weightjoints.x = 0;
					vtx.weightjoints.y = 0;
					vtx.weightjoints.z = 0;
					vtx.weightjoints.w = 0;
				}
				else
				{
					const MFloatArray &w = weights[vertexIdx];

					unsigned numWeights;
					if (w.length() > 4)
					{
						if (outputWeightProblem)
						{
							std::cout << "<atom exporter: 警告> 模型顶点中有的权值数目大于4个!" << std::endl;
							outputWeightProblem = false;
						}
						numWeights = 4;
					}
					else if (w.length() == 0)
					{
						::MessageBox (::GetActiveWindow(), "模型中有的顶点没有权值！", "atom exporter", MB_OK|MB_ICONHAND);
						return false;
					}
					else
					{
						numWeights = w.length();
					}

					const MIntArray &wj = weightjoints[vertexIdx];
					if (wj.length() != w.length())
					{
						return false;
					}

					vtx.weights.set(0.f, 0.f, 0.f, 0.f);
					vtx.weightjoints.x = 0;
					vtx.weightjoints.y = 0;
					vtx.weightjoints.z = 0;
					vtx.weightjoints.w = 0;
					for (unsigned iw = 0; iw < numWeights; ++iw)
					{
						if (w[iw] == 0.f)
						{
							std::cout << "<atom exporter: 警告> 模型中出现0权值!" << std::endl;
						}
						vtx.weights.xyzw[iw] = w[iw];
						vtx.weightjoints.xyzw[iw] = wj[iw];
					}
				}

				mesh.pushSuperVertex (vtx);
				verticesIndices.push_back (vertexIdx);
				uvIndices.push_back (uvIdx);
			}
		}
	}

	return true;
}

bool NM_Translator::getVertexJointWeights (const MDagPath &mesh, MFnSkinCluster *skinCluster, std::vector<MFloatArray> &weights, std::vector<MIntArray> &weightjoints, ATOM_Skeleton *skeleton)
{
	MItGeometry iterGeom(mesh);
	for (int i = 0; !iterGeom.isDone(); iterGeom.next(), ++i)
	{
		MObject component = iterGeom.component();
		unsigned numWeights;
		MFloatArray tmpWeights;
		MIntArray tmpJoints;
		MStatus stat = skinCluster->getWeights (mesh, component, tmpWeights, numWeights);
		if (stat != MS::kSuccess)
		{
			std::cout << "Get vertex joint weights failed." << std::endl;
			return false;
		}
		MDagPathArray influenceObjs;
		skinCluster->influenceObjects(influenceObjs, &stat);
		if (stat != MS::kSuccess)
		{
			std::cout << "Get influence objects failed." << std::endl;
			return false;
		}
		tmpJoints.setLength (tmpWeights.length());
		for (unsigned j = 0; j < influenceObjs.length(); ++j)
		{
			bool foundJoint = false;
			MString pathName = influenceObjs[j].partialPathName ();
			for (unsigned k = 0; k < skeleton->getJoints().size() && !foundJoint; ++k)
			{
				if (pathName == skeleton->getJoints()[k].name)
				{
					foundJoint = true;
					tmpJoints[j] = skeleton->getJoints()[k].id;
				}
			}
		}

		weights[i].setLength (0);
		weightjoints[i].setLength (0);
		for (unsigned n = 0; n < tmpWeights.length(); ++n)
		{
			float weightValue = tmpWeights[n];
			if (weightValue > 0.001f)
			{
				unsigned idx = 0;
				for (; idx < weights[i].length(); ++idx)
				{
					if (weightValue > weights[i][idx])
					{
						weights[i].insert(weightValue, idx);
						weightjoints[i].insert(tmpJoints[n], idx);
						break;
					}
				}

				if (idx == weights[i].length())
				{
					weights[i].append (tmpWeights[n]);
					weightjoints[i].append (tmpJoints[n]);
				}
			}
		}
	}

	return true;
}

void NM_Translator::getNormal (const MFnMesh &meshFn, MItMeshVertex &vIt, int meshVertexIndex, MItMeshPolygon &pIt, int faceVertexIndex, MVector &n) const
{
	MVectorArray nArray;
	bool smooth = true;

	int pvIndex = 0;
	vIt.setIndex (meshVertexIndex, pvIndex);

	if (1)
	{
		MIntArray edgeList;
		vIt.getConnectedEdges (edgeList);
		const unsigned nEdge = edgeList.length ();
		for (unsigned ei = 0; ei < nEdge; ++ei)
		{
			if (!meshFn.isEdgeSmooth (edgeList[ei]))
			{
				smooth = false;
				break;
			}
		}
	}

	if (smooth)
	{
		vIt.getNormals (nArray);
		n = MVector::zero;

		const unsigned nEnd = nArray.length ();
		for (unsigned ni = 0; ni != nEnd; ++ni)
		{
			n += nArray[ni];
		}

		n.normalize ();
	}
	else
	{
		pIt.getNormal (faceVertexIndex, n);
	}
}

void NM_Translator::outputDagInDependentOrder (const MDagPath &dagPath, MSelectionList  &done)
{
	if (!done.hasItem (dagPath))
	{
		MDagPath dp(dagPath);

		dp.pop ();

		if (dp.length () && _nodeList.hasItem (dp) && !done.hasItem (dp))
		{
			outputDagInDependentOrder (dp, done);
		}

		assert (_dagMap.find (dagPath.partialPathName().asChar()) == _dagMap.end ());

		CDagData *dd (new CDagData (done.length(), dagPath, getParent (dagPath), false));
		_dagMap[dagPath.partialPathName().asChar()] = dd;
		_orderedDagList.push_back (dd);
		done.add (dagPath);
	}
}

void NM_Translator::outputExportData (void)
{
	if (_nodeList.length() == 0)
	{
		std::cout << "Nothing to export." << std::endl;
		return;
	}

	MDagPath dagPath;

	_orderedDagList.clear ();
	_orderedDagList.reserve (_nodeList.length ());

	_dagMap.clear ();

	MSelectionList doneList;
	for (MItSelectionList sIt(_nodeList); !sIt.isDone(); sIt.next ())
	{
		if (sIt.getDagPath (dagPath))
		{
			outputDagInDependentOrder (dagPath, doneList);
		}
	}

	MyModel model;

	bool exportTheActions = _vertexAnimationInfo.actionSet.size() > 0 && _exportOptions.exportActions();
	const double tmStart = MAnimControl::minTime().as (MTime::kMilliseconds);
	const double tmEnd = MAnimControl::maxTime().as (MTime::kMilliseconds);
	const double tmInc = MTime (MAnimControl::playbackBy(), MTime::uiUnit()).as (MTime::kMilliseconds);

	if (exportTheActions)
	{
		model.setNumActions (_vertexAnimationInfo.actionSet.size());
		int actionindex = 0;
		for (std::map<std::string, VertexAnimationAction>::iterator it = _vertexAnimationInfo.actionSet.begin();
			 it != _vertexAnimationInfo.actionSet.end ();
			 ++it, 
			 ++actionindex)
		{
			model.setActionName (actionindex, it->first.c_str());
			model.setActionHash (actionindex, 0);
			model.setActionProps (actionindex, &it->second.properties);

			const std::vector<int> &frameList = it->second.frameList;
			model.setNumActionFrames (actionindex, frameList.size());
			if (frameList.size() > 0)
			{
				for (unsigned k = 0; k < frameList.size(); ++k)
				{
					double tm2 = MTime(frameList[k], MTime::kFilm).as(MTime::kMilliseconds);
					double tm1 = MTime(frameList[0], MTime::kFilm).as(MTime::kMilliseconds);
					model.setFrameTime (actionindex, k, tm2 - tm1);
				}
			}
		}
	}

	bool exportTheMeshes = _exportOptions.exportMeshes();

	if (1)
	{
		std::vector<MyMesh> meshList;

		for (MeshList::const_iterator mi = _meshDataList.begin(); mi != _meshDataList.end (); ++mi)
		{
			const MTime savedCurrentTime = MAnimControl::currentTime ();

			std::vector<MyMesh> m;

			bool hasSkeleton = (*mi)->SkinCluster() != 0;//_skeleton && _skeleton->getJoints().size() > 0;

			MFnMesh meshFn ((*mi)->DagPath());

			bool isBillboard = meshFn.name().length()>3 && (!strncmp(meshFn.name().asChar(), "bb_", 3) || strstr(meshFn.name().asChar(), "_bb_"));

			TransparencyMode transparencyMode = TM_NORMAL;
			if (!strncmp(meshFn.name().asChar(), "ta_", 3) || strstr (meshFn.name().asChar(), "_ta_"))
			{
				transparencyMode = TM_ADDITIVE;
			}
			else if (!strncmp(meshFn.name().asChar(), "tp_", 3) || strstr (meshFn.name().asChar(), "_tp_"))
			{
				transparencyMode = TM_PARTICLE;
			}
			else if (!strncmp(meshFn.name().asChar(), "tm_", 3) || strstr (meshFn.name().asChar(), "_tm_"))
			{
				transparencyMode = TM_MODULATE;
			}
			else if (!strncmp(meshFn.name().asChar(), "tM_", 3) || strstr (meshFn.name().asChar(), "_tM_"))
			{
				transparencyMode = TM_MODULATE_B;
			}
			else if (!strncmp(meshFn.name().asChar(), "tn_", 3) || strstr (meshFn.name().asChar(), "_tn_"))
			{
				transparencyMode = TM_IMODULATE;
			}
			else if (!strncmp(meshFn.name().asChar(), "tN_", 3) || strstr (meshFn.name().asChar(), "_tN_"))
			{
				transparencyMode = TM_IMODULATE_B;
			}
			else if (!strncmp(meshFn.name().asChar(), "tb_", 3) || strstr (meshFn.name().asChar(), "_tb_"))
			{
				transparencyMode = TM_INORMAL;
			}

			bool uvAnimationNoIpol = false;
			if (!strncmp(meshFn.name().asChar(), "bu_", 3) || strstr (meshFn.name().asChar(), "_bu_"))
			{
				uvAnimationNoIpol = true;
			}
			bool fixBlending = false;
			if (!strncmp(meshFn.name().asChar(), "fb_", 3) || strstr (meshFn.name().asChar(), "_fb_"))
			{
				fixBlending = true;
			}

			MaterialStyle style = MS_DEFAULT;
			if (!strncmp(meshFn.name().asChar(), "Mjs_", 3) || strstr (meshFn.name().asChar(), "_Mjs_"))
			{
				style = MS_METAL;
			}
			else if (!strncmp(meshFn.name().asChar(), "Msl_", 3) || strstr (meshFn.name().asChar(), "_Msl_"))
			{
				style = MS_PLASTIC;
			}
			else if (!strncmp(meshFn.name().asChar(), "Mpf_", 3) || strstr (meshFn.name().asChar(), "_Mpf_"))
			{
				style = MS_SKIN;
			}

			std::vector<std::vector<unsigned> > verticesIndices;
			std::vector<std::vector<unsigned> > uvIndices;
			const MObjectArray &shaders = (*mi)->getShaders ();
			const MIntArray &indices = (*mi)->getShaderIndices ();

			if (hasSkeleton)
			{
				/*
				if ((*mi)->getWeights().empty ())
				{
					(*mi)->getWeights().resize (meshFn.numVertices());
				}
				if ((*mi)->getWeightJoints().empty ())
				{
					(*mi)->getWeights().resize (meshFn.numVertices());
				}
				*/

				if (!outputMeshW (meshFn, (*mi)->DagPath(), m, verticesIndices, uvIndices, (*mi)->getWeights(), (*mi)->getWeightJoints(), shaders, indices))
				{
					std::cout << "Output mesh failed: " << (*mi)->DagPath().partialPathName().asChar() << std::endl;
					continue;
				}
			}
			else
			{
				if (!outputMesh (meshFn, (*mi)->DagPath(), m, verticesIndices, uvIndices, shaders, indices))
				{
					std::cout << "Output mesh failed: " << (*mi)->DagPath().partialPathName().asChar() << std::endl;
					continue;
				}
			}

			MMatrix worldMatrix = (*mi)->DagPath().inclusiveMatrix ();
			ATOM_Matrix4x4f matWorld(
				worldMatrix(0,0), worldMatrix(0,1), worldMatrix(0,2), worldMatrix(0,3),
				worldMatrix(1,0), worldMatrix(1,1), worldMatrix(1,2), worldMatrix(1,3),
				worldMatrix(2,0), worldMatrix(2,1), worldMatrix(2,2), worldMatrix(2,3),
				worldMatrix(3,0), worldMatrix(3,1), worldMatrix(3,2), worldMatrix(3,3)
				);

			for (unsigned mesh = 0; mesh < m.size(); ++mesh)
			{
				m[mesh].useSkeleton (hasSkeleton);

				if (_exportOptions.exportVertexColor()||_exportOptions.exportVertexTransparency())
				{
					m[mesh].setVertexFlags (m[mesh].getVertexFlags() | MyMesh::HasColors);
				}

				if (isBillboard)
				{
					m[mesh].setVertexFlags (m[mesh].getVertexFlags() | MyMesh::ATOM_Billboard);
				}

				if (uvAnimationNoIpol)
				{
					m[mesh].setVertexFlags (m[mesh].getVertexFlags() | MyMesh::UVAnimationNoIpol);
				}

				if (fixBlending)
				{
					m[mesh].setVertexFlags (m[mesh].getVertexFlags() | MyMesh::FixBlending);
				}

				m[mesh].setTransparencyMode (transparencyMode);
				m[mesh].setMaterialStyle (style);
				m[mesh].setSkeleton ((*mi)->SkeletonObj());
				m[mesh].setWorldMatrix (matWorld);

				for (unsigned prop = 0; prop < (*mi)->getProp()->getNumProperties(); ++prop)
				{
					m[mesh].addProperty ((*mi)->getProp()->getPropertyName(prop), (*mi)->getProp()->getPropertyValue(prop));
				}

				PropertyEditor *properties = _materialviewer.getPropertyByName (m[mesh].getMaterial().name.c_str());
				if (properties)
				{
					for (unsigned prop = 0; prop < properties->getNumProperties(); ++prop)
					{
						m[mesh].addProperty (properties->getPropertyName(prop), properties->getPropertyValue(prop));
					}
				}
			}

			if (exportTheActions)
			{
				if (1 || exportTheMeshes)
				{
					for (unsigned i = 0; i < m.size(); ++i)
					{
						m[i].setNumActions (_vertexAnimationInfo.actionSet.size());
					}


					const double tmStart = MAnimControl::minTime().as (MTime::kMilliseconds);
					const double tmEnd = MAnimControl::maxTime().as (MTime::kMilliseconds);
					const double tmInc = MTime (MAnimControl::playbackBy(), MTime::uiUnit()).as (MTime::kMilliseconds);

					unsigned actionindex = 0;

					for (std::map<std::string, VertexAnimationAction>::iterator it = _vertexAnimationInfo.actionSet.begin();
						 it != _vertexAnimationInfo.actionSet.end ();
						 ++it, ++actionindex)
					{
						const std::vector<int> &frameList = it->second.frameList;

						std::vector<MFloatPointArray> keyFrames(frameList.size());
						std::vector<MFloatArray> keyFramesU(frameList.size());
						std::vector<MFloatArray> keyFramesV(frameList.size());
						std::vector<MMatrix> keyFramesMatrix(frameList.size());

						MFnMesh meshFn ((*mi)->DagPath());
						unsigned oldCount = 0;
						for (unsigned i = 0; i < frameList.size(); ++i)
						{
							MStatus stat = MAnimControl::setCurrentTime (MTime(frameList[i] * tmInc, MTime::kMilliseconds));

							if (!hasSkeleton)
							{
								meshFn.getPoints (keyFrames[i], MSpace::kWorld);
								keyFramesMatrix[i] = (*mi)->DagPath().inclusiveMatrix ();
							}

							meshFn.getUVs (keyFramesU[i], keyFramesV[i]);
							if (oldCount != 0 && oldCount != keyFramesU[i].length())
							{
								char buffer[256];
								sprintf (buffer, "%s: 动画过程中UV点数量发生变化!", (*mi)->getName());
								::MessageBox (::GetActiveWindow(), buffer, "错误", MB_OK|MB_ICONHAND);
								return;
							}
							oldCount = keyFramesU[i].length ();

							// get material
							const MIntArray &faceShaderIndices = indices;
							//MObjectArray shaders;
							//meshFn.getConnectedShaders ((*mi)->DagPath().instanceNumber (), shaders, faceShaderIndices);
							//assert (shaders.length() == m.size());

							for (unsigned j = 0; j < m.size(); ++j)
							{
								material mat;
								if (!getMaterial (shaders[j], &mat))
								{
									std::cout << "Get Animating ATOM_Material failed." << std::endl;
									return;
								}
								m[j].addMaterialFrame (actionindex, mat.transparency);
							}

							unsigned numPs = keyFrames[i].length();
							unsigned numUs = keyFramesU[i].length();
							unsigned numVs = keyFramesV[i].length();
							assert(numUs == numVs);
						}
						MAnimControl::setCurrentTime (savedCurrentTime);

						for (unsigned i = 0; i < frameList.size(); ++i)
						{
							for (unsigned j = 0; j < m.size(); ++j)
							{
								unsigned numVerts = m[j].getNumVertices();
								std::vector<ATOM_Vector3f> newVerts(hasSkeleton ? 0 : numVerts);
								std::vector<ATOM_Vector2f> newUVs(numVerts);

								for (unsigned v = 0; v < numVerts; ++v)
								{
									if (!hasSkeleton)
									{
										unsigned vertexIdx = verticesIndices[j][v];
										unsigned numMeshVerts = keyFrames[i].length();
										assert(vertexIdx < numMeshVerts);
										const MPoint &pt = keyFrames[i][vertexIdx];
										newVerts[v].set (pt.x, pt.y, pt.z);
									}

									unsigned uvIdx = uvIndices[j][v];
									unsigned numMeshUs = keyFramesU[i].length();
									unsigned numMeshVs = keyFramesV[i].length();
									assert(numMeshUs == numMeshVs && uvIdx < numMeshUs);
									float uVal = keyFramesU[i][uvIdx];
									float vVal = keyFramesV[i][uvIdx];
									newUVs[v].set (uVal, 1.f - vVal);
								}

								ATOM_Matrix4x4f matWorld(
									keyFramesMatrix[i](0,0),keyFramesMatrix[i](0,1),keyFramesMatrix[i](0,2),keyFramesMatrix[i](0,3),
									keyFramesMatrix[i](1,0),keyFramesMatrix[i](1,1),keyFramesMatrix[i](1,2),keyFramesMatrix[i](1,3),
									keyFramesMatrix[i](2,0),keyFramesMatrix[i](2,1),keyFramesMatrix[i](2,2),keyFramesMatrix[i](2,3),
									keyFramesMatrix[i](3,0),keyFramesMatrix[i](3,1),keyFramesMatrix[i](3,2),keyFramesMatrix[i](3,3));

								matWorld >> ATOM_Matrix4x4f::getRotateXMatrix (ATOM_HalfPi);
								matWorld >> ATOM_Matrix4x4f::getScaleMatrix (ATOM_Vector3f(1.f, 1.f, -1.f));

								m[j].addAnimationFrame (actionindex, matWorld, newVerts, newUVs);
							}
						}
					}
				}
			}

			for (unsigned i = 0; i < m.size(); ++i)
			{
				meshList.push_back (m[i]);
			}
		}

		for (unsigned i =0; i < meshList.size(); ++i)
		{
			model.addMesh (meshList[i]);
		}

		meshList.resize (0);
	}

	if (exportTheActions && _skeleton && _skeleton->getJoints().size())
	{
		unsigned actionindex = 0;

		for (std::map<std::string, VertexAnimationAction>::iterator it = _vertexAnimationInfo.actionSet.begin();
			 it != _vertexAnimationInfo.actionSet.end ();
			 ++it, ++actionindex)
		{
			const std::vector<int> &frameList = it->second.frameList;
			ATOM_BBox actionBBox;
			actionBBox.beginExtend ();

			for (unsigned i = 0; i < frameList.size(); ++i)
			{
				std::vector<ATOM_Matrix4x4f> matrices(_skeleton->getJoints().size());

				MAnimControl::setCurrentTime (MTime(frameList[i] * tmInc, MTime::kMilliseconds));
				for (unsigned j = 0; j < _skeleton->getJoints().size(); ++j)
				{
					ATOM_Matrix4x4f matrix = MMatrixToMatrix (_skeleton->getJoints()[j].jointDag.inclusiveMatrix());
					int parent = _skeleton->getJoints()[j].parentIndex;
					if ( parent >= 0)
					{
						ATOM_Matrix4x4f matrixParentInverse;
						matrixParentInverse.invertAffineFrom (MMatrixToMatrix (_skeleton->getJoints()[parent].jointDag.inclusiveMatrix()));
						matrix = matrixParentInverse >> matrix;
					}

					ATOM_Matrix4x4f matrixBindInverse;
					matrixBindInverse.invertAffineFrom (_skeleton->getJoints()[j].localMatrix);
					matrices[j] = (matrixBindInverse >> matrix);
				}

				for (MeshList::iterator it = _meshDataList.begin(); it != _meshDataList.end(); ++it)
				{
					MFnMesh mesh((*it)->DagPath ());
					MPoint minPoint = mesh.boundingBox().min ();
					MPoint maxPoint = mesh.boundingBox().max ();
					MBoundingBox bbox (minPoint, maxPoint);
					bbox.transformUsing ((*it)->DagPath().inclusiveMatrix ());
					minPoint = bbox.min ();
					maxPoint = bbox.max ();
					actionBBox.extend (ATOM_Vector3f(minPoint.z, minPoint.y, minPoint.x));
					actionBBox.extend (ATOM_Vector3f(maxPoint.z, maxPoint.y, maxPoint.x));
				}

				if (exportTheMeshes)
				{
					for (unsigned mesh = 0; mesh < model.meshes().size(); ++mesh)
					{
						if (/*model.meshes()[mesh].getSkeleton() == */_skeleton)
						{
							model.meshes()[mesh].addSkeletonFrame (actionindex, matrices);
						}
					}
				}
				else
				{
					model.setFrameJoints (actionindex, i, matrices);
				}
			}

			if (!exportTheMeshes)
			{
				model.setActionHash (actionindex, _skeleton->getHash());
			}

			model.getActionBBox (actionindex) = actionBBox;
		}
		//MAnimControl::setCurrentTime (savedCurrentTime);
	}

	//for (unsigned ai = 0; ai < _vertexAnimationInfo.actionSet.size(); ++ai)
	//{
	//	for (unsigned mesh = 1; mesh < model.meshes().size(); ++mesh)
	//	{
	//		for (unsigned frame = 0; frame < model.meshes()[mesh].getNumSkeletonAnimationFrames(); ++frame)
	//		{
	//			const std::vector<ATOM_Matrix4x4f> &m1 = model.meshes()[0].getSkeletonAnimationFrame(frame).jointMatrices;
	//			const std::vector<ATOM_Matrix4x4f> &m2 = model.meshes()[mesh].getSkeletonAnimationFrame(frame).jointMatrices;
	//			if (m1.size() != m2.size())
	//			{
	//				printf ("Error: mesh%d and mesh%d have different joint matrix count!\n", 0, mesh);
	//			}
	//			else
	//			{
	//				for (unsigned matrix = 0; matrix < m1.size(); ++matrix)
	//				{
	//					for (unsigned e = 0; e < 16; ++e)
	//					{
	//						float delta = m1[matrix].m[e] - m2[matrix].m[e];
	//						if (delta < 0.f) delta = -delta;
	//						if (delta > 0.01f)
	//						{
	//							printf ("Error: mesh%d and mesh%d have different joint matrix value (%f)\n", 0, mesh, delta);
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	model.setSkeleton (_skeleton);
	model.save (_outFileName.c_str(), _exportOptions.exportSkeletons(), exportTheActions, exportTheMeshes, &_skeletonviewer, &_modelprops, &_vertexAnimationInfo);

	if (exportTheActions && _exportOptions.saveAnimationSettings ())
	{
		std::string filename = _outFileName + ".lst";
		saveAnimationInfo (_vertexAnimationInfo, filename.c_str());
	}
}

void NM_Translator::setShareDiffuseMap (bool b)
{
	_shareDiffuseMap = b;
}

void NM_Translator::setShareNormalMap (bool b)
{
	_shareNormalMap = b;
}

bool NM_Translator::loadSkeleton (const MDagPath &meshDagPath, ATOM_Skeleton *skeleton)
{
	MFnSkinCluster *skinCluster = 0;

	MFnMesh mesh(meshDagPath);

	MItDependencyNodes kDepNodeIt(MFn::kSkinClusterFilter);
	for (; !kDepNodeIt.isDone () && !skinCluster; kDepNodeIt.next ())
	{
		MObject obj = kDepNodeIt.item ();
		skinCluster = new MFnSkinCluster(obj);
		unsigned numGeoms = skinCluster->numOutputConnections();
		for (unsigned iGeom = 0; iGeom < numGeoms; ++iGeom)
		{
			unsigned index = skinCluster->indexForOutputConnection(iGeom);
			MObject kOutputObj = skinCluster->outputShapeAtIndex (index);
			if (kOutputObj != mesh.object())
			{
				delete skinCluster;
				skinCluster = 0;
				break;
			}
		}
	}

	if (!skinCluster)
	{
		return false;
	}

	MStatus stat = skeleton->load (skinCluster);

	return stat == MS::kSuccess;
}

bool NM_Translator::loadSkeletonNames (const MDagPath &meshDagPath, ATOM_Skeleton *skeleton)
{
	MFnSkinCluster *skinCluster = 0;

	MFnMesh mesh(meshDagPath);

	MItDependencyNodes kDepNodeIt(MFn::kSkinClusterFilter);
	for (; !kDepNodeIt.isDone () && !skinCluster; kDepNodeIt.next ())
	{
		MObject obj = kDepNodeIt.item ();
		skinCluster = new MFnSkinCluster(obj);
		unsigned numGeoms = skinCluster->numOutputConnections();
		for (unsigned iGeom = 0; iGeom < numGeoms; ++iGeom)
		{
			unsigned index = skinCluster->indexForOutputConnection(iGeom);
			MObject kOutputObj = skinCluster->outputShapeAtIndex (index);
			if (kOutputObj != mesh.object())
			{
				delete skinCluster;
				skinCluster = 0;
				break;
			}
		}
	}

	if (!skinCluster)
	{
		return false;
	}

	MStatus stat = skeleton->loadNames (skinCluster);

	return stat == MS::kSuccess;
}

bool NM_Translator::loadAnimationInfo (VertexAnimationInfo &info, const char *filename)
{
	info.actionSet.clear ();

	ATOM_TiXmlDocument doc(filename);
	if (!doc.LoadFile ())
	{
		return false;
	}

	ATOM_TiXmlElement *eRoot = doc.RootElement ();
	if (!eRoot)
	{
		return false;
	}

	for (ATOM_TiXmlElement *eAction = eRoot->FirstChildElement("Action"); eAction; eAction = eAction->NextSiblingElement ("Action"))
	{
		const char *actionName = eAction->Attribute ("Name");
		if (actionName)
		{
			if (info.actionSet.find (actionName) != info.actionSet.end ())
			{
				continue;
			}
			int customKey = 0;
			if (!eAction->Attribute ("CustomKeys", &customKey))
			{
				continue;
			}

			std::vector<int> frameList;
			int frameInterval = 1;

			if (customKey)
			{
				for (ATOM_TiXmlElement *eFrame = eAction->FirstChildElement ("KeyFrame"); eFrame; eFrame = eFrame->NextSiblingElement ("KeyFrame"))
				{
					int frame;

					if (0 == eFrame->Attribute ("Frame", &frame))
					{
						continue;
					}

					frameList.push_back (frame);
				}
			}
			else
			{
				int frameStart, frameEnd;

				if (!eAction->Attribute ("FrameInterval", &frameInterval))
				{
					continue;
				}

				if (!eAction->Attribute ("FrameStart", &frameStart))
				{
					continue;
				}

				if (!eAction->Attribute ("FrameEnd", &frameEnd))
				{
					continue;
				}

				frameList.push_back (frameStart);
				frameList.push_back (frameEnd);
			}

			VertexAnimationAction &action = info.actionSet[actionName];
			action.actionName = actionName;
			action.frameInterval = frameInterval;
			action.keyFrames = frameList;
			action.useCustomKeyFrames = customKey != 0;
			if (action.useCustomKeyFrames)
			{
				action.frameList = action.keyFrames;
			}
			else
			{
				action.frameList.clear ();
				for (int i = action.keyFrames.front(); i < action.keyFrames.back(); i += action.frameInterval)
				{
					action.frameList.push_back (i);
				}
				action.frameList.push_back (action.keyFrames.back());
			}
		}
	}

	return true;
}

bool NM_Translator::saveAnimationInfo (const VertexAnimationInfo &info, const char *filename)
{
	ATOM_TiXmlDocument doc(filename);
	ATOM_TiXmlDeclaration eDecl("1.0", "gb2312", "");
	doc.InsertEndChild (eDecl);

	ATOM_TiXmlElement eRoot("Actions");
	for (std::map<std::string, VertexAnimationAction>::const_iterator it = info.actionSet.begin(); it != info.actionSet.end(); ++it)
	{
		ATOM_TiXmlElement eAction("Action");
		eAction.SetAttribute("Name", it->first.c_str());
		eAction.SetAttribute("CustomKeys", it->second.useCustomKeyFrames ? 1 : 0);
		if (it->second.useCustomKeyFrames)
		{
			for (unsigned i = 0; i < it->second.keyFrames.size(); ++i)
			{
				ATOM_TiXmlElement eFrame("KeyFrame");
				eFrame.SetAttribute ("Frame", it->second.keyFrames[i]);
				eAction.InsertEndChild (eFrame);
			}
		}
		else
		{
			eAction.SetAttribute ("FrameInterval", it->second.frameInterval);
			eAction.SetAttribute ("FrameStart", it->second.keyFrames.front());
			eAction.SetAttribute ("FrameEnd", it->second.keyFrames.back());
		}
		eRoot.InsertEndChild (eAction);
	}

	doc.InsertEndChild (eRoot);

	return doc.SaveFile ();
}

//MTime NM_Translator::frameToTime (unsigned frame) const
//{
//	const MTime::Unit uiUnit = MTime::uiUnit ();
//	MTime tmStart = MAnimControl::minTime();
//	MTime tmEnd = MAnimControl::maxTime();
//	MTime tmStart2 = MAnimControl::animationStartTime ();
//	MTime tmEnd2 = MAnimControl::animationEndTime ();
//	double inc = MAnimControl::playbackBy();
//
//	double start = tmStart.as (MTime::uiUnit);
//	double end = tmEnd.as (MTime::uiUnit);
//	double start2 = tmStart2.as (MTime::uiUnit);
//	double end2 = tmEnd2.as (MTime::uiUnit);
//
//	MTime tmFrame(start + inc * frame, MTime::uiUnit());
//
//	return tmFrame;
//}
//
//
