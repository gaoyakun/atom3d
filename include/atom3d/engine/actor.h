/**	\file engine/actor.h
 *	角色节点基类.
 *
 *	\author 高雅昆
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_ACTOR_H
#define __ATOM3D_ENGINE_ACTOR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"
#include "../ATOM_kernel.h"
#include "../ATOM_render.h"

#include "basedefs.h"
#include "components.h"

class ATOM_Actor;

//! \class ATOM_ActorPart
//! 部件类型.
//! 角色中的部件类型基类
//! \author 高雅昆
//! \ingroup engine
//! 
class ATOM_ENGINE_API ATOM_ActorPart: public ATOM_Object
{
public:
	//! 候选者索引类型
	typedef int CandidateId;
	static const int invalid_Id;
	static const int any_Id;

private:
	struct CandidateInfo
	{
		ATOM_STRING desc;
		CandidateId id;
	};

public:
	//! 构造函数
	ATOM_ActorPart (void);

	//! 析构函数
	virtual ~ATOM_ActorPart (void);

	//! 执行准备动作.
	//! 此函数为内部使用，渲染前角色中的每个部件会调用此函数
	virtual void apply (void) = 0;

	//! 设置此部件的颜色.
	//! \param color 颜色
	//! \param property 网格属性
	virtual void setColor (const ATOM_Vector4f &color, const char *property) = 0;

	//! 设置此部件的贴图.
	//! \param albedo 颜色贴图
	//! \param normalmap 法线贴图
	//! \param property 网格属性
	virtual void setTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap, const char *property) = 0;

	//! 同步此部件.
	virtual void sync (void) = 0;

	//! 通过XML文件载入
	//! 将会载入此部件的所有候选列表(包括候选者文件名和索引值)以及当前的候选
	//! \param xmlElement XmlElement
	//! \return true 成功 false 失败
	virtual bool loadFromXml (ATOM_TiXmlElement *xmlElement);

	//! 写入XML文件
	//! 将会写入此部件的所有候选列表(包括候选者文件名和索引值)以及当前的候选
	//! \param xmlElement XmlElement
	virtual void writeToXml (ATOM_TiXmlElement *xmlElement);

public:
	//! 设置当前部件索引
	//! \param id 部件索引，如果为invalid_Id则会清除当前候选，如果为any_Id不会对当前候选产生影响，但会设置当前索引值
	//! \return true 成功 false 失败，通常是因为传入了无效的索引值
	bool setCurrentCandidate (CandidateId id);

	//! 增加一个候选者
	//! \param desc 候选者描述
	//! \return 增加的部件索引，如果失败返回invalid_Id
	CandidateId addCandidate (const char *desc);

	//! 增加一个候选者，手工设置索引
	//! \param id 候选者索引
	//! \param desc 候选者描述
	//! \return 增加的部件索引，如果成功返回的值和参数id相同，如果失败返回invalid_Id
	CandidateId addCandidate (CandidateId id, const char *desc);

	//! 删除一个候选者
	//! \param id 候选者索引
	//! \return true 成功 false 不存在对应id的候选者
	bool removeCandidate (CandidateId id);

	//! 删除所有候选者
	void clearCandidates (void);

	//! 获取候选者数量
	//! \return 候选者数量
	unsigned getNumCandidates (void) const;

	//! 获取某个候选者的索引
	//! \param index 候选者数组索引，参见getNumCandidates
	//! \return 候选者索引，失败返回invalid_Id
	CandidateId getCandidateId (unsigned index) const;

	//! 设置某个候选者的索引
	//! \param index 候选者数组索引，参见getNumCandidates
	//! \param id 新的索引
	//! \return true 成功 false index越界或新的id已经在使用中
	bool setCandidateId (unsigned index, CandidateId id);

	//! 获取某个候选者的数组索引
	//! \param id 该候选者的Id
	//! \return 该候选者的数组索引
	int getCandidateIndexById (CandidateId id) const;

	//! 获取当前候选者索引
	//! \return 当前候选者索引，如果当前候选者未设置则返回invalid_Id
	CandidateId getCurrentCandidate (void) const;

	//! 获取某个候选者的描述
	//! \param id 该候选者的索引
	//! \return 该候选者的文件名
	const char *getCandidateDesc (CandidateId id) const;

	//! 设置某个候选者的描述
	//! \param id 候选者索引
	//! \param desc 新的描述串
	void setCandidateDesc (CandidateId id, const char *desc);

	//! 设置名称
	//! \param name 要设置的名称
	bool setName (const char *name);

	//! 获取名称
	//! \return 部件类型名称
	const char *getName (void) const;

	//! 设置所属角色
	//! \param actor 角色实例
	void setActor (ATOM_Actor *actor);

	//! 获取所属角色
	//! \return 所属角色实例
	ATOM_Actor *getActor (void) const;

	//! 设置自定义数据
	//! \param userdata 自定义数据
	void setUserData (void *userData);

	//! 获取自定义数据
	//! \return 自定义数据
	void *getUserData (void) const;

protected:
	//! 当候选者改变时调用
	//! 派生类应重载此方法以执行具体的载入资源工作
	//! \param oldId 旧的候选者索引
	//! \param newId 新的候选者索引
	//! \return true 成功 false 失败
	virtual bool onCandidateChange (CandidateId oldId, CandidateId newId) = 0;

private:
	CandidateId generateId (void) const;

private:
	ATOM_VECTOR<CandidateInfo> _candidates;
	ATOM_STRING _name;
	ATOM_Actor *_actor;
	CandidateId _currentCandidate;
	void *_userData;
};

//! \class ATOM_ActorModifierPart
//! 修改器部件类型.
//! 此种部件是用来修饰其他实体部件的，如颜色，贴图，材质，几何变换等等
//! \author 高雅昆
//! \ingroup engine
//! 
class ATOM_ENGINE_API ATOM_ActorModifierPart: public ATOM_ActorPart
{
public:
	//! \copydoc ATOM_ActorPart::loadFromXml
	virtual bool loadFromXml (ATOM_TiXmlElement *xmlElement);

	//! \copydoc ATOM_ActorPart::writeToXml
	virtual void writeToXml (ATOM_TiXmlElement *xmlElement);

	//! \copydoc ATOM_ActorPart::sync
	virtual void sync (void);

public:
	//! 增加所影响的部件
	//! \param partName 所影响的部件名
	//! \return true 成功 false 如果partName为空或者已存在相同名称的部件则失败
	bool addPart (const char *partName);

	//! 从影响部件列表中删除
	//! \param partName 要删除的部件名
	void removePart (const char *partName);

	//! 清空影响部件列表
	void clearAllParts (void);

	//! 获取影响部件列表项项数
	//! \return 项数
	unsigned getNumParts (void) const;

	//! 获取某个影响部件名称
	//! \param index 影响部件列表项索引
	//! \return 部件名称
	const char *getPart (unsigned index) const;

	//! 设置网格属性.
	//! 只有该属性被设置的网格才会受到此修改器影响
	//! \param property 属性名称
	void setMeshProperty (const char *property);

	//! 获取网格属性
	const char *getMeshProperty (void) const;

private:
	int getPartIndex (const char *name) const;

protected:
	ATOM_VECTOR<ATOM_STRING> _affectParts;
	ATOM_VECTOR<ATOM_ActorPart*> _parts;
	ATOM_STRING _meshProperty;
};

//! \class ATOM_ActorComponentsPart
//! Mesh及动作部件类型.
//! \author 高雅昆
//! \ingroup engine
//! 
class ATOM_ENGINE_API ATOM_ActorComponentsPart: public ATOM_ActorPart
{
	ATOM_CLASS(engine, ATOM_ActorComponentsPart, ATOM_ActorComponentsPart)

public:
	//! 析构函数
	virtual ~ATOM_ActorComponentsPart (void);

	//! \copydoc ATOM_ActorPart::apply
	virtual void apply (void);

	//! \copydoc ATOM_ActorPart::sync
	virtual void sync (void);

	//! \copydoc ATOM_ActorPart::setColor
	virtual void setColor (const ATOM_Vector4f &color, const char *property);

	//! \copydoc ATOM_ActorPart::setTextures
	virtual void setTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap, const char *property);

public:
	//! 获取当前的部件实例
	//! \return 当前部件实例
	ATOM_Components getCurrentComponents (void) const;

	//! 设置当前部件实例
	//! 此方法直接设置部件实例，而不是通过候选索引
	//! \param part 部件实例，如果不为NULL则会将当前部件索引设置为any_Id，否则设置为invalid_Id
	void setCurrentComponents (ATOM_Components part);

protected:
	//! \copydoc ATOM_ActorPart::onCandidateChange
	virtual bool onCandidateChange (CandidateId oldId, CandidateId newId);

private:
	ATOM_Components _currentComponents;
};

//! \class ATOM_ActorColorPart
//! 颜色部件类型.
//! 用于可换色的部件
//! \author 高雅昆
//! \ingroup engine
//! 
class ATOM_ENGINE_API ATOM_ActorColorPart: public ATOM_ActorModifierPart
{
	ATOM_CLASS(engine, ATOM_ActorColorPart, ATOM_ActorColorPart)

public:
	//! 构造函数
	ATOM_ActorColorPart (void);

	//! 析构函数
	virtual ~ATOM_ActorColorPart (void);

public:
	// \copydoc ATOM_ActorPart::apply
	virtual void apply (void);

	// \copydoc ATOM_ActorPart::setColor
	virtual void setColor (const ATOM_Vector4f &color, const char *property);

	// \copydoc ATOM_ActorPart::setTextures
	virtual void setTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap, const char *property);

public:
	//! 获取某个候选颜色
	//! \param id 候选者索引
	//! \return 颜色
	ATOM_Vector4f getColorById (ATOM_ActorPart::CandidateId id) const;

	//! 获取当前颜色值
	//! \return 当前颜色值
	const ATOM_Vector4f & getCurrentColor (void) const;

	//! 设置当前颜色值
	//! 此方法直接设置颜色值，而不是通过候选索引，当前索引将会被设置为any_Id
	//! \param color 颜色值
	void setCurrentColor (const ATOM_Vector4f &color);

protected:
	//! \copydoc ATOM_ActorPart::onCandidateChange
	virtual bool onCandidateChange (CandidateId oldId, CandidateId newId);

private:
	ATOM_Vector4f _currentColor;
};

//! \class ATOM_ActorTexturePart
//! 贴图部件类型.
//! 用于可换贴图的部件
//! \author 高雅昆
//! \ingroup engine
//! 
class ATOM_ENGINE_API ATOM_ActorTexturePart: public ATOM_ActorModifierPart
{
	ATOM_CLASS(engine, ATOM_ActorTexturePart, ATOM_ActorTexturePart)

public:
	//! 构造函数
	ATOM_ActorTexturePart (void);

	//! 析构函数
	virtual ~ATOM_ActorTexturePart (void);

public:
	//! \copydoc ATOM_ActorPart::apply
	virtual void apply (void);

	//! \copydoc ATOM_ActorPart::setColor
	virtual void setColor (const ATOM_Vector4f &color, const char *property);

	//! \copydoc ATOM_ActorPart::setTextures
	virtual void setTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap, const char *property);

public:
	//! 获取某个候选者的颜色贴图文件名
	//! \param id 候选者索引
	//! \return 颜色贴图文件名
	ATOM_STRING getAlbedoFileNameById (ATOM_ActorPart::CandidateId id) const;

	//! 获取某个候选者的颜色法线贴图文件名
	//! \param id 候选者索引
	//! \return 法线贴图文件名
	ATOM_STRING getNormalMapFileNameById (ATOM_ActorPart::CandidateId id) const;

	//! 获取当前颜色贴图
	//! \return 颜色贴图指针
	ATOM_Texture *getCurrentAlbedo (void) const;

	//! 获取当前法线贴图
	//! \return 当前法线贴图指针
	ATOM_Texture *getCurrentNormalMap (void) const;

	//! 设置当前贴图
	//! 此方法直接设置当前贴图，而不是使用候选者索引，当前候选者索引将会被设置为any_Id
	//! \param albedo 颜色贴图
	//! \param normalmap 法线贴图
	void setCurrentTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap);

protected:
	//! \copydoc ATOM_ActorPart::onCandidateChange
	virtual bool onCandidateChange (CandidateId oldId, CandidateId newId);

private:
	ATOM_AUTOREF(ATOM_Texture) _currentAlbedo;
	ATOM_AUTOREF(ATOM_Texture) _currentNormalMap;
};

//! \class ATOM_ActorTransformPart
//! 坐标变换部件类型
//! \author 高雅昆
//! \ingroup engine
class ATOM_ENGINE_API ATOM_ActorTransformPart: public ATOM_ActorPart
{
	ATOM_CLASS(engine, ATOM_ActorTransformPart, ATOM_ActorTransformPart)

public:
	//! 构造函数
	ATOM_ActorTransformPart (void);

	//! 析构函数
	virtual ~ATOM_ActorTransformPart (void);

public:
	//! \copydoc ATOM_ActorPart::apply
	virtual void apply (void);

	//! \copydoc ATOM_ActorPart::sync
	virtual void sync (void);

	//! \copydoc ATOM_ActorPart::setColor
	virtual void setColor (const ATOM_Vector4f &color, const char *property);

	//! \copydoc ATOM_ActorPart::setTextures
	virtual void setTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap, const char *property);

public:
	//! 设置变换矩阵
	//! 此方法直接设置变换矩阵，而不是通过候选索引，当前候选者索引将会被设置为any_Id
	//! \param matrix 变换矩阵
	void setCurrentTransform (const ATOM_Matrix4x4f &matrix);

	//! 获取当前变换矩阵
	//! \return 当前变换矩阵
	const ATOM_Matrix4x4f &getCurrentTransform (void) const;

protected:
	//! \copydoc ATOM_ActorPart::onCandidateChange
	virtual bool onCandidateChange (CandidateId oldId, CandidateId newId);

private:
	bool parseDesc (const char *desc, ATOM_Matrix4x4f &matrix) const;

private:
	ATOM_Matrix4x4f _matrix;
};

//! \class ATOM_ActorBindingPart
//! 骨骼绑定部件类型.
//! \author 高雅昆
//! \ingroup engine
//! 
class ATOM_ENGINE_API ATOM_ActorBindingPart: public ATOM_ActorPart
{
	ATOM_CLASS(engine, ATOM_ActorBindingPart, ATOM_ActorBindingPart)

public:
	struct BindingInfo
	{
		ATOM_STRING nodeFileName;
		ATOM_STRING boneName;
		ATOM_Matrix4x4f bindMatrix;
	};

public:
	//! 构造函数
	ATOM_ActorBindingPart (void);

	//! 析构函数
	virtual ~ATOM_ActorBindingPart (void);

public:
	//! \copydoc ATOM_ActorPart::apply
	virtual void apply (void);

	//! \copydoc ATOM_ActorPart::sync
	virtual void sync (void);

	//! \copydoc ATOM_ActorPart::setColor
	virtual void setColor (const ATOM_Vector4f &color, const char *property);

	//! \copydoc ATOM_ActorPart::setTextures
	virtual void setTextures (ATOM_Texture *albedo, ATOM_Texture *normalmap, const char *property);

public:
	//! 设置绑定骨骼名
	//! \param boneName 绑定骨骼名
	void setBoneName (const char *boneName);

	//! 获取绑定骨骼名
	//! \return 绑定骨骼名
	const char *getBoneName (void) const;

	//! 设置绑定节点.
	//! 此方法直接设置绑定节点，而不是通过候选索引
	//! \param node 节点指针，如果为NULL当前候选索引将会被设置成invalid_Id，否则设置为any_Id
	void setCurrentNode (ATOM_Node *node);

	//! 获取当前绑定节点
	//! \return 当前绑定节点
	ATOM_Node *getCurrentNode (void) const;

	//! 设置绑定变换
	//! 此方法直接设置绑定节点变换，而不是通过候选索引
	//! \param matrix 变换矩阵
	void setCurrentBindMatrix (const ATOM_Matrix4x4f &matrix);

	//! 获取当前绑定变换
	//! \return 当前绑定变换
	const ATOM_Matrix4x4f &getCurrentBindMatrix (void) const;

	//! 获取指定Id的候选者的绑定信息
	//! \param id 候选者Id
	//! \param info 如果成功，这个绑定信息结构会被填充
	//! \return true 成功 false 失败
	bool getBindingInfoById (CandidateId id, BindingInfo &info) const;

protected:
	//! \copydoc ATOM_ActorPart::onCandidateChange
	virtual bool onCandidateChange (CandidateId oldId, CandidateId newId);

private:
	bool parseDesc (const char *desc, ATOM_STRING &filename, ATOM_STRING &boneName, ATOM_Matrix4x4f &matrix) const;

private:
	ATOM_STRING _boneName;
	ATOM_Matrix4x4f _bindMatrix;
	ATOM_AUTOREF(ATOM_Node) _currentBinding;
};

//! \class ATOM_ActorPartDefine
//! 角色的部件系统定义
//! 该类管理了一系列的部件集合
//! \author 高雅昆
//! \ingroup engine
class ATOM_ENGINE_API ATOM_ActorPartDefine
{
public:
	//! 构造函数
	ATOM_ActorPartDefine (ATOM_Actor *actor);

	//! 析构函数
	~ATOM_ActorPartDefine (void);

public:
	//! 通过xml读取
	//! \param xmlElement XmlElement
	//! \return true 成功 false 失败
	bool loadFromXml (ATOM_TiXmlElement *xmlElement);

	//! 写入到xml
	//! \param xmlElement XmlElement
	void writeToXml (ATOM_TiXmlElement *xmlElement);

public:
	//! 创建新的mesh部件类型
	//! \param name 部件类型名称
	//! \return 成功返回部件指针，如果该名称已被使用返回NULL
	ATOM_ActorComponentsPart *newComponentsPart (const char *name);

#if 0
	//! 创建新的颜色部件类型
	//! \param name 部件类型名称
	//! \return 成功返回部件指针，如果该名称已被使用返回NULL
	ATOM_ActorColorPart *newColorPart (const char *name);

	//! 创建新的贴图部件类型
	//! \param name 部件类型名称
	//! \return 成功返回部件指针，如果该名称已被使用返回NULL
	ATOM_ActorTexturePart *newTexturePart (const char *name);

	//! 创建新的骨骼绑定部件类型
	//! \param name 部件类型名称
	//! \return 成功返回部件指针，如果该名称已被使用返回NULL
	ATOM_ActorBindingPart *newBindingPart (const char *name);
#endif

	//! 添加一个部件类型
	//! \param part 要添加的部件类型
	//! \return true 成功 false 如果part为空或已经存在相同名称的part则失败
	bool addPart (ATOM_ActorPart *part);

	//! 删除一个部件类型
	//! \param name 要删除部件类型名称
	void removePartByName (const char *name);

	//! 删除一个部件类型
	//! \param part 要删除部件类型指针
	void removePart (ATOM_ActorPart *part);

	//! 删除所有部件类型
	void removeAllParts (void);

	//! 通过名称查找一个部件类型指针
	//! \param name 要查找的部件类型名称
	ATOM_ActorPart *getPartByName (const char *name) const;

	//! 获取组所有部件类型的数量
	//! \return 数量
	unsigned getNumParts (void) const;

	//! 获取某个部件类型的名称
	//! \param index 部件类型列表索引，参见getNumParts
	//! \return 部件类型名称，如果index无效返回NULL
	const char *getPartName (unsigned index) const;

	//! 获取某个部件类型的指针
	//! \param index 部件列表索引，参见getNumParts
	//! \return 部件类型指针，如果index无效返回NULL
	ATOM_ActorPart *getPart (unsigned index) const;

private:
	ATOM_Actor *_actor;
	ATOM_VECTOR<ATOM_AUTOREF(ATOM_ActorPart)> _parts;
};

//! \class ATOM_Actor
//! 角色节点基类.
//! 管理游戏中的角色，支持更换部件，武器绑定和动作.下面代码使用ATOM_Actor类进行换肤
//! \code
//!		Actor->setPart ("head", 34); // 使用34号头部模型
//!		Actor->setPart ("bodycolor", 12);	// 使用12号身体颜色
//!	\endcode
//! \author 高雅昆
//! \ingroup engine
//! 
class ATOM_ENGINE_API ATOM_Actor: public ATOM_VisualNode
{
public:
	//! 构造函数
	ATOM_Actor (void);

	//! 析构函数
	virtual ~ATOM_Actor (void);

public:
	//! \copydoc ATOM_Node::accept
	virtual void accept (ATOM_Visitor &visitor);

	//! \copydoc ATOM_VisualNode::rayIntersectionTest
	virtual bool rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const;

	//! \copydoc ATOM_Node::supportMTLoading
	virtual bool supportMTLoading (void);

	//! \copydoc ATOM_Node::getBoundingbox
	virtual const ATOM_BBox& getBoundingbox(void) const;

	//! \copydoc ATOM_Node::getWorldBoundingbox
	virtual const ATOM_BBox& getWorldBoundingbox (void) const;

protected:
	//! \copydoc ATOM_Node::buildBoundingbox
	virtual void buildBoundingbox (void) const;

	//! \copydoc ATOM_Node::onLoad
	virtual bool onLoad(ATOM_RenderDevice *device);

	//! \copydoc ATOM_Node::assign
	virtual void assign(ATOM_Node *other) const;

public:
	//! \copydoc ATOM_VisualNode::setupRenderQueue
	virtual void setupRenderQueue (ATOM_CullVisitor *visitor);

	//! \copydoc ATOM_VisualNode::skipClipTest
	virtual void skipClipTest (bool b);

	//! \copydoc ATOM_VisualNode::isSkipClipTest
	virtual bool isSkipClipTest (void) const;

public:
	//! 获取部件系统管理器实例
	//! \return 部件系统管理器实例
	ATOM_ActorPartDefine & getPartDefine (void) const;

	//! 设置某个部件类型的当前候选者索引
	//! \param partName 部件类型名称
	//! \param id 候选者索引
	//! \return 返回之前的当前候选者索引
	ATOM_ActorPart::CandidateId setPart (const char *partName, ATOM_ActorPart::CandidateId id);

	//! 获取某个部件类型的当前候选者索引
	//! \param partName 部件类型名称
	//! \return 候选者索引
	ATOM_ActorPart::CandidateId getPart (const char *partName) const;

	//! 播放动作
	//! 要想正确播放动作，必须添加动作部件
	//! \param actionName 动作名
	//! \param flags 参数
	//! \param loopCount 循环次数，为0代表始终循环
	//! \param forceReset 如果为true的话，设置相同的动作将会强制从头播放
	//! \param fadeTime 动作融合进来的毫秒数
	//! \param speed 播放此动作的速率，如果为0则不更改当前速率，使用默认的速度值
	//! \return 动作索引
	bool doAction (const char *actionName, unsigned flags, unsigned loopCount = 0, bool forceReset = false, unsigned fadeTime = 300, float speed = 0.f);

	//! 设定动作的默认播放速率.
	//! 此方法用来设定后面的doAction调用时所采用的默认速率(doAction方法的speed参数为0.0)
	//! \param flags 参数
	//! \param speed 速率, 1.0为正常速率
	void setDefaultActionSpeed (unsigned flags, float speed);

	//! 设定当前动作的播放速率.
	//! 此方法用来设定当前正在播放的动作速率，不会影响默认播放速率
	//! \param flags 参数
	//! \param speed 速率, 1.0为正常速率
	void setCurrentActionSpeed (unsigned flags, float speed);

	//! 当前动作播放进度归零
	void resetCurrentAction (unsigned flags);

	//! 获取内部Geode节点
	//! \return 节点指针
	ATOM_Geode *getInternalGeode (void) const;

	//! 同步部件状态.
	//! 此函数内部使用
	void syncParts (void);

	//! 通知角色实例部件需要被同步
	void notifyPartChange (void);

	//! 保存到文件.
	//! \param filename 文件名
	//! \return true 成功 false 失败
	bool saveToFile (const char *filename) const;

	//! 保存到XML文档
	//! \param doc XML文档
	void saveToXML (ATOM_TiXmlDocument &doc) const;

public:
	//! 设置属性值
	//! \param name 属性名称
	//! \param value 属性值
	void setProperty (const char *name, const ATOM_Variant &value);

	//! 查询属性值
	//! \param name 属性名称
	//! \return 属性值，如果未找到指定名称的属性，则返回NONE
	const ATOM_Variant &getProperty (const char *name);

	//! 获取属性数量.
	//! \return 属性的个数
	unsigned getNumProperties (void) const;

	//! 获取属性名称
	//! \param index 属性的索引
	//! \return 属性的名称，如果索引无效返回NULL，返回的字符串指针并不能保证长期有效，当属性表发生变化(例如插入或删除操作)，该字符串可能被释放
	const char *getPropertyName (unsigned index) const;

	//! 获取属性值
	//! \param index 属性索引
	//! \return 属性值，如果索引无效返回NONE
	const ATOM_Variant &getPropertyValue (unsigned index) const;

	//! 删除所有属性
	void removeAllProperties (void);

public:
	void setAction (const ATOM_STRING &actionName);
	const ATOM_STRING &getAction (void) const;

private:
	bool loadProperties (ATOM_TiXmlElement *element);
	void writeProperties (ATOM_TiXmlElement *element) const;

private:
	ATOM_STRING _action;
	bool _partChange;
	ATOM_AUTOREF(ATOM_Geode) _geode;
	ATOM_ActorPartDefine *_partDefine;
	ATOM_HASHMAP<ATOM_STRING, ATOM_Variant> _properties;

	ATOM_CLASS(engine, ATOM_Actor, ATOM_Actor)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_Actor)
	ATOM_DECLARE_NODE_FILE_TYPE(ATOM_Actor, ATOM_Actor, "act", "ATOM3D actor")

	//=========================================================================
	// wangjian added
public:
	virtual void onLoadFinished();
private:
	// 直接加载接口
	bool loadSync();
	//=========================================================================
};

#endif // __ATOM3D_ENGINE_ACTOR_H
/*! @} */
