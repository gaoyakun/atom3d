#ifndef __ATOM_GEOMETRY_ETMESH_H
#define __ATOM_GEOMETRY_ETMESH_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <map>
#include "../ATOM_math.h"

class ATOM_ETMesh
{
	struct EdgeKey
	{
	  int v[2];

	  inline EdgeKey (int v0 = -1, int v1 = -1);
	  inline bool operator < (const EdgeKey &rhs) const;
	};

	struct TriangleKey
	{
	  int v[3];

	  inline TriangleKey (int v0 = -1, int v1 = -1, int v2 = -1);
	  inline bool operator < (const TriangleKey& rhs) const;
	};

public:
    struct Edge;
    typedef Edge* (*ECreator)(int,int);
    typedef std::map<EdgeKey,Edge*> EMap;
    typedef EMap::iterator EMapIterator;
    typedef EMap::const_iterator EMapConstIterator;

    struct Triangle;
    typedef Triangle* (*TCreator)(int,int,int);
    typedef std::map<TriangleKey,Triangle*> TMap;
    typedef TMap::iterator TMapIterator;
    typedef TMap::const_iterator TMapConstIterator;

    struct Edge
    {
      Edge (int v0, int v1);
      virtual ~Edge ();

      int v[2];
      Triangle *t[2];
    };

    struct Triangle
    {
      Triangle (int v0, int v1, int v2);
      virtual ~Triangle ();

      int v[3];
      Edge *e[3];
      Triangle *t[3];
    };


    ATOM_ETMesh (ECreator ec = 0, TCreator tc = 0);
    virtual ~ATOM_ETMesh ();

    inline const EMap& GetEdges () const;
    inline const TMap& GetTriangles () const;

    Triangle * InsertTriangle (int v0, int v1, int v2);
    bool RemoveTriangle (int v0, int v1, int v2);

    bool IsClosed () const;

protected:
    static Edge *CreateEdge (int v0, int v1);
    ECreator _M_ec;
    EMap _M_emap;

    static Triangle *CreateTriangle (int v0, int v1, int v2);
    TCreator _M_tc;
    TMap _M_tmap;
};

inline ATOM_ETMesh::EdgeKey::EdgeKey (int v0, int v1) {
  if (v0 < v1)
  {
    v[0] = v0;
    v[1] = v1;
  }
  else
  {
    v[0] = v1;
    v[1] = v0;
  }
}

inline bool ATOM_ETMesh::EdgeKey::operator< (const EdgeKey& rhs) const {
  if (v[1] < rhs.v[1] )
    return true;
  if (v[1] > rhs.v[1] )
    return false;
  return v[0] < rhs.v[0];
}

// inline member functions of TriangleKey
inline ATOM_ETMesh::TriangleKey::TriangleKey (int v0, int v1, int v2) {
  if (v0 < v1)
  {
    if (v0 < v2)
    {
      v[0] = v0;
      v[1] = v1;
      v[2] = v2;
    }
    else
    {
      v[0] = v2;
      v[1] = v0;
      v[2] = v1;
    }
  }
  else
  {
    if (v1 < v2)
    {
      v[0] = v1;
      v[1] = v2;
      v[2] = v0;
    }
    else
    {
      v[0] = v2;
      v[1] = v0;
      v[2] = v1;
    }
  }
}

inline bool ATOM_ETMesh::TriangleKey::operator < (const TriangleKey& rhs) const {
  if (v[2] < rhs.v[2])
    return true;
  if (v[2] > rhs.v[2])
    return false;
  if (v[1] < rhs.v[1])
    return true;
  if (v[1] > rhs.v[1])
    return false;
  return v[0] < rhs.v[0];
}

// inline member functions of ATOM_ETMesh
inline const ATOM_ETMesh::EMap& ATOM_ETMesh::GetEdges () const {
  return _M_emap;
}

inline const ATOM_ETMesh::TMap& ATOM_ETMesh::GetTriangles () const {
  return _M_tmap;
}

#endif // __ATOM_GEOMETRY_ETMESH_H
