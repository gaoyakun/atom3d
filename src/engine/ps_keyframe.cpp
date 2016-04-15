#include "StdAfx.h"
#include "ps_keyframe.h"

ATOM_ParticleSystemKeyFrame::ATOM_ParticleSystemKeyFrame (void)
{
}

ATOM_ParticleSystemKeyFrame::ATOM_ParticleSystemKeyFrame (const ATOM_PSKeyFrameData &data)
{
	setValue (data);
}

void ATOM_ParticleSystemKeyFrame::visit (ATOM_Node &node)
{
}

void ATOM_ParticleSystemKeyFrame::visit (ATOM_ParticleSystem &node)
{
	const ATOM_PSKeyFrameData &data = getValue ();

	node.setEmitInterval (data.emitInterval);
	node.setEmitCount (data.emitCount);
	node.setEmitterShapeSize (data.emitShapeSize);
	node.setEmitterShapeSizeVar (data.emitShapeSizeVar);
	node.setEmitterConeRadius (data.emitCone);
	node.setEmitterConeRadiusVar (data.emitConeVar);
	node.setParticleVelocity (data.velocity);
	node.setParticleVelocityVar (data.velocityVar);
	node.setParticleLife (data.life);
	node.setParticleLifeVar (data.lifeVar);
	node.setParticleSize1 (data.size1);
	node.setParticleSize1Var (data.size1Var);
	node.setParticleSize2 (data.size2);
	node.setParticleSize2Var (data.size2Var);
	node.setParticleAccel (data.accel);
	node.setParticleAccelVar (data.accelVar);
	node.setGravity (data.gravity);
	node.setWind (data.wind);
	node.setScalar (data.scale);
	node.setTransparency (data.transparency);
	node.setColor (data.color);
	//--- wangjian added ---//
	node.setColorMultiplier(data.colorMultiplier);
}

void ATOM_ParticleSystemKeyFrameValue::setParticleSystemParams (const ATOM_PSKeyFrameData &data)
{
	_data = data;
}

const ATOM_PSKeyFrameData &ATOM_ParticleSystemKeyFrameValue::getParticleSystemParams (void) const
{
	return _data;
}

