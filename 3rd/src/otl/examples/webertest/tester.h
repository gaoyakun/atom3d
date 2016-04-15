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

#ifndef _TESTER_H_
#define _TESTER_H_

#include "opentree/opentree.h"
#include "testxmlparser.h"
#include "testerdata.h"

/** \addtogroup WeberTest
 * Generates Weber Trees and compares them against pregenerated trees.
 * Great for refactoring the weber algorithm to check if you broke something.
 * Only implemented for otl-weber, not for arbaro-weber.
 */

/// @{

class Tester
{
private:
  opentree::otTree* createTree(const char* file, const char* format, const char* algo);
  opentree::otTree* loadPregenTree(const char* params);
  bool test(TesterData& data);

public:
  Tester() {};
  ~Tester() {};
  void Run(const char* file);
};

/// }@

#endif //_TESTER_H_
