/********************************************************************
	created:	2009/02/10
	created:	10:2:2009   13:10
	filename: 	SphereEmitter.h
	author:		Dashi Bai
	
	history:	
*********************************************************************/
#pragma once

#include "ParticleEmitter.h"

//! ��״������
class SphereEmitter: public ATOM_ParticleEmitter
{
	ATOM_CLASS(engine, SphereEmitter, SphereEmitter)
	ATOM_DECLARE_SCRIPT_INTERFACE(SphereEmitter)

public:
	//! ���캯��
	SphereEmitter();

	//! ��ʼ������
	virtual void initParticle (ATOM_ParticleSystem *ps, ATOM_Particle *particle, unsigned emitTime);

	//! ���跢����
	virtual void reset();

	//!y����������
	DECLARE_EMITTER_PROPERTY(float, _missAngleY, getMissAngleY, setMissAngleY)

	//! �ٶ���Сֵ
	DECLARE_EMITTER_PROPERTY(float, _velocityMin, getVelocityMin, setVelocityMin)

	//! �ٶ����ֵ
	DECLARE_EMITTER_PROPERTY(float, _velocityMax, getVelocityMax, setVelocityMax)

	//! ������Сֵ
	DECLARE_EMITTER_PROPERTY(float, _lifeMin, getLifeMin, setLifeMin)

	//! �������ֵ
	DECLARE_EMITTER_PROPERTY(float, _lifeMax, getLifeMax, setLifeMax)

	//! ��ʼ��С��Сֵ
	DECLARE_EMITTER_PROPERTY(float, _size1Min, getSize1Min, setSize1Min)

	//! ��ʼ��С���ֵ
	DECLARE_EMITTER_PROPERTY(float, _size1Max, getSize1Max, setSize1Max)	

	//! ������С��Сֵ
	DECLARE_EMITTER_PROPERTY(float, _size2Min, getSize2Min, setSize2Min)

	//! ������С���ֵ
	DECLARE_EMITTER_PROPERTY(float, _size2Max, getSize2Max, setSize2Max)

	////! ��ʼ��ɫ��Сֵ
	//DECLARE_EMITTER_PROPERTY(ATOM_Vector4f, _color1Min, getColor1Min, setColor1Min)

	////! ��ʼ��ɫ���ֵ 
	//DECLARE_EMITTER_PROPERTY(ATOM_Vector4f, _color1Max, getColor1Max, setColor1Max)

	////! ������ɫ��Сֵ
	//DECLARE_EMITTER_PROPERTY(ATOM_Vector4f, _color2Min, getColor2Min, setColor2Min)

	////! ������ɫ���ֵ
	//DECLARE_EMITTER_PROPERTY(ATOM_Vector4f, _color2Max, getColor2Max, setColor2Max)

	//! ���ٶ���Сֵ
	DECLARE_EMITTER_PROPERTY(float, _accMin, getAccMin, setAccMin)

	//! ���ٶ����ֵ
	DECLARE_EMITTER_PROPERTY(float, _accMax, getAccMax, setAccMax)

	//! ��Чʱ���
	DECLARE_EMITTER_PROPERTY(int, _validDuration, getValidDuration, setValidDuration)

	//! ��Чʱ���
	DECLARE_EMITTER_PROPERTY(int, _invalidDuration, getInvalidDuration, setInvalidDuration)

protected:
	// ��Բ���ϳ�ʼ��λ��
	void initPosition4Circle(ATOM_Particle *particle);

	// �ڷ����ϳ�ʼ��λ��
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

