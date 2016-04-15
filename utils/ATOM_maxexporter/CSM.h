#ifndef __CSM_H__
#define __CSM_H__

#include "Header.h"

struct MaterialInfo
{
	std::string name;
	string texture;
	bool alphaTest;
	bool doubleSide;

	MaterialInfo (void)
	{
		alphaTest = false;
		doubleSide = false;
	}
};

/*! \brief CSM文件的Header */
struct CSMHeader
{
	int ident;				//!< 用于标明和识别.CSM文件的魔数，必须等于CSM1

	int numTags;			//!< 标记的数量
	int numAnimFrames;		//!< 动画的帧数量
	int numBones;			//!< 骨骼的数量
	int numSubMesh;			//!< 子网格的数量

	int nHeaderSize;		//!< Header的字节大小，同时也等于到Tag Data的字节偏移量
	int nOffBones;			//!< 到骨骼信息的字节偏移量
	int nOffSubMesh;		//!< 到Sub Mesh的字节偏移量
	int nFileSize;			//!< 整个CSM文件的大小

	CSMHeader()
		:ident( 0 ),
		numTags( 0 ),
		numAnimFrames( 0 ),
		numBones( 0 ),
		numSubMesh( 0 ),
		nHeaderSize( sizeof( CSMHeader ) ),
		nOffBones( 0 ),
		nOffSubMesh( 0 ),
		nFileSize( 0 )
	{
	}
};

/*! \brief 标示模型的绑定点 */
struct CSMTagData
{
	char	name[MAX_STRING_LENGTH];			//!< Tag的名称
	int		numAnimFrames;						//!< Tag的动画帧数量
	vector< MATRIX >	vFrameData;				//!< Tag的绑定变换矩阵

	CSMTagData()
	{
		memset( name, 0, sizeof( char ) * MAX_STRING_LENGTH );
		numAnimFrames = 0;
	}
};

/*! \brief 骨骼节点信息 */
struct CSMBoneData
{
	char name[MAX_STRING_LENGTH];	//!< 骨骼名称
	int ID;							//!< ID
	int parentID;					//!< 父骨骼的ID
	ATOM_Matrix4x4f relativeMat;			//!< 相对于父骨骼的矩阵

	CSMBoneData()
	{
		memset( name, 0, MAX_STRING_LENGTH );
		ID = -1;
		parentID = -1;
		memset( &relativeMat, 0, sizeof( ATOM_Matrix4x4f) );
	}
};

/*! \brief 顶点数据 */
struct CSMVertexData
{
	float position[3];		//!< 顶点位置
	float normal[3];		//!< 顶点法线
	float u, v;				//!< 纹理坐标

	CSMVertexData()
	{
		memset( position, 0, sizeof( float ) * 3 );
		memset( normal, 0, sizeof( float ) * 3 );
		u = v = 0.0f;
	}
};

/*! \brief 蒙皮数据 */
struct CSMSkinData
{
	int bones[4];			//!< 关联骨骼
	float boneWeights[4];	//!< 关联骨骼的权重
	CSMSkinData()
	{
		memset( bones, -1, sizeof( int ) * 4 );
		memset( boneWeights, 0, sizeof( float ) * 4 );
	}
};

/*! \brief 三角形面数据 */
struct CSMTriangleData
{
	int vertexIndices[3];			//!< 三个顶点的坐标索引
	float normal[3];				//!< 面法线

	CSMTriangleData()
	{
		memset( vertexIndices, -1, sizeof( int ) * 3 );
		memset( normal, 0, sizeof( float ) * 3 );
	}
};

/*! \brief 模型的子网格信息 */
struct CSMSubMeshHeader
{
	char	name[MAX_STRING_LENGTH];	//!< 子网格的名称

	int		numAnimFrames;		//!< 动画的帧数量
	int		numVertices;		//!< 顶点的数量
	int		numFaces;			//!< 三角形面的数量

	int		nHeaderSize;		//!< Header的字节大小，同时也等于到Texture File的字节偏移量
	int		nOffVertices;		//!< 到顶点数据的字节偏移量
	int		nOffSkin;			//!< 到蒙皮数据的字节偏移量
	int		nOffFaces;			//!< 到三角形面数据的字节偏移量
	int		nOffEnd;			//!< 到该子网格数据部分结尾的字节偏移量，也是下一个网格数据的开始点，或者文件结尾

	/*! \brief 默认构造函数 */
	CSMSubMeshHeader()
	{
		memset( name, 0, sizeof( char ) * MAX_STRING_LENGTH );
		numAnimFrames = numVertices = numFaces = 0;
		nHeaderSize = nOffVertices = nOffSkin = nOffFaces = nOffEnd = 0;
	}

	/*! \brief 构造函数 */
	CSMSubMeshHeader( char n[], int nAF, int nV, int nF );
};

/*! \brief 动画描述信息 */
struct CSMAnimation
{
	string animName;
	int firstFrame;
	int numFrames;
	int numLoops;
	int fps;

	CSMAnimation()
	{
		firstFrame = numLoops = numFrames = fps = 0;
	}
};

/*! \brief 子网格数据 */
struct CSMSubMesh
{
	CSMSubMeshHeader	subMeshHeader;						//!< 子网格Header
	MaterialInfo		materialInfo;
	ATOM_Matrix4x4f	worldMat;							//!< 世界矩阵（暂时没有使用，矩阵已经乘到了mesh的顶点中）
	vector< CSMVertexData >		vVertexData;				//!< 顶点数据
	vector< CSMSkinData >		vSkinData;					//!< 纹理坐标数据
	vector< CSMTriangleData >	vTriangleData;				//!< 三角形数据

	/*! \brief 默认构造函数 */
	CSMSubMesh() 
	{
		memset( &subMeshHeader, 0, sizeof( subMeshHeader ) );
		worldMat.makeIdentity();
	};

	/*! \brief 构造函数 */
	CSMSubMesh( char meshName[], const MaterialInfo &materialInfo, int numAnimFrame, int numVertices, int numTriangles );

	/*! \brief 生成Sub Mesh的Header信息 */
	void GenHeaderInfo( char name[], int numAnimFrames = 1 );
};
typedef vector< CSMSubMesh* > SubMeshList;				//!< 子网格列表

#endif