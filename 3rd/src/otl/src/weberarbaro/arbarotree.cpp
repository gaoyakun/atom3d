//  #**************************************************************************
//  #
//  #    $Id: arbarotree.cpp,v 1.2 2005/08/07 16:34:33 kirchdorfer Exp $  
//  #          - tree class - it generates the tree beginning from the trunk
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

#include "arbarotree.h"
#include "arbarostem.h"
#include "arbarosegment.h"
#include "arbaroleaf.h"

namespace Arbaro
{

  Tree::Tree(): trunk_rotangle(0) 
  {
  }

  Tree::~Tree()
  {
    trunks.delAll();
  }

  void Tree::make() 
  {
    // create the trunk and all its stems and leaves
    otMatrix trf;
    Level lpar = params.level[0];
    for (int i=0; i<lpar.branches; i++) 
    {
      trunkDirection(trf,lpar);
      float angle = calcVariance(360);
      float dist = calcVariance(params.trunk.dist);
      trf.translate(otVector3(dist*sin(angle), dist*cos(angle),0));
      Stem* trunk = new Stem(this,0,0,trf,0);
      trunks.add(trunk);
      //trunk->index=0;
      trunk->make();
    }
  }

  void Tree::trunkDirection(otMatrix& trf, Level& level) 
  {
    // get rotation angle
    float rotangle;

    if (level.rotate>=0) 
    { // rotating trunk
      trunk_rotangle = (trunk_rotangle + level.rotate+calcVariance(level.rotateV)+360);
      while (trunk_rotangle>=360.0) trunk_rotangle -= 360;
      while (trunk_rotangle<0.0) trunk_rotangle += 360;
      rotangle = trunk_rotangle;
    } 
    else 
    { // alternating trunks
      if (fabsf(trunk_rotangle) != 1) trunk_rotangle = 1;
      trunk_rotangle = -trunk_rotangle;
      rotangle = trunk_rotangle * (180+level.rotate+calcVariance(level.rotateV));
    }
    
    // get downangle
    float downangle;
    downangle = level.downAngle+calcVariance(level.downAngleV);
    
    Stem::rotxzDegree(trf,downangle,rotangle);
  }
}
