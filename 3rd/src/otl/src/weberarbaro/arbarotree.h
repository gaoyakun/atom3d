//  #**************************************************************************
//  #
//  #    $Id: arbarotree.h,v 1.2 2005/08/07 16:34:33 kirchdorfer Exp $  
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

#ifndef _WEBERARBARO_TREE_H_
#define _WEBERARBARO_TREE_H_

#include <opentree/weber/webertypes.h>
#include <opentree/utils/otarray.h>
#include <opentree/utils/otmatrix.h>

using namespace opentree;

#include "arbarostem.h"

namespace Arbaro
{
  /// \addtogroup Arbaro_Weber_Algorithm
  /// @{

  /**
  * A class for creation of threedimensional tree objects.
  * A tree has one or more trunks, with several levels of
  * branches all of which are instances of the Stem class.
  */
  class Tree 
  {
  private:
    // the trunks (one for trees, many for bushes)
    float trunk_rotangle;

    /**
      * Calcs the trunk direction. Of special importance for plants with
      * multiple trunks.
      */
    void trunkDirection(otMatrix& trf, Level& lpar);

  public:
    otArray<Stem*> trunks;

    TreeData params;
    
    Tree();
    
    ~Tree();
    
    void make();

    int getLeafCount()
    {
      if (params.leaves == 0) return 0;
      
      int leafCount = 0;
      for (unsigned int t=0; t<trunks.getCount(); t++) 
      {
        leafCount += trunks.get(t)->leafCount();
      }
      return leafCount;
    }
    
    int getStemCount()
    {
      int stemCount = trunks.getCount();
      for (unsigned int t=0; t<trunks.getCount(); t++) 
      {
        stemCount += trunks.get(t)->substemTotal();
      }
      return stemCount;
    }
  };
  /// }@
}

#endif //_WEBERARBARO_TREE_H_
