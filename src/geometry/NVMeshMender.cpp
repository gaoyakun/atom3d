/*********************************************************************NVMH4****
Path:  
File:  

Copyright NVIDIA Corporation 2003
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.


Comments:

  Todo
  -	Performance improvements, right now for each vertex I am building the list
    of it's neighbors, when we could do a single pass and build the full adjacency
	map in the beginning.  Note: I tried this and didn't see a real perf improvement.

  - I'd like to provide a non c++ standard library interface, probably 
    a simple c interface for all those simple c folks. or the 
	old nvMeshMender interface for all those using it already
  
*/

#include <set>

#include "nvMeshMender.h"

namespace
{
    const unsigned int NO_GROUP=  0xFFFFFFFF; 
};

void ATOM_MeshMender::Triangle::Reset()
{
    handled = false;
    group = NO_GROUP;
}

class ATOM_MeshMender::CanSmoothChecker
{
public:
    virtual bool CanSmooth(ATOM_MeshMender::Triangle* t1, ATOM_MeshMender::Triangle* t2, const float& minCreaseAngle)=0;
};


class CanSmoothNormalsChecker: public ATOM_MeshMender::CanSmoothChecker
{
public:
    virtual bool CanSmooth(ATOM_MeshMender::Triangle* t1, ATOM_MeshMender::Triangle* t2, const float& minCreaseAngle)
    {

	    assert(t1 && t2);
	    //for checking the angle, we want these to be normalized,
	    //they may not be for whatever reason
	    ATOM_Vector3f tmp1 = t1->normal;
	    ATOM_Vector3f tmp2 = t2->normal;
    tmp1.normalize ();
    tmp2.normalize ();

	    if(dotProduct( tmp1, tmp2 ) >= minCreaseAngle )
	    {
		    return true;
	    }
	    else if( ( tmp1 == ATOM_Vector3f(0,0,0) ) && ( tmp2 == ATOM_Vector3f(0,0,0) ) )
	    {
		    // check for them both being null, then they are 
		    // welcome to smooth no matter what the minCreaseAngle is
		    return true;
	    }
	    return false;
    }
};

class CanSmoothTangentsChecker: public ATOM_MeshMender::CanSmoothChecker
{
public:
    virtual bool CanSmooth(ATOM_MeshMender::Triangle* t1, ATOM_MeshMender::Triangle* t2, const float& minCreaseAngle)
    {

	    assert(t1 && t2);
	    //for checking the angle, we want these to be normalized,
	    //they may not be for whatever reason
	    ATOM_Vector3f tmp1 = t1->tangent;
	    ATOM_Vector3f tmp2 = t2->tangent;
        tmp1.normalize ();
        tmp2.normalize ();

	    if(dotProduct( tmp1, tmp2 ) >= minCreaseAngle )
	    {
		    return true;
	    }
	    else if( ( tmp1 == ATOM_Vector3f(0,0,0) ) && ( tmp2 == ATOM_Vector3f(0,0,0) ) )
	    {
		    // check for them both being null, then they are 
		    // welcome to smooth no matter what the minCreaseAngle is
		    return true;
	    }
	    return false;
    }
};

class CanSmoothBinormalsChecker: public ATOM_MeshMender::CanSmoothChecker
{
public:
    virtual bool CanSmooth(ATOM_MeshMender::Triangle* t1, ATOM_MeshMender::Triangle* t2, const float& minCreaseAngle)
    {

	    assert(t1 && t2);
	    //for checking the angle, we want these to be normalized,
	    //they may not be for whatever reason
	    ATOM_Vector3f tmp1 = t1->binormal;
	    ATOM_Vector3f tmp2 = t2->binormal;
    tmp1.normalize();
    tmp2.normalize();

	    if(dotProduct( tmp1, tmp2 ) >= minCreaseAngle )
		    return true;
	    else if( ( tmp1 == ATOM_Vector3f(0,0,0) ) && ( tmp2 == ATOM_Vector3f(0,0,0) ) )
	    {
		    // check for them both being null, then they are 
		    // welcome to smooth no matter what the minCreaseAngle is
		    return true;
	    }
	    return false;
    }
};


bool operator<( const ATOM_Vector3f& lhs, const ATOM_Vector3f& rhs )
{
    //needed to have a vertex in a map.
    //must be an absolute sort so that we can reliably find the exact
    //position again, not a fuzzy compare for equality based on an epsilon.
    if ( lhs.x == rhs.x )
    {
        if ( lhs.y == rhs.y )
        {
            if ( lhs.z == rhs.z )
            {
                return false;
            }
            else
            {
                return ( lhs.z < rhs.z );
            }
        }
        else
        {
            return ( lhs.y < rhs.y );
        }
    }
    else
    {
        return ( lhs.x < rhs.x );
    }
}


ATOM_MeshMender::ATOM_MeshMender()
{
    MinNormalsCreaseCosAngle =   0.3f; 
    MinTangentsCreaseCosAngle =  0.0f;
    MinBinormalsCreaseCosAngle = 0.0f;	
    WeightNormalsByArea = 0.0f;
    m_RespectExistingSplits = DONT_RESPECT_SPLITS;

}
ATOM_MeshMender::~ATOM_MeshMender()
{

}

void ATOM_MeshMender::UpdateIndices(const size_t oldIndex , const size_t newIndex , TriangleList& curGroup)
{
  //make any triangle that used the oldIndex use the newIndex instead

    for( size_t t = 0; t < curGroup.size(); ++t )
    {
	    TriID tID = curGroup[ t ];
	    for(size_t indx = 0 ; indx < 3 ; ++indx)
	    {
		    if(m_Triangles[tID].indices[indx] == oldIndex)
		    {
			    m_Triangles[tID].indices[indx] = newIndex;
		    }
	    }
    }

}
void ATOM_MeshMender::ProcessNormals(TriangleList& possibleNeighbors,
							    std::vector<Vertex>& theVerts,
							    std::vector<unsigned>& mappingNewToOldVert,
							    ATOM_Vector3f workingPosition)
{
	    NeighborGroupList neighborGroups;//a fresh group for each pass
		unsigned int i;

	    //reset each triangle to prepare for smoothing group building
	    for( i = 0; i < possibleNeighbors.size(); ++i )
	    {
		    m_Triangles[ possibleNeighbors[i] ].Reset();
	    }

	    //now start building groups
	    CanSmoothNormalsChecker canSmoothNormalsChecker;
	    for( i = 0; i < possibleNeighbors.size(); ++i )
	    {
		    Triangle* currTri = &(m_Triangles[ possibleNeighbors[i] ]);
		    assert(currTri);
		    if(!currTri->handled)
		    {
			    BuildGroups(currTri,possibleNeighbors, 
						    neighborGroups, theVerts,
						    &canSmoothNormalsChecker ,MinNormalsCreaseCosAngle );
		    }
	    }

  	
	    std::vector<ATOM_Vector3f> groupNormalVectors;

	    for( i = 0; i < neighborGroups.size(); ++i )
	    {
		    //for each group, calculate the group normal
		    TriangleList& curGroup = neighborGroups[ i ];
		    ATOM_Vector3f gnorm( 0.0f, 0.0f, 0.0f );

		    assert(curGroup.size()!=0 && "should not be a zero group here.");
		    for( size_t t = 0; t < curGroup.size(); ++t )//for each triangle in the group, 
		    {
			    TriID tID = curGroup[ t ];
			    gnorm +=  m_Triangles[ tID ].normal;
		    }
            gnorm.normalize();
		    groupNormalVectors.push_back( gnorm );
	    }
  	

	    //next step, ensure that triangles in different groups are not
	    //sharing vertices. and give the shared vertex their new group vector
	    std::set<size_t> otherGroupsIndices;
	    for( i = 0; i < neighborGroups.size(); ++i )
	    {
		    TriangleList& curGroup = neighborGroups[ i ];
		    std::set<size_t> thisGroupIndices;

		    for( size_t t = 0; t < curGroup.size(); ++t ) //for each tri
		    {
			    TriID tID = curGroup[ t ];
			    for(size_t indx = 0; indx < 3 ; ++indx)//for each vert in that tri
			    {
				    //if it is at the positions in question
				    if( theVerts[ m_Triangles[tID].indices[indx] ].pos  == workingPosition)
				    {
					    //see if another group is already using this vert
					    if(otherGroupsIndices.find( m_Triangles[tID].indices[indx] ) != otherGroupsIndices.end() )
					    {
						    //then we need to make a new vertex
						    Vertex ov;
						    ov = theVerts[ m_Triangles[tID].indices[indx] ];
						    ov.normal = groupNormalVectors[i];
						    size_t oldIndex = m_Triangles[tID].indices[indx];
						    size_t newIndex = theVerts.size();
						    theVerts.push_back(ov);
						    AppendToMapping( oldIndex , m_originalNumVerts , mappingNewToOldVert);
						    UpdateIndices(oldIndex,newIndex,curGroup);
					    }
					    else
					    {
						    //otherwise, just update it with the new vector
                            unsigned n = m_Triangles[tID].indices[indx];
						    theVerts[ m_Triangles[tID].indices[indx] ].normal = groupNormalVectors[i];
					    }

					    //store that we have used this index, so other groups can check
					    thisGroupIndices.insert(m_Triangles[tID].indices[indx]);
				    }
			    }
  			
		    }
  		
		    for(std::set<size_t>::iterator it = thisGroupIndices.begin(); it!= thisGroupIndices.end() ; ++it)
		    {
			    otherGroupsIndices.insert(*it);
		    }

	    }
  
}

void ATOM_MeshMender::ProcessTangents(TriangleList& possibleNeighbors,
							    std::vector< Vertex >&    theVerts,
							    std::vector< unsigned int >& mappingNewToOldVert,
							    ATOM_Vector3f workingPosition)
{
	    NeighborGroupList neighborGroups;//a fresh group for each pass
		unsigned int i;

	    //reset each triangle to prepare for smoothing group building
	    for(i =0; i < possibleNeighbors.size(); ++i)
	    {
		    m_Triangles[ possibleNeighbors[i] ].Reset();
	    }

	    //now start building groups
	    CanSmoothTangentsChecker canSmoothTangentsChecker;
	    for(i =0; i < possibleNeighbors.size(); ++i)
	    {
		    Triangle* currTri = &(m_Triangles[ possibleNeighbors[i] ]);
		    assert(currTri);
		    if(!currTri->handled)
		    {
			    BuildGroups(currTri,possibleNeighbors, 
						    neighborGroups, theVerts,
						    &canSmoothTangentsChecker,MinTangentsCreaseCosAngle  );
		    }
	    }


	    std::vector<ATOM_Vector3f> groupTangentVectors;
  
  	
	    for(i=0; i<neighborGroups.size(); ++i)
	    {
		    ATOM_Vector3f gtang(0,0,0);
		    for(unsigned int t = 0; t < neighborGroups[i].size(); ++t)//for each triangle in the group, 
		    {
			    TriID tID = neighborGroups[i][t];
			    gtang+=  m_Triangles[tID].tangent;
		    }
            gtang.normalize();
		    groupTangentVectors.push_back(gtang);
	    }
  	
	    //next step, ensure that triangles in different groups are not
	    //sharing vertices. and give the shared vertex their new group vector
	    std::set<size_t> otherGroupsIndices;
	    for( i = 0; i < neighborGroups.size(); ++i )
	    {
		    TriangleList& curGroup = neighborGroups[ i ];
		    std::set<size_t> thisGroupIndices;

		    for( size_t t = 0; t < curGroup.size(); ++t ) //for each tri
		    {
			    TriID tID = curGroup[ t ];
			    for(size_t indx = 0; indx < 3 ; indx ++)//for each vert in that tri
			    {
				    //if it is at the positions in question
				    if( theVerts[ m_Triangles[tID].indices[indx] ].pos  == workingPosition)
				    {
					    //see if another group is already using this vert
					    if(otherGroupsIndices.find( m_Triangles[tID].indices[indx] ) != otherGroupsIndices.end() )
					    {
						    //then we need to make a new vertex
						    Vertex ov;
						    ov = theVerts[ m_Triangles[tID].indices[indx] ];
						    ov.tangent = groupTangentVectors[i];
						    size_t oldIndex = m_Triangles[tID].indices[indx];
						    size_t newIndex = theVerts.size();
						    theVerts.push_back(ov);
						    AppendToMapping( oldIndex , m_originalNumVerts , mappingNewToOldVert);
						    UpdateIndices(oldIndex,newIndex,curGroup);
					    }
					    else
					    {
						    //otherwise, just update it with the new vector
						    theVerts[ m_Triangles[tID].indices[indx] ].tangent = groupTangentVectors[i];
					    }

					    //store that we have used this index, so other groups can check
					    thisGroupIndices.insert(m_Triangles[tID].indices[indx]);
				    }
			    }
  			
		    }
  		
		    for(std::set<size_t>::iterator it = thisGroupIndices.begin(); it!= thisGroupIndices.end() ; ++it)
		    {
			    otherGroupsIndices.insert(*it);
		    }

	    }


}


void ATOM_MeshMender::ProcessBinormals(TriangleList& possibleNeighbors,
							    std::vector< Vertex >&    theVerts,
							    std::vector< unsigned int >& mappingNewToOldVert,
							    ATOM_Vector3f workingPosition)
{
	    NeighborGroupList neighborGroups;//a fresh group for each pass
		unsigned int i;

	    //reset each triangle to prepare for smoothing group building
	    for(i =0; i < possibleNeighbors.size(); ++i )
	    {
		    m_Triangles[ possibleNeighbors[i] ].Reset();
	    }

	    //now start building groups
	    CanSmoothBinormalsChecker canSmoothBinormalsChecker;
	    for(i =0; i < possibleNeighbors.size(); ++i )
	    {
		    Triangle* currTri = &(m_Triangles[ possibleNeighbors[i] ]);
		    assert(currTri);
		    if(!currTri->handled)
		    {
			    BuildGroups(currTri,possibleNeighbors, 
						    neighborGroups, theVerts,
						    &canSmoothBinormalsChecker ,MinBinormalsCreaseCosAngle );
		    }
	    }


	    std::vector<ATOM_Vector3f> groupBinormalVectors;
  
  	
	    for(i=0; i<neighborGroups.size(); ++i)
	    {
		    ATOM_Vector3f gbinormal(0,0,0);
		    for(unsigned int t = 0; t < neighborGroups[i].size(); ++t)//for each triangle in the group, 
		    {
			    TriID tID = neighborGroups[i][t];
			    gbinormal+=  m_Triangles[tID].binormal;
		    }
            gbinormal.normalize();
		    groupBinormalVectors.push_back(gbinormal);
	    }
  	
	    //next step, ensure that triangles in different groups are not
	    //sharing vertices. and give the shared vertex their new group vector
	    std::set<size_t> otherGroupsIndices;
	    for( i = 0; i < neighborGroups.size(); ++i )
	    {
		    TriangleList& curGroup = neighborGroups[ i ];
		    std::set<size_t> thisGroupIndices;

		    for( size_t t = 0; t < curGroup.size(); ++t ) //for each tri
		    {
			    TriID tID = curGroup[ t ];
			    for(size_t indx = 0; indx < 3 ; ++indx)//for each vert in that tri
			    {
				    //if it is at the positions in question
				    if( theVerts[ m_Triangles[tID].indices[indx] ].pos  == workingPosition)
				    {
					    //see if another group is already using this vert
					    if(otherGroupsIndices.find( m_Triangles[tID].indices[indx] ) != otherGroupsIndices.end() )
					    {
						    //then we need to make a new vertex
						    Vertex ov;
						    ov = theVerts[ m_Triangles[tID].indices[indx] ];
						    ov.binormal = groupBinormalVectors[i];
						    size_t oldIndex = m_Triangles[tID].indices[indx];
						    size_t newIndex = theVerts.size();
						    theVerts.push_back(ov);
						    AppendToMapping( oldIndex , m_originalNumVerts , mappingNewToOldVert);
						    UpdateIndices(oldIndex,newIndex,curGroup);
					    }
					    else
					    {
						    //otherwise, just update it with the new vector
						    theVerts[ m_Triangles[tID].indices[indx] ].binormal = groupBinormalVectors[i];
					    }

					    //store that we have used this index, so other groups can check
					    thisGroupIndices.insert(m_Triangles[tID].indices[indx]);
				    }
			    }
  			
		    }
  		
		    for(std::set<size_t>::iterator it = thisGroupIndices.begin(); it!= thisGroupIndices.end() ; ++it)
		    {
			    otherGroupsIndices.insert(*it);
		    }

	    }

}


bool ATOM_MeshMender::Mend( 
                  std::vector< Vertex >&    theVerts,
			      std::vector< unsigned int >& theIndices,
			      std::vector< unsigned int >& mappingNewToOldVert,
			      const float minNormalsCreaseCosAngle,
			      const float minTangentsCreaseCosAngle,
			      const float minBinormalsCreaseCosAngle,
			      const float weightNormalsByArea,
			      const NormalCalcOption computeNormals,
			      const ExistingSplitOption respectExistingSplits,
			      const CylindricalFixOption fixCylindricalWrapping)
{
    MinNormalsCreaseCosAngle = minNormalsCreaseCosAngle;
    MinTangentsCreaseCosAngle= minTangentsCreaseCosAngle;
    MinBinormalsCreaseCosAngle = minBinormalsCreaseCosAngle;
    WeightNormalsByArea = weightNormalsByArea;
    m_RespectExistingSplits = respectExistingSplits;

    //fix cylindrical should happen before we do any other calculations
    if(fixCylindricalWrapping == FIX_CYLINDRICAL)
    {
	    FixCylindricalWrapping( theVerts , theIndices , mappingNewToOldVert );
    }


    SetUpData( theVerts, theIndices, mappingNewToOldVert, computeNormals);

    //for each unique position
    for(VertexChildrenMap::iterator vert = m_VertexChildrenMap.begin();
	    vert!= m_VertexChildrenMap.end();
	    ++vert)
    {
	    ATOM_Vector3f workingPosition = vert->first;

	    TriangleList& possibleNeighbors = vert->second;
	    if(computeNormals == CALCULATE_NORMALS)
	    {
		    ProcessNormals(possibleNeighbors, theVerts, mappingNewToOldVert, workingPosition );
	    }
	    ProcessTangents(possibleNeighbors, theVerts, mappingNewToOldVert, workingPosition );
	    ProcessBinormals(possibleNeighbors, theVerts, mappingNewToOldVert, workingPosition );
    }

    UpdateTheIndicesWithFinalIndices(theIndices );
    OrthogonalizeTangentsAndBinormals(theVerts);
  	
    return true;
}

void ATOM_MeshMender::BuildGroups(	Triangle* tri, //the tri of interest
							    TriangleList& possibleNeighbors, //all tris arround a vertex
							    NeighborGroupList& neighborGroups, //the neighbor groups to be updated
							    std::vector< Vertex >& theVerts,
							    CanSmoothChecker* smoothChecker,
							    const float& minCreaseAngle)
{
    if( (!tri)  ||  (tri->handled) )
	    return;
  
    Triangle* neighbor1 = NULL;
    Triangle* neighbor2 = NULL;

    FindNeighbors(tri, possibleNeighbors, &neighbor1, &neighbor2,theVerts);
  
    //see if I can join my first neighbors group
    if(neighbor1 && (neighbor1->group != NO_GROUP))
    {
	    if( smoothChecker->CanSmooth(tri,neighbor1, minCreaseAngle) )
	    {
		    neighborGroups[neighbor1->group].push_back(tri->myID);
		    tri->group = neighbor1->group;
	    }
    }

    //see if I can join my second neighbors group
    if(neighbor2 && (neighbor2->group!= NO_GROUP))
    {
	    if( smoothChecker->CanSmooth(tri,neighbor2, minCreaseAngle) )
	    {
		    neighborGroups[neighbor2->group].push_back(tri->myID);
		    tri->group = neighbor2->group;
	    }
    }
    //I either couldn't join, or they weren't in a group, so I think I'll
    //just go and start my own group...right here we go.
    if(tri->group == NO_GROUP)
    {
	    tri->group = neighborGroups.size();
	    neighborGroups.push_back( TriangleList() );
	    neighborGroups.back().push_back(tri->myID);

    }
    assert((tri->group != NO_GROUP) && "error!: tri should have a group set");
    tri->handled = true;

    //continue growing our group with each neighbor.
    BuildGroups(neighbor1,possibleNeighbors,neighborGroups,theVerts,smoothChecker,minCreaseAngle);
    BuildGroups(neighbor2,possibleNeighbors,neighborGroups,theVerts,smoothChecker,minCreaseAngle);
}


void ATOM_MeshMender::FindNeighbors(Triangle* tri, 
			      TriangleList&possibleNeighbors, 
			      Triangle** neighbor1, 
			      Triangle** neighbor2,
			      std::vector< Vertex >& theVerts)
{
    *neighbor1 = NULL;
    *neighbor2 = NULL;

    std::vector<Triangle*> theNeighbors;
    for(unsigned int n = 0; n < possibleNeighbors.size(); ++n)
    {
	    TriID tID = possibleNeighbors[n];
	    Triangle* possible =&(m_Triangles[ tID]);
	    if(possible != tri ) //check for myself
	    {
		    if( SharesEdge(tri, possible, theVerts)  )
		    {
			    theNeighbors.push_back(possible);  

		    }
	    }
    }
  
    if(theNeighbors.size()>0)
	    *neighbor1 = theNeighbors[0];
    if(theNeighbors.size()>1)
	    *neighbor2 = theNeighbors[1];
}



bool ATOM_MeshMender::TriHasEdge(const size_t& p0,
						    const size_t& p1,
						    const size_t& triA,
						    const size_t& triB,
						    const size_t& triC)
{
    if ( ( ( p0 == triB ) && ( p1 == triA ) ) ||
        ( ( p0 == triA ) && ( p1 == triB ) ) )
    {
	    return true;
    }

    if ( ( ( p0 == triB ) && ( p1 == triC ) ) ||
        ( ( p0 == triC ) && ( p1 == triB ) ) )
    {
	    return true;
    }

    if ( ( ( p0 == triC ) && ( p1 == triA ) ) ||
        ( ( p0 == triA ) && ( p1 == triC ) ) )
    {
	    return true;
    }
    return false;
}

bool ATOM_MeshMender::TriHasEdge(const ATOM_Vector3f& p0,
						    const ATOM_Vector3f& p1,
						    const ATOM_Vector3f& triA,
						    const ATOM_Vector3f& triB,
						    const ATOM_Vector3f& triC)
{
    if ( ( ( p0 == triB ) && ( p1 == triA ) ) ||
        ( ( p0 == triA ) && ( p1 == triB ) ) )
    {
	    return true;
    }

    if ( ( ( p0 == triB ) && ( p1 == triC ) ) ||
        ( ( p0 == triC ) && ( p1 == triB ) ) )
    {
	    return true;
    }

    if ( ( ( p0 == triC ) && ( p1 == triA ) ) ||
        ( ( p0 == triA ) && ( p1 == triC ) ) )
    {
	    return true;
    }
    return false;
}

bool ATOM_MeshMender::SharesEdgeRespectSplits(	Triangle* triA, 
								    Triangle* triB,
			    std::vector< Vertex >& theVerts)
{
    assert(triA && triB && "invalid data passed to SharesEdgeNoSplit");
    //here we want to compare based solely on indices.

    size_t a1 = triA->indices[0];
    size_t b1 = triA->indices[1];
    size_t c1 = triA->indices[2];

    size_t a2 = triB->indices[0];
    size_t b2 = triB->indices[1];
    size_t c2 = triB->indices[2];

    //edge B1->A1
    if( TriHasEdge(b1,a1,a2,b2,c2)  )
	    return true;

    //edge A1->C1
    if( TriHasEdge(a1,c1,a2,b2,c2)  )
	    return true;
  
    //edge C1->B1
    if( TriHasEdge(c1,b1,a2,b2,c2)  )
	    return true;

    return false;
}

bool ATOM_MeshMender::SharesEdge(Triangle* triA, 
			    Triangle* triB,
			    std::vector< Vertex >& theVerts)
{
    assert(triA && triB && "invalid data passed to SharesEdge");

    //check based on position not on indices, because there may be splits
    //we don't care about. unless the user has told us they care about those
    //splits
    if(m_RespectExistingSplits == RESPECT_SPLITS)
    {
	    return SharesEdgeRespectSplits(triA, triB, theVerts);
    }

    ATOM_Vector3f a1 = theVerts[ triA->indices[0] ].pos;
    ATOM_Vector3f b1 = theVerts[ triA->indices[1] ].pos;
    ATOM_Vector3f c1 = theVerts[ triA->indices[2] ].pos;

    ATOM_Vector3f a2 = theVerts[ triB->indices[0] ].pos;
    ATOM_Vector3f b2 = theVerts[ triB->indices[1] ].pos;
    ATOM_Vector3f c2 = theVerts[ triB->indices[2] ].pos;

    //edge B1->A1
    if( TriHasEdge(b1,a1,a2,b2,c2)  )
	    return true;

    //edge A1->C1
    if( TriHasEdge(a1,c1,a2,b2,c2)  )
	    return true;
  
    //edge C1->B1
    if( TriHasEdge(c1,b1,a2,b2,c2)  )
	    return true;
  
    return false;
}

void ATOM_MeshMender::SetUpData(
			      std::vector< Vertex >&    theVerts,
			      std::vector< unsigned int >& theIndices,
			      std::vector< unsigned int >& mappingNewToOldVert,
			      const NormalCalcOption computeNormals)
{
    assert( ((theIndices.size()%3 )== 0) && "expected the indices to be a multiple of 3");
    unsigned int i;

    //initialize the mapping
    for(i = 0 ; i < theVerts.size() ; ++i)
	    mappingNewToOldVert.push_back(i);

    m_originalNumVerts = theVerts.size();

    //set up our triangles
    for(i = 0; i < theIndices.size() ; i += 3)
    {
	    Triangle t;

	    t.indices[0] = theIndices[i+0];
	    t.indices[1] = theIndices[i+1];
	    t.indices[2] = theIndices[i+2];

	    //set up bin, norm, and tan
	    SetUpFaceVectors(t,theVerts, computeNormals);
  	
	    t.myID = m_Triangles.size();//set id, to my index into m_Triangles
	    m_Triangles.push_back(t);
    }

    //build vertex position/traingle pairings.
    //we use the position and not the actual vertex, because there may
    //be multiple coppies of the same vertex for textureing
    //but we don't want that to 
    //effect our decisions about normal smoothing.  
    //note: maybe this should be an option, the position thing.
    for(i= 0; i< m_Triangles.size();++i)
    {

	    for(size_t indx = 0 ; indx < 3 ; ++indx )
	    {
		    ATOM_Vector3f v = theVerts[m_Triangles[i].indices[indx]].pos;
		    VertexChildrenMap::iterator iter = m_VertexChildrenMap.find( v );
		    if(iter != m_VertexChildrenMap.end())
		    {
			    //we found it, so just add ourselves to it.
			    iter->second.push_back(TriID(i));
		    }
		    else
		    {
			    //we didn't find it so join whatever was there.
			    std::vector<TriID> tmp;
			    m_VertexChildrenMap[v] = tmp;
			    m_VertexChildrenMap[v].push_back( TriID(i) );
		    }
	    }
    }
}

//sets up the normal, binormal, and tangent for a triangle
//assumes the triangle indices are set to match whats in the verts
void ATOM_MeshMender::SetUpFaceVectors(Triangle& t, 
							      const std::vector< Vertex >&verts, 
							      const NormalCalcOption computeNormals)
{

    if(computeNormals == CALCULATE_NORMALS)
    { 
	    ATOM_Vector3f edge0 = verts[t.indices[1]].pos - verts[t.indices[0]].pos;
	    ATOM_Vector3f edge1 = verts[t.indices[2]].pos - verts[t.indices[0]].pos;

    t.normal = crossProduct (edge0, edge1);

	    if( WeightNormalsByArea < 1.0f )
	    {
  		
		    ATOM_Vector3f normalizedNorm;
      normalizedNorm = t.normal;
      normalizedNorm.normalize ();
		    ATOM_Vector3f finalNorm = (normalizedNorm * (1.0f - WeightNormalsByArea))
						    + (t.normal * WeightNormalsByArea);
		    t.normal = finalNorm;
	    }

    }
    //need to set up tangents, and binormals here
    GetGradients(  verts[ t.indices[0] ],
                  verts[ t.indices[1] ],
                  verts [t.indices[2] ],
                  t.tangent,
                  t.binormal);
}

void ATOM_MeshMender::OrthogonalizeTangentsAndBinormals( 
					    std::vector< Vertex >&   theVerts )
{
    //put our tangents and binormals through the final orthogonalization
    //with the final processed normals
    size_t len = theVerts.size();
    for(size_t i = 0 ; i < len ; ++ i )
    {
#if 0
	    assert(theVerts[i].normal.getLength() > 0.00001f && 
		    "found zero length normal when calculating tangent basis!,\
		    if you are not using mesh mender to compute normals, you\
		    must still pass in valid normals to be used when calculating\
		    tangents and binormals.");
#endif

	    //now with T and B and N we can get from tangent space to object space
	    //but we want to go the other way, so we need the inverse
	    //of the T, B,N matrix
	    //we can use the Gram-Schmidt algorithm to find the newTangent and the newBinormal
	    //newT = T - (N dotProduct T)N
	    //newB = B - (N dotProduct B)N - (newT dotProduct B)newT

	    //NOTE: this should maybe happen with the final smoothed N, T, and B
	    //will try it here and see what the results look like

	    ATOM_Vector3f tmpTan = theVerts[i].tangent;
	    ATOM_Vector3f tmpNorm = theVerts[i].normal;
	    ATOM_Vector3f tmpBin = theVerts[i].binormal;


	    ATOM_Vector3f newT = tmpTan -  (dotProduct(tmpNorm , tmpTan)  * tmpNorm );
	    ATOM_Vector3f newB = tmpBin - (dotProduct(tmpNorm , tmpBin) * tmpNorm)
						    - (dotProduct(newT,tmpBin)*newT);

    theVerts[i].tangent = newT;
    theVerts[i].tangent.normalize();
    theVerts[i].binormal = newB;
    theVerts[i].binormal.normalize();

	    //this is where we can do a final check for zero length vectors
	    //and set them to something appropriate
	    float lenTan = theVerts[i].tangent.getLength();
	    float lenBin = theVerts[i].binormal.getLength();

	    if( (lenTan <= 0.001f) || (lenBin <= 0.001f)  ) //should be approx 1.0f
	    {	
		    //the tangent space is ill defined at this vertex
		    //so we can generate a valid one based on the normal vector,
		    //which I'm assuming is valid!

		    if(lenTan > 0.5f)
		    {
			    //the tangent is valid, so we can just use that
			    //to calculate the binormal
        theVerts[i].binormal = crossProduct(theVerts[i].normal, theVerts[i].tangent);

		    }
		    else if(lenBin > 0.5)
		    {
			    //the binormal is good and we can use it to calculate
			    //the tangent
        theVerts[i].tangent = crossProduct(theVerts[i].binormal, theVerts[i].normal);
		    }
		    else
		    {
			    //both vectors are invalid, so we should create something
			    //that is at least valid if not correct
			    ATOM_Vector3f xAxis( 1.0f , 0.0f , 0.0f);
			    ATOM_Vector3f yAxis( 0.0f , 1.0f , 0.0f);
			    //I'm checking two possible axis, because the normal could be one of them,
			    //and we want to chose a different one to start making our valid basis.
			    //I can find out which is further away from it by checking the dotProduct product
			    ATOM_Vector3f startAxis;

			    if( dotProduct(xAxis, theVerts[i].normal)  <  dotProduct(yAxis, theVerts[i].normal))
			    {
				    //the xAxis is more different than the yAxis when compared to the normal
				    startAxis = xAxis;
			    }
			    else
			    {
				    //the yAxis is more different than the xAxis when compared to the normal
				    startAxis = yAxis;
			    }

        theVerts[i].tangent = crossProduct(theVerts[i].normal, startAxis);
        theVerts[i].binormal = crossProduct(theVerts[i].normal, theVerts[i].tangent);
		    }
	    }
	    else
	    {
		    //one final sanity check, make sure that they tangent and binormal are different enough
		    if( dotProduct(theVerts[i].binormal, theVerts[i].tangent )  > 0.999f )
		    {
			    //then they are too similar lets make them more different
        theVerts[i].binormal = crossProduct(theVerts[i].normal, theVerts[i].tangent);
		    }

	    }

    }
  
}

void ATOM_MeshMender::GetGradients( const ATOM_MeshMender::Vertex& v0,
                              const ATOM_MeshMender::Vertex& v1,
                              const ATOM_MeshMender::Vertex& v2,
                              ATOM_Vector3f& tangent,
                              ATOM_Vector3f& binormal) const
{
    //using Eric Lengyel's approach with a few modifications
    //from Mathematics for 3D Game Programmming and Computer Graphics
    // want to be able to trasform a vector in Object Space to Tangent Space
    // such that the x-axis cooresponds to the 's' direction and the
    // y-axis corresponds to the 't' direction, and the z-axis corresponds
    // to <0,0,1>, straight up out of the texture map

    //let P = v1 - v0
    ATOM_Vector3f P = v1.pos - v0.pos;
    //let Q = v2 - v0
    ATOM_Vector3f Q = v2.pos - v0.pos;
    float s1 = v1.s - v0.s;
    float t1 = v1.t - v0.t;
    float s2 = v2.s - v0.s;
    float t2 = v2.t - v0.t;


    //we need to solve the equation
    // P = s1*T + t1*B
    // Q = s2*T + t2*B
    // for T and B


    //this is a linear system with six unknowns and six equatinos, for TxTyTz BxByBz
    //[px,py,pz] = [s1,t1] * [Tx,Ty,Tz]
    // qx,qy,qz     s2,t2     Bx,By,Bz

    //multiplying both sides by the inverse of the s,t matrix gives
    //[Tx,Ty,Tz] = 1/(s1t2-s2t1) *  [t2,-t1] * [px,py,pz]
    // Bx,By,Bz                      -s2,s1	    qx,qy,qz  

    //solve this for the unormalized T and B to get from tangent to object space

  
    float tmp = 0.0f;
    if(fabsf(s1*t2 - s2*t1) <= 0.0001f)
    {
	    tmp = 1.0f;
    }
    else
    {
	    tmp = 1.0f/(s1*t2 - s2*t1 );
    }
  
    tangent.x = (t2*P.x - t1*Q.x);
    tangent.y = (t2*P.y - t1*Q.y);
    tangent.z = (t2*P.z - t1*Q.z);
  
    tangent = tmp * tangent;

    binormal.x = (s1*Q.x - s2*P.x);
    binormal.y = (s1*Q.y - s2*P.y);
    binormal.z = (s1*Q.z - s2*P.z);

    binormal = tmp * binormal;



    //after these vectors are smoothed together,
    //they must be again orthogonalized with the final normals
    // see OrthogonalizeTangentsAndBinormals

}

void ATOM_MeshMender::UpdateTheIndicesWithFinalIndices(std::vector< unsigned int >& theIndices )
{
    //theIndices is assumed to be filled with a copy of the in Indices.

    assert(((theIndices.size()/3) == m_Triangles.size()) && "invalid number of tris, or indices.");
    //Note that we do not change the number or the order of indices at all,
    //so we just need to copy the triangles indices to the output.
    size_t oIndex = 0;

    for( size_t i = 0; i < m_Triangles.size();++i )
    {
	    theIndices[oIndex+0] = m_Triangles[i].indices[0];
	    theIndices[oIndex+1] = m_Triangles[i].indices[1];
	    theIndices[oIndex+2] = m_Triangles[i].indices[2];
	    oIndex+=3;
    }
}


void ATOM_MeshMender::FixCylindricalWrapping(	std::vector< Vertex >& theVerts , 
							    std::vector< unsigned int >& theIndices,
							    std::vector< unsigned int >& mappingNewToOldVert)
{
    //when using cylindrical texture coordinate generation,
    //you can end up with triangles that have <s,t> coordinates like
    // <0,0.9> -------------> <0,0>
    // and
    // <0,0.9> -------------> <0,0.1>
    //this will cause the texture to be mapped from 0.9 back to 0.0 or 0.1 when
    //what you really want it to do is 
    //wrap arround to 1.0, then start from 0.0 again.
    //to fix this, we can duplicate a vertex and add 1.0 to the wrapped texture coordinate
    //we need to do this for both the S and the T directions.

    size_t index;
    for(index = 0 ;  index < theIndices.size() ; index += 3)
    {
	    //for each triangle
	    std::set<unsigned int> alreadyDuped;

  	
	    for( unsigned int begin = 0 ; begin < 3 ; ++begin)
	    {

		    unsigned int end = begin +1;
		    if(begin == 2)
			    end = 0;
		    //for each   begin -> end   edge

		    float sBegin = theVerts[ theIndices[index + begin] ].s;
		    float sEnd = theVerts[ theIndices[index + end] ].s;

		    if( sBegin <= 1.0f  && sEnd <= 1.0f  && sBegin >= 0.0f  && sEnd >= 0.0f  )
		    {
			    //we only handle coordinates between 0 and 1 for the cylindrical wrappign fix
			    if( fabsf(sBegin - sEnd) > 0.5f )
			    {
				    unsigned int theOneToDupe = begin;
				    //we have some wrapping going on.
				    if(sBegin > sEnd)
					    theOneToDupe = end;

				    if(alreadyDuped.find(theOneToDupe) == alreadyDuped.end())
				    {
					    size_t oldIndex = theIndices[index + theOneToDupe];
					    Vertex theDupe = theVerts[ oldIndex ];
					    alreadyDuped.insert(theOneToDupe);
					    theDupe.s += 1.0f;
					    theIndices[index + theOneToDupe] = theVerts.size();
					    theVerts.push_back(theDupe);
					    AppendToMapping( oldIndex , m_originalNumVerts , mappingNewToOldVert);
				    }
				    else
				    {
					    theVerts[ theIndices[index + theOneToDupe] ].s += 1.0f;
				    }
			    }
		    }



		    float tBegin = theVerts[ theIndices[index + begin] ].t;
		    float tEnd = theVerts[ theIndices[index + end] ].t;

		    if( tBegin <= 1.0f  && tEnd <= 1.0f  && tBegin >= 0.0f  && tEnd >= 0.0f  )
		    {
			    //we only handle coordinates between 0 and 1 for the cylindrical wrappign fix
			    if( fabsf(tBegin - tEnd) > 0.5f )
			    {
				    unsigned int theOneToDupe = begin;
				    //we have some wrapping going on.
				    if(tBegin > tEnd)
					    theOneToDupe = end;

				    if(alreadyDuped.find(theOneToDupe) == alreadyDuped.end())
				    {
					    size_t oldIndex = theIndices[index + theOneToDupe];
					    Vertex theDupe = theVerts[ oldIndex ];
					    alreadyDuped.insert(theOneToDupe);
					    theDupe.t += 1.0f;
					    theIndices[index + theOneToDupe] = theVerts.size();
					    theVerts.push_back(theDupe);
					    AppendToMapping( oldIndex , m_originalNumVerts , mappingNewToOldVert);
				    }
				    else
				    {
					    theVerts[ theIndices[index + theOneToDupe] ].t += 1.0f;
				    }
			    }
		    }


	    }
    }

}


void ATOM_MeshMender::AppendToMapping(	const size_t oldIndex,
					    const size_t originalNumVerts,
					    std::vector< unsigned int >& mappingNewToOldVert)
{
    if( oldIndex >= originalNumVerts )
    {
	    //then this is a newer vertex we are mapping to another vertex we created in meshmender.
	    //we need to find the original old vertex index to map to.
	    // so we can just use the mapping
  
	    //that is to say, just keep the same mapping for this new one.
	    unsigned int originalVertIndex = mappingNewToOldVert[oldIndex];
	    assert( originalVertIndex < originalNumVerts );
  	
	    mappingNewToOldVert.push_back( originalVertIndex ); 
    }
    else
    {
	    //this is mapping to an original vertex
	    mappingNewToOldVert.push_back( oldIndex );
    }
}
