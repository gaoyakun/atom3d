#include "StdAfx.h"
#include "visible_keyframe.h"

ATOM_VisibleKeyFrame::ATOM_VisibleKeyFrame (void)
{
	setValue (1);
}

ATOM_VisibleKeyFrame::ATOM_VisibleKeyFrame (int visible)
{
	setValue (visible);
}

void ATOM_VisibleKeyFrame::visit (ATOM_Node &node)
{
	node.setShow (getValue() != 0 ? ATOM_Node::SHOW : ATOM_Node::HIDE);
}

