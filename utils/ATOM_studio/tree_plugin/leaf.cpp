#include "StdAfx.h"
#include "treedata.h"
#include "leaf.h"
#include "trunk.h"
#include "tree.h"

#if defined(SUPPORT_BILLBOARD_TREE)

/*
static const char leafMaterialSrc[] = 
  "material { \n"
  "  effect { \n"
  "   source  \" \n"
  "     texture DiffuseTexture;													\n"
  "		float4x4 MVPmatrix;														\n"
  "		float4 vertexOffsetsX[4];												\n"
  "		float4 vertexOffsetsY[4];												\n"
  "		float4 vertexUV[4];														\n"
  "		float4 normals[4];														\n"
  "		float4 lightcolor;														\n"
  "		float4 lightDirEye;														\n"
  "		float4 ambient;															\n"
  "																				\n"
  "		sampler2D diffuseSampler = sampler_state								\n"
  "		{																		\n"
  "			texture = (DiffuseTexture);											\n"
  "			MipFilter = None;													\n"
  "		};																		\n"
  "																				\n"
  "		struct VS_INPUT															\n"
  "		{																		\n"
  "			float4 position: POSITION0;											\n"
  "			float4 params: TEXCOORD0;											\n"
  "		};																		\n"
  "																				\n"
  "		struct VS_OUTPUT														\n"
  "		{																		\n"
  "			float4 position: POSITION0;											\n"
  "			float2 uv: TEXCOORD0;												\n"
  "			float3 normal: TEXCOORD1;											\n"
  "		};																		\n"
  "																				\n"
  "		struct PS_OUTPUT														\n"
  "		{																		\n"
  "			float4 color: COLOR0;												\n"
  "		};																		\n"
  "																				\n"
  "		void vs (in VS_INPUT IN, out VS_OUTPUT OUT)								\n"
  "		{																		\n"
  "			float vertexIndex = IN.position.w;									\n"
  "			float xscale = IN.params.x;											\n"
  "			float yscale = IN.params.y;											\n"
  "			float3 xaxis = vertexOffsetsX[vertexIndex];							\n"
  "			float3 yaxis = vertexOffsetsY[vertexIndex];							\n"
  "			float3 pos = IN.position.xyz + xaxis * xscale + yaxis * yscale;		\n"
  "			OUT.position = mul(float4(pos, 1.0), MVPmatrix);					\n"
  "			OUT.uv.x = vertexUV[vertexIndex].x * IN.params.z + IN.params.w;		\n"
  "			OUT.uv.y = vertexUV[vertexIndex].y;									\n"
  "			OUT.normal = normals[vertexIndex];									\n"
  "		}																		\n"
  "																				\n"
  "		void ps(in VS_OUTPUT IN, out PS_OUTPUT OUT)								\n"
  "		{																		\n"
  "			float3 normal = normalize(IN.normal);								\n"
  "			float d = max(dot(normal, lightDirEye.xyz), 0.f);					\n"
  "			float4 c = d * lightcolor + ambient;								\n"
  "			OUT.color = tex2D(diffuseSampler, IN.uv) * c;						\n"
  "		}																		\n"
  "																				\n"
  "     technique t0 {															\n"
  "       pass P0 {																\n"
  "         CullMode = CCW;														\n"
  "			AlphaTestEnable = True;												\n"
  "			AlphaRef = 128;														\n"
  "			AlphaFunc = Greater;												\n"
  "         VertexShader = compile vs_1_1 vs();									\n"
  "         PixelShader = compile ps_2_0 ps();									\n"
  "       }																		\n"
  "     }\";																	\n"
  "		param lightcolor float4 ActiveLightColor;								\n"
  "		param ambient float4 AmbientLight;										\n"
  "		param lightDirEye float4 ActiveLightDirectionC;							\n"
  "  }																			\n"
  "}";

static const char leafMaterialSrcGBuffer[] = 
  "material { \n"
  "  effect { \n"
  "   source  \" \n"
  "     texture DiffuseTexture;													\n"
  "		float4x4 MVPmatrix;														\n"
  "		float4x4 viewmatrix;													\n"
  "		float4 vertexOffsetsX[4];												\n"
  "		float4 vertexOffsetsY[4];												\n"
  "		float4 vertexUV[4];														\n"
  "		float4 normals[4];														\n"
  "																				\n"
  "		sampler2D diffuseSampler = sampler_state								\n"
  "		{																		\n"
  "			texture = (DiffuseTexture);											\n"
  "			MipFilter = None;													\n"
  "		};																		\n"
  "																				\n"
  "		struct VS_INPUT															\n"
  "		{																		\n"
  "			float4 position: POSITION0;											\n"
  "			float4 params: TEXCOORD0;											\n"
  "		};																		\n"
  "																				\n"
  "		struct VS_OUTPUT														\n"
  "		{																		\n"
  "			float4 position: POSITION0;											\n"
  "			float2 uv: TEXCOORD0;												\n"
  "			float4 viewpos: TEXCOORD1;											\n"
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
  "			float vertexIndex = IN.position.w;									\n"
  "			float xscale = IN.params.x;											\n"
  "			float yscale = IN.params.y;											\n"
  "			float3 xaxis = vertexOffsetsX[vertexIndex];							\n"
  "			float3 yaxis = vertexOffsetsY[vertexIndex];							\n"
  "			float3 pos = IN.position.xyz + xaxis * xscale + yaxis * yscale;		\n"
  "			OUT.position = mul(float4(pos, 1.0), MVPmatrix);					\n"
  "			OUT.viewpos = mul(float4(pos, 1.0), viewmatrix);					\n"
  "			OUT.uv.x = vertexUV[vertexIndex].x * IN.params.z + IN.params.w;		\n"
  "			OUT.uv.y = vertexUV[vertexIndex].y;									\n"
  "			OUT.normal = normals[vertexIndex];									\n"
  "		}																		\n"
  "																				\n"
  "		void ps(in VS_OUTPUT IN, out PS_OUTPUT OUT)								\n"
  "		{																		\n"
  "			OUT.color[0] = tex2D(diffuseSampler, IN.uv);						\n"
  "			OUT.color[1].xyz = normalize(IN.normal);							\n"
  "			OUT.color[1].w = length(IN.viewpos);								\n"
  "		}																		\n"
  "																				\n"
  "     technique t0 {															\n"
  "       pass P0 {																\n"
  "         CullMode = CCW;														\n"
  "			AlphaTestEnable = True;												\n"
  "			AlphaRef = 128;														\n"
  "			AlphaFunc = Greater;												\n"
  "         VertexShader = compile vs_1_1 vs();									\n"
  "         PixelShader = compile ps_2_0 ps();									\n"
  "       }																		\n"
  "     }\";																	\n"
  "  }																			\n"
  "}";

static const char leafMaterialSrcCreateBillboard[] = 
  "material { \n"
  "  effect { \n"
  "   source  \" \n"
  "     texture DiffuseTexture;													\n"
  "		float4x4 MVPmatrix;														\n"
  "		float4x4 viewmatrix;													\n"
  "		float4 vertexOffsetsX[4];												\n"
  "		float4 vertexOffsetsY[4];												\n"
  "		float4 vertexUV[4];														\n"
  "		float4 normals[4];														\n"
  "																				\n"
  "		sampler2D diffuseSampler = sampler_state								\n"
  "		{																		\n"
  "			texture = (DiffuseTexture);											\n"
  "			MipFilter = None;													\n"
  "		};																		\n"
  "																				\n"
  "		struct VS_INPUT															\n"
  "		{																		\n"
  "			float4 position: POSITION0;											\n"
  "			float4 params: TEXCOORD0;											\n"
  "		};																		\n"
  "																				\n"
  "		struct VS_OUTPUT														\n"
  "		{																		\n"
  "			float4 position: POSITION0;											\n"
  "			float2 uv: TEXCOORD0;												\n"
  "			float4 viewpos: TEXCOORD1;											\n"
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
  "			float vertexIndex = IN.position.w;									\n"
  "			float xscale = IN.params.x;											\n"
  "			float yscale = IN.params.y;											\n"
  "			float3 xaxis = vertexOffsetsX[vertexIndex];							\n"
  "			float3 yaxis = vertexOffsetsY[vertexIndex];							\n"
  "			float3 pos = IN.position.xyz + xaxis * xscale + yaxis * yscale;		\n"
  "			OUT.position = mul(float4(pos, 1.0), MVPmatrix);					\n"
  "			OUT.viewpos = mul(float4(pos, 1.0), viewmatrix);					\n"
  "			OUT.uv.x = vertexUV[vertexIndex].x * IN.params.z + IN.params.w;		\n"
  "			OUT.uv.y = vertexUV[vertexIndex].y;									\n"
  "			OUT.normal = normals[vertexIndex];									\n"
  "		}																		\n"
  "																				\n"
  "		void ps(in VS_OUTPUT IN, out PS_OUTPUT OUT)								\n"
  "		{																		\n"
  "			float4 c = tex2D(diffuseSampler, IN.uv);							\n"
  "			OUT.color[0].rgb = c.rgb;											\n"
  "			OUT.color[0].a = c.a > 0.125f ? 1.0 : 0.0;							\n"
  "			OUT.color[1].rgb = normalize(IN.normal) * 0.5 + 0.5;				\n"
  "			OUT.color[1].a = 1.0;												\n"
  "		}																		\n"
  "																				\n"
  "     technique t0 {															\n"
  "       pass P0 {																\n"
  "         CullMode = CCW;														\n"
  "			AlphaTestEnable = True;												\n"
  "			AlphaRef = 128;														\n"
  "			AlphaFunc = Greater;												\n"
  "         VertexShader = compile vs_1_1 vs();									\n"
  "         PixelShader = compile ps_2_0 ps();									\n"
  "       }																		\n"
  "     }\";																	\n"
  "  }																			\n"
  "}";
*/

LeafModel::LeafModel (TreeNode *node)
{
	treeNode = node;
	width = 8.f;
	height = 8.f;
	randomRange = 0.f;
	sizeV = 0.f;
	density = 500;
	flip = true;
	vertexDecl = 0;
	visible = true;
	drawBillboard = false;
}

bool LeafModel::isValid (void) const
{
	return vertexArray && indexArray && material && vertexDecl && leaves.size() > 0;
}

void LeafModel::setTextureFileName (const char *filename)
{
	textureFileName = filename ? filename : "";
	texture = ATOM_CreateTextureResource (textureFileName.c_str());
	treeNode->invalidateBillboard ();
}

const char *LeafModel::getTextureFileName (void) const
{
	return textureFileName.c_str();
}

bool LeafModel::loadXML (ATOM_TiXmlElement *root, unsigned randomSeed)
{
	if (!root)
	{
		return false;
	}

	ATOM_TiXmlElement *eRoot = root;

	ATOM_TiXmlElement *eLeafWidth = eRoot->FirstChildElement("LeafWidth");
	if (eLeafWidth)
	{
		double val = width;
		eLeafWidth->Attribute ("Value", &val);
		width = val;
	}

	ATOM_TiXmlElement *eLeafHeight = eRoot->FirstChildElement("LeafHeight");
	if (eLeafHeight)
	{
		double val = height;
		eLeafHeight->Attribute ("Value", &val);
		height = val;
	}

	ATOM_TiXmlElement *eLeafTexture = eRoot->FirstChildElement("LeafTexture");
	if (eLeafTexture)
	{
		const char *filename = eLeafTexture->Attribute ("FileName");
		setTextureFileName (filename ? filename : "");
	}

	ATOM_TiXmlElement *eLeafSizeV = eRoot->FirstChildElement("LeafSizeV");
	if (eLeafSizeV)
	{
		double val = sizeV;
		eLeafSizeV->Attribute ("Value", &val);
		sizeV = val;
	}
	
	ATOM_TiXmlElement *eLeafRandomRange = eRoot->FirstChildElement("LeafRandomRange");
	if (eLeafRandomRange)
	{
		double val = randomRange;
		eLeafRandomRange->Attribute ("Value", &val);
		randomRange = val;
	}

	ATOM_TiXmlElement *eLeafDensity = eRoot->FirstChildElement("LeafDensity");
	if (eLeafDensity)
	{
		eLeafDensity->Attribute ("Value", &density);
	}

	ATOM_TiXmlElement *eLeafFlip = eRoot->FirstChildElement("LeafFlip");
	if (eLeafFlip)
	{
		int b = flip ? 1 : 0;
		eLeafFlip->Attribute ("Value", &b);
		flip = b != 0;
	}

	ATOM_TiXmlElement *eVisible = eRoot->FirstChildElement("LeafVisible");
	if (eVisible)
	{
		int b = visible ? 1 : 0;
		eVisible->Attribute ("Value", &b);
		visible = b != 0;
	}

	return regenerate (randomSeed);
}

bool LeafModel::regenerate (unsigned randomSeed)
{
	seed = randomSeed;
	srand (randomSeed);

	updateLeafCards (treeNode);
	vertexArray = 0;
	indexArray = 0;

	ATOM_RenderDevice *device = ATOM_GetRenderDevice();

	const unsigned leafCardVertexAttrib = ATOM_VERTEX_ATTRIB_COORD_XYZW|ATOM_VERTEX_ATTRIB_TEX1_4;
	if (leaves.size() > 0)
	{
		vertexArray = device->allocVertexArray (leafCardVertexAttrib, ATOM_USAGE_STATIC, leaves.size() * 4, true);
		if (!vertexArray)
		{
			return false;
		}

		struct LeafVertex
		{
			ATOM_Vector4f pos;
			ATOM_Vector4f params;
		};

		LeafVertex *v = (LeafVertex*)vertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		for (unsigned i = 0; i < leaves.size(); ++i)
		{
			const ATOM_Vector3f &p = leaves[i].centerPoint;
			float w = leaves[i].width;
			float h = leaves[i].height;
			float m = leaves[i].flip ? -1.f : 1.f;
			float n = leaves[i].flip ? 1.f : 0.f;

			v->pos.set(p.x, p.y, p.z, 0.f);
			v->params.x = w;
			v->params.y = h;
			v->params.z = m;
			v->params.w = n;
			v++;

			v->pos.set(p.x, p.y, p.z, 1.f);
			v->params.x = w;
			v->params.y = h;
			v->params.z = m;
			v->params.w = n;
			v++;

			v->pos.set(p.x, p.y, p.z, 2.f);
			v->params.x = w;
			v->params.y = h;
			v->params.z = m;
			v->params.w = n;
			v++;

			v->pos.set(p.x, p.y, p.z, 3.f);
			v->params.x = w;
			v->params.y = h;
			v->params.z = m;
			v->params.w = n;
			v++;
		}
		vertexArray->unlock ();

		indexArray = device->allocIndexArray (ATOM_USAGE_STATIC, leaves.size() * 2 * 3, false, true);
		if (!indexArray)
		{
			return false;
		}
		unsigned short *indices = (unsigned short*)indexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		for (unsigned i = 0; i < leaves.size(); ++i)
		{
			*indices++ = i * 4 + 0;
			*indices++ = i * 4 + 1;
			*indices++ = i * 4 + 3;
			*indices++ = i * 4 + 3;
			*indices++ = i * 4 + 1;
			*indices++ = i * 4 + 2;
		}
		indexArray->unlock ();

		if (!vertexDecl)
		{
			vertexDecl = device->createInterleavedVertexDeclaration (leafCardVertexAttrib);
		}

		if (!material)
		{
			material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/treeleaf_ed.mat");
			if (!material)
			{
				return false;
			}
		}
	}

	bbox.beginExtend ();
	for (unsigned i = 0; i < leaves.size(); ++i)
	{
		const LeafCard &l = leaves[i];
		ATOM_Vector3f v(l.width, l.height, l.width);
		bbox.extend (l.centerPoint - v);
		bbox.extend (l.centerPoint + v);
	}
	
	treeNode->invalidateBoundingbox ();
	treeNode->invalidateBillboard ();

	return true;
}

const ATOM_BBox &LeafModel::getBBox (void) const
{
	return bbox;
}

void LeafModel::setDrawBillboard (bool b)
{
	drawBillboard = b;
}

bool LeafModel::getDrawBillboard (void) const
{
	return drawBillboard;
}

void LeafModel::setVisible (bool b)
{
	if (b != visible)
	{
		treeNode->invalidateBoundingbox ();
		treeNode->invalidateBillboard ();
		visible = b;
	}
}

bool LeafModel::isVisible (void) const
{
	return visible;
}

void LeafModel::updateLeafCards (ATOM_Node *treeNode)
{
	const float overlapEpsl = 0.1f;

	leaves.resize (0);

	ATOM_BBox bbox;
	bbox.beginExtend ();
	// compute leaf bounding box
	for (unsigned i = 0; i < treeNode->getNumChildren(); ++i)
	{
		const ATOM_Matrix4x4f &matrixTest = treeNode->getChild (i)->getO2T ();
		ATOM_Vector3f center(matrixTest.m30, matrixTest.m31, matrixTest.m32);
		float radius = matrixTest.m00;
		bbox.extend (center + ATOM_Vector3f(-radius,  radius, -radius));
		bbox.extend (center + ATOM_Vector3f(-radius,  radius,  radius));
		bbox.extend (center + ATOM_Vector3f(-radius, -radius, -radius));
		bbox.extend (center + ATOM_Vector3f(-radius, -radius,  radius));
		bbox.extend (center + ATOM_Vector3f( radius,  radius, -radius));
		bbox.extend (center + ATOM_Vector3f( radius,  radius,  radius));
		bbox.extend (center + ATOM_Vector3f( radius, -radius, -radius));
		bbox.extend (center + ATOM_Vector3f( radius, -radius,  radius));
	}

	ATOM_Vector3f bboxBounds = bbox.getMax() - bbox.getMin ();
	float interval = bboxBounds.x * bboxBounds.y * bboxBounds.z / density;
	interval = ATOM_pow (interval, 0.333f);

	unsigned dimX = bboxBounds.x / interval + 1;
	unsigned dimY = bboxBounds.y / interval + 1;
	unsigned dimZ = bboxBounds.z / interval + 1;

	// test points
	float leafWidth = width;
	float leafHeight = height;
	for (unsigned i = 0; i < dimX; ++i)
		for (unsigned j = 0; j < dimY; ++j)
			for (unsigned k = 0; k < dimZ; ++k)
			{
				ATOM_Vector3f pointPos(interval * i, interval * j, interval * k);
				pointPos += bbox.getMin();

				bool contains = false;
				for (unsigned s = 0; s < treeNode->getNumChildren(); ++s)
				{
					const ATOM_Matrix4x4f &matrixTestAgainst = treeNode->getChild(s)->getO2T ();
					ATOM_Vector3f center(matrixTestAgainst.m30, matrixTestAgainst.m31, matrixTestAgainst.m32);
					float radius = matrixTestAgainst.m00;
					float dist = (center - pointPos).getLength();
					if (dist <= radius)
					{
						contains = true;
						break;
					}
				}
				if (contains)
				{
					if (randomRange > 0.f)
					{
						float rx = float(rand())/float(RAND_MAX) - 0.5f;
						float ry = float(rand())/float(RAND_MAX) - 0.5f;
						float rz = float(rand())/float(RAND_MAX) - 0.5f;
						pointPos += ATOM_Vector3f(rx * randomRange, ry * randomRange, rz * randomRange);
					}

					float widthV = sizeV * (float(rand())/float(RAND_MAX));
					float heightV = widthV * (height / width);
					addLeafCards (&pointPos, 1, flip ? (rand() < RAND_MAX / 2) : false, widthV, heightV);
				}
			}
}

void LeafModel::addLeafCards (const ATOM_Vector3f *centerPoints, unsigned count, bool flip, float widthV, float heightV)
{
	unsigned size = leaves.size();
	leaves.resize (size + count);

	for (unsigned i = 0; i < count; ++i)
	{
		leaves[size + i].centerPoint = centerPoints[i];
		leaves[size + i].width = width + widthV;
		leaves[size + i].height = height + heightV;
		leaves[size + i].flip = flip;
	}
}

void LeafModel::writeXML (ATOM_TiXmlElement *root)
{
	ATOM_TiXmlElement eLeafWidth("LeafWidth");
	eLeafWidth.SetDoubleAttribute ("Value", width);
	root->InsertEndChild (eLeafWidth);

	ATOM_TiXmlElement eLeafHeight("LeafHeight");
	eLeafHeight.SetDoubleAttribute ("Value", height);
	root->InsertEndChild (eLeafHeight);

	ATOM_TiXmlElement eLeafTexture("LeafTexture");
	eLeafTexture.SetAttribute ("FileName", textureFileName.c_str());
	root->InsertEndChild (eLeafTexture);

	ATOM_TiXmlElement eLeafSizeV ("LeafSizeV");
	eLeafSizeV.SetDoubleAttribute ("Value", sizeV);
	root->InsertEndChild (eLeafSizeV);

	ATOM_TiXmlElement eLeafRandomRange ("LeafRandomRange");
	eLeafRandomRange.SetDoubleAttribute ("Value", randomRange);
	root->InsertEndChild (eLeafRandomRange);

	ATOM_TiXmlElement eLeafDensity ("LeafDensity");
	eLeafDensity.SetAttribute ("Value", density);
	root->InsertEndChild (eLeafDensity);

	ATOM_TiXmlElement eLeafFlip("LeafFlip");
	eLeafFlip.SetAttribute ("Value", flip ? 1 : 0);
	root->InsertEndChild (eLeafFlip);

	ATOM_TiXmlElement eVisible("LeafVisible");
	eVisible.SetAttribute ("Value", visible ? 1 : 0);
	root->InsertEndChild (eVisible);
}

bool LeafModel::renderToBillboard (ATOM_RenderDevice *device)
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

	ATOM_Matrix4x4f viewMatrix;
	device->getTransform (ATOM_MATRIXMODE_MODELVIEW, viewMatrix);
	ATOM_Matrix4x4f mvpMatrix;
	device->getTransform (ATOM_MATRIXMODE_MVP, mvpMatrix);
	//m->getEffect()->getEffect()->setMatrixParameters ("viewmatrix", viewMatrix.m, 1);
	//m->getEffect()->getEffect()->setMatrixParameters ("MVPmatrix", mvpMatrix.m, 1);

	ATOM_Matrix4x4f view;
	device->getTransform (ATOM_MATRIXMODE_VIEW, view);
	ATOM_Vector3f x_axis = view.getRow3 (0);
	ATOM_Vector3f y_axis = view.getRow3 (1);
	ATOM_Matrix4x4f invWorld = treeNode->getInvWorldMatrix ();
	x_axis = invWorld.transformVectorAffine (x_axis);
	y_axis = invWorld.transformVectorAffine (y_axis);
	ATOM_Vector4f offsetsX[4] = {
		-x_axis,
		 x_axis,
		 x_axis,
		-x_axis
	};
	ATOM_Vector4f offsetsY[4] = {
		 y_axis,
		 y_axis,
		-y_axis,
		-y_axis
	};
	ATOM_Vector4f uvs[4] = {
		ATOM_Vector4f(0.f, 0.f, 0.f, 0.f),
		ATOM_Vector4f(1.f, 0.f, 0.f, 0.f),
		ATOM_Vector4f(1.f, 1.f, 0.f, 0.f),
		ATOM_Vector4f(0.f, 1.f, 0.f, 0.f)
	};
	ATOM_Vector4f normals[4] = {
		ATOM_Vector4f(-1.f,  1.f, -0.5f, 0.f),
		ATOM_Vector4f( 1.f,  1.f, -0.5f, 0.f),
		ATOM_Vector4f( 1.f, -1.f, -0.5f, 0.f),
		ATOM_Vector4f(-1.f, -1.f, -0.5f, 0.f)
	};

	m->getParameterTable()->setTexture ("diffuseMap", texture ? texture.get() : ATOM_GetColorTexture(0xFFFFFFFF));
	m->getParameterTable()->setVectorArray ("vertexOffsetsX", offsetsX, 4);
	m->getParameterTable()->setVectorArray ("vertexOffsetsY", offsetsY, 4);
	m->getParameterTable()->setVectorArray ("vertexUV", uvs, 4);
	m->getParameterTable()->setVectorArray ("normals", normals, 4);

	device->setStreamSource (0, vertexArray.get());
	device->setVertexDecl (vertexDecl);

	unsigned numPasses = m->begin (device);
	if (numPasses)
	{
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (m->beginPass (device, pass))
			{
				device->renderStreamsIndexed (indexArray.get(), ATOM_PRIMITIVE_TRIANGLES, indexArray->getNumIndices() / 3, 0);
				m->endPass (device, pass);
			}
		}
	}
	m->end (device);

	return true;
}

bool LeafModel::render (ATOM_RenderDevice *device)
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
	ATOM_Matrix4x4f viewMatrix;
	device->getTransform (ATOM_MATRIXMODE_MODELVIEW, viewMatrix);
	ATOM_Matrix4x4f mvpMatrix;
	device->getTransform (ATOM_MATRIXMODE_MVP, mvpMatrix);

	//m->getEffect()->getEffect()->setMatrixParameters ("viewmatrix", viewMatrix.m, 1);
	//m->getEffect()->getEffect()->setMatrixParameters ("MVPmatrix", mvpMatrix.m, 1);

	ATOM_Matrix4x4f view;
	device->getTransform (ATOM_MATRIXMODE_VIEW, view);
	ATOM_Vector3f x_axis = view.getRow3 (0);
	ATOM_Vector3f y_axis = view.getRow3 (1);
	ATOM_Matrix4x4f invWorld = treeNode->getInvWorldMatrix ();
	x_axis = invWorld.transformVectorAffine (x_axis);
	y_axis = invWorld.transformVectorAffine (y_axis);
	ATOM_Vector4f offsetsX[4] = {
		-x_axis,
		 x_axis,
		 x_axis,
		-x_axis
	};
	ATOM_Vector4f offsetsY[4] = {
		 y_axis,
		 y_axis,
		-y_axis,
		-y_axis
	};
	ATOM_Vector4f uvs[4] = {
		ATOM_Vector4f(0.f, 0.f, 0.f, 0.f),
		ATOM_Vector4f(1.f, 0.f, 0.f, 0.f),
		ATOM_Vector4f(1.f, 1.f, 0.f, 0.f),
		ATOM_Vector4f(0.f, 1.f, 0.f, 0.f)
	};
	ATOM_Vector4f normals[4] = {
		ATOM_Vector4f(-1.f,  1.f, -0.5f, 0.f),
		ATOM_Vector4f( 1.f,  1.f, -0.5f, 0.f),
		ATOM_Vector4f( 1.f, -1.f, -0.5f, 0.f),
		ATOM_Vector4f(-1.f, -1.f, -0.5f, 0.f)
	};

	m->getParameterTable()->setTexture ("diffuseMap", texture ? texture.get() : ATOM_GetColorTexture(0xFFFFFFFF));
	m->getParameterTable()->setVectorArray ("vertexOffsetsX", offsetsX, 4);
	m->getParameterTable()->setVectorArray ("vertexOffsetsY", offsetsY, 4);
	m->getParameterTable()->setVectorArray ("vertexUV", uvs, 4);
	m->getParameterTable()->setVectorArray ("normals", normals, 4);

	device->setStreamSource (0, vertexArray.get());
	device->setVertexDecl (vertexDecl);

	unsigned numPasses = m->begin (device);
	if (numPasses)
	{
		for (unsigned pass = 0; pass < numPasses; ++pass)
		{
			if (m->beginPass (device, pass))
			{
				device->renderStreamsIndexed (indexArray.get(), ATOM_PRIMITIVE_TRIANGLES, indexArray->getNumIndices() / 3, 0);
				m->endPass (device, pass);
			}
		}
	}
	m->end (device);

	return true;
}

bool LeafModel::draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{
	return drawBillboard ? renderToBillboard (device) : render (device);
}

void LeafModel::setWidth (float val)
{
	if (val != width)
	{
		width = val;
		regenerate (seed);
	}
}

float LeafModel::getWidth (void) const
{
	return width;
}

void LeafModel::setHeight (float val)
{
	if (val != height)
	{
		height = val;
		regenerate (seed);
	}
}

float LeafModel::getHeight (void) const
{
	return height;
}

void LeafModel::setRandomRange (float val)
{
	if (val != randomRange)
	{
		randomRange = val;
		regenerate (seed);
	}
}

float LeafModel::getRandomRange (void) const
{
	return randomRange;
}

void LeafModel::setSizeV (float val)
{
	if (val != sizeV)
	{
		sizeV = val;
		regenerate (seed);
	}
}

float LeafModel::getSizeV (void) const
{
	return sizeV;
}

void LeafModel::setDensity (int val)
{
	if (val != density)
	{
		density = val;
		regenerate (seed);
	}
}

int LeafModel::getDensity (void) const
{
	return density;
}

void LeafModel::setFlip (bool val)
{
	if (val != flip)
	{
		flip = val;
		regenerate (seed);
	}
}

bool LeafModel::getFlip (void) const
{
	return flip;
}

unsigned LeafModel::getVertexCount (void) const
{
	return isValid() ? vertexArray->getNumVertices() : 0;
}

unsigned LeafModel::getFaceCount (void) const
{
	return isValid() ? indexArray->getNumIndices() / 3 : 0;
}

bool LeafModel::exportToFile (ATOM_File *f) const
{
	ATOM_VertexArray *vb = vertexArray.get();
	ATOM_IndexArray *ib = indexArray.get();
	unsigned numLeafCardVertices = (vb && ib && visible) ? vb->getNumVertices () : 0;
	unsigned numLeafCardIndices = (vb && ib && visible) ? ib->getNumIndices () : 0;

	// write leaf geometry
	if (f->write (&numLeafCardVertices, sizeof(unsigned)) != sizeof(unsigned))
	{
		return false;
	}
	if (vb)
	{
		void *p = vb->lock (ATOM_LOCK_READONLY, 0, 0, false);
		unsigned numBytes = numLeafCardVertices * sizeof(float) * 8;
		if (f->write (p, numBytes) != numBytes)
		{
			vb->unlock ();
			return false;
		}
		vb->unlock ();
	}

	if (f->write (&numLeafCardIndices, sizeof(unsigned)) != sizeof(unsigned))
	{
		return false;
	}
	if (ib)
	{
		void *p = ib->lock (ATOM_LOCK_READONLY, 0, 0, false);
		unsigned numBytes = numLeafCardIndices * sizeof(unsigned short);
		if (f->write (p, numBytes) != numBytes)
		{
			ib->unlock ();
			return false;
		}
		ib->unlock ();
	}

	// write leaf texture filename
	unsigned leafTextureFileNameLen = textureFileName.length ();
	if (f->write (&leafTextureFileNameLen, sizeof(unsigned)) != sizeof(unsigned))
	{
		return false;
	}

	if (f->write (textureFileName.c_str(), leafTextureFileNameLen) != leafTextureFileNameLen)
	{
		return false;
	}

	return true;
}

ATOM_Material *LeafModel::getMaterial (void) const
{
	return material.get();
}

#endif
