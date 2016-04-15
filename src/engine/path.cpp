#include "StdAfx.h"
#include "path.h"

ATOM_Path3D::ATOM_Path3D (void)
{
	_radius = 1.f;
	_smoothness = 3.f;
	_worldMatrix.makeIdentity();
	_pathDirty = false;
}

ATOM_Path3D::ATOM_Path3D (const ATOM_Path3D &rhs)
{
	_radius = rhs._radius;
	_smoothness = rhs._smoothness;
	_ctlPoints = rhs._ctlPoints;
	_worldMatrix = rhs._worldMatrix;
	_pathDirty = true;
}

ATOM_Path3D::~ATOM_Path3D (void)
{
}

ATOM_Path3D & ATOM_Path3D::operator = (const ATOM_Path3D &rhs)
{
	if (&rhs != this)
	{
		_radius = rhs._radius;
		_smoothness = rhs._smoothness;
		_ctlPoints = rhs._ctlPoints;
		_worldMatrix = rhs._worldMatrix;
		_pathDirty = true;
	}
	return *this;
}

void ATOM_Path3D::setRoundRadius (float radius)
{
	if (_radius != radius)
	{
		_radius = radius;
		_pathDirty = true;
	}
}

float ATOM_Path3D::getRoundRadius (void) const
{
	return _radius;
}

void ATOM_Path3D::setSmoothness (int smoothness)
{
	if (_smoothness != smoothness)
	{
		_smoothness = smoothness;
		_pathDirty = true;
	}
}

int ATOM_Path3D::getSmoothness (void) const
{
	return _smoothness;
}

void ATOM_Path3D::setWorldMatrix (const ATOM_Matrix4x4f &matrix)
{
	_worldMatrix = matrix;
}

const ATOM_Matrix4x4f &ATOM_Path3D::getWorldMatrix (void) const
{
	return _worldMatrix;
}

int ATOM_Path3D::getNumControlPoints (void) const
{
	return _ctlPoints.size();
}

void ATOM_Path3D::insertControlPoint (int point, const ATOM_Matrix4x4f &m)
{
	_ctlPoints.insert (_ctlPoints.begin()+point, m);
	_pathDirty = true;
}

void ATOM_Path3D::appendControlPoint (const ATOM_Matrix4x4f &m)
{
	_ctlPoints.push_back (m);
	_pathDirty = true;
}

void ATOM_Path3D::deleteControlPoint (int point)
{
	_ctlPoints.erase (_ctlPoints.begin()+point);
	_pathDirty = true;
}

void ATOM_Path3D::clearControlPoints (void)
{
	_ctlPoints.resize (0);
	_pathDirty = true;
}

const ATOM_Matrix4x4f &ATOM_Path3D::getControlPoint (int point) const
{
	return _ctlPoints[point];
}

void ATOM_Path3D::setControlPoint (int point, const ATOM_Matrix4x4f &m)
{
	_ctlPoints[point] = m;
	_pathDirty = true;
}

int ATOM_Path3D::getNumPathPoints (void) const
{
	if (_pathDirty)
	{
		_pathDirty = false;
		updatePathPoints ();
	}
	return _pathPoints.size();
}

const ATOM_Matrix4x4f &ATOM_Path3D::getPathPoint (int point) const
{
	if (_pathDirty)
	{
		_pathDirty = false;
		updatePathPoints ();
	}
	return _pathPoints[point];
}

static void interpolateMatrix (const ATOM_Matrix4x4f &m1, const ATOM_Matrix4x4f &m2, float factor, ATOM_Matrix4x4f *out)
{
	ATOM_Vector3f eye1, to1, up1;
	m1.decomposeLookatLH (eye1, to1, up1);

	ATOM_Vector3f eye2, to2, up2;
	m2.decomposeLookatLH (eye2, to2, up2);

	ATOM_Vector3f eye = eye1 + factor * (eye2 - eye1);
	ATOM_Vector3f to = to1 + factor * (to2 - to1);
	ATOM_Vector3f up = up1 + factor * (up2 - up1);
	out->makeLookatLH (eye, to, up);
}

void ATOM_Path3D::getPathTangentByTime (float time, ATOM_Vector3f *tangent) const
{
	if (_pathDirty)
	{
		_pathDirty = false;
		updatePathPoints ();
	}

	time = ATOM_saturate(time);
	float len = _pathSegmentLength.back() * time;
	for (int i = 0; i < _pathPoints.size(); ++i)
	{
		if (len < _pathSegmentLength[i])
		{
			float factor = (len - _pathSegmentLength[i-1])/(_pathSegmentLength[i] - _pathSegmentLength[i-1]);
			*tangent = _pathSegmentTangents[i-1] + factor * (_pathSegmentTangents[i] - _pathSegmentTangents[i-1]);
			return;
		}
	}
	*tangent = _pathSegmentTangents.back();
}

void ATOM_Path3D::getPathPointByTime (float time, ATOM_Matrix4x4f *matrix) const
{
	if (_pathDirty)
	{
		_pathDirty = false;
		updatePathPoints ();
	}

	time = ATOM_saturate(time);
	float len = _pathSegmentLength.back() * time;
	for (int i = 0; i < _pathPoints.size(); ++i)
	{
		if (len < _pathSegmentLength[i])
		{
			if (i == 0)
			{
				*matrix = _pathPoints[0];
			}
			else
			{
				const ATOM_Matrix4x4f &m1 = _pathPoints[i-1];
				const ATOM_Matrix4x4f &m2 = _pathPoints[i];
				float factor = (len - _pathSegmentLength[i-1])/(_pathSegmentLength[i] - _pathSegmentLength[i-1]);
				interpolateMatrix (m1, m2, factor, matrix);
			}
			return;
		}
	}
	*matrix = _pathPoints.back ();
}

void ATOM_Path3D::updatePathPoints (void) const
{
	_pathPoints.resize (0);
	_pathSegmentLength.resize (0);
	_pathSegmentTangents.resize (0);

	if (_smoothness == 0)
	{
		_pathPoints.resize (_ctlPoints.size());
		_pathSegmentLength.resize (_ctlPoints.size());
		_pathSegmentTangents.resize (_ctlPoints.size());

		for (int i = 0; i < _ctlPoints.size(); ++i)
		{
			_pathPoints[i] = _ctlPoints[i];

			if (i > 0)
			{
				_pathSegmentLength[i] = _pathSegmentLength[i-1] + (_pathPoints[i].getRow3(3) - _pathPoints[i-1].getRow3(3)).getLength();
			}
			else
			{
				_pathSegmentLength[i] = 0.f;
			}
		}

		for (int i = 0; i < _pathPoints.size(); ++i)
		{
			if (i < _ctlPoints.size()-1)
			{
				_pathSegmentTangents[i] = _pathPoints[i+1].getRow3(3) - _pathPoints[i-1].getRow3(3);
			}
			else
			{
				_pathSegmentTangents[i] = _pathSegmentTangents[i-1];
			}
		}
	}
	else
	{
		ATOM_VECTOR<ATOM_Vector3f> vecPos[2];
		ATOM_VECTOR<ATOM_Vector3f> vecLook[2];
		int current = 0;
		float radius = ATOM_saturate(_radius) / 3.f;

		for (int i = 0; i < _ctlPoints.size(); ++i)
		{
			const ATOM_Matrix4x4f &m = _ctlPoints[i];
			ATOM_Vector3f eye, to , up;
			m.decomposeLookatLH (eye, to, up);
			vecPos[current].push_back (eye);
			vecLook[current].push_back (to - eye);
		}

		for (int smoothness = 0; smoothness < _smoothness; ++smoothness)
		{
			int next = 1 - current;
			vecPos[next].resize (0);
			vecLook[next].resize (0);

			vecPos[next].push_back (vecPos[current][0]);
			vecLook[next].push_back (vecLook[current][0]);
			for (int n = 1; n < vecPos[current].size()-1; ++n)
			{
				ATOM_Vector3f vPrev = vecPos[current][n-1];
				ATOM_Vector3f vThis = vecPos[current][n];
				ATOM_Vector3f vNext = vecPos[current][n+1];
				ATOM_Vector3f v1 = vThis + (vPrev - vThis) * radius;
				ATOM_Vector3f v2 = vThis + (vNext - vThis) * radius;
				vecPos[next].push_back (v1);
				vecPos[next].push_back (v2);

				ATOM_Vector3f lPrev = vecLook[current][n-1];
				ATOM_Vector3f lThis = vecLook[current][n];
				ATOM_Vector3f lNext = vecLook[current][n+1];
				ATOM_Vector3f l1 = lThis + (lPrev - lThis) * radius;
				ATOM_Vector3f l2 = lThis + (lNext - lThis) * radius;
				vecLook[next].push_back (l1);
				vecLook[next].push_back (l2);
			}
			vecPos[next].push_back (vecPos[current].back());
			vecLook[next].push_back (vecLook[current].back());
			current = next;
		}

		_pathPoints.resize (vecPos[current].size());
		_pathSegmentLength.resize (vecPos[current].size());
		_pathSegmentTangents.resize (vecPos[current].size());
		for (int i = 0; i < vecPos[current].size(); ++i)
		{
			_pathPoints[i] = ATOM_Matrix4x4f::getLookatLHMatrix (vecPos[current][i], vecPos[current][i]+vecLook[current][i], ATOM_Vector3f(0.f, 1.f, 0.f));

			if (i > 0)
			{
				_pathSegmentLength[i] = _pathSegmentLength[i-1] + (_pathPoints[i].getRow3(3) - _pathPoints[i-1].getRow3(3)).getLength();
			}
			else
			{
				_pathSegmentLength[i] = 0.f;
			}
		}
		for (int i = 0; i < _pathPoints.size(); ++i)
		{
			if (i < _pathPoints.size()-1)
			{
				_pathSegmentTangents[i] = _pathPoints[i+1].getRow3(3) - _pathPoints[i].getRow3(3);
			}
			else
			{
				_pathSegmentTangents[i] = _pathSegmentTangents[i-1];
			}
		}
	}

	for (int i = 0; i < _pathSegmentTangents.size(); ++i)
	{
		for (int j=i+1; _pathSegmentTangents[i].getLength()<0.001f && j<_pathSegmentTangents.size()-1; ++j)
		{
			_pathSegmentTangents[i] += _pathSegmentTangents[j];
		}

		if (i > 0 && _pathSegmentTangents[i].getLength()<0.001f)
		{
			_pathSegmentTangents[i] = _pathSegmentTangents[i-1];
		}
	}
}
