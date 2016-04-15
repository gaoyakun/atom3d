/**	\file engine/node.h
 *	SceneGraph节点基类.
 *
 *	\author 高雅昆
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_NODE_H
#define __ATOM3D_ENGINE_NODE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_geometry.h"
#include "../ATOM_kernel.h"
#include "../ATOM_render.h"

#include "camera.h"
#include "spatialcell.h"

#define ATOM_DECLARE_NODE_FILE_TYPE(classname, name, ext, desc)	\
	struct classname##_AutoRegisterFileType	\
	{	\
		classname##_AutoRegisterFileType(void)	\
		{	\
			ATOM_Node::registerNodeFileType (#name, ext, desc);	\
		}	\
	};	\
	static classname##_AutoRegisterFileType __classname##_auto_register_file_type;	\

#define ATOM_IMPLEMENT_NODE_FILE_TYPE(classname)	\
	classname::classname##_AutoRegisterFileType classname::__classname##_auto_register_file_type;	\

class ATOM_GameEntity;
class ATOM_Visitor;

//! \class ATOM_Node
//! SceneGraph节点基类.
//! \author 高雅昆
//! \todo 需要整理
//! \ingroup engine
class ATOM_ENGINE_API ATOM_Node : public ATOM_Object
{
	ATOM_CLASS(engine, ATOM_Node, Node)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_Node)
	friend class ATOM_SpatialCell;
	friend class ATOM_BaseNodeLoader;
	friend class BaseNodeCreator;

public:
	enum
	{
		HIDE = 0,
		SHOW = 1,
		HIDE_PICKABLE = 3
	};

	enum 
	{
		NONPICKABLE = 0,
		PICKABLE = 1
	};

	enum
	{
		NONTOPMOST = 0,
		TOPMOST = 1
	};

	enum
	{
		NONPROJECTABLE = 0,
		PROJECTABLE = 1
	};

	enum
	{
		NONANTITOPMOST = 0,
		ANTITOPMOST = 1
	};

	enum
	{
		INHERIT = -1
	};

public:
	//! 构造函数.
	ATOM_Node (void);

	//! 析构函数.
	virtual ~ATOM_Node (void);

public:
	//! 获取自己的父节点.
	//! \return 父节点，如果没有父节点返回NULL
	ATOM_Node *getParent(void) const;

	//! 设置描述.
	//! \param str 描述
	void setDescribe (const ATOM_STRING &str);

	//! 获取描述.
	//! \return 描述
	const ATOM_STRING &getDescribe (void) const;

	//! 通知节点包围盒需要重新计算.
	void invalidateBoundingbox (void);

	//! 获取该节点世界变换变化的时间戳，每次该节点的世界变换发生变化该时间戳会递增.
	//! \return 世界变换变化的时间戳
	unsigned getChangeStamp (void) const;

	//! 获取该节点所属的空间分割节点结构.
	//! 空间分割节点为八叉树节点，二叉树节点等,目前只有实现八叉树
	//! \return 空间分割节点结构指针
	//! \note 此函数主要为内部使用，外部无需关心
	//! \sa ATOM_SpatialCell::NodeInfo setCellNodeInfo
	ATOM_SpatialCell::NodeInfo *getCellNodeInfo (void) const;

	//! 设置该节点所属的空间分割节点结构.
	//! 空间分割节点为八叉树节点，二叉树节点等,目前只有实现八叉树
	//! \param cellNodeInfo 空间分割节点结构指针
	//! \note 此函数主要为内部使用，外部无需关心
	//! \sa ATOM_SpatialCell::NodeInfo getCellNodeInfo
	void setCellNodeInfo (ATOM_SpatialCell::NodeInfo *cellNodeInfo);

	//! 附加一个游戏单元到此节点.
	//! 主要用于将纯粹用于渲染的场景节点和游戏对象关联起来
	//! \param entity 游戏对象
	//! \sa ATOM_GameEntity getGameEntity
	void setGameEntity (ATOM_GameEntity *entity);

	//! 获取附加到此节点的游戏单元.
	//! \return 游戏对象
	//! \sa ATOM_GameEntity setGameEntity
	ATOM_GameEntity *getGameEntity (void) const;

	//! 装载此节点.
	//! 装载操作通常发生在节点内部属性已经被设定完成之后, 一般来说不要重载此函数，应该重载onLoad函数
	//! \param pDevice 渲染设备
	//! \return true成功 false失败
	bool load(ATOM_RenderDevice *pDevice);

	//! 通过文件装载此节点.
	//! 装载操作通常发生在节点内部属性已经被设定完成之后, 一般来说不要重载此函数，应该重载onLoad函数
	//! \param pDevice 渲染设备
	//! \param filename 文件名
	//! \return true成功 false失败
	bool loadFromFile(ATOM_RenderDevice *pDevice, const char *filename, int loadPriority = ATOM_LoadPriority_IMMEDIATE);	// wangjian modified : 异步加载 添加加载优先级（默认为非异步）

	//! 异步装载此节点.
	//! 装载操作通常发生在节点内部属性已经被设定完成之后, 一般来说不要重载此函数.
	//! \param pDevice 渲染设备
	//! \param flags 载入参数旗标
	//! \param callback 回调函数，在载入完成后被调用。如果为NULL则会使用默认的回调
	//! \param userData 自定义数据，在回调中使用
	//! \param group 载入组索引
	//! \return true成功 false失败
	bool mtload(ATOM_RenderDevice *pDevice, unsigned flags, ATOM_LoadingRequestCallback callback, void *userData, long group);

	//! 异步通过文件装载此节点.
	//! 装载操作通常发生在节点内部属性已经被设定完成之后, 一般来说不要重载此函数.
	//! \param pDevice 渲染设备
	//! \param filename 文件名
	//! \param flags 载入参数旗标
	//! \param callback 回调函数，在载入完成后被调用。如果为NULL则会使用默认的回调
	//! \param userData 自定义数据，在回调中使用
	//! \param group 载入组索引
	//! \return true成功 false失败
	bool mtloadFromFile(ATOM_RenderDevice *pDevice, const char *filename, unsigned flags, ATOM_LoadingRequestCallback callback, void *userData, long group);

	//! 设置是否允许多线程载入.
	//! \param enable 是否允许
	void enableMTLoading (bool enable);

	//! 查询是否允许多线程载入.
	//! \return true表示允许，false表示不允许
	bool isMTLoadingEnabled (void) const;

public:
	//! 允许一个ATOM_Visitor访问自己.
	//! 节点树的遍历采用了ATOM_Visitor设计模式
	//! \param visitor ATOM_Visitor对象
	virtual void accept (ATOM_Visitor &visitor);

	//! 输出信息到log.用于调试
	//! \param indent 缩进
	virtual void dump(const char *indent);

	//! 复制自身
	//! \return 复制的节点，复制失败返回NULL
	virtual ATOM_AUTOREF(ATOM_Node) clone (void) const;

	//! 查询自己是否支持多线程载入.
	//! \return 支持返回true，否则返回false
	virtual bool supportMTLoading (void);

	//! 获取节点在自己局部坐标系中的包围盒.
	//! \return 包围盒
	//! \sa getWorldBoundingbox buildBoundingbox buildWorldBoundingbox
	virtual const ATOM_BBox& getBoundingbox(void) const;

	//! 获取节点在当前世界坐标系下的包围盒.
	//! \return 包围盒
	//! \sa getBoundingbox buildBoundingbox buildWorldBoundingbox
	virtual const ATOM_BBox& getWorldBoundingbox(void) const;

	//! 获取节点当前的世界变换矩阵.
	//! 此变换矩阵由所有父节点一直到根节点的变换级联得来
	//! \param projectionMatrix 当前的投影变换，可能用来计算世界变换
	//! \param viewMatrix 当前的摄像机变换，可能用来计算世界变换
	//! \return 世界变换矩阵
	//! \sa getInvWorldMatrix getO2T setO2T
	virtual const ATOM_Matrix4x4f & getWorldMatrix (void) const;

	//! 获取节点当前的世界变换矩阵的逆矩阵.
	//! \param projectionMatrix 当前的投影变换，可能用来计算世界变换
	//! \param viewMatrix 当前的摄像机变换，可能用来计算世界变换
	//! \return 世界变换矩阵的逆矩阵
	//! \sa getWorldMatrix getO2T setO2T
	virtual const ATOM_Matrix4x4f & getInvWorldMatrix (void) const;

	//! 设置是否禁止几何剪裁
	//! \param b true 禁止几何剪裁 false 允许几何剪裁
	virtual void skipClipTest (bool b);

	//! 查询是否禁止几何剪裁
	//! \return true 禁止几何剪裁 false 允许几何剪裁
	virtual bool isSkipClipTest (void) const;

	//! 是否支持固定管线渲染
	//! \return true 支持 false 不支持
	virtual bool supportFixedFunction (void) const;

public:
	//! 添加一个节点为自己的子节点.
	//! \param node 要添加的节点
	//! \return true表示成功，false表示失败，失败一般是因为传入节点参数为空指针或者传入的节点已经是自己的子节点了
	virtual bool appendChild(ATOM_Node *node);

	//! 删除自己的一个子节点.
	//! \param node 要删除的节点
	//! \return true表示成功，false表示失败，失败一般是因为传入节点参数为空指针或者传入的节点并不是自己的子节点
	virtual bool removeChild(ATOM_Node *node);

	//! 获取自己的子节点数量.
	//! \return 子节点数量
	virtual unsigned getNumChildren (void) const;

	//! 获取指定索引的一个子节点.
	//! \param index 子节点索引，取值范围为0到getNumChildren()-1
	virtual ATOM_Node *getChild (unsigned index) const;

	//! 删除所有子节点.
	virtual void clearChildren(void);

	//! 查询一个节点是否是自己的子节点.
	//! \param node 要查询的节点
	//! \return true表示是自己的子节点，否则不是自己的子节点
	virtual bool hasChild(ATOM_Node *node) const;

	//! 查询指定名称的参考点
	//! \param name 参考点的名称
	//! \param matrix 输出参数, 保存该参考点的变换矩阵
	//! \return true存在指定参考点 false不存在
	virtual bool queryReferencePoint (const char *name, ATOM_Matrix4x4f *matrix) const;

	//! 获取所有参考点列表
	virtual void getReferencePointList (ATOM_VECTOR<ATOM_STRING> &pointlist) const;

public:
	//! 获取自己相对父节点的变换矩阵.
	//! \return 相对父节点的变换矩阵
	//! \sa setO2T getWorldMatrix getInvWorldMatrix
	virtual const ATOM_Matrix4x4f & getO2T(void) const;

	//! 设置自己相对父节点的变换矩阵.
	//! \param Mo2t 相对父节点的变换矩阵
	//! \sa getO2T getWorldMatrix getInvWorldMatrix
	virtual void setO2T(const ATOM_Matrix4x4f &Mo2t);

	//! 获取载入接口
	//! \return 载入接口
	virtual ATOM_LoadInterface *getLoadInterface (void) const;

	//virtual ATOM_Vector3f getOrigin(void) const;
	//virtual ATOM_Vector3f this2Other(const ATOM_Vector3f &V) const;
	//virtual ATOM_Vector3f other2This(const ATOM_Vector3f &V) const;
	//virtual void setOrigin(const ATOM_Vector3f &V);
	//virtual void translate(const ATOM_Vector3f &V);
	//virtual void otherRotate(const ATOM_Vector3f &Axis, float fAngle);
	//virtual void otherRotate(const ATOM_Vector3f &Axis, float tSin, float tCos);	//如果用在CAMERA上，可以出现世界旋转的效果
	//virtual void rotateOther(const ATOM_Vector3f &Axis, float fAngle);
	//virtual void rotateOther(const ATOM_Vector3f &Axis, float tSin, float tCos);
	//virtual void rotateThis(const ATOM_Vector3f &Axis, float fAngle);
	//virtual void rotateThis(const ATOM_Vector3f &Axis, float tSin, float tCos);
	//virtual void lookAt(float eyex, float eyey, float eyez, float centerx, float centery, float centerz, float upx, float upy, float upz);
	//virtual void setTransform(const ATOM_Vector3f &Origin, const ATOM_Vector3f& Axis, float tSin, float tCos);
	//virtual void setTransform(const ATOM_Vector3f &Origin, const ATOM_Vector3f& Axis, float fAngle);

protected:
	//! 计算自己的包围盒.
	//! 每种节点都必须重载此函数执行具体的计算
	//! \sa getBoundingbox getWorldBoundingbox buildWorldBoundingbox
	virtual void buildBoundingbox (void) const;

	//! 计算自己在世界空间的包围盒.
	//! 如果已经实现了buildBoundingbox，也可以不用重载此函数，此函数会自动变换局部坐标系包围盒到世界坐标系
	//! \param projectionMatrix 当前的投影矩阵
	//! \param viewMatrix 当前的摄像机矩阵
	//! \sa getBoundingbox getWorldBoundingbox buildBoundingbox
	virtual void buildWorldBoundingbox (void) const;

	//! 计算自己的世界变换矩阵.
	//! 一般不需要重载
	//! \param projectionMatrix 当前的投影变换
	//! \param viewMatrix 当前的摄像机变换
	//! \sa getWorldMatrix getInvWorldMatrix buildInvWorldMatrix
	virtual void buildWorldMatrix (void) const;

	//! 计算自己的世界变换矩阵的逆矩阵.
	//! 一般不需要重载
	//! \param projectionMatrix 当前的投影变换
	//! \param viewMatrix 当前的摄像机变换
	//! \sa getWorldMatrix getInvWorldMatrix buildWorldMatrix
	virtual void buildInvWorldMatrix (void) const;

	//! 当节点的变换矩阵发生了变化会调用此函数.
	//! 一般不需要重载
	virtual void onTransformChanged (void);

	//! 执行具体的载入.
	//! \param device 渲染设备
	//! \return true成功 false失败
	//! \sa load
	virtual bool onLoad (ATOM_RenderDevice *device);

	//! 执行具体的异步载入.
	//! \param device 渲染设备
	//! \param flags 载入参数旗标
	//! \param callback 异步载入完成后的通知回调函数
	//! \param userData 自定义数据，可在回调函数中取用
	//! \param group 载入组索引
	//! \return true成功 false失败
	//! \sa mtload
	virtual bool onMtLoad (ATOM_RenderDevice *device, unsigned flags, ATOM_LoadingRequestCallback callback, void *userData, long group);

	//! 多线程载入的情况下从硬盘读取的实现，此操作在IO线程执行.
	//! \return 成功返回true，否则返回false
	virtual bool mtLoadFromDisk (ATOM_BaseResourceLoader *loader);

	//! 多线程载入的情况下锁定设备相关对象，此操作在图形线程（一般来说是主线程）执行.
	//! \param device 渲染设备
	//! \param loader loader指针
	//! \return 成功返回true，否则返回false
	virtual bool mtLock (ATOM_RenderDevice *device, ATOM_BaseResourceLoader *loader);

	//! 多线程载入的情况下上载设备数据，此操作在mtLock执行成功以后在单独的载入线程中执行.
	//! \return 成功返回true，否则返回false
	virtual bool mtUploadDeviceData (ATOM_BaseResourceLoader *loader);

	//! 多线程载入的情况下解锁锁定设备相关对象，此操作在mtUploadDeviceData执行完后在图形线程（一般来说是主线程）中执行.
	//! \param device 渲染设备
	//! \param loader loader对象指针
	//! \return 成功返回LOADINGSTAGE_FINISH，失败返回LOADINGSTAGE_NOTLOADED，等待返回LOADINGSTAGE_TRYAGAIN
	virtual ATOM_LoadingStage mtUnlock (ATOM_RenderDevice *device, ATOM_BaseResourceLoader *loader);

	//! 等待自身载入完成
	virtual void mtWait (void);

	//! 复制
	//! \param other 复制目标
	virtual void assign (ATOM_Node *other) const;

public:
	int getShow (void) const;
	int getPickable (void) const;

public:
	static bool registerNodeFileType (const char *classname, const char *fileext, const char *desc);
	static unsigned getNumRegisteredFileTypes (void);
	static void getRegisteredFileInfo (unsigned index, ATOM_STRING &fileext, ATOM_STRING &filedesc);
	//--- wangjian modified ---//
	// 异步加载：增加了异步加载优先级（默认为非异步）
	static ATOM_AUTOREF(ATOM_Node) loadNodeFromFile ( const char *filename, 
													  int loadPriority = ATOM_LoadPriority_IMMEDIATE,
													  int loadFlag = 0 );
	//-------------------------//

public:
	int getPickable2 (void) const;
	void setPickable (int pickable);
	void setPickableRecursive (int enable);
	int getShow2(void) const;
	void setShow(int show);
	void setShowRecursive (int show);
	int isContributeCullingEnabled (void) const;
	void enableContributeCulling (int b);
	void enableContributeCullingRecursive (int b);
	const ATOM_STRING &getLayer (void) const;
	void setLayer (const ATOM_STRING &layer);
	void ignoreFog (int ignore);
	void ignoreFogRecursive (int ignore);
	int isIgnoreFog (void) const;
	void setDrawBoundingbox (int bDraw);
	int getDrawBoundingbox (void) const;
	void setNoOccludedFrameStamp (unsigned frameStamp);
	unsigned getNoOccludedFrameStamp (void) const;
	void setRotation (const ATOM_Vector4f &quaterion);
	const ATOM_Vector4f &getRotation (void) const;
	void setTranslation (const ATOM_Vector3f &translation);
	const ATOM_Vector3f &getTranslation (void) const;
	void setScale (const ATOM_Vector3f &scale);
	const ATOM_Vector3f &getScale (void) const;
	const ATOM_Vector3f &getWorldTranslation (void) const;
	const ATOM_Vector3f &getWorldScale (void) const;
	const ATOM_Vector4f &getWorldRotation (void) const;
	void setNodeFileName (const ATOM_STRING &filename);
	const ATOM_STRING &getNodeFileName (void) const;
	void setTopMost (int bTopMost);
	int getTopMost (void) const;
	int getTopMost2 (void) const;
	void setAntiTopMost (int bAntiTopMost);
	int getAntiTopMost (void) const;
	int getAntiTopMost2 (void) const;
	void setProjectable (int bProjectable);
	int getProjectable (void) const;
	unsigned getVisibleStamp (void) const;
	void setVisibleStamp (unsigned value);
	void setPrivate (int bPrivate);
	int getPrivate (void) const;
	void getCombinedBoundingBox (ATOM_BBox &bbox) const;
	const ATOM_Vector3f &getPredefinedBBoxMin (void) const;
	void setPredefinedBBoxMin (const ATOM_Vector3f &v);
	const ATOM_Vector3f &getPredefinedBBoxMax (void) const;
	void setPredefinedBBoxMax (const ATOM_Vector3f &v);
	void enableGeometryClipping (bool enable);
	bool isGeometryClippingEnabled (void) const;


private:
	void updateDecomposition (void) const;
	void updateComposition (void);
	void updateWorldDecomposition (void) const;

protected:
	ATOM_Node *_parent;
	int _pickable;
	int _showMode;
	int _projectable;
	int _contributeCulling;
	int _ignoreFog;
	int _drawBoundingbox;
	int _topMost;
	int _antiTopMost;
	int _private;
	bool _noOccludedFrameStamp;
	bool _enableMTLoading;
	bool _skipClipTest;
	bool _enableGeoClipping;
	ATOM_BBox _predefinedBBox;

	mutable unsigned _changeStamp;
	mutable unsigned _worldMatrixStamp;
	mutable unsigned _iworldMatrixStamp;
	mutable unsigned _bboxStamp;
	mutable unsigned _wbboxStamp;

	mutable ATOM_Matrix4x4f _worldMatrix;
	mutable ATOM_Matrix4x4f _iworldMatrix;
	mutable ATOM_BBox _boundingBox;
	mutable ATOM_BBox _boundingBoxW;
	mutable bool _decomposeDirty;
	mutable ATOM_Matrix4x4f _rotation;
	mutable ATOM_Vector3f _translation;
	mutable ATOM_Vector3f _scale;
	mutable ATOM_Vector4f _quat;
	mutable ATOM_Vector3f _scaleW;
	mutable ATOM_Vector3f _translationW;
	mutable ATOM_Vector4f _rotationW;
	mutable bool _worldTransformDirty;
	mutable unsigned _visibleStamp;

	ATOM_Transformf _transform;

	typedef ATOM_VECTOR<ATOM_AUTOREF(ATOM_Node)> NodeVector;
	typedef NodeVector::iterator NodeVectorIter;
	typedef NodeVector::const_iterator NodeVectorConstIter;
	NodeVector _children;

	ATOM_GameEntity *_entity;
	ATOM_STRING _layer;
	ATOM_STRING _describe;
	ATOM_STRING _filenameToBeLoad;

	ATOM_SpatialCell::NodeInfo *_cellNodeInfo;

	mutable ATOM_AUTOPTR(ATOM_LoadInterface) _loadInterface;

	//--- wangjian added ---//
	// 异步加载：相关函数和成员
public:
	enum 
	{
		LOAD_NOTLOAD = 0,		// 加载尚未开始
		LOAD_LOADSTARTED,		// 加载已经开始
		LOAD_ATTRIBUTE_LOADED,	// 属性文件加载阶段
		LOAD_LOADED,
		LOAD_ALLFINISHED,		// 

		LOAD_DELAYLOAD = 255,
	};

	void				setLoadFlag(unsigned flag);
	unsigned			getLoadFlag(void) const;					
	void				setLoadPriority(int pri);
	int					getLoadPriority (void) const;
	bool				checkNodeLoadStarted() const;
	bool				checkNodeLoadFinished();
	virtual bool		checkNodeAllFinshed();
	virtual void		onLoadFinished();

	// 重置参数表的脏标记
	void				resetMaterialDirtyFlag();

	unsigned			_load_flag;
	int					_load_priority;

	// 节点属性文件异步加载对象
	/*ATOM_AsyncableLoadAttributeFile					* getAttribFileLoader( const char* filename = 0 );
	ATOM_AUTOPTR(ATOM_AsyncableLoadAttributeFile)	_attribFileLoader;*/
	
protected:
	virtual void		resetMaterialDirtyFlag_impl();

	ATOM_AUTOREF(ATOM_NodeAttributeFile)	_attribFile;
	//----------------------//
};

#endif //__ATOM3D_ENGINE_NODE_H
/*! @} */
