/**	\file particlesystem.h
 *	粒子系统类.
 *
 *	\author 高雅昆
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_PARTICLESYSTEM_H
#define __ATOM3D_ENGINE_PARTICLESYSTEM_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_utils.h"
#include "../ATOM_math.h"
#include "../ATOM_render.h"

#include "node.h"

class ATOM_ENGINE_API ATOM_MultiStreamGeometry;

//! \struct ATOM_Particle
//! 单个粒子结构.
//! \ingroup engine
struct ATOM_Particle
{
	ATOM_Vector3f position;	//!< 粒子的当前位置
	ATOM_Vector3f velocity;	//!< 粒子的当前速度和方向
	float size1;				//!< 粒子的初始大小
	float size2;				//!< 粒子的最终大小
	float rotation;				//!< 粒子的旋转速度
	unsigned lifeSpan;			//!< 粒子的最大存活时间，以毫秒计
	float acceleration;			//!< 粒子的加速值
};

//! \class ATOM_ParticleEmitter
//! 粒子发射器接口类.
//! 粒子系统支持自定义发射器，自定义发射器需要从此接口派生并实现该接口
//! \ingroup engine
class ATOM_ParticleEmitter: public ATOM_Object
{
public:
	//! 初始化新发射粒子的参数.
	//! \param ps 发射器所属的粒子系统
	//! \param particle 新发射的粒子，需要填充其中的参数
	//! \param emitTime 发射新粒子时距离发射第一个粒子时所经过的时间，以毫秒计
	//! \note 粒子的位置和方向都是相对于粒子系统局部坐标系
	virtual void initParticle (ATOM_ParticleSystem *ps, ATOM_Particle *particle, unsigned emitTime) = 0;

	//! 重设发射器
	virtual void reset() {}
};

class ATOM_Camera;

//! \class ATOM_ParticleSystem
//! 粒子系统类.
//! \author 高雅昆
//! \todo buildBoundingbox函数返回不精确结果, 粒子系统不可见时可以降低更新频率, 硬件不支持PointSprite的情况需要实现, 优化更新效率(SIMD)
//! \ingroup engine
class ATOM_ENGINE_API ATOM_ParticleSystem: public ATOM_Node, public ATOM_Drawable
{
public:
	//! 标志位
	enum
	{
		PS_DIRECTIONAL_PARTICLE = (1<<4),	//!< 粒子跟随运行方向
		PS_BLEND_NORMAL = (1<<5),			//!< 正常融合
		PS_BLEND_ADD = (1<<6),				//!< Additive融合
		PS_BLEND_BLENDADD = (1<<7),			//!< Alpha Additive融合
		PS_FOG = (1<<8),					//!< 支持雾
		PS_WORLDSPACE = (1<<9)				//!< 粒子系统运行于世界空间
	};

	enum EmitterShape
	{
		ES_POINT = 0,	//!< 点状发射器
		ES_SPHERE,		//!< 球形发射器
		ES_BOX,			//!< 立方体发射器
		ES_CYLINDER,	//!< 柱形发射器
		ES_CONE			//!< 锥形发射器
	};

	enum ParticleForm
	{
		PF_SPRITE = 0,	//!< 公告板形态(默认)
		PF_LINE			//!< 线形
	};

	enum EmitterBehavior
	{
		EB_SURFACE = 0,	//!< 表面发射
		EB_VOLUME		//!< 内部发射
	};

	//--- wangjian added ---//
	enum DirectionType
	{
		DT_NONE = 0,		// 无方向性(总是面对摄像机）

		DT_VELOCITY,		// 速度方向
		DT_VERTICAL,		// 竖直方向
		DT_HORIZON,			// 水平方向

		DT_TOTAL
	};
	//----------------------//

public:
	//! 构造函数.
	ATOM_ParticleSystem (void);

	//! 析构函数
	virtual ~ATOM_ParticleSystem (void);

public:
	//! \copydoc ATOM_Node::accept
	virtual void accept (ATOM_Visitor &visitor);

	//! \copydoc ATOM_Node::supportFixedFunction
	virtual bool supportFixedFunction (void) const;

	

public:
	//! \copydoc ATOM_drawable::draw
	virtual bool draw(ATOM_RenderDevice* device, ATOM_Camera *camera, ATOM_Material *material);

	//! \copydoc ATOM_Drawable::computeSquaredDistanceToCamera
	virtual float computeSquaredDistanceToCamera (ATOM_Camera *camera, const ATOM_Matrix4x4f &worldMatrix) const;

	//--- wangjian added ---//
	virtual void prepareForSort(ATOM_RenderScheme* renderScheme);
	// 设置绘制排序标记
	virtual void setupSortFlag(int queueid);

	virtual bool checkNodeAllFinshed();
	virtual void onLoadFinished();
	
protected:
	virtual void resetMaterialDirtyFlag_impl();
	//----------------------//

protected:
	//! \copydoc ATOM_Node::onLoad
	virtual bool onLoad(ATOM_RenderDevice *device);

	//! \copydoc ATOM_Node::buildBoundingbox
	virtual void buildBoundingbox (void) const;

	//! \copydoc ATOM_Node::buildWorldBoundingbox
	virtual void buildWorldBoundingbox (void) const;

public:
	//! 设置标志位
	//! \param flags 标志位.
	void setFlags (int flags);

	//! 获取标志位
	//! \return 标志位.
	int getFlags (void) const;

	//! 设置最大粒子数目.
	//! \param count 最大粒子数目,默认为100
	//! \sa getMaxParticleCount
	void setMaxParticleCount (unsigned count);

	//! 获取最大粒子数目.
	//! \return 最大粒子数目,默认为100
	//! \sa setMaxParticleCount
	unsigned getMaxParticleCount (void) const;

	//! 设置粒子系统的更新周期.
	//! 至少要经过这么长时间粒子系统才会更新粒子状态，增加新粒子和删除死亡粒子，此参数目的在于减少CPU的使用
	//! \param interval 时间长度，以毫秒计，默认为20，即每秒最多更新50次
	//! \sa getUpdateInterval
	void setUpdateInterval (unsigned interval);

	//! 获取粒子系统的更新周期.
	//! \return interval 时间长度，以毫秒计
	//! \sa setUpdateInterval
	unsigned getUpdateInterval (void) const;

	//! 设置粒子的发射间隔时间.
	//! 每隔这么长时间就会发射一次粒子
	//! \param interval 时间长度，以毫秒计，默认为100
	//! \sa getEmitInterval
	void setEmitInterval (unsigned interval);

	//! 获取粒子的发射间隔时间.
	//! \return 时间长度，以毫秒计
	//! \sa setEmitInterval
	unsigned getEmitInterval (void) const;

	//! 设置一次发射粒子的数量
	//! \param count 粒子发射数量，默认为1
	//! \sa getEmitCount
	void setEmitCount (unsigned count);

	//! 获取一次发射粒子的数量
	//! \return 粒子发射数量
	//! \sa setEmitCount
	unsigned getEmitCount (void) const;

	//! 设置重力的方向和强度.
	//! \param gravity 重力的方向，该向量的长度表示每秒钟粒子被该重力影响的移动值，默认为(0,0,0)
	//! \sa getGravity
	void setGravity (const ATOM_Vector3f &gravity);

	//! 获取重力的方向和强度.
	//! \return 重力方向和强度，默认为(0,0,0)
	//! \sa setGravity
	const ATOM_Vector3f &getGravity (void) const;

	//! 设置风力的方向和强度.
	//! \param wind 风力的方向，该向量的长度表示每秒钟粒子被该风力影响的移动值，如果该向量为0表示不受风力作用，默认为(0,0,0)
	//! \sa getWind
	void setWind (const ATOM_Vector3f &wind);

	//! 获取风力的方向和强度.
	//! \return 风力方向和强度，默认为(0,0,0)
	//! \sa setWind
	const ATOM_Vector3f &getWind (void) const;

	//! 设置粒子的贴图文件名.
	//! \param filename 贴图的ATOM_VFS文件名
	//! \sa getTextureFileName
	void setTextureFileName (const ATOM_STRING &filename);

	//! 获取粒子的贴图文件名.
	//! \return 贴图的ATOM_VFS文件名
	//! \sa setTextureFileName
	const ATOM_STRING &getTextureFileName (void) const;

	//! 设置粒子的颜色贴图文件名.
	//! \param filename 贴图的ATOM_VFS文件名
	//! \sa getTextureFileName
	void setColorFileName (const ATOM_STRING &filename);

	//! 获取粒子的颜色贴图文件名.
	//! \return 贴图的ATOM_VFS文件名
	//! \sa setTextureFileName
	const ATOM_STRING &getColorFileName (void) const;

	//! 设置粒子的旋转速度
	//! \param val 粒子旋转速度(弧度/秒)
	//! \sa getRotationVelocity setRotationVelocityVar getRotationVelocityVar
	void setRotationVelocity (float val);

	//! 获取粒子的旋转速度
	//! \return 粒子的旋转速度(弧度/秒)
	//! \sa setRotationVelocity setRotationVelocityVar getRotationVelocityVar
	float getRotationVelocity (void) const;

	//! 设置粒子的旋转速度随机范围
	//! \param val 粒子旋转速度随机范围(弧度/秒)
	//! \sa getRotationVelocityVar setRotationVelocity getRotationVelocity
	void setRotationVelocityVar (float val);

	//! 获取粒子的旋转速度随机范围
	//! \return 粒子的旋转速度随机范围(弧度/秒)
	//! \sa setRotationVelocityVar setRotationVelocity getRotationVelocity
	float getRotationVelocityVar (void) const;

	//! 将粒子系统重置为初始状态.
	//! \param clearParticles 是否删除所有活动粒子
	void reset (bool clearParticles);

	//! 更新所有活动粒子状态.
	//! 为了不产生视觉错误，在不可见时我们仍然更新粒子状态。
	//! \param viewMatrix 当前摄像机矩阵
	//! \param projectionMatrix 当前投影矩阵
	void update (const ATOM_Matrix4x4f &viewMatrix, const ATOM_Matrix4x4f &projectionMatrix);

	//! 更新顶端缓冲.
	//! 只有粒子系统可见的情况下才会更新顶点缓冲
	void updateVertexArrays (ATOM_Camera *camera);

	//! 获取所有活动粒子的数量.
	//! \return 所有活动粒子的数量
	unsigned getActiveParticleCount (void) const;

	//! 获取粒子系统材质.
	//! \return 粒子系统的渲染材质
	//! \note 此材质由内部创建，不要在外面修改
	ATOM_Material *getMaterial (void);

	//! 设置粒子系统的缩放系数.
	//! \param scalar 粒子系统的缩放系数
	//! \note 对于拖尾效果的粒子系统不可以使用基于局部坐标系变换的setO2T函数，所以需要此接口，因为使用PointSprite，只能采用等比缩放
	//! \sa getScalar
	void setScalar (float scalar);

	//! 获取粒子系统的缩放系数.
	//! \return 粒子系统的缩放系数
	//! \sa setScalar
	float getScalar (void) const;

	//! 设置粒子的宽高比.
	//! \param aspect 粒子的宽高比
	void setAspect (float aspect);

	//! 获取粒子的宽高比
	//! \return 粒子的宽高比
	float getAspect (void) const;

	//! 设置粒子抖动速度
	//! \param speed 粒子抖动速度
	void setJitterSpeed (float speed);

	//! 获取粒子抖动速度
	//! \return 粒子抖动速度
	float getJitterSpeed (void) const;

	//! 设置粒子抖动强度
	//! \param power 粒子抖动强度
	void setJitterPower (float power);

	//! 获取粒子抖动强度
	//! \return 粒子抖动强度
	float getJitterPower (void) const;

	//! 暂停运行
	void pause (void);

	//! 恢复运行
	void resume (void);

	//! 是否暂停
	bool paused (void) const;

	//! 获取起始时间
	unsigned getStartTick (void) const;

	//! 设置发射器形状
	void setEmitterShape (EmitterShape shape);

	//! 获取发射器形状
	EmitterShape getEmitterShape (void) const;

	//! 设置粒子形态 
	void setParticleForm (ParticleForm form);

	//! 获取粒子形态
	ParticleForm getParticleForm (void) const;

	//! 设置发射方式
	void setEmitterBehavior (EmitterBehavior behavior);

	//! 获取发射方式
	EmitterBehavior getEmitterBehavior (void) const;

	//! 设置发射半径
	void setEmitterConeRadius (float value);

	//! 获取发射半径
	float getEmitterConeRadius (void) const;

	//! 设置发射半径随机范围
	void setEmitterConeRadiusVar (float value);

	//! 设置发射半径随机范围
	float getEmitterConeRadiusVar (void) const;

	//! 设置发射器形状的大小
	void setEmitterShapeSize (const ATOM_Vector3f &size);

	//! 获取发射器形状大小
	const ATOM_Vector3f &getEmitterShapeSize (void) const;

	//! 设置发射器形状的大小随机范围
	void setEmitterShapeSizeVar (const ATOM_Vector3f &size);

	//! 获取发射器形状的大小随机范围
	const ATOM_Vector3f &getEmitterShapeSizeVar (void) const;

	//! 设置发射速率
	void setParticleVelocity (float value);

	//! 获取发射速率
	float getParticleVelocity (void) const;

	//! 设置发射速率随机范围
	void setParticleVelocityVar (float value);

	//! 获取发射速率随机范围
	float getParticleVelocityVar (void) const;

	//! 设置粒子初始生命值
	void setParticleLife (float value);

	//! 获取粒子初始生命值
	float getParticleLife (void) const;

	//! 设置粒子初始生命值随机范围
	void setParticleLifeVar (float value);

	//! 获取粒子初始生命值随机范围
	float getParticleLifeVar (void) const;

	//! 设置粒子初始大小
	void setParticleSize1 (float value);

	//! 获取粒子初始大小
	float getParticleSize1 (void) const;

	//! 设置粒子初始大小随机值
	void setParticleSize1Var (float value);

	//! 获取粒子初始大小随机值
	float getParticleSize1Var (void) const;

	//! 设置粒子结束大小
	void setParticleSize2 (float value);

	//! 获取粒子结束大小
	float getParticleSize2 (void) const;

	//! 设置粒子结束大小随机值
	void setParticleSize2Var (float value);

	//! 获取粒子结束大小随机值
	float getParticleSize2Var (void) const;

	//! 设置粒子加速度
	void setParticleAccel (float value);

	//! 获取粒子加速度
	float getParticleAccel (void) const;

	//! 设置粒子加速度随机值
	void setParticleAccelVar (float value);

	//! 获取粒子加速度随机值
	float getParticleAccelVar (void) const;

	//! 设置全局透明度
	void setTransparency (float alpha);

	//! 获取全局透明度
	float getTransparency (void) const;

	//! 设置延迟时间(毫秒)
	void setDelay (unsigned timeInMs);

	//! 获取延迟时间(毫秒)
	unsigned getDelay (void) const;

	//! 设置全局颜色
	void setColor (const ATOM_Vector4f &color);

	//! 获取全局颜色
	const ATOM_Vector4f &getColor (void) const;

	//---- wangjian added  ----//

	//! 设置全局颜色强度因子
	void setColorMultiplier (const float colorMultiplier);

	//! 获取全局颜色强度因子
	const float getColorMultiplier (void) const;


public:
	void att_setEmitterShape (int shape);
	int att_getEmitterShape (void) const;
	void att_setEmitterBehavior (int behavior);
	int att_getEmitterBehavior (void) const;

	//--- wangjian modified ---//
	void att_setDirectional (int directional);
	int att_getDirectional (void) const;
	//-------------------------//

	void att_setWorldSpace (int worldSpace);
	int att_getWorldSpace (void) const;
	void att_setForm (int form);
	int att_getForm (void) const;
	void att_setBlendMode (int mode);
	int att_getBlendMode (void) const;

private:
	struct ParticleNode
	{
		ATOM_Particle particle;
		unsigned elapsedTime;
		float size;
		float rotation;
		float ageBias;
		float jitterAngle;
		ParticleNode *next;
	};

	ParticleNode *retireParticle (ParticleNode *particle);
	void newParticle (unsigned num, unsigned currentTick, const ATOM_Matrix4x4f &viewMatrix, const ATOM_Matrix4x4f &projectionMatrix, const ATOM_Matrix4x4f &worldMatrix, const ATOM_Matrix4x4f &itworldMatrix);
	void purgeParticles (void);
	void resizeVertexArray (void);
	bool createMaterial (void);
	void initParticle (ATOM_Particle *particle, unsigned emitTime);
	void getParticleInitialPosition (ATOM_Vector3f *pos, ATOM_Vector3f *vel) const;
	
	//--- wangjian added ---//
	// 异步加载：检查异步加载完成标记
	bool checkAsyncLoadFlag();
	// 直接加载接口
	bool loadSync();
	//----------------------//

protected:
	ParticleNode *_activeParticleList;	// 活动粒子链表头指针
	ParticleNode *_freeParticleList;	// 空闲粒子链表头指针

	ATOM_STRING _textureFileName;
	ATOM_STRING _colorFileName;
	ATOM_AUTOREF(ATOM_Texture) _texture;
	ATOM_AUTOREF(ATOM_Texture) _color;
	unsigned _maxParticleCount;
	unsigned _updateInterval;
	unsigned _emitInterval;
	unsigned _emitCount;
	unsigned _activeParticleCount;
	unsigned _starttick;
	int _flags;
	ATOM_Vector3f _gravity;
	ATOM_Vector3f _wind;
	float _scalar;
	float _aspect;
	float _rotation;
	float _rotationVar;
	float _jitterSpeed;
	float _jitterPower;
	EmitterShape _emitterShape;
	ParticleForm	_particleForm;
	EmitterBehavior _emitterBehavior;
	float _emitterConeRadius;
	float _emitterConeRadiusVar;
	float _particleVelocity;
	float _particleVelocityVar;
	float _particleLife;
	float _particleLifeVar;
	float _particleSize1;
	float _particleSize1Var;
	float _particleSize2;
	float _particleSize2Var;
	float _particleAccel;
	float _particleAccelVar;
	ATOM_Vector3f _emitterShapeSize;
	ATOM_Vector3f _emitterShapeSizeVar;
	ATOM_Vector4f _colorValue;

	unsigned	_startEmitTime;
	unsigned	_lastUpdateTime;
	unsigned	_numEmitCount;
	unsigned	_delay;
	bool		_airResistence;
	bool		_needUpdateVertexArray;
	bool		_paused;
	float		_transparency;
	int			_blendMode;
	//--- wangjian added ---//
	float		_colorMultiplier;
	int			_directionType;
	//----------------------//

	ATOM_MultiStreamGeometry *_geometry;
	ATOM_AUTOREF(ATOM_VertexArray) _vertices;
	ATOM_AUTOREF(ATOM_VertexArray) _texcoords;
	ATOM_AUTOREF(ATOM_VertexArray) _velocities;
	ATOM_AUTOREF(ATOM_IndexArray) _indices;

	ATOM_AUTOPTR(ATOM_Material) _material;

	ATOM_BBox _bbox;

	ATOM_CLASS(engine, ATOM_ParticleSystem, ParticleSystem)
    ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_ParticleSystem)
	ATOM_DECLARE_NODE_FILE_TYPE(ATOM_ParticleSystem, ParticleSystem, "par", "ATOM3D particle system")
};

#endif // __ATOM3D_ENGINE_PARTICLESYSTEM_H
/*! @} */
