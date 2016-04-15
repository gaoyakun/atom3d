#include "StdAfx.h"
#include "decal.h"

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_Decal)
  ATOM_ATTRIBUTES_BEGIN(ATOM_Decal)
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Decal, "TextureFileName", getTextureFileName, setTextureFileName, "", "group=ATOM_Decal;type=vfilename;desc='VFS texture file name';desc='贴图文件名'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Decal, "SrcBlend", getSrcBlend, setSrcBlend, ATOM_RenderAttributes::BlendFunc_SrcAlpha, "group=ATOM_Decal;type=int32;enum='Zero 1 One 2 SrcColor 3 InvSrcColor 4 SrcAlpha 5 InvSrcAlpha 6 DestAlpha 7 InvDestAlpha 8 DestColor 9 InvDestColor 10';desc='源融合因子'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Decal, "DestBlend", getDestBlend, setDestBlend, ATOM_RenderAttributes::BlendFunc_InvSrcAlpha, "group=ATOM_Decal;type=int32;enum='Zero 1 One 2 SrcColor 3 InvSrcColor 4 SrcAlpha 5 InvSrcAlpha 6 DestAlpha 7 InvDestAlpha 8 DestColor 9 InvDestColor 10';desc='目标融合因子'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Decal, "AlphaBlend", isBlendEnabled, enableBlend, 1, "group=ATOM_Decal;type=bool;desc='允许Alpha融合'")
	ATOM_ATTRIBUTE_PERSISTENT(ATOM_Decal, "DecalColor", getColor, setColor, ATOM_Vector4f(1.f,1.f,1.f,1.f), "group=ATOM_Decal;type=bool;desc='贴花颜色'")
  ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_Decal, ATOM_Node)


ATOM_Decal::ATOM_Decal (void)
{
	ATOM_STACK_TRACE(ATOM_Decal::ATOM_Decal);

	_srcBlend = ATOM_RenderAttributes::BlendFunc_SrcAlpha;
	_destBlend = ATOM_RenderAttributes::BlendFunc_DestAlpha;
	_enableBlend = 1;
	_color.set(1.f,1.f,1.f,1.f);
}

ATOM_Decal::~ATOM_Decal (void)
{
	ATOM_STACK_TRACE(ATOM_Decal::~ATOM_Decal);
}

void ATOM_Decal::accept (ATOM_Visitor &visitor)
{
	ATOM_STACK_TRACE(ATOM_Decal::accept);

	visitor.visit (*this);
}

bool ATOM_Decal::supportMTLoading (void)
{
	return false;
}

bool ATOM_Decal::supportFixedFunction (void) const
{
	return false;
}

bool ATOM_Decal::onLoad (ATOM_RenderDevice *device)
{
	setPickable (ATOM_Node::NONPICKABLE);

	//if( !_texture )
	{
		//--- wangjian : 异步加载 ---//
		// NOTE:异步加载当前支持的格式是DDS/JPG/PNG TGA暂时不支持，所以如果有TGA格式的纹理会无法显示

		//_load_priority = ATOM_LoadPriority_IMMEDIATE;

		AddStreamingTask();
	}

	//------------------------------------------//

	return true;
}

void ATOM_Decal::buildBoundingbox (void) const
{
	_boundingBox.setMin (ATOM_Vector3f(-0.5f, -0.5f, -0.5f));
	_boundingBox.setMax (ATOM_Vector3f(0.5f, 0.5f, 0.5f));
}

void ATOM_Decal::onTransformChanged (void)
{
	ATOM_Node::onTransformChanged ();
}

bool ATOM_Decal::draw(ATOM_RenderDevice* device, ATOM_Camera *camera, ATOM_Material *material)
{
	//--- wangjian added ---//
	// 异步加载 ： 检查是否完成加载
	if( _load_flag != LOAD_ALLFINISHED )
		return false;
	//---------------------//


	ATOM_Matrix4x4f mvp = camera->getViewProjectionMatrix() >> getWorldMatrix();

	ATOM_Vector3f vertices[8] = {
		ATOM_Vector3f (-0.5f, -0.5f, -0.5f),
		ATOM_Vector3f (-0.5f, -0.5f,  0.5f),
		ATOM_Vector3f (-0.5f,  0.5f, -0.5f),
		ATOM_Vector3f (-0.5f,  0.5f,  0.5f),
		ATOM_Vector3f ( 0.5f, -0.5f, -0.5f),
		ATOM_Vector3f ( 0.5f, -0.5f,  0.5f),
		ATOM_Vector3f ( 0.5f,  0.5f, -0.5f),
		ATOM_Vector3f ( 0.5f,  0.5f,  0.5f)
	};

	unsigned short indices[36] = {
		0, 2, 6,
		0, 6, 4,
		4, 6, 7,
		4, 7, 5,
		5, 7, 3,
		5, 3, 1,
		1, 3, 2,
		1, 2, 0,
		2, 3, 6,
		6, 3, 7,
		0, 4, 5,
		0, 5, 1
	};

	//--- wangjian modified ---//
	material->getParameterTable()->setTexture ("decalTexture", 
												(_texture && _texture->getAsyncLoader()->IsLoadAllFinished()) ? _texture.get() : ATOM_GetColorTexture(0x00FFFFFF) );
	material->getParameterTable()->setVector ("decalColor", _color);
	material->getParameterTable()->setInt ("enableBlend", _enableBlend);
	material->getParameterTable()->setInt ("srcBlendFunc", _srcBlend);
	material->getParameterTable()->setInt ("destBlendFunc", _destBlend);

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
			device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLES, 36, 8, ATOM_VERTEX_ATTRIB_COORD, sizeof(ATOM_Vector3f), vertices, indices);
			material->endPass (device, pass);
		}
	}
	material->end (device);

	//--- wangjian added ---//
	if( effect_id >= 0 )
		material->getCoreMaterial()->setAutoSelectEffect(true);
	//----------------------//

	return true;
}

void ATOM_Decal::assign (ATOM_Node *other) const
{
	ATOM_Node::assign (other);
	ATOM_Decal *decal = dynamic_cast<ATOM_Decal*>(other);
	if (decal)
	{
		decal->setTextureFileName (getTextureFileName ());
		if (!decal->getTexture ())
		{
			decal->setTexture (getTexture());
		}
	}
}

void ATOM_Decal::setTexture (ATOM_Texture *texture)
{
	if (texture != _texture.get())
	{
		_texture = texture;
		_textureFileName = "";
	}
}

ATOM_Texture *ATOM_Decal::getTexture (void) const
{
	return _texture.get();
}

void ATOM_Decal::setTextureFileName (const ATOM_STRING &fileName)
{
	ATOM_STACK_TRACE(ATOM_Decal::setTextureFileName);

	if (fileName != _textureFileName)
	{
		_textureFileName = fileName;

		//--- wangjian : 异步加载 ---//
		// NOTE:异步加载当前支持的格式是DDS/JPG/PNG TGA暂时不支持，所以如果有TGA格式的纹理会无法显示

		//_load_priority = ATOM_LoadPriority_IMMEDIATE;
	
		AddStreamingTask();
		//--------------------------------------------------------//
	}
}

const ATOM_STRING &ATOM_Decal::getTextureFileName (void) const
{
	return _textureFileName;
}

void ATOM_Decal::setSrcBlend(int srcBlend)
{
	_srcBlend = srcBlend;
}

int ATOM_Decal::getSrcBlend (void) const
{
	return _srcBlend;
}

void ATOM_Decal::setDestBlend (int destBlend)
{
	_destBlend = destBlend;
}

int ATOM_Decal::getDestBlend (void) const
{
	return _destBlend;
}

void ATOM_Decal::enableBlend (int enable)
{
	_enableBlend = enable;
}

int ATOM_Decal::isBlendEnabled (void) const
{
	return _enableBlend;
}

void ATOM_Decal::setMaterial (ATOM_Material *material)
{
	_material = material;
}

ATOM_Material *ATOM_Decal::getMaterial (void) const
{
	return _material.get();
}

void ATOM_Decal::setColor (const ATOM_Vector4f &color)
{
	_color = color;
}

const ATOM_Vector4f &ATOM_Decal::getColor (void) const
{
	return _color;
}

//--- wangjian added ---//
// 异步加载
void ATOM_Decal::AddStreamingTask()
{
	// 创建材质
	if( !_material )
		_material = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/decal.mat");
	
	// 加载纹理
	if( !_texture )
	{
		// 同步或者异步加载纹理
		bool bMT =	( ATOM_AsyncLoader::IsRun() );// && ( _load_priority != ATOM_LoadPriority_IMMEDIATE );

		_texture = ATOM_CreateTextureResource ( _textureFileName.c_str(), 
												ATOM_PIXEL_FORMAT_UNKNOWN, 
												bMT ? _load_priority : ATOM_LoadPriority_IMMEDIATE );

		// 异步加载
		if( bMT )
		{
			if( _texture && !_texture->getAsyncLoader()->IsLoadAllFinished() )
			{
				// 加载优先级
				int loadPri = _load_priority == ATOM_LoadPriority_IMMEDIATE ?	_texture->getAsyncLoader()->priority :
																				_load_priority;

				ATOM_LOADRequest_Event * event_load( ATOM_NEW(	ATOM_LOADRequest_Event,
																_textureFileName.c_str(),
																this,
																loadPri,
																_texture->getAsyncLoader(),
																ATOM_LOADRequest_Event::FLAG_SHOW ) );
			}
			else
				_load_flag = LOAD_ALLFINISHED;
		}
		else
			_load_flag = LOAD_ALLFINISHED;
	}
}

//--- wangjian added ---//
void ATOM_Decal::setupSortFlag(int queueid)
{
	//_material->autoSetActiveEffect (queueid);

	unsigned sf_ac = 0;
	unsigned sf_mt = 0;
	unsigned sf_matid = 0;

#if 0
	ULONGLONG matFlag = (ULONGLONG)_material->getActiveEffect();
	ULONGLONG miscFlag = 0;
	if( _texture )
		miscFlag = (ULONGLONG)_texture.get();
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
	{
		miscFlag = (unsigned)_texture.get();
	}
	unsigned sf = matFlag + ( miscFlag & 0x0000ffff );*/

	int effectid = getEffectIdEnd(ATOM_RenderScheme::getCurrentRenderScheme());
	ATOM_ASSERT(effectid>=0);
	sf_ac = (unsigned)_material->getCoreMaterial()->getEffect(effectid);
	if( _texture )
	{
		sf_mt = (unsigned)_texture.get();
	}
#endif

	//setSortFlag(sf);
	setSortFlag(sf_ac,sf_mt,sf_matid);
}
void ATOM_Decal::resetMaterialDirtyFlag_impl()
{
	if( _material )
		_material->getParameterTable()->resetDirtyFlag();
}
//---------------------//

