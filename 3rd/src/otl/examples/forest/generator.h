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

#ifndef _FOREST_GENERATOR_H_
#define _FOREST_GENERATOR_H_

#include <opentree/weber/weber.h>
#include <opentree/weber/webertypes.h>
#include <helper/xmlparser.h>

/// \addtogroup Forest
/// @{

class Generator
{
  opentree::xmlParser parser;
  opentree::TreeData data;
  opentree::iWeber* weber;

public:
  Generator()
  : weber(opentree::newWeber()) 
  {
  }

  ~Generator()
  {
    delete weber; 
  }

  void setFile(const char* file)
  {
    parser.Load(data, file);
    weber->setParams(data);
  }
  
  opentree::otTree* generate(int seed)
  {
    opentree::setRandomSeed(seed);
    return weber->generate();
  }
};

/// }@

#endif // _FOREST_GENERATOR_H_
