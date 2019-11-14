#include "StdAfx.h"
#include "geode2.h"
#include "actionmixer2.h"
#include "parameterchannel.h"

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_Geode)
	ATOM_ATTRIBUTES_BEGIN(ATOM_Geode)
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Geode, "ModelFileName",	getModelFileName, setModelFileName, "", "group=ATOM_Geode;type=vfilename;desc='模型文件名'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Geode, "ModelFileNames", getModelFileNames, setModelFileNames, ATOM_VECTOR<ATOM_STRING>(), "group=ATOM_Geode;desc='模型文件列表'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Geode, "Transparency", getTransparency, setTransparency, 1.f, "group=ATOM_Geode;desc='透明度'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Geode, "ZTransparency", isZTransparencyEnabled, enableZTransparency, 0, "group=ATOM_Geode;desc='防透明穿帮'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_Geode, "ColorMultiplier", getColorMultiplier, setColorMultiplier, 1.f, "group=ATOM_Geode;desc='颜色强度因子'")	// wangjian added
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_Geode, ATOM_VisualNode)

ATOM_IMPLEMENT_NODE_FILE_TYPE(ATOM_Geode)

static const ATOM_VECTOR<ATOM_Matrix3x4f> *boneMatrices = 0;
static float modelAlpha = 1.f;
static ATOM_SharedMesh *currentMesh = 0;

static void boneMatrices_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	if (boneMatrices && !boneMatrices->empty())
	{
		const ATOM_Matrix3x4f *p = &boneMatrices->front();

		if (vsConstantPtr)
		{
			ATOM_Matrix3x4f *dest = (ATOM_Matrix3x4f*)vsConstantPtr;
			const int *jointMap = &currentMesh->getJointMap()[0];
			int num = currentMesh->getJointMap().size();
			param->dimension = num*3;
			//ATOM_Matrix3x4f *dest = (ATOM_Matrix3x4f*)param->getValuePtr ();
			for (unsigned i = 0; i < num; ++i)
			{
				//dest[i].makeIdentity ();
				dest[i] = p[jointMap[i]];
			}
		}
	}
}

static void modelAlpha_Callback(ATOM_EffectParameterValue *param, ATOM_Vector4f *vsConstantPtr, ATOM_Vector4f *psConstantPtr, const ATOM_Shader::ParamDesc *descVS, const ATOM_Shader::ParamDesc *descPS)
{
	switch (param->handle->getParamType())
	{
	case ATOM_MaterialParam::ParamType_Vector:
		{
			param->setVector (ATOM_Vector4f(1.f, 1.f, 1.f, modelAlpha));
			//((ATOM_MaterialVectorParam*)param)->setValue (ATOM_Vector4f(1.f, 1.f, 1.f, modelAlpha));
			if (vsConstantPtr)
			{
				vsConstantPtr->set (1.f, 1.f, 1.f, modelAlpha);
			}

			if (psConstantPtr)
			{
				psConstantPtr->set (1.f, 1.f, 1.f, modelAlpha);
			}
			break;
		}
	}
}

struct BoneMatricesParamRegister
{
	BoneMatricesParamRegister (void)
	{
		ATOM_ParameterChannel::registerChannel ("BoneMatrices", &boneMatrices_Callback, 0, 0, ATOM_MaterialParam::ParamType_VectorArray);
		ATOM_ParameterChannel::registerChannel ("ModelAlpha", &modelAlpha_Callback, 0, 0, ATOM_MaterialParam::ParamType_Vector);
	}
};

static BoneMatricesParamRegister autoBoneMatricesRegister;

ATOM_StaticMesh::ATOM_StaticMesh (ATOM_Geode *geode, ATOM_SharedMesh *sharedMesh)
{
	_geode = geode;
	_sharedMesh = sharedMesh;
	_visible = true;
	_trans = _sharedMesh->isAlphaBlendEnabled ();

#if 1
	_material = _sharedMesh->getMaterial();
#else
#if 0
	_material = ATOM_NEW(ATOM_Material);
	_material->setCoreMaterial (_sharedMesh->getMaterial()->getCoreMaterial());
#else
	_material = _sharedMesh->getMaterial()->clone ();
#endif
#endif

#if 1
	//--- wangjian added ---//
	// 如果不是蒙皮网格，使用INSTANCING
	if( sharedMesh->getGeometry()->getType() == GT_HWINSTANCING )
		_obj_flag |= ATOM_Drawable::OF_GEOINSTANCING;
	// 如果是透明材质 不允许实例化
	/*if( _trans )
		resetObjFlag(ATOM_Drawable::OF_GEOINSTANCING);*/
	//----------------------//
#endif
}

ATOM_StaticMesh::~ATOM_StaticMesh (void)
{
}

ATOM_SharedMesh *ATOM_StaticMesh::getSharedMesh (void) const
{
	return _sharedMesh;
}

void ATOM_StaticMesh::setVisible (bool visible)
{
	_visible = visible;
}

bool ATOM_StaticMesh::getVisible (void) const
{
	return _visible;
}

void ATOM_StaticMesh::setTransparency (float transparency)
{
	if (_material == _sharedMesh->getMaterial() && _material)
	{
		_material = _sharedMesh->getMaterial()->clone ();
	}

	if (_material)
	{
		if (transparency != 1.f)
		{
			_material->getParameterTable()->setInt ("enableAlphaBlend", 1);
			if (_geode->isZTransparencyEnabled())
			{
				_material->getParameterTable()->setInt ("enablePostZPass", 1);
			}

			// 如果是透明材质 不允许实例化
			//resetObjFlag(ATOM_Drawable::OF_GEOINSTANCING);
		}
		else
		{
			_material->getParameterTable()->setInt ("enableAlphaBlend", _trans ? 1 : 0);
			_material->getParameterTable()->setInt ("enablePostZPass", 0);

			// 如果是透明材质 不允许实例化
			//if( _trans )
			//	resetObjFlag(ATOM_Drawable::OF_GEOINSTANCING);
		}
		_material->getParameterTable()->setVector ("transparency", ATOM_Vector4f(1.f, 1.f, 1.f, transparency));
	}
}

void ATOM_StaticMesh::setMaterial (ATOM_Material *material)
{
	_material = material;
}

ATOM_Material *ATOM_StaticMesh::getMaterial (void) const
{
	return _material.get();
}

bool ATOM_StaticMesh::draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{

	bool ret = false;
	if (_sharedMesh)
	{
		if(_sharedMesh->getGeometry()->getType()==GT_HWINSTANCING)
		{
			ATOM_HWInstancingGeometry* hw_instancing_geo = (ATOM_HWInstancingGeometry*)(_sharedMesh->getGeometry());
			hw_instancing_geo->enableInstancing(false);
		}

		//--- wangjian added ---//
#if 1
		const ATOM_Matrix4x4f &worldMatrix = _geode->getWorldMatrix ();
		device->setTransform (ATOM_MATRIXMODE_WORLD, worldMatrix);
#endif
		//----------------------//

		currentMesh = _sharedMesh;

		boneMatrices = &_geode->getActionMixer()->getJointMatrices();
		modelAlpha = _geode->getTransparency();
		bool bSkinning = !currentMesh->getJointMap().empty();
		if( ATOM_RenderSettings::isEditorModeEnabled() )
			bSkinning &= ( boneMatrices && !boneMatrices->empty());

		material->getParameterTable()->setInt ("enableSkinning", bSkinning ? 1 : 0);
		material->getParameterTable()->setInt ("projectable", (_geode->getProjectable()==ATOM_Node::PROJECTABLE)?1:0);

		//--- wangjian added ---//
		//if( bSkinning && currentMesh->getModel()->getAnimationTexture() )
		//{
		//	material->getParameterTable()->setTexture("g_animData", currentMesh->getModel()->getAnimationTexture());
		//	material->getParameterTable()->setTexture("animTexture", currentMesh->getModel()->getAnimationTexture());
		//}
		//----------------------//

		//--- wangjian added ---//
		int effect_id = getEffectIdFront(ATOM_RenderScheme::getCurrentRenderScheme());
		if( effect_id >= 0 )
		{
			material->setActiveEffect(effect_id);
			material->getCoreMaterial()->setAutoSelectEffect(false);
		}
		//----------------------//

		ret = _sharedMesh->draw (device, material);

		currentMesh = 0;
		boneMatrices = 0;

		//--- wangjian added ---//
		if( effect_id >= 0 )
			material->getCoreMaterial()->setAutoSelectEffect(true);
		//----------------------//
	}
	return ret;
}

//--- wangjian added ---//
// 绘制实例化
bool ATOM_StaticMesh::drawBatch(ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material, 
								bool bInstancing/* = true*/, sInstancing * instancing/* = 0*/	)
{
	if( !instancing )
		return false;

	// no instances skip
	if( instancing->getIntanceCount() <= 0 )
		return false;

	// if skinned mesh ,skip
	if( _geode->getActionMixer() )
	{
		bool bSkinning = !_sharedMesh->getJointMap().empty();
		if( ATOM_RenderSettings::isEditorModeEnabled() )
		{
			boneMatrices = &_geode->getActionMixer()->getJointMatrices();
			bSkinning &= ( boneMatrices && !boneMatrices->empty());
		}

		if( bSkinning )
		{
			return false;
		}
	}

	bool ret = false;

	// 使用几何体实例化
	if( bInstancing )
	{
		// not HW Instancing Geometry skip
		if(_sharedMesh->getGeometry()->getType()!=GT_HWINSTANCING)
			return false;

		ATOM_HWInstancingGeometry* hw_instancing_geo = (ATOM_HWInstancingGeometry*)(_sharedMesh->getGeometry());

		// enable hw instancing
		hw_instancing_geo->enableInstancing(true);

		int instance_count = instancing->getIntanceCount();
		
		// 如果使用静态INSTANCINGBUFFER
		bool useStaticInstancingBuffer = ATOM_RenderSettings::isUseInstancingStaticBuffer();
		if( useStaticInstancingBuffer )
		{
			hw_instancing_geo->setInstanceDataAttributes(ATOM_VERTEX_ATTRIB_TEX4_1);
			hw_instancing_geo->setInstanceCount(instance_count);

			// set the instance data ( world matrix )
			material->getParameterTable()->setVectorArray("instanceDatas",(ATOM_Vector4f*)instancing->getInstancedata(),instance_count*3);
		}
		// 如果是动态BUFFER lock/unlock
		else
		{
			void * data = hw_instancing_geo->addInstance(instance_count);
			if( !data )
				return false;
			memcpy(data,instancing->getInstancedata(),sizeof(sInstancing::sInstanceAttribute)*instance_count);
		}

		//material->getParameterTable()->setInt ("enableSkinning", (boneMatrices && !boneMatrices->empty()) ? 1 : 0);
		//material->getParameterTable()->setInt ("projectable", (_geode->getProjectable()==ATOM_Node::PROJECTABLE)?1:0);

		//--- wangjian added ---//
		int effect_id = getEffectIdFront(ATOM_RenderScheme::getCurrentRenderScheme());
		if( effect_id >= 0 )
		{
			material->setActiveEffect(effect_id);
			material->getCoreMaterial()->setAutoSelectEffect(false);
		}
		//----------------------//

		device->setTransform (ATOM_MATRIXMODE_WORLD, ATOM_Matrix4x4f::getIdentityMatrix());

		ret = _sharedMesh->draw(device,material);

		//--- wangjian added ---//
		if( effect_id >= 0 )
			material->getCoreMaterial()->setAutoSelectEffect(true);
		//----------------------//

		// disable hw instancing
		hw_instancing_geo->enableInstancing(false);
	}
	//=================================================
	// 不使用几何体实例化 而是进行BATCH
	//=================================================
	else
	{
		ATOM_HWInstancingGeometry* hw_instancing_geo = (ATOM_HWInstancingGeometry*)(_sharedMesh->getGeometry());
		if( !hw_instancing_geo )
			return false;

		// disable hw instancing
		hw_instancing_geo->enableInstancing(false);

		int instance_count = instancing->getIntanceCount();

		//material->getParameterTable()->setInt ("enableSkinning", (boneMatrices && !boneMatrices->empty()) ? 1 : 0);
		//material->getParameterTable()->setInt ("projectable", (_geode->getProjectable()==ATOM_Node::PROJECTABLE)?1:0);

		//--- wangjian added ---//

		int effect_id = getEffectIdFront(ATOM_RenderScheme::getCurrentRenderScheme());
		if( effect_id >= 0 )
		{
			material->setActiveEffect(effect_id);
			material->getCoreMaterial()->setAutoSelectEffect(false);
		}
		//----------------------//

		device->setTransform (ATOM_MATRIXMODE_WORLD, ATOM_Matrix4x4f::getIdentityMatrix());

		ret = _sharedMesh->draw(device,material);

		//--- wangjian added ---//
		if( effect_id >= 0 )
			material->getCoreMaterial()->setAutoSelectEffect(true);
		//----------------------//
	}

	return ret;
}

void ATOM_StaticMesh::batching(	sInstancing * instanceWrapper, ATOM_Camera *camera	)
{
	if( instanceWrapper )
		instanceWrapper->appendInstance( getWorldTransform(camera) );
}

void ATOM_StaticMesh::prepareForSort(ATOM_RenderScheme* renderScheme)
{
	ATOM_Drawable::prepareForSort(renderScheme);

	bool bSkinning = !_sharedMesh->getJointMap().empty();
	if( ATOM_RenderSettings::isEditorModeEnabled() )
	{
		boneMatrices = &_geode->getActionMixer()->getJointMatrices();
		bSkinning &= ( boneMatrices && !boneMatrices->empty());
	}

	_material->getParameterTable()->setInt ("enableSkinning", bSkinning ? 1 : 0);
	_material->getParameterTable()->setInt ("projectable", (_geode->getProjectable()==ATOM_Node::PROJECTABLE)?1:0);

	//--- wangjian added ---//
	//_material->getParameterTable()->setInt ("enableAnimTexture", bSkinning && _sharedMesh->getModel()->getAnimationTexture() ? 1 : 0 );
	//----------------------//
}
// 设置绘制排序标记
void ATOM_StaticMesh::setupSortFlag(int queueid)
{
	unsigned sf_ac = 0;
	unsigned sf_mt = 0;
	unsigned sf_matid = 0;

#if 0
	ULONGLONG matFlag = (ULONGLONG)_material->getActiveEffect();
	ULONGLONG miscFlag = (ULONGLONG)_sharedMesh;
	ULONGLONG sf = ( matFlag << 32 ) + miscFlag;
#else
	//unsigned matFlag = (unsigned)_material->getActiveEffect();
	/*int effectid = getEffectIdEnd(ATOM_RenderScheme::getCurrentRenderScheme());
	ATOM_ASSERT(effectid!=-1);
	unsigned matFlag = (unsigned)_material->getCoreMaterial()->getEffect(effectid);
	matFlag <<= 16;
	matFlag /= 100000;
	matFlag *= 100000;
	unsigned miscFlag = (unsigned)_sharedMesh;
	unsigned sf = matFlag + ( miscFlag & 0x0000ffff );*/

	int effectid = getEffectIdEnd(ATOM_RenderScheme::getCurrentRenderScheme());
	ATOM_ASSERT(effectid!=-1);
	sf_ac = (unsigned)_material->getCoreMaterial()->getEffect(effectid);
	sf_mt = (unsigned)_sharedMesh;
	sf_matid = _material->getMaterialId();

#endif

	//setSortFlag(sf);
//#if 1
//	sf_matid = (unsigned)_material.get();
//#endif
	setSortFlag(sf_ac,sf_mt,sf_matid);
}
#if 1
ATOM_STRING ATOM_StaticMesh::getModelFileName()
{
	return _geode->getModelFileName();
}
#endif
// 重置材质参数表的脏标记
void ATOM_StaticMesh::resetMaterialDirtyFlag()
{
	if( _material )
	{
		_material->getParameterTable()->resetDirtyFlag();
	}
}
const ATOM_Matrix4x4f & ATOM_StaticMesh::getWorldTransform(ATOM_Camera *camera) const
{
	return _geode->getWorldMatrix ();
}

void ATOM_StaticMesh::setColorMultiplier (float colorMultiplier)
{
	if (_material)
	{
		_material->getParameterTable()->setFloat("colorMultiplier", colorMultiplier);
	}
}
//----------------------//

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_Geode::ATOM_Geode (void)
{
	_transparency = 1.f;
	_zTransparency = 0;
	
	_actionMixer = ATOM_NEW(ATOM_ActionMixer2, this);
	_pickBox = 0;
	_pickBoxDirty = false;

	_modelFileName = "";
	_modelFileNames.resize(0,"");

	_colorMultiplier		= 1.0f;							// wangjian added
}

ATOM_Geode::~ATOM_Geode (void)
{
	ATOM_DELETE(_actionMixer);
	ATOM_DELETE(_pickBox);

	//--- wangjian added ---//
	/*if( ATOM_AsyncLoader::IsRun() )
	{
		for (unsigned i = 0; i < _components.size(); ++i)
		{
			ATOM_SharedModel * model = _components[i].getModel ();
			if (model)
			{
				model->getAsyncLoader()->RemoveListener(this);
			}
		}
	}	*/
	_component_setFlags.clear();
	//----------------------//
}

void ATOM_Geode::accept (ATOM_Visitor &visitor)
{
	visitor.visit (*this);
}

bool ATOM_Geode::removeChild(ATOM_Node *node)
{
	ATOM_AUTOREF(ATOM_Node) ref = node;
	for (int i = 0; i < _attachments.size(); ++i)
	{
		if (_attachments[i].node == node)
		{
			_attachments.erase (_attachments.begin() + i);
			break;
		}
	}
	return ATOM_VisualNode::removeChild (node);
}

bool ATOM_Geode::rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const
{
	ATOM_STACK_TRACE(ATOM_ShapeNode::rayIntersectionTest);
	float t;

	if (_pickBoxDirty)
	{
		_pickBoxDirty = false;
		updatePickBox ();
	}

	const ATOM_BBox &box = _pickBox ? *_pickBox : getBoundingbox();
	bool ret = ray.intersectionTestEx (box, t);
	if (ret)
	{
		if (len)
		{
			*len = t;
		}
		return true;
	}
	return false;
}

bool ATOM_Geode::queryReferencePoint (const char *name, ATOM_Matrix4x4f *matrix) const
{
	if (name)
	{
		for (unsigned i = 0; i < _components.size(); ++i)
		{
			ATOM_SharedModel *model = _components[i].getModel ();
			if (model)
			{
				const ATOM_Variant &var = model->getAttributes()->getAttribute (name);
				if (var.getType() == ATOM_Variant::MATRIX44)
				{
					if (matrix)
					{
						memcpy (matrix, var.getM(), sizeof(ATOM_Matrix4x4f));
					}
					return true;
				}
			}
		}
	}
	return false;
}

void ATOM_Geode::getReferencePointList (ATOM_VECTOR<ATOM_STRING> &pointlist) const
{
	pointlist.resize (0);

	for (unsigned i = 0; i < _components.size(); ++i)
	{
		ATOM_SharedModel *model = _components[i].getModel ();
		if (model)
		{
			unsigned numAttrib = model->getAttributes()->getNumAttributes();
			for (unsigned i = 0; i < numAttrib; ++i)
			{
				const ATOM_Variant &var = model->getAttributes()->getAttributeValue (i);
				if (var.getType() == ATOM_Variant::MATRIX44)
				{
					pointlist.push_back (model->getAttributes()->getAttributeName (i));
				}
			}
		}
	}
}

bool ATOM_Geode::supportMTLoading (void)
{
	return false;
}

void ATOM_Geode::clear (void)
{
	removeAllComponents ();
	getLoadInterface()->setLoadingState (ATOM_LoadInterface::LS_NOTLOADED);
}

void ATOM_Geode::setupRenderQueue (ATOM_CullVisitor *visitor)
{
	for (unsigned i = 0; i < _meshes.size(); ++i)
	{
		ATOM_Camera *camera = visitor->getCamera ();
		
		ATOM_Material *material = _meshes[i]->getMaterial();
		if (!material)
		{
			material = _meshes[i]->getSharedMesh()->getMaterial();
		}

		//--- wangjian modified ---//
		// 将世界变换矩阵的设置放到draw中去
#if 0
		const ATOM_Matrix4x4f &worldMatrix = getWorldMatrix ();
		visitor->addDrawable (_meshes[i].get(), worldMatrix, material, -1, getProjectable()==ATOM_Node::PROJECTABLE);
#else
		const ATOM_Matrix4x4f &worldMatrix = getWorldMatrix ();
		visitor->addDrawable (_meshes[i].get(), worldMatrix, material, -1, getProjectable()==ATOM_Node::PROJECTABLE);
#endif
		//-------------------------//
	}
}

void ATOM_Geode::update (ATOM_Camera *camera)
{
	_actionMixer->updateActions ();
	updateAttachments ();
}

bool ATOM_Geode::onLoad(ATOM_RenderDevice *device)
{
	clear ();

	if (!getNodeFileName().empty ())
	{
		//----------------------------------------------//
		// wangjian modified
		// _modelFileName 必须是NM2或者NM格式
		_modelFileName = getNodeFileName ();
		//----------------------------------------------//
	}

	bool ret = true;

	if( ATOM_AsyncLoader::isEnableLog() )
	{
		if( !_modelFileName.empty() && _load_priority == ATOM_LoadPriority_IMMEDIATE )
			ATOM_LOGGER::log("======= %s : the load priority is immediate =========\n", _modelFileName.c_str() );
	}

	//--------------------------------------------------------------------------------------------//
	// 如果没有模型文件名 则无需加载
	/*if( _modelFileName.empty() && _modelFileNames.empty() )
	{
		ATOM_LOGGER::log("<%s> : ======= has no model file names =========\n", __FUNCTION__ );
		return false;
	}*/
	//--------------------------------------------------------------------------------------------//

	//--------------------------------------------------------------------------------------------//
	// 是否有模型
	bool bHasModel = false;
	//--------------------------------------------------------------------------------------------//

	//--- wangjian added ---//
	// 加载优先级
	int loadPrioriy = _load_priority;

	// 异步加载
	bool bMT = ATOM_AsyncLoader::IsRun();

	// 如果文件名不为空 且 当前开启了资源异步加载模式
	if( !_modelFileName.empty() && bMT )
	{
		char buffer[ATOM_VFS::max_filename_length];
		if (ATOM_CompletePath (_modelFileName.c_str(), buffer))
		{
			_strlwr (buffer);

			// 查找该模型
			ATOM_AUTOREF(ATOM_SharedModel) model = ATOM_LookupObject (ATOM_SharedModel::_classname(), buffer);

			// 如果该模型还不存在 且 优先级为ATOM_LoadPriority_IMMEDIATE
			if( !model && _load_priority == ATOM_LoadPriority_IMMEDIATE )
			{
				// 直接加载 设置加载完成标记
				_load_flag = LOAD_ALLFINISHED;
			}
			// 如果该模型资源不存在 或 该模型资源尚未加载完成
			else if( !model || ( !model->getAsyncLoader()->IsLoadAllFinished() ) )
			{
				// 如果模型存在 且 优先级为ATOM_LoadPriority_IMMEDIATE 则 使用该模型的加载对象的优先级
				// 否则 使用此节点的优先级
				loadPrioriy = ( model && _load_priority == ATOM_LoadPriority_IMMEDIATE ) ?	model->getAsyncLoader()->priority : 
																							_load_priority;

				// 添加资源请求
				ATOM_LOADRequest_Event* event_load( ATOM_NEW(	ATOM_LOADRequest_Event,
																buffer, 
																this, 
																loadPrioriy,
																model ? model->getAsyncLoader() : 0,
																ATOM_LOADRequest_Event::FLAG_SHOW ) );
			}
			// 模型已经加载 设置加载完成标记
			else
			{
				_load_flag = LOAD_ALLFINISHED;
			}

			bHasModel = true;
		}	
	}		
	//----------------------//

	//--- wangjian modified ---//
	// 异步加载
	// 如果加载优先级别是ATOM_LoadPriority_IMMEDIATE，则立即加载 不使用异步加载
	if ( !_modelFileName.empty() && ! addComponents ( ATOM_Components( _modelFileName.c_str(), loadPrioriy ), loadPrioriy ) )
	{
		ret = false;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////

	for (unsigned i = 0; i < _modelFileNames.size(); ++i)
	{
		loadPrioriy = _load_priority;

		if( ATOM_AsyncLoader::isEnableLog() )
		{
			if( !_modelFileNames[i].empty() && _load_priority == ATOM_LoadPriority_IMMEDIATE )
				ATOM_LOGGER::log("======= %s : the load priority is immediate =========\n", _modelFileNames[i].c_str() );
		}

		if( !_modelFileNames[i].empty() )
		{
			//--- wangjian added ---//
			// 异步加载
			if( bMT )
			{
				char buffer[ATOM_VFS::max_filename_length];
				if (ATOM_CompletePath (_modelFileNames[i].c_str(), buffer))
				{
					_strlwr (buffer);

					// 查找该模型
					ATOM_AUTOREF(ATOM_SharedModel) model = ATOM_LookupObject (ATOM_SharedModel::_classname(), buffer);

					// 如果该模型还不存在 且 优先级为ATOM_LoadPriority_IMMEDIATE
					if( !model && _load_priority == ATOM_LoadPriority_IMMEDIATE )
					{
						// 直接加载 设置加载完成标记
						_load_flag = LOAD_ALLFINISHED;
					}
					// 如果该模型资源不存在 或 该模型资源尚未加载完成
					else if( !model || ( !model->getAsyncLoader()->IsLoadAllFinished() ) )
					{
						// 如果模型存在 且 优先级为ATOM_LoadPriority_IMMEDIATE 则 使用该模型的加载对象的优先级
						// 否则 使用此节点的优先级
						loadPrioriy = ( model && _load_priority == ATOM_LoadPriority_IMMEDIATE ) ?	model->getAsyncLoader()->priority : 
																									_load_priority;

						// 添加资源请求
						ATOM_LOADRequest_Event* event_load( ATOM_NEW(	ATOM_LOADRequest_Event, 
																		buffer, 
																		this, 
																		loadPrioriy,
																		model ? model->getAsyncLoader() : 0,
																		ATOM_LOADRequest_Event::FLAG_SHOW ) );
					}
				}
			}
			//----------------------//

			//--- wangjian modified ---//
			// 异步加载
			// 如果加载优先级别是ATOM_LoadPriority_IMMEDIATE，则立即加载 不使用异步加载
			addComponents ( ATOM_Components( _modelFileNames[i].c_str(), loadPrioriy ), loadPrioriy );
			//-------------------------//

			bHasModel = true;
		}
		else
		{
			//_load_flag = LOAD_ALLFINISHED;
		}
	}

	//--- wangjian added ---//
	// 如果是非异步加载 或者 模型文件名没有 则加载完成
	if( !bMT/* || !bHasModel*/ )
		_load_flag = LOAD_ALLFINISHED;
	//----------------------//

	_pickBoxDirty = true;

	return ret;
}

void ATOM_Geode::updatePickBox (void) const
{
	ATOM_Matrix4x4f m0, m1;
	if (queryReferencePoint ("box_1", &m0) && queryReferencePoint ("box_2", &m1))
	{
		if (!_pickBox)
		{
			_pickBox = ATOM_NEW(ATOM_BBox);
		}

		_pickBox->beginExtend ();
		_pickBox->extend (m0.getRow3(3));
		_pickBox->extend (m1.getRow3(3));
	}
	else
	{
		ATOM_DELETE(_pickBox);
		_pickBox = 0;
	}
}

bool ATOM_Geode::loadModel (ATOM_RenderDevice *device, const char *filename)
{
	ATOM_AUTOREF(ATOM_SharedModel) model = ATOM_LookupObject (ATOM_SharedModel::_classname(), filename);
	if (!model)
	{
		model = ATOM_CreateObject (ATOM_SharedModel::_classname(), filename);
		const char *ext = strrchr (filename, '.');
		bool loaded = false;
		if (!stricmp (ext, ".nm"))
			loaded = model->load_nm (device, filename);
		else if (!stricmp (ext, ".nm2"))
			loaded = model->load (device, filename);
			
		if (!loaded)
		{
			model = 0;
			return false;
		}
	}

	addSharedModel (model.get(), ATOM_LoadPriority_IMMEDIATE);	// wangjia modified

	invalidateBoundingbox ();

	return true;
}

bool ATOM_Geode::loadXML (ATOM_RenderDevice *device, const char *filename)
{
	ATOM_AutoFile f(filename, ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
	{
		ATOM_LOGGER::error ("%s(0x%08X,%s): Open file failed.\n", __FUNCTION__, device, filename);
		return false;
	}
	ATOM_VECTOR<char> file_content(f->size()+1);
	char *str = &file_content[0];
	unsigned n = f->read (str, f->size());
	str[n] = '\0';

	ATOM_TiXmlDocument doc;
	doc.Parse (str);
	if (doc.Error())
	{
		ATOM_LOGGER::error ("%s: Parse XML failed.\n", __FUNCTION__);
		return false;
	}

	ATOM_TiXmlElement *root = doc.RootElement();
	ATOM_TiXmlElement *modelElement = root->FirstChildElement ("Model");
	while (modelElement)
	{
		const char *filename = modelElement->Attribute ("FileName");
		if (filename)
		{
			if (!loadModel (device, filename))
			{
				return false;
			}
		}
		modelElement = modelElement->NextSiblingElement ("Model");
	}

	const char *action = root->Attribute ("Action");
	if (action)
	{
		doAction (action, ACTIONFLAGS_UPSIDE|ACTIONFLAGS_DOWNSIDE);
	}

	return true;
}

void ATOM_Geode::buildBoundingbox (void) const
{
	if (_meshes.empty ())
	{
		//--- wangjian modified ---//
		// 将包围盒设置得很大 以便在update时不被剔除掉
		// NOTE:也许可以不用这么设置
		/*if( ATOM_AsyncLoader::IsRun() && _load_priority != ATOM_LoadPriority_IMMEDIATE )
		{
			_boundingBox.setMin (ATOM_Vector3f(-1000.f));
			_boundingBox.setMax (ATOM_Vector3f(1000.f));
		}
		else*/
		{
			_boundingBox.setMin (ATOM_Vector3f(0.f));
			_boundingBox.setMax (ATOM_Vector3f(0.f));
		}
	}
	else
	{
		_boundingBox.beginExtend ();
		for (unsigned i = 0; i < _meshes.size(); ++i)
		{
			_boundingBox.extend (_meshes[i]->getSharedMesh()->getBoundingbox().getMin());
			_boundingBox.extend (_meshes[i]->getSharedMesh()->getBoundingbox().getMax());
		}
	}
}

bool ATOM_Geode::addSharedModel (const char *modelFileName)
{
	return loadModel (ATOM_GetRenderDevice(), modelFileName);
}

// wangjian modified
#if 0
bool ATOM_Geode::addSharedModel (ATOM_SharedModel *model)
{
	if (model)
	{
		//--- wangjian added ---//
		// 异步加载
		// 如果是异步加载，则判断是否可以直接设置加载完成
		bool bMT = ( ATOM_AsyncLoader::IsRun() ) && ( _load_priority != ATOM_LoadPriority_IMMEDIATE );
		if( bMT )
		{
			//// 如果模型当前还未加载完成 
			//if( !model->getAsyncLoader()->IsLoadAllFinished() )
			//{
				for (unsigned i = 0; i < _components.size(); ++i)
				{
					if (_components[i].getModel() == model)
					{
						return true;
					}
				}

			//	_components.push_back (model);
			//	_component_setFlags.push_back(0);

			//	model->getAsyncLoader()->AddListener(this);

			//	return true;
			//}

			_components.push_back (model);
			_component_setFlags.push_back(0);

			// 全部改部件已经加载完成 处理该部件
			if( model->getAsyncLoader()->IsLoadAllFinished() )
			{
				if( ATOM_AsyncLoader::IsRun() && ATOM_AsyncLoader::isEnableLog() )
				{
					ATOM_LOGGER::debug(	"the component %s [ index : %d ] can be directly process! \n", 
										model->getAsyncLoader()->filename.c_str(),
										_component_setFlags.size()-1 );
				}
				processComponent(_component_setFlags.size()-1);
			}else
			{
				if( ATOM_AsyncLoader::IsRun() && ATOM_AsyncLoader::isEnableLog() )
				{
					ATOM_LOGGER::error(	"the component %s [ index : %d ] indirectly process! \n", 
										model->getAsyncLoader()->filename.c_str(),
										_component_setFlags.size()-1 );
				}
			}

			return true;
		}	
		//----------------------//

		// 该模型是非异步加载

		// 检查该模型的加载完成标记
		if( model->getAsyncLoader()->GetLoadStage() != ATOM_AsyncLoader::ATOM_ASYNCLOAD_ALLFINISHED )
			return false;

		bool bHasMesh = false;

		for (unsigned i = 0; i < _components.size(); ++i)
		{
			if (_components[i].getModel() == model)
			{
				return false;
			}
		}

		if (model->getSkeleton() && _skeleton && model->getSkeleton() != _skeleton)
		{
			return false;
		}

		if (model->getSkeleton ())
		{
			_skeleton = model->getSkeleton();
		}

		for (unsigned i = 0; i < model->getNumMeshes(); ++i)
		{
			_meshes.push_back (ATOM_NEW(ATOM_StaticMesh, this, model->getMesh (i)));

			if (_transparency != 1.f)
			{
				_meshes.back()->setTransparency (_transparency);
			}

			if(_colorMultiplier!= 1.0f)
			{
			_meshes.back()->setColorMultiplier(_colorMultiplier);
			}

			bHasMesh  = true;
		}

		for (unsigned i = 0; i < model->getNumTracks(); ++i)
		{
			_tracks.push_back (model->getTrack (i));
		}

		_components.push_back (model);

		//--- wangjian added ---//
		// 异步加载
		_component_setFlags.push_back(1);
		// 计算包围盒
		if( bHasMesh )
		{
			onTransformChanged();
			getWorldBoundingbox();
		}
		//----------------------//

		return true;
	}

	return false;
}

#else

bool ATOM_Geode::addSharedModel (ATOM_SharedModel *model, const int loadPriority)
{
	if (model)
	{
		//--- wangjian added ---//
		// 异步加载
		// 如果是异步加载，则判断是否可以直接设置加载完成
		bool bMT = ( ATOM_AsyncLoader::IsRun() ) && ( loadPriority != ATOM_LoadPriority_IMMEDIATE );
		if( bMT )
		{
			//// 如果模型当前还未加载完成 
			//if( !model->getAsyncLoader()->IsLoadAllFinished() )
			//{
			for (unsigned i = 0; i < _components.size(); ++i)
			{
				if (_components[i].getModel() == model)
				{
					return true;
				}
			}

			//	_components.push_back (model);
			//	_component_setFlags.push_back(0);

			//	model->getAsyncLoader()->AddListener(this);

			//	return true;
			//}

			_components.push_back (model);
			_component_setFlags.push_back(0);

			// 全部改部件已经加载完成 处理该部件
			if( model->getAsyncLoader()->IsLoadAllFinished() )
			{
				if( ATOM_AsyncLoader::IsRun() && ATOM_AsyncLoader::isEnableLog() )
				{
					ATOM_LOGGER::debug(	"the component %s [ index : %d ] can be directly process! \n", 
						model->getAsyncLoader()->filename.c_str(),
						_component_setFlags.size()-1 );
				}
				processComponent(_component_setFlags.size()-1);
			}else
			{
				if( ATOM_AsyncLoader::IsRun() && ATOM_AsyncLoader::isEnableLog() )
				{
					ATOM_LOGGER::error(	"the component %s [ index : %d ] indirectly process! \n", 
						model->getAsyncLoader()->filename.c_str(),
						_component_setFlags.size()-1 );
				}
			}

			return true;
		}	
		//----------------------//

		// 该模型是非异步加载

		// 检查该模型的加载完成标记
		if( model->getAsyncLoader()->GetLoadStage() != ATOM_AsyncLoader::ATOM_ASYNCLOAD_ALLFINISHED )
			return false;

		bool bHasMesh = false;

		for (unsigned i = 0; i < _components.size(); ++i)
		{
			if (_components[i].getModel() == model)
			{
				return false;
			}
		}

		if (model->getSkeleton() && _skeleton && model->getSkeleton() != _skeleton)
		{
			return false;
		}

		if (model->getSkeleton ())
		{
			_skeleton = model->getSkeleton();
		}

		for (unsigned i = 0; i < model->getNumMeshes(); ++i)
		{
			_meshes.push_back (ATOM_NEW(ATOM_StaticMesh, this, model->getMesh (i)));

			if (_transparency != 1.f)
			{
				_meshes.back()->setTransparency (_transparency);
			}

			if(_colorMultiplier!= 1.0f)
			{
				_meshes.back()->setColorMultiplier(_colorMultiplier);
			}

			bHasMesh  = true;
		}

		for (unsigned i = 0; i < model->getNumTracks(); ++i)
		{
			_tracks.push_back (model->getTrack (i));
		}

		_components.push_back (model);

		//--- wangjian added ---//
		// 异步加载
		_component_setFlags.push_back(1);
		// 计算包围盒
		if( bHasMesh )
		{
			_load_flag = LOAD_ALLFINISHED;	// 加载完成
			onTransformChanged();
			getWorldBoundingbox();
		}
		//----------------------//

		return true;
	}

	return false;
}

#endif

bool ATOM_Geode::removeSharedModel (ATOM_SharedModel *model)
{
	if (model)
	{
		unsigned modelIndex;
		for (modelIndex = 0; modelIndex < _components.size(); ++modelIndex)
		{
			if (_components[modelIndex].getModel() == model)
			{
				break;
			}
		}
		if (modelIndex >= _components.size())
		{
			return false;
		}

		if (_skeleton.get() == model->getSkeleton())
		{
			_skeleton = 0;
		}

		for (;;)
		{
			bool removed = false;

			for (unsigned i = 0; i < _meshes.size(); ++i)
			{
				if (_meshes[i]->getSharedMesh()->getModel() == model)
				{
					_meshes.erase (_meshes.begin() + i);
					removed = true;
					break;
				}
			}

			if (!removed)
			{
				break;
			}
		}

		for (;;)
		{
			bool removed = false;

			for (unsigned i = 0; i < _tracks.size(); ++i)
			{
				if (_tracks[i]->getModel() == model)
				{
					_tracks.erase (_tracks.begin() + i);
					removed = true;
					break;
				}
			}

			if (!removed)
			{
				break;
			}
		}

		_components.erase (_components.begin() + modelIndex);
		//--- wangjian added ---//
		// 异步加载
		_component_setFlags.erase(_component_setFlags.begin()+modelIndex);
		//----------------------//

		return true;
	}

	return false;
}

bool ATOM_Geode::findSharedModel (ATOM_SharedModel *model)
{
	for (unsigned i = 0; i < _components.size(); ++i)
	{
		if (_components[i].getModel() == model)
		{
			return true;
		}
	}
	return false;
}

unsigned ATOM_Geode::getNumMeshes (void) const
{
	return _meshes.size();
}

ATOM_StaticMesh *ATOM_Geode::getStaticMesh (unsigned index) const
{
	return _meshes[index].get();
}

ATOM_StaticMesh *ATOM_Geode::getStaticMeshByName (const char *name) const
{
	for (unsigned i = 0; i < _meshes.size(); ++i)
	{
		if (!strcmp (_meshes[i]->getSharedMesh()->getName(), name))
		{
			return _meshes[i].get();
		}
	}
	return 0;
}

void ATOM_Geode::deleteStaticMesh (int index)
{
	if (index < _meshes.size())
	{
		_meshes.erase(_meshes.begin() + index);
		invalidateBoundingbox ();
		_pickBoxDirty = true;
	}
}

unsigned ATOM_Geode::getNumTracks (void) const
{
	return _tracks.size();
}

ATOM_JointAnimationTrack *ATOM_Geode::getTrack (unsigned index) const
{
	return _tracks[index].get();
}

ATOM_JointAnimationTrack *ATOM_Geode::getTrackByName (const char *name) const
{
	for (unsigned i = 0; i < _tracks.size(); ++i)
	{
		if (!strcmp (_tracks[i]->getName(), name))
		{
			return _tracks[i].get();
		}
	}
	return 0;
}

ATOM_Skeleton *ATOM_Geode::getSkeleton (void) const
{
	return _skeleton.get();
}

bool ATOM_Geode::doAction (const char *actionName, unsigned flags, unsigned loopCount, bool forceReset, unsigned fadeTime, float speed)
{
	ATOM_STACK_TRACE(ATOM_Geode::doAction);

	bool upside = (flags & ACTIONFLAGS_UPSIDE) != 0;
	bool downside = (flags & ACTIONFLAGS_DOWNSIDE) != 0;
	if (upside)
	{
		if (!_actionMixer->setUpsideAction (actionName, loopCount, forceReset, fadeTime, speed))
		{
			return false;
		}
	}
	if (downside)
	{
		if (!_actionMixer->setDownsideAction (actionName, loopCount, forceReset, fadeTime, speed))
		{
			return false;
		}
	}
	return true;
}

void ATOM_Geode::stopAction (void)
{
	_actionMixer->clearActions ();
}

void ATOM_Geode::setDefaultActionSpeed (unsigned flags, float speed)
{
	ATOM_STACK_TRACE(ATOM_Geode::setDefaultActionSpeed);

	bool upside = (flags & ACTIONFLAGS_UPSIDE) != 0;
	bool downside = (flags & ACTIONFLAGS_DOWNSIDE) != 0;
	if (upside)
	{
		_actionMixer->setDefaultUpsideActionSpeed(speed);
	}
	if (downside)
	{
		_actionMixer->setDefaultDownsideActionSpeed(speed);
	}
}

void ATOM_Geode::setCurrentActionSpeed (unsigned flags, float speed)
{
	ATOM_STACK_TRACE(ATOM_Geode::setCurrentActionSpeed);

	bool upside = (flags & ACTIONFLAGS_UPSIDE) != 0;
	bool downside = (flags & ACTIONFLAGS_DOWNSIDE) != 0;
	if (upside)
	{
		_actionMixer->setCurrentUpsideActionSpeed(speed);
	}
	if (downside)
	{
		_actionMixer->setCurrentDownsideActionSpeed(speed);
	}
}

ATOM_ActionMixer2 *ATOM_Geode::getActionMixer (void) const
{
	return _actionMixer;
}

void ATOM_Geode::enableManualBoneTransform (const char *boneName, const ATOM_Matrix4x4f &matrix)
{
	if (boneName)
	{
		int boneIndex = getBoneIndex (boneName);
		if (boneIndex >= 0)
		{
			_actionMixer->enableManualBoneTransform (boneIndex, matrix);
		}
	}
}

void ATOM_Geode::disableManualBoneTransform (const char *boneName)
{
	if (boneName)
	{
		int boneIndex = getBoneIndex (boneName);
		if (boneIndex >= 0)
		{
			_actionMixer->disableManualBoneTransform (boneIndex);
		}
	}
}

bool ATOM_Geode::isBoneManualTransformEnabled (const char *boneName) const
{
	if (boneName)
	{
		int boneIndex = getBoneIndex (boneName);
		if (boneIndex >= 0)
		{
			return _actionMixer->isBoneManualTransformEnabled (boneIndex);
		}
	}
	return false;
}

void ATOM_Geode::resetActions (unsigned flags)
{
	ATOM_STACK_TRACE(ATOM_Geode::resetActions);

	if (flags & ATOM_Geode::ACTIONFLAGS_DOWNSIDE)
	{
		_actionMixer->resetDownsideAction ();
	}
	if (flags & ATOM_Geode::ACTIONFLAGS_UPSIDE)
	{
		_actionMixer->resetUpsideAction ();
	}
}

int ATOM_Geode::getBoneIndex (const char *name) const
{
	ATOM_STACK_TRACE(ATOM_Geode::getBoneIndex);

	int boneIndex = -1;
	if (name && _skeleton)
	{
		boneIndex = _skeleton->getAttachPointIndex (name);
	}
	return boneIndex;
}

bool ATOM_Geode::getBoneMatrix(int bone, ATOM_Matrix4x4f& mat, bool suppressManualTransform)
{
	return _actionMixer->getBoneMatrix (bone, mat, suppressManualTransform);
}

bool ATOM_Geode::getBoneMatrix(const char *boneName, ATOM_Matrix4x4f &mat, bool suppressManualTransform)
{
	if (boneName && _skeleton)
	{
		int index = _skeleton->getAttachPointIndex (boneName);
		return (index >= 0) ? getBoneMatrix (index, mat, suppressManualTransform) : false;
	}
	return false;
}

unsigned ATOM_Geode::getNumComponents (void) const
{
	return _components.size();
}

ATOM_Components ATOM_Geode::getComponents (unsigned index) const
{
	return _components[index];
}

// wangjian modified
// 异步加载相关
#if 0

bool ATOM_Geode::addComponents (ATOM_Components components)
{
	if (addSharedModel (components.getModel()))
	{
		_pickBoxDirty = true;
		invalidateBoundingbox ();
		return true;
	}
	return false;
}

#else

bool ATOM_Geode::addComponents (ATOM_Components components, const int loadPriority)
{
	if ( addSharedModel (components.getModel(), loadPriority) )
	{
		_pickBoxDirty = true;
		invalidateBoundingbox ();
		return true;
	}
	return false;
}

#endif

bool ATOM_Geode::removeComponents (ATOM_Components components)
{
	if (removeSharedModel (components.getModel()))
	{
		_pickBoxDirty = true;
		invalidateBoundingbox ();
		return true;
	}
	return false;
}

bool ATOM_Geode::removeComponents (unsigned index)
{
	return removeComponents (_components[index]);
}

void ATOM_Geode::removeAllComponents (void)
{
	_components.resize (0);
	//--- wangjian added ---//
	// 异步加载
	_component_setFlags.resize(0);
	//----------------------//
	_tracks.resize (0);
	_meshes.resize (0);
	_skeleton = 0;
	invalidateBoundingbox ();
	ATOM_DELETE(_pickBox);
	_pickBox = 0;
	_pickBoxDirty = false;
}

void ATOM_Geode::syncComponents (void)
{
}

bool ATOM_Geode::attach (const char *attachPoint, ATOM_Node *node, AttachmentTransformCallback transformCallback, void *userData)
{
	if (!node || !attachPoint || node->getParent())
	{
		return false;
	}

	for (unsigned i = 0; i < _attachments.size(); ++i)
	{
		if (_attachments[i].node == node)
		{
			return false;
		}
	}

	ATOM_Skeleton *skeleton = this->getSkeleton ();
	if (skeleton)
	{
		unsigned index = skeleton->getAttachPointIndex(attachPoint);
		if (index != (unsigned)-1)
		{
			appendChild (node);
			AttachInfo info;
			info.attached = true;
			info.bone = index;
			info.node = node;
			info.callback = transformCallback;
			info.userData = userData;
			_attachments.push_back (info);

			return true;
		}
	}

	AttachInfo info;
	info.attached = false;
	info.attachPoint = attachPoint;
	info.node = node;
	info.callback = transformCallback;
	info.userData = userData;
	_attachments.push_back (info);

	return true/*false*/;
}

void ATOM_Geode::detach (ATOM_Node *node)
{
#if 1
	removeChild (node);
#else
	if (node)
	{
		int index = -1;
		for (int i = 0; i < _attachments.size(); ++i)
		{
			if (_attachments[i].node == node)
			{
				index = i;
				break;
			}
		}
		if (index < 0)
		{
			return;
		}

		if (node->getParent() == this)
		{
			removeChild (node);
		}
		_attachments.erase (_attachments.begin() + index);
	}
#endif
}

void ATOM_Geode::updateAttachments (void)
{
	unsigned numAttachments = _attachments.size();

	if (numAttachments > 0)
	{
		ATOM_Matrix4x4f m;

		for (unsigned i = 0; i < numAttachments; ++i)
		{
			AttachInfo &info = _attachments[i];

			if (!info.attached)
			{
				ATOM_Skeleton *skeleton = this->getSkeleton ();
				if (skeleton)
				{
					unsigned index = skeleton->getAttachPointIndex(info.attachPoint.c_str());
					if (index != (unsigned)-1)
					{
						appendChild (info.node.get());
						info.attached = true;
						info.bone = index;
					}
				}
			}

			if (info.attached)
			{
				if (!getBoneMatrix (info.bone, m, false))
				{
					m.makeIdentity ();
				}

				if (info.callback)
				{
					info.callback (this, info.attachPoint.c_str(), info.node.get(), &m, info.userData);
				}

				info.node->setO2T (m);
			}
		}
	}
}

bool ATOM_Geode::updateComponentsOpHistory (void)
{
	return true;
}

const ATOM_VECTOR<ATOM_STRING> &ATOM_Geode::getModelFileNames (void) const
{
	return _modelFileNames;
}

void ATOM_Geode::setModelFileNames (const ATOM_VECTOR<ATOM_STRING> &modelFileNames)
{
	_modelFileNames = modelFileNames;
}

const ATOM_STRING &ATOM_Geode::getModelFileName (void) const
{
	return _modelFileName;
}

void ATOM_Geode::setModelFileName (const ATOM_STRING &filename)
{
	if (_modelFileName != filename)
	{
		//================================================//
		// wangjian modified
#if 0
		bool doLoad = getLoadInterface()->getLoadingState() != ATOM_LoadInterface::LS_NOTLOADED;
#else
		bool doLoad = (LOAD_ALLFINISHED == _load_flag);
#endif
		clear ();
		_modelFileName = filename;

		if (doLoad)
		{
			_load_flag = LOAD_NOTLOAD;
			load (ATOM_GetRenderDevice());
		}
		//================================================//
	}
}

void ATOM_Geode::setTransparency (float val)
{
	if (val != _transparency)
	{
		//--- wangjian added ---//
		if( _transparency == 1.0f || val == 1.0f )
		{
			ATOM_RenderScheme::setForceCullUpdate(true);
		}
		//----------------------//

		val = ATOM_saturate(val);				// 0 - 1

		_transparency = val;

		for (unsigned i = 0; i < _meshes.size(); ++i)
		{
			_meshes[i]->setTransparency (val);
		}
	}
}

float ATOM_Geode::getTransparency (void) const
{
	return _transparency;
}

void ATOM_Geode::enableZTransparency (int enable)
{
	_zTransparency = enable;
}

int ATOM_Geode::isZTransparencyEnabled (void) const
{
	return _zTransparency;
}

//--- wangjian added ---//
void ATOM_Geode::setColorMultiplier (const float val)
{
	if (val != _colorMultiplier)
	{
		////--- wangjian added ---//
		//if( _transparency == 1.0f || val == 1.0f )
		//{
		//	ATOM_RenderScheme::setForceCullUpdate(true);
		//}
		////----------------------//

		_colorMultiplier = val;

		for (unsigned i = 0; i < _meshes.size(); ++i)
		{
			_meshes[i]->setColorMultiplier (_colorMultiplier);
		}
	}
}

const float ATOM_Geode::getColorMultiplier (void) const
{
	return _colorMultiplier;
}

// 异步加载
void ATOM_Geode::onLoadFinished()
{
	postProcessLoad();

	//ATOM_Node::onLoadFinished();
}
void ATOM_Geode::postProcessLoad()
{
	//bool bHasMesh = false;
	for (unsigned i = 0; i < _components.size(); ++i)
	{
		processComponent(i);
	}

	//// 如果有网格，计算包围盒
	//if( bHasMesh )
	//{
	//	onTransformChanged();
	//	getWorldBoundingbox();
	//}

	//_load_flag = LOAD_ALLFINISHED;
}
bool ATOM_Geode::processComponent(int index)
{
	ATOM_ASSERT( index >= 0 && index < _component_setFlags.size() );
	ATOM_ASSERT( _component_setFlags.size() ==  _components.size() );

	// 检查ID
	if( index < 0 || index >= _component_setFlags.size() )
	{
		//if( ATOM_AsyncLoader::isEnableLog() )
		{	
			ATOM_LOGGER::warning(	"Component index is invalid : the size of _components is : %d , the size of _component_setFlags is : %d, the index is %d\n",
									_components.size(),
									_component_setFlags.size(),
									index	);
		}
		return false;
	}

	if( _component_setFlags.size() !=  _components.size() )
	{
		//if( ATOM_AsyncLoader::isEnableLog() )
		{	
			ATOM_LOGGER::warning(	"Component size is mismatch : the size of _components is : %d , the size of _component_setFlags is : %d, the index is %d\n",
									_components.size(),
									_component_setFlags.size(),
									index	);
		}
		return false;
	}

	// 已经设置过了 无需再设置
	if( _component_setFlags[index] == 1 )
	{
		if( ATOM_AsyncLoader::isEnableLog() )
		{
			ATOM_LOGGER::warning(	"Component %d : the flag of component %s is already set\n",
									index,
									_components[index].getModel()->getAsyncLoader()->filename.c_str() );
		}
		return false;
	}

	bool bHasMesh = false;

	ATOM_SharedModel * model = _components[index].getModel();
	if ( model && model->getAsyncLoader()->IsLoadAllFinished() )
	{
		if( model->getAsyncLoader()->IsLoadFailed() )
		{
			ATOM_LOGGER::error( "<%s>: Component %d : the model %s is load failed! \n", __FUNCTION__, index, model->getAsyncLoader()->filename.c_str() );
			_load_flag = LOAD_ALLFINISHED;
			return false;
		}
		if (model->getSkeleton() && _skeleton && model->getSkeleton() != _skeleton)
		{
			ATOM_LOGGER::error(	"Component %d : the skeleton of %s is not match \n",
								index,
								model->getAsyncLoader()->filename.c_str()	);
			_load_flag = LOAD_ALLFINISHED;
			return false;
		}

		if (model->getSkeleton ())
		{
			_skeleton = model->getSkeleton();
		}

		for (unsigned i = 0; i < model->getNumMeshes(); ++i)
		{
			_meshes.push_back (ATOM_NEW(ATOM_StaticMesh, this, model->getMesh (i)));

			if (_transparency != 1.f)
			{
				_meshes.back()->setTransparency (_transparency);
			}

			if(_colorMultiplier!= 1.0f)
			{
				_meshes.back()->setColorMultiplier (_colorMultiplier);
			}
		
			bHasMesh  = true;
		}

		for (unsigned i = 0; i < model->getNumTracks(); ++i)
		{
			_tracks.push_back (model->getTrack (i));
		}

		if( ATOM_AsyncLoader::IsRun() && ATOM_AsyncLoader::isEnableLog() )
		{
			ATOM_LOGGER::debug(	"the component %s [ index : %d ] flag is set! \n", 
								model->getAsyncLoader()->filename.c_str(),
								index );
		}

		_component_setFlags[index] = 1;

		// 如果有网格， 则可以显示了，计算包围盒
		if( bHasMesh )
		{
			_load_flag = LOAD_ALLFINISHED;
			onTransformChanged();
			getWorldBoundingbox();
		}

		if( ATOM_AsyncLoader::isEnableLog() )
		{
			ATOM_LOGGER::log("component %s is loaded and installed in slot : %d !\n", model->getAsyncLoader()->filename.c_str(), index );
		}

		return true;
	}

	return false;
}

void ATOM_Geode::resetMaterialDirtyFlag_impl()
{
	for (unsigned i = 0; i < _meshes.size(); ++i)
	{
		if( _meshes[i] )
			_meshes[i]->resetMaterialDirtyFlag ();
	}
}
//----------------------//

