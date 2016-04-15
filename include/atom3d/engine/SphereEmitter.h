/********************************************************************
	created:	2009/02/10
	created:	10:2:2009   13:10
	filename: 	SphereEmitter.h
	author:		Dashi Bai
	
	history:	
*********************************************************************/
#pragma once

#include "ParticleEmitter.h"

//! 球状发射器
class SphereEmitter: public ATOM_ParticleEmitter
{
	ATOM_CLASS(engine, SphereEmitter, SphereEmitter)
	ATOM_DECLARE_SCRIPT_INTERFACE(SphereEmitter)

public:
	//! 构造函数
	SphereEmitter();

	//! 初始化粒子
	virtual void initParticle (ATOM_ParticleSystem *ps, ATOM_Particle *particle, unsigned emitTime);

	//! 重设发射器
	virtual void reset();

	//!y方向发射限制
	DECLARE_EMITTER_PROPERTY(float, _missAngleY, getMissAngleY, setMissAngleY)

	//! 速度最小值
	DECLARE_EMITTER_PROPERTY(float, _velocityMin, getVelocityMin, setVelocityMin)

	//! 速度最大值
	DECLARE_EMITTER_PROPERTY(float, _velocityMax, getVelocityMax, setVelocityMax)

	//! 生命最小值
	DECLARE_EMITTER_PROPERTY(float, _lifeMin, getLifeMin, setLifeMin)

	//! 生命最大值
	DECLARE_EMITTER_PROPERTY(float, _lifeMax, getLifeMax, setLifeMax)

	//! 开始大小最小值
	DECLARE_EMITTER_PROPERTY(float, _size1Min, getSize1Min, setSize1Min)

	//! 开始大小最大值
	DECLARE_EMITTER_PROPERTY(float, _size1Max, getSize1Max, setSize1Max)	

	//! 结束大小最小值
	DECLARE_EMITTER_PROPERTY(float, _size2Min, getSize2Min, setSize2Min)

	//! 结束大小最大值
	DECLARE_EMITTER_PROPERTY(float, _size2Max, getSize2Max, setSize2Max)

	////! 开始颜色最小值
	//DECLARE_EMITTER_PROPERTY(ATOM_Vector4f, _color1Min, getColor1Min, setColor1Min)

	////! 开始颜色最大值 
	//DECLARE_EMITTER_PROPERTY(ATOM_Vector4f, _color1Max, getColor1Max, setColor1Max)

	////! 结束颜色最小值
	//DECLARE_EMITTER_PROPERTY(ATOM_Vector4f, _color2Min, getColor2Min, setColor2Min)

	////! 结束颜色最大值
	//DECLARE_EMITTER_PROPERTY(ATOM_Vector4f, _color2Max, getColor2Max, setColor2Max)

	//! 加速度最小值
	DECLARE_EMITTER_PROPERTY(float, _accMin, getAccMin, setAccMin)

	//! 加速度最大值
	DECLARE_EMITTER_PROPERTY(float, _accMax, getAccMax, setAccMax)

	//! 有效时间断
	DECLARE_EMITTER_PROPERTY(int, _validDuration, getValidDuration, setValidDuration)

	//! 无效时间断
	DECLARE_EMITTER_PROPERTY(int, _invalidDuration, getInvalidDuration, setInvalidDuration)

protected:
	// 在圆面上初始化位置
	void initPosition4Circle(ATOM_Particle *particle);

	// 在方面上初始化位置
	void initPosition4Square(ATOM_Particle *particle);

private:
	float                   _missAngleY;
	float					_lifeMin;
	float					_lifeMax;

	float					_velocityMin;
	float					_velocityMax;
	float					_accMin;
	float					_accMax;

	float					_size1Min;
	float					_size1Max;
	float					_size2Min;
	float					_size2Max;
	//ATOM_Vector4f	_color1Min;
	//ATOM_Vector4f	_color1Max;
	//ATOM_Vector4f	_color2Min;
	//ATOM_Vector4f	_color2Max;

	int						_validDuration;
	int						_invalidDuration;

	int						_tick;
};

