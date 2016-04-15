#include "stdafx.h"
#include "CylinderEmitter.h"



ATOM_SCRIPT_INTERFACE_BEGIN(CylinderEmitter)
ATOM_ATTRIBUTES_BEGIN(CylinderEmitter)
ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "RadiusMin", getRadiusMin, setRadiusMin, 0.f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "RadiusMax", getRadiusMax, setRadiusMax, 10.f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "SurfaceType", getSurfaceType, setSurfaceType, SURFACETYPE_SQUARE, 0)
ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "Height", getHeight, setHeight, 0.f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "LifeMin", getLifeMin, setLifeMin, 500.f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "LifeMax", getLifeMax, setLifeMax, 500.f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "DirMin", getDirMin, setDirMin, ATOM_Vector3f(0.f, 1.f, 0.f), 0)
ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "DirMax", getDirMax, setDirMax, ATOM_Vector3f(0.f, 1.f, 0.f), 0)
ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "VelocityMin", getVelocityMin, setVelocityMin, 0.1f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "VelocityMax", getVelocityMax, setVelocityMax, 0.1f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "AccMin", getAccMin, setAccMin, 0.98f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "AccMax", getAccMax, setAccMax, 0.98f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "Size1Min", getSize1Min, setSize1Min, 0.2f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "Size1Max", getSize1Max, setSize1Max, 0.2f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "Size2Min", getSize2Min, setSize2Min, 0.2f, 0)
ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "Size2Max", getSize2Max, setSize2Max, 0.2f, 0)
//ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "Color1Min", getColor1Min, setColor1Min, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
//ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "Color1Max", getColor1Max, setColor1Max, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
//ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "Color2Min", getColor2Min, setColor2Min, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
//ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "Color2Max", getColor2Max, setColor2Max, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), 0)
ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "ValidDuration", getValidDuration, setValidDuration, 1000, 0)
ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "InvalidDuration", getInvalidDuration, setInvalidDuration, 0, 0)
ATOM_ATTRIBUTE_PERSISTENT(CylinderEmitter, "Expend", getExpend, setExpend, 0.f, 0)
ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(CylinderEmitter, ATOM_ParticleEmitter)

// 构造函数
CylinderEmitter::CylinderEmitter()
{
	_radiusMin = 0.f;
	_radiusMax = 10.f;
	_height = 0.f;
	_surfaceType = SURFACETYPE_SQUARE;

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
	_tick = ATOM_GetTick();

	_expend = 0.f;
}

// 初始化粒子
void CylinderEmitter::initParticle (ATOM_ParticleSystem *ps, ATOM_Particle *particle, unsigned emitTime)
{
	// 检查是否处于不发射状态
	if(_invalidDuration)
	{	
		int now = ATOM_GetTick();
		int elapsed = now - _tick;
		elapsed = elapsed % (_validDuration + _invalidDuration);
		if(elapsed > _validDuration)
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
	switch(_surfaceType)
	{
	case SURFACETYPE_CIRCLE:
		initPosition4Circle(particle);
		break;

	default:
		initPosition4Square(particle);
	}

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
		//particle->velocity.y += _expend;
	}

	//particle->color1 = rand(_color1Min, _color1Max);
	//particle->color2 = rand(_color2Min, _color2Max);
	particle->size1 = ATOM_randomf(_size1Min, _size1Max);
	particle->size2 = ATOM_randomf(_size2Min, _size2Max);
	particle->lifeSpan = ATOM_randomf(_lifeMin, _lifeMax);
	particle->acceleration = ATOM_randomf(_accMin, _accMax);
}

void CylinderEmitter::reset()
{
	_tick = ATOM_GetTick();
}

// 用圆形形面初始化位置
void CylinderEmitter::initPosition4Circle(ATOM_Particle *particle)
{
	float radius = ATOM_randomf(_radiusMin, _radiusMax);
	float t = ATOM_randomf(0.f, ATOM_TwoPi);
	float s, c;
	ATOM_sincos (t, &s, &c);
	particle->position.x = c * radius;
	particle->position.z = s * radius;
	particle->position.y = ATOM_randomf(0.f, _height);
}

// 用方形面初始化位置
void CylinderEmitter::initPosition4Square(ATOM_Particle *particle)
{
	particle->position.x = ATOM_randomf(-_radiusMax, _radiusMax);
	if(_radiusMin > 0.f && particle->position.x > -_radiusMin && particle->position.x < _radiusMin)
	{
		if(rand() & 1)
		{
			particle->position.z = ATOM_randomf(-_radiusMax, -_radiusMin);
		}
		else
		{
			particle->position.z = ATOM_randomf(_radiusMin, _radiusMax);
		}
	}
	else
	{
		particle->position.z = ATOM_randomf(-_radiusMax, _radiusMax);
	}
	particle->position.y = ATOM_randomf(0.f, _height);
}
