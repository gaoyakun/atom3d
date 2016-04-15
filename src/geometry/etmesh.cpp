#include "etmesh.h"

ATOM_ETMesh::ATOM_ETMesh (ECreator ec, TCreator tc) {
  _M_ec = ( ec ? ec : CreateEdge );
  _M_tc = ( tc ? tc : CreateTriangle );
}

ATOM_ETMesh::~ATOM_ETMesh () {
  for (EMapIterator it = _M_emap.begin(); it != _M_emap.end(); ++it)
    delete it->second;

  for (TMapIterator it = _M_tmap.begin(); it != _M_tmap.end(); ++it)
    delete it->second;
}

ATOM_ETMesh::Edge * ATOM_ETMesh::CreateEdge (int v0, int v1) {
  return new Edge(v0, v1);
}

ATOM_ETMesh::Triangle * ATOM_ETMesh::CreateTriangle (int v0, int v1, int v2) {
  return new Triangle(v0,v1,v2);
}

ATOM_ETMesh::Triangle * ATOM_ETMesh::InsertTriangle (int v0, int v1, int v2) {
  TriangleKey tk(v0,v1,v2);
  if (_M_tmap.find(tk) != _M_tmap.end())
    return 0;

  Triangle * tri = _M_tc(v0,v1,v2);
  _M_tmap[tk] = tri;

  for (int i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
  {
    EdgeKey ek(tri->v[i0],tri->v[i1]);
    Edge * edge;
    EMapIterator e_it = _M_emap.find(ek);
    if ( e_it == _M_emap.end() )
    {
      edge = _M_ec(tri->v[i0],tri->v[i1]);
      _M_emap[ek] = edge;

      edge->t[0] = tri;
      tri->e[i0] = edge;
    }
    else
    {
      edge = e_it->second;
      assert ( edge );

      if ( edge->t[1] )
      {
        assert ( false );  // mesh must be manifold
        return 0;
      }
      edge->t[1] = tri;

      Triangle * adj = edge->t[0];
      assert ( adj );
      for (int i = 0; i < 3; i++)
      {
        if ( adj->e[i] == edge )
        {
          adj->t[i] = tri;
          break;
        }
      }

      tri->e[i0] = edge;
      tri->t[i0] = adj;
    }
  }

  return tri;
}

bool ATOM_ETMesh::RemoveTriangle (int v0, int v1, int v2) {
  TriangleKey tk(v0,v1,v2);
  TMapIterator t_it = _M_tmap.find(tk);
  if ( t_it == _M_tmap.end() )
    return false;

  Triangle * tri = t_it->second;
  for (int i = 0; i < 3; i++)
  {
    Edge* edge = tri->e[i];
    assert( edge );
    if ( edge->t[0] == tri )
    {
      edge->t[0] = edge->t[1];
      edge->t[1] = 0;
    }
    else if ( edge->t[1] == tri )
    {
      edge->t[1] = 0;
    }
    else
    {
      assert( false );
      return false;
    }

    if ( !edge->t[0] && !edge->t[1] )
    {
      _M_emap.erase(EdgeKey(edge->v[0], edge->v[1]));
      delete edge;
    }

    Triangle * ta = tri->t[i];
    if ( ta )
    {
      for (int j = 0; j < 3; j++)
      {
        if ( ta->t[j] == tri )
        {
            ta->t[j] = 0;
            break;
        }
      }
    }
  }

  _M_tmap.erase(tk);
  delete tri;
  return true;
}

bool ATOM_ETMesh::IsClosed () const {
  EMapConstIterator pkEIter;
  for (EMapConstIterator it = _M_emap.begin(); it != _M_emap.end(); ++it)
  {
    const Edge* edge = it->second;
    if ( !edge->t[0] || !edge->t[1] )
      return false;
  }
  return true;
}

ATOM_ETMesh::Edge::Edge (int v0, int v1)
{
  v[0] = v0;
  v[1] = v1;
  t[0] = 0;
  t[1] = 0;
}

ATOM_ETMesh::Edge::~Edge () {
}

ATOM_ETMesh::Triangle::Triangle (int v0, int v1, int v2) {
  v[0] = v0;
  v[1] = v1;
  v[2] = v2;

  for (int i = 0; i < 3; i++)
  {
    e[i] = 0;
    t[i] = 0;
  }
}

ATOM_ETMesh::Triangle::~Triangle () {
}

