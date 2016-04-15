#ifndef __ATOM_GEOMETRY_CONVEXHULL3D_H
#define __ATOM_GEOMETRY_CONVEXHULL3D_H

#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include "../ATOM_math.h"
#include "etmesh.h"

template <class T>
class ATOM_ConvexHull
  {
    typedef ATOM_Vector3f Vector3;

  public:
    ATOM_ConvexHull (int quantity, const Vector3* vertices);
    virtual ~ATOM_ConvexHull ();

    enum
    {
      HULL_POINT,
      HULL_LINEAR,
      HULL_PLANAR,
      HULL_SPATIAL
    };

    int GetType () const;
    const std::vector<int>& GetConnectivity () const;

  protected:
    void MergePoint (int p);
    void MergeLinear (int p);
    void MergePlanar (int p);
    void MergeSpatial (int p);

    int _M_hull_type;
    std::vector<int> _M_hull_indices;

  private:
    struct Triangle : public ATOM_ETMesh::Triangle
    {
      Triangle (int v0, int v1, int v2): ATOM_ETMesh::Triangle(v0,v1,v2) {}
      Vector3 Normal;
    };

    static ATOM_ETMesh::Triangle* CreateTriangle (int v0, int v1, int v2);
    void InsertTriangle (int v0, int v1, int v2);

    int _M_quantity;
    Vector3* _M_vertices;
    std::vector<int> _M_hull_p;
    Vector3 _M_origin, _M_normal;
    ATOM_ETMesh _M_hull_mesh;
  };

template <class T>
ATOM_ConvexHull<T>::ATOM_ConvexHull (int quantity, const Vector3* vertices): _M_hull_mesh(0,CreateTriangle) {
  T minvalue = vertices[0].x(), maxvalue = minvalue;
  int i;
  for (i = 0; i < quantity; i++)
  {
    if ( vertices[i].x() < minvalue )
      minvalue = vertices[i].x();
    else if ( vertices[i].x() > maxvalue )
      maxvalue = vertices[i].x();

    if ( vertices[i].y() < minvalue )
      minvalue = vertices[i].y();
    else if ( vertices[i].y() > maxvalue )
      maxvalue = vertices[i].y();

    if ( vertices[i].z() < minvalue )
      minvalue = vertices[i].z();
    else if ( vertices[i].z() > maxvalue )
      maxvalue = vertices[i].z();
  }
  T halfrange = ((T)0.5)*(maxvalue - minvalue);
  T invhalfrange = ((T)1.0)/halfrange;

  _M_quantity = quantity;
  _M_vertices = new Vector3[_M_quantity];
  for (i = 0; i < _M_quantity; i++)
  {
    _M_vertices[i].x() = -(T)1.0+invhalfrange*(vertices[i].x()-minvalue);
    _M_vertices[i].y() = -(T)1.0+invhalfrange*(vertices[i].y()-minvalue);
    _M_vertices[i].z() = -(T)1.0+invhalfrange*(vertices[i].z()-minvalue);
  }

  _M_hull_type = HULL_POINT;
  _M_hull_p.push_back(0);
  for (i = 1; i < _M_quantity; i++)
  {
    switch ( _M_hull_type )
    {
    case HULL_POINT:
      MergePoint(i);
      break;
    case HULL_LINEAR:
      MergeLinear(i);
      break;
    case HULL_PLANAR:
      MergePlanar(i);
      break;
    case HULL_SPATIAL:
      MergeSpatial(i);
      break;
    }
  }

  if ( _M_hull_type == HULL_SPATIAL )
  {
    const ATOM_ETMesh::TMap& tmap = _M_hull_mesh.GetTriangles();
    _M_hull_indices.resize(3*tmap.size());
    i = 0;
    ATOM_ETMesh::TMap::const_iterator t_it;
    for (t_it = tmap.begin(); t_it != tmap.end(); t_it++)
    {
      const Triangle* tri = (const Triangle*)t_it->second;
      for (int j = 0; j < 3; j++)
        _M_hull_indices[i++] = tri->v[j];
    }
  }
  else
  {
    int q = (int)_M_hull_p.size();
    _M_hull_indices.resize(q);

    for (i = 0; i < q; i++)
      _M_hull_indices[i] = _M_hull_p[i];
  }

  _M_hull_p.clear();
  delete[] _M_vertices;
  _M_vertices = 0;
}

template <class T>
ATOM_ConvexHull<T>::~ATOM_ConvexHull () {
  assert( _M_vertices == 0 );
}

template <class T>
int ATOM_ConvexHull<T>::GetType () const {
  return _M_hull_type;
}

template <class T>
const std::vector<int>& ATOM_ConvexHull<T>::GetConnectivity () const {
  return _M_hull_indices;
}

template <class T>
void ATOM_ConvexHull<T>::MergePoint (int p) {
  Vector3 diff = _M_vertices[_M_hull_p[0]] - _M_vertices[p];
  if ( diff.getLength() > FLT_EPSILON )
  {
    _M_hull_type = HULL_LINEAR;
    _M_hull_p.push_back(p);
  }
}

template <class T>
void ATOM_ConvexHull<T>::MergeLinear (int p) {
  const Vector3& q0 = _M_vertices[_M_hull_p[0]];
  const Vector3& p1 = _M_vertices[_M_hull_p[1]];
  const Vector3& pt = _M_vertices[p];

  Vector3 edge1 = p1 - q0;
  Vector3 edge2 = pt - q0;
  _M_normal = crossProduct (edge1, edge2);
  T len = _M_normal.normalize();

  if ( len > FLT_EPSILON )
  {
    _M_hull_type = HULL_PLANAR;
    _M_hull_p.push_back(p);
    _M_origin = q0;
    return;
  }

  T e1e2 = dotProduct (edge1, edge2);
  if ( e1e2 < 0.0f )
  {
    _M_hull_p[0] = p;
    return;
  }

  T e1e1 = edge1.getSquaredLength();
  if ( e1e2 > e1e1 )
  {
    _M_hull_p[1] = p;
    return;
  }
}

template <class T>
void ATOM_ConvexHull<T>::MergePlanar (int p) {
  const Vector3& pt = _M_vertices[p];
  T order = dotProduct (_M_normal, pt - _M_origin);
  int i;
  if (Abs(order) <= FLT_EPSILON)
  {
    Vector3* q0;
    Vector3* q1;
    Vector3 edge1, edge2;

    int size = (int)_M_hull_p.size(), sizem1 = size-1;
    int l, u;
    for (l = sizem1, u = 0; u < size; l = u++)
    {
      q0 = &_M_vertices[_M_hull_p[l]];
      q1 = &_M_vertices[_M_hull_p[u]];
      edge1 = *q1 - *q0;
      edge2 = pt - *q0;
      Vector3 tmp = crossProduct (edge1, edge2);
      tmp.normalize();
      order = dotProduct (_M_normal, tmp);
      if ( order < (T)0.0 )
        break;
    }
    if ( u == size )
    {
      return;
    }

    for (i = u+1; i < size; u = i++)
    {
      q0 = &_M_vertices[_M_hull_p[u]];
      q1 = &_M_vertices[_M_hull_p[i]];
      edge1 = *q1 - *q0;
      edge2 = pt - *q0;
      Vector3 tmp = crossProduct (edge1, edge2);
      tmp.normalize();
      order = dotProduct (_M_normal, tmp);
      if ( order >= (T)0.0 )
        break;
    }

    if ( l == sizem1 )
    {
      for (i = l-1; i >= 0; l = i--)
      {
        q0 = &_M_vertices[_M_hull_p[i]];
        q1 = &_M_vertices[_M_hull_p[l]];
        edge1 = *q1 - *q0;
        edge2 = pt - *q0;
        Vector3 tmp = crossProduct (edge1, edge2);
        tmp.normalize();
        order = dotProduct (_M_normal, tmp);
        if ( order >= (T)0.0 )
          break;
      }
    }

    if ( u == l )
    {
      return;
    }

    std::vector<int> tmpHull;
    tmpHull.push_back(p);
    while ( true )
    {
      tmpHull.push_back(_M_hull_p[u]);
      if ( u == l )
        break;

      if ( ++u == size )
        u = 0;
    }
    assert( tmpHull.size() > 2 );

    _M_hull_p = tmpHull;
    return;
  }

  _M_hull_type = HULL_SPATIAL;

  int size = (int)_M_hull_p.size();
  int iq0 = _M_hull_p[0];
  int iq1 = _M_hull_p[1];
  int iq2;

  if ( order > FLT_EPSILON )
  {
    InsertTriangle(p,iq0,iq1);

    for (i = 2; i < size; iq1 = iq2, i++)
    {
      iq2 = _M_hull_p[i];
      InsertTriangle(iq0,iq2,iq1);
      InsertTriangle(p,iq1,iq2);
    }

    InsertTriangle(p,iq1,iq0);
  }
  else
  {
    InsertTriangle(p,iq1,iq0);

    for (i = 2; i < size; iq1 = iq2, i++)
    {
      iq2 = _M_hull_p[i];
      InsertTriangle(iq0,iq1,iq2);
      InsertTriangle(p,iq2,iq1);
    }

    InsertTriangle(p,iq0,iq1);
  }
}

template <class T>
void ATOM_ConvexHull<T>::MergeSpatial (int p) {
  Vector3 diff;
  T order, maxorder = (T)0.0;
  const Triangle* tri;
  const Triangle* maxtri = 0;
  const ATOM_ETMesh::TMap& tmap = _M_hull_mesh.GetTriangles();
  ATOM_ETMesh::TMap::const_iterator t_it;
  for (t_it = tmap.begin(); t_it != tmap.end(); t_it++)
  {
    tri = (const Triangle*)t_it->second;
    diff = _M_vertices[p] - _M_vertices[tri->v[0]];
    diff.normalize();
    order = dotProduct (tri->Normal, diff);
    if ( order > maxorder )
    {
      maxorder = order;
      maxtri = tri;
    }
  }

  if ( !maxtri || maxorder < FLT_EPSILON )
  {
    return;
  }

  std::set<const Triangle*> interior, boundary;
  interior.insert(maxtri);
  const Triangle* adj;
  int i;
  for (i = 0; i < 3; i++)
  {
    adj = (const Triangle*)maxtri->t[i];
    assert( adj );
    boundary.insert(adj);
  }

  typename std::set<const Triangle*>::iterator Iter;
  while ( boundary.size() > 0 )
  {
    std::set<const Triangle*> exterior;

    for (Iter = boundary.begin(); Iter != boundary.end(); Iter++)
    {
      tri = (const Triangle*)*Iter;
      diff = _M_vertices[p] - _M_vertices[tri->v[0]];
      diff.normalize();
      order = dotProduct (tri->Normal, diff);
      if ( order > (T)0.0 )
      {
        interior.insert(tri);

        for (i = 0; i < 3; i++)
        {
          adj = (const Triangle*)tri->t[i];
          assert( adj );
          if ( interior.find(adj) == interior.end() && boundary.find(adj) == boundary.end() )
          {
            exterior.insert(adj);
          }
        }
      }
    }

    boundary = exterior;
  }

  std::map<int,int> terminator;
  int v0, v1;
  for (Iter = interior.begin(); Iter != interior.end(); Iter++)
  {
    tri = (const Triangle*)*Iter;

    for (i = 0; i < 3; i++)
    {
      adj = (const Triangle*)tri->t[i];
      assert( adj );
      if ( interior.find(adj) == interior.end() )
      {
        v0 = tri->v[i];
        v1 = tri->v[(i+1)%3];
        terminator[v0] = v1;
      }
    }
  }

  for (Iter = interior.begin(); Iter != interior.end(); Iter++)
  {
    tri = (const Triangle*)*Iter;
    bool removed = _M_hull_mesh.RemoveTriangle(tri->v[0],tri->v[1],tri->v[2]);
    assert( removed );
    (void)removed;
  }

  int size = (int)terminator.size();
  assert( size >= 3 );
  std::map<int,int>::iterator eit = terminator.begin();
  int vstart = eit->first;
  v0 = vstart;
  for (i = 0; i < size; i++)
  {
    v1 = eit->second;
    InsertTriangle(p,v0,v1);
    eit = terminator.find(v1);
    assert( eit != terminator.end() );
    v0 = v1;
  }
  assert( v0 == vstart );
}

template <class T>
void ATOM_ConvexHull<T>::InsertTriangle (int v0, int v1, int v2) {
  Triangle* tri = (Triangle*) _M_hull_mesh.InsertTriangle(v0,v1,v2);
  assert( tri );

  const Vector3& rv0 = _M_vertices[v0];
  const Vector3& rv1 = _M_vertices[v1];
  const Vector3& rv2 = _M_vertices[v2];
  Vector3 e1 = rv1 - rv0, e2 = rv2 - rv0;
  tri->Normal = crossProduct (e1, e2);
  tri->Normal.normalize();
}

template <class T>
ATOM_ETMesh::Triangle* ATOM_ConvexHull<T>::CreateTriangle (int v0, int v1, int v2) {
  return new Triangle(v0,v1,v2);
}

#endif // __ATOM_GEOMETRY_CONVEXHULL3D_H
