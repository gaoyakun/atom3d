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

/*! \brief CSM�ļ���Header */
struct CSMHeader
{
	int ident;				//!< ���ڱ�����ʶ��.CSM�ļ���ħ�����������CSM1

	int numTags;			//!< ��ǵ�����
	int numAnimFrames;		//!< ������֡����
	int numBones;			//!< ����������
	int numSubMesh;			//!< �����������

	int nHeaderSize;		//!< Header���ֽڴ�С��ͬʱҲ���ڵ�Tag Data���ֽ�ƫ����
	int nOffBones;			//!< ��������Ϣ���ֽ�ƫ����
	int nOffSubMesh;		//!< ��Sub Mesh���ֽ�ƫ����
	int nFileSize;			//!< ����CSM�ļ��Ĵ�С

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

/*! \brief ��ʾģ�͵İ󶨵� */
struct CSMTagData
{
	char	name[MAX_STRING_LENGTH];			//!< Tag������
	int		numAnimFrames;						//!< Tag�Ķ���֡����
	vector< MATRIX >	vFrameData;				//!< Tag�İ󶨱任����

	CSMTagData()
	{
		memset( name, 0, sizeof( char ) * MAX_STRING_LENGTH );
		numAnimFrames = 0;
	}
};

/*! \brief �����ڵ���Ϣ */
struct CSMBoneData
{
	char name[MAX_STRING_LENGTH];	//!< ��������
	int ID;							//!< ID
	int parentID;					//!< ��������ID
	ATOM_Matrix4x4f relativeMat;			//!< ����ڸ������ľ���

	CSMBoneData()
	{
		memset( name, 0, MAX_STRING_LENGTH );
		ID = -1;
		parentID = -1;
		memset( &relativeMat, 0, sizeof( ATOM_Matrix4x4f) );
	}
};

/*! \brief �������� */
struct CSMVertexData
{
	float position[3];		//!< ����λ��
	float normal[3];		//!< ���㷨��
	float u, v;				//!< ��������

	CSMVertexData()
	{
		memset( position, 0, sizeof( float ) * 3 );
		memset( normal, 0, sizeof( float ) * 3 );
		u = v = 0.0f;
	}
};

/*! \brief ��Ƥ���� */
struct CSMSkinData
{
	int bones[4];			//!< ��������
	float boneWeights[4];	//!< ����������Ȩ��
	CSMSkinData()
	{
		memset( bones, -1, sizeof( int ) * 4 );
		memset( boneWeights, 0, sizeof( float ) * 4 );
	}
};

/*! \brief ������������ */
struct CSMTriangleData
{
	int vertexIndices[3];			//!< �����������������
	float normal[3];				//!< �淨��

	CSMTriangleData()
	{
		memset( vertexIndices, -1, sizeof( int ) * 3 );
		memset( normal, 0, sizeof( float ) * 3 );
	}
};

/*! \brief ģ�͵���������Ϣ */
struct CSMSubMeshHeader
{
	char	name[MAX_STRING_LENGTH];	//!< �����������

	int		numAnimFrames;		//!< ������֡����
	int		numVertices;		//!< ���������
	int		numFaces;			//!< �������������

	int		nHeaderSize;		//!< Header���ֽڴ�С��ͬʱҲ���ڵ�Texture File���ֽ�ƫ����
	int		nOffVertices;		//!< ���������ݵ��ֽ�ƫ����
	int		nOffSkin;			//!< ����Ƥ���ݵ��ֽ�ƫ����
	int		nOffFaces;			//!< �������������ݵ��ֽ�ƫ����
	int		nOffEnd;			//!< �������������ݲ��ֽ�β���ֽ�ƫ������Ҳ����һ���������ݵĿ�ʼ�㣬�����ļ���β

	/*! \brief Ĭ�Ϲ��캯�� */
	CSMSubMeshHeader()
	{
		memset( name, 0, sizeof( char ) * MAX_STRING_LENGTH );
		numAnimFrames = numVertices = numFaces = 0;
		nHeaderSize = nOffVertices = nOffSkin = nOffFaces = nOffEnd = 0;
	}

	/*! \brief ���캯�� */
	CSMSubMeshHeader( char n[], int nAF, int nV, int nF );
};

/*! \brief ����������Ϣ */
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

/*! \brief ���������� */
struct CSMSubMesh
{
	CSMSubMeshHeader	subMeshHeader;						//!< ������Header
	MaterialInfo		materialInfo;
	ATOM_Matrix4x4f	worldMat;							//!< ���������ʱû��ʹ�ã������Ѿ��˵���mesh�Ķ����У�
	vector< CSMVertexData >		vVertexData;				//!< ��������
	vector< CSMSkinData >		vSkinData;					//!< ������������
	vector< CSMTriangleData >	vTriangleData;				//!< ����������

	/*! \brief Ĭ�Ϲ��캯�� */
	CSMSubMesh() 
	{
		memset( &subMeshHeader, 0, sizeof( subMeshHeader ) );
		worldMat.makeIdentity();
	};

	/*! \brief ���캯�� */
	CSMSubMesh( char meshName[], const MaterialInfo &materialInfo, int numAnimFrame, int numVertices, int numTriangles );

	/*! \brief ����Sub Mesh��Header��Ϣ */
	void GenHeaderInfo( char name[], int numAnimFrames = 1 );
};
typedef vector< CSMSubMesh* > SubMeshList;				//!< �������б�

#endif