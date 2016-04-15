#include "StdAfx.h"
#include "shape_keyframe.h"

ATOM_ShapeKeyFrame::ATOM_ShapeKeyFrame (void)
{
}

ATOM_ShapeKeyFrame::ATOM_ShapeKeyFrame (const ATOM_ShapeKeyFrameData &data)
{
	setValue (data);
}

void ATOM_ShapeKeyFrame::visit (ATOM_Node &node)
{
}

void ATOM_ShapeKeyFrame::visit (ATOM_ShapeNode &node)
{
	node.setColor (getValue().color);
	node.setTransparency (getValue().transparency);
	node.setShearing (getValue().shearing);
	node.setUVRotation (getValue().uvRotation);
	node.setUVScaleOffset (getValue().uvScaleOffset);
	node.setSize (getValue().size);
	//--- wangjian added ---//
	node.setColorMultiplier (getValue().colorMultiplier);
	node.setResolveScale( getValue().resolveScale);
	//----------------------//
}

void ATOM_ShapeKeyFrameValue::setData (const ATOM_ShapeKeyFrameData &data)
{
	_data = data;
}

const ATOM_ShapeKeyFrameData & ATOM_ShapeKeyFrameValue::getData (void) const
{
	return _data;
}

