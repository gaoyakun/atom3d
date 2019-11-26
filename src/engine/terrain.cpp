#include "stdafx.h"
#include "terrain.h"
#include "terrainquadtree.h"
#include "visitor.h"
#include "grass.h"
#include "imagepacker.h"
#include "terrainrenderdatas.h"

unsigned terrainCount = 0;

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_Terrain)
  ATOM_ATTRIBUTES_BEGIN(ATOM_Terrain)
    ATOM_ATTRIBUTE_PERSISTENT_NODEFAULT(ATOM_Terrain, "FileName", getTerrainFileName, setTerrainFileName, "group=ATOM_Terrain;type=vfilename;desc='地形文件名'")
	ATOM_ATTRIBUTE_PERSISTENT_NODEFAULT(ATOM_Terrain, "Lightmap", getTerrainLightmapFileName, setTerrainLightmapFileName, "group=ATOM_Terrain;type=vfilename;desc='光照图文件名'")
    ATOM_ATTRIBUTE_PERSISTENT(ATOM_Terrain, "MaxPixelError", getMaxPixelError, setMaxPixelError, 10, "group=ATOM_Terrain;min=0;max=50;step=1;desc='LOD误差阀值'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Terrain, "GrassFadeOutDistance", getGrassFadeOutDistance, setGrassFadeOutDistance, 1024, "group=ATOM_Terrain;desc='植被视距'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Terrain, "DrawGrass", getDrawGrass, setDrawGrass, 1, "group=ATOM_Terrain;type=bool;desc='渲染草'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Terrain, "DrawDetail", getDrawDetail, setDrawDetail, 1, "group=ATOM_Terrain;type=bool;desc='渲染细节'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Terrain, "DetailDistance", getDetailDistance, setDetailDistance, 1000.f, "group=ATOM_Terrain;min=500;max=5000;step=1;desc='细节范围'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Terrain, "LODLevel", getDetailLodLevel, setDetailLodLevel, 0, "group=ATOM_Terrain;min=0;max=16;step=1;desc='强制LOD等级'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Terrain, "GrassSizeMin", getGrassSizeVarianceMin, setGrassSizeVarianceMin, 0.8f, "group=ATOM_Terrain;min=0.f;max=10.f;step=0.001;desc='草最小大小'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Terrain, "GrassSizeMax", getGrassSizeVarianceMax, setGrassSizeVarianceMax, 1.2f, "group=ATOM_Terrain;min=0.f;max=10.f;step=0.001;desc='草最大大小'")
  ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_Terrain, ATOM_Node)

ATOM_IMPLEMENT_NODE_FILE_TYPE(ATOM_Terrain)

bool ATOM_Terrain::_editorMode = false;

ATOM_Terrain::ATOM_Terrain (void)
{
	ATOM_STACK_TRACE(ATOM_Terrain::ATOM_Terrain);

	terrainCount++;

	_quadtree = 0;
	_lastViewportH = 0;
	_lastTanHalfFovy = 0;
	_scale.set (1.f, 1.f, 1.f);
	_patchSize = 33;
	_maxPixelError = 10;
	_maxPixelErrorChanged = true;
	_renderMode = RM_COLOR;
	_LODcamera = 0;
	_drawGrass = 1;
	_drawDetail = 1;
	_lodLevel = 0;
	_alphamapWidth = 0;
	_alphamapHeight = 0;
	_heightmapWidth = 0;
	_heightmapHeight = 0;
	_grassSizeVMin = 0.8f;
	_grassSizeVMax = 1.2f;
	_grassFadeOutDistance = 1024.f;
	_detailDistance = 1000.f;
	_renderDatas = ATOM_NEW(ATOM_TerrainRenderDatas);
	_renderDatas->setTerrain (this);
	_renderDatasFF = ATOM_NEW(ATOM_TerrainRenderDatasFF);

	//--- wangjian added ---//
	_renderDatas_shadow = ATOM_NEW(ATOM_TerrainRenderDatas);
	_renderDatas_shadow->setTerrain(this);
	_enableDetailNormal = ATOM_RenderSettings::isTerrDetailNormalEnabled();
	_globalTexture = "globaltexture.dds";
	_diffusemap = 0;
	//----------------------//

	setProjectable (ATOM_Node::PROJECTABLE);
}

ATOM_Terrain::~ATOM_Terrain (void)
{
	ATOM_STACK_TRACE(ATOM_Terrain::~ATOM_Terrain);

	terrainCount--;

	ATOM_DELETE(_quadtree);

	ATOM_DELETE(_renderDatas);

	ATOM_DELETE(_renderDatasFF);

	//--- wangjian added ---//
	ATOM_DELETE(_renderDatas_shadow);
	//----------------------//

	for (unsigned i = 0; i < _grassLayers.size(); ++i)
	{
		ATOM_DELETE(_grassLayers[i]);
	}
	_grassLayers.clear ();
}

void ATOM_Terrain::setDrawDetail (int drawDetail)
{
	_drawDetail = drawDetail;
}

int ATOM_Terrain::getDrawDetail (void) const
{
	return _drawDetail;
}

void ATOM_Terrain::accept (ATOM_Visitor &visitor)
{
	ATOM_STACK_TRACE(ATOM_Terrain::accept);

	visitor.visit (*this);
}

bool ATOM_Terrain::onLoad (ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_Terrain::onLoad);

	if (!_filenameToBeLoad.empty ())
	{
		_terrainFileName = _filenameToBeLoad;
	}

	ATOM_AutoFile f(_terrainFileName.c_str(), ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
	{
		return false;
	}
	unsigned size = f->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = f->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument doc;
	doc.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (doc.Error ())
	{
		ATOM_LOGGER::error ("%s(0x%08X) %s\n", __FUNCTION__, device, doc.ErrorDesc());
		return false;
	}

	ATOM_TiXmlElement *eTerrain = doc.RootElement ();
	if (!eTerrain)
	{
		return false;
	}

	//-----------------------------------------------------//
	// wangjian modified 
	// 根据是否有细节法线纹理选择合适的地形材质
	_enableDetailNormal = ATOM_RenderSettings::isTerrDetailNormalEnabled();
	int detailNormalCount = 0;
	eTerrain->Attribute ("DetailNormalCount", &detailNormalCount);
	const char *materialFileName = _editorMode ? "/editor/materials/terrain.mat" : "/materials/builtin/terrain.mat";
	
	if( _enableDetailNormal && detailNormalCount > 0 )
		materialFileName = _editorMode ? "/editor/materials/terrain_detailbump.mat" : "/materials/builtin/terrain_detailbump.mat";
	else
		_enableDetailNormal = false;

	_material = ATOM_MaterialManager::createMaterialFromCore (device, materialFileName);
	if (!_material)
	{
		return false;
	}
	//-----------------------------------------------------//

	if (!eTerrain->Attribute ("PatchSize", &_patchSize))
	{
		return false;
	}

	if (_patchSize <= 0 || _patchSize > 65 || !ATOM_ispo2 (_patchSize - 1))
	{
		return false;
	}

	if (_patchSize > 33)
	{
		_patchSize = 33;
	}

	double terrainScaleX, terrainScaleY, terrainScaleZ;
	if (!eTerrain->Attribute ("ScaleX", &terrainScaleX) || !eTerrain->Attribute("ScaleY", &terrainScaleY) || !eTerrain->Attribute("ScaleZ", &terrainScaleZ))
	{
		return false;
	}

	if (!eTerrain->Attribute ("AlphaMapWidth", &_alphamapWidth) || !eTerrain->Attribute ("AlphaMapHeight", &_alphamapHeight))
	{
		return false;
	}

	const char *heightmapFileName = eTerrain->Attribute ("HeightMap");
	if (!heightmapFileName)
	{
		return false;
	}

	if (!setHeightMapFileName (heightmapFileName, ATOM_Vector3f(terrainScaleX, terrainScaleY, terrainScaleZ)))
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// wangjian added for detail normal
	_detailNormalFiles.clear();
	_normalmaps.clear();
	_glossnesses.clear();
	_specPowers.clear();
	//------------------------------------//

	_blendTextureFileNames.resize (0);
	_detailScales.resize (0);
	int num_detail_normal = 0;

	ATOM_TiXmlElement *eDetails = eTerrain->FirstChildElement ("Details");
	if (eDetails)
	{
		ATOM_TiXmlElement *eDetail = eDetails->FirstChildElement ("Detail");
		if (!eDetail)
		{
			return false;
		}
		while (eDetail)
		{
			const char *detailTextureFileName = eDetail->Attribute ("FileName");
			if (!detailTextureFileName)
			{
				return false;
			}

			double scaleU, scaleV;
			if (!eDetail->Attribute ("ScaleU", &scaleU) || !eDetail->Attribute ("ScaleV", &scaleV))
			{
				return false;
			}

			if (!addDetailMapFileName (detailTextureFileName, ATOM_Vector2f(scaleU, scaleV), true))
			{
				return false;
			}

			//-----------------------------------------------------//
			// wangjian added for test detail normal map
			const char *detailNormalFileName = eDetail->Attribute ("NormalFileName");
			if (detailNormalFileName)
			{
				// 保存法线文件名
				_detailNormalFiles.back() = detailNormalFileName;

				// 如果开启细节法线，加载法线纹理
				if( _enableDetailNormal )
				{
					ATOM_AUTOREF(ATOM_Texture) tex = ATOM_CreateTextureResource (	detailNormalFileName,
																					ATOM_PIXEL_FORMAT_UNKNOWN, 
																					ATOM_LoadPriority_IMMEDIATE	);
					if (tex)
					{
						num_detail_normal++;
						_normalmaps.back() = tex;
					}
				}
			}

			double SpecPower = 64.0;
			if ( eDetail->Attribute ("SpecPower", &SpecPower) )
			{
				_specPowers.back() = SpecPower;
			}

			double Glossness = 64.0;
			if ( eDetail->Attribute ("Gloss", &Glossness) )
			{
				_glossnesses.back() = Glossness;
			}

			// 重置材质参数
			resetMaterialParameters ();
			//-------------------------------------------------------//

			eDetail = eDetail->NextSiblingElement ("Detail");
		}

		// wangjian added for test detail normal map
		ATOM_ASSERT(num_detail_normal==detailNormalCount);

		ATOM_TiXmlElement *eAlphas = eTerrain->FirstChildElement ("Alphas");
		if (!eAlphas)
		{
			return false;
		}
		
		ATOM_TiXmlElement *eAlpha = eAlphas->FirstChildElement ("Alpha");
		if (!eAlpha)
		{
			return false;
		}
		int n = 0;
		while (eAlpha && n < _alphamaps.size())
		{
			const char *alphaTextureFileName = eAlpha->Attribute ("FileName");
			if (!setAlphaFileName (n++, alphaTextureFileName))
			{
				return false;
			}
			eAlpha = eAlpha->NextSiblingElement ("Alpha");
		}
	}

	_vegetations.resize (0);
	ATOM_TiXmlElement *eVegetations = eTerrain->FirstChildElement ("Vegetations");
	if (eVegetations)
	{
		ATOM_TiXmlElement *eVegetation = eVegetations->FirstChildElement ("Vegetation");
		while (eVegetation)
		{
			_vegetations.resize (_vegetations.size()+1);

			const char *textureFileName = eVegetation->Attribute ("TextureFileName");
			_vegetations.back().textureFileName = textureFileName ? textureFileName : "";
			const char *dataFileName = eVegetation->Attribute ("DensityFileName");
			_vegetations.back().densityFileName = dataFileName ? dataFileName : "";
			eVegetation->QueryFloatAttribute ("Width", &_vegetations.back().width);
			eVegetation->QueryFloatAttribute ("Height", &_vegetations.back().height);

			eVegetation = eVegetation->NextSiblingElement ("Vegetation");
		}
	}

	if (!_vegetations.empty ())
	{
		if (isEditorModeEnabled())
		{
			_grassLayers.resize (_vegetations.size());
			for (unsigned i = 0; i < _grassLayers.size(); ++i)
			{
				_grassLayers[i] = createGrassCluster (&_vegetations[i], 1);
			}
		}
		else
		{
			_grassLayers.resize (1);
			_grassLayers[0] = createGrassCluster (&_vegetations[0], _vegetations.size());
		}
	}

	// wangjian modified
	const char * globalFileName = eTerrain->Attribute ("GlobalTexture");
	if( globalFileName )
		_globalTexture = globalFileName;
#if 0
	if (0 && !_editorMode)
#else
	// 如果不是用细节地形纹理 就使用地形基本纹理[ 编辑器模式下总是会生成或者加载基本纹理 ]
	if( !ATOM_RenderSettings::detailedTerrainEnabled()/* || _editorMode*/ )
#endif
	{
		
		setBaseMapFileName (_globalTexture.c_str());
	}
	//------------------------------------------------------------------------------------//

	resetMaterialParameters ();

	_load_flag = LOAD_ALLFINISHED;

	return true;
}

bool ATOM_Terrain::create (const char *heightmapFileName, const ATOM_Vector3f &scale, unsigned patchSize, const char *diffuseTexture)
{
	ATOM_STACK_TRACE(ATOM_Terrain::create);

	if (!heightmapFileName || !diffuseTexture || !ATOM_ispo2 (patchSize - 1))
	{
		return false;
	}

	unsigned heightmapW;

	ATOM_AutoFile f(heightmapFileName, ATOM_VFS::read);
	if (!f)
	{
		return false;
	}

	unsigned fSize = f->size ();
	if ((fSize & 1) != 0)
	{
		return false;
	}

	unsigned samples = (fSize >> 1);
	heightmapW = ATOM_sqrt (samples);
	if (heightmapW * heightmapW != samples || !ATOM_ispo2(heightmapW - 1))
	{
		return false;
	}

	unsigned short *h = ATOM_NEW_ARRAY(unsigned short, samples);
	f->read (h, fSize);

	float *elevations = ATOM_NEW_ARRAY(float, samples);
	for (unsigned i = 0; i < samples; ++i)
	{
		elevations[i] = h[i] * scale.y;
	}

	if (_editorMode)
	{
		_heightmap = ATOM_GetRenderDevice()->allocTexture (	NULL,
															elevations, 
															heightmapW, 
															heightmapW, 
															ATOM_PIXEL_FORMAT_R32F, 
															ATOM_Texture::TEXTURE2D|ATOM_Texture::NOMIPMAP|ATOM_Texture::RENDERTARGET);
		ATOM_ASSERT(_heightmap);
	}

	ATOM_DELETE_ARRAY(h);

	ATOM_ASSERT(!_quadtree);
	_quadtree = ATOM_NEW(ATOM_TerrainQuadtree, this);

	bool b = _quadtree->build (patchSize, heightmapW, elevations, scale.x, scale.z, 24);
	ATOM_DELETE_ARRAY(elevations);
	if (!b)
	{
	  return false;
	}

	if (!_lightmap)
	{
		_lightmap = ATOM_GetColorTexture (0xFFFFFFFF);
	}

	//--- wangjian modified ---//
	// 异步加载 ：添加异步加载BOOL标记
	_diffusemap = ATOM_CreateTextureResource (diffuseTexture, ATOM_PIXEL_FORMAT_DXT1, ATOM_LoadPriority_IMMEDIATE);
	if (!_diffusemap)
	{
		_diffusemap = ATOM_GetColorTexture (0xFFFFFFFF);
	}

	return true;
}

void ATOM_Terrain::update (void)
{
}

void ATOM_Terrain::buildBoundingbox (void) const
{
//#pragma message("FIXME!!! ATOM_Terrain::buildBoundingbox is incorrect\n")
	ATOM_STACK_TRACE(ATOM_Terrain::buildBoundingbox);

	_boundingBox = getHeightField()->getBBoxTree()->getRootNode()->bbox;
}

void ATOM_Terrain::onTransformChanged (void)
{
  ATOM_Node::onTransformChanged ();
}

void ATOM_Terrain::setupRenderQueue (ATOM_CullVisitor *visitor)
{
	ATOM_STACK_TRACE(ATOM_Terrain::setupRenderQueue);

	// 如果不是用细节地形纹理 就使用地形基本纹理
	if( !ATOM_RenderSettings::detailedTerrainEnabled() )
	{
		setBaseMapFileName (_globalTexture.c_str());
	}

	if (_quadtree && _material)
	{
		ATOM_Camera *camera = visitor->getCamera();
		ATOM_Camera *LODcamera = _LODcamera ? _LODcamera : camera;

		const ATOM_Matrix4x4f &worldMatrix = getWorldMatrix ();

		//--- wangjian added ---//
		const char* renderSchemeName = ATOM_RenderScheme::getCurrentRenderScheme() ? ATOM_RenderScheme::getCurrentRenderScheme()->getName() : 0;
		if( !renderSchemeName )
			return;
		bool bShadowGenPass = false;
		if( !stricmp(renderSchemeName,"shadowmap") )
			bShadowGenPass = true;
		//----------------------//

		if (ATOM_RenderSettings::isFixedFunction())
		{
			visitor->addDrawable (_renderDatasFF, worldMatrix, _material.get());
		}
		else
		{
			//--- wangjian added ---//
			if( bShadowGenPass )
			{
				_renderDatas_shadow->clearPatches();
			}
			else
			{
				_renderDatas->clearPatches ();
			}
			//---------------------//

			//_renderDatas->setFogEnabled (ATOM_Scene::getCurrentScene()->isFogEnabled());

			int viewportH = LODcamera->getViewport().size.h;
			float tanHalfFovy = LODcamera->getTanHalfFovy();
			if (viewportH != _lastViewportH || tanHalfFovy != _lastTanHalfFovy || _maxPixelErrorChanged)
			{
				_maxPixelErrorChanged = false;
				_lastViewportH = viewportH;
				_lastTanHalfFovy = tanHalfFovy;
				_quadtree->setupCamera (viewportH, tanHalfFovy, _maxPixelError);
			}

			int detailLodLevel = this->getDetailLodLevel ();
			const ATOM_Matrix4x4f &viewMatrix = LODcamera->getViewMatrix ();
			ATOM_Vector3f worldEyePos(viewMatrix.m30, viewMatrix.m31, viewMatrix.m32);
			ATOM_Vector3f eyePos = getInvWorldMatrix() >> worldEyePos;

			//--- wangjian modified ---//
			if( bShadowGenPass )
			{
#if 0
				_quadtree->cull ( camera, eyePos, worldMatrix, _renderDatas_shadow, (_diffusemap != NULL && !_editorMode) );
#else
				_quadtree->cull ( camera, eyePos, worldMatrix, _renderDatas_shadow, (_diffusemap != NULL) );
#endif
				visitor->addDrawable (_renderDatas_shadow, worldMatrix, _material.get(), -1, true);
			}
			else
			{
				#if 0
				_quadtree->cull ( camera, eyePos, worldMatrix, _renderDatas, (_diffusemap != NULL && !_editorMode) );
				#else
				_quadtree->cull ( camera, eyePos, worldMatrix, _renderDatas, (_diffusemap != NULL) );
				#endif
				visitor->addDrawable (_renderDatas, worldMatrix, _material.get(), -1, true);
			}
			
			//setDetailLodLevel (detailLodLevel);

			//visitor->addDrawable (_renderDatas, worldMatrix, _material.get(), -1, true);
			//-----------------------//
		}

		if (_renderMode == RM_COLOR && _drawGrass && !_grassLayers.empty())
		{
			ATOM_Matrix4x4f mvp = camera->getViewProjectionMatrix ()>>worldMatrix;
			for (unsigned i = 0; i < _grassLayers.size(); ++i)
			{
				if (_grassLayers[i])
				{
					//--- wangjian modified ---//
					_grassLayers[i]->setVPMatrix (mvp,bShadowGenPass);
					//-------------------------//

					visitor->addDrawable (_grassLayers[i], worldMatrix, _grassLayers[i]->getMaterial());
				}
			}
		}
	}
}

ATOM_Material *ATOM_Terrain::getMaterial (void) const
{
	return _material.get();
}

void ATOM_Terrain::setMaxPixelError (int error)
{
	if (error < 0)
	{
		error = 0;
	}

    if (error != _maxPixelError)
    {
		_maxPixelErrorChanged = true;
		_maxPixelError = error;
    }
}

int ATOM_Terrain::getMaxPixelError (void) const
{
  return _maxPixelError;
}

bool ATOM_Terrain::rayIntersect (const ATOM_Vector3f &start, const ATOM_Vector3f &direction, float &t) const
{
	return _quadtree ? _quadtree->rayIntersect (start, direction, t) : false;
}

void ATOM_Terrain::setDrawGrass (int enable)
{
	_drawGrass = enable;
}

int ATOM_Terrain::getDrawGrass (void) const
{
	return _drawGrass;
}

void ATOM_Terrain::setGrassFadeOutDistance (float distance)
{
	_grassFadeOutDistance = distance;
}

float ATOM_Terrain::getGrassFadeOutDistance (void) const
{
	return _grassFadeOutDistance;
}

ATOM_HeightField *ATOM_Terrain::getHeightField (void) const
{
	return _quadtree ? _quadtree->getHeightField() : 0;
}

void ATOM_Terrain::setTerrainFileName (const ATOM_STRING &filename)
{
	_terrainFileName = filename;
}

const ATOM_STRING &ATOM_Terrain::getTerrainFileName (void) const
{
	return _terrainFileName;
}

const ATOM_Vector3f &ATOM_Terrain::getScale (void) const
{
	return _scale;
}

void ATOM_Terrain::setDetailDistance (float distance)
{
	_detailDistance = distance;
}

float ATOM_Terrain::getDetailDistance (void) const
{
	return _detailDistance;
}

void ATOM_Terrain::setRenderMode (RenderMode mode)
{
	_renderMode = mode;
}

ATOM_Terrain::RenderMode ATOM_Terrain::getRenderMode (void) const
{
	return _renderMode;
}

void ATOM_Terrain::setNormalMap (ATOM_Texture *texture)
{
	_normalmap = texture;
	_material->getParameterTable()->setTexture ("normalMap", _normalmap.get());
}

void ATOM_Terrain::setTerrainLightmapFileName (const ATOM_STRING &fileName)
{
	ATOM_STACK_TRACE(ATOM_Terrain::setTerrainLightmapFileName);

	_lightmapFileName = fileName;

	if (!fileName.empty ())
	{
		_lightmap = ATOM_GetRenderDevice()->createTextureResource (fileName.c_str());
	}
	else
	{
		_lightmap = 0;
	}

	if (!_lightmap)
	{
		_lightmap = ATOM_GetColorTexture (0xFFFFFFFF);
	}
}

const ATOM_STRING &ATOM_Terrain::getTerrainLightmapFileName (void) const
{
	return _lightmapFileName;
}

ATOM_Texture *ATOM_Terrain::getLightmap (void) const
{
	return _lightmap.get();
}

ATOM_Texture *ATOM_Terrain::getBasemap (void) const
{
	return _diffusemap.get();
}

ATOM_Texture *ATOM_Terrain::getNormalmap (void) const
{
	return _normalmap.get();
}

ATOM_Texture *ATOM_Terrain::getHeightmap (void) const
{
	return _heightmap.get();
}

void ATOM_Terrain::setHeightmap (ATOM_Texture *texture)
{
	_heightmap = texture;

	if (_material)
	{
		_material->getParameterTable()->setTexture("heightMap", _heightmap.get());
	}
}

unsigned ATOM_Terrain::getNumDetailMaps (void) const
{
	return _detailmaps.size();
}

ATOM_Texture *ATOM_Terrain::getDetailMap (unsigned index) const
{
	return _detailmaps[index].get();
}

ATOM_Vector4f ATOM_Terrain::getDetailScale (unsigned index) const
{
	return ATOM_Vector4f(_detailScales[index].x, _detailScales[index].y, 0.f, 0.f);
}

void ATOM_Terrain::setDetailScale (unsigned index, const ATOM_Vector2f &value)
{
	_detailScales[index] = value;

	char buffer[256];
	sprintf (buffer, "detail%d_uvscale", index);
	_material->getParameterTable()->setVector (buffer, ATOM_Vector4f(value.x, value.y, _glossnesses[index], _specPowers[index]));		// wangjian modified : 使用Z通道作为GLOSS,使用W通道作为SPEC POWER 

}

unsigned ATOM_Terrain::getNumAlphaMaps (void) const
{
	return _alphamaps.size();
}

ATOM_Texture *ATOM_Terrain::getAlphaMap (unsigned index) const
{
	return _alphamaps[index].get();
}

void ATOM_Terrain::setDetailLodLevel (int lodLevel)
{
	_lodLevel = lodLevel;
}

int ATOM_Terrain::getDetailLodLevel (void) const
{
	return _lodLevel;
}

void ATOM_Terrain::setGrassSizeVarianceMin (float val)
{
	ATOM_STACK_TRACE(ATOM_Terrain::setGrassSizeVarianceMin);

	if (_grassSizeVMin != val)
	{
		_grassSizeVMin  = val;

		for (unsigned i = 0; i < _grassLayers.size(); ++i)
		{
			if (_grassLayers[i])
			{
				_grassLayers[i]->updateScale (_grassSizeVMin, _grassSizeVMax);
			}
		}
	}
}

float ATOM_Terrain::getGrassSizeVarianceMin (void) const
{
	return _grassSizeVMin;
}

void ATOM_Terrain::setGrassSizeVarianceMax (float val)
{
	ATOM_STACK_TRACE(ATOM_Terrain::setGrassSizeVarianceMax);

	if (_grassSizeVMax != val)
	{
		_grassSizeVMax  = val;

		for (unsigned i = 0; i < _grassLayers.size(); ++i)
		{
			if (_grassLayers[i])
			{
				_grassLayers[i]->updateScale (_grassSizeVMin, _grassSizeVMax);
			}
		}
	}
}

float ATOM_Terrain::getGrassSizeVarianceMax (void) const
{
	return _grassSizeVMax;
}

bool ATOM_Terrain::createNormalMap (ATOM_Texture *normalmap, const ATOM_Vector3f *normals) const
{
	ATOM_STACK_TRACE(ATOM_Terrain::createNormalMap);
	unsigned w = normalmap->getWidth();
	unsigned h = normalmap->getHeight();

	ATOM_VECTOR<ATOM_ColorARGB> normalToColors((w+1) * (h+1));
	for (unsigned i = 0; i < (w+1) * (h+1); ++i)
	{
		ATOM_Vector3f v = normals[i] * 0.5f + ATOM_Vector3f(0.5f, 0.5f, 0.5f);
		v.x = ATOM_saturate (v.x);
		v.y = ATOM_saturate (v.y);
		v.z = ATOM_saturate (v.z);
		normalToColors[i].setFloats (v.x, v.y, v.z, 1.f);
	}

	ATOM_BaseImage image(w+1, h+1, ATOM_PIXEL_FORMAT_BGRA8888, &normalToColors[0], 1);
	image.crop (0, 0, w, h);

	return normalmap->updateTexImage (image.getData(), 0, 0, w, h, ATOM_PIXEL_FORMAT_BGRA8888);
}

void ATOM_Terrain::setLODCamera (ATOM_Camera *camera)
{
	_LODcamera = camera;
}

ATOM_Camera *ATOM_Terrain::getLODCamera (void) const
{
	return _LODcamera;
}

void ATOM_Terrain::enableEditorMode (bool enable)
{
	_editorMode = enable;
}

bool ATOM_Terrain::isEditorModeEnabled (void)
{
	return _editorMode;
}

ATOM_Texture *ATOM_Terrain::getHeightTexture (void) const
{
	return _heightmap.get();
}

void ATOM_Terrain::drawWireframe (bool wireframe)
{
	if (_editorMode)
	{
		_material->getParameterTable()->setInt ("drawWireframe", wireframe ? 1 : 0);
	}
}

bool ATOM_Terrain::setAlphaMapSize (int width, int height)
{
	if (width != _alphamapWidth || height != _alphamapHeight)
	{
		if (width <= 0 || height <= 0)
		{
			return false;
		}

		_alphamapWidth = width;
		_alphamapHeight = height;

		for (unsigned i = 0; i < _alphamaps.size(); ++i)
		{
			ATOM_BaseImage img;
			img.init (_alphamaps[i]->getWidth(), _alphamaps[i]->getHeight(), ATOM_PIXEL_FORMAT_BGRA8888, NULL, 1);
			_alphamaps[i]->getTexImage (ATOM_PIXEL_FORMAT_BGRA8888, img.getData());
			img.resize (_alphamapWidth, _alphamapHeight);

			unsigned flags = ATOM_Texture::TEXTURE2D|ATOM_Texture::NOCOMPRESS|ATOM_Texture::NOMIPMAP;
			if (_editorMode)
			{
				flags |= ATOM_Texture::RENDERTARGET;
			}
			_alphamaps[i] = ATOM_GetRenderDevice()->allocTexture (NULL, img.getData(), _alphamapWidth, _alphamapHeight, ATOM_PIXEL_FORMAT_BGRA8888, flags);
		}
	}

	return true;
}

bool ATOM_Terrain::addDetailMapFileName (const char *filename, const ATOM_Vector2f &tile, bool ignoreAlpha)
{
	if (filename)
	{
		//--- wangjian modified ---//
		// 异步加载 ：异步加载BOOL标记
		ATOM_AUTOREF(ATOM_Texture) tex = ATOM_CreateTextureResource (filename,ATOM_PIXEL_FORMAT_UNKNOWN, ATOM_LoadPriority_IMMEDIATE);
		if (tex)
		{
			_detailmaps.push_back (tex);
			_blendTextureFileNames.push_back (filename);
			_detailScales.push_back (tile);

			_alphamaps.resize ((_detailmaps.size()+3)/4);

			if (!ignoreAlpha)
			{
				float value = (_detailmaps.size() == 1) ? 1.f : 0.f;
				updateDetailAlphaValue (_blendTextureFileNames.size()-1, NULL, value);
			}

			//------------------------------------------------------------------------------//
			// wangjian added 添加默认的细节法线贴图
			_normalmaps.push_back(ATOM_GetColorTexture(ATOM_ColorARGB(0.5f,0.5f,1.f,1.f)));
			_specPowers.push_back(64.0f);
			_glossnesses.push_back(0.0f);
			_detailNormalFiles.push_back("");
			//------------------------------------------------------------------------------//

			//resetMaterialParameters ();

			return true;
		}
	}
	return false;
}

void ATOM_Terrain::removeDetailMap (unsigned index)
{
	ATOM_ASSERT(index < _blendTextureFileNames.size());
	removeDetailAlpha (index);
	_detailmaps.erase (_detailmaps.begin() + index);
	_blendTextureFileNames.erase (_blendTextureFileNames.begin() + index);
	_detailScales.erase (_detailScales.begin() + index);
	_alphamaps.resize ((_blendTextureFileNames.size()+3)/4);
	resetMaterialParameters ();
}

const char *ATOM_Terrain::getDetailMapFileName (unsigned index) const
{
	ATOM_ASSERT(index < _blendTextureFileNames.size());
	return _blendTextureFileNames[index].c_str();
}

const ATOM_Vector2f &ATOM_Terrain::getDetailMapTile (unsigned index) const
{
	ATOM_ASSERT(index < _blendTextureFileNames.size());
	return _detailScales[index];
}

void ATOM_Terrain::setDetailMapTile (unsigned index, const ATOM_Vector2f &tile)
{
	ATOM_ASSERT(index < _blendTextureFileNames.size());
	_detailScales[index] = tile;

	if (_material)
	{
		char buffer[256];
		sprintf (buffer, "detail%d_uvscale", index);
		_material->getParameterTable()->setVector (buffer, ATOM_Vector4f(tile.x, tile.y, 0.f, 0.f));
	}
}

bool ATOM_Terrain::setHeightMapFileName (const char *filename, const ATOM_Vector3f &scale)
{
	if (_heightmapFileName != filename)
	{
		_heightmapFileName = filename;

		ATOM_AutoFile f(filename, ATOM_VFS::read);
		if (!f)
		{
			return false;
		}

		unsigned fSize = f->size ();
		if ((fSize & 1) != 0)
		{
			return false;
		}

		unsigned samples = (fSize >> 1);
		unsigned heightmapW = ATOM_sqrt (samples);
		if (heightmapW * heightmapW != samples || !ATOM_ispo2(heightmapW - 1))
		{
			return false;
		}

		unsigned short *h = ATOM_NEW_ARRAY(unsigned short, samples);
		f->read (h, fSize);

		float *elevations = ATOM_NEW_ARRAY(float, samples);
		for (unsigned i = 0; i < samples; ++i)
		{
			elevations[i] = h[i] * scale.y;
		}

		if (_heightmapWidth || _heightmapHeight)
		{
			if (heightmapW != _heightmapWidth || heightmapW != _heightmapHeight || _scale != scale)
			{
				ATOM_DELETE_ARRAY(elevations);
				return false;
			}

			getHeightField()->resetHeights (elevations);

			if (_editorMode)
			{
				_heightmap->updateTexImage (elevations, 0, 0, _heightmap->getWidth(), _heightmap->getHeight(), ATOM_PIXEL_FORMAT_R32F);
			}
			ATOM_Vector3f *normals = ATOM_NEW_ARRAY(ATOM_Vector3f, getHeightField()->getSizeX() * getHeightField()->getSizeZ());
			getHeightField()->computeNormals (normals);
			createNormalMap (_normalmap.get(), normals);
			ATOM_DELETE_ARRAY(normals);
		}
		else
		{
			_heightmapWidth = heightmapW;
			_heightmapHeight = heightmapW;
			_scale = scale;

			if (_editorMode)
			{
				_heightmap = ATOM_GetRenderDevice()->allocTexture (NULL, elevations, heightmapW, heightmapW, ATOM_PIXEL_FORMAT_R32F, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOMIPMAP|ATOM_Texture::RENDERTARGET);
				ATOM_ASSERT(_heightmap);
			}

			ATOM_ASSERT(!_quadtree);
			_quadtree = ATOM_NEW(ATOM_TerrainQuadtree, this);

			if (!_quadtree->build (_patchSize, heightmapW, elevations, scale.x, scale.z, 24))
			{
				ATOM_DELETE_ARRAY(elevations);
				return false;
			}
			ATOM_Vector3f *normals = ATOM_NEW_ARRAY(ATOM_Vector3f, getHeightField()->getSizeX() * getHeightField()->getSizeZ());
			getHeightField()->computeNormals (normals);
			_normalmap = ATOM_GetRenderDevice()->allocTexture (0, 0, getHeightField()->getSizeX()-1, getHeightField()->getSizeZ()-1, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOCOMPRESS|ATOM_Texture::NOMIPMAP);
			createNormalMap (_normalmap.get(), normals);
			ATOM_DELETE_ARRAY(normals);

#if 0
			_normalmap->saveToFile ("/nmap.png");
#endif
		}

		ATOM_DELETE_ARRAY(elevations);

		resetMaterialParameters ();
	}

	return true;
}

const char *ATOM_Terrain::getHeightMapFileName (void) const
{
	return _heightmapFileName.c_str();
}

const ATOM_Vector3f &ATOM_Terrain::getHeightMapScale (void) const
{
	return _scale;
}

void ATOM_Terrain::setBaseMap (ATOM_Texture *texture)
{
	_diffusemap = texture;
	_material->getParameterTable()->setTexture ("baseMap", _diffusemap.get());
}

void ATOM_Terrain::setBaseMapFileName (const char *filename)
{
	// 如果当前已经有diffusemap， 无需生成或者加载
	if( _diffusemap )
		return;

	// 如果没有文件名 
	if (!filename || filename[0] == '\0')
	{
		//_diffusemap = 0;
		
		//char buf[ATOM_VFS::max_filename_length];
		//ATOM_GetNativePathName (_terrainFileName.c_str(), buf);
		////strcpy (buf, _terrainFileName.c_str());
		//char *p = strrchr (buf, '\\');
		//ATOM_ASSERT(p);
		//p[1] = '\0';
		//strcat (buf, "globaltexture.dds");

		generateTerrainMapForNoneDetail(filename);
	}
	else
	{
		//--- wangjian modified ---//
#if 0
		_diffusemap = ATOM_CreateTextureResource (filename, ATOM_PIXEL_FORMAT_DXT1, ATOM_LoadPriority_IMMEDIATE ); // 异步加载 ：异步加载BOOL标记
#else
		// 如果有本地文件 加载 没有 生成并且保存到本地
		generateTerrainMapForNoneDetail(filename);
#endif
		//-------------------------//
	}

	// wangjian added
	// 如果使用基本纹理替代细节纹理，则再无需细节纹理和alpha纹理
	//if( !_editorMode && !ATOM_RenderSettings::detailedTerrainEnabled() )
		//clearAllDetailSpecific();

	// 设置基本纹理
	_material->getParameterTable()->setTexture ("baseMap", _diffusemap.get());
}

bool ATOM_Terrain::setAlphaFileName (unsigned index, const char *filename)
{
	ATOM_ASSERT(index < _alphamaps.size());
	ATOM_AutoFile f(filename, ATOM_VFS::read);
	if (!f)
	{
		return false;
	}
	if (f->size() != _alphamapWidth * _alphamapHeight * 4)
	{
		return false;
	}

	ATOM_VECTOR<unsigned char> p(_alphamapWidth * _alphamapHeight * 4);
	f->read (&p[0], p.size());
	unsigned flags = ATOM_Texture::TEXTURE2D|ATOM_Texture::NOCOMPRESS|ATOM_Texture::NOMIPMAP;
	if (_editorMode)
	{
		flags |= ATOM_Texture::RENDERTARGET;
	}
	ATOM_AUTOREF(ATOM_Texture) alphamap = ATOM_GetRenderDevice()->allocTexture (0, &p[0], _alphamapWidth, _alphamapHeight, ATOM_PIXEL_FORMAT_BGRA8888, flags);
	if (!alphamap)
	{
		return false;
	}
	_alphamaps[index] = alphamap;

	return true;
}

void ATOM_Terrain::updateDetailAlphaValue (int detailIndex, const ATOM_Rect2Di *rect, float value)
{
	int alphaIndex = getAlphaIndexByDetail (detailIndex);
	int alphachannel = getAlphaChannelByDetail (detailIndex);
	ATOM_ASSERT(alphaIndex < _alphamaps.size());

	if (!_alphamaps[alphaIndex])
	{
		unsigned flags = ATOM_Texture::TEXTURE2D|ATOM_Texture::NOMIPMAP|ATOM_Texture::NOCOMPRESS;
		if (_editorMode)
		{
			flags |= ATOM_Texture::RENDERTARGET;
		}
		_alphamaps[alphaIndex] = ATOM_GetRenderDevice()->allocTexture (NULL, NULL, _alphamapWidth, _alphamapHeight, ATOM_PIXEL_FORMAT_BGRA8888, flags);
		_alphamaps[alphaIndex]->clear (0.f, 0.f, 0.f, 0.f);
	}

	ATOM_Rect2Di rc;
	if (!rect)
	{
		rc.point.x = 0;
		rc.point.y = 0;
		rc.size.w = _alphamapWidth;
		rc.size.h = _alphamapHeight;
	}
	else
	{
		rc = *rect;
	}

	unsigned char *data = ATOM_NEW_ARRAY(unsigned char, _alphamapWidth * _alphamapHeight * 4);
	_alphamaps[alphaIndex]->getTexImage (ATOM_PIXEL_FORMAT_BGRA8888, data);

	unsigned char alphavalue = 255 * value;
	for (unsigned row = 0; row < rc.size.h; ++row)
	{
		unsigned char *p = data + (row + rc.point.y) * _alphamapWidth * 4;
		for (unsigned col = 0; col < rc.size.w; ++col)
		{
			p[(col+rc.point.x)*4+alphachannel] = alphavalue;
		}
	}

	_alphamaps[alphaIndex]->updateTexImage (data, 0, 0, _alphamapWidth, _alphamapHeight, ATOM_PIXEL_FORMAT_BGRA8888);
}

void ATOM_Terrain::updateDetailAlpha (int detailIndex, const ATOM_Rect2Di *rect, const float *alphaValue)
{
	int alphaIndex = getAlphaIndexByDetail (detailIndex);
	int alphachannel = getAlphaChannelByDetail (detailIndex);
	ATOM_ASSERT(alphaIndex < _alphamaps.size());
	ATOM_ASSERT(alphaValue);

	if (!_alphamaps[alphaIndex])
	{
		unsigned flags = ATOM_Texture::TEXTURE2D|ATOM_Texture::NOMIPMAP|ATOM_Texture::NOCOMPRESS;
		if (_editorMode)
		{
			flags |= ATOM_Texture::RENDERTARGET;
		}
		_alphamaps[alphaIndex] = ATOM_GetRenderDevice()->allocTexture (NULL, NULL, _alphamapWidth, _alphamapHeight, ATOM_PIXEL_FORMAT_BGRA8888, flags);
		_alphamaps[alphaIndex]->clear (0.f, 0.f, 0.f, 0.f);
	}

	ATOM_Rect2Di rc;
	if (!rect)
	{
		rc.point.x = 0;
		rc.point.y = 0;
		rc.size.w = _alphamapWidth;
		rc.size.h = _alphamapHeight;
	}
	else
	{
		rc = *rect;
	}

	unsigned char *data = ATOM_NEW_ARRAY(unsigned char, _alphamapWidth * _alphamapHeight * 4);
	_alphamaps[alphaIndex]->getTexImage (ATOM_PIXEL_FORMAT_BGRA8888, data);

	for (unsigned row = 0; row < rc.size.h; ++row)
	{
		unsigned char *p = data + (row + rc.point.y) * _alphamapWidth * 4;
		for (unsigned col = 0; col < rc.size.w; ++col)
		{
			p[(col+rc.point.x)*4+alphachannel] = 255 * (*alphaValue++);
		}
	}
}

void ATOM_Terrain::resetMaterialParameters (void) const
{
	if (_material)
	{
		if (_editorMode)
		{
			_material->getParameterTable()->setTexture("heightMap", _heightmap.get());
			_material->getParameterTable()->setVector("invTerrainSize", ATOM_Vector4f(1.f/((_heightmap->getWidth()-1)*_scale.x), 1.f/((_heightmap->getHeight()-1)*_scale.z), 0.f, 0.f));
		}

		char buffer[ATOM_VFS::max_filename_length];
		_material->getParameterTable()->setInt ("numDetails", _detailmaps.size());
		_material->getParameterTable()->setTexture ("normalMap", _normalmap.get());
		_material->getParameterTable()->setTexture ("baseMap", _diffusemap.get());


		for (unsigned i = 0; i < _detailmaps.size(); ++i)
		{
			//-- wangjian added for test detail normal map --//
			float power = 64.0f;
			float gloss = 0.0f;
			if( _enableDetailNormal )
			{
				sprintf (buffer, "detailBump_%d", i);
				_material->getParameterTable()->setTexture (buffer, _normalmaps[i].get());

				power = _specPowers[i];
				gloss = _glossnesses[i];
			}
			//-----------------------------------------------//

			sprintf (buffer, "detail%d", i);
			_material->getParameterTable()->setTexture (buffer, _detailmaps[i].get());

			//--------------------------------------------------------------------//
			// wangjian modified : 保存gloss到z分量，保存spec power到w分量
			sprintf (buffer, "detail%d_uvscale", i);
			_material->getParameterTable()->setVector (	buffer,
														ATOM_Vector4f(_detailScales[i].x, _detailScales[i].y, gloss, power)	);
			//--------------------------------------------------------------------//
		}
		for (unsigned i = 0; i < _alphamaps.size(); ++i)
		{
			sprintf (buffer, "alpha%d", i);
			_material->getParameterTable()->setTexture (buffer, _alphamaps[i].get());
		}
		ATOM_Vector4f uvScale(0.f);
		uvScale.x = 1.f / (getScale().x * (getHeightField()->getSizeX() - 1));
		uvScale.y = 1.f / (getScale().z * (getHeightField()->getSizeZ() - 1));
		_material->getParameterTable()->setVector ("uvScale", uvScale);
	}
}

void ATOM_Terrain::removeAlphaChannel (int alphaIndex, int alphaChannel)
{
	ATOM_ASSERT(alphaIndex < _alphamaps.size());

	unsigned char *data0 = ATOM_NEW_ARRAY(unsigned char, _alphamapWidth * _alphamapHeight * 4);
	_alphamaps[alphaIndex]->getTexImage (ATOM_PIXEL_FORMAT_BGRA8888, data0);

	unsigned char *data1 = NULL;
	if (alphaIndex < _alphamaps.size()-1)
	{
		data1 = ATOM_NEW_ARRAY(unsigned char, _alphamapWidth * _alphamapHeight * 4);
		_alphamaps[alphaIndex+1]->getTexImage (ATOM_PIXEL_FORMAT_BGRA8888, data1);
	}

	for (unsigned i = 0; i < _alphamapWidth * _alphamapHeight; ++i)
	{
		for (unsigned n = alphaChannel; n < 3; ++n)
		{
			data0[i*4+n] = data0[i*4+n+1];
		}

		if (data1)
		{
			data0[i*4+3] = data1[i*4];
		}
		else
		{
			data0[i*4+3] = 0;
		}
	}

	_alphamaps[alphaIndex]->updateTexImage (data0, 0, 0, _alphamapWidth, _alphamapHeight, ATOM_PIXEL_FORMAT_BGRA8888);
	ATOM_DELETE_ARRAY(data0);
	ATOM_DELETE_ARRAY(data1);

	if (alphaIndex < _alphamaps.size()-1)
	{
		removeAlphaChannel (alphaIndex+1, 0);
	}
}

void ATOM_Terrain::removeDetailAlpha (int detailIndex)
{
	int alphaIndex = getAlphaIndexByDetail (detailIndex);
	int alphaChannel = getAlphaChannelByDetail (detailIndex);
	removeAlphaChannel (alphaIndex, alphaChannel);
}

int ATOM_Terrain::getAlphaIndexByDetail (int detail) const
{
	return detail / 4;
}

int ATOM_Terrain::getAlphaChannelByDetail (int detail) const
{
	return detail % 4;
}

bool ATOM_Terrain::save (const char *filename)
{
	if (!_editorMode)
	{
		return false;
	}

	ATOM_STRING fn = filename ? filename : getTerrainFileName();
	if (fn.empty ())
	{
		return false;
	}

	char buf[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (fn.c_str(), buf);

	ATOM_TiXmlDocument doc(buf);
	ATOM_TiXmlDeclaration decl("1.0", "utf-8", "");
	doc.InsertEndChild (decl);

	ATOM_TiXmlElement eTerrain("Terrain");
	eTerrain.SetAttribute ("HeightMap", _heightmapFileName.c_str());
	eTerrain.SetAttribute ("AlphaMapWidth", _alphamapWidth);
	eTerrain.SetAttribute ("AlphaMapHeight", _alphamapHeight);
	eTerrain.SetDoubleAttribute ("ScaleX", _scale.x);
	eTerrain.SetDoubleAttribute ("ScaleY", _scale.y);
	eTerrain.SetDoubleAttribute ("ScaleZ", _scale.z);
	eTerrain.SetAttribute ("PatchSize", 33);
	//---------------------------------------//
	// wangjian added for test detail normals
	int detailNormalCount = 0;
	for( int i = 0; i < _detailNormalFiles.size(); ++i )
	{
		if( !_detailNormalFiles[i].empty() )
			detailNormalCount++;
	}
	eTerrain.SetAttribute("DetailNormalCount",detailNormalCount);
	//---------------------------------------//

	if (_diffusemap)
	{
		strcpy (buf, fn.c_str());
		char *p = strrchr (buf, '/');
		ATOM_ASSERT(p);
		p[1] = '\0';
		strcat (buf, "globaltexture.dds");
		_diffusemap->saveToFileEx (buf, ATOM_PIXEL_FORMAT_DXT1);
		eTerrain.SetAttribute ("GlobalTexture", buf);
	}

	if (!_blendTextureFileNames.empty ())
	{
		ATOM_TiXmlElement eDetails("Details");
		for (unsigned i = 0; i < _blendTextureFileNames.size(); ++i)
		{
			ATOM_TiXmlElement eDetail("Detail");

			// diffuse map
			eDetail.SetAttribute ("FileName", _blendTextureFileNames[i].c_str());

			//--------------------- wangjian added ---------------------------------------//
			if( !_detailNormalFiles[i].empty() )
			{
				eDetail.SetAttribute ("NormalFileName", _normalmaps[i]->getObjectName());	// detail normal map
				eDetail.SetDoubleAttribute ("SpecPower", _specPowers[i]);					
				eDetail.SetDoubleAttribute ("Gloss", _glossnesses[i]);
			}
			//----------------------------------------------------------------------------//

			// UV SCALE
			eDetail.SetDoubleAttribute ("ScaleU", _detailScales[i].x);
			eDetail.SetDoubleAttribute ("ScaleV", _detailScales[i].y);


			eDetails.InsertEndChild (eDetail);
		}
		eTerrain.InsertEndChild (eDetails);
	}

	char path[256];
	strcpy (path, fn.c_str());
	char *p = strrchr (path, '/');
	ATOM_ASSERT(p);
	p[1] = '\0';

	if (!_alphamaps.empty ())
	{
		ATOM_TiXmlElement eAlphas("Alphas");

		char alphaname[256];
		ATOM_BaseImage img;
		for (unsigned i = 0; i < _alphamaps.size(); ++i)
		{
			ATOM_TiXmlElement eAlpha("Alpha");
			sprintf (alphaname, "%salpha%d.raw", path, i);
			eAlpha.SetAttribute ("FileName", alphaname);
			eAlphas.InsertEndChild (eAlpha);

			ATOM_AutoFile f(alphaname, ATOM_VFS::write);
			if (!f)
			{
				return false;
			}
			img.init (_alphamaps[i]->getWidth(), _alphamaps[i]->getHeight(), ATOM_PIXEL_FORMAT_BGRA8888);
			_alphamaps[i]->getTexImage (ATOM_PIXEL_FORMAT_BGRA8888, img.getData());
			f->write (img.getData(), img.getBufferSize());
		}
		eTerrain.InsertEndChild (eAlphas);
	}

	ATOM_VECTOR<ATOM_Vector3f> grassPositions;
	if (!_vegetations.empty ())
	{
		ATOM_TiXmlElement eVegetations("Vegetations");
		for (unsigned i = 0; i < _vegetations.size(); ++i)
		{
			char buffer[256];
			sprintf (buffer, "%svegetation%d.dat", path, i);

			ATOM_TiXmlElement eVegetation("Vegetation");
			eVegetation.SetDoubleAttribute ("Width", _vegetations[i].width);
			eVegetation.SetDoubleAttribute ("Height", _vegetations[i].height);
			eVegetation.SetAttribute ("DensityFileName", buffer);
			eVegetation.SetAttribute ("TextureFileName", _vegetations[i].textureFileName.c_str());
			eVegetations.InsertEndChild (eVegetation);

			ATOM_AutoFile f(buffer, ATOM_VFS::write);
			if (!f)
			{
				return false;
			}

			unsigned num = 0;
			if (_grassLayers[i])
			{
				_grassLayers[i]->getGrassPositions (grassPositions);
				num = grassPositions.size();
			}
			f->write (&num, sizeof(unsigned));
			if (num > 0)
			{
				f->write (&grassPositions[0], num*sizeof(ATOM_Vector3f));
			}
		}
		eTerrain.InsertEndChild (eVegetations);
	}

	doc.InsertEndChild (eTerrain);

	if (!doc.SaveFile ())
	{
		return false;
	}

	if (!_heightmapFileName.empty ())
	{
		ATOM_AutoFile f(_heightmapFileName.c_str(), ATOM_VFS::write);
		if (!f)
		{
			return false;
		}

		if (_heightmap)
		{
			unsigned numHeights = _heightmap->getWidth() * _heightmap->getHeight();
			float *heights = ATOM_NEW_ARRAY(float, numHeights);
			_heightmap->getTexImage (ATOM_PIXEL_FORMAT_R32F, heights);
			unsigned short *shortData = ATOM_NEW_ARRAY(unsigned short, numHeights);
			for (unsigned i = 0; i < numHeights; ++i)
			{
				shortData[i] = ATOM_ftol(heights[i] / _scale.y);
			}
			ATOM_DELETE_ARRAY(heights);

			f->write (shortData, numHeights * sizeof(unsigned short));
			ATOM_DELETE_ARRAY(shortData);
		}
	}

	return true;
}

GrassCluster *ATOM_Terrain::createGrassCluster (VegetationInfo *info, unsigned count)
{
	bool editorMode = ATOM_Terrain::isEditorModeEnabled ();
	ATOM_HeightField *heightfield = _quadtree->getHeightField();

	ATOM_VECTOR<ATOM_BaseImage> grassImages(count);
	ATOM_AUTOREF(ATOM_Texture) combinedTexture;

	for (unsigned i = 0; i < count; ++i)
	{
		const char *textureFileName = info[i].textureFileName.c_str();

		ATOM_AutoFile fimage(textureFileName, ATOM_VFS::read);
		if (!fimage)
		{
			return NULL;
		}
		ATOM_BaseImage &image = grassImages[i];
		if (!image.load (fimage, ATOM_PIXEL_FORMAT_BGRA8888))
		{
			return NULL;
		}
	}

	ImagePacker packer(2, 2);
	for (unsigned i = 0; i < grassImages.size(); ++i)
	{
		packer.putImage (i, grassImages[i].getWidth(), grassImages[i].getHeight(), grassImages[i].getData());
	}
	float packW = packer.getWidth();
	float packH = packer.getHeight();
	combinedTexture = ATOM_GetRenderDevice()->allocTexture (0, packer.getImage(), packer.getWidth(), packer.getHeight(), ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOCOMPRESS);

	for (unsigned i = 0; i < grassImages.size(); ++i)
	{
		const ImagePacker::Rect *rc = packer.getImage (i);
		info[i].tc.x = rc->x/packW;
		info[i].tc.y = rc->y/packH;
		info[i].tc.z = (rc->x+rc->w)/packW;
		info[i].tc.w = (rc->y+rc->h)/packH;
	}

	GrassCluster *grassCluster = ATOM_NEW(GrassCluster, this);
	grassCluster->initialize (32 * heightfield->getSpacingX(), 32 * heightfield->getSpacingZ(), (heightfield->getSizeX()-1)/32, (heightfield->getSizeZ()-1)/32, combinedTexture.get());
	ATOM_Material *grassMaterial = grassCluster->getMaterial();
	if (!grassMaterial)
	{
		ATOM_DELETE(grassCluster);
		return NULL;
	}
	ATOM_Vector4f invTerrainSize(1.f/(getHeightField()->getSizeX()*getHeightField()->getSpacingX()), 1.f/(getHeightField()->getSizeZ()*getHeightField()->getSpacingZ()), 0.f, 0.f);
	ATOM_Vector4f windDir (0.f, 0.f, 1.f, 0.f);
	const float windStrength = 1.5f;
	const float fadeOutDistance = 1024.f;
	grassMaterial->getParameterTable()->setVector ("invTerrainSize", invTerrainSize);
	grassMaterial->getParameterTable()->setVector("windDir", windDir);
	grassMaterial->getParameterTable()->setFloat ("windStrength", windStrength);
	grassMaterial->getParameterTable()->setFloat ("invFadeoutDistance", fadeOutDistance);
	grassMaterial->getParameterTable()->setTexture ("grassTexture", combinedTexture.get());

	ATOM_Vector3f *normals = ATOM_NEW_ARRAY(ATOM_Vector3f, getHeightField()->getSizeX() * getHeightField()->getSizeZ());
	getHeightField()->computeNormals (normals);
	for (unsigned i = 0; i < count; ++i)
	{
		float w = info[i].width;
		float h = info[i].height;
		const char *dataFileName = info[i].densityFileName.c_str();

		int numPoints;
		ATOM_VECTOR<ATOM_Vector3f> positions;
		ATOM_VECTOR<ATOM_Vector3f> normal;

		ATOM_AutoFile f(dataFileName, ATOM_VFS::read);
		if (f)
		{
			f->read (&numPoints, sizeof(int));
			if (numPoints > 0)
			{
				positions.resize(numPoints);
				normal.resize(numPoints);
				f->read (&positions[0], numPoints * sizeof(ATOM_Vector3f));
				for (unsigned n = 0; n < numPoints; ++n)
				{
					positions[n].y = editorMode ? 0.f : heightfield->getRealHeight(positions[n].x, positions[n].z);
					int dimx = ATOM_ftol(positions[n].x / heightfield->getSpacingX());
					int dimz = ATOM_ftol(positions[n].z / heightfield->getSpacingZ());
					dimx %= heightfield->getSizeX();
					dimz %= heightfield->getSizeZ();
					normal[n] = normals[dimz * heightfield->getSizeX() + dimx];
				}
				grassCluster->addLayer(
					ATOM_Vector2f(info[i].tc.x, info[i].tc.y),
					ATOM_Vector2f(info[i].tc.z, info[i].tc.w),
					w, 
					h, 
					numPoints, 
					&positions[0], 
					&normal[0], 
					_grassSizeVMin, 
					_grassSizeVMax
					);
			}
		}

		if (grassCluster && !grassCluster->initVertexArrays ())
		{
			ATOM_DELETE(grassCluster);
			ATOM_DELETE_ARRAY(normals);
			return NULL;
		}
	}

	ATOM_DELETE_ARRAY(normals);

	return grassCluster;
}

unsigned ATOM_Terrain::getNumGrassLayers (void) const
{
	return _vegetations.size();
}

const ATOM_Terrain::VegetationInfo &ATOM_Terrain::getGrassLayerInfo (unsigned index) const
{
	return _vegetations[index];
}

bool ATOM_Terrain::appendGrassLayer (const VegetationInfo &info)
{
	_vegetations.push_back (info);
	_grassLayers.push_back (createGrassCluster (&_vegetations.back(), 1));
	return true;
}

void ATOM_Terrain::removeGrassLayer (unsigned index)
{
	ATOM_DELETE(_grassLayers[index]);
	_grassLayers.erase (_grassLayers.begin() + index);
	_vegetations.erase (_vegetations.begin() + index);
}

void ATOM_Terrain::setGrassLayerSize (unsigned index, float width, float height)
{
	_vegetations[index].width = width;
	_vegetations[index].height = height;

	if (_grassLayers[index])
	{
		_grassLayers[index]->updateSize (width, height);
	}
}

void ATOM_Terrain::removeRandomGrassObjects (unsigned index, const ATOM_Vector3f &position, float radius, unsigned count)
{
	const VegetationInfo &info = _vegetations[index];
	float rx = radius * _scale.x;
	float rz = radius * _scale.z;

	_grassLayers[index]->removeRandomGrassObject (position, rx, rz, count);
}

void ATOM_Terrain::addRandomGrassObjects (unsigned index, const ATOM_Vector3f &position, float radius, unsigned count, const ATOM_Vector3f *normals)
{
	ATOM_HeightField *heightfield = getHeightField();
	for (unsigned i = 0; i < count; ++i)
	{
		const VegetationInfo &info = _vegetations[index];
		float rx = radius * _scale.x;
		float rz = radius * _scale.z;

		ATOM_Vector3f v = position + ATOM_Vector3f(ATOM_randomf (-rx, rx), 0.f, ATOM_randomf (-rz, rz));
		if (_editorMode)
		{
			v.y = 0.f;
		}

		int dimx = ATOM_ftol(v.x / heightfield->getSpacingX());
		int dimz = ATOM_ftol(v.z / heightfield->getSpacingZ());
		dimx %= heightfield->getSizeX();
		dimz %= heightfield->getSizeZ();
		ATOM_Vector3f normal = normals[dimz * heightfield->getSizeX() + dimx];

		_grassLayers[index]->addGrassObject (
			v, 
			info.width, 
			info.height, 
			_grassSizeVMin, 
			_grassSizeVMax, 
			ATOM_Vector2f(info.tc.x, info.tc.y),
			ATOM_Vector2f(info.tc.z, info.tc.w),
			normal
			);
	}
}

unsigned ATOM_Terrain::getAlphaMapWidth (void) const
{
	return _alphamapWidth;
}

unsigned ATOM_Terrain::getAlphaMapHeight (void) const
{
	return _alphamapHeight;
}

//--- wangjian added ---//
void ATOM_Terrain::resetMaterialDirtyFlag_impl()
{
	if( _material )
		_material->getParameterTable()->resetDirtyFlag();
}


void drawQuad (ATOM_RenderDevice *device, ATOM_Material *material, int w, int h)
{
	float deltax = (float)0.5f/(float)w;
	float deltay = (float)0.5f/(float)h;

	// copy old value
	{
		float vertices[4 * 5] = {
			-1.f - deltax, -1.f + deltay, 0.f, 0.f, 1.f,
			1.f - deltax, -1.f + deltay, 0.f, 1.f, 1.f,
			1.f - deltax,  1.f + deltay, 0.f, 1.f, 0.f,
			-1.f - deltax,  1.f + deltay, 0.f, 0.f, 0.f
		};
		unsigned short indices[4] = {
			0, 1, 2, 3
		};

		ATOM_RenderDevice *device = ATOM_GetRenderDevice();
		unsigned numPasses = material->begin (device);
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (material->beginPass (device, pass))
			{
				device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_FAN, 4, 4, ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_TEX1_2, 5 * sizeof(float), vertices, indices);
				material->endPass (device, pass);
			}
		}
		material->end (device);
	}
}
void ATOM_Terrain::generateTerrainMapForNoneDetail( const char* filename )
{
	// 检查文件是否已经存在于本地目录
	char buf[ATOM_VFS::max_filename_length] = {0};
	strcpy (buf, filename);
	char *p = strrchr (buf, '/');
	char* fn = p;
	if( fn )
	{
		fn++;
	}
	
	char native[ATOM_VFS::max_filename_length]={0};
	
	ATOM_GetNativePathName( ( fn && _editorMode ) ? fn : filename, native);
	bool bExistInDisk = ATOM_PhysicVFS().doesFileExists (native);

	// 存在 直接读取加载
	char buffer[ATOM_VFS::max_filename_length];
	if ( !ATOM_CompletePath ( ( fn && _editorMode ) ? fn : filename, buffer ) )
	{
		return;
	}

	if(bExistInDisk)
	{
		
		_diffusemap = ATOM_CreateTextureResource (buffer, ATOM_PIXEL_FORMAT_DXT1, ATOM_LoadPriority_ASYNCBASE );
	}
	// 不存在 自动生成 并且之后保存到本地
	else
	{
		ATOM_RenderDevice *device = ATOM_GetRenderDevice();

		// 材质
		if( !_basemapGenMaterial )
		{
			_basemapGenMaterial = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/tt_basemap.mat");
			ATOM_ASSERT(_basemapGenMaterial);
		}
		_basemapGenMaterial->setActiveEffect ("default");

		int width = 2048;
		int	height = 2048;

	#if 1
		// 根据地形实际尺寸确定basemap的尺寸[ 如果按2560 X 2560的地形使用2048X2048的basemap , texel ratio : 2048 / 2560 = 0.8 ]
		const float ratio = 1.6f;
		const ATOM_BBox & bbox = getWorldBoundingbox();
		const ATOM_Vector3f extent_terrain = bbox.getMax() - bbox.getMin();
		const int width_terrain = (int)extent_terrain.x;
		const int height_terrain = (int)extent_terrain.z;
		width = width_terrain * ratio;
		height = height_terrain * ratio;
	#endif

		if( width < 1 )
			width = 1;
		if( width > 2048 )
			width = 2048;

		if( height < 1 )
			height = 1;
		if( height > 2048 )
			height = 2048;

		ATOM_AUTOREF(ATOM_Texture) tex = device->allocTexture (NULL, NULL, width, height, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);
		
		//------------------------------------------------------------//

		ATOM_AUTOREF(ATOM_DepthBuffer) depth = device->allocDepthBuffer (width, height);
		if (tex && depth)
		{

			ATOM_AUTOREF(ATOM_DepthBuffer) oldDepth = device->getDepthBuffer ();
			ATOM_AUTOREF(ATOM_Texture) oldRT = device->getRenderTarget (0);
			ATOM_Rect2Di oldVP = device->getViewport (NULL);

			int vpW = width;
			int vpH = height;
			device->setRenderTarget(0, tex.get());
			device->setViewport (NULL, ATOM_Rect2Di(0, 0, vpW, vpH));
			device->setDepthBuffer (depth.get());
			device->beginFrame ();

			char buffer[256];
			for (int i = 0; i < 12; ++i)
			{
				sprintf (buffer, "detail%d", i);
				ATOM_Texture *detailMap = i < getNumDetailMaps() ? getDetailMap (i) : ATOM_GetColorTexture(0);
				_basemapGenMaterial->getParameterTable()->setTexture (buffer, detailMap);

				sprintf (buffer, "detail%d_uvscale", i);
				ATOM_Vector4f detailUVScale = i < getNumDetailMaps() ? getDetailScale(i) : ATOM_Vector4f(1,1,1,1);
				_basemapGenMaterial->getParameterTable()->setVector (buffer, detailUVScale);
			}
			for (int i = 0; i < 3; ++i)
			{
				sprintf (buffer, "alpha%d", i);
				ATOM_Texture *alphaMap = i < getNumAlphaMaps() ? getAlphaMap (i) : ATOM_GetColorTexture(0);
				_basemapGenMaterial->getParameterTable()->setTexture (buffer, alphaMap);
			}

			drawQuad (device, _basemapGenMaterial.get(), vpW, vpH);

			device->endFrame ();

			device->setDepthBuffer (oldDepth.get());
			device->setRenderTarget (0, oldRT.get());
			device->setViewport (NULL, oldVP);
		}

		// 保存到场景文件夹下
		tex->saveToFileEx(buffer,ATOM_PIXEL_FORMAT_DXT1);
		// 同步加载进来
		_diffusemap = ATOM_CreateTextureResource (buffer, ATOM_PIXEL_FORMAT_DXT1, ATOM_LoadPriority_IMMEDIATE );
	}
}
void ATOM_Terrain::clearAllDetailSpecific()
{
	_detailmaps.clear();
	_alphamaps.clear();
}

void ATOM_Terrain::setDetailNormalMap(ATOM_STRING & filename,int detailID)
{
	ATOM_ASSERT( _detailNormalFiles.size()==_normalmaps.size() );

	if( detailID<0 || detailID >= _normalmaps.size())
		return;

	if( !filename.empty() )
	{
		_detailNormalFiles[detailID] = filename;
		_normalmaps[detailID] = ATOM_CreateTextureResource (filename.c_str(),
															ATOM_PIXEL_FORMAT_UNKNOWN, 
															ATOM_LoadPriority_IMMEDIATE);

		enableDetailNormal(true);
	}
	else
	{
		_detailNormalFiles[detailID] = "";
		_normalmaps[detailID] = ATOM_GetColorTexture(ATOM_ColorARGB(0.5f,0.5f,1.f,1.f));
	}

	resetMaterialParameters ();
}
ATOM_STRING ATOM_Terrain::getDetailNormalMap(int detailID) const
{
	if( detailID<0 || detailID >= _detailNormalFiles.size())
		return "";
	return _detailNormalFiles[detailID];
}
void ATOM_Terrain::setDetailSpecPower(float power, int detailID)
{
	if( detailID<0 || detailID >= _specPowers.size())
		return;
	_specPowers[detailID] = power;
	resetMaterialParameters ();
}
float ATOM_Terrain::getDetailSpecPower(int detailID)
{
	if( detailID<0 || detailID >= _specPowers.size())
		return 64.f;
	return _specPowers[detailID];
}
void ATOM_Terrain::setDetailGloss(float gloss, int detailID)
{
	if( detailID<0 || detailID >= _glossnesses.size())
		return;
	_glossnesses[detailID] = gloss;
	resetMaterialParameters ();
}
float ATOM_Terrain::getDetailGloss(int detailID)
{
	if( detailID<0 || detailID >= _glossnesses.size())
		return 0.0f;
	return _glossnesses[detailID];
}
void ATOM_Terrain::checkEnableDetailNormal()
{
	// 是否开启细节纹理
	bool bEnableDetail = ATOM_RenderSettings::detailedTerrainEnabled();
	if( !bEnableDetail )
	{
		enableDetailNormal(false);
		return;
	}

	// 是否开启细节法线纹理
	enableDetailNormal(ATOM_RenderSettings::isTerrDetailNormalEnabled());
}

void ATOM_Terrain::enableDetailNormal(bool enable)
{
	if( _enableDetailNormal != enable )
	{
		_enableDetailNormal = enable;

		const char *materialFileName = "";
		if( enable )
			materialFileName = _editorMode ? "/editor/materials/terrain_detailbump.mat" : "/materials/builtin/terrain_detailbump.mat";
		else
			materialFileName = _editorMode ? "/editor/materials/terrain.mat" : "/materials/builtin/terrain.mat";
		_material = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), materialFileName);
		if (!_material)
		{
			return;
		}

		processDetailNormal(enable);
		resetMaterialParameters();
	}
}

void ATOM_Terrain::processDetailNormal(bool enable)
{
	// 如果开启细节法线，加载法线纹理
	if( enable )
	{
		ATOM_ASSERT( _detailNormalFiles.size()==_normalmaps.size() );
		for( int i = 0; i < _detailNormalFiles.size(); ++i )
		{
			if( _detailNormalFiles[i].empty() )
				continue;

			ATOM_AUTOREF(ATOM_Texture) tex = ATOM_CreateTextureResource (	_detailNormalFiles[i].c_str(),
																			ATOM_PIXEL_FORMAT_UNKNOWN, 
																			ATOM_LoadPriority_IMMEDIATE	);
			if (tex)
			{
				_normalmaps[i] = tex;
			}
		}
	}
	else
	{
		for( int i = 0; i < _normalmaps.size(); ++i )
		{
			_normalmaps[i] = ATOM_GetColorTexture( ATOM_ColorARGB(0.5f,0.5f,1.f,1.f) );
		}
	}
}
//----------------------//
