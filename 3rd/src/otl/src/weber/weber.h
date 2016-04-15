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

#ifndef _OT_WEBER_H_
#define _OT_WEBER_H_

#include "opentree/opentree.h"
#include "collector.h"
#include "opentree/ottree.h"

#include "tree.h"

namespace opentree
{

  /** \addtogroup Weber_Algorithm
  * Implementation of the Tree generation algorithm of J. Weber and J.Penn.
  * described in "Creation and Rendering of Realistic Trees"
  *
  * The implementation of the algorithm is based on the following two projects:
  * <br>Arbaro: http://arbaro.sourceforge.net (c) Wolfram Diestel
  * <br>TReal: http://members.chello.nl/~l.vandenheuvel2/TReal/ (c) Ton van den Heuvel
  */

  /// @{

  class otWeber : public iWeber
  {
  private:
    otWeberTree tree;

  public:
    otWeber() {}
    virtual ~otWeber() {}

    void setParams(TreeData& params)
    {
      tree.setParams(params);
    }

    TreeData* getParams()
    {
      return &tree.params;
    }

    otTree* generate()
    {
      tree.make();
      otTree* ottree;
      {
        Collector col(&tree);
        col.collect();
        ottree = col.returnTree();
      }
      return ottree;
    }
  };

/// }@

}

#endif //_OT_WEBER_H_
