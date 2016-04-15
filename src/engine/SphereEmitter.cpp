#include "stdafx.h"
#include "SphereEmitter.h"








ATOM_SCRIPT_INTERFACE_BEGIN(SphereEmitter)
ATOM_ATTRIBUTES_BEGIN(SphereEmitter)
ATOM_ATTRIBUTE_PERSISTENT(SphereEmitter, "MissAngleY", getMissAngleY, setMissAngleY, 135.f, 0)
ATOM_ATTRIBUTE_PERSISTENT(SphereEmitter, "LifeMin", getLifeMin, setLifeMin, 500.f, 0)
ATOM_ATTRIBUTE_PERSISTENT(SphereEmitter, "LifeMax", getLifeMax, setLifeMax, 500.f, 0)
ATOM_ATTRIBUTE_PERSISTENT(SphereEmitter, "VelocityMin", getVelocityMin, setVelocityMin, 0.1f, 0)
ATOM_ATTRIBUTE_PERSISTENT(SphereEmitter, "VelocityMax", getVelocityMax, setVelocityMax, 0.1f, 0)
ATOM_ATTRIBUTE_PERSISTENT(SphereEmitter, "AccMin", getAccMin, setAccMin, 0.98f, 0)
ATOM_ATTRIBUTE_PERSISTENT(SphereEmitter, "AccMax", getAccMax, setAccMax, 0.98f, 0)
ATOM_ATTRIBUTE_PERSISTENT(SphereEmitter, "Size1Min", getSize1Min, setSize1Min, 0.2f, 0)
ATOM_ATTRIBUTE_PERSISTENT(SphereEmitter, "Size1Max", getSize1Max, setSize1Max, 0.2f, 0)
ATOM_ATTRIBUTE_PERSISTENT(SphereEmitter, "Size2Min", getSize2Min, setSize2Min, 0.2f, 0)
ATOM_ATTRIBUTE_PERSISTENT(SphereEmitter, "Size2Max", getSize2Max, setSize2Max, 0.2f, 0)
//ATOM_ATTRIBUTE_PERSISTENT(SphereEmitter, "Color1Min", getColor1Min, setColor1Min, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
//ATOM_ATTRIBUTE_PERSISTENT(SphereEmitter, "Color1Max", getColor1Max, setColor1Max, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
//ATOM_ATTRIBUTE_PERSISTENT(SphereEmitter, "Color2Min", getColor2Min, setColor2Min, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
//ATOM_ATTRIBUTE_PERSISTENT(SphereEmitter, "Color2Max", getColor2Max, setColor2Max, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
ATOM_ATTRIBUTE_PERSISTENT(SphereEmitter, "ValidDuration", getValidDuration, setValidDuration, 1000, 0)
ATOM_ATTRIBUTE_PERSISTENT(SphereEmitter, "InvalidDuration", getInvalidDuration, setInvalidDuration, 0, 0)
ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(SphereEmitter, ATOM_ParticleEmitter)

// 构造函数
SphereEmitter::SphereEmitter()
{
	_missAngleY = 135.f;
	_lifeMin = 500.f;
	_lifeMax = 500.f;

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
	_tick = ATOM_GetTick();

}

// 初始化粒子
void SphereEmitter::initParticle (ATOM_ParticleSystem *ps, ATOM_Particle *particle, unsigned emitTime)
{
	//particle->color1 = 0;
	//particle->color2 = 0;
	particle->size1 = 0;
	particle->size2 = 0;
	particle->lifeSpan = 0;

	// 检查是否处于不发射状态
	if(_invalidDuration)
	{	
		int now = ATOM_GetTick();
		int elapsed = now - _tick;
		elapsed = elapsed % (_validDuration + _invalidDuration);
		if(elapsed > _validDuration)
		{	// 处于无效时间
			return;
		}
	}

	// 取得位置
	particle->position.set (0.f, 0.f, 0.f);

	
	float z = -1.f + 2.f * float(rand())/float(RAND_MAX);
	float radius = ATOM_sqrt (1.f - z * z);
	float t = -ATOM_Pi + 2.f * ATOM_Pi * float(rand())/float(RAND_MAX);
	float s, c;
	ATOM_sincos (t, &s, &c);
	float x = c * radius;
	float y = s * radius;

	if ( ATOM_Pi*90/180 - acos(y/ATOM_sqrt(x*x + y*y + z*z)) < ATOM_Pi*(_missAngleY - 90)/180  )
	{//在扇形区域,返回
		return;
	}

	particle->velocity.set (x, y, z);
	particle->velocity *= ATOM_randomf(_velocityMin, _velocityMax);

	//particle->color1 = rand(_color1Min, _color1Max);
	//particle->color2 = rand(_color2Min, _color2Max);
	particle->size1 = ATOM_randomf(_size1Min, _size1Max);
	particle->size2 = ATOM_randomf(_size2Min, _size2Max);
	particle->lifeSpan = ATOM_randomf(_lifeMin, _lifeMax);
	particle->acceleration = ATOM_randomf(_accMin, _accMax);
}

void SphereEmitter::reset()
{
	_tick = ATOM_GetTick();
}
