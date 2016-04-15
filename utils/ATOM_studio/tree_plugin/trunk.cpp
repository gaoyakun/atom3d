#include "StdAfx.h"

#if defined(SUPPORT_BILLBOARD_TREE)

#include "trunk.h"
#include "leaf.h"
#include "tree.h"

#include <opentree/opentree.h>
#include <opentree/utils/otvertices.h>
#include <opentree/utils/ottriangles.h>
#include <opentree/mesher/treemesher.h>
#include <opentree/weber/weber.h>

#include <assimp.hpp>      // C++ importer interface
#include <aiScene.h>       // Output data structure
#include <aiPostProcess.h> // Post processing flags

/*
static const char trunkMaterialSrc[] = 
  "material { \n"
  "  effect { \n"
  "   source  \" \n"
  "     texture DiffuseTexture;\n"
  "     technique t0 {\n"
  "       pass P0 {\n"
  "         Texture[0] = DiffuseTexture;\n"
  "         CullMode = CCW;\n"
  "         ColorOp[0] = SelectArg1;\n"
  "         ColorArg1[0] = Texture;\n"
  "			ColorOp[1] = Disable;\n"
  "         VertexShader = null;\n"
  "         PixelShader = null;\n"
  "       }\n"
  "     }\";\n"
  "  }\n"
  "}";

static const char trunkMaterialSrcGBuffer[] = 
  "material { \n"
  "  effect { \n"
  "   source  \" \n"
  "     texture DiffuseTexture;													\n"
  "		float4x4 MVPmatrix;														\n"
  "		float4x4 viewmatrix;													\n"
  "																				\n"
  "		sampler2D diffuseSampler = sampler_state								\n"
  "		{																		\n"
  "			texture = (DiffuseTexture);											\n"
  "		};																		\n"
  "																				\n"
  "		struct VS_INPUT															\n"
  "		{																		\n"
  "			float3 position: POSITION0;											\n"
  "			float3 normal: NORMAL;												\n"
  "			float2 uv: TEXCOORD0;												\n"
  "		};																		\n"
  "																				\n"
  "		struct VS_OUTPUT														\n"
  "		{																		\n"
  "			float4 position: POSITION0;											\n"
  "			float2 uv: TEXCOORD0;												\n"
  "			float3 viewpos: TEXCOORD1;											\n"
  "			float3 normal: TEXCOORD2;											\n"
  "		};																		\n"
  "																				\n"
  "		struct PS_OUTPUT														\n"
  "		{																		\n"
  "			float4 color[2]: COLOR0;											\n"
  "		};																		\n"
  "																				\n"
  "		void vs (in VS_INPUT IN, out VS_OUTPUT OUT)								\n"
  "		{																		\n"
  "			OUT.position = mul(float4(IN.position, 1.0), MVPmatrix);			\n"
  "			OUT.uv = IN.uv;														\n"
  "			OUT.viewpos = mul(float4(IN.position, 1.0), viewmatrix).xyz;		\n"
  "			OUT.normal = mul(float4(IN.normal, 0.0), viewmatrix).xyz;			\n"
  "		}																		\n"
  "																				\n"
  "		void ps(in VS_OUTPUT IN, out PS_OUTPUT OUT)								\n"
  "		{																		\n"
  "			OUT.color[0] = tex2D(diffuseSampler, IN.uv);						\n"
  "			OUT.color[0].a = 1.0;												\n"
  "			OUT.color[1].xyz = normalize(IN.normal).xyz;						\n"
  "			OUT.color[1].w = length(IN.viewpos);								\n"
  "		}																		\n"
  "																				\n"
  "     technique t0 {															\n"
  "       pass P0 {																\n"
  "         CullMode = CCW;														\n"
  "         VertexShader = compile vs_1_1 vs();									\n"
  "         PixelShader = compile ps_2_0 ps();									\n"
  "       }																		\n"
  "     }\";																	\n"
  "  }																			\n"
  "}";

static const char trunkMaterialSrcCreateBillbard[] = 
  "material { \n"
  "  effect { \n"
  "   source  \" \n"
  "     texture DiffuseTexture;													\n"
  "		float4x4 MVPmatrix;														\n"
  "		float4x4 viewmatrix;													\n"
  "																				\n"
  "		sampler2D diffuseSampler = sampler_state								\n"
  "		{																		\n"
  "			texture = (DiffuseTexture);											\n"
  "		};																		\n"
  "																				\n"
  "		struct VS_INPUT															\n"
  "		{																		\n"
  "			float3 position: POSITION0;											\n"
  "			float3 normal: NORMAL;												\n"
  "			float2 uv: TEXCOORD0;												\n"
  "		};																		\n"
  "																				\n"
  "		struct VS_OUTPUT														\n"
  "		{																		\n"
  "			float4 position: POSITION0;											\n"
  "			float2 uv: TEXCOORD0;												\n"
  "			float3 viewpos: TEXCOORD1;											\n"
  "			float3 normal: TEXCOORD2;											\n"
  "		};																		\n"
  "																				\n"
  "		struct PS_OUTPUT														\n"
  "		{																		\n"
  "			float4 color[2]: COLOR0;											\n"
  "		};																		\n"
  "																				\n"
  "		void vs (in VS_INPUT IN, out VS_OUTPUT OUT)								\n"
  "		{																		\n"
  "			OUT.position = mul(float4(IN.position, 1.0), MVPmatrix);			\n"
  "			OUT.uv = IN.uv;														\n"
  "			OUT.viewpos = mul(float4(IN.position, 1.0), viewmatrix).xyz;		\n"
  "			OUT.normal = mul(float4(IN.normal, 0.0), viewmatrix).xyz;			\n"
  "		}																		\n"
  "																				\n"
  "		void ps(in VS_OUTPUT IN, out PS_OUTPUT OUT)								\n"
  "		{																		\n"
  "			OUT.color[0] = tex2D(diffuseSampler, IN.uv);						\n"
  "			OUT.color[0].a = 1.0;												\n"
  "			OUT.color[1].rgb = normalize(IN.normal).xyz * 0.5 + 0.5;			\n"
  "			OUT.color[1].a = 1.0;												\n"
  "		}																		\n"
  "																				\n"
  "     technique t0 {															\n"
  "       pass P0 {																\n"
  "         CullMode = CCW;														\n"
  "         VertexShader = compile vs_1_1 vs();									\n"
  "         PixelShader = compile ps_2_0 ps();									\n"
  "       }																		\n"
  "     }\";																	\n"
  "  }																			\n"
  "}";
*/

class MyTriangles: public opentree::otTriangles
{
	unsigned short *_i;
	unsigned _counter;

public:
	MyTriangles (unsigned short *i): _i(i), _counter(0) {}

public:
	void addTriangle(int v1, int v2, int v3)
	{
		*_i++ = v1;
		*_i++ = v2;
		*_i++ = v3;
		_counter += 3;
	}
};

struct MyVertex
{
	ATOM_Vector3f pos;
	ATOM_Vector3f normal;
	ATOM_Vector2f uv;
};

class MyVertexList: public opentree::otVertices
{
	MyVertex *_v;
	ATOM_BBox &_bbox;

public:
	MyVertexList (MyVertex *v, ATOM_BBox &bbox): _v(v), _bbox(bbox) {}

public:
    virtual void add(int index, float x, float y, float z, float nx, float ny,
      float nz, float r, float g, float b, float a, float u, float v)
	{
		_v->pos.x = x;
		_v->pos.y = z;
		_v->pos.z = -y;
		_v->normal.x = nx;
		_v->normal.y = nz;
		_v->normal.z = -ny;
		_v->uv.x = u;
		_v->uv.y = v;

		_bbox.extend (_v->pos);

		_v++;
	}
};

static void copyTreeDataToOT (const TreeData &treeData, opentree::TreeData &params)
{
	params.trunk.scale = treeData.trunk.scale;
	params.trunk.scaleV = treeData.trunk.scaleV;
	params.trunk.baseSplits = treeData.trunk.baseSplits;
	params.trunk.dist = treeData.trunk.dist;
	for (int i = 0; i < 5; ++i)
	{
		params.level[i].levelNumber = treeData.level[i].levelNumber;
		params.level[i].downAngle = treeData.level[i].downAngle;
		params.level[i].downAngleV = treeData.level[i].downAngleV;
		params.level[i].rotate = treeData.level[i].rotate;
		params.level[i].rotateV = treeData.level[i].rotateV;
		params.level[i].branches = treeData.level[i].branches;
		params.level[i].branchDist = treeData.level[i].branchDist;
		params.level[i].length = treeData.level[i].length;
		params.level[i].lengthV = treeData.level[i].lengthV;
		params.level[i].taper = treeData.level[i].taper;
		params.level[i].segSplits = treeData.level[i].segSplits;
		params.level[i].splitAngle = treeData.level[i].splitAngle;
		params.level[i].splitAngleV = treeData.level[i].splitAngleV;
		params.level[i].curveRes = treeData.level[i].curveRes;
		params.level[i].curve = treeData.level[i].curve;
		params.level[i].curveBack = treeData.level[i].curveBack;
		params.level[i].curveV = treeData.level[i].curveV;
	}
	params.shape =			treeData.shape;
	params.baseSize =		treeData.baseSize;
	params.scale =			treeData.scale;
	params.scaleV =			treeData.scaleV;
	params.levels =			treeData.levels;
	params.ratio =			treeData.ratio;
	params.ratioPower =		treeData.ratioPower;
	params.lobes =			treeData.lobes;
	params.lobeDepth =		treeData.lobeDepth;
	params.flare =			treeData.flare;
	params.leaves =			0;
	params.leafScale =		1.f;
	params.leafScaleX =		1.f;
	params.leafQuality =	0.1f;
	params.leafShapeRatio =	4;
	params.leafBend =		0.f;
	params.attractionUp =	treeData.attractionUp;
	params.pruneRatio =		treeData.pruneRatio;
	params.prunePowerLow =	treeData.prunePowerLow;
	params.prunePowerHigh =	treeData.prunePowerHigh;
	params.pruneWidth =		treeData.pruneWidth;
	params.pruneWidthPeak =	treeData.pruneWidthPeak;
}

TrunkModel::TrunkModel (TreeNode *node)
: vertexDecl(0)
, visible(true) 
, drawAlternate(false)
, drawBillboard(false)
, treeNode(node)
{
	memset (&treeData, 0xFF, sizeof(TreeData));
}

bool TrunkModel::isValid (void) const 
{
	if (drawAlternate && (!alternateVertexArray || !alternateIndexArray))
	{
		return false;
	}
	if (!drawAlternate && (!vertexArray || !indexArray))
	{
		return false;
	}

	return material && vertexDecl;
}

void TrunkModel::setTextureFileName (const char *filename)
{
	textureFileName = filename ? filename : "";
	texture = ATOM_CreateTextureResource (textureFileName.c_str());
	treeNode->invalidateBillboard ();
}

const char *TrunkModel::getTextureFileName (void) const
{
	return textureFileName.c_str();
}

bool TrunkModel::loadXML (ATOM_TiXmlElement *root, unsigned randomSeed)
{
	TreeData td;
	if (!readTreeDataFromXML (root, td))
	{
		return false;
	}

	ATOM_TiXmlElement *eBarkTexture = root->FirstChildElement("BarkTexture");
	if (eBarkTexture)
	{
		const char *filename = eBarkTexture->Attribute ("FileName");
		setTextureFileName (filename ? filename : "");
	}

	ATOM_TiXmlElement *eDrawAlternate = root->FirstChildElement("DrawAlternate");
	if (eDrawAlternate)
	{
		int val = drawAlternate ? 1 : 0;
		eDrawAlternate->Attribute ("Value", &val);
		drawAlternate = val != 0;
	}

	ATOM_TiXmlElement *eAlternateModel = root->FirstChildElement("AlternateModel");
	if (eAlternateModel)
	{
		const char *filename = eAlternateModel->Attribute ("FileName");
		if (filename && filename[0])
		{
			if (!setAlternateModel (filename))
			{
				return false;
			}
		}
	}

	ATOM_TiXmlElement *eVisible = root->FirstChildElement ("TrunkVisible");
	if (eVisible)
	{
		int val = visible ? 1 : 0;
		eVisible->Attribute ("Value", &val);
		visible = val != 0;
	}

	if (setTreeData (td, randomSeed))
	{
		treeNode->invalidateBoundingbox ();
		treeNode->invalidateBillboard ();
		return true;
	}

	return false;
}

bool TrunkModel::regenerate (unsigned randomSeed)
{
	srand (randomSeed);

	vertexArray = 0;
	indexArray = 0;
	ATOM_RenderDevice *device = ATOM_GetRenderDevice();

	opentree::TreeData params;
	copyTreeDataToOT (treeData, params);

	int numLevels = params.levels;
	if (numLevels == 0)
	{
		return false;
	}

	opentree::iWeber *gen = opentree::newWeberArbaro();
	gen->setParams (params);
	opentree::otTree *ottree = gen->generate ();
	delete gen;

	const unsigned trunkVertexAttrib = ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_NORMAL|ATOM_VERTEX_ATTRIB_TEX1_2;

	opentree::MesherTree tree(ottree);
	int curveRes[5] = { 6, 5, 3, 3, 3 };
	int circleRes[5] = { 7, 5, 3, 3, 3 };
	for (unsigned i = 0; i < 5; ++i)
	{
		tree.setCurveRes (i, curveRes[i]);
		tree.setCircleRes (i, circleRes[i]);
	}
	tree.useTriangleLeaves ();

	int vertexCount = 0;
	int indexCount = 0;
	int indexCount2 = 0;

	tree.getVerticesCount (0, &vertexCount);
	tree.getIndicesCount (0, &indexCount);

	if (numLevels > 1)
	{
		tree.getVerticesCount (1, &vertexCount);
		tree.getIndicesCount (1, &indexCount2);
		indexCount += indexCount2;
	}

	if (numLevels > 2)
	{
		tree.getVerticesCount (2, &vertexCount);
		tree.getIndicesCount (2, &indexCount2);
		indexCount += indexCount2;
	}

	if (numLevels > 3)
	{
		tree.getVerticesCount (3, &vertexCount);
		tree.getIndicesCount (3, &indexCount2);
		indexCount += indexCount2;
	}

	if (vertexCount == 0 || indexCount == 0)
	{
		return false;
	}

	if (vertexCount > 65534)
	{
		return false;
	}

	vertexArray = device->allocVertexArray (trunkVertexAttrib, ATOM_USAGE_STATIC, vertexCount, true);
	if (!vertexArray)
	{
		return false;
	}
	MyVertex *v = (MyVertex*)vertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	bboxNative.beginExtend ();
	MyVertexList vertices(v, bboxNative);
	tree.getVertices (0, vertices);
	if (numLevels > 1)
	{
		tree.getVertices (1, vertices);
	}
	if (numLevels > 2)
	{
		tree.getVertices (2, vertices);
	}
	if (numLevels > 3)
	{
		tree.getVertices (3, vertices);
	}
	vertexArray->unlock ();

	indexArray = device->allocIndexArray (ATOM_USAGE_STATIC, indexCount * 3, false, true);
	unsigned short *i = (unsigned short*)indexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	MyTriangles indices(i);
	tree.getIndices (0, indices, 0);
	if (numLevels > 1)
	{
		tree.getIndices (1, indices, 0);
	}
	if (numLevels > 2)
	{
		tree.getIndices (2, indices, 0);
	}
	if (numLevels > 3)
	{
		tree.getIndices (3, indices, 0);
	}
	indexArray->unlock ();

	delete ottree;

	if (!vertexDecl)
	{
		vertexDecl = device->createInterleavedVertexDeclaration (trunkVertexAttrib);
	}

	if (!material)
	{
		material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/treetrunk_ed.mat");
		if (!material)
		{
			return false;
		}
	}

	treeNode->invalidateBoundingbox ();
	treeNode->invalidateBillboard ();

	return true;
}

const ATOM_BBox &TrunkModel::getBBox (void) const
{
	return drawAlternate ? bboxAlternate : bboxNative;
}

void TrunkModel::setDrawBillboard (bool b)
{
	drawBillboard = b;
}

bool TrunkModel::getDrawBillboard (void) const
{
	return drawBillboard;
}

void TrunkModel::setVisible (bool b)
{
	if (b != visible)
	{
		treeNode->invalidateBoundingbox ();
		treeNode->invalidateBillboard ();
		visible = b;
	}
}

bool TrunkModel::isVisible (void) const
{
	return visible;
}

const TreeData &TrunkModel::getTreeData (void) const
{
	return treeData;
}

bool TrunkModel::setTreeData (const TreeData &other, unsigned randomSeed)
{
	if (memcmp (&other, &treeData, sizeof(TreeData)))
	{
		treeData = other;
		return regenerate (randomSeed);
	}
	return true;
}

bool TrunkModel::renderToBillboard (ATOM_RenderDevice *device)
{
	if (!visible)
	{
		return true;
	}

	if (!isValid ())
	{
		return false;
	}

	ATOM_Material *m = material.get();

	m->getParameterTable()->setInt ("createBillboard", 1);
	m->getParameterTable()->setTexture ("diffuseMap", texture ? texture.get() : ATOM_GetColorTexture(0xFFFFFFFF));

	ATOM_VertexArray *vb = drawAlternate ? alternateVertexArray.get() : vertexArray.get();
	ATOM_IndexArray *ib = drawAlternate ? alternateIndexArray.get() : indexArray.get();
	device->setStreamSource (0, vb);
	device->setVertexDecl (vertexDecl);
	unsigned numPasses = m->begin(device);
	if (numPasses > 0)
	{
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (m->beginPass (device, pass))
			{
				device->renderStreamsIndexed (ib, ATOM_PRIMITIVE_TRIANGLES, ib->getNumIndices() / 3, 0);
				m->endPass (device, pass);
			}
		}
	}
	m->end (device);

	return true;
}

bool TrunkModel::draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{
	return drawBillboard ? renderToBillboard (device) : render (device);
}

bool TrunkModel::render (ATOM_RenderDevice *device)
{
	if (!visible)
	{
		return true;
	}

	if (!isValid ())
	{
		return false;
	}

	ATOM_Material *m = material.get();

	m->getParameterTable()->setInt ("createBillboard", 0);
	m->getParameterTable()->setTexture ("diffuseMap", texture ? texture.get() : ATOM_GetColorTexture(0xFFFFFFFF));

	ATOM_VertexArray *vb = drawAlternate ? alternateVertexArray.get() : vertexArray.get();
	ATOM_IndexArray *ib = drawAlternate ? alternateIndexArray.get() : indexArray.get();
	device->setStreamSource (0, vb);
	device->setVertexDecl (vertexDecl);
	unsigned numPasses = m->begin(device);
	if (numPasses)
	{
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (m->beginPass (device, pass))
			{
				device->renderStreamsIndexed (ib, ATOM_PRIMITIVE_TRIANGLES, ib->getNumIndices() / 3, 0);
				m->endPass (device, pass);
			}
		}
	}
	m->end (device);

	return true;
}

void TrunkModel::writeXML (ATOM_TiXmlElement *root)
{
	ATOM_TiXmlElement eShape("Shape");
	eShape.SetAttribute ("Value", treeData.shape);
	root->InsertEndChild (eShape);

	ATOM_TiXmlElement eBaseSize("BaseSize");
	eBaseSize.SetDoubleAttribute ("Value", treeData.baseSize);
	root->InsertEndChild (eBaseSize);

	ATOM_TiXmlElement eScale("Scale");
	eScale.SetDoubleAttribute ("Value", treeData.scale);
	root->InsertEndChild (eScale);

	ATOM_TiXmlElement eScaleV("ScaleV");
	eScaleV.SetDoubleAttribute ("Value", treeData.scaleV);
	root->InsertEndChild (eScaleV);

	ATOM_TiXmlElement eLevels("LevelCount");
	eLevels.SetAttribute ("Value", treeData.levels);
	root->InsertEndChild (eLevels);

	ATOM_TiXmlElement eRatio("Ratio");
	eRatio.SetDoubleAttribute ("Value", treeData.ratio);
	root->InsertEndChild (eRatio);

	ATOM_TiXmlElement eRatioPower("RatioPower");
	eRatioPower.SetDoubleAttribute ("Value", treeData.ratioPower);
	root->InsertEndChild (eRatioPower);

	ATOM_TiXmlElement eLobes("Lobes");
	eLobes.SetAttribute ("Value", treeData.lobes);
	root->InsertEndChild (eLobes);

	ATOM_TiXmlElement eLobeDepth("LobeDepth");
	eLobeDepth.SetDoubleAttribute ("Value", treeData.lobeDepth);
	root->InsertEndChild (eLobeDepth);

	ATOM_TiXmlElement eFlare("Flare");
	eFlare.SetDoubleAttribute ("Value", treeData.flare);
	root->InsertEndChild (eFlare);

	ATOM_TiXmlElement eAttractionUp("AttractionUp");
	eAttractionUp.SetDoubleAttribute ("Value", treeData.attractionUp);
	root->InsertEndChild (eAttractionUp);

	ATOM_TiXmlElement ePruneRatio("PruneRatio");
	ePruneRatio.SetDoubleAttribute ("Value", treeData.pruneRatio);
	root->InsertEndChild (ePruneRatio);

	ATOM_TiXmlElement ePrunePowerLow("PrunePowerLow");
	ePrunePowerLow.SetDoubleAttribute ("Value", treeData.prunePowerLow);
	root->InsertEndChild (ePrunePowerLow);

	ATOM_TiXmlElement ePrunePowerHigh("PrunePowerHigh");
	ePrunePowerHigh.SetDoubleAttribute ("Value", treeData.prunePowerHigh);
	root->InsertEndChild (ePrunePowerHigh);

	ATOM_TiXmlElement ePruneWidth("PrunePowerWidth");
	ePruneWidth.SetDoubleAttribute ("Value", treeData.pruneWidth);
	root->InsertEndChild (ePruneWidth);

	ATOM_TiXmlElement ePruneWidthPeak("PruneWidthPeak");
	ePruneWidthPeak.SetDoubleAttribute ("Value", treeData.pruneWidthPeak);
	root->InsertEndChild (ePruneWidthPeak);

	ATOM_TiXmlElement eTrunk("Trunk");
	ATOM_TiXmlElement eTrunkScale("Scale");
	eTrunkScale.SetDoubleAttribute ("Value", treeData.trunk.scale);
	eTrunk.InsertEndChild (eTrunkScale);
	ATOM_TiXmlElement eTrunkScaleV("ScaleV");
	eTrunkScaleV.SetDoubleAttribute ("Value", treeData.trunk.scaleV);
	eTrunk.InsertEndChild (eTrunkScaleV);
	ATOM_TiXmlElement eTrunkBaseSplits("BaseSplits");
	eTrunkBaseSplits.SetDoubleAttribute ("Value", treeData.trunk.baseSplits);
	eTrunk.InsertEndChild (eTrunkBaseSplits);
	ATOM_TiXmlElement eTrunkDist("Dist");
	eTrunkDist.SetDoubleAttribute ("Value", treeData.trunk.dist);
	eTrunk.InsertEndChild (eTrunkDist);
	root->InsertEndChild (eTrunk);

	ATOM_TiXmlElement eLevelsDesc("Levels");
	for (unsigned i = 0; i < treeData.levels; ++i)
	{
		ATOM_TiXmlElement eLevel("Level");
		ATOM_TiXmlElement eLevelDownAngle ("DownAngle");
		eLevelDownAngle.SetDoubleAttribute ("Value", treeData.level[i].downAngle);
		eLevel.InsertEndChild (eLevelDownAngle);
		ATOM_TiXmlElement eLevelDownAngleV ("DownAngleV");
		eLevelDownAngleV.SetDoubleAttribute ("Value", treeData.level[i].downAngleV);
		eLevel.InsertEndChild (eLevelDownAngleV);
		ATOM_TiXmlElement eLevelRotate ("Rotate");
		eLevelRotate.SetDoubleAttribute ("Value", treeData.level[i].rotate);
		eLevel.InsertEndChild (eLevelRotate);
		ATOM_TiXmlElement eLevelRotateV ("RotateV");
		eLevelRotateV.SetDoubleAttribute ("Value", treeData.level[i].rotateV);
		eLevel.InsertEndChild (eLevelRotateV);
		ATOM_TiXmlElement eLevelBranches ("Branches");
		eLevelBranches.SetAttribute ("Value", treeData.level[i].branches);
		eLevel.InsertEndChild (eLevelBranches);
		ATOM_TiXmlElement eLevelBranchDist ("BranchDist");
		eLevelBranchDist.SetDoubleAttribute ("Value", treeData.level[i].branchDist);
		eLevel.InsertEndChild (eLevelBranchDist);
		ATOM_TiXmlElement eLevelLength ("Length");
		eLevelLength.SetDoubleAttribute ("Value", treeData.level[i].length);
		eLevel.InsertEndChild (eLevelLength);
		ATOM_TiXmlElement eLevelLengthV ("LengthV");
		eLevelLengthV.SetDoubleAttribute ("Value", treeData.level[i].lengthV);
		eLevel.InsertEndChild (eLevelLengthV);
		ATOM_TiXmlElement eLevelTaper ("Taper");
		eLevelTaper.SetDoubleAttribute ("Value", treeData.level[i].taper);
		eLevel.InsertEndChild (eLevelTaper);
		ATOM_TiXmlElement eLevelSegSplits ("SegSplits");
		eLevelSegSplits.SetDoubleAttribute ("Value", treeData.level[i].segSplits);
		eLevel.InsertEndChild (eLevelSegSplits);
		ATOM_TiXmlElement eLevelSplitAngle ("SplitAngle");
		eLevelSplitAngle.SetDoubleAttribute ("Value", treeData.level[i].splitAngle);
		eLevel.InsertEndChild (eLevelSplitAngle);
		ATOM_TiXmlElement eLevelSplitAngleV ("SplitAngleV");
		eLevelSplitAngleV.SetDoubleAttribute ("Value", treeData.level[i].splitAngleV);
		eLevel.InsertEndChild (eLevelSplitAngleV);
		ATOM_TiXmlElement eLevelCurveRes ("CurveRes");
		eLevelCurveRes.SetAttribute ("Value", treeData.level[i].curveRes);
		eLevel.InsertEndChild (eLevelCurveRes);
		ATOM_TiXmlElement eLevelCurve ("Curve");
		eLevelCurve.SetDoubleAttribute ("Value", treeData.level[i].curve);
		eLevel.InsertEndChild (eLevelCurve);
		ATOM_TiXmlElement eLevelCurveBack ("CurveBack");
		eLevelCurveBack.SetDoubleAttribute ("Value", treeData.level[i].curveBack);
		eLevel.InsertEndChild (eLevelCurveBack);
		ATOM_TiXmlElement eLevelCurveV ("CurveV");
		eLevelCurveV.SetDoubleAttribute ("Value", treeData.level[i].curveV);
		eLevel.InsertEndChild (eLevelCurveV);

		eLevelsDesc.InsertEndChild (eLevel);
	}
	root->InsertEndChild (eLevelsDesc);

	ATOM_TiXmlElement eBarkTexture("BarkTexture");
	eBarkTexture.SetAttribute ("FileName", textureFileName.c_str());
	root->InsertEndChild (eBarkTexture);

	ATOM_TiXmlElement eDrawAlternate("DrawAlternate");
	eDrawAlternate.SetAttribute ("Value", drawAlternate ? 1 : 0);
	root->InsertEndChild (eDrawAlternate);

	ATOM_TiXmlElement eAlternateModel("AlternateModel");
	eAlternateModel.SetAttribute ("FileName", modelFileName.c_str());
	root->InsertEndChild (eAlternateModel);

	ATOM_TiXmlElement eVisible("TrunkVisible");
	eVisible.SetAttribute ("Value", visible ? 1 : 0);
	root->InsertEndChild (eVisible);
}

unsigned TrunkModel::getVertexCount (void) const
{
	return isValid() ? vertexArray->getNumVertices() : 0;
}

unsigned TrunkModel::getFaceCount (void) const
{
	if (!isValid ())
	{
		return 0;
	}

	ATOM_IndexArray *ib = drawAlternate ? alternateIndexArray.get() : indexArray.get();
	return ib->getNumIndices() / 3;
}

bool TrunkModel::setAlternateModel (const char *filename)
{
	ATOM_RenderDevice *device = ATOM_GetRenderDevice();
	char buffer[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (filename, buffer);

	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile (buffer, aiProcess_ConvertToLeftHanded|aiProcess_Triangulate|aiProcess_GenNormals|aiProcess_SortByPType|aiProcess_GenSmoothNormals|aiProcess_PreTransformVertices|aiProcess_OptimizeMeshes);
	if (!scene)
	{
		::MessageBoxA (device->getWindow()->getWindowInfo()->handle, "无法打开文件或不识别的模型文件类型!", "ATOM Studio", MB_OK|MB_ICONHAND);
		return false;
	}

	aiNode *sceneroot = scene->mRootNode;
	if (!sceneroot)
	{
		::MessageBoxA (device->getWindow()->getWindowInfo()->handle, "模型为空!", "ATOM Studio", MB_OK|MB_ICONHAND);
		return false;
	}

	aiMesh ** const rawmeshes = scene->mMeshes;
	aiNode **children = sceneroot->mChildren;
	aiNode *child;
	unsigned *meshid;

	struct MyVertex
	{
		ATOM_Vector3f pos;
		ATOM_Vector3f norm;
		ATOM_Vector2f uv;
	};
	ATOM_VECTOR<MyVertex> vertices;
	ATOM_VECTOR<unsigned short> indices;

	unsigned nodeindex = 0;
	unsigned indexOffset = 0;
	bool rootnode = true;
	bool promptNoUV = true;
	bool promptNoNormal = true;

	do
	{
		if (rootnode)
		{
			child = sceneroot;
			rootnode = false;
		}
		else
		{
			child = children[nodeindex++];
		}

		if (child->mNumMeshes <= 0)
		{
			continue;
		}

		meshid = child->mMeshes;
		for (unsigned i = 0; i < child->mNumMeshes; ++i)
		{
			aiMesh *mesh = rawmeshes[meshid[i]];
			if (rawmeshes[meshid[i]]->GetNumUVChannels() == 0)
			{
				if (promptNoUV)
				{
					::MessageBoxA (device->getWindow()->getWindowInfo()->handle, "没有贴图坐标的部分已忽略!", "ATOM Studio", MB_OK|MB_ICONWARNING);
					promptNoUV = false;
				}
				continue;
			}

			if (mesh->mNormals == 0)
			{
				if (promptNoNormal)
				{
					::MessageBoxA (device->getWindow()->getWindowInfo()->handle, "没有法线的部分已忽略!", "ATOM Studio", MB_OK|MB_ICONWARNING);
					promptNoNormal = false;
				}
				continue;
			}

			for (unsigned j = 0; j < mesh->mNumVertices; ++j)
			{

				MyVertex v;
				aiVector3D p = mesh->mVertices[j];
				aiVector3D n = mesh->mNormals[j];
				aiVector3D t = mesh->mTextureCoords[0][j];
				v.pos.set(p.x, p.y, p.z);
				v.norm.set(n.x, n.y, n.z);
				v.uv.set(t.x, t.y);
				vertices.push_back (v);
			}

			for (unsigned j = 0; j < mesh->mNumFaces; ++j)
			{
				if (mesh->mFaces[j].mNumIndices != 3)
				{
					continue;
					//::MessageBoxA (_editor->getRenderWindow()->getWindowInfo()->handle, "模型没有成功被三角化!", "ATOM Studio", MB_OK|MB_ICONHAND);
					//return false;
				}
				unsigned *idx = (unsigned*)mesh->mFaces[j].mIndices;
				unsigned idx0 = idx[0] + indexOffset;
				unsigned idx1 = idx[1] + indexOffset;
				unsigned idx2 = idx[2] + indexOffset;
				if (idx0 > 65534 || idx1 > 65534 || idx2 > 65534)
				{
					::MessageBoxA (device->getWindow()->getWindowInfo()->handle, "模型顶点数已超过范围，最大为65534!", "ATOM Studio", MB_OK|MB_ICONHAND);
					return false;
				}
				indices.push_back (idx0);
				indices.push_back (idx1);
				indices.push_back (idx2);
			}
		}
	}
	while (nodeindex < sceneroot->mNumChildren);

	if (vertices.empty() || indices.empty ())
	{
		::MessageBoxA (device->getWindow()->getWindowInfo()->handle, "未导入任何内容!", "ATOM Studio", MB_OK|MB_ICONHAND);
		return false;
	}

	bboxAlternate.beginExtend ();
	for (unsigned i = 0; i < vertices.size(); ++i)
	{
		bboxAlternate.extend (vertices[i].pos);
	}

	alternateVertexArray = 0;
	alternateIndexArray = 0;

	alternateVertexArray = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_NORMAL|ATOM_VERTEX_ATTRIB_TEX1_2, ATOM_USAGE_STATIC, vertices.size(), true);
	if (!alternateVertexArray)
	{
		return false;
	}
	void *p = alternateVertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	memcpy (p, &vertices[0], sizeof(float) * 8 * vertices.size());
	alternateVertexArray->unlock ();

	alternateIndexArray = device->allocIndexArray (ATOM_USAGE_STATIC, indices.size(), false, true);
	if (!alternateIndexArray)
	{
		return false;
	}
	p = alternateIndexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	memcpy (p, &indices[0], indices.size() * sizeof(unsigned short));
	alternateIndexArray->unlock ();

	modelFileName = filename;

	treeNode->invalidateBillboard ();
	treeNode->invalidateBoundingbox ();

	return true;
}

void TrunkModel::setDrawAlternate (bool b)
{
	drawAlternate = b;
}

bool TrunkModel::getDrawAlternate (void) const
{
	return drawAlternate;
}

bool TrunkModel::exportToFile (ATOM_File *f) const
{
	ATOM_VertexArray *vb = drawAlternate ? alternateVertexArray.get() : vertexArray.get();
	ATOM_IndexArray *ib = drawAlternate ? alternateIndexArray.get() : indexArray.get();

	unsigned numTrunkVertices = (vb && ib && visible) ? vb->getNumVertices () : 0;
	unsigned numTrunkIndices = (vb && ib && visible) ? ib->getNumIndices () : 0;
	void *p;

	// write trunk geometry
	if (f->write (&numTrunkVertices, sizeof(unsigned)) != sizeof(unsigned))
	{
		return false;
	}
	if (vb)
	{
		p = vb->lock (ATOM_LOCK_READONLY, 0, 0, false);
		if (!p)
		{
			return false;
		}
		unsigned numBytes = numTrunkVertices * (sizeof(float) * 3 + sizeof(float) * 3 + sizeof(float) * 2);
		if(f->write (p, numBytes) != numBytes)
		{
			vb->unlock ();
			return false;
		}
		vb->unlock ();
	}

	if (f->write (&numTrunkIndices, sizeof(unsigned)) != sizeof(unsigned))
	{
		return false;
	}
	if (ib)
	{
		p = ib->lock (ATOM_LOCK_READONLY, 0, 0, false);
		if (!p)
		{
			return false;
		}
		unsigned numBytes = numTrunkIndices * sizeof(unsigned short);
		if (f->write (p, numBytes) != numBytes)
		{
			ib->unlock ();
			return false;
		}
		ib->unlock ();
	}

	// write trunk texture filename
	unsigned trunkTextureFileNameLen = textureFileName.length ();
	if (f->write (&trunkTextureFileNameLen, sizeof(unsigned)) != sizeof(unsigned))
	{
		return false;
	}

	if (f->write (textureFileName.c_str(), trunkTextureFileNameLen) != trunkTextureFileNameLen)
	{
		return false;
	}

	return true;
}

ATOM_Material *TrunkModel::getMaterial (void) const
{
	return material.get();
}

#endif
