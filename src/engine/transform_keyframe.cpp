#include "StdAfx.h"
#include "transform_keyframe.h"

ATOM_TranslateKeyFrame::ATOM_TranslateKeyFrame (void)
{
	setValue (ATOM_Vector3f(0.f, 0.f, 0.f));
}

ATOM_TranslateKeyFrame::ATOM_TranslateKeyFrame (const ATOM_Vector3f &t)
{
	setValue (t);
}

void ATOM_TranslateKeyFrame::visit (ATOM_Node &node)
{
	ATOM_Matrix4x4f m = node.getO2T ();
	m.setRow3 (3, getValue());
	node.setO2T (m);
}

ATOM_ScaleKeyFrame::ATOM_ScaleKeyFrame (void)
{
	setValue (ATOM_Vector3f(1.f, 1.f, 1.f));
}

ATOM_ScaleKeyFrame::ATOM_ScaleKeyFrame (const ATOM_Vector3f &t)
{
	setValue (t);
}

void ATOM_ScaleKeyFrame::visit (ATOM_Node &node)
{
	ATOM_Vector3f s, t;
	ATOM_Matrix4x4f r;
	ATOM_Matrix4x4f m = node.getO2T();
	m.decompose (t, r, s);

	m.m00 = r.m00 * getValue().x;
	m.m01 = r.m01 * getValue().x;
	m.m02 = r.m02 * getValue().x;
	m.m10 = r.m10 * getValue().y;
	m.m11 = r.m11 * getValue().y;
	m.m12 = r.m12 * getValue().y;
	m.m20 = r.m20 * getValue().z;
	m.m21 = r.m21 * getValue().z;
	m.m22 = r.m22 * getValue().z;

	node.setO2T (m);
}

ATOM_RotateKeyFrame::ATOM_RotateKeyFrame (void)
{
	setValue (ATOM_Vector3f(0.f, 0.f, 0.f));
}

ATOM_RotateKeyFrame::ATOM_RotateKeyFrame (const ATOM_Vector3f &r)
{
	setValue (r);
}

void ATOM_RotateKeyFrame::visit (ATOM_Node &node)
{
	ATOM_Vector3f s, t;
	ATOM_Matrix4x4f r;
	ATOM_Matrix4x4f m = node.getO2T();
	m.decompose (t, r, s);

	ATOM_Quaternion q;
	q.fromEulerXYZ (getValue());
	q.toMatrix (r);

	m.m00 = r.m00 * s.x;
	m.m01 = r.m01 * s.x;
	m.m02 = r.m02 * s.x;
	m.m10 = r.m10 * s.y;
	m.m11 = r.m11 * s.y;
	m.m12 = r.m12 * s.y;
	m.m20 = r.m20 * s.z;
	m.m21 = r.m21 * s.z;
	m.m22 = r.m22 * s.z;

	node.setO2T (m);
}

ATOM_AlphaKeyFrame::ATOM_AlphaKeyFrame (void)
{
	setValue (1.f);
}

ATOM_AlphaKeyFrame::ATOM_AlphaKeyFrame (float alpha)
{
	setValue (alpha);
}

void ATOM_AlphaKeyFrame::visit (ATOM_Geode &node)
{
	node.setTransparency (getValue());
}

void ATOM_AlphaKeyFrame::visit (ATOM_ParticleSystem &node)
{
}

