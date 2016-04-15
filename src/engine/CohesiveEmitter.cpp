#include "stdafx.h"
#include "CohesiveEmitter.h"








ATOM_SCRIPT_INTERFACE_BEGIN(CohesiveEmitter)
ATOM_ATTRIBUTES_BEGIN(CohesiveEmitter)
ATOM_ATTRIBUTE_PERSISTENT(CohesiveEmitter, "Radius", getRadius, setRadius, 100.f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CohesiveEmitter, "MissAngleY", getMissAngleY, setMissAngleY, 135.f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CohesiveEmitter, "LifeMin", getLifeMin, setLifeMin, 500.f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CohesiveEmitter, "LifeMax", getLifeMax, setLifeMax, 500.f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CohesiveEmitter, "VelocityMin", getVelocityMin, setVelocityMin, 0.1f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CohesiveEmitter, "VelocityMax", getVelocityMax, setVelocityMax, 0.1f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CohesiveEmitter, "AccMin", getAccMin, setAccMin, 0.98f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CohesiveEmitter, "AccMax", getAccMax, setAccMax, 0.98f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CohesiveEmitter, "Size1Min", getSize1Min, setSize1Min, 0.2f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CohesiveEmitter, "Size1Max", getSize1Max, setSize1Max, 0.2f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CohesiveEmitter, "Size2Min", getSize2Min, setSize2Min, 0.2f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CohesiveEmitter, "Size2Max", getSize2Max, setSize2Max, 0.2f, 0)
//ATOM_ATTRIBUTE_PERSISTENT(CohesiveEmitter, "Color1Min", getColor1Min, setColor1Min, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
//ATOM_ATTRIBUTE_PERSISTENT(CohesiveEmitter, "Color1Max", getColor1Max, setColor1Max, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
//ATOM_ATTRIBUTE_PERSISTENT(CohesiveEmitter, "Color2Min", getColor2Min, setColor2Min, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
//ATOM_ATTRIBUTE_PERSISTENT(CohesiveEmitter, "Color2Max", getColor2Max, setColor2Max, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
ATOM_ATTRIBUTE_PERSISTENT(CohesiveEmitter, "ValidDuration", getValidDuration, setValidDuration, 1000, 0)
ATOM_ATTRIBUTE_PERSISTENT(CohesiveEmitter, "InvalidDuration", getInvalidDuration, setInvalidDuration, 0, 0)
ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(CohesiveEmitter, ATOM_ParticleEmitter)

// 构造函数
CohesiveEmitter::CohesiveEmitter()
{
	_radius  = 100.f;
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
void CohesiveEmitter::initParticle (ATOM_ParticleSystem *ps, ATOM_Particle *particle, unsigned emitTime)
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

	float x = ATOM_randomf(-_radius, _radius);
	float y = ATOM_randomf(-_radius, _radius);
	float z = ATOM_randomf(-_radius, _radius);
	particle->position.set(x,  y, z);
	float fLength = particle->position.getLength();
	if ( fLength > _radius )
	{//不在圆内
		return;
	}
 
	if (  ATOM_Pi*90/180 - acos(y/fLength) < ATOM_Pi*(_missAngleY - 90)/180  )
	{//在扇形区域,返回
		return;
	}

	//速度反向
	particle->velocity.set (-x, -y, -z);
	particle->velocity.normalize();
	particle->velocity *= ATOM_randomf(_velocityMin, _velocityMax);

	//particle->color1 = rand(_color1Min, _color1Max);
	//particle->color2 = rand(_color2Min, _color2Max);
	particle->size1 = ATOM_randomf(_size1Min, _size1Max);
	particle->size2 = ATOM_randomf(_size2Min, _size2Max);
	particle->lifeSpan = ATOM_randomf(_lifeMin, _lifeMax );
	particle->acceleration = ATOM_randomf(_accMin, _accMax);
}

void CohesiveEmitter::reset()
{
	_tick = ATOM_GetTick();
}
