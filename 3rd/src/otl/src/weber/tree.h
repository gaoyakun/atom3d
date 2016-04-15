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

#ifndef _WEBER_TREE_H_
#define _WEBER_TREE_H_

#include <opentree/weber/webertypes.h>
#include <opentree/utils/otarray.h>
#include <opentree/utils/otmatrix.h>

namespace opentree
{
  class otWeberStem;
  class otWeberStemRelations;

  /// \addtogroup Weber_Algorithm
  /// @{

  /** Class for creating a 3d tree object ala Weber.
   * A tree has one or more trunks, with several levels of
   * branches all of which are instances of the Stem class.
   */
  class otWeberTree 
  {
    friend class otWeberLeafFactory;

  private:
    float trunk_rotangle;
    unsigned int leaves;

  public:
    otArray<otWeberStemRelations*> stemrelations;

    TreeData params;
    Level* level;
    
    otWeberTree() : trunk_rotangle(0), leaves(0)
    {
    }
    
    ~otWeberTree()
    {
      stemrelations.delAll();
    }

    /// Generates a tree from the TreeData
    void make();

    void setParams(TreeData& params)
    {
      this->params = params;
      level = &params.level[0];
    }

    unsigned int getLeafCount()
    {
      return leaves;
    }

  };

/// }@

}

#endif //_WEBER_TREE_H_
