#include "StdAfx.h"
#include "light_keyframe.h"

ATOM_LightKeyFrame::ATOM_LightKeyFrame (void)
{
}

ATOM_LightKeyFrame::ATOM_LightKeyFrame (const ATOM_LightKeyFrameData &data)
{
	setValue (data);
}

void ATOM_LightKeyFrame::visit (ATOM_Node &node)
{
}

void ATOM_LightKeyFrame::visit (ATOM_LightNode &node)
{
	const ATOM_LightKeyFrameData &data = getValue ();
	node.setLightType (data.type);
	node.setLightColor (data.color);
	node.setLightAttenuation (data.atten);
}

void ATOM_LightKeyFrameValue::setData (const ATOM_LightKeyFrameData &data)
{
	_data = data;
}

const ATOM_LightKeyFrameData & ATOM_LightKeyFrameValue::getData (void) const
{
	return _data;
}

