#include "StdAfx.h"
#include "visualnode.h"

void ATOM_VisualNode::accept (ATOM_Visitor &visitor)
{
	visitor.visit (*this);
}

bool ATOM_VisualNode::rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const
{
	return false;
}

void ATOM_VisualNode::update (ATOM_Camera *camera)
{
}


