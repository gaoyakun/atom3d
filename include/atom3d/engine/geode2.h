#ifndef __ATOM3D_ENGINE_GEODE2_H
#define __ATOM3D_ENGINE_GEODE2_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "basedefs.h"
#include "node.h"
#include "model2.h"
#include "visualnode.h"

class ATOM_Geode;
class ATOM_RenderQueue;
class ATOM_Camera;
class ATOM_ActionMixer2;

class ATOM_ENGINE_API ATOM_StaticMesh: public ATOM_ReferenceObj, public ATOM_Drawable
{
public:
	ATOM_StaticMesh (ATOM_Geode *geode, ATOM_SharedMesh *sharedMesh);
	virtual ~ATOM_StaticMesh (void);

public:
	ATOM_SharedMesh *getSharedMesh (void) const;
	void setMaterial (ATOM_Material *material);
	ATOM_Material *getMaterial (void) const;
	void setVisible (bool visible);
	bool getVisible (void) const;
	void setTransparency (float transparency);

	//---- wangjian added  ----//
	//! 设置全局颜色强度因子
	void setColorMultiplier (const float colorMultiplier);
	//-------------------------//

public:
	virtual bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);

	//--- wangjian added ---//
	// 绘制实例化
	virtual bool		drawBatch(	ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material, 
									bool bInstancing = true, sInstancing * instancing = 0	);

	virtual void		batching(	sInstancing * instanceWrapper, ATOM_Camera *camera	);

	// 在放入RENDERQUEUE之前进行
	virtual void		prepareForSort(ATOM_RenderScheme* renderScheme);
	// 设置绘制排序标记
	virtual void		setupSortFlag(int queueid);
#if 1
	ATOM_STRING			getModelFileName();
#endif
	// 重置材质参数表的脏标记
	void resetMaterialDirtyFlag();

	const ATOM_Matrix4x4f & getWorldTransform(ATOM_Camera *camera) const;
	//----------------------//

private:
	ATOM_Geode *_geode;
	ATOM_SharedMesh *_sharedMesh;
	bool _visible;
	bool _trans;
	ATOM_AUTOPTR(ATOM_Material) _material;
};

class ATOM_ENGINE_API ATOM_Geode: public ATOM_VisualNode
{
public:
	enum
	{
		ACTIONFLAGS_UPSIDE = (1<<0),
		ACTIONFLAGS_DOWNSIDE = (1<<1),
		ACTIONFLAGS_FORCERESET = (1<<2)
	};
	typedef void (ATOM_CALL *AttachmentTransformCallback)(ATOM_Geode *geode, const char *boneName, ATOM_Node *attachedNode, ATOM_Matrix4x4f *m, void *userData);

public:
	ATOM_Geode (void);
	virtual ~ATOM_Geode (void);

public:
	virtual bool removeChild(ATOM_Node *node);
	virtual void accept (ATOM_Visitor &visitor);
	virtual bool supportMTLoading (void);
	virtual void clear (void);
	virtual void setupRenderQueue (ATOM_CullVisitor *visitor);
	virtual void update (ATOM_Camera *camera);
	virtual bool rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const;
	virtual bool queryReferencePoint (const char *name, ATOM_Matrix4x4f *matrix) const;
	virtual void getReferencePointList (ATOM_VECTOR<ATOM_STRING> &pointlist) const;

protected:
	virtual bool onLoad(ATOM_RenderDevice *device);
	virtual void buildBoundingbox (void) const;

private:
	bool loadModel (ATOM_RenderDevice *device, const char *filename);
	bool loadXML (ATOM_RenderDevice *device, const char *filename);

public:
	bool addSharedModel (const char *modelFileName);

	// wangjian modified
#if 0
	bool addSharedModel (ATOM_SharedModel *model);
#else
	bool addSharedModel (ATOM_SharedModel *model, const int loadPriority);
#endif

	bool removeSharedModel (ATOM_SharedModel *model);
	bool findSharedModel (ATOM_SharedModel *model);
	unsigned getNumMeshes (void) const;
	ATOM_StaticMesh *getStaticMesh (unsigned index) const;
	ATOM_StaticMesh *getStaticMeshByName (const char *name) const;
	void deleteStaticMesh (int index);
	unsigned getNumTracks (void) const;
	ATOM_JointAnimationTrack *getTrack (unsigned index) const;
	ATOM_JointAnimationTrack *getTrackByName (const char *name) const;
	ATOM_Skeleton *getSkeleton (void) const;

	unsigned getNumComponents (void) const;
	ATOM_Components getComponents (unsigned index) const;

	// wangjian modified 
	// 异步加载相关
#if 0
	bool addComponents (ATOM_Components components);
#else
	bool addComponents (ATOM_Components components, const int loadPriority);
#endif

	bool removeComponents (ATOM_Components components);
	bool removeComponents (unsigned index);
	void removeAllComponents (void);
	void syncComponents (void);

public:
	virtual bool attach (const char *attachPoint, ATOM_Node *node, AttachmentTransformCallback transformCallback = 0, void *userData = 0);
	virtual void detach (ATOM_Node *node);
	virtual void updateAttachments (void);
	virtual bool updateComponentsOpHistory (void);
	virtual void resetActions (unsigned flags);
	virtual bool doAction (const char *actionName, unsigned flags, unsigned loopCount = 0, bool forceReset = false, unsigned fadeTime = 500, float speed = 0.f);
	virtual void stopAction (void);

public:
	void setDefaultActionSpeed (unsigned flags, float speed);
	void setCurrentActionSpeed (unsigned flags, float speed);
	ATOM_ActionMixer2 *getActionMixer (void) const;
	void enableManualBoneTransform (const char *boneName, const ATOM_Matrix4x4f &matrix);
	void disableManualBoneTransform (const char *boneName);
	bool isBoneManualTransformEnabled (const char *boneName) const;
	int getBoneIndex (const char *name) const;
	bool getBoneMatrix(int bone, ATOM_Matrix4x4f& mat, bool suppressManualTransform = false);
	bool getBoneMatrix(const char *boneName, ATOM_Matrix4x4f &mat, bool suppressManualTransform = false);
	void setTransparency (float val);
	float getTransparency (void) const;
	void enableZTransparency (int enable);
	int isZTransparencyEnabled (void) const;

	//---- wangjian added  ----//
	//! 设置全局颜色强度因子
	void setColorMultiplier (const float colorMultiplier);
	//! 获取全局颜色强度因子
	const float getColorMultiplier (void) const;
	//-------------------------//


public:
	const ATOM_VECTOR<ATOM_STRING> &getModelFileNames (void) const;
	void setModelFileNames (const ATOM_VECTOR<ATOM_STRING> &modelFileNames);
	const ATOM_STRING &getModelFileName (void) const;
	void setModelFileName (const ATOM_STRING &filename);

private:
	void updatePickBox (void) const;

protected:
	float	_transparency;
	int		_zTransparency;
	float	_colorMultiplier;						// wangjian added

	ATOM_VECTOR<ATOM_Components> _components;
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_StaticMesh)> _meshes;
	ATOM_VECTOR<ATOM_AUTOPTR(ATOM_JointAnimationTrack)> _tracks;
	ATOM_AUTOPTR(ATOM_Skeleton) _skeleton;
	ATOM_ActionMixer2 *_actionMixer;

	ATOM_VECTOR<ATOM_STRING> _modelFileNames;
	ATOM_STRING _modelFileName;
	mutable ATOM_BBox *_pickBox;
	mutable bool _pickBoxDirty;

	struct AttachInfo
	{
		ATOM_STRING attachPoint;
		bool attached;
		unsigned bone;
		AttachmentTransformCallback callback;
		void *userData;
		ATOM_AUTOREF(ATOM_Node) node;
	};
	ATOM_VECTOR<AttachInfo> _attachments;

	ATOM_CLASS(engine, ATOM_Geode, Geode)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_Geode)
	ATOM_DECLARE_NODE_FILE_TYPE(ATOM_Geode, Geode, "nm;nm2;geode", "ATOM3D geode2")

	//--- wangjian added ---//
	// 异步加载：相关函数和成员
public:
	//virtual bool checkNodeAllFinshed();
	virtual void onLoadFinished();
	
protected:
	virtual void resetMaterialDirtyFlag_impl();

	void postProcessLoad();
	bool processComponent(int index);
	ATOM_VECTOR<int> _component_setFlags;
	//----------------------//
};

#endif // __ATOM3D_ENGINE_GEODE2_H
