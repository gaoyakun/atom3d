#include "StdAfx.h"

#if defined(SUPPORT_BILLBOARD_TREE)

#include "treedata.h"
#include "tree.h"

static const char billboardMaterialSrc[] = 
  "material { \n"
  "  effect { \n"
  "   source  \" \n"
  "     texture DiffuseTexture;													\n"
  "     texture NormalTexture;													\n"
  "		float4x4 MVPmatrix;														\n"
  "		float4 vertexOffsetsX[4];												\n"
  "		float4 vertexOffsetsY[4];												\n"
  "		float4 uvOffset;														\n"
  "		float4 vertexOffset;													\n"
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
  "		sampler2D normalSampler = sampler_state									\n"
  "		{																		\n"
  "			texture = (NormalTexture);											\n"
  "			MipFilter = Point;													\n"
  "		};																		\n"
  "																				\n"
  "		struct VS_INPUT															\n"
  "		{																		\n"
  "			float4 position: POSITION0;											\n"
  "			float2 params: TEXCOORD0;											\n"
  "		};																		\n"
  "																				\n"
  "		struct VS_OUTPUT														\n"
  "		{																		\n"
  "			float4 position: POSITION0;											\n"
  "			float2 uv: TEXCOORD0;												\n"
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
  "			float xscale = vertexOffset.x;										\n"
  "			float yscale = vertexOffset.y;										\n"
  "			float3 xaxis = vertexOffsetsX[vertexIndex];							\n"
  "			float3 yaxis = vertexOffsetsY[vertexIndex];							\n"
  "			float3 pos = IN.position.xyz + xaxis * xscale + yaxis * yscale;		\n"
  "			pos += vertexOffset.z * vertexOffsetsX[1];							\n"
  "			pos += vertexOffset.w * vertexOffsetsY[1];							\n"
  "			OUT.position = mul(float4(pos, 1.0), MVPmatrix);					\n"
  "			OUT.uv = IN.params + uvOffset.xy;									\n"
  "		}																		\n"
  "																				\n"
  "		void ps(in VS_OUTPUT IN, out PS_OUTPUT OUT)								\n"
  "		{																		\n"
  "			float3 normal = tex2D(normalSampler, IN.uv).rgb * 2.0 - 1.0;		\n"
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

static const char billboardMaterialSrcGBuffer[] = 
  "material { \n"
  "  effect { \n"
  "   source  \" \n"
  "     texture DiffuseTexture;													\n"
  "     texture NormalTexture;													\n"
  "		float4x4 MVPmatrix;														\n"
  "		float4x4 viewmatrix;													\n"
  "		float4 vertexOffsetsX[4];												\n"
  "		float4 vertexOffsetsY[4];												\n"
  "		float4 vertexOffset;													\n"
  "		float4 uvOffset;														\n"
  "																				\n"
  "		sampler2D diffuseSampler = sampler_state								\n"
  "		{																		\n"
  "			texture = (DiffuseTexture);											\n"
  "			MipFilter = None;													\n"
  "		};																		\n"
  "																				\n"
  "		sampler2D normalSampler = sampler_state									\n"
  "		{																		\n"
  "			texture = (NormalTexture);											\n"
  "			MipFilter = Point;													\n"
  "		};																		\n"
  "																				\n"
  "		struct VS_INPUT															\n"
  "		{																		\n"
  "			float4 position: POSITION0;											\n"
  "			float2 params: TEXCOORD0;											\n"
  "		};																		\n"
  "																				\n"
  "		struct VS_OUTPUT														\n"
  "		{																		\n"
  "			float4 position: POSITION0;											\n"
  "			float2 uv: TEXCOORD0;												\n"
  "			float4 viewpos: TEXCOORD1;											\n"
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
  "			float xscale = vertexOffset.x;										\n"
  "			float yscale = vertexOffset.y;										\n"
  "			float3 xaxis = vertexOffsetsX[vertexIndex];							\n"
  "			float3 yaxis = vertexOffsetsY[vertexIndex];							\n"
  "			float3 pos = IN.position.xyz + xaxis * xscale + yaxis * yscale;		\n"
  "			pos += vertexOffset.z * vertexOffsetsX[1];							\n"
  "			pos += vertexOffset.w * vertexOffsetsY[1];							\n"
  "			OUT.position = mul(float4(pos, 1.0), MVPmatrix);					\n"
  "			OUT.uv = IN.params + uvOffset.xy;									\n"
  "			OUT.viewpos = mul(float4(pos, 1.0), viewmatrix);					\n"
  "		}																		\n"
  "																				\n"
  "		void ps(in VS_OUTPUT IN, out PS_OUTPUT OUT)								\n"
  "		{																		\n"
  "			OUT.color[0] = tex2D(diffuseSampler, IN.uv);						\n"
  "			OUT.color[1].xyz = tex2D(normalSampler, IN.uv).rgb * 2.0 - 1.0;		\n"
  "			OUT.color[1].w = length(IN.viewpos);								\n"
  "		}																		\n"
  "																				\n"
  "     technique t0 {															\n"
  "       pass P0 {																\n"
  "         CullMode = None;													\n"
  "			AlphaTestEnable = True;												\n"
  "			AlphaRef = 128;														\n"
  "			AlphaFunc = Greater;												\n"
  "         VertexShader = compile vs_1_1 vs();									\n"
  "         PixelShader = compile ps_2_0 ps();									\n"
  "       }																		\n"
  "     }\";																	\n"
  "  }																			\n"
  "}";

TreeNode::TreeNode (void)
: _trunkModel(this)
, _leafModel(this)
{
	_renderMode = DRAW_NORMAL;
	_LODdistance = 2000.f;
	_billboardDirty = false;
	_billboardVertexDecl = 0;
	_randomSeedLeaves = ATOM_GetTick();
	_randomSeedTrunk = ATOM_GetTick();
}

TreeNode::~TreeNode (void)
{
}

void TreeNode::buildBoundingbox (void) const
{
	_boundingBox.beginExtend ();

	if (_trunkModel.isValid() && _trunkModel.isVisible())
	{
		_boundingBox.extend (_trunkModel.getBBox().getMin());
		_boundingBox.extend (_trunkModel.getBBox().getMax());
	}

	if (_leafModel.isValid() && _leafModel.isVisible())
	{
		_boundingBox.extend (_leafModel.getBBox().getMin());
		_boundingBox.extend (_leafModel.getBBox().getMax());
	}

	if (_boundingBox.getMin().x > _boundingBox.getMax().x)
	{
		_boundingBox.setMin (ATOM_Vector3f(-100.f, -100.f, -100.f));
		_boundingBox.setMax (ATOM_Vector3f(100.f, 100.f, 100.f));
	}
}

void TreeNode::checkBillboardValid (void)
{
	if ((_renderMode == DRAW_AUTO || _renderMode == DRAW_BILLBOARD) && _billboardDirty)
	{
		generateBillboard (ATOM_GetRenderDevice(), 1024, 1024);
		_billboardDirty = false;
	}
}

void TreeNode::setupRenderQueue (ATOM_CullVisitor *visitor)
{
	_trunkModel.setDrawBillboard (_renderMode == CREATE_BILLBOARD);
	_leafModel.setDrawBillboard (_renderMode == CREATE_BILLBOARD);
	ATOM_Matrix4x4f worldMatrix = getWorldMatrix();
	visitor->addDrawable (&_trunkModel, worldMatrix, _trunkModel.getMaterial());
	visitor->addDrawable (&_leafModel, worldMatrix, _leafModel.getMaterial());
	/*
	if (_renderMode != CREATE_BILLBOARD)
	{
		if (_renderMode == DRAW_AUTO)
		{
			const ATOM_Matrix4x4f &viewMatrix = visitor->getCamera()->getViewMatrix();
			const ATOM_Matrix4x4f &projMatrix = visitor->getCamera()->getProjectionMatrix();
			const ATOM_Matrix4x4f &worldMatrix = getWorldMatrix();
			ATOM_Vector3f cameraPos(viewMatrix.m30, viewMatrix.m31, viewMatrix.m32);
			ATOM_Vector3f worldPos(worldMatrix.m30, worldMatrix.m31, worldMatrix.m32);
			_cameraDistanceSq = (cameraPos - worldPos).getSquaredLength ();
		}

		bool gbuffer = (visitor->getFlags() & ATOM_CullVisitor::CULL_GBUFFER) != 0;
		if (gbuffer)
		{
			if (_trunkModel.isValid() && !_trunkModel.supportGBuffer ())
			{
				gbuffer = false;
			}
			else if (_leafModel.isValid() && !_leafModel.supportGBuffer ())
			{
				gbuffer = false;
			}
		}

		visitor->getRenderQueue().add (this, gbuffer ? ATOM_RenderQueue::GBUFFER_LAYER : ATOM_RenderQueue::SOLID_LAYER, getWorldMatrix(), this, 0, gbuffer ? 1 : 0);
	}
	else
	{
		visitor->getRenderQueue().add (this, ATOM_RenderQueue::SOLID_LAYER, getWorldMatrix(), this, 0, 2);
	}
	*/
}

bool TreeNode::rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const
{
	return false;
}
/*
void TreeNode::render(ATOM_RenderDevice* device, ATOM_Camera *camera, ATOM_MaterialObject *material, ATOM_Node *node, const ATOM_Matrix4x4f &worldMatrix, unsigned userdata, unsigned pass)
{
	RenderMode mode = _renderMode;

	if (_renderMode == DRAW_AUTO)
	{
		if (_cameraDistanceSq > _LODdistance * _LODdistance)
		{
			mode = DRAW_BILLBOARD;
		}
		else
		{
			mode = DRAW_NORMAL;
		}
	}

	if (mode == DRAW_BILLBOARD)
	{
		ATOM_Material *m = userdata == 1 ? _billboardMaterialGBuffer.get() : _billboardMaterial.get();

		if (userdata == 1)
		{
			ATOM_Matrix4x4f viewMatrix;
			device->getTransform (ATOM_MATRIXMODE_MODELVIEW, viewMatrix);
			m->getEffect()->getEffect()->setMatrixParameters ("viewmatrix", viewMatrix.m, 1);
		}
		ATOM_Matrix4x4f mvpMatrix;
		device->getTransform (ATOM_MATRIXMODE_MVP, mvpMatrix);
		m->getEffect()->getEffect()->setMatrixParameters ("MVPmatrix", mvpMatrix.m, 1);
		m->getEffect()->getEffect()->setTexture ("DiffuseTexture", _billboardTexture.get());
		m->getEffect()->getEffect()->setTexture ("NormalTexture", _billboardNormalTexture.get());

		ATOM_Matrix4x4f view;
		device->getTransform (ATOM_MATRIXMODE_VIEW, view);
		ATOM_Vector3f x_axis = view.getRow3 (0);
		ATOM_Vector3f y_axis = view.getRow3 (1);
		ATOM_Matrix4x4f invWorld = node->getInvWorldMatrix ();
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
		ATOM_Vector4f uvOffset[4] = {
			ATOM_Vector4f(0.f, 0.f, 0.f, 0.f),
			ATOM_Vector4f(0.5f, 0.f, 0.f, 0.f),
			ATOM_Vector4f(0.f, 0.5f, 0.f, 0.f),
			ATOM_Vector4f(0.5f, 0.5f, 0.f, 0.f)
		};
		m->getEffect()->getEffect()->setVectorParameters ("vertexOffsetsX", (const float*)offsetsX, 4);
		m->getEffect()->getEffect()->setVectorParameters ("vertexOffsetsY", (const float*)offsetsY, 4);

		int viewIndex;
		ATOM_Vector3f cameraPos(view.m30, view.m31, view.m32);
		cameraPos = invWorld.transformPointAffine (cameraPos);
		if (cameraPos.z < 0.f)
		{
			if (ATOM_abs(cameraPos.x) <= -cameraPos.z)
			{
				viewIndex = 0;
			}
			else if (cameraPos.x < 0.f)
			{
				viewIndex = 3;
			}
			else
			{
				viewIndex = 1;
			}
		}
		else
		{
			if (ATOM_abs(cameraPos.x) <= cameraPos.z)
			{
				viewIndex = 2;
			}
			else if (cameraPos.x < 0.f)
			{
				viewIndex = 3;
			}
			else
			{
				viewIndex = 1;
			}
		}
		ATOM_Vector4f voffset(_billboardInfo[viewIndex].width, _billboardInfo[viewIndex].height, _billboardInfo[viewIndex].originX, _billboardInfo[viewIndex].originY);
		m->getEffect()->getEffect()->setVectorParameters ("uvOffset", uvOffset[viewIndex].xyzw, 1);
		m->getEffect()->getEffect()->setVectorParameters ("vertexOffset", voffset.xyzw, 1);

		device->setVertexDecl (_billboardVertexDecl);
		device->setStreamSource (0, _billboardVertexArray.get());
		unsigned numPasses;
		if (m->begin (&numPasses))
		{
			for (unsigned pass = 0; pass < numPasses; ++pass)
			{
				if (m->beginPass (pass))
				{
					device->renderStreamsIndexed (_billboardIndexArray.get(), ATOM_PRIMITIVE_TRIANGLES, 2);
					m->endPass ();
				}
			}
			m->end ();
		}
	}
	else
	{
		switch (userdata)
		{
		case 0:
			_trunkModel.render (device, false);
			_leafModel.render (device, false);
			break;
		case 1:
			_trunkModel.render (device, true);
			_leafModel.render (device, true);
			break;
		case 2:
			_trunkModel.renderToBillboard (device);
			_leafModel.renderToBillboard (device);
			break;
		}
	}
}
*/
bool TreeNode::onLoad(ATOM_RenderDevice *device)
{
	if (_fileName.empty ())
	{
		TreeData data;
		initTreeData (data);

		if (!_trunkModel.setTreeData (data, _randomSeedTrunk))
		{
			return false;
		}
		return true;
	}

	char buffer[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (_fileName.c_str(), buffer);
	ATOM_TiXmlDocument doc(buffer);
	if (!doc.LoadFile ())
	{
		return false;
	}

	ATOM_TiXmlElement *eRoot = doc.RootElement();
	if (!eRoot)
	{
		return false;
	}

	ATOM_TiXmlElement *eRandomSeed = eRoot->FirstChildElement ("RandomSeed");
	if (eRandomSeed)
	{
		int value;
		if (ATOM_TIXML_SUCCESS == eRandomSeed->QueryIntAttribute ("Leaves", &value))
		{
			_randomSeedLeaves = value;
		}

		if (ATOM_TIXML_SUCCESS == eRandomSeed->QueryIntAttribute ("Trunk", &value))
		{
			_randomSeedTrunk = value;
		}
	}

	if (!_trunkModel.loadXML (eRoot, _randomSeedTrunk))
	{
		return false;
	}

	ATOM_TiXmlElement *eClusters = eRoot->FirstChildElement("Clusters");
	if (eClusters)
	{
		ATOM_TiXmlElement *eCluster = eClusters->FirstChildElement ("Cluster");
		while (eCluster)
		{
			double scale = 1;
			double tx = 0, ty = 0, tz = 0;
			eCluster->Attribute ("Scale", &scale);
			eCluster->Attribute ("TranslateX", &tx);
			eCluster->Attribute ("TranslateY", &ty);
			eCluster->Attribute ("TranslateZ", &tz);
			ATOM_Matrix4x4f o2t = ATOM_Matrix4x4f::getIdentityMatrix ();
			o2t.m00 = o2t.m11 = o2t.m22 = scale;
			o2t.m30 = tx;
			o2t.m31 = ty;
			o2t.m32 = tz;

			ATOM_HARDREF(ATOM_ShapeNode) shape;
			shape->setType (ATOM_ShapeNode::SPHERE);
			shape->setMaterialFileName ("/materials/builtin/shape.mat");
			ATOM_Material *m = shape->getMaterial();
			if (m)
			{
				float randomR = (float)rand() / float(RAND_MAX);
				float randomG = (float)rand() / float(RAND_MAX);
				float randomB = (float)rand() / float(RAND_MAX);
				m->getParameterTable ()->setVector ("diffuseColor", ATOM_Vector4f(randomR, randomG, randomB, 1.f));
			}
			shape->setO2T (o2t);
			appendChild (shape.get());

			eCluster = eCluster->NextSiblingElement ("Cluster");
		}
	}

	if (!_leafModel.loadXML (eRoot, _randomSeedLeaves))
	{
		return false;
	}

	return true;
}

void TreeNode::setFileName (const ATOM_STRING &fileName)
{
	_fileName = fileName;
}

const ATOM_STRING &TreeNode::getFileName (void) const
{
	return _fileName;
}

void TreeNode::setLODDistance (float d)
{
	_LODdistance = d;
}

float TreeNode::getLODDistance (void) const
{
	return _LODdistance;
}

bool TreeNode::generateBillboard (ATOM_RenderDevice *device, unsigned textureWidth, unsigned textureHeight)
{
	ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)> children;
	for (unsigned i = 0; i < getNumChildren(); ++i)
	{
		children.push_back (getChild (i));
	}
	for (unsigned i = 0; i < children.size(); ++i)
	{
		removeChild (children[i].get());
	}

	bool succ = createBillboardTextures (device, textureWidth, textureHeight) &&  updateBillboardGeometry (device);

	for (unsigned i = 0; i < children.size(); ++i)
	{
		appendChild (children[i].get());
	}

	return succ;
}

bool TreeNode::createBillboardTextures (ATOM_RenderDevice *device, unsigned w, unsigned h)
{
	if (!_billboardTexture || _billboardTexture->getWidth() != w || _billboardTexture->getHeight() != h)
	{
		_billboardTexture = 0;
		_billboardTexture = device->allocTexture (0, 0, w, h, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET|ATOM_Texture::NOCOMPRESS|ATOM_Texture::NOMIPMAP);
	}

	if (!_billboardNormalTexture || _billboardNormalTexture->getWidth() != w || _billboardNormalTexture->getHeight() != h)
	{
		_billboardNormalTexture = 0;
		_billboardNormalTexture = device->allocTexture (0, 0, w, h, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::RENDERTARGET|ATOM_Texture::NOCOMPRESS|ATOM_Texture::NOMIPMAP);
	}

	// save old parent
	ATOM_AUTOREF(ATOM_Node) oldParent = getParent ();
	if (oldParent)
	{
		oldParent->removeChild (this);
	}

	// save old draw mode
	RenderMode rm = getRenderMode ();
	int drawBBox = getDrawBoundingbox ();

	// save device states
	ATOM_AUTOREF(ATOM_Texture) oldRenderTarget0 = device->getRenderTarget (0);
	ATOM_AUTOREF(ATOM_Texture) oldRenderTarget1 = device->getRenderTarget (1);
	ATOM_AUTOREF(ATOM_DepthBuffer) oldDepthBuffer = device->getDepthBuffer ();
	ATOM_Rect2Di oldViewport = device->getViewport (0);
	ATOM_Rect2Di oldScissorRect = device->getScissorRect (0);
	bool scissorTestEnabled = device->isScissorTestEnabled (0);
	//bool isFogEnabled = device->isFogEnabled ();
	//bool wireframe = device->isWireFrameMode ();
	ATOM_ColorARGB clearColor = device->getClearColor (0);
	device->pushMatrix (ATOM_MATRIXMODE_WORLD);
	device->pushMatrix (ATOM_MATRIXMODE_VIEW);
	device->pushMatrix (ATOM_MATRIXMODE_PROJECTION);

	// initialize device states for billbard rendering
	ATOM_AUTOREF(ATOM_DepthBuffer) depthBuffer = device->allocDepthBuffer (w, h);
	device->setClearColor (0, 0.5f, 0.5f, 0.f, 0.f);
	//device->enableWireFrameMode (false);
	//device->enableFog (false);
	device->enableScissorTest (0, false);
	device->setRenderTarget (0, _billboardTexture.get());
	device->setRenderTarget (1, _billboardNormalTexture.get());
	device->setDepthBuffer (depthBuffer.get());
	device->setViewport (0, 0, 0, w, h);
	device->clear (true, true, true);

	// create new scene
	ATOM_SDLScene scene;
	scene.getRootNode()->appendChild (this);
	setRenderMode (CREATE_BILLBOARD);
	setDrawBoundingbox (0);

	// compute common parameters
	ATOM_BBox bbox = getBoundingbox ();
	float cw, ch, hw, hh;
	float left = bbox.getMin().x;
	float right = bbox.getMax().x;
	float bottom = bbox.getMin().y;
	float top = bbox.getMax().y;
	ATOM_Vector3f eyePos;
	ATOM_Vector3f eyeDir;
	ATOM_Vector3f boxExtents = bbox.getMax() - bbox.getMin();
	float fFar = ATOM_max3(boxExtents.x, boxExtents.y, boxExtents.z);
	unsigned viewportW = w / 2;
	unsigned viewportH = h / 2;
	int viewportX[4] = { 0, viewportW, 0, viewportW };
	int viewportY[4] = { 0, 0, viewportH, viewportH };

	if (device->beginFrame ())
	{
		// render four views
		for (unsigned i = 0; i < 4; ++i)
		{
			switch (i)
			{
			case 0:
				// look direction is +Z
				cw = (bbox.getMin().x + bbox.getMax().x) * 0.5f;
				ch = (bbox.getMin().y + bbox.getMax().y) * 0.5f;
				hw = (bbox.getMax().x - bbox.getMin().x) * 0.5f;
				hh = (bbox.getMax().y - bbox.getMin().y) * 0.5f;
				_billboardInfo[i].width = hw;
				_billboardInfo[i].height = hh;
				_billboardInfo[i].originX = bbox.getMin().x + hw;
				_billboardInfo[i].originY = bbox.getMin().y + hh;
				eyePos.set (cw, ch, bbox.getMin().z - 2.f);
				eyeDir.set (0.f, 0.f, 1.f);
				break;
			case 1:
				// look direction is -X
				cw = (bbox.getMin().z + bbox.getMax().z) * 0.5f;
				ch = (bbox.getMin().y + bbox.getMax().y) * 0.5f;
				hw = (bbox.getMax().z - bbox.getMin().z) * 0.5f;
				hh = (bbox.getMax().y - bbox.getMin().y) * 0.5f;
				_billboardInfo[i].width = hw;
				_billboardInfo[i].height = hh;
				_billboardInfo[i].originX = bbox.getMin().z + hw;
				_billboardInfo[i].originY = bbox.getMin().y + hh;
				eyePos.set (bbox.getMax().x + 2.f, ch, cw);
				eyeDir.set (-1.f, 0.f, 0.f);
				break;
			case 2:
				// look direction is -Z
				cw = (bbox.getMin().x + bbox.getMax().x) * 0.5f;
				ch = (bbox.getMin().y + bbox.getMax().y) * 0.5f;
				hw = (bbox.getMax().x - bbox.getMin().x) * 0.5f;
				hh = (bbox.getMax().y - bbox.getMin().y) * 0.5f;
				_billboardInfo[i].width = hw;
				_billboardInfo[i].height = hh;
				_billboardInfo[i].originX = -bbox.getMax().x + hw;
				_billboardInfo[i].originY = bbox.getMin().y + hh;
				eyePos.set (cw, ch, bbox.getMax().z + 2.f);
				eyeDir.set (0.f, 0.f, -1.f);
				break;
			case 3:
				// look direction is +X
				cw = (bbox.getMin().z + bbox.getMax().z) * 0.5f;
				ch = (bbox.getMin().y + bbox.getMax().y) * 0.5f;
				hw = (bbox.getMax().z - bbox.getMin().z) * 0.5f;
				hh = (bbox.getMax().y - bbox.getMin().y) * 0.5f;
				_billboardInfo[i].width = hw;
				_billboardInfo[i].height = hh;
				_billboardInfo[i].originX = -bbox.getMax().z + hw;
				_billboardInfo[i].originY = bbox.getMin().y + hh;
				eyePos.set (bbox.getMin().x - 2.f, ch, cw);
				eyeDir.set (1.f, 0.f, 0.f);
				break;
			}

			scene.getCamera()->setOrtho (-hw, hw, -hh, hh, 1.f, fFar + 10.f);
			scene.getCamera()->lookAt (eyePos, eyePos + eyeDir, ATOM_Vector3f(0.f, 1.f, 0.f)); 
			scene.getCamera()->setViewport (viewportX[i], viewportY[i], viewportW, viewportH);

			scene.render (device, false);
		}
		device->endFrame ();
	}
	// restore device states
	device->setRenderTarget (0, oldRenderTarget0.get());
	device->setRenderTarget (1, oldRenderTarget1.get());
	device->setDepthBuffer (oldDepthBuffer.get());
	device->setViewport (0, oldViewport);
	device->setScissorRect (0, oldScissorRect);
	device->enableScissorTest (0, scissorTestEnabled);
	//device->enableFog (isFogEnabled);
	//device->enableWireFrameMode (wireframe);
	device->setClearColor (0, clearColor);
	device->popMatrix (ATOM_MATRIXMODE_WORLD);
	device->popMatrix (ATOM_MATRIXMODE_VIEW);
	device->popMatrix (ATOM_MATRIXMODE_PROJECTION);

	// restore node tree
	scene.getRootNode()->removeChild (this);
	if (oldParent)
	{
		oldParent->appendChild (this);
	}

	// restore render mode
	setRenderMode (rm);
	setDrawBoundingbox (drawBBox);

	return true;
}

void TreeNode::setRenderMode (RenderMode renderMode)
{
	_renderMode = renderMode;
}

TreeNode::RenderMode TreeNode::getRenderMode (void)
{
	return _renderMode;
}

bool TreeNode::updateBillboardGeometry (ATOM_RenderDevice *device)
{
	/*
	if (!_billboardVertexArray)
	{
		_billboardVertexArray = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD_XYZW|ATOM_VERTEX_ATTRIB_TEX1_2, ATOM_USAGE_STATIC, 4, true);
	}

	struct MyVertex
	{
		ATOM_Vector4f position;
		ATOM_Vector2f params;
	};
	MyVertex *v = (MyVertex*)_billboardVertexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
	v->position.x = 0.f;
	v->position.y = 0.f;
	v->position.z = 0.f;
	v->position.w = 0.f;
	v->params.x = 0.f;
	v->params.y = 0.f;
	v++;

	v->position.x = 0.f;
	v->position.y = 0.f;
	v->position.z = 0.f;
	v->position.w = 1.f;
	v->params.x = 0.5f;
	v->params.y = 0.f;
	v++;

	v->position.x = 0.f;
	v->position.y = 0.f;
	v->position.z = 0.f;
	v->position.w = 2.f;
	v->params.x = 0.5f;
	v->params.y = 0.5f;
	v++;

	v->position.x = 0.f;
	v->position.y = 0.f;
	v->position.z = 0.f;
	v->position.w = 3.f;
	v->params.x = 0.f;
	v->params.y = 0.5f;
	v++;

	_billboardVertexArray->unlock ();

	if (!_billboardIndexArray)
	{
		_billboardIndexArray = device->allocIndexArray (ATOM_USAGE_STATIC, 6, false, true);
		unsigned short *p = (unsigned short*)_billboardIndexArray->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
		*p++ = 0;
		*p++ = 1;
		*p++ = 2;
		*p++ = 0;
		*p++ = 2;
		*p++ = 3;
		_billboardIndexArray->unlock ();
	}

	if (!_billboardMaterial)
	{
		_billboardMaterial = ATOM_HARDREF(ATOM_Material)();
		if (!_billboardMaterial->load (billboardMaterialSrc))
		{
			return false;
		}
	}

	if (!_billboardMaterialGBuffer)
	{
		_billboardMaterialGBuffer = ATOM_HARDREF(ATOM_Material)();
		if (!_billboardMaterialGBuffer->load (billboardMaterialSrcGBuffer))
		{
			return false;
		}
	}

	if (!_billboardVertexDecl)
	{
		_billboardVertexDecl = device->createInterleavedVertexDeclaration (ATOM_VERTEX_ATTRIB_COORD_XYZW|ATOM_VERTEX_ATTRIB_TEX1_2);
	}
	*/
	return true;
}

bool TreeNode::exportTreeModel (const char *filename)
{
	bool exportTrunk = _trunkModel.isValid() && _trunkModel.isVisible();
	bool exportLeaves = _leafModel.isValid() && _leafModel.isVisible();
	if (!exportTrunk && !exportLeaves)
	{
		::MessageBoxA (ATOM_GetRenderDevice()->getWindow()->getWindowInfo()->handle, "无可导出内容!", "ATOM Studio", MB_OK|MB_ICONHAND);
		return false;
	}

	if (_billboardDirty)
	{
		generateBillboard (ATOM_GetRenderDevice(), 1024, 1024);
		_billboardDirty = false;
	}

	if (!_billboardVertexArray || !_billboardIndexArray || !_billboardTexture || !_billboardNormalTexture)
	{
		return false;
	}

	unsigned numBillboardVertices = _billboardVertexArray->getNumVertices ();
	unsigned numBillboardIndices = _billboardIndexArray->getNumIndices ();
	unsigned version = 2;
	void *p;

	ATOM_AutoFile f(filename, ATOM_VFS::binary|ATOM_VFS::write);
	if (!f)
	{
		return false;
	}

	// write signature
	unsigned sig = ATOM_MAKE_FOURCC('A','T','T','R');
	f->write(&sig, sizeof(sig));

	// write version
	f->write (&version, sizeof(unsigned));

	// write bounding box
	invalidateBoundingbox ();
	ATOM_BBox bbox = getBoundingbox ();
	if (f->write (&bbox, sizeof(ATOM_BBox)) != sizeof(ATOM_BBox))
	{
		return false;
	}
	ATOM_BBox trunkBBox = _trunkModel.getBBox ();
	if (f->write (&trunkBBox, sizeof(ATOM_BBox)) != sizeof(ATOM_BBox))
	{
		return false;
	}
	ATOM_BBox leafBBox = _leafModel.getBBox ();
	if (f->write (&leafBBox, sizeof(ATOM_BBox)) != sizeof(ATOM_BBox))
	{
		return false;
	}

	if (!_trunkModel.exportToFile (f))
	{
		return false;
	}

	if (!_leafModel.exportToFile (f))
	{
		return false;
	}

	// write billboard info
	f->write (_billboardInfo, 4 * sizeof(BillboardInfo));

	// write billboard gemoetry
	f->write (&numBillboardVertices, sizeof(unsigned));
	p = _billboardVertexArray->lock (ATOM_LOCK_READONLY, 0, 0, false);
	f->write (p, numBillboardVertices * sizeof(float) * 6);
	_billboardVertexArray->unlock ();
	f->write (&numBillboardIndices, sizeof(unsigned));
	p = _billboardIndexArray->lock (ATOM_LOCK_READONLY, 0, 0, false);
	f->write (p, numBillboardIndices * sizeof(unsigned short));
	_billboardIndexArray->unlock ();

	// write billboard color texture
	unsigned texSize;
	void *colorTexture = _billboardTexture->saveToMemEx (ATOM_PIXEL_FORMAT_DXT1, ATOM_ImageCodec::DDS, &texSize);
	if (!colorTexture)
	{
		return false;
	}
	f->write (&texSize, sizeof(unsigned));
	f->write (colorTexture, texSize);
	ATOM_FREE(colorTexture);

	// write billboard normal texture
	void *normalTexture = _billboardNormalTexture->saveToMemEx (ATOM_PIXEL_FORMAT_DXT1, ATOM_ImageCodec::DDS, &texSize);
	if (!normalTexture)
	{
		return false;
	}
	f->write (&texSize, sizeof(unsigned));
	f->write (normalTexture, texSize);
	ATOM_FREE(normalTexture);

	// write LOD distance
	f->write (&_LODdistance, sizeof(float));

	return true;
}

bool TreeNode::save (const char *filename)
{
	char buffer[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (filename, buffer);
	ATOM_TiXmlDocument doc(buffer);

	ATOM_TiXmlDeclaration eDecl("1.0", "gb2312", "");
	doc.InsertEndChild (eDecl);

	ATOM_TiXmlElement eRoot("Params");

	ATOM_TiXmlElement eRandomSeed ("RandomSeed");
	eRandomSeed.SetAttribute ("Leaves", _randomSeedLeaves);
	eRandomSeed.SetAttribute ("Trunk", _randomSeedTrunk);
	eRoot.InsertEndChild (eRandomSeed);

	_trunkModel.writeXML (&eRoot);
	_leafModel.writeXML (&eRoot);

	ATOM_TiXmlElement eClusters("Clusters");
	for (unsigned i = 0; i < getNumChildren (); ++i)
	{
		ATOM_Node *node = getChild (i);
		ATOM_ShapeNode *shape = dynamic_cast<ATOM_ShapeNode*>(node);
		if (shape)
		{
			const ATOM_Matrix4x4f &o2t = shape->getO2T ();
			ATOM_TiXmlElement eCluster("Cluster");
			eCluster.SetDoubleAttribute ("Scale", o2t.m00);
			eCluster.SetDoubleAttribute ("TranslateX", o2t.m30);
			eCluster.SetDoubleAttribute ("TranslateY", o2t.m31);
			eCluster.SetDoubleAttribute ("TranslateZ", o2t.m32);
			eClusters.InsertEndChild (eCluster);
		}
	}
	eRoot.InsertEndChild (eClusters);

	doc.InsertEndChild (eRoot);

	return doc.SaveFile ();
}

void TreeNode::invalidateBillboard (void)
{
	_billboardDirty = true;
}

TrunkModel &TreeNode::getTrunk (void)
{
	return _trunkModel;
}

LeafModel &TreeNode::getLeaves (void)
{
	return _leafModel;
}

void TreeNode::regenerateLeaves (bool reseed)
{
	if (reseed)
	{
		_randomSeedLeaves = ATOM_GetTick();
	}
	_leafModel.regenerate (_randomSeedLeaves);
}

void TreeNode::regenerateTrunk (bool reseed)
{
	if (reseed)
	{
		_randomSeedTrunk = ATOM_GetTick();
	}
	_trunkModel.regenerate (_randomSeedTrunk);
}

unsigned TreeNode::getRandomSeedLeaves (void) const
{
	return _randomSeedLeaves;
}

unsigned TreeNode::getRandomSeedTrunk (void) const
{
	return _randomSeedTrunk;
}

#endif
