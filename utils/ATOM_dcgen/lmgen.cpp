#include "lmgen.h"
#include "dcmaterials.h"
#include "dcvisitor.h"
#include "misc.h"

SceneLightmapGen::SceneLightmapGen (void)
{
	_scene = 0;
	_terrainLightmapSize = 1024;
	_objectLightmapSize = 16;
	_terrainLightmapBlurFactor = 1;
}

SceneLightmapGen::~SceneLightmapGen (void)
{
}

void SceneLightmapGen::setScene (ATOM_SDLScene *scene)
{
	_scene = scene;
}

void SceneLightmapGen::setTerrainLightmapFileName (const char *filename)
{
	_terrainLightmapFileName = filename;
}

void SceneLightmapGen::setTerrainLightmapBlurFactor (unsigned n)
{
	_terrainLightmapBlurFactor = n;
}

void SceneLightmapGen::setTerrainLightmapSize (unsigned size)
{
	_terrainLightmapSize = size;
}

void SceneLightmapGen::setMeshLightmapPath (const char *path)
{
	_objectLightmapPath = path;
}

void SceneLightmapGen::setMeshLightmapSize (unsigned size)
{
	_objectLightmapSize = size;
}

bool SceneLightmapGen::generate (void)
{
	if (!_scene)
	{
		return false;
	}

	// save device states
	pushDeviceStates ();

	// save old fog state and turn off fogging
	bool fogState = _scene->isFogEnabled ();
	_scene->enableFog (false);

	// find terrain node
	ATOM_AUTOREF(ATOM_Terrain) terrain;
	FindTerrainVisitor ftv;
	ftv.traverse (*_scene->getRootNode ());
	terrain = ftv.terrain;

	// generate terrain lightmap
	if (terrain && !_terrainLightmapFileName.empty () && _terrainLightmapSize > 0)
	{
		ATOM_LOGGER::log ("Generating terrain lightmap of size %d...", _terrainLightmapSize);
		TerrainLightmapGen tlg;
		tlg.setTerrain (terrain.get());
		tlg.setScene (_scene);
		ATOM_AUTOREF(ATOM_Texture) lm = tlg.generate (_terrainLightmapSize, _terrainLightmapBlurFactor);
		if (lm && lm->saveToFile (_terrainLightmapFileName.c_str()))
		{
			terrain->setTerrainLightmapFileName (_terrainLightmapFileName);
		}
		ATOM_LOGGER::log ("OK\n");
	}

	// restore fog state
	_scene->enableFog (fogState);

	// restore device states
	popDeviceStates ();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TerrainLightmapGen::TerrainLightmapGen (void)
{
	_scene = 0;
}

TerrainLightmapGen::~TerrainLightmapGen (void)
{
}

void TerrainLightmapGen::setTerrain (ATOM_Terrain *terrain)
{
	_terrain = terrain;
}

void TerrainLightmapGen::setScene (ATOM_SDLScene *scene)
{
	_scene = scene;
}

ATOM_AUTOREF(ATOM_Texture) TerrainLightmapGen::generateTerrainNormalmap (unsigned size)
{
	unsigned terrainSize = _terrain->getHeightField()->getSize ();
	ATOM_VECTOR<ATOM_Vector3f> normals(terrainSize * terrainSize);

	_terrain->getHeightField()->computeNormals (&normals[0]);

	ATOM_VECTOR<ATOM_ColorARGB> normalToColors(terrainSize * terrainSize);
	for (unsigned i = 0; i < terrainSize * terrainSize; ++i)
	{
		ATOM_Vector3f v = normals[i] * 0.5f + ATOM_Vector3f(0.5f, 0.5f, 0.5f);
		v.x = ATOM_saturate (v.x);
		v.y = ATOM_saturate (v.y);
		v.z = ATOM_saturate (v.z);
		normalToColors[i].setFloats (v.x, v.y, v.z, 1.f);
	}

	ATOM_BaseImage image(terrainSize, terrainSize, ATOM_PIXEL_FORMAT_BGRA8888, &normalToColors[0], 1);
	image.resize (size, size);

	ATOM_AUTOREF(ATOM_Texture) normalmap = ATOM_GetRenderDevice()->allocTexture (0, image.getData(), size, size, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D);

	return normalmap;
}

ATOM_AUTOREF(ATOM_Texture) TerrainLightmapGen::generate (unsigned size, unsigned blurFactor)
{
	unsigned theSize = size;
	size = 2048;

	ATOM_AUTOREF(ATOM_Texture) normalmap = generateTerrainNormalmap (1024);
	_terrain->setNormalMap (normalmap.get());

	ATOM_Vector3f lightDir = -_scene->getLight()->getDirection();
	lightDir.set (1.f, 1.f, 1.f);
	lightDir.normalize ();

	ATOM_Vector4f lightColor = _scene->getLight()->getColor4f();
	ATOM_Vector4f ambient = _scene->getAmbientLight ();
	//ambient.set(0.4f, 0.4f, 0.4f, 1.f);
	ATOM_ColorARGB ambientARGB(ambient.x, ambient.y, ambient.z, 1.f);

	const unsigned renderTargetSize = 1024;
	const unsigned subDiv = size > 512 ? size * 2 / 1024 : 1;

	ATOM_BBox terrainBoundingbox = _terrain->getWorldBoundingbox (ATOM_Matrix4x4f::getIdentityMatrix(), ATOM_Matrix4x4f::getIdentityMatrix());
	float radius = terrainBoundingbox.getDiagonalSize() * 0.5f;
	ATOM_Vector3f minpt = terrainBoundingbox.getMin ();
	ATOM_Vector3f maxpt = terrainBoundingbox.getMax ();
	float stepX = (maxpt.x - minpt.x) / float(subDiv);
	float stepY = (maxpt.y - minpt.y) / float(subDiv);
	float stepZ = (maxpt.z - minpt.z) / float(subDiv);
	float totalRadius = terrainBoundingbox.getDiagonalSize() * 0.5f;
	float stepRadius = totalRadius / float(subDiv);
	stepRadius += 100.f;

	ATOM_RenderDevice *device = ATOM_GetRenderDevice();
	ATOM_AUTOREF(ATOM_Texture) target = device->allocTexture (0, 0, renderTargetSize, renderTargetSize, ATOM_PIXEL_FORMAT_RGBA32F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET);

	ATOM_VECTOR<ATOM_Vector4f> textureContent (renderTargetSize * renderTargetSize);
	ATOM_VECTOR<ATOM_ColorARGB> lightmapBuffer(size * size);
	for (unsigned i = 0; i < size * size; ++i)
	{
		lightmapBuffer[i] = ambientARGB;
	}

	for (unsigned x = 0; x < subDiv; ++x)
		for (unsigned y = 0; y < subDiv; ++y)
			for (unsigned z = 0; z < subDiv; ++z)
			{
				pushDeviceStates ();
				device->setClearColor (device->getCurrentView(), 1.f, 1.f, 1.f, 1.f);
				device->setRenderTarget (0, target.get());

				ATOM_Vector3f center;
				center.x = minpt.x + (x + 0.5f) * stepX;
				center.y = minpt.y + (y + 0.5f) * stepY;
				center.z = minpt.z + (z + 0.5f) * stepZ;
				ATOM_Vector3f eye = center + lightDir * 2 * totalRadius;
				_scene->getCamera()->setOrtho (-stepRadius, stepRadius, -stepRadius, stepRadius, 1, 4 * totalRadius + 100.f);
				_scene->getCamera()->lookAt (eye, center, ATOM_Vector3f(0.f, 1.f, 0.f));
				_scene->getCamera()->setViewport (0, 0, target->getWidth(), target->getHeight());

				device->setColorWriteEnable (false, false, false, false);
				_scene->getCamera()->setOrtho (-stepRadius, stepRadius, -stepRadius, stepRadius, 1, 4 * totalRadius + 100);
				_scene->getCamera()->lookAt (eye, center, ATOM_Vector3f(0.f, 1.f, 0.f));
				_scene->render (device, true);

				DC_CullVisitor cullVisitor;
				cullVisitor.mask = DC_CullVisitor::CM_TERRAIN;
				ATOM_CullVisitor *oldCullvisitor = _scene->getCullVisitor();
				_scene->setCullVisitor (&cullVisitor);
				device->setColorWriteEnable (true, true, true, true);
				ATOM_Terrain::RenderMode renderMode = _terrain->getRenderMode ();
				_terrain->setRenderMode (ATOM_Terrain::RM_UVNORMAL);
				_scene->render (device);
				_terrain->setRenderMode (renderMode);
				_scene->setCullVisitor (oldCullvisitor);

				target->getTexImage (ATOM_PIXEL_FORMAT_RGBA32F, &textureContent[0]);

				for (unsigned n = 0; n < renderTargetSize * renderTargetSize; ++n)
				{
					const ATOM_Vector4f &v = textureContent[n];
					if (v.z != 1.f || v.w != 1.f)
					{
						float tu = v.x;
						float tv = v.y;
						float nx = v.z;
						float nz = v.w;
						float ny = ATOM_sqrt(1.f - nx * nx - nz * nz);
						unsigned texPosX = ATOM_ftol(size * tu + 0.5f);
						if (texPosX > size-1)
						{
							texPosX = size-1;
						}
						unsigned texPosY = ATOM_ftol(size * tv + 0.5f);
						if (texPosY > size-1)
						{
							texPosY = size-1;
						}
						float factor = dotProduct (lightDir, ATOM_Vector3f(nx, ny, nz));
						if (factor < 0.f) factor = 0.f;
						ATOM_Vector4f color = ambient + factor * lightColor;
						color.x = ATOM_saturate (color.x);
						color.y = ATOM_saturate (color.y);
						color.z = ATOM_saturate (color.z);
						//if (color.x < 0.2f || color.y < 0.2f || color.z < 0.2)
						//{
						//	__asm int 3
						//}
						lightmapBuffer[texPosX + size * texPosY].setFloats (color.x, color.y, color.z, 1.f);
						//if (lightmapBuffer[texPosX + size * texPosY] == ATOM_ColorARGB(0))
						//{
						//	__asm int 3
						//}
					}
				}

				popDeviceStates ();
				//target->SaveToFile ("/xxx.dds");
			}

	ATOM_BaseImage image(size, size, ATOM_PIXEL_FORMAT_BGRX8888, &lightmapBuffer[0], 1);

	image.resize (theSize, theSize);

	for (unsigned i = 0; i < blurFactor; ++i)
	{
		image.resize (theSize, theSize);
	}

	ATOM_AUTOREF(ATOM_Texture) tmpTexture = device->allocTexture (0, image.getData(), theSize, theSize, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D);
	
	return tmpTexture;
}

