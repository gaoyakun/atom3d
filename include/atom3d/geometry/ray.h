#ifndef ATOM_GEOMETRY_RAY_H_
#define ATOM_GEOMETRY_RAY_H_

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_math.h"

class ATOM_Ray
{
	enum Classification
	{
		MMM,
		MMP,
		MPM,
		MPP,
		PMM,
		PMP,
		PPM,
		PPP,
		POO,
		MOO,
		OPO,
		OMO,
		OOP,
		OOM,
		OMM,
		OMP,
		OPM,
		OPP,
		MOM,
		MOP,
		POM,
		POP,
		MMO,
		MPO,
		PMO,
		PPO
	};

	float x, y, z;	// ray origin
	float i, j, k;  // ray direction
	float ii, ij, ik; // inverses of direction
	float ibyj, jbyi, kbyj, jbyk, ibyk, kbyi; // slopes
	float c_xy, c_xz, c_yx, c_yz, c_zx, c_zy; // precomputations
	int classification;

private:
	bool (ATOM_Ray::*bboxIntersection)(const ATOM_BBox &bbox) const;
	bool (ATOM_Ray::*bboxIntersectionEx)(const ATOM_BBox &bbox, float &t) const;

public:
	bool intersectionTest (const ATOM_BBox &bbox) const;
	bool intersectionTestEx (const ATOM_BBox &bbox, float &t) const;
	bool intersectionTest (const ATOM_Vector3f &v0, const ATOM_Vector3f &v1, const ATOM_Vector3f &v2, bool cull, float &u, float &v, float &d) const;

private:
	bool qtestMMM (const ATOM_BBox &bbox) const;
	bool qtestMMP (const ATOM_BBox &bbox) const;
	bool qtestMPM (const ATOM_BBox &bbox) const;
	bool qtestMPP (const ATOM_BBox &bbox) const;
	bool qtestPMM (const ATOM_BBox &bbox) const;
	bool qtestPMP (const ATOM_BBox &bbox) const;
	bool qtestPPM (const ATOM_BBox &bbox) const;
	bool qtestPPP (const ATOM_BBox &bbox) const;
	bool qtestPOO (const ATOM_BBox &bbox) const;
	bool qtestMOO (const ATOM_BBox &bbox) const;
	bool qtestOPO (const ATOM_BBox &bbox) const;
	bool qtestOMO (const ATOM_BBox &bbox) const;
	bool qtestOOP (const ATOM_BBox &bbox) const;
	bool qtestOOM (const ATOM_BBox &bbox) const;
	bool qtestOMM (const ATOM_BBox &bbox) const;
	bool qtestOMP (const ATOM_BBox &bbox) const;
	bool qtestOPM (const ATOM_BBox &bbox) const;
	bool qtestOPP (const ATOM_BBox &bbox) const;
	bool qtestMOM (const ATOM_BBox &bbox) const;
	bool qtestMOP (const ATOM_BBox &bbox) const;
	bool qtestPOM (const ATOM_BBox &bbox) const;
	bool qtestPOP (const ATOM_BBox &bbox) const;
	bool qtestMMO (const ATOM_BBox &bbox) const;
	bool qtestMPO (const ATOM_BBox &bbox) const;
	bool qtestPMO (const ATOM_BBox &bbox) const;
	bool qtestPPO (const ATOM_BBox &bbox) const;

	bool qtestMMMEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestMMPEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestMPMEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestMPPEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestPMMEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestPMPEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestPPMEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestPPPEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestPOOEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestMOOEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestOPOEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestOMOEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestOOPEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestOOMEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestOMMEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestOMPEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestOPMEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestOPPEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestMOMEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestMOPEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestPOMEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestPOPEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestMMOEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestMPOEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestPMOEx (const ATOM_BBox &bbox, float &t) const;
	bool qtestPPOEx (const ATOM_BBox &bbox, float &t) const;

public:
	ATOM_Ray (void);
	ATOM_Ray (const ATOM_Vector3f &origin, const ATOM_Vector3f &dir);
	void set (const ATOM_Vector3f &origin, const ATOM_Vector3f &dir);
	ATOM_Vector3f getOrigin (void) const;
	ATOM_Vector3f getDirection (void) const;
  };


#endif//ATOM_GEOMETRY_RAY_H_
