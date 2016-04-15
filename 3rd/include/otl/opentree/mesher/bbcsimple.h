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

#ifndef _OT_BBCSIMPLE_H_
#define _OT_BBCSIMPLE_H_

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !! This implementation is highly experimental (hacky and messy).        !!
// !! Read at your own risk. Don't say it's bad code, we know it :)        !!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#include "opentree/otleaf.h"

namespace opentree
{
  class BBCSimple
  {
  public:
  class Billboard : public iMesher
  {
    otVector3 max_v;
    otVector3 min_v;
    otVector3 normal;

    otMatrix trf;

    float j;

  public:
    Billboard(const otVector3& max_v, const otVector3& min_v, const otMatrix& trf, float j)
    : max_v(max_v), min_v(min_v), trf(trf), j(j)
    {
      normal = trf.apply(otVector3(0,-1,0));
    }

    ~Billboard() {}

    unsigned int getVerticesCount() { return 4; }
    void getVertices(otVertices& vertices)
    {
      otVector3 v1 = trf.apply(otVector3(min_v.X,j, min_v.Z));
      otVector3 v2 = trf.apply(otVector3(max_v.X,j, min_v.Z));
      otVector3 v3 = trf.apply(otVector3(min_v.X,j, max_v.Z));
      otVector3 v4 = trf.apply(otVector3(max_v.X,j, max_v.Z));

      otVector3 c(0,0.6f,0);

      vertices.add(otVertex(v1,normal,c,0,0));
      vertices.add(otVertex(v2,normal,c,1,0));
      vertices.add(otVertex(v3,normal,c,0,1));
      vertices.add(otVertex(v4,normal,c,1,1));
    }

    unsigned int getIndicesCount() { return 2; }
    void getIndices(otTriangles& triangles, unsigned int offset)
    {
      triangles.add(otTriangle(0,1,2)+offset);
      triangles.add(otTriangle(1,3,2)+offset);
    }

    const otVector3& getNormal() { return normal; }
  };

  private:
    otArray<Billboard*> bbs;

    class Linker
    {
    public:
      Linker* child;
      //otLeaf* object;
      otVector3 local_pos;
      float dist;
    };

    Linker* linkers;
    unsigned int linkers_pointer;

    class RotGroup
    {
    public:
      RotGroup() : rot_group(0), rot_group_last(0) 
      {
        dist_min = otVector3(FLT_MAX,FLT_MAX,FLT_MAX);
        dist_max = -otVector3(FLT_MAX,FLT_MAX,FLT_MAX);
      }

      Linker* rot_group;
      Linker* rot_group_last;

      otVector3 dist_min;
      otVector3 dist_max;

      otVector3 normal;
    };
    RotGroup* rot_group;

    int rot_groups;
    int max_groups;
    int filled_groups;

  public:
    BBCSimple(otTree* tree, int rot_groups) : rot_groups(rot_groups)
    {
      max_groups = rot_groups*rot_groups*rot_groups;
      linkers_pointer = 0;
      linkers = new Linker[tree->getLeafCount()];
      rot_group = new RotGroup[max_groups];
      printf("start: %d\n", time(0));
      for (int iz=0; iz<rot_groups; iz++)
      {
        for (int iy=0; iy<rot_groups; iy++)
        {
          for (int ix=0; ix<rot_groups; ix++)
          {
            otVector3 rotnorm;
            rotnorm.X=((ix+0.5f)/rot_groups*2-1);
            rotnorm.Y=((iy+0.5f)/rot_groups*2-1);
            rotnorm.Z=((iz+0.5f)/rot_groups*2-1);
            rotnorm.normalize();
            rot_group[ix+iy*rot_groups+iz*rot_groups*rot_groups].normal = rotnorm;
          }
        }
      }
      for (int i=0; i<tree->getLeafCount(); i++)
        check(&tree->getLeaf(i));
      printf("end: %d\n", time(0));
      printf("checked: %d\n\n", tree->getLeafCount());
      int total=0;
      filled_groups=0;
      for (int i=0; i<max_groups; i++)
      {
        int n = 0;

        Linker* link = rot_group[i].rot_group;

        if (link)
        {
          n++;
          while (link != rot_group[i].rot_group_last)
          {
            n++;
            link = link->child;
          }
        }
        //printf("\nGroup %d (<%.2f,%.2f,%.2f>) has %d leaves", i, rot_group[i].normal.X, rot_group[i].normal.Y, rot_group[i].normal.Z, n);
        if (n > 0)
        {
          //printf(" between %.2f and %.2f", rot_group[i].dist_min.Y, rot_group[i].dist_max.Y);
          filled_groups++;
        }

        total += n;
      }
      printf("\n");
      printf("Total count of %d leaves\n", total);
      printf("Filled groups: %d of %d\n", filled_groups, max_groups);

      calcBillboards();
    }
    ~BBCSimple()
    {
      delete [] linkers;
      delete [] rot_group;
      bbs.delAll();
    }

    unsigned int getBillboardCount() { return bbs.getCount(); }
    BBCSimple::Billboard* getBillboard(unsigned int idx) { return bbs.get(idx); }

    void calcBillboards()
    {
      bbs.setGrowRate(filled_groups * rot_groups);
      for (int i=0; i<max_groups; i++)
      {
        if (!rot_group[i].rot_group)
          continue;

        otVector3 n = rot_group[i].normal;

        otVector3 rot = n & otVector3(0,1,0);
        float sign = n | otVector3(0,1,0);

        otMatrix trf;
        trf.grotateRad(acosf(sign), rot);
        otVector3 new_normal = trf * otVector3(0,1,0);

        float length = rot_group[i].dist_max.X-rot_group[i].dist_min.X;
        float width = rot_group[i].dist_max.Z-rot_group[i].dist_min.Z;

        float delta = (rot_group[i].dist_max.Y-rot_group[i].dist_min.Y)/(rot_groups-1);
        if (delta == 0) delta = 1;

        float j = rot_group[i].dist_min.Y;

        for (int jj=0; jj<rot_groups; jj++)
        {

          Linker* link = rot_group[i].rot_group;

          otVector3 max_v = rot_group[i].dist_min, min_v= rot_group[i].dist_max;

          int ctr = 0;

          if (link)
          {
            if (link->local_pos.Y > j && link->local_pos.Y < j + delta)
            {
              ctr++;
              if (max_v.X < link->local_pos.X ) max_v.X = link->local_pos.X+0.25f;
              if (min_v.X > link->local_pos.X ) min_v.X = link->local_pos.X-0.25f;
              if (max_v.Y < link->local_pos.Y ) max_v.Y = link->local_pos.Y+0.25f;
              if (min_v.Y > link->local_pos.Y ) min_v.Y = link->local_pos.Y-0.25f;
              if (max_v.Z < link->local_pos.Z ) max_v.Z = link->local_pos.Z+0.25f;
              if (min_v.Z > link->local_pos.Z ) min_v.Z = link->local_pos.Z-0.25f;
            }

            while (link != rot_group[i].rot_group_last)
            {
              if (link->local_pos.Y > j && link->local_pos.Y < j + delta)
              {
                ctr++;
                if (max_v.X < link->local_pos.X ) max_v.X = link->local_pos.X+0.25f;
                if (min_v.X > link->local_pos.X ) min_v.X = link->local_pos.X-0.25f;
                if (max_v.Y < link->local_pos.Y ) max_v.Y = link->local_pos.Y+0.25f;
                if (min_v.Y > link->local_pos.Y ) min_v.Y = link->local_pos.Y-0.25f;
                if (max_v.Z < link->local_pos.Z ) max_v.Z = link->local_pos.Z+0.25f;
                if (min_v.Z > link->local_pos.Z ) min_v.Z = link->local_pos.Z-0.25f;
              }

              link = link->child;
            }

          }

          if (ctr == 0)
          {
            min_v = otVector3(0,0,0);
            max_v = otVector3(0,0,0);
          }

          bbs.add(new Billboard(max_v, min_v, trf, j));

          j+=delta;
        }
      }
    }
        
/*
    unsigned int getVertices(otVertices& vertices)
    {
      for (int i=0; i<max_groups; i++)
      {
        if (!rot_group[i].rot_group)
          continue;

        otVector3 n = rot_group[i].normal;

        otVector3 rot = n & otVector3(0,1,0);
        float sign = n | otVector3(0,1,0);

        otMatrix trf;
        trf.grotateRad(acosf(sign), rot);
        otVector3 new_normal = trf * otVector3(0,1,0);

        if (!(n == new_normal))
        {
          int k=43;
        }

        float length = rot_group[i].dist_max.X-rot_group[i].dist_min.X;
        float width = rot_group[i].dist_max.Z-rot_group[i].dist_min.Z;

        otVector3 c(0,0.6f,0);

        //rot_group[i].dist_min.X = -5;
        //rot_group[i].dist_min.Y = 0;
        //rot_group[i].dist_min.Z = -5;
        //rot_group[i].dist_max.X = 5;
        //rot_group[i].dist_max.Y = 10;
        //rot_group[i].dist_max.Z = 5;

        float delta = (rot_group[i].dist_max.Y-rot_group[i].dist_min.Y)/(rot_groups-1);
        if (delta == 0) delta = 1;

        float j = rot_group[i].dist_min.Y;

        printf("\nAngle: Min: %.4f,\t Max: %.4f\n", rot_group[i].dist_min.Y, rot_group[i].dist_max.Y);

        for (int jj=0; jj<rot_groups; jj++)
        {

          Linker* link = rot_group[i].rot_group;

          otVector3 max_v = rot_group[i].dist_min, min_v= rot_group[i].dist_max;

          int ctr = 0;

          if (link)
          {
            if (link->local_pos.Y > j && link->local_pos.Y < j + delta)
            {
              ctr++;
              if (max_v.X < link->local_pos.X ) max_v.X = link->local_pos.X+0.25f;
              if (min_v.X > link->local_pos.X ) min_v.X = link->local_pos.X-0.25f;
              if (max_v.Y < link->local_pos.Y ) max_v.Y = link->local_pos.Y+0.25f;
              if (min_v.Y > link->local_pos.Y ) min_v.Y = link->local_pos.Y-0.25f;
              if (max_v.Z < link->local_pos.Z ) max_v.Z = link->local_pos.Z+0.25f;
              if (min_v.Z > link->local_pos.Z ) min_v.Z = link->local_pos.Z-0.25f;
            }

            while (link != rot_group[i].rot_group_last)
            {
              if (link->local_pos.Y > j && link->local_pos.Y < j + delta)
              {
                ctr++;
                if (max_v.X < link->local_pos.X ) max_v.X = link->local_pos.X+0.25f;
                if (min_v.X > link->local_pos.X ) min_v.X = link->local_pos.X-0.25f;
                if (max_v.Y < link->local_pos.Y ) max_v.Y = link->local_pos.Y+0.25f;
                if (min_v.Y > link->local_pos.Y ) min_v.Y = link->local_pos.Y-0.25f;
                if (max_v.Z < link->local_pos.Z ) max_v.Z = link->local_pos.Z+0.25f;
                if (min_v.Z > link->local_pos.Z ) min_v.Z = link->local_pos.Z-0.25f;
              }

              link = link->child;
            }

          }

          if (ctr == 0)
          {
            min_v = otVector3(0,0,0);
            max_v = otVector3(0,0,0);
            //printf("e");
          }
          //else
            //printf("f");

          otVector3 v1 = trf.apply(otVector3(min_v.X,j, min_v.Z));
          otVector3 v2 = trf.apply(otVector3(max_v.X,j, min_v.Z));
          otVector3 v3 = trf.apply(otVector3(min_v.X,j, max_v.Z));
          otVector3 v4 = trf.apply(otVector3(max_v.X,j, max_v.Z));

          otVector3 normal = trf.apply(otVector3(0,-1,0));

          vertices.add(otVertex(v1,normal,c,0,0));
          vertices.add(otVertex(v2,normal,c,1,0));
          vertices.add(otVertex(v3,normal,c,0,1));
          vertices.add(otVertex(v4,normal,c,1,1));

          //printf("v(%d,%d): <%.1f,%.1f,%.1f><%.1f,%.1f,%.1f><%.1f,%.1f,%.1f><%.1f,%.1f,%.1f>\n",
            //i,jj,v1.X,v1.Y,v1.Z,v2.X,v2.Y,v2.Z,v3.X,v3.Y,v3.Z,v4.X,v4.Y,v4.Z);

          //vertices.add(otVertex(v1,normal,-c,0,0));
          //vertices.add(otVertex(v2,normal,-c,1,0));
          //vertices.add(otVertex(v3,normal,-c,0,1));
          //vertices.add(otVertex(v4,normal,-c,1,1));

          printf("-> %d, %.2f, %d\n",i, j, jj);

          j+=delta;
        }
      }

      return vertices.getCount();
    }

    unsigned int getVertexCount()
    {
      return filled_groups * 4 * rot_groups;
    }

    unsigned int getIndices(otTriangles& triangles, unsigned int offset)
    {
      int ii = 0;
      for (int i=0; i<max_groups; i++)
      {
        if (!rot_group[i].rot_group)
          continue;

        ii++;

        for (int j=0; j<rot_groups; j++)
        {
          triangles.add(otTriangle(0,1,2)+offset+i*rot_groups*4+j*4);
          triangles.add(otTriangle(1,3,2)+offset+i*rot_groups*4+j*4);
          //triangles.add(otTriangle(0,2,1)+offset+i*8+4);
          //triangles.add(otTriangle(1,2,3)+offset+i*8+4);
        }
      }
      return triangles.getCount();
    }

    unsigned int getIndexCount()
    {
      return filled_groups * 2 * rot_groups;
    }
*/
    void check(otLeaf* leaf)
    {
      //otVector3 rot = leaf->transform.getRotationDegrees();
      //if (rot.X > 180) rot.X -= 180;
      //if (rot.Y > 180) rot.Y -= 180;
      //if (rot.Z > 180) rot.Z -= 180;
      otVector3 rot = leaf->transform.apply(otVector3(0,1,0));
      rot.X = ( rot.X + 1 ) / 2;
      rot.Y = ( rot.Y + 1 ) / 2;
      rot.Z = ( rot.Z + 1 ) / 2;
      
      // Group = x * rot_groups^1 + x * rot_groups^2 + x * rot_groups^3
      otVector3 rot_group(int(rot.X*rot_groups),
                          int(rot.Y*rot_groups),
                          int(rot.Z*rot_groups));

      int rot_group_no = rot_group.X + rot_group.Y * rot_groups 
                       + rot_group.Z * rot_groups * rot_groups;

      // -- Get Translation --

      // Get Translation (dot product between roatation normal and translation
      float translation = this->rot_group[rot_group_no].normal | leaf->transform.getTranslation();

      if (translation > this->rot_group[rot_group_no].dist_max.Y) 
        this->rot_group[rot_group_no].dist_max.Y = translation;
      if (translation < this->rot_group[rot_group_no].dist_min.Y) 
        this->rot_group[rot_group_no].dist_min.Y = translation;

      otVector3 rot2 = this->rot_group[rot_group_no].normal & otVector3(0,1,0);
      float sign = this->rot_group[rot_group_no].normal | otVector3(0,1,0);

      otMatrix trf;
      trf.grotateRad(acosf(sign), rot2);

      // Get Translation (dot product between roatation normal and translation
      float t_x = leaf->transform.getTranslation() | trf.apply(otVector3(1,0,0));

      if (t_x > this->rot_group[rot_group_no].dist_max.X) 
        this->rot_group[rot_group_no].dist_max.X = t_x;
      if (t_x < this->rot_group[rot_group_no].dist_min.X) 
        this->rot_group[rot_group_no].dist_min.X = t_x;

      // Get Translation (dot product between roatation normal and translation
      float t_z = leaf->transform.getTranslation() | trf.apply(otVector3(0,0,1));

      if (t_z > this->rot_group[rot_group_no].dist_max.Z) 
        this->rot_group[rot_group_no].dist_max.Z = t_z;
      if (t_z < this->rot_group[rot_group_no].dist_min.Z) 
        this->rot_group[rot_group_no].dist_min.Z = t_z;

      otVector3 lp = otVector3(t_x, translation, t_z);
      add(rot_group_no, translation, lp);
    }

    void add(int group, float translation, otVector3 &local_pos)
    {
      Linker* child = &linkers[linkers_pointer++];
      if (rot_group[group].rot_group == 0) 
        rot_group[group].rot_group = child;
      if (rot_group[group].rot_group_last != 0) 
        rot_group[group].rot_group_last->child = child;
      //child->object = leaf;
      child->local_pos = local_pos;
      child->dist = translation;
      rot_group[group].rot_group_last = child;
    }
  };
};

#endif // _OT_BBCSIMPLE_H_
