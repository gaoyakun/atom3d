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

#ifndef _MESHER_BILLCLOUD_H_
#define _MESHER_BILLCLOUD_H_

#include <opentree/utils/otbox3.h>
#include <opentree/utils/otarray.h>

namespace opentree
{

  /// \addtogroup Mesher
  /// @{

  /** !!NOT FULLY IMPLEMENTED!! Cloud of Billboards
   * Creates a cloud of billboards through the mesh.
   * This currently works only with leaves. Make it more generic.
   */
  class MesherBillCloud : public iMesher
  {
  public:
    struct BillBoardPlane
    {
    public:
      otMatrix trf;
      otArray<otLeaf*> leaves;
      float length, height;
    };

  private:
    otTree* tree;

    otBox3 boundingBox;

    otArray<BillBoardPlane*> planes;

  public:
    MesherBillCloud(otTree* t) : tree(t)
    {
      for (unsigned int i = 0; i < tree->getLeafCount(); i++)
      {
        otLeaf& leaf = tree->getLeaf(i);
        otVector3 v1(0,0,0);
        otVector3 v2(0,0,leaf.width);
        otVector3 v3(leaf.length,0,0);
        otVector3 v4(leaf.length,0,leaf.width);

        boundingBox.extend(leaf.transform * v1);
        boundingBox.extend(leaf.transform * v2);
        boundingBox.extend(leaf.transform * v3);
        boundingBox.extend(leaf.transform * v4);
      }
    }

    virtual ~MesherBillCloud()
    {
      planes.delAll();
    }

    void findBillboards(float errorval)
    {
      otArray<otLeaf*> leaves(tree->getLeafCount());

      for(unsigned int leafNo = 0; leafNo < tree->getLeafCount(); leafNo++)
      {
        otLeaf* leaf = &tree->getLeaf(leafNo);
        leaves.add(leaf);
      }

      int range = (int)boundingBox.getSize().X/2;

      for(int rho = -range; rho < range; rho+=2)
      {
        for(int theta = 0; theta < 180; theta+=15)
        {
          for(int phi = 0; phi < 180; phi+=15)
          {
            BillBoardPlane* plane = new BillBoardPlane();

            //This is probably wrong, the rotation of the plane has not been taken into account.
            plane->length = boundingBox.getSize().Y;
            plane->height = boundingBox.getSize().Z;

            plane->trf.grotZDegree((float)phi);
            plane->trf.grotYDegree((float)theta);
            plane->trf.thisXtranslate((float)rho);

            otVector3 planeNormal(plane->trf.getX());

            // Iterate the faces to find those that allign with the billboard.
            for(unsigned int leafNo = 0; leafNo < leaves.getCount(); leafNo++)
            {
              otLeaf* leaf = leaves.get(leafNo);

              otVector3 leafNormal(leaf->transform.getY());

              float angle = acos(planeNormal | leafNormal);

              // Check if the face is perpendicular with the billboard(within a margin of error). 
              if (angle < errorval)
              {
                // Distance from the face to the billboard.
                float D = fabsf(planeNormal | (leaf->transform.getTranslation()-plane->trf.getTranslation()));

                // Check if the face is sufficiently close to the billboard.
                if (D < errorval)  
                {
                  // Add the plane to the array, 
                  // if its already there, just add the face to the list.
                  plane->leaves.add(leaf);

                  // This face will be rendered to a billboard, 
                  // so we dont have to check against it anymore.
                  leaves.remove(leafNo);

                  // Since our array replaces the this leaf with the last we have to decrement the count
                  leafNo--;
                }
              }
            }
            if (plane->leaves.getCount() > 0)
            {
              planes.add(plane);
            }
	    else
	    {
              delete plane;
	    }
          }
        }
      }
      if (leaves.getCount()> 0)
        printf("Warning: '%d' leaves left!\n", leaves.getCount());
    }

    unsigned int getBillboardCount()
    {
      return planes.getCount();
    }

    BillBoardPlane* getBillboard(unsigned int i)
    {
      return planes.get(i);
    }

    otVector3 getCenter()
    {
      return boundingBox.getCenter();
    }
  };
}

#endif //_MESHER_BILLCLOUD_H_
