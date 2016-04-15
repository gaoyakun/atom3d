#ifndef __ATOM_MATH_FRUSTUM3D_H
#define __ATOM_MATH_FRUSTUM3D_H

#include "plane.h"

class ATOM_Frustum3d
{
  public:
    enum
    {
        RIGHT	  = 0,		// The RIGHT side of the frustum
        LEFT	  = 1,		// The LEFT	 side of the frustum
        BOTTOM	= 2,		// The BOTTOM side of the frustum
        TOP		  = 3,		// The TOP side of the frustum
        BACK	  = 4,		// The BACK	side of the frustum (far plane)
        FRONT   = 5			// The FRONT side of the frustum (near plane)
    }; 

public:
    const ATOM_Plane & operator [] (int index) const;
          ATOM_Plane & operator [] (int index);
    const ATOM_Plane * planes (void) const;
          ATOM_Plane * planes (void);
    void  fromMatrix  (const ATOM_Matrix4x4f &matMVP);
                                          
private:
    ATOM_Plane _planes[6];
};

#include "frustum3d.inl"

#endif // __ATOM_MATH_FRUSTUM3D_H
