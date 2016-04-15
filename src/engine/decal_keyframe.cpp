#include "StdAfx.h"
#include "decal_keyframe.h"

ATOM_DecalKeyFrame::ATOM_DecalKeyFrame (void)
{
}

ATOM_DecalKeyFrame::ATOM_DecalKeyFrame (const ATOM_DecalKeyFrameData &data)
{
	setValue (data);
}

void ATOM_DecalKeyFrame::visit (ATOM_Node &node)
{
}

void ATOM_DecalKeyFrame::visit (ATOM_Decal &node)
{
	node.setColor (getValue().color);
}

void ATOM_DecalKeyFrameValue::setData (const ATOM_DecalKeyFrameData &data)
{
	_data = data;
}

const ATOM_DecalKeyFrameData & ATOM_DecalKeyFrameValue::getData (void) const
{
	return _data;
}

