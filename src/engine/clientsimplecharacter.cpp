#include "stdafx.h"
#include "ClientSimpleCharacter.h"

#define USE_ASYNC_ATTRIBUTEFILE_IO 1

ATOM_SCRIPT_INTERFACE_BEGIN(ClientSimpleCharacter)
ATOM_ATTRIBUTES_BEGIN(ClientSimpleCharacter)
ATOM_ATTRIBUTE_PERSISTENT(ClientSimpleCharacter, "Action", getAction, setAction, "", "group=ClientSimpleCharacter")
ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ClientSimpleCharacter, ATOM_Geode)

ATOM_IMPLEMENT_NODE_FILE_TYPE(ClientSimpleCharacter)

// wangjian added
static void ATOM_CALL CSP_FIXEDEFFECT_NoRotate(ATOM_Geode *geode, const char *boneName, ATOM_Node *attachedNode, ATOM_Matrix4x4f *m, void *userData)
{
	m->setRow(0, 1.f, 0.f, 0.f, 0.f);
	m->setRow(1, 0.f, 1.f, 0.f, 0.f);
	m->setRow(2, 0.f, 0.f, 1.f, 0.f);
}

//--- wangjian modified ---//
// 异步加载

ATOM_AUTOREF(ATOM_Node) LoadNode(const char *filename, ATOM_Node *parentNode, int loadPriority = ATOM_LoadPriority_IMMEDIATE )
{
	ATOM_STACK_TRACE(LoadNode);

	ATOM_AUTOREF(ATOM_Node) node = ATOM_Node::loadNodeFromFile (filename, loadPriority);
	if (node)
	{
		if (parentNode)
		{
			parentNode->appendChild (node.get());
		}
		return node;
	}
	return 0;
}

// 构造函数
ClientSimpleCharacter::ClientSimpleCharacter()
{
	ATOM_STACK_TRACE(ClientSimpleCharacter::ClientSimpleCharacter);
	_bindLightColor = 0xFFFFFFFF;
	_bindLightDir.set (1.f, 0.f, 0.f);
	_bindAmbient = 0;
	_bindFOV = 0.f;
	_bindLightIntensity = 1.0f;
}

//--- wangjian added ---//
ClientSimpleCharacter::~ClientSimpleCharacter()
{
	_attachInfoList.clear();
	_avatarInfoList.clear();
	_materialDependInfoList.clear();
}
//----------------------//

// 读配置
bool ClientSimpleCharacter::loadAttribute(const TiXmlElement *pXmlElement)
{
	ATOM_STACK_TRACE(ClientSimpleCharacter::loadAttribute);
	int nValue;
	const char* pValue;
	EquipInfo info;

	// 获取参考点
	_referencePointMap.clear ();
	for (const ATOM_TiXmlElement *pointElement = pXmlElement->FirstChildElement("point"); pointElement; pointElement = pointElement->NextSiblingElement("point"))
	{
		const char *name = pointElement->Attribute("Name");
		if (!name)
		{
			continue;
		}

		const char *mtx = pointElement->Attribute("Matrix");
		if (!mtx)
		{
			continue;
		}

		ATOM_Matrix4x4f matrix;
		if (16 != sscanf(mtx, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", 
			&matrix.m00, &matrix.m01, &matrix.m02, &matrix.m03,
			&matrix.m10, &matrix.m11, &matrix.m12, &matrix.m13,
			&matrix.m20, &matrix.m21, &matrix.m22, &matrix.m23,
			&matrix.m30, &matrix.m31, &matrix.m32, &matrix.m33))
		{
			continue;
		}

		_referencePointMap[name] = matrix;
	}

	const ATOM_TiXmlElement *eFOV = pXmlElement->FirstChildElement("FOV");
	_bindFOV = 0.f;
	if (eFOV)
	{
		float value = 0.f;
		eFOV->QueryFloatAttribute("value", &value);
		_bindFOV = value;
	}

	_bindLightColor = 0xFFFFFFFF;
	_bindLightDir.set (1.f, 0.f, 0.f);
	_bindAmbient = 0;
	_bindLightIntensity = 1.0f;

	const ATOM_TiXmlElement *eLightColor = pXmlElement->FirstChildElement("lightcolor");
	if (eLightColor)
	{
		int color = 0xFFFFFFFF;
		eLightColor->QueryIntAttribute ("value", &color);
		_bindLightColor.setRaw (color);
	}
	const ATOM_TiXmlElement *eLightDir = pXmlElement->FirstChildElement ("lightdir");
	if (eLightDir)
	{
		const char *dir = eLightDir->Attribute ("value");
		if (!dir || 3 != sscanf(dir, "%f,%f,%f", &_bindLightDir.x, &_bindLightDir.y, &_bindLightDir.z))
		{
			_bindLightDir.set (1.f, 0.f, 0.f);
		}
	}
	const ATOM_TiXmlElement *eLightIntensity = pXmlElement->FirstChildElement ("lightintensity");
	if (eLightIntensity)
	{
		float value = 0.f;
		eLightIntensity->QueryFloatAttribute("value", &value);
		_bindLightIntensity = value;
	}
	const ATOM_TiXmlElement *eAmbientColor = pXmlElement->FirstChildElement("ambient");
	if (eAmbientColor)
	{
		int color = 0;
		eAmbientColor->QueryIntAttribute ("value", &color);
		_bindAmbient.setRaw (color);
	}

	// 生成子节点
	_attachInfoList.clear();
	const TiXmlElement* pSubElement = pXmlElement->FirstChildElement("equip");
	for(; pSubElement; pSubElement = pSubElement->NextSiblingElement("equip"))
	{
		info._skeletonIndex = pSubElement->Attribute("SkeletonIndex", &nValue) ? nValue : 0;
		pValue = pSubElement->Attribute("AttachPoint");
		info._attachPoint = pValue ? pValue : "";
		pValue = pSubElement->Attribute("Filename");
		info._filename = pValue ? pValue : "";
		info._node = LoadNode( info._filename.c_str(), NULL, _load_priority );		// wangjian modified : 异步加载 优先级
		if (info._node)
		{
			info._node->setPrivate (1);
		}
		_attachInfoList.push_back(info);
	}

	AvatarInfo avatarInfo;
	_avatarInfoList.clear ();
	for (const TiXmlElement *pAvartarElement = pXmlElement->FirstChildElement("mat"); pAvartarElement; pAvartarElement = pAvartarElement->NextSiblingElement("mat"))
	{
		avatarInfo.meshId = -1;
		pAvartarElement->QueryIntAttribute("mesh", &avatarInfo.meshId);

		const char *fn = pAvartarElement->Attribute ("material");
		if (!fn)
		{
			continue;
		}
		avatarInfo.materialFileName = fn;
		_avatarInfoList.push_back (avatarInfo);
	}

	/*
	_altParams.clear ();
	for (TiXmlElement* pAltTexElement = pXmlElement->FirstChildElement("alt"); pAltTexElement; pAltTexElement = pAltTexElement->NextSiblingElement("alt"), ++n)
	{
		_altParams.push_back (ATOM_NEW(ATOM_ParameterTable, this->getStaticMesh(
			_altTextures.resize (_altTextures.size() + 1);
		const char *filename = pAltTexElement->Attribute("file");
		_altTextures.back().filename = filename ? filename : "";
		_altTextures.meshIndex = -1;
		pAltTexElement->QueryIntAttribute ("mesh", &_altTextures.meshIndex);
		const char *param = pAltTexElement->Attribute("
			_altTextures.push_back (filename ? filename : "");
	}
	*/


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// wangjian added
	// 如果有固定的特效绑定
	_fixedEffectList.clear();
	for (const TiXmlElement *pFXElement = pXmlElement->FirstChildElement("fx"); pFXElement; pFXElement = pFXElement->NextSiblingElement("fx"))
	{
		const char* bonename = pFXElement->Attribute("bone");
		if( !bonename )
		{
			continue;
		}
		const char * fx = pFXElement->Attribute ("effect");
		if (!fx)
		{
			continue;
		}

		int noRotate = 1;
		pFXElement->QueryIntAttribute("noRotate",&noRotate);

		ATOM_MAP<ATOM_STRING,sFixedEffectContainer>::iterator iter = _fixedEffectList.find(bonename);
		if( iter == _fixedEffectList.end() )
		{
			sFixedEffectContainer fixedEffects;

			sFixedEffect effect;
			effect._effect = fx;
			effect._noRotate = noRotate;

			fixedEffects._effects.push_back(effect);

			_fixedEffectList[bonename] = fixedEffects;
		}
		else
		{
			sFixedEffectContainer & fixedEffects = iter->second;

			sFixedEffect effect;
			effect._effect = fx;
			effect._noRotate = noRotate;

			fixedEffects._effects.push_back(effect);
		}
	}

	return ATOM_Geode::loadAttribute(pXmlElement);
}

// 写配置
bool ClientSimpleCharacter::writeAttribute(TiXmlElement *pXmlElement)
{
	ATOM_STACK_TRACE(ClientSimpleCharacter::writeAttribute);

	// 保存参考点
	for (ATOM_HASHMAP<ATOM_STRING, ATOM_Matrix4x4f>::iterator it = _referencePointMap.begin(); it!=_referencePointMap.end(); ++it)
	{
		TiXmlElement pointElement("point");

		pointElement.SetAttribute("Name", it->first.c_str());

		char szBuffer[256]={0};
		ATOM_Matrix4x4f & Matrix = it->second;
		sprintf (	szBuffer, "%.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f",
			Matrix.m00, Matrix.m01, Matrix.m02, Matrix.m03,
			Matrix.m10, Matrix.m11, Matrix.m12, Matrix.m13,
			Matrix.m20, Matrix.m21, Matrix.m22, Matrix.m23,
			Matrix.m30, Matrix.m31, Matrix.m32, Matrix.m33);
		pointElement.SetAttribute("Matrix", szBuffer);

		pXmlElement->InsertEndChild(pointElement);
	}

	// fov
	if( _bindFOV != 0.0f )
	{
		TiXmlElement fovElement("FOV");

		char szBuffer[256]={0};
		sprintf( szBuffer, "%f", _bindFOV );

		fovElement.SetAttribute("value", szBuffer);

		pXmlElement->InsertEndChild(fovElement);
	}

	// light color
	if( _bindLightColor != ATOM_ColorARGB(0xFFFFFFFF) )
	{
		TiXmlElement lightColorElement("lightcolor");

		lightColorElement.SetAttribute("value", _bindLightColor);

		pXmlElement->InsertEndChild(lightColorElement);
	}

	// light dir
	if( _bindLightDir != ATOM_Vector3f(1,0,0) )
	{
		TiXmlElement lightDirElement("lightdir");

		char szBuffer[256]={0};
		sprintf(szBuffer, "%f, %f, %f", _bindLightDir.x, _bindLightDir.y, _bindLightDir.z);

		lightDirElement.SetAttribute("value", szBuffer);

		pXmlElement->InsertEndChild(lightDirElement);
	}
	// light intensity
	if( _bindLightIntensity != 1.0f )
	{
		TiXmlElement lightIntensityElement("lightintensity");

		char szBuffer[256]={0};
		sprintf( szBuffer, "%f", _bindLightIntensity );

		lightIntensityElement.SetAttribute("value", szBuffer);

		pXmlElement->InsertEndChild(lightIntensityElement);
	}

	// ambient color
	if( _bindAmbient != ATOM_ColorARGB(0) )
	{
		TiXmlElement ambientColorElement("ambient");

		ambientColorElement.SetAttribute("value", _bindAmbient);

		pXmlElement->InsertEndChild(ambientColorElement);
	}

	// attach info list
	int count = _attachInfoList.size();
	for(int i=0; i<count; ++i)
	{
		EquipInfo& info = _attachInfoList[i];

		TiXmlElement n("equip");
		if(!info._attachPoint.empty())
		{
			n.SetAttribute("SkeletonIndex", info._skeletonIndex);
			n.SetAttribute("AttachPoint", info._attachPoint.c_str());
		}
		n.SetAttribute("Filename", info._filename.c_str());

		pXmlElement->InsertEndChild(n);
	}

	// avatar info list
	for( int i = 0; i < _avatarInfoList.size(); ++i )
	{
		AvatarInfo & avatarInfo = _avatarInfoList[i];

		TiXmlElement avatarElement("mat");

		avatarElement.SetAttribute("mesh", avatarInfo.meshId);
		avatarElement.SetAttribute("material", avatarInfo.materialFileName.c_str());

		pXmlElement->InsertEndChild(avatarElement);
	}

	return ATOM_Geode::writeAttribute(pXmlElement);
}

void ClientSimpleCharacter::accept (ATOM_Visitor &visitor)
{
	ATOM_STACK_TRACE(ClientSimpleCharacter::accept);
	visitor.visit (*this);

	if (dynamic_cast<ATOM_XmlSaveVisitor*>(&visitor))
	{
		visitor.skipChildren ();
	}
}

//================================================================================================//
// wangjian modified

#if !USE_ASYNC_ATTRIBUTEFILE_IO

// 创建
bool ClientSimpleCharacter::onLoad(ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ClientSimpleCharacter::onLoad);

	if (!_filenameToBeLoad.empty ())
	{
		const char *fileName = _filenameToBeLoad.c_str();
		ATOM_AutoFile f(fileName, ATOM_VFS::read|ATOM_VFS::text);
		if (!f)
		{
			ATOM_LOGGER::error ("<%s> Open file (%s) failed\n", __FUNCTION__, fileName);
			return false;
		}
		unsigned size = f->size ();
		char *p = ATOM_NEW_ARRAY(char, size);
		if (!p)
		{
			ATOM_LOGGER::fatal ("<%s> Out of memory\n", __FUNCTION__);
			return false;
		}

		size = f->read (p, size);
		p[size] = '\0';
		ATOM_TiXmlDocument doc;
		doc.Parse (p);
		ATOM_DELETE_ARRAY(p);
		if (doc.Error ())
		{
			ATOM_LOGGER::error ("<%s> parse xml file (%s) failed: %s\n", __FUNCTION__, fileName, doc.ErrorDesc());
			return false;
		}

		int load_pri = _load_priority;

		TiXmlElement *element = doc.RootElement();
		if (!loadAttribute (element))
		{
			ATOM_LOGGER::error ("<%s> load attribute failed (%s)\n", __FUNCTION__, fileName);
			return false;
		}

		_load_priority = load_pri;
	}

	bool state = ATOM_Geode::onLoad(device);

	//---------- wangjian modified ----------//
	// 如果是直接加载 直接绑定 和 赋予材质
	bool bMT = ( ATOM_AsyncLoader::IsRun() );// && (_load_priority != ATOM_LoadPriority_IMMEDIATE);
	if( !bMT || _load_flag == LOAD_ALLFINISHED )
	{
		loadCompnent();

		/*
		_altParams.clear ();
		for (TiXmlElement* pAltTexElement = pXmlElement->FirstChildElement("alt"); pAltTexElement; pAltTexElement = pAltTexElement->NextSiblingElement("alt"), ++n)
		{
			_altParams.push_back (ATOM_NEW(ATOM_ParameterTable, this->getStaticMesh(
				_altTextures.resize (_altTextures.size() + 1);
			const char *filename = pAltTexElement->Attribute("file");
			_altTextures.back().filename = filename ? filename : "";
			_altTextures.meshIndex = -1;
			pAltTexElement->QueryIntAttribute ("mesh", &_altTextures.meshIndex);
			const char *param = pAltTexElement->Attribute("
				_altTextures.push_back (filename ? filename : "");
		}
		*/

		for (int i = 0; i < _avatarInfoList.size(); ++i)
		{
			int mesh = _avatarInfoList[i].meshId;
			if (mesh < 0 || mesh >= getNumMeshes())
			{
				continue;
			}

			ATOM_AUTOPTR(ATOM_Material) m = ATOM_NEW(ATOM_Material);
			if ( m && m->loadXML( device, _avatarInfoList[i].materialFileName.c_str(), _load_priority ) )
			{
				//----- 设置材质的全局ID ----//
				m->setMaterialId(_avatarInfoList[i].materialFileName);
				//--------------------------//

				getStaticMesh(mesh)->setMaterial (m.get());
			}
		}
	}
	else
	{
		for (int i = 0; i < _avatarInfoList.size(); ++i)
		{
			int mesh = _avatarInfoList[i].meshId;
			if (mesh < 0 /*|| mesh >= getNumMeshes()*/)
			{
				continue;
			}

			ATOM_AUTOPTR(ATOM_Material) m = ATOM_NEW(ATOM_Material);
			if (m && m->loadXML( device, _avatarInfoList[i].materialFileName.c_str(), _load_priority))
			{
				//----- 设置材质的全局ID ----//
				m->setMaterialId(_avatarInfoList[i].materialFileName);
				//---------------------------//

				sMaterialDependInfo mdinfo;
				mdinfo._meshid = mesh;
				mdinfo._material_dependent = m;
				_materialDependInfoList.push_back(mdinfo);
			}
		}
	}
	//--------------------------------------//

	return state;
}

#else


bool ClientSimpleCharacter::onLoad(ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ClientSimpleCharacter::onLoad);

	if ( !_filenameToBeLoad.empty () )
	{
		// 是否开启了异步加载模式
		bool bMT = ( ATOM_AsyncLoader::IsRun() );// && (_load_priority != ATOM_LoadPriority_IMMEDIATE);

		//==============================================================
		// 如果直接加载
		//==============================================================
		if( !bMT )
		{
			bool createNew = false;
			_attribFile = ATOM_LookupOrCreateObject (ATOM_NodeAttributeFile::_classname(), _filenameToBeLoad.c_str(), &createNew);
			return loadSync();
		}
		//==============================================================
		// 如果异步加载 
		//==============================================================
		else
		{
			//---------------------------------------------
			// 查找该节点属性文件对象
			//---------------------------------------------
			bool createNew = false;
			_attribFile = ATOM_LookupOrCreateObject (ATOM_NodeAttributeFile::_classname(), _filenameToBeLoad.c_str(), &createNew);

			//---------------------------------------------
			// 如果该节点属性文件对象还不存在 且 
			// 优先级为ATOM_LoadPriority_IMMEDIATE
			//---------------------------------------------
			if( createNew && _load_priority == ATOM_LoadPriority_IMMEDIATE )
			{
				// 直接加载
				return loadSync();
			}
			//---------------------------------------------
			// 如果该节点属性文件对象不存在 或 
			// 该模型资源尚未加载完成
			// 使用异步加载
			//---------------------------------------------
			else if( createNew || ( !_attribFile->getAsyncLoader()->IsLoadAllFinished() ) )
			{
				// 如果节点属性文件对象存在 且 优先级为ATOM_LoadPriority_IMMEDIATE 则 使用该节点属性文件对象的加载对象的优先级
				// 否则 使用此节点的优先级
				int loadPrioriy = ( false == createNew && _load_priority == ATOM_LoadPriority_IMMEDIATE ) ?	_attribFile->getAsyncLoader()->priority : 
					_load_priority;

				// 添加资源请求
				ATOM_LOADRequest_Event* event_load( ATOM_NEW(	ATOM_LOADRequest_Event,
					_filenameToBeLoad.c_str(),
					this, 
					loadPrioriy,
					_attribFile ? _attribFile->getAsyncLoader() : 0,
					ATOM_LOADRequest_Event::/*FLAG_SHOW*/FLAG_NOT_SHOW ) );

				if( ATOM_AsyncLoader::isEnableLog() )
				{
					ATOM_LOGGER::debug( "<%s>: submit load request for attribute file %s of Composition node [ Address : %x, priority %d ]\n",
						__FUNCTION__,
						_filenameToBeLoad.c_str(),
						_attribFile ? (unsigned)(_attribFile->getAsyncLoader()) : 0,
						loadPrioriy );
				}

				// 如果是新创建的对象 异步加载之
				if( createNew )
				{
					return _attribFile->loadAsync(_filenameToBeLoad.c_str(),loadPrioriy);
				}
				// 否则 如果该对象尚未加载完成且是被放弃的对象 重新处理该对象
				else
				{
					if( _attribFile->getAsyncLoader()->abandoned )
						ATOM_AsyncLoader::ReProcessLoadObject( _attribFile->getAsyncLoader() );
				}

				return true;
			}
			//---------------------------------------------
			// 如果该属性文件对象存在 且 已经加载完成
			//---------------------------------------------
			else
			{
				// 更改加载标记为属性文件加载完成
				_load_flag = LOAD_ATTRIBUTE_LOADED;

				ATOM_ASSERT(_attribFile.get());
				const ATOM_TiXmlElement * root = _attribFile->getDocument().RootElement();
				if (!root)
				{
					return false;
				}

				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 如果当前是否有变换，保存当前的变换[因为loadAttribute方法可能会重设变换]
				ATOM_STRING filenameTobeload = "";
				if( false == _filenameToBeLoad.empty() )
					filenameTobeload = _filenameToBeLoad;
				bool bHasTransform = false;
				ATOM_Transformf temp_transform;
				if( !_transform.getO2T().almostEqual( ATOM_Matrix4x4f::getIdentityMatrix() ) )
				{
					temp_transform = _transform;
					bHasTransform = true;
				}
				bool bPickable = getPickable2();// 先保存Pickable
				int pri_saved = _load_priority;	// 先保存加载优先级因为loadAttribute会改变_load_priority的值
				//////////////////////////////////////////////////////////////////////////////////////////////////////

				if (!loadAttribute (root))
				{
					return false;
				}

				//////////////////////////////////////////////////////////////////////////////////////////////////////
				_load_priority = pri_saved;		// 恢复加载优先级
				setPickable(bPickable);
				// 如果有变换，则设置回原来的变换
				if( bHasTransform )
				{
					setO2T(temp_transform.getO2T());
				}
				if( _filenameToBeLoad.empty() && false == filenameTobeload.empty() )
					_filenameToBeLoad = filenameTobeload;
				//////////////////////////////////////////////////////////////////////////////////////////////////////
			}
		}
	}

	// 
	if( ATOM_Geode::onLoad(device) )
	{
		loadCompnent();

		{
			// 如果有avatar信息
			int avatarCountProcessed = 0;
			for (int i = 0; i < _avatarInfoList.size(); ++i)
			{
				int mesh = _avatarInfoList[i].meshId;
				if (mesh < 0 || mesh >= getNumMeshes())
				{
					continue;
				}

				ATOM_AUTOPTR(ATOM_Material) m = ATOM_NEW(ATOM_Material);
				if ( m && m->loadXML( device, _avatarInfoList[i].materialFileName.c_str(), _load_priority ) )
				{
					//----- 设置材质的全局ID ----//
					m->setMaterialId(_avatarInfoList[i].materialFileName);
					//--------------------------//

					getStaticMesh(mesh)->setMaterial (m.get());
				}
				avatarCountProcessed++;
			}

			if( avatarCountProcessed != _avatarInfoList.size() )
			{
				for (int i = 0; i < _avatarInfoList.size(); ++i)
				{
					int mesh = _avatarInfoList[i].meshId;
					if (mesh < 0 /*|| mesh >= getNumMeshes()*/)
					{
						continue;
					}

					ATOM_AUTOPTR(ATOM_Material) m = ATOM_NEW(ATOM_Material);
					if (m && m->loadXML( device, _avatarInfoList[i].materialFileName.c_str(), _load_priority))
					{
						//----- 设置材质的全局ID ----//
						m->setMaterialId(_avatarInfoList[i].materialFileName);
						//---------------------------//

						sMaterialDependInfo mdinfo;
						mdinfo._meshid = mesh;
						mdinfo._material_dependent = m;
						_materialDependInfoList.push_back(mdinfo);
					}
				}
			}

			// 处理固定特效		
			processFixedEffects();
		}

		return true;
	}

	return false;
}

#endif

bool ClientSimpleCharacter::onMtLoad (ATOM_RenderDevice *device, unsigned flags, ATOM_LoadingRequestCallback callback, void *userData, long group)
{
	ATOM_STACK_TRACE(ClientSimpleCharacter::onMtLoad);
	if (!_filenameToBeLoad.empty ())
	{
		ATOM_AutoFile f(_filenameToBeLoad.c_str(), ATOM_VFS::read|ATOM_VFS::text);
		if (!f)
		{
			return false;
		}
		unsigned size = f->size ();
		char *p = ATOM_NEW_ARRAY(char, size);
		size = f->read (p, size);
		p[size] = '\0';
		ATOM_TiXmlDocument doc;
		doc.Parse (p);
		ATOM_DELETE_ARRAY(p);
		if (doc.Error ())
		{
			ATOM_LOGGER::error ("%s(0x%08X) %s\n", __FUNCTION__, device, doc.ErrorDesc());
			return false;
		}

		TiXmlElement *element = doc.RootElement();
		if (!loadAttribute (element))
		{
			return false;
		}
	}

	bool state = ATOM_Geode::onMtLoad(device, flags, callback, userData, group);

	loadCompnent();

	return state;
}

bool ClientSimpleCharacter::removeChild (ATOM_Node *node)
{
	ATOM_AUTOREF(ATOM_Node) ref = node;

	for(ATOM_VECTOR<EquipInfo>::iterator iter=_attachInfoList.begin(); iter != _attachInfoList.end(); ++iter)
	{
		EquipInfo& info = *iter;
		if(info._node.get() == node)
		{
			_attachInfoList.erase (iter);
			break;
		}
	}

	return ATOM_Geode::removeChild (node);
}

// 绑定部件
ATOM_Node* ClientSimpleCharacter::attach(const char *attachPoint, const char *filename)
{
	ATOM_STACK_TRACE(ClientSimpleCharacter::attach);
	ATOM_AUTOREF(ATOM_Node) node = LoadNode(filename, NULL,_load_priority); // wangjian modified : 异步加载 优先级
	if(NULL == node.get())
	{
		return NULL;
	}

	if(NULL == attachPoint || '\0' == *attachPoint)
	{
		appendChild(node.get());
	}
	else
	{
		ATOM_Geode::attach(attachPoint, node.get());
	}

	EquipInfo info;
	info._skeletonIndex = 0;
	info._attachPoint = attachPoint ? attachPoint : "";
	info._filename = filename ? filename : "";
	info._node = node;
	_attachInfoList.push_back(info);
	return node.get();
}

void ClientSimpleCharacter::detach (ATOM_Node *node)
{
	ATOM_STACK_TRACE(ClientSimpleCharacter::detach);
	for(ATOM_VECTOR<EquipInfo>::iterator iter=_attachInfoList.begin(); iter != _attachInfoList.end(); ++iter)
	{
		EquipInfo& info = *iter;
		if(info._node.get() == node)
		{
			_attachInfoList.erase(iter);
			break;
		}
	}
	ATOM_Geode::detach (node);
}

int ClientSimpleCharacter::getEquipCount() const
{
	ATOM_STACK_TRACE(ClientSimpleCharacter::getEquipCount);
	return _attachInfoList.size();
}

const char* ClientSimpleCharacter::getEquipFilename(int index) const
{
	ATOM_STACK_TRACE(ClientSimpleCharacter::getEquipFilename);
	if(index <0 || index >= _attachInfoList.size())
	{
		return NULL;
	}

	return _attachInfoList[index]._filename.c_str();
}

const char* ClientSimpleCharacter::getEquipAttachPoint(int index) const
{
	ATOM_STACK_TRACE(ClientSimpleCharacter::getEquipAttachPoint);
	if(index <0 || index >= _attachInfoList.size())
	{
		return NULL;
	}

	return _attachInfoList[index]._attachPoint.c_str() ;
}

ATOM_Node* ClientSimpleCharacter::getEquipNode(int index)
{
	ATOM_STACK_TRACE(ClientSimpleCharacter::getEquipNode);
	if(index <0 || index >= _attachInfoList.size())
	{
		return NULL;
	}

	return _attachInfoList[index]._node.get();

}

void ClientSimpleCharacter::setAction(const ATOM_STRING& name)
{
	ATOM_STACK_TRACE(ClientSimpleCharacter::setAction);
	_actionName = name;

	doAction (name.empty() ? 0 : name.c_str(), ATOM_Geode::ACTIONFLAGS_DOWNSIDE|ATOM_Geode::ACTIONFLAGS_UPSIDE, 0, true);
}

const ATOM_STRING& ClientSimpleCharacter::getAction() const
{
	ATOM_STACK_TRACE(ClientSimpleCharacter::getAction);
	return _actionName;
}

bool ClientSimpleCharacter::queryReferencePoint (const char *name, ATOM_Matrix4x4f *matrix) const
{
	if (!name)
	{
		return false;
	}

	ATOM_HASHMAP<ATOM_STRING, ATOM_Matrix4x4f>::const_iterator it = _referencePointMap.find (name);
	if (it == _referencePointMap.end ())
	{
		return ATOM_Geode::queryReferencePoint (name, matrix);
	}

	if (matrix)
	{
		*matrix = it->second;
	}

	return true;
}

void ClientSimpleCharacter::getReferencePointList (ATOM_VECTOR<ATOM_STRING> &pointlist) const
{
	ATOM_Geode::getReferencePointList (pointlist);

	for (ATOM_HASHMAP<ATOM_STRING, ATOM_Matrix4x4f>::const_iterator it = _referencePointMap.begin(); it!=_referencePointMap.end(); ++it)
	{
		if (std::find(pointlist.begin(), pointlist.end(), it->first) == pointlist.end())
		{
			pointlist.push_back (it->first);
		}
	}
}

void ClientSimpleCharacter::loadCompnent()
{
	ATOM_STACK_TRACE(ClientSimpleCharacter::loadCompnent);
	int count = _attachInfoList.size();
	for(int i=0; i<count; ++i)
	{
		EquipInfo& info = _attachInfoList[i];
		if(NULL == info._node.get())
		{
			continue;
		}

		ATOM_CompositionNode *comp = dynamic_cast<ATOM_CompositionNode*>(info._node.get());
		if (comp)
		{
			comp->play ();
		}

		if(info._attachPoint.empty())
		{
			appendChild(info._node.get());
		}
		else
		{
			ATOM_Geode::attach(info._attachPoint.c_str(), info._node.get());
		}
	}

	if(!_actionName.empty())
	{
		setAction(_actionName.c_str());
	}
}

void ClientSimpleCharacter::assign (ATOM_Node *other) const
{
	ATOM_STACK_TRACE(ClientSimpleCharacter::assign);
	ATOM_Geode::assign (other);

	ClientSimpleCharacter *node = dynamic_cast<ClientSimpleCharacter*>(other);

	if (node)
	{
		node->_attachInfoList = _attachInfoList;

		for (unsigned i = 0; i < _attachInfoList.size(); ++i)
		{
			node->_attachInfoList[i]._node = LoadNode(node->_attachInfoList[i]._filename.c_str(), NULL,_load_priority); // wangjian modified : 异步加载 优先级
		}
	}

	node->getLoadInterface()->setLoadingState (ATOM_LoadInterface::LS_NOTLOADED);
}

const ATOM_BBox& ClientSimpleCharacter::getBoundingbox(void) const
{
	ATOM_STACK_TRACE(ClientSimpleCharacter::getBoundingbox);
	ATOM_BBox bbox = ATOM_Geode::getBoundingbox ();

#if 0
	if (getNumChildren() > 0)
	{
		for (unsigned i = 0; i < getNumChildren(); ++i)
		{
			ATOM_BBox childBBox = getChild (i)->getBoundingbox ();
			childBBox.transform (getChild(i)->getO2T());
			bbox.extend (childBBox.getMin());
			bbox.extend (childBBox.getMax());
		}
	}
#endif

	_boundingBox = bbox;
	return _boundingBox;
}

const ATOM_BBox& ClientSimpleCharacter::getWorldBoundingbox (void) const
{
	ATOM_STACK_TRACE(ClientSimpleCharacter::getWorldBoundingbox);
	ATOM_BBox bbox = ATOM_Geode::getWorldBoundingbox ();
#if 0
	if (getNumChildren() > 0)
	{
		for (unsigned i = 0; i < getNumChildren(); ++i)
		{
			const ATOM_BBox &Childbbox = getChild (i)->getWorldBoundingbox ();
			bbox.extend (Childbbox.getMin());
			bbox.extend (Childbbox.getMax());
		}
	}
#endif

	_boundingBoxW = bbox;
	return _boundingBoxW;
}

bool ClientSimpleCharacter::rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const
{
	return ATOM_Geode::rayIntersectionTest (camera, ray, len);
}

ATOM_ColorARGB ClientSimpleCharacter::getBindLightColor (void) const
{
	return _bindLightColor;
}

const ATOM_Vector3f &ClientSimpleCharacter::getBindLightDir (void) const
{
	return _bindLightDir;
}

ATOM_ColorARGB ClientSimpleCharacter::getBindAmbientColor (void) const
{
	return _bindAmbient;
}

float ClientSimpleCharacter::getBindFOV (void) const
{
	return _bindFOV;
}

float ClientSimpleCharacter::getBindLightIntensity (void) const
{
	return _bindLightIntensity;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//--- wangjian added ---//
// 异步加载完成

#if !USE_ASYNC_ATTRIBUTEFILE_IO

void ClientSimpleCharacter::onLoadFinished()
{
	ATOM_Geode::onLoadFinished();

	loadCompnent();
	
	for (int i = 0; i < _materialDependInfoList.size(); ++i)
	{
		int mesh = _materialDependInfoList[i]._meshid;
		if (mesh < 0 || mesh >= getNumMeshes())
		{
			continue;
		}

		if( _materialDependInfoList[i]._material_dependent )
		{
			if( getStaticMesh(mesh) )
				getStaticMesh(mesh)->setMaterial (_materialDependInfoList[i]._material_dependent.get());
		}
	}

	// 处理固定特效		
	processFixedEffects();
}

bool ClientSimpleCharacter::loadSync()
{
	_load_flag = LOAD_ALLFINISHED;
	return true;
}

#else


void ClientSimpleCharacter::onLoadFinished()
{
	//--------------------------------------------------
	// 如果加载标记当前为LOAD_LOADSTARTED 
	// 那么设置完成标记为LOAD_ATTRIBUTE_LOADED
	//--------------------------------------------------
	if( _load_flag == LOAD_LOADSTARTED && !_filenameToBeLoad.empty () )
	{
		_load_flag = LOAD_ATTRIBUTE_LOADED;

		ATOM_ASSERT(_attribFile.get());

		const ATOM_TiXmlElement * root = _attribFile->getDocument().RootElement();
		if (!root)
		{
			return;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// 如果当前是否有变换，保存当前的变换[因为loadAttribute方法可能会重设变换]
		ATOM_STRING filenameTobeload = "";
		if( false == _filenameToBeLoad.empty() )
			filenameTobeload = _filenameToBeLoad;
		bool bHasTransform = false;
		ATOM_Transformf temp_transform;
		if( !_transform.getO2T().almostEqual( ATOM_Matrix4x4f::getIdentityMatrix() ) )
		{
			temp_transform = _transform;
			bHasTransform = true;
		}
		bool bPickable = getPickable2();// 先保存Pickable
		int pri_saved = _load_priority;	// 先保存加载优先级因为loadAttribute会改变_load_priority的值
		//////////////////////////////////////////////////////////////////////////////////////////////////////

		if (!loadAttribute (root))
		{
			return;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		_load_priority = pri_saved;		// 恢复加载优先级
		setPickable(bPickable);
		// 如果有变换，则设置回原来的变换
		if( bHasTransform )
		{
			setO2T(temp_transform.getO2T());
		}
		if( _filenameToBeLoad.empty() && false == filenameTobeload.empty() )
			_filenameToBeLoad = filenameTobeload;
		//////////////////////////////////////////////////////////////////////////////////////////////////////

		// 加载
		if( ATOM_Geode::onLoad( ATOM_GetRenderDevice() ) )
		{
			loadCompnent();

			// 如果有avatar信息
			int avatarCountProcessed = 0;
			for (int i = 0; i < _avatarInfoList.size(); ++i)
			{
				int mesh = _avatarInfoList[i].meshId;
				if (mesh < 0 || mesh >= getNumMeshes())
				{
					continue;
				}

				ATOM_AUTOPTR(ATOM_Material) m = ATOM_NEW(ATOM_Material);
				if ( m && m->loadXML( ATOM_GetRenderDevice(), _avatarInfoList[i].materialFileName.c_str(), _load_priority ) )
				{
					//----- 设置材质的全局ID ----//
					m->setMaterialId(_avatarInfoList[i].materialFileName);
					//--------------------------//

					getStaticMesh(mesh)->setMaterial (m.get());
				}
				avatarCountProcessed++;
			}

			if( avatarCountProcessed != _avatarInfoList.size() )
			{
				for (int i = 0; i < _avatarInfoList.size(); ++i)
				{
					int mesh = _avatarInfoList[i].meshId;
					if (mesh < 0)
					{
						continue;
					}

					ATOM_AUTOPTR(ATOM_Material) m = ATOM_NEW(ATOM_Material);
					if (m && m->loadXML( ATOM_GetRenderDevice(), _avatarInfoList[i].materialFileName.c_str(), _load_priority ))
					{
						//----- 设置材质的全局ID ----//
						m->setMaterialId(_avatarInfoList[i].materialFileName);
						//---------------------------//

						sMaterialDependInfo mdinfo;
						mdinfo._meshid = mesh;
						mdinfo._material_dependent = m;
						_materialDependInfoList.push_back(mdinfo);
					}
				}
			}
		}
	}
	//--------------------------------------------------
	// 其他模型动作资源加载完成
	//--------------------------------------------------
	else
	{
		ATOM_Geode::onLoadFinished();

		// 如果有换皮信息，设置对应网格的换皮材质
		for (int i = 0; i < _materialDependInfoList.size(); ++i)
		{
			int mesh = _materialDependInfoList[i]._meshid;
			if (mesh < 0 || mesh >= getNumMeshes())
			{
				continue;
			}

			if( _materialDependInfoList[i]._material_dependent )
			{
				if( getStaticMesh(mesh) )
					getStaticMesh(mesh)->setMaterial (_materialDependInfoList[i]._material_dependent.get());
			}
		}

		// 处理固定特效		
		processFixedEffects();
	}

}

// 直接加载接口
bool ClientSimpleCharacter::loadSync()
{
	// 如果属性文件不存在 退出
	if( !_attribFile )
		return false;

	// 如果属性文件加载失败 退出
	if( _attribFile->getAsyncLoader()->IsLoadFailed() )
		return false;

	// 如果属性文件还未加载完成
	if( !_attribFile->getAsyncLoader()->IsLoadAllFinished() )
	{
		// 直接读取属性文件
		if( false == _attribFile->loadSync(_filenameToBeLoad.c_str()) )
		{
			// 设置加载完成标记
			_load_flag = LOAD_ALLFINISHED;
			return false;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	ATOM_STRING filenameTobeload = "";
	if( false == _filenameToBeLoad.empty() )
		filenameTobeload = _filenameToBeLoad;
	int load_pri = _load_priority;
	TiXmlElement *element = _attribFile->getDocument().RootElement();
	if (!loadAttribute (element))
	{
		ATOM_LOGGER::error ("<%s> load attribute failed (%s)\n", __FUNCTION__, _filenameToBeLoad.c_str());
		// 设置加载完成标记
		_load_flag = LOAD_ALLFINISHED;
		return false;
	}
	_load_priority = load_pri;
	if( _filenameToBeLoad.empty() && false == filenameTobeload.empty() )
		_filenameToBeLoad = filenameTobeload;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool state = ATOM_Geode::onLoad( ATOM_GetRenderDevice() );
	if( !state )
	{
		ATOM_LOGGER::error("%s : load ATOM_Geode failed! \n",__FUNCTION__);
		// 设置加载完成标记
		_load_flag = LOAD_ALLFINISHED;
		return false;
	}

	loadCompnent();

	{
		// 如果有avatar信息
		int avatarCountProcessed = 0;
		for (int i = 0; i < _avatarInfoList.size(); ++i)
		{
			int mesh = _avatarInfoList[i].meshId;
			if (mesh < 0 || mesh >= getNumMeshes())
			{
				continue;
			}

			ATOM_AUTOPTR(ATOM_Material) m = ATOM_NEW(ATOM_Material);
			if ( m && m->loadXML( ATOM_GetRenderDevice(), _avatarInfoList[i].materialFileName.c_str(), _load_priority ) )
			{
				//----- 设置材质的全局ID ----//
				m->setMaterialId(_avatarInfoList[i].materialFileName);
				//--------------------------//

				getStaticMesh(mesh)->setMaterial (m.get());
			}
			avatarCountProcessed++;
		}

		if( avatarCountProcessed != _avatarInfoList.size() )
		{
			for (int i = 0; i < _avatarInfoList.size(); ++i)
			{
				int mesh = _avatarInfoList[i].meshId;
				if (mesh < 0)
				{
					continue;
				}

				ATOM_AUTOPTR(ATOM_Material) m = ATOM_NEW(ATOM_Material);
				if (m && m->loadXML( ATOM_GetRenderDevice(), _avatarInfoList[i].materialFileName.c_str(), _load_priority ))
				{
					//----- 设置材质的全局ID ----//
					m->setMaterialId(_avatarInfoList[i].materialFileName);
					//---------------------------//

					sMaterialDependInfo mdinfo;
					mdinfo._meshid = mesh;
					mdinfo._material_dependent = m;
					_materialDependInfoList.push_back(mdinfo);
				}
			}
		}

		processFixedEffects();
	}

	return true;
}

//----------------------//
void ClientSimpleCharacter::processFixedEffects()
{
	// 必须要有骨骼存在 才能得到绑定骨骼
	if( getSkeleton() )
	{
		ATOM_MAP<ATOM_STRING,sFixedEffectContainer>::iterator iter = _fixedEffectList.begin();
		for( ; iter != _fixedEffectList.end(); ++iter )
		{
			const char* bonename = iter->first.c_str();

			sFixedEffectContainer & effects = iter->second;
			for( int i = 0; i < effects._effects.size(); ++i )
			{

#if 0
				if(getBoneIndex(bonename) == -1)
				{
					DEBUG_MSG(_STR("<%s> : error(Miss bone:%s)",  __FUNCTION__, bonename));
				}
#endif

				ATOM_AUTOREF(ATOM_CompositionNode) pEffectNode = ATOM_Node::loadNodeFromFile ( effects._effects[i]._effect.c_str(), _load_priority );

				// 一个骨骼只能绑定一件装备
				//DetachEquipment(nSlotId);

				// 绑定
				if (pEffectNode.get())
				{
					pEffectNode->setO2T (ATOM_Matrix4x4f::getIdentityMatrix());
					pEffectNode->setTopMost(ATOM_Node::INHERIT);
					pEffectNode->setShow(ATOM_Node::INHERIT);
					pEffectNode->setAntiTopMost(ATOM_Node::INHERIT);

					// 绑定
					if( !ATOM_Geode::attach( bonename, pEffectNode.get(), effects._effects[i]._noRotate ?  &CSP_FIXEDEFFECT_NoRotate : 0 ) )
					{
						continue;
					}

					// 播放特效
					pEffectNode->play ();

					// 无法被点选
					pEffectNode->setPickable(false);
				}
			}
		}
	}
}

#endif
