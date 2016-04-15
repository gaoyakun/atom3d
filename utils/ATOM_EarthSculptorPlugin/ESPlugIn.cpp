#include <ATOM_kernel.h>
#include <ATOM_vfs.h>
#include <ATOM_dbghlp.h>
#include <ATOM_image.h>

#include "ESCore.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
  switch (ul_reason_for_call)
  {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
}

static bool IsProjectDirectory (const char *path)
{
	ATOM_PhysicVFS vfs;
	char buffer[ATOM_VFS::max_filename_length];
	vfs.identifyPath (path, buffer);
	if (!vfs.isDir (buffer))
	{
		return false;
	}

	if (buffer[strlen(buffer)-1] != '\\')
		strcat (buffer, "\\");
	strcat (buffer, ".astudio");
	return vfs.isDir (buffer);
};

static ATOM_STRING getFileNameWithoutExt (const char *filename)
{
	char buffer[ATOM_VFS::max_filename_length];
	ATOM_PhysicVFS vfs;
	vfs.identifyPath (filename, buffer);
	char *fn = strrchr (buffer, '\\')+1;
	char *dot = strrchr (buffer, '.');
	return ATOM_STRING (fn, dot-fn);
}

static ATOM_STRING replaceFileName (const char *filename, const char *name, const char *ext)
{
	char buffer[ATOM_VFS::max_filename_length];
	ATOM_PhysicVFS vfs;
	vfs.identifyPath (filename, buffer);
	char *fn = strrchr (buffer, '\\')+1;
	strcpy (fn, name);
	if (ext)
	{
		strcat (fn, ext);
	}
	else
	{
		const char *newExt = strrchr(filename, '.');
		if (newExt)
		{
			strcat (fn, newExt);
		}
	}

	return buffer;
}

static ATOM_STRING convertPhysToVFS (const char *phys, const char *proj)
{
	char buffer_phys[ATOM_VFS::max_filename_length];
	char buffer_proj[ATOM_VFS::max_filename_length];
	char result[ATOM_VFS::max_filename_length];

	ATOM_PhysicVFS vfs;
	vfs.identifyPath (phys, buffer_phys);
	vfs.identifyPath (proj, buffer_proj);

	unsigned offset = strlen(buffer_proj);
	if (buffer_proj[offset-1] == '\\')
	{
		offset--;
	}

	strcpy (result, buffer_phys + offset);
	for (unsigned i = 0; i < strlen(result); ++i)
	{
		if (result[i] == '\\')
			result[i] = '/';
	}

	return result;
}

static ATOM_STRING findProjectDirectory (const char *file)
{
	ATOM_PhysicVFS vfs;
	char buffer[ATOM_VFS::max_filename_length];
	vfs.identifyPath (file, buffer);

	for(;;)
	{
		if (IsProjectDirectory (buffer))
		{
			return buffer;
		}
		char *p = strrchr (buffer, '\\');
		if (!p)
		{
			return "";
		}
		*p = '\0';
	}
}

static float g_Scale = 10.f;

//called on Earthsculptor startup (intialize your plugin here)
bool Init(ESPlugIn& out_plugIn)
{
  out_plugIn.m_type = PlugIn_Export; //specify plugin type
  strcpy(out_plugIn.m_sName,"ATOM3D exporter"); //max 256 chars
  memcpy(out_plugIn.m_fileDescription, "ATOM3D terrain (.xml)\0*.xml;\0\0",30);//strcpy doens't work, because of multiple \0 (null) chars

  //define the UI
  out_plugIn.m_numControls = 0;
  out_plugIn.m_bUseUI = true; //create a window and UI
  out_plugIn.m_controls[out_plugIn.m_numControls] = new UIControl("缩放",UI_FLOAT,&g_Scale);
  out_plugIn.m_controls[out_plugIn.m_numControls]->m_minValue = 1.f;
  out_plugIn.m_controls[out_plugIn.m_numControls]->m_maxValue = 50.f;
  out_plugIn.m_numControls++;

  out_plugIn.m_bStickyUI = false; //plugin window closes after run is executed
  out_plugIn.m_dataRequest = DATA_REQUEST_DETAILMAP | DATA_REQUEST_COLORMAP;

  return true;
}


static void formatString(const char* format, char *buffer, unsigned bufferSize, va_list args) 
{
	vsnprintf(buffer, bufferSize, format, args);
}

static void alert_err (const char *format,...)
{
	va_list args;
	va_start(args, format);
	char buffer[2048];
	formatString(format, buffer, 2048, args);
	va_end(args);

	::MessageBoxA (::GetActiveWindow(), buffer, "错误", MB_OK|MB_ICONHAND);
}


static void alert_warning (const char *format,...)
{
	va_list args;
	va_start(args, format);
	char buffer[2048];
	formatString(format, buffer, 2048, args);
	va_end(args);

	::MessageBoxA (::GetActiveWindow(), buffer, "警告", MB_OK|MB_ICONWARNING);
}

static bool alert_ask (const char *format,...)
{
	va_list args;
	va_start(args, format);
	char buffer[2048];
	formatString(format, buffer, 2048, args);
	va_end(args);
	return ::MessageBoxA (::GetActiveWindow(), buffer, "错误", MB_YESNO|MB_ICONQUESTION) == IDYES;
}


static bool saveGlobalTexture (ESPlugIn& in_plugIn, const char *filename, int imageSize)
{
	//allocated large texture
	Image outputImage;
	MapData* pMap = in_plugIn.m_mapData;
	int numDetailTextures = pMap->m_numDetailTextures;

	BYTE* pImageData = new BYTE[imageSize*imageSize*3];
	GetTerrainTextures(pImageData, imageSize, true, true, false);

	BYTE* pCorrectedData = new BYTE[imageSize*imageSize*3];
	for (unsigned i = 0; i < imageSize; ++i)
	{
		BYTE *src = pImageData + i * imageSize * 3;
		BYTE *dst = pCorrectedData + i * imageSize * 3 + imageSize * 3 - 3;
		for (unsigned j = 0; j < imageSize; ++j)
		{
			dst[0] = src[0];
			dst[1] = src[1];
			dst[2] = src[2];
			dst -= 3;
			src += 3;
		}
	}

	char fn[ATOM_VFS::max_filename_length];
	strcpy (fn, filename);
	outputImage.Set(pCorrectedData,imageSize,imageSize,3,fn);
	bool result = outputImage.Save();
	delete [] pImageData;
	delete [] pCorrectedData;
	outputImage.m_pData = NULL;

	return result;
}

static bool isDetailUsed (ESPlugIn& in_plugIn, int detail)
{
	if (detail >= in_plugIn.m_mapData->m_numDetailTextures)
		return false;

	BYTE *detailMask = detail < 4 ? in_plugIn.m_mapData->m_pDetailMap : in_plugIn.m_mapData->m_pDetailMap2;
	for (unsigned i = 0; i < in_plugIn.m_mapData->m_detailMapSize * in_plugIn.m_mapData->m_detailMapSize * 4; i+=4)
	{
		if (detailMask[i+(detail%4)] != 0)
			return true;
	}

	return false;
}

//run the plugin (after UI, export/import file dialogs)
bool Run(ESPlugIn& in_plugIn)
{ 
	char buffer[ATOM_VFS::max_filename_length];
	ATOM_PhysicVFS vfs;
	vfs.identifyPath (in_plugIn.m_filePath, buffer);

	ATOM_STRING projDir = findProjectDirectory (buffer);
	if (projDir.empty ())
	{
		alert_err ("所选的文件不在已知的资源库中.");
		return false;
	}

	MapData *mapData = in_plugIn.m_mapData;
	int size = mapData->m_heightMapSize;

	ATOM_TiXmlDocument doc(in_plugIn.m_filePath);
	doc.InsertEndChild (ATOM_TiXmlDeclaration ("1.0", "gb2312", ""));

	ATOM_TiXmlElement eTerrain("Terrain");

	// height map
	{
		ATOM_STRING heightMapFileName = replaceFileName (in_plugIn.m_filePath, (getFileNameWithoutExt(in_plugIn.m_filePath)+"_hm").c_str(), ".raw");
		FILE *fp = fopen (heightMapFileName.c_str(), "wb");
		if (!fp)
		{
			alert_err ("创建文件<%s>失败！", heightMapFileName.c_str());
			return false;
		}
		float *heights = mapData->m_pHeightMap;
		unsigned short *rawHeights = ATOM_NEW_ARRAY(unsigned short, mapData->m_heightMapSize * mapData->m_heightMapSize);
		unsigned short *rawSave = rawHeights;
		for (unsigned y = 0; y < mapData->m_heightMapSize; ++y)
		{
			unsigned short *dst = rawHeights + y * mapData->m_heightMapSize + mapData->m_heightMapSize - 1;
			float *src = heights + y * mapData->m_heightMapSize;
			for (unsigned x = 0; x < mapData->m_heightMapSize; ++x)
			{
				*dst = (*src) * g_Scale;
				dst--;
				src++;
			}
		}

		unsigned cb = mapData->m_heightMapSize * mapData->m_heightMapSize * sizeof(unsigned short);
		unsigned written = fwrite (rawSave, 1, cb, fp);
		fclose (fp);
		ATOM_DELETE_ARRAY(rawSave);

		if (written != cb)
		{
			alert_err ("写入文件<%s>失败！", heightMapFileName.c_str());
			return false;
		}

		ATOM_STRING heightMapFileNameV = convertPhysToVFS (heightMapFileName.c_str(), projDir.c_str());
		eTerrain.SetAttribute ("HeightMap", heightMapFileNameV.c_str());
	}

	// global map
	{
		ATOM_STRING colorMapFileName = replaceFileName (in_plugIn.m_filePath, (getFileNameWithoutExt(in_plugIn.m_filePath)+"_cm").c_str(), ".png");
		if (!saveGlobalTexture (in_plugIn, colorMapFileName.c_str(), 2048))
		{
			alert_err ("写入文件<%s>失败！", colorMapFileName.c_str());
			return false;
		}
		ATOM_STRING colorMapFileNameV = convertPhysToVFS (colorMapFileName.c_str(), projDir.c_str());
		eTerrain.SetAttribute ("GlobalTexture", colorMapFileNameV.c_str());
	}

	eTerrain.SetAttribute ("AlphaMapWidth", mapData->m_detailMapSize);
	eTerrain.SetAttribute ("AlphaMapHeight", mapData->m_detailMapSize);
	eTerrain.SetDoubleAttribute("ScaleX", g_Scale);
	eTerrain.SetDoubleAttribute ("ScaleY", 1);
	eTerrain.SetDoubleAttribute ("ScaleZ", g_Scale);
	eTerrain.SetAttribute ("PatchSize", 65);

	// details & alphas
	{
		ATOM_TiXmlElement eDetails("Details");

		unsigned char *alphas[2];
		alphas[0] = 0;
		alphas[1] = 0;
		unsigned actualNumDetails = 0;

		for (unsigned detail = 0; detail < mapData->m_numDetailTextures; ++detail)
		{
			if (isDetailUsed (in_plugIn, detail))
			{
				ATOM_TiXmlElement eDetail("Detail");

				char buffer[ATOM_VFS::max_filename_length];
				sprintf (buffer, "detail%d", detail);
				const char *oldName = mapData->m_pDetailTextures[detail]->m_sName;
				ATOM_STRING newName = replaceFileName (in_plugIn.m_filePath, buffer, strrchr(oldName, '.'));
				if (!::CopyFileA (oldName, newName.c_str(), FALSE))
				{
					alert_err ("拷贝文件<%s>到<%s>失败！", oldName, newName);
					return false;
				}
				ATOM_STRING vName = convertPhysToVFS (newName.c_str(), projDir.c_str());
				eDetail.SetAttribute ("FileName", vName.c_str());
				eDetail.SetAttribute ("ScaleU", mapData->m_heightMapSize / mapData->m_detailTextureScale[detail]);
				eDetail.SetAttribute ("ScaleV", mapData->m_heightMapSize / mapData->m_detailTextureScale[detail]);
				eDetails.InsertEndChild (eDetail);

				if (!alphas[actualNumDetails/4])
				{
					alphas[actualNumDetails/4] = ATOM_NEW_ARRAY(unsigned char, mapData->m_detailMapSize * mapData->m_detailMapSize * 4);
				}
				unsigned char *alphaBuffer = alphas[actualNumDetails/4];

				BYTE *detailMask = detail < 4 ? in_plugIn.m_mapData->m_pDetailMap : in_plugIn.m_mapData->m_pDetailMap2;
				int compSrc = detail % 4;
				int compDst = actualNumDetails % 4;
#if 1
				for (unsigned i = 0; i < mapData->m_detailMapSize; ++i)
				{
					unsigned char *dst = alphaBuffer + i * mapData->m_detailMapSize * 4 + mapData->m_detailMapSize * 4 - 4 + compDst;
					unsigned char *src = detailMask + i * mapData->m_detailMapSize * 4 + compSrc;
					for (unsigned j = 0; j < mapData->m_detailMapSize; ++j)
					{
						*dst = *src;
						dst -= 4;
						src += 4;
					}
				}
#else
				const int compMap[4] = { 0, 1, 2, 3 };
				for (unsigned i = 0; i < in_plugIn.m_mapData->m_detailMapSize * in_plugIn.m_mapData->m_detailMapSize * 4; i+=4)
				{
					alphaBuffer[i+compMap[compDst]] = detailMask[i+compSrc];
				}
#endif
				++actualNumDetails;
			}
		}

		if (actualNumDetails > 0)
		{
			eTerrain.InsertEndChild (eDetails);

			ATOM_STRING alphaFileName[2] = {
				replaceFileName (in_plugIn.m_filePath, "alpha0", ".raw"),
				replaceFileName (in_plugIn.m_filePath, "alpha1", ".raw")
			};

			ATOM_TiXmlElement eAlphas ("Alphas");
			for (unsigned a = 0; a < 2; ++a)
			{
				if (alphas[a])
				{
					FILE *fp = fopen (alphaFileName[a].c_str(), "wb");
					if (!fp)
					{
						ATOM_DELETE_ARRAY(alphas[0]);
						ATOM_DELETE_ARRAY(alphas[1]);
						alert_err ("打开文件%s失败!", alphaFileName[a].c_str());
						return false;
					}
					unsigned cb = mapData->m_detailMapSize * mapData->m_detailMapSize * 4;
					unsigned written = fwrite (alphas[a], 1, cb, fp);
					fclose (fp);

					if (cb != written)
					{
						alert_err ("写入文件%s失败!", alphaFileName[a].c_str());
						ATOM_DELETE_ARRAY(alphas[0]);
						ATOM_DELETE_ARRAY(alphas[1]);
						return false;
					}

					ATOM_TiXmlElement eAlpha("Alpha");
					eAlpha.SetAttribute ("FileName", alphaFileName[a].c_str());
					eAlphas.InsertEndChild (eAlpha);

					ATOM_DELETE_ARRAY(alphas[a]);
					alphas[a] = 0;
				}
			}

			eTerrain.InsertEndChild (eAlphas);
		}
	}

	doc.InsertEndChild (eTerrain);
	
	if (!doc.SaveFile ())
	{
		alert_err ("保存文件<%s>失败!", in_plugIn.m_filePath);
		return false;
	}

	return true;
}

bool Die(ESPlugIn& in_plugIn)
{
	for(int i=0; i<in_plugIn.m_numControls; ++i)
		delete in_plugIn.m_controls[i];

	in_plugIn.m_numControls = 0;

  return true;
}
