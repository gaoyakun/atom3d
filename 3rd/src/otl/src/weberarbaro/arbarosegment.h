//  #**************************************************************************
//  #
//  #    $Id: arbarosegment.h,v 1.1 2005/08/07 15:44:00 kirchdorfer Exp $  
//  #            - Segment class - a segment is a part of a stem
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

#include <opentree/utils/otarray.h>
#include <opentree/utils/otmatrix.h>
#include <opentree/weber/webertypes.h>

using namespace opentree;

class Stem;

namespace Arbaro
{

  /// \addtogroup Arbaro_Weber_Algorithm
  /// @{

  /**
  * A segment class, multiple segments form a stem.
  */
  class Segment 
  {
  private:
    //otMatrix transf;
    otVector3 pos;
    float rad;

  public:
    
    Segment(otMatrix& trf, float r)
    {
      //transf = trf; 
      pos = trf.getTranslation();
      rad = r;
    }

    ~Segment() 
    {
    }

    float getRad()
    { 
      return rad; 
    }

    otVector3 getTranslation()
    {
      //return transf.getTranslation();
      return pos; 
    }
  };
  /// }@
}
