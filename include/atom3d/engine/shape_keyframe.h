#ifndef __ATOM3D_ENGINE_SHAPE_KEYFRAME_H
#define __ATOM3D_ENGINE_SHAPE_KEYFRAME_H

#include "composition_keyframevalue.h"
#include "composition_io.h"

struct ATOM_ShapeKeyFrameData
{
	ATOM_Vector4f color;
	float transparency;
	float shearing;
	float uvRotation;
	ATOM_Vector4f uvScaleOffset;
	ATOM_Vector3f size;
	//--- wangjian added ---//
	float colorMultiplier;
	float resolveScale;
	//----------------------//

	ATOM_ShapeKeyFrameData (void)
	{
	}

	ATOM_ShapeKeyFrameData (float)
	{
	}

	ATOM_ShapeKeyFrameData & operator /= (float)
	{
		ATOM_ASSERT(0);
		return *this;
	}

	ATOM_ShapeKeyFrameData & operator -= (const ATOM_ShapeKeyFrameData &)
	{
		ATOM_ASSERT(0);
		return *this;
	}

	ATOM_ShapeKeyFrameData operator - (void) const
	{
		ATOM_ASSERT(0);
		return ATOM_ShapeKeyFrameData(*this);
	}

	ATOM_ShapeKeyFrameData operator / (float) const
	{
		ATOM_ASSERT(0);
		return ATOM_ShapeKeyFrameData(*this);
	}

	ATOM_ShapeKeyFrameData operator * (float) const
	{
		ATOM_ASSERT(0);
		return ATOM_ShapeKeyFrameData(*this);
	}

	ATOM_ShapeKeyFrameData operator * (const ATOM_ShapeKeyFrameData &) const
	{
		ATOM_ASSERT(0);
		return ATOM_ShapeKeyFrameData(*this);
	}

	ATOM_ShapeKeyFrameData operator + (const ATOM_ShapeKeyFrameData &) const
	{
		ATOM_ASSERT(0);
		return ATOM_ShapeKeyFrameData(*this);
	}

	ATOM_ShapeKeyFrameData operator - (const ATOM_ShapeKeyFrameData&) const
	{
		ATOM_ASSERT(0);
		return ATOM_ShapeKeyFrameData(*this);
	}
};

template <>
struct KeyValueXMLWriter<ATOM_ShapeKeyFrameData>
{
	void operator ()(long x, const ATOM_ShapeKeyFrameData &val, ATOM_TiXmlElement *xml) const
	{
		xml->SetAttribute ("Time", x);
		xml->SetDoubleAttribute ("Transparency", val.transparency);
		xml->SetDoubleAttribute ("Shearing", val.shearing);
		xml->SetDoubleAttribute ("UVRotation", val.uvRotation);

		char buffer[256];

		sprintf (buffer, "%f,%f,%f,%f", val.uvScaleOffset.x, val.uvScaleOffset.y, val.uvScaleOffset.z, val.uvScaleOffset.w);
		xml->SetAttribute ("UVScaleOffset", buffer);

		sprintf (buffer, "%f,%f,%f", val.size.x, val.size.y, val.size.z);
		xml->SetAttribute ("Size", buffer);

		sprintf (buffer, "%f,%f,%f,%f", val.color.x, val.color.y, val.color.z, val.color.w);
		xml->SetAttribute ("Color", buffer);

		//--- wangjian added ---//
		xml->SetDoubleAttribute ("ColorMultiplier", val.colorMultiplier);
		xml->SetDoubleAttribute ("ResolveScale", val.resolveScale);
		//----------------------//
	}
};

template <>
struct KeyValueXMLLoader<ATOM_ShapeKeyFrameData>
{
	bool operator ()(long &x, ATOM_ShapeKeyFrameData &val, const ATOM_TiXmlElement *xml) const
	{
		if (xml->QueryIntAttribute ("Time", (int*)&x) != ATOM_TIXML_SUCCESS)
			x = 0;

		if (xml->QueryFloatAttribute ("Transparency", &val.transparency) != ATOM_TIXML_SUCCESS)
			val.transparency = 1.f;

		if (xml->QueryFloatAttribute ("Shearing", &val.shearing) != ATOM_TIXML_SUCCESS)
			val.shearing = 0.f;

		if (xml->QueryFloatAttribute ("UVRotation", &val.uvRotation) != ATOM_TIXML_SUCCESS)
			val.uvRotation = 0.f;

		const char *uvScaleOffset = xml->Attribute ("UVScaleOffset");
		if (!uvScaleOffset || 4 != sscanf(uvScaleOffset, "%f,%f,%f,%f", &val.uvScaleOffset.x, &val.uvScaleOffset.y, &val.uvScaleOffset.z, &val.uvScaleOffset.w))
			val.uvScaleOffset.set (1.f, 1.f, 0.f, 0.f);

		const char *size = xml->Attribute ("Size");
		if (!size || 3 != sscanf(size, "%f,%f,%f", &val.size.x, &val.size.y, &val.size.z))
			val.size.set (1.f, 1.f, 1.f);

		const char *color = xml->Attribute ("Color");
		if (!color || 4 != sscanf(color, "%f,%f,%f,%f", &val.color.x, &val.color.y, &val.color.z, &val.color.w))
			val.color.set (1.f, 1.f, 1.f, 1.f);

		//--- wangjian added ---//
		if (xml->QueryFloatAttribute ("ColorMultiplier", &val.colorMultiplier) != ATOM_TIXML_SUCCESS)
			val.colorMultiplier = 1.f;
		if (xml->QueryFloatAttribute ("ResolveScale", &val.resolveScale) != ATOM_TIXML_SUCCESS)
			val.resolveScale = 0.f;
		//----------------------//

		return true;
	}
};

struct ShapeLinearPolicy
{
	ATOM_ShapeKeyFrameData ipol (const ATOM_ShapeKeyFrameData &val1, const ATOM_ShapeKeyFrameData &val2, float t) const
	{
		ATOM_ShapeKeyFrameData result;

		result.transparency = val1.transparency + (val2.transparency - val1.transparency) * t;
		result.shearing = val1.shearing + (val2.shearing - val1.shearing) * t;
		result.uvRotation = val1.uvRotation + (val2.uvRotation - val1.uvRotation) * t;
		result.uvScaleOffset = val1.uvScaleOffset + (val2.uvScaleOffset - val1.uvScaleOffset) * t;
		result.size = val1.size + (val2.size - val1.size) * t;
		result.color = val1.color + (val2.color - val1.color) * t;
		//--- wangjian added ---//
		result.colorMultiplier = val1.colorMultiplier + (val2.colorMultiplier - val1.colorMultiplier) * t;
		result.resolveScale = val1.resolveScale + (val2.resolveScale - val1.resolveScale) * t;
		//----------------------//
		return result;
	}
};

class ATOM_ENGINE_API ATOM_ShapeKeyFrame: public ATOM_CompositionKeyFrameT<ATOM_ShapeKeyFrameData>
{
public:
	typedef ShapeLinearPolicy linear_policy_type;

public:
	ATOM_ShapeKeyFrame (void);
	ATOM_ShapeKeyFrame (const ATOM_ShapeKeyFrameData &data);

	virtual void visit (ATOM_Node &node);
	virtual void visit (ATOM_ShapeNode &node);
};
ATOM_DECLARE_COMPOSITION_KEYFRAME(ATOM_ShapeKeyFrame, ATOM_ShapeKeyFrameData)

class ATOM_ENGINE_API ATOM_ShapeKeyFrameValue: public ATOM_CompositionKeyFrameValue
{
public:
	void setData (const ATOM_ShapeKeyFrameData &data);
	const ATOM_ShapeKeyFrameData & getData (void) const;

private:
	ATOM_ShapeKeyFrameData _data;
};

#endif // __ATOM3D_ENGINE_SHAPE_KEYFRAME_H

