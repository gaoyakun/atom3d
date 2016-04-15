#include "StdAfx.h"
#include "lightnode.h"
#include "timemanager.h"

#define ATTENUATION_NOT_USED ATOM_Vector3f(-1000000.0f, -1000000.0f, -1000000.0f)

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_LightNode)
	ATOM_ATTRIBUTES_BEGIN(ATOM_LightNode)
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_LightNode, "Color", getLightColor, setLightColor, ATOM_Vector4f(1.f,1.f,1.f,1.f), "type=rgba;group=ATOM_LightNode;desc='颜色'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_LightNode, "Intensity", getLightIntensity, setLightIntensity, 1.f, "group=ATOM_LightNode;desc='强度'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_LightNode, "Type", getLightType, setLightType, ATOM_Light::Point, "type=int32;enum='Point 1 Directional 0 Spot 2';group=ATOM_LightNode;desc='类型'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_LightNode, "Attenuation", getLightAttenuation, setLightAttenuation, /*ATTENUATION_NOT_USED*/ATOM_Vector3f(1.f, 2.f/50.f, 13.f/2500.f), "group=ATOM_LightNode;step=0.00001f;precision=5;desc='衰减常数'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_LightNode, "HasAttenuation", getHasAttenuation, setHasAttenuation, 1, "group=ATOM_LightNode;type=bool;desc='是否有衰减'")
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_LightNode, ATOM_Node)

ATOM_IMPLEMENT_NODE_FILE_TYPE(ATOM_LightNode)

ATOM_LightNode::ATOM_LightNode (void)
{
	ATOM_STACK_TRACE(ATOM_LightNode::ATOM_LightNode);

	_light = ATOM_NEW(ATOM_Light);
	_light->setLightType (ATOM_Light::Unknown);
	_light->setColor4f (ATOM_Vector4f(1.f, 1.f, 1.f, 1.f));
	_light->setRange (ATOM_Vector3f(1.f, 1.f, 1.f));

	//===================================================================//
	// wangjian modified
#if 1
	_light->setAttenuation (ATOM_Vector3f(1.f, 2.f/50.f, 13.f/2500.f));
#else
	_light->setAttenuation (ATTENUATION_NOT_USED);
#endif
	//===================================================================//

	_color.set(1.f, 1.f, 1.f, 1.f);
	_transformChanged = true;

	setPickable (0);

	//--- wangjian added ---//
	_load_priority = ATOM_LoadPriority_IMMEDIATE;
	_load_flag = LOAD_ALLFINISHED;
	//----------------------//
}

ATOM_LightNode::~ATOM_LightNode (void)
{
	ATOM_STACK_TRACE(ATOM_LightNode::~ATOM_LightNode);
}

void ATOM_LightNode::accept (ATOM_Visitor &visitor)
{
	ATOM_STACK_TRACE(ATOM_LightNode::accept);

	visitor.visit (*this);
}

bool ATOM_LightNode::supportMTLoading (void)
{
	return false;
}

void ATOM_LightNode::buildBoundingbox(void) const
{
	switch (_light->getLightType())
	{
	case ATOM_Light::Point:
		{
			_boundingBox.setMin (ATOM_Vector3f(-1.f, -1.f, -1.f));
			_boundingBox.setMax (ATOM_Vector3f(1.f, 1.f, 1.f));
			break;
		}
	case ATOM_Light::Spot:
		{
			// TODO:
			// fall through
		}
	case ATOM_Light::Directional:
		{
			// fall through
		}
	default:
		{
			_boundingBox.setMin (ATOM_Vector3f(-100000.f, -100000.f, -100000.f));
			_boundingBox.setMax (ATOM_Vector3f(100000.f, 100000.f, 100000.f));
			break;
		}
	};
}

void ATOM_LightNode::onTransformChanged (void)
{
	ATOM_Node::onTransformChanged ();

	_transformChanged = true;
}

void ATOM_LightNode::setLightType (int type)
{
	if (type != _light->getLightType ())
	{
		_light->setLightType ((ATOM_Light::LightType)type);

		onTransformChanged ();
	}
}

int ATOM_LightNode::getLightType (void) const
{
	return _light->getLightType ();
}

void ATOM_LightNode::setLightColor (const ATOM_Vector4f &color)
{
	_color.setVector3 (color.getVector3());
}

const ATOM_Vector4f &ATOM_LightNode::getLightColor (void) const
{
	return _color;
}

void ATOM_LightNode::setLightIntensity (float intensity)
{
	_color.w = intensity;
}

float ATOM_LightNode::getLightIntensity (void) const
{
	return _color.w;
}

void ATOM_LightNode::updateLightParams (const ATOM_Matrix4x4f &projMatrix, const ATOM_Matrix4x4f &viewMatrix)
{
	if (_transformChanged)
	{
		_transformChanged =false;

		const ATOM_Matrix4x4f &worldMatrix = getWorldMatrix ();

		ATOM_Light::LightType type = _light->getLightType ();

		if (type == ATOM_Light::Point || type == ATOM_Light::Spot)
		{
			_light->setPosition (ATOM_Vector3f(worldMatrix.m30, worldMatrix.m31, worldMatrix.m32));
		}

		if (type == ATOM_Light::Directional || type == ATOM_Light::Spot)
		{
			_light->setDirection (worldMatrix.transformVector (ATOM_Vector3f(0.f, 0.f, 1.f)));
		}

		_light->setRange (getScale ());
	}

	//const ATOM_Vector4f &colorFactor = _useGlobalColorKeys ? ATOM_RenderSettings::getLightTimeFactor () : _colorKeys.getCurrentValue ();
	_light->setColor4f (_color);
}

bool ATOM_LightNode::onLoad (ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_LightNode::onLoad);

	if (getNodeFileName().empty ())
	{
		return true;
	}
	else
	{
		ATOM_AutoFile f(getNodeFileName().c_str(), ATOM_VFS::read|ATOM_VFS::text);
		if (!f)
		{
			return false;
		}
		unsigned size = f->size();
		char *p = (char*)ATOM_MALLOC(size);
		size = f->read (p, size);
		p[size] = '\0';
		ATOM_TiXmlDocument doc;
		if (!doc.Parse (p))
		{
			ATOM_FREE(p);
			return false;
		}
		ATOM_FREE(p);

		return loadAttribute (doc.RootElement ());
	}
}

void ATOM_LightNode::setLightAttenuation (const ATOM_Vector3f &val)
{
	_light->setAttenuation (val);
}

const ATOM_Vector3f &ATOM_LightNode::getLightAttenuation (void) const
{
	return _light->getAttenuation ();
}
//---------------------------------------------------//
// wangjian added
void ATOM_LightNode::setHasAttenuation (int has)
{
	_light->setHasAttenuation(has);
}
int ATOM_LightNode::getHasAttenuation (void) const
{
	return _light->hasAttenuationParam();
}
//---------------------------------------------------//
void ATOM_LightNode::assign (ATOM_Node *other) const
{
	ATOM_Node::assign (other);

	ATOM_LightNode *node = dynamic_cast<ATOM_LightNode*>(other);

	if (node)
	{
		*(node->_light.get()) = *(_light.get());
	}
}

bool ATOM_LightNode::supportFixedFunction (void) const
{
	return false;
}

ATOM_Light *ATOM_LightNode::getLight (void) const
{
	return _light.get();
}
