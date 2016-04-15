#ifndef __ATOM3D_GEOMETRY_LSS_COLLISION_H
#define __ATOM3D_GEOMETRY_LSS_COLLISION_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"

class ATOM_LSSCollisionContext
{
public:
	ATOM_Vector3f radius;
	ATOM_Vector3f velocityR3;
	ATOM_Vector3f positionR3;

	ATOM_Vector3f velocity;
	ATOM_Vector3f normalizedVelocity;
	ATOM_Vector3f basePoint;

	bool foundCollision;
	float nearestDistance;
	ATOM_Vector3f intersectionPoint;
};

void LSS_Triangle_IntersectionTest (ATOM_LSSCollisionContext *colPackage, const ATOM_Vector3f &p1, const ATOM_Vector3f &p2, const ATOM_Vector3f &p3);

#endif // __ATOM3D_GEOMETRY_LSS_COLLISION_H
