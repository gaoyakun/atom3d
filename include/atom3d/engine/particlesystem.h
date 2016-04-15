/**	\file particlesystem.h
 *	����ϵͳ��.
 *
 *	\author ������
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
//! �������ӽṹ.
//! \ingroup engine
struct ATOM_Particle
{
	ATOM_Vector3f position;	//!< ���ӵĵ�ǰλ��
	ATOM_Vector3f velocity;	//!< ���ӵĵ�ǰ�ٶȺͷ���
	float size1;				//!< ���ӵĳ�ʼ��С
	float size2;				//!< ���ӵ����մ�С
	float rotation;				//!< ���ӵ���ת�ٶ�
	unsigned lifeSpan;			//!< ���ӵ������ʱ�䣬�Ժ����
	float acceleration;			//!< ���ӵļ���ֵ
};

//! \class ATOM_ParticleEmitter
//! ���ӷ������ӿ���.
//! ����ϵͳ֧���Զ��巢�������Զ��巢������Ҫ�Ӵ˽ӿ�������ʵ�ָýӿ�
//! \ingroup engine
class ATOM_ParticleEmitter: public ATOM_Object
{
public:
	//! ��ʼ���·������ӵĲ���.
	//! \param ps ����������������ϵͳ
	//! \param particle �·�������ӣ���Ҫ������еĲ���
	//! \param emitTime ����������ʱ���뷢���һ������ʱ��������ʱ�䣬�Ժ����
	//! \note ���ӵ�λ�úͷ��������������ϵͳ�ֲ�����ϵ
	virtual void initParticle (ATOM_ParticleSystem *ps, ATOM_Particle *particle, unsigned emitTime) = 0;

	//! ���跢����
	virtual void reset() {}
};

class ATOM_Camera;

//! \class ATOM_ParticleSystem
//! ����ϵͳ��.
//! \author ������
//! \todo buildBoundingbox�������ز���ȷ���, ����ϵͳ���ɼ�ʱ���Խ��͸���Ƶ��, Ӳ����֧��PointSprite�������Ҫʵ��, �Ż�����Ч��(SIMD)
//! \ingroup engine
class ATOM_ENGINE_API ATOM_ParticleSystem: public ATOM_Node, public ATOM_Drawable
{
public:
	//! ��־λ
	enum
	{
		PS_DIRECTIONAL_PARTICLE = (1<<4),	//!< ���Ӹ������з���
		PS_BLEND_NORMAL = (1<<5),			//!< �����ں�
		PS_BLEND_ADD = (1<<6),				//!< Additive�ں�
		PS_BLEND_BLENDADD = (1<<7),			//!< Alpha Additive�ں�
		PS_FOG = (1<<8),					//!< ֧����
		PS_WORLDSPACE = (1<<9)				//!< ����ϵͳ����������ռ�
	};

	enum EmitterShape
	{
		ES_POINT = 0,	//!< ��״������
		ES_SPHERE,		//!< ���η�����
		ES_BOX,			//!< �����巢����
		ES_CYLINDER,	//!< ���η�����
		ES_CONE			//!< ׶�η�����
	};

	enum ParticleForm
	{
		PF_SPRITE = 0,	//!< �������̬(Ĭ��)
		PF_LINE			//!< ����
	};

	enum EmitterBehavior
	{
		EB_SURFACE = 0,	//!< ���淢��
		EB_VOLUME		//!< �ڲ�����
	};

	//--- wangjian added ---//
	enum DirectionType
	{
		DT_NONE = 0,		// �޷�����(��������������

		DT_VELOCITY,		// �ٶȷ���
		DT_VERTICAL,		// ��ֱ����
		DT_HORIZON,			// ˮƽ����

		DT_TOTAL
	};
	//----------------------//

public:
	//! ���캯��.
	ATOM_ParticleSystem (void);

	//! ��������
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
	// ���û���������
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
	//! ���ñ�־λ
	//! \param flags ��־λ.
	void setFlags (int flags);

	//! ��ȡ��־λ
	//! \return ��־λ.
	int getFlags (void) const;

	//! �������������Ŀ.
	//! \param count ���������Ŀ,Ĭ��Ϊ100
	//! \sa getMaxParticleCount
	void setMaxParticleCount (unsigned count);

	//! ��ȡ���������Ŀ.
	//! \return ���������Ŀ,Ĭ��Ϊ100
	//! \sa setMaxParticleCount
	unsigned getMaxParticleCount (void) const;

	//! ��������ϵͳ�ĸ�������.
	//! ����Ҫ������ô��ʱ������ϵͳ�Ż��������״̬�����������Ӻ�ɾ���������ӣ��˲���Ŀ�����ڼ���CPU��ʹ��
	//! \param interval ʱ�䳤�ȣ��Ժ���ƣ�Ĭ��Ϊ20����ÿ��������50��
	//! \sa getUpdateInterval
	void setUpdateInterval (unsigned interval);

	//! ��ȡ����ϵͳ�ĸ�������.
	//! \return interval ʱ�䳤�ȣ��Ժ����
	//! \sa setUpdateInterval
	unsigned getUpdateInterval (void) const;

	//! �������ӵķ�����ʱ��.
	//! ÿ����ô��ʱ��ͻᷢ��һ������
	//! \param interval ʱ�䳤�ȣ��Ժ���ƣ�Ĭ��Ϊ100
	//! \sa getEmitInterval
	void setEmitInterval (unsigned interval);

	//! ��ȡ���ӵķ�����ʱ��.
	//! \return ʱ�䳤�ȣ��Ժ����
	//! \sa setEmitInterval
	unsigned getEmitInterval (void) const;

	//! ����һ�η������ӵ�����
	//! \param count ���ӷ���������Ĭ��Ϊ1
	//! \sa getEmitCount
	void setEmitCount (unsigned count);

	//! ��ȡһ�η������ӵ�����
	//! \return ���ӷ�������
	//! \sa setEmitCount
	unsigned getEmitCount (void) const;

	//! ���������ķ����ǿ��.
	//! \param gravity �����ķ��򣬸������ĳ��ȱ�ʾÿ�������ӱ�������Ӱ����ƶ�ֵ��Ĭ��Ϊ(0,0,0)
	//! \sa getGravity
	void setGravity (const ATOM_Vector3f &gravity);

	//! ��ȡ�����ķ����ǿ��.
	//! \return ���������ǿ�ȣ�Ĭ��Ϊ(0,0,0)
	//! \sa setGravity
	const ATOM_Vector3f &getGravity (void) const;

	//! ���÷����ķ����ǿ��.
	//! \param wind �����ķ��򣬸������ĳ��ȱ�ʾÿ�������ӱ��÷���Ӱ����ƶ�ֵ�����������Ϊ0��ʾ���ܷ������ã�Ĭ��Ϊ(0,0,0)
	//! \sa getWind
	void setWind (const ATOM_Vector3f &wind);

	//! ��ȡ�����ķ����ǿ��.
	//! \return ���������ǿ�ȣ�Ĭ��Ϊ(0,0,0)
	//! \sa setWind
	const ATOM_Vector3f &getWind (void) const;

	//! �������ӵ���ͼ�ļ���.
	//! \param filename ��ͼ��ATOM_VFS�ļ���
	//! \sa getTextureFileName
	void setTextureFileName (const ATOM_STRING &filename);

	//! ��ȡ���ӵ���ͼ�ļ���.
	//! \return ��ͼ��ATOM_VFS�ļ���
	//! \sa setTextureFileName
	const ATOM_STRING &getTextureFileName (void) const;

	//! �������ӵ���ɫ��ͼ�ļ���.
	//! \param filename ��ͼ��ATOM_VFS�ļ���
	//! \sa getTextureFileName
	void setColorFileName (const ATOM_STRING &filename);

	//! ��ȡ���ӵ���ɫ��ͼ�ļ���.
	//! \return ��ͼ��ATOM_VFS�ļ���
	//! \sa setTextureFileName
	const ATOM_STRING &getColorFileName (void) const;

	//! �������ӵ���ת�ٶ�
	//! \param val ������ת�ٶ�(����/��)
	//! \sa getRotationVelocity setRotationVelocityVar getRotationVelocityVar
	void setRotationVelocity (float val);

	//! ��ȡ���ӵ���ת�ٶ�
	//! \return ���ӵ���ת�ٶ�(����/��)
	//! \sa setRotationVelocity setRotationVelocityVar getRotationVelocityVar
	float getRotationVelocity (void) const;

	//! �������ӵ���ת�ٶ������Χ
	//! \param val ������ת�ٶ������Χ(����/��)
	//! \sa getRotationVelocityVar setRotationVelocity getRotationVelocity
	void setRotationVelocityVar (float val);

	//! ��ȡ���ӵ���ת�ٶ������Χ
	//! \return ���ӵ���ת�ٶ������Χ(����/��)
	//! \sa setRotationVelocityVar setRotationVelocity getRotationVelocity
	float getRotationVelocityVar (void) const;

	//! ������ϵͳ����Ϊ��ʼ״̬.
	//! \param clearParticles �Ƿ�ɾ�����л����
	void reset (bool clearParticles);

	//! �������л����״̬.
	//! Ϊ�˲������Ӿ������ڲ��ɼ�ʱ������Ȼ��������״̬��
	//! \param viewMatrix ��ǰ���������
	//! \param projectionMatrix ��ǰͶӰ����
	void update (const ATOM_Matrix4x4f &viewMatrix, const ATOM_Matrix4x4f &projectionMatrix);

	//! ���¶��˻���.
	//! ֻ������ϵͳ�ɼ�������²Ż���¶��㻺��
	void updateVertexArrays (ATOM_Camera *camera);

	//! ��ȡ���л���ӵ�����.
	//! \return ���л���ӵ�����
	unsigned getActiveParticleCount (void) const;

	//! ��ȡ����ϵͳ����.
	//! \return ����ϵͳ����Ⱦ����
	//! \note �˲������ڲ���������Ҫ�������޸�
	ATOM_Material *getMaterial (void);

	//! ��������ϵͳ������ϵ��.
	//! \param scalar ����ϵͳ������ϵ��
	//! \note ������βЧ��������ϵͳ������ʹ�û��ھֲ�����ϵ�任��setO2T������������Ҫ�˽ӿڣ���Ϊʹ��PointSprite��ֻ�ܲ��õȱ�����
	//! \sa getScalar
	void setScalar (float scalar);

	//! ��ȡ����ϵͳ������ϵ��.
	//! \return ����ϵͳ������ϵ��
	//! \sa setScalar
	float getScalar (void) const;

	//! �������ӵĿ�߱�.
	//! \param aspect ���ӵĿ�߱�
	void setAspect (float aspect);

	//! ��ȡ���ӵĿ�߱�
	//! \return ���ӵĿ�߱�
	float getAspect (void) const;

	//! �������Ӷ����ٶ�
	//! \param speed ���Ӷ����ٶ�
	void setJitterSpeed (float speed);

	//! ��ȡ���Ӷ����ٶ�
	//! \return ���Ӷ����ٶ�
	float getJitterSpeed (void) const;

	//! �������Ӷ���ǿ��
	//! \param power ���Ӷ���ǿ��
	void setJitterPower (float power);

	//! ��ȡ���Ӷ���ǿ��
	//! \return ���Ӷ���ǿ��
	float getJitterPower (void) const;

	//! ��ͣ����
	void pause (void);

	//! �ָ�����
	void resume (void);

	//! �Ƿ���ͣ
	bool paused (void) const;

	//! ��ȡ��ʼʱ��
	unsigned getStartTick (void) const;

	//! ���÷�������״
	void setEmitterShape (EmitterShape shape);

	//! ��ȡ��������״
	EmitterShape getEmitterShape (void) const;

	//! ����������̬ 
	void setParticleForm (ParticleForm form);

	//! ��ȡ������̬
	ParticleForm getParticleForm (void) const;

	//! ���÷��䷽ʽ
	void setEmitterBehavior (EmitterBehavior behavior);

	//! ��ȡ���䷽ʽ
	EmitterBehavior getEmitterBehavior (void) const;

	//! ���÷���뾶
	void setEmitterConeRadius (float value);

	//! ��ȡ����뾶
	float getEmitterConeRadius (void) const;

	//! ���÷���뾶�����Χ
	void setEmitterConeRadiusVar (float value);

	//! ���÷���뾶�����Χ
	float getEmitterConeRadiusVar (void) const;

	//! ���÷�������״�Ĵ�С
	void setEmitterShapeSize (const ATOM_Vector3f &size);

	//! ��ȡ��������״��С
	const ATOM_Vector3f &getEmitterShapeSize (void) const;

	//! ���÷�������״�Ĵ�С�����Χ
	void setEmitterShapeSizeVar (const ATOM_Vector3f &size);

	//! ��ȡ��������״�Ĵ�С�����Χ
	const ATOM_Vector3f &getEmitterShapeSizeVar (void) const;

	//! ���÷�������
	void setParticleVelocity (float value);

	//! ��ȡ��������
	float getParticleVelocity (void) const;

	//! ���÷������������Χ
	void setParticleVelocityVar (float value);

	//! ��ȡ�������������Χ
	float getParticleVelocityVar (void) const;

	//! �������ӳ�ʼ����ֵ
	void setParticleLife (float value);

	//! ��ȡ���ӳ�ʼ����ֵ
	float getParticleLife (void) const;

	//! �������ӳ�ʼ����ֵ�����Χ
	void setParticleLifeVar (float value);

	//! ��ȡ���ӳ�ʼ����ֵ�����Χ
	float getParticleLifeVar (void) const;

	//! �������ӳ�ʼ��С
	void setParticleSize1 (float value);

	//! ��ȡ���ӳ�ʼ��С
	float getParticleSize1 (void) const;

	//! �������ӳ�ʼ��С���ֵ
	void setParticleSize1Var (float value);

	//! ��ȡ���ӳ�ʼ��С���ֵ
	float getParticleSize1Var (void) const;

	//! �������ӽ�����С
	void setParticleSize2 (float value);

	//! ��ȡ���ӽ�����С
	float getParticleSize2 (void) const;

	//! �������ӽ�����С���ֵ
	void setParticleSize2Var (float value);

	//! ��ȡ���ӽ�����С���ֵ
	float getParticleSize2Var (void) const;

	//! �������Ӽ��ٶ�
	void setParticleAccel (float value);

	//! ��ȡ���Ӽ��ٶ�
	float getParticleAccel (void) const;

	//! �������Ӽ��ٶ����ֵ
	void setParticleAccelVar (float value);

	//! ��ȡ���Ӽ��ٶ����ֵ
	float getParticleAccelVar (void) const;

	//! ����ȫ��͸����
	void setTransparency (float alpha);

	//! ��ȡȫ��͸����
	float getTransparency (void) const;

	//! �����ӳ�ʱ��(����)
	void setDelay (unsigned timeInMs);

	//! ��ȡ�ӳ�ʱ��(����)
	unsigned getDelay (void) const;

	//! ����ȫ����ɫ
	void setColor (const ATOM_Vector4f &color);

	//! ��ȡȫ����ɫ
	const ATOM_Vector4f &getColor (void) const;

	//---- wangjian added  ----//

	//! ����ȫ����ɫǿ������
	void setColorMultiplier (const float colorMultiplier);

	//! ��ȡȫ����ɫǿ������
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
	// �첽���أ�����첽������ɱ��
	bool checkAsyncLoadFlag();
	// ֱ�Ӽ��ؽӿ�
	bool loadSync();
	//----------------------//

protected:
	ParticleNode *_activeParticleList;	// ���������ͷָ��
	ParticleNode *_freeParticleList;	// ������������ͷָ��

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
