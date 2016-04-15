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

#ifndef _OT_OTCOMPARE_H_
#define _OT_OTCOMPARE_H_

#include "opentree/ottree.h"
#include "opentree/otstem.h"
#include "opentree/otleaf.h"

namespace opentree
{
  /// \addtogroup Utilities
  /// @{

  /** Compares Trees
   * This class allows the comparision of two different otTree classes.
   * It compares all stems and leaves in order to evaluate the equivality of the trees.
   */
  class otCompare
  {
  private:
    bool compareTree(otTree* tree1, otTree* tree2)
    {
      if (tree1->getTrunkCount() != tree2->getTrunkCount())
        return false;

      for (unsigned int i = 0; i < tree1->getTrunkCount(); i++)
      {
        if (!compareStem(tree1->getTrunk(i), tree2->getTrunk(i)))
          return false;
      }

      return true;
    }

    bool compareStem(otStem* stem1, otStem* stem2)
    {
      if (stem1->getChildCount() != stem2->getChildCount())
        return false;

      if (fabs(stem1->getPosSpline()->getLength() - stem2->getPosSpline()->getLength()) > 0.001f)
        return false;

      for (float i = 0.0f; i<=1.0f; i+=0.1f)
      {
        if (!(stem1->getPosSpline()->getPoint(i) == stem2->getPosSpline()->getPoint(i)))
          return false;
        if (stem1->getRadSpline()->getPoint(i) != stem2->getRadSpline()->getPoint(i))
          return false;
      }

      if (stem1->getLeafCount() != stem2->getLeafCount())
        return false;

      for (unsigned int i = 0; i < stem1->getLeafCount(); i++)
      {
        if (!compareLeaf(stem1->getLeaf(i), stem2->getLeaf(i)))
          return false;
      }

      for (unsigned int i = 0; i < stem1->getChildCount(); i++)
      {
        if (!compareStem(stem1->getChild(i), stem2->getChild(i)))
          return false;
      }

      return true;
    }

    bool compareLeaf(otLeaf& leaf1, otLeaf& leaf2)
    {
      if (leaf1.length != leaf2.length)
        return false;

      if (leaf1.width != leaf2.width)
        return false;

      if (!(leaf1.transform == leaf2.transform))
        return false;

      return true;
    }

  public:
    otCompare() {}
    ~otCompare() {}

    /// Compares two trees against each other.
    bool compare(otTree* tree1, otTree* tree2)
    {
      return compareTree(tree1, tree2);
    }
  };

  /// @}
}

#endif //_OT_OTCOMPARE_H_
