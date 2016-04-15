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

#ifndef _OT_OTTREESAVER_H_
#define _OT_OTTREESAVER_H_

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
  class otTreeSaver
  {
  private:
    unsigned int getTreeSize(otTree* tree)
    {
      unsigned int sum = 5;

      sum += 5; //Magic Number "OTL01"

      for (unsigned int i = 0; i < tree->getTrunkCount(); i++)
      {
        sum += getStemSize(tree->getTrunk(i));
      }

      return sum;
    }

    unsigned int getStemSize(otStem* stem)
    {
      unsigned int sum = 6;
      unsigned int i;

      sum += stem->getPosSpline()->getSerialiser()->getDataLength();
      sum += stem->getRadSpline()->getSerialiser()->getDataLength();

      for (i = 0; i < stem->getLeafCount(); i++)
      {
        sum += getLeafSize();
      }

      for (i = 0; i < stem->getChildCount(); i++)
      {
        sum += getStemSize(stem->getChild(i));
      }

      return sum;
    }

    unsigned int getLeafSize()
    {
      //Leaf == 1 otMatrix + 2 float => 18 float => 72byte
      return 72;
    }

    // ----------------------------------------------------------------------

    unsigned int saveTree(otTree* tree, char* data)
    {
      unsigned int sum = 10;

      //Magic Number
      strncpy(data, "OTL01", 5);

      //let's hope we never have more than 256 trunks :)
      data[5] = tree->getTrunkCount();
      *(unsigned int*)(data+6) = tree->getLeafCount(); 

      for (unsigned int i = 0; i < tree->getTrunkCount(); i++)
      {
        sum += saveStem(tree->getTrunk(i),data+sum);
      }

      return sum;
    }

    unsigned int saveStem(otStem* stem, char* data)
    {
      unsigned int sum = 10;

      data[0] = stem->levelNo;

      *(int*)&data[1] = stem->getLeafCount();
      
      *(float*)&data[5] = stem->offset;

      //let's hope we never have more than 256 children :)
      data[9] = stem->getChildCount();

      sum += stem->getPosSpline()->getSerialiser()->writeData(data+sum);
      sum += stem->getRadSpline()->getSerialiser()->writeData(data+sum);

      unsigned int i;

      for (i = 0; i < stem->getLeafCount(); i++)
      {
        sum += saveLeaf(stem->getLeaf(i),data+sum);
      }

      for (i = 0; i < stem->getChildCount(); i++)
      {
        sum += saveStem(stem->getChild(i),data+sum);
      }

      return sum;
    }

    unsigned int saveLeaf(otLeaf& leaf, char* data)
    {
      float* M = leaf.transform;
      *(float*)(data+ 0) = M[ 0]; 
      *(float*)(data+ 4) = M[ 1];
      *(float*)(data+ 8) = M[ 2];
      *(float*)(data+12) = M[ 3];
      *(float*)(data+16) = M[ 4];
      *(float*)(data+20) = M[ 5];
      *(float*)(data+24) = M[ 6];
      *(float*)(data+28) = M[ 7];
      *(float*)(data+32) = M[ 8];
      *(float*)(data+36) = M[ 9];
      *(float*)(data+40) = M[10];
      *(float*)(data+44) = M[11];
      *(float*)(data+48) = M[12];
      *(float*)(data+52) = M[13];
      *(float*)(data+56) = M[14];
      *(float*)(data+60) = M[15];
      *(float*)(data+64) = leaf.length; 
      *(float*)(data+68) = leaf.width;
      return 72;
    }

  public:
    otTreeSaver() {}
    ~otTreeSaver() {}

    /// Writes output to #data and return length of the string.
    unsigned int save(otTree* tree, char*& data)
    {
      unsigned int size = getTreeSize(tree);
      data = new char[size];
      saveTree(tree, data);
      return size;
    }
  };

  /// @}
}

#endif //_OT_OTCOMPARE_H_
