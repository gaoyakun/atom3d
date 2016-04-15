#ifndef __ATOM3D_ENGINE_PATH_H
#define __ATOM3D_ENGINE_PATH_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

class ATOM_ENGINE_API ATOM_Path3D
{
public:
	ATOM_Path3D (void);
	ATOM_Path3D (const ATOM_Path3D &rhs);
	virtual ~ATOM_Path3D (void);

public:
	ATOM_Path3D & operator = (const ATOM_Path3D &rhs);
	void setRoundRadius (float radius);
	float getRoundRadius (void) const;
	void setSmoothness (int smoothness);
	int getSmoothness (void) const;
	void setWorldMatrix (const ATOM_Matrix4x4f &matrix);
	const ATOM_Matrix4x4f &getWorldMatrix (void) const;
	int getNumControlPoints (void) const;
	void insertControlPoint (int point, const ATOM_Matrix4x4f &m);
	void appendControlPoint (const ATOM_Matrix4x4f &m);
	void deleteControlPoint (int point);
	void clearControlPoints (void);
	const ATOM_Matrix4x4f &getControlPoint (int point) const;
	void setControlPoint (int point, const ATOM_Matrix4x4f &m);

public:
	int getNumPathPoints (void) const;
	const ATOM_Matrix4x4f &getPathPoint (int point) const;
	float getPathLength (void) const;
	void getPathPointByTime (float time, ATOM_Matrix4x4f *matrix) const;
	void getPathTangentByTime (float time, ATOM_Vector3f *tangent) const;
	void getWorldPathPointByTime (float time, ATOM_Matrix4x4f *matrix) const;
	void updatePathPoints (void) const;

private:
	float _radius;
	int _smoothness;
	ATOM_Matrix4x4f _worldMatrix;
	ATOM_VECTOR<ATOM_Matrix4x4f> _ctlPoints;
	mutable ATOM_VECTOR<ATOM_Matrix4x4f> _pathPoints;
	mutable ATOM_VECTOR<float> _pathSegmentLength;
	mutable ATOM_VECTOR<ATOM_Vector3f> _pathSegmentTangents;
	mutable bool _pathDirty;
};

#endif // __ATOM3D_ENGINE_PATH_H
