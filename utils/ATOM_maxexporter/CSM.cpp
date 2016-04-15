#include "CSM.h"

//////////////////////////CSMSubMeshHeader////////////////////////////////
/*! \param n[] 子网格名称
*	\param nAF 动画帧数 
*	\param nV 顶点数
*	\param nF 面数
*/
CSMSubMeshHeader::CSMSubMeshHeader( char n[], int nAF, int nV, int nF )
:	numAnimFrames( nAF ),
numVertices( nV ),
numFaces( nF )
{
	strcpy( name, TruncateString( n ) );
	int numSkinData;
	if ( nAF > 1 )
	{
		numSkinData = numVertices;
	}
	else
	{
		numSkinData = 0;
	}
	nHeaderSize = sizeof( CSMSubMeshHeader );
	nOffVertices = nHeaderSize + MAX_STRING_LENGTH;
	nOffSkin = nOffVertices + sizeof( CSMVertexData ) * numVertices;
	nOffFaces = nOffSkin + sizeof( CSMSkinData ) * numSkinData;
	nOffEnd = nOffFaces + sizeof( CSMTriangleData ) * numFaces;
}

/////////////////////////////////CSMSubMesh////////////////////////////////
/*! \param meshName[] 子网格名称
*	\param texFile[] 纹理名称
*	\param numAnimFrame 动画帧数
*	\param numVertices 顶点数
*	\param numTriangles 面数
*/
CSMSubMesh::CSMSubMesh( char meshName[], const MaterialInfo &m, int numAnimFrame, int numVertices, int numTriangles )
: subMeshHeader( meshName, numAnimFrame, numVertices, numTriangles )
, materialInfo(m)
{	
	worldMat.makeIdentity();
}

/*! \param name[] 子网格的名称
*	\param numAnimFrames 动画帧数
*/
void CSMSubMesh::GenHeaderInfo( char name[], int numAnimFrames )
{
	memset( &subMeshHeader, 0, sizeof( subMeshHeader ) );

	strcpy( subMeshHeader.name, TruncateString( name ) );

	subMeshHeader.numAnimFrames = numAnimFrames;
	subMeshHeader.numVertices = static_cast< int >( vVertexData.size() );
	int numSkinData;
	if ( numAnimFrames > 1 )
	{
		numSkinData = static_cast< int >( vVertexData.size() );
	}
	else
	{
		numSkinData = 0;
	}
	subMeshHeader.numFaces = static_cast< int >( vTriangleData.size() );

	subMeshHeader.nHeaderSize = sizeof( CSMSubMeshHeader );
	subMeshHeader.nOffVertices = subMeshHeader.nHeaderSize + MAX_STRING_LENGTH;
	subMeshHeader.nOffSkin = subMeshHeader.nOffVertices + sizeof( CSMVertexData ) * subMeshHeader.numVertices;
	subMeshHeader.nOffFaces = subMeshHeader.nOffSkin + sizeof( CSMSkinData ) * numSkinData;
	subMeshHeader.nOffEnd = subMeshHeader.nOffFaces + sizeof( CSMTriangleData ) * subMeshHeader.numFaces;
}
