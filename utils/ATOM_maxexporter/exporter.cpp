#include "StdAfx.h"

#include "exporter.h"
#include "mesh.h"
#include "model.h"
#include "resource.h"
#include "config.h"
#include "skeleton.h"
#include "tinyxml.h"
#include "propedit.h"
#include "CrowdSimulationModel.h"

// Defined in EditTriObj, in the 3dsMax samples
#define ET_MASTER_CONTROL_REF 0
#define ET_VERT_BASE_REF  1

// Defined in PolyEdit, in the 3dsMax samples
#define EPOLY_PBLOCK 0
#define EPOLY_MASTER_CONTROL_REF  1
#define EPOLY_VERT_BASE_REF 2

extern "C" HINSTANCE hModelHandle;

static ATOM_MaxExporterClassDesc classDesc;
extern"C" void *GetMaxExporterDesc (void)
{
	return &classDesc;
}

extern"C" int GetMaxVersion (void)
{
	return VERSION_3DSMAX;
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

class ExporterLogger: public Logger
{
	HWND _hEdit;
	HWND _hDialog;

public:
	ExporterLogger ()
	{
		_hEdit = NULL;
	}

	void setEdit (HWND hWndEdit)
	{
		_hEdit = hWndEdit;
	}

	void setDialog (HWND hWndDlg)
	{
		_hDialog = hWndDlg;
	}

	virtual void output (const char *fmt, ...)
	{
		if (_hEdit)
		{
			va_list args;
			va_start(args, fmt);

			char buffer[2048];
			vsnprintf (buffer, 2048, fmt, args);
		
			int ndx = ::GetWindowTextLength (_hEdit);
			HWND prevFocus = ::SetFocus (_hEdit);
#ifdef WIN32
			::SendMessage (_hEdit, EM_SETSEL, (WPARAM)ndx, (LPARAM)ndx);
#else
			::SendMessage (_hEdit, EM_SETSEL, 0, MAKELONG (ndx, ndx));
#endif
			::SendMessage (_hEdit, EM_REPLACESEL, 0, (LPARAM)((LPSTR)buffer));
			
			va_end(args);
		}
	}
};

static ExporterLogger logger;

INT_PTR CALLBACK ATOM_MaxExporter::MaxExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static ATOM_MaxExporter *translator = NULL;
	static int animchoice = 0; // 0: no animation 1: vertex animation 2: bone animation

	switch(message) {
		case WM_INITDIALOG:
			{
				translator = (ATOM_MaxExporter*)lParam;
				
				// 搜集3D MAX场景信息
				INode *rootNode = GetCOREInterface()->GetRootNode();
				logger.setEdit (::GetDlgItem (hWnd, IDC_LOG));
				translator->pEM = new ExportManager(&logger);
				translator->pEM->Gather3DSMAXSceneData( rootNode, TRUE, translator->_exportSelected, &translator->_materialviewer);

				translator->_skeleton = translator->pEM->CreateSkeleton ();
				if (translator->_skeleton)
				{
					translator->_skeletonviewer.setSkeleton (translator->_skeleton, translator->_exportOptions.getSelectBoneByName());
				}

				::SendMessage (::GetDlgItem (hWnd, IDC_EXPORTCOLOR), BM_SETCHECK, translator->_exportOptions.exportVertexColor() ? BST_CHECKED : BST_UNCHECKED, 0);
				::SendMessage (::GetDlgItem (hWnd, IDC_EXPORTVERTEXALPHA), BM_SETCHECK, translator->_exportOptions.exportVertexTransparency() ? BST_CHECKED : BST_UNCHECKED, 0);
				::SendMessage (::GetDlgItem (hWnd, IDC_SAVEANIMATION_SETTINGS), BM_SETCHECK, translator->_exportOptions.saveAnimationSettings() ? BST_CHECKED : BST_UNCHECKED, 0);
				::SendMessage (::GetDlgItem (hWnd, IDC_EXPOPT_ACTIONS), BM_SETCHECK, translator->_exportOptions.exportActions() ? BST_CHECKED : BST_UNCHECKED, 0);
				::SendMessage (::GetDlgItem (hWnd, IDC_EXPOPT_MESHES), BM_SETCHECK, translator->_exportOptions.exportMeshes() ? BST_CHECKED : BST_UNCHECKED, 0);
				::SendMessage (::GetDlgItem (hWnd, IDC_EXPOPT_SKELETONS), BM_SETCHECK, translator->_exportOptions.exportSkeletons() ? BST_CHECKED : BST_UNCHECKED, 0);
				::SendMessage (::GetDlgItem (hWnd, IDC_DOUBLESIDE), BM_SETCHECK, translator->_exportOptions.doubleSide() ? BST_CHECKED : BST_UNCHECKED, 0);
				::SendMessage (::GetDlgItem (hWnd, IDC_SELECTBONEBYNAME), BM_SETCHECK, translator->_exportOptions.getSelectBoneByName() ? BST_CHECKED : BST_UNCHECKED, 0);
				::SendMessage (::GetDlgItem (hWnd, IDC_EXPORTBINDABLE), BM_SETCHECK, translator->_exportOptions.getBindable() ? BST_CHECKED : BST_UNCHECKED, 0);
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
					translator->_skeletonviewer.setSkeleton (translator->_skeleton, translator->_exportOptions.getSelectBoneByName());
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

					translator->_exportOptions.setBindable(
						BST_CHECKED == ::SendMessage (::GetDlgItem(hWnd, IDC_EXPORTBINDABLE), BM_GETCHECK, 0, 0));

					if (!translator->_exportOptions.exportActions() && !translator->_exportOptions.exportMeshes() && !translator->_exportOptions.exportSkeletons())
					{
						logger.output ("**没有东西可导出!\n");
						break;
					}
					
					if (!translator->exportScene (translator->_outputFileName.c_str()))
					{
						logger.output ("**输出失败!\n");
					}
					else
					{
						logger.output ("输出完成!\n");
					}

					break;
				}
			case IDC_EXPORT_FINISH:
				{
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

	strlwr (buffer);
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

bool ATOM_MaxExporter::translateFileName (const std::string &fileName, const std::string &savepath, std::string &vfsFileName, std::string &physicFileName, bool share) const
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

ATOM_MaxExporter::ATOM_MaxExporter (void)
{
	_maxNameLen = 0;
	_exportSkeleton = true;
	_exportSkeletonMesh = true;
	_exportSkeletonAnim = true;
	_options = 0;
	_skeleton = 0;
	pEM = 0;
}

ATOM_MaxExporter::~ATOM_MaxExporter (void)
{
}

void ATOM_MaxExporter::clear (void)
{
}

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

int ATOM_MaxExporter::ExtCount()
{
	return 1;
}

const TCHAR *ATOM_MaxExporter::Ext(int n)
{
	return _T("nm");
}

const TCHAR *ATOM_MaxExporter::LongDesc()
{
	return _T("ATOM3D model");
}

const TCHAR *ATOM_MaxExporter::ShortDesc()
{
	return _T("ATOM3D model");
}

const TCHAR *ATOM_MaxExporter::AuthorName()
{
	return _T("GYK");
}

const TCHAR *ATOM_MaxExporter::CopyrightMessage()
{
	return _T("");
}

const TCHAR *ATOM_MaxExporter::OtherMessage1()
{
	return _T("");
}

const TCHAR *ATOM_MaxExporter::OtherMessage2()
{
	return _T("");
}

unsigned int ATOM_MaxExporter::Version()
{
	return 100;
}

void ATOM_MaxExporter::ShowAbout(HWND hWnd)
{
}

BOOL ATOM_MaxExporter::SupportsOptions(int ext, DWORD options)
{
	return TRUE;
}

bool ATOM_MaxExporter::exportScene (const char *filename)
{
	std::cout << "atom mesh translator version 0.5.0" << std::endl;
	std::cout << "By GYK@server01" << std::endl;
	MyModel model(&logger);

	bool exportTheActions = _vertexAnimationInfo.actionSet.size() > 0 && _exportOptions.exportActions();
	bool exportTheMeshes = _exportOptions.exportMeshes();
	bool exportSkeleton = _exportOptions.exportSkeletons();
	bool exportBindable = _exportOptions.getBindable();

	pEM->ExportToMyModel (filename, _skeleton, &model, &_vertexAnimationInfo, exportTheMeshes, exportTheActions, exportBindable, exportSkeleton, _shareDiffuseMap);

	model.setSkeleton (_skeleton);
	return model.save (filename, exportSkeleton, exportTheActions, exportTheMeshes, &_skeletonviewer, &_modelprops, &_vertexAnimationInfo);
}

void getBaseObjectAndID( Object*& object, SClass_ID& sid )
{
	/*
	if( object == NULL )
		return ;

	sid = object->SuperClassID();

	if( sid == WSM_DERIVOB_CLASS_ID || sid == DERIVOB_CLASS_ID || sid == GEN_DERIVOB_CLASS_ID )
	{
		IDerivedObject * derivedObject =( IDerivedObject* ) object;

		if( derivedObject->NumModifiers() > 0 )
		{
			// Remember that 3dsMax has the mod stack reversed in its internal structures.
			// So that evaluating the zero'th modifier implies evaluating the whole modifier stack.
			ObjectState state = derivedObject->Eval( 0, 0 );
			object = state.obj;
		}
		else
		{
			object = derivedObject->GetObjRef();
		}

		sid = object->SuperClassID();
	}
	*/
}

int ATOM_MaxExporter::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	_outputFileName = identifyFileName(name);
	
	std::string filename = _outputFileName + ".lst";
	loadAnimationInfo (_vertexAnimationInfo, filename.c_str());
	getAnimationRange (&_vertexAnimationInfo.rangeInfo);
	_modelprops.clear ();

	_exportSelected = (options & SCENE_EXPORT_SELECTED) == SCENE_EXPORT_SELECTED;
	
	::DialogBoxParam(hModelHandle, MAKEINTRESOURCE(IDD_EXPORTER), GetActiveWindow(), MaxExporterOptionsDlgProc, (LPARAM)this);
	
	delete pEM;
	pEM = 0;

	return 1;
}

bool ATOM_MaxExporter::loadAnimationInfo (VertexAnimationInfo &info, const char *filename)
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

bool ATOM_MaxExporter::saveAnimationInfo (const VertexAnimationInfo &info, const char *filename)
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

void ATOM_MaxExporter::setShareDiffuseMap (bool b)
{
	_shareDiffuseMap = b;
}

void ATOM_MaxExporter::setShareNormalMap (bool b)
{
	_shareNormalMap = b;
}

//MTime ATOM_MaxExporter::frameToTime (unsigned frame) const
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
