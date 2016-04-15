//$Id: n3_sggeometry.h 1736 2008-12-15 07:59:49Z gyk $

#ifndef ATOM_GEOMETRY_H_
#define ATOM_GEOMETRY_H_

#if _MSC_VER > 1000
# pragma once
#endif

#if _MSC_VER
#pragma warning(disable:4251)
#endif

#include "ATOM_basedefs.h"

#include "geometry/transform.h"
#include "geometry/striper.h"
#include "geometry/frustum2d.h"
#include "geometry/radixsort.h"

#include "geometry/convexhull3d.h"
#include "geometry/collisionobject.h"
#include "geometry/lss_collision.h"

#include "geometry/misc.h"
#include "geometry/ray.h"
#include "geometry/aabbtree.h"
#include "geometry/aabbtreebuilder.h"
#include "geometry/vcache.h"

#include "geometry/NVMeshMender.h"

#endif//ATOM_GEOMETRY_H_
