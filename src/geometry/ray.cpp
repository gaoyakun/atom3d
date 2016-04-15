#include "ray.h"

inline bool intersect_test_AABB_Ray ( const ATOM_Vector3f &center, 
                                      const ATOM_Vector3f &extends,
                                      const ATOM_Vector3f &origin,
                                      const ATOM_Vector3f &direction) 
{
  float WdU[3], AWdU[3], DdU[3], ADdU[3], AWxDdU[3];

  ATOM_Vector3f diff = origin - center;

  WdU[0] = direction.x;
  AWdU[0] = ATOM_abs(WdU[0]);
  DdU[0] = diff.x;
  ADdU[0] = ATOM_abs(DdU[0]);
  if ( ADdU[0] > extends.x && DdU[0] * WdU[0] >= 0.f )
    return false;

  WdU[1] = direction.y;
  AWdU[1] = ATOM_abs(WdU[1]);
  DdU[1] = diff.y;
  ADdU[1] = ATOM_abs(DdU[1]);
  if ( ADdU[1] > extends.y && DdU[1] * WdU[1] >= 0.f )
    return false;

  WdU[2] = direction.z;
  AWdU[2] = ATOM_abs(WdU[2]);
  DdU[2] = diff.z;
  ADdU[2] = ATOM_abs(DdU[2]);
  if ( ADdU[2] > extends.z && DdU[2] * WdU[2] >= 0.f )
    return false;

  ATOM_Vector3f WxD = crossProduct(direction, diff);

  AWxDdU[0] = ATOM_abs(WxD.x);
  if ( AWxDdU[0] > extends.y * AWdU[2] + extends.z * AWdU[1] )
    return false;

  AWxDdU[1] = ATOM_abs(WxD.y);
  if ( AWxDdU[1] > extends.x * AWdU[2] + extends.z * AWdU[0] )
    return false;

  AWxDdU[2] = ATOM_abs(WxD.z);
  if ( AWxDdU[2] > extends.x * AWdU[1] + extends.y * AWdU[0] )
    return false;

  return true;
}

ATOM_Ray::ATOM_Ray (void)
{
}

ATOM_Ray::ATOM_Ray (const ATOM_Vector3f &origin, const ATOM_Vector3f &dir)
{
	set (origin, dir);
}

void ATOM_Ray::set (const ATOM_Vector3f &origin, const ATOM_Vector3f &dir)
{
	float l = 1.f / dir.getLength ();

	x = origin.x;
	y = origin.y;
	z = origin.z;
	i = dir.x * l;
	j = dir.y * l;
	k = dir.z * l;

	ii = 1.0f / i;
	ij = 1.0f / j;
	ik = 1.0f / k;

	//ray slopes
	ibyj = i * ij;
	jbyi = j * ii;
	jbyk = j * ik;
	kbyj = k * ij;
	ibyk = i * ik;
	kbyi = k * ii;
	c_xy = y - jbyi * x;
	c_xz = z - kbyi * x;
	c_yx = x - ibyj * y;
	c_yz = z - kbyj * y;
	c_zx = x - ibyk * z;
	c_zy = y - jbyk * z;	

	//ray slope classification
	if(i < 0)
	{
		if(j < 0)
		{
			if(k < 0)
			{
				bboxIntersection = &ATOM_Ray::qtestMMM;
				bboxIntersectionEx = &ATOM_Ray::qtestMMMEx;
			}
			else if(k > 0){
				bboxIntersection = &ATOM_Ray::qtestMMP;
				bboxIntersectionEx = &ATOM_Ray::qtestMMPEx;
			}
			else//(k >= 0)
			{
				bboxIntersection = &ATOM_Ray::qtestMMO;
				bboxIntersectionEx = &ATOM_Ray::qtestMMOEx;
			}
		}
		else//(j >= 0)
		{
			if(k < 0)
			{
				bboxIntersection = (j > 0) ? &ATOM_Ray::qtestMPM : &ATOM_Ray::qtestMOM;
				bboxIntersectionEx = (j > 0) ? &ATOM_Ray::qtestMPMEx : &ATOM_Ray::qtestMOMEx;
			}
			else//(k >= 0)
			{
				if((j==0) && (k==0))
				{
					bboxIntersection = &ATOM_Ray::qtestMOO;	
					bboxIntersectionEx = &ATOM_Ray::qtestMOOEx;	
				}
				else if(k==0)
				{
					bboxIntersection = &ATOM_Ray::qtestMPO;
					bboxIntersectionEx = &ATOM_Ray::qtestMPOEx;
				}
				else if(j==0)
				{
					bboxIntersection = &ATOM_Ray::qtestMOP;
					bboxIntersectionEx = &ATOM_Ray::qtestMOPEx;
				}
				else
				{
					bboxIntersection = &ATOM_Ray::qtestMPP;
					bboxIntersectionEx = &ATOM_Ray::qtestMPPEx;
				}
			}
		}
	}
	else//(i >= 0)
	{
		if(j < 0)
		{
			if(k < 0)
			{
				bboxIntersection = (i > 0) ? &ATOM_Ray::qtestPMM : &ATOM_Ray::qtestOMM;
				bboxIntersectionEx = (i > 0) ? &ATOM_Ray::qtestPMMEx : &ATOM_Ray::qtestOMMEx;
			}
			else//(k >= 0)
			{				
				if((i==0) && (k==0))
				{
					bboxIntersection = &ATOM_Ray::qtestOMO;
					bboxIntersectionEx = &ATOM_Ray::qtestOMOEx;
				}
				else if(k==0)
				{
					bboxIntersection = &ATOM_Ray::qtestPMO;
					bboxIntersectionEx = &ATOM_Ray::qtestPMOEx;
				}
				else if(i==0)
				{
					bboxIntersection = &ATOM_Ray::qtestOMP;
					bboxIntersectionEx = &ATOM_Ray::qtestOMPEx;
				}
				else
				{
					bboxIntersection = &ATOM_Ray::qtestPMP;
					bboxIntersectionEx = &ATOM_Ray::qtestPMPEx;
				}
			}
		}
		else//(j >= 0)
		{
			if(k < 0)
			{
				if((i==0) && (j==0))
				{
					bboxIntersection = &ATOM_Ray::qtestOOM;
					bboxIntersectionEx = &ATOM_Ray::qtestOOMEx;
				}
				else if(i==0)
				{
					bboxIntersection = &ATOM_Ray::qtestOPM;
					bboxIntersectionEx = &ATOM_Ray::qtestOPMEx;
				}
				else if(j==0)
				{
					bboxIntersection = &ATOM_Ray::qtestPOM;
					bboxIntersectionEx = &ATOM_Ray::qtestPOMEx;
				}
				else
				{
					bboxIntersection = &ATOM_Ray::qtestPPM;
					bboxIntersectionEx = &ATOM_Ray::qtestPPMEx;
				}
			}
			else//(k > 0)
			{
				if(i==0)
				{
					if(j==0)
					{
						bboxIntersection = &ATOM_Ray::qtestOOP;
						bboxIntersectionEx = &ATOM_Ray::qtestOOPEx;
					}
					else if(k==0)
					{
						bboxIntersection = &ATOM_Ray::qtestOPO;
						bboxIntersectionEx = &ATOM_Ray::qtestOPOEx;
					}
					else
					{
						bboxIntersection = &ATOM_Ray::qtestOPP;
						bboxIntersectionEx = &ATOM_Ray::qtestOPPEx;
					}
				}
				else
				{
					if((j==0) && (k==0))
					{
						bboxIntersection = &ATOM_Ray::qtestPOO;
						bboxIntersectionEx = &ATOM_Ray::qtestPOOEx;
					}
					else if(j==0)
					{
						bboxIntersection = &ATOM_Ray::qtestPOP;
						bboxIntersectionEx = &ATOM_Ray::qtestPOPEx;
					}
					else if(k==0)
					{
						bboxIntersection = &ATOM_Ray::qtestPPO;
						bboxIntersectionEx = &ATOM_Ray::qtestPPOEx;
					}
					else
					{
						bboxIntersection = &ATOM_Ray::qtestPPP;
						bboxIntersectionEx = &ATOM_Ray::qtestPPPEx;
					}
				}
			}			
		}
	}
}

bool ATOM_Ray::qtestMMM (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if ((x < x0) || (y < y0) || (z < z0)
		|| (jbyi * x0 - y1 + c_xy > 0)
		|| (ibyj * y0 - x1 + c_yx > 0)
		|| (jbyk * z0 - y1 + c_zy > 0)
		|| (kbyj * y0 - z1 + c_yz > 0)
		|| (kbyi * x0 - z1 + c_xz > 0)
		|| (ibyk * z0 - x1 + c_zx > 0)
		)
		return false;
	
	return true;
}

bool ATOM_Ray::qtestMMMEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestMMM (bbox))
	{
		return false;
	}
	
	t = (bbox.getMax().x - x) * ii;
	float t1 = (bbox.getMax().y - y) * ij;
	if (t1 > t) t = t1;
	float t2 = (bbox.getMax().z - z) * ik;
	if (t2 > t) t = t2;

	return true;
}

bool ATOM_Ray::qtestMMP (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if ((x < x0) || (y < y0) || (z > z1)
		|| (jbyi * x0 - y1 + c_xy > 0)
		|| (ibyj * y0 - x1 + c_yx > 0)
		|| (jbyk * z1 - y1 + c_zy > 0)
		|| (kbyj * y0 - z0 + c_yz < 0)
		|| (kbyi * x0 - z0 + c_xz < 0)
		|| (ibyk * z1 - x1 + c_zx > 0)
		)
		return false;
	
	return true;
}

bool ATOM_Ray::qtestMMPEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestMMP (bbox))
	{
		return false;
	}

	t = (bbox.getMax().x - x) * ii;
	float t1 = (bbox.getMax().y - y) * ij;
	if (t1 > t) t = t1;
	float t2 = (bbox.getMin().z - z) * ik;
	if (t2 > t) t = t2;

	return true;
}

bool ATOM_Ray::qtestMPM (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if ((x < x0) || (y > y1) || (z < z0)
		|| (jbyi * x0 - y0 + c_xy < 0) 
		|| (ibyj * y1 - x1 + c_yx > 0)
		|| (jbyk * z0 - y0 + c_zy < 0) 
		|| (kbyj * y1 - z1 + c_yz > 0)
		|| (kbyi * x0 - z1 + c_xz > 0)
		|| (ibyk * z0 - x1 + c_zx > 0)
		)
		return false;
	
	return true;
}

bool ATOM_Ray::qtestMPMEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestMPM (bbox))
	{
		return false;
	}

	t = (bbox.getMax().x - x) * ii;
	float t1 = (bbox.getMin().y - y) * ij;
	if (t1 > t) t = t1;
	float t2 = (bbox.getMax().z - z) * ik;
	if (t2 > t) t = t2;
	
	return true;
}

bool ATOM_Ray::qtestMPP (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if ((x < x0) || (y > y1) || (z > z1)
		|| (jbyi * x0 - y0 + c_xy < 0) 
		|| (ibyj * y1 - x1 + c_yx > 0)
		|| (jbyk * z1 - y0 + c_zy < 0)
		|| (kbyj * y1 - z0 + c_yz < 0)
		|| (kbyi * x0 - z0 + c_xz < 0)
		|| (ibyk * z1 - x1 + c_zx > 0)
		)
		return false;
	
	return true;
}

bool ATOM_Ray::qtestMPPEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestMPP (bbox))
	{
		return false;
	}

	t = (bbox.getMax().x - x) * ii;
	float t1 = (bbox.getMin().y - y) * ij;
	if (t1 > t) t = t1;
	float t2 = (bbox.getMin().z - z) * ik;
	if (t2 > t) t = t2;

	return true;
}

bool ATOM_Ray::qtestPMM (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if ((x > x1) || (y < y0) || (z < z0)
		|| (jbyi * x1 - y1 + c_xy > 0)
		|| (ibyj * y0 - x0 + c_yx < 0)
		|| (jbyk * z0 - y1 + c_zy > 0)
		|| (kbyj * y0 - z1 + c_yz > 0)
		|| (kbyi * x1 - z1 + c_xz > 0)
		|| (ibyk * z0 - x0 + c_zx < 0)
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestPMMEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestPMM (bbox))
	{
		return false;
	}

	t = (bbox.getMin().x - x) * ii;
	float t1 = (bbox.getMax().y - y) * ij;
	if (t1 > t) t = t1;
	float t2 = (bbox.getMax().z - z) * ik;
	if (t2 > t) t = t2;

	return true;
}

bool ATOM_Ray::qtestPMP (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if ((x > x1) || (y < y0) || (z > z1)
		|| (jbyi * x1 - y1 + c_xy > 0)
		|| (ibyj * y0 - x0 + c_yx < 0)
		|| (jbyk * z1 - y1 + c_zy > 0)
		|| (kbyj * y0 - z0 + c_yz < 0)
		|| (kbyi * x1 - z0 + c_xz < 0)
		|| (ibyk * z1 - x0 + c_zx < 0)
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestPMPEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestPMP (bbox))
	{
		return false;
	}

	t = (bbox.getMin().x - x) * ii;
	float t1 = (bbox.getMax().y - y) * ij;
	if (t1 > t) t = t1;
	float t2 = (bbox.getMin().z - z) * ik;
	if (t2 > t) t = t2;

	return true;
}

bool ATOM_Ray::qtestPPM (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if ((x > x1) || (y > y1) || (z < z0)
		|| (jbyi * x1 - y0 + c_xy < 0)
		|| (ibyj * y1 - x0 + c_yx < 0)
		|| (jbyk * z0 - y0 + c_zy < 0) 
		|| (kbyj * y1 - z1 + c_yz > 0)
		|| (kbyi * x1 - z1 + c_xz > 0)
		|| (ibyk * z0 - x0 + c_zx < 0)
		)
		return false;
	
	return true;
}

bool ATOM_Ray::qtestPPMEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestPPM (bbox))
	{
		return false;
	}

	t = (bbox.getMin().x - x) * ii;
	float t1 = (bbox.getMin().y - y) * ij;
	if (t1 > t) t = t1;
	float t2 = (bbox.getMax().z - z) * ik;
	if (t2 > t) t = t2;
	
	return true;
}

bool ATOM_Ray::qtestPPP (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if ((x > x1) || (y > y1) || (z > z1)
		|| (jbyi * x1 - y0 + c_xy < 0)
		|| (ibyj * y1 - x0 + c_yx < 0)
		|| (jbyk * z1 - y0 + c_zy < 0)
		|| (kbyj * y1 - z0 + c_yz < 0)
		|| (kbyi * x1 - z0 + c_xz < 0)
		|| (ibyk * z1 - x0 + c_zx < 0)
		)
		return false;
	
	return true;
}

bool ATOM_Ray::qtestPPPEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestPPP (bbox))
	{
		return false;
	}

	t = (bbox.getMin().x - x) * ii;
	float t1 = (bbox.getMin().y - y) * ij;
	if (t1 > t) t = t1;
	float t2 = (bbox.getMin().z - z) * ik;
	if (t2 > t) t = t2;

	return true;
}

bool ATOM_Ray::qtestOMM (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if((x < x0) || (x > x1)
		|| (y < y0) || (z < z0)
		|| (jbyk * z0 - y1 + c_zy > 0)
		|| (kbyj * y0 - z1 + c_yz > 0)
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestOMMEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestOMM (bbox))
	{
		return false;
	}

	t = (bbox.getMax().y - y) * ij;
	float t2 = (bbox.getMax().z - z) * ik;
	if (t2 > t) t = t2;

	return true;
}

bool ATOM_Ray::qtestOMP (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if((x < x0) || (x > x1)
		|| (y < y0) || (z > z1)
		|| (jbyk * z1 - y1 + c_zy > 0)
		|| (kbyj * y0 - z0 + c_yz < 0)
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestOMPEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestOMP (bbox))
	{
		return false;
	}

	t = (bbox.getMax().y - y) * ij;
	float t2 = (bbox.getMin().z - z) * ik;
	if (t2 > t) t = t2;

	return true;
}

bool ATOM_Ray::qtestOPM (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if((x < x0) || (x > x1)
		|| (y > y1) || (z < z0)
		|| (jbyk * z0 - y0 + c_zy < 0) 
		|| (kbyj * y1 - z1 + c_yz > 0)
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestOPMEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestOPM (bbox))
	{
		return false;
	}

	t = (bbox.getMin().y - y) * ij;
	float t2 = (bbox.getMax().z - z) * ik;
	if (t2 > t) t = t2;

	return true;
}

bool ATOM_Ray::qtestOPP (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if((x < x0) || (x > x1)
		|| (y > y1) || (z > z1)
		|| (jbyk * z1 - y0 + c_zy < 0)
		|| (kbyj * y1 - z0 + c_yz < 0)
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestOPPEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestOPP (bbox))
	{
		return false;
	}

	t = (bbox.getMin().y - y) * ij;
	float t2 = (bbox.getMin().z - z) * ik;
	if (t2 > t) t = t2;

	return true;
}

bool ATOM_Ray::qtestMOM (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if((y < y0) || (y > y1)
		|| (x < x0) || (z < z0) 
		|| (kbyi * x0 - z1 + c_xz > 0)
		|| (ibyk * z0 - x1 + c_zx > 0)
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestMOMEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestMOM (bbox))
	{
		return false;
	}

	t = (bbox.getMax().x - x) * ii;
	float t2 = (bbox.getMax().z - z) * ik;
	if (t2 > t) t = t2;

	return true;
}

bool ATOM_Ray::qtestMOP (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if((y < y0) || (y > y1)
		|| (x < x0) || (z > z1) 
		|| (kbyi * x0 - z0 + c_xz < 0)
		|| (ibyk * z1 - x1 + c_zx > 0)
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestMOPEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestMOP (bbox))
	{
		return false;
	}

	t = (bbox.getMax().x - x) * ii;
	float t2 = (bbox.getMin().z - z) * ik;
	if (t2 > t) t = t2;

	return true;
}

bool ATOM_Ray::qtestPOM (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if((y < y0) || (y > y1)
		|| (x > x1) || (z < z0)
		|| (kbyi * x1 - z1 + c_xz > 0)
		|| (ibyk * z0 - x0 + c_zx < 0)
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestPOMEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestPOM (bbox))
	{
		return false;
	}

	t = (bbox.getMin().x - x) * ii;
	float t2 = (bbox.getMax().z - z) * ik;
	if (t2 > t) t = t2;

	return true;
}

bool ATOM_Ray::qtestPOP (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if((y < y0) || (y > y1)
		|| (x > x1) || (z > z1)
		|| (kbyi * x1 - z0 + c_xz < 0)
		|| (ibyk * z1 - x0 + c_zx < 0)
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestPOPEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestPOP (bbox))
	{
		return false;
	}

	t = (bbox.getMin().x - x) * ii;
	float t2 = (bbox.getMin().z - z) * ik;
	if (t2 > t) t = t2;

	return true;
}

bool ATOM_Ray::qtestMMO (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if((z < z0) || (z > z1)
		|| (x < x0) || (y < y0) 
		|| (jbyi * x0 - y1 + c_xy > 0)
		|| (ibyj * y0 - x1 + c_yx > 0)
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestMMOEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestMMO (bbox))
	{
		return false;
	}

	t = (bbox.getMax().x - x) * ii;
	float t2 = (bbox.getMax().y - y) * ij;
	if (t2 > t) t = t2;

	return true;
}

bool ATOM_Ray::qtestMPO (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if((z < z0) || (z > z1)
		|| (x < x0) || (y > y1) 
		|| (jbyi * x0 - y0 + c_xy < 0) 
		|| (ibyj * y1 - x1 + c_yx > 0)
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestMPOEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestMPO (bbox))
	{
		return false;
	}

	t = (bbox.getMax().x - x) * ii;
	float t2 = (bbox.getMin().y - y) * ij;
	if (t2 > t) t = t2;

	return true;
}

bool ATOM_Ray::qtestPMO (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if((z < z0) || (z > z1)
		|| (x > x1) || (y < y0) 
		|| (jbyi * x1 - y1 + c_xy > 0)
		|| (ibyj * y0 - x0 + c_yx < 0)  
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestPMOEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestPMO (bbox))
	{
		return false;
	}

	t = (bbox.getMin().x - x) * ii;
	float t2 = (bbox.getMax().y - y) * ij;
	if (t2 > t) t = t2;

	return true;
}

bool ATOM_Ray::qtestPPO (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if((z < z0) || (z > z1)
		|| (x > x1) || (y > y1)
		|| (jbyi * x1 - y0 + c_xy < 0)
		|| (ibyj * y1 - x0 + c_yx < 0)
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestPPOEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestPPO (bbox))
	{
		return false;
	}

	t = (bbox.getMin().x - x) * ii;
	float t2 = (bbox.getMin().y - y) * ij;
	if (t2 > t) t = t2;

	return true;
}

bool ATOM_Ray::qtestMOO (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	//float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if((x < x0)
		|| (y < y0) || (y > y1)
		|| (z < z0) || (z > z1)
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestMOOEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestMOO (bbox))
	{
		return false;
	}

	t = (bbox.getMax().x - x) * ii;

	return true;
}

bool ATOM_Ray::qtestPOO (const ATOM_BBox &bbox) const
{
	//float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if((x > x1)
		|| (y < y0) || (y > y1)
		|| (z < z0) || (z > z1)
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestPOOEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestPOO (bbox))
	{
		return false;
	}

	t = (bbox.getMin().x - x) * ii;

	return true;
}

bool ATOM_Ray::qtestOMO (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	//float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if((y < y0)
		|| (x < x0) || (x > x1)
		|| (z < z0) || (z > z1)
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestOMOEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestOMO (bbox))
	{
		return false;
	}

	t = (bbox.getMax().y - y) * ij;

	return true;
}

bool ATOM_Ray::qtestOPO (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	//float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if((y > y1)
		|| (x < x0) || (x > x1)
		|| (z < z0) || (z > z1)
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestOPOEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestOPO (bbox))
	{
		return false;
	}

	t = (bbox.getMin().y - y) * ij;

	return true;
}

bool ATOM_Ray::qtestOOM (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	//float z1 = bbox.getMax().z;

	if((z < z0)
		|| (x < x0) || (x > x1)
		|| (y < y0) || (y > y1)
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestOOMEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestOOM (bbox))
	{
		return false;
	}

	t = (bbox.getMax().z - z) * ik;

	return true;
}

bool ATOM_Ray::qtestOOP (const ATOM_BBox &bbox) const
{
	float x0 = bbox.getMin().x;
	float y0 = bbox.getMin().y;
	//float z0 = bbox.getMin().z;
	float x1 = bbox.getMax().x;
	float y1 = bbox.getMax().y;
	float z1 = bbox.getMax().z;

	if((z > z1)
		|| (x < x0) || (x > x1)
		|| (y < y0) || (y > y1)
		)
		return false;

	return true;
}

bool ATOM_Ray::qtestOOPEx (const ATOM_BBox &bbox, float &t) const
{
	if (!qtestOOP (bbox))
	{
		return false;
	}

	t = (bbox.getMin().z - z) * ik;

	return true;
}

ATOM_Vector3f ATOM_Ray::getOrigin (void) const
{
	return ATOM_Vector3f(x, y, z);
}

ATOM_Vector3f ATOM_Ray::getDirection (void) const
{
	return ATOM_Vector3f(i, j, k);
}

bool ATOM_Ray::intersectionTest (const ATOM_BBox &bbox) const 
{ 
	return (this->*bboxIntersection)(bbox);
}

bool ATOM_Ray::intersectionTestEx (const ATOM_BBox &bbox, float &t) const 
{ 
	return (this->*bboxIntersectionEx)(bbox, t);
}

bool ATOM_Ray::intersectionTest (const ATOM_Vector3f &v0, const ATOM_Vector3f &v1, const ATOM_Vector3f &v2, bool cull, float &u, float &v, float &d) const
{
	ATOM_Vector3f start(x, y, z);
	ATOM_Vector3f normal(i, j, k);

	ATOM_Vector3f edge1 = v1 - v0;
	ATOM_Vector3f edge2 = v2 - v0;
	ATOM_Vector3f pvec = crossProduct (normal, edge2);
	float det = dotProduct (edge1, pvec);

	if (!cull)
	{
		if (ATOM_equal(det, 0.f))
		{
			return false;
		}

		float inv_det = 1.0f / det;
		ATOM_Vector3f tvec = start - v0;
		u = inv_det * dotProduct (tvec, pvec);

		if (u < 0.f || u > 1.f)
		{
			return false;
		}

		ATOM_Vector3f qvec = crossProduct(tvec, edge1);
		v = inv_det * dotProduct (normal, qvec);

		if (v < 0.f || u + v > 1.f)
		{
			return false;
		}

		d = dotProduct (edge2, qvec) * inv_det;

		return true;
	}
	else
	{
		if (det < 0.f)
		{
			return false;
		}

		ATOM_Vector3f tvec = start - v0;
		u = dotProduct (tvec, pvec);
		if (u < 0.f || u > det) 
		{
			return false;
		}

		ATOM_Vector3f qvec = crossProduct (tvec, edge1);
		v = dotProduct (normal, qvec);
		if (v < 0.f || u + v > det)
		{
			return false;
		}

		d = dotProduct (edge2, qvec) / det;

		return true;
	}
}
