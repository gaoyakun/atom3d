#include <ATOM.h>
#include "fwconverter.h"
#include "heightfield_aogen.h"

FWConverter::FWConverter (void)
{
	_alphamapWidth = 512;
	_alphamapHeight = 512;
}

FWConverter::FWConverter (const char *inputFileName, const char *outputFileName, const char *terrainPath, int alphamapWidth, int alphamapHeight, int aomapSize)
{
	setInputFileName (inputFileName);
	setOutputFileName (outputFileName);
	setTerrainPath (terrainPath);
	setAlphaMapWidth (alphamapWidth);
	setAlphaMapHeight (alphamapHeight);
	setAmbientOcclusionMapSize (aomapSize);
}

void FWConverter::setInputFileName (const char *inputFileName)
{
	_inputFileName = inputFileName ? inputFileName : "";
}

void FWConverter::setOutputFileName (const char *outputFileName)
{
	_outputFileName = outputFileName ? outputFileName : "";
}

void FWConverter::setTerrainPath (const char *terrainPath)
{
	_terrainPath = terrainPath;
}

void FWConverter::setAlphaMapWidth (int width)
{
	_alphamapWidth = width;
}

void FWConverter::setAlphaMapHeight (int height)
{
	_alphamapHeight = height;
}

void FWConverter::setAmbientOcclusionMapSize (int size)
{
	_aomapSize = size;
}

bool FWConverter::convert (void)
{
	if (_inputFileName.empty () || _outputFileName.empty ())
	{
		return false;
	}

	TerrainInfo info;
	if (!buildInfo (&info))
	{
		return false;
	}

	return true;
}

ATOM_STRING FWConverter::identifyFileName (const ATOM_STRING &filename) const
{
	char buffer[ATOM_VFS::max_filename_length];
	ATOM_PhysicVFS pfs;
	if (!pfs.identifyPath (filename.c_str(), buffer))
	{
		return "";
	}
	return buffer;
}

ATOM_STRING FWConverter::getDirectoryPart (const ATOM_STRING &filename) const
{
	ATOM_STRING ident = identifyFileName (filename);
	const char *p = strrchr (ident.c_str(), '\\');
	return p ? ident.substr(0, p-ident.c_str()) : "";
}

bool FWConverter::buildInfo (FWConverter::TerrainInfo *info)
{
	doCallback (FWConverter::PS_READING_FREEWORD_FILE, 0);
	TiXmlDocument doc(_inputFileName.c_str());
	if (!doc.LoadFile ())
	{
		setErrorMessage  ("ERR: file <%s> could not be opened.\n", _inputFileName.c_str());
		return false;
	}

	ATOM_STRING dir = getDirectoryPart (_inputFileName);

	TiXmlElement *eRoot = doc.RootElement ();
	if (!eRoot)
	{
		setErrorMessage  ("ERR: <%s> is not a valid FREEWORLD3D configuration file.\n");
		return false;
	}

	TiXmlElement *eTerrain = eRoot->FirstChildElement ("ATOM_Terrain");
	if (!eTerrain)
	{
		setErrorMessage  ("ERR: No TERRAIN element found in root element.\n");
		return false;
	}

	if (!eTerrain->Attribute ("Size", &info->terrainSize))
	{
		setErrorMessage  ("ERR: No SIZE attribute found in TERRAIN element.\n");
		return false;
	}

	if (!ATOM_ispo2 (info->terrainSize - 1))
	{
		setErrorMessage  ("ERR: Invalid terrain size.\n");
		return false;
	}

	double terrainSpacing;
	if (!eTerrain->Attribute ("Step", &terrainSpacing))
	{
		setErrorMessage  ("ERR: No STEP attribute found in TERRAIN element.\n");
		return false;
	}
	info->spacing = terrainSpacing;

	TiXmlElement *eHeightmap = eTerrain->FirstChildElement ("Heightmap");
	if (!eHeightmap)
	{
		setErrorMessage  ("ERR: No HEIGHTMAP element found in TERRAIN element.\n");
		return false;
	}

	const char *format = eHeightmap->Attribute ("Format");
	if (!format || strcmp (format, "16bit RAW"))
	{
		setErrorMessage  ("ERR: Heightmap file must be 16bit RAW format.\n");
		return false;
	}

	double vscale;
	if (!eHeightmap->Attribute ("Scale", &vscale))
	{
		setErrorMessage  ("ERR: No SCALE attribute found in HEIGHTMAP element.\n");
		return false;
	}
	info->vscale = vscale;

	const char *heightmapFileName = eHeightmap->Attribute ("File");
	if (!heightmapFileName)
	{
		setErrorMessage  ("ERR: No FILE attribute found in HEIGHTMAP element.\n");
		return false;
	}
	info->heightmapFileName = identifyFileName(concateFileName(dir.c_str(), heightmapFileName));
	FILE *fp = fopen (info->heightmapFileName.c_str(), "rb");
	if (!fp)
	{
		setErrorMessage  ("ERR: File <%s> could not be opened.\n", info->heightmapFileName.c_str());
		return false;
	}
	fseek (fp, 0, SEEK_END);
	unsigned len = ftell (fp);
	fseek (fp, 0, SEEK_SET);
	if (len != info->terrainSize * info->terrainSize * sizeof(unsigned short))
	{
		setErrorMessage  ("ERR: Heightmap file <%s> has wrong size.\n", info->heightmapFileName.c_str());
		return false;
	}
	ATOM_VECTOR<unsigned short> heightData(info->terrainSize * info->terrainSize);
	fread (&heightData[0], 1, len, fp);
	fclose (fp);
	const char *fileNamePart = strrchr (info->heightmapFileName.c_str(), '\\');
	ATOM_ASSERT(fileNamePart);
	ATOM_STRING vfsPath = concateFileName (_terrainPath.c_str(), fileNamePart+1);
	{
		ATOM_AutoFile f(vfsPath.c_str(), ATOM_VFS::write);
		if (!f)
		{
			setErrorMessage  ("ERR: Could not write file <%s>.\n", vfsPath.c_str());
			return false;
		}
		for(int row = info->terrainSize-1; row >= 0; --row)
		{
			f->write (&heightData[0] + row * info->terrainSize, info->terrainSize * sizeof(unsigned short));
		}
	}
	info->heightmapFileName = vfsPath;

	TiXmlElement *eLayers = eRoot->FirstChildElement ("TerrainLayers");
	if (!eLayers)
	{
		setErrorMessage  ("ERR: No TERRAINLAYERS element found in root element.\n");
		return false;
	}

	int layerCount;
	if (!eLayers->Attribute ("Count", &layerCount))
	{
		setErrorMessage  ("ERR: No COUNT attribute found in TERRAINLAYERS element.\n");
		return false;
	}
	if (layerCount == 0)
	{
		setErrorMessage  ("ERR: Zero terrain layer count found.\n");
		return false;
	}

	info->layers.resize (0);
	TiXmlElement *eLayer = eLayers->FirstChildElement ("TerrainLayer");
	if (!eLayer)
	{
		setErrorMessage  ("ERR: No TERRAINLAYER element found in TERRAINLAYERS element.\n");
		return false;
	}

	while (eLayer)
	{
		info->layers.resize (info->layers.size() + 1);
		TerrainLayerInfo &layerInfo = info->layers.back ();

		TiXmlElement *eTexture = eLayer->FirstChildElement ("Texture");
		if (!eTexture)
		{
			setErrorMessage  ("ERR: No TEXTURE element found in TERRAINLAYER element.\n");
			return false;
		}
		const char *textureFile = eTexture->Attribute ("File");
		if (!textureFile)
		{
			setErrorMessage  ("ERR: No FILE attribute found in TEXTURE element.\n");
			return false;
		}
		layerInfo.detailFileName = identifyFileName(concateFileName(dir.c_str(), textureFile));
		const char *p = strrchr (layerInfo.detailFileName.c_str(), '\\');
		ATOM_ASSERT(p);
		char vfsPath[ATOM_VFS::max_filename_length];
		char nativePath[ATOM_VFS::max_filename_length];
		ATOM_CompletePath (concateFileName(_terrainPath.c_str(), p+1).c_str(), vfsPath);
		ATOM_GetNativePathName (vfsPath, nativePath);
		if (!::CopyFileA (layerInfo.detailFileName.c_str(), nativePath, FALSE))
		{
			setErrorMessage  ("ERR: Copy file <%s> to <%s> failed.\n", layerInfo.detailFileName.c_str(), nativePath);
			return false;
		}
		layerInfo.detailFileName = vfsPath;

		double ScaleU, ScaleV;
		if (!eTexture->Attribute ("ScaleU", &ScaleU) || !eTexture->Attribute ("ScaleV", &ScaleV))
		{
			setErrorMessage  ("ERR: No SCALEU/SCALEV attribute found in TEXTURE element.\n");
			return false;
		}
		layerInfo.scaleU = ScaleU;
		layerInfo.scaleV = ScaleV;

		TiXmlElement *eAlpha = eLayer->FirstChildElement ("AlphaMap");
		if (!eAlpha && info->layers.size() != 1)
		{
			setErrorMessage  ("ERR: No ALPHAMAP element found in TERRAINLAYER element.\n");
			return false;
		}
		if (eAlpha)
		{
			const char *alphaFile = eAlpha->Attribute ("File");
			if (!alphaFile)
			{
				setErrorMessage  ("ERR: No FILE attribute found in ALPHAMAP element.\n");
				return false;
			}
			layerInfo.alphaFileName = identifyFileName (concateFileName(dir.c_str(), alphaFile));
			layerInfo.alphaChannel = 0;

			if (!eAlpha->Attribute ("Width", &layerInfo.alphamapWidth))
			{
				setErrorMessage  ("ERR: No WIDTH attribute found in ALPHAMAP element.\n");
				return false;
			}

			if (!eAlpha->Attribute ("Height", &layerInfo.alphamapHeight))
			{
				setErrorMessage  ("ERR: No HEIGHT attribute found in ALPHAMAP element.\n");
				return false;
			}
		}

		eLayer = eLayer->NextSiblingElement ("TerrainLayer");
	}

	if (info->layers.size() != layerCount)
	{
		setErrorMessage  ("ERR: ATOM_Terrain layer count is %d, bug %d terrain layers found.\n", layerCount, info->layers.size());
		return false;
	}

	TiXmlElement *eGrass = eRoot->FirstChildElement ("Vegetation");
	if (eGrass)
	{
		int vegetationCount;
		if (!eGrass->Attribute ("count", &vegetationCount))
		{
			setErrorMessage  ("ERR: No count attribute found in VEGETATION element.\n");
			return false;
		}

		if (vegetationCount > 0)
		{
			TiXmlElement *grassLayer = eGrass->FirstChildElement ("Layer");
			if (!grassLayer)
			{
				setErrorMessage  ("ERR: No LAYER element found in VEGETATION element.\n");
				return false;
			}

			info->grasses.resize (0);
			while (grassLayer)
			{
				info->grasses.resize (info->grasses.size()+1);
				TerrainGrassInfo &grassInfo = info->grasses.back ();

				double w, h;
				if (!grassLayer->Attribute ("Width", &w))
				{
					setErrorMessage  ("ERR: No WIDTH element found in LAYER element.\n");
					return false;
				}
				if (!grassLayer->Attribute ("Height", &h))
				{
					setErrorMessage  ("ERR: No HEIGHT element found in LAYER element.\n");
					return false;
				}

				grassInfo.width = w;
				grassInfo.height = h;

				const char *grassTexture = grassLayer->Attribute ("Texture");
				if (!grassTexture)
				{
					setErrorMessage  ("ERR: No TEXTURE attribute found in LAYER element.\n");
					return false;
				}
				grassInfo.grassTexture = identifyFileName (concateFileName(dir.c_str(), grassTexture));
				const char *fileNamePart = strrchr (grassInfo.grassTexture.c_str(), '\\');
				ATOM_ASSERT(fileNamePart);
				ATOM_STRING vfsPath = concateFileName (_terrainPath.c_str(), fileNamePart+1);
				char nativeGrassTextureFileName[ATOM_VFS::max_filename_length];
				ATOM_GetNativePathName (vfsPath.c_str(), nativeGrassTextureFileName);
				if (!::CopyFileA (grassInfo.grassTexture.c_str(), nativeGrassTextureFileName, false))
				{
					setErrorMessage  ("ERR: Could not copy file <%s> to <%s>.\n", grassInfo.grassTexture.c_str(), nativeGrassTextureFileName);
					return false;
				}
				grassInfo.grassTexture = vfsPath;

				const char *densityFile = grassLayer->Attribute ("DataFile");
				if (!densityFile)
				{
					setErrorMessage  ("ERR: No DATAFILE attribute found in LAYER element.\n");
					return false;
				}
				ATOM_STRING grassDensity = identifyFileName (concateFileName(dir.c_str(), densityFile));
				FILE *fp = fopen (grassDensity.c_str(), "rb");
				if (!fp)
				{
					setErrorMessage  ("ERR: File <%s> could not be opened.\n", grassDensity.c_str());
					return false;
				}
				int mapW, mapH;
				fread (&mapW, sizeof(int), 1, fp);
				fread (&mapH, sizeof(int), 1, fp);
				grassInfo.grassPoints.resize (0);
				for (int i = 0; i < mapH; ++i)
					for (int j = 0; j < mapW; ++j)
					{
						int density;
						fread (&density, sizeof(int), 1, fp);
						for (int d = 0; d < density; ++d)
						{
							ATOM_Vector3f v;
							fread (&v, sizeof(float), 3, fp);
							grassInfo.grassPoints.push_back (v);
						}
					}

				grassLayer = grassLayer->NextSiblingElement ("Layer");
			}

			if (info->grasses.size() != vegetationCount)
			{
				setErrorMessage  ("ERR: Grass layer count is %d, but %d grass layers found.\n", vegetationCount, info->grasses.size());
				return false;
			}
		}
	}

	MeshCache meshCache;
	SceneMesh sceneMeshes;

	TiXmlElement *eMeshCache= eRoot->FirstChildElement ("CachedMeshGroups");
	if (eMeshCache)
	{
		int meshGroupCount;
		if (eMeshCache->Attribute ("Count", &meshGroupCount) && meshGroupCount > 0)
		{
			TiXmlElement *eMeshGroup = eMeshCache->FirstChildElement ("CachedMeshGroup");
			while (eMeshGroup)
			{
				TiXmlElement *eMesh = eMeshGroup->FirstChildElement ("ATOM_Mesh");
				while (eMesh)
				{
					const char *meshFileName = eMesh->Attribute ("Filename");
					int meshId;
					if (meshFileName && eMesh->Attribute ("Id", &meshId))
					{
						meshCache.objMeshNames[meshId] = meshFileName;
					}
					eMesh = eMesh->NextSiblingElement ("ATOM_Mesh");
				}
				eMeshGroup = eMeshGroup->NextSiblingElement ("CachedMeshGroup");
			}
		}

		if (!meshCache.objMeshNames.empty ())
		{
			TiXmlElement *eSceneLayers = eRoot->FirstChildElement ("SceneLayers");
			if (eSceneLayers)
			{
				TiXmlElement *eSceneLayer = eSceneLayers->FirstChildElement ("SceneLayer");
				while (eSceneLayer)
				{
					TiXmlElement *eSceneMesh = eSceneLayer->FirstChildElement ("SceneMesh");
					while (eSceneMesh)
					{
						const char *matrix = eSceneMesh->Attribute ("Matrix");
						int meshId;
						if (matrix && eSceneMesh->Attribute ("CachedMeshId", &meshId))
						{
							float m[16];
							if (16 == sscanf (matrix, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", 
								&m[0],&m[1],&m[2],&m[3],&m[4],&m[5],&m[6],&m[7],&m[8],&m[9],&m[10],&m[11],
								&m[12],&m[13],&m[14],&m[15]))
							{
								ATOM_Matrix4x4f mat(m[0],m[1],m[2],m[3],m[4],m[5],m[6],m[7],m[8],m[9],m[10],m[11],m[12],m[13],m[14],m[15]);
								ATOM_MAP<int, ATOM_STRING>::const_iterator it = meshCache.objMeshNames.find (meshId);
								if (it != meshCache.objMeshNames.end ())
								{
									sceneMeshes.sceneMeshes.push_back (std::pair<ATOM_STRING, ATOM_Matrix4x4f>(it->second, mat));
								}
							}
						}
						eSceneMesh = eSceneMesh->NextSiblingElement ("SceneMesh");
					}
					eSceneLayer = eSceneLayer->NextSiblingElement ("SceneLayer");
				}
			}
		}
	}
	doCallback (FWConverter::PS_READING_FREEWORD_FILE, 100);

	doCallback (FWConverter::PS_BUILDING_ALPHA_MAPS, 0);
	if (!buildAlphaMaps (info, _terrainPath.c_str()))
	{
		return false;
	}
	doCallback (FWConverter::PS_BUILDING_ALPHA_MAPS, 100);

	doCallback (FWConverter::PS_BUILDING_GLOBAL_MAP, 0);
	if (!buildGlobalMap (info, _terrainPath.c_str(), 2048))
	{
		return false;
	}
	doCallback (FWConverter::PS_BUILDING_GLOBAL_MAP, 100);

	doCallback (FWConverter::PS_COLLAPSING_ALPHA_MAPS, 0);
	if (!collapseAlphaMaps (info, _terrainPath.c_str()))
	{
		return false;
	}
	doCallback (FWConverter::PS_COLLAPSING_ALPHA_MAPS, 100);

	doCallback (FWConverter::PS_BUILDING_GRASS, 0);
	if (!buildGrassDataFiles (info, _terrainPath.c_str()))
	{
		return false;
	}
	doCallback (FWConverter::PS_BUILDING_GRASS, 100);

	doCallback (FWConverter::PS_WRITING_TERRAIN_FILE_1, 0);
	if (!writeTerrainFile (info, genTerrainFileName (_terrainPath.c_str()).c_str(), false))
	{
		return false;
	}
	doCallback (FWConverter::PS_WRITING_TERRAIN_FILE_1, 100);

	doCallback (FWConverter::PS_WRITING_SCENE_FILE, 0);
	if (!writeSceneFile (info, &sceneMeshes))
	{
		return false;
	}
	doCallback (FWConverter::PS_WRITING_SCENE_FILE, 100);

	doCallback (FWConverter::PS_BUILDING_AO_MAP, 0);
	if (!buildTerrainAOMap (info, _terrainPath.c_str()))
	{
		return false;
	}
	doCallback (FWConverter::PS_BUILDING_AO_MAP, 100);

	doCallback (FWConverter::PS_WRITING_TERRAIN_FILE_2, 0);
	if (!writeTerrainFile (info, genTerrainFileName (_terrainPath.c_str()).c_str(), true))
	{
		return false;
	}
	doCallback (FWConverter::PS_WRITING_TERRAIN_FILE_2, 100);

	doCallback (FWConverter::PS_FINISHED, 0);

	return true;
}

ATOM_STRING FWConverter::genAlphaMapFileName (const char *dir, int index) const
{
	ATOM_ASSERT(dir && dir[0]);
	char buffer[32];
	sprintf (buffer, "alpha%d.raw", index);
	return concateFileName (dir, buffer);
}

ATOM_STRING FWConverter::genGrassDataFileName (const char *dir, int index) const
{
	ATOM_ASSERT(dir && dir[0]);
	char buffer[32];
	sprintf (buffer, "vegetation%d.dat", index);
	return concateFileName (dir, buffer);
}

bool FWConverter::buildAlphaMaps (TerrainInfo *info, const char *dir)
{
	if (info->layers.size() == 1)
	{
		return true;
	}

	ATOM_VECTOR<ATOM_BaseImage> originAlphas(info->layers.size());
	ATOM_VECTOR<ATOM_BaseImage> newAlphas(info->layers.size());
	
	for (unsigned i = 0; i < info->layers.size(); ++i)
	{
		ATOM_BaseImage &image = originAlphas[i];

		if (i != 0)
		{
			TerrainLayerInfo &layerInfo = info->layers[i];

			const char *fileName = layerInfo.alphaFileName.c_str();
			FILE *fp = fopen (fileName, "rb");
			if (!fp)
			{
				setErrorMessage  ("ERR: File <%s> could not be opened.\n", fileName);
				return false;
			}
			fseek (fp, 0, SEEK_END);
			unsigned fileSize = ftell (fp);
			if (fileSize != layerInfo.alphamapWidth * layerInfo.alphamapHeight)
			{
				setErrorMessage  ("ERR: Alpha map <%s> should be 8bit RAW format.\n");
				return false;
			}
			fseek (fp, 0, SEEK_SET);

			image.init (layerInfo.alphamapWidth, layerInfo.alphamapHeight, ATOM_PIXEL_FORMAT_GREY8, 0, 1);
			fread (image.getData(), 1, fileSize, fp);
			fclose (fp);

			if (layerInfo.alphamapWidth != _alphamapWidth || layerInfo.alphamapHeight != _alphamapHeight)
			{
				image.resize (_alphamapWidth, _alphamapHeight);
			}
		}
		else
		{
			image.init (_alphamapWidth, _alphamapHeight, ATOM_PIXEL_FORMAT_GREY8, 0, 1);
			memset (image.getData(), 0xFF, _alphamapWidth * _alphamapHeight);
		}

		if (i == info->layers.size()-1 && i != 0)
		{
			newAlphas[i].init (_alphamapWidth, _alphamapHeight, ATOM_PIXEL_FORMAT_GREY8, image.getData(), 1);
		}
		else
		{
			newAlphas[i].init (_alphamapWidth, _alphamapHeight, ATOM_PIXEL_FORMAT_GREY8, 0, 1);
		}
	}

	for (unsigned i = 0; i < newAlphas.size(); ++i)
	{
		if (i != newAlphas.size()-1)
		{
			unsigned char *data = newAlphas[i].getData();

			for (unsigned j = 0; j < _alphamapWidth * _alphamapHeight; ++j)
			{
				float a = originAlphas[i].getData()[j] / 255.f;
				for (unsigned k = i+1; k < newAlphas.size(); ++k)
				{
					a *= (1.f - originAlphas[k].getData()[j] / 255.f);
				}
				*data++ = ATOM_ftol (a * 255.f);
			}
		}

		ATOM_STRING alphamapFileName = genAlphaMapFileName (dir, i);
		ATOM_AutoFile f(alphamapFileName.c_str(), ATOM_VFS::write);
		if (!f)
		{
			setErrorMessage  ("ERR: could not open file <%s> for write.\n", alphamapFileName);
			return false;
		}
		f->write (newAlphas[i].getData(), _alphamapWidth * _alphamapHeight);
		info->layers[i].alphaFileName = alphamapFileName;
		info->layers[i].alphaChannel = 0;
	}

	return true;
}

bool FWConverter::buildGrassDataFiles (TerrainInfo *info, const char *dir)
{
	for (unsigned i = 0; i < info->grasses.size(); ++i)
	{
		ATOM_STRING filename = genGrassDataFileName (dir, i);
		ATOM_AutoFile f(filename.c_str(), ATOM_VFS::write);
		if (!f)
		{
			setErrorMessage  ("ERR: Could not open file <%s> for write.\n", filename.c_str());
			return false;
		}

		float size = (info->terrainSize-1) * info->spacing;
		int numPoints = info->grasses[i].grassPoints.size();
		f->write (&numPoints, sizeof(int));

		if (numPoints > 0)
		{
			ATOM_VECTOR<ATOM_Vector3f> &v = info->grasses[i].grassPoints;
			for (unsigned j = 0; j < numPoints; ++j)
			{
				v[j].z *= -1.f; 
				v[j].z += size;
			}

			f->write (&v[0], sizeof(ATOM_Vector3f)*numPoints);
		}

		info->grasses[i].densityDataFile = filename;
	}
	return true;
}

ATOM_STRING FWConverter::concateFileName (const char *part1, const char *part2) const
{
	ATOM_ASSERT(part1 && part1[0] && part2 && part2[0]);
	ATOM_STRING s = part1;
	if (s[s.length()-1] != '/' && s[s.length()-1] != '\\')
	{
		s += '/';
	}
	if (part2[0] == '/' || part2[0] == '\\')
	{
		part2++;
	}
	s += part2;
	return s;
}

bool FWConverter::buildGlobalMap (TerrainInfo *info, const char *dir, unsigned size)
{
	ATOM_BaseImage currentImage(size, size, ATOM_PIXEL_FORMAT_ARGB8888, 0, 1);
	memset (currentImage.getData(), 0, size * size * 4);

	for (unsigned i = 0; i < info->layers.size(); ++i)
	{
		ATOM_BaseImage imageDetail;
		if (!imageDetail.load (ATOM_AutoFile (info->layers[i].detailFileName.c_str(), ATOM_VFS::read), ATOM_PIXEL_FORMAT_ARGB8888))
		{
			setErrorMessage  ("ERR: File <%s> could not be opened.\n", info->layers[i].detailFileName.c_str());
			return false;
		}

		unsigned dw = imageDetail.getWidth();
		unsigned dh = imageDetail.getHeight();
		unsigned w = dw * info->layers[i].scaleU;
		unsigned h = dh * info->layers[i].scaleV;
		if (w > 8192)
		{
			dw = 8192 / info->layers[i].scaleU;
			w = dw * info->layers[i].scaleU;
		}
		if (h > 8192)
		{
			dh = 8192 / info->layers[i].scaleV;
			h = dw * info->layers[i].scaleV;
		}

		if (dw == 0 || dh == 0)
		{
			setErrorMessage  ("ERR: Two many scales of detail file <%s>\n", info->layers[i].detailFileName.c_str());
			return false;
		}

		if (dw != imageDetail.getWidth() || dh != imageDetail.getHeight())
		{
			imageDetail.resize (dw, dh);
		}

		ATOM_BaseImage expandedDetail;

		expandedDetail.init (w, h, ATOM_PIXEL_FORMAT_ARGB8888);
		unsigned r = w / dw;
		for (unsigned j = 0; j < h; ++j)
		{
			unsigned char *dst = expandedDetail.getData() + j * w * 4;
			unsigned char *src = imageDetail.getData() + (j % dh) * dw * 4;
			for (unsigned k = 0; k < r; ++k)
			{
				memcpy (dst, src, dw * 4);
				dst += dw * 4;
			}
			memcpy (dst, src, (w - r * dw) * 4);
		}

		if (w != size || h != size)
		{
			expandedDetail.resize (size, size);
		}

		if (info->layers.size() == 1)
		{
			currentImage = expandedDetail;
			break;
		}

		ATOM_AutoFile falpha(info->layers[i].alphaFileName.c_str(), ATOM_VFS::read);
		if (!falpha)
		{
			setErrorMessage  ("ERR: File <%s> could not be opened.\n", info->layers[i].alphaFileName.c_str());
		}
		ATOM_BaseImage alpha(_alphamapWidth, _alphamapWidth, ATOM_PIXEL_FORMAT_GREY8, 0, 1);
		if (falpha->size() != alpha.getWidth() * alpha.getHeight())
		{
			setErrorMessage  ("ERR: Invalid alpha map file <%s>.\n", info->layers[i].alphaFileName.c_str());
		}
		falpha->read (alpha.getData(), alpha.getWidth() * alpha.getHeight());
		alpha.resize (size, size);

		unsigned char *p1 = currentImage.getData();
		unsigned char *p2 = expandedDetail.getData();
		unsigned char *a = alpha.getData();
		for (unsigned n = 0; n < size * size; ++n)
		{
			for (unsigned m = 0; m < 4; ++m)
			{
				float s1 = (*p1)/255.f;
				float s2 = (*p2)/255.f;
				float t = (*a)/255.f;
				s1 += s2 * t;
				unsigned s = ATOM_ftol (s1 * 255.f);
				if (s > 255) s = 255;
				*p1 = s;

				++p1;
				++p2;
			}

			++a;
		}
	}

	ATOM_STRING globalTextureFileName = concateFileName (dir, "globaltexture.png");
	ATOM_AutoFile f(globalTextureFileName.c_str(), ATOM_VFS::write);
	if (!f)
	{
		setErrorMessage  ("ERR: Could not open file <%s> for write.\n", globalTextureFileName.c_str());
		return false;
	}

	currentImage.flip ();

	if (!currentImage.save (f, ATOM_PIXEL_FORMAT_ARGB8888))
	{
		setErrorMessage  ("ERR: Save image <%s> failed.\n", globalTextureFileName.c_str());
		return false;
	}

	info->globalTextureFileName = globalTextureFileName;

	return true;
}

bool FWConverter::collapseAlphaMaps (TerrainInfo *info, const char *dir)
{
	if (info->layers.size() == 1)
	{
		return true;
	}

	ATOM_VECTOR<ATOM_BaseImage> collapsedAlphaMaps((info->layers.size() + 3) / 4);
	for (unsigned i = 0; i < collapsedAlphaMaps.size(); ++i)
	{
		unsigned n = (i+1) * 4 > info->layers.size() ? info->layers.size() - i * 4 : 4;
		ATOM_VECTOR<ATOM_BaseImage> alphas(n);
		for (unsigned j = 0; j < n; ++j)
		{
			const char *filename = info->layers[i*4+j].alphaFileName.c_str();
			ATOM_AutoFile f(filename, ATOM_VFS::read);
			if (!f)
			{
				setErrorMessage  ("ERR: File <%s> could not be opened.\n", filename);
				return false;
			}
			if (f->size() != _alphamapWidth * _alphamapHeight)
			{
				setErrorMessage  ("ERR: Invalid alpha map file <%s>.\n", filename);
				return false;
			}
			alphas[j].init (_alphamapWidth, _alphamapHeight, ATOM_PIXEL_FORMAT_GREY8);
			f->read (alphas[j].getData(), _alphamapWidth * _alphamapHeight);
		}

		collapsedAlphaMaps[i].init (_alphamapWidth, _alphamapHeight, ATOM_PIXEL_FORMAT_ARGB8888);
		unsigned char *dst = collapsedAlphaMaps[i].getData();
		memset (dst, 0, _alphamapWidth * _alphamapHeight * 4);
		for (unsigned j = 0; j < _alphamapWidth * _alphamapHeight; ++j)
		{
			for (unsigned k = 0; k < n; ++k)
			{
				dst[k] = alphas[k].getData()[j];
			}
			dst += 4;
		}
	}

	for (unsigned i = 0; i < info->layers.size(); ++i)
	{
		ATOM_Remove (info->layers[i].alphaFileName.c_str());
	}

	for (unsigned i = 0; i < collapsedAlphaMaps.size(); ++i)
	{
		collapsedAlphaMaps[i].flip ();

		ATOM_STRING fn = genAlphaMapFileName (_terrainPath.c_str(), i);
		ATOM_AutoFile f(fn.c_str(), ATOM_VFS::write);
		if (!f)
		{
			setErrorMessage  ("ERR: Could not open file <%s> for write.\n", fn.c_str());
			return false;
		}
		f->write (collapsedAlphaMaps[i].getData(), _alphamapWidth * _alphamapHeight * 4);

		unsigned n = i * 4;
		unsigned c = 0;
		while (n < (i+1) * 4 && n < info->layers.size())
		{
			info->layers[n].alphaFileName = fn.c_str();
			info->layers[n].alphaChannel = c++;
			++n;
		}
	}

	return true;
}

bool FWConverter::writeTerrainFile (TerrainInfo *info, const char *filename, bool writeAOMap)
{
	char nativeFileName[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (filename, nativeFileName);
	TiXmlDocument doc(nativeFileName);

	TiXmlDeclaration eDecl("1.0", "gb2312", "");
	doc.InsertEndChild (eDecl);
		;
	TiXmlElement eRoot("ATOM_Terrain");
	eRoot.SetAttribute ("HeightMap", info->heightmapFileName.c_str());
	eRoot.SetAttribute ("GlobalTexture", info->globalTextureFileName.c_str());
	eRoot.SetAttribute ("AlphaMapWidth", _alphamapWidth);
	eRoot.SetAttribute ("AlphaMapHeight", _alphamapHeight);
	if (writeAOMap)
	{
		eRoot.SetAttribute ("AOTexture", genTerrainAOMapFileName(_terrainPath.c_str()).c_str());
	}

	TiXmlElement eDetails("Details");
	for (unsigned i = 0; i < info->layers.size(); ++i)
	{
		TiXmlElement eDetail("Detail");
		eDetail.SetAttribute ("FileName", info->layers[i].detailFileName.c_str());
		eDetail.SetDoubleAttribute ("ScaleU", info->layers[i].scaleU);
		eDetail.SetDoubleAttribute ("ScaleV", -info->layers[i].scaleV);
		eDetails.InsertEndChild (eDetail);
	}
	eRoot.InsertEndChild (eDetails);

	if (info->layers.size() > 1)
	{
		TiXmlElement eAlphas("Alphas");
		for (unsigned i = 0; i < (info->layers.size() + 3) / 4; ++i)
		{
			TiXmlElement eAlpha ("Alpha");
			eAlpha.SetAttribute ("FileName", info->layers[i*4].alphaFileName.c_str());
			eAlphas.InsertEndChild (eAlpha);
		}
		eRoot.InsertEndChild (eAlphas);
	}

	eRoot.SetDoubleAttribute ("ScaleX", info->spacing);
	eRoot.SetDoubleAttribute ("ScaleY", info->vscale);
	eRoot.SetDoubleAttribute ("ScaleZ", info->spacing);
	eRoot.SetAttribute ("PatchSize", 65);

	if (info->grasses.size() > 0)
	{
		TiXmlElement eGrasses("Vegetations");
		for (unsigned i = 0; i < info->grasses.size(); ++i)
		{
			TiXmlElement eGrass ("Vegetation");
			eGrass.SetDoubleAttribute ("Width", info->grasses[i].width * 2.f);
			eGrass.SetDoubleAttribute ("Height", info->grasses[i].height * 2.f);
			eGrass.SetAttribute ("DensityFileName", info->grasses[i].densityDataFile.c_str());
			eGrass.SetAttribute ("TextureFileName", info->grasses[i].grassTexture.c_str());
			eGrasses.InsertEndChild (eGrass);
		}
		eRoot.InsertEndChild (eGrasses);
	}

	doc.InsertEndChild (eRoot);

	return doc.SaveFile ();
}

ATOM_STRING FWConverter::genTerrainFileName (const char *dir) const
{
	return concateFileName (dir, "terrain.xml");
}

bool FWConverter::writeSceneFile (TerrainInfo *info, SceneMesh *meshes)
{

	ATOM_HARDREF(ATOM_Node) rootNode;
	ATOM_HARDREF(ATOM_NodeOctree) octreeNode;
	rootNode->appendChild (octreeNode.get());

	ATOM_HARDREF(ATOM_Terrain) terrainNode;
	ATOM_STRING terrainFileName = genTerrainFileName (_terrainPath.c_str());
	terrainNode->setTerrainFileName (terrainFileName);
	if (!terrainNode->load (ATOM_GetRenderDevice ()))
	{
		setErrorMessage  ("ERR: Load terrain failed.\n");
		return false;
	}
	octreeNode->appendChild (terrainNode.get());

	for (unsigned i = 0; i < meshes->sceneMeshes.size(); ++i)
	{
		ATOM_STRING meshFileName = meshes->sceneMeshes[i].first;
		if (meshFileName.length() > 4 && !_stricmp(meshFileName.c_str()+meshFileName.length()-4, ".obj"))
		{
			meshFileName = meshFileName.substr(0, meshFileName.length()-4) + ".nm";

			char buffer[ATOM_VFS::max_filename_length];
			ATOM_GetNativePathName ("/", buffer);
			ATOM_STRING identRootPath = identifyFileName (buffer);
			ATOM_STRING identFilePath = identifyFileName (meshFileName);
			if (identRootPath.length() >= identFilePath.length() || _strnicmp(identRootPath.c_str(), identFilePath.c_str(), identRootPath.length()))
			{
				ATOM_LOGGER::warning ("File <%s> is not located within the resource root directory, skipped.\n", identFilePath.c_str());
				continue;
			}
			strcpy (buffer, identFilePath.c_str() + identRootPath.length());
			for (unsigned j = 0; j < strlen(buffer); ++j)
			{
				if (buffer[j] == '\\')
				{
					buffer[j] = '/';
				}
			}

			ATOM_HARDREF(ATOM_Geode) geode;
			ATOM_ASSERT(geode);
			geode->setModelFileName (buffer);
			if (geode->load (ATOM_GetRenderDevice ()))
			{
				float hfsize = (info->terrainSize - 1.f) * info->spacing;

				ATOM_Matrix4x4f mat = meshes->sceneMeshes[i].second;
				float tx = mat.m30;
				float ty = mat.m31;
				float tz = mat.m32;
				tz *= -1.f;
				tz += hfsize;
				mat.m30 = 0.f;
				mat.m31 = 0.f;
				mat.m32 = 0.f;
				mat.m02 = -mat.m02;
				mat.m12 = -mat.m12;
				mat.m22 = -mat.m22;
				mat >>= ATOM_Matrix4x4f::getScaleMatrix (ATOM_Vector3f(1.f, 1.f, -1.f));
				mat >>= ATOM_Matrix4x4f::getRotateMatrixAngleAxis (ATOM_HalfPi, ATOM_Vector3f(0.f, 1.f, 0.f));
				mat.m30 = tx;
				mat.m31 = ty;
				mat.m32 = tz;

				geode->setO2T (mat);
				octreeNode->appendChild (geode.get());
			}
			else
			{
				ATOM_LOGGER::warning ("File <%s> is not a atom mesh file, skipped.\n", identFilePath.c_str());
				continue;
			}
		}
	}

	ATOM_SDLScene scene;
	if (scene.load (_outputFileName.c_str()))
	{
		bool b = true;

		while (b)
		{
			b = false;

			for (unsigned i = 0; i < scene.getRootNode()->getNumChildren(); ++i)
			{
				ATOM_Node *node = scene.getRootNode()->getChild (i);
				if (node && node->getLayer() == "FWImp")
				{
					scene.getRootNode()->removeChild (node);
					b = true;
					break;
				}
			}
		}
	}

	if (rootNode->getNumChildren() == 1)
	{
		rootNode = rootNode->getChild (0);
	}

	if (rootNode)
	{
		rootNode->setLayer ("FWImp");
		scene.getRootNode()->appendChild (rootNode.get());
	}

	if (!scene.save (_outputFileName.c_str()))
	{
		setErrorMessage  ("ERR: Write scene file failed.\n");
		return false;
	}

	return true;
}

ATOM_STRING FWConverter::genTerrainAOMapFileName (const char *dir) const
{
	return concateFileName (dir, "ao.png");
}

static bool launch_child_process (const char *cmdline, int *retval)
{
	STARTUPINFO startupinfo;
	::GetStartupInfoA (&startupinfo);

	char Args[4096];
	//char *pEnvCmd = getenv ("COMSPEC");
	//strcpy (Args, pEnvCmd ? pEnvCmd : "cmd");
	//strcat (Args, " /c ");
	strcpy (Args, cmdline);

	PROCESS_INFORMATION processinfo;
	if (!::CreateProcess (NULL, Args, NULL, NULL, TRUE, 0, NULL, NULL, &startupinfo, &processinfo))
	{
		return false;
	}

	::WaitForSingleObject (processinfo.hProcess, INFINITE);

	ULONG rc;

	if (!::GetExitCodeProcess (processinfo.hProcess, &rc))
	{
		rc = 0;
	}

	::CloseHandle (processinfo.hProcess);

	if (retval)
	{
		*retval = rc;
	}

	return true;
}

bool FWConverter::buildTerrainAOMap (TerrainInfo *info, const char *dir)
{
#if 0
	ATOM_STRING aoFileName = genTerrainAOMapFileName(_terrainPath.c_str());
	Terrain_AOGenerator aoGen (_outputFileName.c_str(), aoFileName.c_str());
	aoGen.setProcessCallback (&FWConverter::doAOCallback, this);

	if (!aoGen.generate (_aomapSize))
	{
		setErrorMessage  ("Generate Ambient Occlusion Map Failed.\n");
		return false;
	}

	return true;
#else
	return true;
#endif
}

void FWConverter::setProcessCallback (void (__cdecl *callback)(void *userdata, FWConverter::ProcessStage stage, int percent), void *userdata)
{
	_callback = callback;
	_callbackData = userdata;
}

void FWConverter::doAOCallback (void *userdata, int percent)
{
	const FWConverter *converter = (const FWConverter*)userdata;
	converter->doCallback (FWConverter::PS_BUILDING_AO_MAP, percent);
}

void FWConverter::doCallback (ProcessStage stage, int percent) const
{
	if (_callback)
	{
		_callback (_callbackData, stage, percent);
	}
}

void FWConverter::setErrorMessage (const char *format,...)
{
	char buffer[2048];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, 2048, format, args);
    va_end(args);
	_errorMessage = buffer;
	printf (_errorMessage.c_str());
}

const char *FWConverter::getErrorMessage (void) const
{
	return _errorMessage.c_str();
}
