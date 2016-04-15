#include "stdafx.h"
#include "TaperEmitter.h"








ATOM_SCRIPT_INTERFACE_BEGIN(TaperEmitter)
ATOM_ATTRIBUTES_BEGIN(TaperEmitter)
ATOM_ATTRIBUTE_PERSISTENT(TaperEmitter, "Angle", getAngle, setAngle, 0.8f, 0)
ATOM_ATTRIBUTE_PERSISTENT(TaperEmitter, "Ring", getRing, setRing, 0, 0)
ATOM_ATTRIBUTE_PERSISTENT(TaperEmitter, "LifeMin", getLifeMin, setLifeMin, 500.f, 0)
ATOM_ATTRIBUTE_PERSISTENT(TaperEmitter, "LifeMax", getLifeMax, setLifeMax, 500.f, 0)
ATOM_ATTRIBUTE_PERSISTENT(TaperEmitter, "VelocityMin", getVelocityMin, setVelocityMin, 0.1f, 0)
ATOM_ATTRIBUTE_PERSISTENT(TaperEmitter, "VelocityMax", getVelocityMax, setVelocityMax, 0.1f, 0)
ATOM_ATTRIBUTE_PERSISTENT(TaperEmitter, "AccMin", getAccMin, setAccMin, 0.98f, 0)
ATOM_ATTRIBUTE_PERSISTENT(TaperEmitter, "AccMax", getAccMax, setAccMax, 0.98f, 0)
ATOM_ATTRIBUTE_PERSISTENT(TaperEmitter, "Size1Min", getSize1Min, setSize1Min, 0.2f, 0)
ATOM_ATTRIBUTE_PERSISTENT(TaperEmitter, "Size1Max", getSize1Max, setSize1Max, 0.2f, 0)
ATOM_ATTRIBUTE_PERSISTENT(TaperEmitter, "Size2Min", getSize2Min, setSize2Min, 0.2f, 0)
ATOM_ATTRIBUTE_PERSISTENT(TaperEmitter, "Size2Max", getSize2Max, setSize2Max, 0.2f, 0)
//ATOM_ATTRIBUTE_PERSISTENT(TaperEmitter, "Color1Min", getColor1Min, setColor1Min, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
//ATOM_ATTRIBUTE_PERSISTENT(TaperEmitter, "Color1Max", getColor1Max, setColor1Max, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
//ATOM_ATTRIBUTE_PERSISTENT(TaperEmitter, "Color2Min", getColor2Min, setColor2Min, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
//ATOM_ATTRIBUTE_PERSISTENT(TaperEmitter, "Color2Max", getColor2Max, setColor2Max, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
ATOM_ATTRIBUTE_PERSISTENT(TaperEmitter, "ValidDuration", getValidDuration, setValidDuration, 1000, 0)
ATOM_ATTRIBUTE_PERSISTENT(TaperEmitter, "InvalidDuration", getInvalidDuration, setInvalidDuration, 0, 0)
ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(TaperEmitter, ATOM_ParticleEmitter)

// 构造函数
TaperEmitter::TaperEmitter()
{
	_angle = 0.8f;
	_ring = 0;

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
void TaperEmitter::initParticle (ATOM_ParticleSystem *ps, ATOM_Particle *particle, unsigned emitTime)
{
	// 检查是否处于不发射状态
	if(_invalidDuration)
	{	
		int now = ATOM_GetTick();
		int elapsed = now - _tick;
		elapsed = elapsed % (_validDuration + _invalidDuration);
		if(elapsed > _validDuration)
		{	// 处于无效时间
//			particle->color1 = 0;
//			particle->color2 = 0;
			particle->size1 = 0;
			particle->size2 = 0;
			particle->lifeSpan = 0;
			return;
		}
	}

	// 取得位置
	particle->position.set (0.f, 0.f, 0.f);

	float t = _ring ? _angle : ATOM_randomf(0.f, _angle);
	float s, c;
	ATOM_sincos (t, &s, &c);
	float y = c;
	float xz = s;

	t = -ATOM_Pi + 2.f * ATOM_Pi * float(rand())/float(RAND_MAX);
	ATOM_sincos (t, &s, &c);
	float x = c * xz;
	float z = s * xz;

	particle->velocity.set(x, y, z);
	particle->velocity *= ATOM_randomf(_velocityMin, _velocityMax);

	//float radius = ntan(_angle);
	//if(!_ring)
	//{
	//	radius = rand(0.f, radius);
	//}

	//float t = -ATOM_Pi + 2.f * ATOM_Pi * float(rand())/float(RAND_MAX);
	//float s, c;
	//ATOM_sincos (t, &s, &c);
	//float x = c * radius;
	//float z = s * radius;
	//particle->velocity.set(x, 1.f, z);
	//particle->velocity.normalize();
	//particle->velocity *= rand(_velocityMin, _velocityMax);

//	particle->color1 = rand(_color1Min, _color1Max);
//	particle->color2 = rand(_color2Min, _color2Max);
	particle->size1 = ATOM_randomf(_size1Min, _size1Max);
	particle->size2 = ATOM_randomf(_size2Min, _size2Max);
	particle->lifeSpan = ATOM_randomf(_lifeMin, _lifeMax);
	particle->acceleration = ATOM_randomf(_accMin, _accMax);
}

void TaperEmitter::reset()
{
	_tick = ATOM_GetTick();
}
