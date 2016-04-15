#include "stdafx.h"
#include "RingEmitter.h"








ATOM_SCRIPT_INTERFACE_BEGIN(RingEmitter)
ATOM_ATTRIBUTES_BEGIN(RingEmitter)
ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "Radius", getRadius, setRadius, 5.f, 0)
ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "AngleVelocity", getAngleVelocity, setAngleVelocity, 1.f, 0)
ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "LifeMin", getLifeMin, setLifeMin, 500.f, 0)
ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "LifeMax", getLifeMax, setLifeMax, 500.f, 0)
ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "DirMin", getDirMin, setDirMin, ATOM_Vector3f(0.f, 1.f, 0.f), 0)
ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "DirMax", getDirMax, setDirMax, ATOM_Vector3f(0.f, 1.f, 0.f), 0)
ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "VelocityMin", getVelocityMin, setVelocityMin, 0.1f, 0)
ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "VelocityMax", getVelocityMax, setVelocityMax, 0.1f, 0)
ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "AccMin", getAccMin, setAccMin, 0.98f, 0)
ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "AccMax", getAccMax, setAccMax, 0.98f, 0)
ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "Size1Min", getSize1Min, setSize1Min, 0.2f, 0)
ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "Size1Max", getSize1Max, setSize1Max, 0.2f, 0)
ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "Size2Min", getSize2Min, setSize2Min, 0.2f, 0)
ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "Size2Max", getSize2Max, setSize2Max, 0.2f, 0)
//ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "Color1Min", getColor1Min, setColor1Min, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
//ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "Color1Max", getColor1Max, setColor1Max, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
//ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "Color2Min", getColor2Min, setColor2Min, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
//ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "Color2Max", getColor2Max, setColor2Max, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "ValidDuration", getValidDuration, setValidDuration, 1000, 0)
ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "InvalidDuration", getInvalidDuration, setInvalidDuration, 0, 0)
ATOM_ATTRIBUTE_PERSISTENT(RingEmitter, "Expend", getExpend, setExpend, 0.f, 0)
ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(RingEmitter, ATOM_ParticleEmitter)

// 构造函数
RingEmitter::RingEmitter()
{
	_radius = 5.f;
	_angleVelocity = 1.f;

	_lifeMin = 500.f;
	_lifeMax = 500.f;

	_dirMin = ATOM_Vector3f(0.f, 1.f, 0.f);
	_dirMax = ATOM_Vector3f(0.f, 1.f, 0.f);
	_velocityMin = 0.1f;
	_velocityMax = 0.1f;
	_accMin = 0.98f;
	_accMax = 0.98f;

	_size1Min = 0.2f;
	_size1Max = 0.2f;
	_size2Min = 0.2f;
	_size2Max = 0.2f;
	//_color1Min = ATOM_Vector4f(1.f, 1.f, 1.f, 1.f);
	//_color1Max = ATOM_Vector4f(1.f, 1.f, 1.f, 1.f);
	//_color2Min = ATOM_Vector4f(1.f, 1.f, 1.f, 1.f);
	//_color2Max = ATOM_Vector4f(1.f, 1.f, 1.f, 1.f);
	
	_validDuration = 1000;
	_invalidDuration = 0;	

	_expend = 0.f;
	_count = 0;

	_tick = ATOM_GetTick();
}

// 初始化粒子
void RingEmitter::initParticle (ATOM_ParticleSystem *ps, ATOM_Particle *particle, unsigned emitTime)
{
	// 检查是否处于不发射状态
	if(_invalidDuration)
	{		
		int now = ATOM_GetTick();
		int elapsed = now - _tick;	
		int temp = elapsed % (_validDuration + _invalidDuration);
		if(temp > _validDuration)
		{	// 处于无效时间
			//particle->color1 = 0;
			//particle->color2 = 0;
			particle->size1 = 0;
			particle->size2 = 0;
			particle->lifeSpan = 0;
			return;
		}
	}

	// 取得位置
	float t = 0.01f*_angleVelocity*_count++; 
	float s, c;
	ATOM_sincos (t, &s, &c);
	particle->position.x = c * _radius;
	particle->position.z = s * _radius;
	particle->position.y = 0;

	ATOM_Vector3f dir = ATOM_randomT(_dirMin, _dirMax);
	dir.normalize();
	particle->velocity = dir*ATOM_randomf(_velocityMin, _velocityMax);
	if(0.f != _expend)
	{		
		ATOM_Vector3f dir(particle->position);
		dir.y += _expend;
		dir.normalize();
		float l = particle->velocity.getLength();
		particle->velocity = l*dir;
	}

	//particle->color1 = rand(_color1Min, _color1Max);
	//particle->color2 = rand(_color2Min, _color2Max);
	particle->size1 = ATOM_randomf(_size1Min, _size1Max);
	particle->size2 = ATOM_randomf(_size2Min, _size2Max);
	particle->lifeSpan = ATOM_randomf(_lifeMin, _lifeMax);
	particle->acceleration = ATOM_randomf(_accMin, _accMax);
}

void RingEmitter::reset()
{
	_tick = ATOM_GetTick();
}

