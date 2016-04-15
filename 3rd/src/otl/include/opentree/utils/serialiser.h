/*
  Copyright (c) 2005 Pascal Kirchdorfer, Seth Yastrov

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

#ifndef __SERIALISER_H_
#define __SERIALISER_H_

#include "otvector3.h"

namespace opentree
{

  /// Interface used for serialiser classes.
  class iSerialiser
  {
  public:
    virtual ~iSerialiser() {}

    /// Returns the byte count of the serialised string.
    virtual unsigned int getDataLength() = 0;
    
    /// Writes the spline in the #data string and returns its length.
    virtual unsigned int writeData(char* data) = 0;
  };

};

#endif // __SERIALISER_H_
