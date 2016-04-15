#include "StdAfx.h"
#include "material.h"
#include "paramcallback.h"
#include "parameterchannel.h"
#include "stateset_io.h"
#include "renderscheme.h"

//--- wangjian added : ��ʱ�����ã���Ӱ������ ---//
//* ���ڼ��궨��ĺϷ��ԣ�������Ϸ����򲻻ᶨ��ú� *//
//class ShaderDefineChecker
//{
//public:
//	enum ePREREQ_TYPE
//	{
//		NO_PREREQUISITE,
//		DEVICECAPS_PREREQUISITE,
//		MATERIAL_PREREQUISITE,
//		USEROPTION_PREREQUISITE,
//		PREREQUISITE_ALL,
//	};
//	enum eDEVICECAPSPREREQUISITE
//	{
//		HARDWARE_PCF,
//		SM30,
//		ATI_FETCH4,
//		DCP_ALL,
//	};
//
//	ShaderDefineChecker()
//	{
//		const ATOM_STRING classes_name[PREREQUISITE_ALL] = {	"NONE", 
//																"DeviceCaps", 
//																"Material", 
//																"UserOption" 
//															};
//		const ePREREQ_TYPE classes_value[PREREQUISITE_ALL] =	{	NO_PREREQUISITE, 
//																	DEVICECAPS_PREREQUISITE, 
//																	MATERIAL_PREREQUISITE, 
//																	USEROPTION_PREREQUISITE
//																};
//		for( int i = 0; i < PREREQUISITE_ALL; ++i )
//			_classes[classes_name[i]] = classes_value[i];
//
//		const ATOM_STRING dcps_name[DCP_ALL] = {	"Hardware_PCF",
//													"SM30",
//													"ATI_Fetch4"
//												};
//		const eDEVICECAPSPREREQUISITE dcps_value[DCP_ALL] = {	HARDWARE_PCF, 
//																SM30, 
//																ATI_FETCH4	};
//		for( int i = 0; i < DCP_ALL; ++i )
//			_devicecaps_prerequisites[dcps_name[i]] = dcps_value[i];
//	}
//	~ShaderDefineChecker()
//	{
//		_classes.clear();
//		_devicecaps_prerequisites.clear();
//	}
//
//	bool checkDefine( ATOM_RenderDevice *device, const ATOM_STRING & type, const ATOM_STRING & prerequisite )
//	{
//		ATOM_MAP<ATOM_STRING,ePREREQ_TYPE>::const_iterator iter = _classes.find(type);
//		if( iter == _classes.end() )
//			return false;
//		ePREREQ_TYPE t = iter->second;
//		ATOM_ASSERT( t >= NO_PREREQUISITE && t < PREREQUISITE_ALL );
//
//		switch( t )
//		{
//		case NO_PREREQUISITE :
//			return true;
//		case DEVICECAPS_PREREQUISITE :
//			return checkDevicePrerequisite(device,prerequisite);
//		}
//
//		return true;
//	}
//
//private:
//
//	bool checkDevicePrerequisite(ATOM_RenderDevice *device, const ATOM_STRING & prerequisite )
//	{
//		ATOM_MAP<ATOM_STRING,eDEVICECAPSPREREQUISITE>::const_iterator iter = _devicecaps_prerequisites.find(prerequisite);
//		if( iter == _devicecaps_prerequisites.end() )
//			return false;
//		eDEVICECAPSPREREQUISITE dcp = iter->second;
//		ATOM_ASSERT( dcp >= HARDWARE_PCF && dcp < DCP_ALL );
//
//		switch( dcp )
//		{
//		case HARDWARE_PCF :
//			return device->getCapabilities().hardware_pcf;
//		case SM30 :
//			return device->getCapabilities().shader_model >= 3;
//		case ATI_FETCH4 :
//			return device->getCapabilities().ati_fetch4;
//		}
//
//		return true;
//	}
//
//private:
//	ATOM_MAP<ATOM_STRING,ePREREQ_TYPE> _classes;
//	ATOM_MAP<ATOM_STRING,eDEVICECAPSPREREQUISITE> _devicecaps_prerequisites;
//};
//static ShaderDefineChecker g_shaderDefineChecker;
//----------------------//

unsigned load_time = 0;
unsigned createvsps_time = 0;

struct ParamArrayInfo
{
	ATOM_STRING arraySizeStr;
	void *arrayValues;
	unsigned size;
	ParamArrayInfo (void)
	{
		arrayValues = 0;
		size = 0;
	}
	~ParamArrayInfo (void)
	{
		ATOM_FREE(arrayValues);
	}
};

struct ParamArrayInfoMap
{
	ATOM_HASHMAP<ATOM_STRING, ParamArrayInfo*> arrayInfoMap;
	ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOREF(ATOM_Shader)> shaderMap;

	~ParamArrayInfoMap (void)
	{
		for (ATOM_HASHMAP<ATOM_STRING, ParamArrayInfo*>::iterator it = arrayInfoMap.begin(); it != arrayInfoMap.end(); ++it)
		{
			ATOM_DELETE(it->second);
		}
	}

	ATOM_Shader *getShader (const char *name) const
	{
		ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOREF(ATOM_Shader)>::const_iterator it = shaderMap.find (name);
		return it == shaderMap.end() ? 0 : it->second.get();
	}
};

static bool parseFloat (const char *str, float &value)
{
	value = atof (str);
	return true;
}

static bool parseInt (const char *str, int &value)
{
	value = atoi (str);
	return true;
}

static ATOM_VECTOR<float> parseFloatArray (const char *str)
{
	ATOM_VECTOR<float> ret;
	const char *s = str;

	for (;;)
	{
		ret.push_back (atof (s));
		s = strchr (s, ',');
		if (!s)
		{
			break;;
		}
		s++;
	}

	return ret;
}

static ATOM_VECTOR<int> parseIntArray (const char *str)
{
	ATOM_VECTOR<int> ret;
	const char *s = str;

	for (;;)
	{
		ret.push_back (atoi (s));
		s = strchr (s, ',');
		if (!s)
		{
			break;
		}
		s++;
	}

	return ret;
}


ATOM_MaterialPass::ATOM_MaterialPass (ATOM_RenderDevice *device, unsigned numParams)
{
	_streamDefine = 0;
	_stateset = device->allocRenderStateSet (0);
	_gpuProgram = device->allocGPUProgram (0);
	_minRegisterIndexVS = 0;
	_totalRegisterCountVS = 0;
	_minRegisterIndexPS = 0;
	_totalRegisterCountPS = 0;
	_inBegin = false;

	if (numParams)
	{
		// filled with -1
		_paramDescsVS.resize (numParams);
		memset (&_paramDescsVS[0], 0x0, sizeof(_paramDescsVS[0])*numParams);

		_paramDescsPS.resize (numParams);
		memset (&_paramDescsPS[0], 0x0, sizeof(_paramDescsPS[0])*numParams);
	}
}

ATOM_MaterialPass::~ATOM_MaterialPass (void)
{
}

void ATOM_MaterialPass::setStateSet (ATOM_RenderStateSet *stateSet)
{
	_stateset = stateSet;
}

ATOM_RenderStateSet *ATOM_MaterialPass::getStateSet (void) const
{
	return _stateset.get();
}

void ATOM_MaterialPass::setGPUProgram (ATOM_GPUProgram *gpuProgram)
{
	_gpuProgram = gpuProgram;
}

ATOM_GPUProgram *ATOM_MaterialPass::getGPUProgram (void) const
{
	return _gpuProgram.get();
}

void ATOM_MaterialPass::setStreamDefine (unsigned streamDefine)
{
	_streamDefine = streamDefine;
}

unsigned ATOM_MaterialPass::getStreamDefine (void) const
{
	return _streamDefine;
}

bool ATOM_MaterialPass::begin (ATOM_RenderDevice *device, ATOM_ParameterTable *paramTable)
{
	if (_inBegin)
	{
		return false;
	}

	//-----------------------------------------------------------------------------//
	// wangjian added
#if 1
	if (this != paramTable->_currentPass)
	{
		paramTable->_currentPass = this;
		paramTable->updateShaderConstantMirrors ();
	}

	for (int i = 0; i < _paramInfo.size(); ++i)
	{
		ParamCallbackInfo *info = &_paramInfo[i];
		ATOM_EffectParameterValue *value = paramTable->getParameter(info->param->getParamIndex());
		paramTable->evalValue (value);

		if (info->info->callback)
		{
			info->info->callback(value, info->info->context, info->info->context2);
		}
	}

	paramTable->commitShaderConstants (device);
#endif
	//-----------------------------------------------------------------------------//

	if (commit ())
	{
		_inBegin = true;
		return true;
	}

	return false;
}

void ATOM_MaterialPass::end (void)
{
	_inBegin = false;
}

bool ATOM_MaterialPass::commit (void)
{
	if (_stateset && !_stateset->commit())
	{
		return false;
	}

	if (_gpuProgram && !_gpuProgram->bind())
	{
		return false;
	}

	return true;
}

//--- wangjian added ---//
void ATOM_MaterialPass::addParamInfo(ATOM_MaterialParam *param,const ATOM_MaterialParam::CallbackInfo *info)
{
	ParamCallbackInfo paramInfo;
	paramInfo.param = param;
	paramInfo.info = info;
	_paramInfo.push_back (paramInfo);
}
//----------------------//

const ATOM_Shader::ParamDesc *ATOM_MaterialPass::getShaderParamDescVS (void) const
{
	return _paramDescsVS.empty () ? nullptr : &_paramDescsVS[0];
}

const ATOM_Shader::ParamDesc *ATOM_MaterialPass::getShaderParamDescPS (void) const
{
	return _paramDescsPS.empty () ? nullptr : &_paramDescsPS[0];
}

unsigned ATOM_MaterialPass::getMinRegisterIndexVS (void) const
{
	return _minRegisterIndexVS;
}

unsigned ATOM_MaterialPass::getRegisterCountVS (void) const
{
	return _totalRegisterCountVS;
}

unsigned ATOM_MaterialPass::getMinRegisterIndexPS (void) const
{
	return _minRegisterIndexPS;
}

unsigned ATOM_MaterialPass::getRegisterCountPS (void) const
{
	return _totalRegisterCountPS;
}

bool ATOM_MaterialPass::isInBegin (void) const
{
	return _inBegin;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_MaterialEffect::ATOM_MaterialEffect (const char *name, const char *scheme, int layer, ATOM_CoreMaterial *material)
{
	_material = material;
	_renderSchemeLayerNumber = layer;
	_name = name ? name : "";
	_scheme = scheme ? scheme : "";
	_schemeFactory = scheme ? ATOM_RenderScheme::getRegisteredFactory(scheme) : 0;

	//--- wangjian added ---//
	_enabled = true;
	//----------------------//
}

ATOM_MaterialEffect::~ATOM_MaterialEffect (void)
{
}

unsigned ATOM_MaterialEffect::addPass (ATOM_MaterialPass *pass)
{
	if (pass)
	{
		_passes.push_back (pass);
		return _passes.size()-1;
	}
	return 0;
}

void ATOM_MaterialEffect::removePass (unsigned index)
{
	if (index < _passes.size())
	{
		_passes.erase (_passes.begin()+index);
	}
}

void ATOM_MaterialEffect::removeAllPasses(void)
{
	_passes.resize(0);
}

unsigned ATOM_MaterialEffect::getNumPasses (void) const
{
	return _passes.size();
}

ATOM_MaterialPass *ATOM_MaterialEffect::getPass (unsigned index) const
{
	return (index < _passes.size()) ? _passes[index].get() : 0;
}

ATOM_CoreMaterial *ATOM_MaterialEffect::getMaterial (void) const
{
	return _material;
}

int ATOM_MaterialEffect::getRenderSchemeLayerNumber (void) const
{
	return _renderSchemeLayerNumber;
}

const char *ATOM_MaterialEffect::getName (void) const
{
	return _name.c_str();
}

const char *ATOM_MaterialEffect::getRenderScheme (void) const
{
	return _scheme.c_str();
}

bool ATOM_MaterialEffect::validate (ATOM_RenderScheme *currentRenderScheme, int currentSchemeLayer) const
{
	//--- wangjian added ---//
	if( !_enabled )
	{
		return false;
	}
	//----------------------//

	if (!_schemeFactory)
	{
		return false;
	}

	if (!currentRenderScheme)
	{
		return false;
	}

	if (_schemeFactory != currentRenderScheme->getFactory())
	{
		return false;
	}

	if (currentSchemeLayer >= 0 && _renderSchemeLayerNumber != currentSchemeLayer)
	{
		return false;
	}

	return true;
}

void ATOM_MaterialEffect::gatherUsedParams (void)
{
	for (ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_MaterialParam)>::const_iterator it = _material->_params.begin(); it != _material->_params.end(); ++it)
	{
		ATOM_MaterialParam *param = it->second.get();
		for (int i = 0; i < param->getNumCallbacks(); ++i)
		{
			const ATOM_MaterialParam::CallbackInfo *info = param->getCallback (i);

			//------------ wangjian modified -----------//
#if 0
			if ( !info->effect || info->effect == this )
			{
				ParamCallbackInfo paramInfo;
				paramInfo.param = param;
				paramInfo.info = info;
				_paramInfo.push_back (paramInfo);
			}
#else
			// ����Ǹ�Effect,��ѡ������ĸ�PASS�Ĳ���������
			if( info->effect == this )
			{
				if( info->pass )
				{
					info->pass->addParamInfo(param,info);
				}
				else
				{
					ParamCallbackInfo paramInfo;
					paramInfo.param = param;
					paramInfo.info = info;
					_paramInfo.push_back (paramInfo);
				}
			}
			// ���EFFECTû�� �����EFFECT����Ĳ���������
			else if (!info->effect )
			{
				ParamCallbackInfo paramInfo;
				paramInfo.param = param;
				paramInfo.info = info;
				_paramInfo.push_back (paramInfo);
			}
#endif
			//----------------------------------------------//
		}
	}
}

void ATOM_MaterialEffect::linkShaderParameters (void)
{
	for (unsigned i = 0; i < _passes.size(); ++i)
	{
		int firstIndexVS = 10000;
		int lastIndexVS = -1;
		int lastCountVS = 0;

		int firstIndexPS = 10000;
		int lastIndexPS = -1;
		int lastCountPS = 0;

		for (ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_MaterialParam)>::const_iterator it = _material->_params.begin(); it != _material->_params.end(); ++it)
		{
			ATOM_MaterialParam *param = it->second.get();
			if (param->getParamType() == ATOM_MaterialParam::ParamType_Sampler)
			{
				continue;
			}

			ATOM_Shader *vertexshader = _passes[i]->getGPUProgram()->getShader (ATOM_SHADER_TYPE_VERTEXSHADER);
			if (vertexshader)
			{
				ATOM_Shader::ParamHandle handle = vertexshader->getParamHandle (it->first.c_str(), 0);
				if (handle)
				{
					ATOM_Shader::ParamDesc *desc = &_passes[i]->_paramDescsVS[param->getParamIndex()];
					if (vertexshader->getParamDesc (handle, desc))
					{
						if (desc->registerIndex < firstIndexVS)
						{
							firstIndexVS = desc->registerIndex;
						}
						if (desc->registerIndex > lastIndexVS)
						{
							lastIndexVS = desc->registerIndex;
							lastCountVS = desc->registerCount;
						}

						if (param->getValueCallback())
						{
							param->addApplyCallback (nullptr, vertexshader, handle, this, _passes[i].get());
						}
						param->setFlags (param->getFlags()|ATOM_MaterialParam::Flag_ShaderConstantVS);
						//--- wangjian added ---//
						//param->setPassIndex(i);
						//_passes[i]->addParamCallbackInfo( param, param->getCallback(param->getNumCallbacks()-1) );
						//----------------------//
					}
				}
			}

			ATOM_Shader *pixelshader = _passes[i]->getGPUProgram()->getShader (ATOM_SHADER_TYPE_PIXELSHADER);
			if (pixelshader)
			{
				ATOM_Shader::ParamHandle handle = pixelshader->getParamHandle (it->first.c_str(), 0);
				if (handle)
				{
					ATOM_Shader::ParamDesc *desc = &_passes[i]->_paramDescsPS[param->getParamIndex()];
					if (pixelshader->getParamDesc (handle, desc))
					{
						if (desc->registerIndex < firstIndexPS)
						{
							firstIndexPS = desc->registerIndex;
						}
						if (desc->registerIndex > lastIndexPS)
						{
							lastIndexPS = desc->registerIndex;
							lastCountPS = desc->registerCount;
						}
						if (param->getValueCallback())
						{
							param->addApplyCallback (nullptr, pixelshader, handle, this, _passes[i].get());
						}
						param->setFlags (param->getFlags()|ATOM_MaterialParam::Flag_ShaderConstantPS);
						//--- wangjian added ---//
						//param->setPassIndex(i);
						//_passes[i]->addParamCallbackInfo(param,param->getCallback(param->getNumCallbacks()-1));
						//----------------------//
					}
				}
			}
		}
		_passes[i]->_minRegisterIndexVS = lastCountVS>0 ? firstIndexVS : 0;
		_passes[i]->_totalRegisterCountVS = lastCountVS>0 ? lastIndexVS+lastCountVS-firstIndexVS : 0;
		_passes[i]->_minRegisterIndexPS = lastCountPS>0 ? firstIndexPS : 0;
		_passes[i]->_totalRegisterCountPS = lastCountPS>0 ? lastIndexPS+lastCountPS-firstIndexPS : 0;
	}
}

//--- wangjian added ---//
void ATOM_MaterialEffect::enable(bool bEnable)
{
	_enabled = bEnable;
}
bool ATOM_MaterialEffect::isEnable(void) const
{
	return _enabled;
}
//----------------------//

/*
void ATOM_MaterialEffect::applySamplerIndices (void)
{
	for (ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_MaterialParam)>::const_iterator it = _params.begin(); it != _params.end(); ++it)
	{
		ATOM_MaterialParam *param = it->second.get();
		if (param->getParamType() == ATOM_MaterialParam::ParamType_Sampler)
		{
			ATOM_MaterialSamplerParam *samplerParam = (ATOM_MaterialSamplerParam*)param;
			samplerParam->setIndex (0);

			for (unsigned i = 0; i < _passes.size(); ++i)
			{
				ATOM_Shader *shader = _passes[i]->getGPUProgram()->getShader (ATOM_SHADER_TYPE_PIXELSHADER);
				if (!shader)
				{
					continue;
				}

				ATOM_Shader::ParamHandle handle = shader->getParamHandle (it->first.c_str(), 0);
				if (!handle)
				{
					continue;
				}

				int index = shader->getSamplerIndex (handle);
				if (index >= 0)
				{
					samplerParam->setIndex (index);
					break;
				}
			}
		}
	}
}
*/

//--- wangjian added ---//
// ����shader�Ķ������ļ��ļ���
//ATOM_File * ATOM_CoreMaterial::_shaderBin = 0;
//bool ATOM_CoreMaterial::_readBin = false;
//----------------------//
//--- wangjian added ---//
//ATOM_AsyncableLoadMaterial::ATOM_AsyncableLoadMaterial()
//{
//
//}
//void ATOM_AsyncableLoadMaterial::SetAttribute( ATOM_CoreMaterial * material )
//{
//	// �������ü���
//	//m_texture = texture;
//	host = material;
//}
////void ATOM_AsyncableLoadMaterial::GetAttribute( int & width, int & height, ATOM_PixelFormat & fmt )
////{
////	/*width = m_width;
////	height = m_height;
////	fmt = m_format;*/
////}
//bool ATOM_AsyncableLoadMaterial::Load()
//{
//	if( __super::Load() == false )
//		return false;
//
//	/////////////////////////////////////////////////////
//	//if( flag != ATOM_AsyncLoader::ATOM_ASYNCLOAD_INLOADING )
//	//	return false;
//
//	//ATOM_AutoFile fp(filename.c_str(), ATOM_VFS::read);
//	//if (!fp)
//	//{
//	//	ATOM_LOGGER::error ("%s(%s, %u, %u)  Open file failed.\n", __FUNCTION__, filename.c_str());
//	//	return false;
//	//}
//	//unsigned size = fp->size ();
//	//if (!size)
//	//{
//	//	return false;
//	//}
//
//	//void* buffer = GetBuffer(size);
//	//if( buffer )
//	//	fp->read (buffer, size);
//
//	Done();
//
//	return true;
//}
//bool ATOM_AsyncableLoadMaterial::OnloadEnd()
//{
//	// �ݼ����ü��� ���Ϊ0 ��������
//	//ATOM_Texture * texture = m_texture.get();
//	//m_texture = 0;
//	//if( texture )
//	if( host )
//	{
//		/*ATOM_Texture * tex = dynamic_cast<ATOM_Texture*>(host);
//		if(tex)
//			tex->loadTexImageFromFile(0,0,0,ATOM_PIXEL_FORMAT_UNKNOWN);*/
//	}
//
//	return __super::OnloadEnd();
//}
//void ATOM_AsyncableLoadMaterial::AddShaderCreationItem(	ATOM_ShaderD3D9 * shader,
//	ATOM_STRING		* shader_code,
//	ATOM_STRING		entryPoint,
//	int				shaderCompileTarget,
//	bool			createFromBin,
//	ATOM_File *		binFile)
//{
//
//}
//---------------------------------------------------------------------//

ATOM_MaterialParam *ATOM_CoreMaterial::getParameter (const char *name) const
{
	if (name)
	{
		ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_MaterialParam)>::const_iterator it = _params.find (name);
		if (it != _params.end())
		{
			return it->second.get();
		}
	}
	return 0;
}

void ATOM_CoreMaterial::addParameter (const char *name, ATOM_MaterialParam *param)
{
	int index = _params.size();
	param->setParamIndex (index);
	_params[name] = param;
}

unsigned ATOM_CoreMaterial::getNumParameters (void) const
{
	return _params.size();
}

ATOM_CoreMaterial::ParamHandle ATOM_CoreMaterial::getParameterHandle (const char *name) const
{
	if (name)
	{
		ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_MaterialParam)>::const_iterator it = _params.find (name);
		return (it == _params.end()) ? 0 : it->second.get();
	}
	return 0;
}

bool setFloatParameter (ATOM_CoreMaterial::ParamHandle param, float value)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_Float:
			((ATOM_MaterialFloatParam*)param)->setValue (value);
			return true;
		case ATOM_MaterialParam::ParamType_Int:
			((ATOM_MaterialIntParam*)param)->setValue (value);
			return true;
		}
	}
	return false;
}

float getFloatParameter (ATOM_CoreMaterial::ParamHandle param)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_Float:
			return ((ATOM_MaterialFloatParam*)param)->getValue ();
		case ATOM_MaterialParam::ParamType_Int:
			return ((ATOM_MaterialIntParam*)param)->getValue ();
		}
	}
	return 0.f;
}

bool setFloatArrayParameter (ATOM_CoreMaterial::ParamHandle param, const float *values, unsigned count)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_FloatArray:
			((ATOM_MaterialFloatArrayParam*)param)->setValue (values, count);
			return true;
		}
	}
	return false;
}

const float *getFloatArrayParameter (ATOM_CoreMaterial::ParamHandle param)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_FloatArray:
			return ((ATOM_MaterialFloatArrayParam*)param)->getValue ();
		}
	}
	return 0;
}

bool setIntParameter (ATOM_CoreMaterial::ParamHandle param, int value)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_Int:
			((ATOM_MaterialIntParam*)param)->setValue (value);
			return true;
		case ATOM_MaterialParam::ParamType_Float:
			((ATOM_MaterialFloatParam*)param)->setValue (value);
			return true;
		}
	}
	return false;
}

int getIntParameter (ATOM_CoreMaterial::ParamHandle param)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_Int:
			return ((ATOM_MaterialIntParam*)param)->getValue ();
		case ATOM_MaterialParam::ParamType_Float:
			return ((ATOM_MaterialFloatParam*)param)->getValue ();
		}
	}
	return 0;
}

bool setIntArrayParameter (ATOM_CoreMaterial::ParamHandle param, const int *values, unsigned count)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_IntArray:
			((ATOM_MaterialIntArrayParam*)param)->setValue (values, count);
			return true;
		}
	}
	return false;
}

const int *getIntArrayParameter (ATOM_CoreMaterial::ParamHandle param)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_IntArray:
			return ((ATOM_MaterialIntArrayParam*)param)->getValue ();
		}
	}
	return 0;
}

bool setVectorParameter (ATOM_CoreMaterial::ParamHandle param, const ATOM_Vector4f &value)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_Vector:
			((ATOM_MaterialVectorParam*)param)->setValue (value);
			return true;
		}
	}
	return false;
}

ATOM_Vector4f getVectorParameter (ATOM_CoreMaterial::ParamHandle param)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_Vector:
			return ((ATOM_MaterialVectorParam*)param)->getValue ();
		}
	}
	return ATOM_Vector4f(0.f);
}

bool setVectorArrayParameter (ATOM_CoreMaterial::ParamHandle param, const ATOM_Vector4f *values, unsigned count)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_VectorArray:
			((ATOM_MaterialVectorArrayParam*)param)->setValue (values, count);
			return true;
		}
	}
	return false;
}

const ATOM_Vector4f *getVectorArrayParameter (ATOM_CoreMaterial::ParamHandle param)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_VectorArray:
			return ((ATOM_MaterialVectorArrayParam*)param)->getValue ();
		}
	}
	return 0;
}

bool setMatrix44Parameter (ATOM_CoreMaterial::ParamHandle param, const ATOM_Matrix4x4f &value)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_Matrix44:
			((ATOM_MaterialMatrix44Param*)param)->setValue (value);
			return true;
		}
	}
	return false;
}

ATOM_Matrix4x4f getMatrix44Parameter (ATOM_CoreMaterial::ParamHandle param)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_Matrix44:
			return ((ATOM_MaterialMatrix44Param*)param)->getValue ();
		}
	}
	return ATOM_Matrix4x4f::getIdentityMatrix ();
}

bool setMatrix44ArrayParameter (ATOM_CoreMaterial::ParamHandle param, const ATOM_Matrix4x4f *values, unsigned count)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_Matrix44Array:
			((ATOM_MaterialMatrix44ArrayParam*)param)->setValue (values, count);
			return true;
		}
	}
	return false;
}

const ATOM_Matrix4x4f *getMatrix44ArrayParameter (ATOM_CoreMaterial::ParamHandle param)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_Matrix44Array:
			return ((ATOM_MaterialMatrix44ArrayParam*)param)->getValue ();
		}
	}
	return 0;
}

bool setMatrix43Parameter (ATOM_CoreMaterial::ParamHandle param, const ATOM_Matrix3x4f &value)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_Matrix43:
			((ATOM_MaterialMatrix43Param*)param)->setValue (value);
			return true;
		}
	}
	return false;
}

ATOM_Matrix3x4f getMatrix43Parameter (ATOM_CoreMaterial::ParamHandle param)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_Matrix43:
			return ((ATOM_MaterialMatrix43Param*)param)->getValue ();
		}
	}
	return ATOM_Matrix3x4f::getIdentityMatrix ();
}

bool setMatrix43ArrayParameter (ATOM_CoreMaterial::ParamHandle param, const ATOM_Matrix3x4f *values, unsigned count)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_Matrix43Array:
			((ATOM_MaterialMatrix43ArrayParam*)param)->setValue (values, count);
			return true;
		}
	}
	return false;
}

const ATOM_Matrix3x4f *getMatrix43ArrayParameter (ATOM_CoreMaterial::ParamHandle param)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_Matrix43Array:
			return ((ATOM_MaterialMatrix43ArrayParam*)param)->getValue ();
		}
	}
	return 0;
}

bool setTextureParameter (ATOM_CoreMaterial::ParamHandle param, ATOM_Texture *texture)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_Texture:
			((ATOM_MaterialTextureParam*)param)->setValue (texture);
			return true;
		}
	}
	return false;
}

ATOM_Texture *getTextureParameter (ATOM_CoreMaterial::ParamHandle param)
{
	if (param)
	{
		switch (((ATOM_MaterialParam*)param)->getParamType())
		{
		case ATOM_MaterialParam::ParamType_Texture:
			return ((ATOM_MaterialTextureParam*)param)->getValue ();
		}
	}
	return 0;
}

bool ATOM_CoreMaterial::setFloat (ATOM_CoreMaterial::ParamHandle param, float value)
{
	return setFloatParameter (param, value);
}

float ATOM_CoreMaterial::getFloat (ATOM_CoreMaterial::ParamHandle param) const
{
	return getFloatParameter (param);
}

bool ATOM_CoreMaterial::setFloatArray (ATOM_CoreMaterial::ParamHandle param, const float *values, unsigned count)
{
	return setFloatArrayParameter (param, values, count);
}

const float *ATOM_CoreMaterial::getFloatArray (ATOM_CoreMaterial::ParamHandle param) const
{
	return getFloatArrayParameter (param);
}

bool ATOM_CoreMaterial::setInt (ATOM_CoreMaterial::ParamHandle param, int value)
{
	return setIntParameter (param, value);
}

int ATOM_CoreMaterial::getInt (ATOM_CoreMaterial::ParamHandle param) const
{
	return getIntParameter (param);
}

bool ATOM_CoreMaterial::setIntArray (ATOM_CoreMaterial::ParamHandle param, const int *values, unsigned count)
{
	return setIntArrayParameter (param, values, count);
}

const int *ATOM_CoreMaterial::getIntArray (ATOM_CoreMaterial::ParamHandle param) const
{
	return getIntArrayParameter (param);
}

bool ATOM_CoreMaterial::setVector (ATOM_CoreMaterial::ParamHandle param, const ATOM_Vector4f &value)
{
	return setVectorParameter (param, value);
}

ATOM_Vector4f ATOM_CoreMaterial::getVector (ATOM_CoreMaterial::ParamHandle param) const
{
	return getVectorParameter (param);
}

bool ATOM_CoreMaterial::setVectorArray (ATOM_CoreMaterial::ParamHandle param, const ATOM_Vector4f *values, unsigned count)
{
	return setVectorArrayParameter (param, values, count);
}

const ATOM_Vector4f *ATOM_CoreMaterial::getVectorArray (ATOM_CoreMaterial::ParamHandle param) const
{
	return getVectorArrayParameter (param);
}

bool ATOM_CoreMaterial::setMatrix44 (ATOM_CoreMaterial::ParamHandle param, const ATOM_Matrix4x4f &value)
{
	return setMatrix44Parameter (param, value);
}

ATOM_Matrix4x4f ATOM_CoreMaterial::getMatrix44 (ATOM_CoreMaterial::ParamHandle param) const
{
	return getMatrix44Parameter (param);
}

bool ATOM_CoreMaterial::setMatrix44Array (ATOM_CoreMaterial::ParamHandle param, const ATOM_Matrix4x4f *values, unsigned count)
{
	return setMatrix44ArrayParameter (param, values, count);
}

const ATOM_Matrix4x4f *ATOM_CoreMaterial::getMatrix44Array (ATOM_CoreMaterial::ParamHandle param) const
{
	return getMatrix44ArrayParameter (param);
}

bool ATOM_CoreMaterial::setMatrix43 (ATOM_CoreMaterial::ParamHandle param, const ATOM_Matrix3x4f &value)
{
	return setMatrix43Parameter (param, value);
}

ATOM_Matrix3x4f ATOM_CoreMaterial::getMatrix43 (ATOM_CoreMaterial::ParamHandle param) const
{
	return getMatrix43Parameter (param);
}

bool ATOM_CoreMaterial::setMatrix43Array (ATOM_CoreMaterial::ParamHandle param, const ATOM_Matrix3x4f *values, unsigned count)
{
	return setMatrix43ArrayParameter (param, values, count);
}

const ATOM_Matrix3x4f *ATOM_CoreMaterial::getMatrix43Array (ATOM_CoreMaterial::ParamHandle param) const
{
	return getMatrix43ArrayParameter (param);
}

bool ATOM_CoreMaterial::setTexture (ATOM_CoreMaterial::ParamHandle param, ATOM_Texture *texture)
{
	return setTextureParameter (param, texture);
}

ATOM_Texture *ATOM_CoreMaterial::getTexture (ATOM_CoreMaterial::ParamHandle param) const
{
	return getTextureParameter (param);
}

bool ATOM_CoreMaterial::setFloat (const char *name, float value)
{
	return setFloat (getParameterHandle (name), value);
}

float ATOM_CoreMaterial::getFloat (const char *name) const
{
	return getFloat (getParameterHandle (name));
}

bool ATOM_CoreMaterial::setFloatArray (const char *name, const float *values, unsigned count)
{
	return setFloatArray (getParameterHandle (name), values, count);
}

const float *ATOM_CoreMaterial::getFloatArray (const char *name) const
{
	return getFloatArray (getParameterHandle (name));
}

bool ATOM_CoreMaterial::setInt (const char *name, int value)
{
	return setInt (getParameterHandle (name), value);
}

int ATOM_CoreMaterial::getInt (const char *name) const
{
	return getInt (getParameterHandle (name));
}

bool ATOM_CoreMaterial::setIntArray (const char *name, const int *values, unsigned count)
{
	return setIntArray (getParameterHandle (name), values, count);
}

const int *ATOM_CoreMaterial::getIntArray (const char *name) const
{
	return getIntArray (getParameterHandle (name));
}

bool ATOM_CoreMaterial::setVector (const char *name, const ATOM_Vector4f &value)
{
	return setVector (getParameterHandle (name), value);
}

ATOM_Vector4f ATOM_CoreMaterial::getVector (const char *name) const
{
	return getVector (getParameterHandle (name));
}

bool ATOM_CoreMaterial::setVectorArray (const char *name, const ATOM_Vector4f *values, unsigned count)
{
	return setVectorArray (getParameterHandle (name), values, count);
}

const ATOM_Vector4f *ATOM_CoreMaterial::getVectorArray (const char *name) const
{
	return getVectorArray (getParameterHandle (name));
}

bool ATOM_CoreMaterial::setMatrix44 (const char *name, const ATOM_Matrix4x4f &value)
{
	return setMatrix44 (getParameterHandle (name), value);
}

ATOM_Matrix4x4f ATOM_CoreMaterial::getMatrix44 (const char *name) const
{
	return getMatrix44 (getParameterHandle (name));
}

bool ATOM_CoreMaterial::setMatrix44Array (const char *name, const ATOM_Matrix4x4f *values, unsigned count)
{
	return setMatrix44Array (getParameterHandle (name), values, count);
}

const ATOM_Matrix4x4f *ATOM_CoreMaterial::getMatrix44Array (const char *name) const
{
	return getMatrix44Array (getParameterHandle (name));
}

bool ATOM_CoreMaterial::setTexture (const char *name, ATOM_Texture *texture)
{
	return setTexture (getParameterHandle (name), texture);
}

ATOM_Texture *ATOM_CoreMaterial::getTexture (const char *name) const
{
	return getTexture (getParameterHandle (name));
}

void ATOM_CoreMaterial::initParameterTable (ATOM_ParameterTable *paramTable)
{
	if (paramTable)
	{
		paramTable->clear ();

		paramTable->_shaderConstantMirrorVS.resize (256);
		memset (&paramTable->_shaderConstantMirrorVS[0], 0, sizeof(float)*4*256);

		paramTable->_shaderConstantMirrorPS.resize (256);
		memset (&paramTable->_shaderConstantMirrorPS[0], 0, sizeof(float)*4*256);

		for (ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_MaterialParam)>::const_iterator it = _params.begin(); it != _params.end(); ++it)
		{
			paramTable->addParameter (it->first.c_str(), it->second.get());
		}
	}
}

ATOM_ParameterTable *ATOM_CoreMaterial::getDefaultParameterTable (void) const
{
	return _defParams.get();
}

//--- wangjian added ---//
static int register_integer = 0;
static int register_float = 0;
static int register_bool = 0;
static int register_sampler = 0;
//----------------------//
static ATOM_STRING generateParamDecl (const char *name, ATOM_MaterialParam *param, ParamArrayInfoMap *arrayInfoMap)
{
	char buffer[1024];
	int registerIndex = param->getRegisterIndex();

	switch (param->getParamType ())
	{
	case ATOM_MaterialParam::ParamType_Float:
		if (registerIndex >= 0)
		{
			sprintf (buffer, "float %s : register(c%d);", name, registerIndex);
		}
		else
		{
			//--- wangjian modified ---//
			sprintf (buffer, "float %s;", name);
			/*sprintf (buffer, "float %s : register(c%d);", name, register_float );
			register_float++;*/
			//-------------------------//

		}
		break;
	case ATOM_MaterialParam::ParamType_FloatArray:
		{
			ATOM_HASHMAP<ATOM_STRING, ParamArrayInfo*>::const_iterator it = arrayInfoMap->arrayInfoMap.find (name);
			if (it == arrayInfoMap->arrayInfoMap.end ()) return "";
			if (registerIndex >= 0)
			{
				sprintf (buffer, "float %s[%s] : register(c%d);", name, it->second->arraySizeStr.c_str(), registerIndex);
			}
			else
			{
				//--- wangjian modified ---//
				sprintf (buffer, "float %s[%s];", name, it->second->arraySizeStr.c_str());
				//sprintf (buffer, "float %s[%s] : register(c%d);", name, it->second->arraySizeStr.c_str(), register_float );
				//register_float+= atoi(it->second->arraySizeStr.c_str());
				//-------------------------//
			}
			break;
		}
	case ATOM_MaterialParam::ParamType_Int:
		//--- wangjian modified ---//
		//sprintf (buffer, "int %s;", name);
		sprintf (buffer, "int %s : register(i%d);", name, register_integer );
		register_integer++;
		//-------------------------//
		break;
	case ATOM_MaterialParam::ParamType_IntArray:
		{
			ATOM_HASHMAP<ATOM_STRING, ParamArrayInfo*>::const_iterator it = arrayInfoMap->arrayInfoMap.find (name);
			if (it == arrayInfoMap->arrayInfoMap.end ()) return "";
			if (registerIndex >= 0)
			{
				sprintf (buffer, "int %s[%s] : register(c%d);", name, it->second->arraySizeStr.c_str(), registerIndex);
			}
			else
			{
				//--- wangjian modified ---//
				sprintf (buffer, "int %s[%s];", name, it->second->arraySizeStr.c_str());
				//sprintf (buffer, "int %s[%s] : register(i%d);", name, it->second->arraySizeStr.c_str(), register_integer);
				//register_integer += atoi(it->second->arraySizeStr.c_str());
				//-------------------------//
			}
			break;
		}
	case ATOM_MaterialParam::ParamType_Matrix44:
		if (registerIndex >= 0)
		{
			sprintf (buffer, "float4x4 %s : register(c%d);", name, registerIndex);
		}
		else
		{
			//--- wangjian modified ---//
			sprintf (buffer, "float4x4 %s;", name);
			//sprintf (buffer, "float4x4 %s : register(c%d);", name, register_float);
			//register_float += 4;
			//-------------------------//
		}
		break;
	case ATOM_MaterialParam::ParamType_Matrix44Array:
		{
			ATOM_HASHMAP<ATOM_STRING, ParamArrayInfo*>::const_iterator it = arrayInfoMap->arrayInfoMap.find (name);
			if (it == arrayInfoMap->arrayInfoMap.end ()) return "";
			if (registerIndex >= 0)
			{
				sprintf (buffer, "float4x4 %s[%s] : register(c%d);", name, it->second->arraySizeStr.c_str(), registerIndex);
			}
			else
			{
				//--- wangjian modified ---//
				sprintf (buffer, "float4x4 %s[%s];", name, it->second->arraySizeStr.c_str());
				//sprintf (buffer, "float4x4 %s[%s] : register(c%d);", name, it->second->arraySizeStr.c_str(), register_float);
				//register_float += atoi(it->second->arraySizeStr.c_str())*4;
				//-------------------------//
			}
			break;
		}
	case ATOM_MaterialParam::ParamType_Matrix43:
		if (registerIndex >= 0)
		{
			sprintf (buffer, "float4x3 %s : register(c%d);", name, registerIndex);
		}
		else
		{
			//--- wangjian modified ---//
			sprintf (buffer, "float4x3 %s;", name);
			//sprintf (buffer, "float4x3 %s : register(c%d);", name, register_float);
			//register_float += 3;
			//-------------------------//
		}
		break;
	case ATOM_MaterialParam::ParamType_Matrix43Array:
		{
			ATOM_HASHMAP<ATOM_STRING, ParamArrayInfo*>::const_iterator it = arrayInfoMap->arrayInfoMap.find (name);
			if (it == arrayInfoMap->arrayInfoMap.end ()) return "";
			if (registerIndex >= 0)
			{
				sprintf (buffer, "float4x3 %s[%s] : register(c%d);", name, it->second->arraySizeStr.c_str(), registerIndex);
			}
			else
			{
				//--- wangjian modified ---//
				sprintf (buffer, "float4x3 %s[%s];", name, it->second->arraySizeStr.c_str());
				//sprintf (buffer, "float4x3 %s[%s] : register(c%d);", name, it->second->arraySizeStr.c_str(), register_float);
				//register_float += atoi(it->second->arraySizeStr.c_str())*3;
				//-------------------------//
			}
			break;
		}
	case ATOM_MaterialParam::ParamType_Vector:
		if (registerIndex >= 0)
		{
			sprintf (buffer, "float4 %s : register(c%d);", name, registerIndex);
		}
		else
		{
			//--- wangjian modified ---//
			sprintf (buffer, "float4 %s;", name);
			//sprintf (buffer, "float4 %s : register(c%d);", name, register_float);
			//register_float++;
			//-------------------------//
		}
		break;
	case ATOM_MaterialParam::ParamType_VectorArray:
		{
			ATOM_HASHMAP<ATOM_STRING, ParamArrayInfo*>::const_iterator it = arrayInfoMap->arrayInfoMap.find (name);
			if (it == arrayInfoMap->arrayInfoMap.end ()) return "";
			if (registerIndex >= 0)
			{
				sprintf (buffer, "float4 %s[%s] : register(c%d);", name, it->second->arraySizeStr.c_str(), registerIndex);
			}
			else
			{
				//--- wangjian modified ---//
				sprintf (buffer, "float4 %s[%s];", name, it->second->arraySizeStr.c_str());
				//sprintf (buffer, "float4 %s[%s] : register(c%d);", name, it->second->arraySizeStr.c_str(), register_float);
				//register_float += atoi(it->second->arraySizeStr.c_str());
				//-------------------------//
			}
			break;
		}
	case ATOM_MaterialParam::ParamType_Sampler:
		//--- wangjian modified ---//
		sprintf (buffer, "sampler %s;", name);
		//sprintf (buffer, "sampler %s : register(s%d);", name,register_sampler);
		//register_sampler++;
		//-------------------------//
		break;
	default:
		buffer[0] = '\0';
		break;
	}

	return buffer;
}

static bool isIdentifierChar (char ch)
{
	if (ch >= '0' && ch <='9')
		return true;

	if (ch >= 'a' && ch <= 'z')
		return true;

	if (ch >= 'A' && ch <= 'Z')
		return true;

	if (ch == '_')
		return true;

	return false;
}

ATOM_STRING ATOM_MaterialEffect::prepareShaderSource (const char *shaderCode, ParamArrayInfoMap *arrayInfoMap) const
{
	
	ATOM_STRING result;

	//--- wangjian added ---//
	/*register_integer = 0;
	register_float = 0;
	register_bool = 0;
	register_sampler = 0;*/
	//----------------------//

	for (ATOM_HASHMAP<ATOM_STRING, ATOM_STRING>::const_iterator it = _material->_definitions.begin(); it != _material->_definitions.end(); ++it)
	{
		result += "#define ";
		result += it->first;
		if (!it->second.empty ())
		{
			result += ' ';
			result += it->second;
		}
		result += '\n';
	}

	for (ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_MaterialParam)>::const_iterator it = _material->_params.begin(); it != _material->_params.end(); ++it)
	{
		const char *paramName = it->first.c_str();

		const char *exist = shaderCode;

		for (;;)
		{
			exist = strstr (exist, paramName);
			if (exist)
			{
				if (exist > shaderCode && isIdentifierChar (*(exist-1)))
				{
					exist++;
					continue;
				}

				if (isIdentifierChar (*(exist+strlen(paramName))))
				{
					exist++;
					continue;
				}
			}
			break;
		}

		if (!exist)
		{
			continue;
		}

		ATOM_STRING decl = generateParamDecl (paramName, it->second.get(), arrayInfoMap);
		result += decl;
		result += '\n';
	}

	result += shaderCode;

	return result;
}

unsigned ATOM_MaterialEffect::begin (ATOM_RenderDevice *device, ATOM_ParameterTable *paramTable)
{
	unsigned numPasses = _passes.size();
	if (numPasses > 0)
	{
#if 1
		for (int i = 0; i < _paramInfo.size(); ++i)
		{
			ParamCallbackInfo *info = &_paramInfo[i];
			ATOM_EffectParameterValue *value = paramTable->getParameter(info->param->getParamIndex());
			paramTable->evalValue (value);

			if (info->info->callback)
			{
				info->info->callback(value, info->info->context, info->info->context2);
			}
		}
#else
		for (ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_MaterialParam)>::const_iterator it = _material->_params.begin(); it != _material->_params.end(); ++it)
		{
			it->second->apply (this);
		}
#endif
	}
	//----------------------------------------------------//
	return numPasses;
}

void ATOM_MaterialEffect::end (ATOM_RenderDevice *device)
{
	device->loadDefaultRenderStates();
}

bool ATOM_MaterialEffect::beginPass (ATOM_RenderDevice *device, ATOM_ParameterTable *paramTable, unsigned pass)
{
	if (pass < _passes.size())
	{
		device->getStateCache()->setCurrentPass (pass);
		return _passes[pass]->begin (device, paramTable);
	}

	return false;
}

void ATOM_MaterialEffect::endPass (ATOM_RenderDevice *device, unsigned pass)
{
	if (pass < _passes.size())
	{
		_passes[pass]->end ();
	}
}

ATOM_CoreMaterial::ATOM_CoreMaterial (void)
{
	_autoSelectEffect = false;

	//--- wangjian added ---//
	// ������effect���
	_shaderBin = 0;
	_readBin = false;
	_shaderCode = "";
	//validationBitIndex = 0;
	//----------------------//
}

ATOM_CoreMaterial::~ATOM_CoreMaterial (void)
{
	//--- wangjian added ---//
	closeShaderBinFile();
	_renderSchemeLayerSupports.clear();
	//----------------------//
}

bool ATOM_CoreMaterial::isAutoSelectEffect (void) const
{
	return _autoSelectEffect;
}

//--- wangjian added ---//
void ATOM_CoreMaterial::setAutoSelectEffect(bool bAuto)
{
	_autoSelectEffect = bAuto;
}
//void ATOM_CoreMaterial::appendLayerValidationBit(int layer,unsigned bitIndex)
//{
//	ATOM_MAP<int,unsigned>::iterator iter = layerValidationBitMask.find(layer);
//	if( iter == layerValidationBitMask.end() )
//	{
//		layerValidationBitMask.insert(std::make_pair(layer,1<<bitIndex));
//	}
//	else
//		iter->second |= 1 << bitIndex;
//}
//void ATOM_CoreMaterial::appendSchemeLayerEffect( ATOM_RenderSchemeFactory * scheme, int layer, unsigned effectid, ATOM_MaterialEffect * effect )
//{
//	unsigned schemelayerkey = ( (unsigned)scheme << 16 ) + layer < 0 ? 1000 : layer; 
//	ATOM_MAP<unsigned,sSchemeLayerEffects>::iterator iter = schemeLayerEffects.find(schemelayerkey);
//	if( iter == schemeLayerEffects.end() )
//	{
//		sSchemeLayerEffects sle;
//		sle.effects.insert( std::make_pair(effectid, effect) );
//		schemeLayerEffects.insert( std::make_pair(schemelayerkey, sle) );
//	}
//	else
//	{
//		sSchemeLayerEffects & sle = iter->second;
//		ATOM_MAP<unsigned,ATOM_MaterialEffect*>::iterator iter = sle.effects.find(effectid);
//		if( iter == sle.effects.end() )
//		{
//			sle.effects.insert( std::make_pair(effectid, effect) );
//		}
//	}
//}
//----------------------//

bool ATOM_CoreMaterial::supportRenderSchemeLayer (ATOM_RenderSchemeFactory *factory, int layer)
{
	for (unsigned i = 0; i < _effects.size(); ++i)
	{
		if (_effects[i]->_schemeFactory == factory && _effects[i]->_renderSchemeLayerNumber == layer)
		{
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// wangjian added 
// ��addEffectʱ��effect��renderscheme�Լ������ڵ�layerid���浽_RenderSchemeLayerSupports��ȥ
void ATOM_CoreMaterial::saveEffectRenderSchemeLayer(ATOM_MaterialEffect* effect)
{
	ATOM_RenderSchemeFactory* factory = effect->_schemeFactory;
	if( !factory )
		return;

	RENDERSCHEMELAYERS::iterator iter = _renderSchemeLayerSupports.find(factory);
	if( iter == _renderSchemeLayerSupports.end() )
	{
		sRenderSchemeLayers renderschemelayers;
		int layerId = effect->getRenderSchemeLayerNumber();
		renderschemelayers._layers.push_back(layerId);

		sRenderSchemeLayerEffects layerEffects;
		layerEffects._effects.push_back(_effects.size()-1);
		renderschemelayers._layerEffects.insert(std::make_pair(layerId,layerEffects));

		_renderSchemeLayerSupports.insert(std::make_pair(factory,renderschemelayers));
	}
	else
	{
		ATOM_DEQUE<int> & layers = iter->second._layers;
		for(int i = 0; i < layers.size(); ++i )
		{
			if( layers[i] == effect->getRenderSchemeLayerNumber() )
			{
				ATOM_MAP<int,sRenderSchemeLayerEffects>::iterator iterLayerEffects = iter->second._layerEffects.find(layers[i]);
				if( iterLayerEffects != iter->second._layerEffects.end() )
					iterLayerEffects->second._effects.push_back(_effects.size()-1);

				return;
			}
		}

		int layerId = effect->getRenderSchemeLayerNumber();
		layers.push_back(layerId);

		sRenderSchemeLayerEffects layerEffects;
		layerEffects._effects.push_back(_effects.size()-1);
		iter->second._layerEffects.insert(std::make_pair(layerId,layerEffects));
	}

	//����layer������
	int layerId = effect->getRenderSchemeLayerNumber();
	if( layerId >= 0 )
	{
		ATOM_STRING layer_name = factory->getLayerName(layerId);
		if( !layer_name.empty() )
			_renderLayerNames.insert(layer_name);
	}
}
// �õ���CoreMaterial֧�ֵ�scheme��Ӧ��layer����
ATOM_DEQUE<int> *  ATOM_CoreMaterial::getRenderSchemeLayers(ATOM_RenderScheme* renderScheme)
{
	//ATOM_ASSERT(renderScheme);
	if( renderScheme )
	{
		ATOM_RenderSchemeFactory * factory = renderScheme->getFactory();

		// ���Ҹ�scheme
		RENDERSCHEMELAYERS::iterator iter = _renderSchemeLayerSupports.find(factory);
		if( iter != _renderSchemeLayerSupports.end() )
			return &(iter->second._layers);

		//----------------------- ���û���ҵ� ----------------------//

		sRenderSchemeLayers renderschemelayers;
		
		// ѭ����scheme���е�layer
		for( int i = 0; i < factory->getLayerCount(); ++i )
		{
			// �õ���layer������
			ATOM_STRING layer_name = factory->getLayerName(i);

			// �ڸú��Ĳ��ʵ�layer���ּ����в��Ҹ�����
			if( _renderLayerNames.find(layer_name) == _renderLayerNames.end() )
				continue;

			// ����ҵ��˶�Ӧ��layer, �����layer��ID
			renderschemelayers._layers.push_back(i);

			// ѭ���ú��Ĳ���֧�ֵ����е�scheme
			RENDERSCHEMELAYERS::iterator iter_scheme = _renderSchemeLayerSupports.begin();
			for( ;iter_scheme != _renderSchemeLayerSupports.end(); ++iter_scheme )
			{
				// �õ���scheme��֧�ֵ�layer
				sRenderSchemeLayers & rsl = iter_scheme->second;

				// ѭ�����е�layer
				for( int layer = 0; layer < rsl._layers.size(); ++layer )
				{
					// �õ���layer������
					int layerId = rsl._layers[layer];
					ATOM_STRING layerName = iter_scheme->first->getLayerName(layerId);

					// �������ƥ�䣬�ҵ���
					if( layerName == layer_name )
					{
						// ����effectID����
						sRenderSchemeLayerEffects layerEffects = rsl._layerEffects.find(layerId)->second;

						// �����Ӧ��layer��effectID����
						renderschemelayers._layerEffects.insert(std::make_pair(i,layerEffects));

						// �˳�ѭ��
						break;
					}
				}

				// �˳�ѭ��
				break;
			}
		}

		// ��Ӹ�scheme
		_renderSchemeLayerSupports.insert( std::make_pair( factory, renderschemelayers ) );

		// �����¼ӵ�
		return &(_renderSchemeLayerSupports[factory]._layers);
	}

	return 0;
}
// �õ���CoreMaterial֧�ֵ�scheme��layer�е�effect id����
ATOM_DEQUE<int> * ATOM_CoreMaterial::getRenderSchemeLayerEffects(ATOM_RenderScheme* renderScheme,int layer)
{
	//ATOM_ASSERT(renderScheme);

	if( renderScheme )
	{
		RENDERSCHEMELAYERS::iterator iter = _renderSchemeLayerSupports.find(renderScheme->getFactory());
		if( iter != _renderSchemeLayerSupports.end() )
		{
			ATOM_MAP<int,sRenderSchemeLayerEffects>::iterator iterEffects = iter->second._layerEffects.find(layer);
			if( iterEffects != iter->second._layerEffects.end() )
				return &(iterEffects->second._effects);
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////




ATOM_MaterialEffect *ATOM_CoreMaterial::autoSelectEffect (ATOM_RenderScheme *currentRenderScheme, int currentSchemeLayer)
{
	for (unsigned i = 0; i < _effects.size(); ++i)
	{
		if (_effects[i]->validate (currentRenderScheme, currentSchemeLayer))
		{
			return _effects[i].get();
		}
	}

	return 0;
}

bool ATOM_CoreMaterial::loadXML (ATOM_RenderDevice *device, const char *filename)
{
	load_time = 0;
	createvsps_time = 0;

	unsigned load_start = ATOM_GetTick();

	if( !filename || '\0' == filename[0] )
		return false;

	//--- wangjian added ---//
	// ������effect���
	// ����������ļ����ڣ����ض������ļ�
	openShaderBinFile(filename);
	// ����������ļ�Ϊ�գ���ʾ�����޷�����
	if( !_shaderBin )
		return false;
	//----------------------//

	// read XML file
	ATOM_AutoFile f(filename, ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
	{
		ATOM_LOGGER::error ("%s(0x%08X,%s): Open file failed.\n", __FUNCTION__, device, filename);

		//--- wangjian added ---//
		// ������effect���
		_readBin = false;
		closeShaderBinFile(true);
		//----------------------//

		return false;
	}
	ATOM_VECTOR<char> file_content(f->size()+1);
	char *str = &file_content[0];
	unsigned n = f->read (str, f->size());
	str[n] = '\0';

	if (!loadString (device, str))
	{
		ATOM_LOGGER::error ("%s(0x%08X,%s): Load material failed.\n", __FUNCTION__, device, filename);

		//--- wangjian added ---//
		_readBin = false;
		closeShaderBinFile(true);
		//----------------------//

		return false;
	}

	//--- wangjian added ---//
	// ������effect���
	_readBin = false;
	closeShaderBinFile();
	//----------------------//

	load_time = ATOM_GetTick() - load_start;

	/*ATOM_LOGGER::debug("<%s>: load %s total time : %u(ms), vs/ps create time: %u(ms). the percent is : %f\n", 
	__FUNCTION__, 
	filename, 
	load_time, createvsps_time, 
	(float)createvsps_time/(float)load_time);*/

	return true;
}

ATOM_CoreMaterial::Include::~Include (void)
{
	ATOM_DELETE (doc);

	for (unsigned i = 0; i < children.size(); ++i)
	{
		ATOM_DELETE (children[i]);
	}
	children.clear ();
}

bool ATOM_CoreMaterial::loadIncludes_r (ATOM_CoreMaterial::Include *includes, ATOM_TiXmlDocument *doc)
{
	ATOM_TiXmlElement *root = doc->RootElement();
	if (!root || stricmp(root->Value(), "material"))
	{
		// No root element named "material" found
		ATOM_LOGGER::error ("%s: Docuemnt has no root element named 'material'.\n", __FUNCTION__);
		return false;
	}
	includes->doc = doc;

	ATOM_TiXmlElement *includeElement = root->FirstChildElement ("include");
	while (includeElement)
	{
		const char *filename = includeElement->Attribute ("filename");
		if (filename)
		{
			ATOM_AutoFile f(filename, ATOM_VFS::read|ATOM_VFS::text);
			if (!f)
			{
				ATOM_LOGGER::error ("%s: included file <%s> not found.\n", __FUNCTION__, filename);
				return false;
			}

			//=================== wangjian added ======================//
			// ����Ƕ�ȡ������SHADER�ļ�
			if( _readBin && _shaderBin )
			{
				// �������shader�������ļ����� �Ƚ�include�ļ��Ͷ������ļ���ʱ��
				if( ATOM_RenderSettings::isShaderBinaryCompileEnabled() )
				{
					// ���Ȼ�ȡ���ļ����޸�ʱ��
					char nativepath_bin[ATOM_VFS::max_filename_length] = {0};
					char nativepath_inc[ATOM_VFS::max_filename_length] = {0};
					ATOM_STRING file_bin = _shaderBin->getName();
					ATOM_VFS * vfs_bin = ATOM_GetNativePathName(_shaderBin->getName(),nativepath_bin);
					ATOM_VFS * vfs_inc = ATOM_GetNativePathName(filename,nativepath_inc);
					ATOM_ASSERT(vfs_bin==vfs_inc);
					WIN32_FIND_DATA find_data_bin;
					WIN32_FIND_DATA find_data_inc;
					::FindFirstFileA(nativepath_bin,&find_data_bin);
					::FindFirstFileA(nativepath_inc,&find_data_inc);
					ULONGLONG qw_bin;
					ULONGLONG qw_inc;
					qw_bin = (((ULONGLONG) find_data_bin.ftLastWriteTime.dwHighDateTime) << 32) + find_data_bin.ftLastWriteTime.dwLowDateTime;
					qw_inc = (((ULONGLONG) find_data_inc.ftLastWriteTime.dwHighDateTime) << 32) + find_data_inc.ftLastWriteTime.dwLowDateTime;

					// ���INCLUDE�ļ��޸�ʱ�� ��MAT�ļ��޸�ʱ��֮��
					if( qw_inc > qw_bin )
					{
						ATOM_CloseFile( _shaderBin );
						_shaderBin = 0;
						vfs_bin->removeFile(nativepath_bin,true);
						_shaderBin = ATOM_OpenFile(file_bin.c_str(), ATOM_VFS::write|ATOM_VFS::binary);
						if( _shaderBin )
							_readBin = false;
					}
				}
			}

			////////////////////////////////////////////////////////////

			ATOM_VECTOR<char> file_content(f->size()+1);
			char *str = &file_content[0];
			unsigned n = f->read (str, f->size());
			str[n] = '\0';

			ATOM_TiXmlDocument *childDoc = ATOM_NEW(ATOM_TiXmlDocument);
			childDoc->Parse (str);
			if (childDoc->Error())
			{
				ATOM_LOGGER::error ("%s: included file <%s> is not a valid XML file.\n", __FUNCTION__, filename);
				ATOM_DELETE(childDoc);
				return false;
			}

			Include *include = ATOM_NEW(Include);
			if (!loadIncludes_r (include, childDoc))
			{
				ATOM_DELETE(include);
				ATOM_DELETE(childDoc);
				return false;
			}

			includes->children.push_back (include);
			//if (!parseXML (device, root))
			//{
			//	return false;
			//}
		}
		includeElement = includeElement->NextSiblingElement ("include");
	}

	return true;
}
bool ATOM_CoreMaterial::parseXML (ATOM_RenderDevice *device, ATOM_TiXmlElement *element, ParamArrayInfoMap *arrayInfoMap)
{
	// parse parameters and shader code
	if (!parseParamAndShaderCode (device, element, _shaderCode, arrayInfoMap))
	{
		return false;
	}

	//--- wangjian added ---//
	//_shaderCode = prepareShaderSource (arrayInfoMap);
	//----------------------//

	// parse effects
	ATOM_TiXmlElement *effectElement = element->FirstChildElement ("effect");
	while (effectElement)
	{
		if (!parseEffectElement (device, effectElement, _shaderCode.c_str(), arrayInfoMap))
		{
			// effect parse failed
			return false;
		}
		effectElement = effectElement->NextSiblingElement ("effect");
	}

	return true;
}

ATOM_CoreMaterial::Include *ATOM_CoreMaterial::parseIncludes (ATOM_TiXmlDocument *doc)
{
	ATOM_CoreMaterial::Include *includes = ATOM_NEW(ATOM_CoreMaterial::Include);
	if (!loadIncludes_r (includes, doc))
	{
		ATOM_DELETE(includes);
		return 0;
	}
	return includes;
}

bool ATOM_CoreMaterial::parse_r (ATOM_RenderDevice *device, Include *includes, ParamArrayInfoMap *arrayInfoMap)
{
	for (unsigned i = 0; i < includes->children.size(); ++i)
	{
		if (!parse_r(device, includes->children[i], arrayInfoMap))
		{
			return false;
		}
	}

	if (!parseXML (device, includes->doc->RootElement(), arrayInfoMap))
	{
		return false;
	}

	return true;
}
//--- wangjian modified ---//
bool ATOM_CoreMaterial::preprocess_r (Include *includes)
//bool ATOM_CoreMaterial::preprocess_r (ATOM_RenderDevice *device,Include *includes)
//-------------------------//
{
	for (unsigned i = 0; i < includes->children.size(); ++i)
	{
		if (!preprocess_r (/*device,*/includes->children[i]))
		{
			return false;
		}
	}

	if (!parseDefines (/*device,*/includes->doc->RootElement()))
	{
		return false;
	}

	return true;
}

bool ATOM_CoreMaterial::loadXMLElement (ATOM_RenderDevice *device, ATOM_TiXmlDocument *doc)
{
	if (!doc)
	{
		return false;
	}

	Include *includes = parseIncludes (doc);
	if (!includes)
	{
		ATOM_LOGGER::error ("%s: Parse included materails failed.\n", __FUNCTION__);
		ATOM_DELETE(doc);
		return false;
	}

	//--- wangjian modified ---//
	if (!preprocess_r (/*device,*/includes))
	//-------------------------//
	{
		ATOM_LOGGER::error ("%s: Preprocess failed.\n", __FUNCTION__);
		ATOM_DELETE(includes);
		return false;
	}

	ParamArrayInfoMap arrayInfoMap;

	if (!parse_r (device, includes, &arrayInfoMap))
	{
		ATOM_DELETE(includes);
		return false;
	}

	ATOM_DELETE(includes);

	_defParams = ATOM_NEW(ATOM_ParameterTable, this);

	return true;
}

bool ATOM_CoreMaterial::loadString (ATOM_RenderDevice *device, const char *str)
{
	// parse XML file
	ATOM_TiXmlDocument *doc = ATOM_NEW(ATOM_TiXmlDocument);
	doc->Parse (str);
	if (doc->Error())
	{
		ATOM_LOGGER::error ("%s: Parse XML failed.\n", __FUNCTION__);
		ATOM_DELETE(doc);
		return false;
	}

	ATOM_TiXmlElement *root = doc->RootElement();
	if (!root || stricmp(root->Value(), "material"))
	{
		// No root element named "material" found
		ATOM_LOGGER::error ("%s: No <material> root element found.\n", __FUNCTION__);
		ATOM_DELETE(doc);
		return false;
	}

	int autoSelectEffect = 1;
	root->QueryIntAttribute ("auto", &autoSelectEffect);
	_autoSelectEffect = autoSelectEffect != 0;

	if (loadXMLElement (device, doc))
	{
		for (int i = 0; i < _effects.size(); ++i)
		{
			_effects[i]->gatherUsedParams();
		}
		return true;
	}

	return false;
}

unsigned ATOM_CoreMaterial::getNumEffects (void) const
{
	return _effects.size();
}

void ATOM_CoreMaterial::addEffect (ATOM_MaterialEffect *effect)
{
	if (effect)
	{
		_effects.push_back (effect);

		//--- wangjian added ---//
		saveEffectRenderSchemeLayer(effect);
		//----------------------//
	}
}

ATOM_MaterialEffect *ATOM_CoreMaterial::getEffect (unsigned index) const
{
	return _effects[index].get();
}

ATOM_MaterialEffect *ATOM_CoreMaterial::getEffectByName (const char *name) const
{
	if (!name)
	{
		return 0;
	}

	for (unsigned i = 0; i < _effects.size(); ++i)
	{
		if (!strcmp (_effects[i]->getName(), name))
		{
			return _effects[i].get();
		}
	}

	return 0;
}

//--- wangjian added ---//
//ATOM_AsyncableLoadMaterial * ATOM_CoreMaterial::getAsyncLoader()
//{
//	if (!_asyncLoader)
//	{
//		_asyncLoader = ATOM_NEW(ATOM_AsyncableLoadMaterial);
//	}
//	return _asyncLoader.get();
//}
// ������effect���
void ATOM_CoreMaterial::openShaderBinFile(const char* filename)
{
	//_readBin = false;
	//return;

	// ����������ļ��Ѿ���
	if( _shaderBin )
		return;

	// ����ò���shader�������Ѿ����ɺ���
	ATOM_STRING binaryFile = filename;
	unsigned int index = binaryFile.find_last_of('.')+1;
	binaryFile.replace(binaryFile.begin()+index,binaryFile.end(),"fxb");
	_shaderBin = ATOM_OpenFile(binaryFile.c_str(), ATOM_VFS::read|ATOM_VFS::binary);

	// ����������ļ������ڣ�д�������ļ�
	if (!_shaderBin)
	{
		//���ɶ������ļ� ׼��д��
		ATOM_LOGGER::log ("%s : %s has no binary shader, so generate it and save.\n", __FUNCTION__, filename);
		_shaderBin = ATOM_OpenFile(binaryFile.c_str(), ATOM_VFS::write|ATOM_VFS::binary);
		if( _shaderBin )
			_readBin = false;
	}
	// ����������ļ�����
	else
	{
		// ���SHADER�������ɶ������ļ�û�п���
		if( false == ATOM_RenderSettings::isShaderBinaryCompileEnabled() )
		{
			// ���������ļ�
			_readBin = true;
		}
		// ���� �Ƚ��ļ�ʱ��
		else
		{
			// ���Ȼ�ȡ�����ļ����޸�ʱ��
			char nativepath_txt[ATOM_VFS::max_filename_length] = {0};
			char nativepath_bin[ATOM_VFS::max_filename_length] = {0};
			ATOM_VFS * vfs_txt = ATOM_GetNativePathName(filename,nativepath_txt);
			ATOM_VFS * vfs_bin = ATOM_GetNativePathName(binaryFile.c_str(),nativepath_bin);
			ATOM_ASSERT(vfs_bin==vfs_txt);
			WIN32_FIND_DATA find_data_txt;
			WIN32_FIND_DATA find_data_bin;
			::FindFirstFileA(nativepath_txt,&find_data_txt);
			::FindFirstFileA(nativepath_bin,&find_data_bin);
			ULONGLONG qw_txt;
			ULONGLONG qw_bin;
			qw_txt = (((ULONGLONG) find_data_txt.ftLastWriteTime.dwHighDateTime) << 32) + find_data_txt.ftLastWriteTime.dwLowDateTime;
			qw_bin = (((ULONGLONG) find_data_bin.ftLastWriteTime.dwHighDateTime) << 32) + find_data_bin.ftLastWriteTime.dwLowDateTime;

			// ���TXT�ļ��޸�ʱ�� �� BIN�ļ��޸�ʱ��֮��
			if( qw_txt > qw_bin )
			{
				//ATOM_LOGGER::log ("%s : binary shader of %s is old than shader text source, so regenerate it and save.\n", __FUNCTION__, filename);

				ATOM_CloseFile( _shaderBin );
				_shaderBin = 0;
				vfs_bin->removeFile(nativepath_bin,true);
				_shaderBin = ATOM_OpenFile(binaryFile.c_str(), ATOM_VFS::write|ATOM_VFS::binary);
				if( _shaderBin )
					_readBin = false;
			}
			else
			{
				_readBin = true;
			}
		}
	}
	//----------------------//
}
void ATOM_CoreMaterial::closeShaderBinFile(bool hasError)
{
	// ����������ļ���
	if(_shaderBin)
	{
		ATOM_STRING name = "";
		ATOM_VFS * vfs = 0;

		if( hasError )
		{
			name = _shaderBin->getName();
			vfs = ATOM_VFS::vfsGetVFS(_shaderBin->getHandle());
		}

		// �رն������ļ�
		ATOM_CloseFile( _shaderBin );
		_shaderBin = 0;

		// ����������� ɾ���������ļ�
		if( hasError && vfs )
			vfs->removeFile(name.c_str(),true);
	}
}
//----------------------//

////////////////////////////////////////////////////////////////////////////////////////////////////////

const ATOM_VECTOR<ATOM_MaterialEffect::RequireEntry> &ATOM_MaterialEffect::getRequireEntries (void) const
{
	return _requireEntries;
}

bool ATOM_CoreMaterial::parseEffectElement (ATOM_RenderDevice *device, ATOM_TiXmlElement *element, const char * shaderCode, ParamArrayInfoMap *arrayInfoMap)
{
	const char *renderScheme = element->Attribute ("scheme");
	const char *schemeLayer = element->Attribute ("schemelayer");
	const char *name = element->Attribute ("name");
	if (!name)
	{
		ATOM_LOGGER::error ("%s: No <name> element in effect declaration.\n", __FUNCTION__);
		return false;
	}
	int layer = ATOM_RenderScheme::getRenderSchemeLayerNumber (renderScheme, schemeLayer);

	ATOM_AUTOPTR(ATOM_MaterialEffect) effect = ATOM_NEW(ATOM_MaterialEffect, name, renderScheme, layer, this);
	ATOM_TiXmlElement *requireElement = element->FirstChildElement ("requirements");
	while (requireElement)
	{
		if (!parseRequire (device, requireElement, effect.get()))
		{
			ATOM_LOGGER::error ("%s: Invalid requirements for effect \"%s\".\n", __FUNCTION__, name);
			return false;
		}
		requireElement = requireElement->NextSiblingElement ("requirements");
	}

	ATOM_TiXmlElement *passElement = element->FirstChildElement ("pass");
	while (passElement)
	{
		if (!parsePass (device, passElement, effect.get(), shaderCode, arrayInfoMap))
		{
			// parse pass failed
			return false;
		}
		passElement = passElement->NextSiblingElement ("pass");
	}

	// check pass
	if (effect->getNumPasses() == 0)
	{
		ATOM_LOGGER::error ("%s: No pass defined for effect \"%s\".\n", __FUNCTION__, effect->getName());
		return false;
	}

	//effect->applySamplerIndices ();
	effect->linkShaderParameters ();

	addEffect (effect.get());

	return true;
}

bool ATOM_CoreMaterial::parseCode (ATOM_RenderDevice *device, ATOM_TiXmlElement *element, ATOM_STRING &code) const
{
	ATOM_TiXmlNode *node = element->FirstChild ();
	if (node)
	{
		const char *codeStr = node->Value ();
		if (codeStr)
		{
			code += codeStr;
		}
	}

	return true;
}

bool ATOM_CoreMaterial::parseRequire (ATOM_RenderDevice *device, ATOM_TiXmlElement *element, ATOM_MaterialEffect *effect) const
{
	struct RequireInfo
	{
		ATOM_MaterialEffect::Compare compare;
		const char *name;
	};

	RequireInfo info[6] = {
		{ATOM_MaterialEffect::EQ, "eq"},
		{ATOM_MaterialEffect::GE, "ge"},
		{ATOM_MaterialEffect::GT, "gt"},
		{ATOM_MaterialEffect::LE, "le"},
		{ATOM_MaterialEffect::LT, "lt"},
		{ATOM_MaterialEffect::NE, "ne"}
	};

	for (unsigned i = 0; i < 6; ++i)
	{
		ATOM_TiXmlElement *e = element->FirstChildElement (info[i].name);
		while (e)
		{
			const char *paramName = e->Attribute("param");
			if (!paramName)
			{
				return false;
			}
			ATOM_MaterialParam *param = getParameter (paramName);
			if (!param)
			{
				return false;
			}
			param->setFlags(param->getFlags()|ATOM_MaterialParam::Flag_ConditionValue);

			const char *compareValue = e->Attribute ("value");
			if (!compareValue)
			{
				return false;
			}

			switch (param->getParamType ())
			{
			case ATOM_MaterialParam::ParamType_Float:
				{
					float val = atof (compareValue);
					effect->_requireEntries.resize (effect->_requireEntries.size()+1);
					effect->_requireEntries.back().param = param;
					effect->_requireEntries.back().context = (void*)((unsigned&)val);
					effect->_requireEntries.back().compare = info[i].compare;
					break;
				}
			case ATOM_MaterialParam::ParamType_Int:
				{
					int val = atoi (compareValue);
					effect->_requireEntries.resize (effect->_requireEntries.size()+1);
					effect->_requireEntries.back().param = param;
					effect->_requireEntries.back().context = (void*)(val);
					effect->_requireEntries.back().compare = info[i].compare;
					break;
				}
			}
			e = e->NextSiblingElement (info[i].name);
		}
	}

	return true; 
}

void ATOM_CoreMaterial::setupArrayParams (ParamArrayInfoMap *arrayInfoMap, ATOM_Shader *shader)
{
	for (ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_MaterialParam)>::const_iterator it = _params.begin(); it != _params.end(); ++it)
	{
		ATOM_MaterialParam *param = it->second.get();
		switch (param->getParamType ())
		{
		case ATOM_MaterialParam::ParamType_FloatArray:
			{
				ATOM_HASHMAP<ATOM_STRING, ParamArrayInfo*>::const_iterator it2 = arrayInfoMap->arrayInfoMap.find (it->first.c_str());
				ATOM_ASSERT(it2 != arrayInfoMap->arrayInfoMap.end());

				ATOM_Shader::ParamHandle handle = shader->getParamHandle (it->first.c_str(), 0);
				if (handle)
				{
					int dimension = shader->getArrayLength (handle);
					if (dimension > 0)
					{
						ATOM_MaterialFloatArrayParam *arrayParam = (ATOM_MaterialFloatArrayParam*)param;
						arrayParam->setDimension (dimension);
						unsigned size = sizeof(float)*dimension;
						if (size > it2->second->size)
						{
							size = it2->second->size;
						}
						if (size > 0)
						{
							memcpy (arrayParam->getValuePtr(), it2->second->arrayValues, size);
						}
					}
				}
				break;
			}
		case ATOM_MaterialParam::ParamType_IntArray:
			{
				ATOM_HASHMAP<ATOM_STRING, ParamArrayInfo*>::const_iterator it2 = arrayInfoMap->arrayInfoMap.find (it->first.c_str());
				ATOM_ASSERT(it2 != arrayInfoMap->arrayInfoMap.end());

				ATOM_Shader::ParamHandle handle = shader->getParamHandle (it->first.c_str(), 0);
				if (handle)
				{
					int dimension = shader->getArrayLength (handle);
					if (dimension > 0)
					{
						ATOM_MaterialIntArrayParam *arrayParam = (ATOM_MaterialIntArrayParam*)param;
						arrayParam->setDimension (dimension);
						unsigned size = sizeof(float)*dimension;
						if (size > it2->second->size)
						{
							size = it2->second->size;
						}
						if (size > 0)
						{
							memcpy (arrayParam->getValuePtr(), it2->second->arrayValues, size);
						}
					}
				}
				break;
			}
		case ATOM_MaterialParam::ParamType_Matrix44Array:
			{
				ATOM_HASHMAP<ATOM_STRING, ParamArrayInfo*>::const_iterator it2 = arrayInfoMap->arrayInfoMap.find (it->first.c_str());
				ATOM_ASSERT(it2 != arrayInfoMap->arrayInfoMap.end());

				ATOM_Shader::ParamHandle handle = shader->getParamHandle (it->first.c_str(), 0);
				if (handle)
				{
					int dimension = shader->getArrayLength (handle);
					if (dimension > 0)
					{
						ATOM_MaterialMatrix44ArrayParam *arrayParam = (ATOM_MaterialMatrix44ArrayParam*)param;
						arrayParam->setDimension (dimension);
						unsigned size = sizeof(float)*16*dimension;
						if (size > it2->second->size)
						{
							size = it2->second->size;
						}
						if (size > 0)
						{
							memcpy (arrayParam->getValuePtr(), it2->second->arrayValues, size);
						}
					}
				}
				break;
			}
		case ATOM_MaterialParam::ParamType_Matrix43Array:
			{
				ATOM_HASHMAP<ATOM_STRING, ParamArrayInfo*>::const_iterator it2 = arrayInfoMap->arrayInfoMap.find (it->first.c_str());
				ATOM_ASSERT(it2 != arrayInfoMap->arrayInfoMap.end());

				ATOM_Shader::ParamHandle handle = shader->getParamHandle (it->first.c_str(), 0);
				if (handle)
				{
					int dimension = shader->getArrayLength (handle);
					if (dimension > 0)
					{
						ATOM_MaterialMatrix43ArrayParam *arrayParam = (ATOM_MaterialMatrix43ArrayParam*)param;
						arrayParam->setDimension (dimension);
						unsigned size = sizeof(float)*12*dimension;
						if (size > it2->second->size)
						{
							size = it2->second->size;
						}
						if (size > 0)
						{
							memcpy (arrayParam->getValuePtr(), it2->second->arrayValues, size);
						}
					}
				}
				break;
			}
		case ATOM_MaterialParam::ParamType_VectorArray:
			{
				ATOM_HASHMAP<ATOM_STRING, ParamArrayInfo*>::const_iterator it2 = arrayInfoMap->arrayInfoMap.find (it->first.c_str());
				ATOM_ASSERT(it2 != arrayInfoMap->arrayInfoMap.end());

				ATOM_Shader::ParamHandle handle = shader->getParamHandle (it->first.c_str(), 0);
				if (handle)
				{
					int dimension = shader->getArrayLength (handle);
					if (dimension > 0)
					{
						ATOM_MaterialVectorArrayParam *arrayParam = (ATOM_MaterialVectorArrayParam*)param;
						arrayParam->setDimension (dimension);
						unsigned size = sizeof(float)*4*dimension;
						if (size > it2->second->size)
						{
							size = it2->second->size;
						}
						if (size > 0)
						{
							memcpy (arrayParam->getValuePtr(), it2->second->arrayValues, size);
						}
					}
				}
				break;
			}
		}
	}
}

bool ATOM_CoreMaterial::parsePass (ATOM_RenderDevice *device, ATOM_TiXmlElement *element, ATOM_MaterialEffect *effect, const char *shaderCode, ParamArrayInfoMap *arrayInfoMap)
{
	ATOM_AUTOPTR(ATOM_MaterialPass) pass = ATOM_NEW(ATOM_MaterialPass, device, _params.size());

	unsigned streamdefine = 0;

	ATOM_TiXmlElement *streamDefineElement = element->FirstChildElement ("streamdefine");
	if (streamDefineElement)
	{
		streamdefine = parseStreamDefine (device, streamDefineElement);
		pass->setStreamDefine (streamdefine);
	}

	ATOM_STRING code = "";
	// if has shader code
	if (*shaderCode != '\0')
	{
		// add uniform parameter definitions to shader code
		code = effect->prepareShaderSource (shaderCode, arrayInfoMap);
	}
	
		if (!parseVertexShader (device, element, pass.get(), code.c_str(), arrayInfoMap))
		{
			// invalid vertex shader
			return false;
		}

		if (!parsePixelShader (device, element, pass.get(), code.c_str(), arrayInfoMap))
		{
			// invalid pixel shader
			return false;
		}

		ATOM_Shader *vertexshader_ = pass->getGPUProgram()->getShader (ATOM_SHADER_TYPE_VERTEXSHADER);
		if (vertexshader_)
		{
			setupArrayParams (arrayInfoMap, vertexshader_);
		}

		ATOM_Shader *pixelshader_ = pass->getGPUProgram()->getShader (ATOM_SHADER_TYPE_PIXELSHADER);
		if (pixelshader_)
		{
			setupArrayParams (arrayInfoMap, pixelshader_);
		}
	//}

	// load alpha blending states
	ATOM_TiXmlElement *alphablendElement = element->FirstChildElement ("alphablend_state");
	if (alphablendElement)
	{
		ATOM_AlphaBlendAttributes *state = pass->getStateSet()->useAlphaBlending(0);
		if (!loadAlphaBlendStates (device, state, alphablendElement, this, pass.get(), effect))
		{
			ATOM_LOGGER::error ("%s: Invalid renderstates declaration in effect \"%s\".\n", __FUNCTION__, effect->getName());
			return false;
		}
	}
	else
	{
		pass->getStateSet()->unuseAlphaBlending ();
	}

	// load color write states
	ATOM_TiXmlElement *colorwriteElement = element->FirstChildElement ("colorwrite_state");
	if (colorwriteElement)
	{
		ATOM_ColorWriteAttributes *state = pass->getStateSet()->useColorWrite (0);
		if (!loadColorWriteStates(device, state, colorwriteElement, this, pass.get(), effect))
		{
			ATOM_LOGGER::error ("%s: Invalid renderstates declaration in effect \"%s\".\n", __FUNCTION__, effect->getName());
			return false;
		}
	}
	else
	{
		pass->getStateSet()->unuseColorWrite ();
	}

	// load depth states
	ATOM_TiXmlElement *depthElement = element->FirstChildElement ("depth_state");
	if (depthElement)
	{
		ATOM_DepthAttributes *state = pass->getStateSet()->useDepth (0);
		if (!loadDepthStates(device, state, depthElement, this, pass.get(), effect))
		{
			ATOM_LOGGER::error ("%s: Invalid renderstates declaration in effect \"%s\".\n", __FUNCTION__, effect->getName());
			return false;
		}
	}
	else
	{
		pass->getStateSet()->unuseDepth ();
	}

	// load stencil states
	ATOM_TiXmlElement *stencilElement = element->FirstChildElement ("stencil_state");
	if (stencilElement)
	{
		ATOM_StencilAttributes *state = pass->getStateSet()->useStencil (0);
		if (!loadStencilStates (device, state, stencilElement, this, pass.get(), effect))
		{
			ATOM_LOGGER::error ("%s: Invalid renderstates declaration in effect \"%s\".\n", __FUNCTION__, effect->getName());
			return false;
		}
	}
	else
	{
		pass->getStateSet()->unuseStencil ();
	}

	// load rasterizer states
	ATOM_TiXmlElement *rasterizerElement = element->FirstChildElement ("rasterizer_state");
	if (rasterizerElement)
	{
		ATOM_RasterizerAttributes *state = pass->getStateSet()->useRasterizer (0);
		if (!loadRasterizerStates (device, state, rasterizerElement, this, pass.get(), effect))
		{
			ATOM_LOGGER::error ("%s: Invalid renderstates declaration in effect \"%s\".\n", __FUNCTION__, effect->getName());
			return false;
		}
	}
	else
	{
		pass->getStateSet()->unuseRasterizer ();
	}

	// load multisample state
	ATOM_TiXmlElement *multisampleElement = element->FirstChildElement ("multisample_state");
	if (multisampleElement)
	{
		ATOM_MultisampleAttributes *state = pass->getStateSet()->useMultisample (0);
		if (!loadMultisampleStates (device, state, multisampleElement, this, pass.get(), effect))
		{
			ATOM_LOGGER::error ("%s: Invalid renderstates declaration in effect \"%s\".\n", __FUNCTION__, effect->getName());
			return false;
		}
	}
	else
	{
		pass->getStateSet()->unuseMultisample ();
	}

	ATOM_Shader *pixelshader = pass->getGPUProgram()->getShader (ATOM_SHADER_TYPE_PIXELSHADER);
	ATOM_Shader *vertexshader = pass->getGPUProgram()->getShader (ATOM_SHADER_TYPE_VERTEXSHADER);

	// setup sampler states for programmable pipeline
	if (pixelshader || vertexshader)
	{
		ATOM_Shader::ParamHandle handle;

		for (ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_MaterialParam)>::const_iterator it = _params.begin(); it != _params.end(); ++it)
		{
			ATOM_MaterialParam *param = it->second.get();
			if (param->getParamType() == ATOM_MaterialParam::ParamType_Sampler)
			{
				ATOM_MaterialTextureParam *texParam = ((ATOM_MaterialSamplerParam*)param)->getTexParam();
				handle = pixelshader ? pixelshader->getParamHandle (it->first.c_str(), 0) : 0;
				if (handle)
				{
					unsigned index = pixelshader->getSamplerIndex (handle);
					ATOM_SamplerAttributes *srcState = ((ATOM_MaterialSamplerParam*)param)->getSamplerStates();
					if (srcState)
					{
						if (texParam)
						{
							texParam->addApplyCallback (&stateParamCallback_Texture, srcState, 0, effect, pass.get());
						}
						pass->getStateSet()->useSampler (index, srcState);
					}
				}
				handle = vertexshader ? vertexshader->getParamHandle (it->first.c_str(), 0) : 0;
				if (handle)
				{
					unsigned index = vertexshader->getSamplerIndex (handle);
					ATOM_SamplerAttributes *srcState = ((ATOM_MaterialSamplerParam*)param)->getSamplerStates();
					if (srcState)
					{
						if (texParam)
						{
							texParam->addApplyCallback (&stateParamCallback_Texture, srcState, 0, effect, pass.get());
						}
						pass->getStateSet()->useSampler (ATOM_RenderAttributes::DMapStage_0+index, srcState);
					}
				}
			}
		}
	}

	// override sampler states if sampler state is explicit declared
	ATOM_TiXmlElement *samplerElement = element->FirstChildElement ("sampler_state");
	while (samplerElement)
	{
		int samplerIndex = -1;
		samplerElement->QueryIntAttribute ("index", &samplerIndex);
		if (samplerIndex < 0)
		{
			// fixed function pixel pipeline requires the sampler index
			ATOM_LOGGER::error ("%s: sampler states declaration for fixed function pipeline requires sampler index.\n", __FUNCTION__);
			return false;
		}

		// discard old first
		pass->getStateSet()->unuseSampler (samplerIndex);
		// override
		ATOM_SamplerAttributes *state = pass->getStateSet()->useSampler (samplerIndex, 0);
		ATOM_MaterialTextureParam *texParam = nullptr;
		if (!loadSamplerStates (device, state, samplerElement, this, pass.get(), effect, &texParam))
		{
			// load failed
			ATOM_LOGGER::error ("%s: Invalid renderstates declaration in effect \"%s\".\n", __FUNCTION__, effect->getName());
			return false;
		}
		if (texParam)
		{
			texParam->addApplyCallback (&stateParamCallback_Texture, state, 0, effect, pass.get());
		}

		samplerElement = samplerElement->NextSiblingElement ("sampler_state");
	}

	// load alpha test states
	ATOM_TiXmlElement *alphaTestElement = element->FirstChildElement ("alphatest_state");
	if (alphaTestElement)
	{
		ATOM_AlphaTestAttributes *state = pass->getStateSet()->useAlphaTest (0);
		if (!loadAlphaTestStates (device, state, alphaTestElement, this, pass.get(), effect))
		{
			ATOM_LOGGER::error ("%s: Invalid renderstates declaration in effect \"%s\".\n", __FUNCTION__, effect->getName());
			return false;
		}
	}
	else
	{
		pass->getStateSet()->unuseAlphaTest ();
	}

	effect->addPass (pass.get());

	return true;
}

static unsigned vertexAttribFromString (const char *str, const char *type)
{
	if (!stricmp (str, "position"))
	{
		if (!type || !stricmp (type, "float3"))
		{
			return ATOM_VERTEX_ATTRIB_COORD;
		}
		else if (!stricmp (type, "float4"))
		{
			return ATOM_VERTEX_ATTRIB_COORD_XYZW;
		}
		else if (!strcmp (type, "float3rhw"))
		{
			return ATOM_VERTEX_ATTRIB_COORD_XYZRHW;
		}
		else
		{
			return 0;
		}
	}
	else if (!stricmp (str, "normal"))
	{
		return ATOM_VERTEX_ATTRIB_NORMAL;
	}
	else if (!stricmp (str, "color0"))
	{
		return ATOM_VERTEX_ATTRIB_PRIMARY_COLOR;
	}
	else if (!stricmp (str, "psize"))
	{
		return ATOM_VERTEX_ATTRIB_PSIZE;
	}
	else
	{
		char buf[12];
		for (int layer = 0; layer < 8; ++layer)
		{
			sprintf (buf, "texcoord%d", layer);
			if (!stricmp (str, buf))
			{
				if (!type || !stricmp(type, "float2"))
				{
					return ATOM_VERTEX_ATTRIB_TEX(layer, 2);
				}
				else if (!stricmp (type, "float"))
				{
					return ATOM_VERTEX_ATTRIB_TEX(layer, 1);
				}
				else if (!stricmp (type, "float3"))
				{
					return ATOM_VERTEX_ATTRIB_TEX(layer, 3);
				}
				else if (!stricmp (type, "float4"))
				{
					return ATOM_VERTEX_ATTRIB_TEX(layer, 4);
				}
				else
				{
					return 0;
				}
			}
		}
	}

	return 0;
}

unsigned ATOM_CoreMaterial::parseStreamDefine (ATOM_RenderDevice *device, ATOM_TiXmlElement *element) const
{
	unsigned streamdefine = 0;

	ATOM_TiXmlElement *streamElement = element->FirstChildElement ("stream");
	while (streamElement)
	{
		const char *semantec = streamElement->Attribute ("semantec");
		if (!semantec)
		{
			return 0;
		}

		const char *type = streamElement->Attribute ("type");

		unsigned attrib = vertexAttribFromString (semantec, type);
		if (attrib == 0)
		{
			// invalid stream
			return 0;
		}

		streamdefine |= attrib;
		streamElement = streamElement->NextSiblingElement ("stream");
	}

	return streamdefine;
}

bool ATOM_CoreMaterial::parseVertexShader (ATOM_RenderDevice *device, ATOM_TiXmlElement *element, ATOM_MaterialPass *pass, const char *shaderCode, ParamArrayInfoMap *arrayInfoMap) const
{
	ATOM_TiXmlElement *vsElement = element->FirstChildElement ("vertexshader");
	if (vsElement)
	{
		const char *entry = vsElement->Attribute ("entry");
		if (!entry)
		{
			// no vs entry
			ATOM_LOGGER::error ("%s: No <entry> in vertex shader declaration.\n", __FUNCTION__);
			return false;
		}
		const char *target = vsElement->Attribute ("target");
		if (!target)
		{
			// no vs target
			ATOM_LOGGER::error ("%s: No <target> in vertex shader declaration.\n", __FUNCTION__);
			return false;
		}
		ATOM_STRING shaderKey;
		shaderKey += entry;
		shaderKey += target;

		ATOM_AUTOREF(ATOM_Shader) vs = arrayInfoMap->getShader (shaderKey.c_str());
		if (vs)
		{
			if (vs->getShaderType () != ATOM_SHADER_TYPE_VERTEXSHADER)
			{
				ATOM_LOGGER::error ("%s: <%s> is not a vertex shader.\n", __FUNCTION__, entry);
				return false;
			}
			pass->getGPUProgram ()->addShader (vs.get());
		}
		else
		{
			// wangjian modified
			unsigned shader_model_require = 0;

			int compile_target;
			if (!strcmp (target, "vs_2_0"))
			{
				compile_target = ATOM_SHADER_COMPILE_TARGET_VS_2_0;
				shader_model_require = 2;
			}
			else if (!strcmp (target, "vs_2_x"))
			{
				compile_target = ATOM_SHADER_COMPILE_TARGET_VS_2_X;
				shader_model_require = 3;
			}
			else if (!strcmp (target, "vs_3_0"))
			{
				compile_target = ATOM_SHADER_COMPILE_TARGET_VS_3_0;
				shader_model_require = 3;
			}
			else if (!strcmp (target, "vs_4_0"))
			{
				compile_target = ATOM_SHADER_COMPILE_TARGET_VS_4_0;
				shader_model_require = 4;
			}
			else if (!strcmp (target, "vs_4_1"))
			{
				compile_target = ATOM_SHADER_COMPILE_TARGET_VS_4_1;
				shader_model_require = 4;
			}
			else if (!strcmp (target, "vs_5_0"))
			{
				compile_target = ATOM_SHADER_COMPILE_TARGET_VS_5_0;
				shader_model_require = 5;
			}
			else
			{
				// invalid compile target
				ATOM_LOGGER::error ("%s: Invalid vertex shader target \"%s\".\n", __FUNCTION__, target);
				return false;
			}

			//--- wangjian added ---//
			// ��鵱ǰ�豸֧�ֵ�SM �Ƿ����� ������ ֱ���˳�
			const ATOM_RenderDeviceCaps &caps = device->getCapabilities();
			if( caps.shader_model < shader_model_require )
			{
				ATOM_LOGGER::error ("%s: shader model is not on demand : the device sm is %u, the demand is %u \n", __FUNCTION__, caps.shader_model, shader_model_require );
				return false;
			}

			// ʹ���豸֧�ֵ�sm����ǯ��compile_target�����������֧�ֵ�SM�汾������Ͱ汾
			/*const int sms_1[6] = { 0, 0, 0, 2, 3, 4 };
			const int sms_ext[6] = { 0, 1, 0, 0, 0, 0 };
			compile_target = sms_1[device->getCapabilities().shader_model] + sms_ext[device->getCapabilities().shader_model_ext];*/
			/*const int sms_0[6] = { 2, 2, 3, 4, 4, 5 };
			const int sms_1[6] = { 0, 0, 0, 2, 3, 4 };
			const int sms_ext[6] = { 0,0,1,0,0,0 };
			if( sms_0[compile_target] > device->getCapabilities().shader_model )
				compile_target = sms_1[device->getCapabilities().shader_model] + sms_ext[device->getCapabilities().shader_model_ext];*/
			//----------------------//

			//--- wangjian modified ---//
			// �������ļ����
			// ����Ǽ����Ѿ����ڵĶ������ļ�
			unsigned int shaderBufferSize = 0;
			ATOM_VECTOR<char> shaderBuffer;
			if( _readBin && _shaderBin )
			{
				// ��shaderbuffer�Ĵ�С
				_shaderBin->read(&shaderBufferSize,sizeof(unsigned int));

#if 0
				ATOM_LOGGER::log("file : %s, vertexshader : %s, , size : %d \n", _shaderBin->getName(), entry, shaderBufferSize );
#endif
				// ��ȡshaderbuffer
				if( shaderBufferSize )
				{
					shaderBuffer.resize(shaderBufferSize+1);
					char *str = &shaderBuffer[0];
					unsigned n = _shaderBin->read (str, shaderBufferSize);
					str[n] = '\0';
				}
			}

			ATOM_AUTOREF(ATOM_Shader) vs = device->allocVertexShader (0);

			//--- wangjian modified : add two more arguments ---//
			if( !vs )
			{
				// create vertex shader failed
				ATOM_LOGGER::error ("%s: create vertex shader failed.\n", __FUNCTION__);
				return false;
			}
			//// ���ʹ���첽���� ��VS�Ĵ�����������Ⱦ�߳��н���
			//if( ATOM_AsyncLoader::IsRun() && 
			//	false == ATOM_AsyncLoader::CheckImmediateResource( getObjectName() ) )
			//{
			//	/*getAsyncLoader()->AddShaderCreationItem( vs, 
			//											_readBin && shaderBufferSize ? &shaderBuffer[0] : shaderCode,
			//											entry,
			//											compile_target,
			//											_readBin,
			//											_shaderBin	);*/
			//}
			//else
			//{
				
				unsigned begin_time = ATOM_GetTick();

				if ( !vs->create (	_readBin && _shaderBin && shaderBufferSize ? &shaderBuffer[0] : shaderCode, 
									entry, 
									compile_target, 
									_readBin && _shaderBin && shaderBufferSize,								// true:ͨ����ȡ��������������shader��false:��shaderд��������ļ�
									_shaderBin ) )							// �������ļ�
				{
					// create vertex shader failed
					ATOM_LOGGER::error ("%s: create vertex shader failed.\n", __FUNCTION__);
					return false;
				}

				createvsps_time += ATOM_GetTick() - begin_time;
			/*}*/
			

			arrayInfoMap->shaderMap[shaderKey] = vs;
			pass->getGPUProgram ()->addShader (vs.get());
		}
	}
	//--- wangjian added ---//
	// �������ļ����
	else
	{
		if( _shaderBin )
		{
			// shader buffer ��СΪ0
			unsigned int size = 0;

			// д
			if( !_readBin )
			{
				_shaderBin->write(&size,sizeof(unsigned int));
			}
			// ��
			else
			{
				_shaderBin->read(&size,sizeof(unsigned int));
			}
		}
	}
	//----------------------//

	return true;
}

bool ATOM_CoreMaterial::parsePixelShader (ATOM_RenderDevice *device, ATOM_TiXmlElement *element, ATOM_MaterialPass *pass, const char *shaderCode, ParamArrayInfoMap *arrayInfoMap) const
{
	ATOM_TiXmlElement *psElement = element->FirstChildElement ("pixelshader");
	if (psElement)
	{
		const char *entry = psElement->Attribute ("entry");
		if (!entry)
		{
			// no ps entry
			ATOM_LOGGER::error ("%s: No <entry> in pixel shader declaration.\n", __FUNCTION__);
			return false;
		}
		const char *target = psElement->Attribute ("target");
		if (!target)
		{
			// no ps target
			ATOM_LOGGER::error ("%s: No <target> in pixel shader declaration.\n", __FUNCTION__);
			return false;
		}
		ATOM_STRING shaderKey;
		shaderKey += entry;
		shaderKey += target;

		ATOM_AUTOREF(ATOM_Shader) ps = arrayInfoMap->getShader (shaderKey.c_str());
		if (ps)
		{
			if (ps->getShaderType () != ATOM_SHADER_TYPE_PIXELSHADER)
			{
				ATOM_LOGGER::error ("%s: <%s> is not a pixel shader.\n", __FUNCTION__, entry);
				return false;
			}
			pass->getGPUProgram ()->addShader (ps.get());
		}
		else
		{
			unsigned shader_model_require = 0;

			int compile_target;
			if (!strcmp (target, "ps_2_0"))
			{
				compile_target = ATOM_SHADER_COMPILE_TARGET_PS_2_0;
				shader_model_require = 2;
			}
			else if (!strcmp (target, "ps_2_x"))
			{
				compile_target = ATOM_SHADER_COMPILE_TARGET_PS_2_X;
				shader_model_require = 3;
			}
			else if (!strcmp (target, "ps_3_0"))
			{
				compile_target = ATOM_SHADER_COMPILE_TARGET_PS_3_0;
				shader_model_require = 3;
			}
			else if (!strcmp (target, "ps_4_0"))
			{
				compile_target = ATOM_SHADER_COMPILE_TARGET_PS_4_0;
				shader_model_require = 4;
			}
			else if (!strcmp (target, "ps_4_1"))
			{
				compile_target = ATOM_SHADER_COMPILE_TARGET_PS_4_1;
				shader_model_require = 4;
			}
			else if (!strcmp (target, "ps_5_0"))
			{
				compile_target = ATOM_SHADER_COMPILE_TARGET_PS_5_0;
				shader_model_require = 5;
			}
			else
			{
				// invalid compile target
				ATOM_LOGGER::error ("%s: Invalid pixel shader target \"%s\".\n", __FUNCTION__, target);
				return false;
			}

			//--- wangjian added ---//
			// ��鵱ǰ�豸֧�ֵ�SM �Ƿ����� ������ ֱ���˳�
			const ATOM_RenderDeviceCaps &caps = device->getCapabilities();
			if( caps.shader_model < shader_model_require )
			{
				ATOM_LOGGER::error ("%s: shader model is not on demand : the device sm is %u, the demand is %u \n", 
									__FUNCTION__, 
									caps.shader_model, 
									shader_model_require );
				return false;
			}

			// ʹ���豸֧�ֵ�sm����ǯ��compile_target�����������֧�ֵ�SM�汾������Ͱ汾
			/*const int sms_1[6] = { 0, 0, 0, 2, 3, 4 };
			const int sms_ext[6] = { 0, 1, 0, 0, 0, 0 };
			compile_target = sms_1[device->getCapabilities().shader_model] + sms_ext[device->getCapabilities().shader_model_ext] + 6;*/
			/*
			const int sms_0[6] = { 2, 2, 3, 4, 4, 5 };
			const int sms_1[6] = { 0, 0, 0, 2, 3, 4 };
			const int sms_ext[6] = { 0,0,1,0,0,0 };
			if( sms_0[compile_target-6] > device->getCapabilities().shader_model )
				compile_target = sms_1[device->getCapabilities().shader_model] + sms_ext[device->getCapabilities().shader_model_ext];
			*/
			//----------------------//

			//--- wangjian added ---//
			// �������ļ����
			unsigned int shaderBufferSize = 0;
			ATOM_VECTOR<char> shaderBuffer;
			if( _readBin && _shaderBin )
			{
				// ��ȡshader buffer��С
				_shaderBin->read(&shaderBufferSize,sizeof(unsigned int));
#if 0
				ATOM_LOGGER::log("file : %s, pixelshader : %s, , size : %d \n", _shaderBin->getName(), entry, shaderBufferSize );
#endif
				// ��ȡshader buffer
				if( shaderBufferSize )
				{
					shaderBuffer.resize(shaderBufferSize+1);
					char *str = &shaderBuffer[0];
					unsigned n = _shaderBin->read (str, shaderBufferSize);
					str[n] = '\0';
				}
			}
			//---------------------//

			ATOM_AUTOREF(ATOM_Shader) ps = device->allocPixelShader (0);

			unsigned begin_time = ATOM_GetTick();

			//--- wangjian modified : add two more arguments ---//
			if (	!ps || 
					!ps->create (	_readBin && _shaderBin && shaderBufferSize ? &shaderBuffer[0] : shaderCode, entry, compile_target, 
									_readBin && _shaderBin && shaderBufferSize,		// true : �Ӷ������ļ��ж�ȡshader���� / false : д��������ļ�
									_shaderBin	)									// �������ļ�
				)
			{
				// create vertex shader failed
				ATOM_LOGGER::error ("%s: create pixel shader failed.\n", __FUNCTION__);
				return false;
			}

			createvsps_time += ATOM_GetTick() - begin_time;

			arrayInfoMap->shaderMap[shaderKey] = ps;
			pass->getGPUProgram ()->addShader (ps.get());
		}
	}
	//--- wangjian added ---//
	else
	{
		if( _shaderBin )
		{
			// shader buffer ��СΪ0
			unsigned int size = 0;

			// д
			if( !_readBin )
			{
				_shaderBin->write(&size,sizeof(unsigned int));
			}
			// ��
			else
			{
				_shaderBin->read(&size,sizeof(unsigned int));
			}
		}
	}
	//----------------------//

	return true;
}
//--- wangjian modified ---//
bool ATOM_CoreMaterial::parseDefines (ATOM_TiXmlElement *element)
//bool ATOM_CoreMaterial::parseDefines (ATOM_RenderDevice *device,ATOM_TiXmlElement *element)
{
	ATOM_TiXmlElement *defineElement = element->FirstChildElement ("define");
	while (defineElement)
	{
		if (!parseDefineElement (/*device,*/defineElement))
		{
			return false;
		}
		defineElement = defineElement->NextSiblingElement ("define");
	}
	return true;
}

bool ATOM_CoreMaterial::parseParamAndShaderCode (ATOM_RenderDevice *device, ATOM_TiXmlElement *element, ATOM_STRING &shaderCode, ParamArrayInfoMap *arrayInfoMap)
{
	ATOM_TiXmlElement *paramElement = element->FirstChildElement ("param");
	while (paramElement)
	{
		if (!parseParamElement (device, paramElement, arrayInfoMap))
		{
			// parameter parse failed
			return false;
		}
		paramElement = paramElement->NextSiblingElement ("param");
	}

	// wangjian added
#if 1
	// �����BIN��ȡ ���������CODE
	if( _readBin && _shaderBin)
	{
		return true;
	}
#endif

	ATOM_TiXmlElement *codeElement = element->FirstChildElement ("code");
	while (codeElement)
	{
		if (!parseCode (device, codeElement, shaderCode))
		{
			// parse code failed
			ATOM_LOGGER::error ("%s: Parse shader code failed.\n", __FUNCTION__);
			return false;
		}
		codeElement = codeElement->NextSiblingElement ("code");
	}

	return true;
}

static ATOM_MaterialParam::EditorType parseEditorType (const char *str)
{
	ATOM_MaterialParam::EditorType editorType = ATOM_MaterialParam::ParamEditorType_Disable;

	if (str)
	{
		if (!stricmp (str, "none"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_None;
		}
		else if (!stricmp (str, "bool"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_Bool;
		}
		else if (!stricmp (str, "color"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_Color;
		}
		else if (!stricmp (str, "direction"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_Direction;
		}
		else if (!stricmp (str, "e_primitivetype"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_Constant_PrimitiveType;
		}
		else if (!stricmp (str, "e_colormask"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_Constant_ColorMask;
		}
		else if (!stricmp (str, "e_shademode"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_Constant_ShadeMode;
		}
		else if (!stricmp (str, "e_blendfunc"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_Constant_BlendFunc;
		}
		else if (!stricmp (str, "e_blendop"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_Constant_BlendOp;
		}
		else if (!stricmp (str, "e_comparefunc"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_Constant_CompareFunc;
		}
		else if (!stricmp (str, "e_stencilop"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_Constant_StencilOp;
		}
		else if (!stricmp (str, "e_fillmode"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_Constant_FillMode;
		}
		else if (!stricmp (str, "e_cullmode"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_Constant_CullMode;
		}
		else if (!stricmp (str, "e_frontface"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_Constant_FrontFace;
		}
		else if (!stricmp (str, "e_samplerfilter"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_Constant_SamplerFilter;
		}
		else if (!stricmp (str, "e_sampleraddress"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_Constant_SamplerAddress;
		}
		else if (!stricmp (str, "e_textureop"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_Constant_TextureOp;
		}
		else if (!stricmp (str, "e_texturearg"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_Constant_TextureArg;
		}
		else if (!stricmp (str, "e_texcoordgen"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_Constant_TexCoordGen;
		}
		else if (!stricmp (str, "e_fogmode"))
		{
			editorType = ATOM_MaterialParam::ParamEditorType_Constant_FogMode;
		}
	}
	return editorType;
}

//--- wangjian modified ��Ӱ������ ---//
bool ATOM_CoreMaterial::parseDefineElement (ATOM_TiXmlElement *element)
//bool ATOM_CoreMaterial::parseDefineElement (ATOM_RenderDevice *device,ATOM_TiXmlElement *element)
{
	/*
	//--- wangjian added ---//
	// �����豸����������ѡ������_definitions
	const char *prereq_type = element->Attribute("prereq_type");
	if( prereq_type )
	{
		const char *prerequisite = element->Attribute("prereq_name");
		if( prerequisite )
		{
			if( !g_shaderDefineChecker.checkDefine( device, prereq_type, prerequisite ) )
				return true;
		}
	}
	//----------------------//
	*/

	const char *name = element->Attribute("name");
	if (!name)
	{
		// definition without 'name' attribute
		ATOM_LOGGER::error ("%s: No <name> element found in definition declaration.\n", __FUNCTION__);
		return false;
	}

	const char *value = element->Attribute("value");
	if (!value)
	{
		value = "";
	}

	_definitions[name] = value;

	return true;
}

bool ATOM_CoreMaterial::parseParamElement (ATOM_RenderDevice *device, ATOM_TiXmlElement *element, ParamArrayInfoMap *arrayInfoMap)
{
	const char *name = element->Attribute("name");
	if (!name)
	{
		// parameter without 'name' attribute
		ATOM_LOGGER::error ("%s: No <name> element found in parameter declaration.\n", __FUNCTION__);
		return false;
	}

	if (getParameter (name))
	{
		// duplicated parameter name
		ATOM_LOGGER::error ("%s: parameter named \"%s\" already declared.\n", __FUNCTION__, name);
		return false;
	}

	const char *type = element->Attribute("type");
	if (!type)
	{
		// no parameter type defined
		ATOM_LOGGER::error ("%s: No <type> element found in parameter declaration.\n", __FUNCTION__);
		return false;
	}

	int registerIndex = -1;
	element->QueryIntAttribute ("register", &registerIndex);

	ATOM_STRING arraySizeStr;
	ATOM_MaterialParam::Type paramType = parseTypeString (type, arraySizeStr);
	if (paramType == ATOM_MaterialParam::ParamType_Unknown)
	{
		// invalid parameter type
		ATOM_LOGGER::error ("%s: Couldn't determine data type for parameter \"%s\".\n", __FUNCTION__, name);
		return false;
	}

	// parse default value
	const char *def = element->Attribute("default");

	// parse editor type
	ATOM_MaterialParam::EditorType editorType = parseEditorType (element->Attribute("editortype"));

	switch (paramType)
	{
	case ATOM_MaterialParam::ParamType_Float:
		{
			float defaultValue = 0.f;
			ATOM_AUTOPTR(ATOM_MaterialFloatParam) param = ATOM_NEW(ATOM_MaterialFloatParam, defaultValue);
			param->setEditorType (editorType);
			param->setRegisterIndex (registerIndex);
			if (def)
			{
				if (def[0] == '$')
				{
					const ATOM_ParameterChannel::Channel *channel = ATOM_ParameterChannel::getChannel (def+1);
					if (!channel)
					{
						ATOM_LOGGER::error ("%s: No parameter channel named \"%s\".\n", __FUNCTION__, def+1);
						return false;
					}
					param->setValueCallback (channel->callback);
				}
				else 
				{
					if (!parseFloat (def, defaultValue))
					{
						return false;
					}
					param->setValue (defaultValue);
				}
			}
			addParameter (name, param.get());
			break;
		}
	case ATOM_MaterialParam::ParamType_FloatArray:
		{
			ATOM_AUTOPTR(ATOM_MaterialFloatArrayParam) param = ATOM_NEW(ATOM_MaterialFloatArrayParam, 0);
			param->setEditorType (editorType);
			param->setRegisterIndex (registerIndex);
			ParamArrayInfo *arrayInfo = 0;
			if (arrayInfoMap->arrayInfoMap.find (name) != arrayInfoMap->arrayInfoMap.end())
			{
				arrayInfo = arrayInfoMap->arrayInfoMap[name];
			}
			else
			{
				arrayInfo = ATOM_NEW(ParamArrayInfo);
				arrayInfoMap->arrayInfoMap[name] = arrayInfo;
			}
			arrayInfo->arraySizeStr = arraySizeStr;
			arrayInfo->size = 0;
			arrayInfo->arrayValues = 0;

			if (def)
			{
				if (def[0] == '$')
				{
					const ATOM_ParameterChannel::Channel *channel = ATOM_ParameterChannel::getChannel (def+1);
					if (!channel)
					{
						ATOM_LOGGER::error ("%s: No parameter channel named \"%s\".\n", __FUNCTION__, def+1);
						return false;
					}
					param->setValueCallback (channel->callback);
				}
				else 
				{
					ATOM_VECTOR<float> arrayValue = parseFloatArray (def);
					arrayInfo->size = sizeof(float)*arrayValue.size();
					if (arrayInfo->size > 0)
					{
						arrayInfo->arrayValues = ATOM_MALLOC(arrayInfo->size);
						memcpy (arrayInfo->arrayValues, &arrayValue[0], arrayInfo->size);
					}
				}
			}
			addParameter (name, param.get());
			break;
		}
	case ATOM_MaterialParam::ParamType_Int:
		{
			int defaultValue = 0;
			ATOM_AUTOPTR(ATOM_MaterialIntParam) param = ATOM_NEW(ATOM_MaterialIntParam, defaultValue);
			param->setEditorType (editorType);
			param->setRegisterIndex (registerIndex);
			if (def)
			{
				if (def[0] == '$')
				{
					const ATOM_ParameterChannel::Channel *channel = ATOM_ParameterChannel::getChannel (def+1);
					if (!channel)
					{
						ATOM_LOGGER::error ("%s: No parameter channel named \"%s\".\n", __FUNCTION__, def+1);
						return false;
					}
					param->setValueCallback (channel->callback);
				}
				else 
				{
					if (!parseInt (def, defaultValue))
					{
						ATOM_LOGGER::error ("%s: Invalid parameter declaration for parameter \"%s\".\n", __FUNCTION__, name);
						return false;
					}
					param->setValue (defaultValue);
				}
			}
			addParameter (name, param.get());
			break;
		}
	case ATOM_MaterialParam::ParamType_IntArray:
		{
			ATOM_AUTOPTR(ATOM_MaterialIntArrayParam) param = ATOM_NEW(ATOM_MaterialIntArrayParam, 0);
			param->setEditorType (editorType);
			param->setRegisterIndex (registerIndex);
			ParamArrayInfo *arrayInfo = 0;
			if (arrayInfoMap->arrayInfoMap.find (name) != arrayInfoMap->arrayInfoMap.end())
			{
				arrayInfo = arrayInfoMap->arrayInfoMap[name];
			}
			else
			{
				arrayInfo = ATOM_NEW(ParamArrayInfo);
				arrayInfoMap->arrayInfoMap[name] = arrayInfo;
			}
			arrayInfo->arraySizeStr = arraySizeStr;
			arrayInfo->size = 0;
			arrayInfo->arrayValues = 0;

			if (def)
			{
				if (def[0] == '$')
				{
					const ATOM_ParameterChannel::Channel *channel = ATOM_ParameterChannel::getChannel (def+1);
					if (!channel)
					{
						ATOM_LOGGER::error ("%s: No parameter channel named \"%s\".\n", __FUNCTION__, def+1);
						return false;
					}
					param->setValueCallback (channel->callback);
				}
				else 
				{
					ATOM_VECTOR<int> arrayValue = parseIntArray (def);
					arrayInfo->size = sizeof(int)*arrayValue.size();
					if (arrayInfo->size > 0)
					{
						arrayInfo->arrayValues = ATOM_MALLOC(arrayInfo->size);
						memcpy (arrayInfo->arrayValues, &arrayValue[0], arrayInfo->size);
					}
				}
			}
			addParameter (name, param.get());
			break;
		}
	case ATOM_MaterialParam::ParamType_Matrix44:
		{
			ATOM_Matrix4x4f defaultValue = ATOM_Matrix4x4f::getIdentityMatrix ();
			ATOM_AUTOPTR(ATOM_MaterialMatrix44Param) param = ATOM_NEW(ATOM_MaterialMatrix44Param, defaultValue);
			param->setEditorType (editorType);
			param->setRegisterIndex (registerIndex);
			if (def)
			{
				if (def[0] == '$')
				{
					const ATOM_ParameterChannel::Channel *channel = ATOM_ParameterChannel::getChannel (def+1);
					if (!channel)
					{
						ATOM_LOGGER::error ("%s: No parameter channel named \"%s\".\n", __FUNCTION__, def+1);
						return false;
					}
					param->setValueCallback (channel->callback);
				}
				else 
				{
					ATOM_VECTOR<float> m = parseFloatArray (def);
					if (m.size() != 16)
					{
						ATOM_LOGGER::error ("%s: Invalid parameter declaration for parameter \"%s\".\n", __FUNCTION__, name);
						return false;
					}
					param->setValue (*((ATOM_Matrix4x4f*)&m[0]));
				}
			}
			addParameter (name, param.get());
			break;
		}
	case ATOM_MaterialParam::ParamType_Matrix44Array:
		{
			ATOM_AUTOPTR(ATOM_MaterialMatrix44ArrayParam) param = ATOM_NEW(ATOM_MaterialMatrix44ArrayParam, 0);
			param->setEditorType (editorType);
			param->setRegisterIndex (registerIndex);
			ParamArrayInfo *arrayInfo = 0;
			if (arrayInfoMap->arrayInfoMap.find (name) != arrayInfoMap->arrayInfoMap.end())
			{
				arrayInfo = arrayInfoMap->arrayInfoMap[name];
			}
			else
			{
				arrayInfo = ATOM_NEW(ParamArrayInfo);
				arrayInfoMap->arrayInfoMap[name] = arrayInfo;
			}
			arrayInfo->arraySizeStr = arraySizeStr;
			arrayInfo->size = 0;
			arrayInfo->arrayValues = 0;

			if (def)
			{
				if (def[0] == '$')
				{
					const ATOM_ParameterChannel::Channel *channel = ATOM_ParameterChannel::getChannel (def+1);
					if (!channel)
					{
						ATOM_LOGGER::error ("%s: No parameter channel named \"%s\".\n", __FUNCTION__, def+1);
						return false;
					}
					param->setValueCallback (channel->callback);
				}
				else 
				{
					ATOM_VECTOR<float> arrayValue = parseFloatArray (def);
					arrayInfo->size = sizeof(float)*arrayValue.size();
					if (arrayInfo->size > 0)
					{
						arrayInfo->arrayValues = ATOM_MALLOC(arrayInfo->size);
						memcpy (arrayInfo->arrayValues, &arrayValue[0], arrayInfo->size);
					}
				}
			}
			addParameter (name, param.get());
			break;
		}
	case ATOM_MaterialParam::ParamType_Matrix43:
		{
			ATOM_Matrix3x4f defaultValue = ATOM_Matrix3x4f::getIdentityMatrix ();
			ATOM_AUTOPTR(ATOM_MaterialMatrix43Param) param = ATOM_NEW(ATOM_MaterialMatrix43Param, defaultValue);
			param->setEditorType (editorType);
			param->setRegisterIndex (registerIndex);
			if (def)
			{
				if (def[0] == '$')
				{
					const ATOM_ParameterChannel::Channel *channel = ATOM_ParameterChannel::getChannel (def+1);
					if (!channel)
					{
						ATOM_LOGGER::error ("%s: No parameter channel named \"%s\".\n", __FUNCTION__, def+1);
						return false;
					}
					param->setValueCallback (channel->callback);
				}
				else 
				{
					ATOM_VECTOR<float> m = parseFloatArray (def);
					if (m.size() != 12)
					{
						ATOM_LOGGER::error ("%s: Invalid parameter declaration for parameter \"%s\".\n", __FUNCTION__, name);
						return false;
					}
					param->setValue (*((ATOM_Matrix3x4f*)&m[0]));
				}
			}
			addParameter (name, param.get());
			break;
		}
	case ATOM_MaterialParam::ParamType_Matrix43Array:
		{
			ATOM_AUTOPTR(ATOM_MaterialMatrix43ArrayParam) param = ATOM_NEW(ATOM_MaterialMatrix43ArrayParam, 0);
			param->setEditorType (editorType);
			param->setRegisterIndex (registerIndex);
			ParamArrayInfo *arrayInfo = 0;
			if (arrayInfoMap->arrayInfoMap.find (name) != arrayInfoMap->arrayInfoMap.end())
			{
				arrayInfo = arrayInfoMap->arrayInfoMap[name];
			}
			else
			{
				arrayInfo = ATOM_NEW(ParamArrayInfo);
				arrayInfoMap->arrayInfoMap[name] = arrayInfo;
			}
			arrayInfo->arraySizeStr = arraySizeStr;
			arrayInfo->size = 0;
			arrayInfo->arrayValues = 0;

			if (def)
			{
				if (def[0] == '$')
				{
					const ATOM_ParameterChannel::Channel *channel = ATOM_ParameterChannel::getChannel (def+1);
					if (!channel)
					{
						ATOM_LOGGER::error ("%s: No parameter channel named \"%s\".\n", __FUNCTION__, def+1);
						return false;
					}
					param->setValueCallback (channel->callback);
				}
				else 
				{
					ATOM_VECTOR<float> arrayValue = parseFloatArray (def);
					arrayInfo->size = sizeof(float)*arrayValue.size();
					if (arrayInfo->size > 0)
					{
						arrayInfo->arrayValues = ATOM_MALLOC(arrayInfo->size);
						memcpy (arrayInfo->arrayValues, &arrayValue[0], arrayInfo->size);
					}
				}
			}
			addParameter (name, param.get());
			break;
		}
	case ATOM_MaterialParam::ParamType_Vector:
		{
			ATOM_Vector4f defaultValue(0.f);
			ATOM_AUTOPTR(ATOM_MaterialVectorParam) param = ATOM_NEW(ATOM_MaterialVectorParam, defaultValue);
			param->setEditorType (editorType);
			param->setRegisterIndex (registerIndex);
			if (def)
			{
				if (def[0] == '$')
				{
					const ATOM_ParameterChannel::Channel *channel = ATOM_ParameterChannel::getChannel (def+1);
					if (!channel)
					{
						ATOM_LOGGER::error ("%s: No parameter channel named \"%s\".\n", __FUNCTION__, def+1);
						return false;
					}
					param->setValueCallback (channel->callback);
				}
				else 
				{
					ATOM_VECTOR<float> v = parseFloatArray (def);
					if (v.size() > 4)
					{
						ATOM_LOGGER::error ("%s: Invalid parameter declaration for parameter \"%s\".\n", __FUNCTION__, name);
						return false;
					}
					else
					{
						while (v.size() < 4)
						{
							v.push_back (0.f);
						}
					}
					param->setValue (*((ATOM_Vector4f*)&v[0]));
				}
			}
			addParameter (name, param.get());
			break;
		}
	case ATOM_MaterialParam::ParamType_VectorArray:
		{
			ATOM_AUTOPTR(ATOM_MaterialVectorArrayParam) param = ATOM_NEW(ATOM_MaterialVectorArrayParam, 0);
			param->setEditorType (editorType);
			param->setRegisterIndex (registerIndex);
			ParamArrayInfo *arrayInfo = 0;
			if (arrayInfoMap->arrayInfoMap.find (name) != arrayInfoMap->arrayInfoMap.end())
			{
				arrayInfo = arrayInfoMap->arrayInfoMap[name];
			}
			else
			{
				arrayInfo = ATOM_NEW(ParamArrayInfo);
				arrayInfoMap->arrayInfoMap[name] = arrayInfo;
			}
			arrayInfo->arraySizeStr = arraySizeStr;
			arrayInfo->size = 0;
			arrayInfo->arrayValues = 0;

			if (def)
			{
				if (def[0] == '$')
				{
					const ATOM_ParameterChannel::Channel *channel = ATOM_ParameterChannel::getChannel (def+1);
					if (!channel)
					{
						ATOM_LOGGER::error ("%s: No parameter channel named \"%s\".\n", __FUNCTION__, def+1);
						return false;
					}
					param->setValueCallback (channel->callback);
				}
				else 
				{
					ATOM_VECTOR<float> arrayValue = parseFloatArray (def);
					arrayInfo->size = sizeof(float)*arrayValue.size();
					if (arrayInfo->size > 0)
					{
						arrayInfo->arrayValues = ATOM_MALLOC(arrayInfo->size);
						memcpy (arrayInfo->arrayValues, &arrayValue[0], arrayInfo->size);
					}
				}
			}
			addParameter (name, param.get());
			break;
		}
	case ATOM_MaterialParam::ParamType_Texture:
		{
			ATOM_AUTOREF(ATOM_Texture) defaultValue;
			ATOM_AUTOPTR(ATOM_MaterialTextureParam) param = ATOM_NEW(ATOM_MaterialTextureParam);
			param->setEditorType (editorType);
			param->setRegisterIndex (registerIndex);

			if (def)
			{
				if (def[0] == '$')
				{
					const ATOM_ParameterChannel::Channel *channel = ATOM_ParameterChannel::getChannel (def+1);
					if (!channel)
					{
						ATOM_LOGGER::error ("%s: No parameter channel named \"%s\".\n", __FUNCTION__, def+1);
						return false;
					}
					param->setValueCallback (channel->callback);
				}
				else
				{
					defaultValue = ATOM_CreateTextureResource (def,ATOM_PIXEL_FORMAT_UNKNOWN,10000);	// Ĭ���첽���� ����һ���ǳ��ߵ����ȼ�
					param->setFileName(def);
				}
			}
			param->setValue (defaultValue.get());
			addParameter (name, param.get());
			break;
		}
	case ATOM_MaterialParam::ParamType_Sampler:
		{
			ATOM_AUTOPTR(ATOM_SamplerAttributes) samplerAttributes = device->allocRenderAttributes (ATOM_RenderAttributes::RS_Sampler);
			ATOM_MaterialTextureParam *texParam = nullptr;
			if (!loadSamplerStates (device, samplerAttributes.get(), element, this, nullptr, nullptr, &texParam))
			{
				ATOM_LOGGER::error ("%s: Invalid sampler declaration for parameter \"%s\".\n", __FUNCTION__, name);
				return false;
			}
			ATOM_AUTOPTR(ATOM_MaterialSamplerParam) samplerParam = ATOM_NEW(ATOM_MaterialSamplerParam);
			samplerParam->setEditorType (editorType);
			samplerParam->setRegisterIndex (registerIndex);
			samplerParam->setIndex (0);
			samplerParam->setSamplerStates (samplerAttributes.get());
			samplerParam->setTexParam (texParam);
			addParameter (name, samplerParam.get());
			break;
		}
	default:
		// invalid type
		ATOM_LOGGER::error ("%s: Unknown parameter type for parameter\"%s\".\n", __FUNCTION__, name);
		return false;
	}

	// TODO: set default value

	return true;
}

bool ATOM_CoreMaterial::parseDimension (const char *str, unsigned *value) const
{
	unsigned t;

	// Check if str is a number
	if (sscanf (str, "%u", &t) == 1)
	{
		*value = t;
		return true;
	}

	// Check if str is a definition
	ATOM_HASHMAP<ATOM_STRING, ATOM_STRING>::const_iterator it = _definitions.find (str);
	if (it == _definitions.end ())
	{
		return false;
	}

	if (sscanf (it->second.c_str(), "%u", &t) == 1)
	{
		*value = t;
		return true;
	}

	return false;
}

static ATOM_STRING parseArrayDim (const char *type, const char *str)
{
	ATOM_STRING ret;
	size_t len = strlen (str);
	size_t type_len = strlen (type);

	if (len > type_len+2)
	{
		if (!memcmp (type, str, type_len) && str[type_len] == '[')
		{
			const char *start = str + type_len + 1;
			const char *end = strchr (start, ']');
			if (end)
			{
				return ATOM_STRING (start, end - start);
			}
		}
	}
	return "";
}

ATOM_MaterialParam::Type ATOM_CoreMaterial::parseTypeString (const char *str, ATOM_STRING &arraySizeStr) const
{
	if (!strcmp (str, "float"))
	{
		return ATOM_MaterialParam::ParamType_Float;
	}
	else if (!strcmp (str, "float4") || !strcmp(str, "float3") || !strcmp(str, "float2"))
	{
		return ATOM_MaterialParam::ParamType_Vector;
	}
	else if (!strcmp (str, "float4x4"))
	{
		return ATOM_MaterialParam::ParamType_Matrix44;
	}
	else if (!strcmp (str, "float4x3"))
	{
		return ATOM_MaterialParam::ParamType_Matrix43;
	}
	else if (!strcmp (str, "texture"))
	{
		return ATOM_MaterialParam::ParamType_Texture;
	}
	else if (!strcmp (str, "sampler") || !strcmp (str, "sampler2D") || !strcmp (str, "sampler3D") || !strcmp (str, "samplerCUBE"))
	{
		return ATOM_MaterialParam::ParamType_Sampler;
	}
	else if (!strcmp (str, "int"))
	{
		return ATOM_MaterialParam::ParamType_Int;
	}
	else 
	{
		arraySizeStr = parseArrayDim ("float", str);
		if (!arraySizeStr.empty ())
		{
			return ATOM_MaterialParam::ParamType_FloatArray;
		}
		arraySizeStr = parseArrayDim ("float4", str);
		if (!arraySizeStr.empty ())
		{
			return ATOM_MaterialParam::ParamType_VectorArray;
		}
		arraySizeStr = parseArrayDim ("float3", str);
		if (!arraySizeStr.empty ())
		{
			return ATOM_MaterialParam::ParamType_VectorArray;
		}
		arraySizeStr = parseArrayDim ("float2", str);
		if (!arraySizeStr.empty ())
		{
			return ATOM_MaterialParam::ParamType_VectorArray;
		}
		arraySizeStr = parseArrayDim ("float4x4", str);
		if (!arraySizeStr.empty ())
		{
			return ATOM_MaterialParam::ParamType_Matrix44Array;
		}
		arraySizeStr = parseArrayDim ("float4x3", str);
		if (!arraySizeStr.empty ())
		{
			return ATOM_MaterialParam::ParamType_Matrix43Array;
		}
		arraySizeStr = parseArrayDim ("int", str);
		if (!arraySizeStr.empty ())
		{
			return ATOM_MaterialParam::ParamType_IntArray;
		}
		return ATOM_MaterialParam::ParamType_Unknown;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////

ATOM_Material::ATOM_Material (void)
{
	_matId = -1;
}

ATOM_Material::~ATOM_Material (void)
{
	_effectInuse.clear();
	_layerInuse.clear();
}

ATOM_Material::ATOM_Material (const ATOM_Material &other)
{
	setCoreMaterial (other._coreMaterial.get());
	_coreMaterialFileName = other._coreMaterialFileName;

	//--- wangjian added ---//
	_effectInuse	= other._effectInuse;
	_layerInuse		= other._layerInuse;
	_matId			= other._matId;
	//----------------------//
}

ATOM_Material &ATOM_Material::operator = (const ATOM_Material &other)
{
	setCoreMaterial (other._coreMaterial.get());
	_coreMaterialFileName = other._coreMaterialFileName;

	//--- wangjian added ---//
	_effectInuse	= other._effectInuse;
	_layerInuse		= other._layerInuse;
	_matId			= other._matId;
	//----------------------//

	return *this;
}

ATOM_CoreMaterial *ATOM_Material::getCoreMaterial (void)
{
	return _coreMaterial.get();
}

const char *ATOM_Material::getCoreMaterialFileName (void) const
{
	return _coreMaterialFileName.c_str();
}

void ATOM_Material::setCoreMaterialFileName (const char *filename)
{
	_coreMaterialFileName = filename ? filename : "";
}

bool ATOM_Material::isSameAs (ATOM_Material *other) const
{
	if (_coreMaterial != other->_coreMaterial)
	{
		return false;
	}

	if (_parameterTable != other->_parameterTable)
	{
		if (!_parameterTable || !other->_parameterTable)
		{
			return false;
		}

		if (!_parameterTable->hasSameParametersAs (other->_parameterTable.get()))
		{
			return false;
		}
	}

	return true;
}

void ATOM_Material::setCoreMaterial (ATOM_CoreMaterial *coreMaterial)
{
	if (_coreMaterial != coreMaterial)
	{
		_parameterTable = coreMaterial ? coreMaterial->getDefaultParameterTable()->clone() : 0;
		_activeEffect = 0;
		_coreMaterial = coreMaterial;
	}
}

ATOM_MaterialEffect *ATOM_Material::getActiveEffect (void)
{
	return _activeEffect.get();
}

void ATOM_Material::setActiveEffect (const char *name)
{
	if (_coreMaterial)
	{
		_activeEffect = _coreMaterial->getEffectByName (name);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
//--- wangjian modified ---//
#if 0

int ATOM_Material::getValidEffectIndex (ATOM_RenderScheme *scheme, int schemeLayer) const
{
	if (_coreMaterial)
	{
		for (int i = 0; i < _coreMaterial->getNumEffects (); ++i)
		{
			ATOM_MaterialEffect *effect = _coreMaterial->getEffect(i);
			if (!effect->validate (scheme, schemeLayer))
			{
				continue;
			}
			if (!_parameterTable->isEffectValid (i))
			{
				continue;
			}
			return i;
		}
	}
	return -1;
}

void ATOM_Material::autoSetActiveEffect (void)
{
	_activeEffect = 0;

	if (_coreMaterial)
	{
		for (unsigned i = 0; i < _coreMaterial->getNumEffects (); ++i)
		{
			ATOM_MaterialEffect *effect = _coreMaterial->getEffect(i);
			if (!effect->validate (ATOM_RenderScheme::getCurrentRenderScheme(), ATOM_RenderScheme::getCurrentRenderSchemeLayer()))
			{
				continue;
			}
			if (!_parameterTable->isEffectValid (i))
			{
				continue;
			}
			_activeEffect = effect;
			return;
		}
	}
}

#else

void ATOM_Material::setActiveEffect(int index)
{
	ATOM_ASSERT(index>=0 && index < _coreMaterial->getNumEffects() );
	_activeEffect = _coreMaterial->getEffect(index);
}

int ATOM_Material::getValidEffectIndex (ATOM_RenderScheme *scheme, int schemeLayer, int & debuginfo)
{
	debuginfo = 0;


	// ������effect������в��Ҷ�Ӧscheme��layer��effect
#if 0
	unsigned key = ( ((unsigned)scheme) << 16 ) + ( schemeLayer < 0 ? 1000 : schemeLayer );
#else
	UINT64 key = ( ((UINT64)scheme) << 32 ) + (UINT64)( schemeLayer < 0 ? 1000 : schemeLayer );
#endif

	// ����������validation����ֵû�б仯 �� ��scheme��layerû����ʹ�õļ�����
	if( !_parameterTable->isDirty(scheme,schemeLayer) )
	{
		if( _layerInuse.find(key) == _layerInuse.end() )
			return -1;
	}

	// ��ʹ�õ�effect�����в���
	bool bSchemeFound = false;

	EFFECT_MAP::iterator iter = _effectInuse.find(key);
	if( iter != _effectInuse.end() )
	{
		// �ҵ���
		bSchemeFound = true;

		// ����������validation����ֵû�б仯 ֱ�ӷ��ظ�effect������
		if( !_parameterTable->isDirty(scheme,schemeLayer) )
		{
			if( iter->second.enabled )
			{
				// ��effect��������� �� ��effect���õ���
				ATOM_AUTOPTR(ATOM_MaterialEffect) effect = _coreMaterial->getEffect(iter->second.id);
				if( effect && effect->isEnable() )
				{	
					// ��û�У� ����
					if( _layerInuse.find(key) == _layerInuse.end() )
						_layerInuse.insert(key);

					return iter->second.id;
				}
			}
			else
			{
				if( _layerInuse.find(key) != _layerInuse.end() )
					_layerInuse.erase(key);

				return -1;
			}
		}
	}

	// ����ҵ��� ���Ⱦ��Ǽ������ҵ���
	if( bSchemeFound )
	{
		int index = iter->second.id;
		if( index >= 0 )
		{
			if (_parameterTable->isEffectValid (index))
			{
				iter->second.enabled = 1;
				_layerInuse.insert(key);
				return index;
			}
		}

		// �����ʹ�ù���EFFECT�����в���
		ATOM_SET<int>::iterator iter_mru = iter->second._mru.begin();
		for(;iter_mru != iter->second._mru.end();++iter_mru)
		{
			index = *iter_mru;
			if( index >= 0 )
			{
				if (_parameterTable->isEffectValid (index))
				{
					iter->second.id = index;
					iter->second.enabled = 1;
					_layerInuse.insert(key);
					return index;
				}
			}
		}
	}

	//----------------------//

	bool bRemoveLayer = true;

	// ���� ����ѡ����ʵ�EFFECT
	if (_coreMaterial)
	{
		// �����ڸ�layer��ѡ����ʵ�EFFECT
		ATOM_DEQUE<int> * layerEffects = _coreMaterial->getRenderSchemeLayerEffects(scheme,schemeLayer);
		if( !layerEffects )
		{
			for (int i = 0; i < _coreMaterial->getNumEffects (); ++i)
			{
				ATOM_AUTOPTR(ATOM_MaterialEffect) effect = _coreMaterial->getEffect(i);
				if ( effect && !effect->validate (scheme, schemeLayer))
				{
					continue;
				}
				if (!_parameterTable->isEffectValid (i))
				{
					if( !effect->isEnable() )
						bRemoveLayer = false;

					continue;
				}

				// �ҵ��� ��������EFFECT ���� �����µ�EFFECT
				if( bSchemeFound )
				{
					iter->second.id = i;
					iter->second.enabled = 1;
				}
				else
				{
					sEffectUse eu;
					eu.id = i;
					eu.enabled = 1;
					_effectInuse.insert( std::make_pair(key,eu) );
				}
				//----------------------//

				// insert into the most recently used set
				_effectInuse[key]._mru.insert(i);
				_layerInuse.insert(key);

				return i;
			}
		}
		else
		{
			int effectid = -1;
			int best_effect_id = -1;
			int best_effect_index = -1;

			for(int i = 0; i < layerEffects->size(); ++i)
			{
				effectid = (*layerEffects)[i];
				ATOM_ASSERT(effectid>=0);

				if (!_parameterTable->isEffectValid (effectid))
				{
					ATOM_AUTOPTR(ATOM_MaterialEffect) effect = _coreMaterial->getEffect(effectid);
					if( effect && !effect->isEnable() )
						bRemoveLayer = false;

					continue;
				}

				best_effect_id = effectid;
				best_effect_index = i;

				// �ҵ��� ��������EFFECT ���� �����µ�EFFECT
				if( bSchemeFound )
				{
					iter->second.id = effectid;
					iter->second.enabled = 1;
				}
				else
				{
					sEffectUse eu;
					eu.id = effectid;
					eu.enabled = 1;
					_effectInuse.insert( std::make_pair(key,eu) );
				}
				
				if( best_effect_index != 0 )
				{
					layerEffects->erase(layerEffects->begin()+best_effect_index);
					layerEffects->push_front(best_effect_id);
				}

				// insert into the most recently used set
				_effectInuse[key]._mru.insert(effectid);

				_layerInuse.insert(key);

				debuginfo = 1;

				return effectid;
			}
		}

		// ���û���ҵ� �� �������validation����ֵû�б仯
		if( !bSchemeFound && !_parameterTable->isDirty(scheme,schemeLayer) )
		{
			sEffectUse eu;
			eu.id = -1;
			eu.enabled = 0;
			_effectInuse.insert( std::make_pair(key,eu) );
		}
		else if( bSchemeFound && _parameterTable->isDirty(scheme,schemeLayer) )
		{
			iter->second.enabled = 0;
		}
	}

	if( bRemoveLayer && _layerInuse.find(key) != _layerInuse.end() )
		_layerInuse.erase(key);

	return -1;
}
void ATOM_Material::autoSetActiveEffect (int queueid/*=-1*/)
{
	_activeEffect = 0;

	ATOM_RenderScheme * renderScheme = ATOM_RenderScheme::getCurrentRenderScheme();
	unsigned int renderLayer;
	if( queueid < 0 )
		renderLayer = ATOM_RenderScheme::getCurrentRenderSchemeLayer() < 0 ? 1000 : ATOM_RenderScheme::getCurrentRenderSchemeLayer();
	else
		renderLayer = queueid;

	// ������ʹ�õ�EFFECT���в��Ҷ�Ӧ��scheme�͸�renderqueue��EFFECT
#if 0
	unsigned key = ( (unsigned)renderScheme << 16 ) + renderLayer;
#else
	UINT64 key = ( (UINT64)renderScheme << 32 ) + (UINT64)renderLayer;
#endif

	EFFECT_MAP::iterator iter = _effectInuse.find(key);

	bool bSchemeFound = false;
	if( iter != _effectInuse.end() )
	{
		// �ҵ���
		bSchemeFound = true;

		// ����������VALIDATION����ֵû�б仯 ֱ�ӷ��ظ�EFFECT������
		if( !_parameterTable->isDirty() )
		{
			if( iter->second.enabled && iter->second.id >= 0 )
				_activeEffect = _coreMaterial->getEffect(iter->second.id);

			return;
		}
	}
	// ����ҵ��� ���Ⱦ��Ǽ������ҵ���
	if( bSchemeFound )
	{
		int index = iter->second.id;
		if( index >= 0 )
		{
			if (_parameterTable->isEffectValid (index))
			{
				_parameterTable->setDirty(0);

				ATOM_MaterialEffect *effect = _coreMaterial->getEffect(index);
				_activeEffect = effect;
				iter->second.enabled = 1;

				return;
			}

		}

		// �����ʹ�ù���EFFECT�����в���
		ATOM_SET<int>::iterator iter_mru = iter->second._mru.begin();
		for(;iter_mru != iter->second._mru.end();++iter_mru)
		{
			index = *iter_mru;
			if( index >= 0 )
			{
				if (_parameterTable->isEffectValid (index))
				{
					_parameterTable->setDirty(0);

					ATOM_MaterialEffect *effect = _coreMaterial->getEffect(index);
					_activeEffect = effect;

					iter->second.id = index;
					iter->second.enabled = 1;

					return;
				}
			}
		}
	}

	//----------------------//

	// ���� ����ѡ��EFFECT
	if (_coreMaterial)
	{
		// �����ڸ�layer��ѡ����ʵ�EFFECT
		//int renderSchemeLayer = ATOM_RenderScheme::getCurrentRenderSchemeLayer();
		ATOM_DEQUE<int> * layerEffects = _coreMaterial->getRenderSchemeLayerEffects(renderScheme,renderLayer/*renderSchemeLayer*/);
		if( !layerEffects )
		{
			for (unsigned i = 0; i < _coreMaterial->getNumEffects (); ++i)
			{
				ATOM_MaterialEffect *effect = _coreMaterial->getEffect(i);

				if (!effect->validate (renderScheme, renderLayer/*ATOM_RenderScheme::getCurrentRenderSchemeLayer()*/))
				{
					continue;
				}

				if (!_parameterTable->isEffectValid (i))
				{
					continue;
				}

				_activeEffect = effect;

				_parameterTable->setDirty(0);

				// �ҵ��� ��������EFFECT ���� �����µ�EFFECT
				if( bSchemeFound )
				{
					iter->second.enabled = 1;
					iter->second.id = i;
				}
				else
				{
					sEffectUse eu;
					eu.enabled = 1;
					eu.id = i;
#if 0
					_effectInuse.insert( std::make_pair(key,eu) );
#else

#endif
				}

				// insert into the most recently used set
				_effectInuse[key]._mru.insert(i);

				return;
			}
		}
		else
		{
			int effectid = -1;
			int best_effect_id = -1;
			int best_effect_index = -1;

			for(int i = 0; i < layerEffects->size(); ++i)
			{
				effectid = (*layerEffects)[i];
				ATOM_ASSERT(effectid>=0);

				ATOM_MaterialEffect *effect = _coreMaterial->getEffect(effectid);

				if (!_parameterTable->isEffectValid (effectid))
				{
					continue;
				}

				_activeEffect = effect;

				best_effect_id = effectid;
				best_effect_index = i;

				_parameterTable->setDirty(0);

				// �ҵ��� ��������EFFECT ���� �����µ�EFFECT
				if( bSchemeFound )
				{
					iter->second.enabled = 1;
					iter->second.id = effectid;
				}
				else
				{
					sEffectUse eu;
					eu.enabled = 1;
					eu.id = effectid;
					_effectInuse.insert( std::make_pair(key,eu) );
				}

				if( best_effect_index != 0 )
				{
					layerEffects->erase( layerEffects->begin() + best_effect_index );
					layerEffects->push_front( best_effect_id );
				}

				// insert into the most recently used set
				_effectInuse[key]._mru.insert(effectid);
				
				return;
			}
		}

		// ���û���ҵ� �� �������validation����ֵû�б仯
		if( !bSchemeFound && !_parameterTable->isDirty() )
		{
			sEffectUse eu;
			eu.enabled = 0;
			eu.id = -1;
			_effectInuse.insert( std::make_pair(key,eu) );
		}
		else if( bSchemeFound && _parameterTable->isDirty() )
		{
			iter->second.enabled = 0; 
		}
	}
}

#endif
////////////////////////////////////////////////////////////////////////////////


ATOM_ParameterTable *ATOM_Material::getParameterTable (void)
{
	if (_coreMaterial)
	{
		return _parameterTable.get();
	}
	return 0;
}

unsigned ATOM_Material::begin (ATOM_RenderDevice *device)
{
	if (_coreMaterial)
	{
		if (!_activeEffect || _coreMaterial->isAutoSelectEffect())
		{
			autoSetActiveEffect ();
			if (!_activeEffect)
			{
				return 0;
			}
		}

		//--- wangjian modified ---//
		return _activeEffect->begin (device, _parameterTable.get());
		//-------------------------//
	}
	return 0;
}

void ATOM_Material::end (ATOM_RenderDevice *device)
{
	if (_activeEffect)
	{
		_activeEffect->end (device);
	}
}

bool ATOM_Material::beginPass (ATOM_RenderDevice *device, unsigned pass)
{
	if (_activeEffect)
	{
		//--- wangjian added ---//
		//for (	ATOM_HASHMAP<ATOM_STRING, ATOM_AUTOPTR(ATOM_MaterialParam)>::const_iterator it = _coreMaterial->_params.begin(); 
		//		it != _coreMaterial->_params.end(); 
		//		++it	)
		//{
		//	//if( it->second->validatePass(pass) != 0 )
		//	{
		//		it->second->apply ( _activeEffect.get()/*, pass*/ );
		//	}
		//}
		//----------------------//

		return _activeEffect->beginPass (device, _parameterTable.get(), pass);
	}
	return false;
}

void ATOM_Material::endPass (ATOM_RenderDevice *device, unsigned pass)
{
	if (_activeEffect)
	{
		_activeEffect->endPass (device, pass);
	}
}

static void logInvalidParamValue (const char *function, const char *name, const char *value)
{
	ATOM_LOGGER::warning ("%s: Invalid material parameter <%s=%s>\n", function, name, value);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// wangjian modified

// ���ز��ʲ�����ʱ��˳�����ø����õ�������ʱ���м�������Ĳ������������һ������������ȼ��Ĳ���
#if 0

bool ATOM_Material::loadXML (ATOM_RenderDevice *device, const char *filename)
{
	// read XML file
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

	if (!loadString (device, str))
	{
		ATOM_LOGGER::error ("%s(0x%08X,%s): Load material failed.\n", __FUNCTION__, device, filename);
		return false;
	}

	return true;
}

bool ATOM_Material::loadString (ATOM_RenderDevice *device, const char *str)
{
	// parse XML file
	ATOM_TiXmlDocument doc;
	doc.Parse (str);
	if (doc.Error())
	{
		ATOM_LOGGER::error ("%s: Parse XML failed.\n", __FUNCTION__);
		return false;
	}

	ATOM_TiXmlElement *root = doc.RootElement();
	if (!root || stricmp(root->Value(), "material"))
	{
		// No root element named "material" found
		ATOM_LOGGER::error ("%s: No <material> root element found.\n", __FUNCTION__);
		return false;
	}

	const char *coreMaterialFileName = root->Attribute ("core");
	if (!coreMaterialFileName)
	{
		// No core material specified
		ATOM_LOGGER::error ("%s: No <core> attribute found.\n", __FUNCTION__);
		return false;
	}

	ATOM_AUTOREF(ATOM_CoreMaterial) coreMaterial = ATOM_MaterialManager::loadCoreMaterial (device, coreMaterialFileName);
	if (!coreMaterial)
	{
		ATOM_LOGGER::error ("%s: Load core material failed.\n", __FUNCTION__);
		return false;
	}

	setCoreMaterial (coreMaterial.get());
	_coreMaterialFileName = coreMaterialFileName;

	_parameterTable->loadFromXML (root);

	return true;
}

#else

bool ATOM_Material::loadXML (ATOM_RenderDevice *device, const char *filename, int texLoadPriority/* = ATOM_LoadPriority_ASYNCBASE*/)
{
	// read XML file
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

	if ( !loadString (device, str, texLoadPriority ) )
	{
		ATOM_LOGGER::error ("%s(0x%08X,%s): Load material failed.\n", __FUNCTION__, device, filename);
		return false;
	}

	return true;
}
bool ATOM_Material::loadString (ATOM_RenderDevice *device, const char *str, int texLoadPriority/* = ATOM_LoadPriority_ASYNCBASE*/)
{
	// parse XML file
	ATOM_TiXmlDocument doc;
	doc.Parse (str);
	if (doc.Error())
	{
		ATOM_LOGGER::error ("%s: Parse XML failed.\n", __FUNCTION__);
		return false;
	}

	ATOM_TiXmlElement *root = doc.RootElement();
	if (!root || stricmp(root->Value(), "material"))
	{
		// No root element named "material" found
		ATOM_LOGGER::error ("%s: No <material> root element found.\n", __FUNCTION__);
		return false;
	}

	const char *coreMaterialFileName = root->Attribute ("core");
	if (!coreMaterialFileName)
	{
		// No core material specified
		ATOM_LOGGER::error ("%s: No <core> attribute found.\n", __FUNCTION__);
		return false;
	}

	ATOM_AUTOREF(ATOM_CoreMaterial) coreMaterial = ATOM_MaterialManager::loadCoreMaterial (device, coreMaterialFileName);
	if (!coreMaterial)
	{
		ATOM_LOGGER::error ("%s: Load core material failed.\n", __FUNCTION__);
		return false;
	}

	setCoreMaterial (coreMaterial.get());
	_coreMaterialFileName = coreMaterialFileName;

	// ��������ļ������ȼ�
	_parameterTable->loadFromXML ( root, texLoadPriority );

	return true;
}

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ATOM_Material::saveXML (const char *filename) const
{
	// parse XML file
	char nativeFileName[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (filename, nativeFileName);
	ATOM_TiXmlDocument doc (nativeFileName);

	ATOM_TiXmlElement root("material");

	root.SetAttribute ("core", _coreMaterialFileName.c_str());

	_parameterTable->saveToXML (&root);

	doc.InsertEndChild (root);

	return doc.SaveFile ();
}

ATOM_AUTOPTR(ATOM_Material) ATOM_Material::clone (void) const
{
	ATOM_AUTOPTR(ATOM_Material) material = ATOM_NEW (ATOM_Material);
	material->setCoreMaterial (_coreMaterial.get());
	material->_parameterTable = _parameterTable->clone ();
	material->_coreMaterialFileName = _coreMaterialFileName;

	//--- wangjian added ---//
	material->_effectInuse = _effectInuse;
	material->_layerInuse = _layerInuse;
	material->_matId = _matId;
	//----------------------//

	return material;
}


//--- wangjian added ---//
// ���ò��ʵ�ȫ��ID��
void ATOM_Material::setMaterialId(ATOM_STRING strId)
{
	_matId = ATOM_MaterialManager::appendMaterialId(strId.c_str());
}
unsigned ATOM_Material::getMaterialId(void) const
{
	return _matId;
}
//----------------------//
