#ifndef __CROWDSIMULATIONMODEL_H__
#define __CROWDSIMULATIONMODEL_H__

#include "Utility.h"

class ATOM_Skeleton;
class MyModel;
class MaterialViewer;
struct VertexAnimationInfo;

class Logger
{
public:
	virtual void output (const char *fmt, ...) = 0;
};

/*! \brief �����������������Ѽ�3dmax��Ϣ�����CSM,AM,CFG���ļ� */
class ExportManager
{
public:	
	/*! \brief Ĭ�Ϲ��캯�� */
	ExportManager(Logger *logger);

	/*! \brief �������� */
	~ExportManager();

	/*! \brief �洢���˹�����submesh�Լ�����ص���Ƥ��Ϣ */
	struct SubMeshBones
	{
		MaxTriObjData *pMaxObjData;			//!< ���˹�����MaxTriObjData
		ISkinContextData *pSkinContext;		//!< �洢����Ƥ��Ϣ
		ISkin *pSkin;						//!< ������mesh��������Ĺ����б�

		SubMeshBones()
		{
			pMaxObjData = NULL;
			pSkinContext = NULL;
			pSkin = NULL;
		}
	};

	typedef vector< SubMeshBones > SubMeshBoneList;			//!< ������Ƥ���������б�
	typedef vector< Matrix3 > BoneFrames;					//!< �����Ķ���֡��Ϣ

	/*! \brief ������Ϣ */
	struct Bone
	{
		INode *pBoneNode;				//!< �洢������3dmax�ڵ�
		int index;						//!< ����
		Bone *pParentBone;				//!< ������
		Matrix3 relativeMat;			//!< ����ڸ������ľ���
		BoneFrames localFrames;			//!< �ڱ��ؿռ��ڵı任
		BoneFrames boneFrames;			//!< �ؼ�֡

		Bone()
		{
			index = -1;
			pBoneNode = NULL;
			pParentBone = NULL;
			memset( &relativeMat, 0, sizeof( Matrix3 ) );
		}
	};

	typedef map< string, Bone > BoneList;			//!< ���������ƣ�������

	/*! \brief ��3DS MAX�ĳ������Ѽ�������������� */
	void Gather3DSMAXSceneData( INode *pRootNode, 
								BOOL bExportAnim, 
								BOOL bExportSelections,
								MaterialViewer *materialViewer);

	ATOM_Skeleton *CreateSkeleton (void);
	BOOL ExportToMyModel (const char *outFileName, ATOM_Skeleton *skeleton, MyModel *model, VertexAnimationInfo *info, bool exportMeshes, bool exportActions, bool exportBindable, bool exportSkeleton, bool shareDiffuseMap);

private:
	/*! \brief ���ο������캯�� */
	ExportManager( const ExportManager &rhEM );

	/*! \brief ���θ�ֵ������ */
	ExportManager& operator=( const ExportManager &rhEM );

	/*! \brief ���ݽڵ������Ѱ�ҽڵ� */
	INode *FindNodeByName( INode *pNode, string nodeName );

	/*! \brief �Ѽ��ڵ���Ϣ������һ���ݹ������������ĺ��� */
	void GatherRecursiveNodeData( INode *pNode, BOOL bExportAnim, BOOL bExportSelections, MaterialViewer *materialViewer );

	/*! \brief �Ѽ��ڵ���Ϣ */
	BOOL GatherNodeData( INode *pNode, BOOL bExportAnim, BOOL bExportSelections, MaterialViewer *materialViewer );

	/*! \brief �Ѽ�Mesh�����ݴ洢��CSMSubMesh�� */
	void GatherMeshData( Mesh &lMesh, MaxTriObjData *pMaxTriData );

	/*! \brief �Ѽ�Material��ֻ�����洢��CSMSubMesh�� */
	void GatherMaterialData( Mtl *pMtl, MaxTriObjData *pMaxTriData, MaterialViewer *materialViewer );

	/*! \brief ����INode��modifier stack������skin modifier(SKIN_CLASSID) */
	Modifier* GetSkinMode( INode *pNode );

	/*! \brief �Ѽ������Skin��Ϣ��Ӱ�������Ȩ�أ��洢��CSMSubMesh�� */
	void GatherSkinData( ISkinContextData* pContext, ISkin *pSkin, MaxTriObjData *pMaxTriData );

	/*! \brief �Ѽ�ISkin���������Ĺ��� */
	void GatherBones( ISkin *pSkin );

	/*! \brief ���ɹ����� */
	void GenBoneTree();

	/*! \brief ���һ���ڵ��Ƿ��ǹ��� */
	BOOL IsBone( INode *pNode );

	/*! \brief ��ȡ��������ȥ��scale�� */
	Matrix3 GetBoneTM( INode *pNode, TimeValue t = 0 );

	/*! \brief �Ѽ��������� */
	void GatherBoneFrames( Bone *pBone );

	/*! \brief ��ýڵ�������丸�ڵ����Ծ��� */
	Matrix3 GetRelativeMatrix( INode *pNode, TimeValue t = 0 );

	/*! \brief ��ù���������丸��������Ծ��� */
	Matrix3 GetRelativeBoneMatrix( Bone *pBone, TimeValue t = 0 );

	/*! \brief ��ù����ڱ��ؿռ��ڵı任���� */
	Matrix3 GetLocalBoneTranMatrix( Bone *pBone, TimeValue t );

	void insertBoneR (Bone *bone, ATOM_Skeleton *skeleton);

	int findNewJointIndex (ATOM_Skeleton *skeleton, int oldIndex);

private:
	int			m_numAnimFrames;				//!< ������֡��Ϣ
	int			m_startFrame;
	int			m_endFrame;

	MaxTriObjList	m_vpMaxObjs;				//!< �洢���������еĽڵ�

	SubMeshBoneList m_submeshBones;				//!< Sub Mesh�Ĺ�����Ϣ
	BoneList	m_boneList;						//!< �����б�

	Logger *	m_logger;
};

#endif