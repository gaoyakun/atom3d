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

#ifndef _OT_OTTREELOADER_H_
#define _OT_OTTREELOADER_H_

#include "opentree/ottree.h"
#include "opentree/otstem.h"
#include "opentree/otleaf.h"

#include "opentree/utils/otpolylinespline.h"
#include "opentree/utils/otpolylinespline3.h"

namespace opentree
{
  /// \addtogroup Helper
  /// @{

  /** Serializes Trees
   * This class allows the serialization of a otTree classes.
   * It returns a binary string of all stems and leaves.
   */
  class otTreeLoader
  {
  private:
    otTree* loadTree(const char* data)
    {
      unsigned int sum = 10;

      //check Magic Number
      if (strncmp(data, "OTL01", 5) != 0)
      {
        otAssert("No or wrong Magic Number");
        return 0;
      }

      //let's hope we never have more than 256 trunks :)
      unsigned int trunkcount = data[5];
      unsigned int leafcount = *(unsigned int*)(data+6);

      otTree* tree = new otTree(trunkcount, leafcount);
      for (unsigned int i = 0; i<trunkcount; i++)
      {
        otStem* stem = new otStem(data[sum], data[sum+1], data[sum+2], 0);

        tree->addTrunk(stem);
        sum += loadStem(stem,data+sum);
      }

      return tree;
    }

    unsigned int loadStem(otStem* stem, const char* data)
    {
      unsigned int sum = 10;

      unsigned int leafcount = *(int*)&data[1];
      unsigned int childcount = data[5];

      sum += loadPosSpline(stem, data+sum);
      sum += loadRadSpline(stem, data+sum);

      for (unsigned int i = 0; i<leafcount; i++)
      {
        sum += loadLeaf(stem,data+sum);
      }

      for (unsigned int i = 0; i<childcount; i++)
      {
        otStem* child = new otStem(data[sum], *(int*)&data[1], data[sum+5],
          *(float*)&data[sum+6]);

        stem->addChild(child);
        sum += loadStem(child,data+sum);
      }

      return sum;
    }

    unsigned int loadLeaf(otStem* stem, const char* data)
    {
      otMatrix M;
      M[ 0] = *(float*)(data+ 0); 
      M[ 1] = *(float*)(data+ 4);
      M[ 2] = *(float*)(data+ 8);
      M[ 3] = *(float*)(data+12);
      M[ 4] = *(float*)(data+16);
      M[ 5] = *(float*)(data+20);
      M[ 6] = *(float*)(data+24);
      M[ 7] = *(float*)(data+28);
      M[ 8] = *(float*)(data+32);
      M[ 9] = *(float*)(data+36);
      M[10] = *(float*)(data+40);
      M[11] = *(float*)(data+44);
      M[12] = *(float*)(data+48);
      M[13] = *(float*)(data+52);
      M[14] = *(float*)(data+56);
      M[15] = *(float*)(data+60);
      otLeaf leaf(M,*(float*)(data+64),*(float*)(data+68));
      stem->addLeaf(leaf);
      return 72;
    }

    unsigned int loadPosSpline(otStem* stem, const char* data)
    {
      unsigned sum = 1;

      unsigned count = data[0];

      otPolyLineSpline3* spline = new otPolyLineSpline3(count);
      stem->setPosSpline(spline);

      for (unsigned int i=0; i<count; i++)
      {
        otVector3 v(*(float*)(data+sum+ 4), *(float*)(data+sum+ 8), *(float*)(data+sum+12));
        spline->setPoint(*(float*)(data+sum+ 0), v);
        sum += 16;
      }

      return sum;
    }

    unsigned int loadRadSpline(otStem* stem, const char* data)
    {
      unsigned sum = 1;

      unsigned count = data[0];

      otPolyLineSpline* spline = new otPolyLineSpline(count);
      stem->setRadSpline(spline);

      for (unsigned int i=0; i<count; i++)
      {
        spline->setPoint(*(float*)(data+sum+ 0), *(float*)(data+sum+ 4));
        sum += 8;
      }

      return sum;
    }

  public:
    otTreeLoader() {}
    ~otTreeLoader() {}

    /// Writes output to #data and return length of the string.
    otTree* load(const char* data)
    {
      return loadTree(data);
    }
  };

  /// @}
}

#endif //_OT_OTTREELOADER_H_
