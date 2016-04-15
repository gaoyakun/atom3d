#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "CSM.h"

/*! \brief 3dMax的顶点法线 */
struct MaxVertexNormal
{
	Point3 normal;					//!< 顶点法线
	int materialID;					//!< 对应三角形面使用的纹理ID
	int smoothGroup;				//!< smoothing group（请参考3DMAX SDK）

	MaxVertexNormal()
	{
		memset( normal, 0, sizeof( Point3 ) );
		materialID = -1;
		smoothGroup = -1;
	}
};

BOOL operator==( const MaxVertexNormal &lh, const MaxVertexNormal &rh );

/*! \brief 3dMax的顶点数据 */
struct MaxVertexData
{
	float position[3];							//!< 顶点位置
	vector< MaxVertexNormal > vtxNormals;		//!< 顶点法线（1个顶点可能有多个法线，根据smoothgroup）
	int bones[4];								//!< 关联骨骼
	float boneWeights[4];						//!< 关联骨骼的权重

	MaxVertexData()
	{
		memset( position, 0, sizeof( float ) * 3 );
		memset( bones, -1, sizeof( int ) * 4 );
		memset( boneWeights, 0, sizeof( float ) * 4 );
	}
};

/*! \brief 3dMax的纹理坐标数据 */
struct MaxTexCoordData
{
	float u, v;		//!< 纹理坐标

	MaxTexCoordData()
	{
		u = v = 0.0f;
	}
};

/*! \brief 3dMax的三角形面数据 */
struct MaxTriangleData
{
	int vertexIndices[3];			//!< 三个顶点的坐标索引
	int texCoordIndices[3];			//!< 三个顶点的纹理坐标索引
	float normal[3];				//!< 面法线
	int materialID;					//!< 所使用的纹理ID
	int smoothGroup;				//!< smoothing group（请参考3DMAX SDK）

	MaxTriangleData()
	{
		memset( vertexIndices, -1, sizeof( int ) * 3 );
		memset( texCoordIndices, -1, sizeof( int ) * 3 );
		memset( normal, 0, sizeof( float ) * 3 );
		materialID = 0;
	}
};

/*! \bref 3dmax中一个TriObj（非骨骼）的数据 */
struct MaxTriObjData
{
	string				objName;								//!< 名称
	ATOM_Matrix4x4f		worldMat;								//!< 世界矩阵
	ATOM_Matrix4x4f		relativeMat;							//!< 相对矩阵
	int					numAnimFrames;							//!< 动画帧数
	int					numTextures;							//!< 纹理数目（等于它所分割的Sub Mesh数目）
	vector< MaterialInfo >	vMaterialInfos;								//!< 纹理名称
	vector< MaxVertexData >			vVertexData;				//!< 顶点数据
	vector< MaxTexCoordData >		vTexCoordData;				//!< 纹理坐标数据
	vector< MaxTriangleData >		vTriangleData;				//!< 三角形数据
	SubMeshList			vSubMeshes;								//!< 子网格数据

	MaxTriObjData()
	{
		memset( &worldMat, 0, sizeof( ATOM_Matrix4x4f ) );
		numAnimFrames = 1;
		numTextures = 0;
	}

	~MaxTriObjData()
	{
		for ( SubMeshList::iterator i = vSubMeshes.begin(); i != vSubMeshes.end(); i ++ )
		{
			SAFE_DELETE( *i );
		}
	}
};

typedef vector< MaxTriObjData* > MaxTriObjList;				//!< MaxTriObjData列表

/*! \brief 用于转储用的顶点结构（从3DMAX顶点到CSMVertex过渡用的） */
struct MediateVertex
{
	int vtxIndex;			//!< 在原3dmax顶点列表中的索引
	int smoothGroup;		//!< smoothing group（请参考3DMAX SDK）
	float pos[3];			//!< 位置
	float u, v;				//!< 纹理坐标
	float normal[3];		//!< 顶点法线
	int bones[4];			//!< 关联骨骼
	float boneWeights[4];	//!< 关联骨骼的权重

	MediateVertex()
	{
		vtxIndex = -1;
		u = v = 0.0f;
		memset( pos, 0, sizeof( float ) * 3 );
		memset( normal, 0, sizeof( float ) * 3 );
		memset( bones, -1, sizeof( int ) * 4 );
		memset( boneWeights, 0, sizeof( float ) * 4 );
	}
};

BOOL operator==( const MediateVertex &lh, const MediateVertex &rh );

typedef vector< MediateVertex > MedVertexList;
typedef vector< MediateVertex >::iterator MedVertexIter;

/*! \brief 用于转储用的组，同一个组的顶点和三角形使用的是同一张材质 */
struct MediateGroup
{
	int iMeshID;										//!< 该组所关联的Materials ID
	MedVertexList vVertices;							//!< 顶点列表
	vector< CSMTriangleData > vTriangles;				//!< 三角形列表
};

static const float EPSILON = 0.0001f;

class Utility
{
public:
	float FilterData( float &f );
	void FilterData( MATRIX &mat );
	void FilterData( Matrix3 &mat );
	void FilterData( CSMVertexData &vtx );
	void FilterData( CSMSkinData &skinData );
	BOOL IsFloatEqual( const float lf, const float rf );
	BOOL IsMatrixEqual( const MATRIX lmat, const MATRIX rmat );

	/*! \brief 将模型的顶点从本地空间转换到世界空间（包括法线） */
	void TransformToWorld( CSMSubMesh *pSubMesh, MATRIX mat );

	/*! \brief 将模型的顶点从本地空间转换到世界空间（包括法线） */
	void TransformToWorld( MaxTriObjData *pMaxTriObj, MATRIX mat );

	/*! \brief 将3dmax右手坐标系下的矩阵转换到DX左手坐标系下的矩阵 */
	ATOM_Matrix4x4f TransformToDXMatrix( const Matrix3 &matrix );

	/*! \brief 计算顶点法线（不考虑smooth group） */
	void ComputeVertexNormals( MaxTriObjData *pMaxTriData );

	/*! \brief 计算顶点法线（考虑smooth group） */
	void ComputeVertexNormalsOpt( MaxTriObjData *pMaxTriData );

	/*! \brief 将Max数据转为CSM数据 */
	void MaxDataToCSMData( MaxTriObjData *pMaxObjData );

	static Utility * GetInstance()
	{
		static Utility * pSingleton = new Utility();
		return pSingleton;
	}

private:
	Utility(){};
	Utility( const Utility &cpy );
};

#define UTILITY Utility::GetInstance()

#endif