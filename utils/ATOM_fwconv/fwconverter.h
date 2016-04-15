#ifndef __ATOM_TOOLS_FWCONVERTER_H
#define __ATOM_TOOLS_FWCONVERTER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <ATOM_dbghlp.h>
#include <ATOM_math.h>

class FWConverter
{
public:
	enum ProcessStage
	{
		PS_UNKNOWN,
		PS_READING_FREEWORD_FILE,
		PS_BUILDING_ALPHA_MAPS,
		PS_BUILDING_GLOBAL_MAP,
		PS_COLLAPSING_ALPHA_MAPS,
		PS_BUILDING_GRASS,
		PS_WRITING_TERRAIN_FILE_1,
		PS_WRITING_SCENE_FILE,
		PS_BUILDING_AO_MAP,
		PS_WRITING_TERRAIN_FILE_2,
		PS_FINISHED
	};

private:
	struct TerrainLayerInfo
	{
		ATOM_STRING detailFileName;
		float scaleU;
		float scaleV;
		ATOM_STRING alphaFileName;
		int alphaChannel;
		int alphamapWidth;
		int alphamapHeight;
	};

	struct TerrainGrassInfo
	{
		float width;
		float height;
		ATOM_STRING densityDataFile;
		ATOM_STRING grassTexture;
		ATOM_VECTOR<ATOM_Vector3f> grassPoints;
	};

	struct MeshCache
	{
		ATOM_MAP<int, ATOM_STRING> objMeshNames;
	};

	struct SceneMesh
	{
		ATOM_VECTOR<std::pair<ATOM_STRING, ATOM_Matrix4x4f> > sceneMeshes;
	};

	struct TerrainInfo
	{
		int terrainSize;
		float spacing;
		float vscale;
		ATOM_STRING heightmapFileName;
		ATOM_STRING globalTextureFileName;
		ATOM_VECTOR<TerrainLayerInfo> layers;
		ATOM_VECTOR<TerrainGrassInfo> grasses;
	};

public:
	FWConverter (void);
	FWConverter (const char *inputFileName, const char *outputFileName, const char *terrainPath, int alphamapWidth, int alphamapHeight, int aomapSize);

public:
	void setInputFileName (const char *inputFileName);
	void setOutputFileName (const char *outputFileName);
	void setTerrainPath (const char *terrainPath);
	void setAlphaMapWidth (int width);
	void setAlphaMapHeight (int height);
	void setAmbientOcclusionMapSize (int size);
	bool convert (void);
	void setProcessCallback (void (__cdecl *callback)(void *userdata, FWConverter::ProcessStage stage, int percent), void *userdata);
	const char *getErrorMessage (void) const;

private:
	ATOM_STRING identifyFileName (const ATOM_STRING &filename) const;
	ATOM_STRING getDirectoryPart (const ATOM_STRING &filename) const;
	bool buildInfo (TerrainInfo *info);
	bool buildAlphaMaps (TerrainInfo *info, const char *dir);
	bool buildGlobalMap (TerrainInfo *info, const char *dir, unsigned size);
	bool buildGrassDataFiles (TerrainInfo *info, const char *dir);
	bool buildTerrainAOMap (TerrainInfo *info, const char *dir);
	bool writeTerrainFile (TerrainInfo *info, const char *filename, bool writeAOMap);
	bool writeSceneFile (TerrainInfo *info, SceneMesh *meshes);
	bool collapseAlphaMaps (TerrainInfo *info, const char *dir);
	ATOM_STRING genAlphaMapFileName (const char *dir, int index) const;
	ATOM_STRING genGrassDataFileName (const char *dir, int index) const;
	ATOM_STRING genTerrainFileName (const char *dir) const;
	ATOM_STRING genTerrainAOMapFileName (const char *dir) const;
	ATOM_STRING concateFileName (const char *part1, const char *part2) const;
	void setErrorMessage (const char *format,...);
	void doCallback (ProcessStage stage, int percent) const;
	static void doAOCallback (void *userdata, int percent);

private:
	ATOM_STRING _inputFileName;
	ATOM_STRING _outputFileName;
	ATOM_STRING _terrainPath;
	ATOM_STRING _errorMessage;
	int _alphamapWidth;
	int _alphamapHeight;
	int _aomapSize;
	void (__cdecl *_callback)(void *userdata, FWConverter::ProcessStage stage, int);
	void *_callbackData;
};

#endif // __ATOM_TOOLS_FWCONVERTER_H
