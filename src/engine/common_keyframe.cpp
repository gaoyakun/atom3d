#include "StdAfx.h"
#include "common_keyframe.h"

ATOM_TransparencyKeyFrame::ATOM_TransparencyKeyFrame (void)
{
	setValue (1.f);
}

ATOM_TransparencyKeyFrame::ATOM_TransparencyKeyFrame (float transparency)
{
	setValue (transparency);
}

void ATOM_TransparencyKeyFrame::visit (ATOM_Node &node)
{
}

void ATOM_TransparencyKeyFrame::visit (ATOM_Geode &node)
{
	node.setTransparency (getValue());
}

void ATOM_TransparencyKeyFrame::visit (ATOM_ParticleSystem &node)
{
	node.setTransparency (getValue());
}

void ATOM_TransparencyKeyFrame::visit (ATOM_ShapeNode &node)
{
	node.setTransparency (getValue());
}
