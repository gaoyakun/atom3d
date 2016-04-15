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

#ifndef _OT_TREE_H_
#define _OT_TREE_H_

#include "otleaf.h"

#include "opentree/opentree.h"
#include "opentree/utils/otarray.h"

namespace opentree
{
  /** \addtogroup Tree_Structure
   *  Classes used to define a tree.
   */

  /// @{

  class otStem;

  /** Tree class
   * Contains all stems and leaves which define the tree.
   */
  class otTree
  {
  private:
    /// Array of all trunks
    otArray<otStem*> trunks;

    /// Array of all leaves (same as parsing all stems)
    otArray<otLeaf> leaves;

  public:
    /** Creates an empty tree
     * \param trunks Defines how may trunks will be added. 
     *               (Initializes array in advance => faster)
     * \param leaves Defines how may leaves will be added. 
     *               (Initializes array in advance => faster)
     */
    otTree(unsigned int trunks = 0, unsigned int leaves = 0)
    : trunks(trunks), leaves(leaves)
    {
    }

    /** Destructs the tree
     * Please note that all stems and leaves of the tree will be deleted!
     */
    virtual ~otTree() 
    {
      trunks.delAll();
    }

    /// Returns the number of trunks added.
    unsigned int getTrunkCount()
    {
      return trunks.getCount();
    }

    /// Returns a trunk identified by the index in the array.
    otStem* getTrunk(unsigned int i)
    {
      return trunks.get(i);
    }

    /// Adds a new stem to the list of trunks.
    void addTrunk(otStem* stem);

    /// Adds a new Leaf to the tree.
    /// Don't call that if you added the leaf to a stem already!
    void addLeaf(otLeaf& leaf)
    {
      leaves.add(leaf);
    }

    /// Returns the number of leaves.
    unsigned int getLeafCount()
    {
      return leaves.getCount();
    }

    /// Returns Leaf number #i from the tree.
    otLeaf& getLeaf(unsigned int i)
    {
      return leaves.get(i);
    }
  };

  /// @}
};

#include "otstem.h"

namespace opentree
{
  inline void otTree::addTrunk(otStem* stem)
  {
    stem->tree = this;
    trunks.add(stem);
  }
};

#endif // _OT_TREE_H_
