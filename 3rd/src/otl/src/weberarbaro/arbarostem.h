//  #**************************************************************************
//  #
//  #    $Id: arbarostem.h,v 1.4 2006/05/15 20:48:54 kirchdorfer Exp $  
//  #             - Stem class - here is most of the logic of 
//  #               the tree generating algorithm
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

#ifndef _WEBERARBARO_STEM_H_
#define _WEBERARBARO_STEM_H_

#include <opentree/utils/otarray.h>
#include <opentree/weber/webertypes.h>
#include <opentree/utils/otmatrix.h>

using namespace opentree;

namespace Arbaro
{
  class Tree;
  class Segment;
  class Leaf;

  /// \addtogroup Arbaro_Weber_Algorithm
  /// @{

  /**
  * A helper class for making 3d trees, this class makes a stem
  * (trunk or branch). Most of the generation algorithm is here.
  */
  class Stem 
  {
  private:
    Tree* tree;
    Stem* parent; // the parent stem
    Stem* clonedFrom; // the stem, from which this clone spreads out 

    otMatrix transf;

    static const float MIN_STEM_LEN; 
    static const float MIN_STEM_RADIUS; 

    float offset; // how far from the parent's base

    float length;
    float baseRadius;

    float lengthChildMax;
    float substemsPerSegment;
    float substemRotangle;

    float leavesPerSegment;
    float splitCorrection;

    float splitErrorValue, substemErrorValue, leavesErrorValue;

    bool pruneTest; // flag for pruning cycles

    /**
      * Make a clone of the stem at this position
      * 
      * @param trf The base transformation for the clone
      * @param start_segm Start segment number, i.e. the height, where
      *        the clone spreads out
      * @return The clone stem object
      */
    Stem* clone(otMatrix& trf, int start_segm);

    /**
      * Apply pruning to the stem. If it grows out of the 
      * pruning envelope, it is shortened.
      */
    void pruning();
    
    /**
      * Calcs stem length from parameters and parent length
      * 
      * @return the stem length
      */
    float stemLength();
    
    // makes the segments of the stem
    int makeSegments(int start_seg,int end_seg);
    
    /**
      * Tests if a point is inside the pruning envelope
      * 
      * @param vector the point to test
      * @return true if the point is inside the pruning envelope
      */
    bool isInsideEnvelope(otVector3 vector);	
    
    /**
      * Calcs a new direction for the current segment
      * 
      * @param trf The transformation of the previous segment
      * @param nsegm The number of the segment ( for testing, if it's the
      *              first stem segment
      * @return The new transformation of the current segment
      */
    void newDirection(otMatrix& trf, int nsegm);
    
    /**
      * Calcs the base radius of the stem
      * 
      * @return
      */
    float stemBaseRadius();
    
    /**
      * Precalcs some stem parameters used later during when generating
      * the current stem
      */
    void prepareSubstemParams();
    
    /**
      * Number of leaves of the stem
      * 
      * @return
      */
    float leavesPerBranch();
    
    /**
      * Make substems of the current stem
      * 
      * @param index
      * @param seg_trf
      * @param segment
      */
    void makeSubstems(int index, otMatrix seg_trf, Segment* segment);
    
    /**
      * Calcs the direction of a substem from the parameters
      * 
      * @param trf Current stem segment's trf becomes new trf
      * @param offset The offset of the substem from the base of the currents stem
      */
    void substemDirection(otMatrix& trf, float offset);
    
    /**
      * Creates the leaves for the current stem segment
      */
    void makeLeaves(int index, otMatrix seg_trf, Segment* segment);
    
    /**
      * Make clones of the current stem at the current segment
      * 
      * @param trf The current segments's direction
      * @param nseg The number of the current segment
      * @return Segments outside the pruning envelope, -1 if stem
      *                  clone is completely inside the envelope
      */
    int makeClones(otMatrix& trf,int nseg);
    
    /**
      * Gives a clone a new direction (splitting)
      * 
      * @param trf The base transformation of the clone becomes new transf
      * @param s_angle The splitting angle
      * @param nseg The segment number, where the clone begins
      * @param nsplits The number of clones
      */
    void split(otMatrix& trf, float s_angle, int nseg, int nsplits);
    
  public:
    TreeData& par;
    Level& lpar;
    float segmentLength;

    otArray<Segment*> segments; // the segments forming the stem
    otArray<Stem*> clones;      // the stem clones (for splitting)
    otArray<Stem*> substems;    // the substems
    otArray<Leaf*> leaves;      // the leaves

    int segmentCount;

    int stemlevel; // the branch level, could be > 4

    float getOffset() { return offset; }
    Stem* getParent() { return parent; }
    Stem* getClonedFrom() { return clonedFrom; }
    float getLength() { return length; }
    otMatrix getTransformation() { return transf; }

    /**
      * Creates a new stem
      * 
      * @param tr the tree object 
      * @param growsOutOf parent stem
      * @param stlev the stem level
      * @param trf the base transformation of the stem
      * @param offs the offset of ste stem within the parent stem (0..1)
      */
    Stem(Tree* tr, Stem* growsOutOf, int stlev, otMatrix trf, float offs);

    ~Stem();

    /**
      * Makes the stem, i.e. calculates all its segments, clones and substems
      * a.s.o. recursively
      */
    void make();

    /**
      * Calcs the stem radius at a given offset
      * 
      * @param h the offset at which the radius is calculated
      * @return the stem radius at this position
      */
    float stemRadius(float h);
    
    /**
      *  Returns the total number of all the substems and substems of substems a.s.o.
      *  of the current stem 
      *
      * @return totals number of susbstems and all theire children a.s.o
      */
    long substemTotal();
    
    long leafCount();
    
    float getShapeRatio(float ratio, int shape);

    /// local rotation about the x and z axees
    static inline void rotxzDegree(otMatrix& trf, float delta, float rho) 
    { 
      float sir = sin(rho*D2RAD);
      float cor = cos(rho*D2RAD);
      float sid = sin(delta*D2RAD);
      float cod = cos(delta*D2RAD);

      otMatrix m;

      m[0] = cor;
      m[4] = -sir*cod;
      m[8] = sir*sid;

      m[1] = sir;
      m[5] = cor*cod;
      m[9] = -cor*sid;

      m[2] = 0;
      m[6] = sid;
      m[10] = cod;

      trf.rot(m);
    }
  };
  /// }@
}

#endif //_WEBERARBARO_STEM_H_
