//  #**************************************************************************
//  #
//  #    $file$  - Leaf class - creation, rotation and output of leafs
//  #
//  #    Copyright (C) 2003  Wolfram Diestel
//  #    Copyright (C) 2005  Pascal Kirchdorfer (porting to c++)
//  #
//  #    This program is free software; you can redistribute it and/or modify
//  #    it under the terms of the GNU General Public License as published by
//  #    the Free Software Foundation; either version 2 of the License, or
//  #    (at your option) any later version.
//  #
//  #    This program is distributed in the hope that it will be useful,
//  #    but WITHOUT ANY WARRANTY; without even the implied warranty of
//  #    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  #    GNU General Public License for more details.
//  #
//  #    You should have received a copy of the GNU General Public License
//  #    along with this program; if not, write to the Free Software
//  #    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//  #
//  #    Send comments and bug fixes to diestel@steloj.de
//  #
//  #**************************************************************************/

#include "arbaroleaf.h"
#include <opentree/utils/otvector3.h>
#include <math.h>

namespace Arbaro
{

  void Leaf::setLeafOrientation() 
  {
    // FIXME: make this function as fast as possible - a tree has a lot of leafs
    
    // rotation outside 
    otVector3 pos = transf.getTranslation();

    // the z-vector of transf is parallel to the axis of the leaf, the y-vector
    // is the normal (of the upper side) of the leaf
    otVector3 norm = transf.getY();
    
    float bend_angle = params.leafBend;
    bend_angle *= (atan2(pos.Y,pos.X) - atan2(norm.Y,norm.X));
    
    // rotate about global z-axis
    transf.grotZRad(bend_angle);
    
    // rotation up
    norm = transf.getY();

    bend_angle = params.leafBend;
    bend_angle *= atan2(sqrt(norm.X*norm.X + norm.Y*norm.Y), norm.Z);

    transf.lrotXRad(bend_angle);
  }

  void Leaf::make() 
  {
    float quality = sqrt(params.leafQuality);
    length = params.leafScale/sqrt(quality);
    width = params.leafScale*params.leafScaleX/sqrt(quality);

    if (params.leafBend != 0) 
      setLeafOrientation();
  }
}
