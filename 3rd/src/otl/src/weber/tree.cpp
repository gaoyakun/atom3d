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

#include "tree.h"
#include "stem.h"
#include "stemrelations.h"
#include "trunkfactory.h"
#include "stemfactory.h"
namespace opentree
{
  void otWeberTree::make() 
  {
    otWeberTrunkFactory trunkfact(this, *level);

    stemrelations.setGrowRate(level->branches);
    for (int i=0; i<level->branches; i++) 
    {
      otWeberStemRelations* relation = new otWeberStemRelations(0, 0);
      otWeberStem trunk = trunkfact.create();

      otWeberStemFactory stemfact(&trunk, relation);
      if (stemfact.make())
      {
        stemrelations.add(relation);
      }
      else
      {
        delete relation;
      }
    }
  }
}
