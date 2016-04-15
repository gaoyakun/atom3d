#ifndef __ATOM3D_ENGINE_PS_KEYFRAME_H
#define __ATOM3D_ENGINE_PS_KEYFRAME_H

#include "composition_keyframevalue.h"
#include "composition_io.h"

struct ATOM_PSKeyFrameData
{
	int emitInterval;
	int emitCount;
	ATOM_Vector3f emitShapeSize;
	ATOM_Vector3f emitShapeSizeVar;
	float emitCone;
	float emitConeVar;
	float velocity;
	float velocityVar;
	float life;
	float lifeVar;
	float size1;
	float size1Var;
	float size2;
	float size2Var;
	float accel;
	float accelVar;
	ATOM_Vector3f gravity;
	ATOM_Vector3f wind;
	ATOM_Vector4f color;
	float scale;
	float transparency;
	//--- wangjian added ---//
	float colorMultiplier;

	ATOM_PSKeyFrameData (void)
	{
	}

	ATOM_PSKeyFrameData (float)
	{
		ATOM_ASSERT(0);
	}

	ATOM_PSKeyFrameData & operator /= (float)
	{
		ATOM_ASSERT(0);
		return *this;
	}

	ATOM_PSKeyFrameData & operator -= (const ATOM_PSKeyFrameData &)
	{
		ATOM_ASSERT(0);
		return *this;
	}

	ATOM_PSKeyFrameData operator - (void) const
	{
		ATOM_ASSERT(0);
		return ATOM_PSKeyFrameData(*this);
	}

	ATOM_PSKeyFrameData operator / (float) const
	{
		ATOM_ASSERT(0);
		return ATOM_PSKeyFrameData(*this);
	}

	ATOM_PSKeyFrameData operator * (float) const
	{
		ATOM_ASSERT(0);
		return ATOM_PSKeyFrameData(*this);
	}

	ATOM_PSKeyFrameData operator * (const ATOM_PSKeyFrameData &) const
	{
		ATOM_ASSERT(0);
		return ATOM_PSKeyFrameData(*this);
	}

	ATOM_PSKeyFrameData operator + (const ATOM_PSKeyFrameData &) const
	{
		ATOM_ASSERT(0);
		return ATOM_PSKeyFrameData(*this);
	}

	ATOM_PSKeyFrameData operator - (const ATOM_PSKeyFrameData &) const
	{
		ATOM_ASSERT(0);
		return ATOM_PSKeyFrameData(*this);
	}
};

template <>
struct KeyValueXMLWriter<ATOM_PSKeyFrameData>
{
	void operator ()(long x, const ATOM_PSKeyFrameData &val, ATOM_TiXmlElement *xml) const
	{
		char buffer[256];

		xml->SetAttribute ("Time", x);
		xml->SetAttribute ("EmitInterval", (int)val.emitInterval);
		xml->SetAttribute ("EmitCount", (int)val.emitCount);

		sprintf (buffer, "%f,%f,%f", val.emitShapeSize.x, val.emitShapeSize.y, val.emitShapeSize.z);
		xml->SetAttribute ("EmitShapeSize", buffer);

		sprintf (buffer, "%f,%f,%f", val.emitShapeSizeVar.x, val.emitShapeSizeVar.y, val.emitShapeSizeVar.z);
		xml->SetAttribute ("EmitShapeSizeVar", buffer);

		sprintf (buffer, "%f,%f,%f", val.color.x, val.color.y, val.color.z);
		xml->SetAttribute ("Color", buffer);

		xml->SetDoubleAttribute ("EmitCone", val.emitCone);
		xml->SetDoubleAttribute ("EmitConeVar", val.emitConeVar);
		xml->SetDoubleAttribute ("Velocity", val.velocity);
		xml->SetDoubleAttribute ("VelocityVar", val.velocityVar);
		xml->SetDoubleAttribute ("Life", val.life);
		xml->SetDoubleAttribute ("LifeVar", val.lifeVar);
		xml->SetDoubleAttribute ("Size1", val.size1);
		xml->SetDoubleAttribute ("Size1Var", val.size1Var);
		xml->SetDoubleAttribute ("Size2", val.size2);
		xml->SetDoubleAttribute ("Size2Var", val.size2Var);
		xml->SetDoubleAttribute ("Accel", val.accel);
		xml->SetDoubleAttribute ("AccelVar", val.accelVar);
		xml->SetDoubleAttribute ("Scale", val.scale);
		xml->SetDoubleAttribute ("Transparency", val.transparency);
		

		sprintf (buffer, "%f,%f,%f", val.gravity.x, val.gravity.y, val.gravity.z);
		xml->SetAttribute ("Gravity", buffer);

		sprintf (buffer, "%f,%f,%f", val.wind.x, val.wind.y, val.wind.z);
		xml->SetAttribute ("Wind", buffer);

		//--- wangjian added ---//
		xml->SetDoubleAttribute ("ColorMultiplier", val.colorMultiplier);
	}
};

template <>
struct KeyValueXMLLoader<ATOM_PSKeyFrameData>
{
	bool operator ()(long &x, ATOM_PSKeyFrameData &val, const ATOM_TiXmlElement *xml) const
	{
		if (xml->QueryIntAttribute ("Time", (int*)&x) != ATOM_TIXML_SUCCESS)
			return false;

		if (xml->QueryIntAttribute ("EmitInterval", (int*)&val.emitInterval) != ATOM_TIXML_SUCCESS)
			return false;

		if (xml->QueryIntAttribute ("EmitCount", (int*)&val.emitCount) != ATOM_TIXML_SUCCESS)
			return false;

		const char *shapeSize = xml->Attribute ("EmitShapeSize");
		if (!shapeSize)
			return false;

		if (3 != sscanf (shapeSize, "%f,%f,%f", &val.emitShapeSize.x, &val.emitShapeSize.y, &val.emitShapeSize.z))
			return false;

		const char *shapeSizeVar = xml->Attribute ("EmitShapeSizeVar");
		if (!shapeSizeVar)
			return false;

		if (3 != sscanf (shapeSizeVar, "%f,%f,%f", &val.emitShapeSizeVar.x, &val.emitShapeSizeVar.y, &val.emitShapeSizeVar.z))
			return false;

		if (xml->QueryFloatAttribute ("EmitCone", &val.emitCone) != ATOM_TIXML_SUCCESS)
			return false;

		if (xml->QueryFloatAttribute ("EmitConeVar", &val.emitConeVar) != ATOM_TIXML_SUCCESS)
			return false;

		if (xml->QueryFloatAttribute ("Velocity", &val.velocity) != ATOM_TIXML_SUCCESS)
			return false;

		if (xml->QueryFloatAttribute ("VelocityVar", &val.velocityVar) != ATOM_TIXML_SUCCESS)
			return false;

		if (xml->QueryFloatAttribute ("Life", &val.life) != ATOM_TIXML_SUCCESS)
			return false;

		if (xml->QueryFloatAttribute ( "LifeVar", &val.lifeVar) != ATOM_TIXML_SUCCESS)
			return false;

		if (xml->QueryFloatAttribute ("Size1", &val.size1) != ATOM_TIXML_SUCCESS)
			return false;

		if (xml->QueryFloatAttribute ("Size1Var", &val.size1Var) != ATOM_TIXML_SUCCESS)
			return false;

		if (xml->QueryFloatAttribute ("Size2", &val.size2) != ATOM_TIXML_SUCCESS)
			return false;

		if (xml->QueryFloatAttribute ("Size2Var", &val.size2Var) != ATOM_TIXML_SUCCESS)
			return false;

		if (xml->QueryFloatAttribute ("Accel", &val.accel) != ATOM_TIXML_SUCCESS)
			return false;

		if (xml->QueryFloatAttribute ("AccelVar", &val.accelVar) != ATOM_TIXML_SUCCESS)
			return false;

		if (xml->QueryFloatAttribute ("Scale", &val.scale) != ATOM_TIXML_SUCCESS)
			return false;

		if (xml->QueryFloatAttribute ("Transparency", &val.transparency) != ATOM_TIXML_SUCCESS)
			return false;

		const char *gravity = xml->Attribute ("Gravity");
		if (!gravity)
			return false;

		if (3 != sscanf (gravity, "%f,%f,%f", &val.gravity.x, &val.gravity.y, &val.gravity.z))
			return false;

		const char *wind = xml->Attribute ("Wind");
		if (!wind)
			return false;

		if (3 != sscanf (wind, "%f,%f,%f", &val.wind.x, &val.wind.y, &val.wind.z))
			return false;

		const char *color = xml->Attribute ("Color");
		if (!color)
		{
			val.color.set (1.f, 1.f, 1.f, 1.f);
		}
		else if (3 != sscanf (color, "%f,%f,%f", &val.color.x, &val.color.y, &val.color.z))
		{
			val.color.set (1.f, 1.f, 1.f, 1.f);
		}
		else
		{
			val.color.w = 1.f;
		}

		//--- wangjian added ---//
		if (xml->QueryFloatAttribute ("ColorMultiplier", &val.colorMultiplier) != ATOM_TIXML_SUCCESS)
			return false;

		return true;
	}
};


#define DECLARE_PS_LINEAR_IPOL(member) result.member = val1.member + (val2.member - val1.member) * t

struct PSLinearPolicy
{
	ATOM_PSKeyFrameData ipol (const ATOM_PSKeyFrameData &val1, const ATOM_PSKeyFrameData &val2, float t) const
	{
		ATOM_PSKeyFrameData result;

		DECLARE_PS_LINEAR_IPOL(emitInterval);
		DECLARE_PS_LINEAR_IPOL(emitCount);
		DECLARE_PS_LINEAR_IPOL(emitShapeSize);
		DECLARE_PS_LINEAR_IPOL(emitShapeSizeVar);
		DECLARE_PS_LINEAR_IPOL(emitCone);
		DECLARE_PS_LINEAR_IPOL(emitConeVar);
		DECLARE_PS_LINEAR_IPOL(velocity);
		DECLARE_PS_LINEAR_IPOL(velocityVar);
		DECLARE_PS_LINEAR_IPOL(life);
		DECLARE_PS_LINEAR_IPOL(lifeVar);
		DECLARE_PS_LINEAR_IPOL(size1);
		DECLARE_PS_LINEAR_IPOL(size1Var);
		DECLARE_PS_LINEAR_IPOL(size2);
		DECLARE_PS_LINEAR_IPOL(size2Var);
		DECLARE_PS_LINEAR_IPOL(accel);
		DECLARE_PS_LINEAR_IPOL(accelVar);
		DECLARE_PS_LINEAR_IPOL(gravity);
		DECLARE_PS_LINEAR_IPOL(wind);
		DECLARE_PS_LINEAR_IPOL(scale);
		DECLARE_PS_LINEAR_IPOL(transparency);
		DECLARE_PS_LINEAR_IPOL(color);
		//--- wangjian added ---//
		DECLARE_PS_LINEAR_IPOL(colorMultiplier);

		return result;
	}
};

class ATOM_ENGINE_API ATOM_ParticleSystemKeyFrame: public ATOM_CompositionKeyFrameT<ATOM_PSKeyFrameData>
{
public:
	typedef PSLinearPolicy linear_policy_type;

public:
	ATOM_ParticleSystemKeyFrame (void);
	ATOM_ParticleSystemKeyFrame (const ATOM_PSKeyFrameData &data);

public:
	virtual void visit (ATOM_Node &node);
	virtual void visit (ATOM_ParticleSystem &node);
};
ATOM_DECLARE_COMPOSITION_KEYFRAME(ATOM_ParticleSystemKeyFrame, ATOM_PSKeyFrameData)

class ATOM_ENGINE_API ATOM_ParticleSystemKeyFrameValue: public ATOM_CompositionKeyFrameValue
{
public:
	void setParticleSystemParams (const ATOM_PSKeyFrameData &data);
	const ATOM_PSKeyFrameData &getParticleSystemParams (void) const;

private:
	ATOM_PSKeyFrameData _data;
};

#endif // __ATOM3D_ENGINE_PS_KEYFRAME_H
