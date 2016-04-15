#include "StdAfx.h"
#include "weapontrail_keyframe.h"

ATOM_WeaponTrailKeyFrame::ATOM_WeaponTrailKeyFrame (void)
{
}

ATOM_WeaponTrailKeyFrame::ATOM_WeaponTrailKeyFrame (const ATOM_WeaponTrailKeyFrameData &data)
{
	setValue (data);
}

void ATOM_WeaponTrailKeyFrame::visit (ATOM_Node &node)
{
}

void ATOM_WeaponTrailKeyFrame::visit (ATOM_WeaponTrail &node)
{
	node.setColor (getValue().color);
}

void ATOM_WeaponTrailKeyFrameValue::setData (const ATOM_WeaponTrailKeyFrameData &data)
{
	_data = data;
}

const ATOM_WeaponTrailKeyFrameData & ATOM_WeaponTrailKeyFrameValue::getData (void) const
{
	return _data;
}

