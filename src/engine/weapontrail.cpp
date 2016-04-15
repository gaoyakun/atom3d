#include "StdAfx.h"
#include "weapontrail.h"

#define WT_STAGE_NORMAL		1
#define WT_STAGE_FADEOUT	2

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_WeaponTrail)
	ATOM_ATTRIBUTES_BEGIN(ATOM_WeaponTrail)
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_WeaponTrail, "Point0", getStartPointName, setStartPointName, "", "group=ATOM_WeaponTrail;desc='绑定点0'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_WeaponTrail, "Point1", getEndPointName, setEndPointName, "", "group=ATOM_WeaponTrail;desc='绑定点1'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_WeaponTrail, "Persistance", getPersistantOfVision, setPersistantOfVision, 40, "group=ATOM_WeaponTrail;desc='视觉暂留时间'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_WeaponTrail, "TrailColor", getColor, setColor, ATOM_Vector4f(1.f, 1.f, 1.f, 1.f), "group=ATOM_WeaponTrail;type=rgba;desc='刀光颜色'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_WeaponTrail, "TrailTexture", getTextureFileName, setTextureFileName, "", "group=ATOM_WeaponTrail;type=vfilename;desc='刀光贴图'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_WeaponTrail, "BlendMode", getBlendMode, setBlendMode, 0, "group=ATOM_WeaponTrail;type=int32;enum='A+1 0 A+A 1 1+1 2';desc='Alpha模式'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_WeaponTrail, "Resolution", getReslolution, setResolution, 3, "group=ATOM_WeaponTrail;desc='平滑度'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_WeaponTrail, "Power", getPower, setPower, 1.f, "group=ATOM_WeaponTrail;desc='刀光强度'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_WeaponTrail, "Enabled", isEnabled, enable, 1, "group=ATOM_WeaponTrail;desc='刀光激活'")
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_WeaponTrail, ATOM_VisualNode)

ATOM_WeaponTrail::ATOM_WeaponTrail (void)
{
	_resolution = 3;
	_persistant = 40;
	_weaponNode = 0;
	_blendMode = 0;
	_power = 1.f;
	_enabled = true;
	_isValid = false;
}

ATOM_WeaponTrail::~ATOM_WeaponTrail (void)
{
}

void ATOM_WeaponTrail::accept (ATOM_Visitor &visitor)
{
	visitor.visit (*this);
}

void ATOM_WeaponTrail::setupRenderQueue (ATOM_CullVisitor *visitor)
{
	if (!_material)
	{
		return;
	}

	if (_vertices.size() > 2)
	{
		visitor->addDrawable (this, ATOM_Matrix4x4f::getIdentityMatrix(), _material.get());
	}
}

void ATOM_WeaponTrail::skipClipTest (bool b)
{
}

bool ATOM_WeaponTrail::isSkipClipTest (void) const
{
	return false;
}

void ATOM_WeaponTrail::update (ATOM_Camera *camera)
{
	ATOM_Node *targetNode = _targetNode.get();
	//if (!targetNode)
	//{
	//	targetNode = getParent();
	//}

	if (targetNode)
	{
		if (targetNode != _weaponNode)
		{
			_weaponNode = targetNode;
			_isValid = true;

			ATOM_Matrix4x4f m;
			if (!_weaponNode->queryReferencePoint (_startPointName.c_str(), &m))
			{
				_isValid = false;
			}
			_trailStart = m.getRow3(3);

			if (!_weaponNode->queryReferencePoint (_endPointName.c_str(), &m))
			{
				_isValid = false;
			}
			_trailEnd = m.getRow3(3);
		}
	}

	unsigned t = ATOM_APP->getFrameStamp().currentTick;

	if (_frames.empty () && !_enabled)
	{
		return;
	}

	if (_frames.size() > 0)
	{
		for (unsigned i = 0; i < _frames.size(); ++i)
		{
			unsigned delta_time = t - _frames[i].tick;
			float a = ATOM_saturate (1.f - float(delta_time)/float(_persistant));
			a *= a;
			a *= a;

			_vertices[i*2].color.setFloats (_color.x, _color.y, _color.z, a * _color.w);
			_vertices[i*2+1].color = _vertices[i*2].color;
			_vertices[i*2].uv.set (0.f, 1.f - float(i+1)/float(_frames.size()));
			_vertices[i*2+1].uv.set (1.f, 1.f - float(i+1)/float(_frames.size()));
		}
	}

	if (targetNode && _isValid)
	{
		const ATOM_Matrix4x4f &parentWorldMatrix = targetNode->getWorldMatrix ();
		ATOM_Vector3f trans, scale;
		ATOM_Matrix3x3f r;
		ATOM_Quaternion quat;
		parentWorldMatrix.decompose (trans, r, scale);
		quat.fromMatrix (r);

		if (_frames.empty () 
			|| _frames.back().translation != trans
			|| _frames.back().rotation != quat
			|| _frames.back().scaling != scale)
		{
			if (_frames.empty () || _resolution <= 1)
			{
				Frame frame;
				frame.translation = trans;
				frame.scaling = scale;
				frame.rotation = quat;
				frame.tick = t;
				_frames.push_back (frame);

				TrailVertex vertex;

				vertex.xyz = parentWorldMatrix.transformPoint (_trailStart);
				ATOM_ASSERT(vertex.xyz != ATOM_Vector3f(0.f, 0.f, 0.f));
				vertex.color.setFloats (_color.x, _color.y, _color.z, _color.w);
				vertex.uv = ATOM_Vector2f(0.f, 0.f);
				_vertices.push_back (vertex);
				_indices.push_back (_vertices.size() - 1);

				vertex.xyz = parentWorldMatrix.transformPoint (_trailEnd);
				ATOM_ASSERT(vertex.xyz != ATOM_Vector3f(0.f, 0.f, 0.f));
				vertex.color.setFloats (_color.x, _color.y, _color.z, _color.w);
				vertex.uv = ATOM_Vector2f(1.f, 0.f);
				_vertices.push_back (vertex);
				_indices.push_back (_vertices.size() - 1);
			}
			else
			{
				Frame frameLast = _frames.back();

				for (int i = 0; i < _resolution; ++i)
				{
					float factor = float(i+1)/float(_resolution);
					Frame frame;
					frame.tick = frameLast.tick + (t - frameLast.tick) * factor;

					frame.translation = frameLast.translation + (trans - frameLast.translation) * factor;
					frame.scaling = frameLast.scaling + (scale - frameLast.scaling) * factor;
					frame.rotation = ATOM_Quaternion::fastSlerp (frameLast.rotation, quat, factor);
					ATOM_Matrix4x4f _m;
					frame.rotation.toMatrix (_m);
					_m.setRow3 (3, frame.translation);
					_m >>= ATOM_Matrix4x4f::getScaleMatrix (frame.scaling);

					_frames.push_back (frame);

					TrailVertex vertex;

					vertex.xyz = _m.transformPoint (_trailStart);
					ATOM_ASSERT(vertex.xyz != ATOM_Vector3f(0.f, 0.f, 0.f));
					vertex.color.setFloats (_color.x, _color.y, _color.z, _color.w);
					vertex.uv = ATOM_Vector2f(0.f, 0.f);
					_vertices.push_back (vertex);
					_indices.push_back (_vertices.size() - 1);

					vertex.xyz = _m.transformPoint (_trailEnd);
					ATOM_ASSERT(vertex.xyz != ATOM_Vector3f(0.f, 0.f, 0.f));
					vertex.color.setFloats (_color.x, _color.y, _color.z, _color.w);
					vertex.uv = ATOM_Vector2f(1.f, 0.f);
					_vertices.push_back (vertex);
					_indices.push_back (_vertices.size() - 1);
				}
			}
			/*
			frame.tick = t;
			frame.invMatrix = targetNode->getInvWorldMatrix ();
			_frames.push_back (frame);

			TrailVertex vertex;

			vertex.xyz = parentWorldMatrix.transformPoint (_trailStart);
			vertex.color.setFloats (_color.x, _color.y, _color.z, _color.w);
			vertex.uv = ATOM_Vector2f(0.f, 0.f);
			_vertices.push_back (vertex);
			_indices.push_back (_vertices.size() - 1);

			vertex.xyz = parentWorldMatrix.transformPoint (_trailEnd);
			vertex.color.setFloats (_color.x, _color.y, _color.z, _color.w);
			vertex.uv = ATOM_Vector2f(1.f, 0.f);
			_vertices.push_back (vertex);
			_indices.push_back (_vertices.size() - 1);
			*/
			invalidateBoundingbox ();
		}
	}
}

bool ATOM_WeaponTrail::rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const
{
	return false;
}

bool ATOM_WeaponTrail::draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material)
{
	if (_frames.size () <= 1)
	{
		return true;
	}

	material->getParameterTable()->setTexture("texture", _texture ? _texture.get() : ATOM_GetColorTexture (0xFFFFFFFF));
	material->getParameterTable()->setVector("power", ATOM_Vector4f(_power, _power, _power, 1.f));
	switch (_blendMode)
	{
	case 1:
		material->getParameterTable()->setInt ("srcBlend", ATOM_RenderAttributes::BlendFunc_SrcAlpha);
		material->getParameterTable()->setInt ("destBlend", ATOM_RenderAttributes::BlendFunc_InvSrcAlpha);
		break;
	case 2:
		material->getParameterTable()->setInt ("srcBlend", ATOM_RenderAttributes::BlendFunc_One);
		material->getParameterTable()->setInt ("destBlend", ATOM_RenderAttributes::BlendFunc_One);
		break;
	case 0:
	default:
		material->getParameterTable()->setInt ("srcBlend", ATOM_RenderAttributes::BlendFunc_SrcAlpha);
		material->getParameterTable()->setInt ("destBlend", ATOM_RenderAttributes::BlendFunc_One);
		break;
	}

	int numFaded = 0;
	for (unsigned i = 0; i < _frames.size(); ++i)
	{
		if (_vertices[i*2].color.getByteA() != 0)
			break;

		++numFaded;
	}

	if (numFaded > 0)
	{
		_frames.erase (_frames.begin(), _frames.begin() + numFaded);
		_vertices.erase (_vertices.begin(), _vertices.begin() + numFaded * 2);
		_indices.resize (_vertices.size());
		ATOM_ASSERT(_frames.size() * 2 == _vertices.size());
	}

	if (_frames.size() > 1)
	{
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
				device->renderInterleavedMemoryStreamsIndexed (ATOM_PRIMITIVE_TRIANGLE_STRIP, _indices.size(), _vertices.size(), ATOM_VERTEX_ATTRIB_COORD|ATOM_VERTEX_ATTRIB_PRIMARY_COLOR|ATOM_VERTEX_ATTRIB_TEX1_2, sizeof(TrailVertex), &_vertices[0], &_indices[0]);
				material->endPass (device, pass);
			}
		}
		material->end (device);

		//--- wangjian added ---//
		if( effect_id >= 0 )
			material->getCoreMaterial()->setAutoSelectEffect(true);
		//----------------------//
	}

	return true;
}

//--- wangjian added ---//
// 设置绘制排序标记
void ATOM_WeaponTrail::setupSortFlag(int queueid)
{
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
	unsigned sf = matFlag + ( miscFlag & 0x0000ffff );
	setSortFlag(sf);*/


	unsigned sf_ac = 0;
	unsigned sf_mt = 0;
	unsigned sf_matid = 0;

	int effectid = getEffectIdEnd(ATOM_RenderScheme::getCurrentRenderScheme());
	ATOM_ASSERT(effectid>=0);
	sf_ac = (unsigned)_material->getCoreMaterial()->getEffect(effectid);
	if( _texture )
		sf_mt = (unsigned)_texture.get();

	setSortFlag(sf_ac,sf_mt,sf_matid);
}
void ATOM_WeaponTrail::resetMaterialDirtyFlag_impl()
{
	if( _material )
		_material->getParameterTable()->resetDirtyFlag();
}
//----------------------//

void ATOM_WeaponTrail::setResolution (int resolution)
{
	_resolution = resolution;
}

int ATOM_WeaponTrail::getReslolution (void) const
{
	return _resolution;
}

void ATOM_WeaponTrail::setPersistantOfVision (unsigned timeInMs)
{
	_persistant = timeInMs;
}

unsigned ATOM_WeaponTrail::getPersistantOfVision (void) const
{
	return _persistant;
}

void ATOM_WeaponTrail::setColor (const ATOM_Vector4f &color)
{
	_color = color;
}

const ATOM_Vector4f &ATOM_WeaponTrail::getColor (void) const
{
	return _color;
}

void ATOM_WeaponTrail::setBlendMode (int mode)
{
	_blendMode = mode;
}

int ATOM_WeaponTrail::getBlendMode (void) const
{
	return _blendMode;
}

bool ATOM_WeaponTrail::onLoad (ATOM_RenderDevice *device)
{
	_material = ATOM_MaterialManager::createMaterialFromCore (device, "/materials/builtin/weapontrail.mat");

	if (!_material)
	{
		return false;
	}

	return true;
}

void ATOM_WeaponTrail::buildBoundingbox (void) const
{
	if (_frames.size() > 0)
	{
		_boundingBox.beginExtend ();
		const ATOM_Matrix4x4f &invMatrix = getInvWorldMatrix ();

		for (unsigned i = 0; i < _frames.size(); ++i)
		{
			_boundingBox.extend (invMatrix.transformPoint (_vertices[i*2].xyz));
			_boundingBox.extend (invMatrix.transformPoint (_vertices[i*2+1].xyz));
		}
	}
	else
	{
		_boundingBox.setMin (0.f);
		_boundingBox.setMax (0.f);
	}
}

void ATOM_WeaponTrail::setStartPointName (const ATOM_STRING &str)
{
	_startPointName = str;
	_weaponNode = 0;
}

const ATOM_STRING &ATOM_WeaponTrail::getStartPointName (void) const
{
	return _startPointName;
}

void ATOM_WeaponTrail::setEndPointName (const ATOM_STRING &str)
{
	_endPointName = str;
	_weaponNode = 0;
}

const ATOM_STRING &ATOM_WeaponTrail::getEndPointName (void) const
{
	return _endPointName;
}

void ATOM_WeaponTrail::reset (void)
{
	_vertices.resize (0);
	_indices.resize (0);
	_frames.resize (0);
	invalidateBoundingbox ();
}

void ATOM_WeaponTrail::setTextureFileName (const ATOM_STRING &filename)
{
	_textureFileName = filename;

	//--- wangjian modified ---------------------------------//
	// 异步加载相关
	bool bMt = ( ATOM_AsyncLoader::IsRun() );// && ( _load_priority != ATOM_LoadPriority_IMMEDIATE );

	_texture = ATOM_CreateTextureResource (_textureFileName.c_str(),ATOM_PIXEL_FORMAT_UNKNOWN,_load_priority);

	if( _texture && bMt )
	{
		if( !_texture->getAsyncLoader()->IsLoadAllFinished() )
		{
			// 加载优先级
			int loadPri = ( _load_priority == ATOM_LoadPriority_IMMEDIATE ) ?	_texture->getAsyncLoader()->priority :
																				_load_priority;

			ATOM_LOADRequest_Event* event_load( ATOM_NEW(	ATOM_LOADRequest_Event, 
															_textureFileName.c_str(), 
															this,
															loadPri,
															_texture->getAsyncLoader(),
															ATOM_LOADRequest_Event::FLAG_SHOW ) );
		}
		else
		{
			_load_flag = LOAD_ALLFINISHED;
	}
	}
	else
	{
		_load_flag = LOAD_ALLFINISHED;
	}
	//--------------------------------------------------------//
	
}

const ATOM_STRING &ATOM_WeaponTrail::getTextureFileName (void) const
{
	return _textureFileName;
}

void ATOM_WeaponTrail::setPower (float power)
{
	_power = power;
}

float ATOM_WeaponTrail::getPower (void) const
{
	return _power;
}

void ATOM_WeaponTrail::enable (int b)
{
	_enabled = b != 0;
}

int ATOM_WeaponTrail::isEnabled (void) const
{
	return _enabled ? 1 : 0;
}

void ATOM_WeaponTrail::setTarget (ATOM_Node *node)
{
	if (_targetNode.get() != node)
	{
		_targetNode = node;
		_weaponNode = 0;
	}
}

ATOM_Node *ATOM_WeaponTrail::getTarget (void) const
{
	return _targetNode.get();
}

bool ATOM_WeaponTrail::isPlaying (void) const
{
	return _frames.size() > 1;
}

