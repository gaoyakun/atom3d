#include "CrowdSimulationModel.h"
#include "skeleton.h"
#include "model.h"
#include "config.h"
#include "vertexanimation.h"
#include "materialviewer.h"

static std::string identifyFileName (const char *filename)
{
	char buffer[MAX_PATH];

	if (!::GetFullPathName (filename, MAX_PATH, buffer, 0))
	{
		return "";
	}

	return buffer;
}

static std::string generateVFSPath (const char *root, const char *subdir)
{
	char identRoot[MAX_PATH];
	char identSub[MAX_PATH];
	::GetFullPathName (root, MAX_PATH, identRoot, 0);
	::GetFullPathName (subdir, MAX_PATH, identSub, 0);
	int rootLen = strlen(identRoot);
	int subLen = strlen(identSub);
	if (rootLen > subLen)
	{
		return "";
	}
	if (_strnicmp (identRoot, identSub, rootLen))
	{
		return "";
	}
	char *s = identSub + rootLen;
	for (char *s2 = s; *s2; ++s2)
	{
		if (*s2 == '\\')
		{
			*s2 = '/';
		}
	}
	std::string ret;
	if (s[0] != '/')
	{
		ret += '/';
	}
	ret += s;
	return ret;
}

static bool confirmFilePath (std::string &fileName)
{
	fileName = identifyFileName (fileName.c_str());
	if (fileName.empty ())
	{
		return false;
	}

	FILE *fp = fopen (fileName.c_str(), "rb");
	if (fp)
	{
		fclose (fp);
		return true;
	}

	const char *p = strrchr (fileName.c_str(), '\\');
	if (p)
	{
		return true;
	}

	OPENFILENAME ofn;
	char fileNameBuffer[MAX_PATH];

	memset (&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = ::GetActiveWindow ();
	ofn.lpstrFile = fileNameBuffer;
	strcpy (ofn.lpstrFile, fileName.c_str());
	ofn.nMaxFile = sizeof(fileNameBuffer);

	char filterBuffer[MAX_PATH];
	strcpy (filterBuffer, "*.");
	const char *ext = strrchr (fileName.c_str(), '.');
	strcat (filterBuffer, ext ? ext+1 : "*");
	char *s2 = filterBuffer+strlen(filterBuffer)+1;
	strcpy (s2, filterBuffer);
	*(s2 + strlen(s2) + 1) = '\0';

	ofn.lpstrFilter = filterBuffer;
	ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;

	if (::GetOpenFileName (&ofn))
	{
		fileName = fileNameBuffer;
		return true;
	}

	return false;
}

bool translateFileName (const std::string &fileName, const std::string &savepath, std::string &vfsFileName, std::string &physicFileName, bool share)
{
	std::string srcPath = fileName;
	if (!confirmFilePath (srcPath))
	{
		return false;
	}

	std::string FilePart = strrchr (srcPath.c_str(), '\\') + 1;
	std::string SavePath = savepath.substr (0, savepath.find_last_of ('\\') + 1);
	std::string dir;

	ExporterConfig &exporterConfig = ExporterConfig::getInstance();
	if (!share)
	{
		dir = generateVFSPath (exporterConfig.rootDirectory.c_str(), SavePath.c_str());
		if (dir.empty ())
		{
			MessageBox (::GetActiveWindow(), "保存的路径有问题，请确保是保存在资源根目录之下。", "atom exporter", MB_OK|MB_ICONHAND);
			return false;
		}
	}
	else
	{
		dir = generateVFSPath (exporterConfig.rootDirectory.c_str(), exporterConfig.shareDirectory.c_str());
		if (dir[dir.length()-1] != '/')
		{
			dir += '/';
		}
	}

	vfsFileName = dir + FilePart;

	physicFileName = share ? exporterConfig.shareDirectory : SavePath;
	if (physicFileName[physicFileName.length()-1] != '\\') physicFileName += '\\';
	physicFileName += FilePart;

	return true;
}

//////////////////////////////ExportManager//////////////////////////////////

ExportManager::ExportManager(Logger *logger)
{
	m_numAnimFrames = 0;
	m_logger = logger;
}

ExportManager::~ExportManager()
{
	for ( MaxTriObjList::iterator i = m_vpMaxObjs.begin(); i != m_vpMaxObjs.end(); i ++ )
	{
		delete *i;
	}
}

/*! \param pNode 场景中开始寻找的节点
 *	\param nodeName 寻找节点的名称
 */
INode * ExportManager::FindNodeByName( INode *pNode, string nodeName )
{
	if ( nodeName == pNode->GetName() )
	{
		return pNode;
	}
	else
	{
		int numChildren = pNode->NumberOfChildren();
		for ( int i = 0; i < numChildren; i++ )
		{
			INode *childNode = pNode->GetChildNode( i );
			INode *resNode = NULL;
			resNode = FindNodeByName( childNode, nodeName );
			if ( resNode != NULL )
			{
				return resNode;
			}
		}
		return NULL;
	}
}

/*! \param pRootNode 3dmax场景的根节点
 *	\param bExportAnim 是否导出动画
 *	\param bExportSelections 是否只导出选择的节点
 */
void ExportManager::Gather3DSMAXSceneData( INode *pRootNode, 
										   BOOL bExportAnim, 
										   BOOL bExportSelections,
										   MaterialViewer *materialViewer)
{
	m_logger->output ("========收集场景信息========\n");

	//! 搜集场景的帧信息
	Interval ivAnimRange = GetCOREInterface()->GetAnimRange();
	m_startFrame = ivAnimRange.Start() / GetTicksPerFrame();
	m_endFrame = ivAnimRange.End() / GetTicksPerFrame();
	m_numAnimFrames = m_endFrame - m_startFrame + 1;
	m_logger->output ("起始帧:%d, 结束帧:%d, 共%d帧\n", m_startFrame, m_endFrame, m_numAnimFrames);

	if ( bExportAnim != TRUE )
	{
		// 没有动画则场景只有1帧
		m_numAnimFrames = 1;
	}

	// 搜集所有节点信息
	m_logger->output ("--收集节点信息--\n");
	GatherRecursiveNodeData( pRootNode, bExportAnim, bExportSelections, materialViewer );

#if 1
	// 所有Mesh转换到世界空间
	for ( MaxTriObjList::iterator i = m_vpMaxObjs.begin(); i != m_vpMaxObjs.end(); i ++ )
	{
		MaxTriObjData *pMaxObj = *i;
		UTILITY->TransformToWorld( pMaxObj, pMaxObj->worldMat );
	}
#endif

	//! 搜集动画数据
	if ( bExportAnim == TRUE )
	{
		//! 生成骨骼树结构
		GenBoneTree();

		//! 搜集骨骼蒙皮信息
		for ( SubMeshBoneList::iterator i = m_submeshBones.begin(); i != m_submeshBones.end(); i ++ )
		{
			MaxTriObjData *pMaxObjData = i->pMaxObjData;
			ISkinContextData *pSkinContext = i->pSkinContext;
			ISkin *pSkin = i->pSkin;

			GatherSkinData( pSkinContext, pSkin, pMaxObjData );
		}

		//! 搜集骨骼动画信息
		for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i ++ )
		{
			GatherBoneFrames( &( i->second ) );
		}
	}

	//! 将Max数据转为CSM数据
	for ( MaxTriObjList::iterator i = m_vpMaxObjs.begin(); i != m_vpMaxObjs.end(); i ++ )
	{
		MaxTriObjData *pTriData = *i;
		UTILITY->MaxDataToCSMData( pTriData );
	}
}

/*! \param pNode 场景中的节点
 *	\param bExportAnim 是否导出动画
 */
void ExportManager::GatherRecursiveNodeData( INode *pNode, BOOL bExportAnim, BOOL bExportSelections, MaterialViewer *materialViewer )
{
	GatherNodeData( pNode, bExportAnim, bExportSelections, materialViewer );

	// 递归搜集场景树中的节点
	int numChildren = pNode->NumberOfChildren();
	for ( int i = 0; i < numChildren; i++ )
	{
		INode *childNode = pNode->GetChildNode( i );
		GatherRecursiveNodeData( childNode, bExportAnim, bExportSelections, materialViewer );
	}
}

bool isWhiteSpace (char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

void trimLeadingWhiteSpace (std::string &s)
{
	int len = s.length();
	int pos = 0;

	for (; pos < len && isWhiteSpace(s[pos]); ++pos)
		;

	s = s.substr (pos);
}

std::string getToken (std::string &src)
{
	std::string ret;
	trimLeadingWhiteSpace (src);

	int len = src.length();
	if (len > 0)
	{
		if (src[0] == '=')
		{
			src = src.substr(1);
			ret = "=";
		}
		else
		{
			int pos = 0;
			for (; pos < len && src[pos] != '=' && !isWhiteSpace(src[pos]); ++pos)
				;

			ret = src.substr(0, pos);
			src = src.substr(pos);
		}
	}
	return ret;
}

bool getPair (std::string &src, std::string &key, std::string &value)
{
	key = getToken (src);
	if (key.empty ())
	{
		return false;
	}

	if (getToken (src) != "=")
	{
		return false;
	}

	value = getToken (src);
	if (value.empty ())
	{
		return false;
	}

	return true;
}

/*! \param pNode 场景中的节点
 *	\param bExportAnim 是否导出动画
 *	\return 如果该节点是本插件所需的节点，则返回TRUE，否则返回FALSE
 */
BOOL ExportManager::GatherNodeData( INode *pNode, BOOL bExportAnim, BOOL bExportSelections, MaterialViewer *materialViewer )
{
	// 获得BaseObject
	ObjectState os = pNode->EvalWorldState( 0 );
	Object *pObj = os.obj;
	std::string name = pNode->GetName();

	// 在本export插件中，只输出未隐藏的triObject
	if ( pObj != NULL &&
		 (FALSE == bExportSelections || pNode->Selected()) && 
		 FALSE == pNode->IsNodeHidden() &&
		 FALSE == IsBone( pNode ) &&		// 不是骨骼
		 pObj->CanConvertToType( triObjectClassID ) )
	{
		TriObject* pTriObj = static_cast< TriObject* >( pObj->ConvertToType( 0, triObjectClassID ) );
		if ( pTriObj != NULL )
		{
			m_logger->output ("发现可输出节点<%s>\n", pNode->GetName());

			MaxTriObjData *pMaxTriData = new MaxTriObjData();

			// 名称
			pMaxTriData->objName = pNode->GetName();

			// Mesh的世界矩阵
			Matrix3 worldMat = pNode->GetObjectTM( 0 );
			Matrix3 parentMat = pNode->GetParentTM( 0 );
			Matrix3 relativeMat = worldMat * Inverse( parentMat );
			
			pMaxTriData->worldMat = UTILITY->TransformToDXMatrix( worldMat );
			pMaxTriData->relativeMat = UTILITY->TransformToDXMatrix( relativeMat );

			// 搜集Mesh数据
			Mesh &lMesh = pTriObj->GetMesh();
			GatherMeshData( lMesh, pMaxTriData );

			// 搜集材质数据
			Mtl* pMtl = pNode->GetMtl();
			if ( pMtl != NULL )
			{
				GatherMaterialData( pMtl, pMaxTriData, materialViewer );
				
				if ( pMaxTriData->numTextures > 0 )
				{
					for ( vector< MaxTriangleData >::iterator i = pMaxTriData->vTriangleData.begin(); i != pMaxTriData->vTriangleData.end(); i ++ )
					{
						int faceID = i - pMaxTriData->vTriangleData.begin();
						// 获得该三角形面的Material ID
						i->materialID = lMesh.getFaceMtlIndex( faceID ) % pMaxTriData->numTextures;
					}
				}
			}
			else
			{
				m_logger->output("\t没有材质\n");

				// 添加一个空材质
				pMaxTriData->vMaterialInfos.push_back( MaterialInfo());
				pMaxTriData->numTextures ++;
			}

			// 计算顶点法线（必须在搜集材质之后，因为法线分组与材质相关）
			UTILITY->ComputeVertexNormalsOpt( pMaxTriData );

			if ( bExportAnim == TRUE )
			{
				// 搜集骨骼动画
				Modifier *pMf = GetSkinMode( pNode );
				if ( pMf != NULL )
				{
					ISkin* pSkin = static_cast< ISkin* > ( pMf->GetInterface( I_SKIN ) );
					ISkinContextData* pContext = pSkin->GetContextInterface( pNode );

					// 先存储起来，待会再搜集该Node的骨骼蒙皮信息（这么做是要等待所有的骨骼索引被最终确定）
					SubMeshBones pSubMeshBones;
					pSubMeshBones.pMaxObjData = pMaxTriData;
					pSubMeshBones.pSkinContext = pContext;
					pSubMeshBones.pSkin = pSkin;
					m_submeshBones.push_back( pSubMeshBones );

					// 搜集骨骼
					GatherBones( pSkin );

					// 有动画
					pMaxTriData->numAnimFrames = m_numAnimFrames;
				}
				else
				{
					m_logger->output("\t没有骨骼\n");
					// 没有动画
					pMaxTriData->numAnimFrames = 1;
				}
			}
			
			m_vpMaxObjs.push_back( pMaxTriData );

			if ( pTriObj != pObj )
			{
				pTriObj->DeleteMe();
			}
		}
		return TRUE;
	}
	else if (IsBone(pNode))
	{
		MSTR userPropertiesBuffer;
		pNode->GetUserPropBuffer (userPropertiesBuffer);
		if (!userPropertiesBuffer.isNull())
		{
			std::string buffer = userPropertiesBuffer.data();
			std::string key, value;
			while (getPair (buffer, key, value))
			{
				if (!stricmp (key.c_str(), "bindable") && !stricmp(value.c_str(), "true"))
				{
					Bone bone;
					bone.pBoneNode = pNode;
					string boneName = pNode->GetName();

					m_logger->output ("发现绑定点: %s\n", boneName.c_str());
					m_boneList.insert( BoneList::value_type( boneName, bone ) );
				}
			}
		}
	}
	
	return FALSE;
}

/*! \param lMesh 3dmax网格
	\param pMaxTriData 存储搜集到的网格数据
 */
void ExportManager::GatherMeshData( Mesh &lMesh, MaxTriObjData *pMaxTriData )
{
	//! 搜集顶点信息
	int nVerts = lMesh.getNumVerts();
	pMaxTriData->vVertexData.clear();
	for ( int i = 0; i < nVerts; i++ )
	{
		MaxVertexData vd;
		Point3 &pts = lMesh.getVert( i );

		/*! 3Dmax9是Z-up的右手坐标系，而DX是Y-up的左手坐标系，所以需要调整坐标（交换y坐标和z坐标） */
		vd.position[0] = -pts.y;
		vd.position[1] = pts.z;
		vd.position[2] = pts.x;

		pMaxTriData->vVertexData.push_back( vd );
	}

	m_logger->output ("\t找到顶点%d个\n", nVerts);

	//! 搜集纹理坐标信息
	int nTVerts = lMesh.getNumTVerts();
	pMaxTriData->vTexCoordData.clear();
	for ( int i = 0; i < nTVerts; i++ )
	{
		MaxTexCoordData tcd;
		UVVert &lTVert = lMesh.getTVert( i );
		/*! 3dmax的u,v坐标系是这样	\n
		  （0,1) ----- (1,1)		\n
		        |     |				\n
				|     |				\n
		   (0,0) ----- (1,0)		\n
		   而dx的u,v坐标系是这样	\n
		  （0,0) ----- (1,0)		\n
		        |     |				\n
		        |     |				\n
		   (0,1) ----- (1,1)		\n
		   所以，需要调整v坐标 */
		tcd.u = lTVert.x;
		tcd.v = 1 - lTVert.y;

		pMaxTriData->vTexCoordData.push_back( tcd );
	}
	m_logger->output ("\t找到贴图坐标%d个\n", nTVerts);

	//! 建立法线信息
	lMesh.buildNormals();
	//! 搜集三角形面信息
	int nTris = lMesh.getNumFaces();
	m_logger->output ("\t找到三角形%d个\n", nTris);
	for ( int i = 0; i < nTris; i++ )
	{
		MaxTriangleData td;
		//! 顶点坐标索引
		if ( nVerts > 0 )
		{
			Face& lFace = lMesh.faces[i];
			DWORD* pVIndices = lFace.getAllVerts();

			/*! 3Dmax9中三角形是逆时针，而DX是顺时针，所以需要调换顺序 */
			td.vertexIndices[0] = pVIndices[0];
			td.vertexIndices[1] = pVIndices[2];
			td.vertexIndices[2] = pVIndices[1];
		}
		else 
		{
			m_logger->output ("\t(**错误**)查找顶点索引失败\n");
		}

		//! 纹理坐标索引
		if ( nTVerts > 0 )
		{
			TVFace& lTVFace = lMesh.tvFace[i];
			DWORD* pUVIndices = lTVFace.getAllTVerts();		

			/*! 3Dmax9中三角形是逆时针，而DX是顺时针，所以需要调换顺序 */
			td.texCoordIndices[0] = pUVIndices[0];
			td.texCoordIndices[1] = pUVIndices[2];
			td.texCoordIndices[2] = pUVIndices[1];
		}
		else
		{
			m_logger->output ("\t(**错误**)查找贴图坐标索引失败\n");
		}

		//! 法线
		Point3& nvtx = Normalize( lMesh.getFaceNormal( i ) );
		/*! 3Dmax9是Z-up的右手坐标系，而DX是Y-up的左手坐标系，所以需要调整坐标（交换y坐标和z坐标） */
		td.normal[0] = -nvtx.y;
		td.normal[1] = nvtx.z;
		td.normal[2] = nvtx.x;

		//! Smoothing Group
		td.smoothGroup = lMesh.faces[i].smGroup;

		pMaxTriData->vTriangleData.push_back( td );
	}
}

/*! \param pMtl 3dmax材质
	\param pSubMesh 存储搜集到的网格数据
*/
void ExportManager::GatherMaterialData( Mtl *pMtl, MaxTriObjData *pMaxTriData, MaterialViewer *materialViewer )
{
	if ( pMtl == NULL )
	{
		return;
	}

	//! 检测是否是一个标准的Material
	if ( pMtl->ClassID() == Class_ID( DMTL_CLASS_ID, 0 ) )
	{
		StdMat *pStdMtl = ( StdMat * )pMtl;

		Texmap *pTex = pMtl->GetSubTexmap( ID_DI );
		//! 如果材质是一个图像文件，则
		if ( pTex != NULL && pTex->ClassID() == Class_ID( BMTEX_CLASS_ID, 0 ) )
		{
			BitmapTex* pBmpTex = static_cast< BitmapTex* >( pTex );
			//string texName = TruncatePath( pBmpTex->GetMapName() );
			//! 存储纹理名称
			BitmapInfo bitmapInfo;
			bitmapInfo.SetName (pBmpTex->GetMapName());
			BMMGetFullFilename (&bitmapInfo);
			string texName = bitmapInfo.Name();

			pMaxTriData->vMaterialInfos.push_back( MaterialInfo() );
			pMaxTriData->numTextures ++;
			pMaxTriData->vMaterialInfos.back().texture = texName;
			pMaxTriData->vMaterialInfos.back().name = pMtl->GetName();
			pMaxTriData->vMaterialInfos.back().doubleSide = pStdMtl->GetTwoSided () == TRUE;
		}
		else //! 材质不是一个图像文件，留空
		{
			pMaxTriData->vMaterialInfos.push_back( MaterialInfo() );
			pMaxTriData->vMaterialInfos.back().name = pMtl->GetName();
			pMaxTriData->vMaterialInfos.back().doubleSide = false;
			pMaxTriData->numTextures ++;
		}

		materialViewer->addMaterials (pMaxTriData->vMaterialInfos.back().name.c_str());
	}

	for ( int i = 0; i < pMtl->NumSubMtls(); i ++ )
	{
		Mtl *pSubMtl = pMtl->GetSubMtl( i );
		GatherMaterialData( pSubMtl, pMaxTriData, materialViewer );
	}
}

/*! \param pNode 3dmax场景节点
	\return 包含蒙皮信息的Modifier
 */
Modifier* ExportManager::GetSkinMode( INode *pNode )
{
	Object* pObj = pNode->GetObjectRef();
	if( !pObj )
	{
		return NULL;
	}
	while( pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID )
	{
		IDerivedObject *pDerivedObj = dynamic_cast< IDerivedObject* >( pObj );
		int modStackIndex = 0;
		while( modStackIndex < pDerivedObj->NumModifiers() )
		{
			Modifier* mod = pDerivedObj->GetModifier( modStackIndex );
			if( mod->ClassID() == SKIN_CLASSID )
			{
				return mod;
			}
			modStackIndex++;
		}
		pObj = pDerivedObj->GetObjRef();
	}
	return NULL;
}

void ExportManager::GenBoneTree()
{
	vector< INode* > freshBoneNodes; // 存储需要新加入到BoneList中的骨骼节点
	//! 寻找到所有的骨骼
	for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i ++ )
	{
		INode *boneNode = i->second.pBoneNode;

		INode *parentNode = boneNode->GetParentNode();
		if ( FALSE == parentNode->IsRootNode() && parentNode != NULL )
		{
			string parentBoneName = parentNode->GetName();
			BoneList::iterator k = m_boneList.find( parentBoneName );
			if ( k == m_boneList.end() ) // 该父骨骼节点不在模型的蒙皮骨骼列表中
			{
				// 加入到待搜索列表中
				freshBoneNodes.push_back( parentNode );
			}
		}
	}
	//! 加入所有新找到的骨骼到骨骼列表
	for ( vector< INode* >::iterator i = freshBoneNodes.begin(); i != freshBoneNodes.end(); i ++ )
	{
		INode *pBoneNode = *i;
		while ( TRUE )
		{
			string boneName = pBoneNode->GetName();
			BoneList::iterator k = m_boneList.find( boneName );
			if ( k == m_boneList.end() )
			{
				Bone newBone;
				newBone.pBoneNode = pBoneNode;
				m_boneList.insert( BoneList::value_type( boneName, newBone ) );

				pBoneNode = pBoneNode->GetParentNode();
				if ( TRUE == pBoneNode->IsRootNode() || pBoneNode == NULL )
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}

	//! 生成骨骼索引
	int realIndex = 0;
	for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i ++, realIndex ++ )
	{
		i->second.index = realIndex;
	}

	//! 建立骨骼的父子关系
	for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i ++ )
	{
		INode *boneNode = i->second.pBoneNode;
		INode *parentBoneNode = boneNode->GetParentNode();

		if ( FALSE == parentBoneNode->IsRootNode() && parentBoneNode != NULL )
		{
			string parentBoneName = parentBoneNode->GetName();
			BoneList::iterator k = m_boneList.find( parentBoneName );
			assert( k != m_boneList.end() );
			i->second.pParentBone = &( k->second );
		}
	}

	//! 建立与父骨骼的相对矩阵
	for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i ++ )
	{
		i->second.relativeMat = GetRelativeBoneMatrix( &( i->second ) );
	}

	//! 建立所有骨骼相对于父骨骼的动画帧
	for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i ++ )
	{
		for ( int j = 0; j < m_numAnimFrames; j ++ )
		{
			//! 第j帧的时刻time
			int time = (j + m_startFrame) * GetTicksPerFrame();

			Matrix3 relTraMatT = GetLocalBoneTranMatrix( &( i->second ), time );

			i->second.localFrames.push_back( relTraMatT );
		}
	}
}

/*! 这个函数很诡异，对于Bone都返回FALSE，对于Biped，footstep返回FALSE，其余的Biped则返回TRUE
	\param pNode 3dmax场景节点
	\return 判断结果
 */
BOOL ExportManager::IsBone( INode *pNode )
{
	if( pNode == NULL )
	{
		return FALSE; 
	}

	ObjectState os = pNode->EvalWorldState( 0 ); 
	if ( !os.obj ) 
	{
		return FALSE;
	}

	ULONG a = os.obj->ClassID().PartA();
	ULONG b = os.obj->ClassID().PartB();

	//! 检测是否是Bone
	if( os.obj->ClassID() == Class_ID( BONE_CLASS_ID, 0 ) )
	{
		return TRUE;
	}

	if ( os.obj->ClassID() == BONE_OBJ_CLASSID)
	{
		return TRUE;
	}

	//! dummy节点也算为骨骼
	if( os.obj->ClassID() == Class_ID( DUMMY_CLASS_ID, 0 ) )
	{
		return TRUE;
	}

	//! 检测是否是Biped
	Control *cont = pNode->GetTMController();   
	if( cont->ClassID() == BIPSLAVE_CONTROL_CLASS_ID ||       //others biped parts    
		cont->ClassID() == BIPBODY_CONTROL_CLASS_ID )         //biped root "Bip01"     
	{
		return TRUE;
	}

	return FALSE;   
}

/*! \param pContext 3dmax蒙皮信息
	\param pSkin 与mesh所相关联的骨骼息
	\param pSubMesh 存储搜集到的网格数据
 */
void ExportManager::GatherSkinData( ISkinContextData* pContext, ISkin *pSkin, MaxTriObjData *pMaxTriData )
{
	//! 将存储在单个Sub Mesh中的Bone索引对应到“搜集完场景中所有的Bone”之后的索引
	map< int, int > oriToRealMap;
	int iBoneCnt = pSkin->GetNumBones();
	for ( int oriIndex = 0; oriIndex < iBoneCnt; oriIndex ++ )
	{
		string boneName = pSkin->GetBone( oriIndex )->GetName();

		//! 寻找到真正的索引并存储到映射表
		BoneList::iterator iter = m_boneList.find( boneName );
		int realIndex = iter->second.index;
		if ( iter != m_boneList.end() )
		{
			oriToRealMap.insert( map<int, int>::value_type( oriIndex, realIndex ) );
		}
		else
		{
			throw std::runtime_error( "Couldn't find bone!" );
		}
	}

	//! 更新索引，并存储蒙皮信息
	int numVtx = pContext->GetNumPoints();
	for ( int iVtx = 0; iVtx < numVtx; iVtx++ )
	{
		int num = pContext->GetNumAssignedBones( iVtx );
		if (num > 4)
		{
			m_logger->output ("(**警告**)物体<%s>第<%d>个顶点权值为%d个，超过4个权值导出效果将可能出问题!\n", pMaxTriData->objName.c_str(), iVtx, num);
		}
		else if (num == 0)
		{
			m_logger->output ("(**警告**)物体<%s>第<%d>个顶点权值为0!\n", pMaxTriData->objName.c_str(), iVtx);
		}

		//assert( num <= 4 && "本插件最多一个顶点只能绑定4个骨骼，你超过了，现在将失败退出" );
		for ( int iVBone = 0; iVBone < num; iVBone++ )
		{
			int oriBoneIdx = pContext->GetAssignedBone( iVtx, iVBone );
			float weight = pContext->GetBoneWeight( iVtx, iVBone );

			int realBoneIdx = oriToRealMap.find( oriBoneIdx )->second;

			if (iVBone < 4)
			{
				pMaxTriData->vVertexData[iVtx].bones[iVBone] = realBoneIdx;
				pMaxTriData->vVertexData[iVtx].boneWeights[iVBone] = UTILITY->FilterData( weight );
			}
			else
			{
				int minIndex = 0;
				float minWeight = pMaxTriData->vVertexData[iVtx].boneWeights[minIndex];
				for (int n = 1; n < 4; ++n)
				{
					float f = pMaxTriData->vVertexData[iVtx].boneWeights[n];
					if (f < minWeight)
					{
						minWeight = f;
						minIndex = n;
					}
				}
				float w = UTILITY->FilterData(weight);
				if (w > minWeight)
				{
					pMaxTriData->vVertexData[iVtx].bones[minIndex] = realBoneIdx;
					pMaxTriData->vVertexData[iVtx].boneWeights[minIndex] = w;
				}
			}
		}

		for (int remain = num; remain < 4; ++remain)
		{
			pMaxTriData->vVertexData[iVtx].bones[remain] = 0;
			pMaxTriData->vVertexData[iVtx].boneWeights[remain] = 0.f;
		}
	}
}

/*! \param pSkin 包含与mesh所相关联的骨骼列表 */
void ExportManager::GatherBones( ISkin *pSkin )
{
	int iBoneCnt = pSkin->GetNumBones();
	m_logger->output ("\t获取骨骼%d个\n", iBoneCnt);

	for ( int i = 0; i < iBoneCnt; i ++ )
	{
		Bone bone;
		bone.pBoneNode = pSkin->GetBone( i );
		string boneName = bone.pBoneNode->GetName();
		
		m_boneList.insert( BoneList::value_type( boneName, bone ) );
	}
}

/*! \param pBone 骨骼 */
void ExportManager::GatherBoneFrames( Bone *pBone )
{
	INode *pBoneNode = pBone->pBoneNode;

	// 获得节点的transform control
	Control *c = pBoneNode->GetTMController();

	//! 测试是否是一个biped controller(关于代码的解释请参见3dmax SDK Document)
	if ( ( c->ClassID() == BIPSLAVE_CONTROL_CLASS_ID ) ||
		 ( c->ClassID() == BIPBODY_CONTROL_CLASS_ID ) ||
		 ( c->ClassID() == FOOTPRINT_CLASS_ID ) )
	{
		//!	Get the Biped Export Interface from the controller 
		IBipedExport *BipIface = ( IBipedExport * ) c->GetInterface( I_BIPINTERFACE );

		//!	Remove the non uniform scale
		BipIface->RemoveNonUniformScale( TRUE );

		//!	Release the interface when you are done with it
		c->ReleaseInterface( I_BIPINTERFACE, BipIface );
	}

	vector< Bone* > tempList;
	//! 遍历骨骼的父节点，直到根节点，加入到列表
	Bone *pRootBone = pBone;
	tempList.push_back( pRootBone );
	while ( pRootBone->pParentBone != NULL )
	{
		pRootBone = pRootBone->pParentBone;
		tempList.push_back( pRootBone );
	}

	//! baseMat是将Vworld转为Vlocal
	Matrix3 baseMat;
	baseMat.IdentityMatrix();
	//! 从根节点遍历，直到当前节点
	for ( vector< Bone* >::reverse_iterator riter = tempList.rbegin(); riter != tempList.rend(); riter ++ )
	{
		Bone *pB = *riter;
		baseMat =  baseMat * Inverse( pB->relativeMat );
	}

	//! 逐帧搜集
	for ( int i = 0; i < m_numAnimFrames; i ++ )
	{
		Matrix3 frame = baseMat;

		//! 第i帧的时刻time
		int time = (i + m_startFrame) * GetTicksPerFrame();

		//! 从当前节点开始，向父节点遍历，直到根节点停止
		for ( vector< Bone* >::iterator iter = tempList.begin(); iter != tempList.end(); iter ++ )
		{
			Bone *pB = *iter;
			//! 乘以第i帧的子骨骼相对于本地坐标系的变换
			frame = frame * pB->localFrames[i];
			//! 变换至父坐标系
			frame = frame * pB->relativeMat;
		}

		//! 至此获得了时刻time的关键帧矩阵
		pBone->boneFrames.push_back( frame );
	}
}

void ExportManager::insertBoneR (Bone *bone, ATOM_Skeleton *skeleton)
{
	if (bone->pParentBone)
	{
		insertBoneR (bone->pParentBone, skeleton);
	}

	std::vector<ATOM_Skeleton::joint> &joints = skeleton->getJoints ();
	for (unsigned i = 0; i < joints.size(); ++i)
	{
		if (joints[i].remapIndex == bone->index)
			return;
	}

	ATOM_Skeleton::joint newJoint;
	newJoint.name = bone->pBoneNode->GetName();
	newJoint.id = joints.size();
	newJoint.parentIndex = -1;
	if (bone->pParentBone)
	{
		for (int i = 0; i < joints.size(); ++i)
		{
			if (joints[i].remapIndex == bone->pParentBone->index)
			{
				newJoint.parentIndex = i;
				break;
			}
		}
	}
	newJoint.hashCode = ATOM_Skeleton::calcHash (newJoint.name.c_str(), newJoint.name.length());
	newJoint.remapIndex = bone->index;
	newJoint.localMatrix = UTILITY->TransformToDXMatrix (bone->relativeMat);
	newJoint.bindMatrix = newJoint.parentIndex == -1 ? newJoint.localMatrix : joints[newJoint.parentIndex].bindMatrix >> newJoint.localMatrix;
	joints.push_back (newJoint);
}

ATOM_Skeleton *ExportManager::CreateSkeleton (void)
{
	if (m_boneList.empty ())
	{
		return 0;
	}

	ATOM_Skeleton *skeleton = new ATOM_Skeleton;

	//! 将Bones写入文件
	for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i++)
	{
		insertBoneR (&i->second, skeleton);
	}

	return skeleton;
}

int ExportManager::findNewJointIndex (ATOM_Skeleton *skeleton, int oldIndex)
{
	for (int i = 0; i < skeleton->getJoints().size(); ++i)
	{
		if (skeleton->getJoints()[i].remapIndex == oldIndex)
			return i;
	}
	return -1;
}

BOOL ExportManager::ExportToMyModel (const char *outFileName, ATOM_Skeleton *skeleton, MyModel *model, VertexAnimationInfo *info, bool exportMeshes, bool exportActions, bool exportBindable, bool exportSkeleton, bool shareDiffuseMap)
{
	for ( MaxTriObjList::iterator i = m_vpMaxObjs.begin(); i != m_vpMaxObjs.end(); i ++ )
	{
		MaxTriObjData *pMaxObj = *i;
		//UTILITY->TransformToWorld( pMaxObj, exportBindable ? pMaxObj->relativeMat : pMaxObj->worldMat );

		for ( SubMeshList::iterator j = pMaxObj->vSubMeshes.begin(); j != pMaxObj->vSubMeshes.end(); j ++ )
		{
			CSMSubMesh *pSubMesh = *j;

			unsigned numAnimFrames = pSubMesh->subMeshHeader.numAnimFrames;
			unsigned numVertices = pSubMesh->subMeshHeader.numVertices;
			unsigned numFaces = pSubMesh->subMeshHeader.numFaces;

			MyMesh myMesh;
			for ( int k = 0; k < pSubMesh->subMeshHeader.numFaces; k ++ )
			{
				CSMTriangleData *pTriData = &pSubMesh->vTriangleData[k];

				for (int l = 0; l < 3; ++l)
				{
					CSMVertexData *pVertex = &(pSubMesh->vVertexData[pTriData->vertexIndices[l]]);
					MyMesh::Vertex vtx;
					memset (&vtx, 0, sizeof(MyMesh::Vertex));

					vtx.position.set (pVertex->position[0], pVertex->position[1], pVertex->position[2]);
					vtx.normal.set (pVertex->normal[0], pVertex->normal[1], pVertex->normal[2]);
					vtx.texcoords.set (pVertex->u, pVertex->v);

					if (numAnimFrames > 1)
					{
						for (int n = 0; n < 4; ++n)
						{
							vtx.weightjoints.xyzw[n] = findNewJointIndex(skeleton, pSubMesh->vSkinData[pTriData->vertexIndices[l]].bones[n]);
							vtx.weights.xyzw[n] = pSubMesh->vSkinData[pTriData->vertexIndices[l]].boneWeights[n];
							if (vtx.weightjoints.xyzw[n] < 0)
								vtx.weightjoints.xyzw[n] = 0;
						}
					}

					myMesh.pushSuperVertex (vtx);
				}
			}

			std::string fileName = identifyFileName (pSubMesh->materialInfo.texture.c_str());
			std::string vfsFileName, physicFileName;
			if(!fileName.empty ())
			{
				translateFileName (fileName.c_str(), outFileName, vfsFileName, physicFileName, shareDiffuseMap);
			}
			if (exportMeshes && _stricmp (fileName.c_str(), physicFileName.c_str()))
			{
				::CopyFileA (fileName.c_str(), physicFileName.c_str(), FALSE);
			}

			material &mat = myMesh.getMaterial();
			mat.name = "";
			mat.diffuseMap1 = vfsFileName;
			mat.emissiveColor.set (0.f, 0.f, 0.f);
			mat.diffuseColor.set (1.f, 1.f, 1.f);
			mat.ambientColor.set (0.f, 0.f, 0.f);
			mat.alphaTest = pSubMesh->materialInfo.alphaTest;
			mat.doubleSide = pSubMesh->materialInfo.doubleSide;
			mat.transparency = 1.f;
			mat.diffuseChannel1 = 0;

			myMesh.setSkeleton (skeleton);
			myMesh.useSkeleton (skeleton != 0);
			myMesh.setNumActions (info->actionSet.size());

			model->addMesh (myMesh);
		}
	}

	if (info->actionSet.size() > 0)
	{
		model->setNumActions (info->actionSet.size());
		int actionindex = 0;
		for (std::map<std::string, VertexAnimationAction>::iterator it = info->actionSet.begin(); it != info->actionSet.end(); ++it, ++actionindex)
		{
			model->setActionName (actionindex, it->first.c_str());
			model->setActionHash (actionindex, 0);
			model->setActionProps (actionindex, &it->second.properties);

			const std::vector<int> &frameList = it->second.frameList;
			model->setNumActionFrames (actionindex, frameList.size());
			if (frameList.size() > 0)
			{
				for (unsigned k = 0; k < frameList.size(); ++k)
				{
					model->setFrameTime (actionindex, k, frameList[k] * TicksToSec(GetTicksPerFrame()) * 1000);
				}
			}

			ATOM_BBox actionBBox;
			actionBBox.beginExtend ();
			for (unsigned i = 0; i < frameList.size(); ++i)
			{
				std::vector<ATOM_Matrix4x4f> matrices(skeleton->getJoints().size());

				for (unsigned j = 0; j < skeleton->getJoints().size(); ++j)
				{
					BoneList::iterator itBone = m_boneList.find (skeleton->getJoints()[j].name);
					matrices[j] = UTILITY->TransformToDXMatrix(itBone->second.localFrames[frameList[i]-info->rangeInfo.frameStart]);
				}

				for (unsigned mesh = 0; mesh < model->meshes().size(); ++mesh)
				{
					model->meshes()[mesh].addAnimationFrame (actionindex, ATOM_Matrix4x4f::getIdentityMatrix(), std::vector<ATOM_Vector3f>(), std::vector<ATOM_Vector2f>());
					model->meshes()[mesh].addMaterialFrame (actionindex, 1.f);

					if (skeleton)
					{
						model->meshes()[mesh].addSkeletonFrame (actionindex, matrices);
					}
				}
				model->setFrameJoints (actionindex, i, matrices);
			}
		}
	}

	return TRUE;
}

/*! \param pNode 3dmax场景节点
	\param t 时间点
	\return 骨骼矩阵
 */
Matrix3 ExportManager::GetBoneTM( INode *pNode, TimeValue t )   
{   
	Matrix3 tm = pNode->GetNodeTM( t );
	tm.NoScale();   
	return tm;   
} 

/*! \param pNode 3dmax场景中的节点
	\param t 时间
	\return 在时刻t节点相对于其父节点的矩阵
 */
Matrix3 ExportManager::GetRelativeMatrix( INode *pNode, TimeValue t /* = 0  */ )
{
	Matrix3 worldMat = pNode->GetNodeTM( t );
	Matrix3 parentMat = pNode->GetParentTM( t );

	//! 因为NodeWorldTM = NodeLocalTM * ParentWorldTM（注意：3dmax9中的矩阵是右乘）\n
	//! 所以NodeLocalTM = NodeWorldTM * Inverse( ParentWorldTM )
	Matrix3 relativeMat = worldMat * Inverse( parentMat );

	return relativeMat;
}

/*! 根骨骼的节点可能在场景中还有父节点（比如Scene Root）
	\param pBone 骨骼
	\param t 时间
	\return 在时刻t骨骼相对于其父骨骼的矩阵
 */
Matrix3 ExportManager::GetRelativeBoneMatrix( Bone *pBone, TimeValue t /* = 0  */ )
{
	if ( pBone->pParentBone == NULL )
	{
		Matrix3 worldMat = pBone->pBoneNode->GetNodeTM( t );
		return worldMat;
	}
	else
	{
		return GetRelativeMatrix( pBone->pBoneNode, t );
	}
}

/*! \param pBone 3dmax场景中的节点
	\param t 时间
	\return 骨骼在时刻t在本地空间内的变换
 */
Matrix3 ExportManager::GetLocalBoneTranMatrix( Bone *pBone, TimeValue t )
{
	Matrix3 relMatT0 = GetRelativeBoneMatrix( pBone, 0 );
	Matrix3 relMatTN = GetRelativeBoneMatrix( pBone, t );
	//! 因为relMatT = transformMatT * relMat0
	//! 所以transformMatT = relMatT * Inverse( relMat0 ) 
	Matrix3 transformMatT = relMatTN * Inverse( relMatT0 );

	return transformMatT;
}