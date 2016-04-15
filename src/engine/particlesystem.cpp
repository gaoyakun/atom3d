#include "StdAfx.h"
#include "particlesystem.h"

ATOM_IMPLEMENT_NODE_FILE_TYPE(ATOM_ParticleSystem)

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_ParticleSystem)
  ATOM_ATTRIBUTES_BEGIN(ATOM_ParticleSystem)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "ParticleTexture", getTextureFileName, setTextureFileName, "", "group=ATOM_ParticleSystem;type=vfilename;desc='粒子贴图'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "ColorTexture", getColorFileName, setColorFileName, "", "group=ATOM_ParticleSystem;type=vfilename;desc='粒子颜色贴图'")
	
	//--- wangjian modified ---//
	//ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "Directional", att_getDirectional, att_setDirectional, 0, "group=ATOM_ParticleSystem;type=bool;desc='调整粒子方向'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "Directional", att_getDirectional, att_setDirectional, 0, "group=ATOM_ParticleSystem;type=int32;enum='无 0 速度方向 1 竖直方向 2 水平方向 3';desc='调整粒子方向'")
	//-------------------------//
	
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "WorldSpace", att_getWorldSpace, att_setWorldSpace, 1, "group=ATOM_ParticleSystem;type=bool;desc='粒子拖尾'")
    ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "MaxParticleCount", getMaxParticleCount, setMaxParticleCount, 500, "group=ATOM_ParticleSystem;desc='最大粒子数'")
    ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "UpdateInterval", getUpdateInterval, setUpdateInterval, 50, "group=ATOM_ParticleSystem;desc='更新间隔'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "Delay", getDelay, setDelay, 0, "group=ATOM_ParticleSystem;desc='延迟播放'")
    ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "EmitInterval", getEmitInterval, setEmitInterval, 20, "group=ATOM_ParticleSystem;desc='发射间隔'")
    ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "EmitCount", getEmitCount, setEmitCount, 3, "group=ATOM_ParticleSystem;desc='发射数量'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "Gravity", getGravity, setGravity, ATOM_Vector3f(0, 0, 0), "group=ATOM_ParticleSystem;desc='重力'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "Wind", getWind, setWind, ATOM_Vector3f(0, 0, 0), "group=ATOM_ParticleSystem;desc='风力'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "Scalar", getScalar, setScalar, 1.f, "group=ATOM_ParticleSystem;desc='缩放'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "Aspect", getAspect, setAspect, 1.f, "group=ATOM_ParticleSystem;desc='粒子宽高比'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "JitterSpeed", getJitterSpeed, setJitterSpeed, 1.f, "group=ATOM_ParticleSystem;desc='抖动速度'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "JitterPower", getJitterPower, setJitterPower, 0.f, "group=ATOM_ParticleSystem;desc='抖动强度'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "Rotation", getRotationVelocity, setRotationVelocity, 0.f, "group=ATOM_ParticleSystem;desc='旋转速度'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "RotationVar", getRotationVelocityVar, setRotationVelocityVar, 0.f, "group=ATOM_ParticleSystem;desc='旋转速度随机'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "EmitterShape", att_getEmitterShape, att_setEmitterShape, 0, "group=ATOM_ParticleSystem;type=int32;enum='点 0 球 1 盒子 2 圆柱 3 圆锥 4';desc='发射器形状'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "ParticleForm", att_getForm, att_setForm, ATOM_ParticleSystem::PF_SPRITE, "group=ATOM_ParticleSystem;type=int32;enum='公告板 0 线 1';desc='粒子形态'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "EmitterBehavior", att_getEmitterBehavior, att_setEmitterBehavior, 0, "group=ATOM_ParticleSystem;type=int32;enum='表面 0 内部 1';desc='发射方式'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "EmitterConeRadius", getEmitterConeRadius, setEmitterConeRadius, 0.f, "group=ATOM_ParticleSystem;desc='发射角'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "EmitterConeRadiusVar", getEmitterConeRadiusVar, setEmitterConeRadiusVar, 0.1f, "group=ATOM_ParticleSystem;desc='发射角随机'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "EmitterSize", getEmitterShapeSize, setEmitterShapeSize, ATOM_Vector3f(1.f, 1.f, 1.f), "group=ATOM_ParticleSystem;desc='发射器大小'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "EmitterSizeVar", getEmitterShapeSizeVar, setEmitterShapeSizeVar, ATOM_Vector3f(0.f, 0.f, 0.f), "group=ATOM_ParticleSystem;desc='发射器大小随机'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "Velocity", getParticleVelocity, setParticleVelocity, 8.f, "group=ATOM_ParticleSystem;desc='粒子速度'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "VelocityVar", getParticleVelocityVar, setParticleVelocityVar, 2.f, "group=ATOM_ParticleSystem;desc='粒子速度随机'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "Life", getParticleLife, setParticleLife, 3000.f, "group=ATOM_ParticleSystem;desc='粒子寿命'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "LifeVar", getParticleLifeVar, setParticleLifeVar, 500.f, "group=ATOM_ParticleSystem;desc='粒子寿命随机'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "Size1", getParticleSize1, setParticleSize1, 1.f, "group=ATOM_ParticleSystem;desc='粒子初始大小'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "Size1Var", getParticleSize1Var, setParticleSize1Var, 0.1f, "group=ATOM_ParticleSystem;desc='粒子初始大小随机'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "Size2", getParticleSize2, setParticleSize2, 0.f, "group=ATOM_ParticleSystem;desc='粒子结束大小'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "Size2Var", getParticleSize2Var, setParticleSize2Var, 0.1f, "group=ATOM_ParticleSystem;desc='粒子结束大小随机'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "Accel", getParticleAccel, setParticleAccel, -0.01f, "group=ATOM_ParticleSystem;desc='粒子加速度'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "AccelVar", getParticleAccelVar, setParticleAccelVar, -0.01f, "group=ATOM_ParticleSystem;desc='粒子加速度随机'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "Alpha", getTransparency, setTransparency, 1.f, "group=ATOM_ParticleSystem;desc='透明度'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "Color", getColor, setColor, ATOM_Vector4f(1.f,1.f,1.f,1.f), "group=ATOM_ParticleSystem;type=rgba;desc='颜色'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "BlendMode", att_getBlendMode, att_setBlendMode, 0, "group=ATOM_ParticleSystem;type=int32;enum='A+1 0 A+A 1 1+1 2';desc='粒子Alpha模式'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_ParticleSystem, "ColorMultiplier", getColorMultiplier, setColorMultiplier, 1.f, "group=ATOM_ParticleSystem;desc='颜色强度因子'")	// wangjian added
  ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_ParticleSystem, ATOM_Node)

class NoiseVectorTable
{
	ATOM_VECTOR<ATOM_Vector3f> _v;

public:
	NoiseVectorTable (unsigned size): _v(size * 2)
	{
		PerlinNoise noise;

		noise.NewPrimeNumbers();
		for (unsigned i = 0; i < size; ++i)
		{
			_v[i].x = noise.PerlinNoise1D (i, 10.f, 0.1f);
		}

		noise.NewPrimeNumbers();
		for (unsigned i = 0; i < size; ++i)
		{
			_v[i].y = noise.PerlinNoise1D (i, 10.f, 0.1f);
		}

		noise.NewPrimeNumbers();
		for (unsigned i = 0; i < size; ++i)
		{
			_v[i].z = noise.PerlinNoise1D (i, 10.f, 0.1f);
		}

		for (unsigned i = 0; i < size; ++i)
		{
			_v[size+i] = _v[size-1-i];
		}

		for (unsigned i = 0; i < _v.size(); ++i)
		{
			_v[i].normalize ();
		}
	}

	ATOM_Vector3f getVector (float factor) const
	{
		int i = ATOM_ftol((factor - ATOM_floor (factor)) * _v.size()) % _v.size();
		return _v[i];
	}
};

static NoiseVectorTable noiseTable(128);

unsigned particlesysCount = 0;

ATOM_ParticleSystem::ATOM_ParticleSystem (void)
{
	ATOM_STACK_TRACE(ATOM_ParticleSystem::ATOM_ParticleSystem);

	particlesysCount++;

	_activeParticleList = 0;
	_freeParticleList = 0;
	_maxParticleCount = 100;
	_emitInterval = 100;
	_emitCount = 1;
	_activeParticleCount = 0;
	_gravity.set (0.f, 0.f, 0.f);
	_wind.set (0.f, 0.f, 0.f);
	_startEmitTime = 0;
	_lastUpdateTime = 0;
	_numEmitCount = 0;
	_scalar = 1.f;
	_aspect = 1.f;
	_airResistence = false;
	_updateInterval = 50;
	_needUpdateVertexArray = false;
	_paused = true;
	_starttick = 0;
	_delay = 0;
	_blendMode = 0;
	_rotation = 0.f;
	_rotationVar = 0.f;
	_jitterSpeed = 1.f;
	_jitterPower = 0.f;
	_emitterShape = ES_POINT;
	_particleForm = PF_SPRITE;
	_emitterBehavior = EB_SURFACE;
	_emitterConeRadius = 0.f;
	_emitterConeRadiusVar = 0.1f;
	_emitterShapeSize.set (1.f, 1.f, 1.f);
	_emitterShapeSizeVar.set (0.f, 0.f, 0.f);
	_colorValue.set (1.f,1.f,1.f,1.f);
	_particleVelocity = 0.2f;
	_particleVelocityVar = 0.2f;
	_particleLife = 1.f;
	_particleLifeVar = 0.5f;
	_particleSize1 = 0.4f;
	_particleSize1Var = 0.1f;
	_particleSize2 = 0.f;
	_particleSize2Var = 0.1f;
	_particleAccel = -0.01f;
	_particleAccelVar = -0.01f;
	_transparency = 1.f;
	_flags = ATOM_ParticleSystem::PS_WORLDSPACE;

	//--- wangjian added ---//
	_colorMultiplier = 1.0f;
	_directionType = ATOM_ParticleSystem::DT_NONE;		// 默认没有方向性
	//----------------------//

	_geometry = ATOM_NEW(ATOM_MultiStreamGeometry);
}

ATOM_ParticleSystem::~ATOM_ParticleSystem (void)
{
	ATOM_STACK_TRACE(ATOM_ParticleSystem::~ATOM_ParticleSystem);

	particlesysCount--;

	purgeParticles ();

	ATOM_DELETE(_geometry);
}

void ATOM_ParticleSystem::accept (ATOM_Visitor &visitor)
{
	ATOM_STACK_TRACE(ATOM_ParticleSystem::accept);

	visitor.visit (*this);
}

bool ATOM_ParticleSystem::onLoad(ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_ParticleSystem::onLoad);

	//--- wangjian modified ---//

#if 0

	if (!getNodeFileName().empty ())
	{
		const char *fileName = getNodeFileName().c_str();
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

		TiXmlElement *element = doc.RootElement();
		if (!loadAttribute (element))
		{
			ATOM_LOGGER::error ("<%s> load attribute failed (%s)\n", __FUNCTION__, fileName);
			return false;
		}
	}

	if (!_material && !createMaterial())
	{
		return false;
	}

#else

	if (!getNodeFileName().empty ())
	{
		// 是否开启了异步加载模式
		bool bMT = ATOM_AsyncLoader::IsRun(); // && _load_priority != ATOM_LoadPriority_IMMEDIATE;

		//==============================================================
		// 如果直接加载
		//==============================================================
		if(!bMT)
		{
			bool createNew = false;
			_attribFile = ATOM_LookupOrCreateObject (ATOM_NodeAttributeFile::_classname(), getNodeFileName().c_str(), &createNew);
			return loadSync();
		}
		//==============================================================
		// 如果异步加载 
		//==============================================================
		else
		{
			const char * fileName = getNodeFileName().c_str();

			//---------------------------------------------
			// 查找该节点属性文件对象
			//---------------------------------------------
			bool createNew = false;
			_attribFile = ATOM_LookupOrCreateObject (ATOM_NodeAttributeFile::_classname(), fileName, &createNew);

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
																fileName,
																this, 
																loadPrioriy,
																_attribFile ? _attribFile->getAsyncLoader() : 0,
																ATOM_LOADRequest_Event::/*FLAG_SHOW*/FLAG_NOT_SHOW ) );

				if( ATOM_AsyncLoader::isEnableLog() )
				{
					ATOM_LOGGER::debug( "<%s>: submit load request for attribute file %s of Composition node [ Address : %x, priority %d ]\n", 
										__FUNCTION__,
										fileName,
										_attribFile ? (unsigned)(_attribFile->getAsyncLoader()) : 0,
										loadPrioriy );
				}

				// 如果是新创建的对象 异步加载之
				if( createNew )
				{
					return _attribFile->loadAsync(fileName,loadPrioriy);
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
				_load_flag = LOAD_ATTRIBUTE_LOADED;

				ATOM_ASSERT(_attribFile.get());

				// 如果出错 设置加载完成 退出
				if( _attribFile->getAsyncLoader()->IsLoadFailed() )
				{
					_load_flag = LOAD_ALLFINISHED;
					return false;
				}

				const ATOM_TiXmlElement * root = _attribFile->getDocument().RootElement();
				if (!root)
				{
					return false;
				}
				
				int pri_saved = _load_priority;	// 先保存加载优先级 因为loadAttribute会改变_load_priority的值
				if (!loadAttribute (root))
				{
					return false;
				}
				_load_priority = pri_saved;		// 恢复加载优先级

				if (!_material && !createMaterial())
				{
					return false;
				}

				return true;
			}
		}
	}

	// 创建材质
	if (!_material && !createMaterial())
	{
		return false;
	}

#endif

	return true;
}

void ATOM_ParticleSystem::buildBoundingbox (void) const
{
	ATOM_STACK_TRACE(ATOM_ParticleSystem::buildBoundingbox);

	if (_flags & PS_WORLDSPACE)
	{
		_boundingBox = getWorldBoundingbox ();
		_boundingBox.transform (getInvWorldMatrix ());
	}
	else
	{
		_boundingBox = _bbox;
	}
}

void ATOM_ParticleSystem::buildWorldBoundingbox (void) const
{
	ATOM_STACK_TRACE(ATOM_ParticleSystem::buildWorldBoundingbox);

	if (_flags & PS_WORLDSPACE)
	{
		if (_activeParticleCount > 0)
		{
			_boundingBoxW = _bbox;
		}
		else
		{
			const ATOM_Vector3f center = getWorldMatrix().getRow3 (3);
			const ATOM_Vector3f ext(0.5f, 0.5f, 0.5f);
			_boundingBoxW.setMin (center - ext);
			_boundingBoxW.setMax (center + ext);
		}
	}
	else
	{
		_boundingBoxW = getBoundingbox ();
		_boundingBoxW.transform (getWorldMatrix ());
	}
}

void ATOM_ParticleSystem::setFlags (int flags)
{
	if (_flags != flags)
	{
		_flags = flags;
		_needUpdateVertexArray = true;
		_material = 0;
	}
}

int ATOM_ParticleSystem::getFlags (void) const
{
	return _flags;
}

void ATOM_ParticleSystem::setMaxParticleCount (unsigned count)
{
	if (count != _maxParticleCount)
	{
		if (count > _maxParticleCount)
		{
			_indices = 0;
		}

		_maxParticleCount = count;
		//resizeVertexArray ();
		invalidateBoundingbox ();
	}
}

unsigned ATOM_ParticleSystem::getMaxParticleCount (void) const
{
	return _maxParticleCount;
}

void ATOM_ParticleSystem::setEmitInterval (unsigned interval)
{
	if (interval != _emitInterval)
	{
		_emitInterval = interval ? interval : 1;
		_startEmitTime = 0;
	}
}

unsigned ATOM_ParticleSystem::getEmitInterval (void) const
{
	return _emitInterval;
}

void ATOM_ParticleSystem::setEmitCount (unsigned count)
{
	_emitCount = count;
}

unsigned ATOM_ParticleSystem::getEmitCount (void) const
{
	return _emitCount;
}

void ATOM_ParticleSystem::setGravity (const ATOM_Vector3f &gravity)
{
	if (_gravity != gravity)
	{
		_gravity = gravity;
		invalidateBoundingbox ();
	}
}

const ATOM_Vector3f &ATOM_ParticleSystem::getGravity (void) const
{
	return _gravity;
}

void ATOM_ParticleSystem::setWind (const ATOM_Vector3f &wind)
{
	if (_wind != wind)
	{
		_wind = wind;
		_airResistence = (_wind.x != 0 || _wind.y != 0 || _wind.z != 0);
		invalidateBoundingbox ();
	}
}

const ATOM_Vector3f &ATOM_ParticleSystem::getWind (void) const
{
	return _wind;
}

void ATOM_ParticleSystem::setTextureFileName (const ATOM_STRING &filename)
{
	ATOM_STACK_TRACE(ATOM_ParticleSystem::setTextureFileName);

	_textureFileName = filename;

	if (!_textureFileName.empty ())
	{
		ATOM_RenderDevice *device = ATOM_GetRenderDevice();

		//--- wangjian modified ---//
		// 异步加载 ：添加纹理异步加载优先级
		//bool bMT = ( ATOM_AsyncLoader::IsRun() ) && ( _load_priority != ATOM_LoadPriority_IMMEDIATE );
		ATOM_AUTOREF(ATOM_Texture) newTexture = ATOM_CreateTextureResource (	_textureFileName.c_str(), ATOM_PIXEL_FORMAT_UNKNOWN, 
																				_load_priority	);
		
		
		//if( newTexture && bMT )
		//{
		//	// 添加资源请求
		//	ATOM_LOADRequest_Event* event_load( ATOM_NEW(ATOM_LOADRequest_Event,_textureFileName.c_str(),this,_load_priority) );
		//	//model->getAsyncLoader()->AddListener(this);
		//	event_load->_obj = newTexture->getAsyncLoader();
		//	//event_load->_task_assigned = true;
		//	_load_flag = LOAD_LOADSTARTED; // 加载开始
		//}
		
		//-------------------------//

		if (newTexture != _texture)
		{
			_texture = newTexture;
			_material = 0;
		}
	}
	else if (_texture)
	{
		_texture = 0;
		_material = 0;
	}
}

const ATOM_STRING &ATOM_ParticleSystem::getTextureFileName (void) const
{
	return _textureFileName;
}

void ATOM_ParticleSystem::setColorFileName (const ATOM_STRING &filename)
{
	ATOM_STACK_TRACE(ATOM_ParticleSystem::setColorFileName);

	_colorFileName = filename;

	if (!_colorFileName.empty ())
	{
		ATOM_RenderDevice *device = ATOM_GetRenderDevice();
		//--- wangjian modified ---//
		// 异步加载 ：设置纹理加载优先级
		//bool bMT = ( ATOM_AsyncLoader::IsRun() ) && ( _load_priority != ATOM_LoadPriority_IMMEDIATE );
		ATOM_AUTOREF(ATOM_Texture) newTexture = ATOM_CreateTextureResource (	_colorFileName.c_str(), ATOM_PIXEL_FORMAT_UNKNOWN, 
																				_load_priority	);
		//-------------------------//
		if (_color != newTexture)
		{
			_color = newTexture;
			_material = 0;
		}
	}
	else if (_color)
	{
		_color = 0;
		_material = 0;
	}
}

const ATOM_STRING &ATOM_ParticleSystem::getColorFileName (void) const
{
	return _colorFileName;
}

void ATOM_ParticleSystem::setRotationVelocity (float val)
{
	_rotation = val;
}

float ATOM_ParticleSystem::getRotationVelocity (void) const
{
	return _rotation;
}

void ATOM_ParticleSystem::setRotationVelocityVar (float val)
{
	_rotationVar = val;
}

float ATOM_ParticleSystem::getRotationVelocityVar (void) const
{
	return _rotationVar;
}

void ATOM_ParticleSystem::setUpdateInterval (unsigned interval)
{
	_updateInterval = interval;
}

unsigned ATOM_ParticleSystem::getUpdateInterval (void) const
{
	return _updateInterval;
}

void ATOM_ParticleSystem::reset (bool clearParticles)
{
	ATOM_STACK_TRACE(ATOM_ParticleSystem::reset);

	if (clearParticles)
	{
		_freeParticleList = _activeParticleList;
		_activeParticleCount = 0;
	}

	_startEmitTime = 0;
	_lastUpdateTime = 0;
	_numEmitCount = 0;
	_paused = true;

	_starttick = ATOM_APP->getFrameStamp().currentTick;
}

void ATOM_ParticleSystem::update (const ATOM_Matrix4x4f &viewMatrix, const ATOM_Matrix4x4f &projectionMatrix)
{
	ATOM_STACK_TRACE(ATOM_ParticleSystem::update);

	//--- wangjian added ---//
	// 异步加载
	if( ! checkAsyncLoadFlag() )
		return;
	//----------------------//

	//if (_paused)
	//{
	//	_pausedTime += ATOM_APP->getFrameStamp().elapsedTick;
	//	return;
	//}

	unsigned tick = ATOM_APP->getFrameStamp().currentTick;
	if (_starttick == 0)
	{
		_starttick = tick;
	}

	if (_delay && tick - _starttick < _delay)
	{
		return;
	}

	if (_lastUpdateTime == 0)
	{
		_lastUpdateTime = tick;
	}
	unsigned updateElapsed = tick - _lastUpdateTime;
	if (updateElapsed < _updateInterval)
	{
		return;
	}
	//else if (updateElapsed > 1000)
	//{
	//	// prevent pause too much time causing too many particles emitted
	//	tick = _lastUpdateTime + 1000;
	//	updateElapsed = 1000;
	//}

	invalidateBoundingbox ();

	_bbox.beginExtend ();

	_needUpdateVertexArray = true;

	float updateElapsedf = updateElapsed * 0.001f;
	_lastUpdateTime = tick;

	ParticleNode *activeParticles = _activeParticleList;
	_activeParticleCount = 0;

	while (activeParticles && activeParticles != _freeParticleList)
	{
		ATOM_Particle *p = &activeParticles->particle;
		activeParticles->elapsedTime += updateElapsed;
		float age = float(activeParticles->elapsedTime)/float(p->lifeSpan);

		if (age >= 1)
		{
			// The particle is dead
			_freeParticleList = activeParticles;
			break;
		}
		else
		{
			++_activeParticleCount;

			p->velocity += _gravity * updateElapsedf;

			if (_airResistence)
			{
				p->velocity += (_wind - p->velocity) * updateElapsedf;
			}

			float len = p->velocity.getLength ();
			if (len > 0.0001f)
			{
				float s = 1.f + updateElapsedf * p->acceleration / len;
				if (s < 0.f) s = 0.f;
				p->velocity *= s;
			}

			p->position += updateElapsedf * p->velocity * _scalar;

			activeParticles->jitterAngle = (activeParticles->elapsedTime + p->lifeSpan * activeParticles->ageBias) * _jitterSpeed * 0.001f;

			_bbox.extend (p->position);

			activeParticles->size = p->size1 + p->size2 * age;

			activeParticles->rotation += p->rotation * updateElapsedf;

			activeParticles = activeParticles->next;
		}
	}

	int newParticleCount = 0;

	if (_startEmitTime == 0)
	{
		newParticleCount = (_emitCount > _maxParticleCount) ? _maxParticleCount : _emitCount;
		_numEmitCount = 1;
		_startEmitTime = tick;
	}
	else
	{
		unsigned emitElapsed = tick - _startEmitTime;
		int count = emitElapsed / _emitInterval + 1;
		if (count > _numEmitCount)
		{
			newParticleCount = _emitCount;
			_numEmitCount = count;

			if (_activeParticleCount + newParticleCount > _maxParticleCount)
			{
				newParticleCount = _maxParticleCount - _activeParticleCount;
			}
		}
	}

	if (newParticleCount > 0)
	{
		const ATOM_Matrix4x4f &worldmatrix = getWorldMatrix ();
		ATOM_Matrix4x4f itworldmatrix = worldmatrix;
		itworldmatrix.m30 = 0.f;
		itworldmatrix.m31 = 0.f;
		itworldmatrix.m32 = 0.f;
		itworldmatrix.invertAffine ();
		itworldmatrix.transpose ();

		newParticle (newParticleCount, tick, viewMatrix, projectionMatrix, worldmatrix, itworldmatrix);
	}
}

void ATOM_ParticleSystem::updateVertexArrays (ATOM_Camera *camera)
{
	ATOM_STACK_TRACE(ATOM_ParticleSystem::updateVertexArrays);

	if (!_material && !createMaterial())
	{
		return;
	}

	if (_activeParticleCount > 0)
	{
		resizeVertexArray ();
	}

	if (_needUpdateVertexArray && _activeParticleCount > 0)
	{
		{
			ATOM_Vector4f *vertices = (ATOM_Vector4f*)_vertices->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
			ParticleNode *activeParticles = _activeParticleList;
			while (activeParticles && activeParticles != _freeParticleList)
			{
				vertices->x = activeParticles->particle.position.x;
				vertices->y = activeParticles->particle.position.y;
				vertices->z = activeParticles->particle.position.z;
				vertices->w = 0;
				vertices++;
				vertices->x = activeParticles->particle.position.x;
				vertices->y = activeParticles->particle.position.y;
				vertices->z = activeParticles->particle.position.z;
				vertices->w = 1;
				vertices++;
				vertices->x = activeParticles->particle.position.x;
				vertices->y = activeParticles->particle.position.y;
				vertices->z = activeParticles->particle.position.z;
				vertices->w = 2;
				vertices++;
				vertices->x = activeParticles->particle.position.x;
				vertices->y = activeParticles->particle.position.y;
				vertices->z = activeParticles->particle.position.z;
				vertices->w = 3;
				vertices++;

				activeParticles = activeParticles->next;
			}
			_vertices->unlock ();
		}

		{
			ATOM_Vector4f *uvs = (ATOM_Vector4f*)_texcoords->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
			const ParticleNode *activeParticles = _activeParticleList;
			while (activeParticles && activeParticles != _freeParticleList)
			{
				float age = float(activeParticles->elapsedTime)/float(activeParticles->particle.lifeSpan);

				uvs->x = activeParticles->size;
				uvs->y = activeParticles->rotation;
				uvs->z = activeParticles->jitterAngle;
				uvs->w = age;
				uvs++;
				uvs->x = activeParticles->size;
				uvs->y = activeParticles->rotation;
				uvs->z = activeParticles->jitterAngle;
				uvs->w = age;
				uvs++;
				uvs->x = activeParticles->size;
				uvs->y = activeParticles->rotation;
				uvs->z = activeParticles->jitterAngle;
				uvs->w = age;
				uvs++;
				uvs->x = activeParticles->size;
				uvs->y = activeParticles->rotation;
				uvs->z = activeParticles->jitterAngle;
				uvs->w = age;
				uvs++;
				activeParticles = activeParticles->next;
			}
			_texcoords->unlock ();
		}

		//if ((_flags & PS_DIRECTIONAL_PARTICLE) != 0 || (_flags & PS_JITTER) != 0)
		{
			ATOM_Vector3f *vel = (ATOM_Vector3f*)_velocities->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
			const ParticleNode *activeParticles = _activeParticleList;
			while (activeParticles && activeParticles != _freeParticleList)
			{
				//--- wangjian modified ---//
				if( _directionType == DT_HORIZON )
				{
					*vel++ = ATOM_Vector3f(1,0,0);
					*vel++ = ATOM_Vector3f(1,0,0);
					*vel++ = ATOM_Vector3f(1,0,0);
					*vel++ = ATOM_Vector3f(1,0,0);					
				}
				else if( _directionType == DT_VERTICAL )
				{
					*vel++ = ATOM_Vector3f(0,1,0);
					*vel++ = ATOM_Vector3f(0,1,0);
					*vel++ = ATOM_Vector3f(0,1,0);
					*vel++ = ATOM_Vector3f(0,1,0);
				}
				else
				{
					*vel++ = activeParticles->particle.velocity;
					*vel++ = activeParticles->particle.velocity;
					*vel++ = activeParticles->particle.velocity;
					*vel++ = activeParticles->particle.velocity;
				}
				//-------------------------//
				
				activeParticles = activeParticles->next;
			}
			_velocities->unlock ();
		}

		_needUpdateVertexArray = false;
	}
}

unsigned ATOM_ParticleSystem::getActiveParticleCount (void) const
{
	return _activeParticleCount;
}

ATOM_Material *ATOM_ParticleSystem::getMaterial (void)
{
	ATOM_STACK_TRACE(ATOM_ParticleSystem::getMaterial);

	if (!_material && !createMaterial ())
	{
		return 0;
	}

	return _material.get();
}

ATOM_ParticleSystem::ParticleNode *ATOM_ParticleSystem::retireParticle (ParticleNode *particle)
{
	ParticleNode *ret = particle->next;
	--_activeParticleCount;

	if (particle != _activeParticleList)
	{
		particle->elapsedTime = _activeParticleList->elapsedTime;
		particle->particle = _activeParticleList->particle;
		particle->size = _activeParticleList->size;
	}

	ParticleNode *p = _activeParticleList->next;
	_activeParticleList->next = _freeParticleList;
	_freeParticleList = _activeParticleList;
	_activeParticleList = p;

	return ret;
}

void ATOM_ParticleSystem::newParticle (unsigned num, unsigned currentTick, const ATOM_Matrix4x4f &viewMatrix, const ATOM_Matrix4x4f &projectionMatrix, const ATOM_Matrix4x4f &worldMatrix, const ATOM_Matrix4x4f &itworldMatrix)
{
	for (unsigned i = 0; i < num; ++i)
	{
		if (_freeParticleList && _freeParticleList->next)
		{
			ParticleNode *pTmp = _freeParticleList->next->next;
			_freeParticleList->next->next = _activeParticleList;
			_activeParticleList = _freeParticleList->next;
			_freeParticleList->next = pTmp;
		}
		else
		{
			ParticleNode *p = ATOM_NEW(ParticleNode);
			p->next = _activeParticleList;
			_activeParticleList = p;
		}

		_activeParticleList->elapsedTime = 0;
		_activeParticleList->rotation = 0.f;

		ATOM_Particle *particle = &_activeParticleList->particle;
		particle->acceleration = 1.f;

		initParticle (particle, currentTick - _startEmitTime);

		particle->size1 *= _scalar;
		particle->size2 *= _scalar;
		particle->size2 -= particle->size1;

		_activeParticleList->size = particle->size1;
		_activeParticleList->rotation = 0.f;

		if (_flags & PS_WORLDSPACE)
		{
			particle->position <<= worldMatrix;
			particle->velocity = worldMatrix.transformVector (particle->velocity);
		}

		_bbox.extend (particle->position);

		_activeParticleList->ageBias = float(rand ()) / float(RAND_MAX);

		++_activeParticleCount;
	}
}

void ATOM_ParticleSystem::purgeParticles (void)
{
	ATOM_STACK_TRACE(ATOM_ParticleSystem::purgeParticles);

	while (_activeParticleList)
	{
		ParticleNode *p = _activeParticleList->next;
		ATOM_DELETE (_activeParticleList);
		_activeParticleList = p;
	}

	_freeParticleList = 0;
}

void ATOM_ParticleSystem::resizeVertexArray (void)
{
	ATOM_STACK_TRACE(ATOM_ParticleSystem::resizeVertexArray);

	ATOM_RenderDevice *device = ATOM_GetRenderDevice ();

	if (!_vertices || _vertices->getNumVertices() < _activeParticleCount * 4)
	{
		if (_vertices)
		{
			_geometry->removeStream (_vertices.get());
		}

		_vertices = device->allocVertexArray (ATOM_VERTEX_ATTRIB_COORD_XYZW, ATOM_USAGE_DYNAMIC, (_activeParticleCount + 50) * 4);
		_geometry->addStream (_vertices.get());

		if (_texcoords)
		{
			_geometry->removeStream (_texcoords.get());
		}

		_texcoords = device->allocVertexArray (ATOM_VERTEX_ATTRIB_TEX1_4, ATOM_USAGE_DYNAMIC, (_activeParticleCount + 50) * 4);
		_geometry->addStream (_texcoords.get());
	}

	if (!_velocities || _velocities->getNumVertices() < _activeParticleCount * 4)
	{
		if (_velocities)
		{
			_geometry->removeStream (_velocities.get());
		}

		_velocities = device->allocVertexArray (ATOM_VERTEX_ATTRIB_TEX2_3, ATOM_USAGE_DYNAMIC, (_activeParticleCount + 50) * 4);
		_geometry->addStream (_velocities.get());
	}

	if (!_indices)
	{
		switch (_particleForm)
		{
		case PF_SPRITE:
			{
				_indices = device->allocIndexArray (ATOM_USAGE_STATIC, _maxParticleCount * 6, false, true);
				unsigned short *p = (unsigned short*)_indices->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
				for (unsigned short i = 0; i < _maxParticleCount; ++i)
				{
					*p++ = i * 4 + 0;
					*p++ = i * 4 + 1;
					*p++ = i * 4 + 2;
					*p++ = i * 4 + 0;
					*p++ = i * 4 + 2;
					*p++ = i * 4 + 3;
				}
				_indices->unlock ();
				break;
			}
		case PF_LINE:
			{
				_indices = device->allocIndexArray (ATOM_USAGE_STATIC, _maxParticleCount * 4, false, true);
				unsigned short *p = (unsigned short*)_indices->lock (ATOM_LOCK_WRITEONLY, 0, 0, true);
				for (unsigned short i = 0; i < _maxParticleCount; ++i)
				{
					*p++ = i * 4 + 3;
					*p++ = i * 4 + 2;
					*p++ = i * 4 + 0;
					*p++ = i * 4 + 1;
				}
				_indices->unlock ();
				break;
			}
		}

		_geometry->setIndices (_indices.get());
	}
}

bool ATOM_ParticleSystem::createMaterial (void)
{
	ATOM_STACK_TRACE(ATOM_ParticleSystem::createMaterial);

	_material = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/particlesys.mat");

	if (!_material)
	{
		_material = 0;
		return false;
	}

	if (_texture)
	{
		_material->getParameterTable()->setTexture ("particleTexture", _texture.get());
	}

	if (_color)
	{
		_material->getParameterTable()->setTexture ("colorTexture", _color.get());
	}


	return true;
}

float ATOM_ParticleSystem::computeSquaredDistanceToCamera (ATOM_Camera *camera, const ATOM_Matrix4x4f &worldMatrix) const
{
	const ATOM_Matrix4x4f &m = getWorldMatrix ();
	return (camera->getViewMatrix().getRow3(3) - m.getRow3(3)).getSquaredLength();
}

bool ATOM_ParticleSystem::draw(ATOM_RenderDevice* device, ATOM_Camera *camera, ATOM_Material *material)
{
	//--- wangjian modified ---//
	/*_material->getParameterTable()->setInt ("enableDirectional", ((_flags & ATOM_ParticleSystem::PS_DIRECTIONAL_PARTICLE) ? 1 : 0));
	_material->getParameterTable()->setInt ("enableWorldSpace", ((_flags & ATOM_ParticleSystem::PS_WORLDSPACE) ? 1 : 0));
	_material->getParameterTable()->setInt ("hasTexture2D", ((_texture && (_texture->getFlags() & ATOM_Texture::TEXTURE2D)) ? 1 : 0));
	_material->getParameterTable()->setInt ("hasTexture3D", ((_texture && (_texture->getFlags() & ATOM_Texture::TEXTURE3D)) ? 1 : 0));
	_material->getParameterTable()->setInt ("hasColorTexture", (_color ? 1 : 0));*/
	//-------------------------//

	_material->getParameterTable()->setInt ("zfunc", ATOM_RenderAttributes::CompareFunc_LessEqual);
	_material->getParameterTable()->setInt ("zwrite", 0);
	_material->getParameterTable()->setInt ("enableAlphaBlend", 1);
	switch (_blendMode)
	{
	case 0:
		_material->getParameterTable()->setInt ("srcblend", ATOM_RenderAttributes::BlendFunc_SrcAlpha);
		_material->getParameterTable()->setInt ("destblend", ATOM_RenderAttributes::BlendFunc_One);
		break;
	case 1:
		_material->getParameterTable()->setInt ("srcblend", ATOM_RenderAttributes::BlendFunc_SrcAlpha);
		_material->getParameterTable()->setInt ("destblend", ATOM_RenderAttributes::BlendFunc_InvSrcAlpha);
		break;
	case 2:
		_material->getParameterTable()->setInt ("srcblend", ATOM_RenderAttributes::BlendFunc_One);
		_material->getParameterTable()->setInt ("destblend", ATOM_RenderAttributes::BlendFunc_One);
		break;
	}
	_material->getParameterTable()->setTexture ("particleTexture", _texture.get());
	_material->getParameterTable()->setTexture ("colorTexture", _color.get());
	_material->getParameterTable()->setFloat ("jitterPower", _jitterPower);
	_material->getParameterTable()->setFloat ("transparency", _transparency);
	_material->getParameterTable()->setVector ("color", _colorValue);
	//--- wangjian added ---//
	_material->getParameterTable()->setFloat ("colorMultiplier", _colorMultiplier);
	//----------------------//

	ATOM_Matrix4x4f matProj;
	ATOM_Matrix4x4f matView;
	device->getTransform (ATOM_MATRIXMODE_PROJECTION, matProj);
	device->getTransform (ATOM_MATRIXMODE_VIEW, matView);
	ATOM_Vector3f viewDirection = -matView.getRow3 (2);
	ATOM_Vector3f viewUp;
	if (ATOM_abs(viewDirection.y) > 0.8f)
	{
		viewUp.set(0.f, 0.f, 1.f);
	}
	else
	{
		viewUp.set(0.f, 1.f, 0.f);
	}
	ATOM_Vector3f viewXAxis = crossProduct (viewUp, viewDirection);
	viewUp = crossProduct (viewDirection, viewXAxis);

	if ((_flags & PS_WORLDSPACE) == 0)
	{
		ATOM_Matrix4x4f invWorld = getInvWorldMatrix ();
		invWorld.setRow3 (3, 0.f, 0.f, 0.f);
		viewDirection <<= invWorld;
		viewUp <<= invWorld;
		viewXAxis <<= invWorld;
	}

	float scaleX = _aspect;
	float scaleY = 1.f;
	if (_aspect > 1.f)
	{
		scaleX = 1.f;
		scaleY /= _aspect;
	}

	ATOM_Matrix4x4f  preRotatedQuad;
	if (_flags & PS_DIRECTIONAL_PARTICLE)
	{
		preRotatedQuad.setRow (0, -scaleX,  scaleY, 0.f, 0.f);
		preRotatedQuad.setRow (1,  scaleX,  scaleY, 0.f, 0.f);
		preRotatedQuad.setRow (2,  scaleX, -scaleY, 0.f, 0.f);
		preRotatedQuad.setRow (3, -scaleX, -scaleY, 0.f, 0.f);
	}
	else
	{
		preRotatedQuad.setRow (0, -viewXAxis * scaleX + viewUp * scaleY, 0.f);
		preRotatedQuad.setRow (1,  viewXAxis * scaleX + viewUp * scaleY, 0.f);
		preRotatedQuad.setRow (2,  viewXAxis * scaleX - viewUp * scaleY, 0.f);
		preRotatedQuad.setRow (3, -viewXAxis * scaleX - viewUp * scaleY, 0.f);
	}
	material->getParameterTable()->setMatrix44 ("preRotatedQuad", preRotatedQuad);

	ATOM_Matrix4x4f texCoords;
	texCoords.setRow (0, 0.f, 0.f, 0.f, 0.f);
	texCoords.setRow (1, 1.f, 0.f, 0.f, 0.f);
	texCoords.setRow (2, 1.f, 1.f, 0.f, 0.f);
	texCoords.setRow (3, 0.f, 1.f, 0.f, 0.f);

	material->getParameterTable()->setMatrix44 ("texCoords", texCoords);

	//--- wangjian modified ---//
	if (_flags & PS_DIRECTIONAL_PARTICLE )
	{
		
		ATOM_Vector4f viewDir (viewDirection.x, viewDirection.y, viewDirection.z, 0.f);

		// if horizontal
		if(_directionType == DT_HORIZON )
		{
			viewDir = ATOM_Vector4f(0,1,0,1);
		}

		material->getParameterTable()->setVector ("viewDirection", viewDir);
	}
	//-------------------------//

	//--- wangjian added ---//
	int effect_id = getEffectIdFront(ATOM_RenderScheme::getCurrentRenderScheme());
	if( effect_id >= 0 )
	{
		material->setActiveEffect(effect_id);
		material->getCoreMaterial()->setAutoSelectEffect(false);
	}
	//----------------------//

	unsigned numPasses = material->begin (device);
	for (unsigned pass = 0; pass < numPasses; ++pass)
	{
		if (material->beginPass (device, pass))
		{
			switch (_particleForm)
			{
			case PF_SPRITE:
				{
					_geometry->draw (device, ATOM_PRIMITIVE_TRIANGLES, _activeParticleCount * 2, 0);
					break;
				}
			case PF_LINE:
				{
					_geometry->draw (device, ATOM_PRIMITIVE_TRIANGLE_STRIP, _activeParticleCount * 4 - 2, 0);
					break;
				}
			}

			material->endPass (device, pass);
		}
	}
	material->end (device);

	//--- wangjian added ---//
	if( effect_id >= 0 )
		material->getCoreMaterial()->setAutoSelectEffect(true);
	//----------------------//

	return true;
	//device->ClearStreamSource ();
	//device->SetStreamSource (_vertices.get(), VERTEX_ATTRIB_COORD_XYZW);
	//device->SetStreamSource (_texcoords.get(), VERTEX_ATTRIB_TEX1_4);
	//if ((_flags & PS_DIRECTIONAL_PARTICLE) != 0 || (_flags & PS_JITTER) != 0)
	//{
	//	device->SetStreamSource (_velocities.get(), VERTEX_ATTRIB_TEX2_3);
	//}

	//device->RenderStreamsIndexed (_indices.get(), ATOM_PRIMITIVE_TRIANGLES, _activeParticleCount * 2);
}

void ATOM_ParticleSystem::setScalar (float scalar)
{
	if (_scalar != scalar)
	{
		_scalar = scalar;
		invalidateBoundingbox ();
	}
}

float ATOM_ParticleSystem::getScalar (void) const
{
	return _scalar;
}

void ATOM_ParticleSystem::setAspect (float aspect)
{
	_aspect = aspect;
}

float ATOM_ParticleSystem::getAspect (void) const
{
	return _aspect;
}

void ATOM_ParticleSystem::setJitterSpeed (float speed)
{
	_jitterSpeed = speed;
}

float ATOM_ParticleSystem::getJitterSpeed (void) const
{
	return _jitterSpeed;
}

void ATOM_ParticleSystem::setJitterPower (float power)
{
	if (power != _jitterPower)
	{
		if (power == 0.f || _jitterPower == 0.f)
		{
			_material = 0;
		}
		_jitterPower = power;
	}
}

float ATOM_ParticleSystem::getJitterPower (void) const
{
	return _jitterPower;
}

void ATOM_ParticleSystem::pause (void)
{
	_paused = true;
}

void ATOM_ParticleSystem::resume (void)
{
	_paused = false;
}

bool ATOM_ParticleSystem::paused (void) const
{
	return _paused;
}

bool ATOM_ParticleSystem::supportFixedFunction (void) const
{
	return false;
}

unsigned ATOM_ParticleSystem::getStartTick (void) const
{
	return _starttick;
}

void ATOM_ParticleSystem::att_setEmitterShape (int shape)
{
	setEmitterShape ((EmitterShape)shape);
}

int ATOM_ParticleSystem::att_getEmitterShape (void) const
{
	return getEmitterShape ();
}

void ATOM_ParticleSystem::att_setEmitterBehavior (int behavior)
{
	setEmitterBehavior ((EmitterBehavior)behavior);
}

int ATOM_ParticleSystem::att_getEmitterBehavior (void) const
{
	return getEmitterBehavior ();
}
//------- wangjian modified -----------//
#if 0
void ATOM_ParticleSystem::att_setDirectional (int directional)
{
	if (directional)
	{
		setFlags (getFlags() | ATOM_ParticleSystem::PS_DIRECTIONAL_PARTICLE);
	}
	else
	{
		setFlags (getFlags() & ~ATOM_ParticleSystem::PS_DIRECTIONAL_PARTICLE);
	}
}
int ATOM_ParticleSystem::att_getDirectional (void) const
{
	return (getFlags () & ATOM_ParticleSystem::PS_DIRECTIONAL_PARTICLE) ? 1 : 0;
}
#else
void ATOM_ParticleSystem::att_setDirectional (int directionType)
{
	_directionType = directionType;

	if (directionType)
	{
		setFlags (getFlags() | ATOM_ParticleSystem::PS_DIRECTIONAL_PARTICLE);
	}
	else
	{
		setFlags (getFlags() & ~ATOM_ParticleSystem::PS_DIRECTIONAL_PARTICLE);
	}
}
int ATOM_ParticleSystem::att_getDirectional (void) const
{
	return _directionType;
}
#endif
//------------------------------------------------------------------------------//

void ATOM_ParticleSystem::att_setWorldSpace (int worldSpace)
{
	if (worldSpace)
	{
		setFlags (getFlags() | ATOM_ParticleSystem::PS_WORLDSPACE);
	}
	else
	{
		setFlags (getFlags() & ~ATOM_ParticleSystem::PS_WORLDSPACE);
	}
}

int ATOM_ParticleSystem::att_getWorldSpace (void) const
{
	return (getFlags () & ATOM_ParticleSystem::PS_WORLDSPACE) ? 1 : 0;
}

void ATOM_ParticleSystem::att_setForm (int form)
{
	setParticleForm ((ParticleForm)form);
}

int ATOM_ParticleSystem::att_getForm (void) const
{
	return getParticleForm ();
}

void ATOM_ParticleSystem::setEmitterShape (EmitterShape shape)
{
	_emitterShape = shape;
}

ATOM_ParticleSystem::EmitterShape ATOM_ParticleSystem::getEmitterShape (void) const
{
	return _emitterShape;
}

void ATOM_ParticleSystem::setParticleForm (ParticleForm form)
{
	if (_particleForm != form)
	{
		_indices = 0;
		_particleForm = form;
	}
}

ATOM_ParticleSystem::ParticleForm ATOM_ParticleSystem::getParticleForm (void) const
{
	return _particleForm;
}

void ATOM_ParticleSystem::setEmitterBehavior (ATOM_ParticleSystem::EmitterBehavior behavior)
{
	_emitterBehavior = behavior;
}

ATOM_ParticleSystem::EmitterBehavior ATOM_ParticleSystem::getEmitterBehavior (void) const
{
	return _emitterBehavior;
}

void ATOM_ParticleSystem::setEmitterConeRadius (float value)
{
	_emitterConeRadius = value;
}

float ATOM_ParticleSystem::getEmitterConeRadius (void) const
{
	return _emitterConeRadius;
}

void ATOM_ParticleSystem::setEmitterConeRadiusVar (float value)
{
	_emitterConeRadiusVar = value;
}

float ATOM_ParticleSystem::getEmitterConeRadiusVar (void) const
{
	return _emitterConeRadiusVar;
}

//void ATOM_ParticleSystem::setEmitterRate (float value)
//{
//	_emitterRate = value;
//}
//
//float ATOM_ParticleSystem::getEmitterRate (void) const
//{
//	return _emitterRate;
//}
//
//void ATOM_ParticleSystem::setEmitterRateVar (float value)
//{
//	_emitterRateVar = value;
//}
//
//float ATOM_ParticleSystem::getEmitterRateVar (void) const
//{
//	return _emitterRateVar;
//}
//
void ATOM_ParticleSystem::setEmitterShapeSize (const ATOM_Vector3f &size)
{
	_emitterShapeSize = size;
}

const ATOM_Vector3f &ATOM_ParticleSystem::getEmitterShapeSize (void) const
{
	return _emitterShapeSize;
}

void ATOM_ParticleSystem::setEmitterShapeSizeVar (const ATOM_Vector3f &size)
{
	_emitterShapeSizeVar = size;
}

const ATOM_Vector3f &ATOM_ParticleSystem::getEmitterShapeSizeVar (void) const
{
	return _emitterShapeSizeVar;
}

void ATOM_ParticleSystem::setParticleVelocity (float value)
{
	_particleVelocity = value;
}

float ATOM_ParticleSystem::getParticleVelocity (void) const
{
	return _particleVelocity;
}

void ATOM_ParticleSystem::setParticleVelocityVar (float value)
{
	_particleVelocityVar = value;
}

float ATOM_ParticleSystem::getParticleVelocityVar (void) const
{
	return _particleVelocityVar;
}

void ATOM_ParticleSystem::setParticleLife (float value)
{
	_particleLife = value;
}

float ATOM_ParticleSystem::getParticleLife (void) const
{
	return _particleLife;
}

void ATOM_ParticleSystem::setParticleLifeVar (float value)
{
	_particleLifeVar = value;
}

float ATOM_ParticleSystem::getParticleLifeVar (void) const
{
	return _particleLifeVar;
}

void ATOM_ParticleSystem::setParticleSize1 (float value)
{
	_particleSize1 = value;
}

float ATOM_ParticleSystem::getParticleSize1 (void) const
{
	return _particleSize1;
}

void ATOM_ParticleSystem::setParticleSize1Var (float value)
{
	_particleSize1Var = value;
}

float ATOM_ParticleSystem::getParticleSize1Var (void) const
{
	return _particleSize1Var;
}

void ATOM_ParticleSystem::setParticleSize2 (float value)
{
	_particleSize2 = value;
}

float ATOM_ParticleSystem::getParticleSize2 (void) const
{
	return _particleSize2;
}

void ATOM_ParticleSystem::setParticleSize2Var (float value)
{
	_particleSize2Var = value;
}

float ATOM_ParticleSystem::getParticleSize2Var (void) const
{
	return _particleSize2Var;
}

void ATOM_ParticleSystem::setParticleAccel (float value)
{
	_particleAccel = value;
}

float ATOM_ParticleSystem::getParticleAccel (void) const
{
	return _particleAccel;
}

void ATOM_ParticleSystem::setParticleAccelVar (float value)
{
	_particleAccelVar = value;
}

float ATOM_ParticleSystem::getParticleAccelVar (void) const
{
	return _particleAccelVar;
}

void ATOM_ParticleSystem::initParticle (ATOM_Particle *particle, unsigned emitTime)
{
	getParticleInitialPosition (&particle->position, &particle->velocity);
	particle->size1 = _particleSize1 + ATOM_randomf(0.f, _particleSize1Var);
	particle->size2 = _particleSize2 + ATOM_randomf(0.f, _particleSize2Var);
	particle->rotation = _rotation + ATOM_randomf(0.f, _rotationVar);
	particle->lifeSpan = _particleLife + ATOM_randomf(0.f, _particleLifeVar);
	particle->acceleration = _particleAccel + ATOM_randomf(0.f, _particleAccelVar);
}

void ATOM_ParticleSystem::getParticleInitialPosition (ATOM_Vector3f *pos, ATOM_Vector3f *vel) const
{
	if (_emitterShape == ES_POINT)
	{
		pos->set (0.f, 0.f, 0.f);
		float coneRadius = _emitterConeRadius + ATOM_randomf(0.f, _emitterConeRadiusVar);
		vel->set (ATOM_randomf(-coneRadius, coneRadius), 1.f, ATOM_randomf(-coneRadius, coneRadius));
	}
	else
	{
		ATOM_Vector3f shapeSize = _emitterShapeSize + _emitterShapeSizeVar * ATOM_Vector3f(ATOM_randomf(0.f, 1.f), ATOM_randomf(0.f, 1.f), ATOM_randomf(0.f, 1.f));

		switch (_emitterShape)
		{
		case ES_SPHERE:
			{
				float r, x, y, z;
				float alpha = ATOM_randomf (0.f, ATOM_Pi);
				float theta = ATOM_randomf (0.f, ATOM_TwoPi);
				ATOM_sincos (alpha, &r, &y);
				ATOM_sincos (theta, &x, &z);
				x *= r;
				z *= r;

				pos->x = x * shapeSize.x;
				pos->y = y * shapeSize.y;
				pos->z = z * shapeSize.z;
				*vel = *pos;

				if (_emitterBehavior == EB_VOLUME)
				{
					float t = ATOM_randomf(0.01f, 1.f);
					pos->x *= t;
					pos->y *= t;
					pos->z *= t;
				}
				break;
			}
		case ES_BOX:
			{
				float x = ATOM_randomf (-1.f, 1.f);
				float y = ATOM_randomf (-1.f, 1.f);
				float z = ATOM_randomf (-1.f, 1.f);
				if (_emitterBehavior == EB_VOLUME)
				{
					float t = ATOM_max3 (ATOM_abs(x), ATOM_abs(y), ATOM_abs(z));
					if (t != 0.f)
					{
						x /= t;
						y /= t;
						z /= t;
					}
				}
				pos->x = x * shapeSize.x;
				pos->y = y * shapeSize.y;
				pos->z = z * shapeSize.z;

				float coneRadius = _emitterConeRadius + ATOM_randomf(0.f, _emitterConeRadiusVar);
				vel->set (ATOM_randomf(-coneRadius, coneRadius), 1.f, ATOM_randomf(-coneRadius, coneRadius));

				break;
			}

		case ES_CYLINDER:
			{
				float x, y, z;
				float alpha = ATOM_randomf (0.f, ATOM_TwoPi);
				ATOM_sincos (alpha, &x, &z);
				y = ATOM_randomf (-1.f, 1.f);

				float coneRadius = _emitterConeRadius + ATOM_randomf(0.f, _emitterConeRadiusVar);
				vel->x = x;
				vel->y = ATOM_randomf (-coneRadius, coneRadius);
				vel->z = z;

				if (_emitterBehavior == EB_VOLUME)
				{
					float factor = ATOM_randomf (0.f, 1.f);
					x *= factor;
					z *= factor;
				}

				pos->x = x;
				pos->y = y;
				pos->z = z;

				(*pos) *= shapeSize;
				(*vel) *= shapeSize;

				break;
			}
		case ES_CONE:
			{
				float alpha = ATOM_randomf (0.f, ATOM_TwoPi);
				float scale = ATOM_randomf (0.f, 1.f);
				float s, c;
				ATOM_sincos (alpha, &s, &c);
				pos->x = s * scale;
				pos->z *= c * scale;
				pos->y = 2.f - 2.f * scale;

				(*pos) *= shapeSize;

				float t = shapeSize.y * shapeSize.y / ATOM_sqrt(shapeSize.y * shapeSize.y + shapeSize.x * shapeSize.x);
				ATOM_Vector3f vS = ATOM_Vector3f(s, 0.f, c) * shapeSize.x;
				(*vel) = (vS - ATOM_Vector3f(0.f, shapeSize.y, 0.f)) * t;

				break;
			}
		}
	}

	vel->normalize ();
	(*vel) *= (_particleVelocity + ATOM_randomf(0.f, _particleVelocityVar));
}

void ATOM_ParticleSystem::setTransparency (float alpha)
{
	_transparency = alpha;
}

float ATOM_ParticleSystem::getTransparency (void) const
{
	return _transparency;
}

void ATOM_ParticleSystem::setDelay (unsigned timeInMs)
{
	_delay = timeInMs;
}

unsigned ATOM_ParticleSystem::getDelay (void) const
{
	return _delay;
}

void ATOM_ParticleSystem::setColor (const ATOM_Vector4f &color)
{
	_colorValue = color;
}

const ATOM_Vector4f &ATOM_ParticleSystem::getColor (void) const
{
	return _colorValue;
}

void ATOM_ParticleSystem::att_setBlendMode (int mode)
{
	_blendMode = mode;
}

int ATOM_ParticleSystem::att_getBlendMode (void) const
{
	return _blendMode;
}

//--- wangjian added ---//

void ATOM_ParticleSystem::setColorMultiplier (float colorMultiplier)
{
	_colorMultiplier = colorMultiplier;
}

const float ATOM_ParticleSystem::getColorMultiplier (void) const
{
	return _colorMultiplier;
}

// 异步加载 ：检查异步加载完成标记
bool ATOM_ParticleSystem::checkNodeAllFinshed()
{
	// 如果开启场景异步加载 且 当前加载标记为'加载尚未开始'
	if( ATOM_AsyncLoader::isEnableSceneMTLoad() && _load_flag < LOAD_LOADSTARTED )
		return true;

	return checkAsyncLoadFlag();
}

// 加载属性文件结束
void ATOM_ParticleSystem::onLoadFinished()
{
	ATOM_ASSERT(_attribFile.get());

	// 如果出错 设置加载完成 退出
	if( _attribFile->getAsyncLoader()->IsLoadFailed() )
	{
		_load_flag = LOAD_ALLFINISHED;
		return;
	}

	_load_flag = LOAD_ATTRIBUTE_LOADED;

	const ATOM_TiXmlElement * root = _attribFile->getDocument().RootElement();
	if (!root)
	{
		ATOM_LOGGER::error("<%s> : missing XML root element\n",__FUNCTION__);
		return;
	}

	int pri_saved = _load_priority; // 先保存加载优先级 因为loadAttribute会改变_load_priority的值
	if (!loadAttribute (root))
	{
		ATOM_LOGGER::error("<%s> : load Attribute failed\n",__FUNCTION__);
		return;
	}
	_load_priority = pri_saved;		// 恢复加载优先级

	if (!_material)
		createMaterial();
}

bool ATOM_ParticleSystem::checkAsyncLoadFlag()
{
	if( !ATOM_AsyncLoader::IsRun() )					// 如果没有启动异步加载 返回TRUE
		return true;
	
	//if( _load_priority == ATOM_LoadPriority_IMMEDIATE )	// 如果加载优先级为直接加载 返回TRUE
	//	return true;
	
	if( _load_flag == LOAD_ALLFINISHED )				// 如果已经加载完成 返回TRUE
		return true;
	
	/*if( strstr( _textureFileName.c_str(),"huabnag_02.dds") )
	{

		ATOM_LOGGER::log("");
	}*/

	// 检查纹理是否加载完成
	if( _texture && !_texture->getAsyncLoader()->IsLoadAllFinished() )
		return false;
	if( _color && !_color->getAsyncLoader()->IsLoadAllFinished() )
		return false;

	// 设置加载完成标记
	_load_flag = LOAD_ALLFINISHED;
	
	return true;
}

// 直接加载接口
bool ATOM_ParticleSystem::loadSync()
{
	if( false == _attribFile->loadSync( getNodeFileName().c_str() ) )
		return false;

	TiXmlElement *element = _attribFile->getDocument().RootElement();
	int pri_saved = _load_priority;			// 先保存加载优先级 因为loadAttribute会改变_load_priority的值
	if (!loadAttribute (element))
	{
		ATOM_LOGGER::error ("<%s> load attribute failed (%s)\n", __FUNCTION__, getNodeFileName().c_str());
		return false;
	}
	_load_priority = pri_saved;				// 恢复加载优先级

	if (!_material && !createMaterial())
	{
		return false;
	}

	// 设置加载完成标记
	_load_flag = LOAD_ALLFINISHED;

	return true;
}

//--- wangjian added ---//
void ATOM_ParticleSystem::prepareForSort(ATOM_RenderScheme* renderScheme)
{
	ATOM_Drawable::prepareForSort(renderScheme);

	//-----------------------------------------------------------------------------------------------//
	_material->getParameterTable()->setInt ("enableDirectional", ((_flags & ATOM_ParticleSystem::PS_DIRECTIONAL_PARTICLE) ? 1 : 0));
	_material->getParameterTable()->setInt ("enableWorldSpace", ((_flags & ATOM_ParticleSystem::PS_WORLDSPACE) ? 1 : 0));
	_material->getParameterTable()->setInt ("hasTexture2D", ((_texture && (_texture->getFlags() & ATOM_Texture::TEXTURE2D)) ? 1 : 0));
	_material->getParameterTable()->setInt ("hasTexture3D", ((_texture && (_texture->getFlags() & ATOM_Texture::TEXTURE3D)) ? 1 : 0));
	_material->getParameterTable()->setInt ("hasColorTexture", (_color ? 1 : 0));
	//-----------------------------------------------------------------------------------------------//
}
// 设置绘制排序标记
void ATOM_ParticleSystem::setupSortFlag(int queueid)
{
	//-----------------------------------------------------------------------------------------------//
	/*_material->getParameterTable()->setInt ("enableDirectional", ((_flags & ATOM_ParticleSystem::PS_DIRECTIONAL_PARTICLE) ? 1 : 0));
	_material->getParameterTable()->setInt ("enableWorldSpace", ((_flags & ATOM_ParticleSystem::PS_WORLDSPACE) ? 1 : 0));
	_material->getParameterTable()->setInt ("hasTexture2D", ((_texture && (_texture->getFlags() & ATOM_Texture::TEXTURE2D)) ? 1 : 0));
	_material->getParameterTable()->setInt ("hasTexture3D", ((_texture && (_texture->getFlags() & ATOM_Texture::TEXTURE3D)) ? 1 : 0));
	_material->getParameterTable()->setInt ("hasColorTexture", (_color ? 1 : 0));*/
	//-----------------------------------------------------------------------------------------------//

	/*_material->autoSetActiveEffect (queueid);*/

	unsigned sf_ac = 0;
	unsigned sf_mt = 0;
	unsigned sf_matid = 0;

#if 0
	ULONGLONG matFlag = (ULONGLONG)_material->getActiveEffect();
	ULONGLONG miscFlag = 0;
	if( _texture )
		miscFlag = (ULONGLONG)_texture.get();
	if( _color )
		miscFlag |= (ULONGLONG)_color.get();
	ULONGLONG sf = ( matFlag << 32 ) + miscFlag;
#else
	//unsigned matFlag = (unsigned)_material->getActiveEffect();
	/*int effectid = getEffectIdEnd(ATOM_RenderScheme::getCurrentRenderScheme());
	ATOM_ASSERT(effectid!=-1);
	unsigned matFlag = (unsigned)_material->getCoreMaterial()->getEffect(effectid);
	matFlag <<= 16;
	matFlag /= 100000;
	matFlag *= 100000;
	unsigned miscFlag = 0;
	if( _texture )
		miscFlag = (unsigned)_texture.get();
	if( _color )
		miscFlag |= (unsigned)_color.get();
	unsigned sf = matFlag + ( miscFlag & 0x0000ffff );*/

	int effectid = getEffectIdEnd(ATOM_RenderScheme::getCurrentRenderScheme());
	ATOM_ASSERT(effectid>=0);
	sf_ac = (unsigned)_material->getCoreMaterial()->getEffect(effectid);
	if( _texture )
		sf_mt = (unsigned)_texture.get();
	if( _color )
		sf_mt |= (unsigned)_color.get();

#endif

	//setSortFlag(sf);
	setSortFlag(sf_ac,sf_mt,sf_matid);
}

void ATOM_ParticleSystem::resetMaterialDirtyFlag_impl()
{
	if( _material )
		_material->getParameterTable()->resetDirtyFlag();
}
//----------------------//
