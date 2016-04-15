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

#ifndef _MESHER_TREE_H_
#define _MESHER_TREE_H_

#include "stemmesher.h"
#include "leafmesher.h"

#include "opentree/utils/otarray.h"

namespace opentree
{

  /// \addtogroup Mesher
  /// @{
  
  class otTree;
  class otStem;
  class otVertices;
  class otTriangles;

  /** Creates the mesh for a tree's stems
   * Helperclass which allows more comfortable mesh creating by using
   * the MesherStem class.
   */
  class MesherTree
  {
  private:
    otTree* tree;

    otArray<MesherStem> stems;

    unsigned int countStems(otStem* stem);

    void parseStems(otStem* stem);

    char leafedges;

    bool usePseudoSelfShadowing;

  public:
    MesherTree(otTree* t);

    virtual ~MesherTree();
    
    /** Adds the number of new vertices of a stemlevel to the variable vertex
     * \param level Stemlevel: 0 = trunk, 1 = branches, 2 = subbranches, etc...
     * \param vertex Pointer to an Integer to which the new amount of vertices will be added.
     */
    void getVerticesCount(int level, int* vertex);
    /** Adds the number of new triangles of a stemlevel to the variable triangle.
     * Yes, this is the IndexBuffer! One triangle equals 3 indices,
     * \param level Stemlevel: 0 = trunk, 1 = branches, 2 = subbranches, etc...
     * \param triangle Pointer to an Integer to which the new amount of triangles will be added.
     */
    void getIndicesCount(int level, int* triangle);

    /** Adds the vertices of a stemlevel to the variable vertices
     * \param level Stemlevel: 0 = trunk, 1 = branches, 2 = subbranches, etc...
     * \param vertices Object which will receive the created vertices.
     */
    void getVertices(int level, otVertices& vertices);
    /** Adds the triangles of a stemlevel to the variable triangles.
     * \param level Stemlevel: 0 = trunk, 1 = branches, 2 = subbranches, etc...
     * \param triangles Object which will receive the created triangles.
     * \param offset StartIndex of the vertexarray. This allows multiple indexbuffers for one vertexbuffer.
     */
    void getIndices(int level, otTriangles& triangles, unsigned int offset);

    /** Defines how detailed swinging slong the stem's height shall be.
     * \param level Stemlevel: 0 = trunk, 1 = branches, 2 = subbranches, etc...
     * \param res Object which will receive the created triangles.
     */
    void setCurveRes(int level, int res);
    /** Defines how detailed the rounding of the stem shall be.
     * \param level Stemlevel: 0 = trunk, 1 = branches, 2 = subbranches, etc...
     * \param res number of vertices around the stem's circle.
     */
    void setCircleRes(int level, int res);
    
    //------------------------------------------------------------------

    //void usePointLeaves() { leafedges = 1; }
    /// Set the LeafMesher to use triangles for each leaf
    void useTriangleLeaves() { leafedges = 3; }
    /// Set the LeafMesher to use quads for each leaf
    void useQuadLeaves() { leafedges = 4; }

    /** Adds the number of new vertices for all leaves to the variable vertex
     * \param vertex Pointer to an Integer to which the new amount of vertices will be added.
     */
    void getLeavesVerticesCount(int* vertex);
    /** Adds the number of new triangles for all leaves to the variable vertex.
     * Yes, this is the IndexBuffer! One triangle equals 3 indices,
     * \param triangle Pointer to an Integer to which the new amount of triangles will be added.
     */
    void getLeavesIndicesCount(int* triangle);
    
    /** Adds the vertices for all leaves to the variable vertices
     * \param vertices Object which will receive the created vertices.
     */
    void getLeavesVertices(otVertices& vertices);
    /** Adds the triangles for all leaves to the variable triangles.
     * \param triangles Object which will receive the created triangles.
     * \param offset StartIndex of the vertexarray. This allows multiple indexbuffers for one vertexbuffer.
     */
    void getLeavesIndices(otTriangles& triangles, unsigned int offset);

    //------------------------------------------------------------------

    /// Enable PseudoSelfShadowing for the leaves.
    /// This will fill the vertex color with a grey value according to the leaf's position.
    void setLeavesPseudoSelfShadowing(bool selfShadowing);
    bool getLeavesPseudoSelfShadowing();

  };
};

#endif // _MESHER_TREE_H_

