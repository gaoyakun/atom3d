#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "CSM.h"

/*! \brief 3dMax�Ķ��㷨�� */
struct MaxVertexNormal
{
	Point3 normal;					//!< ���㷨��
	int materialID;					//!< ��Ӧ��������ʹ�õ�����ID
	int smoothGroup;				//!< smoothing group����ο�3DMAX SDK��

	MaxVertexNormal()
	{
		memset( normal, 0, sizeof( Point3 ) );
		materialID = -1;
		smoothGroup = -1;
	}
};

BOOL operator==( const MaxVertexNormal &lh, const MaxVertexNormal &rh );

/*! \brief 3dMax�Ķ������� */
struct MaxVertexData
{
	float position[3];							//!< ����λ��
	vector< MaxVertexNormal > vtxNormals;		//!< ���㷨�ߣ�1����������ж�����ߣ�����smoothgroup��
	int bones[4];								//!< ��������
	float boneWeights[4];						//!< ����������Ȩ��

	MaxVertexData()
	{
		memset( position, 0, sizeof( float ) * 3 );
		memset( bones, -1, sizeof( int ) * 4 );
		memset( boneWeights, 0, sizeof( float ) * 4 );
	}
};

/*! \brief 3dMax�������������� */
struct MaxTexCoordData
{
	float u, v;		//!< ��������

	MaxTexCoordData()
	{
		u = v = 0.0f;
	}
};

/*! \brief 3dMax�������������� */
struct MaxTriangleData
{
	int vertexIndices[3];			//!< �����������������
	int texCoordIndices[3];			//!< ���������������������
	float normal[3];				//!< �淨��
	int materialID;					//!< ��ʹ�õ�����ID
	int smoothGroup;				//!< smoothing group����ο�3DMAX SDK��

	MaxTriangleData()
	{
		memset( vertexIndices, -1, sizeof( int ) * 3 );
		memset( texCoordIndices, -1, sizeof( int ) * 3 );
		memset( normal, 0, sizeof( float ) * 3 );
		materialID = 0;
	}
};

/*! \bref 3dmax��һ��TriObj���ǹ����������� */
struct MaxTriObjData
{
	string				objName;								//!< ����
	ATOM_Matrix4x4f		worldMat;								//!< �������
	ATOM_Matrix4x4f		relativeMat;							//!< ��Ծ���
	int					numAnimFrames;							//!< ����֡��
	int					numTextures;							//!< ������Ŀ�����������ָ��Sub Mesh��Ŀ��
	vector< MaterialInfo >	vMaterialInfos;								//!< ��������
	vector< MaxVertexData >			vVertexData;				//!< ��������
	vector< MaxTexCoordData >		vTexCoordData;				//!< ������������
	vector< MaxTriangleData >		vTriangleData;				//!< ����������
	SubMeshList			vSubMeshes;								//!< ����������

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

typedef vector< MaxTriObjData* > MaxTriObjList;				//!< MaxTriObjData�б�

/*! \brief ����ת���õĶ���ṹ����3DMAX���㵽CSMVertex�����õģ� */
struct MediateVertex
{
	int vtxIndex;			//!< ��ԭ3dmax�����б��е�����
	int smoothGroup;		//!< smoothing group����ο�3DMAX SDK��
	float pos[3];			//!< λ��
	float u, v;				//!< ��������
	float normal[3];		//!< ���㷨��
	int bones[4];			//!< ��������
	float boneWeights[4];	//!< ����������Ȩ��

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

/*! \brief ����ת���õ��飬ͬһ����Ķ����������ʹ�õ���ͬһ�Ų��� */
struct MediateGroup
{
	int iMeshID;										//!< ������������Materials ID
	MedVertexList vVertices;							//!< �����б�
	vector< CSMTriangleData > vTriangles;				//!< �������б�
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

	/*! \brief ��ģ�͵Ķ���ӱ��ؿռ�ת��������ռ䣨�������ߣ� */
	void TransformToWorld( CSMSubMesh *pSubMesh, MATRIX mat );

	/*! \brief ��ģ�͵Ķ���ӱ��ؿռ�ת��������ռ䣨�������ߣ� */
	void TransformToWorld( MaxTriObjData *pMaxTriObj, MATRIX mat );

	/*! \brief ��3dmax��������ϵ�µľ���ת����DX��������ϵ�µľ��� */
	ATOM_Matrix4x4f TransformToDXMatrix( const Matrix3 &matrix );

	/*! \brief ���㶥�㷨�ߣ�������smooth group�� */
	void ComputeVertexNormals( MaxTriObjData *pMaxTriData );

	/*! \brief ���㶥�㷨�ߣ�����smooth group�� */
	void ComputeVertexNormalsOpt( MaxTriObjData *pMaxTriData );

	/*! \brief ��Max����תΪCSM���� */
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