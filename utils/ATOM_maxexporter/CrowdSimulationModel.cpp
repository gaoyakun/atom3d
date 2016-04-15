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
			MessageBox (::GetActiveWindow(), "�����·�������⣬��ȷ���Ǳ�������Դ��Ŀ¼֮�¡�", "atom exporter", MB_OK|MB_ICONHAND);
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

/*! \param pNode �����п�ʼѰ�ҵĽڵ�
 *	\param nodeName Ѱ�ҽڵ������
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

/*! \param pRootNode 3dmax�����ĸ��ڵ�
 *	\param bExportAnim �Ƿ񵼳�����
 *	\param bExportSelections �Ƿ�ֻ����ѡ��Ľڵ�
 */
void ExportManager::Gather3DSMAXSceneData( INode *pRootNode, 
										   BOOL bExportAnim, 
										   BOOL bExportSelections,
										   MaterialViewer *materialViewer)
{
	m_logger->output ("========�ռ�������Ϣ========\n");

	//! �Ѽ�������֡��Ϣ
	Interval ivAnimRange = GetCOREInterface()->GetAnimRange();
	m_startFrame = ivAnimRange.Start() / GetTicksPerFrame();
	m_endFrame = ivAnimRange.End() / GetTicksPerFrame();
	m_numAnimFrames = m_endFrame - m_startFrame + 1;
	m_logger->output ("��ʼ֡:%d, ����֡:%d, ��%d֡\n", m_startFrame, m_endFrame, m_numAnimFrames);

	if ( bExportAnim != TRUE )
	{
		// û�ж����򳡾�ֻ��1֡
		m_numAnimFrames = 1;
	}

	// �Ѽ����нڵ���Ϣ
	m_logger->output ("--�ռ��ڵ���Ϣ--\n");
	GatherRecursiveNodeData( pRootNode, bExportAnim, bExportSelections, materialViewer );

#if 1
	// ����Meshת��������ռ�
	for ( MaxTriObjList::iterator i = m_vpMaxObjs.begin(); i != m_vpMaxObjs.end(); i ++ )
	{
		MaxTriObjData *pMaxObj = *i;
		UTILITY->TransformToWorld( pMaxObj, pMaxObj->worldMat );
	}
#endif

	//! �Ѽ���������
	if ( bExportAnim == TRUE )
	{
		//! ���ɹ������ṹ
		GenBoneTree();

		//! �Ѽ�������Ƥ��Ϣ
		for ( SubMeshBoneList::iterator i = m_submeshBones.begin(); i != m_submeshBones.end(); i ++ )
		{
			MaxTriObjData *pMaxObjData = i->pMaxObjData;
			ISkinContextData *pSkinContext = i->pSkinContext;
			ISkin *pSkin = i->pSkin;

			GatherSkinData( pSkinContext, pSkin, pMaxObjData );
		}

		//! �Ѽ�����������Ϣ
		for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i ++ )
		{
			GatherBoneFrames( &( i->second ) );
		}
	}

	//! ��Max����תΪCSM����
	for ( MaxTriObjList::iterator i = m_vpMaxObjs.begin(); i != m_vpMaxObjs.end(); i ++ )
	{
		MaxTriObjData *pTriData = *i;
		UTILITY->MaxDataToCSMData( pTriData );
	}
}

/*! \param pNode �����еĽڵ�
 *	\param bExportAnim �Ƿ񵼳�����
 */
void ExportManager::GatherRecursiveNodeData( INode *pNode, BOOL bExportAnim, BOOL bExportSelections, MaterialViewer *materialViewer )
{
	GatherNodeData( pNode, bExportAnim, bExportSelections, materialViewer );

	// �ݹ��Ѽ��������еĽڵ�
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

/*! \param pNode �����еĽڵ�
 *	\param bExportAnim �Ƿ񵼳�����
 *	\return ����ýڵ��Ǳ��������Ľڵ㣬�򷵻�TRUE�����򷵻�FALSE
 */
BOOL ExportManager::GatherNodeData( INode *pNode, BOOL bExportAnim, BOOL bExportSelections, MaterialViewer *materialViewer )
{
	// ���BaseObject
	ObjectState os = pNode->EvalWorldState( 0 );
	Object *pObj = os.obj;
	std::string name = pNode->GetName();

	// �ڱ�export����У�ֻ���δ���ص�triObject
	if ( pObj != NULL &&
		 (FALSE == bExportSelections || pNode->Selected()) && 
		 FALSE == pNode->IsNodeHidden() &&
		 FALSE == IsBone( pNode ) &&		// ���ǹ���
		 pObj->CanConvertToType( triObjectClassID ) )
	{
		TriObject* pTriObj = static_cast< TriObject* >( pObj->ConvertToType( 0, triObjectClassID ) );
		if ( pTriObj != NULL )
		{
			m_logger->output ("���ֿ�����ڵ�<%s>\n", pNode->GetName());

			MaxTriObjData *pMaxTriData = new MaxTriObjData();

			// ����
			pMaxTriData->objName = pNode->GetName();

			// Mesh���������
			Matrix3 worldMat = pNode->GetObjectTM( 0 );
			Matrix3 parentMat = pNode->GetParentTM( 0 );
			Matrix3 relativeMat = worldMat * Inverse( parentMat );
			
			pMaxTriData->worldMat = UTILITY->TransformToDXMatrix( worldMat );
			pMaxTriData->relativeMat = UTILITY->TransformToDXMatrix( relativeMat );

			// �Ѽ�Mesh����
			Mesh &lMesh = pTriObj->GetMesh();
			GatherMeshData( lMesh, pMaxTriData );

			// �Ѽ���������
			Mtl* pMtl = pNode->GetMtl();
			if ( pMtl != NULL )
			{
				GatherMaterialData( pMtl, pMaxTriData, materialViewer );
				
				if ( pMaxTriData->numTextures > 0 )
				{
					for ( vector< MaxTriangleData >::iterator i = pMaxTriData->vTriangleData.begin(); i != pMaxTriData->vTriangleData.end(); i ++ )
					{
						int faceID = i - pMaxTriData->vTriangleData.begin();
						// ��ø����������Material ID
						i->materialID = lMesh.getFaceMtlIndex( faceID ) % pMaxTriData->numTextures;
					}
				}
			}
			else
			{
				m_logger->output("\tû�в���\n");

				// ���һ���ղ���
				pMaxTriData->vMaterialInfos.push_back( MaterialInfo());
				pMaxTriData->numTextures ++;
			}

			// ���㶥�㷨�ߣ��������Ѽ�����֮����Ϊ���߷����������أ�
			UTILITY->ComputeVertexNormalsOpt( pMaxTriData );

			if ( bExportAnim == TRUE )
			{
				// �Ѽ���������
				Modifier *pMf = GetSkinMode( pNode );
				if ( pMf != NULL )
				{
					ISkin* pSkin = static_cast< ISkin* > ( pMf->GetInterface( I_SKIN ) );
					ISkinContextData* pContext = pSkin->GetContextInterface( pNode );

					// �ȴ洢�������������Ѽ���Node�Ĺ�����Ƥ��Ϣ����ô����Ҫ�ȴ����еĹ�������������ȷ����
					SubMeshBones pSubMeshBones;
					pSubMeshBones.pMaxObjData = pMaxTriData;
					pSubMeshBones.pSkinContext = pContext;
					pSubMeshBones.pSkin = pSkin;
					m_submeshBones.push_back( pSubMeshBones );

					// �Ѽ�����
					GatherBones( pSkin );

					// �ж���
					pMaxTriData->numAnimFrames = m_numAnimFrames;
				}
				else
				{
					m_logger->output("\tû�й���\n");
					// û�ж���
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

					m_logger->output ("���ְ󶨵�: %s\n", boneName.c_str());
					m_boneList.insert( BoneList::value_type( boneName, bone ) );
				}
			}
		}
	}
	
	return FALSE;
}

/*! \param lMesh 3dmax����
	\param pMaxTriData �洢�Ѽ�������������
 */
void ExportManager::GatherMeshData( Mesh &lMesh, MaxTriObjData *pMaxTriData )
{
	//! �Ѽ�������Ϣ
	int nVerts = lMesh.getNumVerts();
	pMaxTriData->vVertexData.clear();
	for ( int i = 0; i < nVerts; i++ )
	{
		MaxVertexData vd;
		Point3 &pts = lMesh.getVert( i );

		/*! 3Dmax9��Z-up����������ϵ����DX��Y-up����������ϵ��������Ҫ�������꣨����y�����z���꣩ */
		vd.position[0] = -pts.y;
		vd.position[1] = pts.z;
		vd.position[2] = pts.x;

		pMaxTriData->vVertexData.push_back( vd );
	}

	m_logger->output ("\t�ҵ�����%d��\n", nVerts);

	//! �Ѽ�����������Ϣ
	int nTVerts = lMesh.getNumTVerts();
	pMaxTriData->vTexCoordData.clear();
	for ( int i = 0; i < nTVerts; i++ )
	{
		MaxTexCoordData tcd;
		UVVert &lTVert = lMesh.getTVert( i );
		/*! 3dmax��u,v����ϵ������	\n
		  ��0,1) ----- (1,1)		\n
		        |     |				\n
				|     |				\n
		   (0,0) ----- (1,0)		\n
		   ��dx��u,v����ϵ������	\n
		  ��0,0) ----- (1,0)		\n
		        |     |				\n
		        |     |				\n
		   (0,1) ----- (1,1)		\n
		   ���ԣ���Ҫ����v���� */
		tcd.u = lTVert.x;
		tcd.v = 1 - lTVert.y;

		pMaxTriData->vTexCoordData.push_back( tcd );
	}
	m_logger->output ("\t�ҵ���ͼ����%d��\n", nTVerts);

	//! ����������Ϣ
	lMesh.buildNormals();
	//! �Ѽ�����������Ϣ
	int nTris = lMesh.getNumFaces();
	m_logger->output ("\t�ҵ�������%d��\n", nTris);
	for ( int i = 0; i < nTris; i++ )
	{
		MaxTriangleData td;
		//! ������������
		if ( nVerts > 0 )
		{
			Face& lFace = lMesh.faces[i];
			DWORD* pVIndices = lFace.getAllVerts();

			/*! 3Dmax9������������ʱ�룬��DX��˳ʱ�룬������Ҫ����˳�� */
			td.vertexIndices[0] = pVIndices[0];
			td.vertexIndices[1] = pVIndices[2];
			td.vertexIndices[2] = pVIndices[1];
		}
		else 
		{
			m_logger->output ("\t(**����**)���Ҷ�������ʧ��\n");
		}

		//! ������������
		if ( nTVerts > 0 )
		{
			TVFace& lTVFace = lMesh.tvFace[i];
			DWORD* pUVIndices = lTVFace.getAllTVerts();		

			/*! 3Dmax9������������ʱ�룬��DX��˳ʱ�룬������Ҫ����˳�� */
			td.texCoordIndices[0] = pUVIndices[0];
			td.texCoordIndices[1] = pUVIndices[2];
			td.texCoordIndices[2] = pUVIndices[1];
		}
		else
		{
			m_logger->output ("\t(**����**)������ͼ��������ʧ��\n");
		}

		//! ����
		Point3& nvtx = Normalize( lMesh.getFaceNormal( i ) );
		/*! 3Dmax9��Z-up����������ϵ����DX��Y-up����������ϵ��������Ҫ�������꣨����y�����z���꣩ */
		td.normal[0] = -nvtx.y;
		td.normal[1] = nvtx.z;
		td.normal[2] = nvtx.x;

		//! Smoothing Group
		td.smoothGroup = lMesh.faces[i].smGroup;

		pMaxTriData->vTriangleData.push_back( td );
	}
}

/*! \param pMtl 3dmax����
	\param pSubMesh �洢�Ѽ�������������
*/
void ExportManager::GatherMaterialData( Mtl *pMtl, MaxTriObjData *pMaxTriData, MaterialViewer *materialViewer )
{
	if ( pMtl == NULL )
	{
		return;
	}

	//! ����Ƿ���һ����׼��Material
	if ( pMtl->ClassID() == Class_ID( DMTL_CLASS_ID, 0 ) )
	{
		StdMat *pStdMtl = ( StdMat * )pMtl;

		Texmap *pTex = pMtl->GetSubTexmap( ID_DI );
		//! ���������һ��ͼ���ļ�����
		if ( pTex != NULL && pTex->ClassID() == Class_ID( BMTEX_CLASS_ID, 0 ) )
		{
			BitmapTex* pBmpTex = static_cast< BitmapTex* >( pTex );
			//string texName = TruncatePath( pBmpTex->GetMapName() );
			//! �洢��������
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
		else //! ���ʲ���һ��ͼ���ļ�������
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

/*! \param pNode 3dmax�����ڵ�
	\return ������Ƥ��Ϣ��Modifier
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
	vector< INode* > freshBoneNodes; // �洢��Ҫ�¼��뵽BoneList�еĹ����ڵ�
	//! Ѱ�ҵ����еĹ���
	for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i ++ )
	{
		INode *boneNode = i->second.pBoneNode;

		INode *parentNode = boneNode->GetParentNode();
		if ( FALSE == parentNode->IsRootNode() && parentNode != NULL )
		{
			string parentBoneName = parentNode->GetName();
			BoneList::iterator k = m_boneList.find( parentBoneName );
			if ( k == m_boneList.end() ) // �ø������ڵ㲻��ģ�͵���Ƥ�����б���
			{
				// ���뵽�������б���
				freshBoneNodes.push_back( parentNode );
			}
		}
	}
	//! �����������ҵ��Ĺ����������б�
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

	//! ���ɹ�������
	int realIndex = 0;
	for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i ++, realIndex ++ )
	{
		i->second.index = realIndex;
	}

	//! ���������ĸ��ӹ�ϵ
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

	//! �����븸��������Ծ���
	for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i ++ )
	{
		i->second.relativeMat = GetRelativeBoneMatrix( &( i->second ) );
	}

	//! �������й�������ڸ������Ķ���֡
	for ( BoneList::iterator i = m_boneList.begin(); i != m_boneList.end(); i ++ )
	{
		for ( int j = 0; j < m_numAnimFrames; j ++ )
		{
			//! ��j֡��ʱ��time
			int time = (j + m_startFrame) * GetTicksPerFrame();

			Matrix3 relTraMatT = GetLocalBoneTranMatrix( &( i->second ), time );

			i->second.localFrames.push_back( relTraMatT );
		}
	}
}

/*! ��������ܹ��죬����Bone������FALSE������Biped��footstep����FALSE�������Biped�򷵻�TRUE
	\param pNode 3dmax�����ڵ�
	\return �жϽ��
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

	//! ����Ƿ���Bone
	if( os.obj->ClassID() == Class_ID( BONE_CLASS_ID, 0 ) )
	{
		return TRUE;
	}

	if ( os.obj->ClassID() == BONE_OBJ_CLASSID)
	{
		return TRUE;
	}

	//! dummy�ڵ�Ҳ��Ϊ����
	if( os.obj->ClassID() == Class_ID( DUMMY_CLASS_ID, 0 ) )
	{
		return TRUE;
	}

	//! ����Ƿ���Biped
	Control *cont = pNode->GetTMController();   
	if( cont->ClassID() == BIPSLAVE_CONTROL_CLASS_ID ||       //others biped parts    
		cont->ClassID() == BIPBODY_CONTROL_CLASS_ID )         //biped root "Bip01"     
	{
		return TRUE;
	}

	return FALSE;   
}

/*! \param pContext 3dmax��Ƥ��Ϣ
	\param pSkin ��mesh��������Ĺ���Ϣ
	\param pSubMesh �洢�Ѽ�������������
 */
void ExportManager::GatherSkinData( ISkinContextData* pContext, ISkin *pSkin, MaxTriObjData *pMaxTriData )
{
	//! ���洢�ڵ���Sub Mesh�е�Bone������Ӧ�����Ѽ��곡�������е�Bone��֮�������
	map< int, int > oriToRealMap;
	int iBoneCnt = pSkin->GetNumBones();
	for ( int oriIndex = 0; oriIndex < iBoneCnt; oriIndex ++ )
	{
		string boneName = pSkin->GetBone( oriIndex )->GetName();

		//! Ѱ�ҵ��������������洢��ӳ���
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

	//! �������������洢��Ƥ��Ϣ
	int numVtx = pContext->GetNumPoints();
	for ( int iVtx = 0; iVtx < numVtx; iVtx++ )
	{
		int num = pContext->GetNumAssignedBones( iVtx );
		if (num > 4)
		{
			m_logger->output ("(**����**)����<%s>��<%d>������ȨֵΪ%d��������4��Ȩֵ����Ч�������ܳ�����!\n", pMaxTriData->objName.c_str(), iVtx, num);
		}
		else if (num == 0)
		{
			m_logger->output ("(**����**)����<%s>��<%d>������ȨֵΪ0!\n", pMaxTriData->objName.c_str(), iVtx);
		}

		//assert( num <= 4 && "��������һ������ֻ�ܰ�4���������㳬���ˣ����ڽ�ʧ���˳�" );
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

/*! \param pSkin ������mesh��������Ĺ����б� */
void ExportManager::GatherBones( ISkin *pSkin )
{
	int iBoneCnt = pSkin->GetNumBones();
	m_logger->output ("\t��ȡ����%d��\n", iBoneCnt);

	for ( int i = 0; i < iBoneCnt; i ++ )
	{
		Bone bone;
		bone.pBoneNode = pSkin->GetBone( i );
		string boneName = bone.pBoneNode->GetName();
		
		m_boneList.insert( BoneList::value_type( boneName, bone ) );
	}
}

/*! \param pBone ���� */
void ExportManager::GatherBoneFrames( Bone *pBone )
{
	INode *pBoneNode = pBone->pBoneNode;

	// ��ýڵ��transform control
	Control *c = pBoneNode->GetTMController();

	//! �����Ƿ���һ��biped controller(���ڴ���Ľ�����μ�3dmax SDK Document)
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
	//! ���������ĸ��ڵ㣬ֱ�����ڵ㣬���뵽�б�
	Bone *pRootBone = pBone;
	tempList.push_back( pRootBone );
	while ( pRootBone->pParentBone != NULL )
	{
		pRootBone = pRootBone->pParentBone;
		tempList.push_back( pRootBone );
	}

	//! baseMat�ǽ�VworldתΪVlocal
	Matrix3 baseMat;
	baseMat.IdentityMatrix();
	//! �Ӹ��ڵ������ֱ����ǰ�ڵ�
	for ( vector< Bone* >::reverse_iterator riter = tempList.rbegin(); riter != tempList.rend(); riter ++ )
	{
		Bone *pB = *riter;
		baseMat =  baseMat * Inverse( pB->relativeMat );
	}

	//! ��֡�Ѽ�
	for ( int i = 0; i < m_numAnimFrames; i ++ )
	{
		Matrix3 frame = baseMat;

		//! ��i֡��ʱ��time
		int time = (i + m_startFrame) * GetTicksPerFrame();

		//! �ӵ�ǰ�ڵ㿪ʼ���򸸽ڵ������ֱ�����ڵ�ֹͣ
		for ( vector< Bone* >::iterator iter = tempList.begin(); iter != tempList.end(); iter ++ )
		{
			Bone *pB = *iter;
			//! ���Ե�i֡���ӹ�������ڱ�������ϵ�ı任
			frame = frame * pB->localFrames[i];
			//! �任��������ϵ
			frame = frame * pB->relativeMat;
		}

		//! ���˻����ʱ��time�Ĺؼ�֡����
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

	//! ��Bonesд���ļ�
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

/*! \param pNode 3dmax�����ڵ�
	\param t ʱ���
	\return ��������
 */
Matrix3 ExportManager::GetBoneTM( INode *pNode, TimeValue t )   
{   
	Matrix3 tm = pNode->GetNodeTM( t );
	tm.NoScale();   
	return tm;   
} 

/*! \param pNode 3dmax�����еĽڵ�
	\param t ʱ��
	\return ��ʱ��t�ڵ�������丸�ڵ�ľ���
 */
Matrix3 ExportManager::GetRelativeMatrix( INode *pNode, TimeValue t /* = 0  */ )
{
	Matrix3 worldMat = pNode->GetNodeTM( t );
	Matrix3 parentMat = pNode->GetParentTM( t );

	//! ��ΪNodeWorldTM = NodeLocalTM * ParentWorldTM��ע�⣺3dmax9�еľ������ҳˣ�\n
	//! ����NodeLocalTM = NodeWorldTM * Inverse( ParentWorldTM )
	Matrix3 relativeMat = worldMat * Inverse( parentMat );

	return relativeMat;
}

/*! �������Ľڵ�����ڳ����л��и��ڵ㣨����Scene Root��
	\param pBone ����
	\param t ʱ��
	\return ��ʱ��t����������丸�����ľ���
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

/*! \param pBone 3dmax�����еĽڵ�
	\param t ʱ��
	\return ������ʱ��t�ڱ��ؿռ��ڵı任
 */
Matrix3 ExportManager::GetLocalBoneTranMatrix( Bone *pBone, TimeValue t )
{
	Matrix3 relMatT0 = GetRelativeBoneMatrix( pBone, 0 );
	Matrix3 relMatTN = GetRelativeBoneMatrix( pBone, t );
	//! ��ΪrelMatT = transformMatT * relMat0
	//! ����transformMatT = relMatT * Inverse( relMat0 ) 
	Matrix3 transformMatT = relMatTN * Inverse( relMatT0 );

	return transformMatT;
}