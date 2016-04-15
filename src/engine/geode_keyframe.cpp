#include "StdAfx.h"
#include "geode_keyframe.h"

ATOM_GeodeKeyFrame::ATOM_GeodeKeyFrame (void)
{
}

ATOM_GeodeKeyFrame::ATOM_GeodeKeyFrame (const ATOM_GeodeKeyFrameData &data)
{
	setValue (data);
}

void ATOM_GeodeKeyFrame::visit (ATOM_Node &node)
{
}

void ATOM_GeodeKeyFrame::visit (ATOM_Geode &node)
{
	const ATOM_GeodeKeyFrameData &data = getValue ();
	if (data.actionIndex >= 0 && data.actionIndex < node.getNumTracks())
	{
		ATOM_JointAnimationTrack *track = node.getTrack (data.actionIndex);
		if (track != node.getActionMixer()->getUpsideTrack ())
		{
			node.doAction (track->getName (), ATOM_Geode::ACTIONFLAGS_DOWNSIDE|ATOM_Geode::ACTIONFLAGS_UPSIDE, data.loop, false, data.fadeTime, data.speed);
		}
	}
	node.setTransparency (data.transparency);

	//--- wangjian added ---//
	node.setColorMultiplier (data.colorMultiplier);
}

void ATOM_GeodeKeyFrameValue::setData (const ATOM_GeodeKeyFrameData &data)
{
	_data = data;
}

const ATOM_GeodeKeyFrameData & ATOM_GeodeKeyFrameValue::getData (void) const
{
	return _data;
}

