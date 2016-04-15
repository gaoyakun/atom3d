#ifndef __ATOM_GEOMETRY_ADJACENCY_H
#define __ATOM_GEOMETRY_ADJACENCY_H

#if _MSC_VER > 1000
# pragma once
#endif

#define		MAKE_ADJ_TRI(x)			(x&0x3fffffff)
#define		GET_EDGE_NB(x)			(x>>30)
#define		IS_BOUNDARY(x)			(x==0xffffffff)

  struct AdjTriangle  // Should be derived from a triangle structure
{									
    unsigned VRef[3];		// Vertex-references
	  unsigned ATri[3];		// Links/References of adjacent triangles. The 2 most significant bits contains
											  // the counterpart edge in the adjacent triangle.
	  unsigned char FindEdge(unsigned vref0, unsigned vref1);
	  unsigned OppositeVertex(unsigned vref0, unsigned vref1);
  };

  struct AdjEdge
{
	  unsigned Ref0;			// Vertex reference
	  unsigned Ref1;			// Vertex reference
	  unsigned FaceNb;		// Owner face
  };

  struct ADJACENCIESCREATE
{
    ADJACENCIESCREATE()		{ DFaces = 0; WFaces = 0; NbFaces = 0; }
  unsigned	NbFaces;		// #faces in source topo
	  unsigned*	DFaces;			// list of faces (dwords) or 0
	  unsigned short*	WFaces;			// list of faces (words) or 0
  };

  class Adjacencies
  {
  public:
	  Adjacencies();
	  ~Adjacencies();
	  unsigned mNbFaces;
	  AdjTriangle* mFaces;
	  bool Init(ADJACENCIESCREATE& create);
	  bool CreateDatabase();
  private:
	  unsigned mNbEdges;
	  unsigned mCurrentNbFaces;
	  AdjEdge* mEdges;
	  bool AddTriangle(unsigned ref0, unsigned ref1, unsigned ref2);
	  bool AddEdge(unsigned ref0, unsigned ref1, unsigned face);
	  bool UpdateLink(unsigned firsttri, unsigned secondtri, unsigned ref0, unsigned ref1);
	  unsigned char FindEdge(AdjTriangle* tri, unsigned vref0, unsigned vref1);
  };

#endif // __ATOM_GEOMETRY_ADJACENCY_H

