#include "StdAfx.h"
#include "trail_keyframe.h"

ATOM_TrailKeyFrame::ATOM_TrailKeyFrame (void)
{
}

ATOM_TrailKeyFrame::ATOM_TrailKeyFrame (const ATOM_TrailKeyFrameData &data)
{
	setValue (data);
}

void ATOM_TrailKeyFrame::visit (ATOM_Node &node)
{
}

void ATOM_TrailKeyFrame::visit (ATOM_RibbonTrail &node)
{
	node.setStartColor (getValue().colorStart);
	node.setEndColor (getValue().colorEnd);
}

void ATOM_TrailKeyFrameValue::setData (const ATOM_TrailKeyFrameData &data)
{
	_data = data;
}

const ATOM_TrailKeyFrameData & ATOM_TrailKeyFrameValue::getData (void) const
{
	return _data;
}

