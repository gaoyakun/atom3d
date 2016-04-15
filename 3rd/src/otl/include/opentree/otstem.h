/*
  Copyright (c) 2004 Pascal Kirchdorfer, Seth Yastrov

  This software is provided 'as-is', without any express or implied warranty. In
  no event will the authors be held liable for any damages arising from the use
  of this software.

  Permission is granted to anyone to use this software for any purpose, including
  commercial applications, and to alter it and redistribute it freely, subject to
  the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim
       that you wrote the original software. If you use this software in a product,
       an acknowledgment in the product documentation would be appreciated but is
       not required.

    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/

#ifndef _OT_STEM_H_
#define _OT_STEM_H_

#include "opentree.h"

#include "opentree/utils/otarray.h"
#include "opentree/utils/spline.h"

#include "ottree.h"

namespace opentree
{
  class otLeaf;
  class otVector3;

  /// \addtogroup Tree_Structure
  /// @{

  /** Stem class
   * Defines one Stem and contains all substems and leaves of this stem.
   */
  class otStem
  {
  private:
    /// Array of all child stems (incl. clones)
    otArray<otStem*> children;
    /// Array of all leaves of this stem.
    otArray<otLeaf> leaves;

    /// Spline defining the stem curve
    iSpline3D* stem_spline;
    /// Spline defining the stem radius
    iSpline1D* radius_spline;

  public:
    /// Pointer to the tree object.
    otTree* tree;
    /// Pointer to the parent stem.
    otStem* parent;
    /// Level Number: Trunk = 0, first child = 1, etc.
    int levelNo;
    /// Position along parent stem curve spline where child split off
    float offset;

  public:
    /** Creates a stem.
     * \param levelNo Level Number: Trunk = 0, first child = 1, etc.
     * \param offset  Position along parent stem curve spline where child 
     *                      split off
     */
    otStem(int levelNo, float offset)
    : parent(0), levelNo(levelNo), offset(offset)
    {
    }

    /** Creates a stem.
     * \param levelNo Level Number: Trunk = 0, first child = 1, etc.
     * \param child_count number of points going to be added to the splines.
     *                      (used to initialize the array => increase speed)
     * \param leaf_count number of leaves going to be added to the splines.
     *                      (used to initialize the array => increase speed)
     * \param offset Position along parent stem curve spline where child split
     *                       off
     */
    otStem(int levelNo, unsigned int leaf_count, unsigned int child_count,
       float offset)
    : children(child_count), leaves(leaf_count), parent(0), levelNo(levelNo),
      offset(offset)
    {
    }


    /** Destructs the tree
     * Please note that all stems and leaves of the tree will be deleted!
     */
    virtual ~otStem()
    {
      children.delAll();
      //leaves.delAll();
    }

    /// Returns a pointer to the position spline.
    iSpline3D* getPosSpline()
    {
      return stem_spline;
    }

    /// Returns a pointer to the position spline.
    void setPosSpline(iSpline3D* s)
    {
      stem_spline = s;
    }

    /// Returns a pointer to the radius spline.
    iSpline1D* getRadSpline()
    {
      return radius_spline;
    }

    /// Returns a pointer to the radius spline.
    void setRadSpline(iSpline1D* s)
    {
      radius_spline = s;
    }

    /// Adds a new Leaf to the stem.
    void addLeaf(otLeaf& leaf)
    {
      tree->addLeaf(leaf);
      leaves.add(leaf);
    }

    /// Returns the number of leaves.
    unsigned int getLeafCount()
    {
      return leaves.getCount();
    }

    /// Returns Leaf number #i from the stem.
    otLeaf& getLeaf(unsigned int i)
    {
      return leaves.get(i);
    }

    /// Adds a new Childstems to the stem.
    void addChild(otStem* stem)
    {
      stem->tree = tree;
      stem->parent = this;
      children.add(stem);
    }

    /// Returns the number of Childstems.
    unsigned int getChildCount()
    {
      return children.getCount();
    }

    /// Returns Childstem number #i from the stem.
    otStem* getChild(unsigned int i)
    {
      return children.get(i);
    }
  };

  /// @}

}

#endif //_OT_STEM_H_
