#ifndef __ATOM3D_ENGINE_TERRAIN_H
#define __ATOM3D_ENGINE_TERRAIN_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "node.h"

class ATOM_TerrainQuadtree;
class ATOM_HeightField;
class GrassCluster;
class ATOM_TerrainRenderDatas;
class ATOM_TerrainRenderDatasFF;

class ATOM_ENGINE_API ATOM_Terrain: public ATOM_Node
{
public:
	struct VegetationInfo
	{
		float width;
		float height;
		ATOM_Vector4f tc;
		ATOM_STRING densityFileName;
		ATOM_STRING textureFileName;
	};

	enum RenderMode
	{
		RM_COLOR,
		RM_WORLDSPACENORMAL,
		RM_WORLDLOOKAT,
		RM_UVNORMAL,
		RM_SHADOWMAP
	};

public:
	ATOM_Terrain (void);
	virtual ~ATOM_Terrain (void);

public:
	virtual void accept (ATOM_Visitor &visitor);
	virtual bool create (const char *heightmapFileName, const ATOM_Vector3f &scale, unsigned patchSize, const char *diffuseTexture);
	virtual void setupRenderQueue (ATOM_CullVisitor *visitor);
	virtual ATOM_Material *getMaterial (void) const;

public:
	bool save (const char *filename);
	ATOM_HeightField *getHeightField (void) const;
	void update (void);
	void setTerrainFileName (const ATOM_STRING &filename);
	const ATOM_STRING &getTerrainFileName (void) const;
	void setMaxPixelError (int error);
	int getMaxPixelError (void) const;
	void setDrawGrass (int enable);
	int getDrawGrass (void) const;
	void setGrassFadeOutDistance (float distance);
	float getGrassFadeOutDistance (void) const;
	bool rayIntersect (const ATOM_Vector3f &start, const ATOM_Vector3f &direction, float &t) const;
	const ATOM_Vector3f &getScale (void) const;
	void setDrawDetail (int drawDetail);
	int getDrawDetail (void) const;
	void setDetailDistance (float distance);
	float getDetailDistance (void) const;
	void setRenderMode (RenderMode mode);
	RenderMode getRenderMode (void) const;
	void setNormalMap (ATOM_Texture *texture);
	void setTerrainLightmapFileName (const ATOM_STRING &fileName);
	const ATOM_STRING &getTerrainLightmapFileName (void) const;
	ATOM_Texture *getLightmap (void) const;
	ATOM_Texture *getBasemap (void) const;
	ATOM_Texture *getNormalmap (void) const;
	ATOM_Texture *getHeightmap (void) const;
	void setHeightmap (ATOM_Texture *texture);
	unsigned getNumDetailMaps (void) const;
	ATOM_Texture *getDetailMap (unsigned index) const;
	ATOM_Vector4f getDetailScale (unsigned index) const;
	void setDetailScale (unsigned index, const ATOM_Vector2f &value);
	unsigned getNumAlphaMaps (void) const;
	ATOM_Texture *getAlphaMap (unsigned index) const;
	unsigned getAlphaMapWidth (void) const;
	unsigned getAlphaMapHeight (void) const;
	void setDetailLodLevel (int lodLevel);
	int getDetailLodLevel (void) const;
	void setGrassSizeVarianceMin (float val);
	float getGrassSizeVarianceMin (void) const;
	void setGrassSizeVarianceMax (float val);
	float getGrassSizeVarianceMax (void) const;
	void setLODCamera (ATOM_Camera *camera);
	ATOM_Camera *getLODCamera (void) const;
	ATOM_Texture *getHeightTexture (void) const;
	void drawWireframe (bool wireframe);

	bool setAlphaMapSize (int width, int height);
	bool addDetailMapFileName (const char *filename, const ATOM_Vector2f &tile, bool ignoreAlpha = false);
	void removeDetailMap (unsigned index);
	const char *getDetailMapFileName (unsigned index) const;
	void setDetailMapTile (unsigned index, const ATOM_Vector2f &tile);
	const ATOM_Vector2f &getDetailMapTile (unsigned index) const;
	bool setHeightMapFileName (const char *filename, const ATOM_Vector3f &scale);
	const char *getHeightMapFileName (void) const;
	const ATOM_Vector3f &getHeightMapScale (void) const;
	void setBaseMapFileName (const char *filename);
	void setBaseMap (ATOM_Texture *texture);
	bool setAlphaFileName (unsigned index, const char *filename);
	const char *getAlphaFileName (unsigned index) const;

	// used for editor only
	unsigned getNumGrassLayers (void) const;
	const VegetationInfo &getGrassLayerInfo (unsigned index) const;
	bool appendGrassLayer (const VegetationInfo &info);
	void removeGrassLayer (unsigned index);
	void addRandomGrassObjects (unsigned index, const ATOM_Vector3f &position, float radius, unsigned count, const ATOM_Vector3f *normals);
	void removeRandomGrassObjects (unsigned index, const ATOM_Vector3f &position, float radius, unsigned count);
	void setGrassLayerSize (unsigned index, float width, float height);

protected:
	virtual void buildBoundingbox (void) const;
	virtual void onTransformChanged (void);
	virtual bool onLoad(ATOM_RenderDevice *device);

private:
	bool createNormalMap (ATOM_Texture *texture, const ATOM_Vector3f *normals) const;
	void updateDetailAlpha (int detailIndex, const ATOM_Rect2Di *rect, const float *alphaValue);
	void updateDetailAlphaValue (int detailIndex, const ATOM_Rect2Di *rect, float value);
	void removeDetailAlpha (int detailIndex);
	void removeAlphaChannel (int alphaIndex, int alphaChannel);
	int getAlphaIndexByDetail (int detail) const;
	int getAlphaChannelByDetail (int detail) const;
	void resetMaterialParameters (void) const;
	GrassCluster *createGrassCluster (VegetationInfo *info, unsigned count);

public:
	static void enableEditorMode (bool enable);
	static bool isEditorModeEnabled (void);


protected:
	ATOM_TerrainQuadtree *_quadtree;
	ATOM_STRING _terrainFileName;
	ATOM_STRING _lightmapFileName;
	ATOM_STRING _heightmapFileName;
	ATOM_VECTOR<ATOM_STRING> _blendTextureFileNames;
	ATOM_VECTOR<ATOM_STRING> _alphaTextureFileNames;
	ATOM_Camera *_LODcamera;
	RenderMode _renderMode;
	ATOM_Vector3f _scale;
	ATOM_Vector3f _eye;
	int _heightmapWidth;
	int _heightmapHeight;
	int _alphamapWidth;
	int _alphamapHeight;
	int _drawDetail;
	int _patchSize;
	int _lastViewportH;
	int _maxPixelError;
	bool _maxPixelErrorChanged;
	int _drawGrass;
	int _lodLevel;
	float _lastTanHalfFovy;
	float _detailDistance;
	float _grassSizeVMin;
	float _grassSizeVMax;
	float _grassFadeOutDistance;
	ATOM_AUTOPTR(ATOM_Material) _material;
	ATOM_AUTOREF(ATOM_Texture) _lightmap;
	ATOM_AUTOREF(ATOM_Texture) _normalmap;
	ATOM_AUTOREF(ATOM_Texture) _diffusemap;
	ATOM_AUTOREF(ATOM_Texture) _heightmap;
	ATOM_VECTOR<ATOM_Vector2f> _detailScales;
	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Texture)> _detailmaps;
	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Texture)> _alphamaps;

	//GrassCluster* _grassLayers;
	ATOM_TerrainRenderDatas *_renderDatas;
	ATOM_TerrainRenderDatasFF *_renderDatasFF;

	ATOM_TerrainRenderDatas *_renderDatas_shadow;

	ATOM_VECTOR<VegetationInfo> _vegetations;
	ATOM_VECTOR<GrassCluster*> _grassLayers;
	static bool _editorMode;

	ATOM_CLASS(engine, ATOM_Terrain, Terrain)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_Terrain)
	ATOM_DECLARE_NODE_FILE_TYPE(ATOM_Terrain, Terrain, "xml", "ATOM3D terrain")

	//--- wangjian added ---//
protected:
	virtual void resetMaterialDirtyFlag_impl();
public :
	void generateTerrainMapForNoneDetail( const char* file );

	void setDetailNormalMap(ATOM_STRING & filename,int detailID);
	ATOM_STRING getDetailNormalMap(int detailID) const;

	void setDetailSpecPower(float power, int detailID);
	float getDetailSpecPower(int detailID);

	void setDetailGloss(float gloss, int detailID);
	float getDetailGloss(int detailID);

	void checkEnableDetailNormal();

private:
	void clearAllDetailSpecific();
	void enableDetailNormal(bool enable);
	void processDetailNormal(bool enable);
	ATOM_AUTOPTR(ATOM_Material) _basemapGenMaterial;

	// wangjian adde for test detail normals
	ATOM_VECTOR<ATOM_STRING>				_detailNormalFiles;		// 细节法线纹理文件集合
	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Texture)> _normalmaps;			// 细节法线集合	
	ATOM_VECTOR<float>						_glossnesses;			// 光泽度集合
	ATOM_VECTOR<float>						_specPowers;			// 高光POWER集合
	bool									_enableDetailNormal;	// 是否开启细节法线
	ATOM_STRING								_globalTexture;			// 全局地表纹理
	//----------------------//



};

#endif // __ATOM3D_ENGINE_TERRAIN_H
