#if 0

#include <ATOM_math.h>
#include <ATOM_utils.h>
#include <ATOM_vfs.h>
#include <ATOM_kernel.h>
#include <ATOM_geometry.h>
#include <ATOM_image.h>
#include "heightfield_aogen.h"

using namespace math;
using namespace utils;
using namespace kernel;
using namespace vfs;
using namespace geometry;
using namespace engine;
using namespace render;
using namespace image;

Terrain_AOGenerator::Terrain_AOGenerator (void)
{
	_callback = 0;
	_callbackData = 0;
}

Terrain_AOGenerator::Terrain_AOGenerator (const char *sceneFileName, const char *bentnormalMap, void (__cdecl *callback)(void*, int), void *userdata)
{
	_bentnormalMap = bentnormalMap ? bentnormalMap : "";
	_sceneFileName = sceneFileName ? sceneFileName : "";
	setProcessCallback (callback, userdata);
}

void Terrain_AOGenerator::setBentNormalMapFileName (const char *bentnormalMapFileName)
{
	_bentnormalMap = bentnormalMapFileName ? bentnormalMapFileName : "";
}

void Terrain_AOGenerator::setProcessCallback (void (__cdecl *callback)(void *userdata, int percent), void *userdata)
{
	_callback = callback;
	_callbackData = userdata;
}

const char *Terrain_AOGenerator::getErrorMessage (void) const
{
	return _errorMessage.c_str();
}

class TerrainFinderVisitor: public ATOM_Visitor
{
public:
	virtual void visit (ATOM_Node &node) {}
	virtual void visit (ATOM_Terrain &node) { terrain = &node; }
	ATOM_AUTOREF(ATOM_Terrain) terrain;
};

class MaxATOM_BBoxFinder: public ATOM_Visitor
{
public:
	float maxATOM_BBoxValue;
	ATOM_STRING modelFileName;

public:
	MaxATOM_BBoxFinder (void)
	{
		maxATOM_BBoxValue = 0;
	}

	virtual void visit (ATOM_Geode &node)
	{
		ATOM_BBox bbox = node.getWorldBoundingbox (ATOM_Matrix4x4f::getIdentityMatrix(), ATOM_Matrix4x4f::getIdentityMatrix());
		if (bbox.getDiagonalSize() > maxATOM_BBoxValue)
		{
			maxATOM_BBoxValue = bbox.getDiagonalSize();
			modelFileName = node.getModelFileName ();
		}
	}
};

class AOPickVisitor: public ATOM_PickVisitor
{
public:
	AOPickVisitor (void)
	{
		setPickAll (true);
	}

	virtual void visit (ATOM_Terrain &node) 
	{ 
		// skip terrain;
		return;
	}

	virtual void visit (ATOM_DSky &node)
	{
		// skip
		return;
	}

	virtual void visit (ATOM_ParticleSystem &node)
	{
		// skip;
		return;
	}

	virtual void visit (ATOM_Geode &node)
	{
		ATOM_PickVisitor::visit (node);
	}
};

void Terrain_AOGenerator::setErrorMessage (const char *format,...)
{
	char buffer[2048];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, 2048, format, args);
    va_end(args);
	_errorMessage = buffer;
}

bool Terrain_AOGenerator::generate (unsigned size)
{
	setErrorMessage ("成功完成");

	if (size == 0)
	{
		setErrorMessage ("Invalid output image size: %d\n", size);
		return false;
	}

	if (!_scene.load (_sceneFileName.c_str()))
	{
		setErrorMessage  ("Load scene file <%s> failed.\n", _sceneFileName.c_str());
		return false;
	}

	const ATOM_Vector4f &ambient = _scene.getAmbientLight ();
	float ambientL = (ambient.x + ambient.y + ambient.z) / 3.f;
	if (ambientL > 1.f) ambientL = 1.f; else if (ambientL < 0.f) ambientL = 0.f;

	TerrainFinderVisitor v;
	v.traverse (*_scene.getRootNode());

	if (!v.terrain)
	{
		setErrorMessage  ("No terrain node found in scene.\n");
		return false;
	}

	ATOM_HeightField *hf = v.terrain->getHeightField();
	if (!hf)
	{
		setErrorMessage  ("No height field found in terrain.\n");
		return false;
	}

	ATOM_AutoFile out(_bentnormalMap.c_str(), ATOM_VFS::write);
	if (!out)
	{
		setErrorMessage  ("Could not write image file <%s>.\n");
		return false;
	}

	ATOM_Camera camera;

	unsigned hfSize = size;
	const float unitLengthX = (hf->getSize()-1) * hf->getSpacingX () / float(hfSize);
	const float unitLengthZ = (hf->getSize()-1) * hf->getSpacingZ () / float(hfSize);

	ATOM_VECTOR<ATOM_ColorARGB> aobuffer(hfSize * hfSize);

	unsigned __int64 totalWork = hfSize * hfSize;
	unsigned __int64 currentWork = 0;
	unsigned tickStart = ::GetTickCount ();
	unsigned tickNow = tickStart;

	if (!createRenderTarget ())
	{
		setErrorMessage  ("Create floating point render target failed.\n");
		return false;
	}

	if (!createAOMaterial ())
	{
		setErrorMessage  ("Create material failed.\n");
		return false;
	}

	createBackground ();
	prepareTerrainVertices (hf);

	if (!ATOM_GetRenderDevice()->BeginFrame ())
	{
		setErrorMessage  ("BeginScene failed.\n");
		return false;
	}

	ATOM_AUTOREF(ATOM_Texture) prevTarget = ATOM_GetRenderDevice()->getRenderTarget ();
	ATOM_AUTOREF(ATOM_Texture) prevDepth = ATOM_GetRenderDevice()->GetRenderDepth ();

	ATOM_VECTOR<ATOM_Vector4f> buffer(_rendertarget[0]->getWidth() * _rendertarget[0]->getWidth());

	int percent = -1;
	int row, col;
	for (row = 0; row < hfSize; ++row)
	{
		for (col = 0; col < hfSize; ++col)
		{
			ATOM_ColorARGB &sample = aobuffer[row * hfSize + col];
			ATOM_Vector3f bentnormal(0.f, 0.f, 0.f);
			ATOM_Vector3f acc(0.f, 0.f, 0.f);
			unsigned term = 0;

			float startx = (col + 0.5f) * unitLengthX;
			float startz = (row + 0.5f) * unitLengthZ;
			ATOM_Vector3f start(startx, hf->getRealHeight(startx, startz), startz);

			float totalSamples = 6 * _rendertarget[0]->getWidth() * _rendertarget[0]->getWidth();
			
			for (unsigned p = 0; p < 6; ++p)
			{
				ATOM_GetRenderDevice()->setRenderTarget (_rendertarget[p].get ());
				ATOM_GetRenderDevice()->Clear (true, true, true);
				setupCamera (start, p, &camera);
				camera.setPerspective (ATOM_HalfPi, 1.f, 0.1f, 10000.f);
				camera.synchronise (ATOM_GetRenderDevice (), ATOM_GetRenderDevice()->getCurrentView());
				drawBackground (&camera);
				drawScene (&camera);

				camera.setPerspective (ATOM_HalfPi, 1.f, 15.f, 10000.f);
				camera.synchronise (ATOM_GetRenderDevice (), ATOM_GetRenderDevice()->getCurrentView());
				drawTerrain (&camera);
			}

			for (unsigned p = 0; p < 6; ++p)
			{
				_rendertarget[p]->GetTexImage (ATOM_PIXEL_FORMAT_RGBA32F, &buffer[0]);
				for (unsigned samp = 0; samp < buffer.size(); ++samp)
				{
					if (buffer[samp].w > 0.5f)
					{
						++term;
						bentnormal += ATOM_Vector3f (buffer[samp].x, buffer[samp].y, buffer[samp].z);
					}
				}
			}

			currentWork++;

			int new_percent = currentWork * 100 / totalWork;
			if (percent != new_percent)
			{
				percent = new_percent;
				if (_callback)
				{
					_callback (_callbackData, percent);
				}
			}

			unsigned intensity = term * 2;
			if (intensity > totalSamples)
				intensity = totalSamples;
			float alpha = float(intensity)/float(totalSamples);

			bentnormal.normalize ();
			bentnormal *= alpha;
			bentnormal *= 0.5f;
			bentnormal += 0.5f;

			alpha = ambientL + alpha * (1.f - ambientL);
			sample.setFloats (bentnormal.x, bentnormal.y, bentnormal.z, alpha);
		}
	}

	ATOM_GetRenderDevice()->EndFrame ();
	ATOM_GetRenderDevice()->setRenderTarget (prevTarget.get(), prevDepth.get());

	setErrorMessage  ("Generation finished.\n");

	ATOM_HARDREF(ATOM_Image) imageBentnormal;
	ATOM_ASSERT(imageBentnormal);

	imageBentnormal->Init (hfSize, hfSize, ATOM_PIXEL_FORMAT_BGRA8888, &aobuffer[0]);
	//imageBentnormal->Resize (size, size);
	//imageBentnormal->Resize (size, size);

	if (!imageBentnormal->Save (out, ATOM_PIXEL_FORMAT_BGRA8888))
	{
		setErrorMessage  ("Save image to <%s> failed.\n", _bentnormalMap.c_str());
		return false;
	}

	return true;
}

bool Terrain_AOGenerator::createRenderTarget (void)
{
	for (unsigned i = 0; i < 6; ++i)
	{
		_rendertarget[i] = ATOM_GetRenderDevice()->allocTexture (0, 0, 32, 32, ATOM_PIXEL_FORMAT_RGBA32F, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET, true);
		if (_rendertarget == 0)
		{
			return false;
		}
	}

	return true;
}


static const char OccAoEffect[] =
	"material { effect { source \"					\n"
	"	float4x4 ModelViewProj;						\n"
	"	struct a2v									\n"
	"	{											\n"
	"		float3 position : POSITION;				\n"
	"	};											\n"
	"	struct v2p									\n"
	"	{											\n"
	"		float4 position : POSITION0;			\n"
	"	};											\n"
	"	struct output								\n"
	"	{											\n"
	"		float4 color : COLOR0;					\n"
	"	};											\n"
	"	void vs( in a2v IN, out v2p OUT )																			\n"
	"	{																											\n"
	"		OUT.position = mul(float4(IN.position, 1), ModelViewProj);												\n"
	"	}																											\n"
	"	void ps( in v2p IN, out output OUT)																			\n"
	"	{																											\n"
	"		OUT.color = float4(0.f, 0.f, 0.f, 0.f);																	\n"
	"	}																											\n"
	"	technique test																								\n"
	"	{																											\n"
	"		pass p0																									\n"
	"		{																										\n"
	"			CullMode = None;																					\n"
	"			AlphaTestEnable = False;																				\n"
	"			AlphaRef=128;																						\n"
	"			AlphaFunc=Greater;																					\n"
	"			vertexshader = compile vs_1_1 vs();																	\n"
	"			pixelshader = compile ps_2_0 ps();																	\n"
	"		}																										\n"
	"	}\";																										\n"
	"	param ModelViewProj matrix44f MVPMATRIX;																	\n"
	"} }																											\n";

static const char UnOccAoEffect[] =
	"material { effect { source \"					\n"
	"	float4x4 ModelViewProj;						\n"
	"	float4x4 WorldMatrix;						\n"
	"	float4 eyePosW;								\n"
	"	struct a2v									\n"
	"	{											\n"
	"		float3 position : POSITION;				\n"
	"		float4 color : COLOR0;					\n"
	"	};											\n"
	"	struct v2p									\n"
	"	{											\n"
	"		float4 position : POSITION0;			\n"
	"		float3 worldpos : TEXCOORD0;			\n"
	"		float4 color : COLOR0;					\n"
	"	};											\n"
	"	struct output								\n"
	"	{											\n"
	"		float4 color : COLOR0;					\n"
	"	};											\n"
	"	void vs( in a2v IN, out v2p OUT )																			\n"
	"	{																											\n"
	"		float4 po = float4(IN.position, 1);																		\n"
	"		OUT.position = mul(po, ModelViewProj);																	\n"
	"		OUT.worldpos = mul(po, WorldMatrix).xyz;																\n"
	"		OUT.color = IN.color;																					\n"
	"	}																											\n"
	"	void ps( in v2p IN, out output OUT)																			\n"
	"	{																											\n"
	"		OUT.color = float4(normalize(IN.worldpos - eyePosW.xyz), 1.f) * IN.color;								\n"
	"	}																											\n"
	"	technique test																								\n"
	"	{																											\n"
	"		pass p0																									\n"
	"		{																										\n"
	"			CullMode = None;																					\n"
	"			ZFunc = Always;																						\n"
	"			ZWriteEnable = False;																				\n"
	"			vertexshader = compile vs_1_1 vs();																	\n"
	"			pixelshader = compile ps_2_0 ps();																	\n"
	"		}																										\n"
	"	}\";																										\n"
	"	param ModelViewProj matrix44f MVPMATRIX;																	\n"
	"	param WorldMatrix matrix44f WORLDMATRIX;																	\n"
	"	param eyePosW float4 WORLDSPACEEYE;																			\n"
	"} }																											\n";

bool Terrain_AOGenerator::createAOMaterial (void)
{
	_occmaterial = ATOM_CreateObject (ATOM_Material::_classname(), 0);	
	if (!_occmaterial->load (OccAoEffect))
	{
		return false;
	}
	_unoccmaterial = ATOM_CreateObject (ATOM_Material::_classname(), 0);	
	if (!_unoccmaterial->load (UnOccAoEffect))
	{
		return false;
	}
	return true;
}

void Terrain_AOGenerator::createBackground (void)
{
	_background = ATOM_GetRenderDevice()->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD, USAGE_STATIC, 36);
	_backgroundc = ATOM_GetRenderDevice()->allocVertexArray (ATOM_VERTEX_ATTRIB_PRIMARY_COLOR, USAGE_STATIC, 36);

	const float scale = 1000.f;
	const ATOM_Vector3f vertices[36] = {
		ATOM_Vector3f(-1.f,  1.f, -1.f) * scale,
		ATOM_Vector3f(-1.f,  1.f,  1.f) * scale,
		ATOM_Vector3f( 1.f,  1.f,  1.f) * scale,
		ATOM_Vector3f(-1.f,  1.f, -1.f) * scale,
		ATOM_Vector3f( 1.f,  1.f,  1.f) * scale,
		ATOM_Vector3f( 1.f,  1.f, -1.f) * scale,

		ATOM_Vector3f(-1.f, -1.f, -1.f) * scale,
		ATOM_Vector3f(-1.f, -1.f,  1.f) * scale,
		ATOM_Vector3f( 1.f, -1.f,  1.f) * scale,
		ATOM_Vector3f(-1.f, -1.f, -1.f) * scale,
		ATOM_Vector3f( 1.f, -1.f,  1.f) * scale,
		ATOM_Vector3f( 1.f, -1.f, -1.f) * scale,

		ATOM_Vector3f(-1.f, -1.f, -1.f) * scale,
		ATOM_Vector3f(-1.f, -1.f,  1.f) * scale,
		ATOM_Vector3f(-1.f,  1.f,  1.f) * scale,
		ATOM_Vector3f(-1.f, -1.f, -1.f) * scale,
		ATOM_Vector3f(-1.f,  1.f,  1.f) * scale,
		ATOM_Vector3f(-1.f,  1.f, -1.f) * scale,

		ATOM_Vector3f( 1.f, -1.f, -1.f) * scale,
		ATOM_Vector3f( 1.f, -1.f,  1.f) * scale,
		ATOM_Vector3f( 1.f,  1.f,  1.f) * scale,
		ATOM_Vector3f( 1.f, -1.f, -1.f) * scale,
		ATOM_Vector3f( 1.f,  1.f,  1.f) * scale,
		ATOM_Vector3f( 1.f,  1.f, -1.f) * scale,

		ATOM_Vector3f(-1.f, -1.f, -1.f) * scale,
		ATOM_Vector3f(-1.f,  1.f, -1.f) * scale,
		ATOM_Vector3f( 1.f,  1.f, -1.f) * scale,
		ATOM_Vector3f(-1.f, -1.f, -1.f) * scale,
		ATOM_Vector3f( 1.f,  1.f, -1.f) * scale,
		ATOM_Vector3f( 1.f, -1.f, -1.f) * scale,

		ATOM_Vector3f(-1.f, -1.f,  1.f) * scale,
		ATOM_Vector3f(-1.f,  1.f,  1.f) * scale,
		ATOM_Vector3f( 1.f,  1.f,  1.f) * scale,
		ATOM_Vector3f(-1.f, -1.f,  1.f) * scale,
		ATOM_Vector3f( 1.f,  1.f,  1.f) * scale,
		ATOM_Vector3f( 1.f, -1.f,  1.f) * scale,
	};

	const unsigned colors[36] = {
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,

		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,

		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,

		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,

		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,

		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
	};

	ATOM_Vector3f *v = (ATOM_Vector3f*)_background->Lock (LOCK_WRITEONLY, 0, 0, true);
	memcpy (v, vertices, 36 * sizeof(ATOM_Vector3f));
	_background->Unlock ();

	unsigned *vc = (unsigned *)_backgroundc->Lock (LOCK_WRITEONLY, 0, 0, true);
	memcpy (vc, colors, 36 * sizeof(unsigned));
	_backgroundc->Unlock ();
}

void Terrain_AOGenerator::drawTerrain (ATOM_Camera *camera)
{
	ATOM_GetRenderDevice()->loadIdentity (MATRIXMODE_WORLD);
	ATOM_GetRenderDevice()->ClearStreamSource ();
	ATOM_GetRenderDevice()->SetStreamSource (_terrainVerts.get(), ATOM_VERTEX_ATTRIB_COORD);

	unsigned nPasses = 0;
	if (_occmaterial->begin (&nPasses))
	{
		for (unsigned pass = 0; pass < nPasses; ++pass)
		{
			if (_occmaterial->beginPass (pass))
			{
				ATOM_GetRenderDevice()->renderStreamsIndexed (_terrainIndices.get(), PRIMITIVE_TRIANGLES, _terrainIndices->GetNumIndices() / 3, 0);
				_occmaterial->endPass ();
			}
		}
		_occmaterial->end ();
	}
}

void Terrain_AOGenerator::drawBackground (ATOM_Camera *camera)
{
	const ATOM_Matrix4x4f &viewMatrix = camera->getViewMatrix ();
	ATOM_Vector3f viewPoint(viewMatrix.m30, viewMatrix.m31, viewMatrix.m32);
	ATOM_GetRenderDevice()->SetTransform (MATRIXMODE_WORLD, ATOM_Matrix4x4f::getTranslateMatrix(viewPoint));

	ATOM_GetRenderDevice()->ClearStreamSource ();
	ATOM_GetRenderDevice()->SetStreamSource (_background.get(), ATOM_VERTEX_ATTRIB_COORD);
	ATOM_GetRenderDevice()->SetStreamSource (_backgroundc.get(), ATOM_VERTEX_ATTRIB_PRIMARY_COLOR);

	unsigned nPasses = 0;
	if (_unoccmaterial->begin (&nPasses))
	{
		for (unsigned pass = 0; pass < nPasses; ++pass)
		{
			if (_unoccmaterial->beginPass (pass))
			{
				ATOM_GetRenderDevice()->renderStreams (PRIMITIVE_TRIANGLES, 12);
				_unoccmaterial->endPass ();
			}
		}
		_unoccmaterial->end ();
	}
}

class MyAoCullVisitor: public ATOM_CullVisitor
{
public:
  virtual void visit (ATOM_Node &node)
  {
	  return;
  }

  virtual void visit (ATOM_DSky &node)
  {
	  return;
  }

  virtual void visit (ATOM_ParticleSystem &node)
  {
	  return;
  }

  virtual void visit (ATOM_Terrain &node)
  {
	  return;
  }
};

void Terrain_AOGenerator::drawScene (ATOM_Camera *camera)
{
	ATOM_UpdateVisitor uv;
	uv.setCamera (camera);
	uv.traverse (*_scene.getRootNode ());

	MyAoCullVisitor v;
	v.setCamera (camera);
	v.traverse (*_scene.getRootNode ());
	v.getRenderQueue().Render (ATOM_GetRenderDevice(), camera, 0, _occmaterial.get());
}

void Terrain_AOGenerator::setupCamera (const ATOM_Vector3f &origin, int dir, ATOM_Camera *camera)
{
	camera->setViewport (0, 0, _rendertarget[0]->getWidth(), _rendertarget[0]->getWidth());

	switch (dir)
	{
	case 0:
		{
			camera->lookAt (origin, origin + ATOM_Vector3f(1.f, 0.f, 0.f), ATOM_Vector3f(0.f, 1.f, 0.f));
			break;
		}
	case 1:
		{
			camera->lookAt (origin, origin + ATOM_Vector3f(-1.f, 0.f, 0.f), ATOM_Vector3f(0.f, 1.f, 0.f));
			break;
		}
	case 2:
		{
			camera->lookAt (origin, origin + ATOM_Vector3f(0.f, 0.f, 1.f), ATOM_Vector3f(0.f, 1.f, 0.f));
			break;
		}
	case 3:
		{
			camera->lookAt (origin, origin + ATOM_Vector3f(0.f, 0.f, -1.f), ATOM_Vector3f(0.f, 1.f, 0.f));
			break;
		}
	case 4:
		{
			camera->lookAt (origin, origin + ATOM_Vector3f(0.f, 1.f, 0.f), ATOM_Vector3f(1.f, 0.f, 0.f));
			break;
		}
	case 5:
		{
			camera->lookAt (origin, origin + ATOM_Vector3f(0.f, -1.f, 0.f), ATOM_Vector3f(1.f, 0.f, 0.f));
			break;
		}
	}
}

void Terrain_AOGenerator::prepareTerrainVertices (ATOM_HeightField *hf)
{
	unsigned size = hf->getSize ();
	if (size > 128)
	{
		size = 128;
	}

	ATOM_VECTOR<ATOM_Vector3f> vertices(size * size);
	ATOM_VECTOR<unsigned short> indices((size-1) * (size-1) * 6);
	float terrainSizeX = hf->getSpacingX () * (hf->getSize() - 1);
	float terrainSizeZ = hf->getSpacingZ () * (hf->getSize() - 1);
	float deltaX = terrainSizeX / (size - 3);
	float deltaZ = terrainSizeZ / (size - 3);
	float x, h, z;
	const float lowHeight = -100.f;

	for (unsigned row = 0; row < size; ++row)
	{
		for (unsigned col = 0; col < size; ++col)
		{
			if (row == 0 || row == size-1 || col == 0 || col == size-1)
			{
				x = (col == 0 ? 0.f : terrainSizeX);
				h = lowHeight;
				z = (row == 0 ? 0.f : terrainSizeZ);
			}
			else
			{
				x = deltaX * (col-1);
				z = deltaZ * (row-1);
				h = hf->getRealHeight (x, z);
			}

			vertices[row * size + col].set (x, h, z);
		}
	}

	for (unsigned row = 0; row < size-1; ++row)
	{
		for (unsigned col = 0; col < size-1; ++col)
		{
			unsigned cellindex = row * (size-1) + col;
			unsigned short base = row * size + col;
			indices[cellindex * 6 + 0] = base;
			indices[cellindex * 6 + 1] = base + 1;
			indices[cellindex * 6 + 2] = base + size;
			indices[cellindex * 6 + 3] = base + size;
			indices[cellindex * 6 + 4] = base + 1;
			indices[cellindex * 6 + 5] = base + size + 1;
		}
	}

	ATOM_VECTOR<unsigned short> indicesOpt(indices.size());
	optimizeFacesLRU (indices.size() / 3, &indices[0], &indicesOpt[0], 24);

	ATOM_VECTOR<unsigned> vertexRemap (vertices.size());
	unsigned numVertices = optimizeVertices (indicesOpt.size() / 3, &indicesOpt[0], &vertexRemap[0]);
	remapIndexArray (&indicesOpt[0], &vertexRemap[0], indicesOpt.size());

	remapVertexArray (&vertices[0], sizeof(ATOM_Vector3f), vertices.size(), &vertexRemap[0]);
	vertices.resize (numVertices);

	unsigned short indexTail = vertices.size();
	vertices.push_back (ATOM_Vector3f (0.f, lowHeight, 0.f));
	vertices.push_back (ATOM_Vector3f (0.f, lowHeight, terrainSizeZ));
	vertices.push_back (ATOM_Vector3f (terrainSizeX, lowHeight, 0.f));
	vertices.push_back (ATOM_Vector3f (terrainSizeX, lowHeight, terrainSizeZ));
	indicesOpt.push_back (indexTail);
	indicesOpt.push_back (indexTail + 1);
	indicesOpt.push_back (indexTail + 3);
	indicesOpt.push_back (indexTail);
	indicesOpt.push_back (indexTail + 3);
	indicesOpt.push_back (indexTail + 2);

	_terrainVerts = ATOM_GetRenderDevice()->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD, USAGE_STATIC, vertices.size(), true);
	ATOM_ASSERT(_terrainVerts);
	void *pVerts = _terrainVerts->Lock (LOCK_WRITEONLY, 0, 0, true);
	ATOM_ASSERT(pVerts);
	memcpy (pVerts, &vertices[0], sizeof(ATOM_Vector3f) * vertices.size());
	_terrainVerts->Unlock ();

	_terrainIndices = ATOM_GetRenderDevice()->allocIndexArray (USAGE_STATIC, indicesOpt.size(), false, true);
	ATOM_ASSERT(_terrainIndices);
	void *pIndices = _terrainIndices->Lock (LOCK_WRITEONLY);
	ATOM_ASSERT(pIndices);
	memcpy (pIndices, &indicesOpt[0], sizeof(unsigned short) * indicesOpt.size());
	_terrainIndices->Unlock ();
}

#endif
