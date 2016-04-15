#include <windowsx.h>
#include <shlobj.h>
#include "mtl_translator.h"
#include "config.h"
#include "resource.h"

#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MItDag.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MFnLambertShader.h>
#include <maya/MFnBlinnShader.h>
#include <maya/MFnPhongShader.h>

extern HINSTANCE hModelHandle;

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

INT_PTR CALLBACK MaterialOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static MTL_Translator *translator = NULL;

	switch(message) {
		case WM_INITDIALOG:
			{
				translator = (MTL_Translator*)lParam;
				CenterWindow(hWnd,GetParent(hWnd));
				if (translator->getOptDoubleSide())
				{
					::SendMessage (::GetDlgItem (hWnd, IDC_M_DOUBLESIDE), BM_SETCHECK, BST_CHECKED, 0);
				}
				if (translator->getOptShareTexture())
				{
					::SendMessage (::GetDlgItem (hWnd, IDC_M_SHARETEXTURE), BM_SETCHECK, BST_CHECKED, 0);
				}
				::SendMessage (::GetDlgItem (hWnd, IDC_M_COLOROPT), CB_ADDSTRING, 0, (LPARAM)"使用材质颜色");
				::SendMessage (::GetDlgItem (hWnd, IDC_M_COLOROPT), CB_ADDSTRING, 0, (LPARAM)"使用顶点颜色");
				::SendMessage (::GetDlgItem (hWnd, IDC_M_COLOROPT), CB_ADDSTRING, 0, (LPARAM)"仅使用贴图颜色");
				::SendMessage (::GetDlgItem (hWnd, IDC_M_COLOROPT), CB_ADDSTRING, 0, (LPARAM)"换发色模式");

				switch (translator->getColorMode())
				{
				case MTL_Translator::MaterialColor:
					::SendMessage (::GetDlgItem (hWnd, IDC_M_COLOROPT), CB_SETCURSEL, 0, 0);
					break;
				case MTL_Translator::VertexColor:
					::SendMessage (::GetDlgItem (hWnd, IDC_M_COLOROPT), CB_SETCURSEL, 1, 0);
					break;
				case MTL_Translator::TextureColor:
					::SendMessage (::GetDlgItem (hWnd, IDC_M_COLOROPT), CB_SETCURSEL, 2, 0);
					break;
				case MTL_Translator::InterpolateColor:
					::SendMessage (::GetDlgItem (hWnd, IDC_M_COLOROPT), CB_SETCURSEL, 3, 0);
					break;
				}
				const std::vector<MTL_Translator::MayaMaterial> &materials = translator->getMaterialList();
				for (unsigned i = 0; i < materials.size(); ++i)
				{
					::SendMessage (::GetDlgItem (hWnd, IDC_MATERIAL_LIST), LB_ADDSTRING, 0, (LPARAM)materials[i].name.c_str());
				}
				if (materials.size() > 0)
				{
					::SendMessage (::GetDlgItem (hWnd, IDC_MATERIAL_LIST), LB_SETCURSEL, 0, 0);
				}

				return TRUE;
			}

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case IDC_SETUPDIR:
				{
					ExporterConfig &exporterConfig = ExporterConfig::getInstance();
					char oldrootDir[MAX_PATH] = { 0 };
					char oldshareDir[MAX_PATH] = { 0 };
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
			case IDOK:
				{
					int index = ::SendMessage (::GetDlgItem (hWnd, IDC_MATERIAL_LIST), LB_GETCURSEL, 0, 0);
					if (index != LB_ERR)
					{
						translator->setOptDoubleSide((BST_CHECKED == ::SendMessage (::GetDlgItem (hWnd, IDC_M_DOUBLESIDE), BM_GETCHECK, 0L, 0L)));
						translator->setOptShareTexture((BST_CHECKED == ::SendMessage (::GetDlgItem (hWnd, IDC_M_SHARETEXTURE), BM_GETCHECK, 0L, 0L)));
						//translator->setOptLighting(true);
						switch (::SendMessage (::GetDlgItem (hWnd, IDC_M_COLOROPT), CB_GETCURSEL, 0, 0))
						{
						case 0:
							translator->setColorMode (MTL_Translator::MaterialColor);
							break;
						case 1:
							translator->setColorMode (MTL_Translator::VertexColor);
							break;
						case 2:
							translator->setColorMode (MTL_Translator::TextureColor);
							break;
						case 3:
							translator->setColorMode (MTL_Translator::InterpolateColor);
							break;
						default:
							translator->setColorMode (MTL_Translator::MaterialColor);
							break;
						}
						translator->setExportIndex (index);
						EndDialog (hWnd, IDOK);
					}
					break;
				}
			case IDCANCEL:
				{
					EndDialog (hWnd, IDCANCEL);
					break;
				}
			}
			break;
		case WM_CLOSE:
			EndDialog(hWnd, IDCANCEL);
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

MTL_Translator::MTL_Translator (void)
{
	_shareTexture = false;
	_doubleside = false;
	_lighting = true;
	_colormode = MaterialColor;
}

MTL_Translator::~MTL_Translator (void)
{
}

void * MTL_Translator::creator (void)
{
	return new MTL_Translator;
}

MPxFileTranslator::MFileKind MTL_Translator::identifyFile (const MFileObject &fileObject, const char *buffer, short size) const
{
	const char *name = fileObject.name().asChar ();
	int nameLength = (int)strlen (name);
	if (nameLength > 5 && !_stricmp (name + nameLength - 5, ".nmtl"))
	{
		return kIsMyFileType;
	}
	return kNotMyFileType;
}

static bool isObjectSelected (const MDagPath& path)
{
   MDagPath sDagPath;
   
   MSelectionList activeList;
   MGlobal::getActiveSelectionList (activeList);
   
   MItSelectionList iter (activeList, MFn::kDagNode);
   
   while (!iter.isDone())
   {
      if (iter.getDagPath (sDagPath))
      {
         if (sDagPath == path)
            return true;
      }
      iter.next();
   }
   return false;
}


MStatus MTL_Translator::writer (const MFileObject &file, const MString &optionsString, MPxFileTranslator::FileAccessMode mode)
{
	_outFileName = identifyFileName(file.fullName().asChar ());

	MStatus stat = MS::kSuccess;
	MItDag dagIt(MItDag::kDepthFirst, MFn::kInvalid, &stat);
	if (stat != MS::kSuccess)
	{
		return stat;
	}

	MSelectionList exportList;

	for (; !dagIt.isDone(); dagIt.next ())
	{
		MDagPath dagPath;
		stat = dagIt.getPath (dagPath);
		if (stat != MS::kSuccess)
		{
			continue;
		}

		if (!isObjectSelected (dagPath))
		{
			continue;
		}

		MFnDagNode fnDagNode (dagPath, &stat);
		if (stat != MS::kSuccess)
		{
			continue;
		}

		if (fnDagNode.isIntermediateObject())
		{
			continue;
		}

		MObject obj = dagPath.node ();
		const char *type = obj.apiTypeStr ();

		if (dagPath.hasFn (MFn::kMesh))
		{
			stat = exportList.add (dagPath, MObject::kNullObj, true);
			if (stat != MS::kSuccess)
			{
				return stat;
			}

			MObjectArray shaderArray;
			MIntArray faceToShaderArray;
			MFnMesh fnMesh(dagPath);

			if (fnMesh.getConnectedShaders (0, shaderArray, faceToShaderArray) == MS::kSuccess)
			{
				for (unsigned int shi = 0; shi < shaderArray.length(); ++shi)
				{
					int id = getMaterialId (shaderArray[shi]);
					if (id != -1)
					{
						continue;
					}
					MayaMaterial m;
					if (!getMaterial (shaderArray[shi], &m.mat, m.fileToCopy))
					{
						continue;
					}
					m.mayaObj = shaderArray[shi];
					MItDependencyGraph shaderIter(m.mayaObj, MFn::kLambert, MItDependencyGraph::kUpstream, MItDependencyGraph::kDepthFirst, MItDependencyGraph::kNodeLevel, &stat);
				    if (stat != MS::kSuccess)
					{
						return stat;
					}
					MFnLambertShader fnShader(shaderIter.thisNode(), &stat);
					if (stat != MS::kSuccess)
					{
						return stat;
					}
					m.name = fnShader.name().asChar();

					_materials.push_back (m);
				}
			}
		}
	}

	if (IDOK == ::DialogBoxParam(hModelHandle, MAKEINTRESOURCE(IDD_MATERIAL_OPTIONS), GetActiveWindow(), MaterialOptionsDlgProc, (LPARAM)this))
	{
		return outputMaterialFile (_materials[_index]) ? MS::kSuccess : MS::kFailure;
	}

	return MS::kSuccess;
}

int MTL_Translator::getMaterialId (const MObject &shaderObj)
{
	for (unsigned i = 0; i < _materials.size(); ++i)
	{
		const MayaMaterial &m = _materials[i];
		if (m.mayaObj == shaderObj)
		{
			return i;
		}
	}
	return -1;
}

bool MTL_Translator::getMaterial (const MObject &shaderObj, material *m, std::vector<std::pair<std::string, std::string> > &fileToCopy)
{
	const MFnDependencyNode dstFn (shaderObj);
	MPlug mShader = MFnDependencyNode (shaderObj).findPlug ("surfaceShader");

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
			case MFn::kLambert:
				{
					MFnLambertShader nShader(plugs[0].node());
					float coeff = nShader.diffuseCoeff ();
					MColor diffuse = nShader.color() * coeff;
					MColor ambient = nShader.ambientColor ();
					MColor emissive = nShader.incandescence ();
					m->ambientColor.set (ambient.r, ambient.g, ambient.b);
					m->diffuseColor.set (diffuse.r, diffuse.g, diffuse.b);
					m->emissiveColor.set (emissive.r, emissive.g, emissive.b);
					m->transparency = 1.f - nShader.transparency ().r;
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
					float coeff = nShader.diffuseCoeff ();
					MColor diffuse = nShader.color() * coeff;
					MColor ambient = nShader.ambientColor ();
					MColor emissive = nShader.incandescence();
					m->ambientColor.set (ambient.r, ambient.g, ambient.b);
					m->diffuseColor.set (diffuse.r, diffuse.g, diffuse.b);
					m->emissiveColor.set (emissive.r, emissive.g, emissive.b);
					m->transparency = 1.f - nShader.transparency ().r;
					m->shininess = nShader.eccentricity () < 0.03125f ? 128.f : 4.f / nShader.eccentricity ();
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
			case MFn::kPhong:
				{
					MFnPhongShader nShader(plugs[0].node ());
					float coeff = nShader.diffuseCoeff ();
					MColor diffuse = nShader.color() * coeff;
					MColor ambient = nShader.ambientColor();
					MColor emissive = nShader.incandescence();
					m->ambientColor.set (ambient.r, ambient.g, ambient.b);
					m->diffuseColor.set (diffuse.r, diffuse.g, diffuse.b);
					m->emissiveColor.set (emissive.r, emissive.g, emissive.b);
					m->transparency = 1.f - nShader.transparency ().r;
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
				}
			}

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
					if (!translateFileName (fileName.c_str(), _outFileName, vfsFileName, physicFileName, _shareTexture))
					{
						return false;
					}
					if (_stricmp (fileName.c_str(), physicFileName.c_str()))
					{
						fileToCopy.push_back (std::pair<std::string, std::string>(fileName, physicFileName));
					}
					m->diffuseMap1 = vfsFileName;
				}
			}
			else
			{
				m->diffuseMap1 = "";
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
					if (m->transparency == 1.f)
					{
						m->transparency = 0.f;
					}

					std::string fileName = identifyFileName (textureFileName.asChar());
					std::string vfsFileName, physicFileName;
					if (!translateFileName (fileName.c_str(), _outFileName, vfsFileName, physicFileName, _shareTexture))
					{
						return false;
					}

					if (m->diffuseMap1 != vfsFileName)
					{
						if (_stricmp (fileName.c_str(), physicFileName.c_str()))
						{
							fileToCopy.push_back (std::pair<std::string, std::string>(fileName, physicFileName));
						}
						m->diffuseMap2 = vfsFileName;
					}
				}

			}
			else
			{
				m->diffuseMap2 = "";
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
					if (!translateFileName (fileName.c_str(), _outFileName, vfsFileName, physicFileName, _shareTexture))
					{
						return false;
					}
					if (_stricmp (fileName.c_str(), physicFileName.c_str()))
					{
						fileToCopy.push_back (std::pair<std::string, std::string>(fileName, physicFileName));
					}
					m->normalMap = vfsFileName;
				}
			}
			else
			{
				m->normalMap = "";
			}
		}
	}

	if (!initialized)
	{
		m->alphaTest = false;
		m->alpharef = 0.f;
		m->ambientColor.set (0.2f, 0.2f, 0.2f);
		m->diffuseChannel1 = -1;
		m->diffuseChannel2 = -1;
		m->diffuseColor.set (1.f, 1.f, 1.f);
		m->diffuseMap1 = "";
		m->diffuseMap2 = "";
		m->normalMap = "";
		m->doubleSide = false;
		m->normalmapChannel = -1;
		m->shininess = 0;
		m->emissiveColor.set (0.f, 0.f, 0.f);
		m->transparency = 1.f;
	}

	return true;
}

bool MTL_Translator::translateFileName (const std::string &fileName, const std::string &savepath, std::string &vfsFileName, std::string &physicFileName, bool share) const
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

static const char *materialTemplate =
"<material class=\"default\" >\n"
"	<flags value=\"%d\" />\n"
"	<diffusecolor value=\"%f,%f,%f,%f\" />\n"
"	<specularcolor value=\"%f,%f,%f,%f\" />\n"
"	<ambientcolor value=\"%f,%f,%f,%f\" />\n"
"	<blendcolor value=\"%f,%f,%f,%f\" />\n"
"	<alpharef value=\"%f\" />\n"
"	<shininess value=\"%f\" />\n"
"	<diffusemap1 value=\"%s\" />\n"
"	<diffusemap2 value=\"%s\" />\n"
"	<normalmap value=\"%s\" />\n"
"</material>\n";

// copy from <def_material.h>
enum
{
	CONSTANT_ALPHA		= (1<<0),
	SMOOTH_ALPHA		= (1<<1),
	ALPHA_TEST			= (1<<2),
	TWOSIDED			= (1<<3),
	CONSTANT_COLOR		= (1<<4),
	SMOOTH_COLOR		= (1<<5),
	INTERPOLATE_COLOR	= (1<<6),
	TEXTURE0			= (1<<7),
	NORMALMAP			= (1<<8),
	LIGHTING			= (1<<9),
	FOG					= (1<<10)
};

bool MTL_Translator::outputMaterialFile (const MayaMaterial &m) const
{
	char *buffer = new char[16 * 1024];
	unsigned flags = 0;
	if (!m.mat.diffuseMap1.empty()) flags |= TEXTURE0;

	if (!m.mat.normalMap.empty()) flags |= NORMALMAP;

	if (m.mat.transparency < 1.f) flags |= SMOOTH_ALPHA;

	if (m.mat.alphaTest) flags |= ALPHA_TEST;

	switch (_colormode)
	{
	case MaterialColor:
		flags |= CONSTANT_COLOR;
		break;
	case VertexColor:
		flags |= SMOOTH_COLOR;
		break;
	case TextureColor:
		break;
	case InterpolateColor:
		flags |= CONSTANT_COLOR;
		flags |= INTERPOLATE_COLOR;
		break;
	default:
		flags |= CONSTANT_COLOR;
		break;
	}
	if (_lighting) flags |= LIGHTING;
	if (_doubleside) flags |= TWOSIDED;

	sprintf (buffer, materialTemplate, 
		flags, 
		m.mat.diffuseColor.x,
		m.mat.diffuseColor.y,
		m.mat.diffuseColor.z,
		1.f,
		m.mat.emissiveColor.x,
		m.mat.emissiveColor.y,
		m.mat.emissiveColor.z,
		1.f,
		m.mat.ambientColor.x,
		m.mat.ambientColor.y,
		m.mat.ambientColor.z,
		1.f,
		m.mat.transparency,
		m.mat.transparency,
		m.mat.transparency,
		1.f,
		m.mat.alpharef,
		m.mat.shininess,
		m.mat.diffuseMap1.c_str(),
		m.mat.diffuseMap2.c_str(),
		m.mat.normalMap.c_str());

	FILE *fp = fopen (_outFileName.c_str(), "wt");
	if (!fp)
	{
		return false;
	}

	fwrite (buffer, 1, strlen(buffer), fp);
	fclose (fp);
	delete [] buffer;

	for (unsigned i = 0; i < m.fileToCopy.size(); ++i)
	{
		if (!::CopyFileA (m.fileToCopy[i].first.c_str(), m.fileToCopy[i].second.c_str(), FALSE))
		{
			return false;
		}
	}

	return true;
}

bool MTL_Translator::getOptDoubleSide (void) const
{
	return _doubleside;
}

bool MTL_Translator::getOptShareTexture (void) const
{
	return _shareTexture;
}

bool MTL_Translator::getOptLighting (void) const
{
	return _lighting;
}

MTL_Translator::ColorMode MTL_Translator::getColorMode (void) const
{
	return _colormode;
}

void MTL_Translator::setOptDoubleSide (bool b)
{
	_doubleside = b;
}

void MTL_Translator::setOptShareTexture (bool b)
{
	_shareTexture = b;
}

void MTL_Translator::setOptLighting (bool b)
{
	_lighting = b;
}

void MTL_Translator::setColorMode (ColorMode colormode)
{
	_colormode = colormode;
}

const std::vector<MTL_Translator::MayaMaterial> &MTL_Translator::getMaterialList(void) const
{
	return _materials;
}

void MTL_Translator::setExportIndex (int index)
{
	_index = index;
}

