#ifndef __ATOM3D_ENGINE_TREEBATCH_H
#define __ATOM3D_ENGINE_TREEBATCH_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <ATOM_math.h>
#include <ATOM_kernel.h>
#include <ATOM_render.h>

#include "drawable.h"

class ATOM_TreeInstance;

class ATOM_TreeTrunkBatch: private ATOM_Noncopyable
{
	struct ConstantLayout
	{
		ATOM_Vector4f vpMatrix[4];
		ATOM_Vector4f ivMatrix[3];
		ATOM_Vector4f worldMatrix[50 * 3];
	};

public:
	ATOM_TreeTrunkBatch (void);
	~ATOM_TreeTrunkBatch (void);

public:
	void setCore (ATOM_TreeInstance *instance);
	ATOM_TreeInstance *getCore (void) const;
	bool begin (ATOM_RenderDevice *device, bool gbuffer, bool hwInstancing, bool shadowmap);
	void drawInstance (ATOM_RenderDevice *device, const ATOM_Matrix4x4f &worldMatrix, const ATOM_Vector3f &worldScale);
	void end (ATOM_RenderDevice *device);

private:
	ConstantLayout _constants;
	unsigned _numInstances;
	ATOM_TreeInstance *_core;
	bool _drawGBuffer;
	bool _drawInstancing;
	bool _shadowMap;
	ATOM_AUTOPTR(ATOM_Material) _material;
	ATOM_AUTOPTR(ATOM_Material) _materialInstancing;
	ATOM_AUTOPTR(ATOM_Material) _materialShadowmap;
	ATOM_VertexDecl _vertexDecl;
	ATOM_VertexDecl _vertexDeclInstancing;
	ATOM_VertexDecl _vertexDeclShadowmap;
};

class ATOM_TreeLeavesBatch: private ATOM_Noncopyable
{
	struct ConstantLayout
	{
		ATOM_Vector4f projMatrix[4];
		ATOM_Vector4f ivMatrix[3];
		ATOM_Vector4f vertexUV[4];
		ATOM_Vector4f normals[4];
		ATOM_Vector4f x[4];
		ATOM_Vector4f y[4];
		ATOM_Vector4f instanceDatas[50 * 4];
	};

public:
	ATOM_TreeLeavesBatch (void);
	~ATOM_TreeLeavesBatch (void);

public:
	void setCore (ATOM_TreeInstance *instance);
	ATOM_TreeInstance *getCore (void) const;
	bool begin (ATOM_RenderDevice *device, bool gbuffer, bool hwInstancing, bool shadowmap);
	void drawInstance (ATOM_RenderDevice *device, const ATOM_Matrix4x4f &worldMatrix, const ATOM_Vector3f &worldScale);
	void end (ATOM_RenderDevice *device);

private:
	ConstantLayout _constants;
	unsigned _numInstances;
	ATOM_TreeInstance *_core;
	bool _drawGBuffer;
	bool _drawInstancing;
	bool _shadowMap;
	ATOM_AUTOPTR(ATOM_Material) _material;
	ATOM_AUTOPTR(ATOM_Material) _materialInstancing;
	ATOM_AUTOPTR(ATOM_Material) _materialShadowmap;
	ATOM_VertexDecl _vertexDecl;
	ATOM_VertexDecl _vertexDeclInstancing;
	ATOM_VertexDecl _vertexDeclShadowmap;
};

class ATOM_TreeBatch: public ATOM_Drawable
{
	struct InstanceTransformInfo
	{
		const ATOM_Matrix4x4f *worldMatrix;
		const ATOM_Vector3f *worldScale;
	};

public:
	ATOM_TreeBatch (ATOM_TreeInstance *instance);
	~ATOM_TreeBatch (void);

public:
	virtual bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);

public:
	void addInstance (int orientation, const ATOM_Matrix4x4f *worldMatrix, const ATOM_Vector3f *worldScale);
	void clear (void);
	void render (ATOM_RenderDevice *device);
	void enableInstancing (bool enable);
	void enableGBuffer (bool enable);
	void enableShadowMap (bool enable);
	void setQueued (bool b);
	bool queued (void) const;

private:
	bool _instancing;
	bool _gbuffer;
	bool _shadowmap;
	bool _queued;
	ATOM_TreeTrunkBatch *_trunkBatch;
	ATOM_TreeLeavesBatch *_leavesBatch;
	ATOM_VECTOR<InstanceTransformInfo> _transformInfos;
	ATOM_VECTOR<InstanceTransformInfo> _billboardTransformInfos[4];
};

#endif // __ATOM3D_ENGINE_TREEBATCH_H
