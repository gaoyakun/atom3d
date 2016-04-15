#include "StdAfx.h"
#include "composition_keyframevalue.h"

void ATOM_CompositionKeyFrameValue::setTranslation (const ATOM_Vector3f &v)
{
	_translation = v;
}

const ATOM_Vector3f &ATOM_CompositionKeyFrameValue::getTranslation  (void) const
{
	return _translation;
}

void ATOM_CompositionKeyFrameValue::setScale (const ATOM_Vector3f &v)
{
	_scale = v;
}

const ATOM_Vector3f &ATOM_CompositionKeyFrameValue::getScale (void) const
{
	return _scale;
}

void ATOM_CompositionKeyFrameValue::setRotation (const ATOM_Vector3f &v)
{
	_rotation = v;
}

const ATOM_Vector3f &ATOM_CompositionKeyFrameValue::getRotation (void) const
{
	return _rotation;
}

void ATOM_CompositionKeyFrameValue::setVisible (int visible)
{
	_visible = visible;
}

int ATOM_CompositionKeyFrameValue::getVisible (void) const
{
	return _visible;
}

