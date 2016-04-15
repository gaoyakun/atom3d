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

#ifndef _WEBER_H_
#define _WEBER_H_

#include <opentree/generator.h>
#include <opentree/ottree.h>
#include "webertypes.h"

namespace opentree
{

  /** \addtogroup Weber
   * Interfaces and Datastructures required by the Weber implementations
   */

  /// \addtogroup Weber
  /// @{

  class iWeber : public iGenerator
  {
  public:
    virtual ~iWeber() {}
    
    virtual void setParams(TreeData& params) = 0;
    virtual TreeData* getParams() = 0;

    virtual otTree* generate() = 0;
  };

  /// OTL Weber algorithm
  iWeber* newWeber();

  /// Arbaro Weber algorithm
  iWeber* newWeberArbaro();

  /// @}

};

#endif //_WEBER_H_
