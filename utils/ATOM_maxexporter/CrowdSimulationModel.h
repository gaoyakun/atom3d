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

/*! \brief 导出管理器，负责搜集3dmax信息，输出CSM,AM,CFG等文件 */
class ExportManager
{
public:	
	/*! \brief 默认构造函数 */
	ExportManager(Logger *logger);

	/*! \brief 析构函数 */
	~ExportManager();

	/*! \brief 存储绑定了骨骼的submesh以及其相关的蒙皮信息 */
	struct SubMeshBones
	{
		MaxTriObjData *pMaxObjData;			//!< 绑定了骨骼的MaxTriObjData
		ISkinContextData *pSkinContext;		//!< 存储了蒙皮信息
		ISkin *pSkin;						//!< 包含与mesh所相关联的骨骼列表

		SubMeshBones()
		{
			pMaxObjData = NULL;
			pSkinContext = NULL;
			pSkin = NULL;
		}
	};

	typedef vector< SubMeshBones > SubMeshBoneList;			//!< 包含蒙皮的子网格列表
	typedef vector< Matrix3 > BoneFrames;					//!< 骨骼的动画帧信息

	/*! \brief 骨骼信息 */
	struct Bone
	{
		INode *pBoneNode;				//!< 存储骨骼的3dmax节点
		int index;						//!< 索引
		Bone *pParentBone;				//!< 父骨骼
		Matrix3 relativeMat;			//!< 相对于父骨骼的矩阵
		BoneFrames localFrames;			//!< 在本地空间内的变换
		BoneFrames boneFrames;			//!< 关键帧

		Bone()
		{
			index = -1;
			pBoneNode = NULL;
			pParentBone = NULL;
			memset( &relativeMat, 0, sizeof( Matrix3 ) );
		}
	};

	typedef map< string, Bone > BoneList;			//!< （骨骼名称，骨骼）

	/*! \brief 从3DS MAX的场景中搜集我们所需的数据 */
	void Gather3DSMAXSceneData( INode *pRootNode, 
								BOOL bExportAnim, 
								BOOL bExportSelections,
								MaterialViewer *materialViewer);

	ATOM_Skeleton *CreateSkeleton (void);
	BOOL ExportToMyModel (const char *outFileName, ATOM_Skeleton *skeleton, MyModel *model, VertexAnimationInfo *info, bool exportMeshes, bool exportActions, bool exportBindable, bool exportSkeleton, bool shareDiffuseMap);

private:
	/*! \brief 屏蔽拷贝构造函数 */
	ExportManager( const ExportManager &rhEM );

	/*! \brief 屏蔽赋值操作符 */
	ExportManager& operator=( const ExportManager &rhEM );

	/*! \brief 根据节点的名称寻找节点 */
	INode *FindNodeByName( INode *pNode, string nodeName );

	/*! \brief 搜集节点信息，这是一个递归搜索场景数的函数 */
	void GatherRecursiveNodeData( INode *pNode, BOOL bExportAnim, BOOL bExportSelections, MaterialViewer *materialViewer );

	/*! \brief 搜集节点信息 */
	BOOL GatherNodeData( INode *pNode, BOOL bExportAnim, BOOL bExportSelections, MaterialViewer *materialViewer );

	/*! \brief 搜集Mesh的数据存储到CSMSubMesh中 */
	void GatherMeshData( Mesh &lMesh, MaxTriObjData *pMaxTriData );

	/*! \brief 搜集Material（只纹理）存储到CSMSubMesh中 */
	void GatherMaterialData( Mtl *pMtl, MaxTriObjData *pMaxTriData, MaterialViewer *materialViewer );

	/*! \brief 搜索INode的modifier stack，查找skin modifier(SKIN_CLASSID) */
	Modifier* GetSkinMode( INode *pNode );

	/*! \brief 搜集顶点的Skin信息（影响骨骼、权重）存储到CSMSubMesh中 */
	void GatherSkinData( ISkinContextData* pContext, ISkin *pSkin, MaxTriObjData *pMaxTriData );

	/*! \brief 搜集ISkin中所关联的骨骼 */
	void GatherBones( ISkin *pSkin );

	/*! \brief 生成骨骼树 */
	void GenBoneTree();

	/*! \brief 检测一个节点是否是骨骼 */
	BOOL IsBone( INode *pNode );

	/*! \brief 获取骨骼矩阵（去除scale） */
	Matrix3 GetBoneTM( INode *pNode, TimeValue t = 0 );

	/*! \brief 搜集骨骼动画 */
	void GatherBoneFrames( Bone *pBone );

	/*! \brief 获得节点相对于其父节点的相对矩阵 */
	Matrix3 GetRelativeMatrix( INode *pNode, TimeValue t = 0 );

	/*! \brief 获得骨骼相对于其父骨骼的相对矩阵 */
	Matrix3 GetRelativeBoneMatrix( Bone *pBone, TimeValue t = 0 );

	/*! \brief 获得骨骼在本地空间内的变换矩阵 */
	Matrix3 GetLocalBoneTranMatrix( Bone *pBone, TimeValue t );

	void insertBoneR (Bone *bone, ATOM_Skeleton *skeleton);

	int findNewJointIndex (ATOM_Skeleton *skeleton, int oldIndex);

private:
	int			m_numAnimFrames;				//!< 场景的帧信息
	int			m_startFrame;
	int			m_endFrame;

	MaxTriObjList	m_vpMaxObjs;				//!< 存储场景中所有的节点

	SubMeshBoneList m_submeshBones;				//!< Sub Mesh的骨骼信息
	BoneList	m_boneList;						//!< 骨骼列表

	Logger *	m_logger;
};

#endif