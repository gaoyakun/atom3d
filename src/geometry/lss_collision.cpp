#include "lss_collision.h"

#define INT(a) ((unsigned&)a)

static bool checkPointInTriangle (const ATOM_Vector3f &point, const ATOM_Vector3f &pa, const ATOM_Vector3f &pb, const ATOM_Vector3f &pc)
{
	ATOM_Vector3f e10 = pb - pa;
	ATOM_Vector3f e20 = pc - pa;
	float a = dotProduct(e10, e10);
	float b = dotProduct(e10, e20);
	float c = dotProduct(e20, e20);
	float ac_bb = a * c - b * b;

	ATOM_Vector3f vp = point - pa;
	float d = dotProduct(vp, e10);
	float e = dotProduct(vp, e20);
	float x = d * c - e * b;
	float y = e * a - d * b;
	float z = x + y - ac_bb;

	return ((INT(z) & ~(INT(x)|INT(y))) & 0x80000000) != 0;
}

static bool getLowestRoot (float a, float b, float c, float maxR, float *root)
{
	float det = b * b - 4.f * a * c;

	if (det < 0.f)
	{
		return false;
	}

	float sqrtD = ATOM_sqrt(det);
	float r1 = (-b - sqrtD) / (2.f * a);
	float r2 = (-b + sqrtD) / (2.f * a);
	if (r1 > r2)
	{
		float tmp = r2;
		r2 = r1;
		r1 = tmp;
	}

	if (r1 > 0.f && r1 < maxR)
	{
		*root = r1;
		return true;
	}

	if (r2 > 0.f && r2 < maxR)
	{
		*root = r2;
		return true;
	}

	return false;
}

void LSS_Triangle_IntersectionTest (ATOM_LSSCollisionContext *colPackage, const ATOM_Vector3f &p1, const ATOM_Vector3f &p2, const ATOM_Vector3f &p3)
{
	ATOM_Plane plane (p1, p2, p3);

	float t0, t1;
	bool embeddedInPlane = false;

	float signedDistToTrianglePlane = plane.signedDistanceTo (colPackage->basePoint);
	float normalDotVelocity = dotProduct (plane.getNormal(), colPackage->velocity);

	if (ATOM_abs(normalDotVelocity) < 0.0001f)
	{
		if (ATOM_abs(signedDistToTrianglePlane) >= 1.f)
		{
			return;
		}
		else
		{
			embeddedInPlane = true;
			t0 = 0.f;
			t1 = 1.f;
		}
	}
	else
	{
		t0 = (-1.f - signedDistToTrianglePlane) / normalDotVelocity;
		t1 = ( 1.f - signedDistToTrianglePlane) / normalDotVelocity;
		if (t0 > t1)
		{
			float tmp = t0;
			t0 = t1;
			t1 = tmp;
		}

		if (t0 > 1.f || t1 < 0.f)
		{
			return;
		}

		t0 = ATOM_saturate (t0);
		t1 = ATOM_saturate (t1);
	}

	ATOM_Vector3f collisionPoint;
	bool foundCollision = false;
	float t = 1.f;

	if (!embeddedInPlane)
	{
		ATOM_Vector3f planeIntersectionPoint = (colPackage->basePoint - plane.getNormal()) + t0 * colPackage->velocity;

		if (checkPointInTriangle (planeIntersectionPoint, p1, p2, p3))
		{
			foundCollision = true;
			t = t0;
			collisionPoint = planeIntersectionPoint;
		}
	}

	if (!foundCollision)
	{
		ATOM_Vector3f velocity = colPackage->velocity;
		ATOM_Vector3f base = colPackage->basePoint;
		float velocitySquaredLength = velocity.getSquaredLength();
		float a, b, c;
		float newT;

		a = velocitySquaredLength;

		b = 2.f * (dotProduct (velocity, base - p1));
		c = (p1 - base).getSquaredLength() - 1.f;
		if (getLowestRoot (a, b, c, t, &newT))
		{
			t = newT;
			foundCollision = true;
			collisionPoint = p1;
		}

		b = 2.f * (dotProduct (velocity, base - p2));
		c = (p2 - base).getSquaredLength() - 1.f;
		if (getLowestRoot (a, b, c, t, &newT))
		{
			t = newT;
			foundCollision = true;
			collisionPoint = p2;
		}

		b = 2.f * (dotProduct (velocity, base - p3));
		c = (p3 - base).getSquaredLength() - 1.f;
		if (getLowestRoot (a, b, c, t, &newT))
		{
			t = newT;
			foundCollision = true;
			collisionPoint = p3;
		}

		ATOM_Vector3f edge = p2 - p1;
		ATOM_Vector3f baseToVertex = p1 - base;
		float edgeSquaredLength = edge.getSquaredLength();
		float edgeDotVelocity = dotProduct (edge, velocity);
		float edgeDotBaseToVertex = dotProduct (edge, baseToVertex);
		a = edgeSquaredLength * (-velocitySquaredLength) + edgeDotVelocity * edgeDotVelocity;
		b = edgeSquaredLength * (2.f * dotProduct(velocity, baseToVertex)) - 2.f * edgeDotVelocity * edgeDotBaseToVertex;
		c = edgeSquaredLength * (1.f - baseToVertex.getSquaredLength()) + edgeDotBaseToVertex * edgeDotBaseToVertex;
		if (getLowestRoot(a, b, c, t, &newT))
		{
			float f = (edgeDotVelocity * newT - edgeDotBaseToVertex) / edgeSquaredLength;
			if (f >= 0.f && f <= 1.f)
			{
				t = newT;
				foundCollision = true;
				collisionPoint = p1 + f * edge;
			}
		}

		edge = p3 - p2;
		baseToVertex = p2 - base;
		edgeSquaredLength = edge.getSquaredLength();
		edgeDotVelocity = dotProduct (edge, velocity);
		edgeDotBaseToVertex = dotProduct (edge, baseToVertex);
		a = edgeSquaredLength * (-velocitySquaredLength) + edgeDotVelocity * edgeDotVelocity;
		b = edgeSquaredLength * (2.f * dotProduct(velocity, baseToVertex)) - 2.f * edgeDotVelocity * edgeDotBaseToVertex;
		c = edgeSquaredLength * (1.f - baseToVertex.getSquaredLength()) + edgeDotBaseToVertex * edgeDotBaseToVertex;
		if (getLowestRoot(a, b, c, t, &newT))
		{
			float f = (edgeDotVelocity * newT - edgeDotBaseToVertex) / edgeSquaredLength;
			if (f >= 0.f && f <= 1.f)
			{
				t = newT;
				foundCollision = true;
				collisionPoint = p2 + f * edge;
			}
		}

		edge = p1 - p3;
		baseToVertex = p3 - base;
		edgeSquaredLength = edge.getSquaredLength();
		edgeDotVelocity = dotProduct (edge, velocity);
		edgeDotBaseToVertex = dotProduct (edge, baseToVertex);
		a = edgeSquaredLength * (-velocitySquaredLength) + edgeDotVelocity * edgeDotVelocity;
		b = edgeSquaredLength * (2.f * dotProduct(velocity, baseToVertex)) - 2.f * edgeDotVelocity * edgeDotBaseToVertex;
		c = edgeSquaredLength * (1.f - baseToVertex.getSquaredLength()) + edgeDotBaseToVertex * edgeDotBaseToVertex;
		if (getLowestRoot(a, b, c, t, &newT))
		{
			float f = (edgeDotVelocity * newT - edgeDotBaseToVertex) / edgeSquaredLength;
			if (f >= 0.f && f <= 1.f)
			{
				t = newT;
				foundCollision = true;
				collisionPoint = p3 + f * edge;
			}
		}
	}

	if (foundCollision)
	{
		float distToCollision = t * colPackage->velocity.getLength ();

		if (!colPackage->foundCollision || distToCollision < colPackage->nearestDistance)
		{
			colPackage->nearestDistance = distToCollision;
			colPackage->intersectionPoint = collisionPoint;
			colPackage->foundCollision = true;
		}
	}
}

