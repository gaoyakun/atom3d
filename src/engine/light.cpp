#include "stdafx.h"
#include "light.h"
#include "visitor.h"

#define ATTENUATION_NOT_USED ATOM_Vector3f(-1000000.0f, -1000000.0f, -1000000.0f)

ATOM_Light::ATOM_Light (void)
{
	_color.set (1.f, 1.f, 1.f, 1.f);
	_direction.set (0.f, -1.f, 0.f);
	_castShadow = 1;
	_type = Directional;
	_range = 100;
	_innerCone = 0.3f;
	_outerCone = 1.2f;
	_attenuation.set (1.f, 0.f, 0.1f);
	//------------------------------//
	// wangjian added
	_hasAttenuation = true;
	//------------------------------//
	_position.set (0.f, 0.f, 0.f);
}

void ATOM_Light::setColorARGB (const ATOM_ColorARGB &color)
{
	color.getFloats (&_color.x, &_color.y, &_color.z, &_color.w);
}

void ATOM_Light::setColor4f (const ATOM_Vector4f &color)
{
	_color = color;
}

ATOM_ColorARGB ATOM_Light::getColorARGB (void) const
{
	return ATOM_ColorARGB (ATOM_saturate(_color.x), ATOM_saturate(_color.y), ATOM_saturate(_color.z), ATOM_saturate(_color.w));
}

const ATOM_Vector4f &ATOM_Light::getColor4f (void) const
{
	return _color;
}

void ATOM_Light::setCastShadow (int cast)
{
	_castShadow = cast;
}

int ATOM_Light::getCastShadow (void) const
{
	return _castShadow;
}

void ATOM_Light::setRange (const ATOM_Vector3f &range)
{
	_range = range;
}

const ATOM_Vector3f &ATOM_Light::getRange (void) const
{
	return _range;
}

void ATOM_Light::setLightType (ATOM_Light::LightType type)
{
	_type = type;
}

ATOM_Light::LightType ATOM_Light::getLightType (void) const
{
	return _type;
}

void ATOM_Light::setInnerCone (float angle)
{
    _innerCone = angle;
}

float ATOM_Light::getInnerCone (void) const
{
	return _innerCone;
}

void ATOM_Light::setOuterCone (float angle)
{
    _outerCone = angle;
}

float ATOM_Light::getOuterCone (void) const
{
	return _outerCone;
}
//---- wangjian modified --------//
void ATOM_Light::setAttenuation (const ATOM_Vector3f &atten)
{
	if( !_hasAttenuation )
		return;

    _attenuation = atten;
}

const ATOM_Vector3f &ATOM_Light::getAttenuation (void) const
{
	return _attenuation;
}
//---- wangjian added ----//
void ATOM_Light::setHasAttenuation(int has)
{
	_hasAttenuation = has;
}
bool ATOM_Light::hasAttenuationParam() const
{
	return _hasAttenuation/*_attenuation != ATTENUATION_NOT_USED*/;
}
//------------------------//
const ATOM_Vector3f &ATOM_Light::getDirection (void) const
{
	return _direction;
}

void ATOM_Light::setDirection (const ATOM_Vector3f &v)
{
	_direction.normalizeFrom (v);
}

const ATOM_Vector3f &ATOM_Light::getPosition (void) const
{
	return _position;
}

void ATOM_Light::setPosition (const ATOM_Vector3f &v)
{
	_position = v;
}

ATOM_Light & ATOM_Light::operator = (const ATOM_Light &other)
{
	_type = other._type;
	_castShadow = other._castShadow;
	_color = other._color;
	_range = other._range;
	_innerCone = other._innerCone;
	_outerCone = other._outerCone;
	_attenuation = other._attenuation;
	_direction = other._direction;
	_position = other._position;

	return *this;
}

